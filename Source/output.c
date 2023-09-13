#include "../Headers/output.h"
#include "../Headers/dfadriver.h"

/// GTK WIDGETS

GtkMenu *menu;
GtkEntry *entry;
GtkWidget *window;
GtkWidget *popover;
GtkBuilder *builder;
GtkButton *menu_button;
GtkTextView *text_view;
GtkButton *include_button;
GtkMenuItem *menu_item_clean;

/// CONSTANTS

const int DEFAULT_WINDOW_WIDTH = 800;
const int DEFAULT_WINDOW_HEIGHT = 600;
const int WARNING_POPOVER_INTERVAL = 3;

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

void add_text_with_color(const gchar *text, GdkRGBA color) {
    // Get the end iterator of the buffer
    GtkTextIter iter;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
    gtk_text_buffer_get_end_iter(buffer, &iter);

    // Append a newline if the buffer is not empty
    if (!gtk_text_iter_is_start(&iter))
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

void add_green_text_to_text_view(const gchar *text) {
    GdkRGBA color = {0.0, 1.0, 0.0, 1.0};
    add_text_with_color(text, color);
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

void add_text_from_input_to_text_view() {
    const gchar *text = gtk_entry_get_text(entry);
    
    if (!is_whitespace(text)) {
        GdkRGBA color = {0.0, 0.0, 1.0, 1.0};
        
        const gchar *trim_text = g_strconcat("\u2B95 Hilera ingresada: ", text, NULL);
        add_text_with_color(trim_text, color);
        gtk_widget_grab_focus(GTK_WIDGET(entry));
    } else {
        gtk_entry_set_text(entry, "");
        gtk_widget_grab_focus(GTK_WIDGET(entry));
        
        GtkWidget *message_label = gtk_label_new("Por favor ingrese una hilera no vacía o repleta de espacios");
    
        popover = gtk_popover_new(GTK_WIDGET(entry));
        gtk_container_add(GTK_CONTAINER(popover), message_label);
        gtk_widget_show_all(popover);
        g_timeout_add_seconds(WARNING_POPOVER_INTERVAL, (GSourceFunc)gtk_widget_hide, popover);
    }
    
    gtk_entry_set_text(entry, "");
}

int **global_Table;
int *global_Accept;
char *global_Simbolos;
char **global_Estados;


void call_DFA(){
    const gchar *hilera = gtk_entry_get_text(entry);
    printf( "%s\n", hilera);
    char **transiciones = my_DFA_driver(global_Table, global_Simbolos, hilera, global_Estados);
    char *ultimo_estado = transiciones[strlen(hilera)];
    char final = ultimo_estado[strlen(ultimo_estado) - 1];
    int estado_final_int = final - '0';
    printf("\nUltimo estado: %c\n", final);
    printf("Acepto 1 O Rechazo 0? %d\n", global_Accept[estado_final_int]);
}


void deploy_window_output(int **Table, int *Accept, char *Simbolos, char **Estados){
    global_Table = Table;
    global_Accept = Accept;
    global_Simbolos = Simbolos;
    global_Estados = Estados;

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "layout.glade", NULL);
    window = GTK_WIDGET(gtk_builder_get_object(builder, "screen"));
    gtk_widget_set_size_request(GTK_WIDGET(window), DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);

    // Load widgets
    popover = gtk_popover_new(NULL);
    menu = GTK_MENU(gtk_builder_get_object(builder, "menu_options"));
    entry = GTK_ENTRY(gtk_builder_get_object(builder, "string_input"));
    text_view = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "text_view"));
    menu_button = GTK_BUTTON(gtk_builder_get_object(builder, "open_menu_button"));
    include_button = GTK_BUTTON(gtk_builder_get_object(builder, "include_button"));
    menu_item_clean = GTK_MENU_ITEM(gtk_builder_get_object(builder, "menu_item_clean"));

    // Connect signals
    g_signal_connect(menu_button, "clicked", G_CALLBACK(show_menu), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);
    g_signal_connect(menu_item_clean, "activate", G_CALLBACK(clear_text_view), NULL);
    g_signal_connect(entry, "activate", G_CALLBACK(add_text_from_input_to_text_view), NULL);
    g_signal_connect(include_button, "clicked", G_CALLBACK(call_DFA), NULL);

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
