////////////////////////////////////////////////////////////////////////
//
// File: AutoAttachDialog.cpp
// Function: Program to set the auto-attaching of a debugger
// 
//   _____ _         _ _             ______                                           _    
//  / ____| |       | (_)           |  ____|                                         | |   
// | (___ | |_ _   _| |_ _ __   __ _| |__ _ __ __ _ _ __ ___   _____      _____  _ __| | __
//  \___ \| __| | | | | | '_ \ / _` |  __| '__/ _` | '_ ` _ \ / _ \ \ /\ / / _ \| '__| |/ /
//  ____) | |_| |_| | | | | | | (_| | |  | | | (_| | | | | | |  __/\ V  V / (_) | |  |   < 
// |_____/ \__|\__, |_|_|_| |_|\__, |_|  |_|  \__,_|_| |_| |_|\___| \_/\_/ \___/|_|  |_|\_\
//              __/ |           __/ |                                                      
//             |___/           |___/                                                       
//
//
// Author: ir. W.E. Huisman
// For license: See the file "LICENSE.txt" in the root folder
//
#include "pch.h"
#include "framework.h"
#include "AutoAttachDebugger.h"
#include "AutoAttachDialog.h"
#include "FileDialog.h"
#include <afxdialogex.h>
#include <RegistryManager.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// This is the key where the debugger functionallity resides
const CString debugkey = _T("HKLM\\Software\\Microsoft\\Windows NT\\currentversion\\image file execution options");

// AboutDlg dialog used for App About

class AboutDlg : public StyleDialog
{
public:
  AboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_ABOUTBOX };
#endif
private:
  StyleButton m_buttonOK;

  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
  DECLARE_MESSAGE_MAP()
};

AboutDlg::AboutDlg() : StyleDialog(IDD_ABOUTBOX)
{
}

void AboutDlg::DoDataExchange(CDataExchange* pDX)
{
  StyleDialog::DoDataExchange(pDX);
  DDX_Control(pDX,IDOK,m_buttonOK);
}

BEGIN_MESSAGE_MAP(AboutDlg, StyleDialog)
END_MESSAGE_MAP()

// AutoAttachDialog dialog

AutoAttachDialog::AutoAttachDialog(CWnd* pParent /*=nullptr*/)
                 :StyleDialog(IDD_AUTOATTACH, pParent)
{
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void AutoAttachDialog::DoDataExchange(CDataExchange* pDX)
{
  StyleDialog::DoDataExchange(pDX);

  DDX_CBString(pDX,IDC_PROGRAM,  m_comboProgram,m_program);
  DDX_Control (pDX,IDC_DBTYPE,   m_comboDebugger);
  DDX_Control (pDX,IDC_DEBUGGER, m_editDebugger,m_debugger);
  DDX_Control (pDX,IDC_CHOOSE,   m_buttonChoose);
  DDX_Control (pDX,IDC_SET,      m_buttonSet);
  DDX_Control (pDX,IDC_DEBUG_NO, m_buttonDebugNo);
  DDX_Control (pDX,IDC_DEBUG_YES,m_buttonDebugYes);
  DDX_Control (pDX,IDC_RESET,    m_buttonReset);
  DDX_Control (pDX,IDCANCEL,     m_buttonReady);

  if(pDX->m_bSaveAndValidate == FALSE)
  {
    SetDebugIcon();
  }
}

BEGIN_MESSAGE_MAP(AutoAttachDialog, StyleDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_CBN_KILLFOCUS(IDC_PROGRAM,&AutoAttachDialog::OnCbnCloseupProgram)
  ON_CBN_SELCHANGE(IDC_PROGRAM,&AutoAttachDialog::OnCbnCloseupProgram)
  ON_CBN_SELCHANGE(IDC_DBTYPE, &AutoAttachDialog::OnCbnCloseupDebuggerType)
  ON_BN_CLICKED(IDC_CHOOSE,    &AutoAttachDialog::OnBnClickedChoose)
  ON_BN_CLICKED(IDC_SET,       &AutoAttachDialog::OnBnClickedButtonSet)
  ON_BN_CLICKED(IDC_RESET,     &AutoAttachDialog::OnBnClickedButtonReset)
  ON_BN_CLICKED(IDCANCEL,      &AutoAttachDialog::OnBnClickedButtonReady)
END_MESSAGE_MAP()


// AutoAttachDialog message handlers

BOOL AutoAttachDialog::OnInitDialog()
{
  StyleDialog::OnInitDialog();
  SetWindowText(_T("Auto attach to debugger"));
  ShowMinMaxButton(true,false);
  SetAboutBoxAndIcon(IDM_ABOUTBOX,IDS_ABOUTBOX);
  ShowSysMenu();

  InitFields();
  InitPrograms();

  UpdateData(FALSE);
  return TRUE;  // return TRUE  unless you set the focus to a control
}

void
AutoAttachDialog::InitFields()
{
  m_comboDebugger.AddString(_T("Visual Studio"));
  m_comboDebugger.AddString(_T("Windows debugger"));
  m_comboDebugger.SetCurSel(0);
  m_type = DebugType::DT_VISUALSTUDIO;

  m_editDebugger.SetMutable(false);
  m_buttonChoose.EnableWindow(false);

  m_buttonDebugNo .LoadBitmaps(MAKEINTRESOURCE(IDB_DEBUG_NO));
  m_buttonDebugYes.LoadBitmaps(MAKEINTRESOURCE(IDB_DEBUG_YES));
}

void
AutoAttachDialog::SetDebugIcon()
{
  GetDebuggingState();
  m_buttonDebugNo .ShowWindow(m_debugging ? SW_HIDE : SW_SHOW);
  m_buttonDebugYes.ShowWindow(m_debugging ? SW_SHOW : SW_HIDE);
}

void
AutoAttachDialog::GetDebuggingState()
{
  m_debugging = false;
  if(m_program.IsEmpty())
  {
    // No program to check (yet)
    return;
  }

  RegistryManager manager(HKEY_LOCAL_MACHINE);
  CString fullkey = debugkey + _T("\\") + m_program;

  CString debugger = manager.GetRegistryString(fullkey,_T("Debugger"),_T(""));
  if(debugger.IsEmpty())
  {
    // Not set. We do NOT debug at the program start
    return;
  }
  if(m_debugger.IsEmpty())
  {
    // Debugger found and no custom debugger: We do NOT debug at the program start
    m_debugging = true;
    return;
  }
  // Check if the expected debugger is set in the registry.
  // Otherwise we do debug, but not with the expected debugger.
  m_debugging = debugger.CompareNoCase(m_debugger) == 0;
}

void
AutoAttachDialog::InitPrograms()
{
  CString filename = _T("AutoAttachPrograms.ini");
  FILE* file(nullptr);
  _tfopen_s(&file,filename,_T("rt"));
  if(file)
  {
    TCHAR buffer[MAX_PATH + 1];
    while(fgets(buffer,MAX_PATH,file))
    {
      CString line(buffer);
      if(line.Right(1) == _T("\n"))
      {
        line = line.Left(line.GetLength() - 1);
      }
      m_programs.push_back(line);
      m_comboProgram.AddString(line);
    }
    fclose(file);
  }

  if(!m_programs.empty())
  {
    m_comboProgram.SetCurSel(0);
    m_program = m_programs[0];
  }
}

void
AutoAttachDialog::WritePrograms()
{
  if(m_changed == false)
  {
    return;
  }

  CString filename = _T("AutoAttachPrograms.ini");
  FILE* file(nullptr);
  _tfopen_s(&file,filename,_T("wt"));
  if(file)
  {
    if(!m_program.IsEmpty())
    {
      fprintf(file,_T("%s\n"),m_program.GetString());
    }

    for(auto& line : m_programs)
    {
      if(line.CompareNoCase(m_program))
      {
        fprintf(file,_T("%s\n"),line.GetString());
      }
    }
    fclose(file);
  }  
  m_changed = false;
}

void
AutoAttachDialog::CheckExtension()
{
  CString program(m_program);
  program.MakeLower();
  int pos = program.ReverseFind(_T('.'));
  if(pos > 0)
  {
    CString extension = program.Mid(pos + 1);
    if(extension.Compare(_T("exe")))
    {
      StyleMessageBox(this,_T("Normally you can only debug *.exe files!!"),_T("ERROR"),MB_OK|MB_ICONWARNING);
    }
  }
  else
  {
    m_program += _T(".exe");
    UpdateData(FALSE);
  }
}

void
AutoAttachDialog::SetDebugger(CString p_program,CString p_debugger)
{
  // See to it that we have a debugger path
  if(p_debugger.IsEmpty())
  {
    CString winddir;
    if(!winddir.GetEnvironmentVariable(_T("windir")))
    {
      winddir = _T("C:\\Windows");
    }
    p_debugger = winddir + _T("\\system32\\vsjitdebugger.exe");
  }

  RegistryManager manager(HKEY_LOCAL_MACHINE);
  CString fullkey = debugkey + _T("\\") + p_program;

  if(manager.SetRegistryString(fullkey,_T("Debugger"),p_debugger))
  {
    CreateToast(STYLE_TOAST_MESSAGE,STYLE_POS_BOTRIGHT,_T("Debugger set to auto-attach to: ") + p_program);
  }
  else
  {
    CreateToast(STYLE_TOAST_ERROR,STYLE_POS_BOTRIGHT,_T("Debugger could **NOT** be set to auto-attach!"));
  }
}

void 
AutoAttachDialog::RemoveDebugger(CString p_program)
{
  RegistryManager manager(HKEY_LOCAL_MACHINE);
  if(manager.DeleteRegistryKey(debugkey,p_program))
  {
    CreateToast(STYLE_TOAST_MESSAGE,STYLE_POS_BOTRIGHT,_T("Debugger auto-attachment is removed from: ") + p_program);
  }
  else
  {
    CreateToast(STYLE_TOAST_ERROR,STYLE_POS_BOTRIGHT,_T("Debugger auto-attachment removal FAILED!"));
  }
}

//////////////////////////////////////////////////////////////////////////
//
// Message handlers
//
//////////////////////////////////////////////////////////////////////////

void AutoAttachDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
  if ((nID & 0xFFF0) == IDM_ABOUTBOX)
  {
    AboutDlg dlgAbout;
    dlgAbout.DoModal();
  }
  else
  {
    StyleDialog::OnSysCommand(nID, lParam);
  }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void AutoAttachDialog::OnPaint()
{
  if (IsIconic())
  {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  }
  else
  {
    StyleDialog::OnPaint();
  }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR AutoAttachDialog::OnQueryDragIcon()
{
  return static_cast<HCURSOR>(m_hIcon);
}

void
AutoAttachDialog::OnCbnCloseupProgram()
{
  UpdateData();
  CheckExtension();

  for(auto& program : m_programs)
  {
    if(program.CompareNoCase(m_program) == 0)
    {
      return;
    }
  }
  m_programs.push_back(m_program);
  m_comboProgram.AddString(m_program);
  m_changed = true;
  UpdateData(FALSE);
}

void
AutoAttachDialog::OnCbnCloseupDebuggerType()
{
  int ind = m_comboDebugger.GetCurSel();
  switch(ind)
  {
    case 0: m_type = DebugType::DT_VISUALSTUDIO;
                     m_debugger.Empty();
                     break;
    case 1: m_type = DebugType::DT_WINDOWSDBG;
                     break;
  }
  m_editDebugger.SetMutable  (m_type == DebugType::DT_WINDOWSDBG);
  m_buttonChoose.EnableWindow(m_type == DebugType::DT_WINDOWSDBG);

  UpdateData(FALSE);
}

void
AutoAttachDialog::OnEnChangeDebugger()
{
  UpdateData();
  UpdateData(FALSE);
}

void
AutoAttachDialog::OnBnClickedChoose()
{
  DocFileDialog dlg(GetSafeHwnd(),true,_T("Select a windbg-like debugger")
                   ,_T("exe"),_T(""),0,_T("All files|*.*"));
  if(dlg.DoModal() == IDOK)
  {
    m_debugger = dlg.GetChosenFile();
    UpdateData(FALSE);
  }
}

void
AutoAttachDialog::OnBnClickedButtonSet()
{
  SetDebugger(m_program,m_debugger);
  UpdateData(FALSE);
}

void
AutoAttachDialog::OnBnClickedButtonReset()
{
  RemoveDebugger(m_program);
  UpdateData(FALSE);
}

void
AutoAttachDialog::OnBnClickedButtonReady()
{
  WritePrograms();
  StyleDialog::OnCancel();
}
