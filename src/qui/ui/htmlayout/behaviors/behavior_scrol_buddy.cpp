#include "behavior_aux.h"


namespace htmlayout 
{
/* ���ڹ���buddy 
 * <widget type="scroll-buddy" buddy="buddy-selector" />
 * ʹ��buddy����ָ����Ҫ��������������
 * ����tab��ǩ̫��һ����ʾ������в���ʹ�ù�����
 * ��������buddy��Ҫ�й��������ߣ�hidden-scroll�����
 * �磺overflow-x:hidden-scroll scroll-manner-x(page-animation:true, step-animation:true, step:auto, page:auto);
**/

struct hscroll_buddy: public behavior
{

    // ctor
    hscroll_buddy(): behavior(HANDLE_BEHAVIOR_EVENT|HANDLE_INITIALIZATION, "hscroll-buddy") {}

    virtual void attached  (HELEMENT he )
    {
        dom::element el(he);

        utf8::ostream o;
        o<<L"<div name=\"left\" /><div name=\"right\" />";
        el.set_html(o.data(),o.length());
    }

    bool find_first_invisible_element_inbuddy(dom::element& buddy,bool bLeft,
        __out dom::element &efind)
    {
        dom::element first_visible;
        if (buddy.children_count() <= 0)
            return false;

//         RECT buddy_rect;
//         buddy.get_location(ROOT_RELATIVE | CONTENT_BOX);
//         RECT child_rect;
        if (bLeft) // ����Ѱ��
        {
            first_visible = buddy.child(0);

            while (first_visible.is_valid() && !first_visible.visible())
            {
                efind = first_visible;
                first_visible = first_visible.next_sibling();
            }
        }
        else
        {
            first_visible = buddy.child(buddy.children_count() - 1);
            while (first_visible.is_valid() && !first_visible.visible())
            {
                efind = first_visible;
                first_visible = first_visible.prev_sibling();
            }
        }
        return efind.is_valid() && !efind.visible();
    }

    virtual BOOL on_event (HELEMENT he, HELEMENT target, BEHAVIOR_EVENTS type, UINT_PTR reason ) 
    { 
        if( type != BUTTON_PRESS )
            return false;

        dom::element self = he;   // widget[type="hscroll-buddy"]
        dom::element prt = self.parent();
        dom::element buddy = prt.find_first(self.get_attribute("buddy"));
        if ( buddy.is_valid() )
        {
            buddy.toggle_state(STATE_FOCUS,true);

            KEY_PARAMS kp;
            kp.alt_state = 0;
            kp.cmd = KEY_DOWN;
            kp.key_code = aux::wcseqi(dom::element(target).get_attribute("name"),L"left") 
                            ? VK_LEFT : VK_RIGHT;
            kp.target = buddy;

            BOOL bx;
            HTMLayoutTraverseUIEvent(HANDLE_KEY,&kp,&bx);
//            HTMLayoutProcessUIEvent(buddy,HANDLE_KEY,&kp,&bx);
        }
        return false;

//        buddy.post_event(MOUSE_ENTER);

        // �ҵ�buddy��ͼ�е�һ�����ɼ��ĺ���Ԫ��
//         dom::element first_invisible;
//         if (find_first_invisible_element_inbuddy(buddy,
//             aux::wcseqi(dom::element(target).get_attribute("name"),L"left"),    // div[name='left'|right]
//             first_invisible))
//         {
//             first_invisible.scroll_to_view(false,true);
//         }

        return false;
    }

   
};

// instantiating and attaching it to the global list
hscroll_buddy hscroll_buddy_instance;

} // htmlayout namespace

