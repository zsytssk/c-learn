#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <X11/Xutil.h>
#include <string.h>
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

    char alphabet[] = "AbCdEfGhIjKlMnOpQrStUvWxYz";
    char letter[2];
    /* Example is in 26.0 x 1.0 coordinate space */
    cairo_scale(cr, 30, 30);
    cairo_set_font_size(cr, 0.8);

    cairo_font_extents_t fe;
    cairo_text_extents_t te;
    /* Drawing code goes here */
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_select_font_face(cr, "Georgia",
                           CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);

    for (int i = 0; i < strlen(alphabet); i++)
    {
        /* get each letter separately
         * (from comp.lang.c FAQ, Question 13.2) */
        *letter = '\0';
        strncat(letter, alphabet + i, 1);

        cairo_text_extents(cr, letter, &te);
        cairo_move_to(cr, i + 0.5 - te.x_bearing - te.width / 2,
                      0.5 - te.y_bearing - te.height / 2);
        cairo_show_text(cr, letter);
    }

    cairo_set_source_rgba(cr, 1, 0.0, 0.0, 0.6);
    cairo_font_extents(cr, &fe);
    for (int i = 0; i < strlen(alphabet); i++)
    {
        *letter = '\0';
        strncat(letter, alphabet + i, 1);

        cairo_text_extents(cr, letter, &te);
        cairo_move_to(cr, i + 0.5 - te.x_bearing - te.width / 2,
                      0.5 - fe.descent + fe.height / 2);
        cairo_show_text(cr, letter);
    }
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
