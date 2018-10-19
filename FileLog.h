#pragma once
#include "PublicDef.h"

#include <strstream>
#include <fstream>

class CFileLog
{
public:
	CFileLog();
	~CFileLog();

public:
	BOOL Init();
	void Release();
	void WriteFileLog( int v_iLogType, SYSTEMTIME &v_SysTime, const char v_szLog[], int v_iLogLen );
	BOOL CreateNewFiles( SYSTEMTIME &v_SysTime );

private:
	CRITICAL_SECTION m_csCS;

	//����ΪC++ ofstream�����ļ������ʽ�������ǰ��־�ļ��޷�������
	//jiangyongcong 20131213
	std::ofstream m_foutCommFile;
	std::ofstream m_foutWarnFile;
	std::ofstream m_foutErrFile;
	std::ofstream m_foutNetFile;

	FILE* m_pModInfoFile;//����C�ļ���ʽ

	SYSTEMTIME m_SysTime;
	BOOL  m_bExit;

private:
	std::string m_strModuleDir;
};