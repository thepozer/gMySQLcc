
#include "mysql_db_all.h"

p_data_list mysql_user_list_new (p_mysql_server	mysql_srv) {
	return gmysqlcc_data_list_new(mysql_srv);
}

gboolean mysql_user_list_delete (p_data_list mysql_usr_lst) {
	gboolean p_only_not_found = FALSE;
	
	return gmysqlcc_data_list_delete(mysql_usr_lst, &mysql_user_list_clean, &p_only_not_found);
}

void mysql_user_list_clean (p_data_list mysql_usr_lst, void * p_only_not_found) {
	
	gboolean htr_remove_user(gpointer key, gpointer value, gpointer user_data) {
		p_mysql_user mysql_usr = (p_mysql_user)value;
		
		if (!*((gboolean *)user_data) || (*((gboolean *)user_data) && !mysql_usr->found)) {
			g_free (key);
			mysql_user_delete(mysql_usr);
			return TRUE;
		}
		return FALSE;
	}
	
	if (mysql_usr_lst->hsh_list != NULL) {
		g_hash_table_foreach_steal(mysql_usr_lst->hsh_list, &htr_remove_user, p_only_not_found);
	}
}

void mysql_user_list_mark_found (p_data_list mysql_usr_lst, gboolean found) {

	void ht_mark_found_database(gpointer key, gpointer value, gpointer user_data) {
		((p_mysql_user)value)->found = *((gboolean *)user_data);
	}
	
	if (mysql_usr_lst->hsh_list != NULL) {
		g_hash_table_foreach(mysql_usr_lst->hsh_list, &ht_mark_found_database, &found);
	}
}

gboolean mysql_user_list_refresh (p_data_list mysql_usr_lst) {
	p_mysql_query mysql_qry;
	p_mysql_user mysql_usr;
	p_mysql_server mysql_srv;
	GArray * arRow;
	gchar * login;
	gchar * host;
	GString * userName;
	gboolean b_onf = TRUE;
	
	userName = g_string_new("");
	mysql_srv = (p_mysql_server) mysql_usr_lst->data;
	
	gmysqlcc_data_list_mark_all(mysql_usr_lst, FALSE);
	
	mysql_qry = mysql_server_query(mysql_srv, (gchar *)NULL);
	
	if (mysql_query_execute_query(mysql_qry, "SELECT Host, User AS UserName FROM `mysql`.`user` ORDER BY Host, User", FALSE)) {
		
		arRow = mysql_query_get_next_record(mysql_qry);
		
		while (arRow != NULL) {
		
			login = g_array_index(arRow, gchar *, 1);
			host = g_array_index(arRow, gchar *, 0);
			g_string_printf(userName, "'%s'@'%s'", login, host);
			if ((mysql_usr = (p_mysql_user)g_hash_table_lookup(mysql_usr_lst->hsh_list, userName->str)) == NULL) {
				mysql_usr = mysql_user_new(mysql_srv, login, host);
				gmysqlcc_data_list_add(mysql_usr_lst, g_strdup(userName->str), mysql_usr, TRUE);
			}
			
			mysql_user_update_from_db(mysql_usr);
			gmysqlcc_data_list_mark(mysql_usr_lst, userName->str, TRUE);
			
			g_array_free(arRow, TRUE);
			arRow = mysql_query_get_next_record(mysql_qry);
			
		}
	} else {
		return FALSE;
	}
	
	/*mysql_user_list_clean(mysql_usr_lst, &b_onf);*/
	
	mysql_query_delete(mysql_qry);
	g_string_free(userName, TRUE);
	
	return TRUE;
}
