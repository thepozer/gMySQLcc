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

#ifndef __GMLC_GUI_CONNEXIONS_H__
#define __GMLC_GUI_CONNEXIONS_H__

#include "gmlc_main.h"

#include "gmlc_mysql_server.h"

G_BEGIN_DECLS

#define GMLC_GUI_TYPE_CONNEXIONS			(gmlc_gui_connexions_get_type ())
#define GMLC_GUI_CONNEXIONS(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj), GMLC_GUI_TYPE_CONNEXIONS, GmlcGuiConnexions))
#define GMLC_GUI_CONNEXIONS_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), GMLC_GUI_TYPE_CONNEXIONS, GmlcGuiConnexionsClass))
#define GMLC_IS_GUI_CONNEXIONS(obj)			(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMLC_GUI_TYPE_CONNEXIONS))
#define GMLC_IS_GUI_CONNEXIONS_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GMLC_GUI_TYPE_CONNEXIONS))
#define GMLC_GET_GUI_CONNEXIONS_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), GMLC_GUI_TYPE_CONNEXIONS, GmlcGuiConnexionsClass))

typedef struct _GmlcGuiConnexions GmlcGuiConnexions;
typedef struct _GmlcGuiConnexionsClass GmlcGuiConnexionsClass;

struct _GmlcGuiConnexions {
	GtkWindow parent;
	
	/* private members */
	GtkWidget * txtName;
	GtkWidget * txtHost;
	GtkWidget * txtPort;
	GtkWidget * txtLogin;
	GtkWidget * txtPasswd;
	GtkWidget * txtAllowedDb;
	GtkWidget * txtLocalSock;
	GtkWidget * chkReadOnly;
	GtkWidget * chkWriteWarning;
	
	GtkWidget * lstListHosts;
	GtkWidget * vbxEditPart;
	
	gboolean bShowEditPart;
	GmlcMysqlServer * pCurrMysqlSrv;
};

struct _GmlcGuiConnexionsClass {
	GtkWindowClass parent_class;
	
	/* define vtable methods and signals here */
};

GType gmlc_gui_connexions_get_type (void);

GmlcGuiConnexions * gmlc_gui_connexions_new ();

G_END_DECLS

#endif /* __GMLC_GUI_CONNEXIONS_H__ */

