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
	{// Ŀ¼�Ѿ�����
		if( 0 == ::CreateDirectory(m_strModuleDir.c_str(),0) )
		{
			char szLog[MAX_PATH*2] = {0};
			sprintf_s(szLog,sizeof(szLog),"\"%s\"�ļ�·������ʧ�ܣ�",m_strModuleDir.c_str());
			g_ShowLog( _enError, szLog );
			return FALSE;
		}
	}

	// ��ʼ��Ŀ¼���ļ�
	GetLocalTime(&m_SysTime);
	if( !CreateNewFiles(m_SysTime) )
	{
		return FALSE;
	}

	// ����д����־��
	m_bExit = FALSE;

	// ����������Ϣ
	std::string strStart = "********************����־������********************";
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

	// �����˳���Ϣ
	std::string strStart = "********************����־�رա�********************";
	WriteFileLog( _enCommon,m_SysTime,strStart.c_str(),(int)strStart.length() );
	WriteFileLog( _enWarning,m_SysTime,strStart.c_str(),(int)strStart.length() );
	WriteFileLog( _enError,m_SysTime,strStart.c_str(),(int)strStart.length() );
	WriteFileLog( _enNet,m_SysTime,strStart.c_str(),(int)strStart.length() );
	WriteFileLog( _enModInfo,m_SysTime,strStart.c_str(),(int)strStart.length() );

	// ��ʶ�˳�
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
	//���õ�ǰʱ�� jiangyongcong 20131213
	m_SysTime=v_SysTime;

	char szTmp[MAX_PATH] = {0};
	sprintf_s(szTmp,sizeof(szTmp),"%04d-%02d-%02d\\",
		v_SysTime.wYear,v_SysTime.wMonth,v_SysTime.wDay);

	// ����������ļ���
	std::string strFilePath = m_strModuleDir + szTmp;
	if( _access(strFilePath.c_str(),0) != 0 )
	{// Ŀ¼������
		if( 0 == ::CreateDirectory(strFilePath.c_str(),0) )
		{
			char szLog[MAX_PATH*2] = {0};
			sprintf_s(szLog,sizeof(szLog),"\"%s\"�ļ�·������ʧ�ܣ�",strFilePath.c_str());
			g_ShowLog( _enError, szLog );
			return FALSE;
		}
	}

	//����ΪC++ ofstream�����ļ������ʽ�������ǰ��־�ļ��޷�������
	//jiangyongcong 20131213

	// ������ǰ��ͨ��־�ļ�
	sprintf_s(szTmp,sizeof(szTmp),"%04d-%02d-%02d_%02d_Comm.log",
		v_SysTime.wYear,v_SysTime.wMonth,v_SysTime.wDay,v_SysTime.wHour);
	std::string strFile = strFilePath+szTmp;
	if (m_foutCommFile.is_open())
	{
		m_foutCommFile.close();
	}
	std::locale::global(std::locale(""));//��ȫ��������Ϊ����ϵͳĬ������
	m_foutCommFile.open( strFile.c_str(), std::ios::out | std::ios::app );
	std::locale::global(std::locale("C"));//��ԭȫ�������趨
	
	// ������ǰ������־�ļ�
	sprintf_s(szTmp,sizeof(szTmp),"%04d-%02d-%02d_%02d_Warning.log",
		v_SysTime.wYear,v_SysTime.wMonth,v_SysTime.wDay,v_SysTime.wHour);
	strFile = strFilePath+szTmp;
	if (m_foutWarnFile.is_open())
	{
		m_foutWarnFile.close();
	}
	std::locale::global(std::locale(""));//��ȫ��������Ϊ����ϵͳĬ������
	m_foutWarnFile.open( strFile.c_str(), std::ios::out | std::ios::app );
	std::locale::global(std::locale("C"));//��ԭȫ�������趨

	// ������ǰ������־�ļ�
	sprintf_s(szTmp,sizeof(szTmp),"%04d-%02d-%02d_%02d_Error.log",
		v_SysTime.wYear,v_SysTime.wMonth,v_SysTime.wDay,v_SysTime.wHour);
	strFile = strFilePath+szTmp;
	if (m_foutErrFile.is_open())
	{
		m_foutErrFile.close();
	}
	std::locale::global(std::locale(""));//��ȫ��������Ϊ����ϵͳĬ������
	m_foutErrFile.open( strFile.c_str(), std::ios::out | std::ios::app );
	std::locale::global(std::locale("C"));//��ԭȫ�������趨

	// ������ǰ������־�ļ�
	sprintf_s(szTmp,sizeof(szTmp),"%04d-%02d-%02d_%02d_Net.log",
		v_SysTime.wYear,v_SysTime.wMonth,v_SysTime.wDay,v_SysTime.wHour);
	strFile = strFilePath+szTmp;
	if (m_foutNetFile.is_open())
	{
		m_foutNetFile.close();
	}
	std::locale::global(std::locale(""));//��ȫ��������Ϊ����ϵͳĬ������
	m_foutNetFile.open( strFile.c_str(), std::ios::out | std::ios::app );
	std::locale::global(std::locale("C"));//��ԭȫ�������趨


	// ������ǰģ����־�ļ�
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
		sprintf_s(szLog,sizeof(szLog),"\"%s\"�ļ���ʧ�ܣ�",strFile.c_str());
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
	{ // ��Ҫ�����µ��ļ�
		if( !CreateNewFiles(v_SysTime) )
		{
			return;
		}
	}

	FILE *pFile = NULL;
	char szDataTime[MAX_PATH] = {0};
	sprintf_s(szDataTime,sizeof(szDataTime),"��%02d:%02d:%02d��", 
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
	case _enModInfo: //������C�ļ���ʽ
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