
#ifndef __GEXECSQL_H__
#define __GEXECSQL_H__

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "mysql_db_all.h"
#include "gUtils.h"
#include "gDump.h"

typedef struct _execSqlWnd {
	GtkWidget * wndMain;
	
	GtkWidget * vbxSQLQueries;
	GtkWidget * lstSQLQueries;
	GtkWidget * txtSQLRequest;
	GtkWidget * sclSQLResult;
	GtkWidget * lstSQLResult;
	GtkWidget * tlbMultiSql;
	GtkWidget * tlbSingleSql;
	
	p_mysql_query msql_qry;
	p_mysql_row mysql_rw;
	
	gboolean multiSqlMode;  // If true display list of SQL Queries
	GPtrArray * lstQueries;
	GString * sqlFilename;
} execSqlWnd;

typedef execSqlWnd * p_execSqlWnd;

execSqlWnd * create_wndSQL(gboolean display, p_mysql_query msql_qry, gboolean multiple);
void ExecSql (execSqlWnd * stWnd, const char * sqlQuery);

#endif /* __GEXECSQL_H__ */
