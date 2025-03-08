#include <string.h>
#include <cairo.h>

int main(int argc, char *argv[])
{
    cairo_surface_t *surface;
    cairo_t *cr;
    int i;
    cairo_text_extents_t te;
    char alphabet[] = "AbCdEfGhIjKlMnOpQrStUvWxYz";
    char letter[2];

    /* Prepare drawing area */
    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 780, 30);
    cr = cairo_create(surface);
    /* Example is in 26.0 x 1.0 coordinate space */
    cairo_scale(cr, 30, 30);
    cairo_set_font_size(cr, 0.8);

    /* Drawing code goes here */
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_select_font_face(cr, "Georgia",
                           CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);

    for (i = 0; i < strlen(alphabet); i++)
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

    /* Write output and clean up */
    cairo_surface_write_to_png(surface, "tips-letter.png");
    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    return 0;
}