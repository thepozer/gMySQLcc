
#ifndef __GDUMP_H__
#define __GDUMP_H__

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "mysql_db_all.h"
#include "gUtils.h"

#define DUMP_LEVEL_NULL 0
#define DUMP_LEVEL_SERVER 1
#define DUMP_LEVEL_DATABASE 2
#define DUMP_LEVEL_TABLE 3
#define DUMP_LEVEL_REQUEST 4

#define DUMP_TYPE_NULL 0
#define DUMP_TYPE_STRUCT 1
#define DUMP_TYPE_STRUCT_DATA 2
#define DUMP_TYPE_DATA 3

#define DUMP_FORMAT_NULL 0
#define DUMP_FORMAT_CSV 1
#define DUMP_FORMAT_XML 2
#define DUMP_FORMAT_SQL 3


typedef struct _dumpWnd {
	GtkWidget * wndDump;
	GtkWidget * rdoDumpServer;
	GtkWidget * rdoDumpDatabase;
	GtkWidget * rdoDumpTable;
	GtkWidget * rdoDumpRequest;
	GtkWidget * txtSQLRequest;
	GtkWidget * lblDumpServerName;
	GtkWidget * lblDumpTableName;
	GtkWidget * lblDumpDatabaseName;
	GtkWidget * chkSeparateFile;
	GtkWidget * chkGroupInDirectory;
	GtkWidget * txtNewDirectory;
	GtkWidget * chkDropDb;
	GtkWidget * chkAddUse;
	GtkWidget * chkDropTable;
 	GtkWidget * chkCompleteInsert;
	GtkWidget * rdoDumpStruct;
  GtkWidget * rdoDumpStructData;
  GtkWidget * rdoDumpData;
	GtkWidget * cmbDataFormat;
	GtkWidget * cmbOutputCharset;

	p_mysql_server mysql_srv;
	p_mysql_database mysql_db;
	p_mysql_table mysql_tbl;
	
	gboolean onlyQuery;
	gint16 dumpLevel;
	gint16 dumpType;
	gint16 dumpFormat;
} dumpWnd;

typedef dumpWnd * p_dumpWnd;

p_dumpWnd create_wndDump (gboolean display, p_mysql_server mysql_srv, p_mysql_database mysql_db, p_mysql_table mysql_tbl, gchar * sqlQuery);
#endif /* __GDUMP_H__ */
