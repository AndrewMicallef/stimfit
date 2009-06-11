// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

// frame.cpp
// These are the top-level and child windows of the application.
// 2007-12-27, Christoph Schmidt-Hieber, University of Freiburg

#ifdef _STFDEBUG
#include <iostream>
#endif
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#include <wx/grid.h>
#include <wx/artprov.h>
#include <wx/printdlg.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/progdlg.h>
#include <wx/splitter.h>
#include <wx/choicdlg.h>
#include <wx/aboutdlg.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#if !wxUSE_MDI_ARCHITECTURE
#error You must set wxUSE_MDI_ARCHITECTURE to 1 in setup.h!
#endif

#ifdef WITH_PYTHON
#include <Python.h>
#include <wx/wxPython/wxPython.h>
#endif

#include "./app.h"
#include "./doc.h"
#include "./view.h"
#include "./graph.h"
#include "./table.h"
#include "./printout.h"
#include "./dlgs/smalldlgs.h"
#include "./copygrid.h"
#ifdef _WINDOWS
#include "./../core/filelib/atflib.h"
#include "./../core/filelib/igorlib.h"
#endif

#include "./childframe.h"

IMPLEMENT_CLASS(wxStfChildFrame, wxStfChildType)

BEGIN_EVENT_TABLE(wxStfChildFrame, wxStfChildType)
EVT_COMBOBOX( wxCOMBOTRACES, wxStfChildFrame::OnComboTraces )
EVT_COMBOBOX( wxCOMBOACTCHANNEL, wxStfChildFrame::OnComboActChannel )
EVT_COMBOBOX( wxCOMBOINACTCHANNEL, wxStfChildFrame::OnComboInactChannel )
EVT_CHECKBOX( wxID_PLOTSELECTED, wxStfChildFrame::OnPlotselected )
EVT_CHECKBOX( wxID_SHOWSECOND, wxStfChildFrame::OnShowsecond )
// workaround for status bar:
EVT_MENU_HIGHLIGHT_ALL( wxStfChildFrame::OnMenuHighlight )
END_EVENT_TABLE()

wxStfChildFrame::wxStfChildFrame(wxDocument* doc, wxView* view, wxStfParentType* parent,
                                 wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size,
                                 long style, const wxString& name) :
wxStfChildType(doc,view,parent,id,title,pos,size,style,name),
    m_notebook(NULL)
{
    m_mgr.SetManagedWindow(this);
    m_mgr.SetFlags( wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_DRAG |
                    wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_ALLOW_ACTIVE_PANE );
}

wxStfChildFrame::~wxStfChildFrame() {
    // deinitialize the frame manager
    m_mgr.UnInit();
}

wxStfGrid* wxStfChildFrame::CreateTable() {
    // create the notebook off-window to avoid flicker
    wxSize client_size = GetClientSize();

    wxStfGrid* ctrl = new wxStfGrid( this, wxID_ANY,
                                     wxDefaultPosition, wxDefaultSize,
                                     wxVSCROLL | wxHSCROLL );
    wxFont font( 8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
    ctrl->SetDefaultCellFont(font);
    ctrl->SetDefaultColSize(108);
    ctrl->SetColLabelSize(20);
    ctrl->SetDefaultCellAlignment(wxALIGN_RIGHT,wxALIGN_CENTRE);
    ctrl->CreateGrid(3,10);

    ctrl->EnableEditing(false);
    return ctrl;
}

wxAuiNotebook* wxStfChildFrame::CreateNotebook() {
    // create the notebook off-window to avoid flicker
    wxSize client_size = GetClientSize();
    m_notebook_style =
        wxAUI_NB_SCROLL_BUTTONS |
        wxAUI_NB_CLOSE_ON_ACTIVE_TAB |/*wxAUI_NB_DEFAULT_STYLE | */
        wxNO_BORDER;

    wxAuiNotebook* ctrl = new wxAuiNotebook( this, wxID_ANY,
                                             wxPoint(client_size.x, client_size.y), wxSize(200,200),
                                             m_notebook_style );

    return ctrl;
}

wxPanel* wxStfChildFrame::CreateTraceCounter() {
    wxSize client_size = GetClientSize();
    wxPanel* ctrl = new wxPanel( this, wxID_ANY, wxDefaultPosition,
                                 wxSize(160,88) );

    pTraceSizer = new wxFlexGridSizer( 0, 1, 2, 2 );

    return ctrl;
}

wxPanel* wxStfChildFrame::CreateChannelCounter() {
    wxPanel* ctrl = new wxPanel( this, wxID_ANY, wxDefaultPosition,
                                 wxSize(320,88) );

    pChannelSizer = new wxFlexGridSizer( 2, 3, 2, 2 );

    return ctrl;
}

void wxStfChildFrame::CreateComboTraces(const std::size_t value) {

    m_traceCounter = CreateTraceCounter();
    pTraceNumberSizer = new wxFlexGridSizer( 0, 3, 2, 2 );

    pSelected = new wxStaticText( m_traceCounter, wxID_ANY, wxT("Selected traces: 0") );
    pTraceSizer->Add( pSelected );

    wxStaticText* pTraceStaticText = new wxStaticText( m_traceCounter, wxID_ANY, wxT("Trace") );
    pTraceNumberSizer->Add( pTraceStaticText, wxALIGN_BOTTOM );

    pSize=new wxStaticText( m_traceCounter, wxID_ANY, wxT("of 0") );

    wxArrayString szTraces;
    szTraces.Alloc(value);
    for (std::size_t n=0;n<value;++n) {
        wxString number;
        number << (int)n+1;
        szTraces.Add(number);
    }
    pTraces=new wxComboBox(	m_traceCounter, wxCOMBOTRACES, wxT("CB1"), wxDefaultPosition,
                                wxSize(64,wxDefaultCoord), szTraces, wxCB_DROPDOWN | wxCB_READONLY);
    pTraceNumberSizer->Add( pTraces, wxALIGN_CENTRE );

    wxString sizeStr;
    sizeStr << wxT("of ") << (int)value;
    pSize->SetLabel(sizeStr);
    pTraceNumberSizer->Add( pSize, wxALIGN_BOTTOM );

    pTraceSizer->Add( pTraceNumberSizer, wxALIGN_BOTTOM );

    pPlotSelected=new wxCheckBox( m_traceCounter, wxID_PLOTSELECTED, wxT("Plot selected traces") );
    pPlotSelected->SetValue(false);
    pTraceSizer->Add( pPlotSelected );

    pTraceIndex = new wxStaticText( m_traceCounter, wxID_ANY, wxT("Current trace index: 0") );
    pTraceSizer->Add( pTraceIndex );

    m_traceCounter->SetSizer( pTraceSizer );
    m_traceCounter->Layout();

    wxStfDoc* pDoc=(wxStfDoc*)GetDocument();
    m_mgr.AddPane( m_traceCounter, wxAuiPaneInfo().Caption(wxT("Trace selection")).Fixed().
                   Position(pDoc->size()-1).CloseButton(false).Floatable().Dock().Top().Name(wxT("SelectionT")) );
    m_table=CreateTable();

    m_mgr.AddPane( m_table, wxAuiPaneInfo().Caption(wxT("Results")).Position(pDoc->size()).
                   CloseButton(false).Floatable().Dock().Top().Name(wxT("Results")) );
    m_mgr.Update();
    Refresh();
}

void wxStfChildFrame::CreateComboChannels(const wxArrayString& channelStrings) {
    m_channelCounter = CreateChannelCounter();

    wxStaticText* pActIndex  = new wxStaticText( m_channelCounter, wxID_ANY, wxT("Active channel index: ") );
    pChannelSizer->Add( pActIndex );

    pActChannel = new wxComboBox( m_channelCounter, wxCOMBOACTCHANNEL, wxT("0"),
                                  wxDefaultPosition, wxSize(64,24), channelStrings, wxCB_DROPDOWN | wxCB_READONLY );
    pChannelSizer->Add( pActChannel );
    pChannelSizer->AddStretchSpacer( );

    wxStaticText* pInactIndex = new wxStaticText( m_channelCounter, wxID_ANY, wxT("Inactive channel index: ") );
    pInactIndex->SetForegroundColour( *wxRED );
    pChannelSizer->Add( pInactIndex );

    pInactChannel = new wxComboBox( m_channelCounter, wxCOMBOINACTCHANNEL, wxT("1"),
                                    wxDefaultPosition, wxSize(64,24), channelStrings, wxCB_DROPDOWN | wxCB_READONLY );
    pChannelSizer->Add( pInactChannel );
    
    pShowSecond = new wxCheckBox( m_channelCounter, wxID_PLOTSELECTED, wxT("Show") );
    pShowSecond->SetValue(true);
    pChannelSizer->Add( pShowSecond );

    // Checkbox to hide inactive channel:
    
    m_channelCounter->SetSizer( pChannelSizer );
    m_channelCounter->Layout();

    m_mgr.AddPane( m_channelCounter, wxAuiPaneInfo().Caption(wxT("Channel selection")).Fixed().
                   Position(0).CloseButton(false).Floatable().Dock().Top().Name(wxT("SelectionC")) );
    m_mgr.Update();

    Refresh();
}

void wxStfChildFrame::SetSelected(std::size_t value) {
    wxString selStr;
    selStr << wxT("Selected traces: ") << (int)value;
    pSelected->SetLabel(selStr);
}

void wxStfChildFrame::SetChannels( std::size_t act, std::size_t inact ) {
    pActChannel->SetSelection( act );
    pInactChannel->SetSelection( inact );
}

std::size_t wxStfChildFrame::GetCurTrace() const {
    return pTraces->GetCurrentSelection();
}

void wxStfChildFrame::SetCurTrace(std::size_t n) {
    pTraces->SetSelection((int)n);
    wxString indStr;
    indStr << wxT("Zero-based index: ") << (int)n;
    pTraceIndex->SetLabel( indStr );
}

void wxStfChildFrame::OnComboTraces(wxCommandEvent& WXUNUSED(event)) {
    wxStfView* pView=(wxStfView*)GetView();
    wxStfDoc* pDoc=(wxStfDoc*)GetDocument();
    pDoc->SetSection(GetCurTrace());
    wxString indStr;
    indStr << wxT("Zero-based index: ") << GetCurTrace();
    pTraceIndex->SetLabel( indStr );
    wxGetApp().OnPeakcalcexecMsg();
    if (pView->GetGraph() != NULL) {
        pView->GetGraph()->Refresh();
        pView->GetGraph()->SetFocus();
    }
}

void wxStfChildFrame::OnComboActChannel(wxCommandEvent& WXUNUSED(event)) {
    if ( pActChannel->GetCurrentSelection() == pInactChannel->GetCurrentSelection()) {
        // correct selection:
        for (int n_c=0;n_c<(int)pActChannel->GetCount();++n_c) {
            if (n_c!=pActChannel->GetCurrentSelection()) {
                pInactChannel->SetSelection(n_c);
                break;
            }
        }
    }

    UpdateChannels();
}

void wxStfChildFrame::OnComboInactChannel(wxCommandEvent& WXUNUSED(event)) {
    if (pInactChannel->GetCurrentSelection()==pActChannel->GetCurrentSelection()) {
        // correct selection:
        for (int n_c=0;n_c<(int)pInactChannel->GetCount();++n_c) {
            if (n_c!=pInactChannel->GetCurrentSelection()) {
                pActChannel->SetSelection(n_c);
                break;
            }
        }
    }

    UpdateChannels();
}

void wxStfChildFrame::UpdateChannels( ) {

    wxStfDoc* pDoc=(wxStfDoc*)GetDocument();

    if ( pDoc != NULL && pDoc->size() > 1) {
        try {
            pDoc->SetCurCh( pActChannel->GetCurrentSelection() );
            pDoc->SetSecCh( pInactChannel->GetCurrentSelection() );
        }
        catch (const std::out_of_range& e) {
            wxString msg(wxT("Error while changing channels\nPlease close file\n"));
            msg += wxString( e.what(), wxConvLocal );
            wxGetApp().ExceptMsg(msg);
            return;
        }

        // Update measurements:
        wxGetApp().OnPeakcalcexecMsg();
        UpdateResults();
        wxStfView* pView=(wxStfView*)GetView();
        if ( pView == NULL ) {
            wxGetApp().ErrorMsg( wxT("View is zero in wxStfDoc::SwapChannels"));
            return;
        }
        if (pView->GetGraph() != NULL) {
            pView->GetGraph()->Refresh();
            pView->GetGraph()->SetFocus();
        }
    }
}

void wxStfChildFrame::OnPlotselected(wxCommandEvent& WXUNUSED(event)) {
    wxStfView* pView=(wxStfView*)GetView();
    if (pView != NULL && pView->GetGraph()!= NULL) { 
        pView->GetGraph()->Refresh();
        pView->GetGraph()->SetFocus();
    }
}

void wxStfChildFrame::OnShowsecond(wxCommandEvent& WXUNUSED(event)) {
    wxStfView* pView=(wxStfView*)GetView();
    if (pView != NULL && pView->GetGraph()!= NULL) { 
        pView->GetGraph()->Refresh();
        pView->GetGraph()->SetFocus();
    }
}

void wxStfChildFrame::ActivateGraph() {
    wxStfView* pView=(wxStfView*)GetView();
    // Set the focus somewhere else:
    if (m_traceCounter != NULL) 
        m_traceCounter->SetFocus();
    if (pView != NULL && pView->GetGraph()!= NULL) { 
        pView->GetGraph()->SetFocus();
    }
}

void wxStfChildFrame::ShowTable(const stf::Table &table,const wxString& caption) {

    // Create and show notebook if necessary:
    if (m_notebook==NULL && !m_mgr.GetPane(m_notebook).IsOk()) {
        m_notebook=CreateNotebook();
        m_mgr.AddPane( m_notebook, wxAuiPaneInfo().Caption(wxT("Analysis results")).
                       Floatable().Dock().Left().Name( wxT("Notebook") ) );
    } else {
        // Re-open notebook if it has been closed:
        if (!m_mgr.GetPane(m_notebook).IsShown()) {
            m_mgr.GetPane(m_notebook).Show();
        }
    }
    wxStfGrid* pGrid = new wxStfGrid( m_notebook, wxID_ANY, wxPoint(0,20), wxDefaultSize );
    wxStfTable* pTable(new wxStfTable(table));
    pGrid->SetTable(pTable,true); // the grid will take care of the deletion
    pGrid->SetEditable(false);
    pGrid->SetDefaultCellAlignment(wxALIGN_RIGHT,wxALIGN_CENTRE);
    for (std::size_t n_row=0; n_row<=table.nRows()+1; ++n_row) {
        pGrid->SetCellAlignment(wxALIGN_LEFT,(int)n_row,0);
    }
    m_notebook->AddPage( pGrid, caption, true );

    // "commit" all changes made to wxAuiManager
    m_mgr.Update();
    wxStfView* pView=(wxStfView*)GetView();
    if (pView != NULL && pView->GetGraph()!= NULL) { 
        pView->GetGraph()->SetFocus();
    }
}

void wxStfChildFrame::UpdateResults() {
    wxStfDoc* pDoc=(wxStfDoc*)GetDocument();
    stf::Table table(pDoc->CurResultsTable());
    
    // Delete or append columns:
    if (m_table->GetNumberCols()<(int)table.nCols()) {
        m_table->AppendCols((int)table.nCols()-(int)m_table->GetNumberCols());
    } else {
        if (m_table->GetNumberCols()>(int)table.nCols()) {
            m_table->DeleteCols(0,(int)m_table->GetNumberCols()-(int)table.nCols());
        }
    }

    // Delete or append row:
    if (m_table->GetNumberRows()<(int)table.nRows()) {
        m_table->AppendRows((int)table.nRows()-(int)m_table->GetNumberRows());
    } else {
        if (m_table->GetNumberRows()>(int)table.nRows()) {
            m_table->DeleteRows(0,(int)m_table->GetNumberRows()-(int)table.nRows());
        }
    }

    for (std::size_t nRow=0;nRow<table.nRows();++nRow) {
        // set row label:
        m_table->SetRowLabelValue((int)nRow,table.GetRowLabel(nRow));
        for (std::size_t nCol=0;nCol<table.nCols();++nCol) {
            if (nRow==0) m_table->SetColLabelValue((int)nCol,table.GetColLabel(nCol));
            if (!table.IsEmpty(nRow,nCol)) {
                wxString entry; entry << table.at(nRow,nCol);
                m_table->SetCellValue((int)nRow,(int)nCol,entry);
            } else {
                m_table->SetCellValue((int)nRow,(int)nCol,wxT("n.a."));
            }
        }
    }
}

void wxStfChildFrame::Saveperspective() {
    wxString perspective = m_mgr.SavePerspective();
    // Save to wxConfig:
    wxGetApp().wxWriteProfileString(wxT("Settings"),wxT("Windows"),perspective);
#ifdef _STFDEBUG
    wxFile persp(wxT("perspective.txt"), wxFile::write);
    persp.Write(perspective);
    persp.Close();
#endif
}

void wxStfChildFrame::Loadperspective() {
    wxString perspective = wxGetApp().wxGetProfileString(wxT("Settings"),wxT("Windows"),wxT(""));
    if (perspective!=wxT("")) {
        m_mgr.LoadPerspective(perspective);
    } else {
        wxGetApp().ErrorMsg(wxT("Couldn't find saved windows settings"));
    }
}


void wxStfChildFrame::Restoreperspective() {
    m_mgr.LoadPerspective(defaultPersp);
    m_mgr.Update();
}

void wxStfChildFrame::OnMenuHighlight(wxMenuEvent& event) {
    wxMenuItem *item = this->GetMenuBar()->FindItem(event.GetId());
    if(item)
        wxLogStatus(item->GetHelp());
    event.Skip();

}

#if wxUSE_DRAG_AND_DROP
bool wxStfFileDrop::OnDropFiles(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y), const wxArrayString& filenames) {
    int nFiles=(int)filenames.GetCount();
    if (nFiles>0) {
        return wxGetApp().OpenFileSeries(filenames);
    } else {
        return false;
    }
}
#endif
