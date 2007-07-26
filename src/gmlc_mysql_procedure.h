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

#ifndef __GMLC_MYSQL_PROCEDURE_H__
#define __GMLC_MYSQL_PROCEDURE_H__

#include <glib.h>
#include <glib-object.h>

#include "gmlc_mysql_database.h"
#include "gmlc_mysql_structure.h"

#define UNUSED_VAR(x) (x = x)

G_BEGIN_DECLS

#define GMLC_TYPE_MYSQL_PROCEDURE			(gmlc_mysql_procedure_get_type ())
#define GMLC_MYSQL_PROCEDURE(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj), GMLC_TYPE_MYSQL_PROCEDURE, GmlcMysqlProcedure))
#define GMLC_MYSQL_PROCEDURE_CLASS(kls)		(G_TYPE_CHECK_CLASS_CAST((kls), GMLC_TYPE_MYSQL_PROCEDURE, GmlcMysqlProcedureClass))
#define GMLC_IS_MYSQL_PROCEDURE(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMLC_TYPE_MYSQL_PROCEDURE))
#define GMLC_IS_MYSQL_PROCEDURE_CLASS(kls)	(G_TYPE_CHECK_CLASS_TYPE ((kls), GMLC_TYPE_MYSQL_PROCEDURE))
#define GMLC_MYSQL_PROCEDURE_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), GMLC_TYPE_MYSQL_PROCEDURE, GmlcMysqlProcedureClass))

#ifndef __GMLC_MYSQL_PROCEDURE_H__TYPE__
#define __GMLC_MYSQL_PROCEDURE_H__TYPE__
typedef struct _GmlcMysqlProcedure	GmlcMysqlProcedure;
#endif /* __GMLC_MYSQL_PROCEDURE_H__TYPE__ */

typedef struct _GmlcMysqlProcedureClass GmlcMysqlProcedureClass;

struct _GmlcMysqlProcedure {
	GObject parent;
	
	GmlcMysqlDatabase * pGmlcMysqlDb;
	gchar * pcName;
	
	gboolean bFlagged;
};

struct _GmlcMysqlProcedureClass {
	GObjectClass parent_class;
	/* Add Signal Functions Here */
};

GType gmlc_mysql_procedure_get_type();

GmlcMysqlProcedure * gmlc_mysql_procedure_new(GmlcMysqlDatabase * pGmlcMysqlDb, gchar * pcName);

G_END_DECLS

#endif /* __GMLC_MYSQL_PROCEDURE_H__ */
