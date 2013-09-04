// Copyright (c) 2013 universeroc. All rights reserved.
//
// universeroc@gmail.com
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "resource.h"

#include "MainDlg.h"


#define MAX_STRING_LEN 1000

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	UIUpdateChildWindows();
	return FALSE;
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(hIconSmall, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

  SetWindowLong(GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_LAYERED);

  SetLayeredWindowAttributes(m_hWnd, RGB(255, 0, 255), 220, LWA_COLORKEY | LWA_ALPHA);


  RECT rect;
  GetWindowRect(&rect);
  int w = rect.right - rect.left;
  int h = rect.bottom - rect.top;

  ::GetWindowRect(GetDesktopWindow(), &rect);
  int width = rect.right - rect.left; // GetSystemMetrics(SM_CXFULLSCREEN);
  int height = rect.bottom - rect.top; // GetSystemMetrics(SM_CYFULLSCREEN);
  height -= 40;  // taskbar height

  SetWindowPos(HWND_TOPMOST, width - w, height - h, w, h, SWP_SHOWWINDOW);

  SetTimer(kTimerID, kTimerInterval, 0);
  SetTimer(kIconTimerID, kIconTimerInterval, 0);

	return TRUE;
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  KillTimer(kTimerID);
  KillTimer(kIconTimerID);

	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CloseDialog(wID);
	return 0;
}

void CMainDlg::GetBasicInfo() {
  POINT pt;
  GetCursorPos(&pt);
  HWND hwnd = WindowFromPoint(pt);
  wchar_t text_buffer[MAX_PATH + 1] = {0};
  wsprintf(text_buffer, L"0x%08X", hwnd);

  // handle info
  SetDlgItemText(IDC_HANDLE, text_buffer);

  // class name
  memset(text_buffer, L'\0', MAX_PATH);
  GetClassName(hwnd, text_buffer, MAX_PATH);
  SetDlgItemText(IDC_CLASSNAME, text_buffer);

  // caption
  memset(text_buffer, L'\0', MAX_PATH);
  ::GetWindowText(hwnd, text_buffer, MAX_PATH);
  SetDlgItemText(IDC_CAPTION, text_buffer);

  // style
  LONG style = ::GetWindowLong(hwnd, GWL_STYLE);
  memset(text_buffer, L'\0', MAX_PATH);
  wsprintf(text_buffer, L"0x%08X", style);
  SetDlgItemText(IDC_STYLE, text_buffer);

  // rect
  RECT rect;
  ::GetWindowRect(hwnd, &rect);
  memset(text_buffer, L'\0', MAX_PATH);
  wsprintf(text_buffer, L"(%d, %d) - (%d, %d)", rect.left, rect.top, rect.right, rect.bottom);
  SetDlgItemText(IDC_RECT, text_buffer);
}

void CMainDlg::GetAdvancedInfo() {

  typedef struct { LONG ID; const wchar_t* t; } StyleMap;
  static StyleMap style_map[] = {
    // { WS_OVERLAPPED, L"WS_OVERLAPPED" },
    { WS_POPUP, L"WS_POPUP" },
    { WS_CHILD, L"WS_CHILD" },
    { WS_MINIMIZE, L"WS_MINIMIZE" },
    { WS_VISIBLE, L"WS_VISIBLE" },
    { WS_DISABLED, L"WS_DISABLED" },
    { WS_CLIPSIBLINGS, L"WS_CLIPSIBLINGS" },
    { WS_CLIPCHILDREN, L"WS_CLIPCHILDREN" },
    { WS_MAXIMIZE, L"WS_MAXIMIZE" },
    { WS_CAPTION, L"WS_CAPTION" },
    { WS_BORDER, L"WS_BORDER" },
    { WS_DLGFRAME, L"WS_DLGFRAME" },
    { WS_VSCROLL, L"WS_VSCROLL" },
    { WS_HSCROLL, L"WS_HSCROLL" },
    { WS_SYSMENU, L"WS_SYSMENU" },
    { WS_THICKFRAME, L"WS_THICKFRAME" },
    { WS_GROUP, L"WS_GROUP" },
    { WS_TABSTOP, L"WS_TABSTOP" },
    { WS_MINIMIZEBOX, L"WS_MINIMIZEBOX" },
    { WS_MAXIMIZEBOX, L"WS_MAXIMIZEBOX" },
    { WS_TILED, L"WS_TILED" },
    { WS_ICONIC, L"WS_ICONIC" },
    { WS_SIZEBOX, L"WS_SIZEBOX" },
    { WS_TILEDWINDOW, L"WS_TILEDWINDOW" },
  };

  static StyleMap exstyle_map[] = {
    // exstyle
    { WS_EX_DLGMODALFRAME, L"WS_EX_DLGMODALFRAME" },
    { WS_EX_NOPARENTNOTIFY, L"WS_EX_NOPARENTNOTIFY" },
    { WS_EX_TOPMOST, L"WS_EX_TOPMOST" },
    { WS_EX_ACCEPTFILES, L"WS_EX_ACCEPTFILES" },
    { WS_EX_TRANSPARENT, L"WS_EX_TRANSPARENT" },

#if (WINVER >= 0x0400)
    { WS_EX_MDICHILD, L"WS_EX_MDICHILD" },
    { WS_EX_TOOLWINDOW, L"WS_EX_TOOLWINDOW" },
    { WS_EX_WINDOWEDGE, L"WS_EX_WINDOWEDGE" },
    { WS_EX_CLIENTEDGE, L"WS_EX_CLIENTEDGE" },
    { WS_EX_CONTEXTHELP, L"WS_EX_CONTEXTHELP" },
#endif

#if(WINVER >= 0x0400)
    { WS_EX_RIGHT, L"WS_EX_RIGHT" },
    { WS_EX_LEFT, L"WS_EX_LEFT" },
    { WS_EX_RTLREADING, L"WS_EX_RTLREADING" },
    { WS_EX_LTRREADING, L"WS_EX_LTRREADING" },
    { WS_EX_LEFTSCROLLBAR, L"WS_EX_LEFTSCROLLBAR" },
    { WS_EX_RIGHTSCROLLBAR, L"WS_EX_RIGHTSCROLLBAR" },
    { WS_EX_CONTROLPARENT, L"WS_EX_CONTROLPARENT" },
    { WS_EX_STATICEDGE, L"WS_EX_STATICEDGE" },
    { WS_EX_APPWINDOW, L"WS_EX_APPWINDOW" },
#endif

#if(_WIN32_WINNT >= 0x0500)
    { WS_EX_LAYERED, L"WS_EX_LAYERED" },
#endif

#if(WINVER >= 0x0500)
    { WS_EX_NOINHERITLAYOUT, L"WS_EX_NOINHERITLAYOUT" },
    { WS_EX_LAYOUTRTL, L"WS_EX_LAYOUTRTL" },
    
#endif

#if(_WIN32_WINNT >= 0x0501)
    { WS_EX_COMPOSITED, L"WS_EX_COMPOSITED" },
#endif

#if(_WIN32_WINNT >= 0x0500)
    { WS_EX_NOACTIVATE, L"WS_EX_NOACTIVATE" },
#endif
  };

  static int style_map_len = sizeof(style_map) / sizeof(StyleMap);
  static int exstyle_map_len = sizeof(exstyle_map) / sizeof(StyleMap);

  POINT pt;
  GetCursorPos(&pt);
  HWND hwnd = WindowFromPoint(pt);
  wchar_t text_buffer[MAX_STRING_LEN + 1] = {0};

  // style
  LONG style = 0;
  style = ::GetWindowLong(hwnd, GWL_STYLE);
  memset(text_buffer, L'\0', MAX_STRING_LEN + 1);
  if (style) {
    for (int i = 0; i < style_map_len; ++i) {
      if (style & style_map[i].ID)
        wsprintf(text_buffer, L"%s | %s", text_buffer, style_map[i].t);
    }
  }
  
  SetDlgItemText(IDC_STYLE_ADVANCE, &text_buffer[3]);  // skip the " | " characters


  // exstyle
  LONG exstyle = 0;
  exstyle = ::GetWindowLong(hwnd, GWL_EXSTYLE);
  memset(text_buffer, L'\0', MAX_STRING_LEN + 1);
  if (exstyle) {
    for (int i = 0; i < exstyle_map_len; ++i) {
      if (exstyle & exstyle_map[i].ID)
        wsprintf(text_buffer, L"%s | %s", text_buffer, exstyle_map[i].t);
    }
  }  

  SetDlgItemText(IDC_EX_STYLE, &text_buffer[3]);  // skip the " | " characters

  // parent
  HWND parent = ::GetParent(hwnd);
  memset(text_buffer, L'\0', MAX_STRING_LEN + 1);
  wsprintf(text_buffer, L"0x%08X", parent);
  SetDlgItemText(IDC_PARENT_HANDLE, text_buffer);

  
  memset(text_buffer, L'\0', MAX_STRING_LEN + 1);
  ::GetWindowText(parent, text_buffer, MAX_STRING_LEN + 1);
  SetDlgItemText(IDC_PARENT_CAPTION, text_buffer);


  // child
  HWND child = ::ChildWindowFromPoint(hwnd, pt);
  memset(text_buffer, L'\0', MAX_STRING_LEN + 1);
  wsprintf(text_buffer, L"0x%08X", child);
  SetDlgItemText(IDC_FIRST_CHILD_HANDLE, text_buffer);
  

  memset(text_buffer, L'\0', MAX_STRING_LEN + 1);
  ::GetWindowText(child, text_buffer, MAX_STRING_LEN + 1);
  SetDlgItemText(IDC_FIRST_CHILD_CAPTION, text_buffer);

  // processid
  DWORD process_id = 0;
  DWORD thread_id = GetWindowThreadProcessId(hwnd, &process_id);

  memset(text_buffer, L'\0', MAX_STRING_LEN + 1);
  wsprintf(text_buffer, L"0x%08X", process_id);
  SetDlgItemText(IDC_PROCESSID, text_buffer);

  // threadid
  memset(text_buffer, L'\0', MAX_STRING_LEN + 1);
  wsprintf(text_buffer, L"0x%08X", thread_id);
  SetDlgItemText(IDC_THREADID, text_buffer);
}

LRESULT CMainDlg::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
  if (wParam == kIconTimerID) {
    static int index = 0;
    if (index == 0) {
      HICON hIcon = AtlLoadIconImage(IDI_ICON_WINDOW8, LR_DEFAULTCOLOR,
        ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
      SetIcon(hIcon, TRUE);
      HICON hIconSmall = AtlLoadIconImage(IDI_ICON_WINDOW8, LR_DEFAULTCOLOR,
        ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
      SetIcon(hIconSmall, FALSE);
      index = 1;
    } else {
      HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR,
        ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
      SetIcon(hIcon, TRUE);
      HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR,
        ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
      SetIcon(hIconSmall, FALSE);
      index = 0;
    }    

  } else if (wParam == kTimerID) {
    POINT pt;
    GetCursorPos(&pt);

    // if no movement, do not refresh the ui.
    if (pt.x == point_.x && pt.y == point_.y)
      return 0;

    point_.x = pt.x;
    point_.y = pt.y;

    // Basic Information
    GetBasicInfo();

    // Advanced Information
    GetAdvancedInfo();
  }
  
  return 0;
}

void CMainDlg::CloseDialog(int nVal)
{
	DestroyWindow();
	::PostQuitMessage(nVal);
}
