// CREresseaFile.h: interface for the CCREresseaFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRERESSEAFILE_H__AB50ADC6_55CB_4A56_83BA_E4C38B1A5A8B__INCLUDED_)
#define AFX_CRERESSEAFILE_H__AB50ADC6_55CB_4A56_83BA_E4C38B1A5A8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCREresseaFile  
{
public:
	CCREresseaFile():m_bEOF(FALSE),m_nNbUnits(0) {setFilename("");}
	CCREresseaFile(const char *pFilename):m_bEOF(FALSE),m_nNbUnits(0) { setFilename(pFilename);}

	/////////

	virtual ~CCREresseaFile() {}

	virtual BOOL open() = 0;
	virtual BOOL process() = 0;

	virtual BOOL nextEntry() = 0;

	virtual BOOL checkHeader() { return TRUE;}
	virtual BOOL toSave() const { return FALSE; }

	/////////////

	const char * filename() const { return m_sFilename;}
	BOOL eof() const { return m_bEOF;}

	const char * buffer() const { return m_sBuffer;}
	int nbUnits() const { return m_nNbUnits;}

protected:

	void setFilename(const char *pFilename) { m_sFilename = pFilename; }
	void setEOF() {m_bEOF = TRUE;}

	const char * setBuffer(const char *s) { m_sBuffer = s;	return (LPCSTR) m_sBuffer;}
	void incNbUnits() { m_nNbUnits++; }

private:

	CString m_sFilename;
	CString m_sBuffer;
	BOOL m_bEOF;
	int m_nNbUnits;
};

#endif // !defined(AFX_CRERESSEAFILE_H__AB50ADC6_55CB_4A56_83BA_E4C38B1A5A8B__INCLUDED_)
