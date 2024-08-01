#pragma once

#include <gtk/gtk.h>
#include "commands.h"

class robotSim
{
private:
    GtkWidget* robotWidget;
    GdkPixbuf *pixbufRobot = NULL;
    GdkPixbuf *pixbufTable = NULL;
    GThread *threadRobot = NULL;
    bool stop_thread = false;
    bool robotInterfaceIsCreate = false;
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
    void stopRobotSim(void);
    GThread *getTheard(void);
    ~robotSim();
    static gboolean draw_callback(GtkWidget *widget, cairo_t *cr, gpointer data);
    static gpointer threadFuncRobot(gpointer data);
    static gboolean queue_draw(gpointer data);
};

