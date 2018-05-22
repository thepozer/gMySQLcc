AM_CFLAGS = $(GMYSQLCC_CFLAGS)
AM_CPPFLAGS = -DLOCALEDIR=\""$(localedir)"\"
bin_PROGRAMS = gmysqlcc

gmysqlcc_LDADD = $(GMYSQLCC_LIBS)
gmysqlcc_SOURCES = gmysqlcc_main.c \
gmysqlcc_app.c \
gmysqlcc.gresources.h \
gmysqlcc.gresources.c


gmysqlcc.gresources.c: gmysqlcc.gresources.xml gmysqlcc.wnd-connect.glade
	$(AM_V_GEN) glib-compile-resources --generate-source $<

gmysqlcc.gresources.h: gmysqlcc.gresources.xml gmysqlcc.wnd-connect.glade
	$(AM_V_GEN) glib-compile-resources --generate-header $<

gsettings_SCHEMAS = net.thepozer.gmysqlcc.gschema.xml
EXTRA_DIST = $(gsettings_SCHEMAS)

@GSETTINGS_RULES@

