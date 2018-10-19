#pragma  once

/**************************************************************************
**       
** Copyright (C) 2018, Company Corporation
** All rights reserved.  
** 
** @file  tool.h
** @brief ����ָ����λ�ý������������ȡ
**  
** @Details   
**  
** @version	1.0
** @author	Jiang.yc
** @date	2018/08/21 16:23 
**  
** @history  
** 
****************************************************************************/



/** 
 * @name DEL_PTR_AND_NULL
 * @brief ��ȫɾ������ָ��
 *
 * @param[in] pData ��ɾ���Ķ���ָ��
 * @param[out] ��
 *
 * @return ��
 *
 */
#define DEL_PTR_AND_NULL(pData) {delete pData, pData = NULL;}

/** 
 * @name DEL_ARRAY_AND_NULL
 * @brief ��ȫɾ���ַ�����ָ��
 *
 * @param[in] pData ��ɾ�����ַ�����ָ��
 * @param[out] ��
 *
 * @return ��
 *
 */
#define DEL_ARRAY_AND_NULL(pData) {delete []pData, pData = NULL;}

/** 
 * @name MAKE_DATA
 * @brief ��type���͵�����val���Ƶ�ptr_pos��ָ��λ�ô�������ptr_pos����val�ĳ���
 *
 * @param[in] ptr_pos �����Ļ���ָ��
 * @param[in] type ��������������
 * @param[in] val ��������������
 * @param[out] ��
 *
 * @return ��
 *
 */
#define MAKE_DATA(ptr_pos,type,val) \
				{ \
				*((type*)ptr_pos) = val;\
				ptr_pos += sizeof(type); \
				}

/** 
 * 
 * @name MAKE_STRING
 * @brief ������Ϊlen���ַ������Ƶ�ptr_pos��ָ��λ�ô�������ptr_pos����len�ĳ���
 *
 * @param[in] ptr_pos �����Ļ���ָ��
 * @param[in] ptr_src �������ַ�������
 * @param[in] len ������ַ�������
 * @param[out] ��
 *
 * @return ��
 *
 */
#define MAKE_STRING(ptr_pos,ptr_src,len) \
				{ \
				::memcpy( ptr_pos,ptr_src,len ); \
				ptr_pos += len; \
				}

/** 
 * 
 * @name GET_DATA
 * @brief ��ptr_pos�������ݣ���Ϊtype���Ͷ�ȡ������ptr_pos����type���͵ĳ���
 *
 * @param[in] ptr_pos ����ȡ�Ļ���ָ��
 * @param[in] type ��������������
 * @param[out] ��
 *
 * @return ��
 *
 */
#define GET_DATA(ptr_pos,type)  *((type*)ptr_pos); \
				ptr_pos += sizeof(type)


/** 
 * 
 * @name GET_STRING
 * @brief ��ptr_pos������Ϊlen���ַ���������ptr_des�������ڽ�β�����'\0',��ptr_pos����len�ĳ���
 *
 * @param[in] ptr_pos ����ȡ�Ļ���ָ��
 * @param[in] ptr_des ��ȡ�����ַ������λ��
 * @param[in] len �������ַ�������
 * @param[out] ��
 *
 * @return ��
 *
 */
#define GET_STRING(ptr_pos,ptr_des,len) \
				{ \
				::memcpy( ptr_des,ptr_pos,len ); \
				ptr_des[len] = 0; \
				ptr_pos += len; \
				}


//��ֹ���������һЩ������Ϣ
#pragma warning(disable:4267) //
#pragma warning(disable:4102) //���棺�����˱�ǩ������δ���ù������������Ըñ�ǩ��
#pragma warning(disable:4244) //

