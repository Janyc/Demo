#include "stdafx.h"
#include "FileLog.h"

CFileLog::CFileLog()
{
	m_pModInfoFile = NULL;
	m_bExit = TRUE;

	::InitializeCriticalSection(&m_csCS);
}

CFileLog::~CFileLog()
{
	::DeleteCriticalSection(&m_csCS);
}

BOOL CFileLog::Init()
{
	char   szModuleDir[MAX_PATH];
	char   szDriver[MAX_PATH];
	char   szDir[MAX_PATH];
	char   szFName[MAX_PATH];
	char   szExt[MAX_PATH];

	::GetModuleFileName( NULL, szModuleDir, sizeof(szModuleDir) );
	::_splitpath_s( szModuleDir,szDriver,sizeof(szDriver),szDir,sizeof(szDir),szFName,sizeof(szFName),szExt,sizeof(szExt));
	::_makepath_s( szModuleDir,sizeof(szModuleDir), szDriver, szDir, NULL, NULL );

	m_strModuleDir = szModuleDir;
	m_strModuleDir += "TS_LOG\\";

	if( _access(m_strModuleDir.c_str(),0) != 0 )
	{// 目录已经存在
		if( 0 == ::CreateDirectory(m_strModuleDir.c_str(),0) )
		{
			char szLog[MAX_PATH*2] = {0};
			sprintf_s(szLog,sizeof(szLog),"\"%s\"文件路径创建失败！",m_strModuleDir.c_str());
			g_ShowLog( _enError, szLog );
			return FALSE;
		}
	}

	// 初始化目录和文件
	GetLocalTime(&m_SysTime);
	if( !CreateNewFiles(m_SysTime) )
	{
		return FALSE;
	}

	// 可以写入日志了
	m_bExit = FALSE;

	// 程序启动信息
	std::string strStart = "********************【日志启动】********************";
	WriteFileLog( _enCommon,m_SysTime,strStart.c_str(),(int)strStart.length() );
	WriteFileLog( _enWarning,m_SysTime,strStart.c_str(),(int)strStart.length() );
	WriteFileLog( _enError,m_SysTime,strStart.c_str(),(int)strStart.length() );
	WriteFileLog( _enNet,m_SysTime,strStart.c_str(),(int)strStart.length() );
	WriteFileLog( _enModInfo,m_SysTime,strStart.c_str(),(int)strStart.length() );

	return TRUE;
}

void CFileLog::Release()
{
	CAutoCS CS(&m_csCS);

	// 程序退出信息
	std::string strStart = "********************【日志关闭】********************";
	WriteFileLog( _enCommon,m_SysTime,strStart.c_str(),(int)strStart.length() );
	WriteFileLog( _enWarning,m_SysTime,strStart.c_str(),(int)strStart.length() );
	WriteFileLog( _enError,m_SysTime,strStart.c_str(),(int)strStart.length() );
	WriteFileLog( _enNet,m_SysTime,strStart.c_str(),(int)strStart.length() );
	WriteFileLog( _enModInfo,m_SysTime,strStart.c_str(),(int)strStart.length() );

	// 标识退出
	m_bExit = TRUE;

	m_foutCommFile.close();
	m_foutWarnFile.close();
	m_foutErrFile.close();
	m_foutNetFile.close();

	if( m_pModInfoFile != NULL )
	{
		::fclose(m_pModInfoFile);
		m_pModInfoFile = NULL;
	}
}

BOOL CFileLog::CreateNewFiles( SYSTEMTIME &v_SysTime )
{
	//重置当前时间 jiangyongcong 20131213
	m_SysTime=v_SysTime;

	char szTmp[MAX_PATH] = {0};
	sprintf_s(szTmp,sizeof(szTmp),"%04d-%02d-%02d\\",
		v_SysTime.wYear,v_SysTime.wMonth,v_SysTime.wDay);

	// 创建当天的文件夹
	std::string strFilePath = m_strModuleDir + szTmp;
	if( _access(strFilePath.c_str(),0) != 0 )
	{// 目录不存在
		if( 0 == ::CreateDirectory(strFilePath.c_str(),0) )
		{
			char szLog[MAX_PATH*2] = {0};
			sprintf_s(szLog,sizeof(szLog),"\"%s\"文件路径创建失败！",strFilePath.c_str());
			g_ShowLog( _enError, szLog );
			return FALSE;
		}
	}

	//更改为C++ ofstream操作文件输出方式，解决当前日志文件无法打开问题
	//jiangyongcong 20131213

	// 创建当前普通日志文件
	sprintf_s(szTmp,sizeof(szTmp),"%04d-%02d-%02d_%02d_Comm.log",
		v_SysTime.wYear,v_SysTime.wMonth,v_SysTime.wDay,v_SysTime.wHour);
	std::string strFile = strFilePath+szTmp;
	if (m_foutCommFile.is_open())
	{
		m_foutCommFile.close();
	}
	std::locale::global(std::locale(""));//将全局区域设为操作系统默认区域
	m_foutCommFile.open( strFile.c_str(), std::ios::out | std::ios::app );
	std::locale::global(std::locale("C"));//还原全局区域设定
	
	// 创建当前警告日志文件
	sprintf_s(szTmp,sizeof(szTmp),"%04d-%02d-%02d_%02d_Warning.log",
		v_SysTime.wYear,v_SysTime.wMonth,v_SysTime.wDay,v_SysTime.wHour);
	strFile = strFilePath+szTmp;
	if (m_foutWarnFile.is_open())
	{
		m_foutWarnFile.close();
	}
	std::locale::global(std::locale(""));//将全局区域设为操作系统默认区域
	m_foutWarnFile.open( strFile.c_str(), std::ios::out | std::ios::app );
	std::locale::global(std::locale("C"));//还原全局区域设定

	// 创建当前错误日志文件
	sprintf_s(szTmp,sizeof(szTmp),"%04d-%02d-%02d_%02d_Error.log",
		v_SysTime.wYear,v_SysTime.wMonth,v_SysTime.wDay,v_SysTime.wHour);
	strFile = strFilePath+szTmp;
	if (m_foutErrFile.is_open())
	{
		m_foutErrFile.close();
	}
	std::locale::global(std::locale(""));//将全局区域设为操作系统默认区域
	m_foutErrFile.open( strFile.c_str(), std::ios::out | std::ios::app );
	std::locale::global(std::locale("C"));//还原全局区域设定

	// 创建当前网络日志文件
	sprintf_s(szTmp,sizeof(szTmp),"%04d-%02d-%02d_%02d_Net.log",
		v_SysTime.wYear,v_SysTime.wMonth,v_SysTime.wDay,v_SysTime.wHour);
	strFile = strFilePath+szTmp;
	if (m_foutNetFile.is_open())
	{
		m_foutNetFile.close();
	}
	std::locale::global(std::locale(""));//将全局区域设为操作系统默认区域
	m_foutNetFile.open( strFile.c_str(), std::ios::out | std::ios::app );
	std::locale::global(std::locale("C"));//还原全局区域设定


	// 创建当前模块日志文件
	sprintf_s(szTmp,sizeof(szTmp),"%04d-%02d-%02d_%02d_ModInfo.log",
		v_SysTime.wYear,v_SysTime.wMonth,v_SysTime.wDay,v_SysTime.wHour);
	strFile = strFilePath+szTmp;
	if( m_pModInfoFile != NULL )
	{
		::fclose(m_pModInfoFile);
		m_pModInfoFile = NULL;
	}
	::fopen_s(&m_pModInfoFile,strFile.c_str(),"a+");
	if( m_pModInfoFile == NULL )
	{
		char szLog[MAX_PATH*2] = {0};
		sprintf_s(szLog,sizeof(szLog),"\"%s\"文件打开失败！",strFile.c_str());
		g_ShowLog( _enError, szLog );
		return FALSE;
	}
	
	return TRUE;
}

void CFileLog::WriteFileLog( int v_iLogType, SYSTEMTIME &v_SysTime, const char v_szLog[], int v_iLogLen )
{
	CAutoCS CS(&m_csCS);
	if( m_bExit )
	{
		return;
	}

	if( v_SysTime.wYear != m_SysTime.wYear 
		|| v_SysTime.wMonth != m_SysTime.wMonth
		|| v_SysTime.wDay != m_SysTime.wDay 
		|| v_SysTime.wHour != m_SysTime.wHour )
	{ // 需要创建新的文件
		if( !CreateNewFiles(v_SysTime) )
		{
			return;
		}
	}

	FILE *pFile = NULL;
	char szDataTime[MAX_PATH] = {0};
	sprintf_s(szDataTime,sizeof(szDataTime),"【%02d:%02d:%02d】", 
		v_SysTime.wHour,v_SysTime.wMinute,v_SysTime.wSecond);

	switch( v_iLogType )
	{
	case _enWarning:
		m_foutWarnFile << szDataTime << v_szLog << std::endl;
		break;
	case _enError:
		m_foutErrFile << szDataTime << v_szLog << std::endl;
		break;
	case _enNet:
		m_foutNetFile << szDataTime << v_szLog << std::endl;
		break;
	case _enModInfo: //保留了C文件方式
		pFile = m_pModInfoFile;
		break;
	default:
		m_foutCommFile << szDataTime << v_szLog << std::endl;
		break;
	}

	if( pFile != NULL )
	{
		fwrite(szDataTime,strlen(szDataTime),1,pFile);
		fwrite(v_szLog,v_iLogLen,1,pFile);
		fwrite("\n",1,1,pFile);
		fflush(pFile);
	}
}