/***************************************************************
* �� �� ��: gD0293.c
* ��������: ֧��40801ת�뵽����40801��Ŀ
*
* ��    ��: ligl
* �������: 2006-11-3 16:11
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_parm_c.h"
#include "com_sys_parm_c.h"
#include "com_branch_c.h"
#include "gl_sub_c.h"
#include "com_item_c.h"
char  def_br_no[6];
char  def_tel[5];
char  cTestbrno[BRNO_LEN+1];
gD0293()
{
   int ret=0;
	double c_bal=0.00, d_bal=0.00;
	char dc_ind;
	struct com_parm_c sCom_parm;
	struct com_branch_c sCom_branch;
	struct com_sys_parm_c sCom_sys_parm;
	struct gl_sub_c sGl_sub;
	struct com_item_c sCom_item;
	memset(&sGl_sub , 0 , sizeof(sGl_sub));
	memset(&sCom_parm , 0 , sizeof(sCom_parm));
	memset(&sCom_branch, 0  ,sizeof(sCom_branch));
	memset(&sCom_sys_parm, 0 , sizeof(sCom_sys_parm));
	memset(&sCom_item, 0 , sizeof(sCom_item));
	ret=sql_begin(); /*������*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}
	memset(cTestbrno, 0 , sizeof(cTestbrno));
	ret=Com_sys_parm_Sel(g_pub_tx.reply,&sCom_sys_parm,"1=1");
	if(ret != 0)
	{
		sprintf( acErrMsg, "�õ���������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}
	/*** ��ʼ��ȫ�ֱ��� ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}

	strcpy( g_pub_tx.ac_id_type,"9" ); /*�˻�����Ϊ�ڲ�*/ 
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*������δ��ȡ�ֻ�*/ 
	strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*���˶�ƾ֤�ţ��������ˮ����ϸ*/ 
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no); 
	strcpy( g_pub_tx.prt_ind,"0" ); /*������*/ 
	g_pub_tx.svc_ind=9001;  /*�ڲ��ʴ�ȡ*/ 
	strcpy(g_pub_tx.prt_ind,"0"); 
	strcpy(g_pub_tx.hst_ind,"1"); /*�ռ�����ϸ*/
	g_pub_tx.tx_date = sCom_sys_parm.sys_date;
	ret=Com_branch_Dec_Sel(g_pub_tx.reply,"br_type='4'");
	if(ret)
	{
		strcpy(acErrMsg,"��������α����!");
		WRITEMSG
		goto ErrExit;
	}

	while(1)
	{
		ret=Com_branch_Fet_Sel(&sCom_branch,g_pub_tx.reply);
		if(ret==100) break;
		if(ret)
		{
		strcpy(acErrMsg,"��ȡ�����α����!");
		WRITEMSG
		goto ErrExit;
		}
		if(strlen(cTestbrno)!=0)
		{
			if(memcmp(cTestbrno,sCom_branch.br_no,BRNO_LEN))
				continue;
		}
		memcpy(g_pub_tx.tel,sCom_branch.br_no+1,2);
		memcpy(g_pub_tx.tel+2,"99",2);
		vtcp_log("%s,%d tel===[%s]",__FILE__,__LINE__,g_pub_tx.tel);
		set_zd_data(DC_TEL,g_pub_tx.tel);
		memcpy(g_pub_tx.tx_br_no,sCom_branch.br_no,5);
		ret=Gl_sub_Dec_Sel(g_pub_tx.reply,"acc_hrt='40801' and br_no='%s'",sCom_branch.br_no);
		if(ret)
		{
		strcpy(acErrMsg,"������������α����!");
		WRITEMSG
		goto ErrExit;
		}
		while(1)
		{
			vtcp_log("%s,%d ",__FILE__,__LINE__);
			ret=Gl_sub_Fet_Sel(&sGl_sub,g_pub_tx.reply);
			if(ret==100)
			{
				break;
			}
			vtcp_log("%s,%d ",__FILE__,__LINE__);
			if(ret)
			{
				strcpy(acErrMsg,"��ȡ�����α����!");
				WRITEMSG
				goto ErrExit;
			}
			vtcp_log("%s,%d ",__FILE__,__LINE__);
			strncpy(g_pub_tx.brf,"���ݹ���ת��",sizeof(g_pub_tx.brf));
			
			if(sGl_sub.cr_bal!=0)
			{
				char cAc_no[8];
				char cAcc_hrt[6];
				memset(cAcc_hrt, 0 , sizeof(cAcc_hrt));
				memset(cAc_no, 0 , sizeof(cAc_no));
				/* ��������֧�� */
				ret=Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_hrt='%s'",sGl_sub.acc_hrt);
				if(ret)
				{
					sprintf(acErrMsg,"����[%s]��ѯ��Ŀ����",sGl_sub.acc_hrt);
					WRITEMSG
					goto ErrExit;
				}
				g_pub_tx.tx_amt1=0.0;
				memcpy(g_pub_tx.opn_br_no,sCom_branch.br_no,5);
				g_pub_tx.tx_amt1=-(sGl_sub.cr_bal);
				g_pub_tx.ct_ind[0]='2';
				g_pub_tx.add_ind[0]='1';
				memcpy(g_pub_tx.cur_no,"01",2);
				vtcp_log("%s,%d ",__FILE__,__LINE__);
				sprintf(acErrMsg, "�������[%s]��Ŀ[%s]�������[%.2f]---------->",sCom_branch.br_no,sGl_sub.acc_hrt,sGl_sub.dr_bal);
				WRITEMSG
				set_zd_data("1211",sCom_item.acc_no);
				set_zd_double("1218",g_pub_tx.tx_amt1);/**��֧�и�**/
				set_zd_data("121A",g_pub_tx.brf);
				set_zd_data("1215",g_pub_tx.ct_ind); 
				set_zd_data("1214",g_pub_tx.cur_no);
				set_zd_data("0152",sGl_sub.acc_hrt);
				vtcp_log(acErrMsg, "%s,%d ----------",__FILE__,__LINE__);
				ret = pubf_acct_proc("A017");
				if (ret != 0)
				{
					sprintf(acErrMsg, "�ǻ���%s��%s��Ŀ����ʴ���!! ret=[%d]", ret);
					WRITEMSG
					goto ErrExit;
				}
				/* ����������� 
				memcpy(cAc_no,sGl_sub.acc_hrt,sizeof(cAc_no)-1);
				memcpy(cAcc_hrt,sGl_sub.acc_hrt,sizeof(cAcc_hrt)-1);
				pub_base_strpack(cAc_no);*/
				vtcp_log(acErrMsg, "%s,%d ----------",__FILE__,__LINE__);
				ret=Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_hrt='%s'",sGl_sub.acc_hrt);
				if(ret)
				{
					sprintf(acErrMsg,"����[%s]��ѯ��Ŀ����",sGl_sub.acc_hrt);
					WRITEMSG
					goto ErrExit;
				}
				g_pub_tx.tx_amt1=0.0;
				g_pub_tx.tx_amt1=sGl_sub.cr_bal;/**��������**/
				
				g_pub_tx.ct_ind[0]='2';
				g_pub_tx.add_ind[0]='1';
				memcpy(g_pub_tx.cur_no,"01",2);
				memcpy(g_pub_tx.opn_br_no,QS_BR_NO,5);
				vtcp_log(acErrMsg, "%s,%d ----------",__FILE__,__LINE__);
				sprintf(acErrMsg, "ͬʱ����[%s]�������Ŀ[%s]---------->",sCom_branch.br_no,cAcc_hrt);
				WRITEMSG
				set_zd_data("1211",sCom_item.acc_no);
				set_zd_double("1218",g_pub_tx.tx_amt1);
				set_zd_data("121A",g_pub_tx.brf);
				set_zd_data("1215",g_pub_tx.ct_ind); 
				set_zd_data("1214",g_pub_tx.cur_no);
				set_zd_data("0152",sGl_sub.acc_hrt);
				vtcp_log(acErrMsg, "%s,%d ----------",__FILE__,__LINE__);
				ret = pubf_acct_proc("A017");
				if (ret != 0)
				{
					sprintf(acErrMsg, "�ǻ���%s��%s��Ŀ����ʴ���!! ret=[%d]", ret);
					WRITEMSG
					goto ErrExit;
				}
			}
		}
		Gl_sub_Clo_Sel();
	}
	Com_branch_Clo_Sel();



OkExit:
    sql_commit();   /*--�ύ����--*/
    strcpy(g_pub_tx.reply,"0000");
		sprintf(acErrMsg,"40801ת��40801�˺Ŵ���ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");
    }
    sql_rollback(); /*--����ع�--*/
	sprintf(acErrMsg,"40801ת��40801�˺Ŵ���ʧ��![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}
