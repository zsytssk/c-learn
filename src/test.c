#include <gtk/gtk.h>

static cairo_surface_t *surface = NULL;

static void do_drawing(GtkDrawingArea *drawing_area,
                       cairo_t *cr,
                       int width,
                       int height,
                       gpointer data)
{
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_select_font_face(cr, "monospace", CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 20.0);

    cairo_move_to(cr, 50.0, 50.0);
    cairo_show_text(cr, "hello world.");

    int x, y;
    float size_font;

    x = 20;
    y = 20;
    size_font = 20.0;

    cairo_text_extents_t te;
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_select_font_face(cr, "monospace",
                           CAIRO_FONT_SLANT_OBLIQUE, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, size_font);
    cairo_text_extents(cr, "hello world!", &te);
    cairo_move_to(cr, x, y);
    cairo_show_text(cr, "hello world!");
    cairo_fill(cr);
}

static void
close_window(void)
{
    if (surface)
        cairo_surface_destroy(surface);
}

static void
activate(GtkApplication *app,
         gpointer user_data)
{
    GtkWidget *window;
    GtkWidget *frame;
    GtkWidget *drawing_area;
    GtkGesture *drag;
    GtkGesture *press;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Drawing Area");

    g_signal_connect(window, "destroy", G_CALLBACK(close_window), NULL);

    frame = gtk_frame_new(NULL);
    gtk_window_set_child(GTK_WINDOW(window), frame);

    drawing_area = gtk_drawing_area_new();
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_area), do_drawing, NULL, NULL);
    /* set a minimum size */
    gtk_widget_set_size_request(drawing_area, 400, 400);

    gtk_frame_set_child(GTK_FRAME(frame), drawing_area);

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc,
         char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}