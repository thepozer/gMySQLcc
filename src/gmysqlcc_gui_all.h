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
#include "gmysqlcc_config.h"
#include "gmysqlcc_helpers.h"


#define _(String) gettext (String)

#ifndef __GMYSQLCC_MAIN_PART__

extern int NbrWnd;
extern p_gmysqlcc_config gmysqlcc_conf;

#ifdef USE_GTKSOURCEVIEW
extern GtkSourceLanguagesManager * LangManager;
#endif /* USE_GTKSOURCEVIEW */

#endif /* __GMYSQLCC_MAIN_PART__ */

/* List server window */

typedef struct _s_gmysqlcc_gui_list_server {
	GtkWidget * window;
	
	GtkWidget * txtName;
	GtkWidget * txtHost;
	GtkWidget * txtPort;
	GtkWidget * txtLogin;
	GtkWidget * txtPasswd;
	GtkWidget * txtAllowedDb;
	GtkWidget * txtLocalSock;
	GtkWidget * chkReadOnly;
	GtkWidget * chkWriteWarning;
	
	GtkWidget * lstListHosts;
	GtkWidget * vbxEditPart;
	
	gboolean show_edit_part;
	p_mysql_server curr_mysql_srv;
	p_gmysqlcc_config gmysqlcc_conf;
} s_gmysqlcc_gui_list_server;

typedef s_gmysqlcc_gui_list_server * p_gmysqlcc_gui_list_server;

p_gmysqlcc_gui_list_server gmysqlcc_gui_list_server_new (p_gmysqlcc_config gmysql_conf);
gboolean gmysqlcc_gui_list_server_display (p_gmysqlcc_gui_list_server gui_list_server, gboolean display);
gboolean gmysqlcc_gui_list_server_delete (p_gmysqlcc_gui_list_server gui_list_server);


/* Server window */

typedef struct _s_gmysqlcc_gui_server {
	GtkWidget * window;
	
/* Database part */
	GtkWidget * lstBase;
	GtkWidget * lstTable;
	GtkWidget * mnuBdOps;
	GtkWidget * mnuTblOps;
	
	p_mysql_server mysql_srv;
	p_mysql_database curr_mysql_db;
	p_mysql_table curr_mysql_tbl;

/* User part */
	GtkWidget *lstUser;
	GtkWidget *txtUserLogin;
	GtkWidget *txtUserHost;
	GtkWidget *txtUserPasswd;
	GtkWidget *chkEncryptedPasswd;
/* Rights Part */
	GtkWidget *lstURUserRights; /* User rights*/
	GtkWidget *lstDRDatabases;/* Database rights */
	GtkWidget *txtDRDbName;
	GtkWidget *lstDRDatabaseRights;
	GtkWidget *lstTRTables; /* Table rights */
	GtkWidget *txtTRDbName;
	GtkWidget *txtTRTblName;
	GtkWidget *lstTRTableRights;
	GtkWidget *lstTRColumnRights;
	GtkWidget *lstCRColumns; /* Column rights */
	GtkWidget *txtCRDbName;
	GtkWidget *txtCRTblName;
	GtkWidget *txtCRColName;
	GtkWidget *lstCRColumnRights;
	
	p_mysql_user	curr_mysql_usr;
	p_mysql_right	curr_database_rights;
} s_gmysqlcc_gui_server;

typedef s_gmysqlcc_gui_server * p_gmysqlcc_gui_server;

p_gmysqlcc_gui_server gmysqlcc_gui_server_new (p_mysql_server mysql_srv);
gboolean gmysqlcc_gui_server_display (p_gmysqlcc_gui_server gui_server, gboolean display);
gboolean gmysqlcc_gui_server_delete (p_gmysqlcc_gui_server gui_server);

/* Exec SQL window */

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
	
	gint currCharset;
	GSList * lstRows;
} s_gmysqlcc_gui_query;

typedef s_gmysqlcc_gui_query * p_gmysqlcc_gui_query;

p_gmysqlcc_gui_query gmysqlcc_gui_query_new (p_mysql_query mysql_qry);
gboolean gmysqlcc_gui_query_set_query (p_gmysqlcc_gui_query gui_query, const gchar * query, gboolean execNow);
gboolean gmysqlcc_gui_query_display (p_gmysqlcc_gui_query gui_query, gboolean display);
gboolean gmysqlcc_gui_query_delete (p_gmysqlcc_gui_query gui_query);

/* Text window */

typedef struct _s_gmysqlcc_gui_text {
	GtkWidget * window;
	GtkWidget * txtContent;
	
	gchar * filename;
} s_gmysqlcc_gui_text;
typedef s_gmysqlcc_gui_text * p_gmysqlcc_gui_text;

p_gmysqlcc_gui_text gmysqlcc_gui_text_new ();
gboolean gmysqlcc_gui_text_set_content (p_gmysqlcc_gui_text gui_text, const gchar * content, const gchar * filename);
gboolean gmysqlcc_gui_text_display (p_gmysqlcc_gui_text gui_text, gboolean display);
gboolean gmysqlcc_gui_text_delete (p_gmysqlcc_gui_text gui_text);

/* Exec SQL File window */

typedef struct _s_gmysqlcc_gui_exec_file {
	GtkWidget * window;
	GtkWidget * cmbDatabaseList;
	GtkWidget * cmbServerList;
  GtkWidget * rbtExistDatabase;
  GtkWidget * rbtNewDatabase;
	GtkWidget * txtNewDatabase;
	GtkWidget * chkStopError;
	GtkWidget * xpdResults;
	GtkWidget * pgbWorking;
	GtkWidget * lblExecutedQueries;
	GtkWidget * lblErrorQueries;
	
	gchar * filename;
	gchar * content;
	p_mysql_server mysql_srv;
	p_mysql_database mysql_db;
	
	p_mysql_multi_query mysql_mlt_qry;
} s_gmysqlcc_gui_exec_file;
typedef s_gmysqlcc_gui_exec_file * p_gmysqlcc_gui_exec_file;

p_gmysqlcc_gui_exec_file gmysqlcc_gui_exec_file_new ();
p_gmysqlcc_gui_exec_file gmysqlcc_gui_exec_file_new_open_file ();
gboolean gmysqlcc_gui_exec_file_set_file (p_gmysqlcc_gui_exec_file gui_xcfl, const gchar * filename);
gboolean gmysqlcc_gui_exec_file_set_content (p_gmysqlcc_gui_exec_file gui_xcfl, const gchar * content);
gboolean gmysqlcc_gui_exec_file_display (p_gmysqlcc_gui_exec_file gui_xcfl, gboolean display);
gboolean gmysqlcc_gui_exec_file_delete (p_gmysqlcc_gui_exec_file gui_xcfl);

/* Dump window */

#define DUMP_TYPE_NULL 0
#define DUMP_TYPE_STRUCT 1
#define DUMP_TYPE_DATA 2
#define DUMP_TYPE_STRUCT_DATA 3

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
	
	e_dumpLevel dumpLevel;
	e_dumpFormat dumpFormat;
	gint16 dumpType;
	gchar * sqlQuery;
} s_gmysqlcc_gui_dump;

typedef s_gmysqlcc_gui_dump * p_gmysqlcc_gui_dump;

p_gmysqlcc_gui_dump gmysqlcc_gui_dump_new (p_mysql_server mysql_srv, p_mysql_database mysql_db, p_mysql_table mysql_tbl, gchar * sqlQuery);
gboolean gmysqlcc_gui_dump_display (p_gmysqlcc_gui_dump gui_dump, gboolean display);
gboolean gmysqlcc_gui_dump_delete (p_gmysqlcc_gui_dump gui_dump);

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



#endif /* __GMYSQLCC_GUI_ALL_H__ */
