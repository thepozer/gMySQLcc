## Process this file with automake to produce Makefile.in

INCLUDES = \
	$(GTK_CFLAGS) \
	$(GTKSourceView_CFLAGS)

AM_CFLAGS =\
	 -Wall 

bin_PROGRAMS = gmysqlcc

gmysqlcc_SOURCES = \
	gDump.c\
	gExecSql.c\
	gListServer.c\
	gServer.c\
	gUtils.c\
	gmysql_conf.c\
	gmysql_conf.h\
	gmysql_utils.c\
	gmysql_utils.h\
	gmysqlcc.c\
	mysql_database.c\
	mysql_db_all.h\
	mysql_dump.c\
	mysql_query.c\
	mysql_row.c\
	mysql_server.c\
	mysql_table.c\
	gTableUtils.h\
	mysql_user.c\
	gmysql_gui.h\
	gText.c

gmysqlcc_LDFLAGS = 

gmysqlcc_LDADD =  \
	$(GTK_LIBS) \
	$(GTKSourceView_LIBS) \
	$(MySQLc_LIB)

