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

#ifndef __GMLC_DUMP_SOURCE_H__
#define __GMLC_DUMP_SOURCE_H__ 

#include "gmlc_main.h"

G_BEGIN_DECLS

#define GMLC_DUMP_TYPE_SOURCE				(gmlc_dump_source_get_type ())
#define GMLC_DUMP_SOURCE(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj), GMLC_DUMP_TYPE_SOURCE, GmlcDumpSource))
#define GMLC_DUMP_IS_SOURCE(obj)			(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMLC_DUMP_TYPE_SOURCE
#define GMLC_DUMP_SOURCE_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), GMLC_DUMP_TYPE_SOURCE, GmlcDumpSourceInterface))

typedef struct _GmlcDumpSource GmlcDumpSource;
typedef struct _GmlcDumpSourceInterface GmlcDumpSourceInterface;

struct _GmlcDumpSourceInterface {
	GTypeInterface parent;
	
	gboolean (* can_get_struct) (GmlcDumpSource * self);
	gchar * (* get_struct) (GmlcDumpSource * self);
	gboolean (* can_get_data) (GmlcDumpSource * self);
	GArray * (* get_data) (GmlcDumpSource * self);
};

/* Exported data */
struct _GmlcDumpSourceData {
	gchar * pcDatabaseName;
	gchar * pcTableName;
	GArray * arHeaders;
	GArray * arDatas; /* GArray of GArray */
};
typedef struct _GmlcDumpSourceData GmlcDumpSourceData;

GType gmlc_dump_source_get_type (void);

gboolean gmlc_dump_source_can_get_struct (GmlcDumpSource * self);
gchar * gmlc_dump_source_get_struct (GmlcDumpSource * self);

gboolean gmlc_dump_source_can_get_data (GmlcDumpSource * self);
GArray * gmlc_dump_source_get_data (GmlcDumpSource * self);

gboolean gmlc_dump_source_data_free(GArray * arSourceData);

G_END_DECLS

#endif /* __GMLC_DUMP_SOURCE_H__ */

