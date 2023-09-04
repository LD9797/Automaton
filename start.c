
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

int main(int argc, char *argv[]) {
    // Init GTK
    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_file ("Start.glade");
    window_start = GTK_WIDGET(gtk_builder_get_object(builder, "window_start"));

    // To finish the program
    g_signal_connect(window_start, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_builder_connect_signals(builder, NULL);

    gtk_widget_show(window_start);
    gtk_main();

    return EXIT_SUCCESS;
}
