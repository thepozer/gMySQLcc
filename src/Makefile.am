## Process this file with automake to produce Makefile.in

INCLUDES = \
	$(GTK_CFLAGS) \
	$(GTKSourceView_CFLAGS) \
	$(MySQLc_CFLAGS)

AM_CFLAGS =\
	 -Wall 

#SUBDIRS = plugins

bin_PROGRAMS = gmysqlcc

gmysqlcc_SOURCES = \
	gmysqlcc_main.c\
	gmysqlcc_config.c\
	gmysqlcc_config.h\
	gmysqlcc_helpers.c\
	gmysqlcc_helpers.h\
	gmysqlcc_gui_all.h\
	gmysqlcc_gui_dump.c\
	gmysqlcc_gui_list_server.c\
	gmysqlcc_gui_query.c\
	gmysqlcc_gui_server.c\
	gmysqlcc_gui_text.c\
	gmysqlcc_gui_misc.c\
	mysql_db_all.h\
	mysql_database.c\
	mysql_dump.c\
	mysql_dump_csv.c\
	mysql_dump_sql.c\
	mysql_dump_xml.c\
	mysql_query.c\
	mysql_row.c\
	mysql_server.c\
	mysql_table.c\
	mysql_user.c

gmysqlcc_LDFLAGS = 

gmysqlcc_LDADD =  \
	$(GTK_LIBS) \
	$(GTKSourceView_LIBS) \
	$(MySQLc_LIBS)

