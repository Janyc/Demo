// PublicFunction.cpp: implementation of the CPublicFunction class.
//
//////////////////////////////////////////////////////////////////////
#include "TF_CN_Cv_DownFileManage.h"
#include "stdafx.h"

#include "PublicFunction.h"
#include <algorithm>
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//namespace InterfaceDealBase
//{
CPublicFunction::CPublicFunction()
{

}

CPublicFunction::~CPublicFunction()
{

}


static unsigned char g_CrcTable[256] = {
    0x00, 0x91, 0xE3, 0x72, 0x07, 0x96, 0xE4, 0x75,  0x0E, 0x9F, 0xED, 0x7C, 0x09, 0x98, 0xEA, 0x7B,
    0x1C, 0x8D, 0xFF, 0x6E, 0x1B, 0x8A, 0xF8, 0x69,  0x12, 0x83, 0xF1, 0x60, 0x15, 0x84, 0xF6, 0x67,
    0x38, 0xA9, 0xDB, 0x4A, 0x3F, 0xAE, 0xDC, 0x4D,  0x36, 0xA7, 0xD5, 0x44, 0x31, 0xA0, 0xD2, 0x43,
    0x24, 0xB5, 0xC7, 0x56, 0x23, 0xB2, 0xC0, 0x51,  0x2A, 0xBB, 0xC9, 0x58, 0x2D, 0xBC, 0xCE, 0x5F,

    0x70, 0xE1, 0x93, 0x02, 0x77, 0xE6, 0x94, 0x05,  0x7E, 0xEF, 0x9D, 0x0C, 0x79, 0xE8, 0x9A, 0x0B,
    0x6C, 0xFD, 0x8F, 0x1E, 0x6B, 0xFA, 0x88, 0x19,  0x62, 0xF3, 0x81, 0x10, 0x65, 0xF4, 0x86, 0x17,
    0x48, 0xD9, 0xAB, 0x3A, 0x4F, 0xDE, 0xAC, 0x3D,  0x46, 0xD7, 0xA5, 0x34, 0x41, 0xD0, 0xA2, 0x33,
    0x54, 0xC5, 0xB7, 0x26, 0x53, 0xC2, 0xB0, 0x21,  0x5A, 0xCB, 0xB9, 0x28, 0x5D, 0xCC, 0xBE, 0x2F,

    0xE0, 0x71, 0x03, 0x92, 0xE7, 0x76, 0x04, 0x95,  0xEE, 0x7F, 0x0D, 0x9C, 0xE9, 0x78, 0x0A, 0x9B,
    0xFC, 0x6D, 0x1F, 0x8E, 0xFB, 0x6A, 0x18, 0x89,  0xF2, 0x63, 0x11, 0x80, 0xF5, 0x64, 0x16, 0x87,
    0xD8, 0x49, 0x3B, 0xAA, 0xDF, 0x4E, 0x3C, 0xAD,  0xD6, 0x47, 0x35, 0xA4, 0xD1, 0x40, 0x32, 0xA3,
    0xC4, 0x55, 0x27, 0xB6, 0xC3, 0x52, 0x20, 0xB1,  0xCA, 0x5B, 0x29, 0xB8, 0xCD, 0x5C, 0x2E, 0xBF,

    0x90, 0x01, 0x73, 0xE2, 0x97, 0x06, 0x74, 0xE5,  0x9E, 0x0F, 0x7D, 0xEC, 0x99, 0x08, 0x7A, 0xEB,
    0x8C, 0x1D, 0x6F, 0xFE, 0x8B, 0x1A, 0x68, 0xF9,  0x82, 0x13, 0x61, 0xF0, 0x85, 0x14, 0x66, 0xF7,
    0xA8, 0x39, 0x4B, 0xDA, 0xAF, 0x3E, 0x4C, 0xDD,  0xA6, 0x37, 0x45, 0xD4, 0xA1, 0x30, 0x42, 0xD3,
    0xB4, 0x25, 0x57, 0xC6, 0xB3, 0x22, 0x50, 0xC1,  0xBA, 0x2B, 0x59, 0xC8, 0xBD, 0x2C, 0x5E, 0xCF
};

static char g_MapTable[64] = {
	//通用版：编码转换表(8/6和7/6)
	//公开版：编码转换表(8/6)
	0x2E,0x2C,0x30,0x31,0x32,0x33,0x34,0x35,
	0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,
	0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,
	0x4D,0x4E,0x4F,0x50,0x51,0x52,0x53,0x54,
	0x55,0x56,0x57,0x58,0x59,0x5A,0x61,0x62,
	0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,
	0x6B,0x6C,0x6D,0x6E,0x6F,0x70,0x71,0x72,
	0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A
};


BYTE CPublicFunction::GetCrcChecksum(const BYTE* v_strBuf, const int v_iLen)
{
    BYTE byCrcChecksum;
    
    byCrcChecksum = 0xff;
	int	j = 0;
    for( int i=v_iLen; i>0; i-- ) 
	{
        byCrcChecksum = g_CrcTable[byCrcChecksum ^ v_strBuf[j++]];
    }//for i

    return 0xff - byCrcChecksum;
}

/// 检查校验和是否正确
BOOL CPublicFunction::CheckCrcChecksum(const BYTE *v_strBuf, const int v_iBufLen)
{
	BYTE	byChecksum = GetCrcChecksum( v_strBuf+1, v_iBufLen-1 );
	if( byChecksum==v_strBuf[0] )//校验和是存放在Net头部中的第一个数据，校验和是怎么得出的呢？  张华注
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BYTE CPublicFunction::GprsCheckSum_Get( const BYTE* v_strBuf, const int v_iLen )
{
	BYTE	byCheckSum = 0;
	
	for(int i=1; i<v_iLen; i++)
	{
		//带进位累加和
	// 	TRACE("0x%02x ",v_strBuf[i]);
		byCheckSum += v_strBuf[i];
		if(byCheckSum < v_strBuf[i])
		{
			byCheckSum++;
		}
// 		TRACE("\n");
	}

    return byCheckSum;
}

BOOL CPublicFunction::GprsCheckSum_Check( const BYTE* v_strBuf, const int v_iBufLen )
{
	BYTE	byCheckSum = GprsCheckSum_Get(v_strBuf,v_iBufLen);

	if(byCheckSum == v_strBuf[0])
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BYTE CPublicFunction::SmsCheckSum_Get( const BYTE* v_strBuf, const int v_iLen )
{
	BYTE	byCheckSum = 0;

	for(int i=0; i<v_iLen; i++)
	{
		//带进位累加和
		byCheckSum += v_strBuf[i];
		if(byCheckSum < v_strBuf[i])
		{
			byCheckSum++;
		}
	}

	//最高位清零
	byCheckSum &= 0x7f;

	//如果最终结果为0，则转义为0x7f
	if(0x00 == byCheckSum)
	{
		byCheckSum = 0x7f;
	}

    return byCheckSum;
}

BOOL CPublicFunction::SmsCheckSum_Check( const BYTE* v_strBuf, const int v_iBufLen )
{
	BYTE	byCheckSum = SmsCheckSum_Get(v_strBuf,v_iBufLen - 1);

	if(byCheckSum == v_strBuf[v_iBufLen-1])
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//不带进位的累加和
BYTE CPublicFunction::RouteCheckSum_Get(const BYTE* v_strBuf, const int v_iLen )
{
	BYTE	byCheckSum = 0;

	for(int i=0; i<v_iLen; i++)
	{
		//不带进位累加和
		byCheckSum += v_strBuf[i];
	}
    return byCheckSum;
}

BOOL CPublicFunction::RouteCheckSum_Check(const BYTE* v_strBuf, const int v_iBufLen )
{
	BYTE	byCheckSum = RouteCheckSum_Get(v_strBuf,v_iBufLen - 1);

	if(byCheckSum == v_strBuf[v_iBufLen-1])
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


/***********************************************
   函数名：ConvertStr
   描述：  去掉字符串中的结束符号，并格式化成固定长度
   参数：  buf       待格式化的字符串
           len       格式化的字符串的长度
   返回值：错误类型
************************************************/
int CPublicFunction::ConvertStr(char buf[], int len)
{
	int		i,j;
	for(i=0;i<len;i++)
	{
		if(buf[i]==0)
		{
			for(j=i;j<len;j++)
				buf[j]=0x20;
			break;
		}
	}
    return 1;
}
/***********************************************
   函数名：ReConvertStr
   描述：  将收到的数据格式化成字符串，并加入结束标志
   参数：  buf          待格式化的数据
           newbuf       格式化后的数据
		   len			待格式化的数据的长度
   返回值：错误类型
************************************************/
int CPublicFunction::ReConvertStr(char buf[], char newbuf[], int len)
{
	int i;

	memcpy((void *)newbuf,(void *)buf,len);
	for(i=len-1;i>=0;i--)
	{
		if(newbuf[i]!=0x20)
			break;
	}
	if(i>=0)
	{
		newbuf[i+1]=0;
	}
	else
	{
		newbuf[0]=0;
	}
	return 1;
}


//检查手机号是否合法
BOOL CPublicFunction::CheckMobileTel(char *v_pszMobileTel)
{
	return TRUE;//测试用
	//
	if ( NULL == v_pszMobileTel )
	{
		return FALSE;
	}

	//手机号必须11位
	int iLen = strlen(v_pszMobileTel);
	if (iLen != 11)
	{
		return FALSE;
	}

	//手机号头必须是13
	if ( (v_pszMobileTel[0] != 0x31) && ((v_pszMobileTel[1] != 0x33)||(v_pszMobileTel[1] != 0x35)) )
	{
		return FALSE;
	}

	//手机号必须全部是数字
	for (int i =0 ; i < 11; ++i)
	{
		if ( (v_pszMobileTel[i] < 0x30 ) || (v_pszMobileTel[i] > 0x39) )
		{
			return FALSE;
		}
	}

	return TRUE;
}

DWORD CPublicFunction::strtoInt(char *IN_szBuf)
{
	char szTemp[20];
	int   iLen=strlen(IN_szBuf);
	DWORD dwVehicleTel=0;
	if(iLen>=9)
	{
		if(iLen<=11)
		{
			memcpy(szTemp,&IN_szBuf[iLen-9],9);
			szTemp[10]=0;
			dwVehicleTel=strtoul(szTemp,NULL,10);
		}
		else
		{
			memcpy(szTemp,&IN_szBuf[2],9);
			szTemp[10]=0;
			dwVehicleTel=strtoul(szTemp,NULL,10);
		}
		
	}
	else
	{
		dwVehicleTel=strtoul(IN_szBuf,NULL,10);
	}
	
			
			
	return dwVehicleTel;
}


int CPublicFunction::str2datetime(char* format, const char* str) 
{
    char buffer[10];
    char result[100];

    if(strlen(str)!=14)   // error
        return -1;
                
    strncpy(buffer, str, 4);
    buffer[4] = '\0';
    strcpy(result, buffer);
    result[4] = '-';
    result[4+1] = '\0';
        
    strncpy(buffer, str+4, 2);
    buffer[2] = '\0';
    strcat(result, buffer);
    result[7] = '-';
    result[7+1] = '\0';
        
    strncpy(buffer, str+6, 2);
    buffer[2] = '\0';
    strcat(result, buffer);
    result[10] = ' ';
    result[10+1] = '\0';

    strncpy(buffer, str+8, 2);
    buffer[2] = '\0';
    if(atoi(buffer) > 23) 
    {
        buffer[0] = '2';
        buffer[1] = '3';
    }
    strcat(result, buffer);
    result[13] = ':';
    result[13+1] = '\0';

    strncpy(buffer, str+10, 2);
    buffer[2] = '\0';
    if(atoi(buffer) > 59) 
    {
        buffer[0] = '5';
        buffer[1] = '9';
    }
    strcat(result, buffer);
    result[16] = ':';
    result[16+1] = '\0';

    strncpy(buffer, str+12, 2);
    buffer[2] = '\0';
    if(atoi(buffer) > 59) 
    {
        buffer[0] = '5';
        buffer[1] = '9';
    }
    strcat(result, buffer);
    strcpy(format, result);

    return 0;
}

int CPublicFunction::datetime2str(char* result, const char* strTime) 
{
    char strTemp[50];
    int j;
    int len = strlen(strTime);
    j = 0;

    for(int i=0; i<len; i++) 
    {
        if(strTime[i]>'9' || strTime[i]<'0')
            continue;
        strTemp[j] = strTime[i];
        j++;
    }
    strTemp[j] = '\0';  // end strTemp string

    strcpy(result, strTemp);

    return 0;
}

int CPublicFunction::str2time(char* result, const char* strTime)
{
	if(6!=strlen(strTime))
		return -1;
	
	char temp[10];

	strncpy(temp, strTime, 2);
	temp[2] = ':';
	strncpy(temp+3, strTime+2, 2);
	temp[5] = ':';
	strcpy(temp+6, strTime+4);
	temp[8] = '\0';

	strcpy(result, temp);

	return 0;
}

int CPublicFunction::bcd2str(char* result, const BYTE* source, const int len) 
{
    const static int step = '0' - 0;
    int              j = 0;
    int              high;
    int              low;
    bool             flag = false;

    for(int i = 0; i < len; i++)
    {
        low = source[i]%16;
        high = source[i]/16;

        if(low < 0 || low > 9 || high < 0 || high > 9)
        {
            TRACE("bcd2str() error: invalid input data.\n");
            CString         strLog;

            strLog.Format( "\n%s:%d:错误的bcd码参数", __FILE__, __LINE__ );
            result[j] = '\0';

            throw std::exception( strLog );
        }
	
        result[j] = (char)high + step;
        j++;
        result[j] = (char)low + step;
        j++;
    }
    result[j] = '\0';
	
    return j;
}

void CPublicFunction::x7f2zero(unsigned char* result, const unsigned char* source, const int len)
{
    for(int i = 0; i < len; i++)
    {	
        if(source[i]==0x7f)
            result[i] = 0;
        else
            result[i] = source[i];
    }
}

void CPublicFunction::zero2x7f(unsigned char* szResult, const unsigned char* szSource, const int iLen)
{
    for(int i = 0; i < iLen; ++i)
    {	
        if(0==szSource[i])
            szResult[i] = 0x7f;
        else
            szResult[i] = szSource[i];
    }
}

int CPublicFunction::bin2str(char* result, const BYTE* source, const int len)
{
    if(len < 0)
        return 0;

    char	elem[3];
	
    result[0] = '\0';
    for(int i = 0; i < len; ++i)
    {
        sprintf(elem, "%02x", source[i]);
        strcat(result, elem);
    }

    return (len*2);
}


int CPublicFunction::CodeX(BYTE dest[], const BYTE source[], const int sourceLen, const int x, const bool eightToX)
{
    // check params
	if(dest==source)
		return -1;

    if(sourceLen <=0)
        return -1;

    if((x<=0)||(x>=8))
        return -1;

    // dispatch
    if(eightToX)
        return Code8ToCodeX(dest, source, sourceLen, x);
    else
        return CodeXToCode8(dest, source, sourceLen, x);
}

int CPublicFunction::Code8ToCodeX(BYTE dest[], const BYTE source[], const int sourceLen, const int x)
{
    const int             step = x;

    int                   destLen = 0;
    int                   stepCount = 0; 
    int                   stepExtra = 0;

    BYTE                  headValue = 0;

    float temp = sourceLen*8.0f/step;
    float temp2 = (float)(int)temp;

    destLen = temp>temp2 ? (int)temp+1 : (int)temp;
    
    int sourceNo = 0;
    int destNo = 0;

    while(true)
    {
        // check if the convertion is End or Wrong
        if( (sourceNo == sourceLen) && (destNo == destLen) )
            break;
        else if( (sourceNo >= sourceLen) || (destNo >= destLen) )
            return -1;

        // calculate dest[destNo]
        sourceNo = (temp=destNo*step/8.0f)>(int)temp ? (int)temp+1 : (int)temp;
        stepExtra = (stepExtra + (8-step))%8;
        if( (destNo==destLen-1) || (stepExtra==0) )
            dest[destNo] = headValue>>(8-step);
        else
            dest[destNo] = (headValue>>(8-step)) | (source[sourceNo]>>stepExtra);
		
        ++destNo;
        headValue = source[sourceNo]<<(8-stepExtra);
    }

    return destLen;
}

int CPublicFunction::CodeXToCode8(BYTE dest[], const BYTE source[], const int sourceLen, const int x)
{
    const int          step = x;
    int                destLen;

    int                destNo;
    int                sourceNo;

    BYTE               tail;
    int                tailLen;

    destLen = sourceLen*step/8;
    destNo = 0;
    sourceNo = 0;
    tail = 0;

//    for(sourceNo = 0; sourceNo < sourceLen; ++sourceNo)
    for(sourceNo = 0; sourceNo < sourceLen-1; ++sourceNo)//@2008-02-29
    {
        tailLen = ((8-step)*(sourceNo+1))%8;
        if(tailLen==0)
            continue;

        tail = source[sourceNo+1]>>(step-tailLen);
        dest[destNo] = source[sourceNo]<<tailLen | tail;
        ++destNo;
    }

    return destLen;
}

bool CPublicFunction::trimRight( char* szDest, const char* szSource, const int iLen )
{
    if( ( iLen < 0 ) || ( szDest==szSource ) || ( 0==szDest ) || ( 0==szSource ) )
        return false;

    memcpy( szDest, szSource, iLen );

    return trimRight( szDest, iLen );
}

bool CPublicFunction::trimRight( char* szDest, const int iLen )
{
	int pos = -1;

	for( int i = 0; i < iLen; ++i )
	{
		switch( szDest[i] )
		{
		case 0x00:
			if( -1 != pos )
			{
				szDest[pos] = 0;
				return true;
			}
			break;

		case 0x20:
			if( -1 == pos )
				pos = i;
			break;

		default:
			pos = -1;
			break;
		}
	}

	if( -1 != pos )
	{
		szDest[pos] = 0;
		return true;
	}
	
    return false;
}

bool CPublicFunction::spaceRight( char* szTarget, const int iLen )
{
    for( int i = 0; i < iLen; ++i )
    {
        if( 0==szTarget[i] )
        {
            memset( szTarget+i, 0x20, iLen-i );
            return true;
        }
    }
    
    return false;
}

int CPublicFunction::str2bin( unsigned char *uszResult, const char* szSource, const int len )
{
    int     iResultLen = 0;
    char    szNum[3];

    for( int i = 0, j = 0; i < len; ++i )
    {
        if( (szSource[i] >= '0' && szSource[i] <= '9') ||
            (szSource[i] >= 'a' && szSource[i] <= 'f') || 
			(szSource[i] >= 'A' && szSource[i] <= 'F')	)
        {
            szNum[j%2] = szSource[i];
            ++j;

            if( 0==(j%2) )
            {
                szNum[2] = 0;
                uszResult[iResultLen] = static_cast<unsigned char>(strtol( szNum, NULL, 16 ));
                ++iResultLen;
            }
        }
        else // skip NOT number char
        {
            continue;
        }
    }

    return iResultLen;
}

int CPublicFunction::Code7ToCode6(BYTE dest[], const BYTE source[], const int sourceLen)
{
	int destLen;
	destLen = ceil(sourceLen*7.0f/6);

	int i,pos,left;
	for(i=0; i<destLen; i++)
	{
		pos = i*6/7;
		left = 7 - i*6%7;

		if(left >= 6)
		{
			dest[i] = 0x3f & (source[pos] >> (left-6));
		}
		else if( (pos+1) <= (sourceLen-1) )
		{
			dest[i] = 0x3f & ((source[pos] << (6-left)) | (source[pos+1] >> (1+left)));
		}
		else
		{
			dest[i] = 0x3f & (source[pos] << (6-left));
		}
	}

	return destLen;
}

int CPublicFunction::Code6ToCode7(BYTE dest[], const BYTE source[], const int sourceLen)
{
	int destLen;
	destLen = sourceLen*6/7;

	int pos,left;
	for(int i=0; i<destLen; i++)
	{
		pos = i*7/6;
		left = 6 - i*7%6;

		dest[i] = 0x7f & ((source[pos] << (7-left)) | (source[pos+1] >> (left-1)));
	}

	return destLen;
}

void CPublicFunction::MapConvert(BYTE dest[], const BYTE source[], const int sourceLen)
{
	for(int i=0; i<sourceLen; i++)
	{
		dest[i] = g_MapTable[source[i]];
	}
}

void CPublicFunction::ReMapConvert(BYTE dest[], const BYTE source[], const int sourceLen)
{
	char *location;
	for(int i=0; i<sourceLen; i++)
	{
		location=std::find(g_MapTable,g_MapTable+64,source[i]);	//std::find is just a function template defined in <algorithm>
		if(location!=(g_MapTable+64))
		{
			dest[i]=location-g_MapTable;
		}
	}
}


unsigned char CPublicFunction::Converse7FtoZero(unsigned char Data)
{
	if (Data==0x7f)
		return 0;
	else
		return Data;
}
//把整型数转换位字符串
//该函数调用后记的释放堆内存
char * CPublicFunction::IntToStr(int Number)
{

   char ch,*str,*right,*left;

   unsigned int Value;

   str = (char *)new char[12*sizeof(char)];

   left = right = str;

   //如果是负数，则应加上负号，left、right向后走。

   if(Number < 0)

   {

      Value = -Number;

      *str = '-';

      left++,right++;

   }

   else

      Value = (unsigned)Number;

   //把数字转换成字符串（倒置的）

   while(Value)

   {

      *right = (Value%10)+0x30;

      Value = Value/10;

      right++;

   }

   *right-- = '\0';

   //把倒置的字符串正放过来

   while(right > left)

   {

      ch = *left;

       *left++ = *right;

       *right-- = ch;

   }

   return str;

} 

/// 内部函数：转义（7E->7D+02   7D->7D+01）
int CPublicFunction::AddFsynToBuf(const char *v_strOldBuf, char *v_strNewBuf, const int v_iBufLen,BOOL v_bNeedEncode)
{
	int		iPos=1,i;
	char	strTemp[1500];

	for( i=0; i<v_iBufLen; i++ )
	{
		switch( v_strOldBuf[i] )
		{
			case 0x7e:
				strTemp[iPos++] = 0x7d;
				strTemp[iPos++] = 0x02;
				break;
			case 0x7d:
				strTemp[iPos++] = 0x7d;
				strTemp[iPos++] = 0x01;
				break;
			default:
				strTemp[iPos++] = v_strOldBuf[i];
				break;
		}//switch(v_strOldBuf[i])
	}//for i
	
	strTemp[0]=0x7e;
	strTemp[iPos++]=0x7e;

	if (v_bNeedEncode)
	{	

//		return EncodeBuf( strTemp, v_strNewBuf, iPos );
		return 0;
	}
	else
	{
		memcpy(v_strNewBuf,strTemp,iPos);
		return iPos;
	}
}
//}


int CPublicFunction::SevenToEight(char *cSource, int len, char *OUT_pEightBitData,int iOutBufferLen)
{
	CString     strMid,strTemp = "";
	CString     strConvert="";
	CString     strAdd="";
	char        cConvert[9];
    int         ln,n,i,pos = 0;
	int         nCount;
	
	for(i=0;i<iOutBufferLen;i++)
		OUT_pEightBitData[i]  =0x00;
	
	//查表，并把数据转换位01字符串
	
	for(i=0; i<len; i++)
	{
		strAdd = "";
		
		itoa((BYTE)cSource[i],cConvert,2);
		ln = (int)strlen(cConvert);
		
		for(n=0; n < 8-ln; n++)
			strAdd += "0";
		strAdd += (CString)cConvert;
		
		//取出有效位7位
		strTemp += strAdd.Mid(1,7);
	}
	
	
	i = pos = 0;
	nCount = strTemp.GetLength() / 8;
	for(n=0; n<nCount; n++)
	{
		strMid = strTemp.Mid(pos,8);
		
		OUT_pEightBitData[i]  = (strMid[0] - 0x30)*128 +  (strMid[1] - 0x30)*64  +
			(strMid[2] - 0x30)*32  +  (strMid[3] - 0x30)*16  +
			(strMid[4] - 0x30)*8   +  (strMid[5] - 0x30)*4   +
			(strMid[6] - 0x30)*2   +  (strMid[7] - 0x30)*1;
		pos += 8;
		i++;
	}
	return i;
}

BOOL CPublicFunction::ConvertFloatToStdGps(double _dbGps, byte *_pStdGPS)
{
	if ( NULL == _pStdGPS)
	{
		return false;
	}

	double          v_dbTemp(0.0);
	byte            v_bTemp(0);
	short           v_sTemp(0);
	int             v_iTemp(0);

	v_dbTemp = _dbGps;
	//----------整数度
	v_bTemp = v_dbTemp;
	memcpy(_pStdGPS, &v_bTemp, 1);
	//----------整数分
	v_dbTemp = (v_dbTemp - v_bTemp)*60;
	v_bTemp = v_dbTemp;
	memcpy(_pStdGPS+1, &v_bTemp, 1);
	//----------小数分
	v_dbTemp = (v_dbTemp - v_bTemp)*100;
	v_bTemp = v_dbTemp;
	memcpy(_pStdGPS+2, &v_bTemp, 1);

	v_dbTemp = (v_dbTemp - v_bTemp)*100;
	v_bTemp = v_dbTemp;
	memcpy(_pStdGPS+3, &v_bTemp, 1);

	return true;
}
