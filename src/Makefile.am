INCLUDES = \
	$(GTK_CFLAGS)

AM_CFLAGS = \
  -ggdb


bin_PROGRAMS = gmysqlcc

gmysqlcc_SOURCES = gDump.c \
  gExecSql.c \
  gListServer.c \
  gServer.c \
  gUtils.c  \
  gmysql_conf.c \
  gmysqlcc.c \
  gmysql_utils.c \
  mysql_dump.c \
  mysql_row.c \
  mysql_table.c \
  mysql_database.c\
  mysql_query.c \
  mysql_server.c

gmysqlcc_LDADD =  \
	$(GTK_LIBS) \
	$(MySQLc_LIB)

