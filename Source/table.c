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
#include "../Headers/output.h"
#include <string.h>


GtkWidget *window_table;
GtkWidget *grid;
GtkWidget *start_window = NULL;
GObject *scrolled_window;
GtkComboBoxText ***combo_boxes_array;
GtkEntry **entry_simbolos_array;
GtkEntry **entry_estados_array;
GtkCheckButton **check_button_array;
GtkWidget *btn_info_table;
GtkWidget *info_table;

int global_estados;
int global_simbolos;

void create_table(int rows, int cols);
void set_widget_background_color(GtkWidget *widget, const gchar *color);
void set_border(GtkWidget *widget);
void set_css(GtkWidget *widget, const gchar *css);
void free_global_combo_boxes_array(int rows);

void deploy_window_table(int estados, int simbolos, GtkWidget *previous_window){
    start_window = previous_window;

    GtkBuilder *builder;
    builder = gtk_builder_new_from_file ("Table.glade");
    window_table = GTK_WIDGET(gtk_builder_get_object(builder, "window_table"));

    g_signal_connect(window_table, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_builder_connect_signals(builder, NULL);

    btn_info_table = GTK_WIDGET(gtk_builder_get_object(builder, "btn_info_estados"));
    info_table = GTK_WIDGET(gtk_builder_get_object(builder, "info_table"));

    global_estados = estados;
    global_simbolos = simbolos;

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
    combo_boxes_array = (GtkComboBoxText ***) (GtkComboBox ***) malloc(rows * sizeof(GtkComboBox **));
    entry_simbolos_array = (GtkEntry **)malloc(cols * sizeof(GtkEntry *));
    entry_estados_array = (GtkEntry **)malloc(rows * sizeof(GtkEntry *));
    check_button_array = (GtkCheckButton **)malloc(rows * sizeof(GtkEntry *));
    for (int i = 0; i < rows; ++i) {
        combo_boxes_array[i] = (GtkComboBoxText **) (GtkComboBox **) malloc(cols * sizeof(GtkComboBox *));
        for (int j = 0; j < cols + 2; ++j) {

            GtkWidget *widget_to_add;

            // Label Editar Estados;
            if (i == 0 && j == 1) {
                gchar *label_text = g_strdup_printf("Editar Estados");
                widget_to_add = gtk_label_new(label_text);
                g_free(label_text);
            }

            // Label Simbolos / Estados
            else if (i == 0 && j == 2) {
                gchar *label_text = g_strdup_printf("Símbolos /\n Estados");
                widget_to_add = gtk_label_new(label_text);
                g_free(label_text);
            }

            // Entries para símbolos
            else if (i == 0 && j >= 3) {
                const char *alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
                widget_to_add = gtk_entry_new();
                gtk_entry_set_width_chars(GTK_ENTRY(widget_to_add), 1);
                gtk_entry_set_max_length(GTK_ENTRY(widget_to_add),1);
                gtk_entry_set_width_chars(GTK_ENTRY(widget_to_add), 1);
                gtk_entry_set_alignment(GTK_ENTRY(widget_to_add), 0.5); // Alinear Centro
                char default_text[] = { alphabet[j - 3], '\0' };
                gtk_entry_set_text(GTK_ENTRY(widget_to_add), default_text);
                entry_simbolos_array[j - 3] = GTK_ENTRY(widget_to_add);
            }

            // Check Radio Buttons
            else if (j == 0 && i != 0) {
                widget_to_add = gtk_check_button_new_with_label("");
                check_button_array[i - 1] = GTK_CHECK_BUTTON(widget_to_add);
            }

            // Entries para estados
            else if (j == 1 && i != 0) {
                widget_to_add = gtk_entry_new();
                gtk_entry_set_width_chars(GTK_ENTRY(widget_to_add), 12);
                gtk_entry_set_width_chars(GTK_ENTRY(widget_to_add), 12);
                gtk_entry_set_max_length(GTK_ENTRY(widget_to_add),12);
                gtk_entry_set_width_chars(GTK_ENTRY(widget_to_add), 12);
                gtk_entry_set_alignment(GTK_ENTRY(widget_to_add), 0.5); // Alinear Centro
                entry_estados_array[i - 1] = GTK_ENTRY(widget_to_add);
            }

            // Labels de los estados
            else if (j == 2 && i != 0) {
                gchar *label_text = g_strdup_printf("%d", i - 1);
                widget_to_add = gtk_label_new(label_text);
                g_free(label_text);

                gtk_widget_set_size_request(widget_to_add, 100, - 1);  // 100 is the minimum width
            }

            // Combo boxes
            else if (j > 2 || i == 1) {

                widget_to_add = gtk_combo_box_text_new();

                // gtk_widget_set_hexpand(widget_to_add, TRUE);
                // gtk_widget_set_vexpand(widget_to_add, TRUE);

                gtk_widget_set_size_request(widget_to_add, 100, 40);

                gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget_to_add), "-");
                for(int x = 0; x < rows - 1; ++x){
                    char *text = g_strdup_printf("%d", x);
                    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget_to_add), text);
                }

                gtk_combo_box_set_active(GTK_COMBO_BOX(widget_to_add), 0);
                combo_boxes_array[i - 1][j - 3] = GTK_COMBO_BOX_TEXT(widget_to_add);

            } else {
                continue;
            }

            // Primera fila resaltada.
            if (i == 1 && j >= 1) {
                set_widget_background_color(widget_to_add, "red");
            }

            // Frames negros.
            if (j >= 1){
                set_border(widget_to_add);
            }

            gtk_grid_attach(GTK_GRID(grid), widget_to_add, j, i, 1, 1);
        }
    }
    g_strdup_printf("DONE");
}

void set_border(GtkWidget *widget){
    gchar *css_str = g_strdup_printf("* {border: 1px solid black;}");
    set_css(widget, css_str);
    g_free(css_str);
}

void set_widget_background_color(GtkWidget *widget, const gchar *color) {
    gchar *css_str = g_strdup_printf("* { color: %s;}", color);
    set_css(widget, css_str);
    g_free(css_str);
}

void set_css(GtkWidget *widget, const gchar *css_str){
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css_str, -1, NULL);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);
}

void on_btn_return_clicked(GtkWidget *button) {
    if (gtk_widget_is_toplevel(window_table)) {
        gtk_widget_hide(window_table);
    }
    free_global_combo_boxes_array(global_estados);
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

void free_global_combo_boxes_array(int rows) {
    for (int i = 0; i < rows; ++i) {
        free(combo_boxes_array[i]);
    }
    free(combo_boxes_array);
}


void on_btn_evaluar_clicked(GtkWidget *button){
    // TODO Validaciones de simbolos vacios
    int **Table;
    Table = malloc(global_estados * sizeof(int *));
    for(int i = 0; i < global_estados; ++i){
        Table[i] = malloc(global_simbolos * sizeof(int));
        for(int j = 0; j < global_simbolos; ++j){
            gchar *text = gtk_combo_box_text_get_active_text(combo_boxes_array[i][j]);
            int number;
            if (g_strcmp0(text, "-") == 0){
                number = -1;
            } else {
                number = atoi(text);
            }
            Table[i][j] = number;
        }
    }

    // Array de int, one hot vector estados de aceptacion.
    int *Accept;
    Accept = malloc(global_estados * sizeof(int));
    for(int i = 0; i < global_estados; ++i){
        if(gtk_toggle_button_get_active((GtkToggleButton *) check_button_array[i])){
            Accept[i] = 1;
        } else {
            Accept[i] = 0;
        }
    }

    // Array de char (1 solo caracter) de los simbolos.
    char *Simbolos;
    Simbolos = malloc(global_simbolos * sizeof(char*));
    for(int i = 0; i < global_simbolos; ++i){
        const gchar *simbolo = gtk_entry_get_text(entry_simbolos_array[i]);
        Simbolos[i] = simbolo[0];
    }

    // Array de strings de los nombres de los estados.
    // Si no hay un nombre para el estado se devuelve el número de estado (en string i.e. "4").
    char **Estados;
    Estados = malloc(global_estados * sizeof(char *));
    for(int i = 0; i < global_estados; ++i){
        const gchar *temp_text = gtk_entry_get_text(entry_estados_array[i]);
        if(strlen(temp_text) == 0){
            char index_str[3];
            sprintf(index_str, "%d", i);
            Estados[i] = strdup(index_str);
        } else {
            Estados[i] = strdup(temp_text);
        }
    }

    deploy_window_output(Table, Accept, Simbolos, Estados, window_table, global_simbolos, global_estados);

    if (gtk_widget_is_toplevel(window_table)) {
        gtk_widget_set_sensitive(window_table, FALSE);
        // gtk_widget_hide(window_table);
    }
}


void on_btn_info_table_clicked(GtkWidget *button) {
    if (info_table != NULL) {
        gtk_dialog_run(GTK_DIALOG(info_table));
        gtk_widget_hide(info_table);
    }
}

void on_btn_ok_clicked(GtkWidget *button) {
    if (info_table != NULL) {
        gtk_widget_hide(info_table);
    }
}