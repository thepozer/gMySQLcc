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

#ifndef __GMLC_GUI_SERVER_TAB_DATA_H__
#define __GMLC_GUI_SERVER_TAB_DATA_H__ 

#include "gmlc_main.h"

#include "gmlc_gui_server.h"

#include "gmlc_mysql_server.h"
#include "gmlc_mysql_database.h"
#include "gmlc_mysql_table.h"
#include "gmlc_mysql_view.h"

G_BEGIN_DECLS

#define GMLC_GUI_TYPE_SERVER_TAB_DATA				(gmlc_gui_server_tab_data_get_type ())
#define GMLC_GUI_SERVER_TAB_DATA(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj), GMLC_GUI_TYPE_SERVER_TAB_DATA, GmlcGuiServerTabData))
#define GMLC_GUI_SERVER_TAB_DATA_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass), GMLC_GUI_TYPE_SERVER_TAB_DATA, GmlcGuiServerTabDataClass))
#define GMLC_IS_GUI_SERVER_TAB_DATA(obj)			(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMLC_GUI_TYPE_SERVER_TAB_DATA))
#define GMLC_IS_GUI_SERVER_TAB_DATA_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GMLC_GUI_TYPE_SERVER_TAB_DATA))
#define GMLC_GET_GUI_SERVER_TAB_DATA_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS ((obj), GMLC_GUI_TYPE_SERVER_TAB_DATA, GmlcGuiServerTabDataClass))

typedef struct _GmlcGuiServerTabData GmlcGuiServerTabData;
typedef struct _GmlcGuiServerTabDataClass GmlcGuiServerTabDataClass;

struct _GmlcGuiServerTabData {
	GtkVBox parent;
	
	/* private members */
	GmlcMysqlServer * pGmlcMysqlSrv;
	GmlcGuiServer * pGmlcGuiSrv;
	
	/* Global toolbar */
	GtkToolItem * btnTlbrSql;
	GtkToolItem * btnTlbrSqlFile;
	
	/* Table/View/Proc/Func Part */
	GtkWidget * lstBase;
	GtkWidget * lstTables;
	GtkWidget * lstViews;
	GtkWidget * lstProcedures;
	GtkWidget * lstFunctions;
	GtkWidget * mnuBdOps;
	GtkWidget * mnuTblOps;
	
	GmlcMysqlDatabase	* pGmlcMysqlDb;
	GmlcMysqlTable		* pGmlcMysqlTbl;
	GmlcMysqlView		* pGmlcMysqlVw;
	GmlcMysqlProcedure	* pGmlcMysqlProc;
	GmlcMysqlFunction	* pGmlcMysqlFunc;
};

struct _GmlcGuiServerTabDataClass {
	GtkVBoxClass parent_class;
	
	/* define vtable methods and signals here */
};

GType gmlc_gui_server_tab_data_get_type (void);

GmlcGuiServerTabData * gmlc_gui_server_tab_data_new (GmlcGuiServer * pGmlcGuiSrv);

G_END_DECLS

#endif /* __GMLC_GUI_SERVER_TAB_DATA_H__ */
