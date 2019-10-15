#include <wx/filepicker.h>
#include <wx/splash.h>
#include <wx/wx.h>

namespace tetrad {

#define PACKAGE_MODIFY
#include "core/Package.h"

/*
 */
class MyApp : public wxApp
{
 public:
  virtual bool OnInit();
};

/*
 */
class MyFrame : public wxFrame
{
 public:
  MyFrame();

 private:
  void OnNewFile(wxCommandEvent& event);
  void OnOpenFile(wxCommandEvent& event);
  void OnSaveFile(wxCommandEvent& event);
  void OnSaveAsFile(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);

 private:
  enum
  {
    FILE_NEW = wxID_HIGHEST + 1,
    FILE_OPEN,
    FILE_SAVE,
    FILE_SAVEAS,
    F_F
  };
  DECLARE_EVENT_TABLE()

 private:
  wxFilePickerCtrl* m_pFilePicker;
  Package m_Package;
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(FILE_NEW, MyFrame::OnNewFile)
EVT_MENU(FILE_OPEN, MyFrame::OnOpenFile)
EVT_MENU(FILE_SAVE, MyFrame::OnSaveFile)
EVT_MENU(FILE_SAVEAS, MyFrame::OnSaveAsFile)
EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
EVT_MENU(wxID_EXIT, MyFrame::OnExit)
END_EVENT_TABLE()

}  // namespace tetrad
