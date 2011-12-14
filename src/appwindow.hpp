/**
 * Modified By: Arian Karbasi
 * Student Id: 20308751
 * Userid: a3karbas
 */

#ifndef APPWINDOW_HPP
#define APPWINDOW_HPP

#include "viewer.hpp"
#include <gtkmm.h>

class AppWindow : public Gtk::Window {
public:
  AppWindow();
  
  void set_filename(std::string name);

protected:

private:
  // A "vertical box" which holds everything in our window
  Gtk::VBox m_vbox;

  // The menubar, with all the menus at the top of the window
  Gtk::MenuBar m_menubar;
  // Each menu itself
  Gtk::Menu m_menu_app;

  //The mode menu
  Gtk::Menu m_menu_mode;
  Gtk::RadioButtonGroup m_menu_mode_group;

  //The options menu
  Gtk::Menu m_menu_options;

  //The board texture menu
  Gtk::Menu m_menu_board_texture;
  Gtk::RadioButtonGroup m_menu_board_texture_group;

  //the lava flow speed
  Gtk::Menu m_menu_lava_flow;
  Gtk::RadioButtonGroup m_menu_lava_flow_group;

  //the water flow speed
  Gtk::Menu m_menu_water_flow;
  Gtk::RadioButtonGroup m_menu_water_flow_group;

  //The sound menu
  Gtk::Menu m_menu_sound;
  Gtk::RadioButtonGroup m_menu_sound_group;

  Gtk::Menu m_menu_skybox;
  Gtk::RadioButtonGroup m_menu_skybox_group;

  //the occlusion setting
  Gtk::Menu m_menu_occlusion;


  //the water ripple settings
  Gtk::Menu m_menu_water_ripple;
  Gtk::RadioButtonGroup m_menu_water_ripple_size_group;
  Gtk::RadioButtonGroup m_menu_water_ripple_speed_group;

  //shadow menu
  Gtk::Menu m_menu_shadow;


  //the label
  Gtk::Label label;

  // The main OpenGL area
  Viewer m_viewer;
};

#endif
