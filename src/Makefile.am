## Process this file with automake to produce Makefile.in

## Created by Anjuta - will be overwritten
## If you don't want it to overwrite it,
## 	Please disable it in the Anjuta project configuration

INCLUDES = \
	$(GTK_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = gmysqlcc

gmysqlcc_SOURCES = \
	gDump.c\
	gDump.h\
	gExecSql.c\
	gExecSql.h\
	gListServer.c\
	gListServer.h\
	gServer.c\
	gServer.h\
	gUtils.c\
	gUtils.h\
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
	mysql_table.c

gmysqlcc_LDFLAGS = 

gmysqlcc_LDADD =  \
	$(GTK_LIBS)\
	$(MySQLc_LIB)

