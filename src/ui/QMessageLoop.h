#pragma once

#include <atlapp.h>

// ������Htmlayout����Ϣѭ��
// ʹ�ô���Ϣѭ�����Ա�֤htmlayout::queue ,gui_task ������������
//    ������ֶ��˳���Ϣѭ���Ĵ���
class QMessageLoop : public CMessageLoop
{
protected:
    BOOL m_bContinueRun;

public:
    QMessageLoop() : m_bContinueRun(TRUE) { }

    // message loop
    int Run()
    {
        BOOL bDoIdle = TRUE;
        int nIdleCount = 0;
        BOOL bRet;

        while (m_bContinueRun)
        {
            while(m_bContinueRun && bDoIdle 
                && !::PeekMessage(&m_msg, NULL, 0, 0, PM_NOREMOVE))
            {
                if(!OnIdle(nIdleCount++))
                    bDoIdle = FALSE;
            }

            bRet = ::GetMessage(&m_msg, NULL, 0, 0);

            if(bRet == -1)
                continue;   // error, don't process
            else if(!bRet || !m_bContinueRun)
                break;   // WM_QUIT, exit message loop
            
            // ִ��gui_task
            htmlayout::queue_hl::execute();

            if(!PreTranslateMessage(&m_msg))
            {
                ::TranslateMessage(&m_msg);
                ::DispatchMessage(&m_msg);
            }

            if(IsIdleMessage(&m_msg))
            {
                bDoIdle = TRUE;
                nIdleCount = 0;
            }
        }
        // �Դ��´�����ʹ��
        m_bContinueRun = TRUE;
        // ����ֵ
        return (int)m_msg.wParam;
    }

    BOOL IsRunning()
    {
        return m_bContinueRun;
    }

    void EndRun()
    {
        m_bContinueRun = FALSE;
    }

    // Overrideables
    // Override to change message filtering
    virtual BOOL PreTranslateMessage(MSG* pMsg)
    {
        return CMessageLoop::PreTranslateMessage(pMsg);
    }

    // override to change idle processing
    virtual BOOL OnIdle(int nIdleCount)
    {
        return CMessageLoop::OnIdle(nIdleCount);
    }
};
