// MyXmlParser.h: interface for the MyXmlParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYXMLPARSER_H__ECE0ED5F_FF88_464D_A389_0B60807C67A5__INCLUDED_)
#define AFX_MYXMLPARSER_H__ECE0ED5F_FF88_464D_A389_0B60807C67A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#import <msxml6.dll>
#include <string>
#include "PublicDef.h"
#include <vector>

///@brief XML文档处理类
class CMyXmlParser  
{
public:
	CMyXmlParser();
	virtual ~CMyXmlParser();

    CMyXmlParser(std::string _strFile);

public:
    /**
     * 以XML格式加载文件
     * 
     * @retval TRUE  成功
     * @retval FALSE 失败
     */
    BOOL LoadFile(std::string _strFile = "");

    /**
    * 创建XML格式配置文件
    * 
    * @param[in] _strRootName  根节点名称，默认"root"
     * 
     * @retval TRUE  成功
     * @retval FALSE 失败
     */
    BOOL CreateFile(std::string _strFile = "", std::string _strRootName = "");
    
    /**
     * 以XML格式加载字符串
     * 
     * @retval TRUE  成功
     * @retval FALSE 失败
     */
    BOOL LoadXml(std::string _strXml);
    
    /**
     * 以XML格式加载字符串
     * 
     * @retval TRUE  成功
     * @retval FALSE 失败
     */
    BOOL LoadXml_SAFEARRARY(PCHAR _pData);

    /**
     * 创建XML格式字符串
     * 
     * @param[in] _strRootName  根节点名称，默认"root"
     * 
     * @return BOOL
     */
    BOOL CreateXml(std::string _strRootName = "");

    /**
     * 检查节点是否存在
     * 
     * @param[in] _strNode       节点名称，即tagname
     * @param[in] _strParentNode 父节点
     * 
     * @retval TRUE  节点存在
     * @retval FALSE 节点不存在
     * 
     */
    BOOL CheckNode(std::string _strNode, std::string _strParentNode = "");

    /**
     * 添加节点
     * 
     * @param[in] _strNode       节点名称，即tagname
     * @param[in] _strValue      节点值
     * @param[in] _strParentNode 不为空，则指定父节点
     * 
     * @retval TRUE  成功
     * @retval FALSE 失败
     */
    BOOL AppendNode(std::string _strNode, std::string _strValue = "", std::string _strParentNode = "");

    /**
     * 将节点添加在父节点的第一个
     * 
     * @param[in] _strNode       节点名称，即tagname
     * @param[in] _strValue      节点值
     * @param[in] _strParentNode 不为空，则指定父节点
     * 
     * @retval TRUE  成功
     * @retval FALSE 失败
     */
    BOOL AppendNodeFront(std::string _strNode, std::string _strValue = "", std::string _strParentNode = "");

    /**
     * 在_strNode2之前添加节点_strNode
     * 
     * @param[in] _strNode       节点名称，即tagname
     * @param[in] _strNode2      指定节点
     * @param[in] _strValue      节点值
     * 
     * @retval TRUE  成功
     * @retval FALSE 失败
     */
    BOOL AppendNodeBefore(std::string _strNode, std::string _strNode2, std::string _strValue = "");

    /**
     * 设置属性值
     *  
     * @param[in] _strNode  节点名
     * @param[in] _strAttr  属性名
     * @param[in] _strValue 属性值
     * 
     * @retval TRUE  成功
     * @retval FALSE 失败
     */
    BOOL SetAttribute(std::string _strNode, std::string _strAttr, std::string _strValue);

    /**
     * 获取属性值
     * 
     * @param[in]  _strNode  节点名
     * @param[in]  _strAttr  属性名
     * @param[out] _strValue 属性值
     * 
     * @retval TRUE  成功
     * @retval FALSE 失败
     */
    BOOL GetAttribute(std::string _strNode, std::string _strAttr, std::string& _strValue);

    /**
     * 获取节点值
     * 
     * @param[in]  _strNode  节点名
     * @param[out] _strValue 节点值
     * 
     * @retval TRUE  成功
     * @retval FALSE 失败
     */
    BOOL GetNodeText(std::string _strNode, std::string& _strValue);

    /**
     * 设置节点值
     * 
     * @param[in]  _strNode  节点名
     * @param[out] _strValue 节点值
     * 
     * @retval TRUE  成功
     * @retval FALSE 失败
    */
    BOOL SetNodeText(std::string _strNode, std::string _strValue);

    /**
     * 保存文档
     * 
     * @retval TRUE  成功
     * @retval FALSE 失败
     */
    BOOL SaveFile( std::string _strFile = "");

    /**
     * 获取当前文档内容
     */
    std::string GetXml();

    /**
     * 获取最近一次错误描述
     */
    const char* GetLastError();

public:
    BOOL GetStaffInfos( std::vector<CStaffInfo> &_vctStaffInfos );
    BOOL GetOilInfos( std::vector<COilInfo> &_vctOilInfos );
    BOOL GetRouteInfos( std::vector<CRouteInfo> &_vctRouteInfos );
    BOOL GetEOrders( std::vector<CEOrder> &_vctEOrders );

    BOOL GetTyreConfigTables( std::vector<CTyreConfigTable> &_vctTables );

private:
    std::string m_strFilePath;
	MSXML2::IXMLDOMDocumentPtr m_xmlDoc;

private:
    char m_szLastError[1024];
};

#endif // !defined(AFX_MYXMLPARSER_H__ECE0ED5F_FF88_464D_A389_0B60807C67A5__INCLUDED_)
