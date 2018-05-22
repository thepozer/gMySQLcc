#ifndef __GMYSQLCC_APP_H__
#define __GMYSQLCC_APP_H__

#include "gmysqlcc_main.h"


#define GMYSQLCC_TYPE_APP (gmysqlcc_app_get_type ())
G_DECLARE_FINAL_TYPE (GmysqlccApp, gmysqlcc_app, GMYSQLCC, APP, GtkApplication)

GmysqlccApp * gmysqlcc_app_new (void);
void          gmysqlcc_app_quit (GmysqlccApp * pApp);

GSettings *   gmysqlcc_app_get_settings (GmysqlccApp * pApp);

#endif /* __GMYSQLCC_APP_H__ */
