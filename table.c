#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <math.h>
#include <ctype.h>
#include "table.h"

GtkWidget *window_table;
GtkWidget *start_window = NULL;

void deploy_window_table(int estados, int simbolos, GtkWidget *previous_window){
    start_window = previous_window;

    GtkBuilder *builder;
    builder = gtk_builder_new_from_file ("Table.glade");
    window_table = GTK_WIDGET(gtk_builder_get_object(builder, "window_table"));

    g_signal_connect(window_table, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_builder_connect_signals(builder, NULL);

    g_strdup_printf("Recibido estados: %d\n", estados);
    g_strdup_printf("Recibido simbolos %d\n", simbolos);

    gtk_widget_show(window_table);

}

void on_btn_return_clicked(GtkWidget *button) {
    if (gtk_widget_is_toplevel(window_table)) {
        gtk_widget_hide(window_table);
    }

    // Show the original window
    if (start_window) {
        gtk_widget_show(start_window);
    }
}

void on_btn_exit_table_clicked(GtkWidget *button) {
    if(window_table != NULL){
        gtk_main_quit();
    }
}