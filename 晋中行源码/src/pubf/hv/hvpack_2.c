#include "find_debug.h"
/*************************************************
  Copyright (C), 2004, DHC
  File name:     hvpack.c
  Author:    韦昭   Version:   V1.0     Date:  2004/8/20
  Description:    大额支付报文相关的功能函数实现。
  Others: 
  Function List:
    对外函数，这些函数提供给外部使用
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
	   结构体清零，请在重新使用一个结构对象前使用，确保安全.

	内部函数，只供本模块的对外函数调用，不提倡在外部调用。
	1. int isTag(char *tagName)
	   判断是否是一个合法的tagName,现在规定是只能是字母或是数字，三位
	   合法返回0，非法返回-1
	2. char *searchTag(char *,char *tagName)
	   在str中查找是否存在tagName，返回tagName前面的":"的位置的指针
	3. void getPackageFlgNo(THVPACKAGE *hv);
	   获得flgNo,没有实现从数据库中生成，只是简单的写入一个字符串
    4. void getPackageReNo(THVPACKAGE *hv);
       获得flgNo,没有实现从数据库中生成，只是简单的写入一个字符串*
    5  int  tagcpy(char *pos,char *tagName,char *data);
	   在POS处插入":tagName:data",并返回这个字串的长度
  History:   
    1. Date:     2004-8-20
       by:   韦昭
       Modification:　建立
	2. Date:     2004-8-26
	   by:　 韦昭
	   Modification:  更新
	                  1) 增加hv_load_tagifno(TTAGINFO *ti,
					     char *fileName)函数
					  2)　修改
					  3)  hv_gen_pack()把原来的只要已经结尾就不再
					  　　动作改为全部刷新。
	3. Date:	2004-12-6
	   by:	weizhao
	   Modification: 修改
					 1)当CMT第1位不为1时产生包不加业务头
					 2)同上，分析包的时候不分析业务头
	4. Date:	2004-12-21
	   by: weizhao
	   Modification:重新过了一次，发现很多要改的地方。
					1)修改了hv_gen_pack()hv_analyze_pack()
					  和hv_get_tag(),几乎全部重写
*************************************************/
#include <time.h>
#include "hvpack_2.h"
#include "hv_pub.h"
#include "errlog.h"

TTAGINFO hv_taginfo;
int  hv_load_tagifno(char *fileName)
{
	if( tag_load(&hv_taginfo,fileName) == TAG_OP_SUCCEE )
	{
		return HV_OP_SUCCEE;
	}
	return HV_OP_FAIL;
}
/*************************************************
  Function:       int hv_set_tag(THVPACKAGE *,char *tagName,char *data)
  Description:    在报文体中插入某个TAG内容。
  Calls:          isTag(tagName)
                  searchTag(char *str,char *tagName)
				  int  tagcpy(char *pos,char *tagName,char *data)
  Called By:      
  Table Accessed: 无
  Table Updated:  无
  Input:    1 THVPACKAGE *hv,要插入的报文的指针；
            2 char *tagName,要插入的tag的名字；
			3 char *data，要插入的tag的内容。
  Output:   无
  Return:   非法TAG名或报文已经生成都返回-1,否则返回0
  Others:   
*************************************************/

int hv_set_tag(THVPACKAGE *hv,char *tagName,char *data)
{
	char *p;
	int tagLength;
	int flg;        /*用来做定长标记*/

	flg = 0;

	/* 判断tagName是否是合法(必须是字母或是数字)，不合法返回-1 */
	if(!isTag(tagName))
	{
		printf("tag名不合法.\n");
		return -1;
	}

	tagLength = tag_get(&hv_taginfo,tagName);
	/*if( tagLength == 0 )
		tagLength = strlen(data);
	else
		flg = 1; del by weizhao 2004-12-31 tag的长度是最大长度，不是固定长度*/
	/*add by weizhao 2004-12-31 实现上面要求*/
	if (tagLength <= 0)
	{
		tagLength = strlen(data);
	}else{
		if (tagLength>strlen(data))
		{
			tagLength = strlen(data);
		}
	}
	

	if(hv->length == 0)
	{   /*当插入的是第一个tag时，初始化报文，补上正文头并调整length*/
		MEMSET_DEBUG(hv->body,(int)(' '),HV_TEXT_STAR_POS); 
		MEMSET_DEBUG(hv->body+HV_TEXT_STAR_POS,'\0',HV_BODY_LENGTH-HV_TEXT_STAR_POS); /*并把后面的数据清零*/
		MEMCPY_DEBUG(hv->body+HV_TEXT_STAR_POS,"{3:",3);
		hv->length = HV_1ST_TAG_POS;
		p=NULL;
	}else
		p = searchTag(hv->body,tagName);
	
	if( p == NULL)
	{
		if(hv->body[hv->length-1]=='}')
			p = hv->body+hv->length-1;
		else
			p = hv->body+hv->length;
	}else{
		if(flg == 1) /*定长*/
		{
			int ti = tagLength+HV_TAGNAME_LENGTH+2;
			memmove(p,p+ti,strlen(p+ti));
			hv->length -= ti;
		}else{  /*不定长，要自己去判断要删除要哪里*/
			char *tp = NULL;
			int ti;
			if ( (tp=strchr(p+HV_TAGNAME_LENGTH+2,':')) != NULL  )
			{
				ti = tp-p;
			}else if((tp=strchr(p+HV_TAGNAME_LENGTH+2,'}')) != NULL){
				ti = tp-p;
			}else{
				tp = hv->body+hv->length;
				ti = tp-p;
			}
			memmove(p,tp,strlen(tp));
			hv->length -= ti;
		}
	}
	
	memmove(p+tagLength+HV_TAGNAME_LENGTH+2,
			p,strlen(p));
	MEMSET_DEBUG(p,' ',tagLength+HV_TAGNAME_LENGTH+2);
	/*tagcpy(p,tagName,data); del by weizhao 2004-12-31*/
	tagcpy2(p,tagName,data,tagLength);
	hv->length +=  tagLength+HV_TAGNAME_LENGTH+2;
}

/*************************************************
  Function:       void hv_set_tag_before(THVPACKAGE *,
                        char *beforeTagName,char *tagName,char *data)
  Description:    在报文体中某个TAG之前插入一个TAG.
  Calls:          searchTag(hv->body,tagName)
  　　　　　　　　tagcpy(p,tagName,data)
  Called By:      
  Table Accessed: 无
  Table Updated:  无
  Input:          1 THVPACKAGE *hv,要插入的报文的指针；
                  2 char *beforTagName,要插入位置的后一个tag;
                  3 char *tagName,要插入的tag的名字；
			      4 char *data，要插入的tag的内容。
  Output:         
  Return:         报文已经生成、非法TAG名或插入位置不存在返回-1,否则返回0
  Others:         
*************************************************/

int hv_set_tag_before(THVPACKAGE *hv,char *beforeTagName,
	                             char *tagName,char *data)
{
	/*变量*/
	char *p;
	/*下面这两个变量用于检查第二个判断*/
	char *pTag;
	char *pBeforeTag;
	int  tagLength;
	int offset = 0;
    
	if(!isTag(tagName))
	{
		printf("[%s][%d]tag名不合法.\n",__FILE__,__LINE__);
		return -1;
	}
	if(!isTag(beforeTagName))
	{
		printf("[%s][%d]tag名不合法.\n",__FILE__,__LINE__);
		return -1;
	}

	tagLength = tag_get(&hv_taginfo,tagName);
	/*if( tagLength == 0 )
	{
		tagLength = strlen(data);
		offset = 0;
	}else
		offset = tagLength; del by weizhao 2004-12-31*/
	/*add by weizhao 2004-12-31*/
	if (tagLength == 0)
	{
		tagLength = strlen(data);
	}else{
		if (tagLength>strlen(data))
		{
			tagLength = strlen(data);
		}
	}

	/*找不到beforeTagname*/
	pTag = searchTag(hv->body,tagName);
	pBeforeTag = searchTag(hv->body,beforeTagName);
	if( pBeforeTag == NULL)
	{
		printf("[%s][%d]找不到beforeTagName\n",__FILE__,__LINE__);
		return -1;
	}
	
	if(pTag !=NULL)
	{
		/*找到beforeTagName和tagName,但tagName不在beforeTagName的前面*/
		if( strchr(pTag+HV_TAGNAME_LENGTH+2+offset,':')!= pBeforeTag )
		{
			printf("[%s][%d]tagName和beforeTagName不在相临位置\n",__FILE__,__LINE__);
			return -1;
		}else{
			memmove(pTag,pBeforeTag,strlen(pBeforeTag));
			hv->length = hv->length - (pBeforeTag - pTag);
		}
	}else
		pTag = pBeforeTag;
	/*插入*/
	memmove(pTag+HV_TAGNAME_LENGTH+tagLength+2,pTag,strlen(pTag));
	MEMSET_DEBUG(pTag,' ',HV_TAGNAME_LENGTH+tagLength+2);
	hv->length += HV_TAGNAME_LENGTH+tagLength+2;
	/*tagcpy(pTag,tagName,data); del by weizhao 2004-12-31*/
	tagcpy2(pTag,tagName,data,tagLength);
	return 0;
}

/*************************************************
  Function:       int hv_gen_pack(THVPACKAGE *hv)
  Description:    组织，生成报文
  Calls:          
  Called By:      
  Table Accessed: 
  Table Updated:  
  Input:          THVPACKAGE *hv,指向一个结构的指针
  Output:         
  Return:         正常生成返回0，报文已经生成返回-1
  Others:         
*************************************************/

int  hv_gen_pack(THVPACKAGE *hv)
{
	char pcTimeStr[22];
	char tmpIdtno[7];
	int i;

	if(hv->CMTCode[0]!='1' 
		&& memcmp(hv->body+HV_TEXT_STAR_POS,"{3:",3)==0 )
	{
		/*前移过业务头{2:}*/
		memmove(hv->body+OPEHEADER,hv->body+HV_TEXT_STAR_POS,
				hv->length-HV_TEXT_STAR_POS);
		hv->length -= HV_TEXT_STAR_POS - OPEHEADER;
	}

	/*未封闭的包加上},并填充非body内容*/
	if(hv->body[hv->length-1] != '}')
	{
		hv->length = hv->length+1;
		hv->body[hv->length-1] = '}';
		hv->body[hv->length] = '\0';

		if(hv->opeStateCode[0]=='\0')
			MEMCPY_DEBUG(hv->opeStateCode,"000",3);

		getPackageFlgNo(hv);
		getPackageReNo(hv);

		MEMCPY_DEBUG(hv->sendTime,hv->workDate,8);
		MEMSET_DEBUG(pcTimeStr,'\0',sizeof(pcTimeStr));
		appGetTime(NULL,0,pcTimeStr);
		MEMCPY_DEBUG(hv->sendTime+8,pcTimeStr+8,6);
	}
	
	if ( hv->CMTCode[0]=='1' && memcmp(hv->body+OPEHEADER,"{3:",3)==0 )
	{
		memmove(hv->body+HV_TEXT_STAR_POS,hv->body+OPEHEADER,
				hv->length-OPEHEADER);
		hv->length += HV_TEXT_STAR_POS - OPEHEADER;
	}
	/*下面是对填写报文*/
	MEMCPY_DEBUG(hv->body+USE,&(hv->use),PHEADER_END-USE);
	hv->body[PHEADER_END]='}';
	MEMCPY_DEBUG(hv->body,"{1:",3);

	MEMSET_DEBUG(tmpIdtno,'\0',sizeof(tmpIdtno));
	sprintf(tmpIdtno,"%06d",hv->length);
	MEMCPY_DEBUG(hv->body+LENGTH,tmpIdtno,6);

	/*业务头*/
	if(hv->CMTCode[0]=='1'){
		MEMCPY_DEBUG(hv->body+OPEHEADER,"{2:",3);

		MEMCPY_DEBUG(hv->body+OPE_CMTCODE,hv->CMTCode,3);
		hv->body[PRI_POS]=hv->PRI;
		MEMSET_DEBUG(hv->body+FORFUTURE,' ',16);/*预留位置16位*/
		hv->body[OPEHEADER_END] = '}';
	}

	/*扫描文件，把其中的'\0'全部用' '代替*/
    for(i=0;i<hv->length;i++)
	{
		if(hv->body[i]=='\0')
		{
			hv->body[i] = ' ';
		}
	}
	hv->body[hv->length]='\0';

}


/*************************************************
  Function:       hv_analyze_pack(THVPACKAGE *hv)
  Description:    分析报文数据，根据报文数据中报文头的内容填充
                  THVPACKAGE中的字段，并检查报文体是否合法.
  Calls:          
  Called By:      
  Table Accessed: 
  Table Updated:  
  Input:          THVPACKAGE *hv,要分析的报文结构指针
  Output:         
  Return:         成功返回0，否则返回-1。
  Others:         
*************************************************/

int hv_analyze_pack(THVPACKAGE *hv)
{
	char *startP ; /*这个变量用于扫描tagName记录开始扫描的位置*/
	char tmpStr[20];
	int tagLength;

	tagLength = 0;

	/*如果这是一个批量支付业务报文 add by weizhao 2005-3-28*/
	if (memcmp(hv->body+CMTCODE,"689",3)==0)
	{
		if(  memcmp(hv->body,"{1:",3) != 0 ||
			 memcmp(hv->body+OPEHEADER-1,"}{B:",4) != 0 )
		{
			printf("[%s][%d]检查到错误的报文结构，报文体不合法。\n",__FILE__,__LINE__);
			return -1;
		}

		/*取得报文长度,由字符串转为数字型*/
		MEMSET_DEBUG(tmpStr,'\0',sizeof(tmpStr));
		MEMCPY_DEBUG(tmpStr,hv->body+LENGTH,6);
		hv->length = atoi(tmpStr);

		/*补充检查最后是否是以"}"结尾*/
		if(hv->body[hv->length-1] != '}')
		{
			printf("检查到错误的报文结构(不以\"}\"结尾)，报文体不合法。\n");
			return -1;
		}

		/*取得报文用途、来往标记、业务类型、业务状态码、报文标识号
		　报文参考号、发送时间、CMT号码、工作日期*/
		MEMCPY_DEBUG(&(hv->use),hv->body+USE,PHEADER_END-USE);
		/*取批量报文中{B:}中的四个字段*/
		MEMSET_DEBUG(tmpStr,'\0',sizeof(tmpStr));
		MEMCPY_DEBUG(tmpStr,hv->body+RECVDNUM_POS,10);
		hv->recvDnum = atoi(tmpStr);

		MEMSET_DEBUG(tmpStr,'\0',sizeof(tmpStr));
		MEMCPY_DEBUG(tmpStr,hv->body+RECVCNUM_POS,10);
		hv->recvCnum = atoi(tmpStr);

		MEMSET_DEBUG(tmpStr,'\0',sizeof(tmpStr));
		MEMCPY_DEBUG(tmpStr,hv->body+SENDDNUM_POS,10);
		hv->sendDnum = atoi(tmpStr);

		MEMSET_DEBUG(tmpStr,'\0',sizeof(tmpStr));
		MEMCPY_DEBUG(tmpStr,hv->body+SENDCNUM_POS,10);	
		hv->sendCnum = atoi(tmpStr);

		hv->pos = SENDCNUM_POS+11;/*指向第一条记录.*/

		return 0;
	}

	/*检查报文体是否合法*/
	/*检查报文体结构,还有最后一个“｝”没有检查*/
	if(hv->body[CMTCODE]!='1')
	{
		startP = hv->body+OPEHEADER;
		if( memcmp(hv->body,"{1:",3)!=0 ||
			memcmp(startP-1,"}{3:",4)!=0 )
		{
			printf("[%s][%d]检查到错误的报文结构，报文体不合法。\n",__FILE__,__LINE__);
			return -1;
		} 
	}else{
		startP = hv->body+HV_TEXT_STAR_POS;
		if(  memcmp(hv->body,"{1:",3) != 0 ||
			 memcmp(hv->body+OPEHEADER-1,"}{2:",4) != 0 ||
			 memcmp(startP-1,"}{3:",4) != 0  )
		{
			printf("[%s][%d]检查到错误的报文结构，报文体不合法。\n",__FILE__,__LINE__);
			return -1;
		} 
	}
	
	/*检查tagName的长度和结构*/
	startP += 3;
	while(1)
	{
		startP = strchr(startP,':');
		if( startP == NULL )
		{
			break;
		}

		if( startP[HV_TAGNAME_LENGTH+1] != ':' )
		{
			printf("检查到错误的tagName长度，这个报文体不合法。\n");
			return -1;
		}
		
		MEMSET_DEBUG(tmpStr,'\0',HV_TAGNAME_LENGTH+1);
		MEMCPY_DEBUG(tmpStr,startP+1,HV_TAGNAME_LENGTH);
		/*tagLength = tag_get(&hv_taginfo,tmpStr);

		if(tagLength<0)
		{
			tagLength = 0;
		} del by weizhao 2004-12-31*/
		tagLength = 0;
		
		startP += (5 + tagLength);
		tagLength = 0;
	}


	/*取得报文长度,由字符串转为数字型*/
	MEMSET_DEBUG(tmpStr,'\0',sizeof(tmpStr));
	MEMCPY_DEBUG(tmpStr,hv->body+LENGTH,6);
	hv->length = atoi(tmpStr);

	/*补充检查最后是否是以"}"结尾*/
	if(hv->body[hv->length-1] != '}')
	{
		printf("检查到错误的报文结构(不以\"}\"结尾)，报文体不合法。\n");
		return -1;
	}

	/*取得报文用途、来往标记、业务类型、业务状态码、报文标识号
	　报文参考号、发送时间、CMT号码、工作日期*/
	MEMCPY_DEBUG(&(hv->use),hv->body+USE,PHEADER_END-USE);
	/*取得优先级和最后16位预留位*/
	if(hv->body[CMTCODE]=='1')
		MEMCPY_DEBUG(&(hv->PRI),hv->body+PRI_POS,OPEHEADER_END-PRI_POS);

	return 0;
}

/*************************************************
  Function:       hv_get_tag(THVPACKAGE *hv,char *tagName,char *data)
  Description:    得到某个TAG的内容,若指定TAG不存在则置data[0]='\0'且返回-1,否
	              则设置data志并返回0。
  Calls:
  Called By:
  Table Accessed:
  Table Updated:
  Input:          THVPACKAGE *hv,
                  char *tagName,
                  char *data
  Output:         
  Return:         若指定TAG不存在则置data[0]='\0'且返回-1,否
	              则设置data志并返回0。
  Others:
*************************************************/
int hv_get_tag(THVPACKAGE *hv,char *tagName,char *data)
{
	char *p;
	char *tmp;
	int  strLength;

	p = searchTag(hv->body,tagName);
	if(p == NULL)
	{
		data[0] = '\0';
		return -1;
	}
	
	/*strLength = tag_get(&hv_taginfo,tagName); add by weizhao 2004-12-31*/
	strLength = 0;
	if(strLength == 0)   /*不是固定长度的时候*/
	{
		tmp = strchr(p+5,':');
		if( tmp ==NULL)
		{/*当tag是最后一个tag的时候*/
			tmp = strrchr( hv->body,(int)('}') );
		}
		strLength = tmp - p - 5;
	}	
	MEMCPY_DEBUG(data,p+5,strLength);
	
	return 0;
}

/*************************************************
  Function:       hv_reset(THVPACKAGE *hv)
  Description:    给结构体清零
  Calls:
  Called By:
  Table Accessed:
  Table Updated:
  Input:          THVPACKAGE *hv
  Output:         
  Return:         
  Others:
*************************************************/

void hv_reset(THVPACKAGE *hv)
{
	MEMSET_DEBUG(hv,0,sizeof(THVPACKAGE));
}
/*************************************************
  Function:       hv_fetchpkg(THVPACKAGE *ihv,THVPACKAGE *ohv)
  Description:    得到批量业务的下一条记录
  Calls:
  Called By:
  Table Accessed:
  Table Updated:
  Input:       THVPACKAGE *ihv,THVPACKAGE *ohv
  Output:         
  Return:      成功0，记录结束1，异常-1 
  Others:
*************************************************/
int  hv_fetchpkg(THVPACKAGE *ihv,THVPACKAGE *ohv)
{
	char *tmpp1=NULL;
	char *tmpp2=NULL;
	char tmpIdtno[20];

	if (ihv->pos>=ihv->length)
	{
		printf("[%s][%d]==记录结束.\n",__FILE__,__LINE__);
		return 1;
	}else if(ihv->pos<SENDCNUM_POS+11){
		return -1;
	}

	if (memcmp(ihv->body+ihv->pos,"{2:",3)!=0)
	{
		printf("[%s][%d]错误的记录.\n",__FILE__,__LINE__);
		return -1;
	}

	/*给ohv赋值*/
	tmpp1 = ihv->body+ihv->pos;
	tmpp2 = strchr(tmpp1,'}');
	if (tmpp2==NULL)
	{
		printf("[%s][%d]==查找记录出错，记录不完整.\n",__FILE__,__LINE__);
		return -1;
	}
	tmpp2++;
	tmpp2 = strchr(tmpp2,'}');
	ohv->length = tmpp2-tmpp1+2+PHEADER_END;
	/*取得报文用途、来往标记、业务类型、业务状态码、报文标识号
	　报文参考号、发送时间、CMT号码、工作日期*/
	MEMCPY_DEBUG(&(ohv->use),ihv->body+USE,PHEADER_END-USE);
	/*取得优先级和最后16位预留位*/
	MEMCPY_DEBUG(&(ohv->PRI),ihv->body+ihv->pos+6,OPEHEADER_END-PRI_POS);
	MEMCPY_DEBUG(ohv->CMTCode,ihv->body+ihv->pos+3,3);
	MEMCPY_DEBUG(ohv->body,tmpp2-ohv->length+1,ohv->length);
	
	MEMCPY_DEBUG(ohv->body+USE,&(ohv->use),PHEADER_END-USE);
	ohv->body[PHEADER_END]='}';
	MEMCPY_DEBUG(ohv->body,"{1:",3);

	MEMSET_DEBUG(tmpIdtno,'\0',sizeof(tmpIdtno));
	sprintf(tmpIdtno,"%06d",ohv->length);
	MEMCPY_DEBUG(ohv->body+LENGTH,tmpIdtno,6);

	ohv->body[ohv->length]='\0';
	ihv->pos += tmpp2-tmpp1+1;
		
	return 0;
}
/*************************************************
其它的函数，内部函数。
*************************************************/


int isTag(char *tagName)
{
	int i;
	/*不用检查tagName的长度，使用时一律只取3位。*/
	for(i=0; i<HV_TAGNAME_LENGTH ; i++)
	{
		if(!isalnum(tagName[i]))
		{
			return 0;
		}
	}

	return 1;
}

/*注：输入的data必须是以“\0”结尾的，不然在就可能会出现错误*/
int  tagcpy(char *pos,char *tagName,char *data)
{
	char *tmpStr;
	int i = HV_TAGNAME_LENGTH+strlen(data)+2;
	tmpStr = malloc(i+1);
	sprintf(tmpStr,":%s:%s",tagName,data);
	MEMCPY_DEBUG(pos,tmpStr,i);
	free(tmpStr);

	return i;
}
/*add by weizhao 2004-12-31*/
int  tagcpy2(char *pos,char *tagName,char *data,int datalen)
{
	int i = HV_TAGNAME_LENGTH+datalen+2;
	pos[0]=':';
	MEMCPY_DEBUG(pos+1,tagName,HV_TAGNAME_LENGTH);
	pos[HV_TAGNAME_LENGTH+1]=':';
	MEMCPY_DEBUG(pos+2+HV_TAGNAME_LENGTH,data,datalen);
	
	return i;
}
void getPackageFlgNo(THVPACKAGE *hv)
{
	/*作用时从数据库中读出并生成*/
	/*Add by SSH,2004.9.8,标示号*/
	char tmpIdtno[9];
	MEMSET_DEBUG(tmpIdtno,'\0',sizeof(tmpIdtno));
	sprintf(tmpIdtno,"%08d",hv_get_idtno());
	MEMCPY_DEBUG(hv->flgNo,tmpIdtno,sizeof(hv->flgNo));
	/***********************************/
}
void getPackageReNo(THVPACKAGE *hv)
{
	char tmpIdtno[21];
	MEMSET_DEBUG(tmpIdtno,'\0',sizeof(tmpIdtno));
	sprintf(tmpIdtno,"%s%08d",HV_BANKCODE_FLGNO,hv_get_refno());
	MEMCPY_DEBUG(hv->reNo,tmpIdtno,sizeof(hv->reNo));
}
