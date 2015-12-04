/*************************************************
* �� �� ��:  spE709.c
* ���������� ��������ʴ�����
*
* ��    ��:  wanglei
* ������ڣ� 2007��6��24��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
#include <stdio.h>
#include <math.h>
#define  EXTERN
#include "public.h"
#include "com_branch_c.h"

int spE709()
{
	int	ret;
	char tmp_ind1[2],tmp_add[2];

	/* ��ʼ���ṹ */
	struct com_branch_c sCom_branch;

	memset(&g_pub_tx,0,sizeof(g_pub_tx) );
	memset(&sCom_branch,0,sizeof(sCom_branch) );
	pub_base_sysinit();

	get_zd_long("0440",&g_pub_tx.erro_date);	/*��������*/
	get_zd_data("0910",g_pub_tx.opn_br_no);	    /*��������*/
	get_zd_data("0670",tmp_ind1);				/*����ʽ1-����2-����*/
	get_zd_data("0680",tmp_add);				/*�������1-��2-��*/
vtcp_log("CTIND[%s]",g_pub_tx.ct_ind);
	get_zd_data("0300",g_pub_tx.ac_no);			/*�ʺ�*/
vtcp_log("[%s][%d]ACTNO=[%s]",__FILE__,__LINE__,g_pub_tx.ac_no);
	get_zd_int("0340",&g_pub_tx.ac_seqn);		/*�ʺ����*/
vtcp_log("CTIND[%s]",g_pub_tx.ct_ind);
	get_zd_double("0410",&g_pub_tx.tx_amt1);	/*���*/
vtcp_log("HAO---LOOK tx_mat1=[%lf]",g_pub_tx.tx_amt1);
	
	g_pub_tx.ct_ind[0] = '2';  /****�˽���ֻ�账��ת�ʵ����****/

	if( tmp_ind1[0]=='1' )
		strcpy(g_pub_tx.brf,"����");
	else if( tmp_ind1[0]=='2' )
		strcpy(g_pub_tx.brf,"����");
	else
	{
    	sprintf(acErrMsg,"�������Ǵ���ʽ����[%s]!!",tmp_ind1);
    	WRITEMSG
		strcpy(g_pub_tx.reply,"P442");
		goto ErrExit;
	}
	strcpy(g_pub_tx.hst_ind,"1");		/*�ռ�����ϸ*/
	
	vtcp_log("[%s][%d] ������ = [%s]\n",__FILE__,__LINE__,g_pub_tx.opn_br_no);
	ret = Com_branch_Sel(g_pub_tx.reply,&sCom_branch,"br_no = %s ",g_pub_tx.opn_br_no);
	if(ret == 100)
	{
    	sprintf(acErrMsg,"�޴˻�����Ϣ[%s]!!",g_pub_tx.opn_br_no);
    	WRITEMSG
		strcpy(g_pub_tx.reply,"S019");
		goto ErrExit;
	}
	else if(ret != 0)
	{
    	sprintf(acErrMsg,"��ѯ������Ϣ�쳣[%s]!!",g_pub_tx.opn_br_no);
    	WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

vtcp_log("CTIND[%s]",g_pub_tx.ct_ind);
	if( tmp_add[0]=='1' )
		strcpy(g_pub_tx.add_ind,"0");
	else if( tmp_add[0]=='2' )
		strcpy(g_pub_tx.add_ind,"1");
	else
	{
    	sprintf(acErrMsg,"�����־��[%s]!!",tmp_ind1);
    	WRITEMSG
		strcpy(g_pub_tx.reply,"P404");
		goto ErrExit;
	}

vtcp_log("CTIND[%s]",g_pub_tx.ct_ind);
vtcp_log("HAO---LOOK tx_mat1=[%lf]",g_pub_tx.tx_amt1);
	ret=pub_acct_redo(tmp_ind1);
	if(ret)
	{
    	sprintf(acErrMsg,"���ó���/���Ǻ�������[%s]!!",g_pub_tx.reply);
    	WRITEMSG
		goto ErrExit;
	}

vtcp_log("HAO---LOOK tx_mat1=[%lf]",g_pub_tx.tx_amt1);
	set_zd_data("0210",g_pub_tx.cur_no);
	set_zd_double("0410",g_pub_tx.tx_amt1);
OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}
