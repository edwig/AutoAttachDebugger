////////////////////////////////////////////////////////////////////////
//
// File: AutoAttachDialog.h
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
#pragma once
#include <vector>

enum class DebugType
{
  DT_UNKNOWN
 ,DT_VISUALSTUDIO
 ,DT_WINDOWSDBG
};

using ProgramArray = std::vector<CString>;

// AutoAttachDialog dialog
class AutoAttachDialog : public StyleDialog
{
// Construction
public:
  AutoAttachDialog(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
  enum { IDD = IDD_AUTOATTACH };

  protected:
  virtual void DoDataExchange(CDataExchange* pDX) override;


// Implementation
protected:
  CString       m_program;
  CString       m_debugger;
  DebugType     m_type { DebugType::DT_UNKNOWN };
  ProgramArray  m_programs;
  bool          m_debugging { false };
  bool          m_changed   { false };

  HICON         m_hIcon = NULL;
  StyleComboBox m_comboProgram;
  StyleComboBox m_comboDebugger;
  StyleEdit     m_editDebugger;
  StyleButton   m_buttonChoose;
  StyleButton   m_buttonSet;
  StyleButton   m_buttonReset;
  StyleButton   m_buttonReady;
  CBitmapButton m_buttonDebugNo;
  CBitmapButton m_buttonDebugYes;

  // Generated message map functions
  virtual BOOL OnInitDialog() override;
          void InitFields();
          void InitPrograms();
          void WritePrograms();
          void CheckExtension();
          void SetDebugIcon();
          void GetDebuggingState();
          void SetDebugger(CString p_program,CString p_debugger);
          void RemoveDebugger(CString p_program);

  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();

  DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnCbnCloseupProgram();
  afx_msg void OnCbnCloseupDebuggerType();
  afx_msg void OnEnChangeDebugger();
  afx_msg void OnBnClickedChoose();
  afx_msg void OnBnClickedButtonSet();
  afx_msg void OnBnClickedButtonReset();
  afx_msg void OnBnClickedButtonReady();
};
