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

#ifndef __GMLC_GUI_SERVER_TAB_EDIT_H__
#define __GMLC_GUI_SERVER_TAB_EDIT_H__ 

#include "gmlc_main.h"
#include "gmlc_gui_server.h"
#include "gmlc_mysql_server.h"

G_BEGIN_DECLS

#define GMLC_GUI_TYPE_SERVER_TAB_EDIT				(gmlc_gui_server_tab_edit_get_type ())
#define GMLC_GUI_SERVER_TAB_EDIT(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj), GMLC_GUI_TYPE_SERVER_TAB_EDIT, GmlcGuiServerTabEdit))
#define GMLC_GUI_SERVER_TAB_EDIT_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass), GMLC_GUI_TYPE_SERVER_TAB_EDIT, GmlcGuiServerTabEditClass))
#define GMLC_IS_GUI_SERVER_TAB_EDIT(obj)			(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMLC_GUI_TYPE_SERVER_TAB_EDIT))
#define GMLC_IS_GUI_SERVER_TAB_EDIT_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GMLC_GUI_TYPE_SERVER_TAB_EDIT))
#define GMLC_GET_GUI_SERVER_TAB_EDIT_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), GMLC_GUI_TYPE_SERVER_TAB_EDIT, GmlcGuiServerTabEditClass))

typedef struct _GmlcGuiServerTabEdit GmlcGuiServerTabEdit;
typedef struct _GmlcGuiServerTabEditClass GmlcGuiServerTabEditClass;

struct _GmlcGuiServerTabEdit {
	GtkVBox parent;
	
	/* private members */
	GmlcMysqlServer * pGmlcMysqlSrv;
	GmlcGuiServer * pGmlcGuiSrv;
	gchar * pcDbName;
	gchar * pcFileName;
	
	/* Toolbar Part */
	GtkWidget * poQueryToolbar;

	/* Query Part */
	GtkWidget * txtSQLRequest;
	GtkWidget * tabSQLResult;
	GtkWidget * statusbarSQL;
	
	GmlcMysqlQuery * pGmlcMysqlQry;
	guint iNumResult;
};

struct _GmlcGuiServerTabEditClass {
	GtkVBoxClass parent_class;
	
	/* define vtable methods and signals here */
};

GType gmlc_gui_server_tab_edit_get_type (void);

GmlcGuiServerTabEdit * gmlc_gui_server_tab_edit_new (GmlcGuiServer * pGmlcGuiSrv, const gchar * pcDefDbName);

gboolean gmlc_gui_server_tab_edit_set_query (GmlcGuiServerTabEdit * pGmlcGuiSrvTabQuery, const gchar * pcQuery, gboolean bExecNow);
gchar * gmlc_gui_server_tab_edit_get_query (GmlcGuiServerTabEdit * pGmlcGuiSrvTabQuery);

void gmlc_gui_server_tab_edit_evt_btn_load_clicked(GtkWidget *widget, gpointer user_data);

G_END_DECLS

#endif /* __GMLC_GUI_SERVER_TAB_EDIT_H__ */
