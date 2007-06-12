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

#ifndef __GMLC_GUI_SERVER_H__
#define __GMLC_GUI_SERVER_H__

#include "gmlc_main.h"

#include "gmlc_mysql_server.h"
#include "gmlc_mysql_database.h"
#include "gmlc_mysql_table.h"
#include "gmlc_mysql_view.h"

G_BEGIN_DECLS

#define GMLC_GUI_TYPE_SERVER			(gmlc_gui_server_get_type ())
#define GMLC_GUI_SERVER(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj), GMLC_GUI_TYPE_SERVER, GmlcGuiServer))
#define GMLC_GUI_SERVER_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), GMLC_GUI_TYPE_SERVER, GmlcGuiServerClass))
#define GMLC_IS_GUI_SERVER(obj)			(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMLC_GUI_TYPE_SERVER))
#define GMLC_IS_GUI_SERVER_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GMLC_GUI_TYPE_SERVER))
#define GMLC_GET_GUI_SERVER_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), GMLC_GUI_TYPE_SERVER, GmlcGuiServerClass))

typedef struct _GmlcGuiServer GmlcGuiServer;
typedef struct _GmlcGuiServerClass GmlcGuiServerClass;

struct _GmlcGuiServer {
	GtkWindow parent;
	
	/* private members */
	GmlcMysqlServer * pGmlcMysqlSrv;
	
	/* General Widgets */
	GtkWidget * nbkGeneral;
	GtkWidget * tlbMainToolbar;
	GtkWidget * poHBoxToolbar;
};

struct _GmlcGuiServerClass {
	GtkWindowClass parent_class;
	
	/* define vtable methods and signals here */
};

GType gmlc_gui_server_get_type (void);

GmlcGuiServer * gmlc_gui_server_new (GmlcMysqlServer * pGmlcMysqlSrv);

void gmlc_gui_server_add_query_tab(GmlcGuiServer * pGmlcGuiSrv, const gchar * pcDbName, const gchar * pcQuery, gboolean bExecNow);

G_END_DECLS

#endif /* __GMLC_GUI_SERVER_H__ */

