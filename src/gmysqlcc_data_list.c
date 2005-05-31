
#include "gmysqlcc_data_list.h"

gboolean internal_mysql_data_list_found_key(gpointer key, gpointer value, gpointer user_data);


p_data_list mysql_data_list_new (void * data, GDestroyNotify key_destroy_func, GDestroyNotify value_destroy_func) {
	p_data_list data_lst;
	
	data_lst = (p_data_list) g_try_malloc(sizeof(s_data_list));
	
	if (data_lst == NULL) {
		return NULL; 
	}
	
	data_lst->data = data;
	data_lst->hsh_list = g_hash_table_new_full(&g_str_hash, &g_str_equal, key_destroy_func, value_destroy_func);
	
	return data_lst;
}

gboolean mysql_data_list_delete (p_data_list data_lst) {
	
	if (data_lst == NULL) {
		return TRUE;
	}
	
	g_hash_table_destroy(data_lst->hsh_list);
	
	g_free(data_lst);
	return TRUE;
}

void * mysql_data_list_get_data(p_data_list data_lst) {
	return data_lst->data;
}

gboolean mysql_data_list_is_exist(p_data_list data_lst, gchar * key) {
	return (NULL != g_hash_table_find (data_lst->hsh_list, &internal_mysql_data_list_found_key, key));
}

gboolean mysql_data_list_is_marked(p_data_list data_lst, gchar * key) {
	p_data_list_item data_item = NULL;
	
	data_item = (p_data_list_item)g_hash_table_find (data_lst->hsh_list, &internal_mysql_data_list_found_key, key);
	
	return data_item->mark;
}

void mysql_data_list_mark (p_data_list data_lst, gchar * key, gboolean mark) {
	p_data_list_item data_item = NULL;
	
	data_item = (p_data_list_item)g_hash_table_find (data_lst->hsh_list, &internal_mysql_data_list_found_key, key);
	
	data_item->mark = mark;
}

void mysql_data_list_mark_all (p_data_list data_lst, gboolean mark) {
	void internal_mysql_data_list_set_mark(gpointer key, gpointer value, gpointer user_data) {
		p_data_list_item data_item = (p_data_list_item)value;
		
		data_item->mark = *(gboolean *)user_data;
	}
	
	g_hash_table_foreach(data_lst->hsh_list, &internal_mysql_data_list_set_mark, &mark);
}

gboolean mysql_data_list_is_updated(p_data_list data_lst, gchar * key) {
	p_data_list_item data_item = NULL;
	
	data_item = (p_data_list_item)g_hash_table_find (data_lst->hsh_list, &internal_mysql_data_list_found_key, key);
	
	return data_item->updated;
}

void mysql_data_list_update (p_data_list data_lst, gchar * key, gboolean updated) {
	p_data_list_item data_item = NULL;
	
	data_item = (p_data_list_item)g_hash_table_find (data_lst->hsh_list, &internal_mysql_data_list_found_key, key);
	
	data_item->updated = updated;
}

gpointer mysql_data_list_get(p_data_list data_lst, gchar * key) {
	p_data_list_item data_item = NULL;
	
	data_item = (p_data_list_item)g_hash_table_find (data_lst->hsh_list, &internal_mysql_data_list_found_key, key);
	
	if (data_item == NULL) {
		return NULL;
	} else {
		return data_item->data;
	}
}

gboolean mysql_data_list_add(p_data_list data_lst, gchar * key, gpointer value, gboolean mark) {
	p_data_list_item data_item = NULL;
	
	if (mysql_data_list_is_exist(data_lst, key)) {
		return FALSE;
	}
	
	data_item = (p_data_list_item) g_try_malloc(sizeof(s_data_list_item));
	
	if (data_item == NULL) {
		return FALSE; 
	}
	
	data_item->data = value;
	data_item->mark = mark;
	
	g_hash_table_insert(data_lst->hsh_list, key, data_item);
	
	return TRUE;
}

gboolean mysql_data_list_remove(p_data_list data_lst, gchar * key) {
	p_data_list_item data_item = NULL;
	
	data_item = (p_data_list_item)g_hash_table_find (data_lst->hsh_list, &internal_mysql_data_list_found_key, key);
	
	if (data_item != NULL) {
		g_hash_table_remove(data_lst->hsh_list, key);
		g_free(data_item);
	}
	
	return TRUE;
}

void mysql_data_list_foreach(p_data_list data_lst, GHFunc func, gpointer user_data) {
	typedef struct {
		GHFunc func;
		gpointer user_data;
	} s_external_call;
	
	s_external_call extern_call;
	
	void internal_mysql_data_list_foreach_item (gpointer key, gpointer value, gpointer user_data) {
		p_data_list_item data_item = (p_data_list_item)value;
		s_external_call * p_extern_call = (s_external_call *)user_data;
		
		p_extern_call->func(key, data_item->data, p_extern_call->user_data);
	}
	
	extern_call.func = func;
	extern_call.user_data = user_data;
	
	g_hash_table_foreach(data_lst->hsh_list, internal_mysql_data_list_foreach_item, &extern_call);
	
}

void mysql_data_list_foreach_mark (p_data_list data_lst, GHFunc func, gpointer user_data, gboolean select_mark) {
	
	typedef struct {
		GHFunc func;
		gpointer user_data;
		gboolean select_mark;
	} s_external_call_mark;
	
	s_external_call_mark extern_call;
	
	void internal_mysql_data_list_foreach_item_mark (gpointer key, gpointer value, gpointer user_data) {
		p_data_list_item data_item = (p_data_list_item)value;
		s_external_call_mark * p_extern_call = (s_external_call_mark *)user_data;
		
		if (p_extern_call->select_mark == data_item->mark) {
			p_extern_call->func(key, data_item->data, p_extern_call->user_data);
		}
	}
	
	extern_call.func = func;
	extern_call.user_data = user_data;
	extern_call.select_mark = select_mark;
	
	g_hash_table_foreach(data_lst->hsh_list, &internal_mysql_data_list_foreach_item_mark, &extern_call);
}

void mysql_data_list_foreach_updated (p_data_list data_lst, GHFunc func, gpointer user_data, gboolean updated) {
	
	typedef struct {
		GHFunc func;
		gpointer user_data;
		gboolean updated;
	} s_external_call_update;
	
	s_external_call_update extern_call;
	
	void internal_mysql_data_list_foreach_item_updated (gpointer key, gpointer value, gpointer user_data) {
		p_data_list_item data_item = (p_data_list_item)value;
		s_external_call_update * p_extern_call = (s_external_call_update *)user_data;
		
		if (p_extern_call->updated == data_item->updated) {
			p_extern_call->func(key, data_item->data, p_extern_call->user_data);
		}
	}
	
	extern_call.func = func;
	extern_call.user_data = user_data;
	extern_call.updated = updated;
	
	g_hash_table_foreach(data_lst->hsh_list, &internal_mysql_data_list_foreach_item_updated, &extern_call);
}

void mysql_data_list_clean_mark (p_data_list data_lst, gboolean select_mark) {
	
	gboolean internal_mysql_data_list_clean_item (gpointer key, gpointer value, gpointer user_data) {
		p_data_list_item data_item = (p_data_list_item)value;
		gboolean select_mark = *((gboolean *)user_data);
		
		return (select_mark == data_item->mark);
	}
	
	g_hash_table_foreach_remove(data_lst->hsh_list, &internal_mysql_data_list_clean_item, &select_mark);
}

gboolean internal_mysql_data_list_found_key(gpointer key, gpointer value, gpointer user_data) {
	return (g_ascii_strcasecmp(key, (gchar *)user_data) == 0);
}
