//
// Created by perrier on 09/11/24.
//

#include "BrowserWindow.h"
#include "Navigation.h"
#include "History.h"

/**
 * @brief Create a new BrowserWindow and set it up with a WebView and navigation buttons
 * @param app the GtkApplication to use
 * @param user_data unused
 */
void BrowserWindow::activate(GtkApplication *app, gpointer user_data) {
    // Create the main window
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "EchoBrowse");
    gtk_window_set_default_size(GTK_WINDOW(window), 1024, 768);

    // Create grid layout
    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Create WebView
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(webkit_web_view_new());
    webkit_web_view_load_uri(web_view, "https://www.google.com");

    // Create navigation buttons
    GtkWidget *back_button = gtk_button_new_with_label("Back");
    GtkWidget *forward_button = gtk_button_new_with_label("Forward");

    // URL entry box
    GtkWidget *url_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(url_entry), "https://www.google.com");

    // Connect signals for navigation and URL entry
    g_signal_connect(back_button, "clicked", G_CALLBACK(Navigation::on_back_button_clicked), web_view);
    g_signal_connect(forward_button, "clicked", G_CALLBACK(Navigation::on_forward_button_clicked), web_view);
    g_signal_connect(url_entry, "activate", G_CALLBACK(on_url_entry_activate), web_view);
    g_signal_connect(web_view, "load-changed", G_CALLBACK(BrowserWindow::on_load_changed), url_entry);


    gtk_widget_set_hexpand(grid, TRUE);
    gtk_widget_set_vexpand(grid, TRUE);
    gtk_widget_set_hexpand(GTK_WIDGET(web_view), TRUE);
    gtk_widget_set_vexpand(GTK_WIDGET(web_view), TRUE);


    // Add widgets to the grid layout
    gtk_grid_attach(GTK_GRID(grid), back_button, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), forward_button, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), url_entry, 2, 0, 8, 1);
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(web_view), 0, 1, 10, 1);

    gtk_window_set_default_size(GTK_WINDOW(window), 1024, 768);

    gtk_widget_show_all(window);
}

/**
 * @brief Load the URL in the WebView when the user hits enter in the URL entry box
 * @param entry the GtkEntry containing the URL
 * @param web_view the WebView to load the URL in
 */
void BrowserWindow::on_url_entry_activate(GtkEntry *entry, WebKitWebView *web_view) {
    const gchar *url = gtk_entry_get_text(entry);

    std::string url_str = std::string(url);
    if (!starts_with_http(url_str)) {
        url_str = "http://" + url_str;
        gtk_entry_set_text(entry, url_str.c_str());
        //url = url_with_protocol.c_str();
    }
    webkit_web_view_load_uri(web_view, url_str.c_str());
}

/**
 * @brief Callback for when the WebView finishes loading a page
 * @param web_view the WebView that has finished loading
 * @param load_event the type of load event
 * @param user_data the URL entry widget to update with the new URL
 *
 * If the load event is WEBKIT_LOAD_FINISHED, update the URL entry
 * with the current URL of the WebView.
 */
void BrowserWindow::on_load_changed(WebKitWebView *web_view, WebKitLoadEvent load_event, gpointer user_data) {
    if (load_event == WEBKIT_LOAD_FINISHED) {
        const gchar *current_url = webkit_web_view_get_uri(web_view);
        GtkWidget *url_entry = GTK_WIDGET(user_data);  // user_data points to the URL entry widget
        gtk_entry_set_text(GTK_ENTRY(url_entry), current_url);

        History::save_to_history(current_url);
    }
}

/**
 * @brief Checks if the given URL starts with "http://" or "https://".
 *
 * @param url The URL to check.
 * @return true if the URL starts with "http://" or "https://", false otherwise.
 */
bool BrowserWindow::starts_with_http(const std::string &url) {
    if (url.compare(0, 7, "http://") == 0) {
        return true;
    }
    if (url.compare(0, 8, "https://") == 0) {
        return true;
    }
    return false;
}
