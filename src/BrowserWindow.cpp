//
// Created by perrier on 09/11/24.
//

#include "BrowserWindow.h"
#include "Navigation.h"
#include "History.h"

static GtkWidget *notebook;

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

    // Create WebView
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(webkit_web_view_new());

    // Create notebook to manage tabs
    notebook = gtk_notebook_new();
    gtk_container_add(GTK_CONTAINER(window), notebook);

    // Create navigation buttons and header
    GtkWidget *back_button = gtk_button_new_with_label("Back");
    GtkWidget *forward_button = gtk_button_new_with_label("Forward");
    GtkWidget *new_tab_button = gtk_button_new_with_label("New Tab");
    GtkWidget *close_tab_button = gtk_button_new_with_label("Close Tab");

    // Pack buttons into a header
    GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(header), new_tab_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(header), close_tab_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(header), back_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(header), forward_button, FALSE, FALSE, 0);

    // Set up main container
    GtkWidget *main_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(main_container), header, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_container), notebook, TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(window), main_container);

    // Connect signals for tab management
    g_signal_connect(new_tab_button, "clicked", G_CALLBACK(&BrowserWindow::on_new_tab), web_view);
    g_signal_connect(close_tab_button, "clicked", G_CALLBACK(&BrowserWindow::on_close_tab), web_view);

    // Initialize with one tab
    on_new_tab();

    apply_css();  // Apply CSS styling (if defined)

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

/**
 * @brief Creates a new tab and adds it to the notebook.
 *
 * This function creates a new GtkWidget containing a WebKitWebView and a
 * GtkEntry for the URL bar. The tab is added to the default notebook with a
 * label that says "New Tab".
 *
 * The function also connects the following signals to the new tab:
 *   - The "activate" signal of the URL entry is connected to
 *     BrowserWindow::on_url_entry_activate.
 *   - The "notify::title" signal of the WebKitWebView is connected to
 *     BrowserWindow::on_tab_title_changed.
 */
void BrowserWindow::on_new_tab() {
    // Create a new vertical box for the tab's content
    GtkWidget *tab_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    // Create a new URL entry
    GtkWidget *url_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(tab_container), url_entry, FALSE, FALSE, 0);

    // Create navigation buttons for each tab
    GtkWidget *back_button = gtk_button_new_with_label("Back");
    GtkWidget *forward_button = gtk_button_new_with_label("Forward");
    GtkWidget *new_tab_button = gtk_button_new_with_label("New Tab");
    GtkWidget *close_tab_button = gtk_button_new_with_label("Close Tab");

    // Add buttons to a navigation bar within the tab
    GtkWidget *nav_bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(nav_bar), back_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(nav_bar), forward_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(nav_bar), new_tab_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(nav_bar), close_tab_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(nav_bar), url_entry, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(tab_container), nav_bar, FALSE, FALSE, 0);

    // Create a new WebKitWebView for the tab
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(webkit_web_view_new());
    gtk_box_pack_start(GTK_BOX(tab_container), GTK_WIDGET(web_view), TRUE, TRUE, 0);

    // Load the homepage
    load_homepage(web_view);

    // Create a label for the tab and add to notebook
    GtkWidget *tab_label = gtk_label_new("New Tab");
    int page_num = gtk_notebook_append_page(GTK_NOTEBOOK(notebook), tab_container, tab_label);

    // Show all widgets in the new tab
    gtk_widget_show_all(tab_container);

    // Connect URL entry and navigation signals for the new tab
    g_signal_connect(url_entry, "activate", G_CALLBACK(on_url_entry_activate), web_view);
    g_signal_connect(back_button, "clicked", G_CALLBACK(Navigation::on_back_button_clicked), web_view);
    g_signal_connect(forward_button, "clicked", G_CALLBACK(Navigation::on_forward_button_clicked), web_view);
    g_signal_connect(new_tab_button,"clicked", G_CALLBACK(BrowserWindow::on_new_tab),web_view);
    g_signal_connect(close_tab_button,"clicked", G_CALLBACK(BrowserWindow::on_close_tab),web_view);
    g_signal_connect(web_view, "notify::title", G_CALLBACK(on_tab_title_changed), tab_label);
}



/**
 * @brief Updates the label of the tab with the title of the WebView
 * @param web_view the WebView that has changed title
 * @param pspec unused
 * @param data the GtkLabel that should be updated with the new title
 *
 * When the title of the WebView changes, this function is called to update the
 * label of the tab with the new title. If the title is NULL, the label is set
 * to "New Tab".
 */
void BrowserWindow::on_tab_title_changed(WebKitWebView *web_view, GParamSpec *pspec, gpointer data) {
    GtkLabel *tab_label = GTK_LABEL(data);
    const gchar *title = webkit_web_view_get_title(web_view);
    gtk_label_set_text(tab_label, title ? title : "New Tab");
}


/**
 * @brief Handles the switching of tabs in the notebook.
 * @param notebook the GtkNotebook containing the tabs
 * @param page the GtkWidget representing the new active page
 * @param page_num the index of the new active page
 * @param user_data user data associated with the callback (unused)
 *
 * This function updates the URL entry with the current URL of the WebView
 * in the newly selected tab. If the URL is NULL, the entry is cleared.
 */
void BrowserWindow::on_tab_switch(GtkNotebook *notebook, GtkWidget *page, guint page_num, gpointer user_data) {
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(gtk_bin_get_child(GTK_BIN(page)));
    const gchar *url = webkit_web_view_get_uri(web_view);
    GtkWidget *url_entry = GTK_WIDGET(user_data);
    gtk_entry_set_text(GTK_ENTRY(url_entry), url ? url : "");
}


/**
 * @brief Closes the currently selected tab in the notebook
 *
 * This function is called when the user wants to close the currently
 * selected tab in the notebook. It removes the tab at the current page
 * number from the notebook.
 */
void BrowserWindow::on_close_tab() {
    int page_num = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    if (page_num != -1) {
        gtk_notebook_remove_page(GTK_NOTEBOOK(notebook), page_num);
    }

    // If no tabs are left, open a new tab or close the browser
    if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook)) == 0) {
        on_new_tab();
    }
}

