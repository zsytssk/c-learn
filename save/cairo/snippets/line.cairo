cairo_set_source_rgb(cr, 1, 1, 1);
cairo_rectangle(cr, 0, 0, 1, 1);
cairo_fill(cr);
cairo_scale(cr, 1.0 / win->width, 1.0 / win->height);

// 正方形
cairo_scale(cr, 250, 250);

cairo_set_source_rgb(cr, 1, 0, 0);
cairo_set_line_width(cr, 0.01);
cairo_move_to(cr, 0.25, 0.25);
cairo_line_to(cr, 0.5, 0.375);
cairo_rel_line_to(cr, 0.25, -0.125);
cairo_arc(cr, 0.5, 0.5, 0.25 * sqrt(2), -0.25 * M_PI, 0.25 * M_PI);
cairo_rel_curve_to(cr, -0.25, -0.125, -0.25, 0.125, -0.5, 0);
cairo_close_path(cr);

cairo_stroke(cr);