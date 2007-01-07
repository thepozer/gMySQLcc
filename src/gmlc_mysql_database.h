
#ifndef __GMLC_MYSQL_DATABASE_H__
#define __GMLC_MYSQL_DATABASE_H__

#include <glib.h>
#include <glib-object.h>

#define UNUSED_VAR(x) (x = x)

G_BEGIN_DECLS

#define GMLC_TYPE_MYSQL_DATABASE			(gmlc_mysql_database_get_type ())
#define GMLC_MYSQL_DATABASE(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj), GMLC_TYPE_MYSQL_DATABASE, GmlcMysqlDatabase))
#define GMLC_MYSQL_DATABASE_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), GMLC_TYPE_MYSQL_DATABASE, GmlcMysqlDatabaseClass))
#define GMLC_IS_MYSQL_DATABASE(obj)			(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMLC_TYPE_MYSQL_DATABASE))
#define GMLC_IS_MYSQL_DATABASE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GMLC_TYPE_MYSQL_DATABASE))
#define GMLC_GET_MYSQL_DATABASE_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), GMLC_TYPE_MYSQL_DATABASE, GmlcMysqlDatabaseClass))

typedef struct _GmlcMysqlDatabase GmlcMysqlDatabase;
typedef struct _GmlcMysqlDatabaseClass GmlcMysqlDatabaseClass;

struct _GmlcMysqlDatabase {
	GObject parent;
	/* define public instance variables here */
};

struct _GmlcMysqlDatabaseClass {
	GObjectClass parent_class;
	/* define vtable methods and signals here */
};

GType gmlc_mysql_database_get_type (void);


GmlcMysqlDatabase * gmlc_mysql_database_new ();


G_END_DECLS

#endif /* __GMLC_MYSQL_DATABASE_H__ */
