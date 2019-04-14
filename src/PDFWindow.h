#ifndef PDFWINDOW_H
#define PDFWINDOW_H 1
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Widget.H>
#include <stdlib.h>
#include <mupdf/pdf.h>
#include <string.h>
#include <limits.h>

// EDIT THIS TO GET MORE PAGES ON THE SCREEN AT ONE TIME!
#define MAX_BUFFERS	6

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

//mupdf

#define GAP 3
#define PAGE_SKIP	2

class PDFWindow : public Fl_Box {
public:
    PDFWindow(int x,int y,int w,int h,const char *l=0);
	int				initPDF();
	int				closePDF();
	int				openPDF(char *fname);
	void			setFull(int makefull);
 	int				IS_FULL;
private:
	int				handle();	// override
	int				resize();	// override
	Fl_Offscreen	offscreen_buffer;
    void			draw();    
	int				xd; // display box x location
	int				yd; // display box y location
	char			szlabel[256]; // window label
	int				mx;	// mouse location relative to window
	int				my;	// mouse location relative to window
	void			resize(int x, int y, int w, int h);
	int				handle(int msg);
	int				hasfocus;
	// mupdf things
	// the main context for this session
	fz_context		*ctx; 
	// current document params 
	fz_document		*doc;
	fz_colorspace	*colorspace;
	char			*docpath;
	char			*doctitle;
	int				pagecount;
	int				resolution;
	int				res_changed_flag;
	int				panx;
	//CSapp parameters
	int				errored;
	// ring array for pages
	int				activepagenumber;
	int				activepageindex;
	fz_pixmap		*image[MAX_BUFFERS]; 
	fz_page			*page[MAX_BUFFERS];
	fz_rect			page_bbox[MAX_BUFFERS];
	fz_display_list	*page_list[MAX_BUFFERS]; 
	int				incomplete[MAX_BUFFERS];
	int				pany[MAX_BUFFERS];
	// client GUI/window system
	int				winw, winh;  // the window size
	int				displayed_page_number; // just for show on the title bar
	int				initshow;
	int				fullscreen;

	// window panning
	int				IN_PAN;
	int				selx, sely;

	int				IN_COMMAND;
	int				IN_ZOOM;
	int				CSapp_zoom_in(int oldres);
	int				CSapp_zoom_out(int oldres);
	int				CSapp_init();
	int				CSapp_open(char *filename, int reload);
	int				CSapp_loadpage(int bufferno, int pageno);
	int				CSapp_renderpage(int bufferno);
	void			CSapp_droppage(int bufferno);
	int				CSapp_masterloader(int bufferno, int pageno);
	int				CSapp_get_buffer_at(int x, int y);
	void			CSapp_drawcanvas(int startpage, int forcetotop);
	void			CSapp_masterpanner(int deltax, int deltay, int redrawflag);
	void			CSapp_close();
	int				CSapp_preclose();
	void			CSapp_reloadpage();
	void			CSapp_onresize(int w, int h);
	void			CSapp_handlezoom(int x, int y, int dir);
	void			CSapp_hor_zoom(int idx); 
	void			CSapp_vert_zoom(int idx); 
	void			CSapp_handlescroll(int modifiers, int dir);
	void			CSapp_onkey(int c);
	void			CSapp_dragmouse(int x, int y);
	void			CSapp_winrepaint();
};

#endif

