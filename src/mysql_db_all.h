
#ifndef __MYSQL_DB_ALL_H__
#define __MYSQL_DB_ALL_H__

#include <glib.h>
#include <mysql/mysql.h>
#include <libintl.h>

#define _(String) gettext (String)

typedef struct _s_mysql_server {
/* Connection infos */
	gchar *				name;
	gchar *				host;
	unsigned int		port;
	gchar *				user;
	gchar *				passwd;
	gchar *				allowedDbs;
	gchar *				localSock;
	
/* Server protections */
	gboolean			read_only;
	gboolean			write_warning;

/* Server datas */
	GHashTable *		hshDbs;
	GHashTable *		hshUsers;
} s_mysql_server;

typedef s_mysql_server * p_mysql_server;

typedef struct _s_mysql_user {
/* User Infos */
	gchar *					login;
	gchar *					host;
	GHashTable *			hshRights;
	
/* Connection Infos */
	p_mysql_server	mysql_srv;
	
/* Update user list informations */
	gboolean				found;
} s_mysql_user;

typedef s_mysql_user * p_mysql_user;

typedef struct _s_mysql_database {
/* Connection Infos */
	gchar *					name;
	p_mysql_server			mysql_srv;
	
/* Tables Informations */
	GList *					lstTables;
	GHashTable *			hshTables;
	
/* Update database list informations */
	gboolean				found;
} s_mysql_database;

typedef s_mysql_database * p_mysql_database;

typedef struct _s_mysql_table {
/* Connection Infos */
	gchar *						name;
	p_mysql_database			mysql_db;
	
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
	int							nbrField;
	int							editResult;
	int							errCode;
	gchar *						errMsg;
	GArray *					rawHeaders;
	gboolean					can_edit;
	
/* Connection Infos */
	p_mysql_server				mysql_srv;
	gchar *						db_name;
	gchar *						abs_tbl_name;
	gchar *						tbl_name;
	
/* Mysql Infos */
	MYSQL *						mysql_link;
	MYSQL_RES *					mysql_result;
	
/* Charset Infos */
	gchar *						charset;
	GIConv						iconv_from;
	GIConv						iconv_to;
} s_mysql_query;

typedef s_mysql_query * p_mysql_query;

typedef struct _s_mysql_multi_query {
	/* Connection Infos */
	p_mysql_database mysql_db;
	
	/* Execution Infos */
	p_mysql_query		mysql_qry;
	void (* status_callback) (struct _s_mysql_multi_query * mysql_mlt_qry, gboolean error, gpointer user_data);
	gpointer status_user_data;
	
	/* Thread params */
	const gchar * content;
	gboolean stop_error;
	gboolean finished;
	gboolean finish_ok;
	
	/* Report Infos */
	GString *				report;
	gint 						nbr_query;
	gint						nbr_error;
} s_mysql_multi_query;

typedef s_mysql_multi_query * p_mysql_multi_query;
	
typedef struct _s_mysql_row {
/* Data infos */
	GArray *					results;
	
/* Connection infos */
	p_mysql_query				mysql_qry;
	
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

gboolean mysql_server_clean_database_list (p_mysql_server mysql_srv, gboolean only_not_found);
void mysql_server_mark_found_all_databases (p_mysql_server mysql_srv, gboolean found);
gboolean mysql_server_refresh_database_list (p_mysql_server mysql_srv);
p_mysql_database mysql_server_get_database (p_mysql_server mysql_srv, const gchar * db_name);

GArray * mysql_server_get_status (p_mysql_server mysql_srv);
gboolean mysql_server_flush_status (p_mysql_server mysql_srv);

gboolean mysql_server_clean_user_list (p_mysql_server mysql_srv, gboolean only_not_found);
void mysql_server_mark_found_all_users (p_mysql_server mysql_srv, gboolean found);
gboolean mysql_server_refresh_user_list (p_mysql_server mysql_srv);

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
gboolean mysql_user_clean_rights_list (p_mysql_user mysql_usr);
gboolean mysql_user_read_rights (p_mysql_user mysql_usr);


#endif /* __MYSQL_DB_ALL_H__ */
