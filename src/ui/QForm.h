#ifndef QForm_h__
#define QForm_h__

#pragma once

#include "QUIEvent.h"

/**
 *  QView��Client�ָ��
**/
class QForm : public QUIEventTarget
{
public:
    QForm(HELEMENT heRoot = NULL);
    
    virtual ~QForm() throw()
    {
        Detach();
    }
    
    /** Attach��View��һ��Ԫ����
     *	return:
     *      TRUE        �ɹ�
     *	params:
     *		-[in]
     *          heRoot  DOM
    **/
    BOOL Attach(__in HELEMENT heRoot);

    virtual void OnAttach()
    {

    }
    
    void Detach()
    {
        OnDetach();

        __DeinitReflectEventEntry();
        __DeinitEventEntry();

        m_heRoot = NULL;
    }

    virtual void OnDetach()
    {

    }

    ECtrl GetRoot()const;

    HWND GetSafeHwnd()const
    {
        HWND hWnd = NULL;
        HTMLayoutGetElementHwnd(m_heRoot, &hWnd, TRUE);
        return hWnd;
    }

    virtual BOOL handle_event(HELEMENT,BEHAVIOR_EVENT_PARAMS&)
    {
        return FALSE;
    }

    virtual BOOL handle_mouse(HELEMENT,MOUSE_PARAMS&)
    {
        return FALSE;
    }

    virtual BOOL handle_key(HELEMENT,KEY_PARAMS&)
    {
        return FALSE;
    }

protected:
    // ���¼��������е��ô˺������Խ����ڴ�����¼����䵽�ϲ�ȥ
    inline void ReflectThisEvent()const
    {
        // ��ֵΪ1024
        InterlockedExchange(/*(LONG*)*/&(quibase::__uibase_bEventReflect), 1024);
    }

	/** ѡ�е�һ��ƥ��selector��DOMԪ��
	 *	-return:		DOM		ƥ���Ԫ��	
	 *	-params:	
	 *		-[in]	selector	��׼��CSS�﷨
	 **/
	ECtrl GetCtrl(const char* selector, ...)const;

private:
    HELEMENT        m_heRoot;
};

#endif // QForm_h__
