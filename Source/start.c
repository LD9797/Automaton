
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <math.h>
#include <ctype.h>
#include "../Headers/table.h"

GtkWidget *window_start;
GtkWidget *loading;
GtkWidget *spinner_loading;
GtkWidget *btn_info_estados;
GtkWidget *info_estados;
GtkWidget *btn_dialog_1;
GtkWidget *btn_dialog_2;
GtkWidget *btn_info_simbolos;
GtkWidget *info_simbolos;
GtkWidget *spn_estados;
GtkWidget *spn_simbolos;
GtkWidget *btn_exit;
GtkWidget *btn_continuar;


void deploy_window();

int main(int argc, char *argv[]) {
    // Init GTK
    gtk_init(&argc, &argv);

    deploy_window();

    gtk_main();

    return EXIT_SUCCESS;
}

void deploy_window(){
    GtkBuilder *builder;
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

    spn_estados = GTK_WIDGET(gtk_builder_get_object(builder, "spn_estados"));
    spn_simbolos = GTK_WIDGET(gtk_builder_get_object(builder, "spn_simbolos"));

    btn_continuar = GTK_WIDGET(gtk_builder_get_object(builder, "btn_continuar"));
    btn_exit = GTK_WIDGET(gtk_builder_get_object(builder, "btn_exit"));

    spinner_loading = GTK_WIDGET(gtk_builder_get_object(builder, "spinner_loading"));

    gtk_widget_show(window_start);
  }

void on_btn_continuar_pressed(GtkWidget *button){
    gtk_widget_set_visible(spinner_loading, TRUE);
}

void on_btn_continuar_clicked(GtkWidget *button) {
     gtk_widget_set_visible(spinner_loading, TRUE);

    GtkSpinner *spinner = GTK_SPINNER(spinner_loading);
    gtk_spinner_start(spinner);

    gdouble cantidad_estados = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spn_estados));
    gdouble cantidad_simbolos = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spn_simbolos));

    int estados_int = (int)cantidad_estados;
    int simbolos_int = (int)cantidad_simbolos;

    g_print("Estados: %d\n", estados_int);
    g_print("Simbolos: %d\n", simbolos_int);

    deploy_window_table(estados_int, simbolos_int, window_start);

    gtk_spinner_stop(spinner);

   if (gtk_widget_is_toplevel(window_start)) {
        gtk_widget_hide(window_start);
   }
}

void on_btn_info_estados_clicked(GtkWidget *button) {
    if (info_estados != NULL) {
        gtk_dialog_run(GTK_DIALOG(info_estados));
        gtk_widget_hide(info_estados);
    }
}

void on_btn_info_simbolos_clicked(GtkWidget *button) {
    if(info_simbolos != NULL){
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

void on_btn_exit_clicked(GtkWidget *button) {
    if(window_start != NULL){
        gtk_main_quit();
    }
}
