#include "robotSim.h"



robotSim::robotSim(const char *filenameRobot, int robotLength, int robotWidth, const char *filenameTable, int tableLength, int tableWidth)
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
    pixbufs.pixbufRobot = pixbufRobot;
    pixbufs.pixbufTable = pixbufTable;
    g_signal_connect(G_OBJECT(robotWidget), "draw", G_CALLBACK(draw_callback), &pixbufs);
}

GtkWidget* robotSim::getWidget(void){
    return robotWidget;
}

robotSim::~robotSim()
{
}

gboolean robotSim::draw_callback(GtkWidget *widget, cairo_t *cr, gpointer data){
    GtkAllocation allocation;
    gtk_widget_get_allocation(widget, &allocation);

    pixbufs_t* pixbufsData = (pixbufs_t*)data;

    int widget_width = allocation.width;
    int widget_height = allocation.height;

    int image1_width = gdk_pixbuf_get_width(pixbufsData->pixbufTable);
    int image1_height = gdk_pixbuf_get_height(pixbufsData->pixbufTable);

    double scale_x = (double)widget_width / (double)image1_width;
    double scale_y = (double)widget_height / (double)image1_height;
    double scale = MIN(scale_x, scale_y);

    int new_width = (int)(image1_width * scale);
    int new_height = (int)(image1_height * scale);

    GdkPixbuf *scaled_pixbuf1 = gdk_pixbuf_scale_simple(pixbufsData->pixbufTable, new_width, new_height, GDK_INTERP_BILINEAR);

    int offset_x = (widget_width - new_width) / 2;
    int offset_y = (widget_height - new_height) / 2;

    gdk_cairo_set_source_pixbuf(cr, scaled_pixbuf1, offset_x, offset_y);
    cairo_paint(cr);

    g_object_unref(scaled_pixbuf1);

    // Draw the second image at a fixed position
    if (pixbufsData->pixbufRobot) {
        int image2_width = gdk_pixbuf_get_width(pixbufsData->pixbufRobot);
        int image2_height = gdk_pixbuf_get_height(pixbufsData->pixbufRobot);

        int new_width2 = (int)(image2_width * scale);
        int new_height2 = (int)(image2_height * scale);

        GdkPixbuf *scaled_pixbuf2 = gdk_pixbuf_scale_simple(pixbufsData->pixbufRobot, new_width2, new_height2, GDK_INTERP_BILINEAR);

        // Fixed position for the second image
        int image2_x = offset_x + (new_width*(0+1500))/3000 - (new_width2/2);
        int image2_y = offset_y + (new_height*(0+1000))/2000 - (new_height2/2);

        gdk_cairo_set_source_pixbuf(cr, scaled_pixbuf2, image2_x, image2_y);
        cairo_paint(cr);
    }

    return FALSE;
}