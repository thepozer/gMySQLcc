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

#ifndef __GMLC_GUI_SERVER_TAB_H__
#define __GMLC_GUI_SERVER_TAB_H__ 

#include "gmlc_main.h"
#include <glib-object.h>

G_BEGIN_DECLS

#define GMLC_GUI_TYPE_SERVER_TAB				(gmlc_gui_server_tab_get_type ())
#define GMLC_GUI_SERVER_TAB(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj), GMLC_GUI_TYPE_SERVER_TAB, GmlcGuiServerTab))
#define GMLC_IS_GUI_SERVER_TAB(obj)				(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMLC_GUI_TYPE_SERVER_TAB))
#define GMLC_GUI_SERVER_TAB_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), GMLC_GUI_TYPE_SERVER_TAB, GmlcGuiServerTabInterface))

typedef struct _GmlcGuiServerTab GmlcGuiServerTab;
typedef struct _GmlcGuiServerTabInterface GmlcGuiServerTabInterface;

struct _GmlcGuiServerTabInterface {
	GTypeInterface parent;
	
	void (* update_ui) (GmlcGuiServerTab * self, gboolean bShow);
};

GType gmlc_gui_server_tab_get_type (void);

void gmlc_gui_server_tab_update_ui (GmlcGuiServerTab * self, gboolean bShow);

G_END_DECLS

#endif /* __GMLC_GUI_SERVER_TAB_H__ */

