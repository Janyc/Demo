#pragma once

#include <string>


#define _DATALOG_FILENAME_	"%04d-%02d-%02d_%02d %02d%02d_Com.txt"
#define _TEXTLOG_FILENAME_	"%04d-%02d-%02d_%02d %02d%02d_Log.txt"

namespace Utility
{
	namespace Log
	{
		namespace FileLog
		{

			//控制文件内容长度为100M
			const long c_slMaxFileSize=100*1024*1024;

			class CSaveLogToFile
			{

			public:
				CSaveLogToFile( void );
				virtual ~CSaveLogToFile(void);

			public:
				int toPrintFileLog(const char v_szDescription[], const char v_szLog[]);
				int toPrintFileLog( const char v_szDescription[], const char v_szBuf[], int v_iBufLen );

				void SetDirName(const char * v_szDirName);
				void EnableLog( void );
				void DisableLog( void);
				BOOL IsEnable(void);

			private:
				void SetLogFileName(void);
				void SetFileDir(void);

			private:
				std::string		m_strFileName;
				std::string		m_strDir;
				std::string		m_strDirName;
				SYSTEMTIME      m_systime;
				CRITICAL_SECTION	m_lock;
				BOOL m_bEnable;

			};

		}
	}

}


using namespace Utility;
using namespace Utility::Log;
using namespace Utility::Log::FileLog;
