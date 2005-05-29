#ifndef __GMYSQLCC_DATA_LIST_H__
#define __GMYSQLCC_DATA_LIST_H__

#include <glib.h>

typedef struct _s_data_list {
	void *				data;
	GHashTable *	hsh_list;
} s_data_list;

typedef s_data_list * p_data_list;

typedef struct _s_data_list_item {
	void *		data;
	gboolean	mark;
} s_data_list_item;

typedef s_data_list_item * p_data_list_item;

typedef void (*gmysqlcc_data_list_clean) (p_data_list data_lst, gpointer data);

p_data_list gmysqlcc_data_list_new (void * data);
gboolean gmysqlcc_data_list_delete (p_data_list data_lst, gmysqlcc_data_list_clean clean, gpointer data);
void * gmysqlcc_data_list_get_data (p_data_list data_lst);

gboolean gmysqlcc_data_list_is_exist  (p_data_list data_lst, gchar * key);
gboolean gmysqlcc_data_list_is_marked (p_data_list data_lst, gchar * key);
void     gmysqlcc_data_list_mark      (p_data_list data_lst, gchar * key, gboolean mark);
void     gmysqlcc_data_list_mark_all  (p_data_list data_lst, gboolean mark);
gpointer gmysqlcc_data_list_get       (p_data_list data_lst, gchar * key);
gboolean gmysqlcc_data_list_add       (p_data_list data_lst, gchar * key, gpointer value, gboolean mark);
gboolean gmysqlcc_data_list_remove    (p_data_list data_lst, gchar * key);
void     gmysqlcc_data_list_foreach   (p_data_list data_lst, GHFunc func, gpointer user_data, gboolean only_marked);


#endif /* __GMYSQLCC_DATA_LIST_H__ */
