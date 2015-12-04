#include <stdlib.h>
#include <stdio.h> 
#include <math.h>
#define EXTERN
#include "hvpack.h"
#include "hv_uniontab_c.h"
#include <sys/types.h>
#include <time.h>
#include <math.h>
#include "string.h"
#include "find_debug.h"



/****************************************************************************/
/* ��������: iGet_br_name(char * or_br_no,char *br_name) */
/* ��������: ����֧��ϵͳ�кŵõ�(����/�ձ�)���Ĵ���                        */
/* ��������: 2006-8-21 15:11                                                */
/* ����:     liuyue                                                      */
/****************************************************************************/ 
int iGet_br_name(char *or_br_no, char *br_name)
{
	int ret=0;
	struct hv_uniontab_c hv_uniontab;    
	char cOrbrno[13];
	MEMSET_DEBUG(cOrbrno, 0 , sizeof(cOrbrno));
	MEMCPY_DEBUG(cOrbrno,or_br_no,sizeof(cOrbrno)-1);
	zip_space(cOrbrno);
	if  (strlen(cOrbrno) == 0 )
		return 1;
	MEMSET_DEBUG( &hv_uniontab, 0x00, sizeof(hv_uniontab) );    
	ret = Hv_uniontab_Sel( g_pub_tx.reply, &hv_uniontab, "or_br_no = '%s' ", cOrbrno );
	if(ret)
	{
		sprintf(acErrMsg,"find hvuniontab error ret=[%d] or_brno=[%s]",ret,or_br_no);
		WRITEMSG
		return(ret);
	}
	MEMCPY_DEBUG(br_name,hv_uniontab.br_name_f,sizeof(hv_uniontab.br_name_f)-1);
	sprintf(acErrMsg,"��������[%s]\n",br_name);
	WRITEMSG
	return ret;
}
/*************************************************
 *��������: find_qsbrno(char *brno,char *qs_brno)*
 *��������: �����кŻ�ȡ����Ӧ�������к� 	 *
 *��������: brno----������(12λ��)               *
 *��������: qs_brno---�������Ĵ���	         *
 *	    0---����   ����---ʧ��               *
 *          liuyue   20060821 20:24              *
 *************************************************/
int find_qsbrno(char *brno,char *qs_br_no)
{
	int ret = 0;
	struct hv_uniontab_c hv_uniontab;
	char	cOrbrno[13];
	char	cQsbrno[13];
	
	sprintf(acErrMsg,"��ʼ����[%s]��Ӧ��������!\n",brno);
	WRITEMSG
	
	MEMSET_DEBUG(&hv_uniontab,0x00,sizeof(hv_uniontab));
	MEMSET_DEBUG(cOrbrno,0x00,sizeof(cOrbrno));
	MEMSET_DEBUG(cQsbrno,0x00,sizeof(cQsbrno));

	MEMCPY_DEBUG(cOrbrno,brno,sizeof(cOrbrno)-1);
	
	if(strlen(cOrbrno)==0) /**Ϊ�յĻ����ұ��ж�Ӧ�������к�***/
	{
		MEMCPY_DEBUG(qs_br_no,HV_QS_BR_NO,strlen(HV_QS_BR_NO));
		sprintf(acErrMsg,"���ҵ�[%s]�������� [%s]\n",brno,qs_br_no);
		WRITEMSG
		return 0;
	}
	ret = Hv_uniontab_Sel(g_pub_tx.reply,&hv_uniontab,"or_br_no='%s' ",cOrbrno);
	if(ret)
	{
		sprintf(acErrMsg,"����������ʧ��!ret=[%d]\n",ret);
		WRITEMSG
		return (-1);	
	}
	else
	{
		MEMCPY_DEBUG(cQsbrno,hv_uniontab.qs_no,sizeof(hv_uniontab.qs_no)-1);
		sprintf(acErrMsg,"���ҵ�[%s]�������� [%s]\n",brno,cQsbrno);
		WRITEMSG
	}
	MEMCPY_DEBUG(qs_br_no,cQsbrno,sizeof(cQsbrno)-1);
	sprintf(acErrMsg,"���ҵ�[%s]�������� [%s]\n",brno,qs_br_no);
	WRITEMSG
	return 0;
}
         
/****************************************************************************/
/*  OBJECT :  string  to  double                                            */
/*  INPUT  :  ibuf      ->  456123                                          */
/*            ibuflen   ->  6                                               */
/*            idot      ->  3      express string = 456.123                 */
/*  OUTPUT :  obuf                                                          */
/*  RETURN :  0         ->  sucessed                                        */
/*            -1        ->  failed                                          */
/****************************************************************************/
int str2dbl( ibuf,ibuflen,idot,obuf )
char  *ibuf       ;
short ibuflen     ;
short idot        ;
double *obuf       ;
{
	char lstrbuf[36];
	short loopi ;
	MEMSET_DEBUG(lstrbuf, 0 , sizeof(lstrbuf));
	/*********** chech input data ****************/
	if( strlen(ibuf) < ibuflen )
		return -1;
	if( strlen(ibuf) < idot )
		return -1;
	if( idot > ibuflen )
		return -1;
	/********** add dot to string *****************/
	for( loopi=0;loopi<ibuflen+1;loopi++ )
	{
		if( loopi == (ibuflen-idot) )
			lstrbuf[loopi] = '.' ;
		else
			lstrbuf[loopi] = *ibuf++ ;
	}
	lstrbuf[loopi] = 0x00 ;
	/********* string to decimal *********************/
	*obuf=atof(lstrbuf);
	return 0;
}
/*******************************************************
 *                   FUNC 3       		     ***
 *��������:	apiaoi    			     ***
 *��������:	ST�������͵�INT��ת��  		     ***
 *����˵��:     ibuf--��ת����   STR		     ***
 *		ilen--��ת����str�ĳ���              ***
 *******************************************************/
int apatoi( char *ibuf , int len )
{
        char    temp[16];

        MEMSET_DEBUG(temp,0x00,sizeof(temp));
        MEMCPY_DEBUG(temp,ibuf,len);
        return(atoi(temp));
}


/*****************************************************
 *��������:iCharacterType							 *	
 *��������:�ж�һ���ַ����е��ַ��Ƿ�ȫ����ĳ������	 *
 *��������:char *buf  ���жϵ��ַ���				 *
 *         int  size  �ַ����ĳ���					 *
 *		   char symp  ��������(n,x,g,G)			     *
 *��������:0-ȫ������  ����-���ڲ�����  			 *	
 *          liuyue   20060816                  		 *	
 *****************************************************/
int iCharacterType(const char *buf,int size,char symp)
{
	const	int n_buf[] = {48,49,50,51,52,53,54,55,56,57}; /***n�ַ�����****/
	int ret = 0;
	/***����   .   20061222*****/
	/* mod by yanqq 20110901 ���е�֧��ϵͳ���人�ּ� */
	/*const	int x_buf[] = {10,13,32,35,38,42,43,45,46,47,48,49,50,51,52,53,54,55,56,57,59,60,61,62,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122}; ***x�ַ�����****/
	/* ������ָʾ ����94 �|�ִ��� wudawei 20131111 */
  const	int x_buf[] = {10,13,32,33,34,35,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,94,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,124};/***x�ַ�����****/

	int index = 0;    /**¼����ַ����Ĵ���**/

	char *cBuf = (char*)malloc(size+1);
	MEMSET_DEBUG(cBuf,0x00,size+1);
	MEMCPY_DEBUG(cBuf,buf,size);


	vtcp_log("[%s][%d����ļ�������===[%s]\n",__FILE__,__LINE__,buf);
		
	if(symp =='n')/****�����n�ַ�����***/
	{
		while(cBuf[index]!=0x00)
		{
			if(iHalfSearch(n_buf,0,9,(int)cBuf[index]))
			{
				sprintf(acErrMsg,"���ַ����������ַ�������cBuf[%d]=[%c]\n",index,(char)cBuf[index]);
				WRITEMSG
				return (-1);
			}
			else /***�ҵ�������ַ����ַ�����,index++****/
				index++;	
		}
	}
	if(symp =='x')/****�����x�ַ�����***/
	{
		
		while(cBuf[index]!=0x00)
		{
			/* mod by yanqq 20110901 ���人�ּ��޸Ľ����ж� */
			/*if(iHalfSearch(x_buf,0,76,(int)cBuf[index]))*/
			if(iHalfSearch(x_buf,0,85,(int)cBuf[index]))
			{
				if((int)cBuf[index]==161&&(int)cBuf[index+1]==163)/*��**/
				{
					index = index +2;
					continue;
				}
				else if((int)cBuf[index]==163&&(int)cBuf[index+1]==172)/*��*/
				{
					index = index +2;
					continue;
				}
				else if((int)cBuf[index]==163&&(int)cBuf[index+1]==161)/*��*/
				{
					index = index +2;
					continue;
				}
				else if((int)cBuf[index]==163&&(int)cBuf[index+1]==168)/*�� */
				{
					index = index +2;
					continue;
				}
				else if((int)cBuf[index]==163&&(int)cBuf[index+1]==169)/*��*/
				{
					index = index +2;
					continue;
				}
				else if((int)cBuf[index]==163&&(int)cBuf[index+1]==191)/*��*/
				{
					index = index +2;
					continue;
				}
				else if((int)cBuf[index]==161&&(int)cBuf[index+1]==177)/*˫��*/
				{
					index = index +2;
					continue;
				}
				else if((int)cBuf[index]==161&&(int)cBuf[index+1]==175)/*����*/
				{
					index = index +2;
					continue;
				}
				else
				{
					sprintf(acErrMsg,"���ַ���������X�ַ�������cBuf[%d]=[%d]cBuf[%d]=[%d]\n",index,cBuf[index],index+1,cBuf[index+1]);
					WRITEMSG
					return (-1);/***���ַ����������ַ�����***/
				}
			}
			else /***�ҵ�������ַ����ַ�����,index++****/
				index++;	
		}
	}
	if(symp =='G')/****�����G�ַ�����***/
	{
		printf("G.cBuf===[%s]\n",buf);
		if(iIsInSet_G(cBuf))
			return (-1);
		if(iIsHalfCharacter(cBuf,strlen(cBuf)))
		{
			vtcp_log("���Ұ�����ֳ��ִ���!\n");
			return (-1);
		}
	}
	if(symp =='g')/****�����g�ַ�����***/TRACE
	{
		
		while(cBuf[index]!=0x00)
		{
			/***��ʱ���ж��Ƿ���ڰ��������*********/
			vtcp_log("[%s][%d]��ʼ����ַ�[%c]===[%d]\n",__FILE__,__LINE__,cBuf[index],(int)cBuf[index]);
			/* mod by yanqq 20110901 ���人�ּ��޸Ľ����ж� */
			/*if(iHalfSearch(x_buf,0,76,(int)cBuf[index]))*/
			if(iHalfSearch(x_buf,0,85,(int)cBuf[index]))
			{
				if((int)cBuf[index]<=128)
				{
					sprintf(acErrMsg,"���ַ���������X�ַ�������cBuf[%d]=[%c]\n",index,(char)cBuf[index]);
					WRITEMSG
					vtcp_log("[%s][%d]int)cBuf[%d]=[%d]\n",__FILE__,__LINE__,index,(int)cBuf[index]);
					return (-1);/***���ַ����������ַ�����***/
				}
				else
				{
					index++;/***��Ȼ����X�����е�����ȫ�ǣ�ѭ������*****/TRACE
					continue;TRACE	
				}
			}
			else /***�ҵ�������ַ����ַ�����,index++****/
				index++;	
		}
		vtcp_log("�ַ�����g������,�������Ƿ������ֳ���!\n");
		ret =iIsHalfCharacter(cBuf,strlen(cBuf)); TRACE
		vtcp_log("������ּ����[%d]!\n",ret);
		if(iIsHalfCharacter(cBuf,strlen(cBuf)))
		{
			vtcp_log("���Ұ�����ֳ��ִ���![%d] [%d]\n",__LINE__,ret);
			return (-1);TRACE
		}
		return (0);
	}

}
/*****************************************
 *��������: iIsInSet_G					 *
 *��������: �ж�һ�����Ƿ�ȫ���ɺ������ *
 *��������: const char *buf  �����ַ���  *
 *��������: 0-ȫ�ɺ������  ����-����    *
 *       liuyue     20060816 			 *
 *****************************************/
int iIsInSet_G(const char *buf)/**�Ƿ���G�ַ������е��ַ�,ȫ������GB-232����***/
{
	char *cBuf = (char*)malloc(strlen(buf));
	int index = 0;

	MEMCPY_DEBUG(cBuf,buf,strlen(buf));
	printf("cBuf===[%s]\n",cBuf);
	
	while(buf[index]!=0x00)
	{
		if((int)buf[index]>128)
			index++;
		else
		{
			sprintf(acErrMsg,"�ַ���������ڰ���ַ�\n");
			WRITEMSG
			return (-1);
		}
	}
	return 0;

}
/********************************************
 *��������:iHalfSearch						*
 *��������:���ֲ���							*
 *��������:buf   ����Ľ��в��ҵĴ�			*
 *		   begin ��ʼλ��					*
 *		   end   ����λ��					*
 *         num   ��Ҫ���ҵ���ֵ				*	
 *��������:0-�ҵ����ַ�  ����-û���ҵ�     	*
 *           liuyue    20060816  			*
 ********************************************/	
int iHalfSearch(const int *buf,int begin,int end,int num)
{
		int mid   = 0;
		mid		  =(int)(begin+end)/2;
		if(begin==end)
		{
			if(num==buf[begin])
				return 0;
			else
			{
				sprintf(acErrMsg,"�����ַ�λ��[%d]�������ַ�[%d]!",begin,num);
				WRITEMSG
				return (-1);
			}
		}
		if(num>buf[mid])
		{
			return	iHalfSearch(buf,mid+1,end,num);
		}
		if(num<buf[mid])
		{
			return	iHalfSearch(buf,begin,mid,num);
		}
		if(num=buf[mid])
		{	
			return 0;
		}
		
}
/**************************************************
 *��������: int iIsHalfCharacter				  *
 *��������:�ж�һ���ַ������Ƿ���ڰ�����ֵ���� *
 *��������:char *buff    �������Ҫ�����ַ���   *
 *		   int   size    buff�ĳ���               *
 *��������:0- ������   ����-���ڰ�����ֵ����    *
 *            liuyue  20060816                    *
 **************************************************/
int iIsHalfCharacter(char *buff,int size)
{
    char *cBuf = (char *)malloc(size+1);
    MEMSET_DEBUG(cBuf,0x00,size+1);
    int index = 0;
    int flag = 0;/****һ�����ֽ�����****/
    MEMCPY_DEBUG(cBuf,buff,size);
/*
    vtcp_log("%s,%d++++++++++++++cBuf=[%s]++++++++\n",__FILE__,__LINE__,cBuf);
*/
    while(cBuf[index]!=0x00)
    {
/*
       vtcp_log("++[%s,%d]++++cBuf[%d]=[%d]++++\n",__FILE__,__LINE__,index,(int)cBuf[index]);
*/
       if((int)cBuf[index]>128)
       {
           flag = 0; /***��ʼ�˰���ַ���***/
            /* rem by LiuHuafeng 20090709 �ſ���GBK����ģʽ
          * if((int)cBuf[index+1]>128)
           	******replace by next line********************/
           	 if ((int)cBuf[index+1]>=64 && (int)cBuf[index+1]<=254)
            {
                flag = 1;/***ƥ�����һ��������**/
                index = index +2 ;
            }            
            
       }
       else
       {
            flag = 1;
            index++;
       }
        	/* rem by LiuHuafeng 20090709 �ſ���GBK�������ģʽ
               *if (flag==0&&(int)cBuf[index+1]<=128)
               ****** replace by next line ****/
                if (flag==0&&((int)cBuf[index+1]<64 || (int) cBuf[index+1]>254))
        {
/*
            vtcp_log("%s,%d++cBuf[%d]=[%d]\n",__FILE__,__LINE__,index+1,(int)cBuf[index+1]);	
            vtcp_log("[%s,%d]! i am here!+++++++",__FILE__,__LINE__); 
*/          
            free(cBuf); /* Added by ChenMing  2006-10-30 15:57 */
            return index+1; 
        }
     }
     free(cBuf);
     return 0;
}
/******************************************
 * �������� :print_notpayin               *
 * �������� :��Ӧpayin�ṹ��ĸ�������    *
 * �������� :hv_notpayin�ṹ��            * 
 *           ch ѡ����� F T A(��ӡ����)  *
 * �������� :0                            *
 ******************************************/									
int print_notpay(HV_NOTPAY_AREA *hv_notpayin,char ch)
{
	switch(ch)
	{
	  case 'A':
	  case 'F':
	     vtcp_log("hv_notpayin->F_opcd=[%c]\n",hv_notpayin->F_opcd[0]);
	     vtcp_log("hv_notpayin->F_lw_ind=[%c]\n",hv_notpayin->F_lw_ind[0]);
	     vtcp_log("hv_notpayin->F_tx_br_no=[%7s]\n",hv_notpayin->F_tx_br_no);
	     vtcp_log("hv_notpayin->F_tx_date=[%8s]\n",hv_notpayin->F_tx_date);
	     vtcp_log("hv_notpayin->F_wt_date=[%8s]\n",hv_notpayin->F_wt_date);
	     vtcp_log("hv_notpayin->F_cmtno=[%3s]\n",hv_notpayin->F_cmtno);
	     vtcp_log("hv_notpayin->F_or_br_no=[%12s]\n",hv_notpayin->F_or_br_no);
	     vtcp_log("hv_notpayin->F_ac_br_no=[%12s]\n",hv_notpayin->F_ac_br_no);
	     vtcp_log("hv_notpayin->F_orderno=[%8s]\n",hv_notpayin->F_orderno);

	     vtcp_log("hv_notpayin->F_actno=[%32s]\n",hv_notpayin->F_actno);
	     vtcp_log("hv_notpayin->F_name=[%60s]\n",hv_notpayin->F_name);
	     vtcp_log("hv_notpayin->F_content=[%255s]\n",hv_notpayin->F_content);
	     vtcp_log("hv_notpayin->F_hv_brf=[%60s]\n",hv_notpayin->F_hv_brf);
	     vtcp_log("hv_notpayin->F_tel=[%6s]\n",hv_notpayin->F_tel);
	     vtcp_log("hv_notpayin->F_respcode=[%c]\n",hv_notpayin->F_respcode[0]);
	     vtcp_log("hv_notpayin->F_o_payin_orderno=[%8s]\n",hv_notpayin->F_o_payin_orderno);
	     vtcp_log("hv_notpayin->F_o_notpay_orderno=[%8s]\n",hv_notpayin->F_o_notpay_orderno);
	     vtcp_log("hv_notpayin->F_o_cur_no=[%3s]\n",hv_notpayin->F_o_cur_no);
	     vtcp_log("hv_notpayin->F_o_txamt=[%15s]\n",hv_notpayin->F_o_txamt);
	     vtcp_log("hv_notpayin->F_o_tx_type=[%c]\n",hv_notpayin->F_o_tx_type);
	     vtcp_log("hv_notpayin->F_o_wt_date=[%8s]\n",hv_notpayin->F_o_wt_date);
	     vtcp_log("hv_notpayin->F_o_fzf_date=[%8s]\n",hv_notpayin->F_o_fzf_date);
	     vtcp_log("hv_notpayin->F_o_ac_br_no=[%12s]\n",hv_notpayin->F_o_ac_br_no);
	     vtcp_log("hv_notpayin->F_o_or_br_no=[%12s]\n",hv_notpayin->F_o_or_br_no);
	     vtcp_log("hv_notpayin->F_o_cmtno=[%3s]\n",hv_notpayin->F_o_cmtno);
	     vtcp_log("hv_notpayin->F_beg_sts=[%c]\n",hv_notpayin->F_beg_sts[0]);
	     vtcp_log("hv_notpayin->F_rcpstat=[%2s]\n",hv_notpayin->F_rcpstat);
	     vtcp_log("hv_notpayin->F_refno=[%8s]\n",hv_notpayin->F_refno);
	     vtcp_log("hv_notpayin->F_resp1=[%8s]\n",hv_notpayin->F_resp1);
	     vtcp_log("hv_notapyin�ṹ���F������ӡ���![%s][%d]",__FILE__,__LINE__);
	     if(ch=='F')
		break;
	case 'T':
	     vtcp_log("hv_notpayin.T_or_br_no=[%12s]\n",hv_notpayin->T_or_br_no);
	     vtcp_log("hv_notpayin.T_orderno=[%8s]\n",hv_notpayin->T_orderno); 		
	     vtcp_log("hv_notpayin.T_tx_date=[%8s]\n",hv_notpayin->T_tx_date); 		
	     vtcp_log("hv_notpayin.T_wt_date=[%8s]\n",hv_notpayin->T_wt_date); 		
	     vtcp_log("hv_notpayin.T_br_no=[%7s]\n",hv_notpayin->T_br_no); 		
	     vtcp_log("hv_notpayin.T_hv_fzf_sts=[%c]\n",hv_notpayin->T_hv_fzf_sts[0]); 		
	     vtcp_log("hv_notpayin.T_cmtno=[%3s]\n",hv_notpayin->T_cmtno); 		
	     vtcp_log("hv_notpayin.T_tx_time=[%8s]\n",hv_notpayin->T_tx_time); 		
	     vtcp_log("hv_notpayin.T_trace_no=[%8s]\n",hv_notpayin->T_trace_no); 		
	     vtcp_log("hv_notpayin.T_pay_qs_no=[%12s]\n",hv_notpayin->T_pay_qs_no); 		
	     vtcp_log("hv_notpayin.T_ac_br_no=[%12s]\n",hv_notpayin->T_ac_br_no); 		
	     vtcp_log("hv_notpayin.T_cash_qs_no=[%12s]\n",hv_notpayin->T_cash_qs_no); 		
	     vtcp_log("hv_notpayin.T_o_wt_date=[%8s]\n",hv_notpayin->T_o_wt_date); 		
	     vtcp_log("hv_notpayin.T_o_or_br_no=[%12s]\n",hv_notpayin->T_o_or_br_no); 		
	     vtcp_log("hv_notpayin.T_o_ac_br_no=[%12s]\n",hv_notpayin->T_o_ac_br_no); 		
	     vtcp_log("hv_notpayin.T_o_payin_orderno=[%8s]\n",hv_notpayin->T_o_payin_orderno); 		
	     vtcp_log("hv_notpayin.T_o_cur_no=[%3s]\n",hv_notpayin->T_o_cur_no); 		
	     vtcp_log("hv_notpayin.T_o_tx_amt=[%16s]\n",hv_notpayin->T_o_tx_amt); 		
	     vtcp_log("hv_notpayin.T_o_tx_type=[%c]\n",hv_notpayin->T_o_tx_type[0]); 		
	     vtcp_log("hv_notpayin.T_o_cmtno=[%3s]\n",hv_notpayin->T_o_cmtno); 		
	     vtcp_log("hv_notpayin.T_sendco=[%4s]\n",hv_notpayin->T_sendco); 		
	     vtcp_log("hv_notpayin.T_receco=[%4s]\n",hv_notpayin->T_receco); 		
	     vtcp_log("hv_notpayin.T_req_date=[%8s]\n",hv_notpayin->T_req_date); 		
	     vtcp_log("hv_notpayin.T_req_br_no=[%12s]\n",hv_notpayin->T_req_br_no); 		
	     vtcp_log("hv_notpayin.T_req_content=[%255s]\n",hv_notpayin->T_req_content); 		
	     vtcp_log("hv_notpayin.T_req_orderno=[%8s]\n",hv_notpayin->T_req_orderno); 		
	     vtcp_log("hv_notpayin.T_es_date=[%8s]\n",hv_notpayin->T_res_date); 		
	     vtcp_log("hv_notpayin.T_res_br_no=[%12s]\n",hv_notpayin->T_res_br_no); 		
	     vtcp_log("hv_notpayin.T_res_content=[%255s]\n",hv_notpayin->T_res_content); 		
	     vtcp_log("hv_notpayin.T_res_orderno=[%8s]\n",hv_notpayin->T_res_orderno); 		
	     vtcp_log("hv_notpayin.T_respcode=[%c]\n",hv_notpayin->T_respcode[0]); 		
	     vtcp_log("hv_notpayin.T_addid=[%8s]\n",hv_notpayin->T_addid); 		
	     vtcp_log("hv_notpayin.T_refno=[%20s]\n",hv_notpayin->T_refno); 		
	     vtcp_log("hv_notpayin.T_hv_prt_ind=[%10s]\n",hv_notpayin->T_hv_prt_ind); 		
	     vtcp_log("hv_notpayin.T_tel=[%6s]\n",hv_notpayin->T_tel); 		
	     vtcp_log("hv_notpayin.T_hv_brf=[%60s]\n",hv_notpayin->T_hv_brf); 		
	     vtcp_log("hv_notpayin.T_lw_ind=[%c]\n",hv_notpayin->T_lw_ind[0]);
	     vtcp_log("hv_notapyin�ṹ���T������ӡ���![%s][%d]",__FILE__,__LINE__);

	     break;
	default:
		vtcp_log("payin�ṹ���ӡ���![%s][%d]\n",__FILE__,__LINE__);	
	
   }
}	

/******************************************
 * �������� :print_payin                  *
 * �������� :��Ӧpayin�ṹ��ĸ�������    *
 * �������� :hv_payin�ṹ��               * 
 *           ch ѡ����� F T A(��ӡ����)  *
 * �������� :0                            *
 ******************************************/									
int print_payin(HV_PAYIN_AREA *hv_payin)
{
	return 0;
}
/******************************************
 * �������� :print_fd123                  *
 * �������� :��Ӧfd123�ṹ��ĸ�������    *
 * �������� :hv_123�ṹ��                 * 
 *           ch 			  *
 * �������� :0                            *
 ******************************************/	
int print_fd123(HV_FD123_AREA *fd123,char ch)
{
	vtcp_log("��������ʾ���ǹ���FD123������\n");
	vtcp_log("cmtno=[%3s]\n",fd123->cmtno);
	vtcp_log("o_cmtno=[%3s]\n",fd123->o_cmtno);
	vtcp_log("pay_qs_no=[%12s]\n",fd123->pay_qs_no);
	vtcp_log("or_br_no=[%12s]\n",fd123->or_br_no);
	vtcp_log("sendco=[%4s]\n",fd123->sendco);
	vtcp_log("pay_opn_br_no=[%12s]\n",fd123->pay_opn_br_no);
	vtcp_log("pay_ac_no=[%12s]\n",fd123->pay_ac_no);
	vtcp_log("pay_name=[%60s]\n",fd123->pay_name);
	vtcp_log("pay_addr=[%60s]\n",fd123->pay_addr);
	vtcp_log("cash_qs_no=[%12s]\n",fd123->cash_qs_no);
	vtcp_log("ac_br_no=[%12s]\n",fd123->ac_br_no);
	vtcp_log("receco=[%4s]\n",fd123->receco);
	vtcp_log("cash_opn_br_no=[%12s]\n",fd123->cash_opn_br_no);
	vtcp_log("cash_ac_no=[%12s]\n",fd123->cash_ac_no);
	vtcp_log("cash_name=[%60s]\n",fd123->cash_name);
	vtcp_log("cash_addr=[%60s]\n",fd123->cash_addr);
	vtcp_log("yw_type=[%2s]\n",fd123->yw_type);
	vtcp_log("orderno=[%8s]\n",fd123->orderno);
	vtcp_log("brf=[%60s]\n",fd123->brf);
	vtcp_log("rate=[%7s]\n",fd123->rate);
	vtcp_log("ratelimit=[%5s]\n",fd123->ratelimit);
	vtcp_log("o_orderno=[%8s]\n",fd123->o_orderno);
	vtcp_log("procode=[%8s]\n",fd123->procode);
	vtcp_log("cr_date=[%8s]\n",fd123->cr_date);
	vtcp_log("cr_br_no=[%12s]\n",fd123->cr_br_no);
	vtcp_log("cr_tx_num=[%8s]\n",fd123->cr_tx_num);
	vtcp_log("qr_br_no=[%12s]\n",fd123->qr_br_no);
	vtcp_log("qr_tx_num=[%8s]\n",fd123->qr_tx_num);
	vtcp_log("tx_type=[%c]\n",fd123->tx_type[0]);
	vtcp_log("notpay_content=[%255s]\n",fd123->notpay_content);
	vtcp_log("notpay_orderno=[%255s]\n",fd123->notpay_orderno);
	vtcp_log("resp=[%c]\n",fd123->resp[0]);
	vtcp_log("operlevel=[%c]\n",fd123->operlevel[0]);
	vtcp_log("proc_sts=[%c]\n",fd123->proc_sts[0]);

	vtcp_log("FD123���ӡ���[%s][%d]\n",__FILE__,__LINE__);
	
}								
