
#ifndef __GSERVER_H__
#define __GSERVER_H__

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <libintl.h>

#include "mysql_db_all.h"
#include "gmysql_conf.h"
#include "gExecSql.h"
/*#include "gTable.h"*/
#include "gUtils.h"
#include "gDump.h"

#define _(String) gettext (String)

typedef struct _servWnd {
	GtkWidget * lstBase;
	GtkWidget * lstTable;
	GtkWidget * wndServer;
	GtkWidget * mnuBdOps;
	p_mysql_server mysql_srv;
	p_mysql_database curr_mysql_db;
	p_mysql_table curr_mysql_tbl;
	GString * currDbName;
	GString * currTblName;
} servWnd;

typedef servWnd * p_servWnd;

p_servWnd create_wndServer (gboolean display, p_mysql_server msql_srv);
#endif /* __GSERVER_H__ */
