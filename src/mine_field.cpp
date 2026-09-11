/* SPDX-License-Identifier: Unlicense */

#include "mine_field.hpp"

namespace kablamo {

MineField::MineField()
{
  set_size_request(field_width(), field_height());
  set_hexpand(false);
  set_vexpand(false);
  add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::POINTER_MOTION_MASK |
             Gdk::LEAVE_NOTIFY_MASK);
}

void MineField::reset()
{
  press_r_ = press_c_ = -1;
  queue_draw();
}

bool MineField::hit(double x, double y, int& r, int& c) const
{
  const int ix = static_cast<int>(x) - kPad;
  const int iy = static_cast<int>(y) - kPad;
  if (ix < 0 || iy < 0)
    return false;
  c = ix / kCell;
  r = iy / kCell;
  return r >= 0 && r < kH && c >= 0 && c < kW;
}

void MineField::draw_raised(const Cairo::RefPtr<Cairo::Context>& cr, double x, double y,
                            double s, bool down) const
{
  cr->set_line_width(1.0);
  cr->set_source_rgb(0.753, 0.753, 0.753);
  cr->rectangle(x, y, s, s);
  cr->fill();
  if (down) {
    cr->set_source_rgb(0.502, 0.502, 0.502);
    cr->move_to(x + 0.5, y + s - 0.5);
    cr->line_to(x + 0.5, y + 0.5);
    cr->line_to(x + s - 0.5, y + 0.5);
    cr->stroke();
    cr->set_source_rgb(1.0, 1.0, 1.0);
    cr->move_to(x + 0.5, y + s - 0.5);
    cr->line_to(x + s - 0.5, y + s - 0.5);
    cr->line_to(x + s - 0.5, y + 0.5);
    cr->stroke();
  } else {
    cr->set_source_rgb(1.0, 1.0, 1.0);
    cr->move_to(x + 0.5, y + s - 0.5);
    cr->line_to(x + 0.5, y + 0.5);
    cr->line_to(x + s - 0.5, y + 0.5);
    cr->stroke();
    cr->set_source_rgb(0.502, 0.502, 0.502);
    cr->move_to(x + 0.5, y + s - 0.5);
    cr->line_to(x + s - 0.5, y + s - 0.5);
    cr->line_to(x + s - 0.5, y + 0.5);
    cr->stroke();
    cr->set_source_rgb(0.251, 0.251, 0.251);
    cr->move_to(x + 1.5, y + s - 1.5);
    cr->line_to(x + s - 1.5, y + s - 1.5);
    cr->line_to(x + s - 1.5, y + 1.5);
    cr->stroke();
  }
}

bool MineField::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
  const int w = get_allocated_width();
  const int h = get_allocated_height();
  cr->set_source_rgb(0.753, 0.753, 0.753);
  cr->rectangle(0, 0, w, h);
  cr->fill();
  cr->set_line_width(2.0);
  cr->set_source_rgb(0.502, 0.502, 0.502);
  cr->move_to(1, h - 1);
  cr->line_to(1, 1);
  cr->line_to(w - 1, 1);
  cr->stroke();
  cr->set_source_rgb(1.0, 1.0, 1.0);
  cr->move_to(1, h - 1);
  cr->line_to(w - 1, h - 1);
  cr->line_to(w - 1, 1);
  cr->stroke();

  for (int r = 0; r < kH; ++r) {
    for (int c = 0; c < kW; ++c) {
      const double x = kPad + c * kCell;
      const double y = kPad + r * kCell;
      const bool down = (r == press_r_ && c == press_c_);
      draw_raised(cr, x, y, kCell, down);
    }
  }
  return true;
}

bool MineField::on_button_press_event(GdkEventButton* event)
{
  if (!event || event->button != 1)
    return false;
  int r = -1, c = -1;
  if (!hit(event->x, event->y, r, c))
    return false;
  press_r_ = r;
  press_c_ = c;
  signal_pressing_.emit(true);
  queue_draw();
  return true;
}

bool MineField::on_button_release_event(GdkEventButton* event)
{
  if (!event || event->button != 1)
    return false;
  press_r_ = press_c_ = -1;
  signal_pressing_.emit(false);
  queue_draw();
  return true;
}

bool MineField::on_motion_notify_event(GdkEventMotion* event)
{
  if (!event || press_r_ < 0)
    return false;
  int r = -1, c = -1;
  const bool on = hit(event->x, event->y, r, c) && r == press_r_ && c == press_c_;
  /* keep original press cell; visual only if still over it — handled in draw via
     checking current pointer would need extra state. M0: stay depressed until release. */
  (void)on;
  return false;
}

bool MineField::on_leave_notify_event(GdkEventCrossing* event)
{
  if (event && event->detail == GDK_NOTIFY_INFERIOR)
    return false;
  if (press_r_ >= 0) {
    press_r_ = press_c_ = -1;
    signal_pressing_.emit(false);
    queue_draw();
  }
  return false;
}

}  // namespace kablamo
