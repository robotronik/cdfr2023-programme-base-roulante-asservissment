#include "robotSim.h"

#define ROBOT_X_OFFSET 155
#define ROBOT_Y_OFFSET 60


robotSim::robotSim(const char *filenameRobot, int robotLength, int robotWidth, const char *filenameTable, int tableLength, int tableWidth):
    robotLength(robotLength),
    robotWidth(robotWidth),
    tableLength(tableLength),
    tableWidth(tableWidth)
{
    pixbufTable = gdk_pixbuf_new_from_file(filenameTable, NULL);
    if (!pixbufTable) {
        g_error("Failed to load table from file");
    }
    // Load the second image
    pixbufRobot = gdk_pixbuf_new_from_file(filenameRobot, NULL);
    if (!pixbufRobot) {
        g_error("Failed to load robot from file");
    }
    robotWidget = gtk_drawing_area_new();
    // Connect the draw signal to our callback function
    g_signal_connect(G_OBJECT(robotWidget), "draw", G_CALLBACK(draw_callback), this);
    threadRobot = g_thread_new("robotPostionUpdateThread", threadFuncRobot, this);
}

GtkWidget* robotSim::getWidget(void){
    return robotWidget;
}

void robotSim::setPosition(int x,int y, int teta){
    this->x = x;
    this->y = y;
    this->teta = teta;
    g_idle_add((GSourceFunc)queue_draw, this);
}

gboolean robotSim::queue_draw(gpointer data) {
    robotSim* robot = (robotSim*)data;
    gtk_widget_queue_draw(robot->robotWidget);
    return FALSE;
}

void robotSim::stopRobotSim(void){
    stop_thread = true;
}

GThread* robotSim::getTheard(void){
    return threadRobot;
}

robotSim::~robotSim()
{
}

gboolean robotSim::draw_callback(GtkWidget *widget, cairo_t *cr, gpointer data){
    GtkAllocation allocation;
    gtk_widget_get_allocation(widget, &allocation);

    robotSim* robot = (robotSim*)data;

    int widget_length = allocation.width;
    int widget_width = allocation.height;

    int image1_length = gdk_pixbuf_get_width(robot->pixbufTable);
    int image1_width = gdk_pixbuf_get_height(robot->pixbufTable);

    double scale_x = (double)widget_length / (double)image1_length;
    double scale_y = (double)widget_width / (double)image1_width;
    double scale = MIN(scale_x, scale_y);

    int new_length = (int)(image1_length * scale);
    int new_width = (int)(image1_width * scale);

    GdkPixbuf *scaled_pixbuf1 = gdk_pixbuf_scale_simple(robot->pixbufTable, new_length, new_width, GDK_INTERP_BILINEAR);
    GdkPixbuf *scaled_pixbufRot1 = gdk_pixbuf_rotate_simple(scaled_pixbuf1,GDK_PIXBUF_ROTATE_UPSIDEDOWN);

    int offset_x = (widget_length - new_length) / 2;
    int offset_y = (widget_width - new_width) / 2;

    gdk_cairo_set_source_pixbuf(cr, scaled_pixbufRot1, offset_x, offset_y);
    cairo_paint(cr);

    g_object_unref(scaled_pixbuf1);
    g_object_unref(scaled_pixbufRot1);

    int new_length2 = (robot->robotLength * new_length)/robot->tableLength;
    int new_width2 = (robot->robotWidth * new_width)/robot->tableWidth;

    GdkPixbuf *scaled_pixbuf2 = gdk_pixbuf_scale_simple(robot->pixbufRobot, new_length2, new_width2, GDK_INTERP_BILINEAR);

    // Fixed position for the second image
    int image2_x = offset_x + (new_length*(-robot->y+(robot->tableLength/2)))/robot->tableLength - (new_length2/2);
    int image2_y = offset_y + (new_width*(-robot->x+(robot->tableWidth/2)))/robot->tableWidth - (new_width2/2);

    cairo_save(cr);
    cairo_translate(cr, image2_x + new_length2 / 2, image2_y + new_width2 / 2);
    cairo_rotate(cr, robot->teta * (G_PI / 180));
    cairo_translate(cr, -(image2_x + new_length2 / 2), -(image2_y + new_width2 / 2));
    gdk_cairo_set_source_pixbuf(cr, scaled_pixbuf2, image2_x, image2_y);
    cairo_paint(cr);
    cairo_restore(cr);

    g_object_unref(scaled_pixbuf2);

    return FALSE;
}


gpointer robotSim::threadFuncRobot(gpointer data) {
    robotSim* robot = (robotSim*)data;
    while (!robot->stop_thread) {
        g_usleep(7000);
        int16_t x;
        int16_t y;
        int16_t theta;
        get_coordinates(x,y,theta);
        robot->setPosition(x,y,theta);
    }
    return NULL;
}