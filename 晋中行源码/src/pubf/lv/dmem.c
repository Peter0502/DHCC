/*************************************************
  Copyright (C), 2005, DHC
  File name:     dmem.c
  Author:    Τ��   Version:   V1.0     Date:  2005-8-25
  Description:    �ֲ�ʽ����ģ�麯��ʵ�֡�
  Others: 
  Function List:
  1. void dm_create(TDMEM *dm);
		��ʼ���ڴ��
	2. void dm_destroy(TDMEM *dm);
		�����ڴ��
	3. TDBLOCK *dm_getBlock(TDMEM *dm,int i);
		�õ���i����Ϣ���ɹ����ظÿ�ָ�룬ʧ�ܷ���NULL
	4. char dm_getChar(TDMEM *dm,int i);
		ȡ�õ�i���ַ���i!=0���Ҳ�������NULL
	5. int  dm_freshLength(TDMEM *dm);
		����ÿ����Ĵ�С����������Ĵ�Сֵ�����ؿ�Ĵ�С
	6. TDBLOCK *dm_addBlock(TDMEM *dm);
		�����еĿ��������һ���飬�������ӵĿ����Ϣ
	7. void dm_delBlock(TDMEM *dm);
		ɾ�����һ���顣
	8. int  dm_deldmem(TDMEM *dm,int num);
		��dm��num��ʼ���ַ���ɾ��,����ɾ����ĳ��ȡ�
	9. int  dm_dmemcat(TDMEM *dm,char *buf,int size);
		��ԭ���������ڼ���buf������0,�������Ӻ�ĳ���
	10. int  dm_dmemcpy(TDMEM *dm,int num,char *buf,int size);
		��10��֮����buf�е����ݿ���dm�У���num��,���ؿ��볤��
	11. int  dm_dmemcpy2(char *buf,TDMEM *dm,int num,int size);
		��dm�ĵ�num������size���ַ����Ƶ�buf�У��ɹ����ؿ�������ݵĳ���
		��size���ܴ���num��ĳ���
	13. int	 dm_memmem(TDMEM *dm,int num,char *buf,int b_size);
		��NUM��ʼ����buf b_size���ַ���ͬ�Ŀ飬������λ�ã�ʧ�ܷ���0
	14. int  dm_dinsertmem(TDMEM *dm,int num,char *buf,int size);
		�ڵ�NUM��λ�ò���buf��size���ֽڣ��ɹ������µĳ��ȣ�ʧ�ܷ���-1
	15. int  dm_dstrlen(TDMEM *dm,int num);
		��num�����ĳ��ȣ�����NUM������Ҫ�ĳ���
	16. int  dm_dblkcpy(TDMEM *dm,int blockNo,in offset,char *buf,int  size);
		��16�෴����dm��blockNo��offsetλ�ÿ�size���ַ���buf
	17. int  dm_dblkcpy2(char *buf,TDMEM *dm,int blockNo,int offset,int size);
		�ڵ�blockNo����offsetλ�ò���buf��size���ַ������ظ��Ƶĸ���
	18. void dm_show(TDMEM *dm);
		��ӡdm�е���������
	19. int  dm_zip0(TDMEM *dm);
		��dm�е�0���ɿո�(С���ר��)
  History:   
    1. Date:     2005-8-25
       by:   Τ��
       Modification:������
    2. Date:		2005-12-6 
    	 by:	 Τ��
    	 Modification:��Ϊ���Ч�ʣ�ɾ��һЩ���õı����ͼ���ʹ�ú�ʱ����
*******************************************************/
#include "dmem.h"
#include "errlog.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void dm_create(TDMEM *dm){
	memset(dm->block.mem,0,DMEM_SIZE+1);
	dm->blockNum = 1;
	dm->length = 0;
	dm->curBlock = &dm->block;
	dm->curPos = dm->block.mem;
	dm->curNum = 0;

	dm->block.prior = NULL;
	dm->block.next = NULL;
	dm->block.length = 0;
	dm->block.blockNo = 1;	
	
	return ;
}

void dm_destroy(TDMEM *dm)
{	
	dm->curBlock = dm_getBlock(dm,dm->blockNum);
	for(;dm->blockNum>1;dm->blockNum--)
  {
  	if(dm->curBlock == NULL )
  	{
  		return ;
  	}else
  	{  	
  		dm->curBlock = dm->curBlock->prior;		
  	}
    /*  Begin  Added by ChenMing      2006-10-29 20:31     */
  	if(dm->curBlock == NULL || dm->curBlock->next == NULL)
  	{
  		return ;
  	}  
  	/*  End Added by ChenMing      2006-10-29 20:31     */
		free(dm->curBlock->next);
		dm->curBlock->next = NULL;/* Added by ChenMing 2006-10-29 20:31 */
	}
	dm_create(dm);

	return;
}

TDBLOCK *dm_getBlock(TDMEM *dm,int i){

	if ( i<1 || i> dm->blockNum )
		return NULL;

	dm->curBlock  = &dm->block;
	for(;i>1;i--)
		dm->curBlock = dm->curBlock->next;

	return dm->curBlock;
}

int  dm_freshLength(TDMEM *dm){
	dm->curBlock = &(dm->block);
	dm->length = dm->curBlock->length;
	
	while(dm->curBlock->next){
		dm->curBlock = dm->curBlock->next;
		dm->length += dm->curBlock->length;
	}
	return dm->length;
	/*int i;
	dm->length = 0;

	for(i=0;i<dm->blockNum;i++)
		dm->length += dm_getBlock(dm,i+1)->length;  

	return dm->length;del by weizhao 2005-12-06 ������dm_getblock��ʱ*/
}

TDBLOCK *dm_addBlock(TDMEM *dm){
	TDBLOCK *tdb,*newtdb;
	
	tdb = dm_getBlock(dm,dm->blockNum);
	if (!tdb)
		return NULL;
	newtdb = (TDBLOCK*)malloc(sizeof(TDBLOCK));		
	memset(newtdb,0,sizeof(TDBLOCK));
	tdb->next = newtdb;
	newtdb->prior = tdb;
	newtdb->next = NULL;
	newtdb->blockNo = tdb->blockNo+1;
	newtdb->length = 0;

	dm->blockNum++;
	dm->curBlock = newtdb;
	dm->curPos = (char *)newtdb ;
	dm->curNum = tdb->blockNo*DMEM_SIZE+1;

	return newtdb;
}

void dm_delBlock(TDMEM *dm){
	TDBLOCK *tdb;
	/*���һ���鲻���ͷŶ������³�ʼ��*/
	if ( dm->blockNum == 1){
		dm_create(dm);
	}
	tdb = dm_getBlock(dm,dm->blockNum);
	if (!tdb)
		return ;
	tdb = tdb->prior;
	dm->blockNum--;
	dm->curBlock = tdb;
	dm->curPos = (char *)tdb + tdb->length ;
	dm->curNum = tdb->blockNo*DMEM_SIZE;
	dm->length -= tdb->next->length;
	
	free(tdb->next);
	tdb->next = NULL;
	
	return ;
}

/*��dm��num��ʼ���ַ���ɾ��*/	
int  dm_deldmem(TDMEM *dm,int num){
	TDBLOCK *pBlock;
	int i;
	int blockNum,offset;

	if (num<1)
		return dm->length;

	/*ɾ��num-1������ַ�Ҫ����Щ*/
	blockNum = (num-1) / DMEM_SIZE + 1;
	offset = (num-1) % DMEM_SIZE  ;
	
	if (blockNum<=dm->blockNum){
		/****Modified by SSH,2006.2.9,dm->blockNum�Ƕ�̬�仯��,�ı䷽��
		for(i=0;i<dm->blockNum-blockNum;i++){
			dm_delBlock(dm);
		}
		****/
		while(dm->blockNum-blockNum>0)
			dm_delBlock(dm);
		pBlock = dm_getBlock(dm,blockNum);
		memset((char *)pBlock+offset,0,DMEM_SIZE-offset+1);
		pBlock->length = offset;
		dm_freshLength(dm);
		dm->curBlock = pBlock;
		dm->curPos = (char *)pBlock + offset-1;
		dm->curNum = num;
	} 

	return dm->length;
}

int  dm_dmemcat(TDMEM *dm,char *buf,int size){
	TDBLOCK *pBlock;
	dm_dmemcpy(dm,dm->length+1,buf,size);
	
	return dm->length;
}

int  dm_dinsertmem(TDMEM *dm,int num,char *buf,int size){
	char *p=NULL;
	int pLen ;

	if ( num < 1 || num > dm->length )
		return -1;
	pLen = dm->length - num + size + 1;	
	p = (char *)malloc(pLen+1);	 
	if (!p)
		return -1;
	memset(p,0,pLen+1);
	memcpy(p,buf,size);
	dm_dmemcpy2(p+size,dm,num,dm_dstrlen(dm,num));
	dm_deldmem(dm,num);
	dm_dmemcpy(dm,num,p,pLen);
	
	return dm->length;
}
	

int  dm_dstrlen(TDMEM *dm,int num){
	return dm->length-num+1;
}
/*�Ӹ���dm��i���ֽڿ�ʼ��size���ֽڸ�buf,*/
int  dm_dmemcpy2(char *buf,TDMEM *dm,int num,int size){
	int firstBlock;
	int rtn;
	int totSize;
	int blockNo,offset;
	totSize = 0;
	firstBlock = 1; 

	if ( num<1 || num>dm->length || size<1 )
		return -1;
	blockNo = (num-1)/DMEM_SIZE + 1;
	offset = (num-1)%DMEM_SIZE ;

	if ( size>dm_dstrlen(dm,num) )
		size = dm_dstrlen(dm,num);
	for (;blockNo<=dm->blockNum;blockNo++){
		rtn = dm_dblkcpy(dm,blockNo,offset,buf,size);
		/*
		printf("%s,%d blockno=[%d],offsen=[%d],size=[%d]rtn=[%d]\n",
			__FILE__,__LINE__,blockNo,offset,size,rtn);
		printf("[%s]\n",buf);
		*/
		if (rtn<=0)
			break;
		if (firstBlock){
			offset = 0;
			firstBlock = 0;
		}
		buf = buf+rtn;
		size -= rtn; 
		totSize += rtn; 	
	}	
	
	return totSize;	
}

int  dm_dmemcpy(TDMEM *dm,int num,char *buf,int size){
	int  rtn;
	int  firstBlock;
	int totSize;
	int  blockNo,offset;

	totSize = 0;
	firstBlock = 1;
	blockNo = (num-1)/DMEM_SIZE + 1;
	offset = (num-1)%DMEM_SIZE ;

	   	
	while ( size >0)
	{
  		if (!dm_getBlock(dm,blockNo))
  		{
  		   	dm_addBlock(dm);
  		}
  		rtn = dm_dblkcpy2(buf,dm,blockNo,offset,size);
  		if (!rtn)			break;
  			
  		if (dm_getBlock(dm,blockNo)->length < offset+rtn)
  		{
  			  dm_getBlock(dm,blockNo)->length = offset+rtn;
  		}  
  		
  		dm_freshLength(dm);
  		if (firstBlock)
  		{
          offset=0;
          firstBlock=0;
      }
  
  		blockNo ++;
  		buf += rtn;
  		size -= rtn;
  		totSize += rtn;
	}
	return totSize;
}

int  dm_dblkcpy(TDMEM *dm,int blockNo,int offset,char *buf,int size){
	TDBLOCK *pBlock;

	if ( blockNo>dm->blockNum  || blockNo<1 || size <1)
		return -1;

	pBlock = dm_getBlock(dm,blockNo);
	if (!pBlock) {
		return -1;
	} 

	if ( size >=pBlock->length-offset)
		size = pBlock->length-offset;
	memcpy(buf,(char *)pBlock+offset,size);
	/*buf[size]='\0'; del by weizhao 2005-12-06 ʯ��Ҫ������value���\0*/
	dm->curBlock = pBlock;
	dm->curPos = (char *)pBlock + offset + size - 1;
	dm->curNum = (pBlock->blockNo-1)*DMEM_SIZE + offset + size ; 
	return size;
}

int  dm_dblkcpy2(char *buf,TDMEM *dm,int blockNo,int offset,int size)
{
	TDBLOCK *pBlock;
	if ( blockNo>dm->blockNum  || blockNo<1 || size <1)
		    return -1;
	pBlock = dm_getBlock(dm,blockNo);
	if (!pBlock) {
		return -1;
	} 
	if ( size > DMEM_SIZE-offset )
		  size = DMEM_SIZE-offset;
	if ( size < 1)
		  return -1;
	memcpy((char *)pBlock+offset,buf,size);
	if (pBlock->length <= offset+size )
		  pBlock->length = offset+size; 

	dm->curBlock = pBlock;
	dm->curPos = (char *)pBlock + pBlock->length-1;
	dm->curNum = (pBlock->blockNo-1)*DMEM_SIZE+pBlock->length;
	return size;
}

void dm_show(TDMEM *dm){
	int i,j,k,m;
	char c;
	TDBLOCK *pBlock;
	char *p;
	int num,showNum;
	pBlock = &(dm->block);
	for(i=0;i<dm->blockNum;i++)
	{
		/**
		errLog(LOG_ERR,0,RPT_TO_TTY,(char *)pBlock,dm->block.length,
			"%s,%d block %d/%d (length:%d):",__FILE__,__LINE__,i+1,dm->blockNum,pBlock->length);
		**/
		errLog(LOG_ERR,0,RPT_TO_LOG,(char *)pBlock,dm->block.length,
			"%s,%d block %d/%d (length:%d):",__FILE__,__LINE__,i+1,dm->blockNum,pBlock->length);
		if(pBlock->length<0) break; /* add by LiuHuafeng 20060928 �ȴ�Τ�ѽ�� */
		/**
		pBlock = pBlock->next;
		**/
		vtcp_log("   01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16\t\t----5----0----5-\n");
		num = pBlock->length;
		k = (num-1) / 16 + 1;
		p = (char *)pBlock;
		for ( j=1;j<=k;j++){
			showNum = 16;
			if (j==k)
				showNum = (num-1)%16+1;
			for ( m=0;m<showNum;m++ ){
				c = *(p+m);
			}
			if ( j==k ){
				for( ;m<16;m++ ){
				}
			}
			for ( m=0;m<showNum;m++ ){
				c = *(p+m);
				if ( c=='\0' )
					c = ' ';
			}
			p+=16;
		}
		pBlock = pBlock->next;  /* del by weizhao 20051206 ��errlog��ʾ�ȽϺã���Ϊ���Դ����ļ���.*/
	}
	return ;
}
				
/*ȡ�õ�num���ַ�������0���Ҳ�������NULL*/
char dm_getChar(TDMEM *dm,int num){
	char *p,ret;
	int  bNo,offset;
	if (num<=0 || num >dm->length)
		return NULL;
	bNo = (num-1)/DMEM_SIZE + 1;
	if ( !(p=(char *)dm_getBlock(dm,bNo)) )
		return NULL;
	p += (num-1)%DMEM_SIZE;
	dm->curBlock = dm_getBlock(dm,bNo);
	dm->curPos = p;
	dm->curNum = num;
	ret = *p;
	return ret;
}
/*��dm��numλ��ʼ������buf��ͬ���ַ���*/
int dm_memmem(TDMEM *dm,int num,char *buf,int b_size){
	int i,j;
	void *dmP;
	int flg;
	
	flg = 0;
	if (num<1)
		num = 1;
	if ( num > dm->length ||  b_size <=0 )
		return 0;
	for(; dm->length-num+1>=b_size ; num++ ){
		flg = 1;
		for ( i=0;i<b_size;i++ ){
			if ( dm_getChar(dm,num+i)!=*(buf+i) ){
				flg = 0;
				break;
			} 
		}
		if ( !flg )
			continue;
		else 
			return	num; 
	}
	return 0;
}

int dm_zip0(TDMEM *dm){
	int i,j;

	dm->curBlock = &(dm->block);
	for (i=0;i<dm->blockNum;i++){
		for(j=0;j<dm->curBlock->length;j++){
			if ( dm->curBlock->mem[j]=='\0' )  
				dm->curBlock->mem[j]=' ';
		}
		dm->curBlock = dm->curBlock->next;
	}
}



