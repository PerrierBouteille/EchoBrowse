//
// Created by perrier on 09/11/24.
//

#include "Navigation.h"


/**
 * @brief Handles the "clicked" signal of the back button in the navigation toolbar
 * @param button the GtkButton that emitted the signal
 * @param web_view the WebView that should go back
 *
 * If the WebView can go back, this method will request it to do so.
 */
void Navigation::on_back_button_clicked(GtkButton *button, WebKitWebView *web_view) {
    if (webkit_web_view_can_go_back(web_view)) {
        webkit_web_view_go_back(web_view);
    }
}

/**
 * @brief Handles the "clicked" signal of the forward button in the navigation toolbar
 * @param button the GtkButton that emitted the signal
 * @param web_view the WebView that should go forward
 *
 * If the WebView can go forward, this method will request it to do so.
 */
void Navigation::on_forward_button_clicked(GtkButton *button, WebKitWebView *web_view) {
    if (webkit_web_view_can_go_forward(web_view)) {
        webkit_web_view_go_forward(web_view);
    }
}