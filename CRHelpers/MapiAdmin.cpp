// MapiAdmin.cpp: implementation of the CMapiAdmin class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MapiAdmin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
CMapiAdmin::CMapiAdmin(bool bLogon, ULONG ulUIParam, LPTSTR lpszProfileName, LPTSTR lpszPassword, FLAGS flFlags)	:
							m_pProfAdmin(NULL),				m_pServiceAdmin(NULL),		m_pMsgServiceTable(NULL),	
							m_pMsgStoresTable(NULL),	m_pSession(NULL),				m_pMessageStore(NULL),
							m_pOutBoxFolder(NULL),		m_pMessage(NULL),				m_pAddressBook(NULL)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	m_bInitialised = false;

	if (bLogon)
		Logon(ulUIParam, lpszProfileName, lpszPassword, flFlags);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
CMapiAdmin::CMapiAdmin()	:
							m_pProfAdmin(NULL),				m_pServiceAdmin(NULL),		m_pMsgServiceTable(NULL),	
							m_pMsgStoresTable(NULL),	m_pSession(NULL),				m_pMessageStore(NULL),
							m_pOutBoxFolder(NULL),		m_pMessage(NULL),				m_pAddressBook(NULL)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	m_bInitialised = false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
CMapiAdmin::~CMapiAdmin()
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	if (m_pProfAdmin)
		m_pProfAdmin->Release();
	
	if (m_pServiceAdmin)
		m_pServiceAdmin->Release();
	
	if (m_pMsgServiceTable)
		m_pMsgServiceTable->Release();
	
	if (m_pMsgStoresTable)
		m_pMsgStoresTable->Release();
	
	if (m_pSession)
		m_pSession->Release();

	if (m_pMessageStore)
		m_pMessageStore->Release();

	if (m_pOutBoxFolder)
		m_pOutBoxFolder->Release();

	if (m_pMessage)
		m_pMessage->Release();

	if (m_pAddressBook)
		m_pAddressBook->Release();

	MAPIUninitialize();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
CString CMapiAdmin::ByteArrayToString(LPSPropValue lpProperty)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	int nSize = lpProperty->Value.bin.cb;
	CString strTmp, strByteArray("");

	for ( int nCount=0; nCount<nSize; nCount++)
	{
		strTmp.Format("%.2x", lpProperty->Value.bin.lpb[nCount]);
		TRACE("\t %x", lpProperty->Value.bin.lpb[nCount]);
		strByteArray += strTmp;
	}

	TRACE("\n");
	return strByteArray;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
CString CMapiAdmin::ByteArrayToString(BYTE* pData, int nSize)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	CString strTmp, strByteArray("");

	for ( int nCount=0; nCount<nSize; nCount++)
	{
		strTmp.Format("%.2x", pData[nCount]);
		TRACE("\t %x", pData[nCount]);
		
		strByteArray += strTmp;
	}

	TRACE("\n");
	return strByteArray;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
bool CMapiAdmin::Logon(	  ULONG ulUIParam, 
										LPTSTR lpszProfileName, 
										LPTSTR lpszPassword, 
										FLAGS flFlags
									)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Logon by piggybacking the current session in use and then furnish member vars with juicy values ( see GetPointers() )
{
	m_bInitialised = false;

	if (FAILED(  MAPIInitialize(NULL)  ))	return false;
	if (FAILED(  MAPILogonEx(ulUIParam, lpszProfileName, lpszPassword, flFlags, &m_pSession)  ))	return false;

	return GetPointers();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
bool CMapiAdmin::GetPointers()
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	// Pointer to ProfileAdministrator:

	if (FAILED(  MAPIAdminProfiles(0, &m_pProfAdmin)  ))		 return false;

	// Pointer to the MessageStore:

	if (FAILED(  m_pSession->GetMsgStoresTable(0, &m_pMsgStoresTable)  ))		 return false;

	// Pointer to IMsgServiceAdmin:

	if (FAILED(  m_pSession->AdminServices(0, &m_pServiceAdmin)  ))		 return false;

	// Pointer to the StatusTable:

	if (FAILED(  m_pSession->GetStatusTable(0, &m_pStatusTable)  ))		return false;

	// Pointer to a MsgStoresTable:

	if (FAILED(  m_pSession->GetMsgStoresTable(0, &m_pMsgStoresTable)  ))	return false;

	// Pointer to a MsgServiceTable:

	if (FAILED(  m_pServiceAdmin->GetMsgServiceTable(0, &m_pMsgServiceTable)  ))	return false;

	return (m_bInitialised = true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
bool CMapiAdmin::CreateMsgService(LPTSTR lpszService, LPTSTR lpszDisplayName )
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	ASSERT(m_bInitialised);

	if (FAILED(  m_pServiceAdmin->CreateMsgService(lpszService, lpszDisplayName, 0, 0)  ))	return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
int CMapiAdmin::QueryRows(LPMAPITABLE pTable, SRestriction* pRestriction, LPSRowSet& lpRows /* out */, int nMin, int nMax)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Useful function that :
//	1. Performs the restriction
//	2. Whittles the table down to the number of rows that satisfy the restriction
//	3. Returns true if the row count is as expected
{
	if (FAILED(  pTable->Restrict(pRestriction /* NULL resets */, TBL_ASYNC)  ))		return false;

	hr = pTable->SeekRow(BOOKMARK_BEGINNING,0,NULL);
	hr = pTable->QueryRows(4000, 0, &lpRows); // Max 4000 rows.
	if (HR_FAILED(hr))	return false;

	if( (lpRows->cRows < (ULONG)nMin)	||	(lpRows->cRows > (ULONG)nMax) )	
		return -1;
	else
		return lpRows->cRows;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
bool CMapiAdmin::CreateNewProfile(LPTSTR lpszNewName)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	ASSERT(m_bInitialised);

	if (FAILED(  m_pProfAdmin->CreateProfile(lpszNewName, NULL, 0, 0)  ))		
		return false;
	else
		return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
bool CMapiAdmin::CreateMessage(DWORD dwRecipientCount, LPCTSTR *ppRecipents, LPCTSTR pMessageSubject, LPCTSTR pMessageText, BOOL bHighImportance)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	ASSERT(m_bInitialised);

	SizedSPropTagArray(3, Columns) = {3,{PR_DEFAULT_STORE,PR_DISPLAY_NAME,PR_ENTRYID}};

    if(FAILED(  m_pMsgStoresTable->SetColumns((LPSPropTagArray)&Columns, 0)  ))		return false;
	
	LPSRowSet lpRows = NULL;

	while (TRUE)
	{
		LPSPropValue	lpProp;

		hr = m_pMsgStoresTable->QueryRows(1, 0, &lpRows);

		if (FAILED(hr) || lpRows->cRows != 1)
			break;

		lpProp = &lpRows->aRow[0].lpProps[0];

		if (lpProp->ulPropTag == PR_DEFAULT_STORE && lpProp->Value.b)
			break;
	}

	if (FAILED(hr))		return false;

	if (lpRows == NULL												||
		lpRows->cRows != 1											||
		lpRows->aRow[0].lpProps[1].ulPropTag != PR_DISPLAY_NAME		||
		lpRows->aRow[0].lpProps[1].Value.lpszA == NULL)
	{
		return false;
	}

	hr = m_pSession->OpenMsgStore(	0,
									lpRows->aRow[0].lpProps[2].Value.bin.cb,
									(ENTRYID*)lpRows->aRow[0].lpProps[2].Value.bin.lpb,
									NULL,
									MDB_NO_DIALOG | MAPI_BEST_ACCESS,
									&m_pMessageStore);

	if (FAILED(hr))		return false;
	
	SizedSPropTagArray(1, OutBox) = {1,{PR_IPM_OUTBOX_ENTRYID}};

	DWORD			cValues;
	LPSPropValue	lpOBProp;
	DWORD			dwObjType;

	hr = m_pMessageStore->GetProps((LPSPropTagArray)&OutBox,
									0,
									&cValues,
									&lpOBProp);

	if (FAILED(hr))		return false;
	
 
	hr = m_pMessageStore->OpenEntry(	lpOBProp->Value.bin.cb,
										(ENTRYID*)lpOBProp->Value.bin.lpb,
										NULL,
										MAPI_BEST_ACCESS,
										&dwObjType,
										(LPUNKNOWN*)&m_pOutBoxFolder);
									
	if (FAILED(hr))		return false;
	 
	hr = m_pOutBoxFolder->CreateMessage(NULL, 0, &m_pMessage);

	if (FAILED(hr))		return false;

    hr = m_pSession->OpenAddressBook(0, NULL, AB_NO_DIALOG, &m_pAddressBook);

	if (FAILED(hr))		return false;

	LPADRLIST		pal			= NULL;
	LPSPropValue	rgAdrProps	= NULL;
	INT				nBufSize	= CbNewADRLIST(dwRecipientCount);

	hr = MAPIAllocateBuffer(nBufSize, (LPVOID FAR *)&pal);

	if (FAILED(hr))		return false;

    ZeroMemory(pal, nBufSize);
	
	pal->cEntries = dwRecipientCount;

	for (DWORD i = 0; i < dwRecipientCount; i++)
	{
		pal->aEntries[i].ulReserved1	= 0;
		pal->aEntries[i].cValues		= 2;

		hr = MAPIAllocateBuffer(2 * sizeof(SPropValue), (LPVOID*)&(pal->aEntries[i].rgPropVals));
		
		if (FAILED(hr))		return false;

		pal->aEntries[i].rgPropVals[0].ulPropTag	= PR_DISPLAY_NAME;
		pal->aEntries[i].rgPropVals[0].Value.LPSZ	= const_cast<LPSTR>(ppRecipents[i]);
		pal->aEntries[i].rgPropVals[1].ulPropTag	= PR_RECIPIENT_TYPE;
		pal->aEntries[i].rgPropVals[1].Value.l		= MAPI_TO;
	
	}

	hr = m_pAddressBook->ResolveName(0, 0, NULL, pal);

	if (FAILED(hr))		return false;

	hr = m_pMessage->ModifyRecipients(MODRECIP_ADD, pal);

	if (FAILED(hr))		return false;

    SPropValue prop = {0};
	
	prop.ulPropTag		= PR_BODY;
	prop.Value.lpszA	= const_cast<LPSTR>(pMessageText);
	
	hr = m_pMessage->SetProps(1, &prop, NULL);

	if (FAILED(hr))		return false;

	prop.ulPropTag	= PR_SUBJECT;
	prop.Value.LPSZ	= const_cast<LPSTR>(pMessageSubject);

	hr = m_pMessage->SetProps(1, &prop, NULL);

	if (FAILED(hr))		return false;

	if (bHighImportance)
	{
		prop.ulPropTag	= PR_IMPORTANCE;
		prop.Value.l	= IMPORTANCE_HIGH;
	
		hr = m_pMessage->SetProps(1, &prop, NULL);

		if (FAILED(hr))		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
bool CMapiAdmin::GetProfileName(LPTSTR& lpszProfileName)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	ASSERT(m_bInitialised);

	// Restrict the columns to just PR_DISPLAY_NAME & PR_RESOURCE_TYPE
	static SizedSPropTagArray(2, Columns) = {2, {PR_DISPLAY_NAME, PR_RESOURCE_TYPE}};
	if (FAILED(  m_pStatusTable->SetColumns((LPSPropTagArray)&Columns, 0)  ))	return false;

	m_sProps.ulPropTag =  PR_RESOURCE_TYPE;
	m_sProps.Value.ul =		MAPI_SUBSYSTEM;

	m_sRestrict.rt = RES_PROPERTY;
	m_sRestrict.res.resProperty.relop = RELOP_EQ;
	m_sRestrict.res.resProperty.ulPropTag = PR_RESOURCE_TYPE;
	m_sRestrict.res.resProperty.lpProp = &m_sProps;

	LPSRowSet    lpRows = NULL;
	if ( QueryRows(m_pStatusTable, &m_sRestrict, lpRows) == 0)	return false;

	// We've found the row:

	lpszProfileName = lpRows->aRow[0].lpProps[0].Value.lpszA;
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
bool CMapiAdmin::RemoveService(LPTSTR lpszDisplayName)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WARNING!! This will delete ALL occurences found (DisplayNames don't have to be unique)
{
	ASSERT(m_bInitialised);

	// Restrict the columns to just PR_DISPLAY_NAME & PR_SERVICE_UID
	static SizedSPropTagArray(2, Columns) = {2, {PR_DISPLAY_NAME, PR_SERVICE_UID}};

	if (FAILED(  m_pMsgServiceTable->SetColumns((LPSPropTagArray)&Columns, 0)  ))	return false;

	m_sRestrict.rt = RES_PROPERTY;
	m_sRestrict.res.resProperty.relop = RELOP_EQ;
	m_sRestrict.res.resProperty.ulPropTag = PR_DISPLAY_NAME;
	m_sRestrict.res.resProperty.lpProp = &m_sProps;

	m_sProps.ulPropTag = PR_DISPLAY_NAME;
	m_sProps.Value.lpszA = lpszDisplayName;	//Name to find.
	
	LPSRowSet    lpRows = NULL;
	if ( QueryRows(m_pMsgServiceTable, &m_sRestrict, lpRows) <= 0)	return false;

	// We've found the Service(s). Go get 'em. We know that the 2nd row has the Service UID.
	bool bReturn=true;
	for ( unsigned int uCount=0; uCount<lpRows->cRows; uCount++)
	{
		LPSPropValue lpProp = &lpRows->aRow[uCount].lpProps[1];
		
		// Perform the deletion:
		if (FAILED(  m_pServiceAdmin->DeleteMsgService( (LPMAPIUID)lpProp->Value.bin.lpb )  ))	
			bReturn = false;
	}

	return bReturn;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
bool CMapiAdmin::RemoveService(BYTE *pbyEntryID, DWORD dwSize)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WARNING!! This will delete ALL occurences found (EntryIDs don't have to be unique)
{
	ASSERT(m_bInitialised);

	static SizedSPropTagArray(3, Columns) = {3,	{ PR_DISPLAY_NAME, PR_ENTRYID, PR_INSTANCE_KEY } };

	if (FAILED(  m_pMsgStoresTable->SetColumns((LPSPropTagArray)&Columns, 0)  ))	return false;

	m_sRestrict.rt = RES_PROPERTY;
	m_sRestrict.res.resProperty.relop = RELOP_EQ;
	m_sRestrict.res.resProperty.ulPropTag = PR_ENTRYID;
	m_sRestrict.res.resProperty.lpProp = &m_sProps;

	m_sProps.ulPropTag = PR_ENTRYID;
	m_sProps.Value.bin.cb = dwSize/2;
	m_sProps.Value.bin.lpb = pbyEntryID;
	
	if (FAILED(  m_pMsgStoresTable->Restrict(&m_sRestrict, TBL_ASYNC)  ))		return false;

	//	Call the message service table's IMAPITable::QueryRows method to locate the desired row 

	LPSRowSet    lpRows = NULL;
	m_pMsgStoresTable->SeekRow(BOOKMARK_BEGINNING,0,NULL);
	m_pMsgStoresTable->QueryRows(4000, 0, &lpRows); // Max 4000 rows.

	int nRows = (int)lpRows->cRows;

	if( nRows == 0 )	return false;


	for ( int nCount=0; nCount< nRows; nCount++)
	{
		// If we got this far, we have found at least one row: Get the INSTANCE KEY :
		LPSPropValue lpProp = &lpRows->aRow[nCount].lpProps[2];

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Read the ServiceUID from the registry:
		
			CRegistryKey regkey;
			CString strKeyName;
			CString strInstanceKey = ByteArrayToString( lpProp );

			LPTSTR lpszProfileName = "";
			if ( ! GetProfileName(lpszProfileName) ) return false;

			strKeyName.Format("Software\\Microsoft\\Windows NT\\CurrentVersion\\Windows Messaging Subsystem\\Profiles\\%s\\%s", 
												lpszProfileName, 
												strInstanceKey
											);

			if ( regkey.Open( HKEY_CURRENT_USER, strKeyName) != ERROR_SUCCESS )
				return false;
			
			// Get the value of the service table's INSTANCE_KEY:
			CString strKey("01023d0c");

			DWORD dwSvcUIDSize=16;
			BYTE* pbyServiceUID = new BYTE[dwSvcUIDSize];
			LONG lReturn = regkey.QueryValue(pbyServiceUID, (LPCTSTR)strKey, &dwSvcUIDSize);
			ASSERT(  lReturn == ERROR_SUCCESS );

		// End: Read the ServiceUID from the registry
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Got the ServiceUID from the registry - find the corresponding row from the MsgServiceTable and delete it:

			static SizedSPropTagArray(3, TableColumns) = {3,	{ PR_DISPLAY_NAME, PR_INSTANCE_KEY, PR_SERVICE_UID }};
			if (FAILED(  m_pMsgServiceTable->SetColumns((LPSPropTagArray)&TableColumns, 0)  ))	return false;

			// Perform appropriate restrictions:
			{
				m_sRestrict.rt = RES_PROPERTY;
				m_sRestrict.res.resProperty.relop = RELOP_EQ;
				m_sRestrict.res.resProperty.ulPropTag = PR_SERVICE_UID;
				m_sRestrict.res.resProperty.lpProp = &m_sProps;

				m_sProps.ulPropTag = PR_SERVICE_UID;
				m_sProps.Value.bin.cb = 16;
				m_sProps.Value.bin.lpb = pbyServiceUID;
				
				if (FAILED(  m_pMsgServiceTable->Restrict(&m_sRestrict, TBL_ASYNC)  ))						return false;
 			}

			LPSRowSet    lpServiceTableRows = NULL;
			m_pMsgServiceTable->SeekRow(BOOKMARK_BEGINNING,0,NULL);
			m_pMsgServiceTable->QueryRows(4000, 0, &lpServiceTableRows); // Max 4000 rows.
			
			if( lpServiceTableRows->cRows != 1 )
				return false;

			// The 3rd property has the Service UID
			lpProp = &lpServiceTableRows->aRow[0].lpProps[2];

			if ( FAILED(  m_pServiceAdmin->DeleteMsgService((LPMAPIUID)lpProp->Value.bin.lpb)  ) )	
				return false;
		
		// END: Got the ServiceUID from the registry - find the corresponding row from the MsgServiceTable and delete it:
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
bool CMapiAdmin::GetStorePath(CString strStoreID, CString& strStorePath)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	ASSERT(m_bInitialised);

	static SizedSPropTagArray(3, Columns) = {3,	{ PR_DISPLAY_NAME, PR_ENTRYID, PR_INSTANCE_KEY } };
	if (FAILED(  m_pMsgStoresTable->SetColumns((LPSPropTagArray)&Columns, 0)  ))	return false;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// CONVERT THE STRING TO A BYTE ARRAY:

		int nSize = strStoreID.GetLength();
		BYTE byData;
		BYTE* pByte = new BYTE[nSize/2];

		for ( int nCount=0; nCount<nSize; nCount+=2)
		{
			CString strTwoChars = strStoreID.Mid(nCount,2);
			LPTSTR lpszChar = strTwoChars.GetBuffer(2);
			sscanf_s( lpszChar, "%x", &byData );	//TRACE("\t pByte[%.2d] : %s=%d\n", ((nCount+2)/2)-1, lpszChar, byData);
			pByte[ ((nCount+2)/2)-1 ] = byData;
		}

	// END STRING TO BYTE ARRAY CONVERSION
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	static SRestriction		sres;
	SPropValue				spv;

	sres.rt = RES_PROPERTY;
	sres.res.resProperty.relop = RELOP_EQ;
	sres.res.resProperty.ulPropTag = PR_ENTRYID;
	sres.res.resProperty.lpProp = &spv;

	spv.ulPropTag = PR_ENTRYID;
	spv.Value.bin.cb = nSize/2;
	spv.Value.bin.lpb = pByte;
	
	if (FAILED(  m_pMsgStoresTable->Restrict(&sres, TBL_ASYNC)  ))		return false;

	//	Call the message service table's IMAPITable::QueryRows method to locate the desired row 

	LPSRowSet    lpRows = NULL;
	m_pMsgStoresTable->SeekRow(BOOKMARK_BEGINNING,0,NULL);
	m_pMsgStoresTable->QueryRows(4000, 0, &lpRows); // Max 4000 rows.
	
	delete pByte;

	if( lpRows->cRows != 1 )
		return false;

	// If we got this far, we only have one row: Get the INSTANCE KEY from this row

	LPSPropValue lpPropTest = &lpRows->aRow[0].lpProps[2];

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Read the StoragePath from the registry:
	
		CRegistryKey regkey;
		CString strKeyName;
		CString strInstanceKey = ByteArrayToString(lpPropTest);

		LPTSTR lpszProfileName = "";
		if ( ! GetProfileName(lpszProfileName) ) return false;

		strKeyName.Format("Software\\Microsoft\\Windows NT\\CurrentVersion\\Windows Messaging Subsystem\\Profiles\\%s\\%s", 
											lpszProfileName, 
											strInstanceKey
										);

		if ( regkey.Open( HKEY_CURRENT_USER, strKeyName) != ERROR_SUCCESS )
			return false;
		
		// Get the value of the service table's INSTANCE_KEY:
		CString strKey("001E6700");

		LONG lReturn = regkey.QueryValue( strStorePath, (LPCTSTR)strKey );
		ASSERT(  lReturn == ERROR_SUCCESS );

	// End: Read the StoragePath from the registry
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
void CMapiAdmin::TestTag(LPSPropValue lpProp)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	switch (lpProp->ulPropTag)
	{
	case PR_DISPLAY_NAME:
		TRACE("---->Found: PR_DISPLAY_NAME");
		TRACE("\t(\"%s\")\n", lpProp->Value.lpszA );
		break;

	case PR_MDB_PROVIDER:
		TRACE("---->Found: PR_MDB_PROVIDER");
		TRACE("\t%d bytes ", lpProp->Value.bin.cb);
		ByteArrayToString(lpProp);
		break;
	
	case PR_RESOURCE_FLAGS:
		TRACE("---->Found: PR_RESOURCE_FLAGS\n");
		break;
	
	case PR_OBJECT_TYPE:
		TRACE("---->Found: PR_OBJECT_TYPE\n");
		break;
	
	case PR_RESOURCE_TYPE:
		TRACE("---->Found: PR_RESOURCE_TYPE\n");
		break;
	
	case PR_DEFAULT_STORE:
		TRACE("---->Found: PR_DEFAULT_STORE\n");
		break;
	
	case PR_RECORD_KEY:
		TRACE("---->Found: PR_RECORD_KEY");
		TRACE("\t%d bytes ", lpProp->Value.bin.cb);
		ByteArrayToString(lpProp);
		break;

	case PR_ENTRYID:
		TRACE("---->Found: PR_ENTRYID");
		TRACE("\t%d bytes ", lpProp->Value.bin.cb);
		ByteArrayToString(lpProp);
		break;

	case PR_PROVIDER_DISPLAY:
		TRACE("---->Found: PR_PROVIDER_DISPLAY\n");
		break;
	
	case PR_SERVICE_UID:
		TRACE("---->Found: PR_SERVICE_UID");
		TRACE("\t%d bytes ", lpProp->Value.bin.cb);
		ByteArrayToString(lpProp);
		break;

	case PR_INSTANCE_KEY:
		TRACE("---->Found: PR_INSTANCE_KEY");
		TRACE("\t%d bytes ", lpProp->Value.bin.cb);
		ByteArrayToString(lpProp);
		break;

	default:
		TRACE("---->** Tag not recogized **\n");
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
bool CMapiAdmin::SendMessage()
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	ASSERT(m_bInitialised);

	if (FAILED(  m_pMessage->SubmitMessage(0)  ))
		return false;
	else
		return true;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Function Header
bool CMapiAdmin::IsLoggedOn()
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
	return m_bInitialised;
}
