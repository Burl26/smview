// generated by Fast Light User Interface Designer (fluid) version 1.0304

#ifndef smview_fluid_h
#define smview_fluid_h
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include "SM_DB.h"
#include <FL/fl_ask.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Button.H>
#include "PDFWindow.h"
#include <time.h>
#include <X11/extensions/XTest.h>

class MainUI {
public:
  MainUI();
  Fl_Double_Window *mainWindow;
private:
  inline void cb_mainWindow_i(Fl_Double_Window*, void*);
  static void cb_mainWindow(Fl_Double_Window*, void*);
public:
  Fl_Tabs *SongList;
private:
  inline void cb_SongList_i(Fl_Tabs*, char*);
  static void cb_SongList(Fl_Tabs*, char*);
public:
  Fl_Group *TabArtist;
  Fl_Browser *ArtistBrowser;
private:
  inline void cb_ArtistBrowser_i(Fl_Browser*, void*);
  static void cb_ArtistBrowser(Fl_Browser*, void*);
public:
  Fl_Group *TabTitle;
  Fl_Browser *TitleBrowser;
private:
  inline void cb_TitleBrowser_i(Fl_Browser*, void*);
  static void cb_TitleBrowser(Fl_Browser*, void*);
public:
  Fl_Group *TabRecent;
  Fl_Browser *RecentBrowser;
private:
  inline void cb_RecentBrowser_i(Fl_Browser*, void*);
  static void cb_RecentBrowser(Fl_Browser*, void*);
public:
  Fl_Button *HideButton;
private:
  inline void cb_HideButton_i(Fl_Button*, char*);
  static void cb_HideButton(Fl_Button*, char*);
public:
  Fl_Browser *IndexTab;
private:
  inline void cb_IndexTab_i(Fl_Browser*, char*);
  static void cb_IndexTab(Fl_Browser*, char*);
public:
  PDFWindow *PDFBox;
  void show();
  void StartsWith(char c);
  int last_artist; 
  int last_title; 
  int last_date; 
  int double_clicked; 
  long dbcount; 
  DBSTRUCT *db; 
  void init();
  void ShowPDF(void *v);
  char *csvfile; 
  void OnClose();
  void HideBrowsers();
  void redraw();
};
#endif
