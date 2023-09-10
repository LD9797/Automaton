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
GtkWidget *grid;
GtkWidget *start_window = NULL;
GObject *scrolled_window;

void create_table(int rows, int cols);
void set_widget_background_color(GtkWidget *widget, const gchar *color);

void deploy_window_table(int estados, int simbolos, GtkWidget *previous_window){
    start_window = previous_window;

    GtkBuilder *builder;
    builder = gtk_builder_new_from_file ("Table.glade");
    window_table = GTK_WIDGET(gtk_builder_get_object(builder, "window_table"));

    g_signal_connect(window_table, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_builder_connect_signals(builder, NULL);

    g_strdup_printf("Recibido estados: %d\n", estados);
    g_strdup_printf("Recibido simbolos %d\n", simbolos);


    scrolled_window = gtk_builder_get_object(builder, "Scrolled1");
    grid = gtk_grid_new();
    create_table(estados + 1, simbolos + 1);
    gtk_widget_show_all(grid);

    gtk_container_add(GTK_CONTAINER(scrolled_window), grid);

    gtk_widget_show(window_table);
}

void create_table(int rows, int cols){
   // gtk_widget_set_hexpand(grid, TRUE);
   // gtk_widget_set_vexpand(grid, TRUE);

    GSList *radio_group = NULL;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols + 2; ++j) {

            GtkWidget *widget_to_add;

            if (i == 0 && j == 1) {
                gchar *label_text = g_strdup_printf("Editar Estados");
                widget_to_add = gtk_label_new(label_text);
                g_free(label_text);
            }

            else if (i == 0 && j == 2) {
                gchar *label_text = g_strdup_printf("Símbolos /\n Estados");
                widget_to_add = gtk_label_new(label_text);
                g_free(label_text);
            }

            else if (i == 0 && j >= 3) {
                gchar *label_text = g_strdup_printf("Header %d", j - 2);
                widget_to_add = gtk_label_new(label_text);
                g_free(label_text);
            }

            else if (j == 0 && i != 0) {
                widget_to_add = gtk_radio_button_new_with_label(radio_group, "");
                radio_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(widget_to_add));
            }

            else if (j == 1 && i != 0) {
                widget_to_add = gtk_entry_new();
            }

            else if (j == 2 && i != 0) {
                gchar *label_text = g_strdup_printf("Row %d", i);
                widget_to_add = gtk_label_new(label_text);
                g_free(label_text);

                gtk_widget_set_size_request(widget_to_add, 100, - 1);  // 100 is the minimum width
            }

            else if (j > 2 || i == 1) {

                widget_to_add = gtk_combo_box_text_new();

                // gtk_widget_set_hexpand(widget_to_add, TRUE);
                // gtk_widget_set_vexpand(widget_to_add, TRUE);

                gtk_widget_set_size_request(widget_to_add, 60, 40);

                gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget_to_add), "Option 1");
                gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget_to_add), "Option 2");
                gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget_to_add), "Option 3");
                gtk_combo_box_set_active(GTK_COMBO_BOX(widget_to_add), 0);
            } else {
                continue;
            }

            if (i == 1) {
                set_widget_background_color(widget_to_add, "red");
            }

            gtk_grid_attach(GTK_GRID(grid), widget_to_add, j, i, 1, 1);
        }
    }
}

void set_widget_background_color(GtkWidget *widget, const gchar *color) {
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gchar *css_str = g_strdup_printf("* { color: %s; border: 1px solid black;}", color);
    GtkCssProvider *provider = gtk_css_provider_new();

    gtk_css_provider_load_from_data(provider, css_str, -1, NULL);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    g_free(css_str);
    g_object_unref(provider);
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