//
// Created by perrier on 09/11/24.
//

#include "History.h"
#include <fstream>

/**
 * @brief Saves the given URL to the history file
 * @param url the URL to be saved
 *
 * Opens the file `history.txt` in append mode and writes the given URL to it.
 * If the file is successfully opened, the URL is written to it followed by a
 * newline character. The file is then closed.
 */
void History::save_to_history(const gchar *url) {
    std::ofstream history_file("history.txt", std::ios::app);
    if (history_file.is_open()) {
        history_file << url << std::endl;
        history_file.close();
    }
}