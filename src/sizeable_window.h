#ifndef sizeable_window_h
#define sizeable_window_h 1
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
 

class sizeable_window : public Fl_Double_Window {
  public:
  sizeable_window(int W, int H, const char *t=0);
  int handle (int e);
};
#endif