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

#ifndef __GMLC_DUMP_FORMAT_H__
#define __GMLC_DUMP_FORMAT_H__ 

#include "gmlc_main.h"

G_BEGIN_DECLS

#define GMLC_DUMP_TYPE_FORMAT				(gmlc_dump_format_get_type ())
#define GMLC_DUMP_FORMAT(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj), GMLC_DUMP_TYPE_FORMAT, GmlcDumpFormat))
#define GMLC_DUMP_IS_FORMAT(obj)			(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMLC_DUMP_TYPE_FORMAT
#define GMLC_DUMP_FORMAT_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), GMLC_DUMP_TYPE_FORMAT, GmlcDumpFormatInterface))

typedef struct _GmlcDumpFormat GmlcDumpFormat;
typedef struct _GmlcDumpFormatInterface GmlcDumpFormatInterface;

struct _GmlcDumpFormatInterface {
	GTypeInterface parent;
	
	gboolean (* set_struct) (GmlcDumpFormat * self, const gchar * pcStruct);
	gboolean (* set_data) (GmlcDumpFormat * self, const GArray * arDatas);
	gchar * (* run) (GmlcDumpFormat * self);
};

GType gmlc_dump_format_get_type (void);

gboolean gmlc_dump_format_set_struct (GmlcDumpFormat * self, const gchar * pcStruct);
gboolean gmlc_dump_format_set_data (GmlcDumpFormat * self, const GArray * arDatas);

gchar * gmlc_dump_format_run (GmlcDumpFormat * self);

G_END_DECLS

#endif /* __GMLC_DUMP_FORMAT_H__ */

