
#include "mysql_db_all.h"

#include <sys/stat.h>
#include <sys/types.h>

p_mysql_server mysql_server_new () {
	p_mysql_server mysql_srv;
	
	mysql_srv = (p_mysql_server) g_try_malloc(sizeof(s_mysql_server));
	
	if (mysql_srv == (p_mysql_server)NULL) {
		return (p_mysql_server)NULL; /* return NULL pointer */
	}
	
	mysql_srv->name = (gchar *) NULL;
	mysql_srv->host = (gchar *) NULL;
	mysql_srv->port = 3306;
	mysql_srv->user = (gchar *) NULL;
	mysql_srv->passwd = (gchar *) NULL;
	mysql_srv->allowedDbs = (gchar *) NULL;
	mysql_srv->localSock = (gchar *) NULL;
	mysql_srv->hshDbs = g_hash_table_new(&g_str_hash, &g_str_equal);
	mysql_srv->hshUsers = g_hash_table_new(&g_str_hash, &g_str_equal);
	
	return mysql_srv;
}

gboolean mysql_server_delete (p_mysql_server mysql_srv) {
	
	if (mysql_srv == (p_mysql_server)NULL) {
		return TRUE;
	}
	
	g_free(mysql_srv->name);
	g_free(mysql_srv->host);
	g_free(mysql_srv->user);
	g_free(mysql_srv->passwd);
	g_free(mysql_srv->allowedDbs);
	g_free(mysql_srv->localSock);
	
	mysql_server_clean_database_list(mysql_srv, FALSE);
	g_hash_table_destroy(mysql_srv->hshDbs);
	
	g_free(mysql_srv);
	return TRUE;
}

p_mysql_query mysql_server_query (p_mysql_server mysql_srv, const gchar * db_name) {
	return mysql_query_new(mysql_srv, db_name);
}

gboolean mysql_server_clean_database_list (p_mysql_server mysql_srv, gboolean only_not_found) {
	
	gboolean htr_remove_database(gpointer key, gpointer value, gpointer user_data) {
		p_mysql_database mysql_db = (p_mysql_database)value;
		
		if (!*((gboolean *)user_data) || (*((gboolean *)user_data) && !mysql_db->found)) {
			mysql_database_delete(mysql_db);
			return TRUE;
		}
		return FALSE;
	}
	
	if (mysql_srv->hshDbs != (GHashTable *) NULL) {
		g_hash_table_foreach_steal(mysql_srv->hshDbs, &htr_remove_database, (gpointer)&only_not_found);
	}
	
	return TRUE;
}

void mysql_server_mark_found_all_databases (p_mysql_server mysql_srv, gboolean found) {

	void ht_mark_found_database(gpointer key, gpointer value, gpointer user_data) {
		((p_mysql_database)value)->found = *((gboolean *)user_data);
	}
	
	if (mysql_srv->hshDbs != (GHashTable *) NULL) {
		g_hash_table_foreach(mysql_srv->hshDbs, &ht_mark_found_database, (gpointer)&found);
	}
}

gboolean mysql_server_refresh_database_list (p_mysql_server mysql_srv) {
	p_mysql_query mysql_qry;
	p_mysql_database mysql_db;
	GArray * arRow;
	gchar * db_name;
	
	mysql_server_mark_found_all_databases(mysql_srv, FALSE);
	
	mysql_qry = mysql_server_query(mysql_srv, (gchar *)NULL);
	
	if (mysql_query_execute_query(mysql_qry, "SHOW DATABASES", FALSE)) {
	
		arRow = mysql_query_get_next_record(mysql_qry);
		
		while (arRow != (GArray *)NULL) {
		
			db_name = g_array_index(arRow, gchar *, 0);
			if ((mysql_db = (p_mysql_database)g_hash_table_lookup(mysql_srv->hshDbs, db_name)) == (p_mysql_database)NULL) {
				mysql_db = mysql_database_new(mysql_srv, db_name);
				g_hash_table_insert(mysql_srv->hshDbs, db_name, mysql_db);
			} else {
				mysql_db->found = TRUE;
			}
			
			g_array_free(arRow, TRUE);
			arRow = mysql_query_get_next_record(mysql_qry);
			
		}
	} else {
		return FALSE;
	}
	
	mysql_server_clean_database_list(mysql_srv, TRUE);
	
	mysql_query_delete(mysql_qry);
	
	return TRUE;
}

p_mysql_database mysql_server_get_database (p_mysql_server mysql_srv, const gchar * db_name){
	return (p_mysql_database)g_hash_table_lookup(mysql_srv->hshDbs, db_name);
}
GArray * mysql_server_get_status (p_mysql_server mysql_srv) {
	p_mysql_query mysql_qry;
	GArray * arStatus = (GArray *)NULL;
	
	mysql_qry = mysql_server_query(mysql_srv, "mysql");
	
	if (!mysql_query_execute_query(mysql_qry, "SHOW STATUS", FALSE)) {
		
		arStatus = mysql_query_get_all_records(mysql_qry);
		
	}
	
	mysql_query_delete(mysql_qry);

	return arStatus;
}

gboolean mysql_server_flush_status (p_mysql_server mysql_srv) {
	p_mysql_query mysql_qry;
	gboolean ret;
	
	mysql_qry = mysql_server_query(mysql_srv, "mysql");
	
	ret = mysql_query_execute_query(mysql_qry, "FLUSH STATUS;", FALSE);
	
	mysql_query_delete(mysql_qry);

	return ret;
}

gboolean mysql_server_clean_user_list (p_mysql_server mysql_srv, gboolean only_not_found) {
	
	gboolean htr_remove_user(gpointer key, gpointer value, gpointer user_data) {
		p_mysql_user mysql_usr = (p_mysql_user)value;
		
		if (!*((gboolean *)user_data) || (*((gboolean *)user_data) && !mysql_usr->found)) {
			g_free (key);
			mysql_user_delete(mysql_usr);
			return TRUE;
		}
		return FALSE;
	}
	
	if (mysql_srv->hshUsers != (GHashTable *) NULL) {
		g_hash_table_foreach_steal(mysql_srv->hshUsers, &htr_remove_user, (gpointer)&only_not_found);
	}
	
	return TRUE;
}

void mysql_server_mark_found_all_users (p_mysql_server mysql_srv, gboolean found) {

	void ht_mark_found_database(gpointer key, gpointer value, gpointer user_data) {
		((p_mysql_user)value)->found = *((gboolean *)user_data);
	}
	
	if (mysql_srv->hshUsers != (GHashTable *) NULL) {
		g_hash_table_foreach(mysql_srv->hshUsers, &ht_mark_found_database, (gpointer)&found);
	}
}

gboolean mysql_server_refresh_user_list (p_mysql_server mysql_srv) {
	p_mysql_query mysql_qry;
	p_mysql_user mysql_usr;
	GArray * arRow;
	gchar * login;
	gchar * host;
	GString * userName;
	
	userName = g_string_new("");
	
	mysql_server_mark_found_all_databases(mysql_srv, FALSE);
	
	mysql_qry = mysql_server_query(mysql_srv, (gchar *)NULL);
	
	if (mysql_query_execute_query(mysql_qry, "SELECT Host, User AS UserName FROM `mysql`.`user` ORDER BY Host, User", FALSE)) {
	
		arRow = mysql_query_get_next_record(mysql_qry);
		
		while (arRow != (GArray *)NULL) {
		
			login = g_array_index(arRow, gchar *, 1);
			host = g_array_index(arRow, gchar *, 0);
			g_string_printf(userName, "'%s'@'%s'", login, host);
			if ((mysql_usr = (p_mysql_user)g_hash_table_lookup(mysql_srv->hshUsers, userName->str)) == (p_mysql_user)NULL) {
				mysql_usr = mysql_user_new(mysql_srv, login, host);
				g_hash_table_insert(mysql_srv->hshUsers, userName->str, mysql_usr);
			} else {
				mysql_usr->found = TRUE;
			}
			
			g_array_free(arRow, TRUE);
			arRow = mysql_query_get_next_record(mysql_qry);
			
		}
	} else {
		return FALSE;
	}
	
	mysql_server_clean_user_list(mysql_srv, TRUE);
	
	mysql_query_delete(mysql_qry);
	g_string_free(userName, TRUE);
	
	return TRUE;
}
