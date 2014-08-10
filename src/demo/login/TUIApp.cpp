#include "TUIApp.h"
#include "ui/WndShadow.h"
#include "LoginDlg.h"
#include "ui/QUIDlgs.h"
#include "crypt/QDataEnDe.h"

TUIApp _Module;
DemoConfig _theStkToolConfig;

TUIApp::TUIApp(void)
{
}

TUIApp::~TUIApp(void)
{
}

BOOL TUIApp::InitRun()
{
	DemoConfig *pCfg = (DemoConfig*)QUIGetConfig();
	
	// ��ʼ����ӰЧ�����
	CWndShadow::Initialize(QUIGetInstance());
	SetTopFrameStyle(WS_QEX_ROUNDCONNER|WS_QEX_WNDSHADOW);

	//////////////////////////////////////////////////////////////////////////
	// ��¼ϵͳ
	LoginDlg().DoModal(NULL, WS_EX_TOOLWINDOW|WS_EX_TOPMOST);

	return FALSE;
}
