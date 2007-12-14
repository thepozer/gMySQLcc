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

#include <gtk/gtk.h>

#include <libintl.h>
#include <stdlib.h>
#include <string.h>

#define __GMYSQLCC_MAIN_PART__

#include "../config.h"

#include "gmlc_mysql_server.h"
#include "gmlc_mysql_database.h"
#include "gmlc_mysql_table.h"
#include "gmlc_mysql_query.h"

int main(int argc, char *argv[]) {
	GmlcMysqlServer * pGmlcMysqlSrv = NULL;
	GmlcMysqlDatabase * pGmlcMysqlDb = NULL;
	GmlcMysqlQuery * pGmlcMysqlQry = NULL;
	GArray * arRow = NULL;
	/*gchar * pcSql = "SELECT * FROM mysql.user ; INSERT INTO pozer.testme SET a = 2 ; SELECT * FROM mysql.help_category;";*/
	gchar * pcSql = "INSERT INTO pozer.testme SET a = 2 ; SELECT * FROM mysql.user ; SELECT * FROM mysql.help_category ; SHOW DATABASES ;";
	glong lVersion = 0;
	gint i = 0;
	gboolean bContinue = FALSE;
	
	/* Init gtk library */
	gtk_init (&argc, &argv);
	
	/* Init gettext library */
	bindtextdomain (PACKAGE, PACKAGE_LOCALE_DIR);
	textdomain (PACKAGE);
	bind_textdomain_codeset(PACKAGE, "UTF-8");
	
	/* Tests des objets */
	pGmlcMysqlSrv = gmlc_mysql_server_new();
	g_object_set(G_OBJECT(pGmlcMysqlSrv), "name", "Test server", "hostname", "127.0.0.1", "port", 3306, 
										  "login", "root", "password", "", NULL);
	g_print("Ajout du serveur de test\n");
	
	pGmlcMysqlQry = gmlc_mysql_query_new(G_OBJECT(pGmlcMysqlSrv), NULL);
	
	g_print("Liste des bases de données 1 : \n");
	arRow = gmlc_mysql_server_databases_name_list(pGmlcMysqlSrv, TRUE);
	for (i = 0; i < arRow->len; i ++) {
		g_print(" # '%s'\n", g_array_index(arRow, gchar *, i));
	}
	g_array_free(arRow, TRUE);
	
	g_object_get(G_OBJECT(pGmlcMysqlSrv), "version", &lVersion, NULL);
	g_print("Version du serveur de test : %ld\n", lVersion);
	
	if (gmlc_mysql_query_execute(pGmlcMysqlQry, "CREATE DATABASE `debug_me-now`", 30, FALSE)) {
		g_print("Database : `debug_me-now` - Creation OK.\n");
		bContinue = TRUE;
	} else {
		g_print("Database : `debug_me-now` - Creation ERREUR !!!\n");
	}
	
	g_print("Liste des bases de données 2 : \n");
	arRow = gmlc_mysql_server_databases_name_list(pGmlcMysqlSrv, TRUE);
	for (i = 0; i < arRow->len; i ++) {
		g_print(" # '%s'\n", g_array_index(arRow, gchar *, i));
	}
	g_array_free(arRow, TRUE);

	g_print("Charset utilisé par le serveur : %s\n", pGmlcMysqlQry->pcSrvCharset);
	
	if (gmlc_mysql_query_execute(pGmlcMysqlQry, "DROP DATABASE `debug_me-now`", 28, FALSE)) {
		g_print("Database : `debug_me-now` - Destruction OK.\n");
		bContinue = TRUE;
	} else {
		g_print("Database : `debug_me-now` - Destruction ERREUR !!!\n");
	}
	
	g_print("Liste des bases de données 3 : \n");
	arRow = gmlc_mysql_server_databases_name_list(pGmlcMysqlSrv, TRUE);
	for (i = 0; i < arRow->len; i ++) {
		g_print(" # '%s'\n", g_array_index(arRow, gchar *, i));
	}
	g_array_free(arRow, TRUE);

	
	g_print("Liste des tables de la base de donnée pozer : \n");
	pGmlcMysqlDb = gmlc_mysql_server_get_database(pGmlcMysqlSrv, "pozer");
	arRow = gmlc_mysql_database_tables_name_list(pGmlcMysqlDb, TRUE);
	for (i = 0; i < arRow->len; i ++) {
		g_print(" - '%s'\n", g_array_index(arRow, gchar *, i));
	}
	g_array_free(arRow, TRUE);
	
	g_print("Liste des vues de la base de donnée pozer : \n");
	pGmlcMysqlDb = gmlc_mysql_server_get_database(pGmlcMysqlSrv, "pozer");
	arRow = gmlc_mysql_database_views_name_list(pGmlcMysqlDb, TRUE);
	for (i = 0; i < arRow->len; i ++) {
		g_print(" - '%s'\n", g_array_index(arRow, gchar *, i));
	}
	g_array_free(arRow, TRUE);
	
	g_print("Liste des tables de la base de donnée mysql : \n");
	pGmlcMysqlDb = gmlc_mysql_server_get_database(pGmlcMysqlSrv, "mysql");
	arRow = gmlc_mysql_database_tables_name_list(pGmlcMysqlDb, TRUE);
	for (i = 0; i < arRow->len; i ++) {
		g_print(" - '%s'\n", g_array_index(arRow, gchar *, i));
	}
	g_array_free(arRow, TRUE);

	g_print("Liste des vues de la base de donnée mysql : \n");
	pGmlcMysqlDb = gmlc_mysql_server_get_database(pGmlcMysqlSrv, "mysql");
	arRow = gmlc_mysql_database_views_name_list(pGmlcMysqlDb, TRUE);
	for (i = 0; i < arRow->len; i ++) {
		g_print(" - '%s'\n", g_array_index(arRow, gchar *, i));
	}
	g_array_free(arRow, TRUE);
	
	g_print("Liste des fonctions de la base de donnée mysql : \n");
	pGmlcMysqlDb = gmlc_mysql_server_get_database(pGmlcMysqlSrv, "mysql");
	arRow = gmlc_mysql_database_functions_name_list(pGmlcMysqlDb, TRUE);
	for (i = 0; i < arRow->len; i ++) {
		g_print(" - '%s'\n", g_array_index(arRow, gchar *, i));
	}
	g_array_free(arRow, TRUE);
	
	g_print("Liste des procedures de la base de donnée mysql : \n");
	pGmlcMysqlDb = gmlc_mysql_server_get_database(pGmlcMysqlSrv, "mysql");
	arRow = gmlc_mysql_database_procedures_name_list(pGmlcMysqlDb, TRUE);
	for (i = 0; i < arRow->len; i ++) {
		g_print(" - '%s'\n", g_array_index(arRow, gchar *, i));
	}
	g_array_free(arRow, TRUE);
	
	
/*
	g_print("Exécution de la query : %s\n", pcSql);
	
	if (gmlc_mysql_query_execute(pGmlcMysqlQry, pcSql, strlen(pcSql), FALSE)) {
		g_print("Execution de la query OK.\n");
		bContinue = TRUE;
	} else {
		g_print("Erreur durant l'exécution de la query !!!\n");
	}
	
	if (bContinue) {
		do {
			g_print ("*** Read data of new query \n");
			while ((arRow = gmlc_mysql_query_next_record(pGmlcMysqlQry)) != NULL) {
				for (i = 0; i < arRow->len; i ++) {
					g_print("'%s';", g_array_index(arRow, gchar *, i));
					g_free(g_array_index(arRow, gchar *, i));
				}
				
				g_array_free(arRow, TRUE);
				
				g_print("\n");
			}
			
			if (pGmlcMysqlQry->iEditResult != 0) {
				g_print ("Nombre de ligne modifiées : %d.\n", pGmlcMysqlQry->iEditResult);
			}
			
		} while (gmlc_mysql_query_goto_next_result(pGmlcMysqlQry));
	}
*/
	
	g_object_unref(G_OBJECT(pGmlcMysqlQry));
	g_object_unref(G_OBJECT(pGmlcMysqlSrv));
	
	return 0;
	
	/* Start the application */
	gtk_main ();
	
	return 0;
}

