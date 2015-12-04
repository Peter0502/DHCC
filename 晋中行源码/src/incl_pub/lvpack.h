/*************************************************
  Copyright (C), 2005, DHCC
  File name:     lvpack.h
  Author:    韦昭   Version:   V1.0     Date:  2005/7/21
  Description:   小额支付报文结构及相关的功能函数。
  Others: 
  Function List:
	1. int  lv_load_tagifno()
	 　在使用以下函数之前，必须用这个函数来载入tag信息。
	 　操作成功返回0，失败返回-1
    2. int  lv_set_tag(TLVPACKAGE *,char *tagName,char *data);
	   在报文体中插入某个TAG内容，非法TAG名返回-1,否则返回0。
	3. int  lv_set_tag_before(TLVPACKAGE *,char *beforeTagName,
	                             char *tagName,char *data);
	   在当前报文体中某个TAG之前插入一个TAG，非法TAG名或插入位置不存在返
	   回-1,否则返回0。
	4. int  lv_gen_pack(TLVPACKAGE *);
	   用于最终组织报文数据，包括报文头和报文体,。
	5. int  lv_analyze_pack(TLVPACKAGE *);
	   分析报文数据，根据报文数据中报文头的内容填充TLVPACKAGE中的字
	   段，并检查报文体是否合法，成功返回0，否则返回-1。
	6. int  lv_get_tag(TLVPACKAGE *,char *tagName,char *data);
       得到某个TAG的内容,若指定TAG不存在则置data[0]='\0'且返回-1,否
	   则设置data志并返回0。
	7. void lv_reset(TLVPACKAGE *)
	   结构体清零，请在重新使用一个结构对象前使用，确保安全
	8. int  lv_fetchpkg(TLVPACKAGE *ihv,TLVPACKAGE *ohv)
	   解析批量报文，成功返回0，异常返回-1，没有记录返回1
	9. int  lv_create(TLVPACKAGE *);
	   由于TLVPAKAGE使用链表结构，所以在使用TLVPAKAGE表要进行初始化，成功返回0,失败返回-1
	10.int  lv_destroy(TLVPACKAGE *);
	   对应lv_create()函数，对结构进行释放，成功返回0，失败返回-1
	11.int  lv_batch_end(TLVPACKAGE *);
	   结束一个批量业务的输入,CMT和PKG报文能用（以上同），成功返回0，失败返回-1
	12.int  lv_isFile(TLVPACKAGE *lv);
	   判断报文是不是文件报文。
	
  History:   
    1. Date:         2005-7-21
       Author:       韦昭
       Modification: 建立
	2. Date:		2005-11-3
	   Author:		 韦昭
	   Modification:　增加lv_isFIle()函数.
	3.Date			2005-11-9
	  Author:		卿苏德
	  Modification:	Col.52 lv_tag.h modified
*************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include "hv_tag.h"
/*have already modified to lv_tag.h*/
#include "dmem.h"

#ifndef _T_LVPACKAGE
#define _T_LVPACKAGE

/*宏定义区*/
#define LV_OP_SUCCEE  0     /*操作成功返回值*/
#define LV_OP_FAIL     -1    /*操作失败返回值*/
#define LV_TAGNAME_LEN 3
#define LV_BODY_LENGTH 28*1024+1
#define LV_PKGHEADER_SIZE 69
#define LV_TRADEHEADER_SIZE 64
#define LV_PKGTRADEHEADER_SIZE 16
#define LV_BHEADER_SIZE 44
#define LV_PH_LEN_POS 3
#define LV_PH_LEN_LEN 6
#define LV_PH_USE_POS 9
#define LV_PH_CMT_POS 57
#define LV_TH_CMT_POS 3
#define LV_BANKCODE_FLGNO "104100004013"  /*银行行号*/

extern TTAGINFO lv_taginfo;         /*这个结构被用于记录tag及对应长度的影射信息*/

/* 小额支付报文结构  */

typedef struct _t_lvpackage
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

	int  pos;				 /*分析批量报文时的偏移位置*/

	int  ITXDebitTotalCount; /*来账借记总笔数*/
	int  ITXCreditTotalCount;/*来账贷记总笔数*/
	int  OTXDebitTotalCount; /*往账借记总笔数*/
	int  OTXCreditTotalCount;/*往账贷记总笔数*/

	unsigned int  packID;			/*PKG包的ID,字符状态为8位s*/
	char packDate[8];		/*包的日期*/
	char respDate[8];		/*应答日期*/	

	char CMTNo[3];			/*当前批量业务CMT号或业务要素集号*/
	char paymPRI;			/*支付优先0：一般1：紧急 2：特急*/
	char localSysSign[40];	/*地方押/全国押*/
	char reserve[16];		/*预留*/
	
	/*下面几个变量只在内部调用*/
	int  pkgType;			/*包的类型，PKG(1)还是CMT(0)*/
	int  isFile;			/*是否文件类型，不对接口开放，内部处理，是文件类型为1*/
	int  isMulti;			/*是否多笔交易的报文，0为否，1为是*/

	int  tradeEnd;			/*是否一个交易结束，1为结束，0为未结束，一开始算是结束*/
	int  tradeStartP;		/*交易开始指针*/
	
	/* 报文体数据（28k，报文头＋业务头+报文体） */
	TDMEM dm;

}TLVPACKAGE;


/*函数*/
int  lv_load_tagifno();
int  lv_set_tag(TLVPACKAGE *lv,char *tagName,char *data);
/*在原tag值处追加data中的数据*/
int  lv_set_tag_multi(TLVPACKAGE *lv,char *tagName,char *data);
int  lv_set_tag_before(TLVPACKAGE *lv,char *beforeTagName,
	                             char *tagName,char *data);
int  lv_gen_pack(TLVPACKAGE *lv);
int  lv_analyze_pack(TLVPACKAGE *lv);
int  lv_get_tag(TLVPACKAGE *lv,char *tagName,char *data);
int lv_get_tag_p(TLVPACKAGE *lv,char *tagName);
/*取不定长的tag,在离tag值offset字节处开始取size个字节，0表示从开始取，9表示从第10个字节开始取*/
int  lv_get_tag_multi(TLVPACKAGE *lv,char *tagName,char *data,int offset,int size);	 
void lv_reset(TLVPACKAGE *lv);
int  lv_fetchpkg(TLVPACKAGE *ilv,TLVPACKAGE *olv);
int  lv_create(TLVPACKAGE *lv);
int  lv_destroy(TLVPACKAGE *lv);
int  lv_batch_end(TLVPACKAGE *lv); 
int  lv_pkg_batch(TLVPACKAGE *lv,char *tagName,char *value);
int  lv_pkg_getHeaderTag(TLVPACKAGE *lv,char *tagName,char *value);
int  lv_isFile(TLVPACKAGE *lv);
int  lv_add_mac(TLVPACKAGE *lv);

static int lv_gen_pack_init(TLVPACKAGE *lv);
static int  lv_fetchpkg_PKG(TLVPACKAGE *ilv,TLVPACKAGE *olv);
static int  lv_fetchpkg_CMT(TLVPACKAGE *ilv,TLVPACKAGE *olv);
static int lv_dtagcpy(TDMEM *dm,char *tagName,char *tagData);
int lv_dgetNextTag(TDMEM *dm,int num,char *tagName,char *value);
static int lv_gen_pack_PKGMulti(TLVPACKAGE *lv);
static int lv_gen_pack_CMTMulti(TLVPACKAGE *lv);
static int lv_gen_pack_CMTSingle(TLVPACKAGE *lv);
static int lv_apk_pkgHeader(TLVPACKAGE *lv);
static int lv_apk_isCMTMulti(TLVPACKAGE *lv);
static int lv_apk_CMTMulti(TLVPACKAGE *lv);
static int lv_apk_isPKGMulti(TLVPACKAGE *lv);
static int lv_apk_PKGMulti(TLVPACKAGE *lv);
static int lv_apk_isPKGSingle(TLVPACKAGE *lv);
static int lv_apk_PKGSingle(TLVPACKAGE *lv);
static int lv_apk_isCMTTrade(TLVPACKAGE *lv);
static int lv_apk_CMTTrade(TLVPACKAGE *lv);
static int lv_get_pack_init(TLVPACKAGE *lv);
static int lv_get_flgNo(TLVPACKAGE *lv);
static int lv_get_reNo(TLVPACKAGE *lv);
static int lv_gen_pack_zip0(TLVPACKAGE *lv);
static int lv_isTrade(TLVPACKAGE *lv);
static int lv_batch_end_PKG(TLVPACKAGE *lv);
static int lv_batch_end_CMT(TLVPACKAGE *lv);

int lv_checksum(TLVPACKAGE *lv);


/*增加的函数，内部调用*/
#endif

