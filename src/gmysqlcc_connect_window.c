#include "gmysqlcc_connect_window.h"

struct _GmysqlccConnectWindow {
    GtkApplicationWindow parent;
    
    
};

G_DEFINE_TYPE(GmysqlccConnectWindow, gmysqlcc_connect_window, GTK_TYPE_APPLICATION_WINDOW);

static void gmysqlcc_connect_window_init (GmysqlccConnectWindow *pWindow) {
    gtk_widget_init_template(GTK_WIDGET(pWindow));
}

static void gmysqlcc_connect_window_class_init (GmysqlccConnectWindowClass *pClass) {
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(pClass), "/net/thepozer/gmysqlcc/gmysqlcc.wnd-connect.glade");
}

gboolean gmsqlcc_cnnct_clbk_delete_event (GtkWidget * widget, GdkEvent * event, gpointer user_data) {
    GmysqlccConnectWindow * pWindow = GMYSQLCC_CONNECT_WINDOW(widget);

    gtk_widget_hide(GTK_WIDGET(pWindow)); //!gmysqlcc_connect_window_close_all(pWindow);

    return TRUE;
}

GmysqlccConnectWindow * gmysqlcc_connect_window_new (GmysqlccApp *pApp) {
    GmysqlccConnectWindow * pWindow = g_object_new (GMYSQLCC_TYPE_CONNECT_WINDOW, "application", pApp, NULL);

    g_signal_connect(pWindow, "delete-event", G_CALLBACK(gmsqlcc_cnnct_clbk_delete_event), pWindow);

    return pWindow;
}
