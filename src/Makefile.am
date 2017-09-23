#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib
lib_LTLIBRARIES = wintilx.la

#######################################
# Build information for each library

# Sources for wintilx
wintilx_la_SOURCES = wintilx.c

# Linker options libTestProgram
wintilx_la_LDFLAGS = -module -avoid-version -shared $(LTLIBINTL)

# Compiler options. Here we are adding the include directory
# to be searched for headers included in the source code.
wintilx_la_CPPFLAGS = -I$(top_srcdir)/include $(GTK2_CFLAGS)

install-exec-hook:
		cd $(DESTDIR)$(libdir) && $(RM) $(lib_LTLIBRARIES) \
		&& chmod -x wintilx.so && sudo lxpanelctl restart

