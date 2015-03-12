#include "stdafx.h"

#ifdef _DEBUG
//#include "./dbg/vld/include/vld.h"
#endif

// lib���̺�dll���̣���Ҫ�������ͷ�ļ�����֧��behavior
// #if defined QUI_LIBPRJ || !defined QUI_NOT_DLL
// #pragma message("--->behavior support")
#include "htmlayout/behaviors/support_.h"
//#endif

//#include "stdafx.h"

#include "QApp.h"
#include "QUIMgr.h"
#include "QConfig.h"
#include "QFrame.h"
#include "AppHelper.h"
#include "WndShadow.h"
#include "QMessageLoop.h"

QApp* QUIGetApp()
{
    return dynamic_cast<QApp*>(ATL::_pAtlModule);
}

int QUIRun(__in HINSTANCE hInst)
{
    // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
    ::DefWindowProc(NULL, 0, 0, 0L);

    // AtlInitCommonControls(ICC_BAR_CLASSES); // add flags to support other controls
    int nRet = -1;
    QApp *pApp = QUIGetApp();
    if (NULL != pApp)
    {
        if (SUCCEEDED(pApp->Init(NULL, hInst)))
        {
			// �����Ż�����
            QUIGetInstance();
            nRet = pApp->Run(hInst);
        }
        pApp->Term();
    }
    return nRet;
}

BOOL QUIIsQuiting()
{
    return QUIGetApp()->IsQuiting();
}

CStdString QUIGetAppName()
{
    return QUIGetApp()->GetAppName();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
QApp::~QApp()
{
	m_bIsQuiting = TRUE;
}

QApp::QApp()
{
//	_theApp = this;
	m_pMainWnd = NULL;
	m_dwWndStyle = 0;
}

BOOL QApp::Run( HINSTANCE hInstance )
{
    SetAppName(NULL);

    // ���ù���Ŀ¼
    SetCurrentDirectory(quibase::GetModulePath());

	// ����Ӧ�ó���
    QConfig *pCfg = QUIGetConfig();
	if ((NULL == pCfg) || !(pCfg->SetConfigPath(GetConfigPath())))
	{
        // ���ʱ��û�м���UI������ʹ��QUI�����MessageBox
        ::MessageBox(NULL,L"��ȡ�����ļ�ʧ�ܣ�", L"����", MB_OK);
        ATLASSERT(FALSE);
        return FALSE;
	}
	// ����UI������
	if (!QUIMgr::GetInstance()->Startup())
	{
        ::MessageBox(NULL,L"UI����������ʧ�ܣ�", L"����", MB_OK);
		ATLASSERT(FALSE);
		return FALSE;
	}

	// Ĭ�Ͽ������ڵ�Բ�Ǻ���ӰЧ��
	CWndShadow::Initialize(hInstance);
	SetTopFrameStyle(WS_QEX_ROUNDCONNER|WS_QEX_WNDSHADOW);

	// �������߳�ѭ��
	QMessageLoop loop;
	AddMessageLoop(&loop);

	// ����������
	if (!InitRun() || (m_pMainWnd == NULL))
	{
		return FALSE;
	}

	loop.Run();
	RemoveMessageLoop();

    UnInit();

    QUIGetMainWnd();

	return TRUE;
}

HWND QApp::GetMainSafeWnd()
{
	if (NULL == m_pMainWnd)
	{
		return NULL;
	}
	return m_pMainWnd->GetSafeHwnd();
}

CStdString QApp::GetConfigPath() const
{
    return quibase::GetModulePath() + L"config.ini"; 
}

void QApp::SaveWindowPos()
{
    QConfig *pCfg = QUIGetConfig();
    HWND hWnd = GetMainSafeWnd();
    if (!::IsWindow(hWnd) || (NULL == pCfg))
    {
        ATLASSERT(FALSE);
        return;
    }

    WINDOWPLACEMENT wp = { 0 };
    wp.length = sizeof(WINDOWPLACEMENT);

    // get window position and iconized/maximized status
    ::GetWindowPlacement(hWnd, &wp);

    wp.flags   = 0;
    wp.showCmd = SW_SHOWNORMAL;

    TCHAR tmp[200];
    WTL::SecureHelper::sprintf_x(tmp, 200,_T("%u,%u,%d,%d,%d,%d"),
        wp.flags,
        wp.showCmd,
        wp.rcNormalPosition.left,
        wp.rcNormalPosition.top,
        wp.rcNormalPosition.right,
        wp.rcNormalPosition.bottom);

    // write position to INI file
    pCfg->SetValue(L"app",L"MainWindowPos", tmp);
}

BOOL QApp::RestoreWindowPos()
{
    HWND hWnd = GetMainSafeWnd();
    QConfig *pCfg = QUIGetConfig();
    if (!::IsWindow(hWnd) || (NULL == pCfg))
    {
        ATLASSERT(FALSE);
        return FALSE;
    }
    // read window position from INI file
    // MainWindow format =0,1,395,198,1012,642
    CStdString sPos = pCfg->GetValue(L"app",L"MainWindowPos");
    if (sPos.IsEmpty())
        return FALSE;

    TCHAR tmp[256];
    _tcsncpy_s(tmp, sPos, _countof(tmp)-2);

    // get WINDOWPLACEMENT info
    WINDOWPLACEMENT wp = { 0 };
    wp.length = sizeof(WINDOWPLACEMENT);
    wp.ptMaxPosition = CPoint(-::GetSystemMetrics(SM_CXBORDER), -::GetSystemMetrics(SM_CYBORDER));
    TCHAR *cp = 0;
    wp.rcNormalPosition = CRect(200, 100, 850, 550);
    if ((cp = _tcstok(tmp, _T(",\r\n"))) != NULL)
        wp.flags = _ttoi(cp);
    if ((cp = _tcstok(NULL,  _T(",\r\n"))) != NULL)
        wp.showCmd = _ttoi(cp);
    if ((cp = _tcstok(NULL,  _T(",\r\n"))) != NULL)
        wp.rcNormalPosition.left = _ttoi(cp);
    if ((cp = _tcstok(NULL,  _T(",\r\n"))) != NULL)
        wp.rcNormalPosition.top = _ttoi(cp);
    if ((cp = _tcstok(NULL,  _T(",\r\n"))) != NULL)
        wp.rcNormalPosition.right = _ttoi(cp);
    if ((cp = _tcstok(NULL,  _T(",\r\n"))) != NULL)
        wp.rcNormalPosition.bottom = _ttoi(cp);
    // sets window's position and iconized/maximized status
    ::SetWindowPlacement(hWnd, &wp);

    // ������һ��wm_move ��Ϣ
    ::SendMessage(hWnd, WM_EXITSIZEMOVE, 0, 0);
    return TRUE;
}

BOOL QApp::UnInit()
{
    // ���򼴽��˳��ˣ� �������״̬
    m_bIsQuiting = TRUE;

    // ��Щ���������ô��ڣ����ڸ���Щ����һ�����Ĺرջ���
    QUIMgr::GetInstance()->Shutdown();

    return TRUE;
}

void QApp::SetAppName( __in LPCWSTR szAppName /*= NULL*/ )
{
    // ����һ���£�����ᱻ�Ż�����
    QUIGetAppName();

    m_sAppName = szAppName;

    if (m_sAppName.Trim().IsEmpty())
    {
        m_sAppName = quibase::GetModuleName(FALSE);
    }
}
