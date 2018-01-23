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

#ifndef __GMLC_GUI_HELPERS_H__
#define __GMLC_GUI_HELPERS_H__

#include "gmlc_main.h"

G_BEGIN_DECLS

GString * askInfo(const gchar * title, const gchar * message, const gchar * defaultValue);
gboolean askConfirmation(const gchar * title, const gchar * message);
gboolean askYesno(const gchar * title, const gchar * message);

gchar * gmlc_helpers_protect_underscore (const gchar * str);

G_END_DECLS

#endif /* __GMLC_GUI_HELPERS_H__ */
