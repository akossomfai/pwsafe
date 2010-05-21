/*
 * Copyright (c) 2003-2010 Rony Shapiro <ronys@users.sourceforge.net>.
 * All rights reserved. Use of the code is allowed under the
 * Artistic License 2.0 terms, as specified in the LICENSE file
 * distributed with this code, or available from
 * http://www.opensource.org/licenses/artistic-license-2.0.php
 */

/** \file pwsgrid.cpp
* 
*/
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include <utility> // for make_pair
#include "PWSgrid.h"
#include "passwordsafeframe.h" // for DispatchDblClickAction()

////@begin XPM images
////@end XPM images


/*!
 * PWSGrid type definition
 */

IMPLEMENT_CLASS( PWSGrid, wxGrid )


/*!
 * PWSGrid event table definition
 */

BEGIN_EVENT_TABLE( PWSGrid, wxGrid )

////@begin PWSGrid event table entries
  EVT_GRID_CELL_RIGHT_CLICK( PWSGrid::OnCellRightClick )
  EVT_GRID_CELL_LEFT_DCLICK( PWSGrid::OnLeftDClick )
  EVT_CHAR( PWSGrid::OnChar )
  EVT_CONTEXT_MENU(PWSGrid::OnContextMenu)

////@end PWSGrid event table entries

END_EVENT_TABLE()

/*!
 * PWSGrid constructors
 */

PWSGrid::PWSGrid(PWScore &core) : m_core(core)
{
  Init();
}

PWSGrid::PWSGrid(wxWindow* parent, PWScore &core,
                 wxWindowID id, const wxPoint& pos,
                 const wxSize& size, long style) : m_core(core)
{
  Init();
  Create(parent, id, pos, size, style);
}


/*!
 * PWSGrid creator
 */

bool PWSGrid::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
////@begin PWSGrid creation
  wxGrid::Create(parent, id, pos, size, style);
  CreateControls();
////@end PWSGrid creation
  return true;
}


/*!
 * PWSGrid destructor
 */

PWSGrid::~PWSGrid()
{
////@begin PWSGrid destruction
////@end PWSGrid destruction
}


/*!
 * Member initialisation
 */

void PWSGrid::Init()
{
////@begin PWSGrid member initialisation
////@end PWSGrid member initialisation
}


/*!
 * Control creation for PWSGrid
 */

void PWSGrid::CreateControls()
{    
////@begin PWSGrid content construction
////@end PWSGrid content construction
  CreateGrid(0, 2, wxGrid::wxGridSelectRows);
  SetColLabelValue(0, _("Title"));
  SetColLabelValue(1, _("User"));
  SetRowLabelSize(0);
  int w,h;
  GetClientSize(&w, &h);
  int cw = w/2; // 2 = number of columns
  SetColSize(0, cw);
  SetColSize(1, cw);
}

void PWSGrid::OnPasswordListModified()
{
  m_row_map.clear();
  m_uuid_map.clear();
 
  ItemListConstIter iter;
  int row = 0;
  for (iter = m_core.GetEntryIter();
       iter != m_core.GetEntryEndIter();
       iter++) {
    AddItem(iter->second, row);
    row++;
  }
}

/*!
 * Should we show tooltips?
 */

bool PWSGrid::ShowToolTips()
{
  return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap PWSGrid::GetBitmapResource( const wxString& name )
{
  // Bitmap retrieval
////@begin PWSGrid bitmap retrieval
  wxUnusedVar(name);
  return wxNullBitmap;
////@end PWSGrid bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon PWSGrid::GetIconResource( const wxString& name )
{
  // Icon retrieval
////@begin PWSGrid icon retrieval
  wxUnusedVar(name);
  return wxNullIcon;
////@end PWSGrid icon retrieval
}

void PWSGrid::DisplayItem(const CItemData &item, int row)
{
  wxString title = item.GetTitle().c_str();
  wxString user = item.GetUser().c_str();
  SetCellValue(row, 0, title);
  SetCellValue(row, 1, user);
}


void PWSGrid::AddItem(const CItemData &item, int row)
{
  int nRows = GetNumberRows();
  if (row == -1)
    row = nRows;
  if (row >= nRows) // add rows as needed
    AppendRows(row - nRows + 1);
  DisplayItem(item, row);
  uuid_array_t uuid;
  item.GetUUID(uuid);
  m_row_map.insert(std::make_pair(row, CUUIDGen(uuid)));
  m_uuid_map.insert(std::make_pair(CUUIDGen(uuid), row));
}

void PWSGrid::UpdateItem(const CItemData &item)
{  
  uuid_array_t uuid;
  item.GetUUID(uuid);
  UUIDRowMapT::iterator iter = m_uuid_map.find(CUUIDGen(uuid));
  if (iter != m_uuid_map.end()) {
    int row = iter->second;
    DisplayItem(item, row);
  }
}

void PWSGrid::Remove(const uuid_array_t &uuid)
{
  UUIDRowMapT::iterator iter = m_uuid_map.find(CUUIDGen(uuid));
  if (iter != m_uuid_map.end()) {
    int row = iter->second;
    DeleteRows(row);
  }  
}

/*!
 * Returns the number of elements in its own books
 */
size_t PWSGrid::GetNumItems() const
{
	wxASSERT(m_uuid_map.empty() || m_uuid_map.size() == m_core.GetNumEntries());
  wxASSERT(m_row_map.empty() || m_row_map.size() == m_core.GetNumEntries());
	return m_core.GetNumEntries();
}

/*!
 * callback from PWSGridTable.  Removes the item(s) from its own books
 * as well as from PWSCore.  The display will be updated by an event
 * generated by PWSGridTable once this function returns.
 */
void PWSGrid::DeleteItems(int row, size_t numItems)
{
  for (size_t N = 0; N < numItems; ++N) {
    RowUUIDMapT::iterator iter = m_row_map.find(row);
    if (iter != m_row_map.end()) {
      UUIDRowMapT::iterator iter_uuid = m_uuid_map.find(iter->second);
      if (iter_uuid != m_uuid_map.end()) {
        uuid_array_t uuid;
        iter_uuid->first.GetUUID(uuid);
        ItemListIter citer = m_core.Find(uuid);
        if (citer != m_core.GetEntryEndIter()){
          m_core.SuspendOnDBNotification();
          m_core.Execute(DeleteEntryCommand::Create(&m_core,
                                                    m_core.GetEntry(citer)));
          m_core.ResumeOnDBNotification();
        }
        m_uuid_map.erase(iter_uuid);
      }
      m_row_map.erase(iter);    
    }
  }
  if (m_core.IsChanged())
    OnPasswordListModified();
}

/*!
 * callback from PWSGridTable.  Removes all items from its own books
 * as well as from PWSCore.  The items from display grid will be 
 * removed by an event generated by PWSGridTable once this function
 * returns
 */
void PWSGrid::DeleteAllItems()
{
  m_uuid_map.clear();
  m_row_map.clear();
  m_core.ClearData();
}

/*!
 * wxEVT_GRID_CELL_RIGHT_CLICK event handler for ID_LISTBOX
 */

void PWSGrid::OnCellRightClick( wxGridEvent& evt )
{
  // We need this function because wxGrid doesn't convert unprocessed 
  // right-mouse-down events to contextmenu events, so we need to do
  // handle it ourselves
  //
  // This is what we should ideally do, but in the ContextMenu handler below, 
  // I can't convert the mouse position to logical(x,y) to grid's row, column.  
  // The row is always 1 more than where I click
  //
  // wxContextMenuEvent cme(wxEVT_CONTEXT_MENU, GetId(), event.GetPosition());
  // cme.SetEventObject(event.GetEventObject());
  // ProcessEvent(cme);
  //
  SetGridCursor(evt.GetRow(), evt.GetCol());
  SelectRow(evt.GetRow());
  dynamic_cast<PasswordSafeFrame*>(GetParent())->OnContextMenu(GetItem(evt.GetRow()));
}

/*!
 * wxEVT_GRID_CELL_ITEM_MENU event handler for ID_LISTBOX
 */

void PWSGrid::OnContextMenu( wxContextMenuEvent& evt )
{
  wxPoint pos = evt.GetPosition();
  if ( pos == wxDefaultPosition ) { //sent from keyboard?
    const int row = GetGridCursorRow();
    SelectRow(row);
    dynamic_cast<PasswordSafeFrame*>(GetParent())->OnContextMenu(GetItem(row));
  }
  else { //sent from mouse.  I don't know how to convert the mouse coords to grid's row,column
    wxASSERT_MSG(false, wxT("Unexpected wxContextMenuEvent from mouse click"));
  }
}

CItemData *PWSGrid::GetItem(int row) const
{
  if (row < 0 || row > const_cast<PWSGrid *>(this)->GetNumberRows())
    return NULL;
  RowUUIDMapT::const_iterator iter = m_row_map.find(row);
  if (iter != m_row_map.end()) {
    uuid_array_t uuid;
    iter->second.GetUUID(uuid);
    ItemListIter itemiter = m_core.Find(uuid);
    if (itemiter == m_core.GetEntryEndIter())
      return NULL;
    return &itemiter->second;
  }
  return NULL;
}


/*!
 * wxEVT_GRID_CELL_LEFT_DCLICK event handler for ID_LISTBOX
 */

void PWSGrid::OnLeftDClick( wxGridEvent& evt )
{
  CItemData *item = GetItem(evt.GetRow());
  if (item != NULL)
    dynamic_cast<PasswordSafeFrame *>(GetParent())->
      DispatchDblClickAction(*item);
}

 void PWSGrid::SelectItem(const CUUIDGen & uuid)
 {
     UUIDRowMapT::const_iterator itr = m_uuid_map.find(uuid);
     if (itr != m_uuid_map.end()) {
         MakeCellVisible(itr->second, 0);
         wxGrid::SelectRow(itr->second);
     }
 }

int  PWSGrid::FindItemRow(const uuid_array_t& uu)
{
     UUIDRowMapT::const_iterator itr = m_uuid_map.find(CUUIDGen(uu));
     if (itr != m_uuid_map.end()) {
       return itr->second;
     }
     return wxNOT_FOUND;
}

/*!
 * wxEVT_CHAR event handler for ID_LISTBOX
 */

void PWSGrid::OnChar( wxKeyEvent& evt )
{
  if (evt.GetKeyCode() == WXK_ESCAPE &&
      PWSprefs::GetInstance()->GetPref(PWSprefs::EscExits)) {
    GetParent()->Close();
  }
  evt.Skip();
}

