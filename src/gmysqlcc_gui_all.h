#ifndef __GMYSQLCC_GUI_ALL_H__
#define __GMYSQLCC_GUI_ALL_H__

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

/*
#define _(String) gettext (String)

#ifndef __GMYSQLCC_MAIN_PART__

extern int NbrWnd;
extern p_gmysql_config gmysql_conf;

#ifdef USE_GTKSOURCEVIEW
extern GtkSourceLanguagesManager * LangManager;
#endif * USE_GTKSOURCEVIEW *

#endif * __GMYSQLCC_MAIN_PART__ *
*/
/* List server window */
/*
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
*/
/* Server window */
/*
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
*/
/* Exec SQL window */
/*
typedef struct _s_gmysqlcc_gui_query {
	GtkWidget * window;
	
	GtkWidget * vbxSQLQueries;
	GtkWidget * lstSQLQueries;
	GtkWidget * txtSQLRequest;
	GtkWidget * sclSQLResult;
	GtkWidget * lstSQLResult;
	GtkWidget * cmbCharset;
  GtkWidget * statusbarSQL;
	
	p_mysql_database mysql_db;
	p_mysql_query mysql_qry;
	p_mysql_row mysql_rw;
	
	int currCharset;
	GSList * lstRows;
} s_gmysqlcc_gui_query;

typedef s_gmysqlcc_gui_query * p_gmysqlcc_gui_query;

p_gmysqlcc_gui_query gmysqlcc_gui_query_new (p_mysql_query mysql_qry);
gboolean gmysqlcc_gui_query_set_query (p_gmysqlcc_gui_query gui_query, const gchar * query, gboolean execNow);
gboolean gmysqlcc_gui_query_display (p_gmysqlcc_gui_query gui_query, gboolean display);
gboolean gmysqlcc_gui_query_delete (p_gmysqlcc_gui_query gui_query);

execSqlWnd * create_wndSQL(gboolean display, p_mysql_query mysql_qry, const gchar * query, gboolean execNow);
void ExecSql (execSqlWnd * stWnd, const char * sqlQuery);*
*/
/* Text window */
/*
typedef struct _textWnd {
	GtkWidget * wndText;
	GtkWidget * txtContent;
	
	gchar * filename;
} textWnd;
typedef textWnd * p_textWnd;

p_textWnd create_wndText (gboolean display, gchar * content, gchar * filename);
*/
/* History window */
/*
typedef struct _historyWnd {
	GtkWidget * wndMain;
	
	GtkWidget * lstHistory;
	
	p_gmysql_history_entry current_entry;
	void * current_entry;
} historyWnd;

typedef historyWnd * p_historyWnd;

p_historyWnd create_wndHistory (gboolean display);
*/
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
#define DUMP_FORMAT_SQL 1
#define DUMP_FORMAT_CSV 2
#define DUMP_FORMAT_XML 3


typedef struct _s_gmysqlcc_gui_dump {
	GtkWidget * window;
	
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
	GtkWidget * cmbOutputFormat;
	GtkWidget * cmbOutputCharset;
	GtkWidget * txtOutputFilename;

	p_mysql_server mysql_srv;
	p_mysql_database mysql_db;
	p_mysql_table mysql_tbl;
	
	gint16 dumpLevel;
	gint16 dumpType;
	gint16 dumpFormat;
	gchar * sqlQuery;
} s_gmysqlcc_gui_dump;

typedef s_gmysqlcc_gui_dump * p_gmysqlcc_gui_dump;

p_gmysqlcc_gui_dump gmysqlcc_gui_dump_new (p_mysql_server mysql_srv, p_mysql_database mysql_db, p_mysql_table mysql_tbl, gchar * sqlQuery);
gboolean gmysqlcc_gui_dump_display (p_gmysqlcc_gui_dump gui_dump, gboolean display);
gboolean gmysqlcc_gui_dump_delete (p_gmysqlcc_gui_dump gui_dump);

/* Misc gui functiond */
/*
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
*/


#endif /* __GMYSQLCC_GUI_ALL_H__ */
