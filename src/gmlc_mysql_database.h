/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License.
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

#ifndef __GMLC_MYSQL_DATABASE_H__
#define __GMLC_MYSQL_DATABASE_H__

#include <glib.h>
#include <glib-object.h>

#include "gmlc_mysql_server.h"
#include "gmlc_mysql_query.h"

#ifndef __GMLC_MYSQL_TABLE_H__TYPE__
#define __GMLC_MYSQL_TABLE_H__TYPE__
typedef struct _GmlcMysqlTable		GmlcMysqlTable;
#endif /* __GMLC_MYSQL_TABLE_H__TYPE__ */

#ifndef __GMLC_MYSQL_VIEW_H__TYPE__
#define __GMLC_MYSQL_VIEW_H__TYPE__
typedef struct _GmlcMysqlView		GmlcMysqlView;
#endif /* __GMLC_MYSQL_VIEW_H__TYPE__ */

#ifndef __GMLC_MYSQL_FUNCTION_H__TYPE__
#define __GMLC_MYSQL_FUNCTION_H__TYPE__
typedef struct _GmlcMysqlFunction	GmlcMysqlFunction;
#endif /* __GMLC_MYSQL_FUNCTION_H__TYPE__ */

#ifndef __GMLC_MYSQL_PROCEDURE_H__TYPE__
#define __GMLC_MYSQL_PROCEDURE_H__TYPE__
typedef struct _GmlcMysqlProcedure	GmlcMysqlProcedure;
#endif /* __GMLC_MYSQL_PROCEDURE_H__TYPE__ */



#include "gmlc_mysql_function.h"
#include "gmlc_mysql_procedure.h"
#include "gmlc_mysql_table.h"
#include "gmlc_mysql_view.h"

#define UNUSED_VAR(x) (x = x)

G_BEGIN_DECLS

#define GMLC_TYPE_MYSQL_DATABASE			(gmlc_mysql_database_get_type ())
#define GMLC_MYSQL_DATABASE(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj), GMLC_TYPE_MYSQL_DATABASE, GmlcMysqlDatabase))
#define GMLC_MYSQL_DATABASE_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), GMLC_TYPE_MYSQL_DATABASE, GmlcMysqlDatabaseClass))
#define GMLC_IS_MYSQL_DATABASE(obj)			(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMLC_TYPE_MYSQL_DATABASE))
#define GMLC_IS_MYSQL_DATABASE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GMLC_TYPE_MYSQL_DATABASE))
#define GMLC_GET_MYSQL_DATABASE_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), GMLC_TYPE_MYSQL_DATABASE, GmlcMysqlDatabaseClass))

#ifndef __GMLC_MYSQL_DATABASE_H__TYPE__
#define __GMLC_MYSQL_DATABASE_H__TYPE__
typedef struct _GmlcMysqlDatabase            GmlcMysqlDatabase;
#endif /* __GMLC_MYSQL_DATABASE_H__TYPE__ */

typedef struct _GmlcMysqlDatabaseClass GmlcMysqlDatabaseClass;

struct _GmlcMysqlDatabase {
	GObject			parent;
	
	/* Database identification */
	GmlcMysqlServer *	pGmlcMysqlSrv;
	gchar *				pcDbName;
	
	/* Database information */
	GHashTable *		htbTables;
	GHashTable *		htbViews;
	GHashTable *		htbFunctions;
	GHashTable *		htbProcedures;
	
	/* Extra information */
	gboolean			bFlagged;
};

struct _GmlcMysqlDatabaseClass {
	GObjectClass		parent_class;
};

GType gmlc_mysql_database_get_type (void);


GmlcMysqlDatabase * gmlc_mysql_database_new (GmlcMysqlServer * pGmlcMysqlSrv, const gchar * pcDbName);

GArray * gmlc_mysql_database_tables_name_list(GmlcMysqlDatabase *pGmlcMysqlDb, gboolean bUpdateList);
GmlcMysqlTable * gmlc_mysql_database_get_table(GmlcMysqlDatabase * pGmlcMysqlDb, const gchar * pcName);
gboolean gmlc_mysql_database_update_tables_list(GmlcMysqlDatabase *pGmlcMysqlDb);

GArray * gmlc_mysql_database_views_name_list(GmlcMysqlDatabase *pGmlcMysqlDb, gboolean bUpdateList);
GmlcMysqlView * gmlc_mysql_database_get_view(GmlcMysqlDatabase * pGmlcMysqlDb, const gchar * pcName);
gboolean gmlc_mysql_database_update_views_list(GmlcMysqlDatabase *pGmlcMysqlDb);

GArray * gmlc_mysql_database_functions_name_list(GmlcMysqlDatabase *pGmlcMysqlDb, gboolean bUpdateList);
GmlcMysqlFunction * gmlc_mysql_database_get_function(GmlcMysqlDatabase * pGmlcMysqlDb, const gchar * pcName);
gboolean gmlc_mysql_database_update_functions_list(GmlcMysqlDatabase *pGmlcMysqlDb);

GArray * gmlc_mysql_database_procedures_name_list(GmlcMysqlDatabase *pGmlcMysqlDb, gboolean bUpdateList);
GmlcMysqlProcedure * gmlc_mysql_database_get_procedure(GmlcMysqlDatabase * pGmlcMysqlDb, const gchar * pcName);
gboolean gmlc_mysql_database_update_procedures_list(GmlcMysqlDatabase *pGmlcMysqlDb);


G_END_DECLS

#endif /* __GMLC_MYSQL_DATABASE_H__ */
