/*************************************************
* �� �� ��: spL307.c
* ����������   
*              ί��Э��Ǽ�
*
* ��    ��:    lance
* ������ڣ�   2003��01��24��
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
#include "mo_trust_prot_c.h"
	
	static	struct	mo_trust_prot_c		mo_trust_prot;
	static	struct	mo_trust_prot_c		p_mo_trust_prot;
	char 	intst_ac_no[20],ln_acno[20];
	char 	amt_ac_no[20];
	char 	afund_ac_no[20];
	int     ac_seqn;
				
spL307()  
{ 		
	int ret=0;
	long cif_no_back,ac_id_tmp;

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
	/* ��� ί��Э���� */
	ret = Mo_trust_prot_Sel( g_pub_tx.reply , &p_mo_trust_prot , 
					" trust_no='%s' " , p_mo_trust_prot.trust_no );
	if( ret!=0 && ret!=100)
	{
		sprintf(acErrMsg,"��ȡί��Э����Ϣ�쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L065");
		ERR_DEAL;
	}else if ( ret == 0 )
	{
		sprintf(acErrMsg,"ί��Э�����Ѵ���![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L066");
		ERR_DEAL;
	}
	sprintf(acErrMsg,"��� ί��Э���� PASS!");
	WRITEMSG
	
	/* ����˺ŵĺϷ��� */
	/* ��� ί�л��˺� */	
	ret = pub_ln_AcCheck( afund_ac_no , ac_seqn , &cif_no_back , &p_mo_trust_prot.afund_ac_id );
	if( ret )
	{
		sprintf(acErrMsg,"���ú���pub_ln_AcCheck����!");
		WRITEMSG
		ERR_DEAL;	
	}
	sprintf(acErrMsg,"��� ί�л��˺� PASS!");
	WRITEMSG	
	/* ��� ����黹�˺� */	
	ret = pub_ln_AcCheck( amt_ac_no , ac_seqn , &cif_no_back , &p_mo_trust_prot.amt_ac_id );
	if( ret )
	{
		sprintf(acErrMsg,"���ú���pub_ln_AcCheck����!");
		WRITEMSG
		ERR_DEAL;	
	}
	sprintf(acErrMsg,"��� ����黹�˺� PASS!");
	WRITEMSG
	/* ��� ��Ϣ�����ʺ� */	
	ret = pub_ln_AcCheck( intst_ac_no , ac_seqn , &cif_no_back , &p_mo_trust_prot.intst_ac_id );
	if( ret )
	{
		sprintf(acErrMsg,"���ú���pub_ln_AcCheck����!");
		WRITEMSG
		ERR_DEAL;	
	}
	sprintf(acErrMsg,"��� ��Ϣ�����ʺ� PASS!");
	WRITEMSG
	
	
		
	/* �Ǽ�ί��Э��Ǽǲ� */
	strcpy( p_mo_trust_prot.tx_br_no , g_pub_tx.tx_br_no );/* ���׻����� */
	sprintf(acErrMsg,"���׻�����[%s]",p_mo_trust_prot.tx_br_no);
	WRITEMSG
	
	ret = Mo_trust_prot_Ins( p_mo_trust_prot , g_pub_tx.reply );
	if (ret)
	{
		sprintf(acErrMsg,"�Ǽ�ί��Э��Ǽǲ�ʧ��![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L064");
		goto ErrExit;
	}			
	sprintf(acErrMsg,"�Ǽ�ί��Э��Ǽǲ� PASS!");
	WRITEMSG		
			
	/* Pub_tx�ṹ��ֵ */

	strcpy ( g_pub_tx.add_ind , "1" );		/* ����:0��1��2���� */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* ��ת:1��2ת */	
	/* �Ƿ��ӡ����(�������۱�־):1��0�� */
	strcpy ( g_pub_tx.prt_ind , "1" );	
	/* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� */
	strcpy ( g_pub_tx.hst_ind , "0" );
	g_pub_tx.svc_ind = 3060;		
	strcpy( g_pub_tx.ac_wrk_ind , "0100000" );
	strcpy ( g_pub_tx.brf,"ί��Э��Ǽ�" );	
	sprintf(acErrMsg,"Pub_tx�ṹ��ֵ PASS!");
	WRITEMSG
		
	/*** �Ǽǽ�����ˮ ***/
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"�Ǽǽ�����ˮ PASS!");
	WRITEMSG
			

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"ί��Э��Ǽǳɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"ί��Э��Ǽ�ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{	
	memset( &mo_trust_prot , 0x00 , sizeof(struct mo_trust_prot_c) );
	memset( &p_mo_trust_prot , 0x00 , sizeof(struct mo_trust_prot_c) );
	
	return 0;	
}

int data_set_struct()
{    
	int ret=0;
	sprintf(acErrMsg,"���븳ֵ����");
	WRITEMSG
			
	get_zd_data("0620",p_mo_trust_prot.trust_no);		/* ί�б�� */
	sprintf(acErrMsg,"ί�б��[%s]",p_mo_trust_prot.trust_no);
	WRITEMSG
	
	get_zd_data( "0210" , p_mo_trust_prot.cur_no);	/* ���� */
	sprintf(acErrMsg,"����[%s]",p_mo_trust_prot.cur_no);
	WRITEMSG	
	
	get_zd_data("0830",p_mo_trust_prot.name);			/* ί���� */
	sprintf(acErrMsg,"ί����[%s]",p_mo_trust_prot.name);
	WRITEMSG
	
	get_zd_data("0300",afund_ac_no);		/* ί���˻� */
	sprintf(acErrMsg,"ί���˻�[%s]",afund_ac_no);
	WRITEMSG
	pub_base_old_acno(afund_ac_no);
	get_zd_data("0310",amt_ac_no);			/* ί�д����黹�˺� */
	sprintf(acErrMsg,"ί�д����黹�˺�[%s]",amt_ac_no);
	WRITEMSG
	pub_base_old_acno(amt_ac_no);

	get_zd_data("0320",intst_ac_no);		/* ��Ϣ�����ʺ� */
	sprintf(acErrMsg,"��Ϣ�����ʺ�[%s]",intst_ac_no);
	WRITEMSG
	pub_base_old_acno(intst_ac_no);
	
	get_zd_data("0700",p_mo_trust_prot.trust_ind);			/* �Ƿ񹫻������{1-��������� 0-�ǹ�����} */
	sprintf(acErrMsg,"�Ƿ񹫻������[%s]",p_mo_trust_prot.trust_ind);
	WRITEMSG

	get_zd_double("0400",&p_mo_trust_prot.trust_amt);			/* ί�н�� */
	sprintf(acErrMsg,"ί�н��[%lf]",p_mo_trust_prot.trust_amt);
	WRITEMSG

	get_zd_data("0370",ln_acno);			/* ί���˴����˺� */
	sprintf(acErrMsg,"ί���˴����˺�[%s]",ln_acno);
	WRITEMSG
	pub_base_old_acno(ln_acno);
	get_zd_long("0340",&p_mo_trust_prot.ln_ac_seqn);		/* �����˻���� */
	sprintf(acErrMsg,"�����˻����[%d]",p_mo_trust_prot.ln_ac_seqn);
	WRITEMSG
		
	/* ���ݱ��������˻���� */
	ret = pub_base_CurToSeqn( p_mo_trust_prot.cur_no , &ac_seqn );
	if ( ret )
		{
			sprintf(acErrMsg,"���ú������ݱ��������˻���Ŵ���!");
			WRITEMSG
			return 1;	
		}
	sprintf(acErrMsg,"���ݱ��������˻���� PASS![%s]",p_mo_trust_prot.cur_no);
	WRITEMSG
	/* ���ø��ݴ�����ʾ�˺ź��˺����ȡ�������ļ�����*/
	ret = pub_ln_lndis_file(ln_acno,p_mo_trust_prot.ln_ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
	if( ret )
	{
		sprintf(acErrMsg,"��ȡ�����Ϣerror");
		WRITEMSG
		return 1;	
	}
	p_mo_trust_prot.ln_ac_id = g_ln_mst.ac_id ;
	
	sprintf(acErrMsg,"ί���˴����˺�[%ld]",p_mo_trust_prot.ln_ac_id);
	WRITEMSG
	TRACE
							
	return 0;
}

