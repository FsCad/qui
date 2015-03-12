#pragma once

#include <map>
#include "QBuffer.h"

class QUIResource
{
    struct _key_string
    {	// functor for operator<
        bool operator()(const CStdString& _Left, const CStdString& _Right) const
        {	// apply operator< to operands
            return _Left.CompareNoCase(_Right) < 0;
        }
    };
    typedef std::map<CStdString,QBuffer*,_key_string>::iterator MapStr2DataItr;

    // �յ���Դ
    static const int NULL_RES_DATA = 0;

public:
    virtual ~QUIResource();

    /**
     *	��zip�ļ��ж�ȡ����
     *		Load֮ǰȷ����ǰû���κ���Դ����
     *	params
     *		-[in]
     *			szZip       Zip��·��
    **/
    template<typename T>
    BOOL LoadFromZip(__in T zip)
    {
        // TODO: ����T�����ж�
        ASSERT ( GetResourceCount() == 0);

        BOOL bOK = FALSE;
        CXUnzip theZip;
        if ( theZip.Open(zip) )
        {
            bOK = LoadZip(&theZip);
            theZip.Close();
        }
        return bOK;
    }

//     BOOL LoadFromZip(__in LPCWSTR szZip);
//     BOOL LoadFromZipBuffer(__in QBuffer& buf);

    // ��ǰ�Ѿ����ڵ���Դ������Ϊ�˴�Load���ͷ�
    // �����Ѿ����ڵ���Դ����ʹ˴μ��ص���Դ��������ԭ������Դ���ͷ�
    BOOL AddedLoadFromZip(__in LPCWSTR szZip);

    /**
     *	��ȡ���ݣ�����NULL�����ȡʧ��
     *		
     *	params
     *		-[in]
     *			szResName       ��Դ���ƣ������ִ�Сд
    **/
    QBuffer* GetResource(__in LPCWSTR szResName);

    /**
     *	�ͷ���Դ�ڴ�
     *		
     *	params
     *		-[in]
     *			szResName       ��Դ���ƣ������ִ�Сд
     *          szResIndicator  ��Դ��ʶ���ţ�
     *                          �����ΪNULL���ͷ���Դ��ʱ��ͬʱ����HL�д�·������
    **/
    void ReleaseResource(__in LPCWSTR szResName, 
        __in LPCWSTR szResIndicator = NULL);

    /**
     *	���һ����Դ�ڴ�
     *		�����Ƕ��ڴ�ָ�룬ʹ��new���룬��Ӻ�Ҫ�Լ��ͷ��ڴ�
     *	params
     *		-[in]
     *			szResName       ��Դ���ƣ������ִ�Сд
     *          pRes            ������Դָ��
     *                          
    **/
    BOOL AddResource(__in LPCWSTR szResName, __in QBuffer* pRes);

    /**
     *	����һ����Դ�ڴ棬ԭ������Դ�ڴ潫���ͷ�
     *		
     *	params
     *		-[in]
     *			szResName       ��Դ���ƣ������ִ�Сд
     *          pRes            ������Դָ��
     *                          �����������Ƕ��ڴ�ָ�룬ʹ��new����
    **/
    BOOL UpdateResource(__in LPCWSTR szResName, __in QBuffer* pNewRes);

    /**
     *	����һ����Դ�ڴ�����֣��µ����ֱ����ǲ����ڵ�
     *		
     *	params
     *		-[in]
     *			szResName       ��Դ���ƣ������ִ�Сд
     *          szNewName       �µ���Դ����
    **/
    BOOL UpdateResourceName(__in LPCWSTR szResName, __in LPCWSTR szNewName);

    /**
     *	-�ͷ����е���Դ�ڴ�
     *      �˴����ڴ���Դ��Ȼ���ͷţ�����HL�����Ѿ����ع�����Դ����cache��
     *      ���cache�ڴ�ֻ�е�page reload�����ڱ����ٵ�ʱ��Żᱻ�ͷŵ���
     *      ������ڴ濪���Ż����濼�ǣ�����Ҫ�ֶ����������cache��
     *      ʹ��HTMLayoutDataReadyAsync���ԴﵽĿ�ġ�
     *		
     *	params
     *		-[in]
     *			szResIndicator      ��ȷ������Դ�������ṩ��ȷ����Դ·����ʶ��
     *                              �����Զ������Դ��ʹ��QUI_CUSTOM_INDICATOR
     *                              NULL������HL��cache
     *			
    **/
    void ReleaseAll(__in LPCWSTR szResIndicator = NULL);

    inline int GetResourceCount()const
    {
        return m_mapData.size();
    }

protected:
    /**
     *	��ȡ��Դ�󣬶���Դ��һ�������������ݼ���
     *		����FALSE����Դ��������
     *	params
     *		-[in]
     *			sResName        ��Դ����
     *		-[inout]
     *          bufData         ��Ҫ�������Դ
     *			
    **/
    virtual BOOL DecodeResource(__in const CStdString &sResName, __inout QBuffer* pData)
    {
        return TRUE;
    }

    // ������Դ��ʱ�����
    virtual BOOL OnLoadFromZip(CXUnzip *zip)
    {
        return TRUE;
    }

    BOOL LoadZip(__in CXUnzip* zip);

    inline MapStr2DataItr _Find(__in LPCWSTR szResName)
    {
        return m_mapData.find(szResName);
    }

    inline MapStr2DataItr _End()
    {
        return m_mapData.end();
    }

    inline BOOL IsExist(__in LPCWSTR szResName)
    {
        return _Find(szResName) != m_mapData.end();
    }

    virtual void OnReleaseResource(__in const CStdString& szResName, 
        __in const CStdString& szResIndicator = L"" );

protected:
    // ϵͳÿ�μ������ݳɹ��󶼻Ỻ�浽�˴�
    std::map<CStdString,QBuffer*,_key_string>	m_mapData;
};

