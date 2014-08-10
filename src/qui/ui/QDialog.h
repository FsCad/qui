#ifndef QDialog_h__
#define QDialog_h__

#pragma once

#include "QFrame.h"
#include "QMessageLoop.h"

/*
 *	�Ի��������
 */
class QDialog : public QFrame
{
	BEGIN_MSG_MAP_EX(QDialog)
		MSG_WM_DESTROY(OnDestroy)
		CHAIN_MSG_MAP(QFrame)
	END_MSG_MAP()
public:
	QDialog(LPCWSTR szResName=NULL) : QFrame(szResName) {}

    virtual ~QDialog() throw()
    {

    }

	/** ����һ��ģ̬�Ի���
	 *	-return:		��׼��ģ̬�Ի��򷵻�ֵ
	 *	-params:	
	 *		-[in]	nDecoratedStyle		�Զ���Ĵ����������
	 *									�鿴����־����ʱ��˵��
     *              hWndParent          ���ΪNULL�����Զ���⵱ǰ�������Ϊ���״���
     *              rcWnd               ���ƴ���λ�ã����ΪNULL��ʹ��Ĭ�ϴ�С��λ��
	 **/
	INT_PTR DoModal(HWND hWndParent = NULL, UINT nStyleEx=0,
        UINT nDecoratedStyle = WS_QEX_WNDSHADOW|WS_QEX_ROUNDCONNER,
        LPCRECT rcWnd = NULL);
		
	void EndDialog(INT_PTR nResult);

protected:
	void OnDestroy();

private:
    QMessageLoop    msg_loop_;
};

#endif // QDialog_h__
