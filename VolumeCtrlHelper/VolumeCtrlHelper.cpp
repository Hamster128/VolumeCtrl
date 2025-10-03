
// VolumeCtrlHelper.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "VolumeCtrlHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define INITIAL_PERCENT 20

static const char* SHM_NAME = "VolumeCtrlSharedMemory";

struct VolumeData {
  int percent;
  float fCurrentVol;
};

static VolumeData* shared = nullptr;

// CVolumeCtrlHelperApp

BEGIN_MESSAGE_MAP(CVolumeCtrlHelperApp, CWinApp)
END_MESSAGE_MAP()


// CVolumeCtrlHelperApp construction

CVolumeCtrlHelperApp::CVolumeCtrlHelperApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CVolumeCtrlHelperApp object

CVolumeCtrlHelperApp theApp;


// CVolumeCtrlHelperApp initialization

BOOL CVolumeCtrlHelperApp::InitInstance()
{
	CWinApp::InitInstance();


	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

  HANDLE hMapFile = CreateFileMappingA(
    INVALID_HANDLE_VALUE,    // use paging file (RAM only)
    NULL,
    PAGE_READWRITE,
    0,
    sizeof(VolumeData),
    SHM_NAME
  );

  if (!hMapFile)
  {
    return 1;
  }

  shared = (VolumeData*)MapViewOfFile(
    hMapFile,
    FILE_MAP_ALL_ACCESS,
    0, 0,
    sizeof(VolumeData)
  );

  if (!shared) {
    return 2;
  }

  shared->percent = INITIAL_PERCENT;

  while (1) {
    Sleep(10000);
  }



#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

