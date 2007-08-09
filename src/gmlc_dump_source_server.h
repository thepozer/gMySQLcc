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

#ifndef __GMLC_DUMP_SOURCE_SERVER_H__
#define __GMLC_DUMP_SOURCE_SERVER_H__ 

#include "gmlc_main.h"
#include "gmlc_mysql_table.h"

G_BEGIN_DECLS

#define GMLC_DUMP_TYPE_SOURCE_SERVER				(gmlc_dump_source_server_get_type ())
#define GMLC_DUMP_SOURCE_SERVER(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj), GMLC_DUMP_TYPE_SOURCE_SERVER, GmlcDumpSourceServer))
#define GMLC_DUMP_SOURCE_SERVER_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass), GMLC_DUMP_TYPE_SOURCE_SERVER, GmlcDumpSourceServerClass))
#define GMLC_DUMP_IS_SOURCE_SERVER(obj)			(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMLC_DUMP_TYPE_SOURCE_SERVER))
#define GMLC_DUMP_IS_SOURCE_SERVER_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GMLC_DUMP_TYPE_SOURCE_SERVER))
#define GMLC_DUMP_GET_SOURCE_SERVER_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), GMLC_DUMP_TYPE_SOURCE_SERVER, GmlcDumpSourceServerClass))

typedef struct _GmlcDumpSourceServer GmlcDumpSourceServer;
typedef struct _GmlcDumpSourceServerClass GmlcDumpSourceServerClass;

struct _GmlcDumpSourceServer {
	GObject parent;
	
	/* private members */
	GmlcMysqlTable * pGmlcMysqlTbl;
};

struct _GmlcDumpSourceServerClass {
	GObjectClass parent_class;
	
	/* define vtable methods and signals here */
};

GType gmlc_dump_source_server_get_type (void);

GmlcDumpSourceServer * gmlc_dump_source_server_new (GmlcMysqlTable *pGmlcMysqlTbl);

G_END_DECLS

#endif /* __GMLC_GUI_SERVER_TAB_DATA_H__ */
