/***************************************************************************
 *            gmlc_mysql_table.h
 *
 *  
 *  Copyright  2007  Didier "pozer" Prolhac
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

#ifndef __GMLC_MYSQL_TABLE_H__
#define __GMLC_MYSQL_TABLE_H__

#include <glib.h>
#include <glib-object.h>

#include "gmlc_mysql_database.h"

#define UNUSED_VAR(x) (x = x)

G_BEGIN_DECLS

#define GMLC_TYPE_MYSQL_TABLE			(gmlc_mysql_table_get_type ())
#define GMLC_MYSQL_TABLE(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj), GMLC_TYPE_MYSQL_TABLE, GmlcMysqlTable))
#define GMLC_MYSQL_TABLE_CLASS(kls)		(G_TYPE_CHECK_CLASS_CAST((kls), GMLC_TYPE_MYSQL_TABLE, GmlcMysqlTableClass))
#define GMLC_IS_MYSQL_TABLE(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMLC_TYPE_MYSQL_TABLE))
#define GMLC_IS_MYSQL_TABLE_CLASS(kls)	(G_TYPE_CHECK_CLASS_TYPE ((kls), GMLC_TYPE_MYSQL_TABLE))
#define GMLC_MYSQL_TABLE_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), GMLC_TYPE_MYSQL_TABLE, GmlcMysqlTableClass))

#ifndef __GMLC_MYSQL_TABLE_H__TYPE__
#define __GMLC_MYSQL_TABLE_H__TYPE__
typedef struct _GmlcMysqlTable		GmlcMysqlTable;
#endif /* __GMLC_MYSQL_TABLE_H__TYPE__ */

typedef struct _GmlcMysqlTableClass GmlcMysqlTableClass;

struct _GmlcMysqlTable {
	GObject parent;
	
	GmlcMysqlDatabase * pGmlcMysqlDb;
	gchar * pcName;
	
	gchar * pcRows;
	gchar * pcSize;
	gchar * pcType;
	
	gboolean bFlagged;
};

struct _GmlcMysqlTableClass {
	GObjectClass parent_class;
	/* Add Signal Functions Here */
};

GType gmlc_mysql_table_get_type();

GmlcMysqlTable * gmlc_mysql_table_new(GmlcMysqlDatabase * pGmlcMysqlDb, gchar * pcTblName);
GmlcMysqlTable * gmlc_mysql_table_new_with_stat(GmlcMysqlDatabase * pGmlcMysqlDb, gchar * pcTblName, gchar * pcRows, gchar * pcSize, gchar * pcType);

G_END_DECLS

#endif /* __GMLC_MYSQL_TABLE_H__ */
