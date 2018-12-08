#pragma once

#include <eh.h>

class CSE
{
public:
	static void MapSEtoCE()
	{ 
		_set_se_translator(TranslateSEtoCE); 
	}

	operator DWORD() 
	{ 
		return (m_er.ExceptionCode); 
	}
	PVOID GetExpAddress()
	{
		return (m_er.ExceptionAddress);
	}	
private:

	CSE(PEXCEPTION_POINTERS pep)
	{
		m_er      = *pep->ExceptionRecord;
		m_context = *pep->ContextRecord;
	}

	static void _cdecl TranslateSEtoCE(UINT dwEC,
		PEXCEPTION_POINTERS pep)
	{
		throw CSE(pep);
	}

private:
	EXCEPTION_RECORD m_er;      // CPU independent exception information
	CONTEXT          m_context; // CPU dependent exception information
};

//转换出错误信息
std::string GetErrorInfo(const char *_pMoifyTime, const char *_pFileName, DWORD _dwLine, const char *_pFuncName, CSE se)
{
    char	v_szExpInfo[3000]= {0};
    sprintf(v_szExpInfo,"Func(%s):ModifyTime(%s) FileName(%s) LineNum(%d) Address(%08x) ErrorType:",
            _pFuncName,
            _pMoifyTime,
            _pFileName,
            _dwLine,
            se.GetExpAddress()
           );

    switch(se)
    {
    case EXCEPTION_ACCESS_VIOLATION:
        strcat(v_szExpInfo,"EXCEPTION_ACCESS_VIOLATION");
        break;

    case EXCEPTION_DATATYPE_MISALIGNMENT:
        strcat(v_szExpInfo,"EXCEPTION_DATATYPE_MISALIGNMENT");
        break;

    case EXCEPTION_BREAKPOINT:
        strcat(v_szExpInfo,"EXCEPTION_BREAKPOINT");
        break;

    case EXCEPTION_SINGLE_STEP:
        strcat(v_szExpInfo,"EXCEPTION_SINGLE_STEP");
        break;

    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        strcat(v_szExpInfo,"EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
        break;

    case EXCEPTION_FLT_DENORMAL_OPERAND:
        strcat(v_szExpInfo,"EXCEPTION_FLT_DENORMAL_OPERAND");
        break;

    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        strcat(v_szExpInfo,"EXCEPTION_FLT_DIVIDE_BY_ZERO");
        break;

    case EXCEPTION_FLT_INEXACT_RESULT:
        strcat(v_szExpInfo,"EXCEPTION_FLT_INEXACT_RESULT");
        break;

    case EXCEPTION_FLT_INVALID_OPERATION:
        strcat(v_szExpInfo,"EXCEPTION_FLT_INVALID_OPERATION");
        break;

    case EXCEPTION_FLT_OVERFLOW:
        strcat(v_szExpInfo,"EXCEPTION_FLT_OVERFLOW");
        break;

    case EXCEPTION_FLT_STACK_CHECK:
        strcat(v_szExpInfo,"EXCEPTION_FLT_STACK_CHECK");
        break;

    case EXCEPTION_FLT_UNDERFLOW:
        strcat(v_szExpInfo,"EXCEPTION_FLT_UNDERFLOW");
        break;

    case EXCEPTION_INT_DIVIDE_BY_ZERO:
        strcat(v_szExpInfo,"EXCEPTION_INT_DIVIDE_BY_ZERO");
        break;

    case EXCEPTION_INT_OVERFLOW:
        strcat(v_szExpInfo,"EXCEPTION_INT_OVERFLOW");
        break;

    case EXCEPTION_PRIV_INSTRUCTION:
        strcat(v_szExpInfo,"EXCEPTION_PRIV_INSTRUCTION");
        break;

    case EXCEPTION_IN_PAGE_ERROR:
        strcat(v_szExpInfo,"EXCEPTION_IN_PAGE_ERROR");
        break;

    case EXCEPTION_ILLEGAL_INSTRUCTION:
        strcat(v_szExpInfo,"EXCEPTION_ILLEGAL_INSTRUCTION");
        break;

    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
        strcat(v_szExpInfo,"EXCEPTION_NONCONTINUABLE_EXCEPTION");
        break;

    case EXCEPTION_STACK_OVERFLOW:
        strcat(v_szExpInfo,"EXCEPTION_STACK_OVERFLOW");
        break;

    case EXCEPTION_INVALID_DISPOSITION:
        strcat(v_szExpInfo,"EXCEPTION_INVALID_DISPOSITION");
        break;

    case EXCEPTION_GUARD_PAGE:
        strcat(v_szExpInfo,"EXCEPTION_GUARD_PAGE");
        break;

    case EXCEPTION_INVALID_HANDLE:
        strcat(v_szExpInfo,"EXCEPTION_INVALID_HANDLE");
        break;

    default:
        strcat(v_szExpInfo,"EXCEPTION_UNKNOW");
        break;
    }

    return std::string(v_szExpInfo);
}