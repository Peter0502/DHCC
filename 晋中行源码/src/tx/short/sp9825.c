/*************************************************
* �� �� ��:  sp9825.c
* ���������� ����Ͷ��Э���Ų�ѯͶ�ʵǼǲ�       
*
* ��    ��: 
* ������ڣ� 2003��1��21�� 
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_invest_c.h"

int sp9825()
{
    struct mo_invest_c sMo_invest;
    int ret;
    char flag[2];
   
    memset(&sMo_invest,0x0,sizeof(struct mo_invest_c));

    get_zd_data("0670",flag);
    get_zd_data("0580",sMo_invest.pact_no);

    sprintf(acErrMsg," pact_no = [%s] ",sMo_invest.pact_no);
    WRITEMSG

	/***
    if (atoi(flag) == 0)
    {
        sprintf(acErrMsg,"�ò���Ϊ¼��״̬�����ÿ�ݲ�ѯ!!");
        WRITEMSG
        goto OkExit;
    }
	***/
   vtcp_log("[%s][%d]bbbbbbbbbbbb hehe ",__FILE__,__LINE__); 
    ret = Mo_invest_Sel(g_pub_tx.reply,&sMo_invest,"pact_no='%s'",
          sMo_invest.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"Select mo_invest error!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"B003");
        goto ErrExit;
    }
     vtcp_log("[%s][%d]aaaaaaaaaaaaaaaa hehe ",__FILE__,__LINE__); 
    set_zd_data("0230",sMo_invest.bkrl_code);
    set_zd_data("0860",sMo_invest.ticket_type);
    set_zd_double("0390",sMo_invest.buy_amt);
    set_zd_double("0400",sMo_invest.par_bal);
    set_zd_long("0440",sMo_invest.isuue_date);
    set_zd_long("0450",sMo_invest.mtr_date);
    set_zd_long("0480",sMo_invest.term);
    set_zd_data("0680",sMo_invest.intst_type);
    set_zd_double("0840",sMo_invest.rate);
	set_zd_long("0460",sMo_invest.buy_date);
	set_zd_double("0410",sMo_invest.pre_intst);
	set_zd_double("0420",sMo_invest.overflow_amt);
	set_zd_double("0430",sMo_invest.abate_amt);
	  set_zd_long("0330",sMo_invest.lx_ac_id);  
	set_zd_double("1003",sMo_invest.singl_feeval);

OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
