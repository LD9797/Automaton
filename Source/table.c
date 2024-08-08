#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "../Headers/table.h"
#include "../Headers/output.h"
#include "../Headers/util.h"

/// GTK WIDGETS

GtkWidget *window_table;
GtkWidget *grid;
GtkWidget *start_window = NULL;
GObject *scrolled_window;
GtkComboBoxText ***combo_boxes_array;
GtkEntry **entry_simbolos_array;
GtkEntry **entry_estados_array;
GtkCheckButton **check_button_array;
GtkWidget *info_table;

static int global_states;
static int global_symbols;

void create_table(int rows, int cols);
void set_widget_background_color(GtkWidget *widget, const gchar *color);
void set_border(GtkWidget *widget);
void set_css(GtkWidget *widget, const gchar *css);
void free_global_combo_boxes_array(int rows);

void deploy_window_table(int num_states, int num_symbols, GtkWidget *previous_window) {
  start_window = previous_window;
  GtkBuilder *builder;
  char *glade_file_path = g_strdup_printf("%s/Table.glade", get_executable_path());
  builder = gtk_builder_new_from_file(glade_file_path);
  window_table = GTK_WIDGET(gtk_builder_get_object(builder, "window_table"));

  g_signal_connect(window_table, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  gtk_builder_connect_signals(builder, NULL);

  info_table = GTK_WIDGET(gtk_builder_get_object(builder, "info_table"));

  global_states = num_states;
  global_symbols = num_symbols;

  scrolled_window = gtk_builder_get_object(builder, "Scrolled1");
  grid = gtk_grid_new();
  create_table(num_states + 1, num_symbols + 1);
  gtk_widget_show_all(grid);

  gtk_container_add(GTK_CONTAINER(scrolled_window), grid);
  gtk_widget_show(window_table);
}

void create_table(int rows, int cols) {

  combo_boxes_array = (GtkComboBoxText ***) malloc(rows * sizeof(GtkComboBox **));
  entry_simbolos_array = (GtkEntry **) malloc(cols * sizeof(GtkEntry *));
  entry_estados_array = (GtkEntry **) malloc(rows * sizeof(GtkEntry *));
  check_button_array = (GtkCheckButton **) malloc(rows * sizeof(GtkEntry *));

  for (int i = 0; i < rows; ++i) {
    combo_boxes_array[i] = (GtkComboBoxText **) malloc(cols * sizeof(GtkComboBox *));
    for (int j = 0; j < cols + 2; ++j) {

      GtkWidget *widget_to_add;

      // Label to display "Editar Estados"
      if (i == 0 && j == 1) {
        gchar *label_text = g_strdup_printf("Editar Estados");
        widget_to_add = gtk_label_new(label_text);
        g_free(label_text);
      }

        // Label to display  "Simbolos / Estados"
      else if (i == 0 && j == 2) {
        gchar *label_text = g_strdup_printf("Símbolos /\n Estados");
        widget_to_add = gtk_label_new(label_text);
        g_free(label_text);
      }

        // Entries for the symbols
      else if (i == 0 && j >= 3) {
        const char *alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        widget_to_add = gtk_entry_new();
        gtk_entry_set_width_chars(GTK_ENTRY(widget_to_add), 1);
        gtk_entry_set_max_length(GTK_ENTRY(widget_to_add), 1);
        gtk_entry_set_width_chars(GTK_ENTRY(widget_to_add), 1);
        gtk_entry_set_alignment(GTK_ENTRY(widget_to_add), 0.5); // Alinear Centro
        char default_text[] = {alphabet[j - 3], '\0'};
        gtk_entry_set_text(GTK_ENTRY(widget_to_add), default_text);
        entry_simbolos_array[j - 3] = GTK_ENTRY(widget_to_add);
      }

        // Check Radio Buttons for acceptance states
      else if (j == 0 && i != 0) {
        widget_to_add = gtk_check_button_new_with_label("");
        check_button_array[i - 1] = GTK_CHECK_BUTTON(widget_to_add);
      }

        // Entries to change the states names
      else if (j == 1 && i != 0) {
        widget_to_add = gtk_entry_new();
        gtk_entry_set_width_chars(GTK_ENTRY(widget_to_add), 12);
        gtk_entry_set_width_chars(GTK_ENTRY(widget_to_add), 12);
        gtk_entry_set_max_length(GTK_ENTRY(widget_to_add), 12);
        gtk_entry_set_width_chars(GTK_ENTRY(widget_to_add), 12);
        gtk_entry_set_alignment(GTK_ENTRY(widget_to_add), 0.5); // Alinear Centro
        entry_estados_array[i - 1] = GTK_ENTRY(widget_to_add);
      }

        // Labels for the states names
      else if (j == 2 && i != 0) {
        gchar *label_text = g_strdup_printf("%d", i - 1);
        widget_to_add = gtk_label_new(label_text);
        g_free(label_text);
        gtk_widget_set_size_request(widget_to_add, 100, -1);
      }

        // Matrix of Combo Boxes to indicate transitions
      else if (j > 2 || i == 1) {

        widget_to_add = gtk_combo_box_text_new();
        gtk_widget_set_size_request(widget_to_add, 100, 40);

        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget_to_add), "-");
        for (int x = 0; x < rows - 1; ++x) {
          char *text = g_strdup_printf("%d", x);
          gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget_to_add), text);
        }

        gtk_combo_box_set_active(GTK_COMBO_BOX(widget_to_add), 0);
        combo_boxes_array[i - 1][j - 3] = GTK_COMBO_BOX_TEXT(widget_to_add);

      } else {
        continue;
      }

      // Initial state indication styling
      if ((i == 1 && j == 1) || (i == 1 && j == 2)) {
        set_widget_background_color(widget_to_add, "#4CAF50");
      }

      // Black border to the widget
      if (j >= 1) {
        set_border(widget_to_add);
      }

      // Attaching widget to the grid
      gtk_grid_attach(GTK_GRID(grid), widget_to_add, j, i, 1, 1);
    }
  }
}

void set_border(GtkWidget *widget) {
  gchar *css_str = g_strdup_printf("* {border: 1px solid black; border-radius: 0px;}");
  set_css(widget, css_str);
  g_free(css_str);
}

void set_widget_background_color(GtkWidget *widget, const gchar *color) {
  gchar *css_str = g_strdup_printf("* { background-color: %s; color: white;}", color);
  set_css(widget, css_str);
  g_free(css_str);
}

void set_css(GtkWidget *widget, const gchar *css_str) {
  GtkStyleContext *context = gtk_widget_get_style_context(widget);
  GtkCssProvider *provider = gtk_css_provider_new();
  gtk_css_provider_load_from_data(provider, css_str, -1, NULL);
  gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
  g_object_unref(provider);
}

void on_btn_evaluar_clicked(GtkWidget *button) {

  // Extracting transitions from the combo boxes
  int **Table;
  Table = malloc(global_states * sizeof(int *));
  for (int i = 0; i < global_states; ++i) {
    Table[i] = malloc(global_symbols * sizeof(int));
    for (int j = 0; j < global_symbols; ++j) {
      gchar *text = gtk_combo_box_text_get_active_text(combo_boxes_array[i][j]);
      int number;
      if (g_strcmp0(text, "-") == 0) {
        number = -1;
      } else {
        number = atoi(text);
      }
      Table[i][j] = number;
    }
  }

  // One hot vector int array for acceptance states
  int *Accept;
  Accept = malloc(global_states * sizeof(int));
  for (int i = 0; i < global_states; ++i) {
    if (gtk_toggle_button_get_active((GtkToggleButton *) check_button_array[i])) {
      Accept[i] = 1;
    } else {
      Accept[i] = 0;
    }
  }

  // Char array containing the symbols
  char *Symbols;
  Symbols = malloc(global_symbols * sizeof(char *));
  for (int i = 0; i < global_symbols; ++i) {
    const gchar *symbol = gtk_entry_get_text(entry_simbolos_array[i]);
    Symbols[i] = symbol[0];
  }

  // String array for the names of the states
  // If no name was given, its number will be used.
  char **States;
  States = malloc(global_states * sizeof(char *));
  for (int i = 0; i < global_states; ++i) {
    const gchar *temp_text = gtk_entry_get_text(entry_estados_array[i]);
    if (strlen(temp_text) == 0) {
      char index_str[3];
      sprintf(index_str, "%d", i);
      States[i] = strdup(index_str);
    } else {
      char *str_name = malloc((strlen(temp_text) + 2) * sizeof(char*));
      sprintf(str_name, "\"%s\"", temp_text);
      States[i] = strdup(str_name);
    }
  }

  // Open the output window
  deploy_window_output(Table, Accept, Symbols, States, window_table, global_symbols, global_states);

  // Freeze the current window
  if (gtk_widget_is_toplevel(window_table)) {
    gtk_widget_set_sensitive(window_table, FALSE);
  }
}

void free_global_combo_boxes_array(int rows) {
  for (int i = 0; i < rows; ++i) {
    free(combo_boxes_array[i]);
  }
  free(combo_boxes_array);
}

void on_btn_return_clicked(GtkWidget *button) {
  if (gtk_widget_is_toplevel(window_table)) {
    gtk_widget_hide(window_table);
  }
  free_global_combo_boxes_array(global_states);
  // Show the original window
  if (start_window) {
    gtk_widget_show(start_window);
  }
}

void on_btn_exit_table_clicked(GtkWidget *button) {
  if (window_table != NULL) {
    gtk_main_quit();
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
