#pragma once

#include <gtk/gtk.h>

class robotSim
{
private:
    typedef struct {
        GdkPixbuf *pixbufRobot;
        GdkPixbuf *pixbufTable;
    } pixbufs_t;

    GtkWidget* robotWidget;
    GdkPixbuf *pixbufRobot = NULL;
    GdkPixbuf *pixbufTable = NULL;
    pixbufs_t pixbufs;

public:
    robotSim(const char *filenameRobot, int robotLength, int robotWidth, const char *filenameTable, int tableLength = 3000, int tableWidth = 2000);
    GtkWidget* getWidget(void);
    ~robotSim();
    static gboolean draw_callback(GtkWidget *widget, cairo_t *cr, gpointer data);
};

