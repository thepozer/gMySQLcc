#include <sys/stat.h>
#include <sys/types.h>

#include <gtk/gtk.h>

#define __VAR_ARFIELDTYPES__

/*#include "mysql_funcs.h"
#include "conf_funcs.h"*/
#include "mysql_db_all.h"
#include "gmysql_conf.h"
#include "gTableUtils.h"
#include "gListServer.h"

#define CONF_DIR ".gmysqlcc"
#define XML_FILE "gmysqlcc_serv.xml"

p_fieldTypeCapability * arFieldTypes;
int szarFieldTypes = -1;

int NbrWnd = 0;

/* Field capabilities */

p_fieldTypeCapability createFieldTypeCapability (char * name, int value, gboolean unsign, gboolean zerofill, gboolean binary, gboolean autoincr) {
	p_fieldTypeCapability p_ftc;
	
	p_ftc = (p_fieldTypeCapability) g_try_malloc(sizeof(p_fieldTypeCapability));
	if (p_ftc != (p_fieldTypeCapability)NULL) {
		p_ftc->name = name;
		p_ftc->typeValue = value;
		p_ftc->unsign = unsign;
		p_ftc->zerofill = zerofill;
		p_ftc->binary = binary;
		p_ftc->autoincr = autoincr;
	}
	
	return p_ftc;
}

void initFieldTypeCapabilities () {
	szarFieldTypes = 0;
	arFieldTypes = (p_fieldTypeCapability *) g_try_malloc(sizeof(p_fieldTypeCapability) * 28);
	if (arFieldTypes != (p_fieldTypeCapability *)NULL) {
		arFieldTypes[szarFieldTypes] = createFieldTypeCapability("TINYINT"			, FTC_TV_LENGTH, TRUE, TRUE, FALSE, TRUE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = createFieldTypeCapability("SMALLINT"			, FTC_TV_LENGTH, TRUE, TRUE, FALSE, TRUE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = createFieldTypeCapability("MEDIUMINT"		, FTC_TV_LENGTH, TRUE, TRUE, FALSE, TRUE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = createFieldTypeCapability("INT"					, FTC_TV_LENGTH, TRUE, TRUE, FALSE, TRUE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = createFieldTypeCapability("INTEGER"			, FTC_TV_LENGTH, TRUE, TRUE, FALSE, TRUE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = createFieldTypeCapability("BIGINT"				, FTC_TV_LENGTH, TRUE, TRUE, FALSE, TRUE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = createFieldTypeCapability("REAL"					, FTC_TV_LENGTH_DECI, TRUE, TRUE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = createFieldTypeCapability("DOUBLE"				, FTC_TV_LENGTH_DECI, TRUE, TRUE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = createFieldTypeCapability("FLOAT"				, FTC_TV_LENGTH_DECI, TRUE, TRUE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = createFieldTypeCapability("DECIMAL"			, FTC_TV_LENGTH_DECI, TRUE, TRUE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = createFieldTypeCapability("NUMERIC"			, FTC_TV_LENGTH_DECI, TRUE, TRUE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = createFieldTypeCapability("CHAR"					, FTC_TV_LENGTH, FALSE, FALSE, TRUE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = createFieldTypeCapability("VARCHAR"			, FTC_TV_LENGTH, FALSE, FALSE, TRUE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = createFieldTypeCapability("DATE"					, FTC_TV_NONE, FALSE, FALSE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = createFieldTypeCapability("TIME"					, FTC_TV_NONE, FALSE, FALSE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = createFieldTypeCapability("TIMESTAMP"		, FTC_TV_NONE, FALSE, FALSE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = createFieldTypeCapability("DATETIME"			, FTC_TV_NONE, FALSE, FALSE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = createFieldTypeCapability("TINYBLOB"			, FTC_TV_NONE, FALSE, FALSE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = createFieldTypeCapability("BLOB"					, FTC_TV_NONE, FALSE, FALSE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = createFieldTypeCapability("MEDIUMBLOB"		, FTC_TV_NONE, FALSE, FALSE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = createFieldTypeCapability("LONGBLOB"			, FTC_TV_NONE, FALSE, FALSE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = createFieldTypeCapability("TINYTEXT"			, FTC_TV_NONE, FALSE, FALSE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = createFieldTypeCapability("TEXT"					, FTC_TV_NONE, FALSE, FALSE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = createFieldTypeCapability("MEDIUMTEXT"		, FTC_TV_NONE, FALSE, FALSE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = createFieldTypeCapability("LONGTEXT"			, FTC_TV_NONE, FALSE, FALSE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = createFieldTypeCapability("ENUM"					, FTC_TV_LIST_VALUE, FALSE, FALSE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = createFieldTypeCapability("SET"					, FTC_TV_LIST_VALUE, FALSE, FALSE, FALSE, FALSE); szarFieldTypes ++;
		arFieldTypes[szarFieldTypes] = (p_fieldTypeCapability)NULL;
	}
}

gboolean doBeforeEnd(gpointer data) {
	p_gmysql_config gmsql_conf = (p_gmysql_config)data;
	GString * configFilePath;

	configFilePath = g_string_new("");
	
	g_string_printf(configFilePath, "%s/%s", g_get_home_dir(), CONF_DIR);
	if (!g_file_test(configFilePath->str, (G_FILE_TEST_IS_DIR | G_FILE_TEST_EXISTS))) {
		mkdir (configFilePath->str, 0700);
	}
	
	g_string_printf(configFilePath, "%s/%s/%s", g_get_home_dir(), CONF_DIR, XML_FILE);
	gmysql_config_write_xml(gmsql_conf, configFilePath->str);
	g_string_free(configFilePath, TRUE);

	return TRUE;
}

int main(int argc, char *argv[]) {
  p_gmysql_config gmsql_conf;
	listServWnd * p_lstsvr;
	GString * configFilePath;

	gtk_init (&argc, &argv);
	
	initFieldTypeCapabilities();
	
	g_printerr("Client librarie version : %s\n", mysql_get_client_info());

	configFilePath = g_string_new("");
	gmsql_conf = gmysql_config_new();

	g_string_printf(configFilePath, "%s/%s/%s", g_get_home_dir(), CONF_DIR, XML_FILE);
	gmysql_config_read_xml(gmsql_conf, configFilePath->str);
	g_string_free(configFilePath, TRUE);
	
	gtk_quit_add(0, doBeforeEnd, gmsql_conf);
	
	p_lstsvr = create_wndListServer(TRUE, gmsql_conf);
	
	gtk_main ();
	
	return 0;
}
