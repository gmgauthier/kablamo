/* SPDX-License-Identifier: Unlicense */

#pragma once

#include "board.hpp"

#include <gtkmm.h>

namespace kablamo {

class MineField : public Gtk::DrawingArea {
 public:
  static constexpr int kCell = 24;
  static constexpr int kPad = 6;

  explicit MineField(Board& board);

  void reset();
  int field_width() const { return kPad * 2 + kW * kCell; }
  int field_height() const { return kPad * 2 + kH * kCell; }

  sigc::signal<void, bool>& signal_pressing() { return signal_pressing_; }
  sigc::signal<void>& signal_changed() { return signal_changed_; }

 protected:
  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
  bool on_button_press_event(GdkEventButton* event) override;
  bool on_button_release_event(GdkEventButton* event) override;
  bool on_motion_notify_event(GdkEventMotion* event) override;
  bool on_leave_notify_event(GdkEventCrossing* event) override;

 private:
  bool hit(double x, double y, int& r, int& c) const;
  bool over_press(int r, int c) const;
  void draw_raised(const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, double s,
                   bool down) const;
  void draw_sunken(const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, double s,
                   bool boom) const;
  void draw_flag(const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, double s) const;
  void draw_mine(const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, double s) const;
  void draw_number(const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, double s,
                   int n);
  void draw_x(const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, double s) const;

  Board& board_;
  int press_r_ = -1;
  int press_c_ = -1;
  int hover_r_ = -1;
  int hover_c_ = -1;
  sigc::signal<void, bool> signal_pressing_;
  sigc::signal<void> signal_changed_;
};

}  // namespace kablamo
