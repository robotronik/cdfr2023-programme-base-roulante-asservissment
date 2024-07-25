#pragma once

#include <gtk/gtk.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <thread>
#include <chrono>
#include <vector>
#include <pthread.h>


void write_to_text_view(GtkTextBuffer *buffer, const gchar *message);
gboolean read_from_pipe(GIOChannel *source, GIOCondition condition, gpointer data);
void redirect_stdout_to_console(GtkTextBuffer *textbuf);