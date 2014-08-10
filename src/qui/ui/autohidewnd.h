#ifndef autohidewnd_h__
#define autohidewnd_h__

#pragma once

#include <map>

// ------------------------------------------------
// FOR	:	�Զ����ش��ڴ���
//      1> ����Ҫ�Զ����صĴ��ڴ��������RegisterAutoHide
//		2> �ڴ��ڵ�
//              WM_ENTERSIZEMOVE/WM_EXITSIZEMOVE/WM_MOUSEMOVE��Ϣ����ʱ
//          ���� AutoHideWndProc(m_hWnd,message,wParam,lParam);
// ------------------------------------------------
class CAutoHideWnd
{
public:
    enum ENUM_WND_ALIGNMENT
    {
        ALIGN_NONE = 0x0,                     //��ͣ��
        ALIGN_TOP = 0x1,                     //ͣ���ϱ�
        ALIGN_LEFT = 0x2,                     //ͣ�����
        ALIGN_RIGHT = 0x4,                     //ͣ���ұ�
        ALIGN_ALL = 0xFF,  //���еı߶�����ͣ�� 
    };

private:
    typedef struct AHW_DATA 
    {
        AHW_DATA(HWND h)
        {
            hWnd = h;
            bMoving = FALSE;
            bHided = FALSE;
            nNearSize = 10;
            nAlign = ALIGN_ALL;
            eAlignType = ALIGN_NONE;
            nWndExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
        }
        HWND	hWnd;
        ENUM_WND_ALIGNMENT	    eAlignType;       //���ڼ�¼����ͣ��״̬
        int	    nNearSize;		// �����߽��������ʱ����
        int	    nSide;		// �߽��С
        UINT    nAlign;     // ��ͣ���ı߽�
        BOOL	bMoving;		// ���ڵ�����С��
        UINT    nWndExStyle;    // �����ص�ʱ��Ϊ������WS_EX_TOPMOST�����ʾ��ʱ���ٻָ���������
        BOOL    bHided;     // �Ƿ��Ѿ�������
    }*LPAHW_DATA;

    typedef std::map<HWND , AHW_DATA>	MAP_AHW;
    typedef MAP_AHW::iterator	MapAhwItr;
    typedef std::pair<HWND, AHW_DATA> PairAHW;

//     SINGLETON_ON_DESTRUCTOR(CAutoHideWnd)
//     {
//         if (NULL != m_hTimer)
//         {
//             ::DeleteTimerQueueTimer(NULL,m_hTimer,NULL);
//         }
//     }
    
public:
    CAutoHideWnd();
    ~CAutoHideWnd();

public:
	// �������߽��������ʱ������
	void RegisterAutoHide(HWND hWnd, UINT nAlign = ALIGN_ALL, 
        int nPixelAutoHide=10, int nPixelSide=4);

    void Unregister(HWND hWnd);

    BOOL ProcessWindowMessage(
        __in HWND hWnd,
        __in UINT uMsg,
        __in WPARAM wParam,
        __in LPARAM lParam,
        _Inout_ LRESULT& lResult);

    inline void DontDetectCur()
    {
        if (nullptr != m_pCurAHW)

        {
            m_pCurAHW->bMoving = TRUE;

        }
    }

protected:
    inline BOOL IsWndHided(LPAHW_DATA pAHW)const
    {
        ATLASSERT(NULL != pAHW);
        return pAHW->bHided;
    }
	
    static VOID CALLBACK AHWTimerCallback(PVOID lpParameter,BOOLEAN TimerOrWaitFired);

	inline LPAHW_DATA FindAHW(HWND hWnd)
	{
		MapAhwItr itr = m_MapAhw.find(hWnd);
		return  (m_MapAhw.end() != itr) ? (&(itr->second)) : NULL;
	}

    /** �������Ǹ��߽�
     *	return:
     *      FALSE   �������κα߽�
     *	params:
     *		-[in]
     *          pAHW        Ҫ���Ĵ���
    **/
	BOOL DetectWhichSideIsNearset(__in LPAHW_DATA pAHW);

    /** ����/��ʾ ע��Ĵ���
     *	params:
     *		-[in]
     *          pAHW    Ҫ���صĴ���
     *          bHide   TRUE������
    **/
	void  HideWnd(__in LPAHW_DATA pAHW,   __in BOOL   bHide);
private:
    MAP_AHW         m_MapAhw;
    HANDLE	        m_hTimer;
    LPAHW_DATA      m_pCurAHW;
    int	            m_nScreenWidth;
};


#endif // apphelper_h__
