/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

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
