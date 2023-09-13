#include <stdlib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <stdbool.h>
#include <pango/pango.h>

#ifndef AUTOMATON_OUTPUT_H
#define AUTOMATON_OUTPUT_H

/// GTK WIDGETS

extern GtkMenu *menu;
extern GtkEntry *entry;
extern GtkWidget *window;
extern GtkWidget *popover;
extern GtkBuilder *builder;
extern GtkButton *menu_button;
extern GtkTextView *text_view;
extern GtkButton *include_button;
extern GtkMenuItem *menu_item_clean;

/// CONSTANTS

extern const int DEFAULT_WINDOW_WIDTH;
extern const int DEFAULT_WINDOW_HEIGHT;
extern const int WARNING_POPOVER_INTERVAL;

/// PUBLIC METHODS

void add_green_text_to_text_view(const gchar *text);

#endif //AUTOMATON_OUTPUT_H
