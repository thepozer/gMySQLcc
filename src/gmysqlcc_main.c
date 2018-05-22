#define __GMYSQLCC_MAIN_PART__
#include "gmysqlcc_main.h"

#include "gmysqlcc_app.h"

int main (int argc, char *argv []) {
	/* Set up internationalization */
	setlocale (LC_ALL, "");
	bindtextdomain (PACKAGE, LOCALEDIR);
	textdomain (PACKAGE);

	g_resources_register(gmysqlcc_get_resource());
	
	return g_application_run(G_APPLICATION(gmysqlcc_app_new()), argc, argv);
}
