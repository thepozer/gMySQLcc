/***************************************************************************
 *            gmlc_mysql_table.h
 *
 *  lun jui 24 00:49:12 2006
 *  Copyright  2006  Didier "pozer" Prolhac
 *  pozer@thepozer.net
 ***************************************************************************/

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

#ifndef GMLC_MYSQL_TABLE_H
#define GMLC_MYSQL_TABLE_H

#include <glib.h>
#include <glib-object.h>

#define UNUSED_VAR(x) (x = x)

G_BEGIN_DECLS

#define GMLC_TYPE_MYSQL_TABLE         (gmlc_mysql_table_get_type ())
#define GMLC_MYSQL_TABLE(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), GMLC_TYPE_MYSQL_TABLE, gmlc_mysql_table))
#define GMLC_MYSQL_TABLE_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), GMLC_TYPE_MYSQL_TABLE, gmlc_mysql_tableClass))
#define GMLC_IS_MYSQL_TABLE(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), GMLC_TYPE_MYSQL_TABLE))
#define GMLC_IS_MYSQL_TABLE_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), GMLC_TYPE_MYSQL_TABLE))
#define GMLC_MYSQL_TABLE_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), GMLC_TYPE_MYSQL_TABLE, gmlc_mysql_tableClass))

typedef struct _gmlc_mysql_table gmlc_mysql_table;
typedef struct _gmlc_mysql_tablePrivate gmlc_mysql_tablePrivate;
typedef struct _gmlc_mysql_tableClass gmlc_mysql_tableClass;

struct _gmlc_mysql_table {
	GObject parent;
	gmlc_mysql_tablePrivate *priv;
};

struct _gmlc_mysql_tableClass {
	GObjectClass parent_class;
	/* Add Signal Functions Here */
};

GType gmlc_mysql_table_get_type();
gmlc_mysql_table *gmlc_mysql_table_new();

G_END_DECLS

#endif /* GMLC_MYSQL_TABLE_H */
