/**
 * Modified By: Arian Karbasi
 * Student Id: 20308751
 * Userid: a3karbas
 */

#ifndef CS488_VIEWER_HPP
#define CS488_VIEWER_HPP

#include <GL/glew.h>
#include "shader.hpp"
#include <gtkmm.h>
#include <gtkglmm.h>
#include <list>
#include <stack>
#include "algebra.hpp"
#include "scene.hpp"
#include "material.hpp"
#include "ChessGame.hpp"
#include "SoundManager.h"

// The "main" OpenGL widget
class Viewer : public Gtk::GL::DrawingArea {
	static const double TRANSLATE_CONSTANT = 0.03;
	static const double THETA_CONSTANT = 0.7;
public:
  Viewer();
  virtual ~Viewer();

  // A useful function that forces this widget to rerender. If you
  // want to render a new frame, do not call on_expose_event
  // directly. Instead call this, which will cause an on_expose_event
  // call when the time is right.
  void invalidate();
  
  enum AXIS {
	  X_AXIS = (1u << 0),
	  Y_AXIS = (1u << 1),
	  Z_AXIS = (1u << 2),
  };
  enum PICKING_STATE {
	  WHITE_SELECT_PIECE,
	  BLACK_SELECT_PIECE,
	  MOVE_PIECE,
	  TEMP_PICK_BLACK,
	  TEMP_PICK_WHITE,
  };
  enum MODE {
	  CAMERA_ROTATE,
	  POSITION_ORIENTATION,
	  GAME_MODE,
  };
  enum BOARD_TEXTURE {
	  BLACK_WHITE_MARBLE,
	  WOOD,
	  METAL,
  };
  enum SKYBOX_TEXTURE {
	  SKYBOX1,
	  SKYBOX2,
	  SKYBOX3,
	  SKYBOX4,
  };
  enum FLOW_SPEED {
	  SLOW,
	  MEDIUM,
	  FAST,
	  SMALL_RIPPLE,
	  MEDIUM_RIPPLE,
	  BIG_RIPPLE,
  };
  enum SOUND {
	  BIT,
	  CLASSICAL,
	  ROLL,
  };

  void set_mode(MODE mode);
  void toggle_zbuffer();
  //set the options menu
  void set_options_menu(Gtk::Menu* options_menu);
  //set mode menu
  void set_mode_menu(Gtk::Menu* mode_menu);
  //SET LABEL
  void set_label (Gtk::Label* label);
  //set the filename
  void set_filename(std::string name);
  //sets the board texture
  void set_board_texture(BOARD_TEXTURE board_texture);
  //set the lava flow speed
  void set_lava_flow(FLOW_SPEED speed);
  //set the water flow speed
  void set_water_flow(FLOW_SPEED speed);
  //set the water ripple
  void set_water_ripple(FLOW_SPEED water_setting);
  //sets the occlusion
  void toggle_occlusion();
  //toggle shadows on and off
  void toggle_shadow();
  //toggles the diffuse lighting
  void toggle_lighting();
  //toggle luigi
  void toggle_luigi();
  //reset the view
  void reset_view();
  //set the sound
  void set_sound(SOUND sound);
  //sets the skybox
  void set_skybox(SKYBOX_TEXTURE skybox);
  //makes a new game
  void new_game();

protected:

  // Events we implement
  // Note that we could use gtkmm's "signals and slots" mechanism
  // instead, but for many classes there's a convenient member
  // function one just needs to define that'll be called with the
  // event.

  // Called when GL is first initialized
  virtual void on_realize();
  // Called when our window needs to be redrawn
  virtual bool on_expose_event(GdkEventExpose* event);
  // Called when the window is resized
  virtual bool on_configure_event(GdkEventConfigure* event);
  // Called when a mouse button is pressed
  virtual bool on_button_press_event(GdkEventButton* event);
  // Called when a mouse button is released
  virtual bool on_button_release_event(GdkEventButton* event);
  // Called when the mouse moves
  virtual bool on_motion_notify_event(GdkEventMotion* event);
  //perform the translations
  virtual void rotate_puppet(float fOldX, float fNewX, float fOldY, float fNewY);
  //draws the puppet
  void draw_pieces(bool picking, int shader_id);
  //
  void draw_fire_pieces(bool picking, int shader_id);
  //draws the board
  void draw_white_board(bool picking, int shader_id);
  //
  void draw_black_board(bool picking, int shader_id);
  //
  void draw_luigi(bool picking, int shader_id);
  //draw the skybox
  void draw_skybox();
  //draws the puddle
  void draw_puddle(bool picking, int shader_id);
  //initialize everything
  void initialize();
  //initialize all the shaders
  void init_shaders();
  //
  void init_colour_texture(GLuint *texture_ID);
  //
  void init_simplex_texture(GLuint *texture_ID);
  //
  void init_gradient_texture(GLuint *texture_ID);
  //
  void set_texture(const char *filename, int width, int height);
  //
  void set_fire_texture();
  //
  void set_white_tile_texture();
  //
  void set_black_tile_texture();
  //
  void set_skybox_texture();
  //
  int timer_tick();
  //
  void pick(int x, int y, SceneNode* node);
  //processHits
  void process(int hits, GLuint buffer[], SceneNode* scene_node);
  //moves a scenenode to the required space
  int move_piece(int old_pos, int new_pos);
  //takes a piece
  void take_piece(int taking_pos);
  //moves the puddle
  int move_puddle(int old_pos, int new_pos);
  //set up depth and frame buffer ids
  void set_up_shadow_map();
  //set up the bias texture -- not sure what this texture is yet in relation to shadowmapping
  void set_up_mapper_texture();
  //set up matrices for drawing
  void set_up_for_drawing(Point3D pos, Vector3D look, bool is_shadow);
  //load all the sounds
  void load_sounds();
  
private:
  Matrix4x4 m_translation;
  Matrix4x4 m_rotate;
  Gtk::Menu* options;
  Gtk::Menu* mode;
  Gtk::Label* feedback_label;
  std::string filename;
  MODE m_mode;
  PICKING_STATE m_state;
  PICKING_STATE prev_state;
  float prev_x;
  float prev_y;
  bool is_z_buffer;
  unsigned int m_axis;
  Shader *piece_shader;
  Shader *white_tile_shader;
  Shader *skybox_shader;
 // Shader *black_tile_shader;

  //perlin noise stuff
  const static int gradient_perlin[16][3];
  const static int gradient_new[32][4];
  const static unsigned char simplex_lookup[64][4];
  const static int colours[256];

  GLuint colour_tex_id;
  GLuint grad_new_tex_id;
  GLuint grad_tex_id;
  GLuint simplex_tex_id;
  GLuint water_tex_id;
  GLuint white_tile_tex_id;
  GLuint black_tile_id;
  GLuint fire_tex_id;
  GLuint depth_tex_id;
  GLuint frame_tex_id;
  GLuint skybox_tex_id[5];

  int location_colour_texture;
  int location_gradient_texture;
  int location_simplex_texture;
  int location_time;
  int location_water_texture;
  int location_fire_texture;
  int location_white_tile_texture;
  int location_noise;
  int location_shadow_map_texture;
  int location_shadow_map_texture_piece;
  int location_ripple_speed;
  int location_ripple_size;
  int location_occlusion;
  int location_lighting;
  int location_skybox_texture;
  float location_time_divider;
  float location_transparency;
//  int location_black_tile_texture;

  sigc::connection timer_val;

  int timer;
  int piece_select_pos;
  int puddle_pos;

  //chess setup
  ChessGame *chess_game;
  SceneNode* chess;
  SceneNode* chess_fire;

  //chess board
  SceneNode* chess_board;
  SceneNode* chess_board_black;

  SceneNode* puddle;

  //Luigi
  SceneNode* luigi;

  SceneNode* selected_piece;

  Point3D light_pos;
  Vector3D light_dir;

  Point3D camera_pos;
  Vector3D camera_dir;

  std::string black_board_filename;
  std::string white_board_filename;
  std::string skybox_texture_filename;

  int lava_flow_speed;
  int water_flow_speed;
  int ripple_speed;
  float ripple_size;

  bool should_shadow;
  bool should_occlude;
  bool should_draw_luigi;
  bool should_draw_puddle;
  int should_light;

  //sounds
  int water_sound, lava_sound, classical_sound, bit_sound, roll_sound, win_sound;
  int lava_source, water_source, classical_source, bit_source, roll_source, win_source;

};

#endif
