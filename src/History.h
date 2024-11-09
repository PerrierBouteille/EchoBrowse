//
// Created by perrier on 09/11/24.
//

#ifndef HISTORY_H
#define HISTORY_H

#include <webkit2/webkit2.h>

class History {
public:
    static void save_to_history(const gchar *url);
};


#endif //HISTORY_H
