/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __GMLC_MISC_HASHTABLE_H__
#define __GMLC_MISC_HASHTABLE_H__

#include <glib.h>
#include <glib-object.h>

#define UNUSED_VAR(x) (x = x)

G_BEGIN_DECLS

gboolean gmlc_misc_hash_table_set_flag(GHashTable * htbLstObjects, gpointer pValue, gboolean bNewValue);
gboolean gmlc_misc_hash_table_set_all_flag(GHashTable * htbLstObjects, gboolean bNewValue);
gboolean gmlc_misc_hash_table_remove_unflag_object(GHashTable * htbLstObjects);

G_END_DECLS

#endif /* __GMLC_MISC_HASHTABLE_H__ */
