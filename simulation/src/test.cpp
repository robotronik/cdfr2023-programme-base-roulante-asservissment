// #include <gtk/gtk.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <fcntl.h>

// // Fonction pour écrire le texte dans le GtkTextView
// void write_to_text_view(GtkTextBuffer *buffer, const gchar *message) {
//     GtkTextIter end;
//     gtk_text_buffer_get_end_iter(buffer, &end);
//     gtk_text_buffer_insert(buffer, &end, message, -1);
// }

// // Callback pour le pipe
// gboolean read_stdout(GIOChannel *channel, GIOCondition condition, gpointer data) {
//     GtkTextBuffer *buffer = GTK_TEXT_BUFFER(data);
//     gchar *message;
//     gsize length;
//     GIOStatus status;

//     status = g_io_channel_read_line(channel, &message, &length, NULL, NULL);
//     if (status == G_IO_STATUS_NORMAL) {
//         write_to_text_view(buffer, message);
//         g_free(message);
//     } else if (status == G_IO_STATUS_ERROR || status == G_IO_STATUS_EOF) {
//         return FALSE;
//     }

//     return TRUE;
// }

// int main(int argc, char *argv[]) {
//     GtkWidget *window;
//     GtkWidget *scrolled_window;
//     GtkWidget *text_view;
//     GtkTextBuffer *buffer;
//     int pipe_fds[2];
//     GIOChannel *channel;

//     gtk_init(&argc, &argv);

//     // Créer la fenêtre principale
//     window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//     gtk_window_set_title(GTK_WINDOW(window), "Redirection stdout vers GTK");
//     gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

//     // Créer un GtkTextView avec un GtkScrolledWindow
//     scrolled_window = gtk_scrolled_window_new(NULL, NULL);
//     gtk_container_add(GTK_CONTAINER(window), scrolled_window);

//     text_view = gtk_text_view_new();
//     gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);
//     gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);

//     buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

//     // Rediriger stdout vers un pipe
//     pipe(pipe_fds);
//     if (fork() == 0) {
//         // Enfant
//         close(pipe_fds[0]);
//         dup2(pipe_fds[1], STDOUT_FILENO);
//         close(pipe_fds[1]);

//         // Exemple de texte imprimé
//         printf("Hello, world!\n");
//         printf("This is redirected output.\n");
//         // Example lines to add to the console
//         for (int i = 0; i < 500; i++) {
//             printf("Line %d: Hello, world!\n", i + 1);
//         }

//         exit(0);
//     } else {
//         // Parent
//         close(pipe_fds[1]);
//         channel = g_io_channel_unix_new(pipe_fds[0]);
//         g_io_add_watch(channel, G_IO_IN, (GIOFunc)read_stdout, buffer);
//     }

//     g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

//     gtk_widget_show_all(window);

//     gtk_main();

//     return 0;
// }
