
#ifndef __GMLC_MISC_CONFIG_H__
#define __GMLC_MISC_CONFIG_H__

#include <glib.h>
#include <glib-object.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../config.h"

#include "gmlc_mysql_server.h"

#define GMYSQLCC_CONF_DIR "gmysqlcc"

#define SERVERS_FILE "gmysqlcc_serv.xml"
#define CONFIG_FILE "gmysqlcc_conf.xml"

G_BEGIN_DECLS

#define GMLC_TYPE_MISC_CONFIG             (gmlc_misc_config_get_type ())
#define GMLC_MISC_CONFIG(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GMLC_TYPE_MISC_CONFIG, GmlcMiscConfig))
#define GMLC_MISC_CONFIG_CLASS(obj)       (G_TYPE_CHECK_CLASS_CAST ((obj), GMLC_TYPE_MISC_CONFIG,  GmlcMiscConfigClass))
#define GMLC_IS_MISC_CONFIG(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMLC_TYPE_MISC_CONFIG))
#define GMLC_IS_MISC_CONFIG_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), GMLC_TYPE_MISC_CONFIG))
#define GMLC_MISC_CONFIG_GET_CLASS        (G_TYPE_INSTANCE_GET_CLASS ((obj), GMLC_TYPE_MISC_CONFIG, GmlcMiscConfigClass))

typedef struct _GmlcMiscConfig            GmlcMiscConfig;
typedef struct _GmlcMiscConfigClass       GmlcMiscConfigClass;

struct _GmlcMiscConfig {
	GObject		parent;
	
	/* instance members */
	
	/* private */
/* config file name */
	gchar * pcConfigFilename;
	gchar * pcServersFilename;
	
/* Fonts' names */
	gchar * pcQueryFontName;
	gchar * pcHelpFontName;
	
/* Servers' list */
	GList * lstServers;
	
};

struct _GmlcMiscConfigClass {
	GObjectClass parent_class;
	
	/* class members */
	
};

GType gmlc_misc_config_get_type (void);

GmlcMiscConfig * gmlc_misc_config_new (void);

gboolean gmlc_misc_config_add_server (GmlcMiscConfig * pGmlcMscCfg, const gchar * name, const gchar * host, const gint port, const gchar * login, const gchar * passwd, const gchar * dbAllowedList, const gchar * localSock, const gboolean read_only, const gboolean write_warning);
gboolean gmlc_misc_config_update_server (GmlcMiscConfig * pGmlcMscCfg, const gchar * oldname, const gchar * name, const gchar * host, const gint port, const gchar * login, const gchar * passwd, const gchar * dbAllowedList, const gchar * localSock, const gboolean read_only, const gboolean write_warning);
gboolean gmlc_misc_config_del_server (GmlcMiscConfig * pGmlcMscCfg, const gchar * name);
GmlcMysqlServer * gmlc_misc_config_get_server (GmlcMiscConfig * pGmlcMscCfg, const gchar * name);

gboolean gmlc_misc_config_read (GmlcMiscConfig * pGmlcMscCfg);
gboolean gmlc_misc_config_write (GmlcMiscConfig * pGmlcMscCfg);

G_END_DECLS

#endif /* __GMLC_MISC_CONFIG_H__ */
