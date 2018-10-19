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

	//更改为C++ ofstream操作文件输出方式，解决当前日志文件无法打开问题
	//jiangyongcong 20131213
	std::ofstream m_foutCommFile;
	std::ofstream m_foutWarnFile;
	std::ofstream m_foutErrFile;
	std::ofstream m_foutNetFile;

	FILE* m_pModInfoFile;//保留C文件方式

	SYSTEMTIME m_SysTime;
	BOOL  m_bExit;

private:
	std::string m_strModuleDir;
};