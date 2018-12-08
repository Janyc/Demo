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

///@brief XML�ĵ�������
class CMyXmlParser  
{
public:
	CMyXmlParser();
	virtual ~CMyXmlParser();

    CMyXmlParser(std::string _strFile);

public:
    /**
     * ��XML��ʽ�����ļ�
     * 
     * @retval TRUE  �ɹ�
     * @retval FALSE ʧ��
     */
    BOOL LoadFile(std::string _strFile = "");

    /**
    * ����XML��ʽ�����ļ�
    * 
    * @param[in] _strRootName  ���ڵ����ƣ�Ĭ��"root"
     * 
     * @retval TRUE  �ɹ�
     * @retval FALSE ʧ��
     */
    BOOL CreateFile(std::string _strFile = "", std::string _strRootName = "");
    
    /**
     * ��XML��ʽ�����ַ���
     * 
     * @retval TRUE  �ɹ�
     * @retval FALSE ʧ��
     */
    BOOL LoadXml(std::string _strXml);
    
    /**
     * ��XML��ʽ�����ַ���
     * 
     * @retval TRUE  �ɹ�
     * @retval FALSE ʧ��
     */
    BOOL LoadXml_SAFEARRARY(PCHAR _pData);

    /**
     * ����XML��ʽ�ַ���
     * 
     * @param[in] _strRootName  ���ڵ����ƣ�Ĭ��"root"
     * 
     * @return BOOL
     */
    BOOL CreateXml(std::string _strRootName = "");

    /**
     * ���ڵ��Ƿ����
     * 
     * @param[in] _strNode       �ڵ����ƣ���tagname
     * @param[in] _strParentNode ���ڵ�
     * 
     * @retval TRUE  �ڵ����
     * @retval FALSE �ڵ㲻����
     * 
     */
    BOOL CheckNode(std::string _strNode, std::string _strParentNode = "");

    /**
     * ��ӽڵ�
     * 
     * @param[in] _strNode       �ڵ����ƣ���tagname
     * @param[in] _strValue      �ڵ�ֵ
     * @param[in] _strParentNode ��Ϊ�գ���ָ�����ڵ�
     * 
     * @retval TRUE  �ɹ�
     * @retval FALSE ʧ��
     */
    BOOL AppendNode(std::string _strNode, std::string _strValue = "", std::string _strParentNode = "");

    /**
     * ���ڵ�����ڸ��ڵ�ĵ�һ��
     * 
     * @param[in] _strNode       �ڵ����ƣ���tagname
     * @param[in] _strValue      �ڵ�ֵ
     * @param[in] _strParentNode ��Ϊ�գ���ָ�����ڵ�
     * 
     * @retval TRUE  �ɹ�
     * @retval FALSE ʧ��
     */
    BOOL AppendNodeFront(std::string _strNode, std::string _strValue = "", std::string _strParentNode = "");

    /**
     * ��_strNode2֮ǰ��ӽڵ�_strNode
     * 
     * @param[in] _strNode       �ڵ����ƣ���tagname
     * @param[in] _strNode2      ָ���ڵ�
     * @param[in] _strValue      �ڵ�ֵ
     * 
     * @retval TRUE  �ɹ�
     * @retval FALSE ʧ��
     */
    BOOL AppendNodeBefore(std::string _strNode, std::string _strNode2, std::string _strValue = "");

    /**
     * ��������ֵ
     *  
     * @param[in] _strNode  �ڵ���
     * @param[in] _strAttr  ������
     * @param[in] _strValue ����ֵ
     * 
     * @retval TRUE  �ɹ�
     * @retval FALSE ʧ��
     */
    BOOL SetAttribute(std::string _strNode, std::string _strAttr, std::string _strValue);

    /**
     * ��ȡ����ֵ
     * 
     * @param[in]  _strNode  �ڵ���
     * @param[in]  _strAttr  ������
     * @param[out] _strValue ����ֵ
     * 
     * @retval TRUE  �ɹ�
     * @retval FALSE ʧ��
     */
    BOOL GetAttribute(std::string _strNode, std::string _strAttr, std::string& _strValue);

    /**
     * ��ȡ�ڵ�ֵ
     * 
     * @param[in]  _strNode  �ڵ���
     * @param[out] _strValue �ڵ�ֵ
     * 
     * @retval TRUE  �ɹ�
     * @retval FALSE ʧ��
     */
    BOOL GetNodeText(std::string _strNode, std::string& _strValue);

    /**
     * ���ýڵ�ֵ
     * 
     * @param[in]  _strNode  �ڵ���
     * @param[out] _strValue �ڵ�ֵ
     * 
     * @retval TRUE  �ɹ�
     * @retval FALSE ʧ��
    */
    BOOL SetNodeText(std::string _strNode, std::string _strValue);

    /**
     * �����ĵ�
     * 
     * @retval TRUE  �ɹ�
     * @retval FALSE ʧ��
     */
    BOOL SaveFile( std::string _strFile = "");

    /**
     * ��ȡ��ǰ�ĵ�����
     */
    std::string GetXml();

    /**
     * ��ȡ���һ�δ�������
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
