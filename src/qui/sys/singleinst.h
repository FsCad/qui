#ifndef singleinst_h__
#define singleinst_h__

#pragma once

// ʵ����AppHelper.cpp��
class QMemFile;
class CSingleInstance
{
public:
	CSingleInstance():m_hEvent(NULL)
    {

    }

	~CSingleInstance()
    { 
        RemoveRestrict();
    }

    /** �Ƴ���ʵ������
     *      Ӧ�ó���������ж�ʵ��
    **/
    void RemoveRestrict();

	/**
	 * @brief	:	
	 * @parma	:	pszName		�¼�����
				bBringLastTop	����Ѿ��д�ʵ��,���䴰�ڴ���ǰ��
	 * @return	:	TRUE		����ʵ������
	 */
	BOOL InstanceAlreadyRun(LPCTSTR pszName,BOOL bBringLastTop=TRUE);

	BOOL SetInstanceMainWnd(HWND hWnd);

	HWND GetInstanceMainWnd();

private:
	HANDLE	m_hEvent;
};

#endif // apphelper_h__
