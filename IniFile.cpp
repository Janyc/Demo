// IniFile.cpp: implementation of the CIniFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IniFile.h"
#include <stdio.h>

// #ifdef _DEBUG
// #undef THIS_FILE
// static char THIS_FILE[]=__FILE__;
// #define new DEBUG_NEW
// #endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIniFile::CIniFile(const char* _szFileName)
{	
    char szDir[500] = {0};
	::GetCurrentDirectory(500, szDir);

	m_szFileName = new char[strlen(szDir)+strlen(_szFileName)+1+1];
	strcpy(m_szFileName, szDir);
	strcat(m_szFileName, "\\");//+1
	strcat(m_szFileName, _szFileName);
}

CIniFile::~CIniFile()
{
	delete[] m_szFileName;
}


int CIniFile::GetProfileString(const char* _szSection, const char* _szKey, char* _szResult, int _nLen)
{
	return	GetPrivateProfileString(_szSection, _szKey, "", _szResult, _nLen, m_szFileName);
}

int CIniFile::GetProfileInt(const char* _szSection, const char* _szKey)
{
	return GetPrivateProfileInt(_szSection, _szKey, 0, m_szFileName);
}

int CIniFile::WriteProfileString(const char* _szSection, const char* _szKey, const char* _szText)
{
	return WritePrivateProfileString(_szSection, _szKey, _szText, m_szFileName);
}

int CIniFile::WriteProfileInt(const char* _szSection, const char*_szKey, int _iValue)
{
	char v_sztemp[STR_VALUE_LEN] = {0};
	sprintf(v_sztemp, "%d",_iValue);
	return WritePrivateProfileString(_szSection,_szKey,v_sztemp,m_szFileName);
}

