/*********************************************************************
* �� �� �� :  
* �������� :  ����ӯ
* ��    �� :  2015/11/26
* ����ģ�� : 	IBPS���˵Ǽ�DETL��
* �������� :  spW226.c
* �������� :
* �����б� :
* ʹ��ע�� :
�������ã�
insert into TX_DEF (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS) values ('5064', 'IBPS���˵Ǽ�detl��', '10000000000000000000110011010011100000111111111110000000000001000111111100000000000110000000100000011000000000000000000000000000', '0', '6', '0');
insert into TX_FLOW_DEF (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('5064', 0, 'W226', '#$');
insert into TX_SUB_DEF (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('W226', 'IBPS���˵Ǽ�DETL��', 'spW226', '0', '0');
***********************************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "public.h"
#include "chnl_tx_detl_c.h"			      /*����֧��������־��*/
char *msstrtok(char *instr, char *outstr, char *delimiter);

int spW226()
{	
	char detl[1024]={0};
	char *str_tmp=detl;
	char temp[100]={0};
	char txtype[6]={0};

	int iRet=0;
		
	struct chnl_tx_detl_c chnl_tx_detl;
	memset(&chnl_tx_detl,0x00,sizeof(chnl_tx_detl));
		
	/**��ʼ�����ýṹ**/
	if (pub_base_sysinit())
	{
		vtcp_log("��ʼ�����ýṹ����!!");
		strcpy(g_pub_tx.reply, "CU06");
		goto ErrExit;
	}	
	get_zd_data("0180",txtype);	 /**��������**/
	if (strlen(txtype) == 0 )
	{
		strcpy(g_pub_tx.reply,"9999");
		set_zd_data("0130","��������δ����!");		
		goto ErrExit;
	}
	get_zd_data("1080",detl);	 /**detl��ϸ�ֶ�**/
  
	if (strlen(detl) == 0 )
	{
		strcpy(g_pub_tx.reply,"9999");
		set_zd_data("0130","��ϸ����δ����!");		
		goto ErrExit;
	}
	vtcp_log("[%s][%d],��ϸ����Ϊ[%s]",__FILE__,__LINE__,detl);
	
	memset(temp, 0x00,sizeof(temp));
	str_tmp=msstrtok(str_tmp,temp,"|");
	chnl_tx_detl.pt_date=atol(temp);
	vtcp_log("[%s][%d]--ƽ̨����=[%ld]",__FILE__,__LINE__,chnl_tx_detl.pt_date);
	
	memset(temp, 0x00,sizeof(temp));
	str_tmp=msstrtok(str_tmp,temp,"|");
	chnl_tx_detl.pt_trace_no=atol(temp);
	vtcp_log("[%s][%d]--ƽ̨��ˮ=[%ld]",__FILE__,__LINE__,chnl_tx_detl.pt_trace_no);
	
	memset(temp, 0x00,sizeof(temp));
	str_tmp=msstrtok(str_tmp,temp,"|");
	chnl_tx_detl.tx_amt=atof(temp);
	vtcp_log("[%s][%d]--���=[%.2f]",__FILE__,__LINE__,chnl_tx_detl.tx_amt);
	
	memset(temp, 0x00,sizeof(temp));
	str_tmp=msstrtok(str_tmp,temp,"|");
	chnl_tx_detl.trace_no=atol(temp);
	vtcp_log("[%s][%d]--������ˮ=[%ld]",__FILE__,__LINE__,chnl_tx_detl.trace_no);
	
	memset(temp, 0x00,sizeof(temp));
	str_tmp=msstrtok(str_tmp,temp,"|");
	strcpy(chnl_tx_detl.or_br_no,temp);
	strcpy(chnl_tx_detl.df_br_no,temp);
	vtcp_log("[%s][%d]--�����к�=[%s]",__FILE__,__LINE__,chnl_tx_detl.or_br_no);
	
  memset(temp, 0x00,sizeof(temp));
	str_tmp=msstrtok(str_tmp,temp,"|");
	strcpy(chnl_tx_detl.df_name,temp);
	vtcp_log("[%s][%d]--�Է�����=[%s]",__FILE__,__LINE__,chnl_tx_detl.df_name);
	
	memset(temp, 0x00,sizeof(temp));
	str_tmp=msstrtok(str_tmp,temp,"|");
	strcpy(chnl_tx_detl.df_ac_no,temp);
	vtcp_log("[%s][%d]--�Է��˺�=[%s]",__FILE__,__LINE__,chnl_tx_detl.df_ac_no);
	
	memset(temp, 0x00,sizeof(temp));
	str_tmp=msstrtok(str_tmp,temp,"|");
	strcpy(chnl_tx_detl.ac_name	,temp);
	vtcp_log("[%s][%d]--����=[%s]",__FILE__,__LINE__,chnl_tx_detl.ac_name);
	
	memset(temp, 0x00,sizeof(temp));
	str_tmp=msstrtok(str_tmp,temp,"|");
	strcpy(chnl_tx_detl.ac_no,temp);
	vtcp_log("[%s][%d]--�˺�=[%s]",__FILE__,__LINE__,chnl_tx_detl.ac_no);
	
	memset(temp, 0x00,sizeof(temp));
	str_tmp=msstrtok(str_tmp,temp,"|");
	strcpy(chnl_tx_detl.filler1,temp);
	vtcp_log("[%s][%d]--����=[%s]",__FILE__,__LINE__,chnl_tx_detl.filler1);
	
	strcpy(chnl_tx_detl.tx_code,"6234");
	strcpy(chnl_tx_detl.syscode,"IBPS");
	strcpy(chnl_tx_detl.chnl_type,"3");
	chnl_tx_detl.tx_date=g_pub_tx.tx_date;
	sprintf(chnl_tx_detl.tx_time,"%6d",g_pub_tx.tx_time);
	chnl_tx_detl.trace_cnt=0;
	chnl_tx_detl.wt_date=g_pub_tx.tx_date;
	strcpy(chnl_tx_detl.orderno,"000000");
	if ( memcmp(txtype,"1012",4)==0 )   /**IBPS��������**/
	{
		strcpy(chnl_tx_detl.sndrcvflag,"SR01");
		strcpy(chnl_tx_detl.filler2,"IBPS��������");
	}
	else if ( memcmp(txtype,"1031",4)==0 ) /**IBPS�������**/
	{
		strcpy(chnl_tx_detl.sndrcvflag,"SR00");
		strcpy(chnl_tx_detl.filler2,"IBPS�������");
	}
	strcpy(chnl_tx_detl.add_ind,"1");
	strcpy(chnl_tx_detl.ct_ind,"2");
	strcpy(chnl_tx_detl.df_br_no,"2");
	strcpy(chnl_tx_detl.dz_ind,"0");
	
	iRet = Chnl_tx_detl_Ins(chnl_tx_detl,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d],�Ǽ�������������ϸ�Ǽǲ�ʧ��",__FILE__,__LINE__);
		Chnl_tx_detl_Debug(&chnl_tx_detl);
		strcpy(g_pub_tx.reply, "D107");
		set_zd_data("0130","�Ǽ�������������ϸ�Ǽǲ�ʧ��!");	
		goto ErrExit;
	}


	OkExit:
		strcpy(g_pub_tx.reply,"0000");
		set_zd_data("0120","0000");
		set_zd_data("0130","IBPS���˵Ǽ�DETL��ɹ�!");
		vtcp_log("success !!: 5064 reply is[%s]", g_pub_tx.reply);
		return 0;
		
	ErrExit:
		vtcp_log("fail !!: 5064 reply is[%s]", g_pub_tx.reply);
		return -1;
}

/**���ܣ�ȡ��instr�е�һ���ָ���delimiterǰ������
������ outstrָ��instr�е�һ��delimiter֮ǰ������
����ֵ��������һ��delimiter��λ��***************/
char * strstr(char *,char *);
char *msstrtok(char *instr, char *outstr, char *delimiter)
{
  char *tmpstr;
  if(memcmp(instr, delimiter, strlen(instr))==0){
    return(NULL);
  }
  if(instr==NULL || strlen(instr)==0){return(NULL);}
  tmpstr=strstr(instr,delimiter);
  if(tmpstr!=NULL){
    memcpy(outstr, instr, strlen(instr)-strlen(tmpstr));
    return(strstr(instr,delimiter)+strlen(delimiter));
  }else{
    memcpy(outstr, instr, strlen(instr));
    return(NULL);
  }
}