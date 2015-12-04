/*************************************************
  Copyright (C), 2004, DHC
  File name:     hvpack.h
  Author:    Τ��   Version:   V1.0     Date:  2004/8/19
  Description:    ���֧�����Ľṹ����صĹ��ܺ�����
  Others: 
  Function List:  (�������壬ʵ����hvpack.c�ļ���)
	1. int  hv_load_tagifno(TTAGINFO *ti,char *fileName)
	 ����ʹ�����º���֮ǰ���������������������tag��Ϣ��
	 �������ɹ�����0��ʧ�ܷ���-1
    2. int hv_set_tag(THVPACKAGE *,char *tagName,char *data);
	   �ڱ������в���ĳ��TAG���ݣ��Ƿ�TAG������-1,���򷵻�0��
	3. void hv_set_tag_before(THVPACKAGE *,char *beforeTagName,
	                             char *tagName,char *data);
	   �ڱ�������ĳ��TAG֮ǰ����һ��TAG���Ƿ�TAG�������λ�ò����ڷ�
	   ��-1,���򷵻�0��
	4. void hv_gen_pack(THVPACKAGE *);
	   ��֯�������ݣ���������ͷ�ͱ����塣
	5. int hv_analyze_pack(THVPACKAGE *);
	   �����������ݣ����ݱ��������б���ͷ���������THVPACKAGE�е���
	   �Σ�����鱨�����Ƿ�Ϸ����ɹ�����0�����򷵻�-1��
	6. int hv_get_tag(THVPACKAGE *,char *tagName,char *data);
       �õ�ĳ��TAG������,��ָ��TAG����������data[0]='\0'�ҷ���-1,��
	   ������data־������0��
	7. void hv_reset(THVPACKAGE *)
	   �ṹ�����㣬��������ʹ��һ���ṹ����ǰʹ�ã�ȷ����ȫ
	8. int  hv_fetchpkg(THVPACKAGE *ihv,THVPACKAGE *ohv)
	   �����������ģ��ɹ�����0���쳣����-1��û�м�¼����1
  History:   
    1. Date:         2004-8-19
       Author:       Τ��
       Modification: ����
	2. Date:     2004-8-26
	   Author:�� Τ��
	   Modification:  ����
	3. Date:	 2005-3-28
	   Author:	 Τ��
	   Modifiaction:  ���£���Ӵ����������ĵĹ��ܡ�
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
/*�궨����*/
#define HV_1ST_TAG_POS 96    /*��һ��tag�Ŀ�ʼλ��,����"{3::001:"�ڶ�����:������ʼλ�õ�ƫ����*/
#define HV_TEXT_STAR_POS 93  /*���Ŀ���ʼλ�õ�ƫ����*/
#define PHEADER 0            /*���ĵ���ʼλ��*/
#define LENGTH 3             /*����ƫ��*/
#define USE  9               /*������;��ƫ����*/
#define MARK 10              /*�������ƫ����*/
#define OPETYPE 11           /*ҵ������ƫ����*/
#define OPESTATECODE 12      /*ҵ��״̬����ƫ����*/
#define FLGNO 15             /*���ı�ʶ��ƫ����*/
#define RENO  23             /*���Ĳο���ƫ����*/
#define SENDTIME  43         /*����ʱ��ƫ����*/
#define CMTCODE  57          /*CMT����ƫ����*/
#define WORKDATE  60         /*��������ƫ����*/
#define PHEADER_END 68       /*����λ��ƫ����*/
#define OPEHEADER 69         /*ҵ��ͷ��ʼλ��ƫ����*/
#define OPE_CMTCODE 72       /*ҵ��ͷCMT����ƫ����*/
#define PRI_POS 75           /*���ȼ���ʾƫ����*/
#define FORFUTURE 76         /*Ԥ�����ֿ�ʼλ��ƫ����*/
#define OPEHEADER_END 92     /*ҵ��ͷ����λ��ƫ����*/
#define HV_TAGNAME_LENGTH 3  /*tagName�ĳ��ȣ��������*/ 
/** rem by LiuHuafeng 20070110 
#define HV_BODY_LENGTH 6144000 *���ĵ���󳤶�6000K*
*****************************************************/
#define HV_BODY_LENGTH 1048576 /*���ĵ���󳤶�1M*/
#define RECVDNUM_POS	72	 /*���˽�Ǳ���λ��ƫ����*/
#define RECVCNUM_POS	82	 /*���˴��Ǳ���λ��ƫ����*/
#define SENDDNUM_POS	92	 /*���˽�Ǳ���λ��ƫ����*/
#define SENDCNUM_POS	102	 /*���˴��Ǳ���λ��ƫ����*/
/*#define HV_TAGINFO_FILE "tin"  tag��Ϣ�ļ�*/

#define HV_OP_SUCCEE  0     /*�����ɹ�����ֵ*/
#define HV_OP_FAIL     -1    /*����ʧ�ܷ���ֵ*/

#define HV_BANKCODE_FLGNO HV_QSBRNO  /*�����к�*/

extern TTAGINFO hv_taginfo;         /*����ṹ�����ڼ�¼tag����Ӧ���ȵ�Ӱ����Ϣ*/
/*extern  int tag_length;      *//*��¼��ǰ�����tag�򳤶�*/
/* ���֧�����Ľṹ  */
typedef struct _t_hvpackage
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
	char PRI;                /*���ȼ����*/
	int  recvDnum;			 /*���˽�Ǳ��� add by weizhao 2005-3-28 ��������*/
	int  recvCnum;			 /*���˴��Ǳ���*/
	int  sendDnum;			 /*���˽�Ǳ���*/
	int  sendCnum;			 /*���˴��Ǳ���*/
	int  pos;				 /*������������ʱ��ƫ��λ��*/
    
	/* ���������ݣ�60k������ͷ��ҵ��ͷ+�����壩*/ 
	char body[HV_BODY_LENGTH];       
}THVPACKAGE;


/* �������� ,���忴ǰ��˵���������÷���hvpack.c�ļ� */
/*���⺯��*/
int  hv_load_tagifno(char *fileName);
int  hv_set_tag(THVPACKAGE *hv,char *tagName,char *data);
int  hv_set_tag_before(THVPACKAGE *hv,char *beforeTagName,
	                             char *tagName,char *data);
int  hv_gen_pack(THVPACKAGE *hv);
/* int  hv_analyze_pack(THVPACKAGE *hv); */
int  hv_get_tag(THVPACKAGE *hv,char *tagName,char *data);
void hv_reset(THVPACKAGE *hv);
int  hv_fetchpkg(THVPACKAGE *ihv,THVPACKAGE *ohv);

/*���ӵĺ������ڲ�����*/
int  isTag(char *tagName);                     /*�Ƿ���һ���Ϸ���tagName,���ڹ涨��ֻ������ĸ�������֣���λ*/
/*char *searchTag(char *str,char *tagName); */     /*��str�в����Ƿ����tagName������tagNameǰ���":"��λ�õ�ָ��*/
void getPackageFlgNo(THVPACKAGE *hv);          /*���flgNo,û��ʵ�ִ����ݿ������ɣ�ֻ�Ǽ򵥵�д��һ���ַ���*/
void getPackageReNo(THVPACKAGE *hv);           /*���flgNo,û��ʵ�ִ����ݿ������ɣ�ֻ�Ǽ򵥵�д��һ���ַ���*/
int  tagcpy(char *pos,char *tagName,char *data);/*��POS������":tagName:data",����������ִ��ĳ���*/
int  tagcpy2(char *pos,char *tagName,char *data,int datalen);
#endif

