#include "sizeable_window.h"

sizeable_window::sizeable_window(int W, int H, const char *t) : Fl_Double_Window(W, H, t) { 

}
int sizeable_window::handle(int e) {
  if (e == FL_FULLSCREEN) {
    printf("Received FL_FULLSCREEN event\n");
  }
  if (Fl_Double_Window::handle(e)) return 1;
  return 0;
}
