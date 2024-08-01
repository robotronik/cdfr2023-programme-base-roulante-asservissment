#include "ledSim.h"


ledSim::ledSim(/* args */){
    ledWidget = gtk_drawing_area_new();
    gtk_widget_set_size_request(ledWidget, 20, 20);
    g_signal_connect(G_OBJECT(ledWidget), "draw", G_CALLBACK(on_draw_led_area), &ledColor);
}

GtkWidget* ledSim::ledGetWidget(void){
    return ledWidget;
}

void ledSim::setcolor(int red, int green, int blue){
    ledColor.red = ((double)red)/255;
    ledColor.green = ((double)green)/255;
    ledColor.blue = ((double)blue)/255;
    g_idle_add((GSourceFunc)queue_draw, this);
}


gboolean ledSim::queue_draw(gpointer data){
    ledSim* led = (ledSim*)data;
    gtk_widget_queue_draw(led->ledWidget);
    return FALSE;
}


void ledSim::ledUpdate(void){
    if(ledOnStatus){
        this->setcolor(125, 255, 0);
    }
    else{
        this->setcolor(0, 0, 0);
    }
}
void ledSim::ledSetStatus(bool status){
    ledOnStatus = status;
    this->ledUpdate();
}

ledSim::~ledSim()
{
}


gboolean ledSim::on_draw_led_area(GtkWidget *widget, cairo_t *cr, gpointer data){
    LedState* ledColor = (LedState*)data;
    // Dimensions de la zone de dessin
    //gint width = gtk_widget_get_allocated_width(widget);
    gint height = gtk_widget_get_allocated_height(widget);
    
    // Calculer la taille et la position du carré pour qu'il soit centré
    gint square_size = 15;
    gint x = 5;
    gint y = (height - square_size) / 2;

    // Définir la couleur rouge pour le dessin
    cairo_set_source_rgb(cr, ledColor->red, ledColor->green, ledColor->blue);  // Rouge

    // Dessiner le carré
    cairo_rectangle(cr, x, y, square_size, square_size);
    cairo_fill(cr);

    return FALSE;  // Indiquer que le dessin est terminé
}