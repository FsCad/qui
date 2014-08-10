#ifndef QUIEVENT_H__
#define QUIEVENT_H__

#pragma once

#include <map>
#include <vector>
#include <algorithm>
#include <WTypes.h>
#include "htmlayout/htmlayout.h"
#include "htmlayout/htmlayout_dom.h"
#include "htmlayout/htmlayout_behavior.h"
#include "htmlayout/value.h"

class QUIEventTarget;

namespace htmlayout
{
    namespace dom
    {
        class EHyperLink;
        class EList;
        class ETree;
        class EOption;
        class ECombobox;
        class EPopup;
        class EMenuItem;
        class ETable;
        class EMenu;
        class ECtrl;
    }
}

using namespace htmlayout::dom;

#ifdef _DEBUG
#include <string>
void TraceBehaviorEventToDC( BEHAVIOR_EVENT_PARAMS &p );
void TraceMouseEventToDC(MOUSE_PARAMS& p);
#endif

#ifndef QUI_EVENT_CALL
#	define QUI_EVENT_CALL
#endif

typedef void (QUI_EVENT_CALL QUIEventTarget::*QUI_PEVENT)(HELEMENT);

namespace quibase
{
    // ���ڼ�¼�¼��Ƿ񱻷���
    // �˽���ֻ֧�ֵ��߳�ģʽ
    // ʹ��TLS���̱߳��ش洢��������չΪ���߳�ģʽ�����ǲ������ô�鷳 -_-!!!
    // �������¼��ص�������ʹ�ô����ݣ��벻Ҫ�������ط�ʹ��
    extern long volatile __uibase_bEventReflect;

    // ���� drag & drop 
    // ��ǰ�����϶���ԭʼDOMֵ������϶�����ʵ�����DOM��clone��
    extern HELEMENT volatile __uibase_heDragOrigin;

    // �϶�����Ԫ��DOMֵ����drop��ʱ�򣬻Ὣ��ֵ����drop��Ӧ����
    extern HELEMENT volatile __uibase_heDragEnter;
};

class QUIEventEntry
{
    // mouse��key��event item
    typedef struct  QUI_ITEM
    {
        enum { _MAX_ID_LEN_ = 64};

        QUI_ITEM(BOOL bUseID,LPCWSTR v,QUI_PEVENT p);
            
        BOOL				bID;
        BOOL				bEnable;
        QUI_PEVENT			pfn;	// function
        wchar_t             sVal[_MAX_ID_LEN_]; // value
    }*LPQUI_ITEM;

    // key  KEY_EVENTS
    // mouse MOUSE_EVENTS
    // event BEHAVIOR_EVENTS
    // ����Ĵ�����
    class CodeHandler   
	{
		typedef std::vector<LPQUI_ITEM> _List;
		typedef _List::iterator _ListItr;

	public:
		CodeHandler(int ec) : m_nHandleCode(ec) {}

		~CodeHandler()
		{
			ClearAll();
		}

		void ClearAll()
		{
			_ListItr iEnd = _End();
			for (_ListItr i = m_list.begin(); i != iEnd; ++i)
			{
				delete *i;
			}
			m_list.clear();
		}

		void AddHandler(LPCWSTR szVal,QUI_PEVENT fn,BOOL bID)
		{
			m_list.push_back(new QUI_ITEM(bID,szVal,fn));
		}

		void Enable(LPCWSTR szVal,BOOL bID,BOOL bEnable=TRUE)
		{
			_ListItr itr = Find(szVal,bID);
			if (_End() != itr)
			{
				(*itr)->bEnable = bEnable;
			}
		}

		void EnableAll(BOOL bEnable = TRUE)
		{
			_ListItr iEnd = _End();
			for (_ListItr i = m_list.begin(); i != iEnd; ++i)
			{
				(*i)->bEnable = bEnable;
			}
		}

		QUI_PEVENT GetHandler(LPCWSTR szVal,BOOL bID)
		{
			_ListItr itr = Find(szVal,bID);
			if (_End() != itr)
			{
				return (*itr)->pfn;
			}
			return NULL;
		}

	protected:
		inline _ListItr _End() { return m_list.end(); }

		_ListItr Find(LPCWSTR szVal,BOOL bID);

	private:
		_List		m_list;
		int	        m_nHandleCode;
	};

    class MapCodeHanlder
    {
        typedef std::map<int, CodeHandler* > _Map;
        typedef _Map::iterator _MapItr;

    public:
        // id ע��
        void IDSignal(int eCode, LPCWSTR szID, QUI_PEVENT pfn)
        {
            CodeHandler *pCH = GetCodeHandler(eCode,TRUE);
            if (NULL != pCH)
            {
                pCH->AddHandler(szID,pfn,TRUE);
            }
        }

        // name ע��
        void NameSignal(int eCode, LPCWSTR szName,QUI_PEVENT pfn)
        {
            CodeHandler *pCH = GetCodeHandler(eCode,TRUE);
            if (NULL != pCH)
            {
                pCH->AddHandler(szName,pfn,FALSE);
            }
        }
	    
        /** ��ȡDOMԪ�ع����¼���eCode������Ӧ����
	     *	-return:	
	     *		�¼���Ӧ����ָ��
	     *	-params:	
	     *		-[in]	
	     *			he		DOM Ԫ��
	     *			eCode	�¼�����
	     **/
	    QUI_PEVENT GetHandlerOfElment(__in HELEMENT he, __in int eCode);


        // ɾ�����е�ע���¼�������
        void Clear();
        
        // �ж���ע����Ŀ
        inline uint GetItemCount()const
        {
            return m_map.size();
        }

    protected:
        /** ��ȡ�����Ӧ�Ĵ�����
	     *	-return:	
	     *		�¼��б�ָ��
	     *	-params:	
	     *		-[in]	
	     *			eCode		�¼�����
	     *			bNewIfNotExist	TRUE, ��������ھ��´���һ������
	     **/
	     CodeHandler* GetCodeHandler(__in int eCode, __in BOOL bNewIfNotExist=FALSE);
         
	     inline _MapItr FindTList(int eCode)
	     {
		    return m_map.find(eCode);
	     }

	     inline _MapItr _End() { return m_map.end(); }

    private:
        _Map     m_map;
    };

public:
	QUIEventEntry()
	{
	}

	~QUIEventEntry()
	{
		ClearAllHandler();
	}

	// �����������
	void ClearAllHandler()
	{
        ClearEventHandler();
        ClearKeyHandler();
        ClearMouseHandler();
	}

    // �Ƿ��Ѿ���ʼ����
    inline BOOL IsInited()const
    {
        return event_handler_.GetItemCount()
            || key_handler_.GetItemCount()
            || mouse_handler_.GetItemCount();
    }

    inline void ClearKeyHandler()
    {
        key_handler_.Clear();
    }

    inline void ClearMouseHandler()
    {
        mouse_handler_.Clear();
    }

    inline void ClearEventHandler()
    {
        event_handler_.Clear();
    }

	void IDSignal(BEHAVIOR_EVENTS eCode, LPCWSTR szID, QUI_PEVENT pfn)
	{
        event_handler_.IDSignal(eCode,szID,pfn);
	}

	void NameSignal(BEHAVIOR_EVENTS eCode, LPCWSTR szName,QUI_PEVENT pfn)
	{
		event_handler_.NameSignal(eCode,szName,pfn);
	}

    void IDSignal(KEY_EVENTS eCode, LPCWSTR szID, QUI_PEVENT pfn)
    {
        key_handler_.IDSignal(eCode,szID,pfn);
    }

    void NameSignal(KEY_EVENTS eCode, LPCWSTR szName,QUI_PEVENT pfn)
    {
        key_handler_.NameSignal(eCode,szName,pfn);
    }

    void IDSignal(MOUSE_EVENTS eCode, LPCWSTR szID, QUI_PEVENT pfn)
    {
        mouse_handler_.IDSignal(eCode,szID,pfn);
    }

    void NameSignal(MOUSE_EVENTS eCode, LPCWSTR szName,QUI_PEVENT pfn)
    {
        mouse_handler_.NameSignal(eCode,szName,pfn);
    }

public:
    // v	void
    // a    ETab
    // b	BOOL
    // c	ECtrl
    // d	json::value
    // e	EEdit
    // k	
    // l	EList
    // m	EMenu
    // mi	EMenuItem
    // n	EButton
    // o	EOption
    // p	EPopup
    // t	ETable
    // x	ECombobox
    // drp	DATA_ROWS_PARAMS
    // bep  BEHAVIOR_EVENT_PARAMS
    union _BehaviorEventHandler
    {
        QUI_PEVENT	pfn;

#define _BEH_CAST2(T,f) ((QUI_PEVENT)(static_cast<QUIEventEntry::_BehaviorEventHandler::T> (f)))

        // 	BUTTON_CLICK = 0,              
        // 		click on button
        // 	BUTTON_PRESS = 1
        // 		mouse down or key down in button
        // 	BUTTON_STATE_CHANGED = 2
        // 		checkbox/radio/slider changed its state/value 
        // 	EDIT_VALUE_CHANGING = 3,
        // 		before text change
        // 	EDIT_VALUE_CHANGED = 4,        
        // 		after text change
        // 	VISIUAL_STATUS_CHANGED = 0x11, 
        // 		broadcast notification, sent to all elements of some container being shown or hidden   
        // 	DISABLED_STATUS_CHANGED = 0x12,
        // 		broadcast notification, sent to all elements of some container that got new value of :disabled state
        // 	ELEMENT_COLLAPSED = 0x90,      
        // 		element was collapsed, so far only behavior:tabs is sending these two to the panels
        // 	ELEMENT_EXPANDED,              
        // 		element was expanded,
        // 	ACTIVATE_CHILD,          
        // 		activate (select) child,
        // 		used for example by accesskeys behaviors to send activation request, e.g. tab on behavior:tabs. 
        //	DO_SWITCH_TAB = ACTIVATE_CHILD,
        //		command to switch tab programmatically, handled by behavior:tabs 
        //		use it as HTMLayoutPostEvent(tabsElementOrItsChild, DO_SWITCH_TAB, tabElementToShow, 0);
        //	UI_STATE_CHANGED,              
        //		ui state changed, observers shall update their visual states.
        //		is sent for example by behavior:richtext when caret position/selection has changed.
        typedef void ( QUI_EVENT_CALL QUIEventTarget::*T_pfn_v_h)(HELEMENT);
        T_pfn_v_h pfn_v_h;
#define B_HANDLER_NAME_v_h(p,e,s,f) (p)->NameSignal((e),(s), _BEH_CAST2(T_pfn_v_h,f));
#define B_HANDLER_ID_v_h(p,e,s,f) (p)->IDSignal((e),(s),_BEH_CAST2(T_pfn_v_h,f));

        //	HYPERLINK_CLICK = 0x80,        
        //		hyperlink click
        typedef void ( QUI_EVENT_CALL QUIEventTarget::*T_pfn_v_k)(EHyperLink);
        T_pfn_v_k pfn_v_k;
#define B_HANDLER_NAME_v_k(p,e,s,f) (p)->NameSignal((e),(s),_BEH_CAST2(T_pfn_v_k,f));
#define B_HANDLER_ID_v_k(p,e,s,f) (p)->IDSignal((e),(s),_BEH_CAST2(T_pfn_v_k,f));

        typedef void (QUI_EVENT_CALL QUIEventTarget::*T_pfn_v_h_h)(HELEMENT,HELEMENT);
        T_pfn_v_h_h pfn_v_h_h;
#define B_HANDLER_NAME_v_h_h(p,e,s,f) (p)->NameSignal((e),(s),_BEH_CAST2(T_pfn_v_h_h,f));
#define B_HANDLER_ID_v_h_h(p,e,s,f) (p)->IDSignal((e),(s),_BEH_CAST2(T_pfn_v_h_h,f));
        // 	SELECT_SELECTION_CHANGED = 5,  
        // 		selection in <select> changed
        // 	SELECT_STATE_CHANGED = 6,      
        // 		node in select expanded/collapsed, heTarget is the node
        typedef void (QUI_EVENT_CALL QUIEventTarget::*T_pfn_v_l_o)(EList,EOption);
        T_pfn_v_l_o pfn_v_l_o;
#define B_HANDLER_NAME_v_l_o(p,e,s,f) (p)->NameSignal((e),(s),_BEH_CAST2(T_pfn_v_l_o,f));
#define B_HANDLER_ID_v_l_o(p,e,s,f) (p)->IDSignal((e),(s),_BEH_CAST2(T_pfn_v_l_o,f));

        // 	SELECT_SELECTION_CHANGED = 5,  
        // 		selection in <select> changed
        // 	SELECT_STATE_CHANGED = 6,      
        // 		node in select expanded/collapsed, heTarget is the node
        typedef void (QUI_EVENT_CALL QUIEventTarget::*T_pfn_v_t_ti)(ETree,EOption);
        T_pfn_v_t_ti pfn_v_t_ti;
#define B_HANDLER_NAME_v_t_o(p,e,s,f) (p)->NameSignal((e),(s),_BEH_CAST2(T_pfn_v_t_ti,f));
#define B_HANDLER_ID_v_t_o(p,e,s,f) (p)->IDSignal((e),(s),_BEH_CAST2(T_pfn_v_t_ti,f));

        typedef void (QUI_EVENT_CALL QUIEventTarget::*T_pfn_v_x_o)(ECombobox,EOption);
        T_pfn_v_x_o pfn_v_x_o;
#define B_HANDLER_NAME_v_x_o(p,e,s,f) (p)->NameSignal((e),(s),_BEH_CAST2(T_pfn_v_x_o,f));
#define B_HANDLER_ID_v_x_o(p,e,s,f) (p)->IDSignal((e),(s),_BEH_CAST2(T_pfn_v_x_o,f));

        // 	MENU_ITEM_ACTIVE = 0xA,        
        // 		menu item activated by mouse hover or by keyboard,
        // 	MENU_ITEM_CLICK = 0xB,         
        // 		menu item click, 
        typedef void (QUI_EVENT_CALL QUIEventTarget::*T_pfn_v_h_mi)(HELEMENT,EMenuItem);
        T_pfn_v_h_mi pfn_v_h_mi;
#define B_HANDLER_NAME_v_h_mi(p,e,s,f) (p)->NameSignal((e),(s),_BEH_CAST2(T_pfn_v_h_mi,f));
#define B_HANDLER_ID_v_h_mi(p,e,s,f) (p)->IDSignal((e),(s),_BEH_CAST2(T_pfn_v_h_mi,f));


        // 	POPUP_REQUEST   = 7,           
        // 		request to show popup just received, 
        // 		here DOM of popup element can be modifed.
        // 	POPUP_READY     = 8,           
        //		popup element has been measured and ready to be shown on screen,
        // 		here you can use functions like ScrollToView.
        // 	POPUP_DISMISSED = 9,
        //		popup element is closed,
        // 		here DOM of popup element can be modifed again - 
        // 		e.g. some items can be removed to free memory.
        //	POPUP_DISMISSING = 0x13,       
        //		popup is about to be closed
        typedef void (QUI_EVENT_CALL QUIEventTarget::*T_pfn_v_h_p)(HELEMENT,EPopup);
        T_pfn_v_h_p pfn_v_h_p;
#define B_HANDLER_NAME_v_h_p(p,e,s,f) (p)->NameSignal((e),(s),_BEH_CAST2(T_pfn_v_h_p,f));
#define B_HANDLER_ID_v_h_p(p,e,s,f) (p)->IDSignal((e),(s),_BEH_CAST2(T_pfn_v_h_p,f));

        //	CONTEXT_MENU_SETUP   = 0xF,    
        //		evt.he is a menu dom element that is about to be shown. 
        //		You can disable/enable items in it.      
        //	CONTEXT_MENU_REQUEST = 0x10,   
        //		"right-click", 
        //		BEHAVIOR_EVENT_PARAMS::he is current popup menu HELEMENT being processed or NULL.
        //		application can provide its own HELEMENT here (if it is NULL) or modify current menu element.
        typedef void (QUI_EVENT_CALL QUIEventTarget::*T_pfn_v_h_m)(HELEMENT,EMenu);
        T_pfn_v_h_m pfn_v_h_m;
#define B_HANDLER_NAME_v_h_m(p,e,s,f) (p)->NameSignal((e),(s), _BEH_CAST2(T_pfn_v_h_m,f));
#define B_HANDLER_ID_v_h_m(p,e,s,f) (p)->IDSignal((e),(s),_BEH_CAST2(T_pfn_v_h_m,f));


        // TABLE_HEADER_CLICK:       
        //		click on some cell in table header, 
        // 		target = the cell, 
        // 		reason = index of the cell (column number, 0..n)
        // TABLE_ROW_CLICK:         
        //		click on data row in the table, target is the row
        // 		target = the row, 
        // 		reason = index of the row (fixed_rows..n)
        // TABLE_ROW_DBL_CLICK:       
        //		mouse dbl click on data row in the table, target is the row
        // 		target = the row, 
        // 		reason = index of the row (fixed_rows..n)
        //		void (QUI_EVENT_CALL QUIEventTarget::*pfn_v_h_h_i)(HELEMENT,HELEMENT,int);
        // SHEET_CELLWIDGET_CLOSED:
        //      sheet ,cell-widget ����ʾ������
        //      ECtrl , cell-widget ���ڵ�td
        typedef void (QUI_EVENT_CALL QUIEventTarget::*T_pfn_v_t_c)(ETable,ECtrl);
        T_pfn_v_t_c pfn_v_t_c;
#define B_HANDLER_NAME_v_t_c(p,e,s,f) (p)->NameSignal((e),(s),_BEH_CAST2(T_pfn_v_t_c,f));
#define B_HANDLER_ID_v_t_c(p,e,s,f) (p)->IDSignal((e),(s),_BEH_CAST2(T_pfn_v_t_c,f));

        //	ROWS_DATA_REQUEST, 
        //		request from virtual grid to data source behavior to fill data in the table
        //		parameters passed throug DATA_ROWS_PARAMS structure.
        typedef void (QUI_EVENT_CALL QUIEventTarget::*T_pfn_v_t_drp)(ETable,DATA_ROWS_PARAMS&);
        T_pfn_v_t_drp pfn_v_t_drp;
#define B_HANDLER_NAME_v_t_drp(p,e,s,f) (p)->NameSignal((e),(s),_BEH_CAST2(T_pfn_v_t_drp,f));
#define B_HANDLER_ID_v_t_drp(p,e,s,f) (p)->IDSignal((e),(s),_BEH_CAST2(T_pfn_v_t_drp,f));
        
        // 	FORM_SUBMIT
        //		behavior:form detected submission event. BEHAVIOR_EVENT_PARAMS::data field contains data to be posted.
        //		BEHAVIOR_EVENT_PARAMS::data is of type T_MAP in this case key/value pairs of data that is about 
        //		to be submitted. You can modify the data or discard submission by returning TRUE from the handler.
        //	FORM_RESET
        //		behavior:form detected reset event (from button type=reset). 
        //		BEHAVIOR_EVENT_PARAMS::data field contains data to be reset.
        //		BEHAVIOR_EVENT_PARAMS::data is of type T_MAP in this case key/value pairs of data that is about 
        //		to be rest. You can modify the data or discard reset by returning TRUE from the handler.
        typedef BOOL (QUI_EVENT_CALL QUIEventTarget::*T_pfn_b_c_d)(ECtrl,json::value&);
        T_pfn_b_c_d pfn_b_c_d;
#define B_HANDLER_NAME_b_c_d(p,e,s,f) (p)->NameSignal((e),(s),_BEH_CAST2(T_pfn_b_c_d,f));
#define B_HANDLER_ID_b_c_d(p,e,s,f) (p)->IDSignal((e),(s),_BEH_CAST2(T_pfn_b_c_d,f));
        
        //////////////////////////////////////////////////////////////////////////
        // δ�����
        //		INIT_DATA_VIEW,                // request to virtual grid to initialize its view
        //		CLOSE_POPUP,                   // close popup request,
        //		REQUEST_TOOLTIP,               // request tooltip, BEHAVIOR_EVENT_PARAMS.he <- is the tooltip element.

        //////////////////////////////////////////////////////////////////////////
        // �Զ����

    };

	/** behavior�¼�����ַ���
	 *	-return:	
	 *		TRUE		�¼�������
	 *		FALSE		�¼�δ���������Ǵ����¼�δע�ᣬ����DOMԪ��δ�Դ��¼�ע��
	 *	-params:	
	 *		-[in]	
	 *			pTarget		
	 *			params		��behavior::handle_event
	 *		-[out]
	 *			pResult		�¼��������ķ���ֵ
	 **/
	BOOL ProcessBehaviorEvent(__in QUIEventTarget *pTarget, 
        __in BEHAVIOR_EVENT_PARAMS& params,__out LRESULT *pResult = NULL) ;

    union _MouseEventHandler
    {
        QUI_PEVENT	pfn;

#define _MEH_CAST2(T,f) ((QUI_PEVENT)(static_cast<QUIEventEntry::_MouseEventHandler::T> (f)))
        // case MOUSE_ENTER:// = 0,
        // case MOUSE_LEAVE:// = 1,
        // case MOUSE_TICK://  = 7, // mouse pressed ticks
        // case MOUSE_IDLE://  = 8, // mouse stay idle for some time
        // case DRAGGING:// = 0x100,
        typedef void ( QUI_EVENT_CALL QUIEventTarget::*T_pfn_v_h)(HELEMENT);
        T_pfn_v_h pfn_v_h;
#define M_HANDLER_NAME_v_h(p,e,s,f) (p)->NameSignal((e),(s),_MEH_CAST2(T_pfn_v_h,f));
#define M_HANDLER_ID_v_h(p,e,s,f) (p)->IDSignal((e),(s),_MEH_CAST2(T_pfn_v_h,f));

        // case MOUSE_MOVE://  = 2,
        // case MOUSE_UP://    = 3,
        // case MOUSE_DOWN://  = 4,
        // case MOUSE_DCLICK:// = 5,
        // case MOUSE_CLICK:// = 0xFF, // mouse click event
        typedef void ( QUI_EVENT_CALL QUIEventTarget::*T_pfn_v_mp)(MOUSE_PARAMS&);
        T_pfn_v_mp pfn_v_mp;
#define M_HANDLER_NAME_v_mp(p,e,s,f) (p)->NameSignal((e),(s),_MEH_CAST2(T_pfn_v_mp,f));
#define M_HANDLER_ID_v_mp(p,e,s,f) (p)->IDSignal((e),(s),_MEH_CAST2(T_pfn_v_mp,f));

        // MOUSE_WHEEL
        typedef void ( QUI_EVENT_CALL QUIEventTarget::*T_pfn_v_h_u_i)(HELEMENT, UINT alt_state, int nDelta);
        T_pfn_v_h_u_i pfn_v_h_u_i;
#define M_HANDLER_NAME_v_h_u_i(p,e,s,f) (p)->NameSignal((e),(s),_MEH_CAST2(T_pfn_v_h_u_i,f));
#define M_HANDLER_ID_v_h_u_i(p,e,s,f) (p)->IDSignal((e),(s),_MEH_CAST2(T_pfn_v_h_u_i,f));

        // case DROP://        = 9,   // item dropped, target is that dropped item 
        // target       �����뵽���Ǹ�Ԫ��
        // src          ���Ϸŵ�ԭʼԪ��
        // draggging    ���Ϸ�Ԫ�صĸ��Ʊ��������Ǳ��Ϸŵ�ԭʼԪ�ر���
        typedef void ( QUI_EVENT_CALL QUIEventTarget::*T_pfn_v_h_h_h)(HELEMENT target, HELEMENT src,HELEMENT dragging);
        T_pfn_v_h_h_h pfn_v_h_h_h;
#define M_HANDLER_NAME_v_h_h_h(p,e,s,f) (p)->NameSignal((e),(s),_MEH_CAST2(T_pfn_v_h_h_h,f));
#define M_HANDLER_ID_v_h_h_h(p,e,s,f) (p)->IDSignal((e),(s),_MEH_CAST2(T_pfn_v_h_h_h,f));

        // case DRAG_ENTER://  = 0xA, // drag arrived to the target element that is one of current drop targets.  
        // case DRAG_LEAVE://  = 0xB, // drag left one of current drop targets. target is the drop target element.  
        typedef void ( QUI_EVENT_CALL QUIEventTarget::*T_pfn_v_h_h)(HELEMENT contbox, HELEMENT dragging);
        T_pfn_v_h_h pfn_v_h_h;
#define M_HANDLER_NAME_v_h_h(p,e,s,f) (p)->NameSignal((e),(s),_MEH_CAST2(T_pfn_v_h_h,f));
#define M_HANDLER_ID_v_h_h(p,e,s,f) (p)->IDSignal((e),(s),_MEH_CAST2(T_pfn_v_h_h,f));

        // case DRAG_REQUEST:// = 0xC,  
        // drag src notification before drag start. To cancel - return true from handler.
        typedef BOOL ( QUI_EVENT_CALL QUIEventTarget::*T_pfn_b_h)(HELEMENT );
        T_pfn_b_h pfn_b_h;
#define M_HANDLER_NAME_b_h(p,e,s,f) (p)->NameSignal((e),(s),_MEH_CAST2(T_pfn_b_h,f));
#define M_HANDLER_ID_b_h(p,e,s,f) (p)->IDSignal((e),(s),_MEH_CAST2(T_pfn_b_h,f));

    };

    /** mouse�¼�����ַ���
	 *	-return:	
	 *		TRUE		�¼�������
	 *		FALSE		�¼�δ���������Ǵ����¼�δע�ᣬ����DOMԪ��δ�Դ��¼�ע��
	 *	-params:	
	 *		-[in]	
	 *			pTarget		
	 *			params		��MOUSE_PARAMS
	 *		-[out]
	 *			bReturn		�¼��������ķ���ֵ��
     *                      ����DRAG_REQUEST������TRUE���Խ�ֹ��ק
	 **/
	BOOL ProcessMouseEvent(__in QUIEventTarget *pTarget, 
        __in MOUSE_PARAMS &prm, __out BOOL &bReturn);

    union _KeyEventHandler
    {
        QUI_PEVENT	pfn;
#define _KEH_CAST2(T,f) ((QUI_PEVENT)(static_cast<QUIEventEntry::_KeyEventHandler::T> (f)))

        // key down
        // key up
        // key char
        typedef void ( QUI_EVENT_CALL QUIEventTarget::*T_pfn_v_h_u_u)(HELEMENT target, UINT code, UINT keyboardStates);
        T_pfn_v_h_u_u pfn_v_h_u_u;
#define K_HANDLER_NAME_v_h_u_u(p,e,s,f) (p)->NameSignal((e),(s),_KEH_CAST2(T_pfn_v_h_u_u,f));
#define K_HANDLER_ID_v_h_u_u(p,e,s,f) (p)->IDSignal((e),(s),_KEH_CAST2(T_pfn_v_h_u_u,f));
       
    };

    BOOL ProcessKeyEvent(__in QUIEventTarget *pTarget, __in KEY_PARAMS &pa)
    {
        _KeyEventHandler _keh;
        _keh.pfn = key_handler_.GetHandlerOfElment(pa.target, pa.cmd);
        if (NULL != _keh.pfn)
        {
            (pTarget->*_keh.pfn_v_h_u_u)(pa.target,pa.key_code,pa.alt_state); 
            return TRUE;
        }
        return FALSE;
    }

private:
    MapCodeHanlder     mouse_handler_;
    MapCodeHanlder     event_handler_;
    MapCodeHanlder     key_handler_;

// 	QString		m_sClass;
// 	QString		m_sBase;
};

class QForm;
class QUIEventTarget
{
    typedef std::vector<QForm*> LstForm;
    typedef LstForm::iterator LstFormItr;

public:
	QUIEventTarget() 
	{
	}

protected:
	virtual QUIEventEntry *__GetEventEntry() 
	{ 
		return &events_entry_; 
	}

    virtual QUIEventEntry *__GetReflectEventEntry() 
    { 
        return &events_reflect_; 
    }

    // ��ʼ���¼���������HL���ڼ��ؽ�����ɵ�ʱ�����
	virtual BOOL __InitEventEntry() 
	{
		return TRUE;
	}

    // ��ʼ�������¼���������HL���ڼ��ؽ�����ɵ�ʱ�����
    virtual BOOL __InitReflectEventEntry() 
    {
        return TRUE;
    }

    inline bool IsFormEventReflected()
    {
        return InterlockedExchange(&quibase::__uibase_bEventReflect, 0) != 0 ;
    }

    // �����¼����������������ٴ����κ��¼���
    // HL�������ٵ�ʱ�����
    void __DeinitEventEntry()
    {
        events_entry_.ClearAllHandler();
    }

    // ���ٷ����¼����������������ٴ����κη����¼���
    // HL�������ٵ�ʱ�����
    void __DeinitReflectEventEntry()
    {
        events_reflect_.ClearAllHandler();
    }

    // ���һ��Form������
    void __AddFormEntry(QForm* pForm);
    
    // �Ƴ�Form������
    void RemoveFormEntry(QForm* pForm)
    {
        LstFormItr i = std::find(forms_.begin(), forms_.end(), pForm);
        if ( i != forms_.end() )
        {
            forms_.erase(i);
        }
    }

protected:
    // �����¼�������
    BOOL handle_event_body(HELEMENT he,BEHAVIOR_EVENT_PARAMS& bep, LRESULT *pResult);

    BOOL handle_mouse_body(__in HELEMENT he, __in MOUSE_PARAMS& mp, __out BOOL &bReturn);

    BOOL handle_key_body(HELEMENT he, KEY_PARAMS& kp);

protected:
    // �����¼�������
    inline BOOL handle_reflect_event(HELEMENT he,BEHAVIOR_EVENT_PARAMS& bep, LRESULT *pResult)
    {
        return events_reflect_.ProcessBehaviorEvent(this, bep, pResult);
    }

    inline BOOL handle_reflect_mouse(__in HELEMENT he, __in MOUSE_PARAMS& mp, __out BOOL &bReturn)
    {
        return events_reflect_.ProcessMouseEvent(this, mp, bReturn);
    }

    inline BOOL handle_reflect_key(HELEMENT he, KEY_PARAMS& kp)
    {
        return events_reflect_.ProcessKeyEvent(this, kp);
    }

private:
    inline BOOL IsFormInChain(QForm* pForm)const
    {
        return std::find(forms_.begin(), forms_.end(), pForm) != forms_.end();
    }

private:
    // �����¼����
    QUIEventEntry       events_entry_;
    // QForm��������¼����·��ص������ڴ���
    QUIEventEntry       events_reflect_;
    // ÿ��QUIEventTarget�������඼�����ж��QForm
    std::vector<QForm*>   forms_;
};

//////////////////////////////////////////////////////////////////////////
// �¼���������
// �¼�����������������ʹ��
#define QUI_DECLARE_EVENT_MAP \
    protected:\
	virtual BOOL __InitEventEntry();\
	virtual BOOL handle_event(HELEMENT,BEHAVIOR_EVENT_PARAMS&);\
    virtual BOOL handle_mouse(HELEMENT,MOUSE_PARAMS&);\
    virtual BOOL handle_key(HELEMENT,KEY_PARAMS&);

// �¼���������ʼ������ʹ��
#define QUI_BEGIN_EVENT_MAP(clsname,basecls)\
	BOOL clsname::handle_event(HELEMENT __he, BEHAVIOR_EVENT_PARAMS& __p )\
	{\
		LRESULT lResult;\
		if ( !QUIEventTarget::handle_event_body(__he, __p, &lResult) ){\
            return basecls::handle_event(__he,__p);\
		}\
		return lResult;\
	}\
	BOOL clsname::handle_mouse(HELEMENT __he, MOUSE_PARAMS& __p )\
    {\
        BOOL bReturn = FALSE; \
        if ( !QUIEventTarget::handle_mouse_body(__he, __p, bReturn) ){\
            return basecls::handle_mouse(__he,__p);\
        }\
        return bReturn;\
    }\
    BOOL clsname::handle_key(HELEMENT __he, KEY_PARAMS& __p )\
    {\
        if ( !QUIEventTarget::handle_key_body(__he, __p) ){\
            return basecls::handle_key(__he,__p);\
        }\
        return FALSE;\
    }\
    BOOL clsname::__InitEventEntry()\
    {\
        typedef basecls _BaseClsName;\
        QUIEventEntry *__p = __GetEventEntry();\
//      ��̫���ܳ�ʼ�����
//        if (__p->IsInited()) { ASSERT(FALSE); return TRUE; }

// ��HL�¼����ݵ�form����
#define CHAIN_HLEVENT_TO_FORM(pForm,selector) \
    (pForm)->Attach(GetCtrl(selector));\
    this->__AddFormEntry(pForm);

// �¼�����������������ʹ��
#define QUI_END_EVENT_MAP() \
		return _BaseClsName::__InitEventEntry();\
	}
//////////////////////////////////////////////////////////////////////////
// �����¼�������������ʹ��
#define QUI_DECLARE_REFLECT_EVENT_MAP \
    private: virtual BOOL __InitReflectEventEntry();

// �����¼���������ʼ������ʹ��
#define QUI_BEGIN_REFLECT_EVENT_MAP(clsname,basecls)\
    BOOL clsname::__InitReflectEventEntry()\
    {\
        typedef basecls _BaseClsName;\
        QUIEventEntry *__p = __GetReflectEventEntry();\
        if (__p->IsInited()) { ATLASSERT(FALSE); return TRUE; }

// �����¼�����������������ʹ��
#define QUI_END_REFLECT_EVENT_MAP() \
        return _BaseClsName::__InitReflectEventEntry();\
    }
////////////////////////////////////////////////////////////////////////// 
// �¼�
//--------------------------------------------------
// button click
#define BN_CLICKED_NAME(s,fn) \
	B_HANDLER_NAME_v_h(__p,BUTTON_CLICK,s,fn);

#define BN_CLICKED_ID(s,fn) \
	B_HANDLER_ID_v_h(__p,BUTTON_CLICK,s,fn);

//--------------------------------------------------
// BUTTON_PRESS
#define BN_PRESS_NAME(s,fn) \
	B_HANDLER_NAME_v_h(__p,BUTTON_PRESS,s,(fn))

#define BN_PRESS_ID(s,fn) \
	B_HANDLER_ID_v_h(__p,BUTTON_PRESS,s,(fn))

//--------------------------------------------------
// checkbox/radio/slider changed its state/value 
// BUTTON_STATE_CHANGED
#define BN_STATECHANGED_NAME(s,fn) \
	B_HANDLER_NAME_v_h(__p,BUTTON_STATE_CHANGED,s,(fn))

#define BN_STATECHANGED_ID(s,fn) \
	B_HANDLER_ID_v_h(__p,BUTTON_STATE_CHANGED,s,(fn))

// slider pos
#define SLD_POSCHANGED_NAME BN_STATECHANGED_NAME
#define SLD_POSCHANGED_ID BN_STATECHANGED_ID
// progress pos
#define PRGS_POSCHANGED_NAME BN_STATECHANGED_NAME
#define PRGS_POSCHANGED_ID BN_STATECHANGED_ID

//--------------------------------------------------
// EDIT_VALUE_CHANGING
#define EDIT_VALUECHANGING_NAME(s,fn) \
	B_HANDLER_NAME_v_h(__p,EDIT_VALUE_CHANGING,s,(fn))

#define EDIT_VALUECHANGING_ID(s,fn) \
	B_HANDLER_ID_v_h(__p,EDIT_VALUE_CHANGING,s,(fn))

//--------------------------------------------------
// EDIT_VALUE_CHANGED
#define EDIT_VALUECHANGED_NAME(s,fn) \
	B_HANDLER_NAME_v_h(__p,EDIT_VALUE_CHANGED,s,(fn))

#define EDIT_VALUECHANGED_ID(s,fn) \
	B_HANDLER_ID_v_h(__p,EDIT_VALUE_CHANGED,s,(fn))

//--------------------------------------------------
// VISIUAL_STATUS_CHANGED
//		broadcast notification, sent to all elements of some container being shown or hidden  
#define VISIUAL_STATUSCHANGED_NAME(s,fn) \
	B_HANDLER_NAME_v_h(__p,VISIUAL_STATUS_CHANGED,s,(fn))

#define VISIUAL_STATUSCHANGED_ID(s,fn) \
	B_HANDLER_ID_v_h(__p,VISIUAL_STATUS_CHANGED,s,(fn))

	//--------------------------------------------------
	// DISABLED_STATUS_CHANGED
	//		broadcast notification, 
	//		sent to all elements of some container that got new value of :disabled state
#define DISABLED_STATUSCHANGED_NAME(s,fn) \
	B_HANDLER_NAME_v_h(__p,DISABLED_STATUS_CHANGED,s,(fn))

#define DISABLED_STATUSCHANGED_ID(s,fn) \
	B_HANDLER_ID_v_h(__p,DISABLED_STATUS_CHANGED,s,(fn))

	//--------------------------------------------------
	// ELEMENT_COLLAPSED
	//		element was collapsed, so far only behavior:tabs is sending these two to the panels
#define ELEMENT_COLLAPSED_NAME(s,fn) \
	B_HANDLER_NAME_v_h(__p,ELEMENT_COLLAPSED,s,(fn))

#define ELEMENT_COLLAPSED_ID(s,fn) \
	B_HANDLER_ID_v_h(__p,ELEMENT_COLLAPSED,s,(fn))
	
	//--------------------------------------------------
	// ELEMENT_EXPANDED
	//		element was EXPANDED, so far only behavior:tabs is sending these two to the panels
#define ELEMENT_EXPANDED_NAME(s,fn) \
	B_HANDLER_NAME_v_h(__p,ELEMENT_EXPANDED,s,(fn))

#define ELEMENT_EXPANDED_ID(s,fn) \
	B_HANDLER_ID_v_h(__p,ELEMENT_EXPANDED,s,(fn))

	//--------------------------------------------------
	// ACTIVATE_CHILD
	//		activate (select) child, 
	//		command to switch tab programmatically, handled by behavior:tabs 
#define ACTIVATE_CHILD_NAME(s,fn) \
	B_HANDLER_NAME_v_h(__p,ACTIVATE_CHILD,s,(fn))

#define ACTIVATE_CHILD_ID(s,fn) \
	B_HANDLER_ID_v_h(__p,ACTIVATE_CHILD,s,(fn))

	//--------------------------------------------------
	// UI_STATE_CHANGED
	//		ui state changed, observers shall update their visual states. 
#define UI_STATECHANGED_NAME(s,fn) \
	B_HANDLER_NAME_v_h(__p,UI_STATE_CHANGED,s,(fn))

#define UI_STATECHANGED_ID(s,fn) \
	B_HANDLER_ID_v_h(__p,UI_STATE_CHANGED,s,(fn))

//--------------------------------------------------
// HYPERLINK_CLICK
#define HYPERLINK_CLICK_NAME(s,fn) \
	B_HANDLER_NAME_v_k(__p,HYPERLINK_CLICK,s,fn)

#define HYPERLINK_CLICK_ID(s,fn) \
	B_HANDLER_ID_v_k(__p,HYPERLINK_CLICK,s,fn)

//////////////////////////////////////////////////////////////////////////
// �������� ���޷���ֵ 
//	void fn(HELEMENT��HELEMENT);
////////////////////////////////////////////////////////////////////////// 
// -------------------------------------------
// SELECT_SELECTION_CHANGED
//		node in select expanded/collapsed, heTarget is the node
#define LIST_SELECTION_CHANGED_NAME(s,fn) \
	B_HANDLER_NAME_v_l_o(__p,SELECT_SELECTION_CHANGED,s,fn);

#define LIST_SELECTION_CHANGED_ID(s,fn) \
	B_HANDLER_ID_v_l_o(__p,SELECT_SELECTION_CHANGED,s,fn)

// SELECT_SELECTION_CHANGED
//		node in select expanded/collapsed, heTarget is the node
#define TREE_SELECTION_CHANGED_NAME(s,fn) \
    B_HANDLER_NAME_v_t_o(__p,SELECT_SELECTION_CHANGED,s,fn);

#define TREE_SELECTION_CHANGED_ID(s,fn) \
    B_HANDLER_ID_v_t_o(__p,SELECT_SELECTION_CHANGED,s,fn)

// combobox
#define CMB_SELECTION_CHANGED_NAME(s,fn) \
	B_HANDLER_NAME_v_x_o(__p,SELECT_SELECTION_CHANGED,s,fn)

#define CMB_SELECTION_CHANGED_ID(s,fn) \
	B_HANDLER_ID_v_x_o(__p,SELECT_SELECTION_CHANGED,s,fn)

// ���������ģ���tree��list��combobox����select
#define UNKNOWN_SELECTION_CHANGED_NAME(s,fn) \
    B_HANDLER_NAME_v_h_h(__p,SELECT_SELECTION_CHANGED,s,fn)

#define UNKNOWN_SELECTION_CHANGED_ID(s,fn) \
    B_HANDLER_ID_v_h_h(__p,SELECT_SELECTION_CHANGED,s,fn)

// -------------------------------------------
// SELECT_STATE_CHANGED �����ڵ�չ��������
#define LIST_SELECT_STATECHANGED_NAME(s,fn) \
	B_HANDLER_NAME_v_l_o(__p,SELECT_STATE_CHANGED,s,fn);

#define LIST_SELECT_STATECHANGED_ID(s,fn) \
	B_HANDLER_ID_v_l_o(__p,SELECT_STATE_CHANGED,s,fn)

#define TREE_SELECT_STATECHANGED_NAME(s,fn) \
    B_HANDLER_NAME_v_t_o(__p,SELECT_STATE_CHANGED,s,fn);

#define TREE_SELECT_STATECHANGED_ID(s,fn) \
    B_HANDLER_ID_v_t_o(__p,SELECT_STATE_CHANGED,s,fn)

// -------------------------------------------
// MENU_ITEM_ACTIVE 
// menu item activated by mouse hover or by keyboard
#define MENUITEM_ACTIVE_NAME(s,fn) \
	B_HANDLER_NAME_v_h_mi(__p,MENU_ITEM_ACTIVE,s,fn)

#define MENUITEM_ACTIVE_ID(s,fn) \
	B_HANDLER_ID_v_h_mi(__p,MENU_ITEM_ACTIVE,s,fn)

// -------------------------------------------
// MENU_ITEM_CLICK 
#define MENUITEM_CLICK_NAME(s,fn) \
	B_HANDLER_NAME_v_h_mi(__p,MENU_ITEM_CLICK,s,fn)

#define MENUITEM_CLICK_ID(s,fn) \
	B_HANDLER_ID_v_h_mi(__p,MENU_ITEM_CLICK,s,fn)

// -------------------------------------------
// POPUP_REQUEST
//		request to show popup just received, 
//		here DOM of popup element can be modifed.
#define POPUP_REQUEST_NAME(s,fn) \
	B_HANDLER_NAME_v_h_p(__p,POPUP_REQUEST,s,(fn))

#define POPUP_REQUEST_ID(s,fn) \
	B_HANDLER_ID_v_h_p(__p,POPUP_REQUEST,s,(fn))

// -------------------------------------------
// POPUP_READY
//		popup element has been measured and ready to be shown on screen,
//		here you can use functions like ScrollToView.
#define POPUP_READY_NAME(s,fn) \
	B_HANDLER_NAME_v_h_p(__p,POPUP_READY,s,(fn))

#define POPUP_READY_ID(s,fn) \
	B_HANDLER_ID_v_h_p(__p,POPUP_READY,s,(fn))

// -------------------------------------------
// POPUP_DISMISSED
#define POPUP_DISMISSED_NAME(s,fn) \
	B_HANDLER_NAME_v_h_p(__p,POPUP_DISMISSED,s,(fn))

#define POPUP_DISMISSED_ID(s,fn) \
	B_HANDLER_ID_v_h_p(__p,POPUP_DISMISSED,s,(fn))

// -------------------------------------------
// POPUP_DISMISSING
#define POPUP_DISMISSING_NAME(s,fn) \
	B_HANDLER_NAME_v_h_p(__p,POPUP_DISMISSING,s,(fn))

#define POPUP_DISMISSING_ID(s,fn) \
	B_HANDLER_ID_v_h_p(__p,POPUP_DISMISSING,s,(fn))

// -------------------------------------------
// CONTEXT_MENU_SETUP
//		evt.he is a menu dom element that is about to be shown.
//		You can disable/enable items in it. 
#define CONTEXT_MENU_SETUP_NAME(s,fn) \
	B_HANDLER_NAME_v_h_m(__p,CONTEXT_MENU_SETUP,s,fn)

#define CONTEXT_MENU_SETUP_ID(s,fn) \
	B_HANDLER_ID_v_h_m(__p,CONTEXT_MENU_SETUP,s,fn)

// -------------------------------------------
// CONTEXT_MENU_REQUEST
//		"right-click", BEHAVIOR_EVENT_PARAMS::he is current popup menu HELEMENT being processed or NULL.
//		application can provide its own HELEMENT here (if it is NULL) or modify current menu element.
#define CONTEXT_MENU_REQUEST_NAME(s,fn) \
	B_HANDLER_NAME_v_h_m(__p,CONTEXT_MENU_REQUEST,s,fn)

#define CONTEXT_MENU_REQUEST_ID(s,fn) \
	B_HANDLER_ID_v_h_m(__p,CONTEXT_MENU_REQUEST,s,fn)

//////////////////////////////////////////////////////////////////////////
// �������� ���޷���ֵ 
//	void fn(HELEMENT��HELEMENT��UINT);
////////////////////////////////////////////////////////////////////////// 
// -------------------------------------------
// TABLE_HEADER_CLICK
//		click on cell in table header,  
//		target = the cell, reason = index of the cell (column number, 0..n)
#define TABLE_HEADER_CLICK_NAME(s,fn) \
	B_HANDLER_NAME_v_t_c(__p,TABLE_HEADER_CLICK,s,fn)

#define TABLE_HEADER_CLICK_ID(s,fn) \
	B_HANDLER_ID_v_t_c(__p,TABLE_HEADER_CLICK,s,fn)

// -------------------------------------------
// TABLE_ROW_CLICK
//		click on data row in the table, 
//		target = the row,reason = index of the row (fixed_rows..n)
#define TABLE_ROW_CLICK_NAME(s,fn) \
	B_HANDLER_NAME_v_t_c(__p,TABLE_ROW_CLICK,s,fn)

#define TABLE_ROW_CLICK_ID(s,fn) \
	B_HANDLER_ID_v_t_c(__p,TABLE_ROW_CLICK,s,fn)

// -------------------------------------------
// TABLE_ROW_DBL_CLICK
//		mouse dbl click on data row in the table, target is the row
//     target = the row, 
//     reason = index of the row (fixed_rows..n)
#define TABLE_ROW_DBLCLICK_NAME(s,fn) \
	B_HANDLER_NAME_v_t_c(__p,TABLE_ROW_DBL_CLICK,s,fn)

#define TABLE_ROW_DBLCLICK_ID(s,fn) \
	B_HANDLER_ID_v_t_c(__p,TABLE_ROW_DBL_CLICK,s,fn)

// -------------------------------------------
// SHEET_CELLWIDGET_CLOSED
#define SHEET_CELLWIDGET_CLOSED_NAME(s,fn) \
    B_HANDLER_NAME_v_t_c(__p,SHEET_CELLWIDGET_CLOSED,s,fn)

#define SHEET_CELLWIDGET_CLOSED_ID(s,fn) \
    B_HANDLER_ID_v_t_c(__p,SHEET_CELLWIDGET_CLOSED,s,fn)

// -------------------------------------------
//	ROWS_DATA_REQUEST,             
//		request from virtual grid to data source behavior to fill data in the table
//		parameters passed throug DATA_ROWS_PARAMS structure.
#define ROWS_DATA_REQUEST_NAME(s,fn) \
	B_HANDLER_NAME_v_t_drp(__p,ROWS_DATA_REQUEST,s,fn);

#define ROWS_DATA_REQUEST_ID(s,fn) \
	B_HANDLER_ID_v_t_drp(__p,ROWS_DATA_REQUEST,s,fn);

// FORM_SUBMIT
//		behavior:form detected submission event. 
//		BEHAVIOR_EVENT_PARAMS::data field contains data to be posted.
//		BEHAVIOR_EVENT_PARAMS::data is of type T_MAP in this case key/value pairs of data that is about 
//		to be submitted. You can modify the data or discard submission by returning TRUE from the handler.
#define FORM_SUBMIT_NAME(s,fn) \
	B_HANDLER_NAME_b_c_d(__p,FORM_SUBMIT | SINKING),s,fn)

#define FORM_SUBMIT_ID(s,fn) \
	B_HANDLER_ID_b_c_d(__p,FORM_SUBMIT | SINKING),s,fn)

//-----------------------------------------------
// FORM_RESET
//		behavior:form detected reset event (from button type=reset). 
//		BEHAVIOR_EVENT_PARAMS::data field contains data to be reset.
//		BEHAVIOR_EVENT_PARAMS::data is of type T_MAP in this case key/value pairs of data that is about 
//		to be rest. You can modify the data or discard reset by returning TRUE from the handler.
#define FORM_RESET_NAME(s,fn) \
	B_HANDLER_NAME_b_c_d(__p,FORM_RESET | SINKING),s,fn)

#define FORM_RESET_ID(s,fn) \
	B_HANDLER_ID_b_c_d(__p,FORM_RESET | SINKING),s,fn)

//-----------------------------------------------
// mouse
// fn: void pfn_v_h(HELEMENT target);
#define _MOUSE_NAME_v_h(s,e,fn)\
    M_HANDLER_NAME_v_h(__p,e,s,fn)
#define _MOUSE_ID_v_h(s,e,fn)\
    M_HANDLER_ID_v_h(__p,e,s,fn)

// MOUSE_ENTER
// fn: void pfn_v_h(HELEMENT target);
#define MOUSE_ENTER_NAME(s,fn)\
    _MOUSE_NAME_v_h(s,MOUSE_ENTER,fn)
#define MOUSE_ENTER_ID(s,fn)\
    _MOUSE_ID_v_h(s,MOUSE_ENTER,fn)

// MOUSE_LEAVE
// fn: void pfn_v_h(HELEMENT target);
#define MOUSE_LEAVE_NAME(s,fn)\
    _MOUSE_NAME_v_h(s,MOUSE_LEAVE,fn)
#define MOUSE_LEAVE_ID(s,fn)\
    _MOUSE_ID_v_h(s,MOUSE_LEAVE,fn)

// MOUSE_IDLE
// fn: void pfn_v_h(HELEMENT target);
#define MOUSE_IDLE_NAME(s,fn)\
    _MOUSE_NAME_v_h(s,MOUSE_IDLE,fn)
#define MOUSE_IDLE_ID(s,fn)\
    _MOUSE_ID_v_h(s,MOUSE_IDLE,fn)

    // MOUSE_TICK
// fn: void pfn_v_h(HELEMENT target);
#define MOUSE_TICK_NAME(s,fn)\
    _MOUSE_NAME_v_h(s,MOUSE_TICK,fn)
#define MOUSE_TICK_ID(s,fn)\
    _MOUSE_ID_v_h(s,MOUSE_TICK,fn)

// DRAGGING - û���������¼�����ֻ��һ����־
// fn: void pfn_v_h(HELEMENT src);
// #define DRAGGING_NAME(s,fn)\
//     _MOUSE_NAME_v_h(s,DRAGGING,fn)
// #define DRAGGING_ID(s,fn)\
//     _MOUSE_ID_v_h(s,DRAGGING,fn)

//----------------------------------------
// fn : void pfn_v_h_mp(MOUSE_PARAMS&);
// case MOUSE_MOVE://  = 2,
// case MOUSE_UP://    = 3,
// case MOUSE_DOWN://  = 4,
// case MOUSE_DCLICK:// = 5,
// case MOUSE_CLICK:// = 0xFF, // mouse click event
#define _MOUSE_NAME_v_mp(s,e,fn)\
    M_HANDLER_NAME_v_mp(__p,e,s,fn)
#define _MOUSE_ID_v_mp(s,e,fn)\
    M_HANDLER_ID_v_mp(__p,e,s,fn)

// !!!!!!!!!! ע�� ����Ҫ !!!!!!!
// <button>test</button> �������� ��Ϊ <button><text>test</text></button>
// ��ť�ϵ��ı�����װ��һ��text��ǩ��ͨ������£���갴�º͵�����Ϣ���ᱻ����
// ���textԪ���Ҫ�벶��mouse_down/up��Ϣ����ʹ��behavior:mouse_updown_capture;
// �磺 <div style="behavior:mouse_updown_capture button; width:50px;height:20px;" >MMMMMMMM</div>
// fn : void pfn_v_h_mp(MOUSE_PARAMS&);
#define MOUSE_DOWN_NAME(s,fn)\
    _MOUSE_NAME_v_mp(s,MOUSE_DOWN,fn)
#define MOUSE_DOWN_ID(s,fn)\
    _MOUSE_ID_v_mp(s,MOUSE_DOWN,fn)

#define MOUSE_CLICK_NAME(s,fn)\
    _MOUSE_NAME_v_mp(s,MOUSE_CLICK,fn)
#define MOUSE_CLICK_ID(s,fn)\
    _MOUSE_ID_v_mp(s,MOUSE_CLICK,fn)

// MOUSE_UP
// fn : void pfn_v_h_mp(MOUSE_PARAMS&);
#define MOUSE_UP_NAME(s,fn)\
    _MOUSE_NAME_v_mp(s,MOUSE_UP,fn)
#define MOUSE_UP_ID(s,fn)\
    _MOUSE_ID_v_mp(s,MOUSE_UP,fn)

// MOUSE_DCLICK
// fn : void pfn_v_h_mp(MOUSE_PARAMS&);
#define MOUSE_DCLICK_NAME(s,fn)\
    _MOUSE_NAME_v_mp(s,MOUSE_DCLICK,fn)
#define MOUSE_DCLICK_ID(s,fn)\
    _MOUSE_ID_v_mp(s,MOUSE_DCLICK,fn)

//----------------------------------------

// MOUSE_WHEEL
// fn: void pfn_v_h_u_i(HELEMENT target, UINT alt_state, int nDelta);
#define MOUSE_WHEEL_NAME(s,fn)\
    M_HANDLER_NAME_v_h_u_i(__p,MOUSE_WHEEL,s,fn)
#define MOUSE_WHEEL_ID(s,fn)\
    M_HANDLER_ID_v_h_u_i(__p,MOUSE_WHEEL,s,fn)

//----------------------------------------

// DROP
// Ԫ���Ϸŵ� [s]box �ϣ�
// fn : void pfn_v_h_h_h(HELEMENT target, HELEMENT src,HELEMENT dragging);
#define DROP_NAME(s,fn)\
    M_HANDLER_NAME_v_h_h_h(__p, DROP, s, fn)
#define DROP_ID(s,fn)\
    M_HANDLER_NAME_v_h_h_h(__p, DROP, s, fn)

//----------------------------------------
// DRAG_ENTER
// fn : void pfn_v_h_h(HELEMENT contbox, HELEMENT dragging);
#define DRAG_ENTER_NAME(s,fn)\
    M_HANDLER_NAME_v_h_h(__p,DRAG_ENTER,s,fn)
#define DRAG_ENTER_ID(s,fn)\
    M_HANDLER_ID_v_h_h(__p,DRAG_ENTER,s,fn)

// DRAG_LEAVE
// fn : void pfn_v_h_h(HELEMENT contbox, HELEMENT dragging);
#define DRAG_LEAVE_NAME(s,fn)\
    M_HANDLER_NAME_v_h_h(__p,DRAG_LEAVE,s,fn)
#define DRAG_LEAVE_ID(s,fn)\
    M_HANDLER_ID_v_h_h(__p,DRAG_LEAVE,s,fn)

//---------------------------------------
// fn: BOOL pfn_b_h(HELEMENT request);
// ���� true��Ϊ��ֹ�϶�
// DRAG_REQUEST
#define DRAG_REQUEST_NAME(s,fn)\
    M_HANDLER_NAME_b_h(__p,DRAG_REQUEST,s,fn)
#define DRAG_REQUEST_ID(s,fn)\
    M_HANDLER_ID_b_h(__p,DRAG_REQUEST,s,fn)


//////////////////////////////////////////////////////////////////////////
//-----------------------------------------------
// KEY
#define _KEY_NAME_v_h_u_u(s,e,fn)\
    K_HANDLER_NAME_v_h_u_u(__p,e,s,fn);
#define _KEY_ID_v_h_u_u(s,e,fn)\
    K_HANDLER_ID_v_h_u_u(__p,e,s,fn);

// KEY_DOWN
#define KEY_DOWN_NAME(s,fn)\
    _KEY_NAME_v_h_u_u(s,KEY_DOWN,fn);
#define KEY_DOWN_ID(s,fn)\
    _KEY_ID_v_h_u_u(s,KEY_DOWN,fn);

// KEY_UP
#define KEY_UP_NAME(s,fn)\
    _KEY_NAME_v_h_u_u(s,KEY_UP,fn);
#define KEY_UP_ID(s,fn)\
    _KEY_ID_v_h_u_u(s,KEY_UP,fn);

// KEY_CHAR
#define KEY_CHAR_NAME(s,fn)\
    _KEY_NAME_v_h_u_u(s,KEY_CHAR,fn);
#define KEY_CHAR_ID(s,fn)\
    _KEY_ID_v_h_u_u(s,KEY_CHAR,fn);


#endif // QUIEVENT_H__

