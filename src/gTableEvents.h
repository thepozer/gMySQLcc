#ifndef __GTABLEEVENTS_H__
#define __GTABLEEVENTS_H__

#include <gtk/gtk.h>
#include <string.h>

#include "mysql_funcs.h"
#include "conf_funcs.h"
#include "gUtils.h"

#include "gTable.h"
#include "gTableUtils.h"

void lstindexused_selected (GtkTreeSelection *selection, gpointer user_data);
void lstindex_selected (GtkTreeSelection *selection, gpointer user_data);
void lstfield_selected (GtkTreeSelection *selection, gpointer user_data);
void cmbfieldtype_selected (GtkWidget *list, GtkWidget *widget, gpointer user_data);
void btnfieldnew_clicked (GtkWidget *widget, gpointer user_data);
void btnfieldadd_clicked (GtkWidget *widget, gpointer user_data);
void btnfieldedit_clicked (GtkWidget *widget, gpointer user_data);
void btnfielddel_clicked (GtkWidget *widget, gpointer user_data);
void btntlbrclose_clicked (GtkWidget *widget, gpointer user_data);
void btntlbrvalidate_clicked (GtkWidget *widget, gpointer user_data);
void destroy(GtkWidget *widget, gpointer user_data);

#endif /* __GTABLE_H__ */
