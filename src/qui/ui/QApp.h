#ifndef QApp_h__
#define QApp_h__

#pragma once

// // #include "QUI.h"
#ifndef __ATLBASE_H__
#include <atlbase.h>
#endif

#include <atlapp.h>
#include "QUIGlobal.h"

class QFrame;

// ���뱻��д����ֻӦ����һ��ʵ��

// #ifdef QUI_NOT_DLL
// class QApp : public CAppModule
// #else
// class QApp : public CAtlDllModuleT<QApp>
// #endif
class QApp : public CAppModule
{
public:
	QApp();
	~QApp();

public:
	BOOL Run( HINSTANCE hInstance );

    inline BOOL IsQuiting()const
    {
        return m_bIsQuiting;
    }

	QFrame* GetMainWnd()const
    { 
        return m_pMainWnd; 
    }
	
    HWND GetMainSafeWnd(); 
	/** ͨ���˺������û��߽��ö������ڵ�Ĭ������
	 *	-params:	
	 *		-[in]	
	 *				WS_QEX_ROUNDCONNER		Բ������
	 *				WS_QEX_WNDSHADOW		��Ӱ����
	 **/
	void SetTopFrameStyle(DWORD dwWndStyle = 0) 
    { 
        m_dwWndStyle = dwWndStyle; 
    }

	DWORD GetTopFrameStyle()const 
    {
        return m_dwWndStyle; 
    }
	
    // ��д�˺������ṩ�汾�Ź���
    virtual LPCWSTR GetAppVersion()const 
    { 
        return L"0"; 
    }

    // ����ͻָ�������λ��
    BOOL RestoreWindowPos();

    void SaveWindowPos();

    // �趨Ӧ�ó�������
    void SetAppName(__in LPCWSTR szAppName = NULL);

    // ��ȡӦ�ó�������
    CStdString GetAppName()const
    {
        return m_sAppName;
    }

protected:
	inline void SetMainWnd(QFrame* pFrame) 
    { 
        m_pMainWnd = pFrame; 
    }

	// �����ļ�·��
	virtual CStdString GetConfigPath()const ;

	// ��д�˺���������������
	virtual BOOL InitRun() 
    { 
        return FALSE; 
    }

    // Ӧ����ֹʱ�Ĳ���
    virtual BOOL UnInit();

private:
	QFrame*				m_pMainWnd;
	DWORD				m_dwWndStyle;	// �������ڵ�����֧��
    BOOL                m_bIsQuiting;   // �Ƿ�����Ϣѭ���Ѿ�����
    CStdString             m_sAppName;
};

inline HWND QUIGetMainWnd()
{
    return QUIGetApp()->GetMainSafeWnd();
}

inline HINSTANCE QUIGetResourceInstance()
{
    return QUIGetApp()->GetResourceInstance();
}

inline HINSTANCE QUIGetInstance()
{
    return QUIGetApp()->GetModuleInstance();
}

inline LPCWSTR QUIGetAppVersion()
{
    return QUIGetApp()->GetAppVersion();
}

// ���ô˺�����������
int QUIRun(__in HINSTANCE hInst);

#endif // QApp_h__
