// // // // #include "QUI.h"
#include "stdafx.h"
#include "ui/QApp.h"

#if 0
#include "AppHelper.h"

#ifdef _DEBUG
LONG WINAPI Q_UnhandledException(__in  struct _EXCEPTION_POINTERS *ExceptionInfo)
{
	QString sExcp = L"δ�����쳣";

	::MessageBox(NULL,sExcp,L"δ�����쳣",MB_OK);
	
	ExitProcess(-1);

	return 0;
}

#else   // release ������

#include "breakpad/breakpad_inc.h"

/**
 *  ʹ��google_break��dump�쳣�����ݣ��Թ�����
**/

class __QExceptionHandler__
{
    ~__QExceptionHandler__()
    {
        if (NULL != m_pEH)
        {
            // ���е�����Ļ�������Ӧ����ûɶ�쳣��
            delete m_pEH;       
        }
    }

public:
    __QExceptionHandler__()
    {
        // �ڱ����������Ŀ¼������"__crash_dumps"�ļ��У�
        // �����쳣dump�����ļ�����д�뵽���ļ�������
        QString sDumpDir = quibase::GetModulePath() + L"__crash_dumps\\";
        sDumpDir.Replace(L'/',L'\\');
        quibase::MakeSureDirExist(sDumpDir);
        _CrtSetReportMode(_CRT_ASSERT, 0);
        m_pEH = new google_breakpad::ExceptionHandler(sDumpDir,NULL,
                    &__QExceptionHandler__::AfterFileDumped, this,       // �ļ�dump��ĵ���
                    google_breakpad::ExceptionHandler::HANDLER_ALL);    // ���������쳣
    }

    static BOOL PreotectProcess()
    {
        static __QExceptionHandler__ _inst;

        return nullptr != _inst.m_pEH;
    }

protected:
    static bool AfterFileDumped(const wchar_t* dump_path, const wchar_t* minidump_id,
        void* context, EXCEPTION_POINTERS* exinfo, MDRawAssertionInfo* assertion,
        bool succeeded) 
    {
        if ( succeeded )
        {   // dump ��� 
            wchar_t szDmpFile[512] = {0};   // dump �ļ���ȫ·��
            swprintf_s(szDmpFile, 512, L"%s%s.dmp", dump_path, minidump_id);
            // ���������½��̴�����ļ����緢�͵���������
        }

        return succeeded;
    }

private:
    google_breakpad::ExceptionHandler*  m_pEH;
};

#endif

#endif

//#define _TEST_QUIBASE_WITHOURT_MAINPROC
// ����_TEST_QUIBASE_WITHOURT_MAINPROC�����Լ�������ڵ㺯��
#ifndef _TEST_QUIBASE_WITHOURT_MAINPROC

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
#ifdef _DEBUG
    _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_DEBUG );	// ������Ϣ��������������������
    //	_CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );	// ���������̨
    _CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_WNDW );	// ���ɻָ��Ĵ�����Ϣ����ʾ�жϴ���
    //	_CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
    _CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_WNDW );	// ����Ϊfalse����ʾ�жϴ���
    //	_CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

//     _CrtSetBreakAlloc(9338);
//     _CrtSetBreakAlloc(1500);
//    _CrtSetBreakAlloc(15561);

    //SetUnhandledExceptionFilter(Q_UnhandledException);
#else   // release�汾
    // ��س���������쳣
#if 0
    if (!__QExceptionHandler__::PreotectProcess())
    {
        // shit!
        // but....life is still go on!
    }
#endif
#endif

    // ��ʼ��COM������Ƿ�ActiveX����������ע�͵�
    // If you are running on NT 4.0 or higher you can use the following call instead to 
    // make the EXE free threaded. This means that calls come in on a random RPC thread.
    //  HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLVERIFY(SUCCEEDED(::OleInitialize(NULL)));

    int nRet = QUIRun(hInstance);

    // ����Ƿ�ActiveX����������ע�͵�
	::CoUninitialize();

	return nRet;
}

#endif
