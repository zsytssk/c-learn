#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <X11/Xutil.h>

#include <cairo.h>
#include <cairo-xlib.h>

#include "win.c"

int main(int argc, char *argv[])
{
    win_t win = dpy_init();
    win_init(&win);
    win_draw(&win);

    win_handle_events(&win);

    win_deinit(&win);
    XCloseDisplay(win.dpy);
    return 0;
}

static void
win_draw(win_t *win)
{
    Visual *visual = DefaultVisual(win->dpy, DefaultScreen(win->dpy));
    cairo_surface_t *surface = cairo_xlib_surface_create(win->dpy, win->win, visual,
                                                         win->width, win->height);
    cairo_t *cr = cairo_create(surface);
    // printf("width:%d,height:%d\n", win->width, win->height);

    // 白色背景
    cairo_scale(cr, win->width, win->height);
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_rectangle(cr, 0, 0, 1, 1);
    cairo_fill(cr);
    cairo_scale(cr, 1.0 / win->width, 1.0 / win->height);

    double x, y, px, ux = 1, uy = 1, dashlength;
    char text[] = "joy";
    cairo_font_extents_t fe;
    cairo_text_extents_t te;

    /* Example is in 26.0 x 1.0 coordinate space */
    cairo_scale(cr, 240, 240);
    cairo_set_font_size(cr, 0.5);

    /* Drawing code goes here */
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_select_font_face(cr, "Georgia",
                           CAIRO_FONT_SLANT_ITALIC, CAIRO_FONT_WEIGHT_BOLD);
    // cairo_font_extents(cr, &fe);

    cairo_device_to_user_distance(cr, &ux, &uy);

    if (ux > uy)
        px = ux;
    else
        px = uy;
    cairo_font_extents(cr, &fe);
    cairo_text_extents(cr, text, &te);
    x = 0.5 - te.x_bearing - te.width / 2;
    y = 0.5 - fe.descent + fe.height / 2;
    printf("te.width=%f,fe.width=%f\n", te.height, fe.height);

    /* baseline, descent, ascent, height */
    cairo_set_line_width(cr, 4 * px);
    dashlength = 9 * px;
    cairo_set_dash(cr, &dashlength, 1, 0);
    cairo_set_source_rgba(cr, 0, 0.6, 0, 0.5);
    cairo_move_to(cr, x + te.x_bearing, y);
    cairo_rel_line_to(cr, te.width, 0);
    cairo_move_to(cr, x + te.x_bearing, y + fe.descent);
    cairo_rel_line_to(cr, te.width, 0);
    cairo_move_to(cr, x + te.x_bearing, y - fe.ascent);
    cairo_rel_line_to(cr, te.width, 0);
    cairo_move_to(cr, x + te.x_bearing, y - fe.height);
    cairo_rel_line_to(cr, te.width, 0);
    cairo_stroke(cr);

    /* extents: width & height */
    cairo_set_source_rgba(cr, 0, 0, 0.75, 0.5);
    cairo_set_line_width(cr, px);
    dashlength = 3 * px;
    cairo_set_dash(cr, &dashlength, 1, 0);
    cairo_rectangle(cr, x + te.x_bearing, y + te.y_bearing, te.width, te.height);
    cairo_stroke(cr);

    /* text */
    cairo_move_to(cr, x, y);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_show_text(cr, text);

    /* bearing */
    cairo_set_dash(cr, NULL, 0, 0);
    cairo_set_line_width(cr, 2 * px);
    cairo_set_source_rgba(cr, 0, 0, 0.75, 0.5);
    cairo_move_to(cr, x, y);
    cairo_rel_line_to(cr, te.x_bearing, te.y_bearing);
    cairo_stroke(cr);

    /* text's advance */
    cairo_set_source_rgba(cr, 0, 0, 0.75, 0.5);
    cairo_arc(cr, x + te.x_advance, y + te.y_advance, 5 * px, 0, 2 * M_PI);
    cairo_fill(cr);

    /* reference point */
    cairo_arc(cr, x, y, 5 * px, 0, 2 * M_PI);
    cairo_set_source_rgba(cr, 0.75, 0, 0, 0.5);
    cairo_fill(cr);

    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}
// static void
// win_draw(win_t *win)
// {
//     Visual *visual = DefaultVisual(win->dpy, DefaultScreen(win->dpy));
//     cairo_surface_t *surface = cairo_xlib_surface_create(win->dpy, win->win, visual,
//                                                          win->width, win->height);
//     cairo_t *cr = cairo_create(surface);

//     cairo_set_source_rgb(cr, 1, 1, 1);

//     cairo_save(cr);
//     cairo_set_font_size(cr, 20);
//     cairo_move_to(cr, 100, 100);
//     cairo_show_text(cr, "Hello World.");
//     cairo_restore(cr);

//     cairo_destroy(cr);
//     cairo_surface_destroy(surface);
// }
