cairo_scale(cr, 120, 120);

int i, j;
cairo_pattern_t *radpat, *linpat;

radpat = cairo_pattern_create_radial(0.25, 0.25, 0.1, 0.5, 0.5, 0.5);
cairo_pattern_add_color_stop_rgb(radpat, 0, 1.0, 0.8, 0.8);
cairo_pattern_add_color_stop_rgb(radpat, 1, 0.9, 0.0, 0.0);

for (i = 1; i < 10; i++)
    for (j = 1; j < 10; j++)
        cairo_rectangle(cr, i / 10.0 - 0.04, j / 10.0 - 0.04, 0.08, 0.08);
cairo_set_source(cr, radpat);
cairo_fill(cr);

linpat = cairo_pattern_create_linear(0.25, 0.35, 0.75, 0.65);
cairo_pattern_add_color_stop_rgba(linpat, 0.00, 1, 1, 1, 0);
cairo_pattern_add_color_stop_rgba(linpat, 0.25, 0, 1, 0, 0.5);
cairo_pattern_add_color_stop_rgba(linpat, 0.50, 1, 1, 1, 0);
cairo_pattern_add_color_stop_rgba(linpat, 0.75, 0, 0, 1, 0.5);
cairo_pattern_add_color_stop_rgba(linpat, 1.00, 1, 1, 1, 0);

cairo_rectangle(cr, 0.0, 0.0, 1, 1);
cairo_set_source(cr, linpat);
cairo_fill(cr);