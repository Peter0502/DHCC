/*************************************************
  Copyright (C), 2005, DHCC
  File name:     lvpack.h
  Author:    Τ��   Version:   V1.0     Date:  2005/7/21
  Description:   С��֧�����Ľṹ����صĹ��ܺ�����
  Others: 
  Function List:
	1. int  lv_load_tagifno()
	 ����ʹ�����º���֮ǰ���������������������tag��Ϣ��
	 �������ɹ�����0��ʧ�ܷ���-1
    2. int  lv_set_tag(TLVPACKAGE *,char *tagName,char *data);
	   �ڱ������в���ĳ��TAG���ݣ��Ƿ�TAG������-1,���򷵻�0��
	3. int  lv_set_tag_before(TLVPACKAGE *,char *beforeTagName,
	                             char *tagName,char *data);
	   �ڵ�ǰ��������ĳ��TAG֮ǰ����һ��TAG���Ƿ�TAG�������λ�ò����ڷ�
	   ��-1,���򷵻�0��
	4. int  lv_gen_pack(TLVPACKAGE *);
	   ����������֯�������ݣ���������ͷ�ͱ�����,��
	5. int  lv_analyze_pack(TLVPACKAGE *);
	   �����������ݣ����ݱ��������б���ͷ���������TLVPACKAGE�е���
	   �Σ�����鱨�����Ƿ�Ϸ����ɹ�����0�����򷵻�-1��
	6. int  lv_get_tag(TLVPACKAGE *,char *tagName,char *data);
       �õ�ĳ��TAG������,��ָ��TAG����������data[0]='\0'�ҷ���-1,��
	   ������data־������0��
	7. void lv_reset(TLVPACKAGE *)
	   �ṹ�����㣬��������ʹ��һ���ṹ����ǰʹ�ã�ȷ����ȫ
	8. int  lv_fetchpkg(TLVPACKAGE *ihv,TLVPACKAGE *ohv)
	   �����������ģ��ɹ�����0���쳣����-1��û�м�¼����1
	9. int  lv_create(TLVPACKAGE *);
	   ����TLVPAKAGEʹ������ṹ��������ʹ��TLVPAKAGE��Ҫ���г�ʼ�����ɹ�����0,ʧ�ܷ���-1
	10.int  lv_destroy(TLVPACKAGE *);
	   ��Ӧlv_create()�������Խṹ�����ͷţ��ɹ�����0��ʧ�ܷ���-1
	11.int  lv_batch_end(TLVPACKAGE *);
	   ����һ������ҵ�������,CMT��PKG�������ã�����ͬ�����ɹ�����0��ʧ�ܷ���-1
	12.int  lv_isFile(TLVPACKAGE *lv);
	   �жϱ����ǲ����ļ����ġ�
	
  History:   
    1. Date:         2005-7-21
       Author:       Τ��
       Modification: ����
	2. Date:		2005-11-3
	   Author:		 Τ��
	   Modification:������lv_isFIle()����.
	3.Date			2005-11-9
	  Author:		���յ�
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

/*�궨����*/
#define LV_OP_SUCCEE  0     /*�����ɹ�����ֵ*/
#define LV_OP_FAIL     -1    /*����ʧ�ܷ���ֵ*/
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
#define LV_BANKCODE_FLGNO "104100004013"  /*�����к�*/

extern TTAGINFO lv_taginfo;         /*����ṹ�����ڼ�¼tag����Ӧ���ȵ�Ӱ����Ϣ*/

/* С��֧�����Ľṹ  */

typedef struct _t_lvpackage
{
	/* ����ͷ�����ֶ�+֧�����ȼ� */
	int length ;             /* ���ĳ���,��ʾ�������ĵĳ���*/
	char use;                /* ������; Ԥ��*/
	char mark;               /* ������� Ԥ��*/
	char opeType;            /* ҵ������ 0����opeType = operationType
	                                           1��С�
											   2������ */
	char opeStateCode[3];    /* ҵ��״̬�� 000 ��ʾ����ҵ��
                                           001 ��ʾ���˺�ȫ��Ѻ��
                                           002 ��ʾ���ʺ˵ط�Ѻ�� */
	char flgNo[8];           /* ���ı�ʶ��,��ͨѶ��Ψһ��ʶ���𷽷����һ��ҵ�� */
	char reNo[20];           /* ���Ĳο���,�����ڽ���ҵ���ִʱƥ��ԭ�е�ҵ��ָ�� */
	char sendTime[14];       /* ����ʱ��,��ʽΪyyyymmddhhmmss */
	char CMTCode[3];         /* CMT���룬��������С��֧��ҵ��ʱΪ000 */
	char workDate[8];        /*��������*/

	int  pos;				 /*������������ʱ��ƫ��λ��*/

	int  ITXDebitTotalCount; /*���˽���ܱ���*/
	int  ITXCreditTotalCount;/*���˴����ܱ���*/
	int  OTXDebitTotalCount; /*���˽���ܱ���*/
	int  OTXCreditTotalCount;/*���˴����ܱ���*/

	unsigned int  packID;			/*PKG����ID,�ַ�״̬Ϊ8λs*/
	char packDate[8];		/*��������*/
	char respDate[8];		/*Ӧ������*/	

	char CMTNo[3];			/*��ǰ����ҵ��CMT�Ż�ҵ��Ҫ�ؼ���*/
	char paymPRI;			/*֧������0��һ��1������ 2���ؼ�*/
	char localSysSign[40];	/*�ط�Ѻ/ȫ��Ѻ*/
	char reserve[16];		/*Ԥ��*/
	
	/*���漸������ֻ���ڲ�����*/
	int  pkgType;			/*�������ͣ�PKG(1)����CMT(0)*/
	int  isFile;			/*�Ƿ��ļ����ͣ����Խӿڿ��ţ��ڲ��������ļ�����Ϊ1*/
	int  isMulti;			/*�Ƿ��ʽ��׵ı��ģ�0Ϊ��1Ϊ��*/

	int  tradeEnd;			/*�Ƿ�һ�����׽�����1Ϊ������0Ϊδ������һ��ʼ���ǽ���*/
	int  tradeStartP;		/*���׿�ʼָ��*/
	
	/* ���������ݣ�28k������ͷ��ҵ��ͷ+�����壩 */
	TDMEM dm;

}TLVPACKAGE;


/*����*/
int  lv_load_tagifno();
int  lv_set_tag(TLVPACKAGE *lv,char *tagName,char *data);
/*��ԭtagֵ��׷��data�е�����*/
int  lv_set_tag_multi(TLVPACKAGE *lv,char *tagName,char *data);
int  lv_set_tag_before(TLVPACKAGE *lv,char *beforeTagName,
	                             char *tagName,char *data);
int  lv_gen_pack(TLVPACKAGE *lv);
int  lv_analyze_pack(TLVPACKAGE *lv);
int  lv_get_tag(TLVPACKAGE *lv,char *tagName,char *data);
int lv_get_tag_p(TLVPACKAGE *lv,char *tagName);
/*ȡ��������tag,����tagֵoffset�ֽڴ���ʼȡsize���ֽڣ�0��ʾ�ӿ�ʼȡ��9��ʾ�ӵ�10���ֽڿ�ʼȡ*/
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


/*���ӵĺ������ڲ�����*/
#endif

