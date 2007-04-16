
#include "gmlc_misc_hashtable.h"

gboolean gmlc_misc_hash_table_set_flag(GHashTable * htbLstObjects, gpointer pValue, gboolean bNewValue) {
	GObject * oDataItem = NULL;
	
	oDataItem = g_hash_table_lookup(htbLstObjects, pValue);
	g_object_set(oDataItem, "flagged", bNewValue, NULL);
	
	return TRUE;
}

gboolean gmlc_misc_hash_table_set_all_flag(GHashTable * htbLstObjects, gboolean bNewValue) {
		
	void internal_gmlc_misc_hash_table_set_flag (gpointer key, gpointer value, gpointer user_data) {
		GObject * oDataItem = G_OBJECT(value);
		gboolean bFlag = *((gboolean *)user_data);
		
		g_object_set(oDataItem, "flagged", bFlag, NULL);
	}
	
	g_hash_table_foreach(htbLstObjects, &internal_gmlc_misc_hash_table_set_flag, &bNewValue);
	
	return TRUE;
}

gboolean gmlc_misc_hash_table_remove_unflag_object(GHashTable * htbLstObjects) {
	
	gboolean internal_gmlc_misc_hash_table_remove_unflag_object (gpointer key, gpointer value, gpointer user_data) {
		GObject * oDataItem = G_OBJECT(value);
		gboolean bFlag = FALSE;
		
		g_object_get(oDataItem, "flagged", &bFlag, NULL);
		
		return !bFlag;
	}
	
	g_hash_table_foreach_remove(htbLstObjects, &internal_gmlc_misc_hash_table_remove_unflag_object, NULL);
	
	return TRUE;
}
