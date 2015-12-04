/*************************************************
  Copyright (C), 2005, DHCC
  File name:     dmem.h
  Author:    Τ��   Version:   V1.0     Date:  2005-8-25
  Description:   �ֲ�ʽ����ģ��
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
		��dm�ĵ�num������size���ַ����Ƶ�buf�У��ɹ����ؿ�������ݵĳ���
		��size���ܴ���num��ĳ���
	11. int  dm_dmemcpy2(char *buf,TDMEM *dm,int num,int size);
		��10��֮����buf�е����ݿ���dm�У���num��,���ؿ��볤��
	13. int	 dm_memmem(TDMEM *dm,int num,char *buf,int b_size);
		��NUM��ʼ����buf b_size���ַ���ͬ�Ŀ飬������λ�ã�ʧ�ܷ���0
	14. int  dm_dinsertmem(TDMEM *dm,int num,char *buf,int size);
		�ڵ�NUM��λ�ò���buf��size���ֽڣ��ɹ������µĳ��ȣ�ʧ�ܷ���-1
	15. int  dm_dstrlen(TDMEM *dm,int num);
		��num�����ĳ��ȣ�����NUM������Ҫ�ĳ���
	16. int  dm_dblkcpy(TDMEM *dm,int blockNo,in offset,char *buf,int  size);
		�ڵ�blockNo����offsetλ�ò���buf��size���ַ������ظ��Ƶĸ���
	17. int  dm_dblkcpy2(char *buf,TDMEM *dm,int blockNo,int offset,int size);
		��16�෴����dm��blockNo��offsetλ�ÿ�size���ַ���buf
	18. void dm_show(TDMEM *dm);
		��ӡdm�е���������
	19. int  dm_zip0(TDMEM *dm);
		��dm�е�0���ɿո�(С���ר��)
  History:   
    1. Date:         2005-8-25
       Author:       Τ��
       Modification: ����
*************************************************/

#ifndef _H_DMEM_
#define _H_DMEM_

#define DMEM_SIZE 1024   /*�ڴ��Ĵ�С*/

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
* ����
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












