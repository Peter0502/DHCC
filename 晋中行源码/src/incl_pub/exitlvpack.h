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
  History:   
    1. Date:         2005-7-21
       Author:       Τ��
       Modification: ����
*************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>

#include "hv_tag.h"
#include "lvpack.h"

#ifndef _T_LVPACKAGE
#define _T_LVPACKAGE

/*�궨����*/
#define LV_OP_SUCCEE  0     /*�����ɹ�����ֵ*/
#define LV_OP_FAIL     -1    /*����ʧ�ܷ���ֵ*/

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

	char CMTNo[3];			/*��ǰ����ҵ��CMT��*/
	char paymPRI;			/*֧������*/
	char localSysSign[40];	/*�ط�Ѻ/ȫ��Ѻ*/
	char reserve[16];		/*Ԥ��*/
    
	char elemntSetNo[3];	/*ҵ��Ҫ�ؼ���*/

	int  pkgType;			/*�������ͣ�PKG(1)����CMT(00)*/
	int  isFile;			/*�Ƿ��ļ����ͣ����Խӿڿ��ţ��ڲ��������ļ�����Ϊ1*/
	/* ���������ݣ�28k������ͷ��ҵ��ͷ+�����壩 */
	char body[LV_BODY_LENGTH];
}TLVPACKAGE;


/*����*/
int  lv_load_tagifno();
int  lv_set_tag(TLVPACKAGE *lv,char *tagName,char *data);
int  lv_set_tag_before(TLVPACKAGE *lv,char *beforeTagName,
	                             char *tagName,char *data);
int  lv_gen_pack(TLVPACKAGE *lv);
int  lv_analyze_pack(TLVPACKAGE *lv);
int  lv_get_tag(TLVPACKAGE *lv,char *tagName,char *data);
void lv_reset(TLVPACKAGE *lv);
int  lv_fetchpkg(TLVPACKAGE *ilv,TLVPACKAGE *olv);
int  lv_create(TLVPACKAGE *lv);
int  lv_destroy(TLVPACKAGE *lv);
int  lv_batch_end(TLVPACKAGE *lv); 

/*���ӵĺ������ڲ�����*/
#endif

