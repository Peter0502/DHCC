/**************************************************************
* �� �� ��: spB901.c
* ����������ͬ�ǽ������,�������ܼ��ʽ���
* ��    ��: 
* ������ڣ�2006��8��18��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_parm_c.h"
#include "mdm_ac_rel_c.h"
#include "in_mst_c.h"
#include "prdt_ac_id_c.h"
#include "accountreg_c.h"
#include "com_branch_c.h"

spB901()
{
	char flag[2],trance_type[2];
	char tx_br_no[6];
	char qs_br_no[6];
	char ctx_br_no[6];
	int ret=0;
	struct	prdt_ac_id_c	sPrdt_ac_id;
	struct accountreg_c accountreg;
	struct  com_branch_c     com_branch;
	struct  com_parm_c     com_parm;
	struct  mdm_ac_rel_c     mdm_ac_rel;
	struct  in_mst_c     in_mst;
	

	memset(&mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&com_parm,0x00,sizeof(struct com_parm_c));
	memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c));
	memset(&in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));
	memset(&g_od_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_od_parm,0x00,sizeof(struct in_parm_c));
	memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&accountreg,0x00,sizeof(struct accountreg_c));
	memset(&com_branch,0x00 , sizeof(struct  com_branch_c));
	memset(tx_br_no,0x00,sizeof(tx_br_no));
	memset(ctx_br_no,0x00,sizeof(ctx_br_no));
	memset(qs_br_no,0x00,sizeof(qs_br_no));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	
	get_zd_data("0680",trance_type);	/* ��ǰ̨��Ӧ 6��ʾ�������ڲ��ʻ��Ŀ  1�Թ��� */
	get_zd_data("0300",g_pub_tx.ac_no);

    pub_base_old_acno( g_pub_tx.ac_no );   /**�Ծ��ʺŵĴ���**/

	get_zd_data("0210",g_pub_tx.cur_no);
	/**get_zd_int("0340",&g_pub_tx.ac_seqn);**/
	if( trance_type[0]=='1' )
		pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn); /* (����)����ת�����˻���� */
	else
		g_pub_tx.ac_seqn=0;
	get_zd_data("0890",g_pub_tx.note_type);
	get_zd_data("0580",g_pub_tx.beg_note_no);
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no);
	get_zd_data("0660",flag);				/** 1-�裻2-�� **/
	if( flag[0]!='1' && flag[0]!='2' )
	{
		sprintf(acErrMsg,"�����־����!! [%s]",flag);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P404");
		goto ErrExit;
	}
	/*get_zd_data("0880",g_pub_tx.brf); modified by liuxuan 20060817*/
	get_zd_data("0670",g_pub_tx.ct_ind); /* 1�ֽ� 2ת�� */
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*������δ��ȡ�ֻ�*/
	strcpy( g_pub_tx.prt_ind,"0" ); /*������*/
	strcpy( g_pub_tx.ac_wrk_ind,"100" ); /*���˶�ƾ֤�ţ��������ˮ����ϸ  ������ liuxuan*/
	strcpy(g_pub_tx.hst_ind,"1"); /*�ռ�����ϸ*/

	if( strlen(g_pub_tx.ac_no)==3 || strlen(g_pub_tx.ac_no)==5|| strlen(g_pub_tx.ac_no)==7 )
	{	/*** ��Ŀ���� ***/
			get_zd_double("0400",&g_pub_tx.tx_amt1);
			if( flag[0]=='1' )
				strcpy(g_pub_tx.add_ind,"0");		/*** �跽 ***/
			else
				strcpy(g_pub_tx.add_ind,"1");		/*** ���� ***/
			g_pub_tx.svc_ind=9001;  				/*** �ڲ��ʴ�ȡ ***/
			set_zd_double("1001",g_pub_tx.tx_amt1);
	}
	else 
	{
			/*** ȡ��Ʒ���룬���ʺ���� ***/
			ret=pub_base_ac_prdt(&sPrdt_ac_id);
			if( ret )
			{
				sprintf(acErrMsg,"ȡ��Ʒ�������!");
				WRITEMSG
				goto ErrExit;
			}
			strcpy( g_pub_tx.ac_id_type,sPrdt_ac_id.ac_id_type );
			if( sPrdt_ac_id.ac_id_type[0]=='2' )
			{
			sprintf(acErrMsg,"�����ʺŲ������˽���!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P403");
			goto ErrExit;
			}

			if( sPrdt_ac_id.ac_id_type[0]=='1' )		/*** �Թ����� ***/
			{
			get_zd_double("0400",&g_pub_tx.tx_amt1);
			if( trance_type[0]!='1' )
			{
				sprintf(acErrMsg,"�����˻�����ѡ����ڽ�������!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P409");
				goto ErrExit;
			}
			ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s' ",sPrdt_ac_id.prdt_no);
			if( ret )
			{
				sprintf(acErrMsg,"ȡ���ڲ�Ʒ��������!! [%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"W013");
				goto ErrExit;
			}
			if( g_dd_parm.cif_type[0]=='1' )
			{
				sprintf(acErrMsg,"������Թ��ʺ�!! [%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"O165");
				goto ErrExit;
			}

			if( flag[0]=='1' )	strcpy(g_pub_tx.add_ind,"0"); /*** �跽���� ***/
			else strcpy(g_pub_tx.add_ind,"1");
			g_pub_tx.svc_ind=1001;  				/*���ڴ�ȡ*/

			set_zd_double("1001",g_pub_tx.tx_amt1);
			}
			else if( sPrdt_ac_id.ac_id_type[0]=='3' 
				|| sPrdt_ac_id.ac_id_type[0]=='4' )		/*** ����͸֧ ***/
			{
			if( flag[0]=='1' ) 
				strcpy(g_pub_tx.add_ind,"1");		/*** ����跽���� ***/
			else
				strcpy(g_pub_tx.add_ind,"0");
			
			if( trance_type[0]=='2' )
			{
				get_zd_double("0400",&g_pub_tx.tx_amt1);
				g_pub_tx.svc_ind=3010;  			/*������ȡ*/
				set_zd_double("1001",g_pub_tx.tx_amt1);
			}
			else if( trance_type[0]=='3' )
			{
				get_zd_double("0400",&g_pub_tx.tx_amt2);
				g_pub_tx.svc_ind=3020;  			/*������ڴ�ȡ*/
				set_zd_double("1002",g_pub_tx.tx_amt2);
			}
			else if( trance_type[0]=='4' )
			{
				get_zd_double("0400",&g_pub_tx.tx_amt3);
				g_pub_tx.svc_ind=3030;  			/*��������ȡ*/
				set_zd_double("1003",g_pub_tx.tx_amt3);
			}
			else if( trance_type[0]=='5' )
			{
				get_zd_double("0400",&g_pub_tx.tx_amt4);
				g_pub_tx.svc_ind=3040;  			/*��������ȡ*/
				set_zd_double("1004",g_pub_tx.tx_amt4);
			}
			else
			{
				sprintf(acErrMsg,"�����˻�����ѡ����������!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P407");
				goto ErrExit;
			}
			}
			else if( sPrdt_ac_id.ac_id_type[0]=='9' )	/*** �ڲ��� ***/
			{
				get_zd_double("0400",&g_pub_tx.tx_amt1);
				if( flag[0]=='1' ) strcpy(g_pub_tx.add_ind,"0");
				else	strcpy(g_pub_tx.add_ind,"1");
				g_pub_tx.svc_ind=9001;  /*�ڲ��ʴ�ȡ*/
				set_zd_double("1001",g_pub_tx.tx_amt1);
			}
			else
			{
				sprintf(acErrMsg,"�ʺ����ʹ���![%s]",g_pub_tx.ac_id_type);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P410");
				goto ErrExit;
			}
	}

		ret=pub_acct_trance();
		if( ret )
		{
			sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
			WRITEMSG
			goto ErrExit;
		}
		ret=insert_accountreg(&accountreg);
		if( ret )
		{
			sprintf(acErrMsg,"ͬ�ǵǼǱ�����ʧ��!");
			WRITEMSG
			goto ErrExit;
		}
	/*set_zd_data("1204",g_pub_tx.cur_no);*/
	
	/*Add by hxc 091230  ���ڽ�����������֧�в�����֧�л���22301��Ŀ���ˣ�����֧�в���������10001��22301��Ŀ����*/
	get_zd_data("0020",tx_br_no);
	sprintf(acErrMsg,"[%s][%d]ͬ��������������[%s]",__FILE__,__LINE__,tx_br_no);
	WRITEMSG
	ret=Com_branch_Sel(g_pub_tx.reply,&com_branch,"br_no='%s' and city_no=1",tx_br_no);
    if(ret==0)
    {
        sprintf(acErrMsg,"[%s][%d]����ͬ��������������[%s],������֧��",__FILE__,__LINE__,com_branch.br_no);
        WRITEMSG
        set_zd_data("0020",tx_br_no);
    }
    else if ( ret == 100 || ret == 1403)
	{
    	sprintf(acErrMsg,"[%s][%d]����ͬ��������������[%s],������֧��",__FILE__,__LINE__,tx_br_no);
    	WRITEMSG
    	memset(qs_br_no,0x00,sizeof(qs_br_no));
    	memcpy(qs_br_no,"10001",5);
    	set_zd_data("0020",qs_br_no);
    	sprintf(acErrMsg,"[%s][%d]ͬ��������������[%s],�������[%s]",__FILE__,__LINE__,tx_br_no,qs_br_no);
    	WRITEMSG
	}
	else if ( ret )
	{
		strcpy( g_pub_tx.reply, "D103" );
    	sprintf(acErrMsg,"[%s][%d]����ͬ��������������[%s]����!\n",__FILE__,__LINE__,tx_br_no);
    	WRITEMSG
		goto ErrExit;
	}
		/*End by hxc 091230   ���ڽ�����������֧�в�����֧�л���22301��Ŀ���ˣ���������������10001��22301��Ŀ����*/
    /**ȡ��Ӧ���ڲ��˻�**/
    if(memcmp(tx_br_no,"11501", 5)!=0){
    	strcpy(ctx_br_no,"10001");
    	}else{
    		strcpy(ctx_br_no,"11501");
    		}
    ret = Com_parm_Sel(g_pub_tx.reply,&com_parm,"parm_code='%s' and parm_seqn=9999",ctx_br_no);/********ȡ�����ж�Ӧ���ڲ��˻��ͽ��ݶ�Ӧ���ڲ��˻�***********/
    if(ret)
    {
        sprintf(acErrMsg,"[%s][%d]�жҸ�����ʱ�ʻ�����!cnt=[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"W025");
        WRITEMSG
        goto ErrExit;
    }
    /** ��鱾Ʊǩ�����Ƿ���� **/
    ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",com_parm.val);/******�������˻���ϵ������������ac_id*************/
    if(ret)
    {
        sprintf(acErrMsg,"[%s][%d]���22301�ֻ�ʧ��!sqlcode=[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"B117");
        WRITEMSG
        goto ErrExit;
    }
    ret = In_mst_Sel(g_pub_tx.reply,&in_mst,"ac_id='%d'",mdm_ac_rel.ac_id);/********�ڲ�̨�����ļ�****************/
    if(ret)
    {
        sprintf(acErrMsg,"[%s][%d]22301�ֻ�����!sqlcode=[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"B117");
        WRITEMSG
        goto ErrExit;
    }
    if( in_mst.sts[0]!='1' && in_mst.sts[0]!='2' )
    {
        sprintf(acErrMsg,"�˺�״̬������״̬![%ld,%s]",in_mst.ac_id,in_mst.sts);
        strcpy(g_pub_tx.reply,"P109");
        WRITEMSG
        goto ErrExit;
    }
    set_zd_data("0310",com_parm.val);/**�ڲ��˻�**/

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"ͬ��������ʳɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"ͬ���������ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

int insert_accountreg(struct accountreg_c *accountreg)
{
	int  ret=0;
	char tmp_no[20];
	struct accountreg_c tmp_accountreg;
	memset(tmp_no,'\0',sizeof(tmp_no));
	get_zd_data("0020",accountreg->br_no);
	get_zd_data("0810",accountreg->br_name);
	get_zd_long("0040",&accountreg->trace_no);
	get_zd_long("0050",&accountreg->tx_date);
	get_zd_long("0060",&accountreg->tx_time);
	get_zd_data("0070",accountreg->tel);
	get_zd_data("0080",accountreg->chk);
	get_zd_data("0870",tmp_no);	/* ���ʱ�Ÿ����н�����,����ʱ���տ��� */
	pub_base_strpack(tmp_no);
	get_zd_data("0690",accountreg->put_flag); /* 1���  2���� */
	if(accountreg->put_flag[0]=='1'){
		memcpy(accountreg->change_no,tmp_no,strlen(tmp_no));/* �����н����� */
		strcpy(accountreg->pay_ac_no,g_pub_tx.ac_no);/* �����ʺ� */
		pub_base_strpack(accountreg->pay_ac_no);
		get_zd_data("0250",accountreg->pay_name);/* �����ʺ� */
		pub_base_strpack(accountreg->pay_name);
		get_zd_data("0961",accountreg->cash_ac_no); /* �տ����ʺ� */
		pub_base_strpack(accountreg->cash_ac_no);
		vtcp_log("accountac_no![%s]",accountreg->cash_ac_no);
		get_zd_data("0962",accountreg->cash_name); /*  �տ������� */
		pub_base_strpack(accountreg->cash_name);
		vtcp_log("accountac_name![%s]",accountreg->cash_name);
		get_zd_data("0963",accountreg->cash_chg_no); /* ������ */
		pub_base_strpack(accountreg->cash_chg_no);
		get_zd_data("0964",accountreg->cash_opn_name); /* ���������� */
		pub_base_strpack(accountreg->cash_opn_name);
	}
	else{
		memcpy(accountreg->cash_chg_no,tmp_no,strlen(tmp_no));/* �տ��н����� */
		strcpy(accountreg->cash_ac_no,g_pub_tx.ac_no);/* �տ��ʺ� */
		pub_base_strpack(accountreg->cash_ac_no);
		get_zd_data("0250",accountreg->cash_name);/* �տ����� */
		pub_base_strpack(accountreg->cash_name);
		get_zd_data("0961",accountreg->pay_ac_no); /* �������ʺ� */
		pub_base_strpack(accountreg->pay_ac_no);
		vtcp_log("accountac_no![%s]",accountreg->pay_ac_no);
		get_zd_data("0962",accountreg->pay_name); /*  ���������� */
		pub_base_strpack(accountreg->pay_name);
		vtcp_log("accountac_name![%s]",accountreg->pay_name);
		get_zd_data("0963",accountreg->change_no);	/* �����н����� */
		pub_base_strpack(accountreg->change_no);
		get_zd_data("0964",accountreg->cash_opn_name); /* ���������� */
		pub_base_strpack(accountreg->cash_opn_name);
	}
	get_zd_data("0700",accountreg->stat); /* 1�Ǽ�  2����  8�Ѵ�ӡ  9ȡ�� */
	get_zd_data("0890",accountreg->note_type);
	get_zd_data("0580",accountreg->note_no);
	accountreg->amt=g_pub_tx.tx_amt1;
	memset(&tmp_accountreg,'\0',sizeof(tmp_accountreg));
	memcpy(&tmp_accountreg,accountreg,sizeof(tmp_accountreg));
	ret = Accountreg_Ins(tmp_accountreg, g_pub_tx.reply );
	Accountreg_Debug(&tmp_accountreg);
	if ( ret )
		return(1);

	return 0;
ErrExit:
	return 1;
}
