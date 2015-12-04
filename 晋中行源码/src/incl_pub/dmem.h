/*************************************************
  Copyright (C), 2005, DHCC
  File name:     dmem.h
  Author:    韦昭   Version:   V1.0     Date:  2005-8-25
  Description:   分布式存贮模块
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
		把dm的第num及其后的size个字符复制到buf中，成功返回考入的数据的长度
		因size可能大于num后的长度
	11. int  dm_dmemcpy2(char *buf,TDMEM *dm,int num,int size);
		与10反之，把buf中的数据考到dm中，从num起,返回考入长度
	13. int	 dm_memmem(TDMEM *dm,int num,char *buf,int b_size);
		从NUM起开始找与buf b_size个字符相同的块，返回首位置，失败返回0
	14. int  dm_dinsertmem(TDMEM *dm,int num,char *buf,int size);
		在第NUM个位置插入buf的size个字节，成功返回新的长度，失败返回-1
	15. int  dm_dstrlen(TDMEM *dm,int num);
		求num到最后的长度，包换NUM，返回要的长度
	16. int  dm_dblkcpy(TDMEM *dm,int blockNo,in offset,char *buf,int  size);
		在第blockNo块中offset位置插入buf的size个字符，返回复制的个数
	17. int  dm_dblkcpy2(char *buf,TDMEM *dm,int blockNo,int offset,int size);
		与16相反，从dm第blockNo块offset位置拷size个字符到buf
	18. void dm_show(TDMEM *dm);
		打印dm中的所有内容
	19. int  dm_zip0(TDMEM *dm);
		把dm中的0换成空格(小额报文专用)
  History:   
    1. Date:         2005-8-25
       Author:       韦昭
       Modification: 建立
*************************************************/

#ifndef _H_DMEM_
#define _H_DMEM_

#define DMEM_SIZE 1024   /*内存块的大小*/

typedef struct t_dblock
{
    char mem[DMEM_SIZE+1] ;
    struct t_dblock *prior;
    struct t_dblock *next;
    unsigned int blockNo;
    unsigned int length;
}TDBLOCK;

typedef struct T_DMEM
{
	TDBLOCK block; 
	unsigned int  blockNum;
	unsigned int  length;
	struct t_dblock *curBlock;
	char *curPos;
	unsigned int  curNum;
}TDMEM;


/*******************************************
* 函数
*******************************************/

void dm_create(TDMEM *dm);		
void dm_destroy(TDMEM *dm);
TDBLOCK *dm_getBlock(TDMEM *dm,int i);
char dm_getChar(TDMEM *dm,int i);
int  dm_freshLength(TDMEM *dm);
TDBLOCK *dm_addBlock(TDMEM *dm);
void dm_delBlock(TDMEM *dm);
int  dm_deldmem(TDMEM *dm,int num);
int  dm_dmemcat(TDMEM *dm,char *buf,int size);
int  dm_dmemcpy(TDMEM *dm,int num,char *buf,int size);
int  dm_dmemcpy2(char *buf,TDMEM *dm,int num,int size);
int  dm_dmemcat(TDMEM *dm,char *buf,int size);
int  dm_memmem(TDMEM *dm,int num,char *buf,int b_size);
int  dm_dinsertmem(TDMEM *dm,int num,char *buf,int size);
int  dm_dstrlen(TDMEM *dm,int num);
int  dm_dblkcpy(TDMEM *dm,int blockNo,int offset,char *buf,int  size);
int  dm_dblkcpy2(char *buf,TDMEM *dm,int blockNo,int offset,int size);
void dm_show(TDMEM *dm);
int  dm_zip0(TDMEM *dm);

#endif












