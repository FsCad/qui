#ifndef QFrame_h__
#define QFrame_h__

#pragma once

#include "QView.h"
#include "ECtrl.h"
#include "WndShadow.h"

class CWndShadow;

// ����htmlayoutʵ�崰����
//		�ṩ���㴰�ڵķ�װʵ�֡�
class QFrame: public QView
{
	BEGIN_MSG_MAP_EX(QFrame)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_GETMINMAXINFO(OnGetMinMaxInfo)
        MSG_WM_NCHITTEST(OnWmNCHittest)
		MESSAGE_HANDLER_EX(WM_SIZE,HackWndShadowMsg)
		MESSAGE_HANDLER_EX(WM_MOVE,HackWndShadowMsg)
		MESSAGE_HANDLER_EX(WM_EXITSIZEMOVE,HackWndShadowMsg)
		MESSAGE_HANDLER_EX(WM_SHOWWINDOW,HackWndShadowMsg)
		MSG_WM_ACTIVATE(OnActive)
        CHAIN_MSG_MAP(QView)
	END_MSG_MAP()
		
public:
	QFrame(LPCWSTR szResName=NULL);

    virtual ~QFrame();
    
	inline INT_PTR GetRetCode()const { return m_nRetID; }

	/** ������ģ̬���ڣ������Ƕ���������Ӵ���
	 *	-return:		TRUE		�����ɹ�
	 *	-params:	
	 *		-[in]	
	 *				hWndParent			Ĭ���ǵ�ǰ���ڼ���״̬�Ķ���(top)����
	 *				nDecoratedStyle		�Զ���Ĵ����������
	 *									�鿴����־����ʱ��˵��
     *              rcWnd               ���ڴ�С��λ�ã����ΪNULL��ʹ��Ĭ���㷨
	 **/
	BOOL Create(HWND hWndParent = ::GetAncestor(::GetActiveWindow(),GA_ROOT),
		UINT nStyle=WS_POPUP|WS_VISIBLE,UINT nStyleEx=0,
		UINT nDecoratedStyle = WS_QEX_WNDSHADOW|WS_QEX_ROUNDCONNER,
        LPCRECT rcWnd = NULL);

	/** ���ñ����������֣���������#wc-title��
	 *		GetWindowText ���Ի�����õı���
	 *	-params:	
	 *		-[in]	pszTitle		����
	 **/
	void SetTitle(LPCWSTR pszTitle);

	// ���麯���������ö��㴰�ڵ���Ӱ������У�������Ч��
	inline void SetShadowSize(int NewSize = 0);

	inline void SetShadowSharpness(unsigned int NewSharpness = 5);

	inline void SetShadowDarkness(unsigned int NewDarkness = 200);
	
	inline void SetShadowPosition(int NewXOffset = 5, int NewYOffset = 5);
	
	inline void SetShadowColor(COLORREF NewColor = 0);

protected:
	// �¼���������������������ش˺������ڷ���֮ǰ���ٵ��ô˺���
	virtual BOOL handle_event (HELEMENT he, BEHAVIOR_EVENT_PARAMS& params );
	// �л����ڵ���󻯺���ͨ״̬
	void ToggleFrameMaxium();
    // HL���¼�֪ͨ
    virtual LRESULT OnHtmlNotify(UINT uMsg, WPARAM wParam, LPARAM lParam);

	// ϵͳ��Ϣ��WM_XXXXX��������
	void OnDestroy();
    void OnGetMinMaxInfo(LPMINMAXINFO pi);
	void OnActive(UINT nState, BOOL bIsMinimazed, HWND hWnd);
    LRESULT OnWmNCHittest( WTL::CPoint pt );
    LRESULT HackWndShadowMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
    LRESULT OnCreate( LPCREATESTRUCT lpcs );

protected:
	INT_PTR			m_nRetID;		// ģ̬�Ի���ʹ�õķ���ֵ,�����[xrole=]Ԫ�ص�ʱ������

private:
    WTL::CRgn       rgn_frame_;         // Բ������
	CWndShadow		wnd_shadow_;		// ��ӰЧ�����
    CSize           size_min_;            // ������С��С
    ECtrl           ctl_title_;       // ���ڱ���
};

#endif // QFrame_h__
