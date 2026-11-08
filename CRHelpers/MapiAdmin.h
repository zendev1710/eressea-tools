// MapiAdmin.h: interface for the CMapiAdmin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPIADMIN_H__A452E001_CA74_11D3_8040_00805FC1DE10__INCLUDED_)
#define AFX_MAPIADMIN_H__A452E001_CA74_11D3_8040_00805FC1DE10__INCLUDED_

#pragma comment ( lib, "mapi32" )

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMapiAdmin  
{
public:
	bool IsLoggedOn();
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTION / DESTRUCTION
	CMapiAdmin(bool bLogon, 
						ULONG ulUIParam=NULL, 
						LPTSTR lpszProfileName=NULL, 
						LPTSTR lpszPassword=NULL, 
						FLAGS flFlags=NULL);

	CMapiAdmin();
	virtual ~CMapiAdmin();
	//////////////////////////////////////////////////////////////////////////////////////////////

	bool SendMessage();
	bool GetStorePath(CString strStoreID, CString& strStorePath);
	bool RemoveService(BYTE* pbyEntryID, DWORD dwSize);
	bool RemoveService(LPTSTR lpszDisplayName);
	
	bool GetProfileName(LPTSTR& lpszProfileName);
	
	bool CreateMessage(	 DWORD dwRecipCount, 
									LPCTSTR* ppRecipents, 
									LPCTSTR pMsgSubject, 
									LPCTSTR pMsgText, 
									BOOL bImportant);

	bool CreateNewProfile( LPTSTR lpszNewName);
	bool CreateMsgService( LPTSTR lpszService, LPTSTR lpszDisplayName );
	bool Logon(	  ULONG ulUIParam=NULL, 
						LPTSTR lpszProfileName=NULL, 
						LPTSTR lpszPassword=NULL, 
						FLAGS flFlags=NULL
					);

protected:
	bool GetPointers();
	void TestTag(LPSPropValue lpProp);
	CString ByteArrayToString(BYTE* pData, int nSize);
	CString ByteArrayToString(LPSPropValue lpProperty);
	int QueryRows( LPMAPITABLE pTable, SRestriction* pRestriction, LPSRowSet& lpRows /* out */, int nMin=0, int nMax=9999);

	bool m_bInitialised;
	LPMAPIFOLDER		m_pOutBoxFolder;
	LPMDB					m_pMessageStore;
	LPMAPITABLE			m_pStatusTable;
	LPMAPITABLE			m_pMsgStoresTable;
	LPMAPITABLE			m_pMsgServiceTable;
	LPPROFADMIN         m_pProfAdmin;
	IMsgServiceAdmin* m_pServiceAdmin;
	IMAPISession*		m_pSession;
	LPMESSAGE			m_pMessage;
	LPADRBOOK			m_pAddressBook;

	SRestriction		 m_sRestrict;    
	SPropValue		   m_sProps;

private:
	HRESULT hr;
};

#endif // !defined(AFX_MAPIADMIN_H__A452E001_CA74_11D3_8040_00805FC1DE10__INCLUDED_)
