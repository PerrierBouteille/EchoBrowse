//
// Created by perrier on 09/11/24.
//
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include "BrowserWindow.h"

/**
 * main:
 * @argc: The number of command line arguments.
 * @argv: An array of command line arguments.
 *
 * Initializes a GTK application and runs it.
 *
 * Returns: The status of the application.
 */
int main(int argc, char **argv) {
    // Initialize GTK application
    GtkApplication *app = gtk_application_new("fr.perrier.EchoBrowse", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(BrowserWindow::activate), NULL);

    // Run the GTK application
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
