/*************************************************************
* �� �� ��: spD091.c
* ��������: ����֧���������

* ��    ��: ���Ĳ�
* �������: 2014/9/28

* �޸ļ�¼:
* ��    ��:
* �� �� ��:
* ��    ע:
**************************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "fn_parm_c.h"
#include "fn_ac_rel_c.h"
#include "in_mst_c.h"
#include "com_branch_c.h"
#include "com_sys_parm_c.h"
#include "fn_br_profit_c.h"
#include "com_parm_c.h"

int spD091()
{
	FILE *fp =NULL; /*��������ͳ��ÿ�������ķ�����*/ 
	char cPrdt_code[10] ;
	char cFilename[10];
	char cTmp_rate[50];
	char cCmd[100];
	int ret = 0;
	int iRet = 0;
	int ttlnum = 0;
	double dTx_amt = 0.00;
	double dProfit_amt = 0.00;
	double dRate = 0.00;
	int iPage = 0;
	struct fn_parm_c sFn_parm;
	struct fn_ac_rel_c sFn_ac_rel;
	struct fn_br_profit_c sFn_br_profit;

	memset(cPrdt_code,0x00,sizeof(cPrdt_code));
	memset(cFilename,0x00,sizeof(cFilename));
	memset(cCmd,0x00,sizeof(cCmd));
	memset(&sFn_parm,0x00,sizeof(sFn_parm));
	memset(&sFn_ac_rel,0x00,sizeof(sFn_ac_rel));
	memset(&sFn_br_profit,0x00,sizeof(sFn_br_profit));
  memset(cTmp_rate,0x00,sizeof(cTmp_rate));

	pub_base_sysinit();
  get_zd_data("0630",cPrdt_code);
  get_zd_data("0730",cTmp_rate);
	dRate = atof(cTmp_rate)/1000000;
	vtcp_log("dRate = '%lf'",dRate);
  
 /* ɾ���ļ� */
  strcpy(cFilename,"lczhsy");    /* ���֧������ */
  sprintf(cCmd,"rm -f %s/report/%s/%s.txt",getenv("HOME"),"*",cFilename);
  vtcp_log("rm -f %s/report/%s/%s.txt",getenv("HOME"),"*",cFilename);
  ret=system(cCmd);
  if( ret )
  {
      vtcp_log("rm file error [%s] [%d]",cCmd,ret);
      strcpy(g_pub_tx.reply,"S035");
      goto ErrExit;
  }
  
  ret=pub_rpt_append_openfile(&fp,QS_BR_NO,cFilename);
  if(ret)
  {
      vtcp_log("%s,%d,���ļ�ʧ��!");
      strcpy(g_pub_tx.reply,"S066");
      goto ErrExit;
  }

	iRet = Fn_parm_Dec_Upd(g_pub_tx.reply,"prdt_code = '%s' " ,cPrdt_code);
	if(iRet == 100)
	{
		sprintf(acErrMsg,"�޴���Ʋ�Ʒ��");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D324");
		goto ErrExit;	
	}
	else if(iRet)
	{
		sprintf(acErrMsg,"��ѯ��Ʋ�Ʒ���������");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D324");
		goto ErrExit;		
	}
	
	ret=Fn_parm_Fet_Upd(&sFn_parm,g_pub_tx.reply);
  if( ret==100 )
  {
      vtcp_log(" û�������Ʋ�Ʒ! ");
      strcpy(g_pub_tx.reply,"D324");
      goto ErrExit;
  }
  else if( ret )
  {
      vtcp_log("  fn_parm fetch error! ret[%d] ",ret);
      strcpy(g_pub_tx.reply,"D324");
      goto ErrExit;
  }
  if(sFn_parm.stat[0] == '8')
  {
  	sprintf(acErrMsg,"����Ʋ�Ʒ�ѷ���֧�����棡[%s]",sFn_parm.title);
		strcpy(g_pub_tx.reply,"H034");
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
		goto ErrExit;		
  	
  }
  if(sFn_parm.stat[0] != '2')
  {
  	sprintf(acErrMsg,"����Ʋ�Ʒδ����ͻ����棡[%s]",sFn_parm.title);
		strcpy(g_pub_tx.reply,"H034");
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
		goto ErrExit;			
  }
  /*ȡ��������˺�*/
  iRet = Fn_ac_rel_Sel(g_pub_tx.reply,&sFn_ac_rel,"prdt_code = '%s'",sFn_parm.prdt_code);
  if(iRet)
  {
  	sprintf(acErrMsg,"ȡ��������˺�ʧ�ܣ�[%s]",sFn_parm.title);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D340");
		goto ErrExit;		
	}
  
  
	iRet = Fn_br_profit_Dec_Upd(g_pub_tx.reply,"prdt_code = '%s' and sts='0' order by br_no",cPrdt_code);  /*ֻ�еǼǵķ�����Ž��м��ˣ������˵��򲻽��з���lpj20151109*/
	if(iRet == 100)
	{
		sprintf(acErrMsg,"��Ʋ�Ʒ����������޼�¼��[%s]",sFn_parm.title);
		strcpy(g_pub_tx.reply,"H034");
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
		goto ErrExit;		
	}
	else if(iRet)
	{
		sprintf(acErrMsg,"������Ʋ�Ʒ������������[%s]",sFn_parm.title);
		WRITEMSG
		strcpy(g_pub_tx.reply,"344");
		goto ErrExit;		
	}
	
	fprintf(fp,"\n\n\n");
	fprintf(fp, "                                           ��Ʋ�Ʒ[%s]�黹֧������ͳ��\n",sFn_parm.prdt_code);
	fprintf(fp, "                                                                                                   ����:%d��%d��%d��\n",g_pub_tx.tx_date/10000,g_pub_tx.tx_date/100%100,g_pub_tx.tx_date%100);
	fprintf(fp, "==============================================================================================================================\n" );
	fprintf(fp, "%-6s     %-10s     %-8s        %-8s\n","������","��Ʋ�Ʒ����","��ƽ��","������");
	fprintf(fp, "==============================================================================================================================\n" );
	
	vtcp_log("return_date[%ld] ",sFn_parm.return_date);
	
	while(1)
	{
		iRet = 	 Fn_br_profit_Fet_Upd(&sFn_br_profit,g_pub_tx.reply);
		if(iRet == 100)
		{
			if(!ttlnum)
			{
				sprintf(acErrMsg,"��Ʋ�Ʒ����������޼�¼��[%s]",sFn_parm.title);
				strcpy(g_pub_tx.reply,"H034");
				set_zd_data(DC_GET_MSG,acErrMsg);
				WRITEMSG
				goto ErrExit;
			}	
			iPage++;
			fprintf( fp, "==============================================================================================================================\n" );
			fprintf( fp, "�ϼ�:                       %-17.2lf  %-17.2lf \n\n                                   ��%ldҳ\n",\
	    dTx_amt,dProfit_amt,iPage);
			break;	
		}
		else if(iRet)
		{
			sprintf(acErrMsg,"��ѯ��Ʋ�Ʒ������������[%s]",sFn_parm.title);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D344");
			goto ErrExit;		
		}
		
		if(pub_base_CompDblVal(sFn_br_profit.profit_amt,0.00) > 0)
		{
			/*�����˺Ŵ���*/
			iRet = my_qs_func(sFn_ac_rel.qs_ac_no,sFn_br_profit.profit_amt);
			if(iRet != 0)
			{
				sprintf(acErrMsg,"��������˺Ŵ���ʧ��!");
				strcpy(g_pub_tx.reply,"H034");
				set_zd_data(DC_GET_MSG,acErrMsg);
				WRITEMSG
				goto ErrExit;
			}
			/*���ݻ������з���*/
			iRet = my_profit_func(sFn_br_profit.br_no,sFn_br_profit.profit_amt);
			if(iRet != 0)
			{
				sprintf(acErrMsg,"���֧���˺Ŵ���ʧ��![%s]",sFn_br_profit.br_no);
				strcpy(g_pub_tx.reply,"H034");
				set_zd_data(DC_GET_MSG,acErrMsg);
				WRITEMSG
				goto ErrExit;
			}
			sFn_br_profit.pay_date = g_pub_tx.tx_date;
			sFn_br_profit.pay_trace_no = g_pub_tx.trace_no;
			sFn_br_profit.sts[0] = '1';
			iRet = Fn_br_profit_Upd_Upd(sFn_br_profit,g_pub_tx.reply);
			if(iRet!= 0)
			{
				sprintf(acErrMsg,"���»��������ʧ��!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D344");	
				goto ErrExit;	
			}
		}
		fprintf(fp,"%6s    %10s%17.2lf%17.2lf\n",sFn_br_profit.br_no,sFn_br_profit.prdt_code,sFn_br_profit.tx_amt,sFn_br_profit.profit_amt);
		dTx_amt +=sFn_br_profit.tx_amt;
		dProfit_amt +=sFn_br_profit.profit_amt;
		ttlnum++;
	}
	
	
	sFn_parm.stat[0] = '8'; /*�ѷ���֧������*/
	sFn_parm.profit_rate = dRate;
	iRet = Fn_parm_Upd_Upd(sFn_parm,g_pub_tx.reply);
	if(iRet!= 0)
	{
		sprintf(acErrMsg,"������Ʋ�Ʒ������ʧ��!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D334");	
		goto ErrExit;	
	}
	fclose(fp);
	Fn_br_profit_Clo_Upd();
	
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log( "Before OK return: reply is[%s]", g_pub_tx.reply);
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:    
    vtcp_log("Before return: reply is[%s]", g_pub_tx.reply);
    if(strlen(g_pub_tx.reply) == 0 || strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"H034");
    }
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}


static int my_qs_func(char ac_no[20],double amt)
{
	
	
	 /* �跽��224199����Ӧ����-����ʽ����㻧(����) */
  g_pub_tx.tx_amt1=amt;
  strcpy(g_pub_tx.ac_no,ac_no);
  strcpy(g_pub_tx.name,"");
  g_pub_tx.cif_no=0;
  g_pub_tx.ac_id=0;
  g_pub_tx.ac_seqn=0;
  g_pub_tx.add_ind[0]='0';
  strcpy(g_pub_tx.hst_ind,"1");               /* �ռ�����ϸ */
  strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
  g_pub_tx.svc_ind=9001;
  strcpy(g_pub_tx.ct_ind,"2");                /* ת�� */
  strcpy(g_pub_tx.ac_id_type,"9");
  strcpy(g_pub_tx.note_type,"");
  strcpy(g_pub_tx.beg_note_no,"");
  strcpy(g_pub_tx.brf,"��Ʒ���֧������");
  vtcp_log("ac_id[%ld] tx_amt[%lf] �裺����Ӧ����-����ʽ����㻧(����) ",g_pub_tx.ac_id,g_pub_tx.tx_amt1);
  if ( pub_acct_trance() )
  {
      vtcp_log("���ô�ȡ������ʧ��!");
      goto ErrExit;
  }
  set_zd_data("102K",g_pub_tx.ct_ind);
  set_zd_double("102I",g_pub_tx.tx_amt1);
  if ( pubf_acct_proc("LC01") )
  {
      vtcp_log("�Ǽǻ����ˮ����!");
      goto ErrExit;
  }	
	
		
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log( "Before OK return: reply is[%s]", g_pub_tx.reply);
    return 0;
ErrExit:    
    vtcp_log("Before return: reply is[%s]", g_pub_tx.reply);
    return 1;
	
}


static int my_profit_func(char br_no[20],double amt)
{
	int iRet = 0;
/*struct com_parm_c sCom_parm;
	struct mdm_ac_rel_c sMdm_ac_rel;
	
	memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
	memset(&sCom_parm,0x00,sizeof(sCom_parm));
	/ȡ�û���������˺�/
	iRet = Com_parm_Sel(g_pub_tx.reply,&sCom_parm,"parm_code = '%s' and parm_seqn =20 ",br_no);
	if(iRet == 100 )
	{
		sprintf(acErrMsg,"�������������޴˻����˺ţ�[%s]",br_no);
		strcpy(g_pub_tx.reply,"H034");
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
		goto ErrExit;
	}
	else if(iRet)
	{
		sprintf(acErrMsg,"��ѯ�������������");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O005");
		goto ErrExit;
		
	}
	
	iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no = '%s'",sCom_parm.val);
	if(iRet == 100)
	{
		sprintf(acErrMsg,"mdm_ac_rel���޴˻����˺ţ�");
		WRITEMSG
		strcpy(g_pub_tx.reply,"B117");
		goto ErrExit;
	}
	else if(iRet)
	{
		sprintf(acErrMsg,"��ѯmdm_ac_rel����");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L015");
		goto ErrExit;	
	}
	*/
	
  g_pub_tx.tx_amt1=amt;
  strcpy(g_pub_tx.ac_no,"51113");
  strcpy(g_pub_tx.name,"");
  g_pub_tx.cif_no = 0;
  /*strcpy(g_pub_tx.tx_br_no,"10001");*/
  strcpy(g_pub_tx.opn_br_no,br_no);
  vtcp_log("g_pub_tx.opn_br_no = %s",g_pub_tx.opn_br_no);
g_pub_tx.ac_id=0;
  g_pub_tx.ac_seqn=0;
  g_pub_tx.add_ind[0]='1';
  strcpy(g_pub_tx.hst_ind,"1");               /* �ռ�����ϸ */
  strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
  g_pub_tx.svc_ind=1001;
  strcpy(g_pub_tx.ct_ind,"2");                /* ת�� */
  strcpy(g_pub_tx.ac_id_type,"");
  strcpy(g_pub_tx.note_type,"");
  strcpy(g_pub_tx.beg_note_no,"");
  strcpy(g_pub_tx.brf,"��Ʒ���֧������");
  vtcp_log("ac_id[%ld] tx_amt[%lf] ����֧���˻�(����) ",g_pub_tx.ac_id,g_pub_tx.tx_amt1);
  if ( pub_acct_trance() )
  {
      vtcp_log("���ô�ȡ������ʧ��!");
      goto ErrExit;
  }
 /*strcpy(g_pub_tx.tx_br_no,"10001");*/
 strcpy(g_pub_tx.opn_br_no,br_no);
  set_zd_data("102K",g_pub_tx.ct_ind);
  set_zd_double("1002",g_pub_tx.tx_amt1);
  if ( pubf_acct_proc("LC02") )
  {
      vtcp_log("�Ǽǻ����ˮ����!");
      goto ErrExit;
  }
	
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log( "Before OK return: reply is[%s]", g_pub_tx.reply);
    return 0;
ErrExit:    
    vtcp_log("Before return: reply is[%s]", g_pub_tx.reply);
    return 1;
	
}

