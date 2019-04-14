// keyboard commands
static const char helpstr[] = {
		"LEFT	scroll left; also SHIFT+Vert mouse wheel and Hor mouse wheel\n"
		"RIGHT	scroll right; also SHIFT+Vert mouse wheel and Hor mouse wheel\n"
		"UP 	scroll down; also Vert mouse wheel\n"
		"DN 	scroll upt; also Vert mouse wheel\n"
		"\n"
		"+ =	zoom in; also CTRL+Vert mouse wheel\n"
		"- _	zoom out; also CTRL+Vert mouse wheel\n"
		"W w	zoom to window Width\n"
		"H h	zoom to window Height\n"
		"F f	zoom to Fit page\n"
		"\n"
		"PGDN	next page\n"
		"PGUP	previous page\n"
		">  	next 10 pages\n"
		"<  	back 10 pages\n"
};
#include "PDFWindow.h"
#include "RETURN.xpm"

// detented zoom factors - this range seems to be reasonable on my screen
static int zoomlist[] = { 36, 54, 72, 96, 120, 144, 180, 216, 288 };
#define MIN_ZOOM	zoomlist[0]
#define MAX_ZOOM	zoomlist[nelem(zoomlist)-1]

PDFWindow::PDFWindow(int x,int y,int w,int h,const char *l)
            : Fl_Box(x,y,w,h,l)
{
	xd = x;
	yd = y;
	winw = w;
	winh = h;
	szlabel[0] = 0;
	mx = 0;
	my = 0;
	hasfocus = 0;
	IN_PAN = 0;
	IN_ZOOM = 0;
	IN_COMMAND = 0;
	IS_FULL = 0;
}
void PDFWindow::resize(int x, int y, int w, int h) 
{
	xd = x;
	yd = y;
	winw = w;
	winh = h;
	Fl_Box::resize(x,y,w,h);	// call parent
	if (doc) CSapp_onresize(winw, winh);
}
void PDFWindow::setFull(int makelarge) 
{
	int wp, hp;
	long l;
	wp = ((Fl_Window *)(this->parent()))->w();
	hp = ((Fl_Window *)(this->parent()))->h();

	if (makelarge) {
		resize(0,0,wp,hp);
		hasfocus = 1;
		IS_FULL = 1;
	} else {
		IS_FULL = 0;
		hasfocus = 0;
		// blindly fish around parent's children and display them
		Fl_Widget *wid;
		for (int i=0; i<((Fl_Window *)(this->parent()))->children(); i++) {
			wid = (Fl_Widget *)(this->parent()->child(i));
			l = (long)(wid->user_data());
			if ((char)l == 'S') wid->show();
			if ((char)l == 'B') wid->show();
			if ((char)l == 'I') wid->show();
		}
		resize(385,4,wp-385,hp-8);
	}
	this->top_window()->redraw();
}
int PDFWindow::handle(int msg) 
{
	int b,c;
	int s;
	int modifier;

	if (msg == FL_ENTER) {
		Fl::focus(this);
		hasfocus = 1;
		return(1);
	}
	if (msg == FL_LEAVE) {
		hasfocus = 0;
	}
	if (hasfocus == 0) return 0;

	c = 0;
	s = Fl::event_state();

	modifier = 0;
	if (s & FL_SHIFT) modifier = 1;
	if (s & FL_CTRL) modifier |= (1<<2);

	// push has happened inside the window
	if (msg == FL_PUSH) {
		if (s & FL_BUTTON1) {
			if (IS_FULL) {
				if ((Fl::e_x < 29) && (Fl::e_y < 29)) {
					setFull(0);
					return(1);
				}
			}
			mx = Fl::e_x-xd;
			my = Fl::e_y-yd;
			if (doc) CSapp_dragmouse(mx, my);
			return(1); // we used this
		}
	} 
	// if there is no document, don't process window messages from here on
	if (doc == NULL) return (1);

	if (msg == FL_DRAG) {
		mx = Fl::e_x-xd;
		my = Fl::e_y-yd;
		CSapp_dragmouse(mx, my);
		return(1); // we used this
	}
	// this only fires if PUSH was initiated inside the window
	if (msg == FL_RELEASE) {
		// stop panning if applicable
		IN_PAN = 0;
		IN_COMMAND = 0;
		return(1); // we used this
	}

	if (msg==FL_MOUSEWHEEL){
		if (IN_COMMAND) return(1);
		IN_COMMAND = 1;
		mx = Fl::e_x-xd;
		my = Fl::e_y-yd;
		// if control key is pressed, then wheel handles zoom
		if (modifier == (1<<2)) {
			// flag the zoom to prevent too many events building up
			if (IN_ZOOM == 0) {
				if (Fl::e_dy > 0) CSapp_handlezoom(mx, my, -1);
				else CSapp_handlezoom(mx, my, 1);
			}
		} else {
			// vertical scroll wheel handling only.  Do horizontal scroll with SHIFT+wheel
			if (Fl::e_dy > 0) CSapp_handlescroll(modifier, -1);
			else if (Fl::e_dy < 0) CSapp_handlescroll(modifier, 1);
			else if (Fl::e_dx < 0) CSapp_handlescroll(1, 1);	// if mouse happens to return a dx
			else if (Fl::e_dx > 0) CSapp_handlescroll(1, -1);
		}
		IN_COMMAND = 0;
		return (1);// we used this
	}

  if (msg==FL_KEYBOARD) {
	b=Fl::event_key();
	switch(b) {
		case FL_Down: c = 0x28; break;	// scroll down
		case FL_Up: c = 0x26; break;	// scroll up
		case FL_Left: c = 0x25; break;	// scroll left
		case FL_Right: c = 0x27; break; // scroll right
		case FL_Page_Up: c = 0x21; break;	// pgup
		case FL_Page_Down: c = 0x22; break;	// pgdn
		case FL_Home: c = 0x24; break;	// home
		case FL_End: c = 0x23; break;	// end
		case 46: c = '>'; break;
		case 44: c = '<'; break;
		case 47: 
			if (modifier == 1) {
				fl_message(helpstr);
				return(1);
			}
			break;
		default: 
			if (b < 256) 
				c=b;
	}
	if (c != 0) {
		CSapp_onkey(c);
		return(1); // we used this

	}
	return (1); // absorb all keyboard events
  }
  return 0; // yield events to other windows
}

int PDFWindow::initPDF() 
{
	return (CSapp_init());
}

int PDFWindow::closePDF() 
{
	if (CSapp_preclose()) {
		CSapp_close();
		fz_drop_context(ctx);
	}
	return 0;
}
int PDFWindow::openPDF(char *fname) 
{
	int retval;
	retval = CSapp_open(fname, 0);
	((Fl_Window *)(this->parent()))->redraw();
	return(retval);
}
void PDFWindow::draw()
{
	CSapp_winrepaint();
}

//=====FROM CSapp======================

int PDFWindow::CSapp_zoom_in(int oldres) 
{
	unsigned long i;
	for (i = 0; i < nelem(zoomlist) - 1; ++i)
		if (zoomlist[i] <= oldres && zoomlist[i+1] > oldres)
			return zoomlist[i+1];
	return zoomlist[i];
}

int PDFWindow::CSapp_zoom_out(int oldres) 
{
	unsigned long i;
	for (i = 0; i < nelem(zoomlist) - 1; ++i)
		if (zoomlist[i] < oldres && zoomlist[i+1] >= oldres)
			return zoomlist[i];
	return zoomlist[0];
}

int PDFWindow::CSapp_init() 
{
	int i;
	resolution = 89;
	doc = NULL;
	docpath = NULL;
	doctitle = NULL;
	ctx = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
	if (!ctx) return 1; // cannot initialize mupdf context

	for (i=0; i<MAX_BUFFERS; i++) {
		image[i] = NULL;
		page[i] = NULL;
		page_list[i] = NULL;
	}
	IN_COMMAND = 0;

#ifdef _WIN32
	colorspace = fz_device_bgr(ctx);
#else
	colorspace = fz_device_rgb(ctx);
#endif
	return 0;
}

int PDFWindow::CSapp_open(char *filename, int reload)
{
	int i;
	int retval;

	retval = 0;
	fz_try(ctx)
	{
		fz_register_document_handlers(ctx);
		// do not use CSS
		fz_set_use_document_css(ctx, 0);
		doc = fz_open_document(ctx, filename);
	}
	fz_catch(ctx)
	{
		if (!reload) {
			fl_alert("cannot open document");
			retval = 1;
		}
	}

	fz_try(ctx)
	{
		if (fz_needs_password(ctx, doc))
			fz_throw(ctx, FZ_ERROR_GENERIC, "Needs a password");
		if (doc->layout)
			fz_throw(ctx, FZ_ERROR_GENERIC, "Document is not a standard PDF");
		pagecount = fz_count_pages(ctx, doc);
		if (pagecount <= 0)
			fz_throw(ctx, FZ_ERROR_GENERIC, "No pages in document");

		// extract filename from path
		docpath = fz_strdup(ctx, filename);
		doctitle = filename;
		if (strrchr(doctitle, '\\'))
			doctitle = strrchr(doctitle, '\\') + 1;
		if (strrchr(doctitle, '/'))
			doctitle = strrchr(doctitle, '/') + 1;
		doctitle = fz_strdup(ctx, doctitle);
	}
	fz_catch(ctx)
	{
		fl_alert("cannot open document");
		retval = 1;
	}
	activepagenumber = 0;
	displayed_page_number = -1;
	if (resolution < MIN_ZOOM)
		resolution = MIN_ZOOM;
	if (resolution > MAX_ZOOM)
		resolution = MAX_ZOOM;
	res_changed_flag = 1;

	if (!reload)
	{
		IN_COMMAND = 0;
		// initialize ring buffer
		for (i=0; i<MAX_BUFFERS; i++) {
			image[i] = NULL;
			page[i] = NULL;
			page_list[i] = NULL;
		}
		panx = 0;
		// lay out new canvas
		initshow = 1;	// first pass
		CSapp_drawcanvas(0, 1);
		CSapp_masterpanner(0,0,0);
		((Fl_Window *)(this->parent()))->redraw();
	}
	return retval;
}

// load the page into a ring buffer slot
int PDFWindow::CSapp_loadpage(int bufferno, int pageno)
{
	fz_device *mdev = NULL;
	int errored = 0;
	fz_cookie cookie = { 0 };

	// temporary bounding box
	page_bbox[bufferno].x0 = 0;
	page_bbox[bufferno].y0 = 0;
	page_bbox[bufferno].x1 = 100;
	page_bbox[bufferno].y1 = 100;

	incomplete[bufferno] = 0;

	fz_try(ctx)
	{
		page[bufferno] = fz_load_page(ctx, doc, pageno);
		page_bbox[bufferno] = fz_bound_page(ctx, page[bufferno]);
	}
	fz_catch(ctx)
	{
		fl_alert("Cannot load page");
		return -1;
	}

	fz_var(mdev);
	fz_try(ctx)
	{
		// Create display lists 
		page_list[bufferno] = fz_new_display_list(ctx, fz_infinite_rect);
		mdev = fz_new_list_device(ctx, page_list[bufferno]);
		cookie.incomplete_ok = 1;
		fz_run_page_contents(ctx, page[bufferno], mdev, fz_identity, &cookie);
		fz_close_device(ctx, mdev);
		fz_drop_device(ctx, mdev);
		mdev = NULL;
	}
	fz_always(ctx)
	{
		fz_drop_device(ctx, mdev);
	}
	fz_catch(ctx)
	{
		fl_alert("Cannot load page");
		errored = 1;
	}

	if (cookie.errors && errored == 0) //is this correct?
	{
		errored = 1;
		fl_alert("Errors found on page. Page rendering may be incomplete.");
		fz_flush_warnings(ctx);
		return -1;
	}

	fz_flush_warnings(ctx);
	return bufferno;
}

// render the page with given resolution the to an image
int PDFWindow::CSapp_renderpage(int bufferno)
{
	fz_device *idev = NULL;
	fz_matrix ctm;
	fz_irect ibounds;
	fz_rect bounds;
	fz_cookie cookie = { 0 };

	ctm = fz_transform_page(page_bbox[bufferno], resolution, 0);
	bounds = fz_transform_rect(page_bbox[bufferno], ctm);
	ibounds = fz_round_rect(bounds);
	bounds = fz_rect_from_irect(ibounds);

	// delete pixmap if already exists
	if (image[bufferno]) fz_drop_pixmap(ctx, image[bufferno]);
	image[bufferno] = NULL;
	fz_var(image[bufferno]);
	fz_var(idev);

	fz_try(ctx)
	{
		image[bufferno] = fz_new_pixmap_with_bbox(ctx, colorspace, ibounds, NULL, 1);
		fz_clear_pixmap_with_value(ctx, image[bufferno], 255);
		if (page_list[bufferno])
		{
			idev = fz_new_draw_device(ctx, fz_identity, image[bufferno]);
			// runpage
			if (page_list[bufferno])
				fz_run_display_list(ctx, page_list[bufferno], idev, ctm, bounds, &cookie);
			fz_close_device(ctx, idev);
		}
	}
	fz_always(ctx)
		fz_drop_device(ctx, idev);
	fz_catch(ctx)
		cookie.errors++;

	if (cookie.errors && errored == 0) //is this correct?
	{
		errored = 1;
		fl_alert("Errors found on page. Page rendering may be incomplete.");
		return 0;
	}

	fz_flush_warnings(ctx);
	return 1;
}
// delete buffer slot and free memory
void PDFWindow::CSapp_droppage(int bufferno)
{
	if (image[bufferno]) {
		fz_drop_pixmap(ctx, image[bufferno]);
		image[bufferno] = NULL;
		fz_drop_page(ctx, page[bufferno]);
		page[bufferno] = NULL;
		fz_drop_display_list(ctx, page_list[bufferno]);
		page_list[bufferno] = NULL;
	}
}
// loads and renders a page into the buffer slot
int PDFWindow::CSapp_masterloader(int bufferno, int pageno)
{
	int r, i;

	this->top_window()->cursor(FL_CURSOR_WAIT);
	//cursor(FL_CURSOR_WAIT);

	// load the page
	i = CSapp_loadpage(bufferno, pageno);
	if (i < 0) return 0;
	// render the page
	r = CSapp_renderpage(i);
	// failed to draw - delete it
	if (r == 0)
	{
		// delete 
		CSapp_droppage(i);
		return -1;
	}
	return i;
}

// get the buffer for the page at the location.  Extend the page height by the GAP just in case you 
// pick on the gap.
int PDFWindow::CSapp_get_buffer_at(int x, int y)
{
	int i;
	for (i=0; i<MAX_BUFFERS; i++) {
		if (image[i]) {
			if ((panx <= x) && (x < (panx + fz_pixmap_width(ctx, image[i])))) {
				if ((pany[i] <= y) && 
					(y < (pany[i] + fz_pixmap_height(ctx, image[i]) + GAP)))
					return i;
			}
		}
	}
	return -1;
}
// fills the screen with buffer images. Deletes or adds pages if required
// forcetotop: forces the start page to the top of the screen, otherwise the
// top page will be drawn at its y position.  If there is a gap a the top, it will
// be filled with the previous page.
void PDFWindow::CSapp_drawcanvas(int startpage, int forcetotop)
{
	int image_h = 0;
	int idx, itry, istart;
	int yoffset;

	yoffset = 0; // default is to start at top
	istart = -1; 
	if (initshow)
		istart = 0;
	else {
		// search for the given page in the ring
		for (idx=0; idx<MAX_BUFFERS; idx++) {
			if (image[idx]) {
				if (page[idx]->number == startpage) {
					// start here
					istart = idx;
					if (!forcetotop) yoffset = pany[idx];
					break;
				}
			}
		}
		// no luck? try the next highest page number
		if (istart == -1) {
			for (idx=0; idx<MAX_BUFFERS; idx++) {
				if (image[idx]) {
					if ((page[idx]->number == (startpage +1)) && (istart == -1)) {
						// this is one higher so there may be a sequence in memory from here on
						// start the ring buffer in the previous slot
						istart = (idx + MAX_BUFFERS - 1) % MAX_BUFFERS; 
						if (!forcetotop) yoffset = pany[idx];
						break;
					}
				}
			}
		}
	}
	// if no existing slot, then start all over.  Memory will reallocated
	if (istart == -1) istart = 0;

	// first, fill gap at the top of the page by loading BACKWARD from start
	// step backwards through each slot and fill the slots until the top of the
	// canvas is covered
	if ((yoffset > 0) && (startpage != 0)) {
		int newstart = -1;
		int newstartpage = -1;
		for (idx=1; idx<MAX_BUFFERS; idx++) {
			// stepping backwards through ring!
			itry = (istart + MAX_BUFFERS - idx) % MAX_BUFFERS;
			if (image[itry]) {  // page exists in memory
				// if page number isn't what we want, drop the page
				if (page[itry]->number != (startpage - idx)) {
					CSapp_droppage(itry);
				} 
			} 
			// load the page if the slot is empty
			if (image[itry] == NULL) {
				if (itry != CSapp_masterloader(itry, startpage-idx)) break;
			}
			// finish processing
			if (image[itry]) {
				// insert the new page above the previous one
				image_h = fz_pixmap_height(ctx, image[itry]);
				pany[itry] = yoffset - GAP - image_h; 
				newstart = itry;
				newstartpage = page[itry]->number;
				yoffset = pany[itry];
				// if this image extends beyond the top screen or we reached page 0, stop loading images
				if ((pany[itry] < 0) || (newstartpage == 0)) break;
			}
		}
		if (newstart >= 0) istart = newstart;
		if (newstartpage >=0) startpage = newstartpage;
	}
	// now step forward from the start location and load FORWARD.
	for (idx=0; idx<MAX_BUFFERS; idx++) {
		itry = (istart + idx) % MAX_BUFFERS;
		if (image[itry]) {  // page exists in memory
			// if page number isn't what we want, drop the page
			if (page[itry]->number != (startpage + idx)) {
				CSapp_droppage(itry);
			} 
		} 
		// load the page if the slot is empty
		if (image[itry] == NULL) {
			if (itry != CSapp_masterloader(itry, startpage+idx)) break;
		}
		// finish processing
		if (image[itry]) {
			// the first page of the list goes to the top
			pany[itry] = yoffset; // move first page to top of the screen
			image_h = fz_pixmap_height(ctx, image[itry]);
			// if this image extends beyond the screen, stop loading images
			if ((pany[itry] + image_h + GAP) > winh) break;
			yoffset = pany[itry] + image_h + GAP;
		}
		if (page[itry]->number == (pagecount-1)) break;
	} 
	// drop the unused ones
	for (itry=(idx+1); itry<MAX_BUFFERS; itry++) {
		if (image[(istart + itry)%MAX_BUFFERS]) 
			CSapp_droppage((istart + itry)%MAX_BUFFERS);
	}

	if (initshow)
	{
		initshow = 0;
		// fist display of window //??page 0 is assumed
		//redraw(); 
	} 
	activepagenumber = page[istart]->number;
	activepageindex = istart;
}
void PDFWindow::CSapp_masterpanner(int deltax, int deltay, int redrawflag) 
{
	int i;
	int image_w = 0;
	int image_h;
	int maxw = 0;
	int newx = panx + deltax;
	int itop = -1;		// the buffer number of the top page
	int ibottom = -1;	// the buffer number of the bottom page
	int ipage0 = -1;    // the buffer number of page 0
	int ilastpage = -1; // the buffer number of last page
	int canbump = 1;	// can bump the last page down to the bottom window border
	int newcanvasflag;	// triggers regenerating the canvas
	int v1, v2;			// general purpose

	if (pagecount == 0) return;
	newcanvasflag = 0;
	// get the overall width and the page with the minimum y from each page
	v2 = pagecount;
	v1 = -1;
	for (i=0; i<MAX_BUFFERS; i++)
	{
		if (image[i]){
			image_w = fz_pixmap_width(ctx, image[i]);
			maxw = MAX(image_w, maxw);
			if (page[i]->number > v1) {ibottom = i; v1 = page[i]->number;}
			if (page[i]->number < v2) {itop = i; v2 = page[i]->number;}
			if (page[i]->number == 0) ipage0 = i;
			if (page[i]->number == (pagecount-1)) ilastpage = i;
		}
	}

	// y pans 
	// y of page zero cannot go positive unless all pages can fit in screen
	if (ipage0 >= 0) {
		int overallht;
		// if all pages fit on screen
		if (ilastpage >= 0) {
			image_h = fz_pixmap_height(ctx, image[ilastpage]);
			// if everything fits on page then center everything
			if (ipage0 != ilastpage) 
				overallht = pany[ibottom] + image_h - pany[ipage0];
			else
				overallht = image_h;
			if (overallht <= winh) {
				// set the y of page 0
				deltay = - pany[ipage0] + (winh - overallht) / 2 ;
				for (i=0; i<MAX_BUFFERS; i++) pany[i] += deltay;
				// dont bother to do anything else
				canbump = 0;
			}
		} 
		// otherwise put page 0 at the top of the screen
		if ((canbump) && ((pany[ipage0]+deltay) > 0)) deltay -= (pany[ipage0] + deltay);
	} 
	if (canbump) {
		// ensure last page always end at or below the bottom of the screen
		if ((ilastpage >= 0) && (canbump)) {  // if last page is not also the first page
			image_h = fz_pixmap_height(ctx, image[ilastpage]);
			// bump everthing back down
			if ((pany[ilastpage] + deltay + image_h) < winh)
				deltay += (winh - pany[ilastpage] - deltay - image_h);
		}
		// do we need to redraw canvas? a) b) c) d)
		for (i=0; i<MAX_BUFFERS; i++) {
			if (image[i]) {
				// apply delta y
				pany[i] += deltay;
				// c) an image fell off the bottom the screen
				if (pany[i] >= winh) {
					newcanvasflag = 1;
					// drop page right now
					CSapp_droppage(i);
					if (i == ibottom) ibottom = (i + MAX_BUFFERS - 1)%MAX_BUFFERS;
					if (image[ibottom] == NULL) ibottom = -1;
				} else {
					// d) an image fell off the top of the screen - drop it right now
					if (i==itop) {
						image_h = fz_pixmap_height(ctx, image[i]);
						if ((pany[i] + image_h) < 0) {
							newcanvasflag = 1;
							CSapp_droppage(i);
							if (image[(i + 1) % MAX_BUFFERS]) 
								itop = (i + 1) % MAX_BUFFERS;
							else itop = -1;
						}
					}
				}
			}
		}
		// do we need to redraw canvas?  test as follows:
		// a) there's a gap at the bottom of the screen
		if (ibottom >= 0){
			image_h = fz_pixmap_height(ctx, image[ibottom]);
			if ((pany[ibottom] + image_h + GAP) < winh) {
				newcanvasflag = 1;
			}
		}
		// b) there's a gap at the top of the screen 
		if (itop >= 0) {
			if (pany[itop] > 0) {
				newcanvasflag = 1;
			}
		}
	}

	if (newcanvasflag) 
		CSapp_drawcanvas(page[itop]->number, 0);

	// only display page numbers if they change
	i = CSapp_get_buffer_at(winw/2, winh/2);
	if (i >=0) i = page[i]->number;
	if ((displayed_page_number != i) || (res_changed_flag)) {
		res_changed_flag = 0;
		displayed_page_number = i;
		char buf[256];
		char buf2[64];
		size_t len;
		sprintf(buf2, " - %d/%d (%d dpi)",
				displayed_page_number + 1, pagecount, resolution);
		len = 256-strlen(buf2);
		if (strlen(doctitle) > len)
		{
			fz_strlcpy(buf, doctitle, len-3);
			fz_strlcat(buf, "...", 256);
			fz_strlcat(buf, buf2, 256);
		}
		else
			sprintf(buf, "%s%s", doctitle, buf2);
		((Fl_Window *)(this->parent()))->copy_label(buf);
	}

	// x pans
	// do not allow x to move right of left edge of window
	if (newx > 0)
		newx = 0;
	// x doesn't move until window width reaches edge of image
	if (newx + maxw < winw)
		newx = winw - maxw;
	// when window is larger than image, then letterbox image
	if (winw >= maxw)
		newx = (winw - maxw) / 2;

	if ((deltay != 0) || (newx != panx) || redrawflag){
		panx = newx;
		redraw();
	}
	this->top_window()->cursor(FL_CURSOR_ARROW);
}

void PDFWindow::CSapp_winrepaint() 
{
	int i;
	int image_w ,image_h, image_n;
	unsigned char *samples;

	offscreen_buffer = fl_create_offscreen(winw, winh);
	fl_begin_offscreen(offscreen_buffer);
	fl_rectf(0, 0, winw, winh, 0x70, 0x70, 0x70);
	for (i=0; i<MAX_BUFFERS; i++)
	{
		if (image[i]) {
			image_w = fz_pixmap_width(ctx, image[i]);
			image_h = fz_pixmap_height(ctx, image[i]);
			image_n = fz_pixmap_components(ctx, image[i]);
			samples = fz_pixmap_samples(ctx, image[i]);
			if (image_n == 2)
			{
				int ix = image_w * image_h;
				unsigned char *color = (unsigned char *)malloc(sizeof(unsigned char));
				unsigned char *s = samples;
				unsigned char *d = color;
				for (; ix > 0 ; ix--)
				{
					d[2] = d[1] = d[0] = *s++;
					d[3] = *s++;
					d += 4;
				}
				fl_draw_image(samples, xd + panx, yd + pany[i], image_w, image_h, image_n, 0);
				free(color);
			}
			if (image_n == 4)
			{ 
				fl_draw_image(samples, panx, pany[i], image_w, image_h, 4, 0);
			}
		}
	}
	// put the browser restore PB on the screen
	if (IS_FULL) fl_draw_pixmap(RETURN_xpm, 0, 0);
	fl_end_offscreen();
	fl_copy_offscreen(xd, yd, winw, winh, offscreen_buffer, 0,0);
	fl_delete_offscreen(offscreen_buffer);
	// at this point, we can accept another zoom event
	IN_ZOOM = 0;
}

void PDFWindow::CSapp_close()
{
	int i;

	fz_free(ctx, doctitle);
	doctitle = NULL;

	fz_free(ctx, docpath);
	docpath = NULL;

	for (i=0; i < MAX_BUFFERS; i++) CSapp_droppage(i);

	fz_drop_document(ctx, doc);
	doc = NULL;

	fz_flush_warnings(ctx);
}

int PDFWindow::CSapp_preclose()
{
	if (doc) {
		pdf_specifics(ctx, doc);
	}
	return 1;
}

void PDFWindow::CSapp_reloadpage()
{
	CSapp_drawcanvas(activepagenumber, 0);
}

void PDFWindow::CSapp_onresize(int w, int h)
{
	winw = w;
	winh = h;
	// repaint is called by masterpanner
	CSapp_masterpanner(0, 0, 1);
}
// general zoom handler
void PDFWindow::CSapp_handlezoom(int x, int y, int dir)
{
	int i, idx;
	int delx, dely;
	float factor, newres;

	IN_ZOOM = 1;
	// zoom about cursor point
	if (-1 == (idx = CSapp_get_buffer_at(x, y))) {
		// failed, so use centre of window
		x = winw/2;
		y = winh/2;
		// and the buffer at that location.  If fail, abort zoom command.
		if (-1 == (idx = CSapp_get_buffer_at(x, y))) {
			IN_ZOOM = 0;
			return;
		}
	}

	// set resolution and get zoom factor
	if (dir > 0)
		newres = CSapp_zoom_in(resolution);
	else
		newres = CSapp_zoom_out(resolution);
	// if nothing changed, then exit
	if (newres == resolution) {
		IN_ZOOM = 0;
		return;
	}
	factor = newres / resolution;
	resolution = newres;

	// calculate the shift in the page position
	delx = x/2 - (x/2 - panx) * factor - panx;
	dely = y - (y - pany[idx])* factor - pany[idx];
	// redraw all pages
	for (i=0; i<MAX_BUFFERS; i++)
		if (image[i]) CSapp_renderpage(i);
	res_changed_flag = 1;
	// redraw everything around our page
	CSapp_drawcanvas(page[idx]->number,0);
	// shift page
	CSapp_masterpanner(delx, dely, 1);
}
// zoom handler for width
void PDFWindow::CSapp_hor_zoom(int idx) 
{
	float newres, factor;
	int delx, dely;
	int  i;

	// set resolution and get zoom factor
	newres = resolution * (float) winw / (fz_pixmap_width(ctx, image[idx]) + 2 * GAP);
	// clip resolution
	if (newres > MAX_ZOOM) newres = MAX_ZOOM;
	if (newres < MIN_ZOOM) newres = MIN_ZOOM;
	// get factor after clipping
	factor = newres / resolution;
	resolution = newres;

	// centre the window horizontally with a gap on left and right edge
	delx = (winw + 2 * GAP)/2 - ((winw + 2 * GAP)/2 - panx) * factor - panx;
	dely = winh/2 - (winh/2 - pany[idx])* factor - pany[idx];
	// redraw all pages
	for (i=0; i<MAX_BUFFERS; i++)
		if (image[i]) CSapp_renderpage(i);
	res_changed_flag = 1;
	// shift page
	CSapp_masterpanner(delx, dely, 0);
	// redraw canvas
	CSapp_drawcanvas(page[idx]->number,0);
	// update window
	CSapp_masterpanner(0, 0, 1);
}
// zoom handler for height
void PDFWindow::CSapp_vert_zoom(int idx) 
{
	int i;
	resolution *= (float) winh / fz_pixmap_height(ctx, image[idx]);
	if (resolution > MAX_ZOOM) resolution = MAX_ZOOM;
	if (resolution < MIN_ZOOM) resolution = MIN_ZOOM;
	// redraw all pages
	for (i=0; i<MAX_BUFFERS; i++)
		if (image[i]) CSapp_renderpage(i);
	res_changed_flag = 1;
	CSapp_drawcanvas(page[idx]->number, 1);
	CSapp_masterpanner(0, 0, 1);
}

void PDFWindow::CSapp_handlescroll(int modifiers, int dir)
{
	// scroll wheel up/down or left/right if shift
	int xstep = 0;
	int ystep = 0;
	if (modifiers & (1<<0)) // shift
		xstep = 20 * dir;
	else
		ystep = 20 * dir;
	CSapp_masterpanner(xstep, ystep, 1);
}
void PDFWindow::CSapp_onkey(int c)
{
	int skip_to_page;
	int x,y;
	int zoomdir;
	int idx;

	if (IN_COMMAND) return;
	IN_COMMAND = 1;
	zoomdir = -1; // default to zoom out
	switch (c)
	{
	// zoom
	case '+':	// in
	case '=':
		zoomdir = 1;
		// don't break;
	case '-':	// out
	case '_':
		// get the zoom point
		x = Fl::event_x();
		y = Fl::event_y();
		//Fl::get_mouse(x, y);
		x = x-xd;
		y = y-yd;
		if ((x < 0) || (x >= winh) || (y < 0) || (y >= winw)) {
			CSapp_handlezoom(x, y, zoomdir);
		}
		break;
	case 'W':	// width
	case 'w':
		// what page is halfway down screen?
		if (-1 == (idx = CSapp_get_buffer_at(winw/2, winh/2))) break;
		// ensure this is the active document
		activepageindex = idx;
		activepagenumber = page[idx]->number;
		CSapp_hor_zoom(idx);
		break;
	case 'H':	// height
	case 'h':
		// what page is halfway down screen?
		if (-1 == (idx = CSapp_get_buffer_at(winw/2, winh/2))) break;
		// ensure this is the active document
		activepageindex = idx;
		activepagenumber = page[idx]->number;
		CSapp_vert_zoom(idx);
		break;
	case 'F':	// fit to width or height
	case 'f':
		// what page is halfway down screen?
		if (-1 == (idx = CSapp_get_buffer_at(winw/2, winh/2))) break;
		// ensure this is the active document
		activepageindex = idx;
		activepagenumber = page[idx]->number;
		// is it horizontal on or vertical
		if (((float) fz_pixmap_width(ctx, image[idx]) / fz_pixmap_height(ctx, image[idx])) >
			((float) winw / winh))
			CSapp_hor_zoom(idx);
		else
			CSapp_vert_zoom(idx);
		break;

	// pan
	case 0x25:  //left
		CSapp_handlescroll(1, 1);
		break;
	case 0x27:  //right
		CSapp_handlescroll(1, -1);
		break;
	case 0x28:  // down
		CSapp_handlescroll(0, -1);
		break;
	case 0x26:  //up
		CSapp_handlescroll(0, 1);
		break; 

	// page jumps - these are always new loads
	case 0x21:  // page up
		if (activepagenumber > 0) {
			CSapp_drawcanvas(activepagenumber-1, 1);
			CSapp_masterpanner(0, 0, 1);
		}
		break;
	case 0x22:  // page down
		if (activepagenumber < (pagecount - 1)) {
			CSapp_drawcanvas(activepagenumber+1, 1);
			CSapp_masterpanner(0, 0, 1);
		}
		break;
	case '<':
		skip_to_page = (activepagenumber >= PAGE_SKIP) ? (activepagenumber - PAGE_SKIP) : 0;
		CSapp_drawcanvas(skip_to_page, 1);
		CSapp_masterpanner(0, 0, 1);
		break;
	case '>':
		if ((activepagenumber + PAGE_SKIP) >= pagecount) 
			skip_to_page = pagecount -1;
		else
			skip_to_page = activepagenumber + PAGE_SKIP;
		CSapp_drawcanvas(skip_to_page, 1);
		CSapp_masterpanner(0, 0, 1);
		break;
	}
	IN_COMMAND = 0;
}

void PDFWindow::CSapp_dragmouse(int x, int y)
{
	if (IN_PAN == 0) {
		// drag mode only initiated when first click falls within a sheet
		if (doc){
			if (CSapp_get_buffer_at(x, y) >= 0) {
				IN_PAN = 1;
				selx = x;
				sely = y;
			}
		}
	} else {
		// take command and start the pan
		IN_COMMAND = 1;
		// do the pan
		int deltax = x - selx;
		int deltay = y - sely;
		if ((deltax !=0) && (deltay != 0))
			CSapp_masterpanner(deltax, deltay, 1);
		selx = x;
		sely = y;
	}
}

