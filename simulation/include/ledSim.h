#pragma once


#include <gtk/gtk.h>


class ledSim
{
private:
    typedef struct {
        double red;
        double green;
        double blue;
    } LedState;

    LedState ledColor;
    GtkWidget* ledWidget;
    bool ledOnStatus = false;
public:
    ledSim();
    GtkWidget* ledGetWidget(void);
    void setcolor(int red, int green, int blue);
    void ledUpdate(void);
    void ledSetStatus(bool status);
    ~ledSim();

    static gboolean on_draw_led_area(GtkWidget *widget, cairo_t *cr, gpointer data);
    static gboolean queue_draw(gpointer data);
};





