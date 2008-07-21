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
#include "gmlc_dump_control.h"
#include "gmlc_dump_source_table.h"

int main(int argc, char *argv[]) {
	GmlcMysqlServer * pGmlcMysqlSrv = NULL;
	/*GmlcMysqlDatabase * pGmlcMysqlDb = NULL;*/
	GmlcMysqlQuery * pGmlcMysqlQry = NULL;
	GArray * arRow = NULL;
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
										  "login", "mysql", "password", "mysql", NULL);
	g_print("Ajout du serveur de test\n");
	
	pGmlcMysqlQry = gmlc_mysql_query_new(pGmlcMysqlSrv, NULL);
	
	g_print("Liste des bases de données 1 : \n");
	arRow = gmlc_mysql_server_databases_name_list(pGmlcMysqlSrv, TRUE);
	for (i = 0; i < arRow->len; i ++) {
		g_print(" # '%s'\n", g_array_index(arRow, gchar *, i));
	}
	g_array_free(arRow, TRUE);
	
	g_object_get(G_OBJECT(pGmlcMysqlSrv), "version", &lVersion, NULL);
	g_print("Version du serveur de test : %ld\n", lVersion);
	
	
	
	
	
	g_object_unref(G_OBJECT(pGmlcMysqlQry));
	g_object_unref(G_OBJECT(pGmlcMysqlSrv));
	
	return 0;
}
