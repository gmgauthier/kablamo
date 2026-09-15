/* SPDX-License-Identifier: Unlicense */

#include "mine_field.hpp"
#include "bevel.hpp"

namespace kablamo {
namespace {

void rgb_for_num(int n, double& r, double& g, double& b)
{
  switch (n) {
    case 1:
      r = 0.0;
      g = 0.0;
      b = 1.0;
      break;
    case 2:
      r = 0.0;
      g = 0.5;
      b = 0.0;
      break;
    case 3:
      r = 1.0;
      g = 0.0;
      b = 0.0;
      break;
    case 4:
      r = 0.0;
      g = 0.0;
      b = 0.5;
      break;
    case 5:
      r = 0.5;
      g = 0.0;
      b = 0.0;
      break;
    case 6:
      r = 0.0;
      g = 0.5;
      b = 0.5;
      break;
    case 7:
      r = 0.0;
      g = 0.0;
      b = 0.0;
      break;
    default:
      r = 0.5;
      g = 0.5;
      b = 0.5;
      break;
  }
}

}  // namespace

MineField::MineField(Board& board)
    : board_(board)
{
  set_size_request(field_width(), field_height());
  set_hexpand(false);
  set_vexpand(false);
  add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::POINTER_MOTION_MASK |
             Gdk::LEAVE_NOTIFY_MASK);
}

void MineField::reset()
{
  press_r_ = press_c_ = hover_r_ = hover_c_ = -1;
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

bool MineField::over_press(int r, int c) const
{
  return press_r_ == r && press_c_ == c && hover_r_ == r && hover_c_ == c;
}

void MineField::draw_raised(const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, double s,
                            bool down) const
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

void MineField::draw_sunken(const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, double s,
                            bool boom) const
{
  if (boom)
    cr->set_source_rgb(1.0, 0.0, 0.0);
  else
    cr->set_source_rgb(0.753, 0.753, 0.753);
  cr->rectangle(x, y, s, s);
  cr->fill();
  cr->set_line_width(1.0);
  cr->set_source_rgb(0.502, 0.502, 0.502);
  cr->move_to(x + 0.5, y + s - 0.5);
  cr->line_to(x + 0.5, y + 0.5);
  cr->line_to(x + s - 0.5, y + 0.5);
  cr->stroke();
}

void MineField::draw_flag(const Cairo::RefPtr<Cairo::Context>& cr, double x, double y,
                          double s) const
{
  const double cx = x + s * 0.5;
  cr->set_source_rgb(0.0, 0.0, 0.0);
  cr->set_line_width(1.4);
  cr->move_to(cx - 1, y + 5);
  cr->line_to(cx - 1, y + s - 7);
  cr->stroke();
  cr->set_line_width(1.2);
  cr->move_to(cx - 5, y + s - 6);
  cr->line_to(cx + 5, y + s - 6);
  cr->stroke();
  cr->set_source_rgb(1.0, 0.0, 0.0);
  cr->move_to(cx - 1, y + 5);
  cr->line_to(cx + 7, y + 9);
  cr->line_to(cx - 1, y + 13);
  cr->close_path();
  cr->fill();
}

void MineField::draw_mine(const Cairo::RefPtr<Cairo::Context>& cr, double x, double y,
                          double s) const
{
  const double cx = x + s * 0.5;
  const double cy = y + s * 0.5;
  cr->set_source_rgb(0.05, 0.05, 0.05);
  cr->arc(cx, cy, 5.2, 0, 2 * G_PI);
  cr->fill();
  cr->set_line_width(1.6);
  cr->move_to(cx, cy - 8);
  cr->line_to(cx, cy + 8);
  cr->move_to(cx - 8, cy);
  cr->line_to(cx + 8, cy);
  cr->move_to(cx - 5.5, cy - 5.5);
  cr->line_to(cx + 5.5, cy + 5.5);
  cr->move_to(cx + 5.5, cy - 5.5);
  cr->line_to(cx - 5.5, cy + 5.5);
  cr->stroke();
  cr->set_source_rgb(1.0, 1.0, 1.0);
  cr->arc(cx - 1.6, cy - 1.6, 1.4, 0, 2 * G_PI);
  cr->fill();
}

void MineField::draw_x(const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, double s) const
{
  cr->set_source_rgb(1.0, 0.0, 0.0);
  cr->set_line_width(2.0);
  cr->move_to(x + 5, y + 5);
  cr->line_to(x + s - 5, y + s - 5);
  cr->move_to(x + s - 5, y + 5);
  cr->line_to(x + 5, y + s - 5);
  cr->stroke();
}

void MineField::draw_number(const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, double s,
                            int n)
{
  double R, G, B;
  rgb_for_num(n, R, G, B);
  auto layout = create_pango_layout(Glib::ustring::format(n));
  Pango::FontDescription desc;
  desc.set_family("Sans");
  desc.set_weight(Pango::WEIGHT_BOLD);
  desc.set_size(12 * Pango::SCALE);
  layout->set_font_description(desc);
  int tw = 0, th = 0;
  layout->get_pixel_size(tw, th);
  cr->set_source_rgb(R, G, B);
  cr->move_to(x + (s - tw) / 2.0, y + (s - th) / 2.0 - 1);
  layout->show_in_cairo_context(cr);
}

bool MineField::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
  const int w = get_allocated_width();
  const int h = get_allocated_height();
  gray(cr);
  cr->rectangle(0, 0, w, h);
  cr->fill();
  draw_bevel(cr, 0, 0, w, h, true, 2.0);

  const bool lost = board_.phase() == Phase::lost;
  for (int r = 0; r < kH; ++r) {
    for (int c = 0; c < kW; ++c) {
      const Cell& cell = board_.at(r, c);
      const double x = kPad + c * kCell;
      const double y = kPad + r * kCell;
      const bool boom = lost && r == board_.boom_r() && c == board_.boom_c();
      const bool show_mine = cell.mine && (cell.revealed || (lost && !cell.flagged));
      const bool wrong_flag = lost && cell.flagged && !cell.mine;

      if (wrong_flag) {
        draw_sunken(cr, x, y, kCell, false);
        draw_mine(cr, x, y, kCell);
        draw_x(cr, x, y, kCell);
      } else if (cell.revealed || show_mine) {
        draw_sunken(cr, x, y, kCell, boom);
        if (cell.mine)
          draw_mine(cr, x, y, kCell);
        else if (cell.adj > 0)
          draw_number(cr, x, y, kCell, cell.adj);
      } else {
        const bool down = over_press(r, c) && !cell.flagged;
        draw_raised(cr, x, y, kCell, down);
        if (cell.flagged)
          draw_flag(cr, x, y, kCell);
      }
    }
  }
  return true;
}

bool MineField::on_button_press_event(GdkEventButton* event)
{
  if (!event)
    return false;
  int r = -1, c = -1;
  if (!hit(event->x, event->y, r, c))
    return false;
  if (board_.phase() == Phase::won || board_.phase() == Phase::lost)
    return false;

  if (event->button == 3) {
    board_.toggle_flag(r, c);
    signal_changed_.emit();
    queue_draw();
    return true;
  }
  if (event->button != 1)
    return false;
  const Cell& cell = board_.at(r, c);
  if (cell.revealed || cell.flagged)
    return false;
  press_r_ = hover_r_ = r;
  press_c_ = hover_c_ = c;
  signal_pressing_.emit(true);
  queue_draw();
  return true;
}

bool MineField::on_button_release_event(GdkEventButton* event)
{
  if (!event || event->button != 1)
    return false;
  const int r = press_r_;
  const int c = press_c_;
  const bool fire = over_press(r, c);
  press_r_ = press_c_ = hover_r_ = hover_c_ = -1;
  signal_pressing_.emit(false);
  if (fire && r >= 0)
    board_.reveal(r, c);
  signal_changed_.emit();
  queue_draw();
  return true;
}

bool MineField::on_motion_notify_event(GdkEventMotion* event)
{
  if (!event || press_r_ < 0)
    return false;
  int r = -1, c = -1;
  if (hit(event->x, event->y, r, c)) {
    hover_r_ = r;
    hover_c_ = c;
  } else {
    hover_r_ = hover_c_ = -1;
  }
  queue_draw();
  return false;
}

bool MineField::on_leave_notify_event(GdkEventCrossing* event)
{
  if (event && event->detail == GDK_NOTIFY_INFERIOR)
    return false;
  if (press_r_ >= 0) {
    hover_r_ = hover_c_ = -1;
    queue_draw();
  }
  return false;
}

}  // namespace kablamo
