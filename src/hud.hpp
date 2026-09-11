/* SPDX-License-Identifier: Unlicense */

#pragma once

#include <gtkmm.h>

namespace kablamo {

enum class Face { smile, wow, dead, won };

class Hud : public Gtk::DrawingArea {
 public:
  Hud();

  void set_remaining(int n);
  void set_seconds(int n);
  void set_face(Face f);
  void reset();

  sigc::signal<void>& signal_new_game() { return signal_new_game_; }

 protected:
  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
  bool on_button_press_event(GdkEventButton* event) override;
  bool on_button_release_event(GdkEventButton* event) override;

 private:
  void draw_lcd(const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, int value) const;
  void draw_digit(const Cairo::RefPtr<Cairo::Context>& cr, double x, double y, int d) const;
  void draw_face(const Cairo::RefPtr<Cairo::Context>& cr, double cx, double cy, double r) const;
  bool in_face(double x, double y) const;

  int remaining_ = 10;
  int seconds_ = 0;
  Face face_ = Face::smile;
  bool face_down_ = false;
  sigc::signal<void> signal_new_game_;
};

}  // namespace kablamo
