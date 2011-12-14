
// pixmap.h
typedef char byte;
typedef unsigned char ubyte;

struct pixmap
{
	unsigned width,
					 height;
	ubyte *data;

	enum {RED = 0, GREEN, BLUE};

	pixmap(const char *fname);
	pixmap();
	~pixmap();

	void read(const char *fname);
	ubyte pixel_elem(int x, int y, int elem);
	ubyte *pixel_pos(int x, int y);
};
