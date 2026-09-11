/* SPDX-License-Identifier: Unlicense */

#pragma once

#include <gtkmm.h>

namespace kablamo {

class MineField : public Gtk::DrawingArea {
 public:
  static constexpr int kW = 9;
  static constexpr int kH = 9;
  static constexpr int kCell = 24;
  static constexpr int kPad = 6;

  MineField();

  void reset();
  int field_width() const { return kPad * 2 + kW * kCell; }
  int field_height() const { return kPad * 2 + kH * kCell; }

  sigc::signal<void, bool>& signal_pressing() { return signal_pressing_; }

 protected:
  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
  bool on_button_press_event(GdkEventButton* event) override;
  bool on_button_release_event(GdkEventButton* event) override;
  bool on_motion_notify_event(GdkEventMotion* event) override;
  bool on_leave_notify_event(GdkEventCrossing* event) override;

 private:
  bool hit(double x, double y, int& r, int& c) const;
  void draw_raised(const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, double s,
                   bool down) const;

  int press_r_ = -1;
  int press_c_ = -1;
  sigc::signal<void, bool> signal_pressing_;
};

}  // namespace kablamo
