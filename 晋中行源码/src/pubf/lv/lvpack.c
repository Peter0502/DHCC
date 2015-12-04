/*************************************************
  Copyright (C), 2005, DHC
  File name:     lvpack.c
  Author:    Τ��   Version:   V1.0     Date:  2005-8-25
  Description:    С��֧�����Ľṹ����صĹ��ܺ���ʵ�֡�
  Others: 
  Function List:
		1. int  lv_load_tagifno()
		 ��	��ʹ�����º���֮ǰ���������������������tag��Ϣ��
		 ��	�����ɹ�����0��ʧ�ܷ���-1

		2. int  lv_set_tag(TLVPACKAGE *,char *tagName,char *data);
			�ڱ������в���ĳ��TAG���ݣ��Ƿ�TAG������-1,���򷵻�0��
			3.  int  lv_set_tag_multi(TLVPACKAGE *lv,char *tagName,char *data);
			����ѭ����tagֵ����д��������,�ɹ�����0��ʧ�ܷ���-1

		4. int  lv_set_tag_before(TLVPACKAGE *,char *beforeTagName,
									 char *tagName,char *data);
			�ڵ�ǰ��������ĳ��TAG֮ǰ����һ��TAG���Ƿ�TAG�������λ�ò����ڷ�
			��-1,���򷵻�0��

		5. int  lv_gen_pack(TLVPACKAGE *);
			����������֯�������ݣ���������ͷ�ͱ�����

		6. int  lv_analyze_pack(TLVPACKAGE *);
			�����������ݣ����ݱ��������б���ͷ���������TLVPACKAGE�е���
			�Σ�����鱨�����Ƿ�Ϸ����ɹ�����0�����򷵻�-1��
		7. void lv_reset(TLVPACKAGE *lv)
			���ýṹ��

		8. int  lv_fetchpkg(TLVPACKAGE *ilv,TLVPACKAGE *olv)
			�����������ģ��ɹ�����0���쳣����-1��û�м�¼����1
			
		9. static int  lv_fetchpkg_PKG(TLVPACKAGE *ilv,TLVPACKAGE *olv)
			��PKG���Ĳ�֣��ɹ����� 0����������1���쳣����-1

		10. static int  lv_fetchpkg_CMT(TLVPACKAGE *ilv,TLVPACKAGE *olv)
			��CMT���Ĳ�֣��ɹ����� 0����������1���쳣����-1
		
		12. int  lv_create(TLVPACKAGE *lv)
			��ʼ�����Ľṹ��

		13. int  lv_destroy(TLVPACKAGE *lv)
			�������Ľṹ��

		14.int  lv_batch_end(TLVPACKAGE *lv)
			��������ҵ��һ��ҵ����ɺ����ҵ������ҵ��ͷ��������ҵ��
			�ɹ�����0��ʧ�ܷ���-1

		15.int lv_get_tag(TLVPACKAGE *lv,char *tagName,char *value)
			ȡ��tag��ȡ���ɹ�����tagֵ�ĳ��ȣ�ʧ�ܷ���-1
		
		16.int lv_get_tag_p(TLVPACKAGE *lv,char *tagName)
		��	�õ�tag��λ�ã��ɹ���������λ�ã�ʧ�ܷ���-1

		17.int lv_get_tag_multi(TLVPACKAGE *lv,char *tagName,char *value,int offset,int size)
			����ȡ��ѭ���ṹ��TAGֵ����offsetλ��ȡsizeλ��value��
		
		18.int  lv_pkg_batch(TLVPACKAGE *lv,char *tagName,char *value)
			��PKG��ǰҵ�����ҵ��ͷ��P����
		
		19.int  lv_pkg_getHeaderTag(TLVPACKAGE *lv,char *tagName,char *value)
			ȡ��PKG���а�ͷ��tagֵ���ɹ�����0��ʧ�ܷ���-1
		
		20.static int lv_dtagcpy(TDMEM *dm,char *tagName,char *tagData)��
			��dm�ṹ��Ӱ���ʽ���һ��tag������ֵ
			�ṹ���£� :tagName:tagData
		
		21.int lv_dgetNextTag(TDMEM *dm,int num,char *tagName,char *value)
			��num֮���ҵ�һ��tag,�ɹ����ر�TAG��ĳ��ȣ�û���ҵ�����0���쳣����-1
		
		22.static int lv_gen_pack_PKGMulti(TLVPACKAGE *lv)
			��������ҵ���PKG��
		
		23.static int lv_gen_pack_CMTMulti(TLVPACKAGE *lv)
			����CMT��ҵ���
		
		24.static int lv_gen_pack_CMTSingle(TLVPACKAGE *lv)
			����CMT��ҵ���

		25.static int lv_apk_pkgHeader(TLVPACKAGE *lv)
			������ͷ
		
		26.static int lv_apk_isCMTMulti(TLVPACKAGE *lv)��
			�����Ƿ�CMT����������
		
		27.static int lv_apk_CMTMulti(TLVPACKAGE *lv)��
			����CMT����������ҵ��ͷ
		
		28.static int lv_apk_isPKGMulti(TLVPACKAGE *lv)
			�����Ƿ�PKG����������
		
		29.static int lv_apk_PKGMulti(TLVPACKAGE *lv)
			����PKG��������ҵ��ͷ

		30.static int lv_apk_isPKGSingle(TLVPACKAGE *lv)
			�����Ƿ�PKG��ҵ�������׼��û�е�ҵ������Զ��壩
		
		31.static int lv_apk_PKGSingle(TLVPACKAGE *lv)
			����PKG��ҵ��ҵ��ͷ
		
		32.static int lv_apk_isCMTTrade(TLVPACKAGE *lv)
			�����Ƿ�CMT��ҵ����
		
		33.static int lv_apk_CMTTrade(TLVPACKAGE *lv)
			����CMT��ҵ����ҵ��ͷ
		
		34.static int lv_gen_pack_init(TLVPACKAGE *lv)
			���ɱ���ǰ��һЩ׼������
		
		35.static int lv_get_flgNo(TLVPACKAGE *lv)
			ȡ�ñ��ı�ʶ��
		
		36.static int lv_get_reNo(TLVPACKAGE *lv)
			ȡ�ñ��Ĳο���
		
		37.static int lv_gen_pack_zip0(TLVPACKAGE *lv)
			�ÿո���汨���е�'\0'�ַ���С��ı����в��ܳ���'\0'
		
		38.static int lv_isTrade(TLVPACKAGE *lv)
			�ж��Ƿ����ڽ��ף��ǽ��׶���ҵ��ͷ{2:},��Ҫ������Ѻ����ҲҪ����
		
		39.static int  lv_batch_end_PKG(TLVPACKAGE *lv)
			����ҵ��һ��ҵ�����PKG�Ĵ���
		
		40.static int  lv_batch_end_CMT(TLVPACKAGE *lv)
			����ҵ��һ��ҵ�����CMT�Ĵ���
		41.int lv_add_mac(TLVPACKAGE *lv)
			pkg��Ϊ1-11��Ҫ�ؼ���ʱ��Ҫ����C15Ѻ��
    
  History:   
    1. Date:     2005-8-25
       by:   Τ��
       Modification:������
    2. Date: 	2005-11-22
       by:   Τ��
����   Modefication:�����lv_checksum()������
*******************************************************/
#include <time.h>
#include "lvpack.h"
#include "errlog.h"
#include "malloc.h"

char *getlvno(TLVPACKAGE *lv);
TTAGINFO lv_taginfo;
/********************************************************
1. int  lv_load_tagifno()
	 ����ʹ�����º���֮ǰ���������������������tag��Ϣ��
	 �������ɹ�����0��ʧ�ܷ���-1
	 %s/etc/tinΪtag��Ϣ���ڵ��ļ���workdirҪ����
*********************************************************/
int  lv_load_tagifno(){
	char fileName[50];
	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/etc/tin",getenv("WORKDIR"));
	if( tag_load(&lv_taginfo,fileName) == 0 )
    {
        return 0;
    }
    return -1;
}
/***********************************************************
2. int  lv_set_tag(TLVPACKAGE *,char *tagName,char *data);
	   �ڱ������в���ĳ��TAG���ݣ��Ƿ�TAG������-1,���򷵻�0��
************************************************************/
int  lv_set_tag(TLVPACKAGE *lv,char *tagName,char *data){

	if ( lv->tradeEnd || lv->dm.length == 0 ){
		dm_dmemcat(&lv->dm,"{3:",3);
		lv->tradeEnd = 0;
		lv->tradeStartP = lv->dm.curNum - 3; 
	}

	lv_dtagcpy(&lv->dm,tagName,data);
	
	return 0;
}

/**************************************************************
3.  int  lv_set_tag_multi(TLVPACKAGE *lv,char *tagName,char *data);
	����ѭ����tagֵ����д��������,�ɹ�����0��ʧ�ܷ���-1
***************************************************************/
int  lv_set_tag_multi(TLVPACKAGE *lv,char *tagName,char *data){
	int ret,num;
	char iTagName[4];

	lv->tradeEnd = 0;
	num = lv_get_tag_p(lv,tagName);
	memset(iTagName,0,sizeof(iTagName));
	ret = lv_dgetNextTag(&(lv->dm),num,iTagName,NULL);
	if (num+ret >= lv->dm.length)
		return dm_dmemcat(&(lv->dm),data,strlen(data));
	else
		return dm_dinsertmem(&(lv->dm),num+ret,data,strlen(data));
	return -1;


	/*num = lv->tradeStartP;
	while(num<=lv->dm.length){
		memset(iTagName,0,sizeof(iTagName));
		ret = lv_dgetNextTag(&(lv->dm),num,iTagName,NULL);
		if (!memcmp(tagName,iTagName,3))
		{
			if (num+ret >= lv->dm.length)
				return dm_dmemcat(&(lv->dm),data,strlen(data));
			else
				return dm_dinsertmem(&(lv->dm),num+ret,data,strlen(data));
		}
		num += ret;
	}del by weizhao 2005-11-8����ǰ����һЩ����*/
}
/**************************************************************
	4. int  lv_set_tag_before(TLVPACKAGE *,char *beforeTagName,
	                             char *tagName,char *data);
	   �ڵ�ǰ��������ĳ��TAG֮ǰ����һ��TAG���Ƿ�TAG�������λ�ò����ڷ�
	   ��-1,���򷵻�0��
**************************************************************/
int  lv_set_tag_before(TLVPACKAGE *lv,char *beforeTagName,
                                 char *tagName,char *data){
	int ret;
	int dataLen;
	char *insertData;

	ret = lv_get_tag_p(lv,beforeTagName);
	if (ret<1)
		return -1;
	dataLen = LV_TAGNAME_LEN+2+strlen(data);
	insertData = (char *)malloc(dataLen+1);
	memset(insertData,0,dataLen+1);
	strcpy(insertData,":");
	memcpy(insertData+1,tagName,3);
	sprintf(insertData,"%s:%s",insertData,data);
	dm_dinsertmem(&(lv->dm),ret,insertData,dataLen); 
	free(insertData);
	lv->tradeEnd = 0;
	return 0;
}

/******************************************************
	5. int  lv_gen_pack(TLVPACKAGE *);
	   ����������֯�������ݣ���������ͷ�ͱ�����,��
******************************************************/
int  lv_gen_pack(TLVPACKAGE *lv){
	/*����ǰһ�������Զ����� */
	lv_gen_pack_init(lv);
	if (lv->isMulti){
	/*��ҵ����*/
		if (lv->pkgType){
		/*����PKG��*/
			lv_add_mac(lv);
			if (lv_gen_pack_PKGMulti(lv))
				return -1;
		}else{
		/*����CMT��*/	
			if (lv_gen_pack_CMTMulti(lv))
				return -1;
		}
	}else{
	/*������ҵ��*/
		if (lv_gen_pack_CMTSingle(lv))
			return -1;
	}
	lv_checksum(lv);
	/*�ѱ����е�0��' '����*/
	lv_gen_pack_zip0(lv);
	return 0;
}

/************************************************************
	6. int  lv_analyze_pack(TLVPACKAGE *);
	   �����������ݣ����ݱ��������б���ͷ���������TLVPACKAGE�е���
	   �Σ�����鱨�����Ƿ�Ϸ����ɹ�����0�����򷵻�-1��
************************************************************/
int  lv_analyze_pack(TLVPACKAGE *lv){
	int ret;

	/*����PKG��ҵ���ģ���fetch������PKG����*/
	if (lv_apk_isPKGSingle(lv)){
		if (lv_apk_PKGSingle(lv)){
			vtcp_log("[%s][%d]-------����PKG��ҵ���ʱ����-------\n",__FILE__,__LINE__);
			return -1;
		}
		return 0;
	}

	/*����ͷ,�����еı��� */
	ret = lv_apk_pkgHeader(lv);	/*apk==analyze_pack,pkgHeader==package header*/
	if (ret){
		vtcp_log("[%s][%d]-----������ͷ�ĳ���--------\n",__FILE__,__LINE__);
		return -1;
	}
	
	/*Ĭ�������ΪCMT��*/
	lv->pkgType = 0;
	lv->isFile = 0;
	lv->isMulti = 0;

	if ( lv_apk_isCMTMulti(lv) ){
		if (lv_apk_CMTMulti(lv)){
			vtcp_log("[%s][%d]-------����CMT����ҵ���ʱ����-------\n",__FILE__,__LINE__);
			return -1;
		}
	}
	if ( lv_apk_isPKGMulti(lv) ){
		
		if (lv_apk_PKGMulti(lv)){
			vtcp_log("[%s][%d]-------����PKG����ҵ���ʱ����-------\n",__FILE__,__LINE__);
			return -1;
		}
	}
	if ( lv_apk_isCMTTrade(lv) ){
		if (lv_apk_CMTTrade(lv)){
			vtcp_log("[%s][%d]------����CMT����ҵ��ͷ����----------\n",__FILE__,__LINE__);
			return -1;
		}
	}
			
	return 0;
}
/******************************************************
	7. ���ýṹ��
******************************************************/
void lv_reset(TLVPACKAGE *lv){
	lv_destroy(lv);
	lv_create(lv);
	return ;
}

/*******************************************************
	8. int  lv_fetchpkg(TLVPACKAGE *ilv,TLVPACKAGE *olv)
	   �����������ģ��ɹ�����0���쳣����-1��û�м�¼����1
	   Ҫ��ihv�Ѿ�������
********************************************************/
int  lv_fetchpkg(TLVPACKAGE *ilv,TLVPACKAGE *olv){
   if (ilv->pkgType)
      return lv_fetchpkg_PKG(ilv,olv);
   else
      return lv_fetchpkg_CMT(ilv,olv);   
}
/********************************************************
	9. ��PKG���Ĳ�֣��ɹ����� 0����������1���쳣����-1
*********************************************************/
static int  lv_fetchpkg_PKG(TLVPACKAGE *ilv,TLVPACKAGE *olv){
   char pkgHeader[LV_PKGHEADER_SIZE+1];
   char pkgTradeHeader[LV_PKGTRADEHEADER_SIZE+1];
   char  *data;
   char  sPKGLen[7];
   char  cmtCode[4];
   int  num,pkgBodyPos;
   
   if (ilv->tradeStartP>ilv->dm.length)
      return 1;
      
   memset(pkgHeader,0,sizeof(pkgHeader));
   memset(pkgTradeHeader,0,sizeof(pkgTradeHeader));
   
   /*�ӱ�����ȡ�ñ���ͷ����ǰҵ���ҵ��ͷ�ͱ�����*/
   /*ȡҵ��ͷ*/
	num = dm_memmem(&(ilv->dm),ilv->tradeStartP,"{2:",3);
	if (!num)	/*û��ҵ����*/
		return 1;
	dm_dmemcpy2(pkgTradeHeader,&(ilv->dm),num,LV_PKGTRADEHEADER_SIZE);
	/*������*/
	pkgBodyPos = num + LV_PKGTRADEHEADER_SIZE;
	num = dm_memmem(&(ilv->dm),pkgBodyPos,"}",1);
	if (!num || num<pkgBodyPos )
		return -1;

	data = (char *)malloc(num-pkgBodyPos+2);
	memset(data,0,num-pkgBodyPos+2);
	dm_dmemcpy2(data,&(ilv->dm),pkgBodyPos,num-pkgBodyPos+1);
	ilv->tradeStartP = num+1; 
	/*����ͷ*/
	dm_dmemcpy2(pkgHeader,&(ilv->dm),1,LV_PKGHEADER_SIZE);

	/*���鱨��,�޸ĳ��Ⱥ�CMTCODE*/
	memset(sPKGLen,0,sizeof(sPKGLen));
	olv->length = LV_PKGHEADER_SIZE+LV_PKGTRADEHEADER_SIZE+num-pkgBodyPos+1;
	printf("%s,%d cmt����[%d=%d+%d+%d-%d+1]",__FILE__,__LINE__,olv->length,LV_PKGHEADER_SIZE,LV_TRADEHEADER_SIZE,num,pkgBodyPos);
	sprintf(sPKGLen,"%06d",olv->length);
	printf("%s,%d cmt����=[%s]",__FILE__,__LINE__,sPKGLen);


	memcpy(pkgHeader+3,sPKGLen,6);

	dm_dmemcat(&(olv->dm),pkgHeader,LV_PKGHEADER_SIZE);
	dm_dmemcat(&(olv->dm),pkgTradeHeader,LV_PKGTRADEHEADER_SIZE);
	dm_dmemcat(&(olv->dm),data,strlen(data));

	free(data);

	/*���µı��Ľ��з���*/
	if (lv_analyze_pack(olv)==-1)
		return -1;

	/*һЩ������ı���*/
	olv->pkgType = 1;
	olv->isFile = 0;
	olv->isMulti = 0;

	olv->tradeStartP = 1;

	return 0;
}

/********************************************************
	9. ��CMT���Ĳ�֣��ɹ����� 0����������1���쳣����-1
*********************************************************/
static int  lv_fetchpkg_CMT(TLVPACKAGE *ilv,TLVPACKAGE *olv){
	/*ȡilv�ı���ͷ�����޸��䳤�Ⱥ�CMTCode�����ط�*/
	char  pkgHeader[LV_PKGHEADER_SIZE+1];
	char  tradeHeader[LV_TRADEHEADER_SIZE+1];
	char  *data;
	char  sPKGLen[7];
	char  cmtCode[4];
	int  num,pkgBodyPos;	

	if (ilv->tradeStartP>ilv->dm.length)
		return 1;

	memset(pkgHeader,0,sizeof(pkgHeader));
	memset(tradeHeader,0,sizeof(tradeHeader));

	/*�ӱ�����ȡ�ñ���ͷ����ǰҵ���ҵ��ͷ�ͱ�����*/
	num = dm_memmem(&(ilv->dm),ilv->tradeStartP,"{2:",3);
	if (!num)	/*û��ҵ����*/
		return 1;
	dm_dmemcpy2(tradeHeader,&(ilv->dm),num,LV_TRADEHEADER_SIZE);
	pkgBodyPos = num + LV_TRADEHEADER_SIZE;
	num = dm_memmem(&(ilv->dm),pkgBodyPos,"}",1);
	if (!num || num<pkgBodyPos )
		return -1;
	data = (char *)malloc(num-pkgBodyPos+2);
	memset(data,0,num-pkgBodyPos+2);
	dm_dmemcpy2(data,&(ilv->dm),pkgBodyPos,num-pkgBodyPos+1);
	ilv->tradeStartP = num+1; 
	dm_dmemcpy2(pkgHeader,&(ilv->dm),1,LV_PKGHEADER_SIZE);

	/*���鱨��,�޸ĳ��Ⱥ�CMTCODE*/
	memset(sPKGLen,0,sizeof(sPKGLen));
	olv->length = LV_PKGHEADER_SIZE+LV_TRADEHEADER_SIZE+num-pkgBodyPos+1;
	printf("%s,%d cmt����[%d=%d+%d+%d-%d+1]",__FILE__,__LINE__,olv->length,LV_PKGHEADER_SIZE,LV_TRADEHEADER_SIZE,num,pkgBodyPos);
	sprintf(sPKGLen,"%06d",olv->length);
	printf("%s,%d cmt����=[%s]",__FILE__,__LINE__,sPKGLen);
	memcpy(pkgHeader+3,sPKGLen,6);
	memset(cmtCode,0,sizeof(cmtCode));
	memcpy(cmtCode,tradeHeader+LV_TH_CMT_POS,LV_TAGNAME_LEN);
	memcpy(pkgHeader+LV_PH_CMT_POS,cmtCode,LV_TAGNAME_LEN);

	dm_dmemcat(&(olv->dm),pkgHeader,LV_PKGHEADER_SIZE);
	dm_dmemcat(&(olv->dm),tradeHeader,LV_TRADEHEADER_SIZE);
	dm_dmemcat(&(olv->dm),data,strlen(data));

	free(data);

	/*���µı��Ľ��з���*/
	if (lv_analyze_pack(olv)==-1)
		return -1;
	/*һЩ������ı���*/
	olv->pkgType = 0;
	olv->isFile = 0;
	olv->isMulti = 0;

	olv->tradeStartP = 1;

	return 0;
}
/**************************************************
10. ��ʼ�����Ľṹ��
***************************************************/
int  lv_create(TLVPACKAGE *lv){
	memset(lv,0,sizeof(TLVPACKAGE)-DMEM_SIZE);
	dm_create(&(lv->dm));
	return 0;
}
/**************************************************
11. �������Ľṹ��
***************************************************/
int  lv_destroy(TLVPACKAGE *lv){
	memset(lv,0,sizeof(TLVPACKAGE)-DMEM_SIZE);
	dm_destroy(&(lv->dm));
	return 0;
}
/***************************************************
12.��������ҵ��һ��ҵ����ɺ����ҵ������ҵ��ͷ��������ҵ�񣬳ɹ�����0��ʧ�ܷ���-1
****************************************************/
int  lv_batch_end(TLVPACKAGE *lv){
	lv->isMulti = 1;
	
	if ( lv->pkgType ){
		return lv_batch_end_PKG(lv);
	}else{
		return lv_batch_end_CMT(lv);
	}
}

/***************************************************
13.ȡ��tag��ȡ���ɹ�����tagֵ�ĳ��ȣ�ʧ�ܷ���-1
***************************************************/
int lv_get_tag(TLVPACKAGE *lv,char *tagName,char *value) {
	int num;
	int ret;
	char iTagName[LV_TAGNAME_LEN+1];

	num = lv->tradeStartP;
	num = dm_memmem(&(lv->dm),num,"{3:",3);
	if (!num)
		return -1;
	num += 3;
	while(1){
		memset(iTagName,0,sizeof(iTagName));
		ret = lv_dgetNextTag(&(lv->dm),num,iTagName,NULL);
		if ( ret< 1)
			return -1;
		if ( !memcmp(tagName,iTagName,LV_TAGNAME_LEN) ){
			lv_dgetNextTag(&(lv->dm),num,iTagName,value);
			return ret-5; /*���ص�value�ĳ���*/
		}
		num += ret;

	}
	return 0;
}

/***************************************************
14.���õ�tag��λ�ã��ɹ���������λ�ã�ʧ�ܷ���-1
****************************************************/
int lv_get_tag_p(TLVPACKAGE *lv,char *tagName) {
	int num;
	int ret;
	char iTagName[LV_TAGNAME_LEN+1];

	num = lv->tradeStartP;
	ret = dm_memmem(&(lv->dm),num,"{3:",3);
	if (ret)
		num = ret;	
	if (!num)
		return -1;
	num += 3;
	while(1){
		memset(iTagName,0,sizeof(iTagName));
		ret = lv_dgetNextTag(&(lv->dm),num-1,iTagName,NULL);
		if ( ret< 1)
			return -1;
		if ( !memcmp(tagName,iTagName,LV_TAGNAME_LEN) ){
			return num; /*����TAG��λ��*/
		}
		num += ret;

	}
	return 0;
}
/*******************************************************
15.������ȡ��ѭ���ṹ��TAGֵ����offsetλ��ȡsizeλ��value��
*******************************************************/
int lv_get_tag_multi(TLVPACKAGE *lv,char *tagName,char *value,int offset,int size) {
	int num;
	int ret;
	char *p;
	char iTagName[LV_TAGNAME_LEN+1];

	num = lv_get_tag_p(lv,tagName);
	if (num<1)
		return -1;

	dm_dmemcpy2(value,&(lv->dm),num+5+offset,size);
	
	return 0;
}

/*******************************************************
16.����PKG��ǰҵ�����ҵ��ͷ��P����
*******************************************************/
int  lv_pkg_batch(TLVPACKAGE *lv,char *tagName,char *value)
{ 
	int  num,ret,tagLength,len,flg; 
	char *buf;

	if(!isTag(tagName))
	{
		printf("tag�����Ϸ�.\n");
		return -1;
	}
	tagLength = tag_get(&lv_taginfo,tagName);

	if (tagLength <= 0)
	{
		tagLength = strlen(value);
	}

	buf = (char *)malloc(tagLength+10);
	memset(buf,'\0',tagLength+10);

	ret = dm_memmem(&(lv->dm),1,"{P:",3);

	if (!ret){
      memcpy(buf,"{P::",4);
      memcpy(buf+4,tagName,3);
      memcpy(buf+7,":",1);
	  memset(buf+8,' ',tagLength);
      memcpy(buf+8,value,strlen(value));
      memcpy(buf+tagLength+8,"}",1);
      len = tagLength+9;
	  flg = 0;
	}else{
		memcpy(buf,":",1);
		memcpy(buf+1,tagName,3);
		memcpy(buf+4,":",1);
		memset(buf+5,' ',tagLength);
		memcpy(buf+5,value,strlen(value));
		len = tagLength+5;
		flg = 1;
	}
	
	ret = dm_memmem(&(lv->dm),1,"{2:",3);
	if (!ret){
		free(buf);
		return -1;
	}
	num = ret-flg;

	dm_dinsertmem(&(lv->dm),num,buf,len);
	free(buf);
	return 0;
}

/******************************************************
17.ȡ��PKG���а�ͷ��tagֵ���ɹ�����0��ʧ�ܷ���-1
******************************************************/
int  lv_pkg_getHeaderTag(TLVPACKAGE *lv,char *tagName,char *value){
   int startP,endP;
   int ret,num;
   char iTagName[4];
	
   memset(iTagName,0,sizeof(iTagName));
   
   startP = dm_memmem(&(lv->dm),1,"{P:",3);
   if (!startP)
      return -1;
   endP = dm_memmem(&(lv->dm),startP,"}",1);
   if (endP>lv->dm.length)
      return -1;
   num = startP;	
   while(num<endP){
      memset(iTagName,0,sizeof(iTagName));
      ret = lv_dgetNextTag(&(lv->dm),num,iTagName,NULL);
      if (ret<1)
         return -1;
      if (!memcmp(tagName,iTagName,3)){
         ret = lv_dgetNextTag(&(lv->dm),num,iTagName,value);
         /*value[ret-5]='\0';*/	
         return 0;
      }   
      num+=ret;
   }
   return -1;
}
/********************************************
* static funtion ,only be used in lvpack.c
********************************************/
/********************************************
18����dm�ṹ��Ӱ���ʽ���һ��tag������ֵ
�ṹ���£� :tagName:tagData
*********************************************/
static int lv_dtagcpy(TDMEM *dm,char *tagName,char *tagData){
	int tagLength;

	if(!isTag(tagName))
	{
		printf("tag�����Ϸ�.\n");
		return -1;
	}
	tagLength = tag_get(&lv_taginfo,tagName);

	if (tagLength <= 0)
	{
		tagLength = strlen(tagData);
	}else{
		if (tagLength>strlen(tagData))
			tagLength = strlen(tagData);
	}

	dm_dmemcat(dm,":",1);
	dm_dmemcat(dm,tagName,3);
	dm_dmemcat(dm,":",1);
	dm_dmemcat(dm,tagData,tagLength);

	return 0;
}
/************************************************************
19 ��num֮���ҵ�һ��tag,�ɹ����ر�TAG��ĳ��ȣ�û���ҵ�����0���쳣����-1
*************************************************************/
int lv_dgetNextTag(TDMEM *dm,int num,char *tagName,char *value){
	int i;

	for( ;num<=dm->length;num++){
		if ( dm_getChar(dm,num)==':' && dm_getChar(dm,num+4)==':' ){
			dm_dmemcpy2(tagName,dm,num+1,LV_TAGNAME_LEN);
			tagName[LV_TAGNAME_LEN]='\0';
			for( i=0;i+num+5<=dm->length;i++){
				if( (num+5+i)>=dm->length || dm_getChar(dm,num+5+i)==':' 
						|| dm_getChar(dm,num+5+i)=='}' ){
					if (value){/*if value==NULL ����¼valueֵ*/
						dm_dmemcpy2(value,dm,num+5,i);
						value[i]='\0';
					}
					break;
				}		
			}
			return i+5; /*���ر�tag�򳤶�*/
		}else if (dm_getChar(dm,num)=='}')
			return 0;
		if ( num>dm->length )
			return -1;
	}
	return i+5; /*���ر�tag�򳤶�*/
}

/***************************************************************
20����������ҵ���PKG��
***************************************************************/
static int lv_gen_pack_PKGMulti(TLVPACKAGE *lv){
   int ret;
   char pkgHeader[LV_PKGHEADER_SIZE+1];
   
   memset(pkgHeader,0,sizeof(pkgHeader));
   if ( !lv->tradeEnd ){
      ret =  lv_batch_end(lv);
      if (!ret)
         return -1;
   }
   lv->length = LV_PKGHEADER_SIZE+lv->dm.length;
   sprintf(pkgHeader,"{1:%06d",lv->length);
   memcpy(pkgHeader+9,&(lv->use),LV_PKGHEADER_SIZE-10);
   pkgHeader[LV_PKGHEADER_SIZE-1] = '}';
   dm_dinsertmem(&(lv->dm),1,pkgHeader,LV_PKGHEADER_SIZE);
         
	return 0;
}

/****************************************************************
21������CMT��ҵ���
****************************************************************/
static int lv_gen_pack_CMTMulti(TLVPACKAGE *lv){
	char pkgHeader[LV_PKGHEADER_SIZE+1];
	char tradeHeader[LV_BHEADER_SIZE+1];
	int  ret;

	memset(pkgHeader,0,sizeof(pkgHeader));
	memset(tradeHeader,0,sizeof(tradeHeader));

   if ( !lv->tradeEnd ){
      ret =  lv_batch_end(lv);
      if (!ret)
         return -1;
   }
   
	lv->length = LV_PKGHEADER_SIZE+LV_BHEADER_SIZE+lv->dm.length;
	sprintf(pkgHeader,"{1:%06d",lv->length);
	memcpy(pkgHeader+9,&(lv->use),LV_PKGHEADER_SIZE-10);
	pkgHeader[LV_PKGHEADER_SIZE-1]='}';

	sprintf(tradeHeader,"{B:%010d%010d%010d%010d}",lv->ITXDebitTotalCount,
			lv->ITXCreditTotalCount,lv->OTXDebitTotalCount,lv->OTXCreditTotalCount);

	dm_dinsertmem(&(lv->dm),1,tradeHeader,LV_BHEADER_SIZE);
	dm_dinsertmem(&(lv->dm),1,pkgHeader,LV_PKGHEADER_SIZE);
	return 0;
}
/************************************************
22������CMT��ҵ���
************************************************/
static int lv_gen_pack_CMTSingle(TLVPACKAGE *lv){
	char pkgHeader[LV_PKGHEADER_SIZE+1];
	char tradeHeader[LV_TRADEHEADER_SIZE+1];
	int  ret;

	memset(pkgHeader,0,sizeof(pkgHeader));
	memset(tradeHeader,0,sizeof(tradeHeader));

	if ( dm_getChar(&(lv->dm),lv->dm.length)!='}' ){
		dm_dmemcat(&(lv->dm),"}",1);
	}
	lv->length = LV_PKGHEADER_SIZE+lv->dm.length;
	if ( lv_isTrade(lv) ){
		lv->length += LV_TRADEHEADER_SIZE;
		memcpy(lv->CMTNo,lv->CMTCode,3);
		strcpy(tradeHeader,"{2:");
		memcpy(tradeHeader+3,lv->CMTNo,3+1+40+16);
		tradeHeader[LV_TRADEHEADER_SIZE-1]='}';
		dm_dinsertmem(&(lv->dm),1,tradeHeader,LV_TRADEHEADER_SIZE);
	}
	sprintf(pkgHeader,"{1:%06d",lv->length);
	memcpy(pkgHeader+9,&(lv->use),LV_PKGHEADER_SIZE-10);
	pkgHeader[LV_PKGHEADER_SIZE-1]='}';
	dm_dinsertmem(&(lv->dm),1,pkgHeader,LV_PKGHEADER_SIZE);
	
	return 0;
}

/*************************************************
23��������ͷ
*************************************************/
static int lv_apk_pkgHeader(TLVPACKAGE *lv){
	/*�ѱ���ͷ��Ϣд��ṹ�壬����鳤���Ƿ���ȷ*/
	char pkgLen[7];
	char pkgHeader[LV_PKGHEADER_SIZE+1]; 
	int iAddLen=0;

	memset(pkgLen,0,sizeof(pkgLen));
	memset(pkgHeader,0,sizeof(pkgHeader));
		vtcp_log("[%s][%d]",__FILE__,__LINE__);
	dm_dmemcpy2(pkgHeader,&(lv->dm),1,LV_PKGHEADER_SIZE);
	memcpy(pkgLen,pkgHeader+LV_PH_LEN_POS,6);
	lv->length = atoi(pkgLen);
	if ( dm_getChar(&(lv->dm),lv->length) != '}' ){
		vtcp_log("[%s][%d]-----���Ľṹ����ȷ------[%c][%d][%d][%c]\n",__FILE__,__LINE__, dm_getChar(&(lv->dm),lv->length-1), lv->length,lv->dm.length,dm_getChar(&(lv->dm),lv->dm.length));
		return -1;
	}
	/* add by LiuHuafeng 
	iAddLen= lv->dm.length/1024;
	 end by LiuHuafeng */
	if ( lv->length != lv->dm.length+iAddLen ){
		vtcp_log("[%s][%d]----���ĳ��Ȳ���ȷ[%d][%d]------\n",__FILE__,__LINE__,lv->length,lv->dm.length);
		return -1;
	}

	memcpy(&(lv->use),pkgHeader+LV_PH_USE_POS,LV_PKGHEADER_SIZE-LV_PH_USE_POS);
	lv->pkgType = 0;
	lv->isFile = 0;
	lv->isMulti = 0;
	lv->tradeEnd = 1;
	lv->tradeStartP = LV_PKGHEADER_SIZE+1;

	return 0;
}

/****************************************************
24�������Ƿ�CMT����������
*****************************************************/
static int lv_apk_isCMTMulti(TLVPACKAGE *lv){
	int ret;
	ret = dm_memmem(&(lv->dm),1,"{B:",3);

	if (!ret)
		return 0;
	return 1;
}
/*************************************************
25������CMT����������ҵ��ͷ
****************************************************/
static int lv_apk_CMTMulti(TLVPACKAGE *lv){
	char str[11];
	int  num,ret;

	memset(str,0,sizeof(str));
	ret = dm_memmem(&(lv->dm),LV_PKGHEADER_SIZE+1,"{B:",3);
	if (!ret)
		return -1;
	num = ret+3;
	dm_dmemcpy2(str,&(lv->dm),num,10);
	lv->ITXDebitTotalCount = atoi(str);
	num += 10;
	memset(str,0,sizeof(str));
	dm_dmemcpy2(str,&(lv->dm),num,10);
	lv->ITXCreditTotalCount = atoi(str);
	num += 10;
	memset(str,0,sizeof(str));
	dm_dmemcpy2(str,&(lv->dm),num,10);
	lv->OTXDebitTotalCount = atoi(str);
	num += 10;
	memset(str,0,sizeof(str));
	dm_dmemcpy2(str,&(lv->dm),num,10);
	lv->OTXCreditTotalCount = atoi(str);

	lv->pkgType = 0;
	lv->isMulti = 1;

	return 0;
}

/********************************************
26�������Ƿ�PKG����������
********************************************/
static int lv_apk_isPKGMulti(TLVPACKAGE *lv){
	int ret;
	ret = dm_memmem(&(lv->dm),LV_PKGHEADER_SIZE+1,"{P:",3);
	if (!ret)
		return 0;
	return 1;
}

static int lv_apk_PKGMulti(TLVPACKAGE *lv){
	char tmp[9];
	int ret;

	lv->pkgType = 1;
	lv->isMulti = 1;

	/*�Ѱ�ͷ�е�packID(PKG����ID)��packDate(��������)��respDate(Ӧ������)��ȡ����
	��Ӧ��tagName�ֱ��ǣ�0BD��30E��BS6*/
	memset(tmp,0,sizeof(tmp));
	ret = lv_pkg_getHeaderTag(lv,"0BD",tmp);
	if (!ret)
		lv->packID = atol(tmp);
	memset(tmp,0,sizeof(tmp));
	ret = lv_pkg_getHeaderTag(lv,"30E",tmp);
	if (!ret)
		memcpy(lv->packDate,tmp,8);
	memset(tmp,0,sizeof(tmp));
	ret = lv_pkg_getHeaderTag(lv,"BS6",tmp);
	if (!ret)
		memcpy(lv->respDate,tmp,8);

	return 0;
}

static int lv_apk_isPKGSingle(TLVPACKAGE *lv){
	if (lv->pkgType && !lv->isMulti)
		return 1;
	return 0;
}

static int lv_apk_PKGSingle(TLVPACKAGE *lv){
	int ret;
	char tmp[13];
		vtcp_log("[%s][%d]",__FILE__,__LINE__);
	ret = lv_apk_pkgHeader(lv);
	if (ret)
		return -1;
	lv->pkgType = 1;
	lv->isMulti = 0;
		vtcp_log("[%s][%d]",__FILE__,__LINE__);
	/*ҵ��ͷ*/
	ret = dm_memmem(&(lv->dm),1,"{2:",3);
	if (!ret)/*mod by chenggx 20110531 ��"ret"�޸�Ϊ"!ret"*/
		return -1;
	memset(tmp,0,sizeof(10));
	dm_dmemcpy2(tmp,&(lv->dm),ret+3,12);
	memcpy(lv->CMTNo,tmp,3);
	memcpy(lv->reserve,tmp+3,9);
		vtcp_log("[%s][%d]",__FILE__,__LINE__);
	return 0;
}
/*������ҵ���е�ҵ��ͷ*/
static int lv_apk_isCMTTrade(TLVPACKAGE *lv){
	int ret;

	if (lv->isMulti)	/*�������������ҵ��ͷ*/
		return 0;
	ret = dm_memmem(&(lv->dm),LV_PKGHEADER_SIZE+1,"{2:",3);
	if (!ret)
		return 0;
	 
	return 1;
}

static int lv_apk_CMTTrade(TLVPACKAGE *lv){
	int ret;
	char tradeHeader[LV_TRADEHEADER_SIZE+1];

	memset(tradeHeader,0,sizeof(tradeHeader));
	ret = dm_memmem(&(lv->dm),LV_PKGHEADER_SIZE+1,"{2:",3);
	if (!ret)
		return -1;
	dm_dmemcpy2(tradeHeader,&(lv->dm),ret,LV_PKGHEADER_SIZE);
	memcpy(lv->CMTNo,tradeHeader+3,3+1+40+16);	/*3=CMTNo,1=paymPRI,40=localSysSign,16=reserve*/

	return 0;
}

static int lv_gen_pack_init(TLVPACKAGE *lv){
	char pcTimeStr[22];

   /*��Щ����д��һЩĬ��ֵ*/
   if (lv->opeType=='\0')
      lv->opeType = '1';   /*Ĭ�ϵ������ΪС��*/
   if (*(lv->opeStateCode)=='\0')
      memcpy(lv->opeStateCode,"000",3);   /*����*/
   if (*(lv->sendTime)=='\0')
   {
	    memset(pcTimeStr,'\0',sizeof(pcTimeStr));
		appGetTime(NULL,0,pcTimeStr);
		memcpy(lv->sendTime,pcTimeStr,14);
   }
   if (*(lv->workDate)=='\0')
	   memcpy(lv->workDate,pcTimeStr,8);
   /*��workDate�ĸ�ֵ��getflgNo֮ǰ�Ŷ�*/
   if (*(lv->flgNo)=='\0')
      lv_get_flgNo(lv);
   if (*(lv->reNo)=='\0')
      lv_get_reNo(lv);
   if ( lv->paymPRI=='\0')
	   lv->paymPRI = '0';
 
	return 0;
}
static int lv_get_flgNo(TLVPACKAGE *lv){
	memcpy(lv->flgNo,getlvno(lv),8);
   return 0;
}
static int lv_get_reNo(TLVPACKAGE *lv){
	memcpy(lv->reNo,"888888888888",12);
	memcpy(lv->reNo+12,lv->flgNo,8);
   return 0;
}


static int lv_gen_pack_zip0(TLVPACKAGE *lv){
	dm_zip0(&(lv->dm));
	return 0;
}

/*�ж��Ƿ����ڽ��ף��ǽ��׶���ҵ��ͷ{2:}*/
static int lv_isTrade(TLVPACKAGE *lv){
	if ( !memcmp(lv->CMTCode,"1",1) )
		return 1;
	return 0;
}

/*����ҵ��һ��ҵ�����PKG�Ĵ���*/
static int  lv_batch_end_PKG(TLVPACKAGE *lv){
   char buf[LV_PKGTRADEHEADER_SIZE+1];
   int ret,num;

   memset(buf,0,sizeof(buf));
	if ( lv->tradeEnd ){		
		lv->isMulti = 1;
		return 0;
	}else{
		
	   memcpy(buf,"{2:",3);
	   memcpy(buf+3,lv->CMTNo,3);
	   memcpy(buf+6,lv->reserve,9);
	   memcpy(buf+15,"}",1);
	   ret = dm_memmem(&(lv->dm),lv->tradeStartP,"{3:",3);
	   if (!ret)
	      return -1;
	   num = ret;
	   dm_dinsertmem(&(lv->dm),num,buf,LV_PKGTRADEHEADER_SIZE);   
		dm_dmemcat(&(lv->dm),"}",1);
		memset(lv->CMTNo,0,3+9);
		lv->isMulti = 1;
		lv->tradeEnd = 1;
	}

	return 0;
}
/*����ҵ��һ��ҵ�����CMT�Ĵ���*/
static int  lv_batch_end_CMT(TLVPACKAGE *lv){
	int num,ret;
	char *data;

	if ( lv->tradeEnd )
		return 0;
	/*���Ӻ��������*/
	num = lv->dm.length;
	dm_dmemcat(&(lv->dm),"}",1);	
	/*����ҵ��ͷ*/
	data = (char *)malloc(LV_TRADEHEADER_SIZE+1);
	memset(data,' ',LV_TRADEHEADER_SIZE); 	/*��ո񣬲�����0*/
	data[LV_TRADEHEADER_SIZE]='\0';
	memcpy(data,"{2:",3);
	memcpy(data+3,lv->CMTNo,3+1+40+16);	/*CMTNo3λ��paymPRI1λ��localSynSign40λ��reserve16λ*/
	data[LV_TRADEHEADER_SIZE-1]='}';
	/*����tradeStartP�����⣬����᲻����������أ�*/
	ret = dm_memmem(&(lv->dm),lv->tradeStartP,"{3:",3);
	if (!ret)
	    return -1;
	num = ret;
	dm_dinsertmem(&(lv->dm),num,data,LV_TRADEHEADER_SIZE);
	/*�����ǰ����ҵ�����Ϣ*/
	memset(lv->CMTNo,0,3+1+40+16);
	lv->tradeEnd = 1; 		/*��ҵ�����*/
	lv->isMulti = 1;
	return 0;
}

/*
**�жϱ����ǲ����ļ����ġ�
**�Ƿ���1�����Ƿ���0
*/
int  lv_isFile(TLVPACKAGE *lv){
	int ret;
	ret = dm_memmem(&(lv->dm),1,"{F:",3);
	if (ret>0)
		return 1;
	else
		return 0;
}


int lv_checksum(TLVPACKAGE *lv){
    char checkSumStr[37];
    char sLength[7];

	/*�ж��Ƿ��Ѿ���{C:}*/
	if ( dm_memmem(&(lv->dm),1,"{C:",3) )
		return -1;
		
    /*����checkSum�ĳ���*/
    lv->length += 36 ;
    memset(sLength,0,sizeof(sLength));
    sprintf(sLength,"%06d",lv->length);
    memcpy(lv->dm.block.mem+3,sLength,6);
    /*���checkSum������*/
    memset(checkSumStr,0,sizeof(checkSumStr));
    strcat(checkSumStr,"{C:");
    memset(checkSumStr+3,' ',32);
    strcat(checkSumStr,"}");
    dm_dmemcat(&(lv->dm),checkSumStr,36);
    return 0;

}

int lv_add_mac(TLVPACKAGE *lv){
	char CMTCode[4],sPkgMac[41];
	int  iCmtCode;

	memset(CMTCode ,0,sizeof(CMTCode));
	memcpy(CMTCode,lv->CMTCode,3);
	iCmtCode = atoi(CMTCode);
	if ( iCmtCode < 1 || iCmtCode >11 ) /*PKGҪ�ؼ���1-11ʱҪ������Ѻ*/
		return 0;
	memset(sPkgMac,0,sizeof(sPkgMac));
	memset(sPkgMac,'0',40);
	lv_pkg_batch(lv,"C15",sPkgMac);
	return 0;
}
/****Add by SSH,2005.11.24,��ʱ�ļ����ʾ��****/
char *getlvno(TLVPACKAGE *lv){
	static char retno[9];
	FILE *fp;
	char filename[128];
	char readbuf[128];
	memset(readbuf,'\0',sizeof(readbuf));
	memset(retno,'\0',sizeof(retno));
	sprintf(filename,"%s/etc/lvno.dat",getenv("WORKDIR"));
	fp=fopen(filename,"rb");
	if(fp==NULL){
		printf("%s,%d,��һ��ʹ������ļ�\n",__FILE__,__LINE__);
		fp=fopen(filename,"wb");
		if(fp==NULL){
			printf("%s,%d,Can not open %s\n",__FILE__,__LINE__,filename);
			sprintf(retno,"%08d",1);
			return(retno);
		}
		printf("%s,%d,|%08s|%8s|%08d|\n",__FILE__,__LINE__,lv->workDate,lv->workDate,1);
		fprintf(fp,"%8s%08d",lv->workDate,1);
		sprintf(retno,"%08d",1);
		fclose(fp);
		return(retno);
	}
	fgets(readbuf,sizeof(readbuf)-1,fp);
	fclose(fp);
	fp=fopen(filename,"wb");
	if(fp==NULL){
		printf("%s,%d,Can not open %s\n",__FILE__,__LINE__,filename);
		sprintf(retno,"%08d",1);
		return(retno);
	}
	if(memcmp(readbuf,lv->workDate,8)!=0){
		fprintf(fp,"%8s%08d",lv->workDate,1);
		sprintf(retno,"%08d",1);
	}else{
		sprintf(retno,"%08d",atoi(readbuf+8)+1);
		fprintf(fp,"%8s%.8s",lv->workDate,retno);
	}
	fclose(fp);
	return(retno);
}
