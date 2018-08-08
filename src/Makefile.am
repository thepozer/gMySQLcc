AM_CFLAGS = $(GTK_CFLAGS) $(MYSQL_CFLAGS) $(SOURCEVIEW_CFLAGS) $(DCONF_CFLAGS) -rdynamic
AM_CPPFLAGS = -DLOCALEDIR=\""$(localedir)"\"
bin_PROGRAMS = gmysqlcc

gmysqlcc_LDADD = $(GTK_LIBS) $(MYSQL_LIBS) $(SOURCEVIEW_LIBS) $(DCONF_LIBS)
gmysqlcc_SOURCES = gmysqlcc.gresources.h \
gmysqlcc.gresources.c \
gmysqlcc_main.c \
gmysqlcc_app.c


gmysqlcc.gresources.c: gmysqlcc.gresources.xml gmysqlcc.wnd-connect.glade gmysqlcc.gresources.h
	$(AM_V_GEN) glib-compile-resources --generate-source $<

gmysqlcc.gresources.h: gmysqlcc.gresources.xml gmysqlcc.wnd-connect.glade
	$(AM_V_GEN) glib-compile-resources --generate-header $<

gsettings_SCHEMAS = net.thepozer.gmysqlcc.gschema.xml
EXTRA_DIST = $(gsettings_SCHEMAS)

@GSETTINGS_RULES@

