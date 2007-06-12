/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License.
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

#ifndef __GMLC_GUI_SERVER_TAB_QUERY_H__
#define __GMLC_GUI_SERVER_TAB_QUERY_H__ 

#include "gmlc_main.h"

#include "gmlc_gui_server.h"

#include "gmlc_mysql_server.h"

G_BEGIN_DECLS

#define GMLC_GUI_TYPE_SERVER_TAB_QUERY				(gmlc_gui_server_tab_query_get_type ())
#define GMLC_GUI_SERVER_TAB_QUERY(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj), GMLC_GUI_TYPE_SERVER_TAB_QUERY, GmlcGuiServerTabQuery))
#define GMLC_GUI_SERVER_TAB_QUERY_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass), GMLC_GUI_TYPE_SERVER_TAB_QUERY, GmlcGuiServerTabQueryClass))
#define GMLC_IS_GUI_SERVER_TAB_QUERY(obj)			(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMLC_GUI_TYPE_SERVER_TAB_QUERY))
#define GMLC_IS_GUI_SERVER_TAB_QUERY_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GMLC_GUI_TYPE_SERVER_TAB_QUERY))
#define GMLC_GET_GUI_SERVER_TAB_QUERY_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), GMLC_GUI_TYPE_SERVER_TAB_QUERY, GmlcGuiServerTabQueryClass))

typedef struct _GmlcGuiServerTabQuery GmlcGuiServerTabQuery;
typedef struct _GmlcGuiServerTabQueryClass GmlcGuiServerTabQueryClass;

struct _GmlcGuiServerTabQuery {
	GtkVBox parent;
	
	/* private members */
	GmlcMysqlServer * pGmlcMysqlSrv;
	GmlcGuiServer * pGmlcGuiSrv;
	gchar * pcDbName;
	
	/* Toolbar Part */
	GtkWidget * poQueryToolbar;
	GtkToolItem *btnExecSql;
	GtkToolItem *btnDumpSql;
	GtkToolItem *btnDuplicateSql;

	/* Query Part */
	GtkWidget * txtSQLRequest;
	GtkWidget * tabSQLResult;
	GtkWidget * lstSQLResult;
	GtkWidget * statusbarSQL;
	
/*
	p_mysql_database mysql_db;
*/
	GmlcMysqlQuery * pGmlcMysqlQry;
	guint iNumResult;
	
/*	gint currCharset;*/
/*	GSList * lstRows;*/
};

struct _GmlcGuiServerTabQueryClass {
	GtkVBoxClass parent_class;
	
	/* define vtable methods and signals here */
};

GType gmlc_gui_server_tab_query_get_type (void);

GmlcGuiServerTabQuery * gmlc_gui_server_tab_query_new (GmlcGuiServer * pGmlcGuiSrv, const gchar * pcDefDbName);

gboolean gmlc_gui_server_tab_query_set_query (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery, const gchar * pcQuery, gboolean bExecNow);
gchar * gmlc_gui_server_tab_query_get_query (GmlcGuiServerTabQuery * pGmlcGuiSrvTabQuery);

G_END_DECLS

#endif /* __GMLC_GUI_SERVER_TAB_QUERY_H__ */
