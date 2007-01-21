
#ifndef __GMLC_MYSQL_QUERY_H__
#define __GMLC_MYSQL_QUERY_H__

#include <glib.h>
#include <glib-object.h>

#include <mysql/mysql.h>

#include "gmlc_mysql_server.h"

#define UNUSED_VAR(x) (x = x)

G_BEGIN_DECLS

#define GMLC_TYPE_MYSQL_QUERY             (gmlc_mysql_query_get_type ())
#define GMLC_MYSQL_QUERY(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GMLC_TYPE_MYSQL_QUERY, GmlcMysqlQuery))
#define GMLC_MYSQL_QUERY_CLASS(obj)       (G_TYPE_CHECK_CLASS_CAST ((obj), GMLC_TYPE_MYSQL_QUERY,  GmlcMysqlQueryClass))
#define GMLC_IS_MYSQL_QUERY(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMLC_TYPE_MYSQL_QUERY))
#define GMLC_IS_MYSQL_QUERY_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), GMLC_TYPE_MYSQL_QUERY))
#define GMLC_MYSQL_QUERY_GET_CLASS        (G_TYPE_INSTANCE_GET_CLASS ((obj), GMLC_TYPE_MYSQL_QUERY, GmlcMysqlQueryClass))

typedef struct _GmlcMysqlQuery            GmlcMysqlQuery;
typedef struct _GmlcMysqlQueryClass       GmlcMysqlQueryClass;

struct _GmlcMysqlQuery {
	GObject		parent;
	
	/* private */
/* Query Infos */
	gchar *		pcQuery;
	gsize		szQuery;
	
/* Result Infos */
	gint		iErrCode;
	gchar *		pcErrMsg;
	gint		iEditResult;
	gint		iNbField;
	gboolean	bNoRecord;
	
/* Connection Infos */
	GmlcMysqlServer *	pGmlcMysqlSrv;
	gchar *		pcDbName;
	
/* Mysql Infos */
	MYSQL *			pMysqlLink;
	MYSQL_RES *		pMysqlResult;
	MYSQL_FIELD *	pMysqlHeaders;
	GArray * 		arMysqlHeaders;
	gchar *			pcSrvCharset;
	gulong			lVersion;
};

struct _GmlcMysqlQueryClass {
	GObjectClass parent_class;
	
	/* class members */
	
};

GType gmlc_mysql_query_get_type (void);

gboolean gmlc_tools_query_is_read_query(const gchar * query);

GmlcMysqlQuery * gmlc_mysql_query_new (GObject * pGmlcMysqlSrv, gchar * pcDbName);

gulong gmlc_mysql_query_get_version(GmlcMysqlQuery * pGmlcMysqlQry);
gchar * gmlc_mysql_query_get_current_charset(GmlcMysqlQuery * pGmlcMysqlQry);

gboolean gmlc_mysql_query_execute(GmlcMysqlQuery * pGmlcMysqlQry, const gchar * pcQuery, gsize szQuery, gboolean bForceWrite);
gboolean gmlc_mysql_query_have_record(GmlcMysqlQuery * pGmlcMysqlQry);
GArray * gmlc_mysql_query_next_record(GmlcMysqlQuery * pGmlcMysqlQry);
gboolean gmlc_mysql_query_have_more_result(GmlcMysqlQuery * pGmlcMysqlQry);
gboolean gmlc_mysql_query_goto_next_result(GmlcMysqlQuery * pGmlcMysqlQry);
gboolean gmlc_mysql_query_free_result (GmlcMysqlQuery * pGmlcMysqlQry);

GArray * gmlc_mysql_query_get_headers(GmlcMysqlQuery * pGmlcMysqlQry);

G_END_DECLS

#endif /* __GMLC_MYSQL_QUERY_H__ */
