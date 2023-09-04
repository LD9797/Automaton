
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
GtkWidget *btn_spin;
GtkWidget *grid1;
GtkBuilder	*builder;

void css_set(GtkCssProvider *, GtkWidget *);

int main(int argc, char *argv[]) {
    // Init GTK
    gtk_init(&argc, &argv);

    GtkCssProvider *cssProvider1;
    GdkDisplay *display;
    GdkScreen *screen;

    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);
    cssProvider1 = gtk_css_provider_new();

    gtk_css_provider_load_from_path(cssProvider1, "gtk.css", NULL);
    gtk_style_context_add_provider_for_screen(screen,
                                              GTK_STYLE_PROVIDER(cssProvider1),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(cssProvider1);
    //gtk_css_provider_load_from_resource(cssProvider1, "/start1/gtk.css");


    builder = gtk_builder_new_from_resource("/start1/Start.glade");
    window_start = GTK_WIDGET(gtk_builder_get_object(builder, "window_start"));


    // To finish the program when closingrun
    g_signal_connect(window_start, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_builder_connect_signals(builder, NULL);




    gtk_widget_show(window_start);
    gtk_main();

    return EXIT_SUCCESS;
}


void css_set(GtkCssProvider * cssProvider, GtkWidget *g_widget) {

    GtkStyleContext *context = gtk_widget_get_style_context(g_widget);

    gtk_style_context_add_provider (context,
                                    GTK_STYLE_PROVIDER(cssProvider),
                                    GTK_STYLE_PROVIDER_PRIORITY_USER);
}