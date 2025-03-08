    cairo_scale(cr, 120, 120);

    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_move_to(cr, 0, 0);
    cairo_line_to(cr, 1, 1);
    cairo_move_to(cr, 1, 0);
    cairo_line_to(cr, 0, 1);
    cairo_set_line_width(cr, 0.2);
    cairo_stroke(cr);

    cairo_rectangle(cr, 0, 0, 0.5, 0.5);
    cairo_set_source_rgba(cr, 1, 0, 0, 0.80);
    cairo_fill(cr);

    cairo_rectangle(cr, 0, 0.5, 0.5, 0.5);
    cairo_set_source_rgba(cr, 0, 1, 0, 0.60);
    cairo_fill(cr);

    cairo_rectangle(cr, 0.5, 0, 0.5, 0.5);
    cairo_set_source_rgba(cr, 0, 0, 1, 0.40);
    cairo_fill(cr);