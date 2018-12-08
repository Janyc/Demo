#include "StdAfx.h"
#include ".\savelogtofile.h"

#include <direct.h>
#include <strstream>
#include <fstream>

#include "JYCCommTool.h"

using namespace std;

namespace Utility
{
	namespace Log
	{
		namespace FileLog
		{
			HINSTANCE g_hInstance = NULL;

			CSaveLogToFile::CSaveLogToFile( void )
				: m_bEnable(FALSE)
			{
				::memset(&m_systime,0x00,sizeof(SYSTEMTIME)) ;
				::InitializeCriticalSection( &m_lock );
			}

			CSaveLogToFile::~CSaveLogToFile(void)
			{
				::DeleteCriticalSection( &m_lock );
			}

			/**
			* 输出二进制数据日志
			*
			* @param[in] v_szDescription 日志描述信息
			* @param[in] v_szBuf 日志内容
			* @param[in] v_iBufLen 日志内容长度
			* 
			* @return
			* -0 返回成功
			*/
			int CSaveLogToFile::toPrintFileLog( const char v_szDescription[], const char v_szBuf[], int v_iBufLen ) 
			{	
				if( !m_bEnable )
				{
					return 0;
				}
				
				CAutoCS cs (&m_lock );

				SetLogFileName();

				char	szTime[128]={0};
				::GetTimeFormat( LOCALE_SYSTEM_DEFAULT
					, TIME_FORCE24HOURFORMAT, NULL
					, "HH:mm:ss", szTime, sizeof(szTime) );

				string	fileName	= m_strDir + m_strFileName;

				std::locale::global(std::locale(""));//将全局区域设为操作系统默认区域
				ofstream	fileStream( fileName.c_str(), ios::out | ios::app );
				std::locale::global(locale("C"));//还原全局区域设定

				char szBuf[10]={0};

				fileStream << szTime << " " << v_szDescription << ": ";
				for( int i = 0; i < v_iBufLen; i ++ )
				{
					/*

					if( 0 == ( i % 23 ) )
					{ 
					fileStream << endl;
					for( int j = 0; j < 10; j ++ )
					{
					fileStream << " " ;
					}
					}
					*/
					::sprintf_s( szBuf, 10, "%02x", (BYTE)v_szBuf[i]);
					fileStream << " "<< szBuf ;
				}
				fileStream << endl ;

				//fileStream.seekp(0,ios::end);
				//int nSize=fileStream.tellp();

				return 0;
			}

			/**
			* 输出文本日志
			*
			* @param[in] v_szDescription 日志描述信息
			* @param[in] v_szBuf 日志内容
			* 
			* @return
			* -0 返回成功
			*/
			int CSaveLogToFile::toPrintFileLog(
				const char v_szDescription[],	// 描述信息
				const char v_szLog[]		        // 日志信息
			)
			{
				if( !m_bEnable )
				{
					return 0;
				}

				CAutoCS cs( &m_lock );

				SetLogFileName();

				char	szTime[128]={0};
				::GetTimeFormat( LOCALE_SYSTEM_DEFAULT
					, TIME_FORCE24HOURFORMAT ,NULL 
					, "HH:mm:ss", szTime, sizeof(szTime) );

				string	fileName	= m_strDir + m_strFileName;

				std::locale::global(std::locale(""));//将全局区域设为操作系统默认区域
				ofstream	fileStream( fileName.c_str(), ios::out | ios::app );
				std::locale::global(locale("C"));//还原全局区域设定

				fileStream << szTime << " " << v_szDescription << ": " ;
				fileStream << v_szLog << endl;

				return 0;
			}

			/*
			*	函数名：SetLogFileName
			*　参　数：
			*	功　能：设置日志文件名
			*	返回值：
			*	作　者：王松辉
			*	时　间：2008-7-30 11:34
			*　备　注：
			*/
			/**
			* 设置日志文件名称
			*
			* @note 文件名称以年月日_时进行命名
			*/
			void CSaveLogToFile::SetLogFileName(void)
			{
				SYSTEMTIME st ;
				GetLocalTime(&st);
				if (   st.wYear==m_systime.wYear 
					&& st.wMonth==m_systime.wMonth 
					&& st.wDay==m_systime.wDay 
					&& st.wHour==m_systime.wHour )
				{
					return ;
				}

				string	strFileName;
				char	szDate[128];
				::sprintf( szDate,"%04d-%02d-%02d_%02d.txt",st.wYear,st.wMonth,st.wDay,st.wHour );

				m_strFileName = szDate;

				m_systime = st;
			}


			/**
			* 设置日志文件存放的目录名称
			*
			* @param[in] v_szDirName 指定目录名称 可指定多级目录
			*/
			void CSaveLogToFile::SetDirName(const char * v_szDirName)
			{
				_ASSERT( NULL!=v_szDirName );
				m_strDirName = v_szDirName;
			}

			/**
			* 设置日志输出路径
			*
			*/
			inline void CSaveLogToFile::SetFileDir(void)
			{
				char	szFilePath[MAX_PATH];
				GetModuleFileName( g_hInstance, szFilePath, sizeof(szFilePath) );
				char    szDrive[MAX_PATH];				// 驱动器
				char    szDir[MAX_PATH];				// 路径
				char    szFileName[MAX_PATH];			// 文件名
				char    szExtName[MAX_PATH];			// 扩展名
				::_splitpath( szFilePath, szDrive, szDir, szFileName, szExtName );
				m_strDir	= szDrive;
				m_strDir	+= szDir;
				m_strDir	+= m_strDirName;
				m_strDir	+= "\\";

				if (false==CMyUtils::FileOrDirExists(m_strDir.c_str()))
				{
					CMyUtils::MakeMultiDir(m_strDir.c_str());
				}
				//_mkdir( m_strDir.c_str() );
			}

			/**
			* 使日志功能有效
			* 
			*/
			void CSaveLogToFile::EnableLog( void )
			{
				SetFileDir();
				SetLogFileName();

				string	fileName	= m_strDir + m_strFileName;

				char	szDate[128];
				::GetDateFormat(LOCALE_SYSTEM_DEFAULT,
					LOCALE_NOUSEROVERRIDE,
					NULL,
					NULL,
					szDate,
					sizeof(szDate)
					);
				char	szTime[128];
				::GetTimeFormat(LOCALE_SYSTEM_DEFAULT,
					TIME_FORCE24HOURFORMAT,
					NULL,
					"HH:mm:ss",
					szTime,
					sizeof(szTime)
					);

				std::locale::global(std::locale(""));//将全局区域设为操作系统默认区域
				ofstream	fileStream( fileName.c_str(), ios::out | ios::app );
				std::locale::global(locale("C"));//还原全局区域设定

				fileStream << szDate << " " << szTime << "  "<<  "==========日志开启==========" << endl << endl;

				m_bEnable	= TRUE;
			}

			/**
			* 使日志功能失效
			*
			*/
			void CSaveLogToFile::DisableLog( void )
			{
				m_bEnable	= FALSE;

				SetFileDir();
				SetLogFileName();

				string	fileName	= m_strDir + m_strFileName;

				char	szDate[128];
				::GetDateFormat(LOCALE_SYSTEM_DEFAULT,
					LOCALE_NOUSEROVERRIDE,
					NULL,
					NULL,
					szDate,
					sizeof(szDate)
					);
				char	szTime[128];
				::GetTimeFormat(LOCALE_SYSTEM_DEFAULT,
					TIME_FORCE24HOURFORMAT,
					NULL,
					"HH:mm:ss",
					szTime,
					sizeof(szTime)
					);

				std::locale::global(std::locale(""));//将全局区域设为操作系统默认区域
				ofstream	fileStream( fileName.c_str(), ios::out | ios::app );
				std::locale::global(locale("C"));//还原全局区域设定

				fileStream << szDate << " " << szTime << "  "<<  "==========日志关闭==========" << endl << endl;

			}

			/**
			* 查看日志功能是否有效
			*
			*/
			BOOL CSaveLogToFile::IsEnable(void)
			{
				return m_bEnable;
			}

		}
	}

}

