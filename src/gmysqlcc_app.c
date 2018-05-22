
#include "gmysqlcc_app.h"

struct _GmysqlccApp {
	GtkApplication parent;
  
	GSettings * pSettings;
};

G_DEFINE_TYPE(GmysqlccApp, gmysqlcc_app, GTK_TYPE_APPLICATION);

static void gmysqlcc_app_init (GmysqlccApp * pApp) {}

static void gmysqlcc_app_activate (GApplication * pApp) {

}

static void gmysqlcc_app_open (GApplication * pApp, GFile **files, gint n_files, const gchar * hint) {

}

static void gmysqlcc_app_class_init(GmysqlccAppClass *class)
{
	G_APPLICATION_CLASS (class)->activate = gmysqlcc_app_activate;
	G_APPLICATION_CLASS (class)->open = gmysqlcc_app_open;
}

GmysqlccApp * gmysqlcc_app_new (void) {
	GmysqlccApp * pApp = NULL;
	
	pApp = g_object_new (GMYSQLCC_TYPE_APP, "application-id", "net.thepozer.gmysqlcc", "flags", G_APPLICATION_HANDLES_OPEN, NULL);
			
	pApp->pSettings = g_settings_new ("net.thepozer.gmysqlcc");
	
	return pApp;
}

GSettings * gmysqlcc_app_get_settings (GmysqlccApp * pApp) {
	return pApp->pSettings;
}

void gmysqlcc_app_quit (GmysqlccApp * pApp) {
	
	g_application_quit(G_APPLICATION(pApp));
}
