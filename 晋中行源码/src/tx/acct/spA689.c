/*************************************************
* �� �� ��: spL689.c
* ����������   
*              ����Ʒת��
*
* ��    ��:    LiuHuafeng
* ������ڣ�   2008-7-14 11:13:21
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2. 
* 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#define EXTERN
#include "public.h"

static	char	 prod_no[4];
	double old_ln_amt=0.00;
			
spA689()
{ 	

	int ret=0;
	int tag=0;
	int amt_flag=0;
	
	char ln_ac_id[20];
	char acc_hrt[6];
	char wrk_sts[2];
	char type[2];
	long ac_id_t;
	struct	dd_mst_c	dd_mst_new;
	struct	dd_parm_c	dd_parm_new;	
	memset(ln_ac_id,0x00,sizeof(ln_ac_id));
	memset(acc_hrt,0x00,sizeof(acc_hrt));
	memset(wrk_sts,0x00,sizeof(wrk_sts));
	memset(type,0x00,sizeof(type));
	memset ( &dd_mst_new , 0x00 , sizeof( struct dd_mst_c ) );
	memset ( &dd_parm_new , 0x00 , sizeof( struct dd_parm_c ) );
	
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
	sprintf(acErrMsg,"ac_id[%ld] [%d] prdt_no=%s",g_dd_mst.ac_id,g_dd_mst.ac_seqn,g_dd_mst.prdt_no );
	WRITEMSG	
	
	sprintf(acErrMsg,"ac_id[%ld] [%d]",g_dd_mst.ac_id,g_dd_mst.ac_seqn );
	WRITEMSG	

	old_ln_amt = g_dd_mst.bal ;
	
	/* ȡ�²�Ʒ��Ϣ */
	ret = Dd_parm_Sel( g_pub_tx.reply , &dd_parm_new, "prdt_no='%s'", prod_no);		
	if( ret==100 )
	{
		sprintf( acErrMsg,"����Ʒ�������޴˼�¼ erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
		goto ErrExit;
	}
	else if( ret !=0 )
	{
		sprintf( acErrMsg,"ȡ����Ʒ�������쳣 erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
		goto ErrExit;
	}
	sprintf(acErrMsg,"ȡ����Ʒ������Ϣ PASS!");
	WRITEMSG		
	sprintf(acErrMsg,"313�����ִ���Ʒ[%s][%s]",g_dd_parm.prdt_no , dd_parm_new.prdt_no);
	WRITEMSG	

	if( g_dd_mst.ac_sts[0]!='1'  )
	{
		sprintf(acErrMsg,"����ʻ�״̬������,�������˽���![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L137");
		goto ErrExit;
	}
	set_zd_data("0210","01");	/* ���� */
	
	/* ��Ʒ�ű仯 */
	if( strcmp( g_dd_parm.prdt_no, dd_parm_new.prdt_no) )
	{
		set_zd_double("1074",-g_dd_mst.bal);
		tag++;
	}
	if( tag==0 )
	{
		sprintf(acErrMsg,"�����ִ���Ʒ������ת��,����![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L140");
		goto ErrExit;
	}
		/**add by zhaoyue 20141119 ���������������˻�����������Ʒת��**/
	  ret = sql_count("trace_log", "ac_id=%ld and ac_seqn=%d and tx_date=%ld and add_ind in('0','1') and amt<>0.00 ", g_dd_mst.ac_id, g_dd_mst.ac_seqn, g_pub_tx.tx_date);
	  if(ret>0){
		sprintf(acErrMsg,"���˻�������������������������Ʒת��!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"A041");
		goto ErrExit;
	}
	/**add by zhaoyue 20141119 end**/
	
	
	/* Pub_tx�ṹ��ֵ */
	g_pub_tx.tx_amt1 = -g_dd_mst.bal;
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
	
	strcpy ( g_pub_tx.brf,"���ڲ�Ʒת��" );
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
    sprintf(acErrMsg,"����Ʒת���ɹ���[%s]",g_pub_tx.reply);
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
	return 0;
}
