INCLUDES = \
	$(GTK_CFLAGS)

AM_CFLAGS = \
  -ggdb


bin_PROGRAMS = gmysqlcc

gmysqlcc_SOURCES = gDump.h \
	gDump.c \
  gExecSql.h \
  gExecSql.c \
  gListServer.h \
  gListServer.c \
  gServer.h \
  gServer.c \
	gTableUtils.h \
  gUtils.h  \
  gUtils.c  \
  gmysql_conf.h \
  gmysql_conf.c \
  gmysqlcc.c \
  gmysql_utils.h \
  gmysql_utils.c \
  mysql_db_all.h \
  mysql_dump.c \
  mysql_row.c \
  mysql_table.c \
  mysql_database.c\
  mysql_query.c \
  mysql_server.c

gmysqlcc_LDADD =  \
	$(GTK_LIBS) \
	$(MySQLc_LIB)

