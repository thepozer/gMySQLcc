
#ifndef __GLISTSERVER_H__
#define __GLISTSERVER_H__

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <libintl.h>

#include "mysql_db_all.h"
#include "gmysql_conf.h"
#include "gServer.h"
#include "gUtils.h"

#define _(String) gettext (String)

typedef struct _listServWnd {
	  GtkWidget * txtName;
		GtkWidget * txtHost;
	  GtkWidget * txtPort;
		GtkWidget * txtLogin;
	  GtkWidget * txtPasswd;
	  GtkWidget * txtAllowedDb;
	  GtkWidget * txtLocalSock;
		GtkWidget * cmbListHosts;
		GtkWidget * lstListHosts;
		GtkWidget * wndListServer;
		p_mysql_server selSrvr;
		p_gmysql_config gmsql_conf;
} listServWnd;
typedef listServWnd * p_listServWnd;

listServWnd * create_wndListServer (gboolean display, p_gmysql_config gmsql_conf);

#endif /* __GLISTSERVER_H__ */
