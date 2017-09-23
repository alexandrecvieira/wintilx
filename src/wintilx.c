/**
 *
 * Copyright (c) 2013 Matej Kollar
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */
/* Adapted for Debian by Alexandre C Vieira */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include <string.h>

#include <lxpanel/misc.h>
#include <lxpanel/plugin.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include "lxpanel.private/ev.h"
#include "lxpanel.private/dbg.h"
#include "lxpanel.private/private.h"

// #define my_COLOR 0x000000

static const char * desktop_manager = "pcmanfm";

typedef struct {
	LXPanel * panel;
	GtkWidget * label;
	Window * current_window;
	//char * desktop_name;
	char * window_title;
	char * app_name;
	Atom title_source;
	guint32 * color;
} WindowTitle;

static void windowtitle_destructor(gpointer user_data);

static void my_update_current_window_title(WindowTitle * egz) {
	char * title = NULL;
	char * app_name = NULL;
	Atom title_source = None;
	Window * f = get_xaproperty(gdk_x11_get_default_root_xwindow(), a_NET_ACTIVE_WINDOW, XA_WINDOW, 0);

	XClassHint *app = XAllocClassHint();

	if (f != NULL) {
		title = get_utf8_property(*f, a_NET_WM_VISIBLE_NAME);

		if (title == NULL) {
			title = get_utf8_property(*f, a_NET_WM_NAME);
		} else {
			title_source = a_NET_WM_VISIBLE_NAME;
		}

		if (title == NULL) {
			title = get_textproperty(*f, XA_WM_NAME);
		} else {
			title_source = a_NET_WM_NAME;
		}

		if (title != NULL) {
			title_source = XA_WM_NAME;
		}

		XGetClassHint(GDK_DISPLAY(), *f, app);
		app_name = app->res_name;
		egz->app_name = app_name;
	}

	if (app != NULL) {
		XFree(app);
	}

	Window * old_window = egz->current_window;
	egz->current_window = f;
	if (old_window != NULL) {
		XFree(old_window);
	}

	egz->title_source = title_source;

	char * old_window_title = egz->window_title;
	egz->window_title = title;
	g_free(old_window_title);
}

/*static void my_update_current_desktop_name(WindowTitle * egz)
 {
 int number_of_desktop_names;
 char * *  desktop_names = get_utf8_property_list(
 gdk_x11_get_default_root_xwindow(),
 a_NET_DESKTOP_NAMES,
 &number_of_desktop_names);
 if (desktop_names == NULL) {
 return NULL;
 }
 int current_desktop = get_net_current_desktop();
 char * ret = NULL;

 if (current_desktop < number_of_desktop_names) {
 ret = g_strdup(desktop_names[current_desktop]);
 } else {
 fprintf(stderr, "Faaaak...\n");
 }
 g_strfreev(desktop_names);

 char * old = egz->desktop_name;
 egz->desktop_name = ret;
 g_free(old);
 }*/

static void my_update_main_label(WindowTitle * egz) {
	//if (egz->desktop_name == NULL) {
	//    gtk_label_set_markup(GTK_LABEL(egz->label), "");
	//    return;
	// }
	gchar * formated = NULL;
	if (egz->window_title == NULL
			|| ((g_strcmp0(egz->window_title, desktop_manager) == 0) && (g_strcmp0(egz->app_name, desktop_manager) == 0))) {
		gtk_label_set_markup(GTK_LABEL(egz->label), "");
		//formated = g_markup_printf_escaped("<span color=\"#%06x\">%s</span>",
		//    egz->color, egz->desktop_name);
	} else {
		formated = g_markup_printf_escaped("<span color=\"#%06x\"><b>%s</b> - %s</span>", egz->color, egz->app_name,
				egz->window_title);
	}
	gtk_label_set_markup(GTK_LABEL(egz->label), formated);
	g_free(formated);
}

static GdkFilterReturn my_event_filter(GdkXEvent *xevent, GdkEvent *event, gpointer user_data) {
	WindowTitle * egz = (WindowTitle *) user_data;
	XEvent *ev = (XEvent *) xevent;

	ENTER;
	DBG("win = 0x%x\n", ev->xproperty.window);
	if (ev->type == ConfigureNotify) {
		if (egz->current_window)
			my_update_main_label(egz);
		RET(GDK_FILTER_CONTINUE);
	}

	return GDK_FILTER_CONTINUE;
}

static void my_update_title_event(GtkWidget *widget, WindowTitle * egz) {
	my_update_current_window_title(egz);
	my_update_main_label(egz);
}

static void my_update_desktop_event(GtkWidget *widget, WindowTitle * egz) {
	// my_update_current_desktop_name(egz);
	my_update_main_label(egz);
}

static GtkWidget *windowtitle_constructor(LXPanel *panel, config_setting_t *settings) {
	WindowTitle * egz = g_new0(WindowTitle, 1);
	GtkWidget *p;

	g_return_val_if_fail(egz != NULL, 0);
	egz->panel = panel;

	if (egz->panel->priv->usefontcolor) {
		egz->color = gcolor2rgb24(&egz->panel->priv->gfontcolor);
	} else {
		egz->color = 0;
	}
	// egz->color = my_COLOR;

	// my_update_current_desktop_name(egz);
	p = gtk_event_box_new();

	lxpanel_plugin_set_data(p, egz, windowtitle_destructor);

	egz->label = gtk_label_new(NULL);

	gtk_container_add(GTK_CONTAINER(p), egz->label);

	gtk_label_set_text(GTK_LABEL(egz->label), "Welcome...");

	gtk_widget_show(egz->label);

	my_update_main_label(egz);

	gdk_window_add_filter(gdk_get_default_root_window (), (GdkFilterFunc)my_event_filter, (gpointer) egz);

	g_signal_connect(G_OBJECT (fbev), "active_window", G_CALLBACK (my_update_title_event), (gpointer ) egz);
	g_signal_connect(G_OBJECT (fbev), "client_list", G_CALLBACK (my_update_title_event), (gpointer ) egz);
	g_signal_connect(G_OBJECT (fbev), "client_list_stacking", G_CALLBACK (my_update_title_event), (gpointer ) egz);

	g_signal_connect(G_OBJECT (fbev), "current_desktop", G_CALLBACK (my_update_desktop_event), (gpointer ) egz);
	g_signal_connect(G_OBJECT (fbev), "desktop_names", G_CALLBACK (my_update_desktop_event), (gpointer ) egz);
	g_signal_connect(G_OBJECT (fbev), "number_of_desktops", G_CALLBACK (my_update_desktop_event), (gpointer ) egz);

	return p;
}

static void windowtitle_destructor(gpointer user_data) {
	WindowTitle * egz = (WindowTitle *) user_data;

	gdk_window_remove_filter(gdk_get_default_root_window (), (GdkFilterFunc)my_event_filter, egz);

	g_signal_handlers_disconnect_by_func(fbev, my_update_desktop_event, egz);
	g_signal_handlers_disconnect_by_func(fbev, my_update_title_event, egz);

	//g_free(egz->desktop_name);
	g_free(egz->window_title);
	g_free(egz->app_name);
	if (egz->current_window) {
		XFree(egz->current_window);
	}
	gtk_widget_destroy(egz->label);
	g_free(egz);
}

FM_DEFINE_MODULE(lxpanel_gtk, windowtitle);

LXPanelPluginInit fm_module_init_lxpanel_gtk = {
		.name = N_("Window Title for LXPanel"), .description = N_("Show title of active window if any."),
		.expand_available = TRUE,
		.new_instance = windowtitle_constructor
};
