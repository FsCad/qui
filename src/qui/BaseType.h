#ifndef _QUI_BASETYPE_
#define _QUI_BASETYPE_

#pragma once

//////////////////////////////////////////////////////////////////////////
// ���ں���
#pragma warning(disable:4995)
// ����ȫ����
#pragma warning(disable:4996)

#include <Windows.h>
#include <vector>

#ifndef WIN32_LEAN_AND_MEAN
#	define WIN32_LEAN_AND_MEAN
#endif

#ifdef _WIN32_WINNT
#	undef _WIN32_WINNT
#	define _WIN32_WINNT 0x0500
#endif

#ifndef _WTL_NO_CSTRING
#   define _WTL_NO_CSTRING
#endif

#ifdef _WTL_USE_CSTRING
#   undef _WTL_USE_CSTRING
#endif

//////////////////////////////////////////////////////////////////////////
// ת�����·��������·��
//      ת��·������ʹ�ô˺���ļ�
// eg:
//  E:\dir\a.h ��ʹ�ô˺�
//      _CONVERT_2_ABSPATH(lib/test.lib)
//  ��չ����Ϊ��
//      E:\dir\a.h/../lib/test.lib  == E:/dir/lib/test.lib
#ifndef _CONVERT_2_ABSPATH
    #define _CONVERT_2_ABSPATH(f) __FILE__##"/../"#f
#endif

// ���ӵ�lib�ļ���lib�ļ�·��Ϊʹ�ô˺���ļ������·����
//  eg:
//      #pragma _LINK_LIBFILE(lib/test.lib)
#ifndef _LINK_LIBFILE
    #define _LINK_LIBFILE(f) comment(lib, _CONVERT_2_ABSPATH(f))
#endif 

//////////////////////////////////////////////////////////////////////////

// #include <Windows.h>
// #include <WTypes.h>

// ����HTMLayout֧��
// #include "./ui/htmlayout/htmlayout.h"
// using namespace htmlayout;
// using namespace htmlayout::dom;

// �ַ���֧��
// #include <shlwapi.h>
// #pragma comment(lib,"shlwapi.lib")

// ��ʹ��WTL�е��ַ����࣬���ַ������Ƕ�����
// �̳���std::stringt,���Ժ�std���ַ����޷�ת��
#ifndef _ATL_TMP_NO_CSTRING
#   define _ATL_TMP_NO_CSTRING
#endif
#include "stdstring.h"

typedef std::vector<CStdString> StringArray;

// ֻ���ø߼��ַ���ת���࣬��ATL::CA2WEX<123>(...)
#ifndef _ATL_EX_CONVERSION_MACROS_ONLY
#   define _ATL_EX_CONVERSION_MACROS_ONLY
#endif

// WTL ֧��
// #include <atlbase.h>
// #include <atlapp.h>
// #include <atlcrack.h>
// #include <atlmisc.h>
// #include <atldef.h>
// 
// using namespace WTL;

// ��������ĺ궨����֧��flash
// #define UIBASE_SUPPORT_FLASH    1

// ��������ĺ궨����֧��web�����
// #define UIBASE_SUPPORT_WEBBROWSER   1

//////////////////////////////////////////////////////////////////////////
// ��ʵ��ģʽ��
#define SINGLETON_ON_DESTRUCTOR(clsname) \
    public: static clsname* GetInstance(){ static clsname _instance;return &_instance; }\
    private:~clsname()

//////////////////////////////////////////////////////////////////////////
#ifndef POINT_FROM_LPARAM
#	define POINT_FROM_LPARAM(p,l) do{p.x=GET_X_LPARAM(l),p.y=GET_Y_LPARAM(l);} while(0)
#endif

#ifndef _HasFlag
#	define _HasFlag(l,f) ((l) & (f))
#endif

#ifndef _AddFlag
#	define _AddFlag(l,f) ((l) |= (f))
#endif

#ifndef _RemoveFlag
#	define _RemoveFlag(l,f) ((l) &= ~(f))
#endif

//////////////////////////////////////////////////////////////////////////
// message relations
// �û��Զ�����Ϣ
#ifndef DECLARE_USER_MESSAGE
#	define DECLARE_USER_MESSAGE(name) \
    const UINT name=::RegisterWindowMessage(L#name);
#endif

//////////////////////////////////////////////////////////////////////////
// �ڴ�й©���

#ifdef _DEBUG
    #define _CRTDBG_MAP_ALLOC 
    #define _CRTDBG_MAP_ALLOC_NEW
    #include<stdlib.h> 
    #include<crtdbg.h>

    #define _QUIMemCheckPoint(s) \
        _CrtMemState __ui_##s; \
        _CrtMemCheckpoint(&(__ui_##s));

    #define _QUIIsMemLeak(s1,s2) \
         { \
             _CrtMemState __ui_s3;\
            if ( _CrtMemDifference(&__ui_s3, &(__ui_##s1),&(__ui_##s2)) )\
            {\
                TRACE(L"------------�ڴ�й©---------------\n");\
                _CrtMemDumpStatistics(&__ui_s3);\
                TRACE(L"-----------------------------------\n");\
                _ASSERT_EXPR(FALSE,L"�����ڴ�й©������!");\
             }\
         }
#else
    #define _QUIMemCheckPoint(s) 
    #define _QUIIsMemLeak(s1,s2)
#endif

#endif
