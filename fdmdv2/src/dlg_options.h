//==========================================================================
// Name:            dlg_options.h
// Purpose:         Dialog for controlling misc FreeDV options
// Created:         Nov 25 2012
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

#ifndef __OPTIONS_DIALOG__
#define __OPTIONS_DIALOG__

#include "fdmdv2_main.h"

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=
// Class OptionsDlg
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=
class OptionsDlg : public wxDialog
{
    public:
    OptionsDlg( wxWindow* parent,
               wxWindowID id = wxID_ANY, const wxString& title = _("Options"), 
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(600,580), 
               long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
        ~OptionsDlg();

        void    ExchangeData(int inout, bool storePersistent);

    protected:
        // Handlers for events.
        void    OnCancel(wxCommandEvent& event);
        void    OnOK(wxCommandEvent& event);
        void    OnClose(wxCloseEvent& event);
        void    OnInitDialog(wxInitDialogEvent& event);
 
        wxTextCtrl   *m_txtCtrlCallSign; // TODO: this should be renamed to tx_txtmsg, and rename all related incl persis strge
        wxCheckBox   *m_ckboxTestFrame;

        wxRadioButton *m_rb_textEncoding1;
        wxRadioButton *m_rb_textEncoding2;

        wxCheckBox   *m_ckbox_events;
        wxTextCtrl   *m_txt_events_regexp;
        wxTextCtrl   *m_txt_events_in;
        wxTextCtrl   *m_txt_events_out;
        wxTextCtrl   *m_txt_event_test;
        wxButton     *m_btn_event_test;

        wxButton*     m_sdbSizer5OK;
        wxButton*     m_sdbSizer5Cancel;

     private:
};

#endif // __OPTIONS_DIALOG__
