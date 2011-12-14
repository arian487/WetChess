/**
 * Modified By: Arian Karbasi
 * Student Id: 20308751
 * Userid: a3karbas
 */
//#include "shader.hpp"
#include "viewer.hpp"
#include "algebra.hpp"
#include "trackball.hpp"
#include "pixmap.hpp"
#include <iostream>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "scene_lua.hpp"

#define SKYBOX_SIZE 500

//this is the gradient proposed by perlin.  Copied straight from his paper
const int Viewer::gradient_perlin[16][3] =
		{{0,1,1},{0,1,-1},{0,-1,1},{0,-1,-1},
		{1,0,1},{1,0,-1},{-1,0,1},{-1,0,-1},
		{1,1,0},{1,-1,0},{-1,1,0},{-1,-1,0},
		{1,0,-1},{-1,0,-1},{0,-1,1},{0,1,1}};

//This is the midpoints of the 32 sided 4d object.  This object is used for simplex_noise
//points suggested by Gustovsson
const int Viewer::gradient_new[32][4] =
		{{0,1,1,1}, {0,1,1,-1}, {0,1,-1,1}, {0,1,-1,-1},
        {0,-1,1,1}, {0,-1,1,-1}, {0,-1,-1,1}, {0,-1,-1,-1},
        {1,0,1,1}, {1,0,1,-1}, {1,0,-1,1}, {1,0,-1,-1},
        {-1,0,1,1}, {-1,0,1,-1}, {-1,0,-1,1}, {-1,0,-1,-1},
        {1,1,0,1}, {1,1,0,-1}, {1,-1,0,1}, {1,-1,0,-1},
        {-1,1,0,1}, {-1,1,0,-1}, {-1,-1,0,1}, {-1,-1,0,-1},
        {1,1,1,0}, {1,1,-1,0}, {1,-1,1,0}, {1,-1,-1,0},
        {-1,1,1,0}, {-1,1,-1,0}, {-1,-1,1,0}, {-1,-1,-1,0}};

//this table was taken directly from gustovsson
//its the magnitude offsets I'm going to use for contributions
//notice there are only 24 non zero elements
const unsigned char Viewer::simplex_lookup[64][4] =
		  {{0,64,128,192},{0,64,192,128},{0,0,0,0},
		  {0,128,192,64},{0,0,0,0},{0,0,0,0},{0,0,0,0},{64,128,192,0},
		  {0,128,64,192},{0,0,0,0},{0,192,64,128},{0,192,128,64},
		  {0,0,0,0},{0,0,0,0},{0,0,0,0},{64,192,128,0},
		  {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
		  {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
		  {64,128,0,192},{0,0,0,0},{64,192,0,128},{0,0,0,0},
		  {0,0,0,0},{0,0,0,0},{128,192,0,64},{128,192,64,0},
		  {64,0,128,192},{64,0,192,128},{0,0,0,0},{0,0,0,0},
		  {0,0,0,0},{128,0,192,64},{0,0,0,0},{128,64,192,0},
		  {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
		  {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
		  {128,0,64,192},{0,0,0,0},{0,0,0,0},{0,0,0,0},
		  {192,0,64,128},{192,0,128,64},{0,0,0,0},{192,64,128,0},
		  {128,64,0,192},{0,0,0,0},{0,0,0,0},{0,0,0,0},
		  {192,64,0,128},{0,0,0,0},{192,128,0,64},{192,128,64,0}};

//This is the permutation matrix used by Ken Perlin to figure out an offset based
//on input...time is involved in my case
const int Viewer::colours[256] = {151,160,137,91,90,15,
		  131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
		  190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
		  88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
		  77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
		  102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
		  135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
		  5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		  223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
		  129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
		  251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
		  49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		  138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180};

Viewer::Viewer()
{
  Glib::RefPtr<Gdk::GL::Config> glconfig;

  // Ask for an OpenGL Setup with
  //  - red, green and blue component colour
  //  - a depth buffer to avoid things overlapping wrongly
  //  - double-buffered rendering to avoid tearing/flickering
  glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB |
                                     Gdk::GL::MODE_DEPTH |
                                     Gdk::GL::MODE_DOUBLE);
  if (glconfig == 0) {
    // If we can't get this configuration, die
    std::cerr << "Unable to setup OpenGL Configuration!" << std::endl;
    abort();
  }

  // Accept the configuration
  set_gl_capability(glconfig);

  // Register the fact that we want to receive these events
  add_events(Gdk::BUTTON1_MOTION_MASK    |
             Gdk::BUTTON2_MOTION_MASK    |
             Gdk::BUTTON3_MOTION_MASK    |
             Gdk::BUTTON_PRESS_MASK      | 
             Gdk::BUTTON_RELEASE_MASK    |
             Gdk::POINTER_MOTION_MASK    |
             Gdk::VISIBILITY_NOTIFY_MASK);

  m_rotate = Matrix4x4();
  m_translation = Matrix4x4();
  //chess = import_lua("puppet.lua");
  m_mode = POSITION_ORIENTATION;

  prev_x = 0;
  prev_y = 0;
  m_axis = 0;
  is_z_buffer = false;
  piece_shader = new Shader();
  white_tile_shader = new Shader();
  skybox_shader = new Shader();
}

Viewer::~Viewer()
{
  // Nothing to do here right now.
}

void Viewer::invalidate()
{
  // Force a rerender
  Gtk::Allocation allocation = get_allocation();
  get_window()->invalidate_rect( allocation, false);
}

void Viewer::on_realize()
{
  // Do some OpenGL setup.
  // First, let the base class do whatever it needs to
  Gtk::GL::DrawingArea::on_realize();
  
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();
  
  if (!gldrawable)
    return;

  if (!gldrawable->gl_begin(get_gl_context()))
    return;

  glShadeModel(GL_SMOOTH);
  glClearColor( 0.4, 0.4, 0.4, 0.0 );
  glDisable(GL_DEPTH_TEST);

  gldrawable->gl_end();
  this->initialize();
}

bool Viewer::on_expose_event(GdkEventExpose* event)
{
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable) return false;

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;

  /*
   * all the shadow passes start here
   */

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,frame_tex_id);	//Rendering offscreen
    glUseProgram(0);
    glViewport(0,0,get_width() * 2,get_height()* 2);
 // gluPerspective(40.0, (GLfloat)get_width()*2/(GLfloat)get_height()*2, 0.1, 1000.0);
    glClear( GL_DEPTH_BUFFER_BIT);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    set_up_for_drawing(light_pos, light_dir, true);
    glCullFace(GL_FRONT);
    if (should_shadow & !should_draw_luigi){
	    draw_fire_pieces(false,piece_shader->id());
	    draw_pieces(false,piece_shader->id());
    }
//  draw_white_board(false);
//  draw_black_board(false);

    set_up_mapper_texture();

  /*
   * now that we have made our shadow pass, lets do the real drawing
   */
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
    glViewport(0,0,1200, 1200);
 // gluPerspective(40.0, (GLfloat)get_width()/(GLfloat)get_height(), 0.1, 1000.0);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	float s[] = {0.15, 0.15, 0.05, 1 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, s);
	GLfloat shininess=10;
	glMaterialfv(GL_FRONT, GL_SHININESS, &shininess);
	GLfloat position[] = {0, 0, 200, 0.0};
	glLightfv(GL_LIGHT0, GL_POSITION, position);
  // Set up for perspective drawing 
//	glTranslatef(camera_pos[0],camera_pos[1], camera_pos[2]);
//	glRotatef(camera_dir[0], 0, 0, 1);


  // Clear framebuffer


//	    GLint test_int;
//	    glGetUniformiv(piece_shader->id(), location_time, &test_int);
//	    std::cout<<test_int<<std::endl;
//	glutSolidTeapot(1);

//  now bind the shader for the tiles and draw them
//    draw_skybox();
//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_LIGHTING);
//    glEnable(GL_BLEND);
    glCullFace(GL_BACK);
	set_up_for_drawing(Point3D(0,0,5), Vector3D(0,0,0), false);
	glRotatef(camera_dir[0],0,1,0);
	glRotatef(-camera_dir[1],1,0,0);
    draw_skybox();
    glEnable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D,depth_tex_id);
    white_tile_shader->bind();
    if (!should_draw_luigi) {
		draw_white_board(false, white_tile_shader->id());
		draw_black_board(false, white_tile_shader->id());
    }
    white_tile_shader->unbind();

	piece_shader->bind();
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D,depth_tex_id);

    // Set up lighting

    // Draw stuff
  	glEnable(GL_BLEND);
  	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  	if (!should_draw_luigi) {
		draw_fire_pieces(false, piece_shader->id());
		draw_pieces(false, piece_shader->id());
		if (should_draw_puddle)
			draw_puddle(false, piece_shader->id());
  	} else {
  		draw_luigi(false, piece_shader->id());
  	}
    glDisable(GL_BLEND);
    piece_shader->unbind();

  // Swap the contents of the front and back buffers so we see what we
  // just drew. This should only be done if double buffering is enabled.
  gldrawable->swap_buffers();

  gldrawable->gl_end();

  return true;
}

bool Viewer::on_configure_event(GdkEventConfigure* event)
{
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable) return false;
  
  if (!gldrawable->gl_begin(get_gl_context()))
    return false;

  // Set up perspective projection, using current size and aspect
  // ratio of display
  GLfloat light[] = {1, 1, 1, 1};
  GLfloat position[] = {0, 1000, 1.0, 0.0};
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light);
  glLightfv(GL_LIGHT0, GL_POSITION, position);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, event->width, event->height);
  gluPerspective(40.0, (GLfloat)event->width/(GLfloat)event->height, 0.1, 10000.0);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  // Reset to modelview matrix mode
  
  glMatrixMode(GL_MODELVIEW);

  gldrawable->gl_end();

  return true;
}

bool Viewer::on_button_press_event(GdkEventButton* event)
{

  prev_x = event->x;
  prev_y = event->y;

  if (event->button == 1) {
	  m_axis |= X_AXIS;
  } else if (event->button == 2) {
	  m_axis |= Y_AXIS;
  } else if (event->button == 3) {
	  m_axis |= Z_AXIS;
  }


//  this->invalidate();
  return true;
}

bool Viewer::on_button_release_event(GdkEventButton* event)
{
	  if (event->button == 1) {
		  m_axis &= ~X_AXIS;
		  if (m_mode == GAME_MODE) {
			  if (m_state == MOVE_PIECE) {
				  m_state = TEMP_PICK_WHITE;
				  pick(event->x, event->y, chess_board);
				  if (m_state == MOVE_PIECE) {
					  m_state = TEMP_PICK_BLACK;
					  pick(event->x, event->y, chess_board_black);
				  }
			  }
			  else if (m_state == WHITE_SELECT_PIECE) {
				  pick(event->x, event->y, chess);
			  }
			  else if (m_state == BLACK_SELECT_PIECE) {
				  pick(event->x, event->y, chess_fire);
			  }
		  }
	//	  std::cout<<m_state<<std::endl;
	  } if (event->button == 2) {
		  m_axis &= ~Y_AXIS;
	  } if (event->button == 3) {
		  m_axis &= ~Z_AXIS;
	  }

  return true;
}

bool Viewer::on_motion_notify_event(GdkEventMotion* event)
{
	  if (m_axis & Z_AXIS) {
		  if (m_mode == POSITION_ORIENTATION)
			  rotate_puppet(prev_x, (float)event->x, prev_y, (float)event->y);
	  }
	  if (m_axis & X_AXIS) {
		  if (m_mode == POSITION_ORIENTATION) {
			  m_translation[0][3] = m_translation[0][3] + (event->x - prev_x) * TRANSLATE_CONSTANT;
			  m_translation[1][3] = m_translation[1][3] - (event->y - prev_y) * TRANSLATE_CONSTANT;
		  }
		  else if (m_mode == CAMERA_ROTATE) {
			  camera_dir[0] -= (event->x - prev_x) * TRANSLATE_CONSTANT * 2;
			  camera_dir[1] += (event->y - prev_y) * TRANSLATE_CONSTANT * 2;
		  }
	  }
	  if (m_axis & Y_AXIS) {
		  if (m_mode == POSITION_ORIENTATION) {
			  m_translation[2][3] = m_translation[2][3] + (event->y - prev_y) * TRANSLATE_CONSTANT;
		  }
	  }

	  prev_x = event->x;
	  prev_y = event->y;

	  return true;
}

void Viewer::draw_pieces(bool picking, int shader_id)
{
	glUniform1f(location_ripple_speed, ripple_speed);
	glUniform1f(location_ripple_size, ripple_size);
  	glUniform1f(location_time_divider, water_flow_speed);
  	glUniform1f(location_transparency, 0.8);
  	glUniform1i(location_noise, 1);
  	glUniform1i(location_water_texture, 3);
  	if (should_occlude)
  		glUniform1i(location_occlusion, 1);
  	else
  		glUniform1i(location_occlusion, 0);

	if (picking)
		piece_shader->unbind();
	chess->set_transform(chess->get_transform() * m_translation * m_rotate);
	chess->walk_gl(picking, shader_id);
	Matrix4x4 inverted = (m_translation * m_rotate).invert();
	chess->set_transform(chess->get_transform() * inverted);
}

void Viewer::draw_puddle(bool picking, int shader_id)
{
	glUniform1f(location_ripple_speed, ripple_speed);
	glUniform1f(location_ripple_size, 1);
  	glUniform1f(location_time_divider, water_flow_speed * 5);
  	glUniform1f(location_transparency, 0.5);
  	glUniform1i(location_noise, 1);
  	glUniform1i(location_water_texture, 3);
  	glUniform1i(location_occlusion, 0);

	if (picking)
		piece_shader->unbind();
	puddle->set_transform(puddle->get_transform() * m_translation * m_rotate);
	puddle->walk_gl(picking, shader_id);
	Matrix4x4 inverted = (m_translation * m_rotate).invert();
	puddle->set_transform(puddle->get_transform() * inverted);
}

void Viewer::draw_fire_pieces(bool picking, int shader_id)
{
	glUniform1f(location_ripple_speed, 100);
	glUniform1f(location_ripple_size, 5);
	glUniform1i(location_shadow_map_texture_piece,7);
  	glUniform1i(location_colour_texture, 0);
  	glUniform1i(location_simplex_texture, 1);
  	glUniform1i(location_gradient_texture, 2);
  	glUniform1f(location_time, timer);

    glUniform1i(location_water_texture, 6);
  	glUniform1f(location_time_divider, lava_flow_speed);
  	glUniform1f(location_transparency, 1);
  	glUniform1i(location_noise, 0);
  	glUniform1i(location_lighting, should_light);
  	if (should_occlude)
  		glUniform1i(location_occlusion, 1);
  	else
  		glUniform1i(location_occlusion, 0);

	if (picking)
		piece_shader->unbind();
	chess_fire->set_transform(chess_fire->get_transform() * m_translation * m_rotate);
	chess_fire->walk_gl(picking, shader_id);
	Matrix4x4 inverted = (m_translation * m_rotate).invert();
	chess_fire->set_transform(chess_fire->get_transform() * inverted);
}

void Viewer::draw_white_board(bool picking, int shader_id)
{
	glUniform1i(location_shadow_map_texture,7);
	//some shader setup before drawing
	  glUniform1i(location_white_tile_texture, 4);
	if (picking)
		white_tile_shader->unbind();
	chess_board->set_transform(chess_board->get_transform() * m_translation * m_rotate);
	chess_board->walk_gl(picking, shader_id);
	Matrix4x4 inverted = (m_translation * m_rotate).invert();
	chess_board->set_transform(chess_board->get_transform() * inverted);
}
void Viewer::draw_black_board(bool picking, int shader_id)
{
	  glUniform1i(location_white_tile_texture, 5);
	if (picking)
		white_tile_shader->unbind();
	chess_board_black->set_transform(chess_board_black->get_transform() * m_translation * m_rotate);
	chess_board_black->walk_gl(picking, shader_id);
	Matrix4x4 inverted = (m_translation * m_rotate).invert();
	chess_board_black->set_transform(chess_board_black->get_transform() * inverted);
}

void Viewer::draw_luigi(bool picking, int shader_id)
{
	glUniform1f(location_ripple_speed, ripple_speed);
	glUniform1f(location_ripple_size, ripple_size);
  	glUniform1f(location_time_divider, water_flow_speed);
  	glUniform1f(location_transparency, 1);
  	glUniform1i(location_noise, 1);
  	glUniform1i(location_water_texture, 3);
  	glUniform1i(location_occlusion, 1);
  	glUniform1i(location_lighting, should_light);
	if (picking)
		piece_shader->unbind();
	luigi->set_transform(luigi->get_transform() * m_translation * m_rotate);
	luigi->walk_gl(picking, shader_id);
	Matrix4x4 inverted = (m_translation * m_rotate).invert();
	luigi->set_transform(luigi->get_transform() * inverted);
}

void Viewer::rotate_puppet(float fOldX, float fNewX, float fOldY, float fNewY)
{
    float  fRotVecX, fRotVecY, fRotVecZ;
    Matrix4x4 mNewMat;

	float fDiameter;
    int iCenterX, iCenterY;
    float fNewModX, fNewModY, fOldModX, fOldModY;

    int width = get_width();
    int height = get_height();

    /* vCalcRotVec expects new and old positions in relation to the center of the
     * trackball circle which is centered in the middle of the window and
     * half the smaller of nWinWidth or nWinHeight.
     */
	fDiameter = width < height ?
	  (float)width * 0.5 : (float)height * 0.5;
	iCenterX = width/ 2;
	iCenterY = height/ 2;
	fOldModX = fOldX - iCenterX;
	fOldModY = fOldY - iCenterY;
	fNewModX = fNewX - iCenterX;
	fNewModY = fNewY - iCenterY;

	vCalcRotVec(fNewModX, fNewModY,
					fOldModX, fOldModY,
					fDiameter,
					&fRotVecX, &fRotVecY, &fRotVecZ);
	/* Negate Y component since Y axis increases downwards
	 * in screen space and upwards in OpenGL.
	 */
	vAxisRotMatrix(fRotVecX, -fRotVecY, fRotVecZ, &mNewMat);
    m_rotate = mNewMat*m_rotate;
}

void Viewer::set_mode(MODE mode)
{
	m_mode = mode;
	if (mode == GAME_MODE) {
		camera_pos = Point3D(0,0,5);
		camera_dir = Vector3D(0,0,0);
	}
}


void Viewer::toggle_zbuffer()
{
	is_z_buffer = !is_z_buffer;
	if (is_z_buffer) {
		glEnable(GL_DEPTH_TEST);
	} else {
		glDisable(GL_DEPTH_TEST);
	}
//	this->invalidate();
}

void Viewer::set_options_menu(Gtk::Menu* options_menu)
{
	options = options_menu;
}

void Viewer::set_mode_menu(Gtk::Menu* mode_menu)
{
	mode = mode_menu;
}

void Viewer::set_label(Gtk::Label* label)
{
	feedback_label = label;
}

void Viewer::set_filename(std::string name)
{
	filename = name;
	//chess = import_lua("temp.lua");
	chess = import_lua(name);
	luigi = import_lua("../lua_scripts/luigi.lua");
	chess_board = import_lua("../lua_scripts/board.lua");
	chess_board_black = import_lua("../lua_scripts/board_black.lua");
	chess_fire = import_lua("../lua_scripts/pieces_fire.lua");
	puddle = import_lua("../lua_scripts/puddle.lua");
}
void Viewer::initialize()
{
	glewInit();
    glEnable(GL_TEXTURE_1D);
    glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	init_shaders();
	init_colour_texture(&colour_tex_id);
	init_simplex_texture(&simplex_tex_id);
	init_gradient_texture(&grad_new_tex_id);

	white_board_filename = "../textures/white_tile.bmp";
	black_board_filename = "../textures/black_tile.bmp";
	skybox_texture_filename = "../textures/skybox_texture_2.bmp";

	set_up_shadow_map();

    glActiveTexture(GL_TEXTURE3);
	set_texture("../textures/reflection.bmp", 256, 256);

	glActiveTexture(GL_TEXTURE4);
	set_white_tile_texture();

	glActiveTexture(GL_TEXTURE5);
	set_black_tile_texture();

	glActiveTexture(GL_TEXTURE6);
	set_fire_texture();

	set_skybox_texture();

	timer = 0;
	lava_flow_speed = 3000;
	water_flow_speed = 2000;
	ripple_size = 2;
	ripple_speed = 100;
	should_shadow = false;
	should_occlude = false;
	should_draw_luigi = false;
	should_draw_puddle = false;
	should_light = 1;
	puddle_pos = 19;

	light_pos = Point3D(33.64,-13.07,69.05);
	light_dir = Vector3D(0,0,-1000);
	camera_pos = Point3D(0,0,5);
	camera_dir = Vector3D(0,0,0);

	chess_game = new ChessGame();
	load_sounds();

	m_state = WHITE_SELECT_PIECE;

	timer_val = Glib::signal_timeout().connect(
	          sigc::mem_fun(*this, &Viewer::timer_tick), 10);

}

void Viewer::init_shaders()
{
	/**
	 * set up the shader for the chess pieces
	 */
	piece_shader->init("../shaders/noise-piece.vert", "../shaders/noise-piece.frag");
	white_tile_shader->init("../shaders/white-tile-shader.vert", "../shaders/white-tile-shader.frag");
	skybox_shader->init("../shaders/skybox.vert", "../shaders/skybox.frag");
	location_colour_texture = glGetUniformLocation( piece_shader->id(), "colour_texture" );
	location_simplex_texture = glGetUniformLocation( piece_shader->id(), "simplex_texture" );
	location_gradient_texture = glGetUniformLocation( piece_shader->id(), "gradient_texture" );
 	location_time = glGetUniformLocation( piece_shader->id(), "time" );
 	location_water_texture = glGetUniformLocation( piece_shader->id(), "water" );
 	location_white_tile_texture = glGetUniformLocation(white_tile_shader->id(), "white_texture");
 	location_transparency = glGetUniformLocation(piece_shader->id(), "transparency");
 	location_time_divider = glGetUniformLocation(piece_shader->id(), "time_divider");
 	location_noise = glGetUniformLocation(piece_shader->id(), "noise");
 	location_shadow_map_texture = glGetUniformLocation(white_tile_shader->id(), "shadow_map");
 	location_shadow_map_texture_piece = glGetUniformLocation(piece_shader->id(), "shadow_map");
 	location_ripple_size = glGetUniformLocation(piece_shader->id(), "ripple_size");
 	location_ripple_speed = glGetUniformLocation(piece_shader->id(), "ripple_speed");
 	location_occlusion = glGetUniformLocation(piece_shader->id(), "should_occlude");
 	location_lighting = glGetUniformLocation(piece_shader->id(), "should_light");
 	location_skybox_texture = glGetUniformLocation(skybox_shader->id(), "skybox_texture");
}

//NOTE:  This texture was taking direcly from Ken Perlins suggested permuatations
void Viewer::init_colour_texture(GLuint *texture_ID)
{
  char *pixels = (char*)malloc( 256*256*4 );;

  glGenTextures(1, texture_ID);
  glBindTexture(GL_TEXTURE_2D, *texture_ID);

  //generating the texture for perlin noise.  This is perlins suggested gradients
  for(int i = 0; i < 256; i++)
    for(int j = 0; j < 256; j++) {
      int offset = (i * 256 + j)*4;
      char colour_value = colours[(j+colours[i]) & 255];
      pixels[offset] = gradient_perlin[colour_value & 15][0] * 64 + 64;
      pixels[offset+1] = gradient_perlin[colour_value & 15][1] * 64 + 64;
      pixels[offset+2] = gradient_perlin[colour_value & 15][2] * 64 + 64;
      pixels[offset+3] = colour_value;
    }

  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
}

/**
 * NOTE:  This is not really a texture.  We are just importing the values for the simplex lookup table.
 * used for simplex noise
 */
void Viewer::init_simplex_texture(GLuint *texture_ID)
{
  glActiveTexture(GL_TEXTURE1);

  glGenTextures(1, texture_ID);
  glBindTexture(GL_TEXTURE_1D, *texture_ID);

  glTexImage1D( GL_TEXTURE_1D, 0, GL_RGBA, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, simplex_lookup );
  glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

  glActiveTexture(GL_TEXTURE0);
}

/*
 * Used for 4D noise only...if I need it.  These textures were taken directly from Gustovssen
 */
void Viewer::init_gradient_texture(GLuint *texture_ID)
{
  char *pixels = (char*)malloc( 256*256*4 );

  glActiveTexture(GL_TEXTURE2);

  glGenTextures(1, texture_ID);
  glBindTexture(GL_TEXTURE_2D, *texture_ID);

  for(int i = 0; i<256; i++)
    for(int j = 0; j<256; j++) {
      int offset = (i*256+j)*4;
      char colour_value = colours[(j+colours[i]) & 255];
      pixels[offset] = gradient_new[colour_value & 0x31][0] * 64 + 64;
      pixels[offset+1] = gradient_new[colour_value & 0x31][1] * 64 + 64;
      pixels[offset+2] = gradient_new[colour_value & 0x31][2] * 64 + 64;
      pixels[offset+3] = gradient_new[colour_value & 0x31][3] * 64 + 64;
    }

  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

  glActiveTexture(GL_TEXTURE0);
}

void Viewer::set_texture(const char *filename, int width, int height) {

	struct pixmap img;
	img.read(filename);
    glGenTextures( 1, &water_tex_id );
    glBindTexture( GL_TEXTURE_2D, water_tex_id );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
    		GL_MODULATE );
//
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
    		GL_LINEAR);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
    		GL_LINEAR);
//
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    //Generate the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.width, img.height, 0, GL_RGB,
    		GL_UNSIGNED_BYTE, img.data);
}

void Viewer::set_skybox_texture()
{
	glActiveTexture(GL_TEXTURE8);
	struct pixmap img;
	img.read(skybox_texture_filename.c_str());
	glGenTextures( 1, &skybox_tex_id[0] );
	glBindTexture( GL_TEXTURE_2D, skybox_tex_id[0] );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
			GL_MODULATE );
	//
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
			GL_LINEAR);
	//
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

	//Generate the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.width, img.height, 0, GL_RGB,
			GL_UNSIGNED_BYTE, img.data);
}

void Viewer::set_white_tile_texture()
{
	struct pixmap img;
	img.read(white_board_filename.c_str());
    glGenTextures( 1, &white_tile_tex_id );
    glBindTexture( GL_TEXTURE_2D, white_tile_tex_id );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
    		GL_MODULATE );
//
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
    		GL_LINEAR);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
    		GL_LINEAR);
//
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    //Generate the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.width, img.height, 0, GL_RGB,
    		GL_UNSIGNED_BYTE, img.data);
}

void Viewer::set_black_tile_texture()
{
	struct pixmap img;
	img.read(black_board_filename.c_str());
    glGenTextures( 1, &black_tile_id );
    glBindTexture( GL_TEXTURE_2D, black_tile_id );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
    		GL_MODULATE );
//
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
    		GL_LINEAR);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
    		GL_LINEAR);
//
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    //Generate the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.width, img.height, 0, GL_RGB,
    		GL_UNSIGNED_BYTE, img.data);
}

void Viewer::set_fire_texture()
{
	struct pixmap img;
	img.read("../textures/lava.bmp");
    glGenTextures( 1, &fire_tex_id );
    glBindTexture( GL_TEXTURE_2D, fire_tex_id );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
    		GL_MODULATE );
//
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
    		GL_LINEAR);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
    		GL_LINEAR);
//
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    //Generate the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.width, img.height, 0, GL_RGB,
    		GL_UNSIGNED_BYTE, img.data);
}

int Viewer::timer_tick()
{
	timer+=1;
	if (timer < 0)
		timer = 0;
	this->invalidate();
	return 1;
}

void Viewer::pick(int x, int y, SceneNode* node)
{
	GLint *vp = new GLint[4];
	GLuint buffer[512];

	glGetIntegerv(GL_VIEWPORT,vp);
	glSelectBuffer(512,buffer);
	glRenderMode(GL_SELECT);
	glInitNames();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	gluPickMatrix(x,vp[3]-y, 1,1,vp);
	gluPerspective(40.0, (GLfloat)vp[2]/(GLfloat)vp[3], 0.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (m_state == TEMP_PICK_WHITE){
		draw_white_board(true, white_tile_shader->id());
		m_state = MOVE_PIECE;
	}
	else if (m_state == TEMP_PICK_BLACK) {
		draw_black_board(true, white_tile_shader->id());
		m_state = MOVE_PIECE;
	}
	else if (m_state == WHITE_SELECT_PIECE) {
		draw_pieces(true, piece_shader->id());
	}
	else if (m_state == BLACK_SELECT_PIECE)
		draw_fire_pieces(true, piece_shader->id());
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glFlush();
	int hits = glRenderMode(GL_RENDER);
	if (hits != 0) {
		process(hits, buffer, node);
	}
}
//processHits
void Viewer::process(int hits, GLuint buffer[], SceneNode* scene_node)
{
	GLuint *ptr = buffer;
	GLuint total_ids;
	GLuint *min_ptr = ptr;
	unsigned int minZ = 0;
	unsigned int total_id_min = 0;
	//lets find which hit is the closest and take that stack
	for (int i = 0; i < hits; i++)
	{
		total_ids = *ptr;
		ptr++;
	//	std::cout << "minZ is " << *ptr << std::endl;
		if (minZ > *ptr || minZ == 0) {
			minZ = *ptr;
			min_ptr = ptr + 3;
			total_id_min = total_ids;
		}
		ptr++;
	//	std::cout << "maxZ is " << *ptr << std::endl;
		ptr++;
		ptr++;
		ptr += total_ids - 1;
	}
	SceneNode* node = scene_node;
	//here we traverse our DAG using the stack, to find the joint and the geo node
	for (unsigned int j = 0; j < total_id_min - 1; j++)
	{
		//std::cout << "traversing " << node->m_name << std::endl;
		unsigned int num = 0;
		for (SceneNode::ChildList::const_iterator children =
			  node->m_children.begin();children != node->m_children.end(); ++children)
		{
			if(num == *min_ptr) {
				node = *children;
				break;
			}
			num++;
		}
		min_ptr++;
	}

	if (m_state == WHITE_SELECT_PIECE || m_state == BLACK_SELECT_PIECE) {
		piece_select_pos = node->get_pos();
		selected_piece = node;
		prev_state = m_state;
		m_state = MOVE_PIECE;
		std::cout<<node->get_pos()<<std::endl;
	}
	else if (m_state == MOVE_PIECE) {
		ChessGame::MOVE_OUTCOMES outcome = chess_game->move(piece_select_pos, node->get_pos());
		//we get a message from the backend chess engine telling us the outcome of the move
		if (outcome == ChessGame::SUCCESS || outcome == ChessGame::CHECK ||
				outcome == ChessGame::CHECKMATE || outcome == ChessGame::TAKEN) {
			if (outcome == ChessGame::CHECK) {
				SM.Play(win_sound);
			}
			selected_piece->set_pos(move_piece(piece_select_pos, node->get_pos()));
			if (outcome == ChessGame::TAKEN || outcome == ChessGame::CHECK || outcome ==
					ChessGame::CHECKMATE) {
				//remove the opposing piece that is being taken
				take_piece(node->get_pos());
			}
			if (prev_state == WHITE_SELECT_PIECE) {
				m_state = BLACK_SELECT_PIECE;
			    SM.Play(water_sound);
			}
			else {
				m_state = WHITE_SELECT_PIECE;
				SM.Play(lava_sound);
			}
		} else {
			std::cout<<"unsuccessful move "<<std::endl;
			m_state = prev_state;
		}
	}
//	std::cout<<"starting new board draw:  *******************"<<std::endl;
//	chess_game->draw_board();
}

int Viewer::move_piece(int old_pos, int new_pos)
{
	int vertical_distance = (new_pos / 8) - (old_pos / 8);
	old_pos += vertical_distance*8;
	int horizontal_distance = new_pos - old_pos;
	old_pos += horizontal_distance;
	double ytrans = vertical_distance * 4.66;
	double xtrans = horizontal_distance * 5.05;
	Vector3D translate;
	if (selected_piece->m_name.find("knight") == 0) {
		translate = Vector3D(xtrans, 0, -ytrans);
	}
	else {
		translate = Vector3D(0, ytrans, xtrans);
	}
	selected_piece->translate(translate);
	return old_pos;
}

int Viewer::move_puddle(int old_pos, int new_pos)
{
	int vertical_distance = (new_pos / 8) - (old_pos / 8);
	old_pos += vertical_distance * 8;
	int horizontal_distance = new_pos - old_pos;
	old_pos += horizontal_distance;
	double ytrans = vertical_distance * 4.66;
	double xtrans = horizontal_distance * 5.05;
	Vector3D translate = Vector3D(ytrans, 0, xtrans);
	puddle->m_children.back()->translate(translate);
	return old_pos;
}

void Viewer::take_piece(int taking_pos)
{
	SceneNode* node;
	if (prev_state == WHITE_SELECT_PIECE)
		node = chess_fire;
	else
		node = chess;

	//find the child that has the taken position
	for (SceneNode::ChildList::const_iterator children =
		  node->m_children.begin();children != node->m_children.end(); ++children)
	{
		//I know I'm cheating by here by translating it rather then removing it
		//but for some reason removing it screwed up picking and I didn't bother
		//figuring it out
		if ( (*children)->get_pos() == taking_pos ) {
			(*children)->translate(Vector3D(4000,4000,4000));
			if (prev_state == BLACK_SELECT_PIECE) {
				puddle_pos = move_puddle(puddle_pos, taking_pos);
				should_draw_puddle = true;
			}
			break;
		}
	}
}

void Viewer::set_up_shadow_map()
{
	//we want to render our map in a higher resolution
	  int shadow_map_width = get_width() * 2;
	  int shadow_map_height = get_height() * 2;

	  glGenTextures(1, &depth_tex_id);
	  glBindTexture(GL_TEXTURE_2D, depth_tex_id);


	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

	  glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_map_width, shadow_map_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
	  glBindTexture(GL_TEXTURE_2D, 0);

	  glGenFramebuffersEXT(1, &frame_tex_id);
	  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_tex_id);

	  glDrawBuffer(GL_NONE);
	  glReadBuffer(GL_NONE);

	  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,GL_TEXTURE_2D, depth_tex_id, 0);

	  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void Viewer::set_up_mapper_texture()
{
	static double projection[16];
	static double model_view[16];

	//maps from the unit cube to 0,1
	//fixes some stupid artifacts in the shadowing
	//people online suggested these numbers
	const GLdouble mapper[16] = {
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0};

	glGetDoublev(GL_MODELVIEW_MATRIX, model_view);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);


	glMatrixMode(GL_TEXTURE);
	glActiveTexture(GL_TEXTURE7);

	glLoadIdentity();
	glLoadMatrixd(mapper);

//	for (int i = 0; i<4; i++) {
//		for (int j = 0; j<4; j++) {
//			std::cout<<model_view[i*j] << "    ";
//		}
//		std::cout<<std::endl;
//	}

	glMultMatrixd (projection);
	glMultMatrixd (model_view);

	// Go back to normal matrix mode
	glMatrixMode(GL_MODELVIEW);

}

void Viewer::set_up_for_drawing(Point3D pos, Vector3D look, bool is_shadow)
{
	  glMatrixMode(GL_PROJECTION);
	  glLoadIdentity();
	  int mult = 1;
	  if (is_shadow)
		  mult = 2;
	  gluPerspective(40.0, (GLfloat)get_width()/(GLfloat)get_height(), 0.1, 10000.0);

	  // change to model view for drawing
	  glMatrixMode(GL_MODELVIEW);
	  glLoadIdentity();
	  gluLookAt(pos[0],pos[1],pos[2],look[0],look[1],look[2],0,1,0);
//	  gluLookAt(0,0,5,0,0,0,0,1,0);
}

void Viewer::set_board_texture(BOARD_TEXTURE board_texture)
{
	if (board_texture == BLACK_WHITE_MARBLE) {
		white_board_filename = "../textures/white_tile.bmp";
		black_board_filename = "../textures/black_tile.bmp";
	}
	else if (board_texture == WOOD) {
		white_board_filename = "../textures/white_wood_tile.bmp";
		black_board_filename = "../textures/black_wood_tile.bmp";
	}
	else if (board_texture == METAL) {
		white_board_filename = "../textures/white_metal_tile.bmp";
		black_board_filename = "../textures/black_metal_tile.bmp";
	}
	glActiveTexture(GL_TEXTURE4);
	set_white_tile_texture();
	glActiveTexture(GL_TEXTURE5);
	set_black_tile_texture();
}

void Viewer::set_lava_flow(FLOW_SPEED flow_speed)
{
	if (flow_speed == SLOW) {
		lava_flow_speed = 3000;
	}
	else if (flow_speed == MEDIUM) {
		lava_flow_speed = 2000;
	}
	else if (flow_speed == FAST) {
		lava_flow_speed = 1000;
	}

}

void Viewer::set_water_flow(FLOW_SPEED flow_speed)
{
	if (flow_speed == SLOW) {
		water_flow_speed = 2000;
	}
	else if (flow_speed == MEDIUM) {
		water_flow_speed = 1000;
	}
	else if (flow_speed == FAST) {
		water_flow_speed = 500;
	}

}

void Viewer::set_water_ripple(FLOW_SPEED water_setting)
{
	if (water_setting == SLOW) {
		ripple_speed = 100;
	}
	else if (water_setting == MEDIUM) {
		ripple_speed = 70;
	}
	else if (water_setting == FAST) {
		ripple_speed = 50;
	}
	else if (water_setting == SMALL_RIPPLE) {
		ripple_size = 3;
	}
	else if (water_setting == MEDIUM_RIPPLE) {
		ripple_size = 2;
	}
	else if (water_setting == BIG_RIPPLE) {
		ripple_size = 1;
	}
}

void Viewer::toggle_shadow()
{
	should_shadow = !should_shadow;
}

void Viewer::draw_skybox()
{
	glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);

   // glColor4f(1,1,1,1);

    skybox_shader->bind();
    glFrontFace(GL_CW);

//    glActiveTexture(GL_TEXTURE8);
//    glBindTexture(GL_TEXTURE_2D, skybox_tex_id[0]);

    double eps = 0.0001;

    glUniform1i(location_skybox_texture, 8);
    glBegin(GL_QUADS);
		glTexCoord2f(0.25, (float)1/3 + eps);
        glVertex3f(  SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE );
        glTexCoord2f(0.5, (float)1/3 + eps);
        glVertex3f( -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE );
        glTexCoord2f(0.5, (float)2/3 - eps);
        glVertex3f( -SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE );
        glTexCoord2f(0.25, (float)2/3 - eps);
        glVertex3f(  SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE );
    glEnd();

//    glActiveTexture(GL_TEXTURE10);
//    glBindTexture(GL_TEXTURE_2D, skybox_tex_id[2]);
    glUniform1i(location_skybox_texture, 8);
    glBegin(GL_QUADS);
		glTexCoord2f(0, (float)1/3 + eps);
        glVertex3f(  SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE );
        glTexCoord2f(0.25, (float)1/3 + eps);
        glVertex3f(  SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE );
        glTexCoord2f(0.25, (float)2/3 - eps);
        glVertex3f(  SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE );
        glTexCoord2f(0, (float)2/3 - eps);
        glVertex3f(  SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE );
    glEnd();

//    glActiveTexture(GL_TEXTURE9);
//    glBindTexture(GL_TEXTURE_2D, skybox_tex_id[1]);
    glUniform1i(location_skybox_texture, 8);
    glBegin(GL_QUADS);
		glTexCoord2f(0.75, (float)1/3 + eps);
		glVertex3f( -SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE );
		glTexCoord2f(1, (float)1/3 + eps);
		glVertex3f(  SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE );
		glTexCoord2f(1, (float)2/3 - eps);
		glVertex3f(  SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE );
		glTexCoord2f(0.75, (float)2/3 - eps);
		glVertex3f( -SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE );
    glEnd();

//    glActiveTexture(GL_TEXTURE11);
//    glBindTexture(GL_TEXTURE_2D, skybox_tex_id[3]);
    glUniform1i(location_skybox_texture, 8);
    glBegin(GL_QUADS);
		glTexCoord2f(0.5, (float)1/3 + eps);
        glVertex3f( -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE );
        glTexCoord2f(0.75, (float)1/3 + eps);
        glVertex3f( -SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE );
        glTexCoord2f(0.75, (float)2/3 - eps);
        glVertex3f( -SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE );
        glTexCoord2f(0.5, (float)2/3 - eps);
        glVertex3f( -SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE );
    glEnd();

//    glActiveTexture(GL_TEXTURE10);
//    glBindTexture(GL_TEXTURE_2D, skybox_tex_id[2]);
    glUniform1i(location_skybox_texture, 8);
    glBegin(GL_QUADS);
		glTexCoord2f(0.5, (float)2/3 + eps);
        glVertex3f( -SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE );
		glTexCoord2f(0.5, 1);
        glVertex3f( -SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE );
        glTexCoord2f(0.25, 1);
        glVertex3f(  SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE );
        glTexCoord2f(0.25, (float)2/3 + eps);
        glVertex3f(  SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE );
    glEnd();

//    glActiveTexture(GL_TEXTURE12);
//    glBindTexture(GL_TEXTURE_2D, skybox_tex_id[4]);
    glFrontFace(GL_CCW);
    glUniform1i(location_skybox_texture, 8);
    glBegin(GL_QUADS);
		glTexCoord2f(0.25, 0);
        glVertex3f( -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE );
        glTexCoord2f(0.5, 0);
        glVertex3f( -SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE );
        glTexCoord2f(0.5, (float)1/3 + eps);
        glVertex3f(  SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE );
        glTexCoord2f(0.25, (float)1/3 + eps);
        glVertex3f(  SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE );
    glEnd();

    glFrontFace(GL_CCW);
    skybox_shader->unbind();
}

void Viewer::load_sounds()
{
	water_sound = SM.LoadWav("../sounds/water.wav");
	lava_sound = SM.LoadWav("../sounds/lava.wav");
	roll_sound = SM.LoadWav("../sounds/rickroll.wav");
	classical_sound = SM.LoadWav("../sounds/classical.wav");
	bit_sound = SM.LoadWav("../sounds/bit.wav");
	win_sound = SM.LoadWav("../sounds/moo.wav");

	water_source = SM.MakeSource();
	lava_source = SM.MakeSource();
	roll_source = SM.MakeSource();
	classical_source = SM.MakeSource();
	bit_source = SM.MakeSource();
	win_source = SM.MakeSource();

    SM.QueueBuffer(water_source, water_sound);
    SM.QueueBuffer(lava_source, lava_sound);
    SM.QueueBuffer(roll_source, roll_sound);
    SM.QueueBuffer(classical_source, classical_sound);
    SM.QueueBuffer(bit_source, bit_sound);
    SM.QueueBuffer(win_source, win_sound);

    SM.SetLoop(classical_sound, true);
    SM.SetLoop(bit_sound, true);
    SM.SetLoop(roll_sound, true);

    SM.Play(bit_sound);
}

void Viewer::reset_view()
{
	m_translation = Matrix4x4();
	m_rotate = Matrix4x4();
	camera_pos = Point3D(0,0,5);
	camera_dir = Vector3D(0,0,0);
}

void Viewer::toggle_occlusion()
{
	should_occlude = !should_occlude;
}

void Viewer::toggle_lighting()
{
	if (should_light == 0) {
		should_light = 1;
	} else {
		should_light = 0;
	}
}

void Viewer::toggle_luigi()
{
	should_draw_luigi = !should_draw_luigi;
}

void Viewer::set_sound(SOUND sound)
{
	if (sound == CLASSICAL) {
		SM.Stop(bit_sound);
		SM.Stop(roll_sound);
		SM.Play(classical_sound);
	} else if (sound == BIT) {
		SM.Stop(classical_sound);
		SM.Stop(roll_sound);
		SM.Play(bit_sound);
	} else if (sound == ROLL) {
		SM.Stop(classical_sound);
		SM.Stop(bit_sound);
		SM.Play(roll_sound);
	}
}

void Viewer::set_skybox(SKYBOX_TEXTURE skybox)
{
	if (skybox == SKYBOX1) {
		skybox_texture_filename = "../textures/skybox_texture_2.bmp";
	} else if (skybox == SKYBOX2) {
		skybox_texture_filename = "../textures/skybox_texture_3.bmp";
	} else if (skybox == SKYBOX3) {
		skybox_texture_filename = "../textures/skybox_texture_4.bmp";
	} else if (skybox == SKYBOX4) {
		skybox_texture_filename = "../textures/skybox_texture_5.bmp";
	}
	glActiveTexture(GL_TEXTURE8);
	set_skybox_texture();
}

void Viewer::new_game()
{
	chess_game = new ChessGame();
	set_filename("pieces.lua");
	should_draw_puddle = false;
	should_draw_luigi = false;
	m_state = WHITE_SELECT_PIECE;
}
