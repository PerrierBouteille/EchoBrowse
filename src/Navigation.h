//
// Created by perrier on 09/11/24.
//

#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <webkit2/webkit2.h>
#include <gtk/gtk.h>

class Navigation {
public:
    static void on_back_button_clicked(GtkButton *button, WebKitWebView *web_view);
    static void on_forward_button_clicked(GtkButton *button, WebKitWebView *web_view);
};

#endif //NAVIGATION_H
