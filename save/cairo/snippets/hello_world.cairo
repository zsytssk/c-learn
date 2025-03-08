cairo_scale(cr, 120, 120);
cairo_text_extents_t te;
/* Drawing code goes here */
cairo_set_source_rgb(cr, 1, 0, 0);
cairo_paint_with_alpha(cr, 0.4);
cairo_select_font_face(cr, "Georgia",
                        CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
cairo_set_font_size(cr, 1.2);
cairo_text_extents(cr, "hello world", &te);
cairo_move_to(cr, 0, te.height / 2);
cairo_show_text(cr, "hello world");