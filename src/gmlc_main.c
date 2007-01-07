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

#define __GMYSQLCC_MAIN_PART__

#include "gmlc_main.h"
#include "gmlc_gui_connexions.h"

int GiNbrWnd = 0;

GmlcMiscConfig * GpGmlcMscCfg;

/*p_gmysqlcc_gui_list_server gmysqlcc_gui_list_server;*/

#ifdef USE_GTKSOURCEVIEW
	GtkSourceLanguagesManager * LangManager;
#endif /* USE_GTKSOURCEVIEW */

gboolean gmysqlcc_main_before_end(gpointer data);

int main(int argc, char *argv[]) {
	GmlcGuiConnexions * pGmlcGuiCnxns = NULL;
	
	/* Init gtk library */
	gtk_init (&argc, &argv);
	
	/* Init gettext library */
	bindtextdomain (PACKAGE, PACKAGE_LOCALE_DIR);
	textdomain (PACKAGE);
	bind_textdomain_codeset(PACKAGE, "UTF-8");
	
	/* Init types list */
	/*gmysqlcc_helpers_field_capability_init_array();*/
	
	/* Read configuration */
	GpGmlcMscCfg = gmlc_misc_config_new();
	
	if (!gmlc_misc_config_read(GpGmlcMscCfg)) {
		g_printerr("Can't start gmysqlcc - Error during configuration ... \n");
		return 1;
	}
	
	/* Add trigger to save configuration before the end of gmysqlcc */
	gtk_quit_add(0, gmysqlcc_main_before_end, NULL);
	
#ifdef USE_GTKSOURCEVIEW
	/* Get Language Manager to select sql type */
	LangManager = gtk_source_languages_manager_new();
#endif /* USE_GTKSOURCEVIEW */
	
	/* Create server list windows */
	pGmlcGuiCnxns = gmlc_gui_connexions_new();
	gtk_widget_show(GTK_WIDGET(pGmlcGuiCnxns));
	
	/* Start the application */
	gtk_main ();
	
	return 0;
}

gboolean gmysqlcc_main_before_end(gpointer data) {
	UNUSED_VAR(data);

	gmlc_misc_config_write(GpGmlcMscCfg);
	
	return TRUE;
}
