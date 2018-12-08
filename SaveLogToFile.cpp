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
			* ���������������־
			*
			* @param[in] v_szDescription ��־������Ϣ
			* @param[in] v_szBuf ��־����
			* @param[in] v_iBufLen ��־���ݳ���
			* 
			* @return
			* -0 ���سɹ�
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

				std::locale::global(std::locale(""));//��ȫ��������Ϊ����ϵͳĬ������
				ofstream	fileStream( fileName.c_str(), ios::out | ios::app );
				std::locale::global(locale("C"));//��ԭȫ�������趨

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
			* ����ı���־
			*
			* @param[in] v_szDescription ��־������Ϣ
			* @param[in] v_szBuf ��־����
			* 
			* @return
			* -0 ���سɹ�
			*/
			int CSaveLogToFile::toPrintFileLog(
				const char v_szDescription[],	// ������Ϣ
				const char v_szLog[]		        // ��־��Ϣ
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

				std::locale::global(std::locale(""));//��ȫ��������Ϊ����ϵͳĬ������
				ofstream	fileStream( fileName.c_str(), ios::out | ios::app );
				std::locale::global(locale("C"));//��ԭȫ�������趨

				fileStream << szTime << " " << v_szDescription << ": " ;
				fileStream << v_szLog << endl;

				return 0;
			}

			/*
			*	��������SetLogFileName
			*���Ρ�����
			*	�����ܣ�������־�ļ���
			*	����ֵ��
			*	�����ߣ����ɻ�
			*	ʱ���䣺2008-7-30 11:34
			*������ע��
			*/
			/**
			* ������־�ļ�����
			*
			* @note �ļ�������������_ʱ��������
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
			* ������־�ļ���ŵ�Ŀ¼����
			*
			* @param[in] v_szDirName ָ��Ŀ¼���� ��ָ���༶Ŀ¼
			*/
			void CSaveLogToFile::SetDirName(const char * v_szDirName)
			{
				_ASSERT( NULL!=v_szDirName );
				m_strDirName = v_szDirName;
			}

			/**
			* ������־���·��
			*
			*/
			inline void CSaveLogToFile::SetFileDir(void)
			{
				char	szFilePath[MAX_PATH];
				GetModuleFileName( g_hInstance, szFilePath, sizeof(szFilePath) );
				char    szDrive[MAX_PATH];				// ������
				char    szDir[MAX_PATH];				// ·��
				char    szFileName[MAX_PATH];			// �ļ���
				char    szExtName[MAX_PATH];			// ��չ��
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
			* ʹ��־������Ч
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

				std::locale::global(std::locale(""));//��ȫ��������Ϊ����ϵͳĬ������
				ofstream	fileStream( fileName.c_str(), ios::out | ios::app );
				std::locale::global(locale("C"));//��ԭȫ�������趨

				fileStream << szDate << " " << szTime << "  "<<  "==========��־����==========" << endl << endl;

				m_bEnable	= TRUE;
			}

			/**
			* ʹ��־����ʧЧ
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

				std::locale::global(std::locale(""));//��ȫ��������Ϊ����ϵͳĬ������
				ofstream	fileStream( fileName.c_str(), ios::out | ios::app );
				std::locale::global(locale("C"));//��ԭȫ�������趨

				fileStream << szDate << " " << szTime << "  "<<  "==========��־�ر�==========" << endl << endl;

			}

			/**
			* �鿴��־�����Ƿ���Ч
			*
			*/
			BOOL CSaveLogToFile::IsEnable(void)
			{
				return m_bEnable;
			}

		}
	}

}

