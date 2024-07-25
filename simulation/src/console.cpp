#include "console.h"

// Fonction pour écrire le texte dans le GtkTextView
void write_to_text_view(GtkTextBuffer *buffer, const gchar *message) {
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer, &end);
    gtk_text_buffer_insert(buffer, &end, message, -1);
}

gboolean read_from_pipe(GIOChannel *source, GIOCondition condition, gpointer data) {
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER((GtkTextBuffer *)data);
    //char buf[256];
    gchar message[256];
    gsize bytes_read;
    GError *error = NULL;

    // Read data from the pipe
    GIOStatus status = g_io_channel_read_chars(source, message, 256, &bytes_read, &error);
    if (status == G_IO_STATUS_NORMAL) {
        message[bytes_read] = '\0';
        //append_line_to_console(console, buf);
        write_to_text_view(buffer, message);
    } else if (status == G_IO_STATUS_ERROR) {
        g_printerr("Error reading from pipe: %s\n", error->message);
        g_error_free(error);
    }
    return TRUE;
}

void redirect_stdout_to_console(GtkTextBuffer *textbuf) {
    // Create a pipe
    int pipe_fd2[2];
    if (pipe(pipe_fd2) == -1) {
        perror("pipe");
        return;
    }

    // Set the pipe to non-blocking mode
    int flags = fcntl(pipe_fd2[0], F_GETFL, 0);
    fcntl(pipe_fd2[0], F_SETFL, flags | O_NONBLOCK);

    // Redirect stdout to the write end of the pipe
    dup2(pipe_fd2[1], STDOUT_FILENO);

    // Create a GIOChannel to read from the pipe
    GIOChannel *channel = g_io_channel_unix_new(pipe_fd2[0]);
    g_io_add_watch(channel, G_IO_IN, (GIOFunc)read_from_pipe, textbuf);
    g_io_channel_unref(channel);
}