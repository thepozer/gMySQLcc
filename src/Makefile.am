## Process this file with automake to produce Makefile.in

INCLUDES = \
	$(GTK_CFLAGS) \
	$(GTKSourceView_CFLAGS) \
	$(MySQLc_CFLAGS)

AM_CFLAGS =\
	 -Wall 

bin_PROGRAMS = gmysqlcc 

gmysqlcc_SOURCES = \
	gmlc_dump_control.c\
	gmlc_dump_control.h\
	gmlc_dump_format.c\
	gmlc_dump_format.h\
	gmlc_dump_source.c\
	gmlc_dump_source.h\
	gmlc_dump_source_query.c\
	gmlc_dump_source_query.h\
	gmlc_dump_source_table.c\
	gmlc_dump_source_table.h\
	gmlc_dump_source_database.c\
	gmlc_dump_source_database.h\
	gmlc_dump_source_server.c\
	gmlc_dump_source_server.h\
	gmlc_gui_helpers.c\
	gmlc_gui_helpers.h\
	gmlc_gui_connexions.c\
	gmlc_gui_connexions.h\
	gmlc_gui_server.c\
	gmlc_gui_server.h\
	gmlc_gui_server_tab.c\
	gmlc_gui_server_tab.h\
	gmlc_gui_server_tab_data.c\
	gmlc_gui_server_tab_data.h\
	gmlc_gui_server_tab_help.c\
	gmlc_gui_server_tab_help.h\
	gmlc_gui_server_tab_query.c\
	gmlc_gui_server_tab_query.h\
	gmlc_main.h\
	gmlc_main.c\
	gmlc_misc_config.c\
	gmlc_misc_config.h\
	gmlc_misc_hashtable.c\
	gmlc_misc_hashtable.h\
	gmlc_mysql_structure.c\
	gmlc_mysql_structure.h\
	gmlc_mysql_server.c\
	gmlc_mysql_server.h\
	gmlc_mysql_query.c\
	gmlc_mysql_query.h\
	gmlc_mysql_database.c\
	gmlc_mysql_database.h\
	gmlc_mysql_table.c\
	gmlc_mysql_table.h\
	gmlc_mysql_view.c\
	gmlc_mysql_view.h\
	gmlc_mysql_procedure.c\
	gmlc_mysql_procedure.h\
	gmlc_mysql_function.c\
	gmlc_mysql_function.h

gmysqlcc_LDFLAGS = 

gmysqlcc_LDADD =  \
	$(GTK_LIBS) \
	$(GTKSourceView_LIBS) \
	$(MySQLc_LIBS)
