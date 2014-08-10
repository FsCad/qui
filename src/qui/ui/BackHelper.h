#pragma once

#include "htmlayout/htmlayout_graphin.h"
#include "ECtrl.h"

class CBackHelper
{
public:
    enum ENUM_BACKTRANS_MODE
    {
        BACK_TRANS_RENDERPARENT = 1,
        BACK_TRANS_USERPARENTBACKIMG = 2,
    };

public:
    CBackHelper(QView *p = nullptr, ENUM_BACKTRANS_MODE m = BACK_TRANS_USERPARENTBACKIMG)
    {
        SetView(p, m);
    }

    void SetView(__in QView* p, __in ENUM_BACKTRANS_MODE m)
    {
        if ((nullptr == p) || !(p->IsWindow()) 
            || !(p->GetStyle() & WS_CHILD))
        {
            ASSERT(FALSE);
            return;
        }

        view_ = p;
        mode_ = m;

        if (BACK_TRANS_RENDERPARENT == m)
            RenderParent();
        else if (BACK_TRANS_USERPARENTBACKIMG == m)
            UseParentBkgnd();
    }

    void RenderParent()
    {
        // html����URI
        CStdString uri;
        uri.Format(L"noway:bk_%p.png", view_->GetSafeHwnd());

        WTL::CRect rc_this;
        WTL::CRect rc_parent;
        view_->GetParent().GetClientRect(&rc_parent);

        // �ҵ��ϲ��ʵ�崰�ھ��
        view_->GetWindowRect(&rc_this);
        view_->ScreenToClient(&rc_this);
        rc_parent.left = rc_this.left;
        rc_parent.top = rc_this.top;

        // �Ƚ����������������ͼ�񿽱�����
        htmlayout::image *img_parent = htmlayout::image::create(rc_parent.Width(),rc_parent.Height());
        if (NULL != img_parent)
        {
            if ( HTMLayoutRender(view_->GetParent().m_hWnd, img_parent->handle(), rc_parent) )
            {
                // �����ױ���ͼ������д���ڴ滺������
                struct write_to_thisback : public htmlayout::writer
                {   
                    QBuffer buf;
                    virtual bool write( aux::bytes data )
                    {
                        return buf.Write(data.start,data.length);
                    }
                } wtt;

                // ����0��ָʾд���ʽΪpng��ʽ
                img_parent->save(wtt, 0);
                // ʹ��͸��Ч��
                // ���ڸ�д����style�ͱ�����ͼ���ַ��ԭ�����Զ���ı������ݽ���ʧ
                ECtrl eBody = view_->GetBody();
                eBody.SetBkgndImage(uri);
                eBody.SetBkgndRepeat(L"no-repeat");
                eBody.SetBkgndPosition(0,0,0,0);
                // �����ݸ��µ�ָ����URI��
                HTMLayoutDataReadyAsync(view_->GetSafeHwnd(), uri,
                    wtt.buf.GetBuffer(0),wtt.buf.GetBufferLen(),HLRT_DATA_IMAGE);
            }
            delete img_parent;
        }
    }

    void UseParentBkgnd()
    {
        QView *p = QUIMgr::QWindowFromHWND(view_->GetParent());
        if (nullptr == p)
        {
            ASSERT(FALSE);
            return;
        }
        ECtrl eBody = p->GetBody();
        LPCWSTR szImg = eBody.get_style_attribute("background-image");
        if (nullptr != szImg)
        {
            ECtrl eThis = view_->GetBody();
            eThis.SetBkgndImage(szImg);

            // �ҵ��ϲ��ʵ�崰�ھ��
            WTL::CRect rc_this;
            view_->GetWindowRect(&rc_this);
            view_->GetParent().ScreenToClient(&rc_this);

            // ȡ��������ȥƫ������ֻ��Ը���bkgnd-pos : 0 0
            eThis.SetBkgndPosition(-rc_this.left, -rc_this.top);
            eThis.SetBkgndRepeat(eBody.get_style_attribute("background-repeat"));
        }
    }

private:
    QView*      view_;
    ENUM_BACKTRANS_MODE mode_;
};

