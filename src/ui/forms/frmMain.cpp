//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmMain.cpp - The main form
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/splitter.h>
#include <wx/toolbar.h>
#include <wx/tbarsmpl.h>
#include <wx/stc/stc.h>

// App headers
#include "../../pgAdmin3.h"
#include "frmMain.h"
#include "frmAbout.h"
#include "../controls/ctlSQLBox.h"

// Event table
BEGIN_EVENT_TABLE(frmMain, wxFrame)
EVT_MENU(mnuExit,  frmMain::OnExit)
EVT_MENU(mnuAbout, frmMain::OnAbout)
END_EVENT_TABLE()

frmMain::frmMain(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{

  // Icon
#ifdef __WXMSW__
  SetIcon(wxIcon("images/pgAdmin3.xpm"));
#else
  SetIcon(wxIcon("images/pgAdmin3.xpm", wxBITMAP_TYPE_XPM));
#endif

  // Build menus
  mnuBar = new wxMenuBar();

  // File Menu
  wxMenu *mnuFile = new wxMenu;
  mnuFile->Append(mnuConnect, "&Connect...", "Connect to a PostgreSQL server");
  mnuFile->Append(mnuPassword, "C&hange password...", "Change your password");
  mnuFile->AppendSeparator();
  mnuFile->Append(mnuSaveDefinition, "&Save definition...", "Save the SQL definition of the selected object");
  mnuFile->Append(mnuSaveSchema, "S&ave DB schema...", "Save the schema of the current database");
  mnuFile->AppendSeparator();
  mnuFile->Append(mnuExit, "E&xit", "Quit this program");
  mnuBar->Append(mnuFile, "&File");

  // Tools Menu
  wxMenu *mnuTools = new wxMenu;
  mnuTools->Append(mnuUpgradeWizard, "&Upgrade Wizard...", "Run the upgrade wizard");
  mnuTools->AppendSeparator();
  mnuTools->Append(mnuOptions, "&Options...", "Show options dialog");
  mnuBar->Append(mnuTools, "&Tools");

  // View Menu
  wxMenu *mnuView = new wxMenu;
  mnuView->Append(mnuSystemObjects, "&System objects", "Show or hide system objects");
  mnuBar->Append(mnuView, "&View");

  // Help Menu
  wxMenu *mnuHelp = new wxMenu;
  mnuHelp->Append(mnuContents, "&Help...", "Open the helpfile");
  mnuHelp->Append(mnuTipOfTheDay, "&Tip of the day...", "Show a tip of the day");
  mnuHelp->AppendSeparator();
  mnuHelp->Append(mnuAbout, "&About...", "Show about dialog");
  mnuBar->Append(mnuHelp, "&Help");

  // Add the Menubar
  SetMenuBar(mnuBar);

  // Status bar
  CreateStatusBar(6);
  static const int iWidths[6] = {-1, 50, 100, 100, 100, 100};
  SetStatusWidths(6, iWidths);
  SetStatusText("Ready.", 0);
  SetStatusText("0 Secs", 1);
  SetStatusText("Object: None", 2);
  SetStatusText("Schema: None", 3);
  SetStatusText("Database: None", 4);
  SetStatusText("Server: None", 5);

  // Toolbar bar

  CreateToolBar();

  // Return objects
  stBar = GetStatusBar();
  tlBar = GetToolBar();

     // Set up toolbar
    wxBitmap tlBarBitmaps[10];

    tlBarBitmaps[0] = wxBitmap("images/connect.xpm");
    tlBarBitmaps[1] = wxBitmap("images/refresh.xpm");
    tlBarBitmaps[2] = wxBitmap("images/create.xpm");
    tlBarBitmaps[3] = wxBitmap("images/drop.xpm");
    tlBarBitmaps[4] = wxBitmap("images/properties.xpm");
    tlBarBitmaps[5] = wxBitmap("images/sql.xpm");
    tlBarBitmaps[6] = wxBitmap("images/viewdata.xpm");
    tlBarBitmaps[7] = wxBitmap("images/vacuum.xpm");
    tlBarBitmaps[8] = wxBitmap("images/record.xpm");
    tlBarBitmaps[9] = wxBitmap("images/stop.xpm");
    tlBar->AddTool(100, _T("Save"), tlBarBitmaps[0], _T("Toggle button 1"), wxITEM_CHECK);
    tlBar->AddTool(101, _T("Save"), tlBarBitmaps[1], _T("Toggle button 1"), wxITEM_CHECK);
    tlBar->AddTool(102, _T("Save"), tlBarBitmaps[2], _T("Toggle button 1"), wxITEM_CHECK);
    tlBar->AddTool(103, _T("Save"), tlBarBitmaps[3], _T("Toggle button 1"), wxITEM_CHECK);
    tlBar->AddTool(104, _T("Save"), tlBarBitmaps[4], _T("Toggle button 1"), wxITEM_CHECK);
    tlBar->AddTool(105, _T("Save"), tlBarBitmaps[5], _T("Toggle button 1"), wxITEM_CHECK);
    tlBar->AddTool(106, _T("Save"), tlBarBitmaps[6], _T("Toggle button 1"), wxITEM_CHECK);
    tlBar->AddTool(107, _T("Save"), tlBarBitmaps[7], _T("Toggle button 1"), wxITEM_CHECK);
    tlBar->AddTool(108, _T("Save"), tlBarBitmaps[8], _T("Toggle button 1"), wxITEM_CHECK);
    tlBar->AddTool(109, _T("Save"), tlBarBitmaps[9], _T("Toggle button 1"), wxITEM_CHECK);



    // Setup the vertical splitter & treeview
  wxSplitterWindow* splVertical = new wxSplitterWindow(this, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN);
  wxSplitterWindow* splHorizontal = new wxSplitterWindow(splVertical, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN);
  tvBrowser = new wxTreeCtrl(splVertical, -1, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxSIMPLE_BORDER);
  splVertical->SplitVertically(tvBrowser, splHorizontal, 200);
  splVertical->SetMinimumPaneSize(50);

  // Setup the horizontal splitter for the listview & sql pane
  wxNotebook* nbListViews = new wxNotebook(splHorizontal, -1, wxDefaultPosition, wxDefaultSize, wxNB_BOTTOM);
  lvProperties = new wxListCtrl(nbListViews, -1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxSIMPLE_BORDER);
  lvStatistics = new wxListCtrl(nbListViews, -1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxSIMPLE_BORDER);
  nbListViews->AddPage(lvProperties, "Properties");
  nbListViews->AddPage(lvStatistics, "Statistics");
  txtSQLPane = new ctlSQLBox(splHorizontal, -1, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxSIMPLE_BORDER | wxTE_READONLY | wxTE_RICH2);
  txtSQLPane->SetBackgroundColour(*wxLIGHT_GREY);
  splHorizontal->SplitHorizontally(nbListViews, txtSQLPane, 300);
  splHorizontal->SetMinimumPaneSize(50);

  // Add some treeview items
  wxTreeItemId itmDummy = tvBrowser->AddRoot("Root node");
	tvBrowser->AppendItem(itmDummy, "Child Node #1");
	tvBrowser->AppendItem(itmDummy, "Child Node #2");
	tvBrowser->Expand(itmDummy);
	itmDummy = tvBrowser->AppendItem(itmDummy, "Child Node #3");
	tvBrowser->AppendItem(itmDummy, "Child Node #4");
	tvBrowser->AppendItem(itmDummy, "Child Node #5");

  // Add some listview items
	lvProperties->InsertColumn(1, "Property", wxLIST_FORMAT_LEFT, 100);
	lvProperties->InsertColumn(2, "Value", wxLIST_FORMAT_LEFT, 400);
	lvProperties->InsertItem(1, "Property #1");
	lvProperties->InsertItem(1, "Property #2");
	lvProperties->InsertItem(1, "Property #3");
	lvStatistics->InsertColumn(1, "Statistic", wxLIST_FORMAT_LEFT, 100);
	lvStatistics->InsertColumn(2, "Value", wxLIST_FORMAT_LEFT, 400);
	lvStatistics->InsertItem(1, "Statistic #1");
	lvStatistics->InsertItem(1, "Statistic #2");
	lvStatistics->InsertItem(1, "Statistic #3");

  // Setup the SQL Pane
  txtSQLPane->InsertText(0, "-- Select all records from pg_class\nSELECT\n  *\nFROM\n  pg_class\nWHERE\n relname LIKE 'pg_%'\nORDER BY\n  rename;");

}

// Event handlers
void frmMain::OnExit(wxCommandEvent& WXUNUSED(event))
{
  // TRUE is to force the frame to close
  Close(TRUE);
}

void frmMain::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  frmAbout *winAbout = new frmAbout(this);
  winAbout->Show(TRUE);
}
