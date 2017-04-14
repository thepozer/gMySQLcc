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

#ifndef __GMLC_MAIN_H__
#define __GMLC_MAIN_H__

#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include <libintl.h>

#include "../config.h"

#include "gmlc_misc_config.h"

#ifdef USE_GTKSOURCEVIEW
	#include <gtksourceview/gtksource.h>
#endif /* USE_GTKSOURCEVIEW */

#ifndef __GMYSQLCC_MAIN_PART__

	extern int GiNbrWnd;
	
	extern GmlcMiscConfig * GpGmlcMscCfg;
	
	extern GtkWidget * GpGmlcGuiCnxns;
	
	#ifdef USE_GTKSOURCEVIEW
		extern GtkSourceLanguageManager * LangManager;
	#endif /* USE_GTKSOURCEVIEW */

#endif /* __GMYSQLCC_MAIN_PART__ */


#endif /* __GMLC_MAIN_H__ */
