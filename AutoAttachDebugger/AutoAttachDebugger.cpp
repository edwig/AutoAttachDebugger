////////////////////////////////////////////////////////////////////////
//
// File: AutoAttachDebugger.cpp
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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// AutoAttachApplication

BEGIN_MESSAGE_MAP(AutoAttachApplication, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// AutoAttachApplication construction

AutoAttachApplication::AutoAttachApplication()
{
}

// The one and only AutoAttachApplication object

AutoAttachApplication theApp;

// AutoAttachApplication initialization

BOOL AutoAttachApplication::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	CShellManager *pShellManager = new CShellManager;

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
  SetRegistryKey(_T("EDO\\AutoAttachDebugger"));

	AutoAttachDialog dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

	ControlBarCleanUp();
	return FALSE;
}

