
#include "gmysqlcc_data_list.h"

gboolean internal_gmysqlcc_data_list_found_key(gpointer key, gpointer value, gpointer user_data);


p_data_list gmysqlcc_data_list_new (void * data) {
	p_data_list data_lst;
	
	data_lst = (p_data_list) g_try_malloc(sizeof(s_data_list));
	
	if (data_lst == NULL) {
		return NULL; 
	}
	
	data_lst->data = data;
	data_lst->hsh_list = g_hash_table_new(&g_str_hash, &g_str_equal);
	
	return data_lst;
}

gboolean gmysqlcc_data_list_delete (p_data_list data_lst, gmysqlcc_data_list_clean clean, gpointer data) {
	
	if (data_lst == NULL) {
		return TRUE;
	}
	
	clean(data_lst, data);
	g_hash_table_destroy(data_lst->hsh_list);
	
	g_free(data_lst);
	return TRUE;
}

void * gmysqlcc_data_list_get_data(p_data_list data_lst) {
	return data_lst->data;
}

gboolean gmysqlcc_data_list_is_exist(p_data_list data_lst, gchar * key) {
	return (NULL != g_hash_table_find (data_lst->hsh_list, &internal_gmysqlcc_data_list_found_key, key));
}

gboolean gmysqlcc_data_list_is_marked(p_data_list data_lst, gchar * key) {
	p_data_list_item data_item = NULL;
	
	data_item = (p_data_list_item)g_hash_table_find (data_lst->hsh_list, &internal_gmysqlcc_data_list_found_key, key);
	
	return data_item->mark;
}

void gmysqlcc_data_list_mark (p_data_list data_lst, gchar * key, gboolean mark) {
	p_data_list_item data_item = NULL;
	
	data_item = (p_data_list_item)g_hash_table_find (data_lst->hsh_list, &internal_gmysqlcc_data_list_found_key, key);
	
	data_item->mark = mark;
}

void gmysqlcc_data_list_mark_all (p_data_list data_lst, gboolean mark) {
	void internal_gmysqlcc_data_list_set_mark(gpointer key, gpointer value, gpointer user_data) {
		p_data_list_item data_item = (p_data_list_item)value;
		
		data_item->mark = *(gboolean *)user_data;
	}
	
	g_hash_table_foreach(data_lst->hsh_list, &internal_gmysqlcc_data_list_set_mark, &mark);
}

gpointer gmysqlcc_data_list_get(p_data_list data_lst, gchar * key) {
	p_data_list_item data_item = NULL;
	
	data_item = (p_data_list_item)g_hash_table_find (data_lst->hsh_list, &internal_gmysqlcc_data_list_found_key, key);
	
	if (data_item == NULL) {
		return NULL;
	} else {
		return data_item->data;
	}
}

gboolean gmysqlcc_data_list_add(p_data_list data_lst, gchar * key, gpointer value, gboolean mark) {
	p_data_list_item data_item = NULL;
	
	if (gmysqlcc_data_list_is_exist(data_lst, key)) {
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

gboolean gmysqlcc_data_list_remove(p_data_list data_lst, gchar * key) {
	p_data_list_item data_item = NULL;
	
	data_item = (p_data_list_item)g_hash_table_find (data_lst->hsh_list, &internal_gmysqlcc_data_list_found_key, key);
	
	if (data_item != NULL) {
		g_hash_table_remove(data_lst->hsh_list, key);
		g_free(data_item);
	}
	
	return TRUE;
}

void gmysqlcc_data_list_foreach(p_data_list data_lst, GHFunc func, gpointer user_data, gboolean only_marked) {
	typedef struct _s_external_call {
		GHFunc func;
		gpointer user_data;
		gboolean only_marked;
	} s_external_call;
	
	s_external_call extern_call;
	
	void internal_gmysqlcc_data_list_foreach_item (gpointer key, gpointer value, gpointer user_data) {
		p_data_list_item data_item = (p_data_list_item)value;
		s_external_call * p_extern_call = (s_external_call *)user_data;
		
		if (!p_extern_call->only_marked || (p_extern_call->only_marked && data_item->mark)) {
			p_extern_call->func(key, data_item->data, p_extern_call->user_data);
		}
	}
	
	extern_call.func = func;
	extern_call.user_data = user_data;
	extern_call.only_marked = only_marked;
	
	g_hash_table_foreach(data_lst->hsh_list, internal_gmysqlcc_data_list_foreach_item, &extern_call);
	
}

gboolean internal_gmysqlcc_data_list_found_key(gpointer key, gpointer value, gpointer user_data) {
	return (g_ascii_strcasecmp(key, (gchar *)user_data) == 0);
}
