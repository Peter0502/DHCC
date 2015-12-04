/*************************************************
  Copyright (C), 2005, DHC
  File name:     lvpack.c
  Author:    韦昭   Version:   V1.0     Date:  2005-8-25
  Description:    小额支付报文结构及相关的功能函数实现。
  Others: 
  Function List:
		1. int  lv_load_tagifno()
		 　	在使用以下函数之前，必须用这个函数来载入tag信息。
		 　	操作成功返回0，失败返回-1

		2. int  lv_set_tag(TLVPACKAGE *,char *tagName,char *data);
			在报文体中插入某个TAG内容，非法TAG名返回-1,否则返回0。
			3.  int  lv_set_tag_multi(TLVPACKAGE *lv,char *tagName,char *data);
			在有循环的tag值中填写后续数据,成功返回0，失败返回-1

		4. int  lv_set_tag_before(TLVPACKAGE *,char *beforeTagName,
									 char *tagName,char *data);
			在当前报文体中某个TAG之前插入一个TAG，非法TAG名或插入位置不存在返
			回-1,否则返回0。

		5. int  lv_gen_pack(TLVPACKAGE *);
			用于最终组织报文数据，包括报文头和报文体

		6. int  lv_analyze_pack(TLVPACKAGE *);
			分析报文数据，根据报文数据中报文头的内容填充TLVPACKAGE中的字
			段，并检查报文体是否合法，成功返回0，否则返回-1。
		7. void lv_reset(TLVPACKAGE *lv)
			重置结构体

		8. int  lv_fetchpkg(TLVPACKAGE *ilv,TLVPACKAGE *olv)
			解析批量报文，成功返回0，异常返回-1，没有记录返回1
			
		9. static int  lv_fetchpkg_PKG(TLVPACKAGE *ilv,TLVPACKAGE *olv)
			对PKG报文拆分，成功返回 0，结束返回1，异常返回-1

		10. static int  lv_fetchpkg_CMT(TLVPACKAGE *ilv,TLVPACKAGE *olv)
			对CMT报文拆分，成功返回 0，结束返回1，异常返回-1
		
		12. int  lv_create(TLVPACKAGE *lv)
			初始化报文结构体

		13. int  lv_destroy(TLVPACKAGE *lv)
			析构报文结构体

		14.int  lv_batch_end(TLVPACKAGE *lv)
			增加批量业务，一笔业务完成后给该业务增加业务头并结束该业务，
			成功返回0，失败返回-1

		15.int lv_get_tag(TLVPACKAGE *lv,char *tagName,char *value)
			取出tag的取，成功返回tag值的长度，失败返回-1
		
		16.int lv_get_tag_p(TLVPACKAGE *lv,char *tagName)
		　	得到tag的位置，成功返回所在位置，失败返回-1

		17.int lv_get_tag_multi(TLVPACKAGE *lv,char *tagName,char *value,int offset,int size)
			用于取有循环结构的TAG值，从offset位置取size位到value中
		
		18.int  lv_pkg_batch(TLVPACKAGE *lv,char *tagName,char *value)
			给PKG当前业务添加业务头｛P：｝
		
		19.int  lv_pkg_getHeaderTag(TLVPACKAGE *lv,char *tagName,char *value)
			取得PKG包中包头的tag值，成功返回0，失败返回-1
		
		20.static int lv_dtagcpy(TDMEM *dm,char *tagName,char *tagData)　
			给dm结构添加按格式添加一个tag和他的值
			结构如下： :tagName:tagData
		
		21.int lv_dgetNextTag(TDMEM *dm,int num,char *tagName,char *value)
			在num之后找第一个tag,成功返回本TAG域的长度，没有找到返回0，异常返回-1
		
		22.static int lv_gen_pack_PKGMulti(TLVPACKAGE *lv)
			生成批量业务的PKG包
		
		23.static int lv_gen_pack_CMTMulti(TLVPACKAGE *lv)
			生成CMT多业务包
		
		24.static int lv_gen_pack_CMTSingle(TLVPACKAGE *lv)
			生成CMT单业务包

		25.static int lv_apk_pkgHeader(TLVPACKAGE *lv)
			分析报头
		
		26.static int lv_apk_isCMTMulti(TLVPACKAGE *lv)　
			分析是否CMT包批量报文
		
		27.static int lv_apk_CMTMulti(TLVPACKAGE *lv)　
			分析CMT包批量报文业务头
		
		28.static int lv_apk_isPKGMulti(TLVPACKAGE *lv)
			分析是否PKG包批量报文
		
		29.static int lv_apk_PKGMulti(TLVPACKAGE *lv)
			分析PKG批量报文业务头

		30.static int lv_apk_isPKGSingle(TLVPACKAGE *lv)
			分析是否PKG单业务包（标准中没有单业务包，自定义）
		
		31.static int lv_apk_PKGSingle(TLVPACKAGE *lv)
			分析PKG单业务业务头
		
		32.static int lv_apk_isCMTTrade(TLVPACKAGE *lv)
			分析是否CMT的业务报文
		
		33.static int lv_apk_CMTTrade(TLVPACKAGE *lv)
			分析CMT的业务报文业务头
		
		34.static int lv_gen_pack_init(TLVPACKAGE *lv)
			生成报文前的一些准备工作
		
		35.static int lv_get_flgNo(TLVPACKAGE *lv)
			取得报文标识号
		
		36.static int lv_get_reNo(TLVPACKAGE *lv)
			取得报文参考号
		
		37.static int lv_gen_pack_zip0(TLVPACKAGE *lv)
			用空格代替报文中的'\0'字符，小额的报文中不能出现'\0'
		
		38.static int lv_isTrade(TLVPACKAGE *lv)
			判断是否属于交易，是交易而有业务头{2:},还要加上质押融资也要加上
		
		39.static int  lv_batch_end_PKG(TLVPACKAGE *lv)
			批量业务一笔业务结束PKG的处理
		
		40.static int  lv_batch_end_CMT(TLVPACKAGE *lv)
			批量业务一笔业务结束CMT的处理
		41.int lv_add_mac(TLVPACKAGE *lv)
			pkg包为1-11的要素集的时候要加入C15押密
    
  History:   
    1. Date:     2005-8-25
       by:   韦昭
       Modification:　建立
    2. Date: 	2005-11-22
       by:   韦昭
　　   Modefication:　添加lv_checksum()函数　
*******************************************************/
#include <time.h>
#include "lvpack.h"
#include "errlog.h"
#include "malloc.h"

char *getlvno(TLVPACKAGE *lv);
TTAGINFO lv_taginfo;
/********************************************************
1. int  lv_load_tagifno()
	 　在使用以下函数之前，必须用这个函数来载入tag信息。
	 　操作成功返回0，失败返回-1
	 %s/etc/tin为tag信息所在的文件，workdir要定义
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
	   在报文体中插入某个TAG内容，非法TAG名返回-1,否则返回0。
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
	在有循环的tag值中填写后续数据,成功返回0，失败返回-1
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
	}del by weizhao 2005-11-8　以前的有一些问题*/
}
/**************************************************************
	4. int  lv_set_tag_before(TLVPACKAGE *,char *beforeTagName,
	                             char *tagName,char *data);
	   在当前报文体中某个TAG之前插入一个TAG，非法TAG名或插入位置不存在返
	   回-1,否则返回0。
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
	   用于最终组织报文数据，包括报文头和报文体,。
******************************************************/
int  lv_gen_pack(TLVPACKAGE *lv){
	/*处理前一参数的自动处理 */
	lv_gen_pack_init(lv);
	if (lv->isMulti){
	/*多业务处理*/
		if (lv->pkgType){
		/*处理PKG包*/
			lv_add_mac(lv);
			if (lv_gen_pack_PKGMulti(lv))
				return -1;
		}else{
		/*处理CMT包*/	
			if (lv_gen_pack_CMTMulti(lv))
				return -1;
		}
	}else{
	/*处理单笔业务*/
		if (lv_gen_pack_CMTSingle(lv))
			return -1;
	}
	lv_checksum(lv);
	/*把报文中的0用' '代替*/
	lv_gen_pack_zip0(lv);
	return 0;
}

/************************************************************
	6. int  lv_analyze_pack(TLVPACKAGE *);
	   分析报文数据，根据报文数据中报文头的内容填充TLVPACKAGE中的字
	   段，并检查报文体是否合法，成功返回0，否则返回-1。
************************************************************/
int  lv_analyze_pack(TLVPACKAGE *lv){
	int ret;

	/*特殊PKG单业务报文，经fetch出来的PKG报文*/
	if (lv_apk_isPKGSingle(lv)){
		if (lv_apk_PKGSingle(lv)){
			vtcp_log("[%s][%d]-------分析PKG单业务包时出错-------\n",__FILE__,__LINE__);
			return -1;
		}
		return 0;
	}

	/*处理报头,对所有的报文 */
	ret = lv_apk_pkgHeader(lv);	/*apk==analyze_pack,pkgHeader==package header*/
	if (ret){
		vtcp_log("[%s][%d]-----处理报文头的出错--------\n",__FILE__,__LINE__);
		return -1;
	}
	
	/*默认下设包为CMT包*/
	lv->pkgType = 0;
	lv->isFile = 0;
	lv->isMulti = 0;

	if ( lv_apk_isCMTMulti(lv) ){
		if (lv_apk_CMTMulti(lv)){
			vtcp_log("[%s][%d]-------分析CMT批量业务包时出错-------\n",__FILE__,__LINE__);
			return -1;
		}
	}
	if ( lv_apk_isPKGMulti(lv) ){
		
		if (lv_apk_PKGMulti(lv)){
			vtcp_log("[%s][%d]-------分析PKG批量业务包时出错-------\n",__FILE__,__LINE__);
			return -1;
		}
	}
	if ( lv_apk_isCMTTrade(lv) ){
		if (lv_apk_CMTTrade(lv)){
			vtcp_log("[%s][%d]------分析CMT包的业务头出错----------\n",__FILE__,__LINE__);
			return -1;
		}
	}
			
	return 0;
}
/******************************************************
	7. 重置结构体
******************************************************/
void lv_reset(TLVPACKAGE *lv){
	lv_destroy(lv);
	lv_create(lv);
	return ;
}

/*******************************************************
	8. int  lv_fetchpkg(TLVPACKAGE *ilv,TLVPACKAGE *olv)
	   解析批量报文，成功返回0，异常返回-1，没有记录返回1
	   要求ihv已经分析过
********************************************************/
int  lv_fetchpkg(TLVPACKAGE *ilv,TLVPACKAGE *olv){
   if (ilv->pkgType)
      return lv_fetchpkg_PKG(ilv,olv);
   else
      return lv_fetchpkg_CMT(ilv,olv);   
}
/********************************************************
	9. 对PKG报文拆分，成功返回 0，结束返回1，异常返回-1
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
   
   /*从报文中取得报文头、当前业务的业务头和报文体*/
   /*取业务头*/
	num = dm_memmem(&(ilv->dm),ilv->tradeStartP,"{2:",3);
	if (!num)	/*没有业务了*/
		return 1;
	dm_dmemcpy2(pkgTradeHeader,&(ilv->dm),num,LV_PKGTRADEHEADER_SIZE);
	/*正文体*/
	pkgBodyPos = num + LV_PKGTRADEHEADER_SIZE;
	num = dm_memmem(&(ilv->dm),pkgBodyPos,"}",1);
	if (!num || num<pkgBodyPos )
		return -1;

	data = (char *)malloc(num-pkgBodyPos+2);
	memset(data,0,num-pkgBodyPos+2);
	dm_dmemcpy2(data,&(ilv->dm),pkgBodyPos,num-pkgBodyPos+1);
	ilv->tradeStartP = num+1; 
	/*报文头*/
	dm_dmemcpy2(pkgHeader,&(ilv->dm),1,LV_PKGHEADER_SIZE);

	/*重组报文,修改长度和CMTCODE*/
	memset(sPKGLen,0,sizeof(sPKGLen));
	olv->length = LV_PKGHEADER_SIZE+LV_PKGTRADEHEADER_SIZE+num-pkgBodyPos+1;
	printf("%s,%d cmt长度[%d=%d+%d+%d-%d+1]",__FILE__,__LINE__,olv->length,LV_PKGHEADER_SIZE,LV_TRADEHEADER_SIZE,num,pkgBodyPos);
	sprintf(sPKGLen,"%06d",olv->length);
	printf("%s,%d cmt长度=[%s]",__FILE__,__LINE__,sPKGLen);


	memcpy(pkgHeader+3,sPKGLen,6);

	dm_dmemcat(&(olv->dm),pkgHeader,LV_PKGHEADER_SIZE);
	dm_dmemcat(&(olv->dm),pkgTradeHeader,LV_PKGTRADEHEADER_SIZE);
	dm_dmemcat(&(olv->dm),data,strlen(data));

	free(data);

	/*对新的报文进行分析*/
	if (lv_analyze_pack(olv)==-1)
		return -1;

	/*一些包体外的变量*/
	olv->pkgType = 1;
	olv->isFile = 0;
	olv->isMulti = 0;

	olv->tradeStartP = 1;

	return 0;
}

/********************************************************
	9. 对CMT报文拆分，成功返回 0，结束返回1，异常返回-1
*********************************************************/
static int  lv_fetchpkg_CMT(TLVPACKAGE *ilv,TLVPACKAGE *olv){
	/*取ilv的报文头，而修改其长度和CMTCode两个地方*/
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

	/*从报文中取得报文头、当前业务的业务头和报文体*/
	num = dm_memmem(&(ilv->dm),ilv->tradeStartP,"{2:",3);
	if (!num)	/*没有业务了*/
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

	/*重组报文,修改长度和CMTCODE*/
	memset(sPKGLen,0,sizeof(sPKGLen));
	olv->length = LV_PKGHEADER_SIZE+LV_TRADEHEADER_SIZE+num-pkgBodyPos+1;
	printf("%s,%d cmt长度[%d=%d+%d+%d-%d+1]",__FILE__,__LINE__,olv->length,LV_PKGHEADER_SIZE,LV_TRADEHEADER_SIZE,num,pkgBodyPos);
	sprintf(sPKGLen,"%06d",olv->length);
	printf("%s,%d cmt长度=[%s]",__FILE__,__LINE__,sPKGLen);
	memcpy(pkgHeader+3,sPKGLen,6);
	memset(cmtCode,0,sizeof(cmtCode));
	memcpy(cmtCode,tradeHeader+LV_TH_CMT_POS,LV_TAGNAME_LEN);
	memcpy(pkgHeader+LV_PH_CMT_POS,cmtCode,LV_TAGNAME_LEN);

	dm_dmemcat(&(olv->dm),pkgHeader,LV_PKGHEADER_SIZE);
	dm_dmemcat(&(olv->dm),tradeHeader,LV_TRADEHEADER_SIZE);
	dm_dmemcat(&(olv->dm),data,strlen(data));

	free(data);

	/*对新的报文进行分析*/
	if (lv_analyze_pack(olv)==-1)
		return -1;
	/*一些包体外的变量*/
	olv->pkgType = 0;
	olv->isFile = 0;
	olv->isMulti = 0;

	olv->tradeStartP = 1;

	return 0;
}
/**************************************************
10. 初始化报文结构体
***************************************************/
int  lv_create(TLVPACKAGE *lv){
	memset(lv,0,sizeof(TLVPACKAGE)-DMEM_SIZE);
	dm_create(&(lv->dm));
	return 0;
}
/**************************************************
11. 析构报文结构体
***************************************************/
int  lv_destroy(TLVPACKAGE *lv){
	memset(lv,0,sizeof(TLVPACKAGE)-DMEM_SIZE);
	dm_destroy(&(lv->dm));
	return 0;
}
/***************************************************
12.增加批量业务，一笔业务完成后给该业务增加业务头并结束该业务，成功返回0，失败返回-1
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
13.取出tag的取，成功返回tag值的长度，失败返回-1
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
			return ret-5; /*返回的value的长度*/
		}
		num += ret;

	}
	return 0;
}

/***************************************************
14.　得到tag的位置，成功返回所在位置，失败返回-1
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
			return num; /*返回TAG的位置*/
		}
		num += ret;

	}
	return 0;
}
/*******************************************************
15.　用于取有循环结构的TAG值，从offset位置取size位到value中
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
16.　给PKG当前业务添加业务头｛P：｝
*******************************************************/
int  lv_pkg_batch(TLVPACKAGE *lv,char *tagName,char *value)
{ 
	int  num,ret,tagLength,len,flg; 
	char *buf;

	if(!isTag(tagName))
	{
		printf("tag名不合法.\n");
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
17.取得PKG包中包头的tag值，成功返回0，失败返回-1
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
18　给dm结构添加按格式添加一个tag和他的值
结构如下： :tagName:tagData
*********************************************/
static int lv_dtagcpy(TDMEM *dm,char *tagName,char *tagData){
	int tagLength;

	if(!isTag(tagName))
	{
		printf("tag名不合法.\n");
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
19 在num之后找第一个tag,成功返回本TAG域的长度，没有找到返回0，异常返回-1
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
					if (value){/*if value==NULL 不记录value值*/
						dm_dmemcpy2(value,dm,num+5,i);
						value[i]='\0';
					}
					break;
				}		
			}
			return i+5; /*返回本tag域长度*/
		}else if (dm_getChar(dm,num)=='}')
			return 0;
		if ( num>dm->length )
			return -1;
	}
	return i+5; /*返回本tag域长度*/
}

/***************************************************************
20　生成批量业务的PKG包
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
21　生成CMT多业务包
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
22　生成CMT单业务包
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
23　分析报头
*************************************************/
static int lv_apk_pkgHeader(TLVPACKAGE *lv){
	/*把报文头信息写入结构体，并检查长度是否正确*/
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
		vtcp_log("[%s][%d]-----报文结构不正确------[%c][%d][%d][%c]\n",__FILE__,__LINE__, dm_getChar(&(lv->dm),lv->length-1), lv->length,lv->dm.length,dm_getChar(&(lv->dm),lv->dm.length));
		return -1;
	}
	/* add by LiuHuafeng 
	iAddLen= lv->dm.length/1024;
	 end by LiuHuafeng */
	if ( lv->length != lv->dm.length+iAddLen ){
		vtcp_log("[%s][%d]----报文长度不正确[%d][%d]------\n",__FILE__,__LINE__,lv->length,lv->dm.length);
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
24　分析是否CMT包批量报文
*****************************************************/
static int lv_apk_isCMTMulti(TLVPACKAGE *lv){
	int ret;
	ret = dm_memmem(&(lv->dm),1,"{B:",3);

	if (!ret)
		return 0;
	return 1;
}
/*************************************************
25　分析CMT包批量报文业务头
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
26　分析是否PKG包批量报文
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

	/*把包头中的packID(PKG包的ID)、packDate(包的日期)、respDate(应答日期)提取出来
	对应的tagName分别是：0BD、30E、BS6*/
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
	/*业务头*/
	ret = dm_memmem(&(lv->dm),1,"{2:",3);
	if (!ret)/*mod by chenggx 20110531 由"ret"修改为"!ret"*/
		return -1;
	memset(tmp,0,sizeof(10));
	dm_dmemcpy2(tmp,&(lv->dm),ret+3,12);
	memcpy(lv->CMTNo,tmp,3);
	memcpy(lv->reserve,tmp+3,9);
		vtcp_log("[%s][%d]",__FILE__,__LINE__);
	return 0;
}
/*分析单业务中的业务头*/
static int lv_apk_isCMTTrade(TLVPACKAGE *lv){
	int ret;

	if (lv->isMulti)	/*不处理批量里的业务头*/
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

   /*这些给包写入一些默认值*/
   if (lv->opeType=='\0')
      lv->opeType = '1';   /*默认的情况下为小额*/
   if (*(lv->opeStateCode)=='\0')
      memcpy(lv->opeStateCode,"000",3);   /*正常*/
   if (*(lv->sendTime)=='\0')
   {
	    memset(pcTimeStr,'\0',sizeof(pcTimeStr));
		appGetTime(NULL,0,pcTimeStr);
		memcpy(lv->sendTime,pcTimeStr,14);
   }
   if (*(lv->workDate)=='\0')
	   memcpy(lv->workDate,pcTimeStr,8);
   /*把workDate的付值放getflgNo之前才对*/
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

/*判断是否属于交易，是交易而有业务头{2:}*/
static int lv_isTrade(TLVPACKAGE *lv){
	if ( !memcmp(lv->CMTCode,"1",1) )
		return 1;
	return 0;
}

/*批量业务一笔业务结束PKG的处理*/
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
/*批量业务一笔业务结束CMT的处理*/
static int  lv_batch_end_CMT(TLVPACKAGE *lv){
	int num,ret;
	char *data;

	if ( lv->tradeEnd )
		return 0;
	/*增加后面的括号*/
	num = lv->dm.length;
	dm_dmemcat(&(lv->dm),"}",1);	
	/*增加业务头*/
	data = (char *)malloc(LV_TRADEHEADER_SIZE+1);
	memset(data,' ',LV_TRADEHEADER_SIZE); 	/*清空格，不是清0*/
	data[LV_TRADEHEADER_SIZE]='\0';
	memcpy(data,"{2:",3);
	memcpy(data+3,lv->CMTNo,3+1+40+16);	/*CMTNo3位，paymPRI1位，localSynSign40位，reserve16位*/
	data[LV_TRADEHEADER_SIZE-1]='}';
	/*由于tradeStartP的问题，这里会不会出现问题呢？*/
	ret = dm_memmem(&(lv->dm),lv->tradeStartP,"{3:",3);
	if (!ret)
	    return -1;
	num = ret;
	dm_dinsertmem(&(lv->dm),num,data,LV_TRADEHEADER_SIZE);
	/*清除当前批量业务的信息*/
	memset(lv->CMTNo,0,3+1+40+16);
	lv->tradeEnd = 1; 		/*本业务结束*/
	lv->isMulti = 1;
	return 0;
}

/*
**判断报文是不是文件报文。
**是返回1，不是返回0
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

	/*判断是否已经有{C:}*/
	if ( dm_memmem(&(lv->dm),1,"{C:",3) )
		return -1;
		
    /*增加checkSum的长度*/
    lv->length += 36 ;
    memset(sLength,0,sizeof(sLength));
    sprintf(sLength,"%06d",lv->length);
    memcpy(lv->dm.block.mem+3,sLength,6);
    /*添加checkSum的内容*/
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
	if ( iCmtCode < 1 || iCmtCode >11 ) /*PKG要素集在1-11时要加入密押*/
		return 0;
	memset(sPkgMac,0,sizeof(sPkgMac));
	memset(sPkgMac,'0',40);
	lv_pkg_batch(lv,"C15",sPkgMac);
	return 0;
}
/****Add by SSH,2005.11.24,暂时的计算表示号****/
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
		printf("%s,%d,第一次使用序号文件\n",__FILE__,__LINE__);
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
