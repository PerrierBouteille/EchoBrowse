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

    // Create navigation buttons
    GtkWidget *back_button = gtk_button_new_with_label("Back");
    GtkWidget *forward_button = gtk_button_new_with_label("Forward");

    // URL entry box
    GtkWidget *url_entry = gtk_entry_new();

    load_homepage(web_view);

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

    apply_css();

    gtk_widget_show_all(window);
}

/**
 * @brief Load the URL in the WebView when the user hits enter in the URL entry box
 * @param entry the GtkEntry containing the URL
 * @param web_view the WebView to load the URL in
 */
void BrowserWindow::on_url_entry_activate(GtkEntry *entry, WebKitWebView *web_view) {
    const gchar *entry_text = gtk_entry_get_text(entry);

    if(entry_text == NULL || *entry_text == '\0') {
        load_homepage(web_view);
        gtk_entry_set_text(entry,"");
    } else {
        std::string entry_string = std::string(entry_text);
        if(is_url(entry_string)) {
            if (!starts_with_http(entry_string)) {
                entry_string = "http://" + entry_string;
                gtk_entry_set_text(entry, entry_string.c_str());
            }
            webkit_web_view_load_uri(web_view, entry_string.c_str());
        } else {
            std::string search_url = "https://www.google.com/search?q=" + entry_string;
            webkit_web_view_load_uri(web_view,search_url.c_str());
        }
    }
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
        GtkWidget *url_entry = GTK_WIDGET(user_data);

        if(g_strcmp0(current_url,"about:blank") == 0) {
            gtk_entry_set_text(GTK_ENTRY(url_entry),"");
        }else {
            gtk_entry_set_text(GTK_ENTRY(url_entry), current_url);
            History::save_to_history(current_url);
        }
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

/**
 * @brief Checks if the given string looks like a URL.
 *
 * This function uses a simple regular expression to check if the given string
 * looks like a URL. The regular expression matches strings that start with
 * "http://" or "https://", followed by at least one alphanumeric character,
 * followed by a dot (.), followed by at least two more alphanumeric characters.
 * The regular expression is case-insensitive.
 *
 * @param text The string to check.
 * @return true if the string looks like a URL, false otherwise.
 */
bool BrowserWindow::is_url(const std::string& text) {
    // Simple regex to check if the text looks like a URL
    std::regex url_regex(R"((http:\/\/|https:\/\/)?[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}.*)");
    return std::regex_match(text, url_regex);
}

/**
 * load_homepage:
 * @web_view: The WebKitWebView to load the content into.
 *
 * Loads the content of ressources/pages/EchoBrowse.html into the given WebKitWebView.
 *
 * This is the homepage of the browser. It is loaded when the browser starts.
 */
void BrowserWindow::load_homepage(WebKitWebView *web_view) {
    std::ifstream file("../src/ressources/pages/EchoBrowse.html");
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    webkit_web_view_load_html(web_view, content.c_str(), NULL);
}

/**
 * @brief Applies the global CSS styles to the browser window.
 *
 * This function creates a new GtkCssProvider and attempts to load
 * CSS styles from the file located at "../src/ressources/css/global.css".
 * If the CSS file is successfully loaded, the styles are added to the
 * default screen's style context with a user-level priority.
 * If the CSS file fails to load, a warning is logged.
 *
 * The GtkCssProvider is unreferenced after applying the styles.
 */
void BrowserWindow::apply_css() {
    GtkCssProvider *css_provider = gtk_css_provider_new();

    if(!gtk_css_provider_load_from_path(css_provider,"../src/ressources/css/global.css", NULL)) {
        g_warning("Failed to load CSS file");
    }

    gtk_style_context_add_provider_for_screen(
            gdk_screen_get_default(),
            GTK_STYLE_PROVIDER(css_provider),
            GTK_STYLE_PROVIDER_PRIORITY_USER
    );

    g_object_unref(css_provider);
}
