/***************************************************************************
 *            gmlc_mysql_table.c
 *
 *  lun jui 24 00:49:12 2006
 *  Copyright  2006  Didier "pozer" Prolhac
 *  pozer@thepozer.net
 ***************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "gmlc_mysql_table.h"
 
static void gmlc_mysql_table_class_init(gmlc_mysql_tableClass *klass);
static void gmlc_mysql_table_init(gmlc_mysql_table *sp);
static void gmlc_mysql_table_finalize(GObject *object);

struct _gmlc_mysql_tablePrivate {
	/* Place Private Members Here */
};

typedef struct _gmlc_mysql_tableSignal gmlc_mysql_tableSignal;
typedef enum _gmlc_mysql_tableSignalType gmlc_mysql_tableSignalType;

enum _gmlc_mysql_tableSignalType {
	/* Place Signal Types Here */
	SIGNAL_TYPE_EXAMPLE,
	LAST_SIGNAL
};

struct _gmlc_mysql_tableSignal {
	gmlc_mysql_table *object;
};

static guint gmlc_mysql_table_signals[LAST_SIGNAL] = { 0 };
static GObjectClass *parent_class = NULL;

GType
gmlc_mysql_table_get_type()
{
	static GType type = 0;

	if(type == 0) {
		static const GTypeInfo our_info = {
			sizeof (gmlc_mysql_tableClass),
			NULL,
			NULL,
			(GClassInitFunc)gmlc_mysql_table_class_init,
			NULL,
			NULL,
			sizeof (gmlc_mysql_table),
			0,
			(GInstanceInitFunc)gmlc_mysql_table_init,
		};

		type = g_type_register_static(G_TYPE_OBJECT, 
			"gmlc_mysql_table", &our_info, 0);
	}

	return type;
}

static void
gmlc_mysql_table_class_init(gmlc_mysql_tableClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	parent_class = g_type_class_peek_parent(klass);
	object_class->finalize = gmlc_mysql_table_finalize;
	
	/* Create signals here:
	   gmlc_mysql_table_signals[SIGNAL_TYPE_EXAMPLE] = g_signal_new(...)
 	*/
}

static void
gmlc_mysql_table_init(gmlc_mysql_table *obj)
{
	obj->priv = g_new0(gmlc_mysql_tablePrivate, 1);
	/* Initialize private members, etc. */
}

static void
gmlc_mysql_table_finalize(GObject *object)
{
	gmlc_mysql_table *cobj;
	cobj = GMLC_MYSQL_TABLE(object);
	
	/* Free private members, etc. */
		
	g_free(cobj->priv);
	G_OBJECT_CLASS(parent_class)->finalize(object);
}

gmlc_mysql_table *
gmlc_mysql_table_new()
{
	gmlc_mysql_table *obj;
	
	obj = GMLC_MYSQL_TABLE(g_object_new(GMLC_TYPE_MYSQL_TABLE, NULL));
	
	return obj;
}
