
#ifndef __GUTILS_H__
#define __GUTILS_H__

#include <gtk/gtk.h>

typedef struct _askFilenameInfos {
	GtkWidget * dialog;
	void * userData;
} askFilenameInfos;
typedef askFilenameInfos * p_askFilenameInfos;

GString * askInfo(const gchar * title, const gchar * message, const gchar * defaultValue);
gboolean askConfirmation(const gchar * title, const gchar * message);
gboolean askYesno(const gchar * title, const gchar * message);
gboolean askFilename(const gchar * title, const gchar * filename, void (*okevent) (GtkWidget *widget, gpointer user_data), gpointer user_data);

GtkWidget * createIconButton(const char * icon, const char * title);

#endif /* __GUTILS_H__ */
