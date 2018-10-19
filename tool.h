#pragma  once

/**************************************************************************
**       
** Copyright (C) 2018, Company Corporation
** All rights reserved.  
** 
** @file  tool.h
** @brief 根据指定的位置进行数据填充或读取
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
 * @brief 安全删除对象指针
 *
 * @param[in] pData 待删除的对象指针
 * @param[out] 无
 *
 * @return 无
 *
 */
#define DEL_PTR_AND_NULL(pData) {delete pData, pData = NULL;}

/** 
 * @name DEL_ARRAY_AND_NULL
 * @brief 安全删除字符数组指针
 *
 * @param[in] pData 待删除的字符数组指针
 * @param[out] 无
 *
 * @return 无
 *
 */
#define DEL_ARRAY_AND_NULL(pData) {delete []pData, pData = NULL;}

/** 
 * @name MAKE_DATA
 * @brief 将type类型的数据val复制到ptr_pos所指的位置处，并将ptr_pos加上val的长度
 *
 * @param[in] ptr_pos 待填充的缓存指针
 * @param[in] type 待填充的数据类型
 * @param[in] val 待填充的数据内容
 * @param[out] 无
 *
 * @return 无
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
 * @brief 将长度为len的字符串复制到ptr_pos所指的位置处，并将ptr_pos加上len的长度
 *
 * @param[in] ptr_pos 待填充的缓存指针
 * @param[in] ptr_src 待填充的字符串内容
 * @param[in] len 待填充字符串长度
 * @param[out] 无
 *
 * @return 无
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
 * @brief 将ptr_pos处的数据，作为type类型读取，并将ptr_pos加上type类型的长度
 *
 * @param[in] ptr_pos 待读取的缓存指针
 * @param[in] type 待填充的数据类型
 * @param[out] 无
 *
 * @return 无
 *
 */
#define GET_DATA(ptr_pos,type)  *((type*)ptr_pos); \
				ptr_pos += sizeof(type)


/** 
 * 
 * @name GET_STRING
 * @brief 将ptr_pos处长度为len的字符串拷贝到ptr_des处，并在结尾处添加'\0',将ptr_pos加上len的长度
 *
 * @param[in] ptr_pos 待读取的缓存指针
 * @param[in] ptr_des 读取到的字符串存放位置
 * @param[in] len 待填充的字符串长度
 * @param[out] 无
 *
 * @return 无
 *
 */
#define GET_STRING(ptr_pos,ptr_des,len) \
				{ \
				::memcpy( ptr_des,ptr_pos,len ); \
				ptr_des[len] = 0; \
				ptr_pos += len; \
				}


//禁止编译器输出一些警告信息
#pragma warning(disable:4267) //
#pragma warning(disable:4102) //警告：定义了标签，但从未引用过。编译器忽略该标签。
#pragma warning(disable:4244) //

