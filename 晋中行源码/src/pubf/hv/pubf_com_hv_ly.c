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
/* 函数名称: iGet_br_name(char * or_br_no,char *br_name) */
/* 功能描述: 根据支付系统行号得到(发报/收报)中心代号                        */
/* 创建日期: 2006-8-21 15:11                                                */
/* 作者:     liuyue                                                      */
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
	sprintf(acErrMsg,"行名＝＝[%s]\n",br_name);
	WRITEMSG
	return ret;
}
/*************************************************
 *函数名称: find_qsbrno(char *brno,char *qs_brno)*
 *函数功能: 根据行号获取到对应的清算行号 	 *
 *函数参数: brno----机构号(12位的)               *
 *函数返回: qs_brno---清算中心代码	         *
 *	    0---正常   其他---失败               *
 *          liuyue   20060821 20:24              *
 *************************************************/
int find_qsbrno(char *brno,char *qs_br_no)
{
	int ret = 0;
	struct hv_uniontab_c hv_uniontab;
	char	cOrbrno[13];
	char	cQsbrno[13];
	
	sprintf(acErrMsg,"开始查找[%s]对应的清算行!\n",brno);
	WRITEMSG
	
	MEMSET_DEBUG(&hv_uniontab,0x00,sizeof(hv_uniontab));
	MEMSET_DEBUG(cOrbrno,0x00,sizeof(cOrbrno));
	MEMSET_DEBUG(cQsbrno,0x00,sizeof(cQsbrno));

	MEMCPY_DEBUG(cOrbrno,brno,sizeof(cOrbrno)-1);
	
	if(strlen(cOrbrno)==0) /**为空的话查找本行对应的清算行号***/
	{
		MEMCPY_DEBUG(qs_br_no,HV_QS_BR_NO,strlen(HV_QS_BR_NO));
		sprintf(acErrMsg,"查找到[%s]的清算行 [%s]\n",brno,qs_br_no);
		WRITEMSG
		return 0;
	}
	ret = Hv_uniontab_Sel(g_pub_tx.reply,&hv_uniontab,"or_br_no='%s' ",cOrbrno);
	if(ret)
	{
		sprintf(acErrMsg,"查找清算行失败!ret=[%d]\n",ret);
		WRITEMSG
		return (-1);	
	}
	else
	{
		MEMCPY_DEBUG(cQsbrno,hv_uniontab.qs_no,sizeof(hv_uniontab.qs_no)-1);
		sprintf(acErrMsg,"查找到[%s]的清算行 [%s]\n",brno,cQsbrno);
		WRITEMSG
	}
	MEMCPY_DEBUG(qs_br_no,cQsbrno,sizeof(cQsbrno)-1);
	sprintf(acErrMsg,"查找到[%s]的清算行 [%s]\n",brno,qs_br_no);
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
 *函数名称:	apiaoi    			     ***
 *函数功能:	ST类型类型到INT的转换  		     ***
 *参数说明:     ibuf--被转化的   STR		     ***
 *		ilen--被转化的str的长度              ***
 *******************************************************/
int apatoi( char *ibuf , int len )
{
        char    temp[16];

        MEMSET_DEBUG(temp,0x00,sizeof(temp));
        MEMCPY_DEBUG(temp,ibuf,len);
        return(atoi(temp));
}


/*****************************************************
 *函数名称:iCharacterType							 *	
 *函数功能:判断一个字符串中的字符是否全属于某个集合	 *
 *函数参数:char *buf  待判断的字符串				 *
 *         int  size  字符串的长度					 *
 *		   char symp  集合名称(n,x,g,G)			     *
 *函数返回:0-全部属于  其他-存在不属于  			 *	
 *          liuyue   20060816                  		 *	
 *****************************************************/
int iCharacterType(const char *buf,int size,char symp)
{
	const	int n_buf[] = {48,49,50,51,52,53,54,55,56,57}; /***n字符集合****/
	int ret = 0;
	/***加入   .   20061222*****/
	/* mod by yanqq 20110901 人行的支付系统扩充汉字集 */
	/*const	int x_buf[] = {10,13,32,35,38,42,43,45,46,47,48,49,50,51,52,53,54,55,56,57,59,60,61,62,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122}; ***x字符集合****/
	/* 按煊总指示 加了94 珅字处理 wudawei 20131111 */
  const	int x_buf[] = {10,13,32,33,34,35,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,94,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,124};/***x字符集合****/

	int index = 0;    /**录入的字符串的次序**/

	char *cBuf = (char*)malloc(size+1);
	MEMSET_DEBUG(cBuf,0x00,size+1);
	MEMCPY_DEBUG(cBuf,buf,size);


	vtcp_log("[%s][%d传入的检验数据===[%s]\n",__FILE__,__LINE__,buf);
		
	if(symp =='n')/****如果是n字符集合***/
	{
		while(cBuf[index]!=0x00)
		{
			if(iHalfSearch(n_buf,0,9,(int)cBuf[index]))
			{
				sprintf(acErrMsg,"该字符不存在于字符集合中cBuf[%d]=[%c]\n",index,(char)cBuf[index]);
				WRITEMSG
				return (-1);
			}
			else /***找到了这个字符在字符集中,index++****/
				index++;	
		}
	}
	if(symp =='x')/****如果是x字符集合***/
	{
		
		while(cBuf[index]!=0x00)
		{
			/* mod by yanqq 20110901 扩充汉字集修改结束判断 */
			/*if(iHalfSearch(x_buf,0,76,(int)cBuf[index]))*/
			if(iHalfSearch(x_buf,0,85,(int)cBuf[index]))
			{
				if((int)cBuf[index]==161&&(int)cBuf[index+1]==163)/*。**/
				{
					index = index +2;
					continue;
				}
				else if((int)cBuf[index]==163&&(int)cBuf[index+1]==172)/*，*/
				{
					index = index +2;
					continue;
				}
				else if((int)cBuf[index]==163&&(int)cBuf[index+1]==161)/*！*/
				{
					index = index +2;
					continue;
				}
				else if((int)cBuf[index]==163&&(int)cBuf[index+1]==168)/*（ */
				{
					index = index +2;
					continue;
				}
				else if((int)cBuf[index]==163&&(int)cBuf[index+1]==169)/*）*/
				{
					index = index +2;
					continue;
				}
				else if((int)cBuf[index]==163&&(int)cBuf[index+1]==191)/*？*/
				{
					index = index +2;
					continue;
				}
				else if((int)cBuf[index]==161&&(int)cBuf[index+1]==177)/*双引*/
				{
					index = index +2;
					continue;
				}
				else if((int)cBuf[index]==161&&(int)cBuf[index+1]==175)/*单引*/
				{
					index = index +2;
					continue;
				}
				else
				{
					sprintf(acErrMsg,"该字符不存在于X字符集合中cBuf[%d]=[%d]cBuf[%d]=[%d]\n",index,cBuf[index],index+1,cBuf[index+1]);
					WRITEMSG
					return (-1);/***该字符不存在在字符集中***/
				}
			}
			else /***找到了这个字符在字符集中,index++****/
				index++;	
		}
	}
	if(symp =='G')/****如果是G字符集合***/
	{
		printf("G.cBuf===[%s]\n",buf);
		if(iIsInSet_G(cBuf))
			return (-1);
		if(iIsHalfCharacter(cBuf,strlen(cBuf)))
		{
			vtcp_log("查找半个汉字出现错误!\n");
			return (-1);
		}
	}
	if(symp =='g')/****如果是g字符集合***/TRACE
	{
		
		while(cBuf[index]!=0x00)
		{
			/***暂时不判断是否存在半个汉字了*********/
			vtcp_log("[%s][%d]开始检测字符[%c]===[%d]\n",__FILE__,__LINE__,cBuf[index],(int)cBuf[index]);
			/* mod by yanqq 20110901 扩充汉字集修改结束判断 */
			/*if(iHalfSearch(x_buf,0,76,(int)cBuf[index]))*/
			if(iHalfSearch(x_buf,0,85,(int)cBuf[index]))
			{
				if((int)cBuf[index]<=128)
				{
					sprintf(acErrMsg,"该字符不存在于X字符集合中cBuf[%d]=[%c]\n",index,(char)cBuf[index]);
					WRITEMSG
					vtcp_log("[%s][%d]int)cBuf[%d]=[%d]\n",__FILE__,__LINE__,index,(int)cBuf[index]);
					return (-1);/***该字符不存在在字符集中***/
				}
				else
				{
					index++;/***虽然不在X集合中但是是全角，循环继续*****/TRACE
					continue;TRACE	
				}
			}
			else /***找到了这个字符在字符集中,index++****/
				index++;	
		}
		vtcp_log("字符集合g检测完毕,进入检测是否半个汉字程序!\n");
		ret =iIsHalfCharacter(cBuf,strlen(cBuf)); TRACE
		vtcp_log("半个汉字检测结果[%d]!\n",ret);
		if(iIsHalfCharacter(cBuf,strlen(cBuf)))
		{
			vtcp_log("查找半个汉字出现错误![%d] [%d]\n",__LINE__,ret);
			return (-1);TRACE
		}
		return (0);
	}

}
/*****************************************
 *函数名称: iIsInSet_G					 *
 *函数功能: 判断一个串是否全是由汉字组成 *
 *函数参数: const char *buf  传入字符串  *
 *函数返回: 0-全由汉字组成  其他-不是    *
 *       liuyue     20060816 			 *
 *****************************************/
int iIsInSet_G(const char *buf)/**是否是G字符集合中的字符,全部采用GB-232编码***/
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
			sprintf(acErrMsg,"字符串里面存在半角字符\n");
			WRITEMSG
			return (-1);
		}
	}
	return 0;

}
/********************************************
 *函数名称:iHalfSearch						*
 *函数功能:二分查找							*
 *函数参数:buf   传入的进行查找的串			*
 *		   begin 开始位置					*
 *		   end   结束位置					*
 *         num   需要查找的数值				*	
 *函数返回:0-找到给字符  其他-没有找到     	*
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
				sprintf(acErrMsg,"出错字符位置[%d]，出错字符[%d]!",begin,num);
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
 *函数名称: int iIsHalfCharacter				  *
 *函数功能:判断一个字符串中是否存在半个汉字的情况 *
 *函数参数:char *buff    传入的需要检测的字符串   *
 *		   int   size    buff的长度               *
 *函数返回:0- 不存在   其他-存在半个汉字的情况    *
 *            liuyue  20060816                    *
 **************************************************/
int iIsHalfCharacter(char *buff,int size)
{
    char *cBuf = (char *)malloc(size+1);
    MEMSET_DEBUG(cBuf,0x00,size+1);
    int index = 0;
    int flag = 0;/****一个汉字结束了****/
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
           flag = 0; /***开始了半个字符了***/
            /* rem by LiuHuafeng 20090709 放开到GBK控制模式
          * if((int)cBuf[index+1]>128)
           	******replace by next line********************/
           	 if ((int)cBuf[index+1]>=64 && (int)cBuf[index+1]<=254)
            {
                flag = 1;/***匹配好了一个汉字了**/
                index = index +2 ;
            }            
            
       }
       else
       {
            flag = 1;
            index++;
       }
        	/* rem by LiuHuafeng 20090709 放开到GBK编码控制模式
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
 * 函数名称 :print_notpayin               *
 * 函数功能 :答应payin结构体的各个部分    *
 * 函数参数 :hv_notpayin结构体            * 
 *           ch 选择参数 F T A(打印所有)  *
 * 函数返回 :0                            *
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
	     vtcp_log("hv_notapyin结构体的F变量打印完毕![%s][%d]",__FILE__,__LINE__);
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
	     vtcp_log("hv_notapyin结构体的T变量打印完毕![%s][%d]",__FILE__,__LINE__);

	     break;
	default:
		vtcp_log("payin结构体打印完毕![%s][%d]\n",__FILE__,__LINE__);	
	
   }
}	

/******************************************
 * 函数名称 :print_payin                  *
 * 函数功能 :答应payin结构体的各个部分    *
 * 函数参数 :hv_payin结构体               * 
 *           ch 选择参数 F T A(打印所有)  *
 * 函数返回 :0                            *
 ******************************************/									
int print_payin(HV_PAYIN_AREA *hv_payin)
{
	return 0;
}
/******************************************
 * 函数名称 :print_fd123                  *
 * 函数功能 :答应fd123结构体的各个部分    *
 * 函数参数 :hv_123结构体                 * 
 *           ch 			  *
 * 函数返回 :0                            *
 ******************************************/	
int print_fd123(HV_FD123_AREA *fd123,char ch)
{
	vtcp_log("下面西显示的是关于FD123的内容\n");
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

	vtcp_log("FD123域打印完毕[%s][%d]\n",__FILE__,__LINE__);
	
}								
