
#include "gmysql_utils.h"

int gmysql_count_noascii_character (const unsigned char * string) {
	int i, size, retNbr = 0;
	
	size = strlen(string);
	
	for (i = 0; i < size; i++) {
		if (string[i] > 127) {
			retNbr ++;
		}
	}
	
	/*g_printerr("*** Count noascii char : '%s'(%d) = (%d)\n", string, size, retNbr);*/
	
	return retNbr;
}

gchar * gmysql_alloc_iconv(GIConv icv, const char * source) {
	gchar * tmpstrRead, * tmpstrWrite, * tmpstr = (gchar *)NULL;
	int size;
	size_t retIcv;
	gsize nbRead, nbWrite;
	
	if (icv > 0) {
		tmpstrRead = (gchar *)source;
		nbRead = strlen(tmpstrRead);
		nbWrite = nbRead + gmysql_count_noascii_character(source) + 1;
		tmpstr = g_try_malloc((nbWrite) * sizeof(gchar));
		if (tmpstr != (gchar *)NULL) {
			*tmpstr = '\0';
			tmpstrWrite = tmpstr;
			
			retIcv = g_iconv(icv, &tmpstrRead, &nbRead, &tmpstrWrite, &nbWrite);
			
			if (retIcv < 0) {
				/*g_printerr("--- Iconv error (%d) : '%s'\n", errno, strerrror(errno));*/
				g_free(tmpstr);
				tmpstr = (gchar *)NULL;
			} else {
				/*g_printerr("+++ Number of translation pb : %d\n", retIcv);
				g_printerr("+++ Translation : '%s':'%s' -> '%s':'%s' \n", source, tmpstrRead, tmpstr, tmpstrWrite);*/
				*tmpstrWrite = '\0';
			}
		}
	}
	
	if (tmpstr == (gchar *)NULL) {
		tmpstr = g_strdup(source);
	}
	
	return tmpstr;
}
