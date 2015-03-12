#ifndef QSkinUI_h__
#define QSkinUI_h__

#pragma once

#include "QFrame.h"

class QSkinUI;

/**
 *	����HL���ڵĻ���
 *			
**/
struct _SkinItem
{
    // method ��ѡ����ѡ�е�DOM���Ǳ��붨��ģ������assert
    _SkinItem(LPCWSTR s, LPCSTR a, LPCWSTR v, LPCSTR method = "activate")
        :selector_(s), atti_(a), val_(v), xcall_(method)
    {
        selector_.Trim();
        atti_.Trim();
        val_.Trim();
        xcall_.Trim();
    }

    inline BOOL IsValid()const
    {
        return !selector_.IsEmpty() && !atti_.IsEmpty();
    }

    bool operator==(const _SkinItem& r)
    {
        return r.selector_.CompareNoCase(selector_) == 0;
    }

    bool operator==(LPCWSTR selctor)
    {
        return selector_.CompareNoCase(CStdString(selctor).Trim()) == 0;
    }

    // <div #box skin="skin1" />
    // ����������
    //      select(div#box).set_attribute(atti_, val_)
    CStdString     selector_;  // �� div#box_1
    CStdStringA     atti_;  // ���� �磺 name/id/�Զ�����ֶ�
    CStdString     val_;   // ����ֵ
    CStdStringA     xcall_; // ���ô����Ժ���Ҫ���õ�behavior_method,Ϊ�ղ�����
                        // ����<style>��������css·���󣬱�����á�activate���Ż���Ч
};
typedef std::vector<_SkinItem*>  SkinItems;

/// ���ڻ���������
///     1> ���������ǻ�css
///     2> ��һ��bag�У�ͬһ��selector�ǲ�Ӧ�ó��ֶ�εģ�
class _Skinbag
{
    friend class QSkinUI;

protected:
    // ����ʹ�ù�����������֮
    ~_Skinbag()
    {
        auto iEnd = _End();
        for (auto i = itms_.begin(); i != iEnd; ++i)
        {
            delete *i;
        }
    }

    _Skinbag(LPCWSTR thumb)
    {
        ATLASSERT(nullptr != thumb);
        ATLASSERT(wcslen(thumb) > 0); 

        thumb_ = thumb;
        complete_ = FALSE;
        visible_ = TRUE;
    }

public:
    /**
     *	��������
     *		
     *	params
     *		-[in]
     *			szThumb     Ԥ��ͼ���ַ
    **/
    static _Skinbag* Create(__in LPCWSTR szThumb)
    {
        return new _Skinbag(szThumb);
    }

    void Destroy()
    {
        delete this;
    }

    inline CStdString GetThumb()const
    {
        return thumb_;
    }

    /**
     *	��Ӧ�����Ƥ����֮ǰ���ж�Ƥ�������Ƿ���������
     *		�������������Ҫ�ⲿ�ṩ����������ݣ�׼����ɺ���Ҫ֪ͨQSkinUI
     *      ��ʱ�ٽ���Ӧ��Ƥ��
     *	params
     *		-[in]
     *			
     *		-[out]
     *			
     *		-[inout]
     *			
    **/
    inline BOOL IsComplete()const
    {
        return complete_;
    }

    inline void SetComplete(__in BOOL bComplete)
    {
        complete_ = bComplete;
    }

    inline BOOL IsVisible()const
    {
        return visible_;
    }

    inline void SetVisible(__in BOOL bVisible)
    {
        visible_ = bVisible;
    }
	
    /**
     *	��skin���ݰ��м���һ����Ҫִ�е���Ŀ
     *		
     *	params
     *		-[in]
     *			selector    ��Ҫ��Ӧ�õ�ѡ����
     *          atti/val    ѡ���� ������-ֵ��
     *          method      DOM��������ֵ����Ҫ���õ�behavior����method
     *                      ����<style>����src�������� ��activate��ʹ����Ч
     *                      method ��ѡ����ѡ�е�DOM���Ǳ��붨��ģ������assert
    **/
    BOOL AddItem(__in LPCWSTR selector, __in LPCSTR atti, 
        __in LPCWSTR val, __in LPCSTR method = "activate")
    {
        _SkinItem* p = new _SkinItem(selector, atti, val, method);
        if (!p->IsValid())
        {
            delete p;

            return FALSE;
        }

        itms_.push_back(p);

        return TRUE;
    }

    // <style #theme>
    inline BOOL AddStyleThemeItem(__in LPCWSTR val)
    {
        return AddItem(L"style#theme", "src", val);
    }

    BOOL RemoveItem(__in LPCWSTR selector)
    {
        auto i = _Find(selector);
        if (_End() != i)
        {
            delete *i;
            itms_.erase(i);

            return TRUE;
        }
        return FALSE;
    }

    inline SkinItems* GetAll()
    {
        return &itms_;
    }

    inline int GetItemCount()const
    {
        return itms_.size();
    }

private:
    inline SkinItems::iterator _Find(__in LPCWSTR selector)
    {
        auto iEnd = _End();
        for (auto i = itms_.begin(); i != iEnd; ++i)
        {
            if (*(*i) == selector)
            {
                return i;
            }
        }
        return iEnd;
    }

    inline SkinItems::iterator _End()
    {
        return itms_.end();
    }

private:
    SkinItems   itms_;
    CStdString     thumb_; // Ԥ��ͼƬ·��
    BOOL        complete_;  // ���ݰ��Ƿ���������
    BOOL        visible_;   // �Ƿ����û��ɼ���
};

/**
 *  ���û����һ��Ƥ��Ԥ����Ŀ��ʱ��QSkinUI�����ڱ��ز鿴��ǰ��Ŀ������Ƥ������
 *  �������û�й�����Ƥ�����ݣ����ص��˺���������Ƥ�����ݡ�
 *  
 *	-return
 *	    NULL    Ƥ������������ֹ
 *      ���ص�Ƥ��������ָ������Ƕ��ڴ�ʹ��new���ɣ����Ҳ�Ҫ�Լ�ɾ����
 *	params
 *		-[in]
 *			pszSkinThumb        Ԥ����Ŀ���õ�Ԥ��ͼ��·��
 *			
**/
typedef _Skinbag* (__stdcall *PFN_REQUEST_SKINBAG)(__in LPCWSTR pszSkinThumb);

// Ƥ��������֪ͨ
DECLARE_USER_MESSAGE(UIBASE_MSG_SKINBAGDOWN);

/**
 *	HL ��������
 *      ֻҪ���ɴ��ڣ���ֱ�����̽����Ż����ٴ���
 *      Ҫô������ǰ̨��Ҫô������״̬
**/
class QSkinUI : public QFrame
{
    QUI_DECLARE_EVENT_MAP;

    SINGLETON_ON_DESTRUCTOR(QSkinUI)
    {
        ClearAll();
    }

    BEGIN_MSG_MAP_EX(QSkinUI)
        MSG_WM_CLOSE(OnClose)
        MSG_WM_KILLFOCUS(OnKillFocus)
        MESSAGE_HANDLER_EX(UIBASE_MSG_SKINBAGDOWN, OnMSG_SKINBAGREADY)
        CHAIN_MSG_MAP(QFrame)
    END_MSG_MAP()

public:
    QSkinUI() ;

    /**
     *	����ȷ�� skin ָ���ڻ����ڼ䶼����Ч��
     *		ͨ���˷���������������ݻᱻ�Զ����١�
     *      ���Ե����߲�Ҫ�Լ�����_Skinbag::Destroy
     *	params
     *		-[in]

     *			skin    Ƥ����
    **/ 
    BOOL AddItem(__in _Skinbag* skin);

    // ��ʾ֮
    static BOOL ShowAt(CPoint pt);
    static BOOL ShowAt(__in HELEMENT he);

    // ����һϵ��Ƥ������
    void SetSkins(__in std::vector<_Skinbag*> *p );
    
    inline void SetSkinRequest(__in PFN_REQUEST_SKINBAG pRequest)
    {
        request_ = pRequest;
    }

    // ʹ�����Ƥ��
    BOOL UseSkin(__in LPCWSTR szThumb, __in int nAlpha);
protected:
    BOOL Create(HWND hWndParent = NULL, UINT nStyle = WS_POPUP | WS_VISIBLE)
    {
        if (!IsWindow())
        {
            return QFrame::Create(hWndParent, nStyle,
                WS_EX_LAYERED|WS_EX_TOOLWINDOW, 0);
        }
        return TRUE;
    }

    virtual LRESULT OnDocumentComplete();

    void OnItemSkinChanged(HELEMENT he);

    void OnSldOpacityChanged(HELEMENT he);

    ECtrl _ThumbBox()
    {
        return GetCtrl("#box_themes");
    }

    void ClearAll();
    
    BOOL ApplySkinbagEx( __in QView* pView, __in _Skinbag* pBag );

    void OnClose();

    void OnKillFocus(HWND h);

    inline CStdString _ThumbSrc(__in ECtrl& itm)
    {
        return itm.get_attribute("thumb_src");
    }

    inline _Skinbag* _ItemData(__in ECtrl& itm)
    {
        return reinterpret_cast<_Skinbag*>(itm.GetData()); 
    }

    inline ETable _Data2Item(__in _Skinbag* p)
    {
        return _ThumbBox().FindFirstWithData(p);
    }

    LRESULT OnMSG_SKINBAGREADY(UINT uMsg, WPARAM w, LPARAM l);

private:
    ECtrl       curitm_;    // ��ǰʹ�õ�Ƥ����Ŀ
    std::vector<_Skinbag*>  skins_;
    PFN_REQUEST_SKINBAG     request_;
};

#endif // QSkinUI_h__
