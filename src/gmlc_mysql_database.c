
#include "gmlc_mysql_database.h"

G_DEFINE_TYPE (GmlcMysqlDatabase, gmlc_mysql_database, G_TYPE_OBJECT)

/* we need to define these two functions */
static void gmlc_mysql_database_init (GmlcMysqlDatabase *pGmlcMysqlDb) {
	
}

static void gmlc_mysql_database_class_init (GmlcMysqlDatabaseClass *pClass) {
	
}


GmlcMysqlDatabase * gmlc_mysql_database_new () {
	return GMLC_MYSQL_DATABASE(g_object_new(GMLC_TYPE_MYSQL_DATABASE, NULL));
}
