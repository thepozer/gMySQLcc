#ifndef __MYSQL_DATA_LIST_H__
#define __MYSQL_DATA_LIST_H__

#include <glib.h>

typedef void (*data_delete_one) (gpointer * , gpointer user_data);

typedef struct _s_data_list {
	void *				data;
	GHashTable *	hsh_list;
} s_data_list;

typedef s_data_list * p_data_list;

typedef struct _s_data_list_item {
	void *		data;
	gboolean	mark;
	gboolean	updated;
} s_data_list_item;

typedef s_data_list_item * p_data_list_item;

p_data_list mysql_data_list_new          (void * data, GDestroyNotify key_destroy_func, GDestroyNotify value_destroy_func);
gboolean mysql_data_list_delete          (p_data_list data_lst);
void * mysql_data_list_get_data          (p_data_list data_lst);

gboolean mysql_data_list_is_exist        (p_data_list data_lst, gchar * key);
gboolean mysql_data_list_is_marked       (p_data_list data_lst, gchar * key);
void     mysql_data_list_mark            (p_data_list data_lst, gchar * key, gboolean mark);
void     mysql_data_list_mark_all        (p_data_list data_lst, gboolean mark);
gboolean mysql_data_list_is_updated      (p_data_list data_lst, gchar * key);
void     mysql_data_list_update          (p_data_list data_lst, gchar * key, gboolean updated);
gpointer mysql_data_list_get             (p_data_list data_lst, gchar * key);
gboolean mysql_data_list_add             (p_data_list data_lst, gchar * key, gpointer value, gboolean mark);
gboolean mysql_data_list_remove          (p_data_list data_lst, gchar * key);
void     mysql_data_list_foreach         (p_data_list data_lst, GHFunc func, gpointer user_data);
void     mysql_data_list_foreach_mark    (p_data_list data_lst, GHFunc func, gpointer user_data, gboolean select_mark);
void     mysql_data_list_foreach_updated (p_data_list data_lst, GHFunc func, gpointer user_data, gboolean updated);
void     mysql_data_list_clean_mark      (p_data_list data_lst, gboolean select_mark);

#endif /* __MYSQL_DATA_LIST_H__ */
