// 白色背景
cairo_scale(cr, win->width, win->height);
cairo_set_source_rgb(cr, 1, 1, 1);
cairo_rectangle(cr, 0, 0, 1, 1);
cairo_fill(cr);
cairo_scale(cr, 1.0 / win->width, 1.0 / win->height);

// 正方形
cairo_scale(cr, 250, 250);
cairo_set_source_rgb(cr, 1, 0, 0);
cairo_rectangle(cr, 0.25, 0.25, 0.5, 0.5);
cairo_fill_preserve(cr);
cairo_set_line_width(cr, 0.01);
cairo_set_source_rgb(cr, 0, 1, 0);
cairo_stroke(cr);
