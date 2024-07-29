#include <gtk/gtk.h>
#include <glib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <thread>
#include <chrono>
#include <vector>
#include <pthread.h>
#include <atomic>
#include <iostream>

#include "hardware_interface.h"
#include "console.h"
#include "i2cProcess.h"
#include "statusTextView.h"
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



static gboolean on_draw_led_area(GtkWidget *widget, cairo_t *cr, gpointer data) {
    // Dimensions de la zone de dessin
    //gint width = gtk_widget_get_allocated_width(widget);
    gint height = gtk_widget_get_allocated_height(widget);
    
    // Calculer la taille et la position du carré pour qu'il soit centré
    gint square_size = 15;
    gint x = 5;
    gint y = (height - square_size) / 2;

    // Définir la couleur rouge pour le dessin
    cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);  // Rouge

    // Dessiner le carré
    cairo_rectangle(cr, x, y, square_size, square_size);
    cairo_fill(cr);

    return FALSE;  // Indiquer que le dessin est terminé
}

int main(int argc, char *argv[]) {
    std::atomic<bool> stop_thread(false);
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
    GtkWidget *scrolledWindowLeftInfo;
    GtkWidget *textViewLeftInfo;
    GtkWidget *boxLed;
    GtkWidget *led1_area;
    GtkWidget *led2_area;
    GtkWidget *led1_label;
    GtkWidget *led2_label;
    GThread *t3, *t4;



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


    scrolledWindowLeftInfo = gtk_scrolled_window_new(NULL, NULL);
    gtk_paned_add1(GTK_PANED(panedLeft), scrolledWindowLeftInfo);
    textViewLeftInfo = gtk_text_view_new();
    t3 = intiThreadStatusTextView(textViewLeftInfo,&stop_thread);
    gtk_container_add(GTK_CONTAINER(scrolledWindowLeftInfo), textViewLeftInfo);


    boxLed = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_paned_add2(GTK_PANED(panedLeft), boxLed);

    led1_label = gtk_label_new("LED 1:");
    gtk_box_pack_start(GTK_BOX(boxLed), led1_label, FALSE, FALSE, 5);


    led1_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(led1_area, 20, 20);
    gtk_box_pack_start(GTK_BOX(boxLed), led1_area, TRUE, TRUE, 0);
    g_signal_connect(G_OBJECT(led1_area), "draw", G_CALLBACK(on_draw_led_area), NULL);

    led2_label = gtk_label_new("LED 2:");
    gtk_box_pack_start(GTK_BOX(boxLed), led2_label, FALSE, FALSE, 5);

    led2_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(led2_area, 20, 20);
    gtk_box_pack_start(GTK_BOX(boxLed), led2_area, TRUE, TRUE, 0);
    g_signal_connect(G_OBJECT(led2_area), "draw", G_CALLBACK(on_draw_led_area), NULL);




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

    // Redirect stdout to the console
    //redirect_stdout_to_console(text_buffer);


    cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(cssProvider,
        "#image_container { background-color: #181818; }"
        ,
        -1, NULL);
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);



    pthread_t t1, t2;
    pthread_create(&t2, nullptr, loop_sys_tick, nullptr);
    pthread_create(&t1, nullptr, stm_main_funct, nullptr);
    t4 = g_thread_new("simulationProcess", simulationProcess, NULL);

    gtk_widget_show_all(window);
    gtk_main();


    pthread_kill(t1, SIGTERM);
    pthread_kill(t2, SIGTERM);
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    g_thread_join(t3);

    return 0;
}
