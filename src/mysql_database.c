
#include "mysql_db_all.h"

p_mysql_database mysql_database_new_create(p_mysql_server mysql_srv, const gchar * db_name) {
	p_mysql_query mysql_qry;
	GString * query;
	gboolean b_ret;
	
	if (db_name == NULL) {
		return NULL;
	}
	
	mysql_qry = mysql_server_query(mysql_srv, NULL);
	if (mysql_qry == NULL) {
		return NULL;
	}
	
	query = g_string_new("");
	g_string_printf(query, "CREATE DATABASE `%s`", db_name);
	
	b_ret = mysql_query_execute_query(mysql_qry, query->str, FALSE);
	
	mysql_query_delete(mysql_qry);
	g_string_free (query, TRUE);
	
	if (b_ret) {
		return mysql_database_new(mysql_srv, db_name);
	} else {
		return NULL;
	}
}

p_mysql_database mysql_database_new(p_mysql_server mysql_srv, const gchar * db_name) {
	p_mysql_database mysql_db;
	
	mysql_db = (p_mysql_database) g_try_malloc(sizeof(s_mysql_database));
	
	if (mysql_db == NULL) {
		return NULL; /* return NULL pointer */
	}
	
	mysql_db->mysql_srv = mysql_srv;
	mysql_db->name = g_strdup(db_name);
	mysql_db->hshTables = g_hash_table_new(&g_str_hash, &g_str_equal);
	mysql_db->found = TRUE;
		
	return mysql_db;
}

gboolean mysql_database_delete(p_mysql_database mysql_db) {
	
	if (mysql_db == NULL) {
		return TRUE;
	}
	
	mysql_database_clean_table_list(mysql_db);
	
	g_free(mysql_db->name);

	g_free(mysql_db);
	return TRUE;
	
}

p_mysql_query mysql_database_query (p_mysql_database mysql_db) {
	return mysql_query_new(mysql_db->mysql_srv, mysql_db->name);	
}

gboolean mysql_database_clean_table_list (p_mysql_database mysql_db) {
	
	gboolean htr_remove_table(gpointer key, gpointer value, gpointer user_data) {
		mysql_table_delete((p_mysql_table)value);
		return TRUE;
	}

	if (mysql_db->hshTables !=  NULL) {
		g_hash_table_foreach_steal(mysql_db->hshTables, &htr_remove_table, NULL);
		g_hash_table_destroy(mysql_db->hshTables);
	}
	
	return TRUE;
}

gboolean mysql_database_refresh_table_list (p_mysql_database mysql_db) {
	p_mysql_query mysql_qry;
	p_mysql_table mysql_tbl;
	GArray * arRow;
	GString * ssql;
	gchar * tbl_name;
	guint db_version;
	
	db_version = mysql_server_get_version(mysql_db->mysql_srv, FALSE);

	mysql_qry = mysql_database_query(mysql_db);
	
	ssql = g_string_new("");
	g_string_printf(ssql, "SHOW TABLE STATUS FROM `%s`", mysql_db->name);
	
	if (mysql_query_execute_query(mysql_qry, ssql->str, FALSE)) {
	
		arRow = mysql_query_get_next_record(mysql_qry);
		
		while (arRow != (GArray *)NULL) {
		
			tbl_name = g_array_index(arRow, gchar *, 0);
			if ((mysql_tbl = (p_mysql_table)g_hash_table_lookup(mysql_db->hshTables, tbl_name)) == (p_mysql_table)NULL) {
				mysql_tbl = mysql_table_new(mysql_db, tbl_name);
				if (db_version <= 40100) {
					mysql_tbl->size = g_strdup(g_array_index(arRow, gchar *, 5));
					mysql_tbl->nbrRow = g_strdup(g_array_index(arRow, gchar *, 3));
					mysql_tbl->type = g_strdup(g_array_index(arRow, gchar *, 1));
				} else {
					mysql_tbl->size = g_strdup(g_array_index(arRow, gchar *, 6));
					mysql_tbl->nbrRow = g_strdup(g_array_index(arRow, gchar *, 4));
					mysql_tbl->type = g_strdup(g_array_index(arRow, gchar *, 1));
				}
				g_hash_table_insert(mysql_db->hshTables, tbl_name, mysql_tbl);
			}
			
			g_array_free(arRow, TRUE);
			arRow = mysql_query_get_next_record(mysql_qry);
		}
		
		g_string_free(ssql, TRUE);
		mysql_query_delete(mysql_qry);
		
		return TRUE;
	} else {
	
		g_string_free(ssql, TRUE);
		mysql_query_delete(mysql_qry);
		
		return FALSE;
	}
}

p_mysql_table mysql_database_get_table (p_mysql_database mysql_db, const gchar * tbl_name) {
	return (p_mysql_table)g_hash_table_lookup(mysql_db->hshTables, tbl_name);
}
