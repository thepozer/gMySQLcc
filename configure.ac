AC_INIT([gmysqlcc], [0.4.0], [dev@thepozer.net])
AC_CONFIG_SRCDIR([src/gmlc_main.c])
AM_INIT_AUTOMAKE([-Wall foreign])
AM_SILENT_RULES([yes])

AC_PROG_CC
PKG_PROG_PKG_CONFIG	
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.18.1])


AC_ARG_WITH([gtksourceview],
	AS_HELP_STRING([--with-gtksourceview], [Activate Gtk+ Source View 3.0 editor]),
	[
		PKG_CHECK_MODULES([GMYSQLCC], [glib-2.0 gtk+-3.0 mysqlclient gtksourceview-3.0], , exit)
		AC_DEFINE(USE_GTKSOURCEVIEW, "1")
	], [
		PKG_CHECK_MODULES([GMYSQLCC], [glib-2.0 gtk+-3.0 mysqlclient], , exit)
	]
)


AC_CONFIG_FILES([
    Makefile
    data/Makefile
    src/Makefile
    po/Makefile.in
])
AC_OUTPUT
