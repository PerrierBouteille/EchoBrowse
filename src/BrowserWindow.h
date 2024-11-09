//
// Created by perrier on 09/11/24.
//

#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <string>

class BrowserWindow {
public:
    static void activate(GtkApplication *app, gpointer user_data);

private:
    static void on_url_entry_activate(GtkEntry *entry, WebKitWebView *web_view);
    static void on_load_changed(WebKitWebView *web_view, WebKitLoadEvent load_event, gpointer user_data);
    static bool starts_with_http(const std::string &url);
};

#endif //BROWSERWINDOW_H