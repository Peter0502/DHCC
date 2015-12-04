#include "find_debug.h"
/*************************************************
  Copyright (C), 2004, DHC
  File name:     hvpack.c
  Author:    Τ��   Version:   V1.0     Date:  2004/8/20
  Description:    ���֧��������صĹ��ܺ���ʵ�֡�
  Others: 
  Function List:
    ���⺯������Щ�����ṩ���ⲿʹ��
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
	   �ṹ�����㣬��������ʹ��һ���ṹ����ǰʹ�ã�ȷ����ȫ.

	�ڲ�������ֻ����ģ��Ķ��⺯�����ã����ᳫ���ⲿ���á�
	1. int isTag(char *tagName)
	   �ж��Ƿ���һ���Ϸ���tagName,���ڹ涨��ֻ������ĸ�������֣���λ
	   �Ϸ�����0���Ƿ�����-1
	2. char *searchTag(char *,char *tagName)
	   ��str�в����Ƿ����tagName������tagNameǰ���":"��λ�õ�ָ��
	3. void getPackageFlgNo(THVPACKAGE *hv);
	   ���flgNo,û��ʵ�ִ����ݿ������ɣ�ֻ�Ǽ򵥵�д��һ���ַ���
    4. void getPackageReNo(THVPACKAGE *hv);
       ���flgNo,û��ʵ�ִ����ݿ������ɣ�ֻ�Ǽ򵥵�д��һ���ַ���*
    5  int  tagcpy(char *pos,char *tagName,char *data);
	   ��POS������":tagName:data",����������ִ��ĳ���
  History:   
    1. Date:     2004-8-20
       by:   Τ��
       Modification:������
	2. Date:     2004-8-26
	   by:�� Τ��
	   Modification:  ����
	                  1) ����hv_load_tagifno(TTAGINFO *ti,
					     char *fileName)����
					  2)���޸�
					  3)  hv_gen_pack()��ԭ����ֻҪ�Ѿ���β�Ͳ���
					  ����������Ϊȫ��ˢ�¡�
	3. Date:	2004-12-6
	   by:	weizhao
	   Modification: �޸�
					 1)��CMT��1λ��Ϊ1ʱ����������ҵ��ͷ
					 2)ͬ�ϣ���������ʱ�򲻷���ҵ��ͷ
	4. Date:	2004-12-21
	   by: weizhao
	   Modification:���¹���һ�Σ����ֺܶ�Ҫ�ĵĵط���
					1)�޸���hv_gen_pack()hv_analyze_pack()
					  ��hv_get_tag(),����ȫ����д
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
  Description:    �ڱ������в���ĳ��TAG���ݡ�
  Calls:          isTag(tagName)
                  searchTag(char *str,char *tagName)
				  int  tagcpy(char *pos,char *tagName,char *data)
  Called By:      
  Table Accessed: ��
  Table Updated:  ��
  Input:    1 THVPACKAGE *hv,Ҫ����ı��ĵ�ָ�룻
            2 char *tagName,Ҫ�����tag�����֣�
			3 char *data��Ҫ�����tag�����ݡ�
  Output:   ��
  Return:   �Ƿ�TAG�������Ѿ����ɶ�����-1,���򷵻�0
  Others:   
*************************************************/

int hv_set_tag(THVPACKAGE *hv,char *tagName,char *data)
{
	char *p;
	int tagLength;
	int flg;        /*�������������*/

	flg = 0;

	/* �ж�tagName�Ƿ��ǺϷ�(��������ĸ��������)�����Ϸ�����-1 */
	if(!isTag(tagName))
	{
		printf("tag�����Ϸ�.\n");
		return -1;
	}

	tagLength = tag_get(&hv_taginfo,tagName);
	/*if( tagLength == 0 )
		tagLength = strlen(data);
	else
		flg = 1; del by weizhao 2004-12-31 tag�ĳ�������󳤶ȣ����ǹ̶�����*/
	/*add by weizhao 2004-12-31 ʵ������Ҫ��*/
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
	{   /*��������ǵ�һ��tagʱ����ʼ�����ģ���������ͷ������length*/
		MEMSET_DEBUG(hv->body,(int)(' '),HV_TEXT_STAR_POS); 
		MEMSET_DEBUG(hv->body+HV_TEXT_STAR_POS,'\0',HV_BODY_LENGTH-HV_TEXT_STAR_POS); /*���Ѻ������������*/
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
		if(flg == 1) /*����*/
		{
			int ti = tagLength+HV_TAGNAME_LENGTH+2;
			memmove(p,p+ti,strlen(p+ti));
			hv->length -= ti;
		}else{  /*��������Ҫ�Լ�ȥ�ж�Ҫɾ��Ҫ����*/
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
  Description:    �ڱ�������ĳ��TAG֮ǰ����һ��TAG.
  Calls:          searchTag(hv->body,tagName)
  ����������������tagcpy(p,tagName,data)
  Called By:      
  Table Accessed: ��
  Table Updated:  ��
  Input:          1 THVPACKAGE *hv,Ҫ����ı��ĵ�ָ�룻
                  2 char *beforTagName,Ҫ����λ�õĺ�һ��tag;
                  3 char *tagName,Ҫ�����tag�����֣�
			      4 char *data��Ҫ�����tag�����ݡ�
  Output:         
  Return:         �����Ѿ����ɡ��Ƿ�TAG�������λ�ò����ڷ���-1,���򷵻�0
  Others:         
*************************************************/

int hv_set_tag_before(THVPACKAGE *hv,char *beforeTagName,
	                             char *tagName,char *data)
{
	/*����*/
	char *p;
	/*�����������������ڼ��ڶ����ж�*/
	char *pTag;
	char *pBeforeTag;
	int  tagLength;
	int offset = 0;
    
	if(!isTag(tagName))
	{
		printf("[%s][%d]tag�����Ϸ�.\n",__FILE__,__LINE__);
		return -1;
	}
	if(!isTag(beforeTagName))
	{
		printf("[%s][%d]tag�����Ϸ�.\n",__FILE__,__LINE__);
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

	/*�Ҳ���beforeTagname*/
	pTag = searchTag(hv->body,tagName);
	pBeforeTag = searchTag(hv->body,beforeTagName);
	if( pBeforeTag == NULL)
	{
		printf("[%s][%d]�Ҳ���beforeTagName\n",__FILE__,__LINE__);
		return -1;
	}
	
	if(pTag !=NULL)
	{
		/*�ҵ�beforeTagName��tagName,��tagName����beforeTagName��ǰ��*/
		if( strchr(pTag+HV_TAGNAME_LENGTH+2+offset,':')!= pBeforeTag )
		{
			printf("[%s][%d]tagName��beforeTagName��������λ��\n",__FILE__,__LINE__);
			return -1;
		}else{
			memmove(pTag,pBeforeTag,strlen(pBeforeTag));
			hv->length = hv->length - (pBeforeTag - pTag);
		}
	}else
		pTag = pBeforeTag;
	/*����*/
	memmove(pTag+HV_TAGNAME_LENGTH+tagLength+2,pTag,strlen(pTag));
	MEMSET_DEBUG(pTag,' ',HV_TAGNAME_LENGTH+tagLength+2);
	hv->length += HV_TAGNAME_LENGTH+tagLength+2;
	/*tagcpy(pTag,tagName,data); del by weizhao 2004-12-31*/
	tagcpy2(pTag,tagName,data,tagLength);
	return 0;
}

/*************************************************
  Function:       int hv_gen_pack(THVPACKAGE *hv)
  Description:    ��֯�����ɱ���
  Calls:          
  Called By:      
  Table Accessed: 
  Table Updated:  
  Input:          THVPACKAGE *hv,ָ��һ���ṹ��ָ��
  Output:         
  Return:         �������ɷ���0�������Ѿ����ɷ���-1
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
		/*ǰ�ƹ�ҵ��ͷ{2:}*/
		memmove(hv->body+OPEHEADER,hv->body+HV_TEXT_STAR_POS,
				hv->length-HV_TEXT_STAR_POS);
		hv->length -= HV_TEXT_STAR_POS - OPEHEADER;
	}

	/*δ��յİ�����},������body����*/
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
	/*�����Ƕ���д����*/
	MEMCPY_DEBUG(hv->body+USE,&(hv->use),PHEADER_END-USE);
	hv->body[PHEADER_END]='}';
	MEMCPY_DEBUG(hv->body,"{1:",3);

	MEMSET_DEBUG(tmpIdtno,'\0',sizeof(tmpIdtno));
	sprintf(tmpIdtno,"%06d",hv->length);
	MEMCPY_DEBUG(hv->body+LENGTH,tmpIdtno,6);

	/*ҵ��ͷ*/
	if(hv->CMTCode[0]=='1'){
		MEMCPY_DEBUG(hv->body+OPEHEADER,"{2:",3);

		MEMCPY_DEBUG(hv->body+OPE_CMTCODE,hv->CMTCode,3);
		hv->body[PRI_POS]=hv->PRI;
		MEMSET_DEBUG(hv->body+FORFUTURE,' ',16);/*Ԥ��λ��16λ*/
		hv->body[OPEHEADER_END] = '}';
	}

	/*ɨ���ļ��������е�'\0'ȫ����' '����*/
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
  Description:    �����������ݣ����ݱ��������б���ͷ���������
                  THVPACKAGE�е��ֶΣ�����鱨�����Ƿ�Ϸ�.
  Calls:          
  Called By:      
  Table Accessed: 
  Table Updated:  
  Input:          THVPACKAGE *hv,Ҫ�����ı��Ľṹָ��
  Output:         
  Return:         �ɹ�����0�����򷵻�-1��
  Others:         
*************************************************/

int hv_analyze_pack(THVPACKAGE *hv)
{
	char *startP ; /*�����������ɨ��tagName��¼��ʼɨ���λ��*/
	char tmpStr[20];
	int tagLength;

	tagLength = 0;

	/*�������һ������֧��ҵ���� add by weizhao 2005-3-28*/
	if (memcmp(hv->body+CMTCODE,"689",3)==0)
	{
		if(  memcmp(hv->body,"{1:",3) != 0 ||
			 memcmp(hv->body+OPEHEADER-1,"}{B:",4) != 0 )
		{
			printf("[%s][%d]��鵽����ı��Ľṹ�������岻�Ϸ���\n",__FILE__,__LINE__);
			return -1;
		}

		/*ȡ�ñ��ĳ���,���ַ���תΪ������*/
		MEMSET_DEBUG(tmpStr,'\0',sizeof(tmpStr));
		MEMCPY_DEBUG(tmpStr,hv->body+LENGTH,6);
		hv->length = atoi(tmpStr);

		/*����������Ƿ�����"}"��β*/
		if(hv->body[hv->length-1] != '}')
		{
			printf("��鵽����ı��Ľṹ(����\"}\"��β)�������岻�Ϸ���\n");
			return -1;
		}

		/*ȡ�ñ�����;��������ǡ�ҵ�����͡�ҵ��״̬�롢���ı�ʶ��
		�����Ĳο��š�����ʱ�䡢CMT���롢��������*/
		MEMCPY_DEBUG(&(hv->use),hv->body+USE,PHEADER_END-USE);
		/*ȡ����������{B:}�е��ĸ��ֶ�*/
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

		hv->pos = SENDCNUM_POS+11;/*ָ���һ����¼.*/

		return 0;
	}

	/*��鱨�����Ƿ�Ϸ�*/
	/*��鱨����ṹ,�������һ��������û�м��*/
	if(hv->body[CMTCODE]!='1')
	{
		startP = hv->body+OPEHEADER;
		if( memcmp(hv->body,"{1:",3)!=0 ||
			memcmp(startP-1,"}{3:",4)!=0 )
		{
			printf("[%s][%d]��鵽����ı��Ľṹ�������岻�Ϸ���\n",__FILE__,__LINE__);
			return -1;
		} 
	}else{
		startP = hv->body+HV_TEXT_STAR_POS;
		if(  memcmp(hv->body,"{1:",3) != 0 ||
			 memcmp(hv->body+OPEHEADER-1,"}{2:",4) != 0 ||
			 memcmp(startP-1,"}{3:",4) != 0  )
		{
			printf("[%s][%d]��鵽����ı��Ľṹ�������岻�Ϸ���\n",__FILE__,__LINE__);
			return -1;
		} 
	}
	
	/*���tagName�ĳ��Ⱥͽṹ*/
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
			printf("��鵽�����tagName���ȣ���������岻�Ϸ���\n");
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


	/*ȡ�ñ��ĳ���,���ַ���תΪ������*/
	MEMSET_DEBUG(tmpStr,'\0',sizeof(tmpStr));
	MEMCPY_DEBUG(tmpStr,hv->body+LENGTH,6);
	hv->length = atoi(tmpStr);

	/*����������Ƿ�����"}"��β*/
	if(hv->body[hv->length-1] != '}')
	{
		printf("��鵽����ı��Ľṹ(����\"}\"��β)�������岻�Ϸ���\n");
		return -1;
	}

	/*ȡ�ñ�����;��������ǡ�ҵ�����͡�ҵ��״̬�롢���ı�ʶ��
	�����Ĳο��š�����ʱ�䡢CMT���롢��������*/
	MEMCPY_DEBUG(&(hv->use),hv->body+USE,PHEADER_END-USE);
	/*ȡ�����ȼ������16λԤ��λ*/
	if(hv->body[CMTCODE]=='1')
		MEMCPY_DEBUG(&(hv->PRI),hv->body+PRI_POS,OPEHEADER_END-PRI_POS);

	return 0;
}

/*************************************************
  Function:       hv_get_tag(THVPACKAGE *hv,char *tagName,char *data)
  Description:    �õ�ĳ��TAG������,��ָ��TAG����������data[0]='\0'�ҷ���-1,��
	              ������data־������0��
  Calls:
  Called By:
  Table Accessed:
  Table Updated:
  Input:          THVPACKAGE *hv,
                  char *tagName,
                  char *data
  Output:         
  Return:         ��ָ��TAG����������data[0]='\0'�ҷ���-1,��
	              ������data־������0��
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
	if(strLength == 0)   /*���ǹ̶����ȵ�ʱ��*/
	{
		tmp = strchr(p+5,':');
		if( tmp ==NULL)
		{/*��tag�����һ��tag��ʱ��*/
			tmp = strrchr( hv->body,(int)('}') );
		}
		strLength = tmp - p - 5;
	}	
	MEMCPY_DEBUG(data,p+5,strLength);
	
	return 0;
}

/*************************************************
  Function:       hv_reset(THVPACKAGE *hv)
  Description:    ���ṹ������
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
  Description:    �õ�����ҵ�����һ����¼
  Calls:
  Called By:
  Table Accessed:
  Table Updated:
  Input:       THVPACKAGE *ihv,THVPACKAGE *ohv
  Output:         
  Return:      �ɹ�0����¼����1���쳣-1 
  Others:
*************************************************/
int  hv_fetchpkg(THVPACKAGE *ihv,THVPACKAGE *ohv)
{
	char *tmpp1=NULL;
	char *tmpp2=NULL;
	char tmpIdtno[20];

	if (ihv->pos>=ihv->length)
	{
		printf("[%s][%d]==��¼����.\n",__FILE__,__LINE__);
		return 1;
	}else if(ihv->pos<SENDCNUM_POS+11){
		return -1;
	}

	if (memcmp(ihv->body+ihv->pos,"{2:",3)!=0)
	{
		printf("[%s][%d]����ļ�¼.\n",__FILE__,__LINE__);
		return -1;
	}

	/*��ohv��ֵ*/
	tmpp1 = ihv->body+ihv->pos;
	tmpp2 = strchr(tmpp1,'}');
	if (tmpp2==NULL)
	{
		printf("[%s][%d]==���Ҽ�¼������¼������.\n",__FILE__,__LINE__);
		return -1;
	}
	tmpp2++;
	tmpp2 = strchr(tmpp2,'}');
	ohv->length = tmpp2-tmpp1+2+PHEADER_END;
	/*ȡ�ñ�����;��������ǡ�ҵ�����͡�ҵ��״̬�롢���ı�ʶ��
	�����Ĳο��š�����ʱ�䡢CMT���롢��������*/
	MEMCPY_DEBUG(&(ohv->use),ihv->body+USE,PHEADER_END-USE);
	/*ȡ�����ȼ������16λԤ��λ*/
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
�����ĺ������ڲ�������
*************************************************/


int isTag(char *tagName)
{
	int i;
	/*���ü��tagName�ĳ��ȣ�ʹ��ʱһ��ֻȡ3λ��*/
	for(i=0; i<HV_TAGNAME_LENGTH ; i++)
	{
		if(!isalnum(tagName[i]))
		{
			return 0;
		}
	}

	return 1;
}

/*ע�������data�������ԡ�\0����β�ģ���Ȼ�ھͿ��ܻ���ִ���*/
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
	/*����ʱ�����ݿ��ж���������*/
	/*Add by SSH,2004.9.8,��ʾ��*/
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
