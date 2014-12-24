//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: GDIDE.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#ifndef GDIDE_BASE_CLASSES_H
#define GDIDE_BASE_CLASSES_H

#include <wx/settings.h>
#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_bmp.h>
#include <wx/panel.h>
#include <wx/artprov.h>
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/hyperlink.h>
#include <wx/textctrl.h>

class BaseStartHerePage : public wxPanel
{
protected:
    wxStaticBitmap* logoBmp;
    wxStaticText* m_staticText9;
    wxStaticBitmap* gettingStartedBmp;
    wxStaticText* gettingStartedTxt;
    wxHyperlinkCtrl* m_hyperLink17;
    wxHyperlinkCtrl* m_hyperLink231;
    wxHyperlinkCtrl* m_hyperLink23;
    wxStaticBitmap* latestProjectsBmp;
    wxStaticText* latestProjectsTxt;
    wxHyperlinkCtrl* lastProject1Bt;
    wxHyperlinkCtrl* lastProject2Bt;
    wxHyperlinkCtrl* lastProject3Bt;
    wxStaticBitmap* communityBmp;
    wxStaticText* communityTxt;
    wxHyperlinkCtrl* m_hyperLink175;
    wxHyperlinkCtrl* m_hyperLink2316;
    wxHyperlinkCtrl* m_hyperLink237;
    wxStaticBitmap* newsBmp;
    wxStaticText* newsTxt;
    wxTextCtrl* newsEdit;
    wxHyperlinkCtrl* newsLink1;
    wxHyperlinkCtrl* newsLink2;
    wxStaticText* m_staticText53;
    wxStaticBitmap* donateBmp;
    wxHyperlinkCtrl* donateLinkBt;
    wxStaticBitmap* githubBmp;
    wxHyperlinkCtrl* m_hyperLink57;

protected:
    virtual void OnLastProject1Click(wxHyperlinkEvent& event) { event.Skip(); }
    virtual void OnLastProject2Click(wxHyperlinkEvent& event) { event.Skip(); }
    virtual void OnLastProject3Click(wxHyperlinkEvent& event) { event.Skip(); }

public:
    BaseStartHerePage(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(700,500), long style = wxTAB_TRAVERSAL);
    virtual ~BaseStartHerePage();
};

#endif