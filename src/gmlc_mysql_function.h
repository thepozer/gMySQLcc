/***************************************************************************
 *            gmlc_mysql_function.h
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

#ifndef __GMLC_MYSQL_FUNCTION_H__
#define __GMLC_MYSQL_FUNCTION_H__

#include <glib.h>
#include <glib-object.h>

#include "gmlc_mysql_database.h"

#define UNUSED_VAR(x) (x = x)

G_BEGIN_DECLS

#define GMLC_TYPE_MYSQL_FUNCTION			(gmlc_mysql_function_get_type ())
#define GMLC_MYSQL_FUNCTION(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj), GMLC_TYPE_MYSQL_FUNCTION, GmlcMysqlFunction))
#define GMLC_MYSQL_FUNCTION_CLASS(kls)		(G_TYPE_CHECK_CLASS_CAST((kls), GMLC_TYPE_MYSQL_FUNCTION, GmlcMysqlFunctionClass))
#define GMLC_IS_MYSQL_FUNCTION(obj)			(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMLC_TYPE_MYSQL_FUNCTION))
#define GMLC_IS_MYSQL_FUNCTION_CLASS(kls)	(G_TYPE_CHECK_CLASS_TYPE ((kls), GMLC_TYPE_MYSQL_FUNCTION))
#define GMLC_MYSQL_FUNCTION_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), GMLC_TYPE_MYSQL_FUNCTION, GmlcMysqlFunctionClass))

#ifndef __GMLC_MYSQL_FUNCTION_H__TYPE__
#define __GMLC_MYSQL_FUNCTION_H__TYPE__
typedef struct _GmlcMysqlFunction	GmlcMysqlFunction;
#endif /* __GMLC_MYSQL_FUNCTION_H__TYPE__ */

typedef struct _GmlcMysqlFunctionClass GmlcMysqlFunctionClass;

struct _GmlcMysqlFunction {
	GObject parent;
	
	GmlcMysqlDatabase * pGmlcMysqlDb;
	gchar * pcName;
	
	gboolean bFlagged;
};

struct _GmlcMysqlFunctionClass {
	GObjectClass parent_class;
	/* Add Signal Functions Here */
};

GType gmlc_mysql_function_get_type();

GmlcMysqlFunction * gmlc_mysql_function_new(GmlcMysqlDatabase * pGmlcMysqlDb, gchar * pcName);

G_END_DECLS

#endif /* __GMLC_MYSQL_FUNCTION_H__ */
