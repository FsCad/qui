#ifndef QUIGlobal_h__
#define QUIGlobal_h__

#pragma once

// // #include "QUI.h"
#include "BaseType.h"
#include "stdstring.h"
#include "htmlayout/htmlayout.h"
#include "htmlayout/htmlayout_dom.h"

// �Զ����QUI��Ϣ
// wParam ����������Ϣ����
DECLARE_USER_MESSAGE(UIBASE_MSG_CODEMSG);

// ϵͳ���½�֪ͨ������Ϣ
DECLARE_USER_MESSAGE(UIBASE_MSG_SYSTRAY);

class QUIConfig;
class QApp;
class QView;

// ��ȡ������ʵ��ָ��
//      !!! ��Ҫ������ָ������ǲ���¶��
QApp* QUIGetApp();

// ��ȡӦ�ó�����
CStdString QUIGetAppName();

#ifndef APP_NAME
#define APP_NAME QUIGetAppName()
#endif

// ��ȡ�����ļ���Ϣ
QUIConfig* QUIGetConfig();

// Ӧ�ó����Ƿ��Ѿ��˳�������Ϣѭ��
//      (���̼����˳�)
BOOL QUIIsQuiting();

////////////////////////////////
// ���Ǳ������ģ���������DLL��������
// ��ȡ�����ھ��
inline HWND QUIGetMainWnd();

// ��ȡӦ�ó�����
inline HINSTANCE QUIGetInstance();
inline HINSTANCE QUIGetResourceInstance();

// ��ȡ�汾��
inline CStdString QUIGetAppVersion();

// Ӧ�ó�����Ϣ��ص���Ϣ
inline LRESULT QUISendCodeTo( HWND hWnd, int nCode, LPARAM lParam)
{
    return SendMessage(hWnd,UIBASE_MSG_CODEMSG,nCode,lParam); 
}

inline BOOL QUIPostCodeTo( HWND hWnd, int nCode, LPARAM lParam)
{
    return PostMessage(hWnd,UIBASE_MSG_CODEMSG,nCode,lParam); 
}

inline BOOL QUIPostCodeToMainWnd( int nCode,LPARAM lParam )
{
    return QUIPostCodeTo(QUIGetMainWnd(),nCode,lParam);
}

inline LRESULT QUISendCodeToMainWnd( int nCode,LPARAM lParam )
{
    return QUISendCodeTo(QUIGetMainWnd(),nCode,lParam);
}

// �Ӵ��ھ��ת������Ӧ��HtmlayoutView
inline QView* QUIViewFromHWND(__in HWND hWnd);
 
inline QView* QUIViewFromHELEMENT(__in HELEMENT he);
 
// ����Զ������ͣ�DOM��HWND�����Ŀؼ�
inline HELEMENT QUIWidgetDOMFromHwnd(__in HWND hWnd);
 
inline HWND QUIHwndFromWidgetDOM(HELEMENT he);

/** ��ѯ����ؼ���ָ��
 *	return:
 *      �ؼ�ָ��ֵ����Ҫ�Լ�������Ҫ��������ת��
 *	params:
 *		-[in]
 *          heCtl       �ؼ�����domֵ
 *          eWidgetType �ؼ����� ENUM_WIDGET_TYPE
**/
LPVOID QUIQuerySpecialCtrl(__in HELEMENT heCtl, __in UINT eWidgetType);

typedef struct QUI_USERMSGPARAM
{
	__inout WPARAM			wParam;
	__inout LPARAM			lParam;
	__inout LPVOID			pData;
	__inout CStdString			sParam;
	__inout LRESULT			lResult;
	// �����߽��յ��˽ṹָ��
	//	ʹ������Ƿ�Ӧ��ɾ����
	//	TRUE	�����߱���ɾ���������ڴ�й¶
	__in	BOOL			bFreeIt;
	BOOL ShouldIDeleteThis()const { return bFreeIt; }
}*LPQUI_USERMSGPARAM;

inline LPQUI_USERMSGPARAM QUIGetUserMsgParam(WPARAM wParam = 0,
	LPARAM lParam = 0,
	LPVOID pData = NULL,
	CStdString sParam = L"",
	BOOL bAcceptorFree=TRUE)
{
	LPQUI_USERMSGPARAM p = new QUI_USERMSGPARAM;
	p->wParam = wParam;
	p->lParam = lParam;
	p->pData = pData;
	p->sParam = sParam;
	p->lResult = 0;
	p->bFreeIt = bAcceptorFree;

	return p;
}

inline void QUIReleaseUserMsgParam(LPQUI_USERMSGPARAM p)
{
	delete p;
}

#ifndef	QUI_ACCEPTOR_HANDLE_USERMSGPARAM
#	define QUI_ACCEPTOR_HANDLE_USERMSGPARAM(p)\
	if ((NULL != (p)) && ((p)->ShouldIDeleteThis()))\
	{ \
		QUIReleaseUserMsgParam(p); \
	}
#endif

#endif // QUIGlobal_h__
