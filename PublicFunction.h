// PublicFunction.h: interface for the CPublicFunction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUBLICFUNCTION_H__4C541386_4F1A_465D_A133_BAA0A5FD392C__INCLUDED_)
#define AFX_PUBLICFUNCTION_H__4C541386_4F1A_465D_A133_BAA0A5FD392C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPublicFunction  
{
public:
	CPublicFunction();
	virtual ~CPublicFunction();
public:
	static BOOL ConvertFloatToStdGps(double _dbGps, byte *_pStdGPS);
	static int SevenToEight(char *cSource, int len,char *OUT_pEightBitData,int iOutBufferLen);
	static unsigned char Converse7FtoZero(unsigned char Data);
	static DWORD strtoInt(char * IN_szBuf);
	static int  ReConvertStr(char buf[], char newbuf[], int len);
	static int  ConvertStr(char buf[], int len);
	static BOOL CheckMobileTel(char *v_pszMobileTel);

	static BOOL CheckCrcChecksum( const BYTE* v_strBuf, const int v_iBufLen );
	static BYTE GetCrcChecksum( const BYTE* v_strBuf, const int v_iLen);

	static BYTE GprsCheckSum_Get( const BYTE* v_strBuf, const int v_iLen );
	static BOOL GprsCheckSum_Check( const BYTE* v_strBuf, const int v_iBufLen );
	static BYTE SmsCheckSum_Get( const BYTE* v_strBuf, const int v_iLen );
	static BOOL SmsCheckSum_Check( const BYTE* v_strBuf, const int v_iBufLen );

	static BYTE RouteCheckSum_Get(const BYTE* v_strBuf, const int v_iLen );
	static BOOL RouteCheckSum_Check(const BYTE* v_strBuf, const int v_iBufLen );


	static void ReMapConvert(BYTE dest[], const BYTE source[], const int sourceLen);
	static void MapConvert(BYTE dest[], const BYTE source[], const int sourceLen);
	static int Code6ToCode7(BYTE dest[], const BYTE source[], const int sourceLen);
	static int Code7ToCode6(BYTE dest[], const BYTE source[], const int sourceLen);
    // if flag then convert source from 8-bit to x-bit
    //         else from x-bit to 8-bit
    // NOT support 7->6 , 6->7 now
    static int CodeX(BYTE        dest[], 
                     const BYTE  source[], 
                     const int   sourceLen,
                     const int   x,
                     const bool  eightToX = true);

    // convert "YYYYMMDDHHMMSS" to "YYYY-MM-DD HH:MM:SS"
    static int str2datetime(char* result, const char* strTime);

    // convert "YYYY-MM-DD HH:MM:SS" to "YYYYMMDDHHMMSS"
    static int datetime2str(char* result, const char* strTime);

    // convert "HHMMSS" to "HH:MM:SS"
    static int str2time(char* result, const char* strTime);
	
    // convert BCD BYTE[] = {0x01,0x23,0x45} to "012345"
    static int bcd2str(char* result, const BYTE* source, const int len);

    // convert binary data to string. 
    // e.g. : BYTE[] = {0x01, 0x7e} to "017e"
    static int bin2str(char* result, const BYTE* source, const int len);

    // convert string to binary data
    // e.g. : "017e" ==> BYTE[] { 0x01, 0x7e }
    static int str2bin( unsigned char *uszResult, const char* szSource, const int len );

    // convert 0x7f to 0x00
    static void x7f2zero(unsigned char* result, const unsigned char* source, const int len);
    static void zero2x7f(unsigned char* szResult, const unsigned char* szSource, const int iLen);

    static int Code8ToCodeX(BYTE dest[], const BYTE source[], const int sourceLen, const int x);
    static int CodeXToCode8(BYTE dest[], const BYTE source[], const int sourceLen, const int x);

    static bool trimRight( char* szDest, const char* szSource, const int iLen );
    static bool trimRight( char* szString, const int iLen );
    
    static bool spaceRight( char* szTarget, const int iLen );
	static char * IntToStr(int Number);
	static int AddFsynToBuf(const char *v_strOldBuf, char *v_strNewBuf, const int v_iBufLen,BOOL v_bNeedEncode=FALSE);
};

#endif // !defined(AFX_PUBLICFUNCTION_H__4C541386_4F1A_465D_A133_BAA0A5FD392C__INCLUDED_)
