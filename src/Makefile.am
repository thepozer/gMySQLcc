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
	gmysql_gui.h\
	gExecSql.c\
	gListServer.c\
	gServer.c\
	gText.c\
	gUtils.c\
	gmysqlcc_gui_all.h\
	gmysqlcc_gui_dump.c\
	gmysql_conf.c\
	gmysql_conf.h\
	gmysql_utils.c\
	gmysql_utils.h\
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

