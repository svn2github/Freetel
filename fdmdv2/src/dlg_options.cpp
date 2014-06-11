//==========================================================================
// Name:            dlg_options.cpp
// Purpose:         Dialog for controlling misc FreeDV options
// Date:            May 24 2013
// Authors:         David Rowe, David Witten
// 
// License:
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2.1,
//  as published by the Free Software Foundation.  This program is
//  distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
//  License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, see <http://www.gnu.org/licenses/>.
//
//==========================================================================

#include "dlg_options.h"

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=
// Class OptionsDlg
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=
OptionsDlg::OptionsDlg(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{

    this->SetSizeHints(wxDefaultSize, wxDefaultSize);

    wxBoxSizer* bSizer30;
    bSizer30 = new wxBoxSizer(wxVERTICAL);

    //------------------------------
    // Test Frames check box
    //------------------------------

    wxStaticBoxSizer* sbSizer_testFrames;
    sbSizer_testFrames = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Test Frames")), wxVERTICAL);

    m_ckboxTestFrame = new wxCheckBox(this, wxID_ANY, _("Enable"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    sbSizer_testFrames->Add(m_ckboxTestFrame, 0, wxALIGN_LEFT, 0);

    bSizer30->Add(sbSizer_testFrames,0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 3);

    //------------------------------
    // Txt Msg Text Box
    //------------------------------

    wxStaticBoxSizer* sbSizer_callSign;
    sbSizer_callSign = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Txt Msg")), wxVERTICAL);

    m_txtCtrlCallSign = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    m_txtCtrlCallSign->SetToolTip(_("Txt Msg you can send along with Voice"));
    sbSizer_callSign->Add(m_txtCtrlCallSign, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);

    bSizer30->Add(sbSizer_callSign,0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 3);
 
    //------------------------------
    // Txt Encoding 
    //------------------------------

    wxStaticBoxSizer* sbSizer_encoding = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Text Encoding")), wxHORIZONTAL);

    m_rb_textEncoding1 = new wxRadioButton( this, wxID_ANY, wxT("Long varicode"), wxDefaultPosition, wxDefaultSize, 0);
    m_rb_textEncoding1->SetValue(true);
    sbSizer_encoding->Add(m_rb_textEncoding1, 0, wxALIGN_LEFT|wxALL, 1);
    m_rb_textEncoding2 = new wxRadioButton( this, wxID_ANY, wxT("Short Varicode"), wxDefaultPosition, wxDefaultSize, 0);
    sbSizer_encoding->Add(m_rb_textEncoding2, 0, wxALIGN_LEFT|wxALL, 1);

    bSizer30->Add(sbSizer_encoding,0, wxALL|wxEXPAND, 3);
 
    //------------------------------
    // Event processing
    //------------------------------

    wxStaticBoxSizer* sbSizer_events;
    sbSizer_events = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Event Processing")), wxVERTICAL);

    // event processing enable

    m_ckbox_events = new wxCheckBox(this, wxID_ANY, _("Enable System Calls"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    sbSizer_events->Add(m_ckbox_events, 0, 0, 5);

    // list of regexps

    wxStaticBoxSizer* sbSizer_regexp = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Regular Expressions to Process Events")), wxHORIZONTAL);
    m_txt_events_regexp_match = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200,100), wxTE_MULTILINE);
    m_txt_events_regexp_match->SetToolTip(_("Enter regular expressions to match events"));
    sbSizer_regexp->Add(m_txt_events_regexp_match, 1, wxEXPAND, 5);
    m_txt_events_regexp_replace = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200,100), wxTE_MULTILINE);
    m_txt_events_regexp_replace->SetToolTip(_("Enter regular expressions to replace events"));
    sbSizer_regexp->Add(m_txt_events_regexp_replace, 1, wxEXPAND, 5);
    sbSizer_events->Add(sbSizer_regexp, 1, wxEXPAND, 5);

    // log of events and responses

    wxStaticBoxSizer* sbSizer_event_log = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Events and Responses")), wxVERTICAL);
    wxBoxSizer* bSizer33 = new wxBoxSizer(wxHORIZONTAL);
    m_txt_events_in = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200,50), wxTE_MULTILINE | wxTE_READONLY);
    bSizer33->Add(m_txt_events_in, 1, wxEXPAND, 5);
    m_txt_events_out = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200,50), wxTE_MULTILINE | wxTE_READONLY);
    bSizer33->Add(m_txt_events_out, 1, wxEXPAND, 5);
    sbSizer_event_log->Add(bSizer33, 1, wxEXPAND, 5);
    sbSizer_events->Add(sbSizer_event_log, 1, wxEXPAND, 5);

    bSizer30->Add(sbSizer_events,0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 3);

    //------------------------------
    // UDP control port
    //------------------------------

    wxStaticBoxSizer* sbSizer_udp;
    sbSizer_udp = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("UDP Control Port")), wxHORIZONTAL);
    m_ckbox_udp_enable = new wxCheckBox(this, wxID_ANY, _("Enable UDP Control Port    UDP Port Nnumber:"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
    sbSizer_udp->Add(m_ckbox_udp_enable, 0,  wxALIGN_CENTER_HORIZONTAL, 5);
    m_txt_udp_port = new wxTextCtrl(this, wxID_ANY,  wxEmptyString, wxDefaultPosition, wxSize(100,-1), 0, wxTextValidator(wxFILTER_DIGITS));
    sbSizer_udp->Add(m_txt_udp_port, 0, wxALIGN_CENTER_HORIZONTAL, 5);

    bSizer30->Add(sbSizer_udp,0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 3);

    //------------------------------
    // Cancel - OK Buttons 
    //------------------------------

    wxBoxSizer* bSizer31 = new wxBoxSizer(wxHORIZONTAL);
    m_sdbSizer5Cancel = new wxButton(this, wxID_CANCEL);
    bSizer31->Add(m_sdbSizer5Cancel, 0, wxALL, 2);
    m_sdbSizer5OK = new wxButton(this, wxID_OK);
    bSizer31->Add(m_sdbSizer5OK, 0, wxALL, 2);

    bSizer30->Add(bSizer31, 0, wxALIGN_RIGHT|wxALL, 0);

    this->SetSizer(bSizer30);
    this->Layout();

    this->Centre(wxBOTH);
 
    // Connect Events -------------------------------------------------------

    this->Connect(wxEVT_INIT_DIALOG, wxInitDialogEventHandler(OptionsDlg::OnInitDialog));

    m_sdbSizer5Cancel->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(OptionsDlg::OnCancel), NULL, this);
    m_sdbSizer5OK->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(OptionsDlg::OnOK), NULL, this);

    event_in_serial = 0;
    event_out_serial = 0;
}

//-------------------------------------------------------------------------
// ~OptionsDlg()
//-------------------------------------------------------------------------
OptionsDlg::~OptionsDlg()
{
    // Disconnect Events

    this->Disconnect(wxEVT_INIT_DIALOG, wxInitDialogEventHandler(OptionsDlg::OnInitDialog));

    m_sdbSizer5Cancel->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(OptionsDlg::OnCancel), NULL, this);
    m_sdbSizer5OK->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(OptionsDlg::OnOK), NULL, this);
}


//-------------------------------------------------------------------------
// ExchangeData()
//-------------------------------------------------------------------------
void OptionsDlg::ExchangeData(int inout, bool storePersistent)
{
    wxConfigBase *pConfig = wxConfigBase::Get();

    if(inout == EXCHANGE_DATA_IN)
    {
        m_txtCtrlCallSign->SetValue(wxGetApp().m_callSign);
        m_ckboxTestFrame->SetValue(wxGetApp().m_testFrames);

        m_ckbox_events->SetValue(wxGetApp().m_events);
        m_txt_events_regexp_match->SetValue(wxGetApp().m_events_regexp_match);
        m_txt_events_regexp_replace->SetValue(wxGetApp().m_events_regexp_replace);
        
        m_ckbox_udp_enable->SetValue(wxGetApp().m_udp_enable);
        m_txt_udp_port->SetValue(wxString::Format(wxT("%i"),wxGetApp().m_udp_port));

        if (wxGetApp().m_textEncoding == 1)
            m_rb_textEncoding1->SetValue(true);
        if (wxGetApp().m_textEncoding == 2)
            m_rb_textEncoding2->SetValue(true);
    }

    if(inout == EXCHANGE_DATA_OUT)
    {
        wxGetApp().m_callSign      = m_txtCtrlCallSign->GetValue();
        wxGetApp().m_testFrames    = m_ckboxTestFrame->GetValue();

        wxGetApp().m_events        = m_ckbox_events->GetValue();
        wxGetApp().m_events_regexp_match = m_txt_events_regexp_match->GetValue();
        wxGetApp().m_events_regexp_replace = m_txt_events_regexp_replace->GetValue();
 
        wxGetApp().m_udp_enable     = m_ckbox_udp_enable->GetValue();
        long port;
        m_txt_udp_port->GetValue().ToLong(&port);
        wxGetApp().m_udp_port       = (int)port;

        if (m_rb_textEncoding1->GetValue())
            wxGetApp().m_textEncoding = 1;
        if (m_rb_textEncoding2->GetValue())
            wxGetApp().m_textEncoding = 2;

        if (storePersistent) {
            pConfig->Write(wxT("/Data/CallSign"), wxGetApp().m_callSign);
            pConfig->Write(wxT("/Data/TextEncoding"), wxGetApp().m_textEncoding);
            pConfig->Write(wxT("/Events/enable"), wxGetApp().m_events);
            pConfig->Write(wxT("/Events/regexp_match"), wxGetApp().m_events_regexp_match);
            pConfig->Write(wxT("/Events/regexp_replace"), wxGetApp().m_events_regexp_replace);
            
            pConfig->Write(wxT("/UDP/enable"), wxGetApp().m_udp_enable);
            pConfig->Write(wxT("/UDP/port"),  wxGetApp().m_udp_port);

            pConfig->Flush();
        }
    }
    delete wxConfigBase::Set((wxConfigBase *) NULL);
}

//-------------------------------------------------------------------------
// OnCancel()
//-------------------------------------------------------------------------
void OptionsDlg::OnCancel(wxCommandEvent& event)
{
    //this->EndModal(wxID_CANCEL);
    this->Show(false);
}

//-------------------------------------------------------------------------
// OnOK()
//-------------------------------------------------------------------------
void OptionsDlg::OnOK(wxCommandEvent& event)
{
    ExchangeData(EXCHANGE_DATA_OUT, true);
    //this->EndModal(wxID_OK);
    this->Show(false);
}

//-------------------------------------------------------------------------
// OnClose()
//-------------------------------------------------------------------------
void OptionsDlg::OnClose(wxCloseEvent& event)
{
    //this->EndModal(wxID_OK);
    this->Show(false);
}

//-------------------------------------------------------------------------
// OnInitDialog()
//-------------------------------------------------------------------------
void OptionsDlg::OnInitDialog(wxInitDialogEvent& event)
{
    ExchangeData(EXCHANGE_DATA_IN, false);
}


void OptionsDlg::updateEventLog(wxString event_in, wxString event_out) {
    wxString event_in_with_serial, event_out_with_serial; 
    event_in_with_serial.Printf(_T("[%d] %s"), event_in_serial++, event_in);
    event_out_with_serial.Printf(_T("[%d] %s"), event_out_serial++, event_out);

    m_txt_events_in->AppendText(event_in_with_serial+"\n");
    m_txt_events_out->AppendText(event_out_with_serial+"\n");
}

