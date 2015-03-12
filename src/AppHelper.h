#ifndef apphelper_h__
#define apphelper_h__

#pragma once

#include <sstream>
#include "stdstring.h"

namespace quibase
{
    /**
     *		
     * ÿ�����ַ�ת����һ���ֽڵ�16������,
     * 
     *  ������0xǰ��,�Ϸ��ַ�Ϊ 0 ~ 9�� A~F ,a ~ f
     *  �ַ����ܳ�Ӧ����ż�� "FF0355" - FF0355
     *
    **/
	BOOL StrToHex(LPCTSTR pStr , BYTE val[]);

    /**
     *	�ļ��Ƿ����	
     *
    **/
    BOOL IsFileExist(LPCTSTR pszFile);

    /**
     *	ͳ���ַ���pszStr���ַ�ch���ֵĴ���	
     *
    **/
	int CountChar(__in LPCTSTR pszStr,__in TCHAR ch);
	
    /**
     *	��ȡ��ǰ��������·�� �� �� '/' ��β	
     *
    **/
    CStdString GetModulePath();

    /**
     *	��ȡ��ǰ�������֣���·��	
     *
     *      bWithExt     TRUE        ������չ�� .exe
    **/
	CStdString GetModuleName(BOOL bWithExt = FALSE);

    /**
     *	������Ϣ��ʽ������	
     *
    **/
	CStdString SysErrorMessage(DWORD dwLastError );

    /** �ػ�/����ϵͳ
     *	return:
     *      TRUE    �ɹ�
     *	params:
     *		-[in]
     *          bReboot     TRUE ����
     *                      FALSE �ػ�
     *          pszMsg      ���û���ʾ����Ϣ
     *                      ��󳤶���3000���ַ�
     *          dwTimeout   ���û���ʾ�ĶԻ����Զ��رյĳ�ʱʱ��
     *                      ��λ�루s����
     *                      0   ����ʾ�û��Ի���ֱ�ӹر�
     *                      (0,MAX_SHUTDOWN_TIMEOUT ]   ��ʾ��ʱ��
     *          bAskUserCloseApp  ��ʾ�û���������
     *                          FALSE ����ǿ�ƹر�Ӧ�ó����п��ܵ������ݶ�ʧ
    **/
    BOOL ShutdownComputer( __in BOOL bReboot, __in LPWSTR pszMsg, 
        __in DWORD dwTimeout = 30,__in BOOL bAskUserCloseApp=TRUE );

    /**
     *		
     * ȡ���ػ�
    **/
    BOOL PreventSystemShutdown();

	/** ��Ļ�������ľ���
	 *	-return:		������
	 *	-params:	
	 *		-[in]	p1,p2
	 **/
	inline int DistanceOfPoint(POINT &p1,POINT &p2)
	{
		return (int)sqrt(pow((float)abs(p1.x - p2.x),2) + pow((float)abs(p1.y - p2.y),2));
	}

    /**
     *	�����ı���������	
     *
     *	@param
     *		-[in]
     *      hwnd        Handle to the window to be associated with the open clipboard. 
     *                  If this parameter is NULL, 
     *                  the open clipboard is associated with the current task. 
    **/
	void CopyTexttoClipboard(const CStdString& sText, HWND hwnd = NULL); 

    /**
     *	��ȡ�������ϵ��ı�	
     *
    **/
	CStdString GetClipboardText(HWND hwnd = NULL); 

    /**  ��ȡ����·��
     *	return
     *      TRUE    ��ȡ�ɹ�
     *	param
     *		-[in]
     *          csidl       ���磺CSIDL_STARTUP
     *		-[out]
     *			sPath       ��ȡ����·��
    **/
    BOOL GetSpeialPath(__in int csidl, __out CStdString &sPath);

    /**
     *	����Ȩ��	
     *
    **/
    BOOL UpgradeProcessPrivilege(); 
    
    /**
     *	����Դ�ļ��ͷų���	
     *
    **/ 
    BOOL ReleaseRes(LPCTSTR filename,WORD wResID, LPCTSTR filetype);  

};

class QMutex 
{ 
	CRITICAL_SECTION cs;
public:
	void Lock()     { EnterCriticalSection(&cs); } 
	void Unlock()   { LeaveCriticalSection(&cs); } 
	QMutex()         { InitializeCriticalSection(&cs); }   
	~QMutex()        { DeleteCriticalSection(&cs); }
};

#endif // apphelper_h__
