#include "../Headers/output.h"
#include "../Headers/dfadriver.h"
#include "../Headers/latexdriver.h"
#include "../Headers/transformer.h"

/// DFA MODEL

int n_states;
int n_symbols;
int **global_table;
int *global_accept;
static char *global_symbols;
static char **global_states;

/// GTK WIDGETS

GtkMenu *menu;
GtkEntry *entry;
GtkWidget *window;
GtkBuilder *builder;
GtkButton *menu_button;
GtkButton *back_button;
GtkTextView *text_view;
GtkButton *include_button;
GtkMenuItem *menu_item_clean;
GtkWidget *table_window = NULL;
GtkCheckMenuItem *check_collapse;
GtkButton *btn_exit_layout;
GtkButton *btn_print_latex;
/// CONSTANTS

const int DEFAULT_WINDOW_WIDTH = 800;
const int DEFAULT_WINDOW_HEIGHT = 600;

/// PUBLIC METHODS

void on_window_destroy(GtkWidget *widget, gpointer user_data) {
  gtk_main_quit();
}

void show_menu(GtkButton *button) {
  gtk_menu_popup_at_widget(GTK_MENU(menu), GTK_WIDGET(button), GDK_GRAVITY_SOUTH, GDK_GRAVITY_NORTH, NULL);
}

void clear_text_view() {
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
  gtk_text_buffer_set_text(buffer, "", -1);
  gtk_entry_set_text(entry, "");
  gtk_widget_grab_focus(GTK_WIDGET(entry));
}

void add_text_with_color(const gchar *text, GdkRGBA color, bool new_line) {
  // Get the end iterator of the buffer
  GtkTextIter iter;
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
  gtk_text_buffer_get_end_iter(buffer, &iter);

  // Append a newline if the buffer is not empty
  if (!gtk_text_iter_is_start(&iter) && new_line)
    gtk_text_buffer_insert(buffer, &iter, "\n", -1);

  // Create a new text tag for the Consolas font, color and font size.
  PangoFontDescription *font_desc = pango_font_description_new();
  pango_font_description_set_family(font_desc, "Consolas");
  pango_font_description_set_size(font_desc, 13 * PANGO_SCALE);

  GtkTextTag *tag = gtk_text_buffer_create_tag(buffer, NULL, "foreground-rgba", &color, "font-desc", font_desc, NULL);
  gtk_text_buffer_insert_with_tags(buffer, &iter, text, -1, tag, NULL);

  // Scroll to the end of the buffer
  gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(text_view), gtk_text_buffer_get_insert(buffer), 0.0, FALSE, 0.0, 1.0);
}

void add_black_text_to_text_view(const gchar *text, bool newline) {
  GdkRGBA color = {0.0, 0.0, 0.0, 1.0};
  add_text_with_color(text, color, newline);
  gtk_entry_set_text(entry, "");
  gtk_widget_grab_focus(GTK_WIDGET(entry));
}

void add_blue_text_to_text_view(const gchar *text, bool newline) {
  GdkRGBA color = {0.0, 0.0, 1.0, 1.0};
  add_text_with_color(text, color, newline);
  gtk_entry_set_text(entry, "");
  gtk_widget_grab_focus(GTK_WIDGET(entry));
}

void add_green_text_to_text_view(const gchar *text, bool newline) {
  GdkRGBA color = {0.0, 1.0, 0.0, 1.0};
  add_text_with_color(text, color, newline);
  gtk_entry_set_text(entry, "");
  gtk_widget_grab_focus(GTK_WIDGET(entry));
}

void add_red_text_to_text_view(const gchar *text, bool newline) {
  GdkRGBA color = {1.0, 0.0, 0.0, 1.0};
  add_text_with_color(text, color, newline);
  gtk_entry_set_text(entry, "");
  gtk_widget_grab_focus(GTK_WIDGET(entry));
}

bool is_whitespace(const gchar *text) {
  while (*text) {
    if (!g_ascii_isspace(*text)) {
      return false;
    }
    text++;
  }
  return true;
}

void add_text_from_input_to_text_view(const gchar *text) {
  const gchar *new_text = g_strconcat("\U0001F4DD Hilera ingresada: ", text, NULL);
  add_black_text_to_text_view(new_text, true);
  gtk_widget_grab_focus(GTK_WIDGET(entry));

  gtk_entry_set_text(entry, "");
}

void call_DFA() {
  const gchar *in_string = g_strdup(gtk_entry_get_text(entry));
  add_text_from_input_to_text_view(in_string);

    gboolean collapse_results = gtk_check_menu_item_get_active(check_collapse);
    init_dfa_driver(global_table, global_accept, global_states, global_symbols, n_symbols, n_states);
    struct Registry registry[strlen(in_string) + 1];
    int registryCount = 0;
    int result = dfa_driver(in_string, registry, &registryCount);

    add_blue_text_to_text_view("\n\xf0\x9f\x9a\xa7 Procesado de la hilera:", false);
    if (registryCount > 0) {
      if (result == INVALID_CHARACTER) {
        char string[100];
        sprintf(string,
                " \u2B95 FINAL: Símbolo inválido encontrado durante el preprocesado de la hilera: %c",
                registry[0].symbol);
        add_blue_text_to_text_view((const gchar *) string, !collapse_results);
      } else if (is_whitespace(in_string)) {
        char string[100];
        sprintf(string,
                " \u2B95 INICIO: Estado #%d (%s), Símbolo a procesar: %c",
                registry[0].state,
                global_states[registry[0].state],
                ' ');
        add_blue_text_to_text_view((const gchar *) string, !collapse_results);
        sprintf(string,
                " \u2B95 FINAL: Estado #%d (%s)",
                registry[0].state,
                global_states[registry[0].state]);
        add_blue_text_to_text_view((const gchar *) string, !collapse_results);
      } else {
        for (int i = 0; i < registryCount; i++) {
          char string[100];

          if (i == 0) {
            sprintf(string,
                    " \u2B95 INICIO: Estado #%d (%s), Símbolo a procesar: %c",
                    registry[i].state,
                    global_states[registry[i].state],
                    registry[i + 1].symbol);
          } else if (registry[i].state == FINAL_CHARACTER) {
            sprintf(string,
                    " \u2B95 FINAL: Estado #%d (%s)",
                    registry[i - 1].state,
                    global_states[registry[i - 1].state]);
          } else {
            if (i + 1 >= registryCount) {
              sprintf(string, " \u2B95 FINAL: Estado #%d (%s)", registry[i].state, global_states[registry[i].state]);
            } else {
              sprintf(string,
                      " \u2B95 PASO #%d: Estado #%d (%s), Símbolo a procesar: %c",
                      registry[i].step,
                      registry[i].state,
                      global_states[registry[i].state],
                      registry[i + 1].symbol);
            }
          }

          add_blue_text_to_text_view((const gchar *) string, !collapse_results);

        }

      }

    } else {
      add_blue_text_to_text_view(" No hay registros de cambios de estado.", !collapse_results);
    }

    if (result == 1) {
      add_green_text_to_text_view("\U00002705 La hilera se acepta.\n", true);
    } else {
      add_red_text_to_text_view("\U0000274C La hilera se rechaza.\n", true);
    }

}

void on_back_button_clicked() {
  if (gtk_widget_is_toplevel(window)) {
    gtk_widget_hide(window);
  }

  if (table_window) {
    gtk_widget_set_sensitive(table_window, TRUE);
  }
}

void on_btn_exit_layout_clicked(){
  if(window != NULL){
    gtk_main_quit();
  }
}

int call_latex_driver(){
  char* sample_regex = dfa_to_regex(global_accept, global_symbols, global_table,  n_symbols, n_states); // "((b(b|ab)*aa|a))*";
  int ret = init_latex_driver(global_table, global_accept, global_states, global_symbols, n_symbols, n_states, sample_regex);
  return ret;
}

void on_btn_print_latex_activate(){
  int ret = call_latex_driver();
  if (ret == 0) draw_graph();
}

void deploy_window_output(int **Table,
                          int *Accept,
                          char *Symbols,
                          char **States,
                          GtkWidget *previous_window,
                          int num_symbols,
                          int num_states) {
  global_table = Table;
  global_accept = Accept;
  global_symbols = Symbols;
  global_states = States;
  table_window = previous_window;
  n_symbols = num_symbols;
  n_states = num_states;

  builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, "layout.glade", NULL);
  window = GTK_WIDGET(gtk_builder_get_object(builder, "screen"));
  gtk_widget_set_size_request(GTK_WIDGET(window), DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);

  // Load widgets
  menu = GTK_MENU(gtk_builder_get_object(builder, "menu_options"));
  entry = GTK_ENTRY(gtk_builder_get_object(builder, "string_input"));
  text_view = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "text_view"));
  back_button = GTK_BUTTON(gtk_builder_get_object(builder, "back_button"));
  menu_button = GTK_BUTTON(gtk_builder_get_object(builder, "open_menu_button"));
  include_button = GTK_BUTTON(gtk_builder_get_object(builder, "include_button"));
  menu_item_clean = GTK_MENU_ITEM(gtk_builder_get_object(builder, "menu_item_clean"));
  check_collapse = GTK_CHECK_MENU_ITEM(gtk_builder_get_object(builder, "menuitem_collapseprocess"));
  btn_exit_layout = GTK_BUTTON(gtk_builder_get_object(builder, "btn_exit_layout"));
  btn_print_latex = GTK_BUTTON(gtk_builder_get_object(builder, "btn_print_latex"));

  // Connect signals
  g_signal_connect(entry, "activate", G_CALLBACK(call_DFA), NULL);
  g_signal_connect(menu_button, "clicked", G_CALLBACK(show_menu), NULL);
  g_signal_connect(include_button, "clicked", G_CALLBACK(call_DFA), NULL);
  g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);
  g_signal_connect(menu_item_clean, "activate", G_CALLBACK(clear_text_view), NULL);
  g_signal_connect(back_button, "clicked", G_CALLBACK(on_back_button_clicked), NULL);
  g_signal_connect(btn_exit_layout, "clicked", G_CALLBACK(on_btn_exit_layout_clicked), NULL);
  g_signal_connect(btn_print_latex, "clicked", G_CALLBACK(on_btn_print_latex_activate), NULL);

  // Init buffer
  GtkTextIter iter;
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
  gtk_text_buffer_get_end_iter(buffer, &iter);
  gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(text_view), &iter, 0.0, false, 0.0, 0.0);
  gtk_text_buffer_set_text(buffer, "", -1);

  // Make the text view inaccessible to mouse selection and events
  gtk_text_view_set_editable(text_view, false);
  gtk_text_view_set_cursor_visible(text_view, false);
  gtk_widget_set_sensitive(GTK_WIDGET(text_view), false);

  gtk_widget_show(window);
}
