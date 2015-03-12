#ifndef QUIMgr_h__
#define QUIMgr_h__

#pragma once

// // #include "QUI.h"
#include <list>
#include "BaseType.h"
#include "QBuffer.h"
#include "QUIGlobal.h"
#include "file/inifile.h"
#include "crypt/CXUnzip.h"
#include "crypt/QDataEnDe.h"

#include "QUIResource.h"

// html�ļ��еı����ָ���
#define CHAR_HTMLVAR_SEP L'^'

// ��Դ���е���Դָʾ
//	�� $:Mainframe\index.html. ��Դ����Ŀ¼�µ�Mainframe\index.html�ļ���
#define QUI_UIPAK_ABSINDICATOR L"qabs:"		// ui���еľ���·��
#define QUI_UIPAK_RELATEINDICATOR L"qrel:"	// ui���е����·��

class QSkinUI;
class QUIFunction;
class _Skinbag;

/**
 *	QUIBase ��һЩ����������
 *		
**/

// ��֧�ֶ��߳�
// ��Դ�������ݹ淶��
//	1��html������utf-8��ʽ
//	2��
class QUIMgr
{
	friend class QView;
    friend class QApp;

	SINGLETON_ON_DESTRUCTOR(QUIMgr)
	{
		Shutdown();
	}

    class _UIData : public QUIResource
    {
    public:
        virtual BOOL DecodeResource(__in const CStdString &sResName, __inout QBuffer* pData);

    protected:
        virtual BOOL OnLoadFromZip(CXUnzip *zip);

    private:
        QDataEnDe   ende_;
    }res_;

public:
	/** ��ʹ��htmlayout������ԣ��紴�����ڣ�֮ǰ��������ô˺���
	 *	-return:	
	 *		TRUE	�ɹ�
	 **/
	BOOL Startup(__in const wchar_t* ui_zip = L"UI.zip");

    inline void SetUIFunction(__in QUIFunction* p)
    {
        m_pFunction = p;
    }

    inline QUIFunction* GetUIFunction()const
    {
        return m_pFunction;
    }

	inline BOOL IsStartup()const 
    { 
        return res_.GetResourceCount() > 0; 
    }

	/** ��Ƥ�����м�������
	 *	-return:	
	 *		LOAD_OK		���سɹ�
	 *		LOAD_DISCARD	����ʧ��
	 *	-params:	
	 *		-[inout]	
	 *			pns		�μ�����QView::OnLoadData
	 **/
	LRESULT OnHtmlayoutLoadData( __inout LPNMHL_LOAD_DATA pns );
	/** ��ȡHTML�ļ�����
	 *	-return:	
	 *		TRUE	�ɹ�
	 *	-params:	
	 *		-[in]	
	 *			szFileName		��Դ�ļ�������Դ��·�����߱����ļ�·����
	 *			pView			�����Դ�ļ�����Դ���е����·������ô��Ҫ�˲������½���·��
	 *		-[out]	
	 *			pBuf			��ȡ��������
	 **/
    BOOL LoadData(LPCWSTR szResPath,__in QView* pView,__out QBuffer* &pBuf);
    BOOL LoadData(LPCWSTR szResPath,__in HELEMENT hPrincipal,__out QBuffer* &pBuf);
    /** �Ӵ��ھ��������Ӧ��QView
	 *	-return:	
	 *		�ɹ����ض�Ӧ��QView*������ΪNULL
	 *	-params:	
	 *		-[in]	hWnd	���ھ��
	 **/
	static QView* QWindowFromHWND(__in HWND hWnd);

	/** ��HtmlԪ�ػ�����Ӧ��QView
	 *	-return:	
	 *		�ɹ����ض�Ӧ��QView*������ΪNULL
	 *	-params:	
	 *		-[in]	he	domԪ��
	 **/
	static QView* QWindowFromHELEMENT(__in HELEMENT he);

    static HWND HwndOfWidgetDOM(__in HELEMENT he);
    static HELEMENT DOMOfWidgetHwnd( __in HWND hWnd );

    /**
     *	�ڽ��̷�Χ�ڽ��л���
     *      ��Ե���QView����������Ĵ��ڻ���
     *      �ڵ���֮ǰ��Ӧ��ȷ�����ݰ��Ѿ�����ȫ���أ�LoadSkinbag��
     *	param
     *		-[in]
     *          pBag    Ƥ������
     *                  ���ΪNULL���´�����HL���ڲ�ʹ�û�������
    **/
    static BOOL ApplyStylebag(__in _Skinbag* pBag);
    // ����������ݰ����첽�ģ�����Ҫ�ṩ�ص����ƣ�֪ͨ��ʱ���������
    static BOOL PrepareSkinbag(__in _Skinbag* pBag);
    static BOOL LoadSkinbag(__in _Skinbag* pBag);
    static BOOL SkinbagComplete(__inout _Skinbag* p);

    // �����е�QView && popup��������͸���ȣ� ��Χ[0, 255]
    static void ApplyOpacity(__in int nAlpha);
    
    /**
     *	��ǰ����������HL����uriָ����Դ��������
     *		
     *	params
     *		-[in]
     *			uri     ��Դ·��
     *          pData   ����
     *          nDataLen    ���ݳ���
     *			bIfExist    TRUE    ֻ�����Ѿ��������uri��ʱ��Ÿ���
    **/
    void UpdateResDataWith(__in LPCWSTR uri, __in LPBYTE pData, __in int nDataLen, BOOL bIfExist = TRUE);
    // ��������Ϊ�գ���ʵ��һ���ֽڣ�����Ϊ 0��
    void UpdateResDataWithNULL(LPCWSTR uri)
    {
        char NULL_RES_DATA = 0;
        UpdateResDataWith(uri, (LPBYTE)&NULL_RES_DATA, sizeof(char));
    }

public:
	/** QView���ڴ�����ʱ��Ҫע�ᵽ�˴�
	 *	-params:	
	 *		-[in]	
	 *			pView	���ڳ��δ���ʱע�ᣬֻ��ע��һ��
	 **/
	static void OnWindowCreate(__in QView *pView);
	/** Htmlayout��������ǰ�������� 
	 *	-params:	
	 *		-[in]	
	 *			pView	���ڼ�������ʱ����
	 **/
	static void OnWindowDestroy(__in QView* pView);

    static BOOL OnWidgetWindowCreate( __in HELEMENT hWidget, __in HWND hWnd );
    static void OnWidgetWindowDestroy(__in HELEMENT hWidget, __in HWND hWnd);

    inline std::vector<QView*>* GetViews()
    {
        return &m_lstWnd;
    }

    static _Skinbag* GetCurSkin()
    {
        return m_pCurSkin;
    }

private:
	enum ENUM_UI_RESPATH 
	{
		UI_RESPATH_ERROR = -1,	// �����,�ַ���Ϊ��
		UI_RESPATH_UNKOWN = 0,	// δ֪���ַ�����Ϊ�գ��Ҳ�Ϊ�����κ�һ�����
		UI_RESPATH_PKGRELATE,	// [qrel:] ���ذ������·��
        UI_RESPATH_PKGABS,		// [qabs:] ���ذ�������·��
        //		UI_RESPATH_HTMLAYOUT,	// [res:] | [theme:]��HTMLayout ʹ�õ���Դ���ţ�
        //		UI_RESPATH_LOCAL,		// [x:xx.jpg] | [xx.jpg] ,�����ļ������Ի����·��
        //        UI_RESPATH_MEMORY,	    // [qmem:] �ڴ�����
	};
    static QUIMgr::ENUM_UI_RESPATH DetectDataPathType( __in const CStdString& sPath );

    /*
	 *	hPrincipal	ΪҪ�����Դ��DOMԪ��
	 *				���ΪNULL����ô���·�������ʧ��
	 */
	static ENUM_UI_RESPATH ParseDataPath(__inout CStdString& sPath,__in HELEMENT hPrincipal);
	/*
	 *	pView	ΪҪ�����Դ��DOMԪ�����ڵ�Htmlayout����
	 ��			���ΪNULL����ô���·�������ʧ��
	 */
	static ENUM_UI_RESPATH ParseDataPath(__inout CStdString& sPath,__in QView *pView);
	static BOOL ConvertToAbsPackagePath( const CStdString& sAbsPath,__inout CStdString& sRelatePath );

    // �ر����еĴ���
    void Shutdown();

    // szOriginPath �� ����debug 
    BOOL _LoadData(__in LPCWSTR szResPath, __in LPCWSTR szOriginPath,
        ENUM_UI_RESPATH nType, __out QBuffer* &pBuf);

private:
	// ϵͳά�����Ѵ�����QView/QFrame�����б�
    static std::vector<QView*>    m_lstWnd;
    // <widget #id >������HWND
    static std::map<HELEMENT, HWND>    m_mapHwndDOM;
    // ��ǰ����ʹ�õ�Ƥ����
    static _Skinbag*           m_pCurSkin;
    static QUIFunction*         m_pFunction;
};

// class QHtmlMgr
// {
// 	QHtmlMgr():m_bStarted(FALSE) {}
// 	SINGLETON_ON_DESTRUCTOR(QHtmlMgr){}
// public:
// 	// ���������ļ�·��
// 	BOOL Startup();
// 	BOOL TranslateHtmlLanguageVar( __inout QString& sHtml );
// 	QString GetLaugString( LPCWSTR szSec,LPCWSTR szKey );
// 	inline BOOL IsStartup()const { return m_bStarted; }
// 
// protected:
// 	BOOL ParseVar(__in const QString& str, __out QString& sSection,__out QString &sKey);
// 
// private:
// 	CIniFile		m_cIni;
// 	BOOL			m_bStarted;
// };

inline QView* QUIViewFromHWND(HWND hWnd)
{
    return QUIMgr::QWindowFromHWND(hWnd);
}

inline  QView* QUIViewFromHELEMENT(HELEMENT he)
{
    return QUIMgr::QWindowFromHELEMENT(he);
}

inline  HELEMENT QUIWidgetDOMFromHwnd(__in HWND hWnd)
{
    return QUIMgr::DOMOfWidgetHwnd(hWnd);
}

inline  HWND QUIHwndFromWidgetDOM(HELEMENT he)
{
    return QUIMgr::HwndOfWidgetDOM(he);
}

// �����Դ���ͣ�������չ����
HTMLayoutResourceType QUIGetResourceType(__in const CStdString& uri);

// ��ȡ ':' ֮������ݣ������� ':',���û���ҵ� ':' ����ȫ���ַ���
//      qrel:xxx -> xxx
inline void QUITrimPrefix(__inout CStdString& sPath)
{
    sPath = sPath.Trim();
    int idx = sPath.Find(L':', 0);
    if ( -1 != idx)
    {
        sPath = sPath.Mid(idx+1);
    }
}

/**
 *	�ֽ�URIΪָʾ������Դ·������[:]���ŷָ�������
 *		uri             qrel:xxx 
 *      indicator       qrel: (���� [:])
 *      path            xxx
 *	params
 *		-[in]
 *			szURI       ��Դ·��
 *		-[out]
 *			sIndicator  ָʾ����
 *          sPath       ·��
**/
BOOL QUISplitResPath(__in LPCWSTR szURI, 
    __out CStdString& sIndicator, __out CStdString& sPath);


#endif // QUIMgr_h__
