#pragma once

/**
 *	QUIBase ��һЩ����������
 *		
**/
class QUIFunction
{
public:
    virtual ~QUIFunction()
    {

    }

    /**
     *  ��ʹ��Ƥ�����ݰ�֮ǰ��Ӧ��ȷ�����ݱ���ȷ������
     *      �������������е��κ�һ������Ӧ��Ƥ��������ֹ
     *      1��PrepareSkinbag
     *      2��LoadSkinbag
    **/

    // ׼������ ����û�б����ؽ���Դ��������
    virtual BOOL PrepareSkinbag(__inout _Skinbag* p)
    {
        return TRUE;
    }

    // �����Ѿ����׼������Ӧ��֪ͨ��ʱ�򣬿��Խ����ݼ��ؽ���Դ��������
    virtual BOOL SkinbagComplete(__inout _Skinbag* p)
    {
        ASSERT(nullptr != p);

        if (nullptr != p)
        {
            p->SetComplete(TRUE);
        }

        return TRUE;
    }

    // �������ݣ���Ҫ�����ؽ���Դ��������
    virtual BOOL LoadSkinbag(__inout _Skinbag* p)
    {
        return TRUE;
    }

    /**
     *	Ӧ�ó�����Ҫ��pView��Ӧ��͸��
     *		
     *  return
     *      true        ����Ӧ��͸��
     *	params
     *		-[in]
     *			pView   ��ҪӦ��͸���Ĵ���ָ��
     *          nAlpha  ͸���� [0, 255]
    **/
    virtual BOOL OnApplyOpacity(__in QView* pView, __in int nAlpha);

    /**
     *  Ӧ�ó�����Ҫ��pView��Ӧ��Ƥ��
     *	-return
     *		TRUE        ����Ӧ��
     *	params
     *		-[in]
     *			pView       ��ҪӦ��Ƥ���Ĵ���
     *          pBag        Ƥ������
    **/
    virtual BOOL OnApplySkinbag(__in QView* pView, __in _Skinbag* pBag)
    {
        return TRUE;
    }

	/**
     *  Ӧ�ó�����pView��Ӧ��Ƥ�������Ѿ����
     *	params
     *		-[in]
     *			pView       Ӧ����Ƥ���Ĵ���
     *          pBag        Ƥ������
    **/
    virtual void ApplySkinbagComplete(__in QView* pView, __in _Skinbag* pBag)
    {
    }
	
    /**
     *  �ֶ�����HL���ڵ��ض�uri��Դ����
     *	-return
     *		FALSE       ��Ҫ����        
     *	params
     *		-[in]
     *			pView       ��ҪӦ�ø��ĵĴ���
     *          uri         ��Դ����
    **/
    virtual BOOL OnUpdateResData(__in QView* pView, __in LPCWSTR uri);
    
    /**
     *	-��ȡ�Զ��������
     *		��QUIMgr���ܽ�����Щָ����Դ���ݵ�ʱ�򣬻�Ӵ˴�ȡ
     *	params
     *		-[in]
     *			szURI       ·��
     *          he          �ĸ�DOM�������Դ
     *		-[out]
     *			pBuf        ����
    **/
    virtual BOOL LoadResData(__in LPCWSTR szURI ,__in HELEMENT he, __out QBuffer* &pBuf)
    {
        pBuf = nullptr;
        return FALSE;
    }
};

