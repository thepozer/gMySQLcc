
#ifndef __GMLC_MYSQL_SERVER_H__
#define __GMLC_MYSQL_SERVER_H__

#include <glib.h>
#include <glib-object.h>

#define UNUSED_VAR(x) (x = x)

G_BEGIN_DECLS

#define GMLC_TYPE_MYSQL_SERVER             (gmlc_mysql_server_get_type ())
#define GMLC_MYSQL_SERVER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GMLC_TYPE_MYSQL_SERVER, GmlcMysqlServer))
#define GMLC_MYSQL_SERVER_CLASS(obj)       (G_TYPE_CHECK_CLASS_CAST ((obj), GMLC_TYPE_MYSQL_SERVER,  GmlcMysqlServerClass))
#define GMLC_IS_MYSQL_SERVER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMLC_TYPE_MYSQL_SERVER))
#define GMLC_IS_MYSQL_SERVER_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), GMLC_TYPE_MYSQL_SERVER))
#define GMLC_MYSQL_SERVER_GET_CLASS        (G_TYPE_INSTANCE_GET_CLASS ((obj), GMLC_TYPE_MYSQL_SERVER, GmlcMysqlServerClass))

typedef struct _GmlcMysqlServer            GmlcMysqlServer;
typedef struct _GmlcMysqlServerClass       GmlcMysqlServerClass;

struct _GmlcMysqlServer {
	GObject		parent;
	
	/* instance members */
	
	/* private */
/* Connection infos */
	gchar *		pcName;
	gchar *		pcHost;
	guint		iPort;
	gchar *		pcLogin;
	gchar *		pcPassword;
	gchar *		pcLocalSock;
	
/* Server protections */
	gchar *		pcAllowedDbs;
	gboolean	bReadOnly;
	gboolean	bWriteWarning;

/* Server datas */
	gulong		lVersion;	
};

struct _GmlcMysqlServerClass {
	GObjectClass parent_class;
	
	/* class members */
	
};

GType gmlc_mysql_server_get_type (void);

GmlcMysqlServer * gmlc_mysql_server_new (void);

gchar * gmlc_mysql_server_generate_xml_config(GmlcMysqlServer * pGmlcMysqlSrv);

G_END_DECLS

#endif /* __GMLC_MYSQL_SERVER_H__ */
