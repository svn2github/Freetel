//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#include "wxcrafter.h"


// Declare the bitmap loading function
extern void wxC9ED9InitBitmapResources();

static bool bBitmapLoaded = false;


PTTDialogBaseClass::PTTDialogBaseClass(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC9ED9InitBitmapResources();
        bBitmapLoaded = true;
    }
    
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(mainSizer);
    
    wxStaticBoxSizer* staticBoxSizer28 = new wxStaticBoxSizer( new wxStaticBox(this, wxID_ANY, _("Audio Tone")), wxVERTICAL);
    
    mainSizer->Add(staticBoxSizer28, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL, 5);
    
    m_ckPTTRtChan = new wxCheckBox(this, wxID_ANY, _("PTT tone on right audio channel"), wxDefaultPosition, wxSize(-1,-1), 0);
    m_ckPTTRtChan->SetValue(false);
    
    staticBoxSizer28->Add(m_ckPTTRtChan, 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL, 5);
    
    wxStaticBoxSizer* staticBoxSizer17 = new wxStaticBoxSizer( new wxStaticBox(this, wxID_ANY, _("Hardware PTT Settings")), wxVERTICAL);
    
    mainSizer->Add(staticBoxSizer17, 1, wxEXPAND, 5);
    
    wxStaticBoxSizer* staticBoxSizer31 = new wxStaticBoxSizer( new wxStaticBox(this, wxID_ANY, _("PTT Port")), wxVERTICAL);
    
    staticBoxSizer17->Add(staticBoxSizer31, 0, wxEXPAND, 5);
    
    wxArrayString m_listCtrlPortsArr;
    m_listCtrlPorts = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxSize(-1,-1), m_listCtrlPortsArr, wxLB_SINGLE);
    
    staticBoxSizer31->Add(m_listCtrlPorts, 1, wxALIGN_CENTER, 0);
    
    wxBoxSizer* boxSizer19 = new wxBoxSizer(wxVERTICAL);
    
    staticBoxSizer17->Add(boxSizer19, 1, wxEXPAND, 5);
    
    wxStaticBoxSizer* staticBoxSizer16 = new wxStaticBoxSizer( new wxStaticBox(this, wxID_ANY, _("Signal polarity")), wxHORIZONTAL);
    
    boxSizer19->Add(staticBoxSizer16, 1, wxEXPAND|wxALIGN_CENTER|wxALIGN_RIGHT, 5);
    
    wxGridSizer* gridSizer17 = new wxGridSizer(  3, 2, 0, 0);
    
    staticBoxSizer16->Add(gridSizer17, 1, wxEXPAND|wxALIGN_RIGHT, 5);
    
    m_ckUseSerialPTT = new wxCheckBox(this, wxID_ANY, _("Use Serial Port PTT"), wxDefaultPosition, wxSize(-1,-1), 0);
    m_ckUseSerialPTT->SetValue(false);
    
    gridSizer17->Add(m_ckUseSerialPTT, 0, wxALIGN_CENTER, 10);
    
    gridSizer17->Add(0, 0, 0, 0, 5);
    
    m_rbUseDTR = new wxRadioButton(this, wxID_ANY, _("Use DTR"), wxDefaultPosition, wxSize(-1,-1), 0);
    m_rbUseDTR->SetValue(1);
    
    gridSizer17->Add(m_rbUseDTR, 0, wxALIGN_CENTER|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    
    m_ckRTSPos = new wxCheckBox(this, wxID_ANY, _("DTR = +V"), wxDefaultPosition, wxSize(-1,-1), 0);
    m_ckRTSPos->SetValue(false);
    
    gridSizer17->Add(m_ckRTSPos, 0, wxALIGN_CENTER|wxALIGN_RIGHT, 5);
    
    m_rbUseRTS = new wxRadioButton(this, wxID_ANY, _("Use RTS"), wxDefaultPosition, wxSize(-1,-1), 0);
    m_rbUseRTS->SetToolTip(_("Toggle the RTS pin for PTT"));
    m_rbUseRTS->SetValue(1);
    
    gridSizer17->Add(m_rbUseRTS, 0, wxALIGN_CENTER|wxALIGN_RIGHT, 5);
    
    m_ckDTRPos = new wxCheckBox(this, wxID_ANY, _("RTS = +V"), wxDefaultPosition, wxSize(-1,-1), 0);
    m_ckDTRPos->SetValue(false);
    m_ckDTRPos->SetToolTip(_("Set Polarity of the RTS line"));
    
    gridSizer17->Add(m_ckDTRPos, 0, wxALIGN_CENTER|wxALIGN_RIGHT, 5);
    
    wxBoxSizer* boxSizer12 = new wxBoxSizer(wxHORIZONTAL);
    
    mainSizer->Add(boxSizer12, 0, wxLEFT|wxRIGHT|wxTOP|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL, 5);
    
    m_buttonOK = new wxButton(this, wxID_OK, _("OK"), wxDefaultPosition, wxSize(-1,-1), 0);
    m_buttonOK->SetDefault();
    
    boxSizer12->Add(m_buttonOK, 0, wxLEFT|wxRIGHT|wxTOP|wxBOTTOM, 5);
    
    m_buttonCancel = new wxButton(this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxSize(-1,-1), 0);
    
    boxSizer12->Add(m_buttonCancel, 0, wxLEFT|wxRIGHT|wxTOP|wxBOTTOM, 5);
    
    m_buttonApply = new wxButton(this, wxID_APPLY, _("Apply"), wxDefaultPosition, wxSize(-1,-1), 0);
    
    boxSizer12->Add(m_buttonApply, 0, wxLEFT|wxRIGHT|wxTOP|wxBOTTOM, 5);
    
    
    SetSizeHints(450,300);
    if ( GetSizer() ) {
         GetSizer()->Fit(this);
    }
    Centre(wxBOTH);
    // Connect events
    this->Connect(wxEVT_INIT_DIALOG, wxInitDialogEventHandler(PTTDialogBaseClass::OnInitDialog), NULL, this);
    m_ckPTTRtChan->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(PTTDialogBaseClass::PTTAudioClick), NULL, this);
    m_listCtrlPorts->Connect(wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(PTTDialogBaseClass::PTTPortSlelcted), NULL, this);
    m_ckUseSerialPTT->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(PTTDialogBaseClass::PTTUseSerialClicked), NULL, this);
    m_rbUseDTR->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(PTTDialogBaseClass::UseDTRCliched), NULL, this);
    m_ckRTSPos->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(PTTDialogBaseClass::DTRVPlusClicked), NULL, this);
    m_rbUseRTS->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(PTTDialogBaseClass::UseRTSClicked), NULL, this);
    m_ckDTRPos->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(PTTDialogBaseClass::RTSVPlusClicked), NULL, this);
    m_buttonOK->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PTTDialogBaseClass::OnOK), NULL, this);
    m_buttonCancel->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PTTDialogBaseClass::OnCancel), NULL, this);
    m_buttonApply->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PTTDialogBaseClass::OnApply), NULL, this);
    
}

PTTDialogBaseClass::~PTTDialogBaseClass()
{
    this->Disconnect(wxEVT_INIT_DIALOG, wxInitDialogEventHandler(PTTDialogBaseClass::OnInitDialog), NULL, this);
    m_ckPTTRtChan->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(PTTDialogBaseClass::PTTAudioClick), NULL, this);
    m_listCtrlPorts->Disconnect(wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(PTTDialogBaseClass::PTTPortSlelcted), NULL, this);
    m_ckUseSerialPTT->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(PTTDialogBaseClass::PTTUseSerialClicked), NULL, this);
    m_rbUseDTR->Disconnect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(PTTDialogBaseClass::UseDTRCliched), NULL, this);
    m_ckRTSPos->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(PTTDialogBaseClass::DTRVPlusClicked), NULL, this);
    m_rbUseRTS->Disconnect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(PTTDialogBaseClass::UseRTSClicked), NULL, this);
    m_ckDTRPos->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(PTTDialogBaseClass::RTSVPlusClicked), NULL, this);
    m_buttonOK->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PTTDialogBaseClass::OnOK), NULL, this);
    m_buttonCancel->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PTTDialogBaseClass::OnCancel), NULL, this);
    m_buttonApply->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PTTDialogBaseClass::OnApply), NULL, this);
    
}
