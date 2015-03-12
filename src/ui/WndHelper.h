#ifndef WndHelper_h__
#define WndHelper_h__

#pragma  once

#include "stdstring.h"
#include "BaseType.h"

namespace quibase
{
    /**
     *	��ȡ��Ļ��С	
     *
    **/
	inline SIZE GetScreenSize();

    /**
     *	���ڴ�С	
     *
    **/
	SIZE GetWindowSize(HWND hWnd);

    /**
     *	���ڿ��ߡ� �ͻ�������	
     *
    **/
	int WinWidth(HWND hWnd);
	int WinHeight(HWND hWnd);
	int ClientWidth(HWND hWnd);
	int ClientHeight(HWND hWnd);

    /**
     * �����Ƿ���С��		
     *
    **/
    BOOL IsWindowMaximized( __in HWND hWnd);

    /**
     * ȫ����ʾ����		
     *
    **/
    void MoveToFullScreen(__in HWND hWnd);

    /**
     * ��������ǰ����ǰ����ʾ		
     *
    **/
    void SetForegroundWindowInternal(HWND hWnd);

    /**
     * ��hWndMove��ʾ��hWndIn��������		
     *
    **/
    void CenterWindowInWindow(HWND hWndMove,HWND hWndIn);

    /**
     *	ʹhWndָ���Ĵ���͸��,	
     *
     *	@param
     *		-[in]
     *       uAlpha     ͸����, [0,255], 0:��ȫ͸��,255:��ȫ��͸��
     *       uFlag  	LWA_COLORKEY|LWA_ALPHA
     *
    **/
	BOOL TransparentWindow(HWND hWnd,UINT uAlpha,
		COLORREF clr=0,UINT uFlag=LWA_ALPHA /*|LWA_COLORKEY*/);

    /**
     *	��ָ�����ڵľ���������ָ�������(�غ�),���ı��С	
     *
     *	@return
     *		
     *	@param
     *		-[in]
     *          point ,�����µ�����,ע����screen����,���Ǵ�������
     *
    **/
	BOOL CenterWindowPoint(HWND hWndMove,POINT point);

    /**
     *	��⵱ǰ����Ƿ���hWnd��ʾ�Ĵ�����
     *
    **/
	BOOL IsCursorInWindow(HWND hWnd);
	
    /**
     *	������������Ļ�����½�
     *
    **/
	void PosWindowAtBottomRightScreen(HWND hWnd);

    /**
     *	��ʾһ����ʾFolder Select	
     *
     *	@param
     *		-[out]
     *      sFolder     ���ѡ����Ok,�����˷��ص��ļ���·��
    **/
	BOOL ShowFolderSelectBox(__out CStdString &sFolder);

    /** Ϊָ�����ڻ���һ���߿�
     *	param
     *		-[in]
     *          hWnd        ��Ҫ������ʾ�Ĵ���
    **/
    void HighlightWindow(HWND hWnd, int nBorderWidth = 3);

    /**
     * win7 ����listview�ĸ��״��ھ��
     *  ������ SHELLDLL_DefView
    **/
    HWND GetShellDLLForWin7();

    /**
     *	��鲢��ͼ��������Ϊ�Լ��ĸ����ڣ�����Ϊԭ������
     *
    **/
    HWND CheckParent(HWND hWnd); 

    /**
     *	���ݽ��кʹ��������Ҵ���
     *
    **/
    HWND GetProcessHwnd(LPCTSTR lpszProcess, LPCTSTR lspzCaption); //

};


#endif // WndHelper_h__

