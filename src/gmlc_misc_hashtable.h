
#ifndef __GMLC_MISC_HASHTABLE_H__
#define __GMLC_MISC_HASHTABLE_H__

#include <glib.h>
#include <glib-object.h>

#define UNUSED_VAR(x) (x = x)

G_BEGIN_DECLS

gboolean gmlc_misc_hash_table_set_flag(GHashTable * htbLstObjects, gpointer pValue, gboolean bNewValue);
gboolean gmlc_misc_hash_table_set_all_flag(GHashTable * htbLstObjects, gboolean bNewValue);
gboolean gmlc_misc_hash_table_remove_unflag_object(GHashTable * htbLstObjects);

G_END_DECLS

#endif /* __GMLC_MISC_HASHTABLE_H__ */
