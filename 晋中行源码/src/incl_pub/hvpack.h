/*************************************************
  Copyright (C), 2004, DHC
  File name:     hvpack.h
  Author:    韦昭   Version:   V1.0     Date:  2004/8/19
  Description:    大额支付报文结构及相关的功能函数。
  Others: 
  Function List:  (函数定义，实现在hvpack.c文件中)
	1. int  hv_load_tagifno(TTAGINFO *ti,char *fileName)
	 　在使用以下函数之前，必须用这个函数来载入tag信息。
	 　操作成功返回0，失败返回-1
    2. int hv_set_tag(THVPACKAGE *,char *tagName,char *data);
	   在报文体中插入某个TAG内容，非法TAG名返回-1,否则返回0。
	3. void hv_set_tag_before(THVPACKAGE *,char *beforeTagName,
	                             char *tagName,char *data);
	   在报文体中某个TAG之前插入一个TAG，非法TAG名或插入位置不存在返
	   回-1,否则返回0。
	4. void hv_gen_pack(THVPACKAGE *);
	   组织报文数据，包括报文头和报文体。
	5. int hv_analyze_pack(THVPACKAGE *);
	   分析报文数据，根据报文数据中报文头的内容填充THVPACKAGE中的字
	   段，并检查报文体是否合法，成功返回0，否则返回-1。
	6. int hv_get_tag(THVPACKAGE *,char *tagName,char *data);
       得到某个TAG的内容,若指定TAG不存在则置data[0]='\0'且返回-1,否
	   则设置data志并返回0。
	7. void hv_reset(THVPACKAGE *)
	   结构体清零，请在重新使用一个结构对象前使用，确保安全
	8. int  hv_fetchpkg(THVPACKAGE *ihv,THVPACKAGE *ohv)
	   解析批量报文，成功返回0，异常返回-1，没有记录返回1
  History:   
    1. Date:         2004-8-19
       Author:       韦昭
       Modification: 建立
	2. Date:     2004-8-26
	   Author:　 韦昭
	   Modification:  更新
	3. Date:	 2005-3-28
	   Author:	 韦昭
	   Modifiaction:  更新，添加处理批量报文的功能。
*************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>

#include "hv_tag.h"

#ifndef _T_HVPACKAGE
#define _T_HVPACKAGE
#include"hv_define.h"
/*宏定义区*/
#define HV_1ST_TAG_POS 96    /*第一个tag的开始位置,就是"{3::001:"第二个“:”和起始位置的偏移量*/
#define HV_TEXT_STAR_POS 93  /*正文块起始位置的偏移量*/
#define PHEADER 0            /*报文的起始位置*/
#define LENGTH 3             /*长度偏移*/
#define USE  9               /*报文用途的偏移量*/
#define MARK 10              /*来往标记偏移量*/
#define OPETYPE 11           /*业务类型偏移量*/
#define OPESTATECODE 12      /*业务状态代码偏移量*/
#define FLGNO 15             /*报文标识号偏移量*/
#define RENO  23             /*报文参考号偏移量*/
#define SENDTIME  43         /*发送时间偏移量*/
#define CMTCODE  57          /*CMT代码偏移量*/
#define WORKDATE  60         /*工作日期偏移量*/
#define PHEADER_END 68       /*结束位置偏移量*/
#define OPEHEADER 69         /*业务头开始位置偏移量*/
#define OPE_CMTCODE 72       /*业务头CMT代码偏移量*/
#define PRI_POS 75           /*优先级标示偏移量*/
#define FORFUTURE 76         /*预留部分开始位置偏移量*/
#define OPEHEADER_END 92     /*业务头结束位置偏移量*/
#define HV_TAGNAME_LENGTH 3  /*tagName的长度，就用这个*/ 
/** rem by LiuHuafeng 20070110 
#define HV_BODY_LENGTH 6144000 *报文的最大长度6000K*
*****************************************************/
#define HV_BODY_LENGTH 1048576 /*报文的最大长度1M*/
#define RECVDNUM_POS	72	 /*来账借记笔数位置偏移量*/
#define RECVCNUM_POS	82	 /*来账贷记笔数位置偏移量*/
#define SENDDNUM_POS	92	 /*往账借记笔数位置偏移量*/
#define SENDCNUM_POS	102	 /*往账贷记笔数位置偏移量*/
/*#define HV_TAGINFO_FILE "tin"  tag信息文件*/

#define HV_OP_SUCCEE  0     /*操作成功返回值*/
#define HV_OP_FAIL     -1    /*操作失败返回值*/

#define HV_BANKCODE_FLGNO HV_QSBRNO  /*银行行号*/

extern TTAGINFO hv_taginfo;         /*这个结构被用于记录tag及对应长度的影射信息*/
/*extern  int tag_length;      *//*记录当前处理的tag域长度*/
/* 大额支付报文结构  */
typedef struct _t_hvpackage
{
	/* 报文头各个字段+支付优先级 */
	int length ;             /* 报文长度,表示整个报文的长度*/
	char use;                /* 报文用途 预留*/
	char mark;               /* 来往标记 预留*/
	char opeType;            /* 业务类型 0：大额，opeType = operationType
	                             1：小额，
	                             2：其它 */
	char opeStateCode[3];    /* 业务状态码 000 表示正常业务
                                           001 表示来账核全国押错
                                           002 表示来帐核地方押错 */
	char flgNo[8];           /* 报文标识号,在通讯上唯一标识发起方发起的一笔业务 */
	char reNo[20];           /* 报文参考号,用于在接收业务回执时匹配原有的业务指令 */
	char sendTime[14];       /* 发送时间,格式为yyyymmddhhmmss */
	char CMTCode[3];         /* CMT号码，批量传送小额支付业务时为000 */
	char workDate[8];        /*工作日期*/
	char PRI;                /*优先级标记*/
	int  recvDnum;			 /*来账借记笔数 add by weizhao 2005-3-28 批量报文*/
	int  recvCnum;			 /*来账贷记笔数*/
	int  sendDnum;			 /*往账借记笔数*/
	int  sendCnum;			 /*往账贷记笔数*/
	int  pos;				 /*分析批量报文时的偏移位置*/
    
	/* 报文体数据（60k，报文头＋业务头+报文体）*/ 
	char body[HV_BODY_LENGTH];       
}THVPACKAGE;


/* 函数定义 ,意义看前面说明，具体用法看hvpack.c文件 */
/*对外函数*/
int  hv_load_tagifno(char *fileName);
int  hv_set_tag(THVPACKAGE *hv,char *tagName,char *data);
int  hv_set_tag_before(THVPACKAGE *hv,char *beforeTagName,
	                             char *tagName,char *data);
int  hv_gen_pack(THVPACKAGE *hv);
/* int  hv_analyze_pack(THVPACKAGE *hv); */
int  hv_get_tag(THVPACKAGE *hv,char *tagName,char *data);
void hv_reset(THVPACKAGE *hv);
int  hv_fetchpkg(THVPACKAGE *ihv,THVPACKAGE *ohv);

/*增加的函数，内部调用*/
int  isTag(char *tagName);                     /*是否是一个合法的tagName,现在规定是只能是字母或是数字，三位*/
/*char *searchTag(char *str,char *tagName); */     /*在str中查找是否存在tagName，返回tagName前面的":"的位置的指针*/
void getPackageFlgNo(THVPACKAGE *hv);          /*获得flgNo,没有实现从数据库中生成，只是简单的写入一个字符串*/
void getPackageReNo(THVPACKAGE *hv);           /*获得flgNo,没有实现从数据库中生成，只是简单的写入一个字符串*/
int  tagcpy(char *pos,char *tagName,char *data);/*在POS处插入":tagName:data",并返回这个字串的长度*/
int  tagcpy2(char *pos,char *tagName,char *data,int datalen);
#endif

