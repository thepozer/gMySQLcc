
#ifndef __MYSQL_DB_ALL_H__
#define __MYSQL_DB_ALL_H__

#include <glib.h>
#include <mysql/mysql.h>

typedef struct _s_mysql_server {
/* Connection infos */
	gchar * name;
	gchar * host;
	unsigned int port;
	gchar * user;
	gchar * passwd;
	gchar * allowedDbs;
	gchar * localSock;
	GList * lstDbs;
	GHashTable * hshDbs;
	GHashTable * hshUsers;
} s_mysql_server;

typedef s_mysql_server * p_mysql_server;

typedef struct _s_mysql_user {
/* User Infos */
	gchar * login;
	gchar * host;
	GHashTable * hshRights;
/* Connection Infos */
	p_mysql_server mysql_srv;
/* Update user list informations */
	gboolean found;
} s_mysql_user;

typedef s_mysql_user * p_mysql_user;

typedef struct _s_mysql_database {
/* Connection Infos */
	gchar * name;
	p_mysql_server mysql_srv;
/* Tables Informations */
	GList * lstTables;
	GHashTable * hshTables;
/* Update database list informations */
	gboolean found;
} s_mysql_database;

typedef s_mysql_database * p_mysql_database;

typedef struct _s_mysql_table {
/* Connection Infos */
	gchar * name;
	p_mysql_database mysql_db;
/* Status infos */
	gchar * nbrRow;
	gchar * size;
	gchar * type;
} s_mysql_table;

typedef s_mysql_table * p_mysql_table;

typedef struct _s_mysql_query {
/* Query Infos */
	gchar * query;
/* Result Infos */
	int nbrField;
	int editResult;
	int errCode;
	gchar * errMsg;
	GArray * rawHeaders;
	gboolean can_edit;
/* Connection Infos */
	p_mysql_server mysql_srv;
	gchar * db_name;
	gchar * abs_tbl_name;
/* Mysql Infos */
	MYSQL * mysql_link;
	MYSQL_RES * mysql_result;
/* Charset Infos */
	gchar * charset;
	GIConv iconv_from;
	GIConv iconv_to;
} s_mysql_query;

typedef s_mysql_query * p_mysql_query;

typedef struct _s_mysql_row {
/* Data infos */
	GArray * results;
/* Connection infos */
	p_mysql_query mysql_qry;
/* Request for update infos */
	gchar * abs_tbl_name;
	gchar * primary_where_part;
} s_mysql_row;

typedef s_mysql_row * p_mysql_row;


/***** Dump informations *****/

typedef struct _s_dump_table_params {
	gboolean drop_table;
	gboolean structure;
	gboolean data;
	gboolean data_complete_insert;
	gchar * sql_filename;
} s_dump_table_params;

typedef s_dump_table_params * p_dump_table_params;

typedef struct _s_dump_database_params {
	gboolean drop_database;
	gboolean use_database;
	gchar * sql_filename;
	s_dump_table_params table;
} s_dump_database_params;

typedef s_dump_database_params * p_dump_database_params;

typedef struct _s_dump_server_params {
	gchar * sql_filename;
	gboolean separate_file;
	gchar * base_directory;
	gboolean group_in_directory;
	gchar * group_directory;
	s_dump_database_params database;
} s_dump_server_params;

typedef s_dump_server_params * p_dump_server_params;

/***** Server functions *****/

p_mysql_server mysql_server_new ();
gboolean mysql_server_delete (p_mysql_server mysql_srv);

p_mysql_query mysql_server_query (p_mysql_server mysql_srv, const gchar * db_name);

gboolean mysql_server_clean_database_list (p_mysql_server mysql_srv, gboolean only_not_found);
void mysql_server_mark_found_all_databases (p_mysql_server mysql_srv, gboolean found);
gboolean mysql_server_refresh_database_list (p_mysql_server mysql_srv);
p_mysql_database mysql_server_get_database (p_mysql_server mysql_srv, const gchar * db_name);

GString * mysql_server_dump (p_mysql_server mysql_srv, const p_dump_server_params params);
gboolean mysql_server_dump_direct (p_mysql_server mysql_srv, const p_dump_server_params params, GIOChannel * file, const gchar * charset);

GArray * mysql_server_get_status (p_mysql_server mysql_srv);
gboolean mysql_server_flush_status (p_mysql_server mysql_srv);

gboolean mysql_server_clean_user_list (p_mysql_server mysql_srv, gboolean only_not_found);
void mysql_server_mark_found_all_users (p_mysql_server mysql_srv, gboolean found);
gboolean mysql_server_refresh_user_list (p_mysql_server mysql_srv);

/***** Database functions *****/

p_mysql_database mysql_database_new(p_mysql_server mysql_srv, const gchar * db_name);
gboolean mysql_database_delete(p_mysql_database mysql_db);

p_mysql_query mysql_database_query (p_mysql_database mysql_db);

gboolean mysql_database_clean_table_list (p_mysql_database mysql_db);
gboolean mysql_database_refresh_table_list (p_mysql_database mysql_db);
p_mysql_table mysql_database_get_table (p_mysql_database mysql_db, const gchar * tbl_name);

GString * mysql_database_dump (p_mysql_database mysql_db, const p_dump_database_params params);
gboolean mysql_database_dump_direct (p_mysql_database mysql_db, const p_dump_database_params params, GIOChannel * file, const gchar * charset);

/***** Table functions *****/

p_mysql_table mysql_table_new(p_mysql_database mysql_db, const gchar * tbl_name);
gboolean mysql_table_delete(p_mysql_table mysql_tbl);

p_mysql_query mysql_table_query (p_mysql_table mysql_tbl);

GString * mysql_table_dump (p_mysql_table mysql_tbl, const p_dump_table_params params);
gboolean mysql_table_dump_direct (p_mysql_table mysql_tbl, const p_dump_table_params params, GIOChannel * file, const gchar * charset);

/***** Query functions *****/

p_mysql_query mysql_query_new(p_mysql_server mysql_srv, const gchar * db_name);
gboolean mysql_query_delete(p_mysql_query mysql_qry);

p_mysql_query mysql_query_duplicate(p_mysql_query mysql_qry);
p_mysql_database mysql_query_get_database(p_mysql_query mysql_qry);

gboolean mysql_query_execute_query(p_mysql_query mysql_qry, const gchar * query, gboolean add_history);
gboolean mysql_query_free_query(p_mysql_query mysql_qry);

GArray * mysql_query_get_headers(p_mysql_query mysql_qry);
GArray * mysql_query_get_next_record(p_mysql_query mysql_qry);
GArray * mysql_query_get_all_records(p_mysql_query mysql_qry);

gchar * mysql_query_get_absolute_table_name (p_mysql_query mysql_qry);
gchar * mysql_query_get_primary_where (p_mysql_query mysql_qry, GArray * datas);
gboolean mysql_query_is_editable (p_mysql_query mysql_qry);

gboolean mysql_query_set_can_edit (p_mysql_query mysql_qry, gboolean new_value);
gboolean mysql_query_get_can_edit (p_mysql_query mysql_qry);

gchar * mysql_query_get_charset(p_mysql_query mysql_qry);
gboolean mysql_query_change_charset (p_mysql_query mysql_qry, const gchar * charset);

/***** Row functions *****/

p_mysql_row mysql_row_new_next_record (p_mysql_query mysql_qry);
gboolean mysql_row_free (p_mysql_row mysql_rw);

gboolean mysql_row_is_editable (p_mysql_row mysql_rw);
gchar * mysql_row_get_field_value(p_mysql_row mysql_rw, int idx);
gchar * mysql_row_set_field_value(p_mysql_row mysql_rw, int idx, const gchar * new_value);
gboolean mysql_row_delete(p_mysql_row mysql_rw);

/***** Dump functions ******/

GString * mysql_dump_database_struct (const gchar * db_name, gboolean drop_database, gboolean use_database);
GString * mysql_dump_table_struct (p_mysql_query mysql_qry, const gchar * tbl_name, gboolean drop_table);
GString * mysql_dump_table_data (p_mysql_query mysql_qry, const gchar * tbl_name, gboolean complete_insert);

gboolean mysql_dump_database_struct_direct (const gchar * db_name, gboolean drop_database, gboolean use_database, GIOChannel * file);
gboolean mysql_dump_table_struct_direct (p_mysql_query mysql_qry, const gchar * tbl_name, gboolean drop_table, GIOChannel * file);
gboolean mysql_dump_table_data_direct (p_mysql_query mysql_qry, const gchar * tbl_name, gboolean complete_insert, GIOChannel * file);

/*
GString * mysql_dump_query_xml (p_mysql_query mysql_qry, gboolean bGlobal, gboolean bDatabase, gboolean bTable);
GString * mysql_dump_query_csv (p_mysql_query mysql_qry);
GString * mysql_dump_query_sql (p_mysql_query mysql_qry, gboolean complete_insert);
*/

gboolean mysql_dump_query_xml_direct (p_mysql_query mysql_qry, GIOChannel * file, gboolean bGlobal, gboolean bDatabase, gboolean bTable, gboolean cdata);
gboolean mysql_dump_query_csv_direct (p_mysql_query mysql_qry, GIOChannel * file);
gboolean mysql_dump_query_sql_direct (p_mysql_query mysql_qry, gboolean complete_insert, GIOChannel * file);

/***** User functions *****/

p_mysql_user mysql_user_new (p_mysql_server mysql_srv, const gchar * login, const gchar * host);
gboolean mysql_user_delete (p_mysql_user mysql_usr);
gboolean mysql_user_clean_rights_list (p_mysql_user mysql_usr);
gboolean mysql_user_read_rights (p_mysql_user mysql_usr);


#endif /* __MYSQL_DB_ALL_H__ */
