
#ifndef __GMYSQLCC_CONNECT_WINDOW_H__
#define __GMYSQLCC_CONNECT_WINDOW_H__

#include <string.h>

#include "gmysqlcc_main.h"
#include "gmysqlcc_app.h"

#define GMYSQLCC_TYPE_CONNECT_WINDOW (gmysqlcc_connect_window_get_type ())
G_DECLARE_FINAL_TYPE (GmysqlccConnectWindow, gmysqlcc_connect_window, GMYSQLCC, CONNECT_WINDOW, GtkApplicationWindow)


GmysqlccConnectWindow * gmysqlcc_connect_window_new      (GmysqlccApp * pApp);
//void                    gmysqlcc_connect_window_open     (GmysqlccConnectWindow * pWindow, GFile * pFile);

#endif /* __GMYSQLCC_CONNECT_WINDOW_H__ */
