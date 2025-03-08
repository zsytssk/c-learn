cairo_scale(cr, 250, 250);

cairo_pattern_t *linpat, *radpat;
linpat = cairo_pattern_create_linear(0, 0, 1, 1);
cairo_pattern_add_color_stop_rgb(linpat, 0, 0, 0.3, 0.8);
cairo_pattern_add_color_stop_rgb(linpat, 1, 0, 0.8, 0.3);

radpat = cairo_pattern_create_radial(0.5, 0.5, 0.25, 0.5, 0.5, 0.75);
cairo_pattern_add_color_stop_rgba(radpat, 0, 0, 0, 0, 1);
cairo_pattern_add_color_stop_rgba(radpat, 0.5, 0, 0, 0, 0);

cairo_set_source(cr, linpat);
cairo_pattern_destroy(linpat);

cairo_mask(cr, radpat);
cairo_pattern_destroy(radpat);