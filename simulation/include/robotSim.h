#pragma once

#include <gtk/gtk.h>

class robotSim
{
private:
    GtkWidget* robotWidget;
    GdkPixbuf *pixbufRobot = NULL;
    GdkPixbuf *pixbufTable = NULL;
    int robotLength;
    int robotWidth;
    int tableLength;
    int tableWidth;
    int x;
    int y;
    int teta;
public:
    robotSim(const char *filenameRobot, int robotLength, int robotWidth, const char *filenameTable, int tableLength = 3000, int tableWidth = 2000);
    GtkWidget* getWidget(void);
    void setPosition(int x,int y, int teta);
    ~robotSim();
    static gboolean draw_callback(GtkWidget *widget, cairo_t *cr, gpointer data);
};

