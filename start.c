
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <math.h>
#include <ctype.h>

GtkWidget *window_start;
GtkWidget *btn_info_estados;
GtkWidget *info_estados;
GtkWidget *btn_dialog_1;
GtkWidget *btn_dialog_2;
GtkWidget *btn_info_simbolos;
GtkWidget *info_simbolos;
GtkBuilder *builder;

int main(int argc, char *argv[]) {
    // Init GTK
    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_file ("Start.glade");
    window_start = GTK_WIDGET(gtk_builder_get_object(builder, "window_start"));

    // To finish the program
    g_signal_connect(window_start, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_builder_connect_signals(builder, NULL);

    btn_info_estados = GTK_WIDGET(gtk_builder_get_object(builder, "btn_info_estados"));
    btn_info_simbolos = GTK_WIDGET(gtk_builder_get_object(builder, "btn_info_simbolos"));

    info_estados = GTK_WIDGET(gtk_builder_get_object(builder, "info_estados"));
    btn_dialog_1 = GTK_WIDGET(gtk_builder_get_object(builder, "btn_dialog_1"));

    info_simbolos = GTK_WIDGET(gtk_builder_get_object(builder, "info_simbolos"));
    btn_dialog_2 = GTK_WIDGET(gtk_builder_get_object(builder, "btn_dialog_2"));

    gtk_widget_show(window_start);
    gtk_main();

    return EXIT_SUCCESS;
}

void on_btn_info_estados_clicked(GtkWidget *button) {
    if (info_estados != NULL) {
        // Open dialog in the center of the parent window.
        gtk_window_set_transient_for(GTK_WINDOW(info_estados), GTK_WINDOW(window_start));
        gtk_window_set_position(GTK_WINDOW(info_estados), GTK_WIN_POS_CENTER_ON_PARENT);

        gtk_dialog_run(GTK_DIALOG(info_estados));
        gtk_widget_hide(info_estados);
    }
}

void on_btn_info_simbolos_clicked(GtkWidget *button) {
    if(info_simbolos != NULL){
        // Open dialog in the center of the parent window.
        gtk_window_set_transient_for(GTK_WINDOW(info_simbolos), GTK_WINDOW(window_start));
        gtk_window_set_position(GTK_WINDOW(info_simbolos), GTK_WIN_POS_CENTER_ON_PARENT);

        gtk_dialog_run(GTK_DIALOG(info_simbolos));
        gtk_widget_hide(info_simbolos);
    }
}

void on_btn_dialog_1_clicked(GtkWidget *button) {
    if (info_estados != NULL) {
        gtk_widget_hide(info_estados);
    }
}

void on_btn_dialog_2_clicked(GtkWidget *button) {
    if(info_simbolos != NULL){
        gtk_widget_hide(info_simbolos);
    }
}
