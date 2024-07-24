#include <gtk/gtk.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <thread>
#include <chrono>
#include <vector>
#include <pthread.h>
#include "simulation.h"

void* loop_sys_tick(void* arg) {
    while (1)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        sys_tick_handler();
    }    
}

void* stm_main_funct(void* arg) {
    stm_main();    
}

void on_resize(GtkWidget *widget, GtkImage *image) {
    GtkAllocation allocation;
    gtk_widget_get_allocation(widget, &allocation);

    GdkPixbuf *pixbuf = gtk_image_get_pixbuf(image);
    if (pixbuf) {
        int original_width = gdk_pixbuf_get_width(pixbuf);
        int original_height = gdk_pixbuf_get_height(pixbuf);
        double aspect_ratio = (double)original_width / original_height;

        int new_width = allocation.width;
        int new_height = allocation.height;

        // Ajuster les dimensions en maintenant le rapport d'aspect
        if (new_width / (double)new_height > aspect_ratio) {
            new_width = (int)(new_height * aspect_ratio);
        } else {
            new_height = (int)(new_width / aspect_ratio);
        }

        // Redimensionner le pixbuf
        GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, new_width, new_height, GDK_INTERP_BILINEAR);
        gtk_image_set_from_pixbuf(image, scaled_pixbuf);
        g_object_unref(scaled_pixbuf);
    } else {
        g_warning("Failed to get pixbuf from GtkImage");
    }
}

// Connecter le signal de redimensionnement
//g_signal_connect(image_container, "size-allocate", G_CALLBACK(on_resize), image);


#define MAX_LINES 1000
typedef struct {
    GtkWidget *text_view;
    GtkTextBuffer *text_buffer;
    GQueue *lines;
    int pipe_fd[2]; // Pipe file descriptors
} ConsoleData;

void append_line_to_console(ConsoleData *console, const char *line) {
    GtkTextIter end_iter;
    GtkTextBuffer *buffer = console->text_buffer;

    // Append line to the queue
    g_queue_push_tail(console->lines, g_strdup(line));

    // If we have more than MAX_LINES, remove the oldest one
    if (g_queue_get_length(console->lines) > MAX_LINES) {
        gchar *oldest_line = (gchar *)g_queue_pop_head(console->lines);
        g_free(oldest_line);

        // Remove the first line from the buffer
        GtkTextIter start_iter;
        gtk_text_buffer_get_start_iter(buffer, &start_iter);
        gtk_text_iter_forward_line(&start_iter);
        gtk_text_buffer_delete(buffer, &start_iter, &end_iter);
    }

    // Append the new line to the buffer
    gtk_text_buffer_get_end_iter(buffer, &end_iter);
    gtk_text_buffer_insert(buffer, &end_iter, line, -1);
    gtk_text_buffer_insert(buffer, &end_iter, "\n", -1);

    // Scroll to the end of the buffer
    GtkTextMark *mark = gtk_text_buffer_create_mark(buffer, NULL, &end_iter, FALSE);
    gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(console->text_view), mark);
    gtk_text_buffer_delete_mark(buffer, mark);
}

// Fonction pour écrire le texte dans le GtkTextView
void write_to_text_view(GtkTextBuffer *buffer, const gchar *message) {
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer, &end);
    gtk_text_buffer_insert(buffer, &end, message, -1);
}

gboolean read_from_pipe(GIOChannel *source, GIOCondition condition, gpointer data) {
    ConsoleData *console = (ConsoleData *)data;
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(console->text_buffer);
    //char buf[256];
    gchar message[256];
    gsize bytes_read;
    gsize length;
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


// Callback pour le pipe
gboolean read_stdout(GIOChannel *channel, GIOCondition condition, gpointer data) {
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(data);
    gchar *message;
    char message2[256];
    gsize length;
    GIOStatus status;

    status = g_io_channel_read_line(channel, &message, &length, NULL, NULL);
    if (status == G_IO_STATUS_NORMAL) {
        if (length > 0) {
            message[length - 1] = '\0'; 
        } else {
            message[0] = '\0';
        }
        write_to_text_view(buffer, message);
        g_free(message);
    } else if (status == G_IO_STATUS_ERROR || status == G_IO_STATUS_EOF) {
        return FALSE;
    }

    return TRUE;
}

void redirect_stdout_to_console(ConsoleData *console) {
    // Create a pipe
    if (pipe(console->pipe_fd) == -1) {
        perror("pipe");
        return;
    }

    // Set the pipe to non-blocking mode
    int flags = fcntl(console->pipe_fd[0], F_GETFL, 0);
    fcntl(console->pipe_fd[0], F_SETFL, flags | O_NONBLOCK);

    // Redirect stdout to the write end of the pipe
    dup2(console->pipe_fd[1], STDOUT_FILENO);

    // Create a GIOChannel to read from the pipe
    GIOChannel *channel = g_io_channel_unix_new(console->pipe_fd[0]);
    g_io_add_watch(channel, G_IO_IN, (GIOFunc)read_from_pipe, console);
    g_io_channel_unref(channel);
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *panedRight;
    GtkWidget *panedLeft;
    GtkWidget *splitVertical;
    GtkWidget *image_container;
    GtkWidget *image;
    GtkWidget *bottom_pane;
    GdkPixbuf *pixbuf;
    GdkPixbuf *window_icon_pixbuf;
    GtkCssProvider *cssProvider;
    GdkDisplay *display;
    GdkScreen *screen;
    ConsoleData console_data = {0};
    std::vector<std::thread*> threads;



    // Initialiser GTK
    gtk_init(&argc, &argv);
    // Créer une fenêtre
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK Responsive Image Example");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), &threads);
    // Charger l'icône pour la fenêtre
    window_icon_pixbuf = gdk_pixbuf_new_from_file("icon.png", NULL); // Assurez-vous que le fichier est bien nommé et accessible
    if (window_icon_pixbuf) {
        gtk_window_set_icon(GTK_WINDOW(window), window_icon_pixbuf);
        g_object_unref(window_icon_pixbuf); // Libérer la mémoire utilisée par le pixbuf
    } else {
        g_warning("Failed to load window icon from file");
    }



    splitVertical = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    panedRight = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
    panedLeft = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
    gtk_widget_set_name(splitVertical, "splitVertical");
    gtk_widget_set_name(panedRight, "panedRight");
    gtk_widget_set_name(panedLeft, "panedLeft");
    gtk_widget_set_size_request(panedLeft, 200, -1);
    gtk_container_add(GTK_CONTAINER(window), splitVertical);
    gtk_paned_add1(GTK_PANED(splitVertical),panedLeft);
    gtk_paned_add2(GTK_PANED(splitVertical),panedRight);



    // Créer le panneau supérieur avec une image
    image_container = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_name(image_container, "image_container");
    gtk_paned_add1(GTK_PANED(panedRight), image_container);
    // Charger l'image
    pixbuf = gdk_pixbuf_new_from_file("icon.png", NULL);
    if (!pixbuf) {
        g_error("Failed to load image from file");
        return 1;
    }
    image = gtk_image_new_from_pixbuf(pixbuf);
    gtk_container_add(GTK_CONTAINER(image_container), image);



    // Créer le panneau inférieur
    bottom_pane = gtk_scrolled_window_new(NULL, NULL);
    gtk_paned_add2(GTK_PANED(panedRight), bottom_pane);

    // Create a GtkTextView for the console output
    console_data.text_view = gtk_text_view_new();
    gtk_container_add(GTK_CONTAINER(bottom_pane), console_data.text_view);

    // Get the buffer for the text view
    console_data.text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(console_data.text_view));

    // Initialize the queue for keeping track of lines
    console_data.lines = g_queue_new();

    // Redirect stdout to the console
    redirect_stdout_to_console(&console_data);

    // Example lines to add to the console
    for (int i = 0; i < 850; i++) {
        printf("Line %d: Hello, world!\n", i + 1);
    }


    cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(cssProvider,
        "#image_container { background-color: #181818; }"
        ,
        -1, NULL);
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);



    pthread_t t1, t2;
    pthread_create(&t1, nullptr, stm_main_funct, nullptr);
    pthread_create(&t2, nullptr, loop_sys_tick, nullptr);


    gtk_widget_show_all(window);
    gtk_main();


    pthread_kill(t1, SIGTERM);
    pthread_kill(t2, SIGTERM);
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);

    return 0;
}
