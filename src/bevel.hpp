/* SPDX-License-Identifier: Unlicense */

#pragma once

#include <cairomm/context.h>
#include <glibmm/refptr.h>

namespace kablamo {

inline void gray(const Cairo::RefPtr<Cairo::Context>& cr)
{
  cr->set_source_rgb(0.753, 0.753, 0.753);
}

inline void draw_bevel(const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, double w,
                       double h, bool sunken, double thick = 2.0)
{
  const double x1 = x + 0.5;
  const double y1 = y + 0.5;
  const double x2 = x + w - 0.5;
  const double y2 = y + h - 0.5;
  cr->set_line_width(thick);
  if (sunken) {
    cr->set_source_rgb(0.502, 0.502, 0.502);
    cr->move_to(x1, y2);
    cr->line_to(x1, y1);
    cr->line_to(x2, y1);
    cr->stroke();
    cr->set_source_rgb(1.0, 1.0, 1.0);
    cr->move_to(x1, y2);
    cr->line_to(x2, y2);
    cr->line_to(x2, y1);
    cr->stroke();
  } else {
    cr->set_source_rgb(1.0, 1.0, 1.0);
    cr->move_to(x1, y2);
    cr->line_to(x1, y1);
    cr->line_to(x2, y1);
    cr->stroke();
    cr->set_source_rgb(0.502, 0.502, 0.502);
    cr->move_to(x1, y2);
    cr->line_to(x2, y2);
    cr->line_to(x2, y1);
    cr->stroke();
  }
}

}  // namespace kablamo
