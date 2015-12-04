/*************************************************
* �� �� ��: spA688.c
* ����������   
*              �Թ����ڲ�Ʒת�� 
*
* ��    ��:    LiuHuafeng
* ������ڣ�   2008-7-13 8:42:40
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2. 
TX_CODE	TX_NAME	BIT_MAP	LOG_IND	TX_TYPE	WRK_STS	YW_CNT_IND	YW_PERCENT
3319	����Ʒת��	10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000	1	4	2	3	
TX_CODE	FLOW_CODE	SUB_TX_CODE	SUB_TX_RSLT
3319	0	A689	#=0000@1@#$
3319	1	A688	#$
SUB_TX_CODE	SUB_TX_NAME	TX_FUNC	ACCT_IND	CHNL_IND
A668	����Ʒת��	spA668	1	0
A669	����Ʒת��	spA669	1	0
SUB_TX_CODE	DC_CODE	ENTRY_CODE
A688	0000	A68800
A689	0000	A68900

ENTRY_CODE	SEQN	CUR_NO	ACC_HRT	DC_IND	USE_BR_IND	CT_TYPE	CT_IND	AMT_NO	BRF
A68800	1	0210	@0152	2	1	1	2	1074	
A68900	1	0210	@0152	2	1	1	2	1074	
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_prdt_rel_c.h"

static	long	 end_date_new;
static	char	 prod_no[4];
		
spA688()  
{ 	
	
	int ret=0;
	int tag=0;
	int amt_flag=0;
	
	char ln_ac_id[20];
	char acc_hrt[6];
	char wrk_sts[2];
	char type[2];
	long ac_id_t;
	memset(ln_ac_id,0x00,sizeof(ln_ac_id));
	memset(acc_hrt,0x00,sizeof(acc_hrt));
	memset(wrk_sts,0x00,sizeof(wrk_sts));
	memset(type,0x00,sizeof(type));

	struct	dd_parm_c	dd_parm_new;	
	struct	cif_basic_inf_c	cif_basic_inf;	
	struct	cif_prdt_rel_c	cif_prdt_rel;	
			
	memset ( &dd_parm_new , 0x00 , sizeof( struct dd_parm_c ) );
	memset ( &cif_basic_inf , 0x00 , sizeof( struct cif_basic_inf_c ) );
	memset ( &cif_prdt_rel , 0x00 , sizeof( struct cif_prdt_rel_c ) );
				
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	sprintf(acErrMsg,"��ʼ��ȫ�ֱ��� PASS!");
	WRITEMSG	
	
	/** ���ݳ�ʼ�� **/
	if ( data_init_struct() )	ERR_DEAL;
	sprintf(acErrMsg,"���ݳ�ʼ�� PASS!");
	WRITEMSG
		
	/** ȡֵ����ֵ **/
	if ( data_set_struct() )        ERR_DEAL;
	sprintf(acErrMsg,"ȡֵ����ֵ PASS!");
	WRITEMSG
		
	/** �������� **/
	
	/* ȡ�ϲ�Ʒ��Ϣ */
	ret = pub_dd_dddis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
			&g_mdm_ac_rel,&g_dd_mst,&g_dd_parm ); /*��ȡ����Ϣ*/
	if( ret )
	{
		sprintf(acErrMsg,"��ȡ����Ϣerror");
		WRITEMSG
		goto ErrExit;
	}

	sprintf(acErrMsg,"ac_id[%ld] [%d]",g_dd_mst.ac_id,g_dd_mst.ac_seqn );
	WRITEMSG

	/* ȡ�²�Ʒ��Ϣ */
	ret = Dd_parm_Sel( g_pub_tx.reply , &dd_parm_new, "prdt_no='%s'", prod_no);		
	if( ret==100 )
	{
		sprintf( acErrMsg,"����Ʒ�������޴˼�¼ erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
		return 1;
	}else if( ret ){
    		sprintf( acErrMsg,"ȡ����Ʒ�������쳣 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
	    		return -1;
    	}
	sprintf(acErrMsg,"ȡ����Ʒ������Ϣ PASS!");
	WRITEMSG		
	sprintf(acErrMsg,"�����ִ���Ʒ[%s][%s]",g_dd_parm.prdt_no , dd_parm_new.prdt_no);
	WRITEMSG
			
	if( g_dd_mst.ac_sts[0]!='1'  )
	{
		sprintf(acErrMsg,"�ʻ�״̬������,�������˽���![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L137");
		goto ErrExit;
	}
	TRACE
	/* �ͻ����ͱ�����ͬ */
	/* ���ݿͻ��Ų�ѯ�ͻ����� */			  
	ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
					" cif_no=%ld " , g_dd_mst.cif_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"��֤����Ӧ�Ŀͻ��Ų�����[%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C020");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"ȡ�ͻ�������Ϣ�쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		goto ErrExit;
	}				
	sprintf(acErrMsg,"�����ִ���Ʒ[%s][%s]",g_dd_parm.prdt_no , dd_parm_new.prdt_no);
	WRITEMSG
	TRACE
	sprintf(acErrMsg,"�����ִ���Ʒ[%s][%s]",g_dd_parm.prdt_no , dd_parm_new.prdt_no);
	WRITEMSG
				
	set_zd_data("0210","01");	/* ���� */
	sprintf(acErrMsg,"�����ִ���Ʒ[%s][%s]",g_dd_parm.prdt_no , dd_parm_new.prdt_no);
	WRITEMSG

	WRITEMSG
	/* ���ޱ仯 */
	sprintf(acErrMsg,"�����ִ���Ʒ[%s][%s]",g_dd_parm.prdt_no , dd_parm_new.prdt_no);
	WRITEMSG	
	get_zd_double("1074",&g_dd_mst.bal);
	g_dd_mst.bal=g_dd_mst.bal * -1;
	set_zd_double("1074",g_dd_mst.bal);		
	
	/* ��Ʒ�ű仯 */
	if( strcmp( g_dd_parm.prdt_no, dd_parm_new.prdt_no) )
	{
		set_zd_double("1074",g_dd_mst.bal);
		tag++;
	}
	if( tag==0 )
	{
		sprintf(acErrMsg,"�����ִ���Ʒ������ת��,����![%s][%s]",g_dd_parm.prdt_no , dd_parm_new.prdt_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L140");
		goto ErrExit;
	}
	/* �ͻ������Ƿ�Ϻ���Ʒ���� */
	/* 20120613 wudw ������Ҫ��Ի����������� */
	/*ret = pub_base_check_cltprdt( cif_basic_inf.type, prod_no );
	if( ret )
	{
		sprintf(acErrMsg,"�¾ɲ�Ʒ�ͻ����Ͳ���ͬ,�������˽���![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L139");
		ERR_DEAL;	
	}		*/
	TRACE

	strcpy( type , "1" );
	strcpy(g_dd_mst.prdt_no,prod_no);	/* ��Ʒ��� */
	
			
	/* ���´�����ļ� */
	ret = Dd_mst_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d" ,g_dd_mst.ac_id , g_dd_mst.ac_seqn );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Dd_mst_Dec_Upd��!ret=[%d]",ret );
		WRITEMSG
		ERR_DEAL
	}
    
	ret = Dd_mst_Fet_Upd( &g_dd_mst,g_pub_tx.reply );   
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "������ļ����޴˼�¼" );
		WRITEMSG
		ERR_DEAL
	}else if ( ret ){
		sprintf( acErrMsg, "ִ��Dd_mst_Fet_Upd����!ret=[%d]",ret );
		WRITEMSG
		ERR_DEAL
	}

	strcpy( g_dd_mst.prdt_no , prod_no );   		/* ��Ʒ���� */
	
	ret = Dd_mst_Upd_Upd( g_dd_mst, g_pub_tx.reply );
	if ( ret == -239 )
	{
		sprintf( acErrMsg, "�޸Ĵ�����ļ���,�����ظ���¼!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L031");
		ERR_DEAL
	}else if( ret ){
		sprintf( acErrMsg,"ִ��Dd_mst_Upd_Upd��!ret=[%d]",ret );
		WRITEMSG
		ERR_DEAL
	}
	Dd_mst_Clo_Upd();
	sprintf(acErrMsg,"���´�����ļ� PASS!");
	WRITEMSG
	
	/*  �޸Ŀͻ�-��Ʒ��ϵ�� */
	ret = Cif_prdt_rel_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and cif_no=%ld" ,g_dd_mst.ac_id , g_dd_mst.cif_no );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Cif_prdt_rel_Dec_Upd��!ret=[%d]",ret );
		WRITEMSG
		ERR_DEAL
	}
    
	ret = Cif_prdt_rel_Fet_Upd( &cif_prdt_rel,g_pub_tx.reply );   
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "�ͻ�-��Ʒ��ϵ�����޴˼�¼" );
		WRITEMSG
		Cif_prdt_rel_Clo_Upd();
	}else if ( ret ){
		sprintf( acErrMsg, "ִ��Cif_prdt_rel_Fet_Upd����!ret=[%d]",ret );
		WRITEMSG
		ERR_DEAL
	}else if( !ret ){

		strcpy( cif_prdt_rel.prdt_code , prod_no );   		/* ��Ʒ���� */
			
		ret = Cif_prdt_rel_Upd_Upd( cif_prdt_rel, g_pub_tx.reply );
		if ( ret == -239 )
		{
			sprintf( acErrMsg, "�޸Ŀͻ�-��Ʒ��ϵ���,�����ظ���¼!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L149");
			ERR_DEAL
		}else if( ret ){
			sprintf( acErrMsg,"ִ��Cif_prdt_rel_Upd_Upd��!ret=[%d]",ret );
			WRITEMSG
			ERR_DEAL
		}
		Cif_prdt_rel_Clo_Upd();
		sprintf(acErrMsg,"���¿ͻ�-��Ʒ��ϵ�� PASS!");
		WRITEMSG		
	}
	
	/* Pub_tx�ṹ��ֵ */
	get_zd_double("1074",&g_dd_mst.bal);
	g_pub_tx.tx_amt1 = g_dd_mst.bal;
	g_pub_tx.ac_id = g_dd_mst.ac_id;		/* �˻�ID */
	g_pub_tx.ac_seqn = g_dd_mst.ac_seqn;	/* �˻���� */
	strcpy ( g_pub_tx.ac_id_type , "1" );	/* �˺����� 1-�Թ����� 3-��� */
	strcpy ( g_pub_tx.intst_type , g_dd_mst.intst_type );	/* ��Ϣ���� */	
	strcpy ( g_pub_tx.add_ind , "1" );		/* ����:0��1��2���� */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* ��ת:1��2ת */	
	/* �Ƿ��ӡ����(�������۱�־):1��0�� */
	strcpy ( g_pub_tx.prt_ind , "1" );	
	/* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� 
	strcpy ( g_pub_tx.hst_ind , "1" );*/
	strcpy ( g_pub_tx.hst_ind , "0" );/*���� ���ݽ������� modify by martin 2009/12/21 10:00:18*/
	g_pub_tx.svc_ind = 1001;			/* ����ת */
	strcpy( g_pub_tx.ac_wrk_ind , "0100000" ); 
	strcpy ( g_pub_tx.brf,"���ڲ�Ʒת��(����)" );
	sprintf(acErrMsg,"Pub_tx�ṹ��ֵ PASS!");
	WRITEMSG
	
	/*** �Ǽǽ�����ˮ ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"�Ǽǽ�����ˮ PASS!");
	WRITEMSG
			

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"����Ʒת��ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"����Ʒת��ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    if ( strcmp( g_pub_tx.reply , "L018" )==0 )
    {
    	strcpy( g_pub_tx.reply , "L115" );
    }
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{	
	return 0;	
}

int data_set_struct()
{    
	int ret=0;
	sprintf(acErrMsg,"���븳ֵ����");
	WRITEMSG
	
	get_zd_data("0310" , g_pub_tx.ac_no); 		/* ����˺� */
	g_pub_tx.ac_seqn = 1;				/* �˺���� */

	get_zd_data("0240" , prod_no); 			/* �²�Ʒ���� */
	sprintf(acErrMsg,"��������=[%ld]",end_date_new);	
	WRITEMSG						
	return 0;
}
