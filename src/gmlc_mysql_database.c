/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <string.h>

#include "gmlc_mysql_database.h"
#include "gmlc_misc_hashtable.h"

static void gmlc_mysql_database_finalize (GmlcMysqlDatabase * pGmlcMysqlDb);
static void gmlc_mysql_database_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);
static void gmlc_mysql_database_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);

static void gmlc_mysql_database_interface_structure_init (gpointer g_iface, gpointer iface_data);
static gchar * gmlc_mysql_database_structure_get_create (GmlcMysqlDatabase * pGmlcMysqlDb, gboolean bMyself, const gchar * pcOtherName);
static gchar * gmlc_mysql_database_structure_get_alter (GmlcMysqlDatabase * pGmlcMysqlDb, gboolean bMyself, const gchar * pcOtherName);
static gchar * gmlc_mysql_database_structure_get_drop (GmlcMysqlDatabase * pGmlcMysqlDb, gboolean bMyself, const gchar * pcOtherName);

enum {
	PROP_0,
	PROP_FLAGGED,
	PROP_SERVER,
	PROP_DB_NAME
};

G_DEFINE_TYPE_WITH_CODE (GmlcMysqlDatabase, gmlc_mysql_database, G_TYPE_OBJECT, 
	G_IMPLEMENT_INTERFACE (GMLC_MYSQL_TYPE_STRUCTURE, gmlc_mysql_database_interface_structure_init));

static void gmlc_mysql_database_interface_structure_init (gpointer g_iface, gpointer iface_data) {
	GmlcMysqlStructureInterface * pIface = (GmlcMysqlStructureInterface *) g_iface;
	pIface->get_create = (gchar * (*) (GmlcMysqlStructure * poSelf, gboolean bMyself, const gchar * pcOtherName))gmlc_mysql_database_structure_get_create;
	pIface->get_alter = (gchar * (*) (GmlcMysqlStructure * poSelf, gboolean bMyself, const gchar * pcOtherName))gmlc_mysql_database_structure_get_alter;
	pIface->get_drop = (gchar * (*) (GmlcMysqlStructure * poSelf, gboolean bMyself, const gchar * pcOtherName))gmlc_mysql_database_structure_get_drop;
}

static void gmlc_mysql_database_class_init (GmlcMysqlDatabaseClass *pClass) {
	GObjectClass *pObjClass = G_OBJECT_CLASS(pClass);
	
	pObjClass->finalize = (GObjectFinalizeFunc) gmlc_mysql_database_finalize;
	
	pObjClass->get_property = gmlc_mysql_database_get_property;
	pObjClass->set_property = gmlc_mysql_database_set_property;
	
	g_object_class_install_property(pObjClass, PROP_SERVER, 
		g_param_spec_object("server", "Server object", "Server object", GMLC_MYSQL_TYPE_SERVER, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property(pObjClass, PROP_DB_NAME, 
		g_param_spec_string("db_name", "Database name", "Name of the database", "", G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property(pObjClass, PROP_FLAGGED, 
		g_param_spec_boolean("flagged", "Object Flagged", "Object Flagged", FALSE, G_PARAM_READWRITE));
}

static void gmlc_mysql_database_init (GmlcMysqlDatabase *pGmlcMysqlDb) {
	pGmlcMysqlDb->pGmlcMysqlSrv = NULL;/* r-c */
	pGmlcMysqlDb->pcDbName = NULL;		/* r-c */
	pGmlcMysqlDb->htbTables = g_hash_table_new_full(&g_str_hash, &g_str_equal, &g_free, &g_object_unref);
	pGmlcMysqlDb->htbViews = g_hash_table_new_full(&g_str_hash, &g_str_equal, &g_free, &g_object_unref);
	pGmlcMysqlDb->htbFunctions = g_hash_table_new_full(&g_str_hash, &g_str_equal, &g_free, &g_object_unref);
	pGmlcMysqlDb->htbProcedures = g_hash_table_new_full(&g_str_hash, &g_str_equal, &g_free, &g_object_unref);
}

static void gmlc_mysql_database_finalize (GmlcMysqlDatabase *pGmlcMysqlDb) {
	g_free(pGmlcMysqlDb->pcDbName);
}

static void gmlc_mysql_database_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec) {
	GmlcMysqlDatabase * pGmlcMysqlDb = GMLC_MYSQL_DATABASE(object);
	
	switch (prop_id) {
		case PROP_SERVER :
			g_value_set_object(value, pGmlcMysqlDb->pGmlcMysqlSrv);
			break;
		case PROP_DB_NAME :
			g_value_set_string(value, pGmlcMysqlDb->pcDbName);
			break;
		case PROP_FLAGGED :
			g_value_set_boolean(value, pGmlcMysqlDb->bFlagged);
			break;
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

static void gmlc_mysql_database_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec) {
	GmlcMysqlDatabase * pGmlcMysqlDb = GMLC_MYSQL_DATABASE(object);
	
	switch (prop_id) {
		case PROP_SERVER :
			pGmlcMysqlDb->pGmlcMysqlSrv = g_value_get_object(value);
			break;
		case PROP_DB_NAME :
			g_free(pGmlcMysqlDb->pcDbName);
			
			pGmlcMysqlDb->pcDbName = g_value_dup_string(value);
			break;
		case PROP_FLAGGED :
			pGmlcMysqlDb->bFlagged = g_value_get_boolean(value);
			break;
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

static gchar * gmlc_mysql_database_structure_get_create (GmlcMysqlDatabase * pGmlcMysqlDb, gboolean bMyself, const gchar * pcOtherName) {
	const gchar * pcDbName = NULL;
	gchar * pcDefCharset = NULL, * pcDefCollation = NULL, * pcSqlQuery = NULL;
	
	if (bMyself) {
		pcDbName = pGmlcMysqlDb->pcDbName;
	} else if (pcOtherName != NULL) {
		pcDbName = pcOtherName; 
	} else {
		pcDbName = "<Name of the database>";
	}
	
	pcDefCharset = gmlc_mysql_query_static_get_one_result(pGmlcMysqlDb->pGmlcMysqlSrv, NULL, "SHOW LOCAL VARIABLES LIKE 'character_set_server'", 1);
	pcDefCollation = gmlc_mysql_query_static_get_one_result(pGmlcMysqlDb->pGmlcMysqlSrv, NULL, "SHOW LOCAL VARIABLES LIKE 'collation_server'", 1);
	
	pcSqlQuery = g_strdup_printf("CREATE DATABASE `%s`\n\tCHARACTER SET '%s'\n\tCOLLATE '%s';", pcDbName, pcDefCharset, pcDefCollation);
	
	return pcSqlQuery;
}

static gchar * gmlc_mysql_database_structure_get_alter (GmlcMysqlDatabase * pGmlcMysqlDb, gboolean bMyself, const gchar * pcOtherName) {
	const gchar * pcDbName = NULL;
	gchar * pcDefCharset = NULL, * pcDefCollation = NULL, * pcSqlQuery = NULL;
	
	if (bMyself) {
		pcDbName = pGmlcMysqlDb->pcDbName;
	} else if (pcOtherName != NULL) {
		pcDbName = pcOtherName; 
	} else {
		pcDbName = "<Name of the database>";
	}
	
	pcDefCharset = gmlc_mysql_query_static_get_one_result(pGmlcMysqlDb->pGmlcMysqlSrv, NULL, "SHOW LOCAL VARIABLES LIKE 'character_set_server'", 1);
	pcDefCollation = gmlc_mysql_query_static_get_one_result(pGmlcMysqlDb->pGmlcMysqlSrv, NULL, "SHOW LOCAL VARIABLES LIKE 'collation_server'", 1);
	
	pcSqlQuery = g_strdup_printf("ALTER DATABASE `%s`\n\tCHARACTER SET '%s'\n\tCOLLATE '%s';", pcDbName, pcDefCharset, pcDefCollation);
	
	return pcSqlQuery;
}

static gchar * gmlc_mysql_database_structure_get_drop (GmlcMysqlDatabase * pGmlcMysqlDb, gboolean bMyself, const gchar * pcOtherName) {
	const gchar * pcDbName = NULL;
	gchar * pcSqlQuery = NULL;
	
	if (bMyself) {
		pcDbName = pGmlcMysqlDb->pcDbName;
	} else if (pcOtherName != NULL) {
		pcDbName = pcOtherName; 
	} else {
		pcDbName = "<Name of the database>";
	}
	
	pcSqlQuery = g_strdup_printf("DROP DATABASE `%s`;", pcDbName);
	
	return pcSqlQuery;
}

GmlcMysqlDatabase * gmlc_mysql_database_new (GmlcMysqlServer * pGmlcMysqlSrv, const gchar * pcDbName) {
	return GMLC_MYSQL_DATABASE(g_object_new(GMLC_MYSQL_TYPE_DATABASE, "server", pGmlcMysqlSrv, "db_name", pcDbName, NULL));
}

GArray * gmlc_mysql_database_tables_name_list(GmlcMysqlDatabase * pGmlcMysqlDb, gboolean bUpdateList) {
	GArray * arList = NULL;
	
	void internal_gmlc_mysql_database_tables_name_list_append_name (gpointer key, gpointer value, gpointer user_data) {
		gchar * pcName = (gchar *) key;
		GArray * arList = (GArray *) user_data;
		UNUSED_VAR(value);
		
		g_array_append_val(arList, pcName);
	}
	
	if (bUpdateList) {
		gmlc_mysql_database_update_tables_list(pGmlcMysqlDb);
	}
	
	arList = g_array_new(TRUE, TRUE, sizeof(gchar *));
	g_hash_table_foreach(pGmlcMysqlDb->htbTables, &internal_gmlc_mysql_database_tables_name_list_append_name, arList);
	
	return arList;
}

GmlcMysqlTable * gmlc_mysql_database_get_table(GmlcMysqlDatabase * pGmlcMysqlDb, const gchar * pcName) {
	
	g_return_val_if_fail(pcName != NULL, NULL);
	
	return GMLC_MYSQL_TABLE(g_hash_table_lookup(pGmlcMysqlDb->htbTables, pcName));
}

gboolean gmlc_mysql_database_update_tables_list(GmlcMysqlDatabase * pGmlcMysqlDb) {
	GmlcMysqlTable * pGmlcMysqlTbl = NULL;
	GmlcMysqlQuery * pGmlcMysqlQry = NULL;
	GArray * arRow = NULL;
	gchar * pcSql = NULL;
	gchar * pcName = NULL, * pcType = NULL, * pcRows = NULL, * pcSize = NULL, * pcEngine = NULL;
	gulong lVersion = 0;
	
	g_object_get(G_OBJECT(pGmlcMysqlDb->pGmlcMysqlSrv), "version", &lVersion, NULL);
	
	pGmlcMysqlQry = gmlc_mysql_query_new (pGmlcMysqlDb->pGmlcMysqlSrv, pGmlcMysqlDb->pcDbName);
	
	if (lVersion < 50000) { // For version < 5.0.0
		pcSql = g_strdup_printf("SHOW TABLE STATUS;");
	} else {
		pcSql = g_strdup_printf("SELECT * FROM INFORMATION_SCHEMA.TABLES WHERE table_schema = '%s';", pGmlcMysqlDb->pcDbName);
	}
	
	if (gmlc_mysql_query_execute(pGmlcMysqlQry, pcSql, strlen(pcSql), TRUE)) {
		
		gmlc_misc_hash_table_set_all_flag(pGmlcMysqlDb->htbTables, FALSE);
		
		while ((arRow = gmlc_mysql_query_next_record(pGmlcMysqlQry)) != NULL) {
			pcName = NULL;
			
			/* Get table information */
			if (lVersion < 50000) { // For version < 5.0.0
				pcName = g_array_index(arRow, gchar *, 0);
				if (lVersion <= 40100) { // For version < 4.1.0
					pcEngine = g_array_index(arRow, gchar *, 1);
					pcRows = g_array_index(arRow, gchar *, 3);
					pcSize = g_array_index(arRow, gchar *, 5);
				} else {
					pcEngine = g_array_index(arRow, gchar *, 1);
					pcRows = g_array_index(arRow, gchar *, 4);
					pcSize = g_array_index(arRow, gchar *, 6);
				}
			} else {
				pcType = g_array_index(arRow, gchar *, 3);
				if (pcType[0] == 'B' || pcType[0] == 'T' || pcType[0] == 'S') { /* Check if type is 'BASE TABLE' or 'TEMPORARY' or 'SYSTEM VIEW' */
					pcName = g_array_index(arRow, gchar *, 2);
					pcEngine = g_array_index(arRow, gchar *, 4);
					pcRows = g_array_index(arRow, gchar *, 7);
					pcSize = g_array_index(arRow, gchar *, 9);
				}
			}
			
			/* Create table object */
			if (pcName != NULL) {
				pGmlcMysqlTbl = GMLC_MYSQL_TABLE(g_hash_table_lookup(pGmlcMysqlDb->htbTables, pcName));
				
				if (pGmlcMysqlTbl == NULL) {
					pGmlcMysqlTbl = gmlc_mysql_table_new_with_stat(pGmlcMysqlDb, pcName, pcRows, pcSize, pcEngine);
					g_hash_table_insert(pGmlcMysqlDb->htbTables, g_strdup(pcName), pGmlcMysqlTbl);
				}
				g_object_set(G_OBJECT(pGmlcMysqlTbl), "flagged", TRUE, NULL);
			}
			
			gmlc_mysql_query_free_record_content(arRow);
		}
		
		gmlc_misc_hash_table_remove_unflag_object(pGmlcMysqlDb->htbTables);
	}
	
	g_free(pcSql);
	
	g_object_unref(pGmlcMysqlQry);
	
	return TRUE;
}

GArray * gmlc_mysql_database_views_name_list(GmlcMysqlDatabase * pGmlcMysqlDb, gboolean bUpdateList) {
	GArray * arList = NULL;
	
	void internal_gmlc_mysql_database_views_name_list_append_name (gpointer key, gpointer value, gpointer user_data) {
		gchar * pcName = (gchar *) key;
		GArray * arList = (GArray *) user_data;
		UNUSED_VAR(value);
		
		g_array_append_val(arList, pcName);
	}
	
	if (bUpdateList) {
		gmlc_mysql_database_update_views_list(pGmlcMysqlDb);
	}
	
	arList = g_array_new(TRUE, TRUE, sizeof(gchar *));
	g_hash_table_foreach(pGmlcMysqlDb->htbViews, &internal_gmlc_mysql_database_views_name_list_append_name, arList);
	
	return arList;
}

GmlcMysqlView * gmlc_mysql_database_get_view(GmlcMysqlDatabase * pGmlcMysqlDb, const gchar * pcName) {
	
	g_return_val_if_fail(pcName != NULL, NULL);
	
	return GMLC_MYSQL_VIEW(g_hash_table_lookup(pGmlcMysqlDb->htbViews, pcName));
}

gboolean gmlc_mysql_database_update_views_list(GmlcMysqlDatabase * pGmlcMysqlDb) {
	GmlcMysqlView * pGmlcMysqlViw = NULL;
	GmlcMysqlQuery * pGmlcMysqlQry = NULL;
	GArray * arListName = NULL;
	GArray * arRow = NULL;
	gchar * pcSql = NULL;
	gchar * pcName = NULL;
	gulong lVersion = 0;
	gint i = 0;
	
	g_object_get(G_OBJECT(pGmlcMysqlDb->pGmlcMysqlSrv), "version", &lVersion, NULL);
	
	if (lVersion >= 50000) { // For version >= 5.0.0
		pGmlcMysqlQry = gmlc_mysql_query_new (pGmlcMysqlDb->pGmlcMysqlSrv, pGmlcMysqlDb->pcDbName);
		pcSql = g_strdup_printf("SELECT table_name FROM INFORMATION_SCHEMA.VIEWS WHERE table_schema = '%s';", pGmlcMysqlDb->pcDbName);
		
		if (gmlc_mysql_query_execute(pGmlcMysqlQry, pcSql, strlen(pcSql), TRUE)) {
			arListName = g_array_new(TRUE, TRUE, sizeof(gchar *));
			while ((arRow = gmlc_mysql_query_next_record(pGmlcMysqlQry)) != NULL) {
				pcName = g_array_index(arRow, gchar *, 0);
				g_array_append_val(arListName, pcName);
				
				g_array_free(arRow, TRUE);
			}
		}
		
		g_free(pcSql);
		g_object_unref(pGmlcMysqlQry);
	}
	
	if (arListName != NULL) {
		gmlc_misc_hash_table_set_all_flag(pGmlcMysqlDb->htbViews, FALSE);
		
		for (i = 0; i < arListName->len; i++) {
			pcName = g_array_index(arListName, gchar *, i);
			pGmlcMysqlViw = GMLC_MYSQL_VIEW(g_hash_table_lookup(pGmlcMysqlDb->htbViews, pcName));
			
			if (pGmlcMysqlViw == NULL) {
				pGmlcMysqlViw = gmlc_mysql_view_new(pGmlcMysqlDb, pcName);
				g_hash_table_insert(pGmlcMysqlDb->htbViews, g_strdup(pcName), pGmlcMysqlViw);
			}
			g_object_set(G_OBJECT(pGmlcMysqlViw), "flagged", TRUE, NULL);
			
			g_free(pcName);
		}
		
		gmlc_misc_hash_table_remove_unflag_object(pGmlcMysqlDb->htbViews);
		
		g_array_free(arListName, TRUE);
	}
	
	return TRUE;
}

GArray * gmlc_mysql_database_functions_name_list(GmlcMysqlDatabase * pGmlcMysqlDb, gboolean bUpdateList) {
	GArray * arList = NULL;
	
	void internal_gmlc_mysql_database_functions_name_list_append_name (gpointer key, gpointer value, gpointer user_data) {
		gchar * pcName = (gchar *) key;
		GArray * arList = (GArray *) user_data;
		UNUSED_VAR(value);
		
		g_array_append_val(arList, pcName);
	}
	
	if (bUpdateList) {
		gmlc_mysql_database_update_functions_list(pGmlcMysqlDb);
	}
	
	arList = g_array_new(TRUE, TRUE, sizeof(gchar *));
	g_hash_table_foreach(pGmlcMysqlDb->htbFunctions, &internal_gmlc_mysql_database_functions_name_list_append_name, arList);
	
	return arList;
}

GmlcMysqlFunction * gmlc_mysql_database_get_function(GmlcMysqlDatabase * pGmlcMysqlDb, const gchar * pcName) {
	
	g_return_val_if_fail(pcName != NULL, NULL);
	
	return GMLC_MYSQL_FUNCTION(g_hash_table_lookup(pGmlcMysqlDb->htbFunctions, pcName));
}

gboolean gmlc_mysql_database_update_functions_list(GmlcMysqlDatabase * pGmlcMysqlDb) {
	GmlcMysqlFunction * pGmlcMysqlFnct = NULL;
	GmlcMysqlQuery * pGmlcMysqlQry = NULL;
	GArray * arListName = NULL;
	GArray * arRow = NULL;
	gchar * pcSql = NULL;
	gchar * pcName = NULL;
	gchar * pcType = NULL;
	gulong lVersion = 0;
	gint i = 0;
	
	g_object_get(G_OBJECT(pGmlcMysqlDb->pGmlcMysqlSrv), "version", &lVersion, NULL);
	
	if (lVersion >= 50000) { // For version >= 5.0.0
		pGmlcMysqlQry = gmlc_mysql_query_new (pGmlcMysqlDb->pGmlcMysqlSrv, pGmlcMysqlDb->pcDbName);
		pcSql = g_strdup_printf("SELECT routine_name, routine_type FROM INFORMATION_SCHEMA.ROUTINES WHERE routine_schema = '%s';", pGmlcMysqlDb->pcDbName);
		
		if (gmlc_mysql_query_execute(pGmlcMysqlQry, pcSql, strlen(pcSql), TRUE)) {
			arListName = g_array_new(TRUE, TRUE, sizeof(gchar *));
			while ((arRow = gmlc_mysql_query_next_record(pGmlcMysqlQry)) != NULL) {
				pcName = g_array_index(arRow, gchar *, 0);
				pcType = g_array_index(arRow, gchar *, 1);
				
				if (pcType[0] == 'F') { /* Check if type is 'FUNCTION' */
					g_array_append_val(arListName, pcName);
				} else {
					g_free(pcName);
				}
				
				g_free(pcType);
				g_array_free(arRow, TRUE);
			}
		}
		
		g_free(pcSql);
		g_object_unref(pGmlcMysqlQry);
	}
	
	if (arListName != NULL) {
		gmlc_misc_hash_table_set_all_flag(pGmlcMysqlDb->htbFunctions, FALSE);
		
		for (i = 0; i < arListName->len; i++) {
			pcName = g_array_index(arListName, gchar *, i);
			pGmlcMysqlFnct = GMLC_MYSQL_FUNCTION(g_hash_table_lookup(pGmlcMysqlDb->htbFunctions, pcName));
			
			if (pGmlcMysqlFnct == NULL) {
				pGmlcMysqlFnct = gmlc_mysql_function_new(pGmlcMysqlDb, pcName);
				g_hash_table_insert(pGmlcMysqlDb->htbFunctions, g_strdup(pcName), pGmlcMysqlFnct);
			}
			g_object_set(G_OBJECT(pGmlcMysqlFnct), "flagged", TRUE, NULL);
			
			g_free(pcName);
		}
		
		gmlc_misc_hash_table_remove_unflag_object(pGmlcMysqlDb->htbFunctions);
		
		g_array_free(arListName, TRUE);
	}
	
	return TRUE;
}

GArray * gmlc_mysql_database_procedures_name_list(GmlcMysqlDatabase * pGmlcMysqlDb, gboolean bUpdateList) {
	GArray * arList = NULL;
	
	void internal_gmlc_mysql_database_procedures_name_list_append_name (gpointer key, gpointer value, gpointer user_data) {
		gchar * pcName = (gchar *) key;
		GArray * arList = (GArray *) user_data;
		UNUSED_VAR(value);
		
		g_array_append_val(arList, pcName);
	}
	
	if (bUpdateList) {
		gmlc_mysql_database_update_procedures_list(pGmlcMysqlDb);
	}
	
	arList = g_array_new(TRUE, TRUE, sizeof(gchar *));
	g_hash_table_foreach(pGmlcMysqlDb->htbProcedures, &internal_gmlc_mysql_database_procedures_name_list_append_name, arList);
	
	return arList;
}

GmlcMysqlProcedure * gmlc_mysql_database_get_procedure(GmlcMysqlDatabase * pGmlcMysqlDb, const gchar * pcName) {
	
	g_return_val_if_fail(pcName != NULL, NULL);
	
	return GMLC_MYSQL_PROCEDURE(g_hash_table_lookup(pGmlcMysqlDb->htbProcedures, pcName));
}

gboolean gmlc_mysql_database_update_procedures_list(GmlcMysqlDatabase * pGmlcMysqlDb) {
	GmlcMysqlProcedure * pGmlcMysqlPrc = NULL;
	GmlcMysqlQuery * pGmlcMysqlQry = NULL;
	GArray * arListName = NULL;
	GArray * arRow = NULL;
	gchar * pcSql = NULL;
	gchar * pcName = NULL;
	gchar * pcType = NULL;
	gulong lVersion = 0;
	gint i = 0;
	
	g_object_get(G_OBJECT(pGmlcMysqlDb->pGmlcMysqlSrv), "version", &lVersion, NULL);
	
	if (lVersion >= 50000) { // For version >= 5.0.0
		pGmlcMysqlQry = gmlc_mysql_query_new (pGmlcMysqlDb->pGmlcMysqlSrv, pGmlcMysqlDb->pcDbName);
		pcSql = g_strdup_printf("SELECT routine_name, routine_type FROM INFORMATION_SCHEMA.ROUTINES WHERE routine_schema = '%s';", pGmlcMysqlDb->pcDbName);
		
		if (gmlc_mysql_query_execute(pGmlcMysqlQry, pcSql, strlen(pcSql), TRUE)) {
			arListName = g_array_new(TRUE, TRUE, sizeof(gchar *));
			while ((arRow = gmlc_mysql_query_next_record(pGmlcMysqlQry)) != NULL) {
				pcName = g_array_index(arRow, gchar *, 0);
				pcType = g_array_index(arRow, gchar *, 1);
				
				if (pcType[0] == 'P') { /* Check if type is 'PROCEDURE' */
					g_array_append_val(arListName, pcName);
				} else {
					g_free(pcName);
				}
				
				g_free(pcType);
				g_array_free(arRow, TRUE);
			}
		}
		
		g_free(pcSql);
		g_object_unref(pGmlcMysqlQry);
	}
	
	if (arListName != NULL) {
		gmlc_misc_hash_table_set_all_flag(pGmlcMysqlDb->htbProcedures, FALSE);
		
		for (i = 0; i < arListName->len; i++) {
			pcName = g_array_index(arListName, gchar *, i);
			pGmlcMysqlPrc = GMLC_MYSQL_PROCEDURE(g_hash_table_lookup(pGmlcMysqlDb->htbProcedures, pcName));
			
			if (pGmlcMysqlPrc == NULL) {
				pGmlcMysqlPrc = gmlc_mysql_procedure_new(pGmlcMysqlDb, pcName);
				g_hash_table_insert(pGmlcMysqlDb->htbProcedures, g_strdup(pcName), pGmlcMysqlPrc);
			}
			g_object_set(G_OBJECT(pGmlcMysqlPrc), "flagged", TRUE, NULL);
			
			g_free(pcName);
		}
		
		gmlc_misc_hash_table_remove_unflag_object(pGmlcMysqlDb->htbProcedures);
		
		g_array_free(arListName, TRUE);
	}
	
	return TRUE;
}


