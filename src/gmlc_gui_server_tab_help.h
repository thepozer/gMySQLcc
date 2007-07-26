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

#ifndef __GMLC_GUI_SERVER_TAB_HELP_H__
#define __GMLC_GUI_SERVER_TAB_HELP_H__ 

#include "gmlc_main.h"

#include "gmlc_gui_server.h"

#include "gmlc_mysql_server.h"

G_BEGIN_DECLS

#define GMLC_GUI_TYPE_SERVER_TAB_HELP				(gmlc_gui_server_tab_help_get_type ())
#define GMLC_GUI_SERVER_TAB_HELP(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj), GMLC_GUI_TYPE_SERVER_TAB_HELP, GmlcGuiServerTabHelp))
#define GMLC_GUI_SERVER_TAB_HELP_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass), GMLC_GUI_TYPE_SERVER_TAB_HELP, GmlcGuiServerTabHelpClass))
#define GMLC_IS_GUI_SERVER_TAB_HELP(obj)			(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMLC_GUI_TYPE_SERVER_TAB_HELP))
#define GMLC_IS_GUI_SERVER_TAB_HELP_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GMLC_GUI_TYPE_SERVER_TAB_HELP))
#define GMLC_GET_GUI_SERVER_TAB_HELP_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS ((obj), GMLC_GUI_TYPE_SERVER_TAB_HELP, GmlcGuiServerTabHelpClass))

typedef struct _GmlcGuiServerTabHelp GmlcGuiServerTabHelp;
typedef struct _GmlcGuiServerTabHelpClass GmlcGuiServerTabHelpClass;

struct _GmlcGuiServerTabHelp {
	GtkVBox parent;
	
	/* private members */
	GmlcMysqlServer * pGmlcMysqlSrv;
	GmlcGuiServer * pGmlcGuiSrv;
	
	/* Help Part */
	GtkWidget *	trvHelpCategories;
	GtkWidget *	txtHelpSearchKeyword;
	GtkWidget *	trvHelpTopics;
	GtkWidget *	txvHelpTopic;
	
	gint iCurrentCategory;
	gint iCurrentTopic;
};

struct _GmlcGuiServerTabHelpClass {
	GtkVBoxClass parent_class;
	
	/* define vtable methods and signals here */
};

GType gmlc_gui_server_tab_help_get_type (void);

GmlcGuiServerTabHelp * gmlc_gui_server_tab_help_new (GmlcGuiServer * pGmlcGuiSrv);

G_END_DECLS

#endif /* __GMLC_GUI_SERVER_TAB_HELP_H__ */
