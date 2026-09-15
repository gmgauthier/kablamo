/* SPDX-License-Identifier: Unlicense */

#include "hud.hpp"

#include <algorithm>
#include <cmath>

#include "bevel.hpp"
#include "mine_field.hpp"

namespace kablamo {
namespace {

constexpr double kDigitW = 13.0;
constexpr double kDigitH = 23.0;
constexpr double kLcdW = 41.0;
constexpr double kLcdH = 25.0;
constexpr double kFaceR = 16.0;

/* bit 0=a (top), 1=b (upper right), 2=c (lower right), 3=d (bottom),
   4=e (lower left), 5=f (upper left), 6=g (middle) */
const unsigned kSeg[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F,
};

void seg_h(const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, double w)
{
  cr->move_to(x + 1, y);
  cr->line_to(x + w - 1, y);
  cr->line_to(x + w - 3, y + 2);
  cr->line_to(x + 3, y + 2);
  cr->close_path();
  cr->fill();
}

void seg_v(const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, double h)
{
  cr->move_to(x, y + 1);
  cr->line_to(x + 2, y + 3);
  cr->line_to(x + 2, y + h - 3);
  cr->line_to(x, y + h - 1);
  cr->line_to(x - 2, y + h - 3);
  cr->line_to(x - 2, y + 3);
  cr->close_path();
  cr->fill();
}

}  // namespace

Hud::Hud()
{
  set_size_request(MineField::kPad * 2 + kW * MineField::kCell, 44);
  set_hexpand(false);
  set_vexpand(false);
  add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
}

void Hud::set_remaining(int n)
{
  remaining_ = n;
  queue_draw();
}

void Hud::set_seconds(int n)
{
  seconds_ = n;
  queue_draw();
}

void Hud::set_face(Face f)
{
  face_ = f;
  queue_draw();
}

void Hud::reset()
{
  remaining_ = 10;
  seconds_ = 0;
  face_ = Face::smile;
  face_down_ = false;
  queue_draw();
}

void Hud::draw_digit(const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, int d) const
{
  if (d < 0 || d > 9)
    d = 0;
  const double w = 9.0;
  const double mid = 10.0;
  auto paint = [&](unsigned m, double R, double G, double B) {
    cr->set_source_rgb(R, G, B);
    if (m & 1)
      seg_h(cr, x + 2, y, w);
    if (m & 2)
      seg_v(cr, x + 2 + w, y + 2, mid);
    if (m & 4)
      seg_v(cr, x + 2 + w, y + 2 + mid, mid);
    if (m & 8)
      seg_h(cr, x + 2, y + 2 + mid * 2, w);
    if (m & 16)
      seg_v(cr, x + 2, y + 2 + mid, mid);
    if (m & 32)
      seg_v(cr, x + 2, y + 2, mid);
    if (m & 64)
      seg_h(cr, x + 2, y + mid + 1, w);
  };
  paint(0x7F, 0.35, 0.04, 0.04);
  paint(kSeg[d], 1.0, 0.14, 0.08);
}

void Hud::draw_lcd(const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, int value) const
{
  if (value < -99)
    value = -99;
  if (value > 999)
    value = 999;
  draw_bevel(cr, x - 2, y - 2, kLcdW + 4, kLcdH + 4, true, 2.0);
  cr->set_source_rgb(0.0, 0.0, 0.0);
  cr->rectangle(x, y, kLcdW, kLcdH);
  cr->fill();
  const bool neg = value < 0;
  int n = std::abs(value);
  const int ones = n % 10;
  n /= 10;
  const int tens = n % 10;
  n /= 10;
  const int hun = n % 10;
  if (neg) {
    cr->set_source_rgb(1.0, 0.15, 0.1);
    cr->rectangle(x + 4, y + 11, 8, 2.5);
    cr->fill();
  } else {
    draw_digit(cr, x + 2, y + 1, hun);
  }
  draw_digit(cr, x + 2 + kDigitW, y + 1, tens);
  draw_digit(cr, x + 2 + kDigitW * 2, y + 1, ones);
  (void)kDigitH;
}

void Hud::draw_face(const Cairo::RefPtr<Cairo::Context>& cr, double cx, double cy, double r) const
{
  const double bs = r * 2 + 6;
  const double bx = cx - bs / 2.0;
  const double by = cy - bs / 2.0;
  gray(cr);
  cr->rectangle(bx, by, bs, bs);
  cr->fill();
  draw_bevel(cr, bx, by, bs, bs, face_down_, 2.0);
  const double inset = face_down_ ? 1.0 : 0.0;
  cr->save();
  cr->translate(inset, inset);
  cr->set_source_rgb(1.0, 1.0, 0.2);
  cr->arc(cx, cy, r - 1, 0, 2 * G_PI);
  cr->fill();
  cr->set_source_rgb(0.0, 0.0, 0.0);
  cr->set_line_width(1.2);
  cr->arc(cx, cy, r - 1, 0, 2 * G_PI);
  cr->stroke();

  if (face_ == Face::dead) {
    cr->set_line_width(1.6);
    cr->move_to(cx - 6, cy - 5);
    cr->line_to(cx - 2, cy - 1);
    cr->move_to(cx - 2, cy - 5);
    cr->line_to(cx - 6, cy - 1);
    cr->move_to(cx + 2, cy - 5);
    cr->line_to(cx + 6, cy - 1);
    cr->move_to(cx + 6, cy - 5);
    cr->line_to(cx + 2, cy - 1);
    cr->stroke();
    cr->arc(cx, cy + 5, 4, G_PI, 2 * G_PI);
    cr->stroke();
  } else if (face_ == Face::won) {
    cr->arc(cx - 4, cy - 3, 3.2, 0, 2 * G_PI);
    cr->fill();
    cr->arc(cx + 4, cy - 3, 3.2, 0, 2 * G_PI);
    cr->fill();
    cr->set_source_rgb(1.0, 1.0, 0.2);
    cr->arc(cx - 4, cy - 3, 1.2, 0, 2 * G_PI);
    cr->fill();
    cr->arc(cx + 4, cy - 3, 1.2, 0, 2 * G_PI);
    cr->fill();
    cr->set_source_rgb(0.0, 0.0, 0.0);
    cr->arc(cx, cy + 3, 5, 0.15 * G_PI, 0.85 * G_PI);
    cr->stroke();
  } else if (face_ == Face::wow) {
    cr->arc(cx - 4, cy - 3, 1.6, 0, 2 * G_PI);
    cr->fill();
    cr->arc(cx + 4, cy - 3, 1.6, 0, 2 * G_PI);
    cr->fill();
    cr->arc(cx, cy + 5, 3.2, 0, 2 * G_PI);
    cr->stroke();
  } else {
    cr->arc(cx - 4, cy - 3, 1.6, 0, 2 * G_PI);
    cr->fill();
    cr->arc(cx + 4, cy - 3, 1.6, 0, 2 * G_PI);
    cr->fill();
    cr->arc(cx, cy + 2, 6, 0.15 * G_PI, 0.85 * G_PI);
    cr->stroke();
  }
  cr->restore();
}

bool Hud::in_face(double x, double y) const
{
  const double cx = get_allocated_width() / 2.0;
  const double cy = get_allocated_height() / 2.0;
  const double half = kFaceR + 4;
  return x >= cx - half && x <= cx + half && y >= cy - half && y <= cy + half;
}

bool Hud::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
  const int w = get_allocated_width();
  const int h = get_allocated_height();
  gray(cr);
  cr->rectangle(0, 0, w, h);
  cr->fill();
  draw_bevel(cr, 0, 0, w, h, true, 2.0);

  const double ly = (h - kLcdH) / 2.0;
  draw_lcd(cr, 8, ly, remaining_);
  draw_lcd(cr, w - 8 - kLcdW, ly, seconds_);
  draw_face(cr, w / 2.0, h / 2.0, kFaceR);
  return true;
}

bool Hud::on_button_press_event(GdkEventButton* event)
{
  if (!event || event->button != 1)
    return false;
  if (!in_face(event->x, event->y))
    return false;
  face_down_ = true;
  queue_draw();
  return true;
}

bool Hud::on_button_release_event(GdkEventButton* event)
{
  if (!event || event->button != 1)
    return false;
  const bool was = face_down_;
  face_down_ = false;
  queue_draw();
  if (was && in_face(event->x, event->y))
    signal_new_game_.emit();
  return was;
}

}  // namespace kablamo
