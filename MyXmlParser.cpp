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
        ::sprintf(m_szLastError,"请求加载的文件名为空");

        return FALSE;
    }
    
    //# 先释放
    if( m_xmlDoc != NULL )
    {
        m_xmlDoc.Release();
        m_xmlDoc = NULL;
    }
    
    //# 初始化XML文档对象
    if(SUCCEEDED(m_xmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument30)))) 
    {
        //# 加载XML字符串
        m_xmlDoc->async = false;
        if( VARIANT_TRUE == m_xmlDoc->load((_bstr_t)m_strFilePath.c_str()))
        {
            return TRUE;
        }
        else
        {
            ZeroMemory(m_szLastError,sizeof(m_szLastError));
            ::sprintf(m_szLastError,"加载XML文档失败");
        }
    }
    else
    {
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"初始化XML文档对象失败");
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
        ::sprintf(m_szLastError,"请求创建的文件名为空");

        return FALSE;
    }
    
    //# 先释放
    if( m_xmlDoc != NULL )
    {
        m_xmlDoc.Release();
        m_xmlDoc = NULL;
    }
    
    //# 初始化XML文档对象
    if(!SUCCEEDED(m_xmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument30)))) 
    {
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"初始化XML文档对象失败");

        return FALSE;
    }
    
    //# 加载XML字符串
    m_xmlDoc->async = false;
    
    char szLastError[1024] = {0};
    try
    {
        //# 创建文档说明
        MSXML2::IXMLDOMProcessingInstructionPtr pProInstruction = NULL;
        pProInstruction = m_xmlDoc->createProcessingInstruction(L"xml", L"version=\"1.0\" encoding=\"utf-8\"");
        m_xmlDoc->appendChild((MSXML2::IXMLDOMNode*)pProInstruction);
        
        //# 创建根节点
        std::string strRootName = "root";
        if (!_strRootName.empty())
        {
            strRootName = _strRootName;
        }

        MSXML2::IXMLDOMElementPtr xmlRoot = NULL;
        xmlRoot = m_xmlDoc->createElement((_bstr_t)strRootName.c_str());
        if( xmlRoot != NULL )
        {
            m_xmlDoc->appendChild(xmlRoot); //# 插入根节点
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
        ::sprintf(szLastError,"未知");
    }
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"创建文档初始化内容并且保存时出现异常:%s",szLastError);

    return FALSE;
}

BOOL CMyXmlParser::AppendNode( std::string _strNode, std::string _strValue /*= ""*/, std::string _strParentNode /*= ""*/ )
{
    char szLastError[1024] = {0};

    if (NULL == m_xmlDoc && !LoadFile())
    {
        ::memcpy(szLastError,m_szLastError,sizeof(szLastError));

        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"全局XML文档对象为空且重新加载失败:%s",szLastError);
        
        return FALSE;
    }
    
    try
    {
        if (!_strParentNode.empty())
        {
            std::string strParentNode = "//";//# 前面加"//"表示任意一级节点下查找
            strParentNode += _strParentNode;

            //# 定位到父节点
            MSXML2::IXMLDOMNodePtr xmlFatherNode = NULL;
            xmlFatherNode = m_xmlDoc->selectSingleNode((_bstr_t)strParentNode.c_str());
            if( NULL == xmlFatherNode )
            {
                ZeroMemory(m_szLastError,sizeof(m_szLastError));
                ::sprintf(m_szLastError,"查询不到节点<%s>",_strParentNode.c_str());

                return FALSE;
            }

            //# 创建新的节点
            MSXML2::IXMLDOMElementPtr xmlNewElement = NULL;
            xmlNewElement = m_xmlDoc->createElement((_bstr_t)_strNode.c_str());
            if( NULL == xmlNewElement )
            {
                ZeroMemory(m_szLastError,sizeof(m_szLastError));
                ::sprintf(m_szLastError,"创建节点<%s>失败",_strNode.c_str());

                return FALSE;
            }
            xmlNewElement->Puttext((_bstr_t)_strValue.c_str());

            // #插入到父节点之后
            if( NULL == xmlFatherNode->appendChild((MSXML2::IXMLDOMNodePtr)xmlNewElement) )
            {
                ZeroMemory(m_szLastError,sizeof(m_szLastError));
                ::sprintf(m_szLastError,"插入节点<%s>失败",_strNode.c_str());

                return FALSE;
		    }
        }
        else
        {
            //# 获得根节点
            MSXML2::IXMLDOMElementPtr xmlRoot = NULL;
            xmlRoot = m_xmlDoc->GetdocumentElement();
            if (NULL == xmlRoot)
            {
                ZeroMemory(m_szLastError,sizeof(m_szLastError));
                ::sprintf(m_szLastError,"获得根节点失败");

                return FALSE;
            }

            //# 创建新的节点
//             MSXML2::IXMLDOMNodePtr xmlNewElement = NULL;
//             xmlNewElement = m_xmlDoc->createNode((_variant_t)(long)1 //# NODE_ELEMENT
//                 , (_bstr_t)_strNode.c_str(), (_bstr_t)(char*)"");
            MSXML2::IXMLDOMElementPtr xmlNewElement = NULL;
            xmlNewElement = m_xmlDoc->createElement((_bstr_t)_strNode.c_str());
            if (NULL == xmlNewElement)
            {
                ZeroMemory(m_szLastError,sizeof(m_szLastError));
                ::sprintf(m_szLastError,"创建节点<%s>失败",_strNode.c_str());

                return FALSE;
            }
            xmlNewElement->Puttext((_bstr_t)_strValue.c_str());

            //# 插入到根节点下
            if(NULL == xmlRoot->appendChild((MSXML2::IXMLDOMNodePtr)xmlNewElement))
            {
                ZeroMemory(m_szLastError,sizeof(m_szLastError));
                ::sprintf(m_szLastError,"插入节点<%s>失败",_strNode.c_str());

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
        ::sprintf(szLastError,"未知"); 
    }    
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"创建新节点<%s/%s>出现异常:%s"
        ,_strParentNode.c_str(),_strNode.c_str(),szLastError);

    return FALSE;
}

BOOL CMyXmlParser::SetAttribute( std::string _strNode, std::string _strAttr, std::string _strValue )
{
    if (_strNode.empty())
    {
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"请求查询的节点名为空");
        
        return FALSE;
    }
    
    char szLastError[1024] = {0};
    if (NULL == m_xmlDoc && !LoadFile())
    {
        ::memcpy(szLastError,m_szLastError,sizeof(szLastError));
        
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"全局XML文档对象为空且重新加载失败:%s",szLastError);
        
        return FALSE;
    }
    
    try
    {
        std::string strNode = "//";//# 前面加"//"表示任意一级节点下查找
        strNode += _strNode;

        //# 定位到节点
        MSXML2::IXMLDOMElementPtr xmlElement = NULL;
        xmlElement = m_xmlDoc->selectSingleNode((_bstr_t)strNode.c_str());
        if( NULL == xmlElement )
        {
            ZeroMemory(m_szLastError,sizeof(m_szLastError));
            ::sprintf(m_szLastError,"查询不到节点<%s>",strNode.c_str());

            return FALSE;
        }

        //# 设置属性值
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
        ::sprintf(szLastError,"未知");
    }
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"设置节点<%s>属性[%s]值[%s]出现异常:%s"
        ,_strNode.c_str(),_strAttr.c_str(),_strValue.c_str(),szLastError);

    return FALSE;
}

BOOL CMyXmlParser::GetAttribute( std::string _strNode, std::string _strAttr, std::string& _strValue )
{
    if (_strNode.empty())
    {
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"请求查询的节点名为空");
        
        return FALSE;
    }

    char szLastError[1024] = {0};
    if (NULL == m_xmlDoc && !LoadFile())
    {
        ::memcpy(szLastError,m_szLastError,sizeof(szLastError));
        
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"全局XML文档对象为空且重新加载失败:%s",szLastError);
        
        return FALSE;
    }
    
    try
    {
        std::string strNode = "//";//# 前面加"//"表示任意一级节点下查找
        strNode += _strNode;

        //# 定位到节点
        MSXML2::IXMLDOMElementPtr xmlElement = NULL;
        xmlElement = m_xmlDoc->selectSingleNode((_bstr_t)strNode.c_str());
        if( NULL == xmlElement )
        {
            ZeroMemory(m_szLastError,sizeof(m_szLastError));
            ::sprintf(m_szLastError,"查询不到节点<%s>",strNode.c_str());

            return FALSE;
        }

        //# 获取属性值
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
        ::sprintf(szLastError,"未知");
    }

    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"查询节点<%s>属性[%s]值出现异常:s",_strNode.c_str(),_strAttr.c_str(),szLastError);
    
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
        ::sprintf(szLastError,"未知");
    }
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"保存文件出现异常:%s",szLastError);
    
    return FALSE;
}

BOOL CMyXmlParser::CheckNode( std::string _strNode, std::string _strParentNode /*= ""*/ )
{
    if (_strNode.empty())
    {
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"请求查询的节点名为空");
        
        return FALSE;
    }
    
    char szLastError[1024] = {0};
    if (NULL == m_xmlDoc && !LoadFile())
    {
        ::memcpy(szLastError,m_szLastError,sizeof(szLastError));
        
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"全局XML文档对象为空且重新加载失败:%s",szLastError);
        
        return FALSE;
    }

    try
    {
        std::string strNode = "//";//# 前面加"//"表示任意一级节点下查找
        if (!_strParentNode.empty())
        {
            strNode += _strParentNode;
            strNode += "//";
        }
        
        strNode += _strNode;
        
        //# 定位到节点
        MSXML2::IXMLDOMNodePtr xmlNode = NULL;
        xmlNode = m_xmlDoc->selectSingleNode((_bstr_t)strNode.c_str());
        if( NULL == xmlNode )
        {
            ZeroMemory(m_szLastError,sizeof(m_szLastError));
            ::sprintf(m_szLastError,"查询不到节点<%s>",strNode.c_str());

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
        ::sprintf(szLastError,"未知");
    }
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"查询节点<%s/%s>出现异常:%s"
        ,_strParentNode.c_str(),_strNode.c_str(),szLastError);
    
    return FALSE;
}
BOOL CMyXmlParser::GetNodeText( std::string _strNode, std::string& _strValue )
{
    if (_strNode.empty())
    {
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"请求查询的节点名为空");
        
        return FALSE;
    }
    
    char szLastError[1024] = {0};
    if (NULL == m_xmlDoc && !LoadFile())
    {
        ::memcpy(szLastError,m_szLastError,sizeof(szLastError));
        
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"全局XML文档对象为空且重新加载失败:%s",szLastError);
        
        return FALSE;
    }
    
    try
    {
        std::string strNode = "//";//# 前面加"//"表示任意一级节点下查找
        strNode += _strNode;
        
        //# 定位到节点
        MSXML2::IXMLDOMNodePtr xmlNode = NULL;
        xmlNode = m_xmlDoc->selectSingleNode((_bstr_t)strNode.c_str());
        if( NULL == xmlNode )
        {
            ZeroMemory(m_szLastError,sizeof(m_szLastError));
            ::sprintf(m_szLastError,"查询不到节点<%s>",strNode.c_str());

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
        ::sprintf(szLastError,"未知");
    }
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"获取节点<%s>值出现异常:%s",_strNode.c_str(),szLastError);
    
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
        ::sprintf(m_szLastError,"请求解析的字符串为空");
        
        return FALSE;
    }
    
    //# 先释放
    if( m_xmlDoc != NULL )
    {
        m_xmlDoc.Release();
        m_xmlDoc = NULL;
    }
    
    char szLastError[1024] = {0};
    try
    {    
        //# 初始化XML文档对象
        if(SUCCEEDED(m_xmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument30)))) 
        {
            //# 加载XML字符串
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
        ::sprintf(m_szLastError,"未知");
    }
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"加载XML格式内容时出现异常:%s",szLastError);
    
	return FALSE;
}
BOOL CMyXmlParser::CreateXml( std::string _strRootName /*= ""*/ )
{
    //# 先释放
    if( m_xmlDoc != NULL )
    {
        m_xmlDoc.Release();
        m_xmlDoc = NULL;
    }
    
    char szLastError[1024] = {0};
    try
    {    
        //# 初始化XML文档对象
        if(!SUCCEEDED(m_xmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument30)))) 
        {
            ZeroMemory(m_szLastError,sizeof(m_szLastError));
            ::sprintf(m_szLastError,"初始化XML文档对象失败");
        
            return FALSE;
        }
    
        //# 加载XML字符串
        m_xmlDoc->async = false;

        //# 创建文档说明
        MSXML2::IXMLDOMProcessingInstructionPtr pProInstruction = NULL;
        pProInstruction = m_xmlDoc->createProcessingInstruction(L"xml", L"version=\"1.0\" encoding=\"utf-8\"");
        m_xmlDoc->appendChild((MSXML2::IXMLDOMNode*)pProInstruction);
        
        //# 创建根节点
        std::string strRootName = "root";
        if (!_strRootName.empty())
        {
            strRootName = _strRootName;
        }
        
        MSXML2::IXMLDOMElementPtr xmlRoot = NULL;
        xmlRoot = m_xmlDoc->createElement((_bstr_t)strRootName.c_str());
        if( xmlRoot != NULL )
        {
            m_xmlDoc->appendChild(xmlRoot); //# 插入根节点
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
        ::sprintf(szLastError,"未知");
    }
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"构造XML格式内容时出现异常:%s",szLastError);
    
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
        ::sprintf(m_szLastError,"请求查询的节点名为空");
        
        return FALSE;
    }
    
    char szLastError[1024] = {0};
    if (NULL == m_xmlDoc && !LoadFile())
    {
        ::memcpy(szLastError,m_szLastError,sizeof(szLastError));
        
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"全局XML文档对象为空且重新加载失败:%s",szLastError);
        
        return FALSE;
    }
    
    try
    {
        std::string strNode = "//";//# 前面加"//"表示任意一级节点下查找
        strNode += _strNode;
        
        //# 定位到节点
        MSXML2::IXMLDOMNodePtr xmlNode = NULL;
        xmlNode = m_xmlDoc->selectSingleNode((_bstr_t)strNode.c_str());
        if( NULL == xmlNode )
        {
            ZeroMemory(m_szLastError,sizeof(m_szLastError));
            ::sprintf(m_szLastError,"查询不到节点<%s>",strNode.c_str());
            
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
        ::sprintf(szLastError,"未知");
    }
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"设置节点<%s>值出现异常:%s",_strNode.c_str(),szLastError);
    
    return FALSE;
}
BOOL CMyXmlParser::GetStaffInfos( std::vector<CStaffInfo> &_vctStaffInfos )
{
//     <?xml version="1.0" encoding="utf-8" ?>
//     <param>
//         <!--终端手机号-->
//         <mobile>13800010001</mobile>
//         <!--车牌号-->
//         <vehiclelic>闽D10011</vehiclelic>
//         <!--人员信息-->
//         <items>
//             <item>
//             　　<!--人员类型： 1司机  2押运员-->
//             　　<type>1</type>
//             　　<!--IC卡编号-->
//             　　<icno></icno>
//             　　<!--人员在系统中编号 GUID-->
//             　　<idno>30842738-AAB4-4c69-B81C-6C9E91157B0B</idno>
//             　　<!--姓名-->
//             　　<name>张三</name>
//             　　<!--是否接单人 1是  0否-->
//             　　<isneeded>1</isneeded>
//         　　</item>
//         </items>
//     </param>
    
    char szLastError[1024] = {0};
    if (NULL == m_xmlDoc && !LoadFile())
    {
        ::memcpy(szLastError,m_szLastError,sizeof(szLastError));
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"全局XML文档对象为空且重新加载失败:%s",szLastError);
        
        return FALSE;
    }
    
    try
    {
        //# 查找根节点下的所有<item>节点
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
        ::sprintf(szLastError,"未知");
    }
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"获取接单人信息列表出现异常:%s",szLastError);
    
    return FALSE;
}
BOOL CMyXmlParser::GetOilInfos( std::vector<COilInfo> &_vctOilInfos )
{
//     <?xml version="1.0" encoding="utf-8" ?>
//     <param>
//         <!--终端手机号-->
//         <mobile>13800010001</mobile>
//         <!--车牌号-->
//         <vehiclelic>闽D10011</vehiclelic>
//         <!--油位变化阈值-->
//         <items>
//         　　<item>
//             　　<!--油位传感器编号 范围1~99-->
//             　　<bh>1</bh>
//             　　<!--油位变化阈值，百分比值,2.1%-->
//             　　<yz>2.1</yz>
//         　　</item>
//         </items>
//     </param>
    
    char szLastError[1024] = {0};
    if (NULL == m_xmlDoc && !LoadFile())
    {
        ::memcpy(szLastError,m_szLastError,sizeof(szLastError));
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"全局XML文档对象为空且重新加载失败:%s",szLastError);
        
        return FALSE;
    }
    
    try
    {
        //# 查找根节点下的所有<item>节点
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
        ::sprintf(szLastError,"未知");
    }
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"获取油位阈值设置列表出现异常:%s",szLastError);
    
    return FALSE;
}
BOOL CMyXmlParser::AppendNodeBefore( std::string _strNode, std::string _strNode2, std::string _strValue /*= ""*/ )
{
    char szLastError[1024] = {0};

    if (NULL == m_xmlDoc && !LoadFile())
    {
        ::memcpy(szLastError,m_szLastError,sizeof(szLastError));

        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"全局XML文档对象为空且重新加载失败:%s",szLastError);
        
        return FALSE;
    }
    
    try
    {
        std::string strNextNode = "//";//# 前面加"//"表示任意一级节点下查找
        strNextNode += _strNode2;
        
        //# 定位到节点
        MSXML2::IXMLDOMNodePtr xmlNextNode = NULL;
        xmlNextNode = m_xmlDoc->selectSingleNode((_bstr_t)strNextNode.c_str());
        if( NULL == xmlNextNode )
        {
            ZeroMemory(m_szLastError,sizeof(m_szLastError));
            ::sprintf(m_szLastError,"查询不到节点<%s>",_strNode2.c_str());
            
            return FALSE;
        }
        MSXML2::IXMLDOMNodePtr xmlParentNode = xmlNextNode->GetparentNode();
        if (NULL == xmlParentNode)
        {
            ZeroMemory(m_szLastError,sizeof(m_szLastError));
            ::sprintf(m_szLastError,"查询不到节点<%s>的父节点",_strNode2.c_str());
            
            return FALSE;
        }
        
        //# 创建新的节点
        MSXML2::IXMLDOMElementPtr xmlNewElement = NULL;
        xmlNewElement = m_xmlDoc->createElement((_bstr_t)_strNode.c_str());
        if( NULL == xmlNewElement )
        {
            ZeroMemory(m_szLastError,sizeof(m_szLastError));
            ::sprintf(m_szLastError,"创建节点<%s>失败",_strNode.c_str());
            
            return FALSE;
        }
        xmlNewElement->Puttext((_bstr_t)_strValue.c_str());
        
        // #插入到父节点之后
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
        ::sprintf(szLastError,"未知"); 
    }    
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"创建新节点<%s>出现异常:%s",_strNode.c_str(),szLastError);

    return FALSE;
}
BOOL CMyXmlParser::AppendNodeFront( std::string _strNode, std::string _strValue /*= ""*/, std::string _strParentNode /*= ""*/ )
{
    char szLastError[1024] = {0};
    
    if (NULL == m_xmlDoc && !LoadFile())
    {
        ::memcpy(szLastError,m_szLastError,sizeof(szLastError));
        
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"全局XML文档对象为空且重新加载失败:%s",szLastError);
        
        return FALSE;
    }
    
    try
    {
        if (!_strParentNode.empty())
        {
            std::string strParentNode = "//";//# 前面加"//"表示任意一级节点下查找
            strParentNode += _strParentNode;
            
            //# 定位到父节点
            MSXML2::IXMLDOMNodePtr xmlFatherNode = NULL;
            xmlFatherNode = m_xmlDoc->selectSingleNode((_bstr_t)strParentNode.c_str());
            if( NULL == xmlFatherNode )
            {
                ZeroMemory(m_szLastError,sizeof(m_szLastError));
                ::sprintf(m_szLastError,"查询不到节点<%s>",_strParentNode.c_str());
                
                return FALSE;
            }
            
            //# 创建新的节点
            MSXML2::IXMLDOMElementPtr xmlNewElement = NULL;
            xmlNewElement = m_xmlDoc->createElement((_bstr_t)_strNode.c_str());
            if( NULL == xmlNewElement )
            {
                ZeroMemory(m_szLastError,sizeof(m_szLastError));
                ::sprintf(m_szLastError,"创建节点<%s>失败",_strNode.c_str());
                
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
                // #插入到父节点之后
                if( NULL == xmlFatherNode->appendChild((MSXML2::IXMLDOMNodePtr)xmlNewElement) )
                {
                    ZeroMemory(m_szLastError,sizeof(m_szLastError));
                    ::sprintf(m_szLastError,"插入节点<%s>失败",_strNode.c_str());
                    
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
        ::sprintf(szLastError,"未知"); 
    }    
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"创建新节点<%s/%s>出现异常:%s"
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
        ::sprintf(m_szLastError,"全局XML文档对象为空且重新加载失败:%s",szLastError);
        
        return FALSE;
    }
    
    try
    {
        //# 查找所有<result/ysxl>节点
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

                    //# 查找所有<result/ysxl/item>节点
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

                                //# 查找所有<result/ysxl/item/jwd/item>节点
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
        ::sprintf(szLastError,"未知");
    }
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"获取运输线路列表出现异常:%s",szLastError);
    
    return FALSE;
}
BOOL CMyXmlParser::GetEOrders( std::vector<CEOrder> &_vctEOrders )
{
//     <?xml version="1.0" encoding="UTF-8"?>
//     <!--根据车辆编号 接单人编号获取任务单信息 -->
//     <!-- 向金洋系统请求获取一个电子作业单的信息" -->
//     <result>
//         <!--"获取操作成功或失败：true 成功；false 失败"-->
//         <success>true</success>
//         <!--"提示信息"-->
//         <message>车辆编号错误</message>
// 
//         <!-- 下面是物流信息电子作业单 -->
//         <dzzyd>
//             <!--物流单基本信息-->
//             <jbxx>
//                 <!-- 物流单编号 GUID编号 -->
//                 <wldbh>30842738-AAB4-4c69-B81C-6C9E91157B0B</wldbh>
//                 <!--物流单类型名称-->
//                 <lx>物流任务类型名称</lx>
//                 <!--任务标题 chari20)-->
//                 <tile>物流任务标题</title>
//                 <!--计划开始时间 -->
//                 <jhkssj>2012-12-10 09:30:00</jhkssj>
//                 <!--计划完成时间 -->
//                 <jhwcsj>2012-12-15 09:30:00</jhwcsj>
//                 <!--本任务单备注信息 char(256)-->
//                 <bz>物流任务备注信息</bz>
//             </jbxx>
// 
//             <!--接单人信息-->
//             <jdr>
//                 <!--comment="车牌号" char(10)-->
//                 <cph>浙B7A530</cph>
//                 <item>
//                     <!--接单人类型 1：驾驶员；2押运员-->
//                     <lx>1</lx>
//                     <!--comment="接单人编号" char(40)-->
//                     <bh>483C0430-AB5D-44dc-9414-8AFAB0079451</bh>
//                 </item>
//                 <!--存在多个接单人-->
//             </jdr>
// 
//             <!--运载货物信息-->
//             <cargo>
//                 <!--货物名称 char(40)-->
//                 <mc>甲基甲烯酸丙酯</mc>
//                 <!--计划装载货物总数量 单位吨 ？ char(10)-->
//                 <sl>24.5</sl>
//             </cargo>
// 
//             <!--客户单位信息 -->
//             <khdw>
//                 <!--单位名称-->
//                 <mc>单位名称</mc>
//                 <!--联系人名称-->
//                 <lxr>联系人名称</lxr>
//                 <!--联系人电话-->
//                 <lxrdh>联系人电话</lxrdh>
//             </khdw>
//             

//             <!--卸货地点信息 -->
//             <zxhdd>
//                 <!--分卸地点信息-->
//                 <item>
//                     <!--序号 1 2 3....-->
//                     <xh>1</xh>
// 
//                     <!--起点名称-->
//                     <qdmc></qdmc>
//                     <!--起点经度-->
//                     <qdjd></qdjd>
//                     <!--起点纬度-->
//                     <qdwd></qdwd>
// 
//                     <!--终点名称-->
//                     <zdmc></zdmc>
//                     <!--终点经度-->
//                     <zdjd></zdjd>
//                     <!--终点纬度-->
//                     <zdwd></zdwd>
// 
//                     <!-- 装卸类型:可以是：装/卸/途中,由终点的操作决定。例如去终点卸货，则定义为卸；只是经过终点，则定义为途中-->
//                     <zxhlx>装/卸/途中</zxhlx>	
//                     <!-- 装载状态 ：空车，重车-->
//                     <zzzt></zzzt>
// 
//                     <!-- 货物名称 -->
//                     <pm></pm>
// 
//                     <!--数量-->
//                     <sl></sl>
// 
//                     <!--终点联系单位-->
//                     <lxdw></lxdw>
// 
//                     <!--终点联系人-->
//                     <lxr></lxr>
//                     <!--终点联系电话-->
//                     <lxdh></lxdh>
//                     <!--备注信息-->
//                     <bz></bz>
//                 </item>
//                 <!--可能有多个卸货地点-->
//                 ...
//             </zxhdd>

//             <!--胎压阈值配置表-->
//             <ty>
//                 <!--胎压阈值表编号GUID编号 -->
//                 <id>1D0418EF-A870-46d8-A02E-103E1817AA47</id>
//                 <!-- 某个传感器信息 -->
//                 <item>
//                     <!--传感器所在位置编号，请用十六进制数值表示-->
//                     <wz>0x0211</wz>
//                     <!--黄色胎压预警下限值 单位：Bar-->
//                     <ytyxx>20.5</ytyxx>
//                     <!--黄色胎压预警上限值-->
//                     <ytysx>23</ytysx>
//                     <!--红色胎压预警下限值-->
//                     <rtyxx>18</rtyxx>
//                     <!--红色胎压预警上限值-->
//                     <rtysx>20.4</rtysx>
//                     
//                     <!--黄色胎温预警下限值，单位：摄氏度-->
//                     <ytwxx>69.3</ytwxx>
//                     <!--黄色胎温预警上限值-->
//                     <ytwsx>71.3</ytwsx>
//                     <!--红色胎温预警下限值-->
//                     <rtwxx>71.4</rtwxx>
//                     <!--红色胎温预警上限值-->
//                     <rtwsx>80.4</rtwsx>
//                 </item>
//                 <!--有多个传感器信息 -->
//             </ty>
// 
//             <!--电子操作票信息-->
//             <dzczp>
//                 <!--电子操作票编号 GUID-->
//                 <id>A6432FC6-E65B-4d42-B775-5F3D15EBA020</id>
//                 <!--电子操作规程信息-->
//                 <item>
//                     <!--规程编号 GUID-->
//                     <id>FC4CBE4A-90DB-4071-8140-3F53B302063A</id>
//                     <!--规程序号 1 2 3 ...-->
//                     <xh>2</xh>
//                     <!--规程类别名称：出车门检、装货、卸货 ？-->
//                     <lx>卸货规程</lx>
//                     <!--该项规程计划完成时间-->
//                     <jhwcsj>2012-12-10 14:30:00</jhwcsj>
// 
//                     <!--作业信息-->
//                     <subitem>
//                         <!--作业编号 GUID -->
//                         <id>A52A1D49-6D00-43a0-80DE-065E71DAB0F7</id>
//                         <!--作业序号 1 2 3...-->
//                         <xh>1</xh>
//                         <!--作业类别名称：安全检查 空车过磅、车辆装载、重车过磅、安全检查... ？-->
//                         <lx>安全检查</lx>
//                         <!--该项作业计划完成时间-->
//                         <jhwcsj>2012-12-10 14:30:00</jhwcsj>
//                         <!--具体的作业内容-->
//                         <zynr>
//                             <!--作业内容编号 GUID-->
//                             <id>DCB25B0C-264E-43e0-A186-89F2FE6CACDE</id>
//                             <!--作业内容序号 1 2 3...-->
//                             <xh>1</xh>
//                             <!--作业内容：零位确认 车辆上磅 人员确认... ？-->
//                             <nr>零位确认</nr>
//                         </zynr> 
//                         <zynr>
//                             <id>00780DE7-663D-41f9-9FE4-722C007D20CD</id>
//                             <xh>2</xh>
//                             <nr>车辆上磅</nr>
//                         </zynr> 
//                         <!--可能有多项作业内容-->
//                     </subitem>
//                     <!-- 可能有多项作业信息 -->
//                 </item>
//                 <!-- 可能有多项电子操作规程 -->
//             </dzczp>
//         </dzzyd>
//     　　<!-- 可能有多项物流信息电子作业单 -->
//     </result>

    char szLastError[1024] = {0};
    if (NULL == m_xmlDoc && !LoadFile())
    {
        ::memcpy(szLastError,m_szLastError,sizeof(szLastError));
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"全局XML文档对象为空且重新加载失败:%s",szLastError);
        
        return FALSE;
    }
    
    try
    {
        //# 查找所有<result/dzzyd>节点
        MSXML2::IXMLDOMNodeListPtr pEOrderList = NULL;
        pEOrderList = m_xmlDoc->selectNodes(L"result//dzzyd");
        if( NULL != pEOrderList )
        {
            for (int i=0;i<min(pEOrderList->Getlength(),5);i++)/*任务单数量：最好不超过5条*/
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

                        //# 查找所有<result/dzzyd/jdr/item>节点
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
                        //# 查找所有<result/dzzyd/zxhdd/item>节点
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
                                    
                                    //# 只需要解析终点相关信息
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
                                    
                                    std::string strZXHLX = "";//# 装卸货类型
                                    xmlNode = xmlLoadPointNode->selectSingleNode(L"zxhlx");
                                    if (NULL != xmlNode)
                                    {
                                        strZXHLX = xmlNode->text.operator const char*();
                                    }

                                    if (strZXHLX.find("装") != -1)
                                    {
                                        EOrderInfo.m_LoadPoints.push_back(LoadPoint);
                                    }
                                    else if (strZXHLX.find("卸") != -1)
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
                        
                        //# 查找所有<result/dzzyd/ty/item>节点
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
                        
                        //# 查找所有<result/dzzyd/dzczp/item>节点
                        MSXML2::IXMLDOMNodeListPtr pProcedureList = NULL;
                        pProcedureList = xmlEOptTickNode->selectNodes(L"item");
                        if( NULL != pProcedureList )
                        {
                            for (int j=0;j<min(pProcedureList->Getlength(),5);j++)/*规程数量：最好不超过5个*/
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
                                    
                                    //# 查找所有<result/dzzyd/dzczp/item/subitem>节点
                                    MSXML2::IXMLDOMNodeListPtr pSubProcedureList = NULL;
                                    pSubProcedureList = xmlProcedureNode->selectNodes(L"subitem");
                                    if( NULL != pSubProcedureList )
                                    {
                                        for (int k=0;k<min(pSubProcedureList->Getlength(),10);k++)/*作业数量：最好不超过10个*/
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

                                                
                                                //# 查找所有<result/dzzyd/dzczp/item/subitem/zynr>节点
                                                MSXML2::IXMLDOMNodeListPtr pSPContentList = NULL;
                                                pSPContentList = xmlSubProcedureNode->selectNodes(L"zynr");
                                                if( NULL != pSPContentList )
                                                {
                                                    for (int l=0;l<min(pSPContentList->Getlength(),20);l++)/*作业内容个数：最好不超过20项*/
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
        ::sprintf(szLastError,"未知");
    }
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"获取任务单列表出现异常:%s",szLastError);
    
    return FALSE;
}

BOOL CMyXmlParser::LoadXml_SAFEARRARY( PCHAR _pData )
{
    if (!_pData)
    {
        ZeroMemory(m_szLastError,sizeof(m_szLastError));
        ::sprintf(m_szLastError,"请求解析的字符串指针为空");
        
        return FALSE;
    }
    
    //# 先释放
    if( m_xmlDoc != NULL )
    {
        m_xmlDoc.Release();
        m_xmlDoc = NULL;
    }
    
    char szLastError[1024] = {0};
    try
    {    
        //# 初始化XML文档对象
        if(SUCCEEDED(m_xmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument30)))) 
        {
            //# 加载XML字符串
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
        ::sprintf(m_szLastError,"未知");
    }
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"加载XML格式内容时出现异常:%s",szLastError);
    
	return FALSE;
}

BOOL CMyXmlParser::GetTyreConfigTables( std::vector<CTyreConfigTable> &_vctTables )
{
	//	   <?xml version="1.0" encoding="utf-8" ?>
	//	   <param>
	//		   <!--终端手机号-->
	//		   <mobile>13800010001</mobile>
	//		   <!--车牌号-->
	//		   <vehiclelic>闽 D10011</vehiclelic>
	// 
	//		   <!--胎压配置表，可以单独下发车头/挂车的配置表，也可以同时下发。同时下
	//		   发则有两个<tyreconfigtable>结构-->
	//		   <tyreconfigtable>
	//			   <!--类型：1 车头，2 挂车-->
	//			   <type>1</type>
	//			   <!--接收器，当类型为 1-车头时才需要-->
	//			   <receptor>3434ef-dfdsf-erer</receptor>
	//			   <!--中继器，当类型为 2-挂车时才需要-->
	//			   <repeater>3434ef-dfdsf-erer</repeater>
	// 
	//			   <!--传感器列表-->
	//			   <items>
	//				   <!--传感器-->
	//				   <item>
	//				   <!--传感器位置编号，请用十六进制数值表示  char(10)，参考 6.1节内容-->
	//				   <no>0x0211</no>
	//				   <!--传感器 ID，请用十六进制数值表示	char(10)-->
	//				   <id>0x12345678</id>
	//				   </item>
	// 
	//				   <!--传感器-->
	//				   <item>
	//				   <!--传感器位置编号，请用十六进制数值表示  char(10)，参考 6.1节内容-->
	//				   <no>0x0221</no>
	//				   <!--传感器 ID，请用十六进制数值表示	char(10)-->
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
        ::sprintf(m_szLastError,"全局XML文档对象为空且重新加载失败:%s",szLastError);
        
        return FALSE;
    }
    
    try
    {
        //# 查找所有<param/tyreconfigtable>节点
        MSXML2::IXMLDOMNodeListPtr pTableList = NULL;
        pTableList = m_xmlDoc->selectNodes(L"param//tyreconfigtable");
        if( NULL == pTableList )
        {
            return TRUE;
        }

        int iTableCnt = min(pTableList->Getlength(),2);/*胎压传感器自动识别配置表最多两个：车头+挂车*/
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

            if ( 1 == table.m_byType )// 配置类型：1-车头
            {
                MSXML2::IXMLDOMNodePtr xmlReceptorNode = xmlTableNode->selectSingleNode(L"receptor");
                if ( NULL == xmlReceptorNode )
                {
                    continue;
                }

                table.m_strReceptorOrRepeator = xmlReceptorNode->text.operator const char*();
            } // 
            else if ( 2 == table.m_byType )// 2-挂车
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
        ::sprintf(szLastError,"未知");
    }
    
    ZeroMemory(m_szLastError,sizeof(m_szLastError));
    ::sprintf(m_szLastError,"获取胎压传感器自动识别配置表出现异常:%s",szLastError);
    
    return FALSE;
}
