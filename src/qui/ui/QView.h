#ifndef QVIEW_H__
#define QVIEW_H__

#pragma once

#ifndef __ATLBASE_H__
#include <atlbase.h>
#endif

#ifndef __ATLAPP_H__
#include <atlapp.h>
#endif

#ifndef __ATLCRACK_H__
#include <atlcrack.h>
#endif

#ifndef __ATLMISC_H__
#include <atlmisc.h>
#endif

#ifndef __ATLWIN_H__
#include <atlwin.h>
#endif

#include "QUIEvent.h"
#include "QUIGlobal.h"
#include "htmlayout/wtl_htmlayout.h"
#include "htmlayout/wtl_htmlayouthost.h"

//#include "dock/MultiPaneCtrl.h"

#ifdef UIBASE_SUPPORT_FLASH
    #include "FlashWnd.h"
#endif

#ifdef UIBASE_SUPPORT_SHELLCTRL
    #include "wtl/ShellCtrls.h"
#endif

#pragma comment(lib,"htmlayout.lib")

// ��չ��񣬺�������ws_ex ����ͻ
enum
{
    WS_QEX_ROUNDCONNER = 0x1,	 // Բ�Ǵ���
    WS_QEX_WNDSHADOW = 0x2L,	// ��ӰЧ��
    WS_QEX_BACKTRANSPARENT = 0x4L,	// ������ʵ���Ӵ���(HWND)�ļ���͸��
    // !!! 2013 / 10/ 31 ȡ��֧��
    // WS_QEX_DRAGMOVE = 0x8L,  // �ڴ������κεط�����������ƶ����ڣ�������ֻ����title�ϵ���ſ����ƶ���
    // WS_QEX_FULLSCRNWND = 0x10L,  // ����ȫ��
    WS_QEX_THICKFRAME = 0x20L,  // ����ϵͳ��ws_thickframeҲ�ܵ����ֶ��������ڴ�С
};

// �Զ���Ĵ��ڿؼ����� <widget type="theWidget" />
enum ENUM_WIDGET_TYPE
{
    WIDGET_TYPE_UNKNOWN = 0,
    WIDGET_TYPE_CUSTOM,         // type="windowed"
    WIDGET_TYPE_TOOLBAR,        // type="toolbar"
    WIDGET_TYPE_DOCKVIEW,        // type="dockview"
    WIDGET_TYPE_DOCKCONTAINER,        // type="dock-container"
    WIDGET_TYPE_WEBBROWSER,     // type="WebBrowser"
    WIDGET_TYPE_FLASH,          // type="flash"
    WIDGET_TYPE_SHELLLIST,      // type="shell-list"
    WIDGET_TYPE_SHELLTREE,      // type="shell-tree"
    WIDGET_TYPE_SHELLCOMBOBOX,  // type="shell-combobox"
};

// ʹ��һ����������QUI_MSG_QUI_MSG_CODEMSG
#define MSG_QUI_MSG(func)\
	if (uMsg == UIBASE_MSG_CODEMSG) \
	{ \
		SetMsgHandled(TRUE); \
		func(wParam,lParam); \
		lResult = 0; \
		if(IsMsgHandled()) \
			return TRUE; \
	}
// ʹ�ö����������QUI_MSG_CODEMSG��wParam�ŷ�֧
#define BEGIN_QUI_MSG\
	if (uMsg == UIBASE_MSG_CODEMSG) \
	{ \
		SetMsgHandled(TRUE);\
        lResult = 0; \
		switch(wParam)\
		{
// �޷���ֵ��code����
#define MSG_QUI_CODE(__c,__f)\
		case __c:{ __f(lParam); break;}
// ���з���ֵ��code����
#define MSG_QUI_CODE_WITHRET(__c,__f)\
        case __c:{ lResult = __f(lParam); break;}
// ������Ϣ�����
#define END_QUI_MSG\
		}\
		if(IsMsgHandled()) \
			return TRUE; \
	}

class _SpecialWidgets;

//////////////////////////////////////////////////////////////////////////
// ����htmlayoutʵ�崰����
//		�ṩǶ��ʽ���Ӵ��ڷ�װʵ��
//		�������ڶ��㴰��
// html ����ֵ
//      1��tranparent  ��ʵ�崰��͸��            
//                     ������QView������Ч    <body tranparent=1>
//      2��-border-radius  ���ö�������Բ��Ч��       
//                     ���Զ���QFrame��Ч    <body style="-border-radius=w/h;" > ����ֵ��λ��px
//                     w / h -> �μ�CreateRoundRectRgn�����������
//      3��drag-move   ���κεط���������ƶ�����  
//                     ���Զ���QFrame��Ч    <body drag-move=1 />
//      4��fullscrn-window    ȫ����ʾ����               
//                     ���Զ���QFrame��Ч    <body fullscrn-window=1>
// �߼��Զ��崰��
//      <widget type="theWidget">   theWidgetΪ����ֵ�е�һ��
//      �����         WebBrowser
//      flash������    flash
//      shell��        shell-tree
//      shell�б�      shell-list
//      shell��Ͽ�    shell-combobox
class _Skinbag;
class QView : 
	public QUIEventTarget,
	public CWindowImpl<QView,CHTMLayoutCtrl>,
	public htmlayout::event_handler,
	public CHTMLayoutHost<QView>,
	public htmlayout::notification_handler<QView>//,
//    public MultiPaneCtrlNotify      // �κ�һ��QView�����Ի��dock֧��
{
    // �������Ҫʹ�� BEGIN_MSG_MAP ������Ϣ����ôһ��Ҫ��ʹ��
    // CHAIN_MSG_MAP() �� ��Ϣ�Ƚ������ĸ��ദ��
    // !!!! ���������Ҫ����WM_CLOSE��Ϣ����ô�����һ��Ҫ��bHandled����Ϊfalse
    //		�Ա�wm_close ��Ϣ���ݵ��ϲ㴦�����򲻻����wm_destroy ��Ϣ
    //		������Դ�ò����ͷţ����ᷢ���ڴ�й¶
    BEGIN_MSG_MAP_EX(QView)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_SIZE(OnSize)
        MSG_WM_DESTROY(OnDestroy)
        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

	typedef CHTMLayoutHost<QView> BaseHost;
	typedef CWindowImpl<QView,CHTMLayoutCtrl> BaseWnd;

public:
	QView(LPCWSTR  szResName=NULL);

    virtual ~QView();

    /**
     *  ��ȡUI��Դ��·��
    **/
	inline CStdString GetHtmlPath()const 
    { 
        return m_sHtmlPath; 
    }

	inline HWND GetSafeHwnd()const 
    {
        return m_hWnd; 
    }
    /**
     *  ��ȡ��ǰ�������뽹��ؼ�
    **/
	inline ECtrl GetFocus()const;
    
    /**
     *  ��ȡ��ǩ<html>��Ӧ�Ŀؼ�ֵ
    **/
	inline ECtrl GetRoot() const;

    /**
     *  ��ȡ��ǩ<body>��Ӧ�Ŀؼ�ֵ
    **/
	inline ECtrl GetBody() const;
    
    /** ĳ���µĿؼ�
     *	return:
     *      ptClient���µĿؼ�ֵ
     *	params:
     *		-[in]
     *          ptClient        �ͻ������꣬��window����
    **/
	inline ECtrl CtrlFromPoint(POINT ptClient)const;

    /** ��ѯ����ؼ�
     *	return:
     *      �ؼ�ָ��ֵ����Ҫ�Լ�������Ҫ��������ת��
     *	params:
     *		-[in]
     *          heCtl       �ؼ�����domֵ
     *          eWidgetType �ؼ�����
    **/
    LPVOID QuerySpecialCtrl(__in HELEMENT heCtl, __in ENUM_WIDGET_TYPE eWidgetType);

    /** ���ô˺���ʹ�ñ������ܹ�ʵ�ּ��԰�͸��Ч����ʹ�����ڱ���ƽ�̵��������ϣ�
     *      1�����㴰�ڣ�ws_popup ����ʹ�ô�Ч����
     *      2���˺������д��ǩ<html>����style�ͱ�����ͼ���ַ��ԭ���ı������ݽ���ʧ
                  ���ʹ��ԭ���Զ���ı���ͼ����ô�Ͳ�Ҫ���ô˺���
     *      3��ֻ�����ڡ���С���仯���Ƕ�̬�������Ĵ���
     *      4�������ڱ����仯��ʱ��Ӧ���ٴ��ֶ����ô˺���
     *	return:
     *      ����FALSE�������
     *          1�����ڲ�����ws_child����
     *          2�����ױ�����ȡ���ɹ�
     *	params:
     *		-[in]
     *          bTransparent       TRUE     ͸��
    **/
    BOOL MakeTransparent(__in BOOL bTransparent = TRUE);

    /** !!! �˺��������ϼ�QViewͨ��CreateControl��
     *      ʵ����<widget type="windowed" src="path_of_html" />
     *      ʱ���ã����CreateControl��ʵ��
     *	params:
     *		-[in]
     *          szHtmlPath      ����Ϊsrc������ֵpath_of_html
     *          heWrapper       ����Ϊwidget��DOMֵ
    **/
    void SetPrivateData(__in LPCWSTR szHtmlPath, __in HELEMENT heWrapper) 
    {
        m_sHtmlPath = szHtmlPath; 
        he_wrapper_ = heWrapper;
    }

    /** �޸�QEX style��
     *	params:
     *		-[in]           �����μ� WS_QEX_...
     *          bitsAdd     ��ӵķ��
     *          bitsRemove  �Ƴ��ķ��
    **/
    inline void ModifyQEXStyle(UINT bitsAdd,UINT bitsRemove)
    {
        ATLASSERT( (bitsRemove & bitsAdd) == 0); // �޽�����bitλ
        _AddFlag(styles_, bitsAdd);
        _RemoveFlag(styles_, bitsRemove);
    }

    inline BOOL HasQEXStyle(UINT bits) const 
    {
        return styles_ & bits;
    }

    /**
     *	������HL��DOM��������szSelectorԪ���滻����
     *	param
     *		-[in]
     *          szSelector      ѡ����
     *          szAtti          �����ֶ�
     *          szVal           ����ֵ
     *
    **/
    void SetElementsAttibute(__in LPCWSTR szSelector, 
        __in LPCSTR szAtti, __in LPCWSTR szVal);

    virtual BOOL ApplySkinbag(__in _Skinbag* pBag);

    // �¼�����Ϣ
protected:
	/** 
	 *	�����಻Ҫ����OnCreate��Ϣ�������Ҫ����ʼ������������OnHtmlLoaded
	 **/
	LRESULT OnCreate(LPCREATESTRUCT lpcs);
    /** 
	 * �ڴ��ڴ�С�仯��ʱ����͸���Ӵ���
	 **/
	void OnSize(UINT nState,WTL::CSize sz);
	/** 
	 * ���������������WM_DESTROY��Ϣ�����һ��Ҫ����Ϣ�ٴ��ݵ������
	 **/
	void OnDestroy();

protected:
	/** ����ھ���xrole���Եİ�ť�ϵ���Ӧ����
	 *	-return:	
	 *			TRUE�����������رմ���
	 *			FALSE������ִ��Ĭ����Ϊ������رմ����ˣ�
	 *	-params:	
	 *		-[in]	nBtn	[IDOK|IDCANCEL...]
	 **/
	virtual BOOL OnDefaultButton(INT_PTR nBtn) { return TRUE; };

	/** ��д�˺�������Զ���Ŀؼ��Ĵ���
	 *	-return:		QView ���ָ��
	 *				�Զ���Ŀؼ�����������QView
	 *	-params:	
	 *		-[in]	he	��QView���崴��֮ǰ������DOMԪ�ؾ��Ѿ���������
	 **/
    virtual QView* CustomControlCreate(HELEMENT he) { return NULL; };

	// �Զ��崰�ڿؼ�����
	virtual LRESULT OnDestroyControl(LPNMHL_DESTROY_CONTROL pnmhl);

    inline ENUM_WIDGET_TYPE DetectWidgetType(ECtrl ctlWidget);

	// �Զ��崰�ڿؼ�����
	virtual LRESULT CreateControl(HWND hWnd, LPNMHL_CREATE_CONTROL pnmcc);

    // HL���¼�֪ͨ
    virtual LRESULT OnHtmlNotify(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	/**���ڣ�HWND��������ɺ���ô˺����Լ���UI
	 *		���Ը��Ǵ˺����Լ�����HTML����
	 *	-return:		TRUE ���سɹ���������OnHtmlLoaded
	 **/
	virtual BOOL OnLoadHtml() ;

	/** ���ش���HTML�ļ� 
	 *	-return:		TRUE	���سɹ�
	 *	-params:	
	 *		-[in]	pszFile  �����ļ�·����������ȫ·�������·����
	 *							�ļ����ݱ�����html��ʽ
	 **/
	BOOL LoadFile(LPCWSTR pszFile);

	/** ���ڴ����HTML�ĵ�
	 *	-return:		TRUE	���سɹ�
	 *	-params:		
	 *		-[in]	pData	ָ�������ڴ�
	 *				nLen	���ݳ��ȣ���Byte����
	 *				pBaseUrl	�ĵ�ʹ����Դ�Ĳ���·��
	 **/
	BOOL LoadHtmlData(LPCBYTE pData,int nLen,LPCWSTR pBaseUrl=NULL);

	/** ����UI��CSS
	 *	-return:		TRUE	���óɹ�
	 *	-params:	
	 *		-[in]	pData	����
	 *				nLen	���ݳ��ȣ���Byte����
	 *				pBaseUrl	CSSʹ����Դ�Ĳ���·��
	 *				pMediaType	�ο�SCiter API
	 **/
	BOOL SetCss( LPCBYTE pData,int nDataLen,LPCWSTR pBaseUrl=NULL,LPCWSTR pMediaType=L"screen" );
    
    /** �Զ����ʵ�崰�ڿؼ���DOM-HWND����Դ����
     *      �﷨: <widget src="....html_path_name" />
     *      src����ֵhtml_path_name��Ҫ����QUIMgr������
     *	return
     *      pCtl ��Ӧ�Ĵ���HWND
     *	param
     *		-[in]
     *          pcc         DOM
     *          pCtl        DOM��Ӧ��ʵ�崰��
    **/
    BOOL _DoCreateCustomCtrl( __in LPNMHL_CREATE_CONTROL& pcc, __in QView* pCtl );

protected: 
	/** ѡ�е�һ��ƥ��selector��DOMԪ��
	 *	-return:		DOM		ƥ���Ԫ��	
	 *	-params:	
	 *		-[in]	selector	��׼��CSS�﷨
	 **/
	ECtrl GetCtrl(const char* selector, ...)const;

    inline ECtrl GetWrapperDOM()const;
    
protected:
	CStdString		m_sHtmlPath;	// ��QUIMgr�еĴ���UI����

private:
    // ���籾������<widget type="windowed" />��չ��ʵ��������CreateControl��
    //      he_wrapper_��Ϊwidget��DOMֵ��ʹ��GetRoot().parent()���ǻ�ȡ�������ֵ��
    // �����������WS_POPUP���ڣ���CreateControl������
    //      he_wrapper_��Ч
    HELEMENT            he_wrapper_; 
    UINT		        styles_;  // ��WS_QEX_...
    _SpecialWidgets*    p_widgets_;
};

#endif // QView_h__
