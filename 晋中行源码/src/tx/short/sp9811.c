/*************************************************
* �� �� ��:  sp9811.c
* ���������� ���ݲ������ͺͺ�ͬ�Ų�ѯ���Ǽǲ�
*
* ��    ��:  rob
* ������ڣ� 2003��1��17�� 
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_ibl_c.h"

int sp9811()
{
    struct mo_ibl_c sMo_ibl_c;
    struct mdm_ac_rel_c sMdm_ac_rel;
    int ret;
    char flag[2];
    char sign[2];
   
    memset(&sMo_ibl_c,0x0,sizeof(struct mo_ibl_c));

    get_zd_data("0670",flag);
    get_zd_data("0720",sign);
    get_zd_data("0730",sMo_ibl_c.pact_no);


    sprintf(acErrMsg,"pact_no=[%s] sign=[%s]!! ",sMo_ibl_c.pact_no,sign);
    WRITEMSG

    if (flag[0] == '0')
    {
    	ret = Mo_ibl_Sel(g_pub_tx.reply,&sMo_ibl_c," pact_no= '%s'",
				sMo_ibl_c.pact_no,sign);
    	if (ret != 0 && ret != 100)
    	{
        	sprintf(acErrMsg,"��ͬ�ſ�����д����!! ret=[%d]",ret);
        	WRITEMSG
        	strcpy(g_pub_tx.reply, "B003");
        	goto ErrExit;
    	}
		else if (ret == 0)
		{
			sprintf(acErrMsg,"�ú�ͬ���Ѿ�����!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"B171");
			goto ErrExit;
		}
        goto OkExit;  /* ¼��ֻ����� */
    }

    ret = Mo_ibl_Sel(g_pub_tx.reply,&sMo_ibl_c," pact_no= '%s' and sign='%s'",
                     sMo_ibl_c.pact_no,sign);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ͬ�ſ�����д����!! ret=[%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "B003");
        goto ErrExit;
    }
    
	/******** delete by rob 20050706
	if (sMo_ibl_c.op_ac_id != 0)
	{
    	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_id = %ld and \
           	ac_seqn= 0 ",sMo_ibl_c.op_ac_id);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"��ѯ�Է��˺Ŵ���!! [%d]",ret);
        	WRITEMSG
        	strcpy(g_pub_tx.reply, "B004");
        	goto ErrExit;
    	}
		set_zd_data( "0300", sMdm_ac_rel.ac_no); *�Է��˺�*
    }
	else
	{
		set_zd_data("0300",sMo_ibl_c.acc_no); * ��Ŀ�� *
	}
	*********/
	set_zd_data( "0300", sMo_ibl_c.op_ac_no); /*�Է��˺�*/
    
	set_zd_data( "0230", sMo_ibl_c.prdt_no); /*��Ʒ���*/
	set_zd_data( "0250", sMo_ibl_c.op_name);  /*��軧��*/
	set_zd_double( "0400", sMo_ibl_c.amt);  /*�����*/
	set_zd_double( "0840", sMo_ibl_c.rate); /*�������*/
	set_zd_double( "0850", sMo_ibl_c.over_rate); /*�������*/
	set_zd_long( "0440", sMo_ibl_c.matr_date);/*��������*/
	set_zd_long("0360", sMo_ibl_c.term); /* ������� */
	
  set_zd_data("0310",sMo_ibl_c.kh_ac_no);  /*(���пͻ��ʺ�)*/
  set_zd_data("0260", sMo_ibl_c.kh_name);  /*���пͻ���*/
	set_zd_data("0520", sMo_ibl_c.op_br_no); /*�����к�*/
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
