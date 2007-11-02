/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __GMLC_DUMP_CONTROL_H__
#define __GMLC_DUMP_CONTROL_H__

#include <glib.h>
#include <glib-object.h>

#include "gmlc_mysql_server.h"
#include "gmlc_dump_source.h"
#include "gmlc_dump_format.h"

#define UNUSED_VAR(x) (x = x)

G_BEGIN_DECLS

#define GMLC_TYPE_DUMP_CONTROL				(gmlc_dump_control_get_type ())
#define GMLC_DUMP_CONTROL(obj)				(G_TYPE_CHECK_INSTANCE_CAST ((obj), GMLC_TYPE_DUMP_CONTROL, GmlcDumpControl))
#define GMLC_DUMP_CONTROL_CLASS(kls)		(G_TYPE_CHECK_CLASS_CAST((kls), 	GMLC_TYPE_DUMP_CONTROL, GmlcDumpControlClass))
#define GMLC_IS_DUMP_CONTROL(obj)			(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMLC_TYPE_DUMP_CONTROL))
#define GMLC_IS_DUMP_CONTROL_CLASS(kls)		(G_TYPE_CHECK_CLASS_TYPE ((kls), 	GMLC_TYPE_DUMP_CONTROL))
#define GMLC_DUMP_CONTROL_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), 	GMLC_TYPE_DUMP_CONTROL, GmlcDumpControlClass))

typedef struct _GmlcDumpControl		GmlcDumpControl;
typedef struct _GmlcDumpControlClass GmlcDumpControlClass;

struct _GmlcDumpControl {
	GObject parent;
	
	GmlcMysqlServer * pGmlcMysqlSrv;
	GmlcDumpSource * pSource;
	GmlcDumpFormat * pFormat;
};

struct _GmlcDumpControlClass {
	GObjectClass parent_class;
	/* Add Signal Functions Here */
};

GType gmlc_dump_control_get_type ();

GmlcDumpControl * gmlc_dump_control_new (GmlcMysqlServer * pGmlcMysqlSrv);

gboolean gmlc_dump_control_dump (GmlcDumpControl * pGmlcDmpCtrl);

gboolean gmlc_dump_control_direct_dump (GmlcMysqlServer * pGmlcMysqlSrv, GmlcDumpSource * pGmlcDmpSrc, GmlcDumpFormat * pGmlcDmpFrmt);

G_END_DECLS

#endif /* __GMLC_DUMP_CONTROL_H__ */
