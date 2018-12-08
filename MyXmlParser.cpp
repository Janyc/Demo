// MyXmlParser.cpp: implementation of the MyXmlParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyXmlParser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyXmlParser::CMyXmlParser()
{
    ZeroMemory(m_szLastError,sizeof(m_szLastError));

    m_xmlDoc = NULL;
    ::CoInitialize(NULL);
}

CMyXmlParser::CMyXmlParser( std::string _strFile ) : m_strFilePath(_strFile)
{
    m_xmlDoc = NULL;
    ::CoInitialize(NULL);
}

CMyXmlParser::~CMyXmlParser()
{
    if( m_xmlDoc != NULL )
    {
        m_xmlDoc.Release();
        m_xmlDoc = NULL;
    }
	::CoUninitialize();
}

BOOL CMyXmlParser::LoadFile( std::string _strFile /*= ""*/ )
{
    if (!_strFile.empty())
    {
        m_strFilePath = _strFile;
    }
    else if (m_strFilePath.empty())
    {
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"������ص��ļ���Ϊ��");

        return FALSE;
    }
    
    //# ���ͷ�
    if( m_xmlDoc != NULL )
    {
        m_xmlDoc.Release();
        m_xmlDoc = NULL;
    }
    
    //# ��ʼ��XML�ĵ�����
    if(SUCCEEDED(m_xmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument30)))) 
    {
        //# ����XML�ַ���
        m_xmlDoc->async = false;
        if( VARIANT_TRUE == m_xmlDoc->load((_bstr_t)m_strFilePath.c_str()))
        {
            return TRUE;
        }
        else
        {
            ZeroMemory(m_szLastError,sizeof(m_szLastError));
            ::sprintf(m_szLastError,"����XML�ĵ�ʧ��");
        }
    }
    else
    {
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"��ʼ��XML�ĵ�����ʧ��");
    }

	return FALSE;
}
BOOL CMyXmlParser::CreateFile( std::string _strFile /*= ""*/, std::string _strRootName /*= ""*/ )
{
    if (!_strFile.empty())
    {
        m_strFilePath = _strFile;
    }
    else if (m_strFilePath.empty())
    {
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"���󴴽����ļ���Ϊ��");

        return FALSE;
    }
    
    //# ���ͷ�
    if( m_xmlDoc != NULL )
    {
        m_xmlDoc.Release();
        m_xmlDoc = NULL;
    }
    
    //# ��ʼ��XML�ĵ�����
    if(!SUCCEEDED(m_xmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument30)))) 
    {
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"��ʼ��XML�ĵ�����ʧ��");

        return FALSE;
    }
    
    //# ����XML�ַ���
    m_xmlDoc->async = false;
    
    char szLastError[1024] = {0};
    try
    {
        //# �����ĵ�˵��
        MSXML2::IXMLDOMProcessingInstructionPtr pProInstruction = NULL;
        pProInstruction = m_xmlDoc->createProcessingInstruction(L"xml", L"version=\"1.0\" encoding=\"utf-8\"");
        m_xmlDoc->appendChild((MSXML2::IXMLDOMNode*)pProInstruction);
        
        //# �������ڵ�
        std::string strRootName = "root";
        if (!_strRootName.empty())
        {
            strRootName = _strRootName;
        }

        MSXML2::IXMLDOMElementPtr xmlRoot = NULL;
        xmlRoot = m_xmlDoc->createElement((_bstr_t)strRootName.c_str());
        if( xmlRoot != NULL )
        {
            m_xmlDoc->appendChild(xmlRoot); //# ������ڵ�
        }

        m_xmlDoc->save((_bstr_t)m_strFilePath.c_str());

        return TRUE;
    }
    catch (CMemoryException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (CFileException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (...)
    {
        ::sprintf(szLastError,"δ֪");
    }
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"�����ĵ���ʼ�����ݲ��ұ���ʱ�����쳣:%s",szLastError);

    return FALSE;
}

BOOL CMyXmlParser::AppendNode( std::string _strNode, std::string _strValue /*= ""*/, std::string _strParentNode /*= ""*/ )
{
    char szLastError[1024] = {0};

    if (NULL == m_xmlDoc && !LoadFile())
    {
        ::memcpy(szLastError,m_szLastError,sizeof(szLastError));

        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"ȫ��XML�ĵ�����Ϊ�������¼���ʧ��:%s",szLastError);
        
        return FALSE;
    }
    
    try
    {
        if (!_strParentNode.empty())
        {
            std::string strParentNode = "//";//# ǰ���"//"��ʾ����һ���ڵ��²���
            strParentNode += _strParentNode;

            //# ��λ�����ڵ�
            MSXML2::IXMLDOMNodePtr xmlFatherNode = NULL;
            xmlFatherNode = m_xmlDoc->selectSingleNode((_bstr_t)strParentNode.c_str());
            if( NULL == xmlFatherNode )
            {
                ZeroMemory(m_szLastError,sizeof(m_szLastError));
                ::sprintf(m_szLastError,"��ѯ�����ڵ�<%s>",_strParentNode.c_str());

                return FALSE;
            }

            //# �����µĽڵ�
            MSXML2::IXMLDOMElementPtr xmlNewElement = NULL;
            xmlNewElement = m_xmlDoc->createElement((_bstr_t)_strNode.c_str());
            if( NULL == xmlNewElement )
            {
                ZeroMemory(m_szLastError,sizeof(m_szLastError));
                ::sprintf(m_szLastError,"�����ڵ�<%s>ʧ��",_strNode.c_str());

                return FALSE;
            }
            xmlNewElement->Puttext((_bstr_t)_strValue.c_str());

            // #���뵽���ڵ�֮��
            if( NULL == xmlFatherNode->appendChild((MSXML2::IXMLDOMNodePtr)xmlNewElement) )
            {
                ZeroMemory(m_szLastError,sizeof(m_szLastError));
                ::sprintf(m_szLastError,"����ڵ�<%s>ʧ��",_strNode.c_str());

                return FALSE;
		    }
        }
        else
        {
            //# ��ø��ڵ�
            MSXML2::IXMLDOMElementPtr xmlRoot = NULL;
            xmlRoot = m_xmlDoc->GetdocumentElement();
            if (NULL == xmlRoot)
            {
                ZeroMemory(m_szLastError,sizeof(m_szLastError));
                ::sprintf(m_szLastError,"��ø��ڵ�ʧ��");

                return FALSE;
            }

            //# �����µĽڵ�
//             MSXML2::IXMLDOMNodePtr xmlNewElement = NULL;
//             xmlNewElement = m_xmlDoc->createNode((_variant_t)(long)1 //# NODE_ELEMENT
//                 , (_bstr_t)_strNode.c_str(), (_bstr_t)(char*)"");
            MSXML2::IXMLDOMElementPtr xmlNewElement = NULL;
            xmlNewElement = m_xmlDoc->createElement((_bstr_t)_strNode.c_str());
            if (NULL == xmlNewElement)
            {
                ZeroMemory(m_szLastError,sizeof(m_szLastError));
                ::sprintf(m_szLastError,"�����ڵ�<%s>ʧ��",_strNode.c_str());

                return FALSE;
            }
            xmlNewElement->Puttext((_bstr_t)_strValue.c_str());

            //# ���뵽���ڵ���
            if(NULL == xmlRoot->appendChild((MSXML2::IXMLDOMNodePtr)xmlNewElement))
            {
                ZeroMemory(m_szLastError,sizeof(m_szLastError));
                ::sprintf(m_szLastError,"����ڵ�<%s>ʧ��",_strNode.c_str());

                return FALSE;
            }
        }

        return TRUE;
    }
    catch (CMemoryException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (CFileException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError)); 
    }
    catch (...)
    {
        ::sprintf(szLastError,"δ֪"); 
    }    
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"�����½ڵ�<%s/%s>�����쳣:%s"
        ,_strParentNode.c_str(),_strNode.c_str(),szLastError);

    return FALSE;
}

BOOL CMyXmlParser::SetAttribute( std::string _strNode, std::string _strAttr, std::string _strValue )
{
    if (_strNode.empty())
    {
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"�����ѯ�Ľڵ���Ϊ��");
        
        return FALSE;
    }
    
    char szLastError[1024] = {0};
    if (NULL == m_xmlDoc && !LoadFile())
    {
        ::memcpy(szLastError,m_szLastError,sizeof(szLastError));
        
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"ȫ��XML�ĵ�����Ϊ�������¼���ʧ��:%s",szLastError);
        
        return FALSE;
    }
    
    try
    {
        std::string strNode = "//";//# ǰ���"//"��ʾ����һ���ڵ��²���
        strNode += _strNode;

        //# ��λ���ڵ�
        MSXML2::IXMLDOMElementPtr xmlElement = NULL;
        xmlElement = m_xmlDoc->selectSingleNode((_bstr_t)strNode.c_str());
        if( NULL == xmlElement )
        {
            ZeroMemory(m_szLastError,sizeof(m_szLastError));
            ::sprintf(m_szLastError,"��ѯ�����ڵ�<%s>",strNode.c_str());

            return FALSE;
        }

        //# ��������ֵ
        xmlElement->setAttribute((_bstr_t)_strAttr.c_str(),(_bstr_t)_strValue.c_str());

        return TRUE;
    }
    catch (CMemoryException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (CFileException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (...)
    {
        ::sprintf(szLastError,"δ֪");
    }
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"���ýڵ�<%s>����[%s]ֵ[%s]�����쳣:%s"
        ,_strNode.c_str(),_strAttr.c_str(),_strValue.c_str(),szLastError);

    return FALSE;
}

BOOL CMyXmlParser::GetAttribute( std::string _strNode, std::string _strAttr, std::string& _strValue )
{
    if (_strNode.empty())
    {
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"�����ѯ�Ľڵ���Ϊ��");
        
        return FALSE;
    }

    char szLastError[1024] = {0};
    if (NULL == m_xmlDoc && !LoadFile())
    {
        ::memcpy(szLastError,m_szLastError,sizeof(szLastError));
        
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"ȫ��XML�ĵ�����Ϊ�������¼���ʧ��:%s",szLastError);
        
        return FALSE;
    }
    
    try
    {
        std::string strNode = "//";//# ǰ���"//"��ʾ����һ���ڵ��²���
        strNode += _strNode;

        //# ��λ���ڵ�
        MSXML2::IXMLDOMElementPtr xmlElement = NULL;
        xmlElement = m_xmlDoc->selectSingleNode((_bstr_t)strNode.c_str());
        if( NULL == xmlElement )
        {
            ZeroMemory(m_szLastError,sizeof(m_szLastError));
            ::sprintf(m_szLastError,"��ѯ�����ڵ�<%s>",strNode.c_str());

            return FALSE;
        }

        //# ��ȡ����ֵ
        _variant_t varValue = xmlElement->getAttribute(_strAttr.c_str());
        CString cstrValue = varValue.bstrVal;
        _strValue = cstrValue.GetBuffer(0);

        return TRUE;
    }
    catch (CMemoryException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (CFileException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (...)
    {
        ::sprintf(szLastError,"δ֪");
    }

    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"��ѯ�ڵ�<%s>����[%s]ֵ�����쳣:s",_strNode.c_str(),_strAttr.c_str(),szLastError);
    
    return FALSE;
}

BOOL CMyXmlParser::SaveFile( std::string _strFile /*= ""*/ )
{
    char szLastError[1024] = {0};
    try
    {
        if (m_xmlDoc != NULL)
        {
            if (!_strFile.empty())
            {
                m_xmlDoc->save((_bstr_t)_strFile.c_str());
            } 
            else if (!m_strFilePath.empty())
            {
                m_xmlDoc->save((_bstr_t)m_strFilePath.c_str());
            }
        }
        
        return TRUE;
    }
    catch (CMemoryException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (CFileException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (...)
    {
        ::sprintf(szLastError,"δ֪");
    }
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"�����ļ������쳣:%s",szLastError);
    
    return FALSE;
}

BOOL CMyXmlParser::CheckNode( std::string _strNode, std::string _strParentNode /*= ""*/ )
{
    if (_strNode.empty())
    {
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"�����ѯ�Ľڵ���Ϊ��");
        
        return FALSE;
    }
    
    char szLastError[1024] = {0};
    if (NULL == m_xmlDoc && !LoadFile())
    {
        ::memcpy(szLastError,m_szLastError,sizeof(szLastError));
        
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"ȫ��XML�ĵ�����Ϊ�������¼���ʧ��:%s",szLastError);
        
        return FALSE;
    }

    try
    {
        std::string strNode = "//";//# ǰ���"//"��ʾ����һ���ڵ��²���
        if (!_strParentNode.empty())
        {
            strNode += _strParentNode;
            strNode += "//";
        }
        
        strNode += _strNode;
        
        //# ��λ���ڵ�
        MSXML2::IXMLDOMNodePtr xmlNode = NULL;
        xmlNode = m_xmlDoc->selectSingleNode((_bstr_t)strNode.c_str());
        if( NULL == xmlNode )
        {
            ZeroMemory(m_szLastError,sizeof(m_szLastError));
            ::sprintf(m_szLastError,"��ѯ�����ڵ�<%s>",strNode.c_str());

            return FALSE;
        }

        return TRUE;
    }
    catch (CMemoryException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (CFileException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (...)
    {
        ::sprintf(szLastError,"δ֪");
    }
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"��ѯ�ڵ�<%s/%s>�����쳣:%s"
        ,_strParentNode.c_str(),_strNode.c_str(),szLastError);
    
    return FALSE;
}
BOOL CMyXmlParser::GetNodeText( std::string _strNode, std::string& _strValue )
{
    if (_strNode.empty())
    {
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"�����ѯ�Ľڵ���Ϊ��");
        
        return FALSE;
    }
    
    char szLastError[1024] = {0};
    if (NULL == m_xmlDoc && !LoadFile())
    {
        ::memcpy(szLastError,m_szLastError,sizeof(szLastError));
        
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"ȫ��XML�ĵ�����Ϊ�������¼���ʧ��:%s",szLastError);
        
        return FALSE;
    }
    
    try
    {
        std::string strNode = "//";//# ǰ���"//"��ʾ����һ���ڵ��²���
        strNode += _strNode;
        
        //# ��λ���ڵ�
        MSXML2::IXMLDOMNodePtr xmlNode = NULL;
        xmlNode = m_xmlDoc->selectSingleNode((_bstr_t)strNode.c_str());
        if( NULL == xmlNode )
        {
            ZeroMemory(m_szLastError,sizeof(m_szLastError));
            ::sprintf(m_szLastError,"��ѯ�����ڵ�<%s>",strNode.c_str());

            return FALSE;
        }

        _strValue = xmlNode->text.operator const char*();
        
        return TRUE;
    }
    catch (CMemoryException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (CFileException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (...)
    {
        ::sprintf(szLastError,"δ֪");
    }
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"��ȡ�ڵ�<%s>ֵ�����쳣:%s",_strNode.c_str(),szLastError);
    
    return FALSE;
}
const char* CMyXmlParser::GetLastError()
{
    return m_szLastError;
}
BOOL CMyXmlParser::LoadXml( std::string _strXml )
{
    if (_strXml.empty())
    {
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"����������ַ���Ϊ��");
        
        return FALSE;
    }
    
    //# ���ͷ�
    if( m_xmlDoc != NULL )
    {
        m_xmlDoc.Release();
        m_xmlDoc = NULL;
    }
    
    char szLastError[1024] = {0};
    try
    {    
        //# ��ʼ��XML�ĵ�����
        if(SUCCEEDED(m_xmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument30)))) 
        {
            //# ����XML�ַ���
            m_xmlDoc->async = false;
            if( VARIANT_TRUE == m_xmlDoc->loadXML((_bstr_t)_strXml.c_str()))
            {
                return TRUE;
            }
        }
    }
    catch (CMemoryException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (CFileException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (...)
    {
        ::sprintf(m_szLastError,"δ֪");
    }
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"����XML��ʽ����ʱ�����쳣:%s",szLastError);
    
	return FALSE;
}
BOOL CMyXmlParser::CreateXml( std::string _strRootName /*= ""*/ )
{
    //# ���ͷ�
    if( m_xmlDoc != NULL )
    {
        m_xmlDoc.Release();
        m_xmlDoc = NULL;
    }
    
    char szLastError[1024] = {0};
    try
    {    
        //# ��ʼ��XML�ĵ�����
        if(!SUCCEEDED(m_xmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument30)))) 
        {
            ZeroMemory(m_szLastError,sizeof(m_szLastError));
            ::sprintf(m_szLastError,"��ʼ��XML�ĵ�����ʧ��");
        
            return FALSE;
        }
    
        //# ����XML�ַ���
        m_xmlDoc->async = false;

        //# �����ĵ�˵��
        MSXML2::IXMLDOMProcessingInstructionPtr pProInstruction = NULL;
        pProInstruction = m_xmlDoc->createProcessingInstruction(L"xml", L"version=\"1.0\" encoding=\"utf-8\"");
        m_xmlDoc->appendChild((MSXML2::IXMLDOMNode*)pProInstruction);
        
        //# �������ڵ�
        std::string strRootName = "root";
        if (!_strRootName.empty())
        {
            strRootName = _strRootName;
        }
        
        MSXML2::IXMLDOMElementPtr xmlRoot = NULL;
        xmlRoot = m_xmlDoc->createElement((_bstr_t)strRootName.c_str());
        if( xmlRoot != NULL )
        {
            m_xmlDoc->appendChild(xmlRoot); //# ������ڵ�
        }
        
        return TRUE;
    }
    catch (CMemoryException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (CFileException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (...)
    {
        ::sprintf(szLastError,"δ֪");
    }
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"����XML��ʽ����ʱ�����쳣:%s",szLastError);
    
    return FALSE;
}

std::string CMyXmlParser::GetXml()
{
    if (NULL == m_xmlDoc)
    {
        return "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    }

    return m_xmlDoc->Getxml().operator const char*();
}
BOOL CMyXmlParser::SetNodeText( std::string _strNode, std::string _strValue )
{
    if (_strNode.empty())
    {
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"�����ѯ�Ľڵ���Ϊ��");
        
        return FALSE;
    }
    
    char szLastError[1024] = {0};
    if (NULL == m_xmlDoc && !LoadFile())
    {
        ::memcpy(szLastError,m_szLastError,sizeof(szLastError));
        
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"ȫ��XML�ĵ�����Ϊ�������¼���ʧ��:%s",szLastError);
        
        return FALSE;
    }
    
    try
    {
        std::string strNode = "//";//# ǰ���"//"��ʾ����һ���ڵ��²���
        strNode += _strNode;
        
        //# ��λ���ڵ�
        MSXML2::IXMLDOMNodePtr xmlNode = NULL;
        xmlNode = m_xmlDoc->selectSingleNode((_bstr_t)strNode.c_str());
        if( NULL == xmlNode )
        {
            ZeroMemory(m_szLastError,sizeof(m_szLastError));
            ::sprintf(m_szLastError,"��ѯ�����ڵ�<%s>",strNode.c_str());
            
            return FALSE;
        }
        
        xmlNode->Puttext((_bstr_t)_strValue.c_str());
        
        return TRUE;
    }
    catch (CMemoryException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (CFileException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (...)
    {
        ::sprintf(szLastError,"δ֪");
    }
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"���ýڵ�<%s>ֵ�����쳣:%s",_strNode.c_str(),szLastError);
    
    return FALSE;
}
BOOL CMyXmlParser::GetStaffInfos( std::vector<CStaffInfo> &_vctStaffInfos )
{
//     <?xml version="1.0" encoding="utf-8" ?>
//     <param>
//         <!--�ն��ֻ���-->
//         <mobile>13800010001</mobile>
//         <!--���ƺ�-->
//         <vehiclelic>��D10011</vehiclelic>
//         <!--��Ա��Ϣ-->
//         <items>
//             <item>
//             ����<!--��Ա���ͣ� 1˾��  2Ѻ��Ա-->
//             ����<type>1</type>
//             ����<!--IC�����-->
//             ����<icno></icno>
//             ����<!--��Ա��ϵͳ�б�� GUID-->
//             ����<idno>30842738-AAB4-4c69-B81C-6C9E91157B0B</idno>
//             ����<!--����-->
//             ����<name>����</name>
//             ����<!--�Ƿ�ӵ��� 1��  0��-->
//             ����<isneeded>1</isneeded>
//         ����</item>
//         </items>
//     </param>
    
    char szLastError[1024] = {0};
    if (NULL == m_xmlDoc && !LoadFile())
    {
        ::memcpy(szLastError,m_szLastError,sizeof(szLastError));
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"ȫ��XML�ĵ�����Ϊ�������¼���ʧ��:%s",szLastError);
        
        return FALSE;
    }
    
    try
    {
        //# ���Ҹ��ڵ��µ�����<item>�ڵ�
        MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
        pNodeList = m_xmlDoc->selectNodes(L"param//items//item");
        if( NULL != pNodeList )
        {
            for (int i=0;i<pNodeList->Getlength();i++)
            {
                MSXML2::IXMLDOMNodePtr xmlStaffNode = pNodeList->Getitem(i);
                if (NULL != xmlStaffNode)
                {
                    CStaffInfo Staff;

                    MSXML2::IXMLDOMNodePtr xmlNode = xmlStaffNode->selectSingleNode(L"type");
                    if (NULL == xmlNode)
                    {
                        continue;
                    }
                    Staff.m_byType = ::atoi(xmlNode->text.operator const char*());

                    xmlNode = xmlStaffNode->selectSingleNode(L"icno");
                    if (NULL == xmlNode)
                    {
                        continue;
                    }
                    Staff.m_strICNo = xmlNode->text.operator const char*();
                    
                    xmlNode = xmlStaffNode->selectSingleNode(L"idno");
                    if (NULL == xmlNode)
                    {
                        continue;
                    }
                    Staff.m_strIDNo = xmlNode->text.operator const char*();
                    
                    xmlNode = xmlStaffNode->selectSingleNode(L"name");
                    if (NULL == xmlNode)
                    {
                        continue;
                    }
                    Staff.m_strName = xmlNode->text.operator const char*();
                    
                    xmlNode = xmlStaffNode->selectSingleNode(L"isneeded");
                    if (NULL == xmlNode)
                    {
                        continue;
                    }
                    Staff.m_byIsNeed = ::atoi(xmlNode->text.operator const char*());

                    _vctStaffInfos.push_back(Staff);
                }//if (NULL != xmlStaffNode)
            }//for (int i=0;i<pNodeList->length();i++)
        }//if( NULL != pNodeList )
        
        return TRUE;
    }
    catch (CMemoryException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (CFileException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (...)
    {
        ::sprintf(szLastError,"δ֪");
    }
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"��ȡ�ӵ�����Ϣ�б�����쳣:%s",szLastError);
    
    return FALSE;
}
BOOL CMyXmlParser::GetOilInfos( std::vector<COilInfo> &_vctOilInfos )
{
//     <?xml version="1.0" encoding="utf-8" ?>
//     <param>
//         <!--�ն��ֻ���-->
//         <mobile>13800010001</mobile>
//         <!--���ƺ�-->
//         <vehiclelic>��D10011</vehiclelic>
//         <!--��λ�仯��ֵ-->
//         <items>
//         ����<item>
//             ����<!--��λ��������� ��Χ1~99-->
//             ����<bh>1</bh>
//             ����<!--��λ�仯��ֵ���ٷֱ�ֵ,2.1%-->
//             ����<yz>2.1</yz>
//         ����</item>
//         </items>
//     </param>
    
    char szLastError[1024] = {0};
    if (NULL == m_xmlDoc && !LoadFile())
    {
        ::memcpy(szLastError,m_szLastError,sizeof(szLastError));
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"ȫ��XML�ĵ�����Ϊ�������¼���ʧ��:%s",szLastError);
        
        return FALSE;
    }
    
    try
    {
        //# ���Ҹ��ڵ��µ�����<item>�ڵ�
        MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
        pNodeList = m_xmlDoc->selectNodes(L"param//items//item");
        if( NULL != pNodeList )
        {
            for (int i=0;i<pNodeList->Getlength();i++)
            {
                MSXML2::IXMLDOMNodePtr xmlOilNode = pNodeList->Getitem(i);
                if (NULL != xmlOilNode)
                {
                    COilInfo OilInfo;
                    
                    MSXML2::IXMLDOMNodePtr xmlNode = xmlOilNode->selectSingleNode(L"bh");
                    if (NULL == xmlNode)
                    {
                        continue;
                    }
                    OilInfo.m_byBH = ::atoi(xmlNode->text.operator const char*());
                    
                    xmlNode = xmlOilNode->selectSingleNode(L"yz");
                    if (NULL == xmlNode)
                    {
                        continue;
                    }
                    OilInfo.m_fYZ = ::atof(xmlNode->text.operator const char*());
                    
                    _vctOilInfos.push_back(OilInfo);
                }//if (NULL != xmlOilNode)
            }//for (int i=0;i<pNodeList->length();i++)
        }//if( NULL != pNodeList )
        
        return TRUE;
    }
    catch (CMemoryException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (CFileException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (...)
    {
        ::sprintf(szLastError,"δ֪");
    }
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"��ȡ��λ��ֵ�����б�����쳣:%s",szLastError);
    
    return FALSE;
}
BOOL CMyXmlParser::AppendNodeBefore( std::string _strNode, std::string _strNode2, std::string _strValue /*= ""*/ )
{
    char szLastError[1024] = {0};

    if (NULL == m_xmlDoc && !LoadFile())
    {
        ::memcpy(szLastError,m_szLastError,sizeof(szLastError));

        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"ȫ��XML�ĵ�����Ϊ�������¼���ʧ��:%s",szLastError);
        
        return FALSE;
    }
    
    try
    {
        std::string strNextNode = "//";//# ǰ���"//"��ʾ����һ���ڵ��²���
        strNextNode += _strNode2;
        
        //# ��λ���ڵ�
        MSXML2::IXMLDOMNodePtr xmlNextNode = NULL;
        xmlNextNode = m_xmlDoc->selectSingleNode((_bstr_t)strNextNode.c_str());
        if( NULL == xmlNextNode )
        {
            ZeroMemory(m_szLastError,sizeof(m_szLastError));
            ::sprintf(m_szLastError,"��ѯ�����ڵ�<%s>",_strNode2.c_str());
            
            return FALSE;
        }
        MSXML2::IXMLDOMNodePtr xmlParentNode = xmlNextNode->GetparentNode();
        if (NULL == xmlParentNode)
        {
            ZeroMemory(m_szLastError,sizeof(m_szLastError));
            ::sprintf(m_szLastError,"��ѯ�����ڵ�<%s>�ĸ��ڵ�",_strNode2.c_str());
            
            return FALSE;
        }
        
        //# �����µĽڵ�
        MSXML2::IXMLDOMElementPtr xmlNewElement = NULL;
        xmlNewElement = m_xmlDoc->createElement((_bstr_t)_strNode.c_str());
        if( NULL == xmlNewElement )
        {
            ZeroMemory(m_szLastError,sizeof(m_szLastError));
            ::sprintf(m_szLastError,"�����ڵ�<%s>ʧ��",_strNode.c_str());
            
            return FALSE;
        }
        xmlNewElement->Puttext((_bstr_t)_strValue.c_str());
        
        // #���뵽���ڵ�֮��
        xmlParentNode->insertBefore(xmlNewElement,(_variant_t)(IDispatch*)xmlNextNode);
        return TRUE;
    }
    catch (CMemoryException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (CFileException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError)); 
    }
    catch (...)
    {
        ::sprintf(szLastError,"δ֪"); 
    }    
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"�����½ڵ�<%s>�����쳣:%s",_strNode.c_str(),szLastError);

    return FALSE;
}
BOOL CMyXmlParser::AppendNodeFront( std::string _strNode, std::string _strValue /*= ""*/, std::string _strParentNode /*= ""*/ )
{
    char szLastError[1024] = {0};
    
    if (NULL == m_xmlDoc && !LoadFile())
    {
        ::memcpy(szLastError,m_szLastError,sizeof(szLastError));
        
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"ȫ��XML�ĵ�����Ϊ�������¼���ʧ��:%s",szLastError);
        
        return FALSE;
    }
    
    try
    {
        if (!_strParentNode.empty())
        {
            std::string strParentNode = "//";//# ǰ���"//"��ʾ����һ���ڵ��²���
            strParentNode += _strParentNode;
            
            //# ��λ�����ڵ�
            MSXML2::IXMLDOMNodePtr xmlFatherNode = NULL;
            xmlFatherNode = m_xmlDoc->selectSingleNode((_bstr_t)strParentNode.c_str());
            if( NULL == xmlFatherNode )
            {
                ZeroMemory(m_szLastError,sizeof(m_szLastError));
                ::sprintf(m_szLastError,"��ѯ�����ڵ�<%s>",_strParentNode.c_str());
                
                return FALSE;
            }
            
            //# �����µĽڵ�
            MSXML2::IXMLDOMElementPtr xmlNewElement = NULL;
            xmlNewElement = m_xmlDoc->createElement((_bstr_t)_strNode.c_str());
            if( NULL == xmlNewElement )
            {
                ZeroMemory(m_szLastError,sizeof(m_szLastError));
                ::sprintf(m_szLastError,"�����ڵ�<%s>ʧ��",_strNode.c_str());
                
                return FALSE;
            }
            xmlNewElement->Puttext((_bstr_t)_strValue.c_str());
            
            MSXML2::IXMLDOMNodePtr xmlFirstNode = NULL;
            xmlFirstNode = xmlFatherNode->GetfirstChild();
            if( NULL != xmlFatherNode )
            {
                xmlFatherNode->insertBefore(xmlNewElement,(_variant_t)(IDispatch*)xmlFirstNode);
            }
            else
            {
                // #���뵽���ڵ�֮��
                if( NULL == xmlFatherNode->appendChild((MSXML2::IXMLDOMNodePtr)xmlNewElement) )
                {
                    ZeroMemory(m_szLastError,sizeof(m_szLastError));
                    ::sprintf(m_szLastError,"����ڵ�<%s>ʧ��",_strNode.c_str());
                    
                    return FALSE;
                }
            }
        }
        else
        {
            return AppendNode(_strNode,_strValue);
        }
        
        return TRUE;
    }
    catch (CMemoryException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (CFileException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError)); 
    }
    catch (...)
    {
        ::sprintf(szLastError,"δ֪"); 
    }    
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"�����½ڵ�<%s/%s>�����쳣:%s"
        ,_strParentNode.c_str(),_strNode.c_str(),szLastError);
    
    return FALSE;
}
BOOL CMyXmlParser::GetRouteInfos( std::vector<CRouteInfo> &_vctRouteInfos )
{

    char szLastError[1024] = {0};
    if (NULL == m_xmlDoc && !LoadFile())
    {
        ::memcpy(szLastError,m_szLastError,sizeof(szLastError));
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"ȫ��XML�ĵ�����Ϊ�������¼���ʧ��:%s",szLastError);
        
        return FALSE;
    }
    
    try
    {
        //# ��������<result/ysxl>�ڵ�
        MSXML2::IXMLDOMNodeListPtr pRouteList = NULL;
        pRouteList = m_xmlDoc->selectNodes(L"result//ysxl");
        if( NULL != pRouteList )
        {
            for (int i=0;i<pRouteList->Getlength();i++)
            {
                MSXML2::IXMLDOMNodePtr xmlRouteNode = pRouteList->Getitem(i);
                if (NULL != xmlRouteNode)
                {
                    CRouteInfo RouteInfo;
                    
                    MSXML2::IXMLDOMNodePtr xmlNode = xmlRouteNode->selectSingleNode(L"id");
                    if (NULL == xmlNode)
                    {
                        continue;
                    }
                    RouteInfo.m_strID = xmlNode->text.operator const char*();
                    
                    xmlNode = xmlRouteNode->selectSingleNode(L"wldbh");
                    if (NULL == xmlNode)
                    {
                        continue;
                    }
                    RouteInfo.m_strWLDBH = xmlNode->text.operator const char*();
                    
                    xmlNode = xmlRouteNode->selectSingleNode(L"mc");
                    if (NULL == xmlNode)
                    {
                        continue;
                    }
                    RouteInfo.m_strMC = xmlNode->text.operator const char*();

                    //# ��������<result/ysxl/item>�ڵ�
                    MSXML2::IXMLDOMNodeListPtr pBranchList = NULL;
                    pBranchList = xmlRouteNode->selectNodes(L"item");
                    if( NULL != pBranchList )
                    {
                        for (int j=0;j<pBranchList->Getlength();j++)
                        {
                            MSXML2::IXMLDOMNodePtr xmlBranchNode = pBranchList->Getitem(j);
                            if (NULL != xmlBranchNode)
                            {
                                CRouteBranch Branch;
                                
                                xmlNode = xmlBranchNode->selectSingleNode(L"qdmc");
                                if (NULL == xmlNode)
                                {
                                    continue;
                                }
                                Branch.m_strQDMC = xmlNode->text.operator const char*();
                                
                                xmlNode = xmlBranchNode->selectSingleNode(L"zdmc");
                                if (NULL == xmlNode)
                                {
                                    continue;
                                }
                                Branch.m_strZDMC = xmlNode->text.operator const char*();
                                
                                xmlNode = xmlBranchNode->selectSingleNode(L"xlkd");
                                if (NULL == xmlNode)
                                {
                                    continue;
                                }
                                Branch.m_wXLKD = ::atol(xmlNode->text.operator const char*());

                                //# ��������<result/ysxl/item/jwd/item>�ڵ�
                                MSXML2::IXMLDOMNodeListPtr pPointList = NULL;
                                pPointList = xmlBranchNode->selectNodes(L"jwd//item");
                                if( NULL != pPointList )
                                {
                                    for (int k=0;k<pPointList->Getlength();k++)
                                    {
                                        MSXML2::IXMLDOMNodePtr xmlPointNode = pPointList->Getitem(k);
                                        if (NULL != xmlPointNode)
                                        {
                                            CRoutePoint Point;
                                            
                                            xmlNode = xmlPointNode->selectSingleNode(L"jd");
                                            if (NULL == xmlNode)
                                            {
                                                continue;
                                            }
                                            Point.m_dbLon = ::atof(xmlNode->text.operator const char*());
                                            
                                            xmlNode = xmlPointNode->selectSingleNode(L"wd");
                                            if (NULL == xmlNode)
                                            {
                                                continue;
                                            }
                                            Point.m_dbLat = ::atof(xmlNode->text.operator const char*());
                                            
                                            xmlNode = xmlPointNode->selectSingleNode(L"lx");
                                            if (NULL == xmlNode)
                                            {
                                                continue;
                                            }
                                            Point.m_strLX = xmlNode->text.operator const char*();
                                            
                                            xmlNode = xmlPointNode->selectSingleNode(L"sm");
                                            if (NULL == xmlNode)
                                            {
                                                continue;
                                            }
                                            Point.m_strSM = xmlNode->text.operator const char*();
                                            
                                            Branch.m_vctPoints.push_back(Point);
                                        }//if (NULL != xmlPointNode)
                                    }//for (int k=0;k<pPointList->length();k++)
                                }//if( NULL != pPointList )
                                
                                RouteInfo.m_vctBranchs.push_back(Branch);
                            }//if (NULL != xmlBranchNode)
                        }//for (int j=0;j<pBranchList->length();j++)
                    }//if( NULL != pBranchList )

                    _vctRouteInfos.push_back(RouteInfo);
                }//if (NULL != xmlRouteNode)
            }//for (int i=0;i<pRouteList->length();i++)
        }//if( NULL != pRouteList )
        
        return TRUE;
    }
    catch (CMemoryException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (CFileException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (...)
    {
        ::sprintf(szLastError,"δ֪");
    }
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"��ȡ������·�б�����쳣:%s",szLastError);
    
    return FALSE;
}
BOOL CMyXmlParser::GetEOrders( std::vector<CEOrder> &_vctEOrders )
{
//     <?xml version="1.0" encoding="UTF-8"?>
//     <!--���ݳ������ �ӵ��˱�Ż�ȡ������Ϣ -->
//     <!-- �����ϵͳ�����ȡһ��������ҵ������Ϣ" -->
//     <result>
//         <!--"��ȡ�����ɹ���ʧ�ܣ�true �ɹ���false ʧ��"-->
//         <success>true</success>
//         <!--"��ʾ��Ϣ"-->
//         <message>������Ŵ���</message>
// 
//         <!-- ������������Ϣ������ҵ�� -->
//         <dzzyd>
//             <!--������������Ϣ-->
//             <jbxx>
//                 <!-- ��������� GUID��� -->
//                 <wldbh>30842738-AAB4-4c69-B81C-6C9E91157B0B</wldbh>
//                 <!--��������������-->
//                 <lx>����������������</lx>
//                 <!--������� chari20)-->
//                 <tile>�����������</title>
//                 <!--�ƻ���ʼʱ�� -->
//                 <jhkssj>2012-12-10 09:30:00</jhkssj>
//                 <!--�ƻ����ʱ�� -->
//                 <jhwcsj>2012-12-15 09:30:00</jhwcsj>
//                 <!--�����񵥱�ע��Ϣ char(256)-->
//                 <bz>��������ע��Ϣ</bz>
//             </jbxx>
// 
//             <!--�ӵ�����Ϣ-->
//             <jdr>
//                 <!--comment="���ƺ�" char(10)-->
//                 <cph>��B7A530</cph>
//                 <item>
//                     <!--�ӵ������� 1����ʻԱ��2Ѻ��Ա-->
//                     <lx>1</lx>
//                     <!--comment="�ӵ��˱��" char(40)-->
//                     <bh>483C0430-AB5D-44dc-9414-8AFAB0079451</bh>
//                 </item>
//                 <!--���ڶ���ӵ���-->
//             </jdr>
// 
//             <!--���ػ�����Ϣ-->
//             <cargo>
//                 <!--�������� char(40)-->
//                 <mc>�׻���ϩ�����</mc>
//                 <!--�ƻ�װ�ػ��������� ��λ�� �� char(10)-->
//                 <sl>24.5</sl>
//             </cargo>
// 
//             <!--�ͻ���λ��Ϣ -->
//             <khdw>
//                 <!--��λ����-->
//                 <mc>��λ����</mc>
//                 <!--��ϵ������-->
//                 <lxr>��ϵ������</lxr>
//                 <!--��ϵ�˵绰-->
//                 <lxrdh>��ϵ�˵绰</lxrdh>
//             </khdw>
//             

//             <!--ж���ص���Ϣ -->
//             <zxhdd>
//                 <!--��ж�ص���Ϣ-->
//                 <item>
//                     <!--��� 1 2 3....-->
//                     <xh>1</xh>
// 
//                     <!--�������-->
//                     <qdmc></qdmc>
//                     <!--��㾭��-->
//                     <qdjd></qdjd>
//                     <!--���γ��-->
//                     <qdwd></qdwd>
// 
//                     <!--�յ�����-->
//                     <zdmc></zdmc>
//                     <!--�յ㾭��-->
//                     <zdjd></zdjd>
//                     <!--�յ�γ��-->
//                     <zdwd></zdwd>
// 
//                     <!-- װж����:�����ǣ�װ/ж/;��,���յ�Ĳ�������������ȥ�յ�ж��������Ϊж��ֻ�Ǿ����յ㣬����Ϊ;��-->
//                     <zxhlx>װ/ж/;��</zxhlx>	
//                     <!-- װ��״̬ ���ճ����س�-->
//                     <zzzt></zzzt>
// 
//                     <!-- �������� -->
//                     <pm></pm>
// 
//                     <!--����-->
//                     <sl></sl>
// 
//                     <!--�յ���ϵ��λ-->
//                     <lxdw></lxdw>
// 
//                     <!--�յ���ϵ��-->
//                     <lxr></lxr>
//                     <!--�յ���ϵ�绰-->
//                     <lxdh></lxdh>
//                     <!--��ע��Ϣ-->
//                     <bz></bz>
//                 </item>
//                 <!--�����ж��ж���ص�-->
//                 ...
//             </zxhdd>

//             <!--̥ѹ��ֵ���ñ�-->
//             <ty>
//                 <!--̥ѹ��ֵ����GUID��� -->
//                 <id>1D0418EF-A870-46d8-A02E-103E1817AA47</id>
//                 <!-- ĳ����������Ϣ -->
//                 <item>
//                     <!--����������λ�ñ�ţ�����ʮ��������ֵ��ʾ-->
//                     <wz>0x0211</wz>
//                     <!--��ɫ̥ѹԤ������ֵ ��λ��Bar-->
//                     <ytyxx>20.5</ytyxx>
//                     <!--��ɫ̥ѹԤ������ֵ-->
//                     <ytysx>23</ytysx>
//                     <!--��ɫ̥ѹԤ������ֵ-->
//                     <rtyxx>18</rtyxx>
//                     <!--��ɫ̥ѹԤ������ֵ-->
//                     <rtysx>20.4</rtysx>
//                     
//                     <!--��ɫ̥��Ԥ������ֵ����λ�����϶�-->
//                     <ytwxx>69.3</ytwxx>
//                     <!--��ɫ̥��Ԥ������ֵ-->
//                     <ytwsx>71.3</ytwsx>
//                     <!--��ɫ̥��Ԥ������ֵ-->
//                     <rtwxx>71.4</rtwxx>
//                     <!--��ɫ̥��Ԥ������ֵ-->
//                     <rtwsx>80.4</rtwsx>
//                 </item>
//                 <!--�ж����������Ϣ -->
//             </ty>
// 
//             <!--���Ӳ���Ʊ��Ϣ-->
//             <dzczp>
//                 <!--���Ӳ���Ʊ��� GUID-->
//                 <id>A6432FC6-E65B-4d42-B775-5F3D15EBA020</id>
//                 <!--���Ӳ��������Ϣ-->
//                 <item>
//                     <!--��̱�� GUID-->
//                     <id>FC4CBE4A-90DB-4071-8140-3F53B302063A</id>
//                     <!--������ 1 2 3 ...-->
//                     <xh>2</xh>
//                     <!--���������ƣ������ż졢װ����ж�� ��-->
//                     <lx>ж�����</lx>
//                     <!--�����̼ƻ����ʱ��-->
//                     <jhwcsj>2012-12-10 14:30:00</jhwcsj>
// 
//                     <!--��ҵ��Ϣ-->
//                     <subitem>
//                         <!--��ҵ��� GUID -->
//                         <id>A52A1D49-6D00-43a0-80DE-065E71DAB0F7</id>
//                         <!--��ҵ��� 1 2 3...-->
//                         <xh>1</xh>
//                         <!--��ҵ������ƣ���ȫ��� �ճ�����������װ�ء��س���������ȫ���... ��-->
//                         <lx>��ȫ���</lx>
//                         <!--������ҵ�ƻ����ʱ��-->
//                         <jhwcsj>2012-12-10 14:30:00</jhwcsj>
//                         <!--�������ҵ����-->
//                         <zynr>
//                             <!--��ҵ���ݱ�� GUID-->
//                             <id>DCB25B0C-264E-43e0-A186-89F2FE6CACDE</id>
//                             <!--��ҵ������� 1 2 3...-->
//                             <xh>1</xh>
//                             <!--��ҵ���ݣ���λȷ�� �����ϰ� ��Աȷ��... ��-->
//                             <nr>��λȷ��</nr>
//                         </zynr> 
//                         <zynr>
//                             <id>00780DE7-663D-41f9-9FE4-722C007D20CD</id>
//                             <xh>2</xh>
//                             <nr>�����ϰ�</nr>
//                         </zynr> 
//                         <!--�����ж�����ҵ����-->
//                     </subitem>
//                     <!-- �����ж�����ҵ��Ϣ -->
//                 </item>
//                 <!-- �����ж�����Ӳ������ -->
//             </dzczp>
//         </dzzyd>
//     ����<!-- �����ж���������Ϣ������ҵ�� -->
//     </result>

    char szLastError[1024] = {0};
    if (NULL == m_xmlDoc && !LoadFile())
    {
        ::memcpy(szLastError,m_szLastError,sizeof(szLastError));
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"ȫ��XML�ĵ�����Ϊ�������¼���ʧ��:%s",szLastError);
        
        return FALSE;
    }
    
    try
    {
        //# ��������<result/dzzyd>�ڵ�
        MSXML2::IXMLDOMNodeListPtr pEOrderList = NULL;
        pEOrderList = m_xmlDoc->selectNodes(L"result//dzzyd");
        if( NULL != pEOrderList )
        {
            for (int i=0;i<min(pEOrderList->Getlength(),5);i++)/*������������ò�����5��*/
            {
                MSXML2::IXMLDOMNodePtr xmlEOrderNode = pEOrderList->Getitem(i);
                if (NULL != xmlEOrderNode)
                {
                    CEOrder EOrderInfo;
                    
                    MSXML2::IXMLDOMNodePtr xmlBaseInfoNode = xmlEOrderNode->selectSingleNode(L"jbxx");
                    if (NULL != xmlBaseInfoNode)
                    {
                        MSXML2::IXMLDOMNodePtr xmlNode = xmlBaseInfoNode->selectSingleNode(L"wldbh");
                        if (NULL != xmlNode)
                        {
                            EOrderInfo.m_BaseInfo.m_strWLDBH = xmlNode->text.operator const char*();
                        }
                        
                        xmlNode = xmlBaseInfoNode->selectSingleNode(L"lx");
                        if (NULL != xmlNode)
                        {
                            EOrderInfo.m_BaseInfo.m_strLX = xmlNode->text.operator const char*();
                        }

                        xmlNode = xmlBaseInfoNode->selectSingleNode(L"title");
                        if (NULL != xmlNode)
                        {
                            EOrderInfo.m_BaseInfo.m_strTitle = xmlNode->text.operator const char*();
                        }
                        
                        xmlNode = xmlBaseInfoNode->selectSingleNode(L"jhkssj");
                        if (NULL != xmlNode)
                        {
                            EOrderInfo.m_BaseInfo.m_strJHKSSJ = xmlNode->text.operator const char*();
                        }
                        
                        xmlNode = xmlBaseInfoNode->selectSingleNode(L"jhwcsj");
                        if (NULL != xmlNode)
                        {
                            EOrderInfo.m_BaseInfo.m_strJHWCSJ = xmlNode->text.operator const char*();
                        }
                        
                        xmlNode = xmlBaseInfoNode->selectSingleNode(L"bz");
                        if (NULL != xmlNode)
                        {
                            EOrderInfo.m_BaseInfo.m_strBZ = xmlNode->text.operator const char*();
                        }
                    }//if (NULL != xmlBaseInfoNode)
                    
                    MSXML2::IXMLDOMNodePtr xmlStaffsNode = xmlEOrderNode->selectSingleNode(L"jdr");
                    if (NULL != xmlStaffsNode)
                    {
                        MSXML2::IXMLDOMNodePtr xmlVehNoNode = xmlStaffsNode->selectSingleNode(L"cph");
                        if (NULL != xmlVehNoNode)
                        {
                            EOrderInfo.m_strVehNo = xmlVehNoNode->text.operator const char*();
                        }

                        //# ��������<result/dzzyd/jdr/item>�ڵ�
                        MSXML2::IXMLDOMNodeListPtr pStaffList = NULL;
                        pStaffList = xmlStaffsNode->selectNodes(L"item");
                        if( NULL != pStaffList )
                        {
                            for (int j=0;j<pStaffList->Getlength();j++)
                            {
                                MSXML2::IXMLDOMNodePtr xmlStaffNode = pStaffList->Getitem(j);
                                if (NULL != xmlStaffNode)
                                {
                                    CEOrderStaff Staff;
                                
                                    MSXML2::IXMLDOMNodePtr xmlNode = xmlStaffNode->selectSingleNode(L"lx");
                                    if (NULL != xmlNode)
                                    {
                                        Staff.m_byLX = ::atoi(xmlNode->text.operator const char*());
                                    }
                                
                                    xmlNode = xmlStaffNode->selectSingleNode(L"bh");
                                    if (NULL != xmlNode)
                                    {
                                        Staff.m_strBH = xmlNode->text.operator const char*();
                                    }
                                
                                    EOrderInfo.m_Staffs.push_back(Staff);
                                }//if (NULL != xmlStaffNode)
                            }//for (int j=0;j<pStaffList->Getlength();j++)
                        }//if( NULL != pStaffList )
                    }//if (NULL != xmlStaffsNode)
                    
                    MSXML2::IXMLDOMNodePtr xmlCargoNode = xmlEOrderNode->selectSingleNode(L"cargo");
                    if (NULL != xmlCargoNode)
                    {
                        MSXML2::IXMLDOMNodePtr xmlNode = xmlCargoNode->selectSingleNode(L"mc");
                        if (NULL != xmlNode)
                        {
                            EOrderInfo.m_CargoInfo.m_strMC = xmlNode->text.operator const char*();
                        }

                        xmlNode = xmlCargoNode->selectSingleNode(L"sl");
                        if (NULL != xmlNode)
                        {
                            EOrderInfo.m_CargoInfo.m_wSL = ::atof(xmlNode->text.operator const char*())*100;
                        }
                    }//if (NULL != xmlCargoNode)
                    
                    MSXML2::IXMLDOMNodePtr xmlCustomerNode = xmlEOrderNode->selectSingleNode(L"khdw");
                    if (NULL != xmlCustomerNode)
                    {
                        MSXML2::IXMLDOMNodePtr xmlNode = xmlCustomerNode->selectSingleNode(L"mc");
                        if (NULL != xmlNode)
                        {
                            EOrderInfo.m_Customer.m_strMC = xmlNode->text.operator const char*();
                        }

                        xmlNode = xmlCustomerNode->selectSingleNode(L"lxr");
                        if (NULL != xmlNode)
                        {
                            EOrderInfo.m_Customer.m_strLXR = xmlNode->text.operator const char*();
                        }
                        
                        xmlNode = xmlCustomerNode->selectSingleNode(L"lxrdh");
                        if (NULL != xmlNode)
                        {
                            EOrderInfo.m_Customer.m_strLXRDH = xmlNode->text.operator const char*();
                        }
                    }//if (NULL != xmlCustomerNode)
                    
                    MSXML2::IXMLDOMNodePtr xmlLoadPointsNode = xmlEOrderNode->selectSingleNode(L"zxhdd");
                    if (NULL != xmlLoadPointsNode)
                    {
                        //# ��������<result/dzzyd/zxhdd/item>�ڵ�
                        MSXML2::IXMLDOMNodeListPtr pLoadPointList = NULL;
                        pLoadPointList = xmlLoadPointsNode->selectNodes(L"item");
                        if( NULL != pLoadPointList )
                        {
                            for (int j=0;j<pLoadPointList->Getlength();j++)
                            {
                                MSXML2::IXMLDOMNodePtr xmlLoadPointNode = pLoadPointList->Getitem(j);
                                if (NULL != xmlLoadPointNode)
                                {
                                    CEOrderLoadPoint LoadPoint;
                                    
                                    //# ֻ��Ҫ�����յ������Ϣ
                                    MSXML2::IXMLDOMNodePtr xmlNode = xmlLoadPointNode->selectSingleNode(L"xh");
                                    if (NULL != xmlNode)
                                    {
                                        LoadPoint.m_byXH = ::atoi(xmlNode->text.operator const char*());
                                    }
                                    
                                    xmlNode = xmlLoadPointNode->selectSingleNode(L"zdmc");
                                    if (NULL != xmlNode)
                                    {
                                        LoadPoint.m_strMC = xmlNode->text.operator const char*();
                                    }

                                    xmlNode = xmlLoadPointNode->selectSingleNode(L"zdjd");
                                    if (NULL != xmlNode)
                                    {
                                        LoadPoint.m_dbJD = ::atof(xmlNode->text.operator const char*());
                                    }
                                    
                                    xmlNode = xmlLoadPointNode->selectSingleNode(L"zdwd");
                                    if (NULL != xmlNode)
                                    {
                                        LoadPoint.m_dbWD = ::atof(xmlNode->text.operator const char*());
                                    }
                                    
                                    xmlNode = xmlLoadPointNode->selectSingleNode(L"sl");
                                    if (NULL != xmlNode)
                                    {
                                        LoadPoint.m_wSL = ::atof(xmlNode->text.operator const char*())*100;
                                    }
                                    
                                    xmlNode = xmlLoadPointNode->selectSingleNode(L"lxr");
                                    if (NULL != xmlNode)
                                    {
                                        LoadPoint.m_strLXR = xmlNode->text.operator const char*();
                                    }
                                    
                                    xmlNode = xmlLoadPointNode->selectSingleNode(L"lxdh");
                                    if (NULL != xmlNode)
                                    {
                                        LoadPoint.m_strLXDH = xmlNode->text.operator const char*();
                                    }
                                    
                                    xmlNode = xmlLoadPointNode->selectSingleNode(L"bz");
                                    if (NULL != xmlNode)
                                    {
                                        LoadPoint.m_strBZ = xmlNode->text.operator const char*();
                                    }
                                    
                                    std::string strZXHLX = "";//# װж������
                                    xmlNode = xmlLoadPointNode->selectSingleNode(L"zxhlx");
                                    if (NULL != xmlNode)
                                    {
                                        strZXHLX = xmlNode->text.operator const char*();
                                    }

                                    if (strZXHLX.find("װ") != -1)
                                    {
                                        EOrderInfo.m_LoadPoints.push_back(LoadPoint);
                                    }
                                    else if (strZXHLX.find("ж") != -1)
                                    {
                                        EOrderInfo.m_UnloadPoints.push_back(LoadPoint);
                                    } 
                                    else
                                    {
                                    }
                                    
                                }//if (NULL != xmlLoadPointNode)
                            }//for (int j=0;j<pLoadPointList->Getlength();j++)
                        }//if( NULL != pLoadPointList )
                    }//if (NULL != xmlLoadPointsNode)
                    
                    MSXML2::IXMLDOMNodePtr xmlTPTsNode = xmlEOrderNode->selectSingleNode(L"ty");
                    if (NULL != xmlTPTsNode)
                    {
                        MSXML2::IXMLDOMNodePtr xmlTPTNoNode = xmlTPTsNode->selectSingleNode(L"id");
                        if (NULL != xmlTPTNoNode)
                        {
                            EOrderInfo.m_strTPTNo = xmlTPTNoNode->text.operator const char*();
                        }
                        
                        //# ��������<result/dzzyd/ty/item>�ڵ�
                        MSXML2::IXMLDOMNodeListPtr pTPTList = NULL;
                        pTPTList = xmlTPTsNode->selectNodes(L"item");
                        if( NULL != pTPTList )
                        {
                            for (int j=0;j<pTPTList->Getlength();j++)
                            {
                                MSXML2::IXMLDOMNodePtr xmlTPTNode = pTPTList->Getitem(j);
                                if (NULL != xmlTPTNode)
                                {
                                    CEOrderTPT TPTInfo;
                                    
                                    MSXML2::IXMLDOMNodePtr xmlNode = xmlTPTNode->selectSingleNode(L"wz");
                                    if (NULL != xmlNode)
                                    {
                                        TPTInfo.m_wWZ = ::strtoul(xmlNode->text.operator const char*(),NULL,16);
                                    }
                                    
                                    xmlNode = xmlTPTNode->selectSingleNode(L"ytyxx");
                                    if (NULL != xmlNode)
                                    {
                                        TPTInfo.m_wYTYXX = ::atof(xmlNode->text.operator const char*())*100;
                                    }
                                    
                                    xmlNode = xmlTPTNode->selectSingleNode(L"ytysx");
                                    if (NULL != xmlNode)
                                    {
                                        TPTInfo.m_wYTYSX = ::atof(xmlNode->text.operator const char*())*100;
                                    }
                                    
                                    xmlNode = xmlTPTNode->selectSingleNode(L"rtyxx");
                                    if (NULL != xmlNode)
                                    {
                                        TPTInfo.m_wRTYXX = ::atof(xmlNode->text.operator const char*())*100;
                                    }
                                    
                                    xmlNode = xmlTPTNode->selectSingleNode(L"rtysx");
                                    if (NULL != xmlNode)
                                    {
                                        TPTInfo.m_wRTYSX = ::atof(xmlNode->text.operator const char*())*100;
                                    }
                                    
                                    xmlNode = xmlTPTNode->selectSingleNode(L"ytwxx");
                                    if (NULL != xmlNode)
                                    {
                                        TPTInfo.m_wYTWXX = ::atof(xmlNode->text.operator const char*())*100;
                                    }
                                    
                                    xmlNode = xmlTPTNode->selectSingleNode(L"ytwsx");
                                    if (NULL != xmlNode)
                                    {
                                        TPTInfo.m_wYTWSX = ::atof(xmlNode->text.operator const char*())*100;
                                    }
                                    
                                    xmlNode = xmlTPTNode->selectSingleNode(L"rtwxx");
                                    if (NULL != xmlNode)
                                    {
                                        TPTInfo.m_wRTWXX = ::atof(xmlNode->text.operator const char*())*100;
                                    }
                                    
                                    xmlNode = xmlTPTNode->selectSingleNode(L"rtwsx");
                                    if (NULL != xmlNode)
                                    {
                                        TPTInfo.m_wRTWSX = ::atof(xmlNode->text.operator const char*())*100;
                                    }
                                    
                                    EOrderInfo.m_TPTs.push_back(TPTInfo);
                                }//if (NULL != xmlTPTNode)
                            }//for (int j=0;j<pTPTList->Getlength();j++)
                        }//if( NULL != pTPTList )
                    }//if (NULL != xmlTPTsNode)
                    
                    MSXML2::IXMLDOMNodePtr xmlEOptTickNode = xmlEOrderNode->selectSingleNode(L"dzczp");
                    if (NULL != xmlEOptTickNode)
                    {
                        MSXML2::IXMLDOMNodePtr xmlEOptTickNoNode = xmlEOptTickNode->selectSingleNode(L"id");
                        if (NULL != xmlEOptTickNoNode)
                        {
                            EOrderInfo.m_strEOptTickNo = xmlEOptTickNoNode->text.operator const char*();
                        }
                        
                        //# ��������<result/dzzyd/dzczp/item>�ڵ�
                        MSXML2::IXMLDOMNodeListPtr pProcedureList = NULL;
                        pProcedureList = xmlEOptTickNode->selectNodes(L"item");
                        if( NULL != pProcedureList )
                        {
                            for (int j=0;j<min(pProcedureList->Getlength(),5);j++)/*�����������ò�����5��*/
                            {
                                MSXML2::IXMLDOMNodePtr xmlProcedureNode = pProcedureList->Getitem(j);
                                if (NULL != xmlProcedureNode)
                                {
                                    CEOrderProcedure Procedure;

                                    MSXML2::IXMLDOMNodePtr xmlNode = xmlProcedureNode->selectSingleNode(L"id");
                                    if (NULL != xmlNode)
                                    {
                                        Procedure.m_strID = xmlNode->text.operator const char*();
                                    }
                                    xmlNode = xmlProcedureNode->selectSingleNode(L"xh");
                                    if (NULL != xmlNode)
                                    {
                                        Procedure.m_byXH = ::atoi(xmlNode->text.operator const char*());
                                    }
                                    xmlNode = xmlProcedureNode->selectSingleNode(L"lx");
                                    if (NULL != xmlNode)
                                    {
                                        Procedure.m_strLX = xmlNode->text.operator const char*();
                                    }
                                    xmlNode = xmlProcedureNode->selectSingleNode(L"jhwcsj");
                                    if (NULL != xmlNode)
                                    {
                                        Procedure.m_strJHWCSJ = xmlNode->text.operator const char*();
                                    }
                                    
                                    //# ��������<result/dzzyd/dzczp/item/subitem>�ڵ�
                                    MSXML2::IXMLDOMNodeListPtr pSubProcedureList = NULL;
                                    pSubProcedureList = xmlProcedureNode->selectNodes(L"subitem");
                                    if( NULL != pSubProcedureList )
                                    {
                                        for (int k=0;k<min(pSubProcedureList->Getlength(),10);k++)/*��ҵ��������ò�����10��*/
                                        {
                                            MSXML2::IXMLDOMNodePtr xmlSubProcedureNode = pSubProcedureList->Getitem(k);
                                            if (NULL != xmlSubProcedureNode)
                                            {
                                                CEOrderSubProcedure SubProcedure;

                                                xmlNode = xmlSubProcedureNode->selectSingleNode(L"id");
                                                if (NULL != xmlNode)
                                                {
                                                    SubProcedure.m_strID = xmlNode->text.operator const char*();
                                                }
                                                xmlNode = xmlSubProcedureNode->selectSingleNode(L"xh");
                                                if (NULL != xmlNode)
                                                {
                                                    SubProcedure.m_byXH = ::atoi(xmlNode->text.operator const char*());
                                                }
                                                xmlNode = xmlSubProcedureNode->selectSingleNode(L"lx");
                                                if (NULL != xmlNode)
                                                {
                                                    SubProcedure.m_strLX = xmlNode->text.operator const char*();
                                                }
                                                xmlNode = xmlSubProcedureNode->selectSingleNode(L"jhwcsj");
                                                if (NULL != xmlNode)
                                                {
                                                    SubProcedure.m_strJHWCSJ = xmlNode->text.operator const char*();
                                                }

                                                
                                                //# ��������<result/dzzyd/dzczp/item/subitem/zynr>�ڵ�
                                                MSXML2::IXMLDOMNodeListPtr pSPContentList = NULL;
                                                pSPContentList = xmlSubProcedureNode->selectNodes(L"zynr");
                                                if( NULL != pSPContentList )
                                                {
                                                    for (int l=0;l<min(pSPContentList->Getlength(),20);l++)/*��ҵ���ݸ�������ò�����20��*/
                                                    {
                                                        MSXML2::IXMLDOMNodePtr xmlSPContentNode = pSPContentList->Getitem(l);
                                                        if (NULL != xmlSPContentNode)
                                                        {
                                                            CEOrderSPC SPContent;
                                                            
                                                            xmlNode = xmlSPContentNode->selectSingleNode(L"id");
                                                            if (NULL != xmlNode)
                                                            {
                                                                SPContent.m_strID = xmlNode->text.operator const char*();
                                                            }
                                                            xmlNode = xmlSPContentNode->selectSingleNode(L"xh");
                                                            if (NULL != xmlNode)
                                                            {
                                                                SPContent.m_byXH = ::atoi(xmlNode->text.operator const char*());
                                                            }
                                                            xmlNode = xmlSPContentNode->selectSingleNode(L"nr");
                                                            if (NULL != xmlNode)
                                                            {
                                                                SPContent.m_strNR = xmlNode->text.operator const char*();
                                                            }

                                                            SubProcedure.m_Contents.push_back(SPContent);
                                                        }//if (NULL != xmlSPContentNode)
                                                    }//for (int l=0;l<min(pSPContentList->Getlength(),20);l++)
                                                }//if( NULL != pSPContentList )

                                                Procedure.m_SubProcedures.push_back(SubProcedure);
                                            }//if (NULL != xmlSubProcedureNode)
                                        }//for (int k=0;k<min(pSubProcedureList->Getlength(),10);k++)
                                    }//if( NULL != pSubProcedureList )

                                    EOrderInfo.m_Procedures.push_back(Procedure);
                                }//if (NULL != xmlProcedureNode)
                            }//for (int j=0;j<min(pProcedureList->Getlength(),5);j++)
                        }//if( NULL != pProcedureList )
                    }//if (NULL != xmlEOptTickNode)
                    
                    _vctEOrders.push_back(EOrderInfo);
                }//if (NULL != xmlEOrderNode)
            }//for (int i=0;i<min(pEOrderList->Getlength(),5);i++)
        }//if( NULL != pEOrderList )
        
        return TRUE;
    }
    catch (CMemoryException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (CFileException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (...)
    {
        ::sprintf(szLastError,"δ֪");
    }
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"��ȡ�����б�����쳣:%s",szLastError);
    
    return FALSE;
}

BOOL CMyXmlParser::LoadXml_SAFEARRARY( PCHAR _pData )
{
    if (!_pData)
    {
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"����������ַ���ָ��Ϊ��");
        
        return FALSE;
    }
    
    //# ���ͷ�
    if( m_xmlDoc != NULL )
    {
        m_xmlDoc.Release();
        m_xmlDoc = NULL;
    }
    
    char szLastError[1024] = {0};
    try
    {    
        //# ��ʼ��XML�ĵ�����
        if(SUCCEEDED(m_xmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument30)))) 
        {
            //# ����XML�ַ���
            m_xmlDoc->async = false;
			COleSafeArray SafeArray;
			SAFEARRAYBOUND saBound;
			saBound.cElements = ::strlen(_pData);
			saBound.lLbound = 0;
			SafeArray.DestroyData();
			SafeArray.Create(VT_UI1, 1, &saBound);
			
			void* pBufsa = NULL;
			SafeArray.ResizeOneDim(::strlen(_pData));
			SafeArray.AccessData(&pBufsa);
			memcpy(pBufsa, _pData, ::strlen(_pData));
			SafeArray.UnaccessData();
		
            if( VARIANT_TRUE == m_xmlDoc->load(SafeArray))
            {
                return TRUE;
            }
        }
    }
    catch (CMemoryException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (CFileException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (...)
    {
        ::sprintf(m_szLastError,"δ֪");
    }
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"����XML��ʽ����ʱ�����쳣:%s",szLastError);
    
	return FALSE;
}

BOOL CMyXmlParser::GetTyreConfigTables( std::vector<CTyreConfigTable> &_vctTables )
{
	//	   <?xml version="1.0" encoding="utf-8" ?>
	//	   <param>
	//		   <!--�ն��ֻ���-->
	//		   <mobile>13800010001</mobile>
	//		   <!--���ƺ�-->
	//		   <vehiclelic>�� D10011</vehiclelic>
	// 
	//		   <!--̥ѹ���ñ����Ե����·���ͷ/�ҳ������ñ�Ҳ����ͬʱ�·���ͬʱ��
	//		   ����������<tyreconfigtable>�ṹ-->
	//		   <tyreconfigtable>
	//			   <!--���ͣ�1 ��ͷ��2 �ҳ�-->
	//			   <type>1</type>
	//			   <!--��������������Ϊ 1-��ͷʱ����Ҫ-->
	//			   <receptor>3434ef-dfdsf-erer</receptor>
	//			   <!--�м�����������Ϊ 2-�ҳ�ʱ����Ҫ-->
	//			   <repeater>3434ef-dfdsf-erer</repeater>
	// 
	//			   <!--�������б�-->
	//			   <items>
	//				   <!--������-->
	//				   <item>
	//				   <!--������λ�ñ�ţ�����ʮ��������ֵ��ʾ  char(10)���ο� 6.1������-->
	//				   <no>0x0211</no>
	//				   <!--������ ID������ʮ��������ֵ��ʾ	char(10)-->
	//				   <id>0x12345678</id>
	//				   </item>
	// 
	//				   <!--������-->
	//				   <item>
	//				   <!--������λ�ñ�ţ�����ʮ��������ֵ��ʾ  char(10)���ο� 6.1������-->
	//				   <no>0x0221</no>
	//				   <!--������ ID������ʮ��������ֵ��ʾ	char(10)-->
	//				   <id>0x12345678</id>
	//				   </item>
	//				   ...
	//			   </items>
	//		   </tyreconfigtable>
	//	   </param>
    
    char szLastError[1024] = {0};
    if ( NULL == m_xmlDoc && !LoadFile() )
    {
        ::memcpy(szLastError,m_szLastError,sizeof(szLastError));
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"ȫ��XML�ĵ�����Ϊ�������¼���ʧ��:%s",szLastError);
        
        return FALSE;
    }
    
    try
    {
        //# ��������<param/tyreconfigtable>�ڵ�
        MSXML2::IXMLDOMNodeListPtr pTableList = NULL;
        pTableList = m_xmlDoc->selectNodes(L"param//tyreconfigtable");
        if( NULL == pTableList )
        {
            return TRUE;
        }

        int iTableCnt = min(pTableList->Getlength(),2);/*̥ѹ�������Զ�ʶ�����ñ������������ͷ+�ҳ�*/
        for ( int iTableIndex=0;iTableIndex<iTableCnt;iTableIndex++ )
        {
            MSXML2::IXMLDOMNodePtr xmlTableNode = pTableList->Getitem(iTableIndex);
            if ( NULL == xmlTableNode )
            {
                continue;
            }
            
            CTyreConfigTable table;
            MSXML2::IXMLDOMNodePtr xmlTypeNode = xmlTableNode->selectSingleNode(L"type");
            if ( NULL == xmlTypeNode )
            {
                continue;
            }
            table.m_byType = ::atoi(xmlTypeNode->text.operator const char*());

            if ( 1 == table.m_byType )// �������ͣ�1-��ͷ
            {
                MSXML2::IXMLDOMNodePtr xmlReceptorNode = xmlTableNode->selectSingleNode(L"receptor");
                if ( NULL == xmlReceptorNode )
                {
                    continue;
                }

                table.m_strReceptorOrRepeator = xmlReceptorNode->text.operator const char*();
            } // 
            else if ( 2 == table.m_byType )// 2-�ҳ�
            {
                MSXML2::IXMLDOMNodePtr xmlRepeatorNode = xmlTableNode->selectSingleNode(L"repeater");
                if ( NULL == xmlRepeatorNode )
                {
                    continue;
                }
                
                table.m_strReceptorOrRepeator = xmlRepeatorNode->text.operator const char*();
            } // 
            else
            {
                continue;
            } // else
            
            MSXML2::IXMLDOMNodeListPtr pItemList = NULL;
            pItemList = xmlTableNode->selectNodes(L"items//item");
            if( NULL == pItemList )
            {
                continue;
            }

            int iItemCnt = pItemList->Getlength();
            for ( int iItemIndex=0;iItemIndex<iItemCnt;iItemIndex++ )
            {
                MSXML2::IXMLDOMNodePtr xmlItemNode = pItemList->Getitem(iItemIndex);
                if ( NULL == xmlItemNode )
                {
                    continue;
                }
                
                CTyreConfigItem item;

                MSXML2::IXMLDOMNodePtr xmlSensorNoNode = xmlItemNode->selectSingleNode(L"no");
                if ( NULL == xmlSensorNoNode )
                {
                    continue;
                }
                item.m_wNo = ::strtoul(xmlSensorNoNode->text.operator const char*(),NULL,16);
                
                MSXML2::IXMLDOMNodePtr xmlSensorIdNode = xmlItemNode->selectSingleNode(L"id");
                if ( NULL == xmlSensorIdNode )
                {
                    continue;
                }
                item.m_dwId = ::strtoul(xmlSensorIdNode->text.operator const char*(),NULL,16);

                table.m_vctItems.push_back(item);
            } // for ( int iItemIndex=0;iItemIndex<iItemCnt;iItemIndex++ )

            _vctTables.push_back(table);
        } // for ( int iTableIndex=0;iTableCnt;iTableIndex<iTableIndex++ )

        return TRUE;
    }
    catch (CMemoryException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (CFileException* e)
    {
        e->GetErrorMessage(szLastError,sizeof(szLastError));
    }
    catch (...)
    {
        ::sprintf(szLastError,"δ֪");
    }
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"��ȡ̥ѹ�������Զ�ʶ�����ñ�����쳣:%s",szLastError);
    
    return FALSE;
}
