#ifndef __GMYSQL_GUI_H__
#define __GMYSQL_GUI_H__

#include "../config.h"

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <libintl.h>

#ifdef USE_GTKSOURCEVIEW
	#include <gtksourceview/gtksourceview.h>
	#include <gtksourceview/gtksourcebuffer.h>
	#include <gtksourceview/gtksourcelanguage.h>
	#include <gtksourceview/gtksourcelanguagesmanager.h>
#endif /* USE_GTKSOURCEVIEW */

#include "mysql_db_all.h"
#include "gmysql_conf.h"
#include "gmysql_utils.h"
/*#include "gmysql_history.h"*/

#define _(String) gettext (String)

#ifndef __GMYSQL_MAIN_PART__

extern int NbrWnd;
extern p_gmysql_config gmysql_conf;

#ifdef USE_GTKSOURCEVIEW
extern GtkSourceLanguagesManager * LangManager;
#endif /* USE_GTKSOURCEVIEW */

#endif /* __GMYSQL_MAIN_PART__ */

/* List server window */

typedef struct _listServWnd {
	  GtkWidget * txtName;
		GtkWidget * txtHost;
	  GtkWidget * txtPort;
		GtkWidget * txtLogin;
	  GtkWidget * txtPasswd;
	  GtkWidget * txtAllowedDb;
	  GtkWidget * txtLocalSock;
		GtkWidget * cmbListHosts;
		GtkWidget * lstListHosts;
		GtkWidget * wndListServer;
		p_mysql_server selSrvr;
		p_gmysql_config gmsql_conf;
} listServWnd;
typedef listServWnd * p_listServWnd;

listServWnd * create_wndListServer (gboolean display, p_gmysql_config gmsql_conf);

/* Server window */

typedef struct _servWnd {
	GtkWidget * lstBase;
	GtkWidget * lstTable;
	GtkWidget * wndServer;
	GtkWidget * mnuBdOps;
	GtkWidget * mnuTblOps;
	
	p_mysql_server mysql_srv;
	p_mysql_database curr_mysql_db;
	p_mysql_table curr_mysql_tbl;
	
	GString * currDbName;
	GString * currTblName;
} servWnd;

typedef servWnd * p_servWnd;

p_servWnd create_wndServer (gboolean display, p_mysql_server msql_srv);

/* Exec SQL window */

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
	
	int currCharset;
	GSList * lstRows;
} execSqlWnd;

typedef execSqlWnd * p_execSqlWnd;

execSqlWnd * create_wndSQL(gboolean display, p_mysql_query mysql_qry, const gchar * query, gboolean execNow);
/*void ExecSql (execSqlWnd * stWnd, const char * sqlQuery);*/

/* Text window */

typedef struct _textWnd {
	GtkWidget * wndText;
	GtkWidget * txtContent;
	
	gchar * filename;
} textWnd;
typedef textWnd * p_textWnd;

p_textWnd create_wndText (gboolean display, gchar * content, gchar * filename);

/* History window */

typedef struct _historyWnd {
	GtkWidget * wndMain;
	
	GtkWidget * lstHistory;
	
	/*p_gmysql_history_entry current_entry;*/
	void * current_entry;
} historyWnd;

typedef historyWnd * p_historyWnd;

p_historyWnd create_wndHistory (gboolean display);

/* Dump window */

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

/* Misc gui functiond */

typedef struct _askFilenameInfos {
	GtkWidget * dialog;
	void * userData;
} askFilenameInfos;
typedef askFilenameInfos * p_askFilenameInfos;

GString * askInfo(const gchar * title, const gchar * message, const gchar * defaultValue);
gboolean askConfirmation(const gchar * title, const gchar * message);
gboolean askYesno(const gchar * title, const gchar * message);
gboolean askFilename(const gchar * title, const gchar * filename, void (*okevent) (GtkWidget *widget, gpointer user_data), gpointer user_data);

GtkWidget * createIconButton(const char * icon, const char * title);



#endif /* __GMYSQL_GUI_H__ */
