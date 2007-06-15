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

#ifndef __GMLC_MYSQL_STRUCTURE_H__
#define __GMLC_MYSQL_STRUCTURE_H__ 

#define UNUSED_VAR(x) (x = x)

#include <glib-object.h>

G_BEGIN_DECLS

#define GMLC_MYSQL_TYPE_STRUCTURE					(gmlc_mysql_structure_get_type ())
#define GMLC_MYSQL_STRUCTURE(obj)					(G_TYPE_CHECK_INSTANCE_CAST((obj), GMLC_MYSQL_TYPE_STRUCTURE, GmlcMysqlStructure))
#define GMLC_MYSQL_IS_STRUCTURE_TAB(obj)			(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMLC_MYSQL_TYPE_STRUCTURE))
#define GMLC_MYSQL_STRUCTURE_GET_INTERFACE(inst)	(G_TYPE_INSTANCE_GET_INTERFACE ((inst), GMLC_MYSQL_TYPE_STRUCTURE, GmlcMysqlStructureInterface))

typedef struct _GmlcMysqlStructure GmlcMysqlStructure;
typedef struct _GmlcMysqlStructureInterface GmlcMysqlStructureInterface;

struct _GmlcMysqlStructureInterface {
	GTypeInterface parent;
	
	gchar * (* get_create) (GmlcMysqlStructure * poSelf, gboolean bMyself, const gchar * pcOtherName);
	gchar * (* get_alter) (GmlcMysqlStructure * poSelf, gboolean bMyself, const gchar * pcOtherName);
	gchar * (* get_drop) (GmlcMysqlStructure * poSelf, gboolean bMyself, const gchar * pcOtherName);
};

GType gmlc_mysql_structure_get_type (void);

gchar * gmlc_mysql_structure_get_create (GmlcMysqlStructure * poSelf, gboolean bMyself, const gchar * pcOtherName);
gchar * gmlc_mysql_structure_get_alter (GmlcMysqlStructure * poSelf, gboolean bMyself, const gchar * pcOtherName);
gchar * gmlc_mysql_structure_get_drop (GmlcMysqlStructure * poSelf, gboolean bMyself, const gchar * pcOtherName);

G_END_DECLS

#endif /* __GMLC_MYSQL_STRUCTURE_H__ */

