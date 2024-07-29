#pragma once

#include <gtk/gtk.h>
#include <glib.h>
#include <thread>
#include <chrono>
#include <atomic>
#include <string>
#include "commands.h"


struct TextBufferData {
    char* text;
    GtkTextBuffer* buffer;
};

struct ThreadDataTextInfo {
    GtkTextBuffer* buffer;
    std::atomic<bool>* stop_thread;
};

GThread*  intiThreadStatusTextView (GtkWidget * view, std::atomic<bool>* stop_thread);
gpointer thread_func(gpointer data);
gboolean update_text(gpointer data);
std::string generateText(void);