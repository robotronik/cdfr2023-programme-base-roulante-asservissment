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
#include "statusTextView.h"
#include "simulation.h"
#include "ledSim.h"
#include "robotSim.h"
#include "odometrieSim.h"


#define DEFAULT_WINDOWS_HEIGHT  600
#define DEFAULT_WINDOWS_WIDTH   800
#define DEFAULT_SPLIT_VERTICAL  200
#define DEFAULT_SPLIT_RIGHT     (DEFAULT_WINDOWS_HEIGHT - 200)
#define MIN_SPLIT_LEFT          25

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


static gboolean on_paned_motion_notify_event(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    return TRUE;
}

static gboolean on_paned_button_press_event(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    return TRUE;
}


int main(int argc, char *argv[]) {
    std::atomic<bool> stop_thread(false);
    GtkWidget *window;
    GtkWidget *panedRight;
    GtkWidget *panedLeft;
    GtkWidget *splitVertical;
    GtkWidget *image_container;
    GtkWidget *bottom_pane;
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
    ledSim *led1;
    ledSim *led2;
    robotSim *robotDrawing;
    odometrieSim *odometrie;



    // Initialiser GTK
    gtk_init(&argc, &argv);
    // Créer une fenêtre
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Simulateur Asservissement Robotronik");
    gtk_window_set_default_size(GTK_WINDOW(window), DEFAULT_WINDOWS_WIDTH, DEFAULT_WINDOWS_HEIGHT);
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
    gtk_widget_set_size_request(panedLeft, 100, -1);
    gtk_container_add(GTK_CONTAINER(window), splitVertical);
    gtk_paned_pack1(GTK_PANED(splitVertical), panedLeft, FALSE, FALSE);
    gtk_paned_pack2(GTK_PANED(splitVertical), panedRight, TRUE, FALSE);
    gtk_paned_set_position(GTK_PANED(splitVertical),DEFAULT_SPLIT_VERTICAL);
    gtk_paned_set_position(GTK_PANED(panedRight), DEFAULT_SPLIT_RIGHT);


    g_signal_connect(panedLeft, "motion-notify-event", G_CALLBACK(on_paned_motion_notify_event), NULL);
    g_signal_connect(panedLeft, "button-press-event", G_CALLBACK(on_paned_button_press_event), NULL);


    scrolledWindowLeftInfo = gtk_scrolled_window_new(NULL, NULL);
    gtk_paned_pack1(GTK_PANED(panedLeft), scrolledWindowLeftInfo, TRUE, FALSE);
    textViewLeftInfo = gtk_text_view_new();
    t3 = intiThreadStatusTextView(textViewLeftInfo,&stop_thread);
    gtk_container_add(GTK_CONTAINER(scrolledWindowLeftInfo), textViewLeftInfo);


    led1 = new ledSim();
    led2 = new ledSim();
    boxLed = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_size_request(boxLed, -1, MIN_SPLIT_LEFT);
    gtk_paned_pack2(GTK_PANED(panedLeft), boxLed, FALSE, FALSE);
    led1_label = gtk_label_new("LED 1:");
    gtk_box_pack_start(GTK_BOX(boxLed), led1_label, FALSE, FALSE, 5);
    led1_area = led1->ledGetWidget();
    gtk_box_pack_start(GTK_BOX(boxLed), led1_area, TRUE, TRUE, 0);
    led2_label = gtk_label_new("LED 2:");
    gtk_box_pack_start(GTK_BOX(boxLed), led2_label, FALSE, FALSE, 5);
    led2_area = led2->ledGetWidget();
    gtk_box_pack_start(GTK_BOX(boxLed), led2_area, TRUE, TRUE, 0);
    simLed1 = led1;
    simLed2 = led2;




    // Create the top panel with image Table and robot
    image_container = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_name(image_container, "image_container");
    gtk_paned_pack1(GTK_PANED(panedRight), image_container, TRUE, FALSE);
    robotDrawing = new robotSim("icon.png",320,250,"table.png");
    gtk_container_add(GTK_CONTAINER(image_container), robotDrawing->getWidget());



    // Créer le panneau inférieur
    bottom_pane = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(bottom_pane, -1, 50);
    gtk_paned_pack2(GTK_PANED(panedRight), bottom_pane, FALSE, FALSE);

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

    odometrie = new odometrieSim();
    odometrieGlobal = odometrie;
    // odometrie->setLeftSpeed(4000);
    // odometrie->setLeftDirection(true);
    // odometrie->setRightSpeed(4000);
    // odometrie->setRightDirection(true);

    pthread_t t1, t2;
    pthread_create(&t2, nullptr, loop_sys_tick, nullptr);
    pthread_create(&t1, nullptr, stm_main_funct, nullptr);
    t4 = g_thread_new("simulationProcess", simulationProcess, NULL);

    gtk_widget_show_all(window);
    gtk_main();

    odometrie->stopThread();
    pthread_kill(t1, SIGTERM);
    pthread_kill(t2, SIGTERM);
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    g_thread_join(t3);
    g_thread_join(t4);
    g_thread_join(robotDrawing->getTheard());


    delete led1;
    delete led2;
    if (cssProvider) {
        g_object_unref(cssProvider);
    }

    return 0;
}
