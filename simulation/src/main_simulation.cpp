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
    return 0;  
}


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
    std::vector<std::thread*> threads;
    GtkWidget *text_view;
    GtkTextBuffer *text_buffer;



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
    text_view = gtk_text_view_new();
    gtk_container_add(GTK_CONTAINER(bottom_pane), text_view);

    // Get the buffer for the text view
    text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

    // Redirect stdout to the console
    redirect_stdout_to_console(text_buffer);

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
