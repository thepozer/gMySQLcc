
#ifndef __GEXECSQL_H__
#define __GEXECSQL_H__

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <libintl.h>

#include "mysql_db_all.h"
#include "gUtils.h"
#include "gDump.h"
#include "gmysql_utils.h"

#define _(String) gettext (String)

typedef struct _execSqlWnd {
	GtkWidget * wndMain;
	
	GtkWidget * vbxSQLQueries;
	GtkWidget * lstSQLQueries;
	GtkWidget * txtSQLRequest;
	GtkWidget * sclSQLResult;
	GtkWidget * lstSQLResult;
	GtkWidget * cmbCharset;
  GtkWidget * statusbarSQL;
	
	p_mysql_query mysql_qry;
	p_mysql_row mysql_rw;
	
	GPtrArray * lstQueries;
	GString * sqlFilename;
	int currCharset;
} execSqlWnd;

typedef execSqlWnd * p_execSqlWnd;

execSqlWnd * create_wndSQL(gboolean display, p_mysql_query mysql_qry, gboolean multiple);
void ExecSql (execSqlWnd * stWnd, const char * sqlQuery);

#endif /* __GEXECSQL_H__ */
