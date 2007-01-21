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

#ifndef __GMLC_GUI_QUERY_H__
#define __GMLC_GUI_QUERY_H__

#include "gmlc_main.h"
#include "gmlc_mysql_server.h"
#include "gmlc_mysql_query.h"

G_BEGIN_DECLS

#define GMLC_GUI_TYPE_QUERY				(gmlc_gui_query_get_type ())
#define GMLC_GUI_QUERY(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj), GMLC_GUI_TYPE_QUERY, GmlcGuiQuery))
#define GMLC_GUI_QUERY_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass), GMLC_GUI_TYPE_QUERY, GmlcGuiQueryClass))
#define GMLC_IS_GUI_QUERY(obj)			(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMLC_GUI_TYPE_QUERY))
#define GMLC_IS_GUI_QUERY_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GMLC_GUI_TYPE_QUERY))
#define GMLC_GET_GUI_QUERY_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), GMLC_GUI_TYPE_QUERY, GmlcGuiQueryClass))

typedef struct _GmlcGuiQuery GmlcGuiQuery;
typedef struct _GmlcGuiQueryClass GmlcGuiQueryClass;

struct _GmlcGuiQuery {
	GtkWindow parent;
	
	/* private members */
	GtkWidget * vbxSQLQueries;
	GtkWidget * lstSQLQueries;
	GtkWidget * txtSQLRequest;
	GtkWidget * tabSQLResult;
	GtkWidget * lstSQLResult;
	GtkWidget * cmbCharset;
	GtkWidget * statusbarSQL;
	
/*
	p_mysql_database mysql_db;
*/
	GmlcMysqlServer * pGmlcMysqlSrv;
	gchar * pcDefDbName;
	GmlcMysqlQuery * pGmlcMysqlQry;
	guint iNumResult;
	
/*	gint currCharset;*/
/*	GSList * lstRows;*/
};

struct _GmlcGuiQueryClass {
	GtkWindowClass parent_class;
	
	/* define vtable methods and signals here */
};

GType gmlc_gui_query_get_type (void);

GmlcGuiQuery * gmlc_gui_query_new (GmlcMysqlServer * pGmlcMysqlSrv, const gchar * pcDbName);

gboolean gmlc_gui_query_set_query (GmlcGuiQuery * pGmlcGuiQry, const gchar * pcQuery, gboolean bExecNow);
gchar * gmlc_gui_query_get_query (GmlcGuiQuery * pGmlcGuiQry);

G_END_DECLS

#endif /* __GMLC_GUI_QUERY_H__ */

