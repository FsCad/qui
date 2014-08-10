#ifndef ECtrl_h__
#define ECtrl_h__

#pragma once

//forcing value to bool 'true' or 'false' (performance warning)
// 4800:    ��type��: ��ֵǿ��Ϊ����ֵ��true����false��(���ܾ���)
// 4927:    ת���Ƿ�����ʽӦ���˶���û������ת����ECtrl -> ETooltip
//          ͨ�����Etooltip(ECtrl& t) ���Խ��������⡣
//          ���Ǵ���̫���ˡ���vs֧����ί�й��캯����ʱ�����޸Ĵ���
#pragma warning(disable:4800 4927)

// // #include "QUI.h"

#include <atldef.h>
#include "time/QTime.h"
#include "BaseType.h"
#include "htmlayout/htmlayout.h"
#include "htmlayout/htmlayout_dom.h"
#include "htmlayout/htmlayout_behavior.h"
// #include <shlobj.h>
// #include <atlshellext.h>
#include "tree.hh"

// ����֧�ֿ���קdockpane��tab��
#define QUI_DOCKPANE_TAB_NAME   L"dockpane_tab__qui_"

class CFlashWnd;
class QView;
class CWebBrowser;

/** ��COLORREFת����html�ܹ�ֱ��ʹ�õ���ɫ
*      Dword -> #xxxxxx
*	return
*      Stirng
*	param
*		-[in]
*          dwColor         ��ɫֵ
**/
inline CStdString COLORREF2HtmlColor(__in COLORREF dwColor)
{
    CStdString sRet;
    sRet.Format(L"#%02X%02X%02X",
        GetRValue(dwColor),
        GetGValue(dwColor),
        GetBValue(dwColor));
    return sRet;
}

/** ��ARGBת����html�ܹ�ֱ��ʹ�õ���ɫ
*      Dword -> #xxxxxx
*	return
*      Stirng
*	param
*		-[in]
*          dwColor         ��ɫֵ
**/
inline CStdString ColorARGB2HtmlColor(__in COLORREF dwColor)
{
    CStdString sRet;
    sRet.Format(L"#%02X%02X%02X",
        (dwColor>>16)&0xff,
        (dwColor>>8)&0xff,
        dwColor&0xff);
    return sRet;
}

namespace htmlayout
{

    struct treelist;
    class CHLShellTree;
    // The PARAM data of all controls
    // ����shell tree
//     typedef struct
//     {
//         CComPtr<IShellFolder> spFolder;
//         CPidl pidlFull;
//         CPidl pidlNode;
//         DWORD dwAttribs;
//     } SHELLITEMINFO, *PSHELLITEMINFO;

    struct _METHOD_PARAM_WITHDOM : public XCALL_PARAMS
    {
        _METHOD_PARAM_WITHDOM(LPCSTR name)
            :XCALL_PARAMS(name),h1(NULL),h2(NULL),h3(NULL)
        {
        }

        HELEMENT    h1;
        HELEMENT    h2;
        HELEMENT    h3;
        DWORD       dw;     // �û��Զ���flag
    };

    enum _ENUM_QUI_CUSTOM_METHOD
    {
        /** behavior��sheet
        *      ��ʾcell-widget
        *	param
        *		-[in]
        *              _METHOD_PARAM_WITHDOM*
        *                  h1      table
        *                  h2      cell
        **/
        QUI_CUSTOM_METHOD_SHEET_SHOWCELLWIDGET = FIRST_APPLICATION_METHOD_ID,
        /** behavior��sheet
        *      �ر�cell-widget
        *	param
        *		-[in]
        *              _METHOD_PARAM_WITHDOM*
        *                  h1      table
        *                  h2      cell
        *                  dw      TRUE    ������ʾ
        **/
        QUI_CUSTOM_METHOD_SHEET_CLOSECELLWIDGET,
        /** behavior��sheet
        *      ��ȡ��ǰ������ʾcell-widget���Ǹ�cell
        *	param
        *		-[in]
        *              _METHOD_PARAM_WITHDOM*
        *                  h1      table
        *      -[out]
        *                  h2      cell    ,NULL, ���Ϊ��
        **/
        QUI_CUSTOM_METHOD_SHEET_GETCUREDITCELL,

    };

    enum
    {
        // ������˸DOMԪ�ر߿�Ķ�ʱ��
        ID_TIMER_ECTRL_FLASHBORDER = 0x0000FFF1,
    };

    namespace dom
    {
        enum _CTRL_SHOW_MODE
        {
            SHOW_MODE_SHOW, // visibility: visible
            SHOW_MODE_HIDE,// visibility: hidden
            SHOW_MODE_COLLAPSE,	 // visibility: collapse
        };

        class ECtrl : public element
        {
            typedef element base;
            class _Edata : public htmlayout::dom::expando
            {
            public:
                _Edata(LPVOID _d) : _data(_d) {}
                virtual ~_Edata() { }
                virtual void finalize()
                {
                    delete this;
                }

            public:
                LPVOID	_data;
            };
        public:
            ECtrl(HELEMENT h=NULL):element(h) {};

            ECtrl& operator=(HELEMENT h);

            ECtrl& operator=(const ECtrl& e);

            ~ECtrl(void) {};

            CStdString ID()
            {
                return get_attribute("id");
            }

            CStdString Name()
            {
                return get_attribute("name");
            }

            inline void SetName(LPCWSTR pszName)
            {
                set_attribute("name",pszName);
            }

            inline void SetID(LPCWSTR pszID)
            {
                set_attribute("id",pszID);
            }
            // ����ʵ�崰�ڣ�<widget type="windowed">
            static ECtrl CreateWindowedWidget();

            BOOL SetFocus();

            // nWay ���뷽ʽ���ο�HTMLayoutSetElementHtml
            void SetHtml(LPCWSTR pszHtml,int nWay = SIH_REPLACE_CONTENT);

            // pData ��������
            void SetData(LPVOID pData);

            // �Ƴ���������
            void RemoveData();

            LPVOID GetData()const ;

            void FlashBorder(int nFlashTime = 3,	// ��˸����
                UINT nMillsecods = 500,	// ��˸Ƶ�ʣ�ms
                LPCWSTR sBorderStyle=L"1px solid red");

            ECtrl FindFirstWithData(LPVOID pData);

            void SetWidth(int nx,LPCWSTR unit=L"px");

            void SetHeight(int ny,LPCWSTR unit=L"px");

            void SetSize(int nx,int ny,LPCWSTR unit=L"px");

            void EnableCtrl(BOOL bEnable = TRUE);
            //
            // nDir Ϊ����
            //   0-�Զ�����,ƫ�����·�

            /** ֱ���ڿؼ���Χ��ʾ��ʾ���ݣ�
            *	-params:
            *		-[in]
            *			sTip	����Ϊhtml����
            *			nDir	��ʾ�ڿؼ��Ķ���
            *			    2 - popup ECtrl below of anchor
            *				8 - popup ECtrl above of anchor
            *				4 - popup ECtrl on left side of anchor
            *				6 - popup ECtrl on right side of anchor
            **/
            void ShowTooltip(LPCWSTR sTip,int nDir=2);

            /** �����������ʱ�Զ���ʾ���ı�����
            *		-[in]
            *			sTipText	���ı�����html����
            *			���ΪNULL��ɾ����ʾ
            **/
            void SetTooltipText(LPCWSTR sTipText=NULL);

            BOOL IsChecked()const
            {
                return get_state(STATE_CHECKED);
            }

            // bCheck=TRUE �� bUncheckSibling = TRUE ��
            //	ѡ�б����ʱ�򽫵�ǰ����state_checked���ֵܽڵ㷴ѡ��
            //  ����ڸ��׾���behavior��switch�Ǻܷ����
            void SetCheck(BOOL bCheck=TRUE,BOOL bUncheckSibling=FALSE);

            BOOL IsHasAttribute(LPCSTR satti)
            {
                return get_attribute(satti) != NULL;
            }

            void SetText(LPCWSTR szFormat, ...);

            // ���������ȡ�����ݿ����ǲ���ȷ��
            // ��ͬ�Ŀؼ�Ӧ�ø��ݿؼ����ص�ʵ���Լ���GetText();
            virtual CStdString GetText()
            {
                return text().c_str();
            }

            void SetValue(const json::value& v)
            {
                set_value(v);
            }

            /** ��ʾ�������ش���
            *		-[in]	_CTRL_SHOW_MODE
            **/
            void ShowCtrl(_CTRL_SHOW_MODE nMode);

            void DeleteAllChild();

            // ɾ��������[ibegin,iend]�ĺ���
            // iend:-1 �����һ��
            void DeleteChild(int ibegin,int iend=-1);

            void ClearBkgndStyle();

            void ClearFrgndStyle();

            void SetCOLORREF(LPCWSTR sColor);

            void SetBkgndColor(LPCWSTR sColor);

            void SetFrgndColor(LPCWSTR sColor);

            void SetBkgndColor(LPCWSTR sTop,LPCWSTR sRight,
                LPCWSTR sBottom,LPCWSTR sLeft);

            void SetFrgndColor(LPCWSTR sTop,LPCWSTR sRight,
                LPCWSTR sBottom,LPCWSTR sLeft);

            void SetBkgndImage(LPCWSTR  sImg);

            void SetFrgndImage(LPCWSTR  sImg);

            // repeat/no-repeat/repeat-x/repeat-y/expand/stretch;
            void SetBkgndRepeat(LPCWSTR sRepeat);

            void SetFrgndRepeat(LPCWSTR sRepeat);

            void SetBkgndPosition(int x,int y);

            void SetFrgndPosition(int x,int y);

            void SetBkgndPosition(int t,int r,int b,int l);

            void SetFrgndPosition(int t,int r,int b,int l);

            void SetBkgndPosition(LPCWSTR sPt);

            void SetFrgndPosition(LPCWSTR sPt);

            // class
            inline CStdString GetClass()
            {
                return get_attribute("class");
            }

            void SetClass(LPCWSTR szClsName = NULL);

            void AddClass(LPCWSTR szClsName);

            void RemoveClass(LPCWSTR szClsName);

            BOOL IsHasClass(LPCWSTR szClsName);

            // 0 ��͸���� 100 ȫ͸��
            void SetTransparent(int nAlpha = 0);

            ECtrl LastChild();
            /** ģ������¼�
            *	return
            *      TRUE    �ɹ�
            *	param
            *		-[in]
            *          uMouseBtn       MOUSE_BUTTONS
            *          ptMouse         �����λ��
            *          uKeybord        KEYBOARD_STATES
            **/
            BOOL SimulateMouseEvent(int cmd,UINT uMouseBtn = MAIN_MOUSE_BUTTON,
                POINT ptMouse = POINT(), UINT uKeybord = 0);

            /** ģ������¼�
            *	return
            *      TRUE    �ɹ�
            *	param
            *		-[in]
            *          cmd         KEY_EVENTS
            *          uKeycode    key scan code, or character unicode for KEY_CHAR
            *          uAltState   KEYBOARD_STATES
            **/
            BOOL SimulateKeybordEvent(UINT uKeycode = 0,UINT cmd = KEY_DOWN, UINT uAltState = 0);
        };

        typedef ECtrl ETableCell;

        class ETableRow : public ECtrl
        {
        public:
            ETableRow(HELEMENT h=NULL):ECtrl(h) {}

            void SetCellText(__in int iCol, LPCWSTR szText);

            void SetCellHtml(__in int iCol, LPCWSTR szHtml);

            CStdString GetCellText(__in int iCol);

            // ������Χ [0, col_count)
            ETableCell GetCell(__in int iCol);

            inline int GetColCount()const
            {
                return children_count();
            }
        };

        class ETable : public ECtrl
        {
        public:
            ETable(HELEMENT h=NULL);

        public:
            int GetSelectedItemCount();

            // iBeginRow �ӵڼ��п�ʼ������0 ��ʼ������
            // ����iBeginRowΪʲôֵ������������ͷ����
            ETableRow GetSelected( int iBeginRow = 0 );

            /** ��ȡ���б�ѡ�е���
            *	return:
            *      ѡ�е�����
            *	params:
            *		-[out]
            *          vItems  ѡ�е���
            **/
            int GetAllSelected(__out std::vector<HELEMENT> & vItems);

            // iRow , 0 ��ʼ��������
            ETableRow GetRow(int iRow);

            inline int GetRowIndex( ETableRow &cRow )
            {
                ATLASSERT(cRow.is_valid() && (cRow.parent() == he));
                return cRow.index() - GetFixedRowCount();
            }

            LPVOID GetRowData(int iRow);

            void SetRowData(int iRow,LPVOID p);
            // iRow , 0 ��ʼ
            // iCol, 0 ��ʼ
            ETableCell GetCell( int iRow,int iCol );

            LPVOID GetCellData(int iRow,int iCol);

            void SetCellData(int iRow,int iCol,LPVOID p);

            void SetCellText(int iRow,int iCol,LPCWSTR pszText);

            void SetCellText( ETableRow &cRow, int iCol,LPCWSTR pszText );

            CStdString GetCellText(int iRow,int iCol);

            CStdString GetCellText( ETableRow &cRow ,int iCol );

            inline int GetFixedRowCount()const
            {
                return get_attribute_int("fixedrows");
            }

            inline int GetRowCount()const
            {
                return children_count() - GetFixedRowCount();
            }

            // -1 ���һ��
            ETableRow InsertRow( LPCWSTR szCells,int iRow = INT_MAX );

            void DeleteRow(int iRow);

            void DeleteAllSelectedRow();

            void DeleteAllRow()
            {
                DeleteChild(GetFixedRowCount());
            }

            /** ѡ����Ŀ
            *	return
            *      ֮ǰѡ�е���Ŀ
            *	param
            *		-[in]
            *          idx     ��Ҫ��ѡ�е���Ŀ����
            *          cRow
            *          bSelect TRUE ѡ��
            **/
            ETableRow SelectRow( int idx,BOOL bSelect = TRUE );

            ETableRow SelectRow( ETableRow cRow,BOOL bSelect = TRUE );

            inline BOOL IsMultiSelect()
            {
                return get_attribute("multiple") != nullptr;
            }

        protected:
            inline int _RealRowIndex(int iRow)
            {
                return iRow + GetFixedRowCount();
            }
        };

        class ETooltip : public ECtrl
        {
        public:
            ETooltip(HELEMENT h=NULL);

        };

        class EOption : public ECtrl
        {
        public:
            EOption(HELEMENT h=NULL);

        public:
            EOption InsertItem(LPCWSTR szItem, int idx = -1);
            /** �����й���ImageList����Ҫ�Ի�ͼ��Ĳ�����
            *	return:
            *      �����Item
            *	params:
            *		-[in]
            *          szItem      item text
            *          iImageIdx   ����ImageList������
            *          idx         ����λ��
            **/
            EOption InsertItem_WithImageIdx(LPCWSTR szItem,int iImageIdx, int idx = -1);
            /** ʹ��InsertItem_WithImageIdx����ʱ��ͼ������
            *	return:
            *      ͼ������ֵ
            *      -1      ��Ч������û�й�����img��
            *      >=0     ��Чֵ
            **/
            int GetItemImageIndex();
        };

        // ����<options>/<option>
        // ���磺dropdown ��popup
        class EOptionsBox : public ECtrl
        {
        public:
            EOptionsBox(HELEMENT h=NULL);

        public:
            int GetItemCount()
            {
                return children_count();
            }

            // idx: -1 last item, 0 first item
            // return : >= 0 ѡ�е���Ŀ������
            //			-1	δ�ҵ�������
            int SetCurSel( __in int idx = -1 )
            {
                return SetCurSel(GetItem(idx));
            }

            int SetCurSel(__in EOption& op);

            // index
            inline int GetItemIndex(__in EOption& op)
            {
                if (!op.is_valid())
                {
                    ATLASSERT(FALSE);
                    return -1;
                }
                return op.index();
            }

            // -1 none selected
            int GetCurSelIndex();

            inline EOption GetCurSelItem()
            {
                return find_first("option:checked,options:checked");
            }

            // Ԫ�������Ƿ���options
            BOOL IsItemGroup(int idx);

            EOption GetItem( int idx );

            CStdString GetItemText(int idx);

            void SetItemText(int idx,LPCWSTR sText);

            CStdString GetItemAttribute(int idx,const char* atti);

            void SetItemAttribute(int idx,LPCSTR satti,LPCWSTR sValue);

            CStdString GetItemStyleAttribute( int idx,LPCSTR satti );

            void SetItemStyleAttribute(int idx,LPCSTR satti,LPCWSTR sValue);

            CStdString GetCurSelItemAttribute(const char* atti);

            EOption SelectItem_IDorName(LPCWSTR sIDorName );

            EOption SelectItem_Attribute(char* atti,LPCWSTR sValue = NULL );

            EOption SelectItem_Text( LPCWSTR sValue );

            EOption SelectItem_ItemData(LPVOID dwData);

            EOption InsertItem( LPCWSTR sItem, int idx = INT_MAX );

            void DeleteItem(int iItem);

            void DeleteAllItem();

            BOOL SetItemData( int idx,LPVOID dwData );

            inline void SetItemData( __in EOption& op, LPVOID dwData )
            {
                op.SetData(dwData);
            }

            LPVOID GetItemData(int idx);

            inline LPVOID GetItemData(__in EOption& op)
            {
                return op.GetData();
            }

        };

        class EProgress : public ECtrl
        {
        public:
            EProgress(HELEMENT h=NULL);

            int GetPos();

            void SetPos(int nPos);

            void SetRange(int nLow,int nHigh);

            void GetRange(__out int &nLow,__out int &nHigh);
        };

        //////////////////////////////////////////////////////////////////////////
        class EList : public EOptionsBox
        {
        public:
            EList(HELEMENT h=NULL);

        };

        class ETree : public EOptionsBox
        {
        public:
            ETree(HELEMENT h=NULL);

        public:
            // ѡ��
            EOption SetCurSel(EOption ti);
            EOption GetCurSelIndex();
            void Expand(EOption tItem);
            // �ڶ�������
            EOption InsertItem( LPCWSTR sItem ,int idx = -1 );
            // ��tiParent�²���
            EOption InsertItem( EOption tiParent, LPCWSTR sItem,int idx = -1);

            EOption GetChildItem(EOption tiParent);
            EOption GetRootItem();
            EOption GetChildItem_WithData( EOption tiParent , LPVOID pData);
        };

//         class EShellTreeItem : public EOption
//         {
//             friend class htmlayout::CHLShellTree;
//             class _Edata : public htmlayout::dom::expando
//             {
//             public:
//                 _Edata(htmlayout::PSHELLITEMINFO _d) : _data(_d) {}
//                 virtual ~_Edata()
//                 {
//                     delete _data;
//                 }
//                 virtual void finalize()
//                 {
//                     delete this;
//                 }
//             public:
//                 htmlayout::PSHELLITEMINFO	_data;
//             };
// 
//         public:
//             EShellTreeItem(HELEMENT h): EOption(h)
//             {
// 
//             }
// 
//             CStdString GetFullPath()
//             {
//                 std::wstring s;
//                 GetData()->pidlFull.GetPath(s);
//                 return s;
//             }
// 
//         protected:
//             void SetData(htmlayout::PSHELLITEMINFO pData);
// 
//             htmlayout::PSHELLITEMINFO GetData();
//         };
// 
//         class EShellTree : public ECtrl
//         {
//         public:
//             EShellTree(HELEMENT h);
// 
//         public:
//             /** ���ø�Ŀ¼
//             *	return:
//             *      TRUE    ���óɹ�
//             *	params:
//             *		-[in]
//             *          pszRootPath  Ŀ¼·���������Ǹ�·��������Ϊclassid
//             *                       ΪNULLʱ����Ŀ¼Ϊ����
//             *          nClsID       CSIDL_DESKTOP....CSIDL_CONTROLS..
//             **/
//             BOOL SetRoot(__in LPCWSTR pszRootPath);
//             BOOL SetRoot(__in int nClsID);
// 
//         };

        class ECombobox : public ECtrl
        {
        protected:
            EOptionsBox		op_box_;
            ECtrl			caption_;

        public:
            ECombobox(HELEMENT h=NULL);

            int GetItemCount()
            {
                return op_box_.GetItemCount();
            }

            // -1 last item, 0 first item
            BOOL SetCurSel( int idx );

            // -1 none selected
            int GetCurSelIndex()
            {
                return op_box_.GetCurSelIndex();
            }

            EOption GetCurSelItem()
            {
                return op_box_.GetCurSelItem();
            }

            // Ԫ�������Ƿ���options
            BOOL IsItemGroup(int idx)
            {
                return op_box_.IsItemGroup(idx);
            }

            ECtrl GetItem(int idx)
            {
                return op_box_.GetItem(idx);
            }

            CStdString GetItemText(int idx)
            {
                return op_box_.GetItemText(idx);
            }

            CStdString GetCurItemText();

            void SetItemText(int idx,LPCWSTR sText)
            {
                op_box_.SetItemText(idx,sText);
            }

            CStdString GetItemAttribute(int idx,const char* atti)
            {
                return op_box_.GetItemAttribute(idx,atti);
            }

            void SetItemAttribute(int idx,LPCSTR satti,LPCWSTR sValue)
            {
                op_box_.SetItemAttribute(idx,satti,sValue);
            }

            CStdString GetItemStyleAttribute( int idx,LPCSTR satti )
            {
                return op_box_.GetItemStyleAttribute(idx,satti);
            }

            void SetItemStyleAttribute(int idx,LPCSTR satti,LPCWSTR sValue)
            {
                op_box_.SetItemStyleAttribute(idx,satti,sValue);
            }

            CStdString GetCurSelItemAttribute(const char* atti)
            {
                return op_box_.GetCurSelItemAttribute(atti);
            }

            EOption SelectItem_IDorName(LPCWSTR sIDorName )
            {
                return op_box_.SelectItem_IDorName(sIDorName);
            }

            EOption SelectItem_Attribute(char* satti,LPCWSTR sValue )
            {
                return op_box_.SelectItem_Attribute(satti,sValue);
            }

            EOption SelectItem_Text( LPCWSTR sValue )
            {
                return op_box_.SelectItem_Text(sValue);
            }

            EOption SelectItem_ItemData(LPVOID dwData)
            {
                return op_box_.SelectItem_ItemData(dwData);
            }

            // idx:-1 last
            // ret: ʵ�ʲ����idx
            EOption InsertItem( LPCWSTR sItem ,int idx = INT_MAX)
            {
                return op_box_.InsertItem(sItem, idx);
            }

            void DeleteAllItem()
            {
                return op_box_.DeleteAllItem();
                caption_.set_text(L"");
            }

            BOOL SetItemData( int idx, LPVOID dwData )
            {
                return op_box_.SetItemData(idx,dwData);
            }

            inline void SetItemData( EOption& op, LPVOID dwData )
            {
                op_box_.SetItemData(op, dwData);
            }

            LPVOID GetItemData(int idx)
            {
                return op_box_.GetItemData(idx);
            }
        };

        class EEditCmb : public ECombobox
        {
        public:
            EEditCmb(HELEMENT h=NULL);

            void SetValue(LPCWSTR sValue);

            inline CStdString GetCaptionText()
            {
                return caption_.GetText();
            }
        };

        class EDate : public ECtrl
        {
        public:
            EDate(HELEMENT h=NULL);

            void SetDate(const QTime& t);

            QTime GetDate();
        };

        class ETime : public ECtrl
        {
        public:
            ETime(HELEMENT h=NULL);

            void SetTime(const QTime& t);

            QTime GetTime();

        };

        class EQTimeCtrl : public ECtrl
        {
        private:
            ECtrl		m_eCaption;
            ECtrl		m_eRoot;
            ECtrl		m_ePopup;

        public:
            EQTimeCtrl(HELEMENT h=NULL);

            void SetTime(const QTime& t);

            QTime GetTime();

        };

        class EFilePath : public ECtrl
        {
        public:
            EFilePath(HELEMENT h=NULL):ECtrl(h)
            {
                //				CTL_TYPE c = get_ctl_type();
                //				ASSERT(get_ctl_type() == CTL_PASSWORD)
            }
        public:
            CStdString GetFileName();
            CStdString GetFilePath();
            void SetFilePath(LPCWSTR pszPath);
        };

        class EFolderPath : public ECtrl
        {
        public:
            EFolderPath(HELEMENT h=NULL):ECtrl(h)
            {

            }
            CStdString GetFolderName();
            CStdString GetFolderPath();
            void SetFolderPath(LPCWSTR pszPath);
        };

        //////////////////////////////////////////////////////////////////////////
        class EEdit : public ECtrl
        {
        public:
            EEdit(HELEMENT h=NULL);
            
        public:
            bool GetSelection( int& start, int& end );
            
            bool SelectText( int start = 0, int end = 0xFFFF );

            bool ReplaceSelection(const wchar_t* text, size_t text_length);

            CStdString GetText() const;

            void SetText(const wchar_t* text, size_t length);

            void SetText(const wchar_t* text);

            void SetInt( int v );

            int GetInt( ) const
            {
                return _wtoi( GetText() );
            }

            void SetDouble( double v );

            double GetDouble()
            {
                return _wtof(GetText());
            }

            int GetCharPos(int x, int y) const;
        };

        class ENumber : public EEdit
        {
        public:
            ENumber(HELEMENT h=NULL);
            
            void SetLimit(int nMin,int nMax);

            
            void SetStep(int nStep);

            int GetNum();
        };

        class EPassword : public EEdit
        {
        public:
            EPassword(HELEMENT h=NULL);
            
        };

        //////////////////////////////////////////////////////////////////////////
        class ECheck : public ECtrl
        {
        public:
            ECheck(HELEMENT h=NULL);
            
        };

        class ERadio : public ECtrl
        {
        public:
            ERadio(HELEMENT h=NULL);
            
        };

        class ERadioGroup : public ECtrl
        {
        public:
            ERadioGroup(HELEMENT h=NULL):ECtrl(h)
            {
            }

            // ��ȡ��ǰ��ѡ�еİ�ť
            ECtrl GetCheck();

            // button ����
            int GetButtonCount()
            {
                return children_count();
            }

            /** ѡ�еڼ���button
            *	return
            *      TRUE        �ɹ�
            *	param
            *		-[in]
            *          idx     �ڼ�����ť����������Ϊ0
            *                  -1  ���һ��
            *          sel     ѡ����
            **/
            BOOL CheckButton(int idx = -1);
            BOOL CheckButton(const char *sel);
        };

        //////////////////////////////////////////////////////////////////////////
        // <div .pagenav>
        //		<table page><tr><td>1</td><td>2</td></tr></table>
        // </div>
        class EPageCtrl : public ECtrl
        {
        private:
            ECtrl m_root;
        public:
            EPageCtrl(HELEMENT h);

        public:
            void SetPageNum(int nPage);
            
            // ��ȡ��ǰѡ�е�index��
            // ����ֵ�� == 0 ��ѡ����
            //		> 0 ѡ�е�ҳ����
            int GetCurPage();
            
            int GetPageNum();
          
            void EPageCtrl::SetCurPage(int iPage);
        };
        //////////////////////////////////////////////////////////////////////////
        class EColorPicker : public ECtrl
        {
        private:
            ECtrl root_;
            ECtrl popup_;

        public :
            EColorPicker(HELEMENT h=NULL);
            
        public:
            CStdString GetColor();
            
            /**
             *	����ȡ������ɫֵ��ARGB����COLORREF��RGB˳���������෴��
             *
            **/
            DWORD GetColorARGB();
            
            DWORD GetCOLORREF();
            
            void SetCOLORREF(DWORD dwColor);
            
            /**
             * ��ʽ��#xxxxxx
             *
            **/
            void SetCOLORREF(LPCWSTR sColor);

            void SetColorARGB( DWORD dwColor );
        };

        //////////////////////////////////////////////////////////////////////////
        // ESlider
        class ESlider : public ECtrl
        {
        public:
            ESlider(HELEMENT h):ECtrl(h)
            {

            }

        public:
            int GetValue()
            {
                return aux::wtoi(get_value().to_string());
            }
            
            int GetMin()
            {
                return aux::wtoi(get_attribute("min"));
            }
            
            int GetMax()
            {
                return aux::wtoi(get_attribute("max"));
            }
            
            void SetRange(int nMin,int nMax)
            {
                set_attribute("min",aux::itow(nMin));
                set_attribute("max",aux::itow(nMax));
            }

            void SetPos(int nPos)
            {
                set_value(json::value(nPos));
            }
        };

        //////////////////////////////////////////////////////////////////////////
        class EStarBox : public ECtrl
        {
        public:
            EStarBox(HELEMENT h=NULL);
            
        public:
            void SetCurSel( int nPri );
            
            // Base Index : 1
            int GetCurSel()
            {
                return aux::wtoi(get_attribute("index"));
            }
        };

        //////////////////////////////////////////////////////////////////////////
        class ETabCtrl : public ECtrl
        {
        public:
            ETabCtrl(HELEMENT h=NULL);

        public:
            static ETabCtrl create();

            int GetTabCount();

            ECtrl GetTab(int idx);

            // ʹ�� [panel="szPanel"] ��strip_�²���
            ECtrl GetTab(LPCWSTR szPanel);

            // ֱ��ʹ��selector��strip_�²���
            ECtrl GetTabWithSelector( const char* szSelector );

            ECtrl GetTabPage(int idx);

            ECtrl GetTabPage(LPCWSTR szPanel);

            ECtrl GetTabPage( ECtrl& eTabItem );

            ECtrl GetPageTab(__in ECtrl &ePage);

            ECtrl InsertTab(LPCWSTR szPanel,LPCWSTR szTab,int idx = -1);

            /**
            *	�Ѿ�����page�����ǲ������������tab������Ϊ�䴴��һ��tab
            *
            *  return
            *      �´�����tab�����ʧ��is_valid ΪFalse
            *	param
            *		-[in]
            *          ePage       �Ѿ����ڵ�ҳ��
            *
            **/
            ECtrl CreateTabOfPage(__in ECtrl ePage, __in LPCWSTR szTabCaption, __in int idx = -1);

            ECtrl GetCurrentTab();

            ECtrl GetTabWithData(LPVOID pData);

            void RemoveTab(LPCWSTR szPanel);

            void RemoveTab(int idx);

            void RemoveTab( ECtrl tab );

            void RemoveAllTabs();

            void SelectTab(int idx);

            void SelectTab(LPCWSTR szPanel);

            void SelectTab(ECtrl ctlTabItem);

            void ShowCloseButton(BOOL bShow=TRUE,LPCWSTR szButtonName=NULL);

            void ShowItemCloseButton(int iItem,BOOL bShow=TRUE,LPCWSTR szButtonName=NULL);

            void ShowItemCloseButton( ECtrl&tabItem,BOOL bShow=TRUE,LPCWSTR szButtonName=NULL );

            BOOL IsShowCloseButton();

        protected:
            ECtrl	strip_;
        };

        //////////////////////////////////////////////////////////////////////////
        // tagbox
        class ETagItem : public ETable
        {
        public:
            ETagItem(HELEMENT h=NULL) : ETable(h) {}

        public:
            CStdString GetTag();

            void SetTag(LPCWSTR szText);
            /** ���ùرհ�ť������
            *	-param
            *		-[in]   pszButtonName   �رհ�ť����
            *                              NULL������ʾ�رհ�ť
            **/
            void ShowCloseButton(LPCWSTR pszButtonName=NULL);

        protected:
            ECtrl TextCell()
            {
                return find_first("tr>td:nth-child(1)");
            }

            ECtrl CloseButtonCell()
            {
                return find_first("tr>td:nth-child(2)");
            }
        };

        class ETagBox : public ECtrl
        {
        public:
            ETagBox(HELEMENT h=NULL) : ECtrl (h)
            {
            }

        public:
            ETagItem AddTag(LPCWSTR szTagText,LPCWSTR szTagName);

            // ѡ��һ��tag
            BOOL CheckTag(int idx);

            int GetTagCount()
            {
                return children_count();
            }

            ETagItem GetCheckedTag();

            int GetAllCheckedTag(__out std::vector<ETagItem>& vtags);

            ETagItem GetTag(int idx);

            ETagItem GetTag(LPCWSTR szTagID);

            void RemoveAllTag();

            /** ��ʾ����ʾ�رհ�ť
            *	-param
            *		-[in]   pszCloseBtnName   �رհ�ť����
            *                                NULL������ʾ�رհ�ť
            **/
            void ShowCloseButton(LPCWSTR pszCloseBtnName=NULL);

        protected:
            inline LPCWSTR _CloseButtonName();
        };

        //////////////////////////////////////////////////////////////////////////
        // menu
        class EMenuItem : public ECtrl
        {
        public:
            EMenuItem(HELEMENT h=NULL):ECtrl(h)
            {

            }

            void Enable(BOOL bEnable = TRUE );
        };

        class EMenu : public ECtrl
        {
        public:
            EMenu(HELEMENT h=NULL):ECtrl(h)
            {

            }

            /** ���á�����ĳ���˵���
            *	return:
            *      �������Ĳ˵���
            *	params:
            *		-[in]
            *          szSelector      find_first(szSelector);
            *          bEnable         FALSE ����
            **/
            EMenuItem EnableMenuItem(LPCWSTR szSelector,BOOL bEnable=TRUE);

            void EnableAllMenuItem(BOOL bEnable = TRUE);
        };

        //////////////////////////////////////////////////////////////////////////
        class EPopup : public ECtrl
        {
        public:
            EPopup(HELEMENT h = NULL):ECtrl(h)
            {

            }
        };

        //////////////////////////////////////////////////////////////////////////
        class EButton : public ECtrl
        {
        public:
            EButton(HELEMENT h):ECtrl(h)
            {

            }
        };

        //////////////////////////////////////////////////////////////////////////
        class EHyperLink : public ECtrl
        {
        public:
            EHyperLink(HELEMENT h):ECtrl(h)
            {

            }
        };

        //////////////////////////////////////////////////////////////////////////
#ifdef UIBASE_SUPPORT_FLASH
        class EFlash : public ECtrl
        {
            friend class QView;
        public:
            EFlash(HELEMENT h) : ECtrl(h)
            {
                ATLASSERT( aux::wcseqi(L"flash",get_attribute("type")) );
            }

        public:
            // �����µ�swf�ļ��������Ǳ���ȫ·��
            void SetSwf(LPCWSTR pszFilePath);

            // ��ȡ��ǰswf�ļ���ַ
            LPCWSTR GetSwfPath();

            // ��ȡ������CFlashWnd����ָ��
            CFlashWnd *GetFlashObject();

            // ���ò������ı�������DOMԪ�ر���
            void SetSwfBkgndColor(DWORD dwRGB);
        };
#endif
        //////////////////////////////////////////////////////////////////////////
#ifdef UIBASE_SUPPORT_WEBBROWSER
        class EWebBrowser : public ECtrl
        {
            friend class QView;
        public:
            EWebBrowser(HELEMENT h) : ECtrl(h)
            {
                ATLASSERT( aux::wcseqi(L"WebBrowser",get_attribute("type")) );
            }

        public:
            // ������URL
            void GotoURL(LPCWSTR psURL);

            // ��ȡ��ǰURL
            LPCWSTR GetURL();

            // ��ȡ������CBrowserHostPtr����ָ��
            CWebBrowser *GetBrowser();

            void SetData( LPVOID pData );
        };
#endif

        //////////////////////////////////////////////////////////////////////////
        // LED
        class ELed : public ECtrl
        {
        public:
            ELed(HELEMENT h) : ECtrl(h)
            {

            }

        public:
            /** ���õƵ�״̬
            *	-param
            *		-[in]
            *          idx     ����ֵ[0, count)
            *          bOn     ���õ���״̬��checked
            **/
            void SetOnOff(int idx,BOOL bOn=TRUE);

            /**ע��״̬���������ô�״̬��Ӧ��img
            *  �����趨��״̬��ʱ�򣬻��Զ��趨ע���img��
            *	return:
            *      TRUE        �ɹ�
            *	params:
            *		-[in]
            *          szStatus    ״̬������NULL
            *          szImg       ״̬��Ӧ��ͼ���ʾ
            **/
            BOOL RegisterStatus(__in LPCSTR szStatus,__in LPCWSTR szImg);

            /** �趨����ע���״̬
            *	return:
            *      FALSE        ʧ�ܣ�Ҳ����״̬δע��
            *	params:
            *		-[in]
            *          idx             led����
            *          szStatus        ��ע��״̬
            *
            **/
            BOOL SetStatus( __in int idx,__in LPCSTR szStatus );

            /** ��ȡ����Ϊidx�ĵƿؼ�
            *	-return
            *      ECtrl����Ҫis_valid�ж����Ƿ���Ч
            *	-param
            *		-[in]
            *          idx     [0,count)
            **/
            ECtrl GetLed(int idx);

            /** ��ȡLED����
            *	-return
            *      int     ��ȡLED���� [0,n]
            **/
            int GetCount();

            /** ���nCount��LED��
            *	-param
            *		-[in]
            *          nCount  ��Ҫ��ӵĸ��� [1,n)
            *          bOn     ��ʼ��״̬
            **/
            void AddLed( int nCount, BOOL bOn=TRUE );

            /** �Ƴ�����nCount��LED
            *	-param
            *		-[in]
            *          nCount  ��Ҫ�Ƴ��ĸ���
            **/
            void RemoveLast(int nCount);

            /** �Ƴ���ǰ���nCount��LED
            *	-param
            *		-[in]
            *          nCount  ��Ҫ�Ƴ��ĸ���
            **/
            void RemoveFirst(int nCount);

            /** �Ƴ�����LED
            *		-[]
            **/
            void RemoveAll();
        };

        //////////////////////////////////////////////////////////////////////////
        // treelist
        // treelist �ؼ��Ĺ�������
        typedef ETableRow ETreeListItem;
        typedef std::vector<HELEMENT> ArrayHE;
        typedef tree<HELEMENT> TreeHE;
        class _qTLStructMan;
        class _qTLStruct        //
        {
            typedef TreeHE::iterator_base base_itr;
            typedef TreeHE::pre_order_iterator pre_itr;
            typedef TreeHE::children_iterator children_itr;

            friend class _qTLStructMan;
        protected:
            // ����ͨ��_qTLStructManע�ᴴ��
            _qTLStruct(HELEMENT heRoot);

            ~_qTLStruct()
            {

            }

        public:

#ifdef _DEBUG
            void _DebugoutputTreeStruct();
#endif
            /** ��ȡheParent�µ�ֱ�Ӻ��ӽڵ�
            *	return:
            *      ���Ӹ�����-1 ����
            *	params:
            *		-[in]
            *          heParent        ���׽ڵ�
            *                          ���ΪNULL�����ȡ��һ���ڵ�
            *		-[out]
            *          arC             ���ӽڵ㣨���Ⱥ�˳��
            **/
            int GetChildren(__in HELEMENT heParent, __out ArrayHE &arC);

            /*
            *	ѡ��hParent�µ��ӽڵ�
            *      *_Callback ����trueֹͣ����
            */
            template<class _Callback>
            void SelectChildItem(__in HELEMENT heParent, _Callback&& cb)
            {
                pre_itr iPos;
                if ((NULL == heParent) || (heParent == m_heTL))
                    iPos = m_t.root();
                else if ( !_FindItem(heParent, iPos) )
                    return;

                children_itr iEnd = m_t.end_child(iPos);
                for (children_itr i = m_t.begin_child(iPos); i != iEnd; ++i)
                {
                    if (cb(*i))
                    {
                        break;
                    }
                }
            }

            /** ��ȡ��heItemΪ���ڵ�Ľڵ���
            *	return
            *      TRUE        �ɹ�
            *	param
            *		-[in]
            *          heItem      �ڵ�
            *		-[out]
            *			t           �ڵ���
            **/
            BOOL GetNodeTree(__in HELEMENT heItem, __out TreeHE& t);

            /** ��ȡheParent�ĵ�iPos������
            *	return:
            *      ���ӽڵ㣬NULL��û���ҵ��������
            *	params:
            *		-[in]
            *          heParent        ���׽ڵ�
            *                          ���ΪNULL�����ȡ��һ���ڵ�
            *          iPos            ����λ��������������Ϊ0
            *                          -1      ���һ������
            *
            **/
            HELEMENT GetNthChild(__in HELEMENT heParent, __in int idx = -1 );

            /** ��ȡ��һ���ֵܽڵ�
            *	return
            *      ��һ���ֵܽڵ�
            *	param
            *		-[in]
            *          heItem      �ڵ�
            *
            **/
            HELEMENT GetNextSiblingItem(__in HELEMENT heItem);

            /** ��ȡ��һ���ֵܽڵ�
            *	return
            *      ��һ���ֵܽڵ�
            *	param
            *		-[in]
            *          heItem      �ڵ�
            *
            **/
            HELEMENT GetPrevSiblingItem(__in HELEMENT heItem);

            /** ��ȡ���ӽڵ�ĸ�����ֻ����ֱ�Ӻ��ӣ�
            *	return
            *      -1  ����
            *	param
            *		-[in]
            *          heItem      �ڵ�
            *                      ���ΪNULL�� ����Ϊ�ǻ�ȡ��һ���ڵ�ĸ���
            *
            **/
            int GetChildrenCount(__in HELEMENT heItem = NULL);

            /** ��ȡ���׽ڵ�
            *	return
            *      NULL û����
            *	param
            *		-[in]
            *          heItem      ˭�ĸ��ף�
            *
            **/
            HELEMENT GetParentItem(__in HELEMENT heItem);

            /** ���뺢�ӽڵ�
            *	return
            *      �Ƿ�ɹ����뺢�ӽڵ㣿
            *	param
            *		-[in]
            *          heParent        ���׽ڵ�
            *                          ���ΪNULL������Ϊ�����ڵ㣨���ĵ�һ���ڵ㣩
            *          heItem          ���ӽڵ�
            *          iPos            ���׵ĵڼ������ӣ�����Ϊ0
            *                          ����Ӧ�ñ����뵽��λ��
            **/
            BOOL InsertChild(__in HELEMENT heParent, __in HELEMENT heItem, __inout int& idx);

            /** ��ȡ�ӽڵ����
            *	return
            *      �ӽڵ����
            *	param
            *		-[in]
            *          heItem      ���׽ڵ㣬
            *                      ���ΪNULL����ȡ��һ���ӽڵ�
            *
            **/
            inline int GetChildItemCount(__in HELEMENT heItem = NULL);

            /** ɾ���ڵ�
            *	return
            *      TRUE        �ɹ�
            *	param
            *		-[in]
            *          heItem      Ŀ��ڵ�
            *
            **/
            BOOL DeleteItem(__in HELEMENT heItem);

            void DeleteAllItem()
            {
                m_t.erase_children(m_t.root());
            }

            void ExpandItem(__in HELEMENT heItem);

            /** չ�� heItem ��������֧���Ӹ���Ҷ�ӣ�
            *	param
            *		-[in]
            *          heItem      ��֧�е�һ���ڵ�
            **/
            void ExpandLeaf(__in HELEMENT heItem);

            // չ�����еķ�֧
            void ExpandAllLeaf();

            void CollapseItem(__in HELEMENT heItem);

            /** �������еڡ�nLevel����Ľڵ�
            *	param
            *		-[in]
            *          nLevel      �ڼ���Ľڵ㣬 ȡֵ��Χ[1, n)
            **/
            void CollapseAll(__in int nLevel);

            inline BOOL IsItemExpand(ETreeListItem& cRow)
            {
                return cRow.get_state(STATE_EXPANDED) // cRow.IsHasAttribute("tl_expand")
                    && !cRow.IsHasAttribute("tl_hide");
            }

            inline BOOL IsItemHidingExpand(ETreeListItem& cRow)
            {
                return cRow.get_state(STATE_EXPANDED) //cRow.IsHasAttribute("tl_expand")
                    && cRow.IsHasAttribute("tl_hide");
            }

            /** �ж�heAncestor�Ƿ���heItem�����ȣ�����/�游/��������
            *      ���ڵ㣨treeelist ������Ȼ�����нڵ�Ķ���
            *      �����������κνڵ�����ȡ�
            *	return
            *      TRUE    ������
            *	param
            *		-[in]
            *          heItem
            *          heAncestor      ���Ƚڵ�
            *
            **/
            BOOL IsAncestor(__in HELEMENT heItem, __in HELEMENT heAncestor);

            /** ���ڵ㣨�������������е�Ŀ��ڵ���
            *	return
            *      TRUE        �ɹ�
            *                  Դ�ڵ㲻������Ŀ��ڵ������
            *	param
            *		-[in]
            *          heItem      Դ�ڵ�
            *          heDest      Ŀ��ڵ�
            **/
            BOOL CutTo(__in HELEMENT heItem, __in HELEMENT heDest);

        protected:
            template<typename T_itr>
            BOOL _FindItem(HELEMENT heItem, __out T_itr &iPos)
            {
                ATLASSERT(NULL != heItem);
                ATLASSERT(m_heTL != heItem);

                TreeHE::fixed_depth_iterator i = m_t.begin_fixed(m_t.root(),
                    ECtrl(heItem).get_attribute_int("tl_level") + 1);   // ��0����treelist�����ǽڵ�
                for ( ; m_t.is_valid(i); ++i)
                {
                    if (*i == heItem)
                    {
                        iPos = i;
                        return TRUE;
                    }
                }
                return FALSE;
            }

            inline BOOL IsValidTreeListItem(__in HELEMENT heItem)
            {
                ETreeListItem cTmp(heItem);
                return cTmp.is_valid() && (m_heTL == cTmp.parent());
            }

        private:
            TreeHE       m_t;
            HELEMENT    m_heTL; // treelist ��DOM
        };

        // �������̣���ʵ������UI�̣߳���TreeList�����ݶ���ע�ᵽ���
        // ����������
        class _qTLStructMan
        {
            typedef std::map<HELEMENT, _qTLStruct*> MapTL;
            typedef MapTL::iterator MapTLItr;

            SINGLETON_ON_DESTRUCTOR(_qTLStructMan)
            {
                MapTLItr iEnd = m_TLs.end();
                for (MapTLItr i = m_TLs.begin(); i != iEnd; ++i)
                {
                    delete i->second;
                }
                m_TLs.clear();
            }

            friend struct treelist;
        public:
            static _qTLStruct* GetTLStruct(HELEMENT heRoot);

        protected:
            _qTLStruct* RegisterStruct(HELEMENT heRoot);

            void UnregisterStruct(HELEMENT heRoot);

            inline MapTLItr _Find(HELEMENT heRoot)
            {
                return m_TLs.find(heRoot);
            }

            inline MapTLItr _End()
            {
                return m_TLs.end();
            }

        private:
            MapTL       m_TLs;
        };

        class ETreeList : protected ETable
        {
            typedef ETable _Base;

        private:
            _qTLStruct *    m_pTL;

        public:
            ETreeList(HELEMENT h=NULL)
                :ETable(h)
            {
                m_pTL = _qTLStructMan::GetTLStruct(h);
            }

            ETreeList& operator=(const ETreeList&o);

            operator HELEMENT()const
            {
                return he;
            }

        public:
            inline BOOL IsValid()
            {
                return _Base::is_valid();
            }

#ifdef _DEBUG
            void _DebugoutputTreeStruct()
            {
                m_pTL->_DebugoutputTreeStruct();
            }
#endif
            /** ����һ�����ӽڵ�
            *	return:
            *      ����item
            *	params:
            *		-[in]
            *          cParent     �������ĸ��ڵ���
            *          szCells     �ڵ�����
            *          idx         ���ڽڵ��µĵڼ���λ��
            *
            **/
            ETreeListItem InsertChild(__in HELEMENT heParent,
                __in LPCWSTR szCells, __in int idx = -1);

            ETreeListItem AppendChild(__in HELEMENT heParent,
                __in LPCWSTR szCells)
            {
                ATLASSERT(NULL != szCells);
                return InsertChild(heParent, szCells, -1);
            }

            ETreeListItem InsertChild(__in HELEMENT heParent,
                __in HELEMENT heItem, __in int idx = -1);

            ETreeListItem AppendChild(__in HELEMENT heParent,
                __in HELEMENT heItem)
            {
                ATLASSERT(NULL != heItem);
                return InsertChild(heParent, heItem, -1);
            }

            /** ���붥��item
            *	return:
            *      �������
            *	params:
            *		-[in]
            *          szCells �е����� ��ʽ�� <td>1</td><td>2</td>
            *          idx     ���뵽�ڼ��У�
            *                  = -1�� ���һ��
            **/
            ETreeListItem InsertItem(__in LPCWSTR szCells, __in int idx = -1)
            {
                return InsertChild(NULL, szCells, idx);
            }

            ETreeListItem GetNextSiblingItem(__in ETreeListItem& cItem)
            {
                return m_pTL->GetNextSiblingItem(cItem);
            }

            ETreeListItem GetPrevSiblingItem(__in ETreeListItem& cItem)
            {
                return m_pTL->GetPrevSiblingItem(cItem);
            }

            /** ��һ���ĵڼ�������
            *	return:
            *      ����item
            *	params:
            *		-[in]
            *          cParent     �ĸ��ڵ��µĺ���
            *          idx         �ڼ������ӣ�[-1, size), = -1, ���һ������
            **/
            ETreeListItem GetNthChild( __in ETreeListItem& cParent, __in int idx = -1 )
            {
                return m_pTL->GetNthChild(cParent, idx);
            }

            // չ��������������Ŀ
            inline BOOL IsItemExpand(ETreeListItem& cItem)
            {
                return m_pTL->IsItemExpand(cItem);
            }

            inline BOOL IsItemHidingExpand(ETreeListItem& cItem)
            {
                return m_pTL->IsItemHidingExpand(cItem);
            }

            void ExpandItem(ETreeListItem& cItem)
            {
                m_pTL->ExpandItem(cItem);
            }

            // չ�����еķ�֧
            void ExpandAll()
            {
                m_pTL->ExpandAllLeaf();
            }

            /** չ�� heItem ��������֧���Ӹ���Ҷ�ӣ�
            *	param
            *		-[in]
            *          heItem      ��֧�е�һ���ڵ�
            **/
            void ExpandLeaf(__in HELEMENT heItem)
            {
                m_pTL->ExpandLeaf(heItem);
            }

            void CollapseItem( ETreeListItem& cItem )
            {
                m_pTL->CollapseItem(cItem);
            }

            /** �������еڡ�nLevel����Ľڵ�
            *	param
            *		-[in]
            *          nLevel      �ڼ���Ľڵ㣬 ȡֵ��Χ[1, n)
            **/
            void CollapseAll(__in int nLevel = 1)
            {
                m_pTL->CollapseAll(nLevel);
            }

            int GetItemCount()const
            {
                return m_pTL->GetChildItemCount(NULL);
            }

            int GetChildItemCount(ETreeListItem &cParent)
            {
                return m_pTL->GetChildItemCount(cParent);
            }

            int GetChildItem(__in ETreeListItem& cParent, __out ArrayHE& vI)
            {
                return m_pTL->GetChildren(cParent, vI);
            }

            /*
            *	ѡ��hParent�µ��ӽڵ�
            *      *_Callback ����trueֹͣ����
            */
            template<class _Callback>
            void SelectChildItem(__in ETreeListItem& cParent, _Callback cb)
            {
                return m_pTL->SelectChildItem(cParent, cb);
            }

            // ����item
            int GetItems(__out ArrayHE& vI)
            {
                return m_pTL->GetChildren(NULL, vI);
            }

            // == -1, ���һ������
            ETreeListItem GetItem(__in int idx = -1)
            {
                return m_pTL->GetNthChild(NULL, idx);
            }

            // ��ȡ���׽ڵ�
            ETreeListItem GetParentItem(ETreeListItem& cItem)
            {
                return m_pTL->GetParentItem(cItem);
            }

            // ��ǰѡ�еĽڵ�
            ETreeListItem GetCurrentItem();

            // ���õ�ǰѡ�е���Ŀ
            //  ����֮ǰѡ�е���Ŀ
            ETreeListItem SetCurrentItem(ETreeListItem &cItem);

            // ����ĳ�������ݵĽڵ�
            ETreeListItem GetItemWithData(LPVOID pData)
            {
                return _Base::FindFirstWithData(pData);
            }

            // ����item
            ETreeListItem SetItemText(ETreeListItem&cItem, LPCWSTR szText);

            void DeleteAllItem()
            {
                m_pTL->DeleteAllItem();
                _Base::DeleteAllRow();
            }

            void DeleteItem(ETreeListItem &cItem);

            /** �ж�heAncestor�Ƿ���heItem�����ȣ�����/�游/��������
            *      ���ڵ㣨treeelist ������Ȼ�����нڵ�Ķ���
            *      �����������κνڵ�����ȡ�
            *	return
            *      TRUE    ������
            *	param
            *		-[in]
            *          heItem
            *          heAncestor      ���Ƚڵ�
            *
            **/
            BOOL IsAncestor(__in HELEMENT heItem, __in HELEMENT heAncestor)
            {
                return m_pTL->IsAncestor(heItem, heAncestor);
            }

            /** ���ڵ㣨�������������е�Ŀ��ڵ���
            *	return
            *      TRUE        �ɹ�
            *                  Դ�ڵ㲻������Ŀ��ڵ������
            *	param
            *		-[in]
            *          heItem      Դ�ڵ�
            *          heDest      Ŀ��ڵ�
            **/
            BOOL CopyTo(__in HELEMENT heItem, __in HELEMENT heDest);
            BOOL CutTo(__in HELEMENT heItem, __in HELEMENT heDest);

            // ��ȡ�ڼ���
            inline int GetItemLevel(__in ETreeListItem& cItem)
            {
                return cItem.get_attribute_int("tl_level");
            }

        protected:
            inline void _SetLevel(__in ETreeListItem& cParent,
                __in ETreeListItem& cItem);

            // ����level
            inline void _SetLevel(__in ETreeListItem& cItem)
            {
                cItem.set_attribute("tl_level", L"0");
            }

            inline int _HE2I(HELEMENT h)
            {
                return (int)h;
            }

            inline int _LevelMargin()
            {
                int n = get_attribute_int("level_margin");
                return n > 10 ? n : 10;  // ��С10px
            }
        };

        //////////////////////////////////////////////////////////////////////////
        class EOutlook : public ECtrl
        {
        public:
            EOutlook(HELEMENT he = NULL):ECtrl(he)
            {

            }

        public:
            /** չ��������һ��item
            *	return:
            *      szItemSelector ָ���item
            *	params:
            *		-[in]
            *          szItemSelector      find_first("li<szItemSelector>")
            *          bExpand             TRUE չ��
            **/
            ECtrl ExpandItem(LPCWSTR szItemSelector,BOOL bExpand=TRUE);

            /** �Ƿ�֧�ֶ��itemͬʱչ��
            *      ʹ�� multi-expand=1 ���ö���Ŀչ��ģʽ
            *	return:
            *      TRUE        ��
            **/
            BOOL IsMultiExand()
            {
                return IsHasAttribute("multi-expand");
            }

            // ���ö�itemͬʱչ��ģʽ
            void EnableMulitExpand(BOOL bEnable = TRUE);

            // ��ʾitem
            void ShowItem(LPCWSTR szSelector,BOOL bShow = TRUE);

            inline ECtrl GetItem(LPCWSTR szSelector)
            {
                return find_first("li%S",szSelector);
            }

            inline ECtrl GetItemPanel(__in ECtrl& itm)
            {
                return itm.find_first(":root>.content");
            }

            ECtrl GetItemPanel(LPCWSTR szSelector);
            
            inline ECtrl GetCurrentExpandItem()
            {
                return find_first("li:expanded");
            }

            // ����һ����Ŀ
            ECtrl InsertItem(__in LPCWSTR szItem, __in LPCWSTR szContTag = L"div",
                __in int idx = INT_MAX);

        };

        //////////////////////////////////////////////////////////////////////////
        // �� sheet
        class ESheet : public ETable
        {
        public:
            ESheet(HELEMENT h = NULL) : ETable(h) {}

        public:
            // ��ʾ������cCell�µ�widget
            BOOL ShowCellWidget( ETableCell cCell );

            BOOL ShowCellWidget( ETableRow cRow, int iCol)
            {
                return ShowCellWidget(cRow.GetCell(iCol));
            }

            BOOL ShowCellWidget(int iRow, int iCol)
            {
                return ShowCellWidget(GetCell(iRow, iCol));
            }

            // �رյ�ǰ��ʾ��cell-widget
            // bUpdateContent       TRUE    ����cell-widget���ݵ�cell��
            void CloseCellWidget( BOOL bUpdateContent = TRUE );

            // �Ƿ�������ʾ��cell-widget
            inline BOOL IsCellWidgetShowing(ETableCell cell)
            {
                return GetCurEditCell() == cell;
            }

            // ��ǰ������ʾ��cell-widget��cell
            ETableCell GetCurEditCell();
        };

        //////////////////////////////////////////////////////////////////////////
        // ����ʱ��behavior
        // <div style="behavior:clock-countdown;">00:30:29</div>
        class ETextCountdown : public ECtrl
        {
        public:
            ETextCountdown(HELEMENT h =NULL ) :ECtrl(h)
            {

            }

        public:
            // ���õ���ʱ
            // szTime ��ʽ�� HH:MM:SS
            void Settime(LPCWSTR szTime)
            {
                json::string ss(szTime);
                xcall("settime", ss);
            }

            // �����ڵĻ���������nSec��
            void Increase(int nSec)
            {
                json::value vl(nSec);
                xcall("increase", vl);
            }

            // �����ڵĻ����ϼ���nSec��
            // �������Ϊ0����ֹͣ����ʱ
            void Decrease(int nSec)
            {
                json::value vl(nSec);
                xcall("decrease", vl);
            }

            void Stop()
            {
                xcall("stop");
            }

            void Start()
            {
                xcall("start");
            }

            int GetCountdown()
            {
                return get_attribute_int("countdown");
            }
        };

        class ETextImage : public ECtrl
        {
        public:
            ETextImage(HELEMENT h): ECtrl(h)
            {

            }

        public:
            void SetImage(__in LPCWSTR szImg);

            void SetText(LPCWSTR szText);
        };

        //////////////////////////////////////////////////////////////////////////
        class EWordInput : public ECtrl
        {
        protected:
            EPopup  popup_;
            EOptionsBox wordlst_;
            EEdit   inpt_;
            ECtrl   cap_;

        public:
            EWordInput(HELEMENT he = NULL);
            
        public:
            // ��ӵ�Caption��
            ECtrl AddCaptionItem(__in LPCWSTR szItem);

            // ���һ����Ŀ
            EOption AddWordItem(__in LPCWSTR szItem);

            // caption�� �Ƿ��Ѿ�����һ��
            inline BOOL IsCaptionHasWord(__in LPCWSTR szWord)
            {
                return FindWord(szWord).is_valid();
            }

            // ��caption�в���
            ECtrl FindWord(__in LPCTSTR szWord);

            // ɾ��caption�е�ѡ����Ŀ
            void DeleteCheckedCaptionItem();

            // ����select
            void FilterSelectByInput();

            // items
            CStdString GetCaptionItems(__in wchar_t chSep);

        protected:
            inline BOOL IsWordCaseSentive()
            {
                return IsHasAttribute("word-case-senstive");
            }
        };
    };
};

#endif // ECtrl_h__
