#include "behavior_aux.h"
#include "ui/ECtrl.h"

namespace htmlayout 
{

// Shell styles for all controls
#define SCT_EX_NOFOLDERS      0x00000001
#define SCT_EX_NOFILES        0x00000002
#define SCT_EX_SHOWHIDDEN     0x00000004
#define SCT_EX_NOREADONLY     0x00000008
#define SCT_EX_LOCALCOMPUTER  0x00000010
#define SCT_EX_FILESYSTEMONLY 0x00000020
#define SCT_EX_NOROOT         0x00000040

/////////////////////////////////////////////////////////////////////////////
// Misc Shell methods

inline BOOL AtlGetFilePidl(LPCTSTR pstrFileName, LPITEMIDLIST* pidl)
{
    ATLASSERT(!::IsBadStringPtr(pstrFileName, MAX_PATH));
    ATLASSERT(pidl);
    *pidl = NULL;
    // Make sure the file name is fully qualified and in Unicode format.
    TCHAR szFileName[MAX_PATH];
    ::GetFullPathName(pstrFileName, sizeof(szFileName)/sizeof(TCHAR), szFileName, NULL);
    USES_CONVERSION;
    LPOLESTR pwchPath = const_cast<LPOLESTR>(T2COLE(pstrFileName));
    // Convert the path name into a PIDL relative to the desktop
    CComPtr<IShellFolder> spFolder;
    if( FAILED( ::SHGetDesktopFolder(&spFolder) ) ) return FALSE;
    ULONG ulAttr;
    if( FAILED(spFolder->ParseDisplayName(NULL, NULL, pwchPath, NULL, pidl, &ulAttr)) ) return FALSE;   
    return TRUE;
};

inline BOOL AtlGetShellPidl(LPCITEMIDLIST pidl, IShellFolder** ppFolder, LPITEMIDLIST* pidlRel)
{
    ATLASSERT(pidl);
    ATLASSERT(ppFolder);
    ATLASSERT(pidlRel);

    *ppFolder = NULL;
    *pidlRel = NULL;

    // Get the desktop folder as a starting point
    CComPtr<IShellFolder> spFolder;
    if( FAILED( ::SHGetDesktopFolder(&spFolder) ) ) return FALSE;

    CPidl pidlMain;
    pidlMain.Copy(pidl);

    // Traverse each PIDL item and create a new IShellFolder for each of
    // them. Eventually we get to the last IShellFolder object and is left
    // with a simple (as opposed to complex) PIDL.
    int nCount = pidlMain.GetCount();
    while( --nCount > 0 ) {
        // Get the next PIDL entry
        CPidl pidlNext;
        pidlNext.Attach( pidlMain.CopyFirstItem() );
        if( pidlNext.IsEmpty() ) return FALSE;
        // Bind to the folder specified in the new item ID list.
        CComPtr<IShellFolder> spNextFolder;
        if( FAILED( spFolder->BindToObject(pidlNext, NULL, IID_IShellFolder, (LPVOID*) &spNextFolder)) ) return FALSE;
        spFolder = spNextFolder;
        // Strip first PIDL entry and copy remaining
        CPidl temp;
        temp.Copy( pidlMain.GetNextItem() );
        pidlMain.Attach(temp.Detach());
    }

    *ppFolder = spFolder.Detach();
    *pidlRel = pidlMain.Detach();
    return TRUE;
};

/*

BEHAVIOR: shell-tree
    goal: �ļ�Ŀ¼��
COMMENTS: 
   <widget type="shell-tree" src="f:\" />
   src  Ϊ��ʼ����ʾ��Ŀ¼
        ���Ϊ�գ���������Ϊ��Ŀ¼
SAMPLE:
   <widget type="shell-tree" src="f:\" />
*/

struct system_shellicon;
extern system_shellicon system_shellicon_instance;

template< class T, typename TItem=int >
class CShellBaseCtrl
{
protected:
    DWORD m_dwShellStyle;

public:
    CShellBaseCtrl() : m_dwShellStyle(0UL)
    {
    }

    void SetShellStyle(DWORD dwStyle)
    {
        m_dwShellStyle = dwStyle;
    }

    DWORD GetShellStyle() const
    {
        return m_dwShellStyle;
    }

    BOOL Populate(int csidl = CSIDL_DESKTOP)
    {
        CPidl pidl;
        if( FAILED( ::SHGetSpecialFolderLocation(NULL, csidl, &pidl) ) ) return FALSE;
        CComPtr<IShellFolder> spDesktop;
        if( FAILED( ::SHGetDesktopFolder(&spDesktop) ) ) return FALSE;
        return Populate(spDesktop, pidl, csidl == CSIDL_DESKTOP ? NULL : (LPCITEMIDLIST)pidl);
    }

    // pstrPath �������Ե� "\" ��β��˫�ľʹ���
    BOOL Populate(LPCWSTR pstrPath)
    {
        ATLASSERT(!::IsBadStringPtr(pstrPath,MAX_PATH));
        USES_CONVERSION;
        CComPtr<IShellFolder> spDesktop;
        if( FAILED( ::SHGetDesktopFolder(&spDesktop) ) ) return FALSE;
        CPidl pidl;
        DWORD dwAttribs = 0;
        DWORD dwEaten = 0;
        // ɵ���ַ�������ת��
        // pstrPath �������Ե� "\" ��β��˫�ľʹ���
        HRESULT hRet = spDesktop->ParseDisplayName(NULL, NULL, 
             const_cast<LPWSTR>(pstrPath), &dwEaten, &pidl, &dwAttribs);
        if( FAILED(hRet) )
        {
            return FALSE;
        }
        return Populate(pidl);
    }

    BOOL Populate(LPCITEMIDLIST pidl)
    {
        CComPtr<IShellFolder> spFolder;
        CPidl pidlItem;
        if( !AtlGetShellPidl(pidl, &spFolder, &pidlItem) ) return FALSE;
        return Populate(spFolder, pidl, pidlItem);
    }

    BOOL Populate(IShellFolder* pFolder, LPCITEMIDLIST pidlPath, LPCITEMIDLIST pidlNode)
    {
        ATLASSERT(pFolder);
        ATLASSERT(pidlPath);
        CComPtr<IShellFolder> spFolder;
        DWORD dwAttribs = SFGAO_FILESYSANCESTOR | SFGAO_HASSUBFOLDER;
        if( pFolder != NULL && !CPidl::PidlIsEmpty(pidlNode) ) {
            // Get the new IShellFolder object
            if( FAILED( pFolder->BindToObject(pidlNode, NULL, IID_IShellFolder, (LPVOID*)&spFolder) ) ) return 0;
            // Get this folder's attributes
            pFolder->GetAttributesOf(1, &pidlNode, &dwAttribs);
        }
        else {
            // Folder is Desktop
            if( FAILED( ::SHGetDesktopFolder(&spFolder) ) ) return FALSE;
            dwAttribs = SFGAO_HASSUBFOLDER;
        }
        T* pT = static_cast<T*>(this);
        return pT->_Populate(spFolder, pidlPath, dwAttribs);
    }

    BOOL GetItemPidl(TItem hItem, LPITEMIDLIST* pidl)
    {
        T* pT = static_cast<T*>(this);
        pT;
        ATLASSERT(::IsWindow(pT->m_hWnd));
        ATLASSERT(pidl);
        // NOTE: We can't really check the 'hItem' argument here because it may
        //       actually be 0 for some controls (i.e. ListView has 0 as index)
        //       so we need to rely on the argument being passed is a valid item!
        *pidl = NULL;
        DWORD_PTR lParam = pT->GetItemData(hItem);
        if( lParam == NULL || lParam == -1 ) return FALSE;
        PSHELLITEMINFO pItem = reinterpret_cast<PSHELLITEMINFO>(lParam);
        *pidl = CPidl::PidlCopy(pItem->pidlFull);
        return TRUE;
    }

    BOOL GetItemPath(TItem hItem, LPTSTR pstrPath)
    {
        ATLASSERT(!::IsBadWritePtr(pstrPath, MAX_PATH));
        pstrPath[0]=_T('\0');
        CPidl pidl;
        if( !GetItemPidl(hItem, &pidl) ) return FALSE;
        if( !::SHGetPathFromIDList(pidl, pstrPath) ) return FALSE;
        return TRUE;
    }

    // Returns TRUE if PIDL should be filtered out...
    BOOL _FilterItem(IShellFolder* pFolder, LPCITEMIDLIST pidl, DWORD& dwAttribs) const
    {
        ATLASSERT(pFolder);
        ATLASSERT(pidl);

        if( m_dwShellStyle == 0 && dwAttribs == 0 ) return FALSE;
        dwAttribs |= SFGAO_DISPLAYATTRMASK | SFGAO_FOLDER | SFGAO_FILESYSTEM | SFGAO_FILESYSANCESTOR;

        // A fix by Anatoly Ivasyuk to only query for limited attributes on a
        // removable media (prevents floppy activity)...
        DWORD dwRemovable = SFGAO_REMOVABLE;
        pFolder->GetAttributesOf(1, &pidl, &dwRemovable);
        if( (dwRemovable & SFGAO_REMOVABLE) != 0 ) dwAttribs &= ~SFGAO_READONLY;
        pFolder->GetAttributesOf(1, &pidl, &dwAttribs);

        // Filter some items
        if( (m_dwShellStyle & SCT_EX_NOFOLDERS) != 0 && (dwAttribs & SFGAO_FOLDER) != 0 ) return TRUE;
        if( (m_dwShellStyle & SCT_EX_NOFILES) != 0 && ((dwAttribs & SFGAO_FOLDER) == 0) ) return TRUE;
        if( (m_dwShellStyle & SCT_EX_NOREADONLY) != 0 && (dwAttribs & SFGAO_READONLY) != 0 ) return TRUE;;
        if( (m_dwShellStyle & SCT_EX_FILESYSTEMONLY) != 0 && ((dwAttribs & (SFGAO_FILESYSTEM | SFGAO_FILESYSANCESTOR)) == 0) ) return TRUE;

        return FALSE;
    }
};

class CHLShellTree :
    public behavior,
    public ETree,
    public CShellBaseCtrl<CHLShellTree>
{
public:
    CHLShellTree(): behavior(HANDLE_BEHAVIOR_EVENT, "shell-tree") {}

    virtual void attached  (HELEMENT he ) 
    { 
        this->he = he;

        LPCWSTR pszFolder = ECtrl(he).get_attribute("src");
        if (NULL == pszFolder)
            Populate();
        else     // ʹ��class id
        {
            if (StrCmpNIW(pszFolder, L"clsid:", 6) == 0)
            {
                if (wcslen(pszFolder) > 6)
                {
                    Populate(_wtoi(pszFolder + 6));
                }
            }
            else
            {
                Populate(pszFolder);
            }
        }
    } 

    /**
     *  ����ʹ��on_event���أ���Ϊ�����ܻ�ȡ״̬�仯���Ǹ�item
    **/
    virtual BOOL handle_event (HELEMENT he, BEHAVIOR_EVENT_PARAMS& params )
    {
        if (SELECT_STATE_CHANGED == params.cmd)
        {
            // ��չ����ʱ����������Ŀ¼
            EShellTreeItem tItem(params.he);
            if (tItem.get_attribute("not-expand-ever")) // ��ûչ����
            {
                OnItemExpanding(tItem); // he ����״̬�仯���Ǹ�item
            }
        }
        return false;
    }

//     BOOL SelectPidl(LPCITEMIDLIST pidlTarget)
//     {
//         // BUG: Assumes tree control's root is desktop!
//         CPidl pidlItem;
//         EShellTreeItem tItem = GetRootItem();
//         while( !CPidl::PidlIsEmpty(pidlTarget) ) 
//         {
//             tItem = GetChildItem(tItem);
//             pidlItem.Attach( CPidl::PidlCopyFirstItem(pidlTarget) );
//             while( tItem.is_valid() ) 
//             {
//                 DWORD_PTR lParam = GetItemData(tItem);
//                 ATLASSERT(lParam>0);
//                 PSHELLITEMINFO pItem = reinterpret_cast<PSHELLITEMINFO>(lParam);
//                 if( pItem->spFolder->CompareIDs(0, pidlItem, pItem->pidlNode) == 0 ) 
//                 {
//                     if( CPidl::PidlGetCount(pidlTarget)>1 ) Expand(tItem);
//                     break;
//                 }
//                 tItem = GetNextSiblingItem(tItem);
//             }
//             if( tItem == NULL ) return FALSE; // folder not found?
//             pidlTarget = CPidl::PidlGetNextItem(pidlTarget);
//         }
//         if( tItem != NULL ) SelectItem(tItem);
//         return TRUE;
//     }

    BOOL _Populate(IShellFolder* pFolder, LPCITEMIDLIST pidlParent, DWORD dwAttribs)
    {
        ATLASSERT(pidlParent);
        DeleteAllItem();
        EShellTreeItem tiParent = (HELEMENT)GetRootItem();
        EShellTreeItem tItem = _InsertItem(pFolder, pidlParent, NULL, 
                    dwAttribs, tiParent);
        Expand(tItem);
        SetCurSel(tItem);
        return TRUE;
    }

    EShellTreeItem _InsertItem(IShellFolder* pFolder, LPCITEMIDLIST pidlPath, LPCITEMIDLIST pidlNode,
        DWORD dwAttribs, EShellTreeItem tiParent)
    {
        ATLASSERT(pFolder);
        ATLASSERT(pidlPath);

        // Create PARAM data
        PSHELLITEMINFO pItem;
        ATLTRY(pItem = new SHELLITEMINFO);
        ATLASSERT(pItem);
        pItem->pidlFull.Copy( pidlPath );
        pItem->pidlFull.Concatenate( pidlNode );
        pItem->pidlNode.Copy( pidlNode );
        pItem->spFolder = pFolder;
        pItem->dwAttribs = dwAttribs;

        SHFILEINFO sfi;
        if( ::SHGetFileInfo((LPCTSTR)(LPCITEMIDLIST)pItem->pidlFull, 0, &sfi, sizeof(sfi),
            SHGFI_PIDL | SHGFI_DISPLAYNAME | SHGFI_SYSICONINDEX 
            | SHGFI_SMALLICON | SHGFI_LINKOVERLAY) ) 
        {
           return AppendItem(tiParent, sfi.szDisplayName, sfi.iIcon, pItem);
        }
        return NULL;
    }

    EShellTreeItem AppendItem( EShellTreeItem& tiParent, LPCWSTR szItem,
        int iImageIdx, PSHELLITEMINFO pItem)
    {
        CStdString sHtml;
        // ������Ǹ�<option>��һ��ռλ����Ŀ
        sHtml.Format(L"<img icon-idx=%d />%s<option/>", iImageIdx, szItem);
        EShellTreeItem tItem = ETree::create("option");
        if (!tiParent.is_valid())
            append(tItem);
        else
            tiParent.append(tItem);
        // ֱ���滻�����е�html
        tItem.SetHtml(sHtml);
        if (tItem.is_valid())
        {
            // img��Ϊinner_html��set_html����������
            // �����Զ�attach��behavior:system_shellicon;
            // �˴��ֶ��������д�behavior
            ECtrl(tItem.find_first("img"))
                .attach((htmlayout::event_handler*)&system_shellicon_instance);
            tItem.SetData(pItem);
            tItem.set_attribute("not-expand-ever",L"true");
        }
        return tItem;
    }

    // NOTE: Sorting added by Anatoly Ivasyuk.
    static int CALLBACK _SortFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
    {
        IShellFolder* piSF = reinterpret_cast<IShellFolder*>(lParamSort);
        PSHELLITEMINFO pItem1 = reinterpret_cast<PSHELLITEMINFO>(lParam1);
        PSHELLITEMINFO pItem2 = reinterpret_cast<PSHELLITEMINFO>(lParam2);
        HRESULT Hr = piSF->CompareIDs(0, pItem1->pidlNode, pItem2->pidlNode);
        if( SUCCEEDED(Hr) ) return (SHORT) (Hr & SHCIDS_COLUMNMASK);
        return 0;
    }

    void OnDeleteItem(EShellTreeItem &tItem)
    {
        PSHELLITEMINFO pItem = reinterpret_cast<PSHELLITEMINFO>(tItem.GetData());
        //ATLASSERT(pItem);
        ATLTRY(delete pItem);
    }

    void OnItemExpanding(EShellTreeItem& tItem)
    {
        //LPNMTREEVIEW pnmtv = (LPNMTREEVIEW) pnmh;

#ifdef __ATLCTRLX_H__
        CWaitCursor cursor;
#endif

        // չ������ȥ����ûչ��������
        tItem.remove_attribute("not-expand-ever");
        // ɾ��ռλ���Ǹ���Ŀ
        EShellTreeItem tiFake = tItem.find_first("option");
        if (tiFake.is_valid())
        {
            tiFake.destroy();
        }

        // ����Ŀ¼�µ����ļ���
        PSHELLITEMINFO pFolderItem = reinterpret_cast<PSHELLITEMINFO>(tItem.GetData());
        CComPtr<IShellFolder> spFolder;
        if( pFolderItem->pidlNode != NULL ) 
        {
            if( FAILED(pFolderItem->spFolder->BindToObject(pFolderItem->pidlNode,
                NULL, IID_IShellFolder, (LPVOID*)&spFolder)) ) 
                return ;
        }
        else 
        {
            spFolder = pFolderItem->spFolder;
        }

        // Add children
        CComPtr<IEnumIDList> spEnum;
        DWORD dwEnumFlags = SHCONTF_FOLDERS;
        if( (m_dwShellStyle & SCT_EX_SHOWHIDDEN) != 0 ) 
            dwEnumFlags |= SHCONTF_INCLUDEHIDDEN;
        if( SUCCEEDED(spFolder->EnumObjects(NULL, dwEnumFlags, &spEnum)) ) 
        {
            CPidl pidl;
            DWORD  dwFetched;
            while( (spEnum->Next(1, &pidl, &dwFetched) == S_OK) && (dwFetched > 0) ) 
            {
                // Get attributes and filter some items
                DWORD dwAttribs = SFGAO_DISPLAYATTRMASK | SFGAO_HASSUBFOLDER;
                if( !_FilterItem(spFolder, pidl, dwAttribs) ) 
                {
                    _InsertItem(spFolder, pFolderItem->pidlFull, pidl, dwAttribs, tItem);
                }
                pidl.Delete();
            }
        }
    }
   
};

// instantiating and attaching it to the global list
CHLShellTree CHLShellTree_instance;

namespace dom
{
    BOOL EShellTree::SetRoot(__in LPCWSTR pszRootPath = NULL)
    {
        DeleteAllChild();

        set_attribute("src",pszRootPath);

        attach(&CHLShellTree_instance);

        return TRUE;
    }

    BOOL EShellTree::SetRoot(__in int nClsID)
    {
        DeleteAllChild();

        wchar_t szBuf[10] = {0};
        swprintf_s(szBuf, 10, L"clsid:%d", nClsID);
        set_attribute("src", szBuf);

        attach(&CHLShellTree_instance);

        return TRUE;
    }
}

}
