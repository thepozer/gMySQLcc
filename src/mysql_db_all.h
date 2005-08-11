
#ifndef __MYSQL_DB_ALL_H__
#define __MYSQL_DB_ALL_H__

#include <glib.h>
#include <mysql/mysql.h>
#include <libintl.h>

#include "mysql_data_list.h"

#define _(String) gettext (String)

typedef struct _s_mysql_server {
/* Connection infos */
	gchar *					name;
	gchar *					host;
	unsigned int		port;
	gchar *					user;
	gchar *					passwd;
	gchar *					allowedDbs;
	gchar *					localSock;
	
/* Server protections */
	gboolean				read_only;
	gboolean				write_warning;

/* Server datas */
	guint						version;
	GHashTable *		hshDbs;
	p_data_list			mysql_usr_lst;
} s_mysql_server;

typedef s_mysql_server * p_mysql_server;

typedef enum _e_mysqlRightType {
	UserSubRightType_Undefined = 0,
	UserSubRightType_Host,
	UserSubRightType_User,
	UserSubRightType_Database,
	UserSubRightType_Proc
} e_mysqlRightType;

typedef struct _s_mysql_right {
	e_mysqlRightType	type;
	p_mysql_server		mysql_srv;
	
	gchar *				host;
	gchar *				login;
	gchar *				db;
	gchar *				routine_name;
	gchar *				routine_type;
	
	GHashTable *		hsh_rights;
	guint64				b_rights;
} s_mysql_right;

typedef s_mysql_right * p_mysql_right;

typedef struct _s_bool_right_item{
	int value;
	gchar * set_name;
	gchar * name;
	int version;
} s_bool_right_item;

#ifdef __GMYSQLCC_MAIN_PART__

s_bool_right_item ar_bri_user[] = {
	{-1,	"Select_priv",				"Select_priv",				0},
	{-1,	"Insert_priv",				"Insert_priv",				0},
	{-1,	"Update_priv",				"Update_priv",				0},
	{-1,	"Delete_priv",				"Delete_priv",				0},
	{-1,	"Create_priv",				"Create_priv",				0},
	{-1,	"Drop_priv",				"Drop_priv",				0},
	{-1,	"Reload_priv",				"Reload_priv",				0},
	{-1,	"Shutdown_priv",			"Shutdown_priv",			0},
	{-1,	"Process_priv",				"Process_priv",				0},
	{-1,	"File_priv",				"File_priv",				0},
	{-1,	"Grant_priv",				"Grant_priv",				0},
	{-1,	"Refereances_priv",			"Refereances_priv",			0},
	{-1,	"Index_priv",				"Index_priv",				0},
	{-1,	"Alter_priv",				"Alter_priv",				0},
	{-1,	"Show_db_priv",				"Show_db_priv",				40000},
	{-1,	"Super_priv",				"Super_priv",				40000},
	{-1,	"Create_tmp_table_priv",	"Create_tmp_table_priv",	40000},
	{-1,	"Lock_tables_priv",			"Lock_tables_priv",			40000},
	{-1,	"Execute_priv",				"Execute_priv",				40000},
	{-1,	"Repl_slave_priv",			"Repl_slave_priv",			40000},
	{-1,	"Repl_client_priv",			"Repl_client_priv",			40000},
	{0, NULL, NULL, 0}
};
guint sz_ar_bri_user = 21;

s_bool_right_item ar_bri_database[] = {
	{-1,	"Select_priv",				"Select_priv",				0},
	{-1,	"Insert_priv",			 	"Insert_priv",				0},
	{-1,	"Update_priv",				"Update_priv",				0},
	{-1,	"Delete_priv",				"Delete_priv",				0},
	{-1,	"Create_priv",				"Create_priv",				0},
	{-1,	"Drop_priv",				"Drop_priv",				0},
	{-1,	"Grant_priv",				"Grant_priv",				0},
	{-1,	"Refereances_priv",			"Refereances_priv",			0},
	{-1,	"Index_priv",				"Index_priv",				0},
	{-1,	"Alter_priv",				"Alter_priv",				0},
	{-1,	"Create_tmp_table_priv",	"Create_tmp_table_priv",	40000},
	{-1,	"Lock_tables_priv",			"Lock_tables_priv",			40000},
	{-1,	"Create_view_priv",			"Create_view_priv",			50000},
	{-1,	"Show_view_priv",			"Show_view_priv",			50000},
	{0, NULL, NULL, 0}
};
guint sz_ar_bri_database = 10;

s_bool_right_item ar_bri_proc[] = {
	{1, "Execute",			"Execute_Priv",			0},
	{2, "Alter Routine",	"Alter_Routine_Priv",	0},
	{4, "Grant",			"Grant_Priv",			0},
	{0, NULL, NULL, 0}
};
guint sz_ar_bri_proc = 3;

#else

extern s_bool_right_item ar_bri_user[];
extern guint sz_ar_bri_user;
extern s_bool_right_item ar_bri_database[];
extern guint sz_ar_bri_database;
extern s_bool_right_item ar_bri_proc[];
extern guint sz_ar_bri_proc;

#endif

typedef struct _s_mysql_user {
/* User Infos */
	gchar *					login;
	gchar *					host;
	gchar *					passwd;
	
	p_mysql_right		user_rights;
	
	GHashTable *		hsh_db_list_rights;
	
/* Connection Infos */
	p_mysql_server	mysql_srv;
} s_mysql_user;

typedef s_mysql_user * p_mysql_user;

typedef struct _s_mysql_database {
/* Connection Infos */
	gchar *					name;
	p_mysql_server	mysql_srv;
	
/* Tables Informations */
	GList *					lstTables;
	GHashTable *		hshTables;
	
/* Update database list informations */
	gboolean				found;
} s_mysql_database;

typedef s_mysql_database * p_mysql_database;

typedef struct _s_mysql_table {
/* Connection Infos */
	gchar *						name;
	p_mysql_database	mysql_db;
	
/* Status infos */
	gchar *						nbrRow;
	gchar *						size;
	gchar *						type;
} s_mysql_table;

typedef s_mysql_table * p_mysql_table;

typedef struct _s_mysql_query {
/* Query Infos */
	gchar *						query;
	
/* Result Infos */
	int								nbrField;
	int								editResult;
	int								errCode;
	gchar *						errMsg;
	GArray *					rawHeaders;
	gboolean					can_edit;
	
/* Connection Infos */
	p_mysql_server		mysql_srv;
	gchar *						db_name;
	gchar *						abs_tbl_name;
	gchar *						tbl_name;
	
/* Mysql Infos */
	MYSQL *						mysql_link;
	MYSQL_RES *				mysql_result;
	
/* Charset Infos */
	gchar *						charset;
	GIConv						iconv_from;
	GIConv						iconv_to;
} s_mysql_query;

typedef s_mysql_query * p_mysql_query;

typedef struct _s_mysql_multi_query {
	/* Connection Infos */
	p_mysql_database	mysql_db;
	
	/* Execution Infos */
	p_mysql_query			mysql_qry;
	void (* status_callback) (struct _s_mysql_multi_query * mysql_mlt_qry, gboolean error, gpointer user_data);
	gpointer status_user_data;
	
	/* Thread params */
	const gchar *			content;
	gboolean					stop_error;
	gboolean					finished;
	gboolean					finish_ok;
	
	/* Report Infos */
	GString *					report;
	gint 							nbr_query;
	gint							nbr_error;
} s_mysql_multi_query;

typedef s_mysql_multi_query * p_mysql_multi_query;
	
typedef struct _s_mysql_row {
/* Data infos */
	GArray *					results;
	
/* Connection infos */
	p_mysql_query			mysql_qry;
	
/* Request for update infos */
	gchar *						abs_tbl_name;
	gchar *						primary_where_part;
} s_mysql_row;

typedef s_mysql_row * p_mysql_row;


/***** Dump informations *****/

typedef enum _e_dumpFormat {
	DumpFormat_Null = 0,
	DumpFormat_Sql,
	DumpFormat_Csv,
	DumpFormat_Xml
} e_dumpFormat;

typedef enum _e_dumpLevel {
	DumpLevel_Null = 0,
	DumpLevel_Server,
	DumpLevel_Database,
	DumpLevel_Table,
	DumpLevel_Query
} e_dumpLevel;

typedef struct _s_mysql_dump {
	/* General informations */
	e_dumpFormat			format;
	e_dumpLevel				level;
	
	/* File informations */
	gchar *						filename;
	gchar *						charset;
	GIOChannel * 			file;
	
	/* Server informations */
	p_mysql_server		mysql_svr;
	gboolean					svr_separate_file;
	gchar *						svr_base_directory;
	gboolean					svr_group_in_directory;
	gchar *						svr_group_directory;
	
	/* Database informations */
	p_mysql_database	mysql_db;
	gboolean					db_drop_database;
	gboolean					db_use_database;
	
	/* Table informations */
	p_mysql_table			mysql_tbl;
	gboolean					tbl_drop_table;
	gboolean					tbl_structure;
	gboolean					tbl_data;
	gboolean					tbl_data_complete_insert;
	
	/* Query informations */
	p_mysql_query			mysql_qry;
	gchar *						qry_string;
} s_mysql_dump;

typedef s_mysql_dump * p_mysql_dump;

/***** Server functions *****/

p_mysql_server mysql_server_new ();
gboolean mysql_server_delete (p_mysql_server mysql_srv);

p_mysql_query mysql_server_query (p_mysql_server mysql_srv, const gchar * db_name);
guint mysql_server_get_version(p_mysql_server mysql_srv, gboolean force_query);

gboolean mysql_server_clean_database_list (p_mysql_server mysql_srv, gboolean only_not_found);
void mysql_server_mark_found_all_databases (p_mysql_server mysql_srv, gboolean found);
gboolean mysql_server_refresh_database_list (p_mysql_server mysql_srv);
p_mysql_database mysql_server_get_database (p_mysql_server mysql_srv, const gchar * db_name);

GArray * mysql_server_get_status (p_mysql_server mysql_srv);
gboolean mysql_server_flush_status (p_mysql_server mysql_srv);

p_data_list mysql_server_get_user_list(p_mysql_server mysql_srv);

/***** Database functions *****/

p_mysql_database mysql_database_new_create(p_mysql_server mysql_srv, const gchar * db_name);
p_mysql_database mysql_database_new(p_mysql_server mysql_srv, const gchar * db_name);
gboolean mysql_database_delete(p_mysql_database mysql_db);

p_mysql_query mysql_database_query (p_mysql_database mysql_db);

gboolean mysql_database_clean_table_list (p_mysql_database mysql_db);
gboolean mysql_database_refresh_table_list (p_mysql_database mysql_db);
p_mysql_table mysql_database_get_table (p_mysql_database mysql_db, const gchar * tbl_name);

/***** Table functions *****/

p_mysql_table mysql_table_new(p_mysql_database mysql_db, const gchar * tbl_name);
gboolean mysql_table_delete(p_mysql_table mysql_tbl);

p_mysql_query mysql_table_query (p_mysql_table mysql_tbl);
GString * mysql_table_get_sql_structure (p_mysql_table mysql_tbl);

/***** Multi-Queries functions *****/

p_mysql_multi_query mysql_multi_query_new (p_mysql_database mysql_db);
gboolean mysql_multi_query_delete(p_mysql_multi_query mysql_mlt_qry);

gboolean mysql_multi_query_from_file(p_mysql_multi_query mysql_mlt_qry, const char * filename, gboolean b_stop_on_error);
gboolean mysql_multi_query_from_string(p_mysql_multi_query mysql_mlt_qry, const char * content, gboolean b_stop_on_error);

void mysql_multi_query_set_status_callback(p_mysql_multi_query mysql_mlt_qry, void (* callback) (p_mysql_multi_query mysql_mlt_qry, gboolean error, gpointer user_data), gpointer user_data);
const gchar * mysql_multi_query_get_report(p_mysql_multi_query mysql_mlt_qry);

/***** Query functions *****/

p_mysql_query mysql_query_new(p_mysql_server mysql_srv, const gchar * db_name);
gboolean mysql_query_delete(p_mysql_query mysql_qry);

p_mysql_query mysql_query_duplicate(p_mysql_query mysql_qry);
p_mysql_database mysql_query_get_database(p_mysql_query mysql_qry);

gboolean mysql_query_execute_query(p_mysql_query mysql_qry, const gchar * query, gboolean force_write);
gboolean mysql_query_free_query(p_mysql_query mysql_qry);

GArray * mysql_query_get_headers(p_mysql_query mysql_qry);
GArray * mysql_query_get_next_record(p_mysql_query mysql_qry);
GArray * mysql_query_get_all_records(p_mysql_query mysql_qry);

gchar * mysql_query_get_absolute_table_name (p_mysql_query mysql_qry, gboolean only_table_name);
gchar * mysql_query_get_primary_where (p_mysql_query mysql_qry, GArray * datas);
gboolean mysql_query_is_editable (p_mysql_query mysql_qry);

gboolean mysql_query_set_can_edit (p_mysql_query mysql_qry, gboolean new_value);
gboolean mysql_query_get_can_edit (p_mysql_query mysql_qry);

gchar * mysql_query_get_charset(p_mysql_query mysql_qry);
gboolean mysql_query_change_charset (p_mysql_query mysql_qry, const gchar * charset);

gboolean mysql_query_is_read_query (p_mysql_query mysql_qry, const gchar * query);

/***** Row functions *****/

p_mysql_row mysql_row_new_next_record (p_mysql_query mysql_qry);
gboolean mysql_row_free (p_mysql_row mysql_rw);

gboolean mysql_row_is_editable (p_mysql_row mysql_rw);
gchar * mysql_row_get_field_value(p_mysql_row mysql_rw, int idx);
gchar * mysql_row_set_field_value(p_mysql_row mysql_rw, int idx, const gchar * new_value);
gboolean mysql_row_delete(p_mysql_row mysql_rw);

/***** Dump functions ******/

p_mysql_dump mysql_dump_new (e_dumpFormat format, e_dumpLevel level);
gboolean mysql_dump_free (p_mysql_dump mysql_dmp);
gboolean mysql_dump_set_filename (p_mysql_dump mysql_dmp, const gchar * sql_filename, const gchar * charset);
gboolean mysql_dump_set_params_server (p_mysql_dump mysql_dmp, p_mysql_server mysql_svr, gboolean separate_file, const gchar * base_directory, gboolean group_in_directory, const gchar * group_directory);
gboolean mysql_dump_set_params_database (p_mysql_dump mysql_dmp, p_mysql_database mysql_db, gboolean drop_database, gboolean use_database);
gboolean mysql_dump_set_database (p_mysql_dump mysql_dmp, p_mysql_database mysql_db);
gboolean mysql_dump_set_params_table (p_mysql_dump mysql_dmp, p_mysql_table mysql_tbl, gboolean drop_table, gboolean structure, gboolean data, gboolean data_complete_insert);
gboolean mysql_dump_set_table (p_mysql_dump mysql_dmp, p_mysql_table mysql_tbl);
gboolean mysql_dump_set_query (p_mysql_dump mysql_dmp, p_mysql_query mysql_qry);
gboolean mysql_dump_set_query_string (p_mysql_dump mysql_dmp, const gchar * qry_string);

void mysql_dump_show_debug(p_mysql_dump mysql_dmp);

gboolean mysql_dump_do_to_disk (p_mysql_dump mysql_dmp);
gboolean mysql_dump_sql_do_to_disk (p_mysql_dump mysql_dmp);
gboolean mysql_dump_xml_do_to_disk (p_mysql_dump mysql_dmp);
gboolean mysql_dump_csv_do_to_disk (p_mysql_dump mysql_dmp);

gchar * mysql_dump_do_to_memory (p_mysql_dump mysql_dmp);
gchar * mysql_dump_csv_do_to_memory (p_mysql_dump mysql_dmp);

/***** User functions *****/

p_mysql_user mysql_user_new (p_mysql_server mysql_srv, const gchar * login, const gchar * host);
gboolean mysql_user_delete (p_mysql_user mysql_usr);

gboolean mysql_user_update_from_db (p_mysql_user mysql_usr);
gchar * mysql_user_get_key (p_mysql_user mysql_usr);

gboolean mysql_user_read_rights (p_mysql_user mysql_usr);
gboolean mysql_user_set_right (p_mysql_user mysql_usr, const gchar * right, const gchar * new_value);

gboolean mysql_user_read_database_rights (p_mysql_user mysql_usr);
gboolean mysql_user_get_database_rights (p_mysql_user mysql_usr, const gchar * db_name);

p_mysql_user mysql_user_create (p_mysql_server mysql_srv, const gchar * login, const gchar * host, const gchar * password, gboolean crypted_password);
gboolean mysql_user_update_key_values (p_mysql_user mysql_usr, const gchar * new_login, const gchar * new_host);
gboolean mysql_user_set_password (p_mysql_user mysql_usr, const gchar * new_password, gboolean crypted);
gboolean mysql_user_remove (p_mysql_user mysql_usr);

/***** User list functions *****/

p_data_list mysql_user_list_new ();
gboolean mysql_user_list_delete (p_data_list mysql_usr_lst);

gboolean mysql_user_list_refresh (p_data_list mysql_usr_lst);

/***** Rights functions *****/

p_mysql_right mysql_right_new_user (p_mysql_server mysql_srv, const gchar * Host, const gchar * Login);
p_mysql_right mysql_right_new_database (p_mysql_server mysql_srv, const gchar * Host, const gchar * Login, const gchar * Db);
p_mysql_right mysql_right_new_database_create (p_mysql_server mysql_srv, const gchar * Host, const gchar * Login, const gchar * Db);

/*p_mysql_right mysql_right_new (); Internal only */
gboolean mysql_right_delete (p_mysql_right mysql_rght);
gboolean mysql_right_read (p_mysql_right mysql_rght);
gboolean mysql_right_update (p_mysql_right mysql_rght, const gchar * right, const gchar * new_value);
gboolean mysql_right_remove (p_mysql_right mysql_rght);

#endif /* __MYSQL_DB_ALL_H__ */
