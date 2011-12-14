/**
 * Modified By: Arian Karbasi
 * Student Id: 20308751
 * Userid: a3karbas
 */

#include "appwindow.hpp"

AppWindow::AppWindow()
{
  set_title("Advanced Ergonomics Laboratory");

  // A utility class for constructing things that go into menus, which
  // we'll set up next.
  using Gtk::Menu_Helpers::MenuElem;
  using Gtk::Menu_Helpers::RadioMenuElem;
  using Gtk::Menu_Helpers::CheckMenuElem;
  
  // Set up the application menu
  // The slot we use here just causes AppWindow::hide() on this,
  // which shuts down the application.
  m_menu_app.items().push_back(MenuElem("_Quit", Gtk::AccelKey("q"),
    sigc::mem_fun(*this, &AppWindow::hide)));
  m_menu_app.items().push_back(MenuElem("_Reset View", Gtk::AccelKey("R"),
    sigc::mem_fun(m_viewer, &Viewer::reset_view)));
  m_menu_app.items().push_back(MenuElem("_New Game", Gtk::AccelKey("A"),
    sigc::mem_fun(m_viewer, &Viewer::new_game)));
  
  //set up the model mode menu
  sigc::slot1<void, Viewer::MODE> mode_slot =
		  sigc::mem_fun (m_viewer, &Viewer::set_mode);

  m_menu_mode.items().push_back( RadioMenuElem(m_menu_mode_group, "_Position/Orientation", Gtk::AccelKey("P"),
  	sigc::bind( mode_slot, Viewer::POSITION_ORIENTATION ) ) );
  m_menu_mode.items().push_back( RadioMenuElem(m_menu_mode_group, "_Camera Rotate", Gtk::AccelKey("U"),
  	sigc::bind( mode_slot, Viewer::CAMERA_ROTATE ) ) );
  m_menu_mode.items().push_back( RadioMenuElem(m_menu_mode_group, "_Game Mode", Gtk::AccelKey("S"),
  	sigc::bind( mode_slot, Viewer::GAME_MODE ) ) );

  //set up the texture options
  sigc::slot1<void, Viewer::BOARD_TEXTURE> board_texture_slot =
		  sigc::mem_fun(m_viewer, &Viewer::set_board_texture);

  m_menu_board_texture.items().push_back( RadioMenuElem(m_menu_board_texture_group,"_Black and White Marble", Gtk::AccelKey("M"),
		  sigc::bind(board_texture_slot, Viewer::BLACK_WHITE_MARBLE)));
  m_menu_board_texture.items().push_back( RadioMenuElem(m_menu_board_texture_group,"Wooden", Gtk::AccelKey("W"),
		  sigc::bind(board_texture_slot, Viewer::WOOD)));
  m_menu_board_texture.items().push_back( RadioMenuElem(m_menu_board_texture_group,"Metallic", Gtk::AccelKey("G"),
		  sigc::bind(board_texture_slot, Viewer::METAL)));

  //set the lava flow menu
  sigc::slot1<void, Viewer::FLOW_SPEED> lava_speed_slot =
		  sigc::mem_fun(m_viewer, &Viewer::set_lava_flow);

  m_menu_lava_flow.items().push_back( RadioMenuElem(m_menu_lava_flow_group,"_Slow", Gtk::AccelKey("B"),
		  sigc::bind(lava_speed_slot, Viewer::SLOW)));
  m_menu_lava_flow.items().push_back( RadioMenuElem(m_menu_lava_flow_group,"_Medium", Gtk::AccelKey("C"),
		  sigc::bind(lava_speed_slot, Viewer::MEDIUM)));
  m_menu_lava_flow.items().push_back( RadioMenuElem(m_menu_lava_flow_group,"_Fast", Gtk::AccelKey("D"),
		  sigc::bind(lava_speed_slot, Viewer::FAST)));

  //set the skybox menu
  sigc::slot1<void, Viewer::SKYBOX_TEXTURE> skybox_slot =
		  sigc::mem_fun(m_viewer, &Viewer::set_skybox);

  m_menu_skybox.items().push_back( RadioMenuElem(m_menu_skybox_group,"_Sky 1", Gtk::AccelKey("W"),
		  sigc::bind(skybox_slot, Viewer::SKYBOX1)));
  m_menu_skybox.items().push_back( RadioMenuElem(m_menu_skybox_group,"_Sky 2", Gtk::AccelKey("X"),
		  sigc::bind(skybox_slot, Viewer::SKYBOX2)));
  m_menu_skybox.items().push_back( RadioMenuElem(m_menu_skybox_group,"_Mountains", Gtk::AccelKey("Z"),
		  sigc::bind(skybox_slot, Viewer::SKYBOX3)));
  m_menu_skybox.items().push_back( RadioMenuElem(m_menu_skybox_group,"_Space", Gtk::AccelKey("Y"),
		  sigc::bind(skybox_slot, Viewer::SKYBOX4)));

  //set the water flow menu
  sigc::slot1<void, Viewer::FLOW_SPEED> water_speed_slot =
		  sigc::mem_fun(m_viewer, &Viewer::set_water_flow);

  m_menu_water_flow.items().push_back( RadioMenuElem(m_menu_water_flow_group,"_Slow", Gtk::AccelKey("E"),
		  sigc::bind(water_speed_slot, Viewer::SLOW)));
  m_menu_water_flow.items().push_back( RadioMenuElem(m_menu_water_flow_group,"_Medium", Gtk::AccelKey("F"),
		  sigc::bind(water_speed_slot, Viewer::MEDIUM)));
  m_menu_water_flow.items().push_back( RadioMenuElem(m_menu_water_flow_group,"_Fast", Gtk::AccelKey("H"),
		  sigc::bind(water_speed_slot, Viewer::FAST)));

  //set the water flow menu
  sigc::slot1<void, Viewer::SOUND> sound_slot =
		  sigc::mem_fun(m_viewer, &Viewer::set_sound);

  m_menu_sound.items().push_back( RadioMenuElem(m_menu_sound_group,"_8 bit", Gtk::AccelKey("O"),
		  sigc::bind(sound_slot, Viewer::BIT)));
  m_menu_sound.items().push_back( RadioMenuElem(m_menu_sound_group,"_Classical", Gtk::AccelKey("Y"),
		  sigc::bind(sound_slot, Viewer::CLASSICAL)));
  m_menu_sound.items().push_back( RadioMenuElem(m_menu_sound_group,"_Mozart", Gtk::AccelKey("W"),
		  sigc::bind(sound_slot, Viewer::ROLL)));

  //set up the water ripple menu
  sigc::slot1<void, Viewer::FLOW_SPEED> water_ripple_slot =
		  sigc::mem_fun(m_viewer, &Viewer::set_water_ripple);

  m_menu_water_ripple.items().push_back( RadioMenuElem(m_menu_water_ripple_speed_group,"_Slow Ripples", Gtk::AccelKey("I"),
		  sigc::bind(water_ripple_slot, Viewer::SLOW)));
  m_menu_water_ripple.items().push_back( RadioMenuElem(m_menu_water_ripple_speed_group,"_Medium Ripples", Gtk::AccelKey("J"),
		  sigc::bind(water_ripple_slot, Viewer::MEDIUM)));
  m_menu_water_ripple.items().push_back( RadioMenuElem(m_menu_water_ripple_speed_group,"_Fast Ripples", Gtk::AccelKey("K"),
		  sigc::bind(water_ripple_slot, Viewer::FAST)));
  m_menu_water_ripple.items().push_back( RadioMenuElem(m_menu_water_ripple_size_group,"_Medium Sized Ripples", Gtk::AccelKey("M"),
		  sigc::bind(water_ripple_slot, Viewer::MEDIUM_RIPPLE)));
  m_menu_water_ripple.items().push_back( RadioMenuElem(m_menu_water_ripple_size_group,"_Big Sized Ripples", Gtk::AccelKey("N"),
		  sigc::bind(water_ripple_slot, Viewer::BIG_RIPPLE)));
  m_menu_water_ripple.items().push_back( RadioMenuElem(m_menu_water_ripple_size_group,"_Small Sized Ripples", Gtk::AccelKey("L"),
		  sigc::bind(water_ripple_slot, Viewer::SMALL_RIPPLE)));


  //set up the shadow menu
  m_menu_shadow.items().push_back( CheckMenuElem("_Toggle Shadow", Gtk::AccelKey("T"),
		  sigc::mem_fun(m_viewer, &Viewer::toggle_shadow)));

  m_menu_occlusion.items().push_back( CheckMenuElem("_Toggle Occlusion", Gtk::AccelKey("O"),
		  sigc::mem_fun(m_viewer, &Viewer::toggle_occlusion)));
  m_menu_occlusion.items().push_back( CheckMenuElem("_Lighting off", Gtk::AccelKey("V"),
		  sigc::mem_fun(m_viewer, &Viewer::toggle_lighting)));
  m_menu_occlusion.items().push_back( CheckMenuElem("_Toggle Luigi", Gtk::AccelKey("U"),
		  sigc::mem_fun(m_viewer, &Viewer::toggle_luigi)));


  // Set up the menu bar
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Application", m_menu_app));
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Mode", m_menu_mode));
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Board Textures", m_menu_board_texture));
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Lava Flow Speed", m_menu_lava_flow));
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Water Flow Speed", m_menu_water_flow));
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Water Ripple Settings", m_menu_water_ripple));
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Shadow", m_menu_shadow));
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Occlusion", m_menu_occlusion));
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Music", m_menu_sound));
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Skybox", m_menu_skybox));
  
  // Pack in our widgets
  
  // First add the vertical box as our single "top" widget
  add(m_vbox);
  // Put the menubar on the top, and make it as small as possible
  m_vbox.pack_start(m_menubar, Gtk::PACK_SHRINK);

  // Put the viewer below the menubar. pack_start "grows" the widget
  // by default, so it'll take up the rest of the window.
  m_viewer.set_size_request(1200, 1200);
  m_vbox.pack_start(m_viewer);
  m_vbox.pack_end(label, Gtk::PACK_SHRINK);

  show_all();

  m_viewer.set_options_menu(&m_menu_options);
  m_viewer.set_mode_menu(&m_menu_mode);
  m_viewer.set_label(&label);
}

void AppWindow::set_filename(std::string name)
{
	m_viewer.set_filename(name);
}
