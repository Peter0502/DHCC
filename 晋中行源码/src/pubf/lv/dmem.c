/*************************************************
  Copyright (C), 2005, DHC
  File name:     dmem.c
  Author:    韦昭   Version:   V1.0     Date:  2005-8-25
  Description:    分布式存贮模块函数实现。
  Others: 
  Function List:
  1. void dm_create(TDMEM *dm);
		初始化内存块
	2. void dm_destroy(TDMEM *dm);
		消毁内存块
	3. TDBLOCK *dm_getBlock(TDMEM *dm,int i);
		得到第i块信息，成功返回该块指针，失败返回NULL
	4. char dm_getChar(TDMEM *dm,int i);
		取得第i个字符，i!=0，找不到返回NULL
	5. int  dm_freshLength(TDMEM *dm);
		根据每个块的大小整理整个块的大小值，返回块的大小
	6. TDBLOCK *dm_addBlock(TDMEM *dm);
		在所有的块后面增加一个块，返回增加的块的信息
	7. void dm_delBlock(TDMEM *dm);
		删除最后一个块。
	8. int  dm_deldmem(TDMEM *dm,int num);
		把dm第num开始的字符都删除,返回删除后的长度。
	9. int  dm_dmemcat(TDMEM *dm,char *buf,int size);
		在原存贮长度内加入buf，包括0,返回增加后的长度
	10. int  dm_dmemcpy(TDMEM *dm,int num,char *buf,int size);
		与10反之，把buf中的数据考到dm中，从num起,返回考入长度
	11. int  dm_dmemcpy2(char *buf,TDMEM *dm,int num,int size);
		把dm的第num及其后的size个字符复制到buf中，成功返回考入的数据的长度
		因size可能大于num后的长度
	13. int	 dm_memmem(TDMEM *dm,int num,char *buf,int b_size);
		从NUM起开始找与buf b_size个字符相同的块，返回首位置，失败返回0
	14. int  dm_dinsertmem(TDMEM *dm,int num,char *buf,int size);
		在第NUM个位置插入buf的size个字节，成功返回新的长度，失败返回-1
	15. int  dm_dstrlen(TDMEM *dm,int num);
		求num到最后的长度，包换NUM，返回要的长度
	16. int  dm_dblkcpy(TDMEM *dm,int blockNo,in offset,char *buf,int  size);
		与16相反，从dm第blockNo块offset位置拷size个字符到buf
	17. int  dm_dblkcpy2(char *buf,TDMEM *dm,int blockNo,int offset,int size);
		在第blockNo块中offset位置插入buf的size个字符，返回复制的个数
	18. void dm_show(TDMEM *dm);
		打印dm中的所有内容
	19. int  dm_zip0(TDMEM *dm);
		把dm中的0换成空格(小额报文专用)
  History:   
    1. Date:     2005-8-25
       by:   韦昭
       Modification:　建立
    2. Date:		2005-12-6 
    	 by:	 韦昭
    	 Modification:　为提高效率，删除一些不用的变量和减少使用耗时操作
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

	return dm->length;del by weizhao 2005-12-06 反复用dm_getblock耗时*/
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
	/*最后一个块不是释放而是重新初始化*/
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

/*把dm第num开始的字符都删除*/	
int  dm_deldmem(TDMEM *dm,int num){
	TDBLOCK *pBlock;
	int i;
	int blockNum,offset;

	if (num<1)
		return dm->length;

	/*删除num-1后面的字符要方便些*/
	blockNum = (num-1) / DMEM_SIZE + 1;
	offset = (num-1) % DMEM_SIZE  ;
	
	if (blockNum<=dm->blockNum){
		/****Modified by SSH,2006.2.9,dm->blockNum是动态变化的,改变方法
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
/*从复制dm第i个字节开始共size个字节给buf,*/
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
	/*buf[size]='\0'; del by weizhao 2005-12-06 石总要求不能在value后加\0*/
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
		if(pBlock->length<0) break; /* add by LiuHuafeng 20060928 等待韦昭解决 */
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
		pBlock = pBlock->next;  /* del by weizhao 20051206 用errlog显示比较好，因为可以存在文件里.*/
	}
	return ;
}
				
/*取得第num个字符，不算0，找不到返回NULL*/
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
/*从dm的num位开始查找与buf相同的字符串*/
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



