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

#include "gmlc_dump_source.h"

static void gmlc_dump_source_base_init (gpointer g_class) {
	static gboolean initialized = FALSE;
	UNUSED_VAR(g_class);
	
	if (!initialized) {
		/* create interface signals here. */
		initialized = TRUE;
	}
}

GType gmlc_dump_source_get_type (void) {
  static GType type = 0;
  
  if (type == 0) {
    static const GTypeInfo info = {
      sizeof (GmlcDumpSourceInterface),
      gmlc_dump_source_base_init,   /* base_init */
      NULL,   /* base_finalize */
      NULL,   /* class_init */
      NULL,   /* class_finalize */
      NULL,   /* class_data */
      0,
      0,      /* n_preallocs */
      NULL    /* instance_init */
    };
    
    type = g_type_register_static (G_TYPE_INTERFACE, "GmlcDumpSource", &info, 0);
  }
  
  return type;
}

gboolean gmlc_dump_source_can_get_struct (GmlcDumpSource * self) {
	return GMLC_DUMP_SOURCE_GET_INTERFACE(self)->can_get_struct(self);
}

gchar * gmlc_dump_source_get_struct (GmlcDumpSource * self) {
	return GMLC_DUMP_SOURCE_GET_INTERFACE(self)->get_struct(self);
}

gboolean gmlc_dump_source_can_get_data (GmlcDumpSource * self) {
	return GMLC_DUMP_SOURCE_GET_INTERFACE(self)->can_get_data(self);
}

GArray * gmlc_dump_source_get_data (GmlcDumpSource * self) {
	return GMLC_DUMP_SOURCE_GET_INTERFACE(self)->get_data(self);
}


gboolean gmlc_dump_source_data_free (GArray * arSourceData) {
	GmlcDumpSourceData * pGmlcDmpSrcData = NULL;
	GArray * arTmp = NULL;
	gchar * pcTmp = NULL;
	gint i = 0, j = 0, k = 0;
	
	g_return_val_if_fail(arSourceData != NULL, TRUE);
	
	for (i = 0; i < arSourceData->len; i ++) {
		pGmlcDmpSrcData = g_array_index(arSourceData, GmlcDumpSourceData *, i);
		
		g_free(pGmlcDmpSrcData->pcDatabaseName);
		g_free(pGmlcDmpSrcData->pcTableName);
		
		for (j = 0; j < pGmlcDmpSrcData->arHeaders->len; j ++) {
			pcTmp = g_array_index(pGmlcDmpSrcData->arHeaders, gchar *, j);
			
			g_free(pcTmp);
		}
		g_array_free(pGmlcDmpSrcData->arHeaders, TRUE);
		
		for (j = 0; j < pGmlcDmpSrcData->arDatas->len; j ++) {
			arTmp = g_array_index(pGmlcDmpSrcData->arDatas, GArray *, j);
			
			for (k = 0; k < arTmp->len; k ++) {
				pcTmp = g_array_index(arTmp, gchar *, k);
				
				g_free(pcTmp);
			}
			g_array_free(arTmp, TRUE);
		}
		g_array_free(pGmlcDmpSrcData->arDatas, TRUE);
	}
	
	return TRUE;
}
