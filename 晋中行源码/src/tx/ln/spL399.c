/*************************************************
* �� �� ��: spL399.c
* ����������   
*              ������оɴ���ת�����ں�Ҫ��ת��ȥ
*              �ֻ�״̬��Ϊ����,���񲿷�:
*              ���ڲ����߽跽����,���������߽跽����,���ú�ת�����෴
* ��    ��:    liuxuan
* ������ڣ�   2007��05��23��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "dc_acc_rel_c.h"
#include "com_item_c.h"   
spL399()  
{
	int ret=0;
	char ori_ac_sts,ac_no[6];
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	sprintf(acErrMsg,"��ʼ��ȫ�ֱ��� PASS!");
	WRITEMSG	

	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset (&g_ln_parm, 0x00, sizeof(struct ln_parm_c));

	get_zd_data("0370",g_pub_tx.ac_no);		/* �ʺ� */
	get_zd_int("0340",&g_pub_tx.ac_seqn);		/* �˻���� */

	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
		&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
	if( ret ) goto ErrExit;

    /* ���ת�����Ƿ��Ǳ����˺� */
    if( strcmp( g_ln_mst.opn_br_no , g_pub_tx.tx_br_no )!=0 )
    {
        sprintf(acErrMsg,"�Ǳ����������˺�,���ɰ���!");
        WRITEMSG
        strcpy ( g_pub_tx.reply , "L222" );
        goto ErrExit;
    }


	ret = pub_ln_check_sts( g_ln_mst,0 ); /*�����״̬*/
	if( ret ) goto ErrExit;

	/* �޸����ļ�״̬ */
	ret = Ln_mst_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d ", g_ln_mst.ac_id , g_pub_tx.ac_seqn );
	if (ret)
	{
		sprintf( acErrMsg,"Prepare Ln_mst_Dec_Upd �α����[%d]",ret);
		WRITEMSG
		ERR_DEAL
	}

	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));

	ret = Ln_mst_Fet_Upd( &g_ln_mst , g_pub_tx.reply );
	if (ret==100)
	{
		sprintf( acErrMsg,"�������ļ�������Ӧ��¼[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L014");
		ERR_DEAL
	}else if( ret )
	{
	 		sprintf( acErrMsg,"fetch Ln_mst_Fet_Upd error code = [%d]",ret);
	 		WRITEMSG
	 		strcpy( g_pub_tx.reply ,"D103" );
	 		ERR_DEAL
 	}
	ori_ac_sts=g_ln_mst.ac_sts[0];
	strcpy( g_ln_mst.ac_sts ,"1");

	ret = Ln_mst_Upd_Upd( g_ln_mst , g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg,"Ln_mst_Upd_Upd ����[%d]",ret);
		WRITEMSG
	 	strcpy( g_pub_tx.reply ,"D103" );
		ERR_DEAL
	}

	Ln_mst_Clo_Upd( );

	/* Pub_tx�ṹ��ֵ */

	g_pub_tx.ac_id = g_ln_mst.ac_id;		/* �˻���� */
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
	strcpy ( g_pub_tx.ct_ind , "2" );		/* ��ת:1��2ת */	
	strcpy ( g_pub_tx.prt_ind , "0" );	/*�Ƿ��ӡ����(�������۱�־):1��0�� */
	strcpy ( g_pub_tx.hst_ind , "1" );	/*����ϸ:1�ռ���2���յ�����3���ջ�����*/
	strcpy ( g_pub_tx.brf,"���ɴ���" );

	/*** ���׼��ʴ��� ***/
	if( pub_ins_trace_log())
	{
		sprintf(acErrMsg,"���׼��ʴ������!");
		WRITEMSG
		goto ErrExit;
	}
	/* Ϊ�ڲ��ʽṹ��ֵ */
	memset(ac_no,'\0',sizeof(ac_no));
	set_zd_data("0210",g_ln_parm.cur_no);/* �Ա��ָ�ֵ */
	switch(ori_ac_sts){
		case '2':
			strcpy(ac_no,"13001");
			break;
		case '3':
			strcpy(ac_no,"13003");
			break;
		case '4':
			strcpy(ac_no,"13004");
			break;
		case '5':
			strcpy(ac_no,"13002");
			break;
		default:
			sprintf(acErrMsg,"�����ʻ�״̬������,�������˽���![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L137");
			goto ErrExit;
	}
	set_zd_data("0210",g_ln_parm.cur_no);
	set_zd_data("1201",ac_no);
	set_zd_data("1204",g_ln_parm.cur_no);
	set_zd_data("1205","2");
	set_zd_double("1208",0-g_ln_mst.bal);
	set_zd_data("120A","���ɴ���");

	g_pub_tx.ac_id =''; 
    get_zd_data("1201",g_pub_tx.ac_no);
    get_zd_data("1206",g_pub_tx.note_type);
    get_zd_data("1207",g_pub_tx.beg_note_no);
    get_zd_data("120A",g_pub_tx.brf);
    get_zd_data("1205",g_pub_tx.ct_ind);
    get_zd_data("1204",g_pub_tx.cur_no);

    strcpy( g_pub_tx.ac_id_type,"9" ); /*�˻�����Ϊ�ڲ�*/
    strcpy( g_pub_tx.ac_get_ind,"00" ); /*������δ��ȡ�ֻ�*/
    strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*���˶�ƾ֤�ţ��������ˮ����ϸ*/
    strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no);
    strcpy(g_pub_tx.add_ind,"0");/*�跽��־*/
    strcpy( g_pub_tx.prt_ind,"0" ); /*������*/
    g_pub_tx.svc_ind=9001;  /*�ڲ��ʴ�ȡ*/
    strcpy(g_pub_tx.prt_ind,"0");
    strcpy(g_pub_tx.hst_ind,"1"); /*�ռ�����ϸ*/
	/* ���ڲ��ּ��� */
/*
    ret=pub_acct_trance();
    if( ret )
    {
        sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
        WRITEMSG
        goto ErrExit;
    }
	ret = pubf_acct_proc("A016");
	if(ret!=0)
	{
		sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
		WRITEMSG
		goto ErrExit;
	}
*/


    /* ���ݻ�ƴ���ȡ����������ʱ��ı����Ŀ */
	struct dc_acc_rel_c dc_acc_rel;
	struct com_item_c   com_item;
	memset(&dc_acc_rel,'\0',sizeof(dc_acc_rel));
    ret = Dc_acc_rel_Sel( g_pub_tx.reply , &dc_acc_rel, "dc_code='%s' and data_code='0152'" ,g_ln_parm.dc_code);
    if( ret && ret!=100 )
    {
        sprintf(acErrMsg,"ȡ��Ŀ���쳣����,�������Ļ�����ϵ!");
        WRITEMSG
        strcpy ( g_pub_tx.reply , "L128" );
        goto ErrExit;
    }else if ( ret==100 ){
        sprintf(acErrMsg,"�޴˿�Ŀ��,�������Ļ�����ϵ!");
        WRITEMSG
        strcpy ( g_pub_tx.reply , "L129" );
        goto ErrExit;
    }
    sprintf(acErrMsg,"��Ŀ������[%s]!",dc_acc_rel.acc_hrt);
    WRITEMSG
	memset(&com_item,'\0',sizeof(com_item));
    ret = Com_item_Sel( g_pub_tx.reply,&com_item,"acc_hrt='%s'",dc_acc_rel.acc_hrt);
    if( ret ) goto ErrExit;
    sprintf(acErrMsg,"��Ŀ��[%s]!",com_item.acc_no);
    WRITEMSG
/*
	set_zd_data("0330",com_item.acc_no);
	set_zd_double("0430",0-g_ln_mst.bal);
*/
	/* �������� *
	set_zd_data("1201",com_item.acc_no);
	set_zd_data("1204",g_ln_parm.cur_no);
	set_zd_data("1205","2");
	set_zd_double("1208",0-g_ln_mst.bal);
    get_zd_data("1201",g_pub_tx.ac_no);
	ret = pubf_acct_proc("a016");
	if(ret!=0)
	{
		sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
		WRITEMSG
		goto ErrExit;
	}
*/
	set_zd_data("1211",com_item.acc_no);
	set_zd_double("1218",g_ln_mst.bal);
	ret = pubf_acct_proc("L399");/*���õǼǻ����ˮ*/
	if(ret!=0)
	{
		sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
		WRITEMSG
		goto ErrExit;
	}
/*
	set_zd_data("1211",com_item.acc_no);
	set_zd_data("1214",g_ln_parm.cur_no);
	set_zd_data("1215","2");
	set_zd_double("1218",g_ln_mst.bal);
	set_zd_double("121A","���ɴ���");
*/
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�ɴ����޸ĳɹ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"�ɴ����޸�ʧ��[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
