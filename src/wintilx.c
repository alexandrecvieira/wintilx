/**
 *
 * Copyright (c) 2017 Alexandre C Vieira
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
/* Code adapted from lxpanel-window-title(written by Matej Kollar) */

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
} WintilxPlugin;

static void wintilx_destructor(gpointer user_data);

static void wintilx_update_current_window_title(WintilxPlugin * wtlx) {
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
		wtlx->app_name = app_name;
	}

	if (app != NULL) {
		XFree(app);
	}

	Window * old_window = wtlx->current_window;
	wtlx->current_window = f;
	if (old_window != NULL) {
		XFree(old_window);
	}

	wtlx->title_source = title_source;

	char * old_window_title = wtlx->window_title;
	wtlx->window_title = title;
	g_free(old_window_title);
}

static void wintilx_update_main_label(WintilxPlugin * wtlx) {
	gchar * formated = NULL;

	if (wtlx->window_title == NULL
			|| ((g_strcmp0(wtlx->window_title, desktop_manager) == 0)
					&& (g_strcmp0(wtlx->app_name, desktop_manager) == 0))) {
		gtk_label_set_markup(GTK_LABEL(wtlx->label), "");
	} else {
		formated = g_markup_printf_escaped("<span color=\"#%06x\"><b>%s</b> - %s</span>", wtlx->color, wtlx->app_name,
				wtlx->window_title);
	}

	gtk_label_set_markup(GTK_LABEL(wtlx->label), formated);

	g_free(formated);
}

static GdkFilterReturn wintilx_event_filter(GdkXEvent *xevent, GdkEvent *event, gpointer user_data) {
	WintilxPlugin * wtlx = (WintilxPlugin *) user_data;
	XEvent *ev = (XEvent *) xevent;

	// ENTER;
	// DBG("win = 0x%x\n", ev->xproperty.window);

	if (ev->type == ConfigureNotify) {
		if (wtlx->current_window) {
			wintilx_update_current_window_title(wtlx);
			wintilx_update_main_label(wtlx);
		}

		RET(GDK_FILTER_CONTINUE);
	}

	return GDK_FILTER_CONTINUE;
}

static void wintilx_update_title_event(GtkWidget *widget, WintilxPlugin * wtlx) {
	wintilx_update_current_window_title(wtlx);
	wintilx_update_main_label(wtlx);
}

static void wintilx_update_desktop_event(GtkWidget *widget, WintilxPlugin * wtlx) {
	wintilx_update_current_window_title(wtlx);
	wintilx_update_main_label(wtlx);
}

static GtkWidget *wintilx_constructor(LXPanel *panel, config_setting_t *settings) {
	WintilxPlugin * wtlx = g_new0(WintilxPlugin, 1);
	GtkWidget *p;

	g_return_val_if_fail(wtlx != NULL, 0);
	wtlx->panel = panel;

	if (wtlx->panel->priv->usefontcolor) {
		wtlx->color = gcolor2rgb24(&wtlx->panel->priv->gfontcolor);
	} else {
		wtlx->color = 0;
	}

	p = gtk_event_box_new();

	lxpanel_plugin_set_data(p, wtlx, wintilx_destructor);

	wtlx->label = gtk_label_new(NULL);

	gtk_container_add(GTK_CONTAINER(p), wtlx->label);

	gtk_label_set_text(GTK_LABEL(wtlx->label), "Welcome...");

	gtk_widget_show(wtlx->label);

	wintilx_update_main_label(wtlx);

	gdk_window_add_filter(gdk_get_default_root_window(), (GdkFilterFunc) wintilx_event_filter, (gpointer) wtlx);

	g_signal_connect(G_OBJECT (fbev), "active_window", G_CALLBACK (wintilx_update_title_event), (gpointer ) wtlx);
	g_signal_connect(G_OBJECT (fbev), "client_list", G_CALLBACK (wintilx_update_title_event), (gpointer ) wtlx);
	g_signal_connect(G_OBJECT (fbev), "client_list_stacking", G_CALLBACK (wintilx_update_title_event),
			(gpointer ) wtlx);

	g_signal_connect(G_OBJECT (fbev), "current_desktop", G_CALLBACK (wintilx_update_desktop_event), (gpointer ) wtlx);
	g_signal_connect(G_OBJECT (fbev), "desktop_names", G_CALLBACK (wintilx_update_desktop_event), (gpointer ) wtlx);
	g_signal_connect(G_OBJECT (fbev), "number_of_desktops", G_CALLBACK (wintilx_update_desktop_event),
			(gpointer ) wtlx);

	return p;
}

static void wintilx_destructor(gpointer user_data) {
	WintilxPlugin * wtlx = (WintilxPlugin *) user_data;

	gdk_window_remove_filter(gdk_get_default_root_window(), (GdkFilterFunc) wintilx_event_filter, wtlx);

	g_signal_handlers_disconnect_by_func(fbev, wintilx_update_desktop_event, wtlx);
	g_signal_handlers_disconnect_by_func(fbev, wintilx_update_title_event, wtlx);

	g_free(wtlx->window_title);
	g_free(wtlx->app_name);
	if (wtlx->current_window) {
		XFree(wtlx->current_window);
	}
	gtk_widget_destroy(wtlx->label);
	g_free(wtlx);
}

FM_DEFINE_MODULE(lxpanel_gtk, wintilx);

LXPanelPluginInit fm_module_init_lxpanel_gtk = {
		.name = "Window Title for LXPanel",
		.description = "Show title of active window if any.",
		.expand_available = TRUE,
		.new_instance = wintilx_constructor,
		.one_per_system = 1
};
