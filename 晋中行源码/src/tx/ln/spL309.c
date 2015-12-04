/*************************************************
* �� �� ��: spL309.c
* ����������   
*              ��������ά��
*
* ��    ��:    lance
* ������ڣ�   2003��03��21��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#include "pubf_data_code.h"
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "mdm_prdt_rel_c.h"

static struct ln_mst_c			ln_mst;
static struct ln_mst_c			ln_mst_tmp;
static struct ln_mst_c			p_ln_mst;
static struct mdm_prdt_rel_c	mdm_prdt_rel;
	
static	char ac_no[20];
static	char ac_no_back[20];
	
static	long cif_no;
static	long cif_no_id;
static	long cif_no_back;
	
static	char cif_type_id[2];
static	int ac_seqn;
static	char type[2];
static  char rate_knd[2];
		
spL309()  
{ 	
	
	int ret=0;
	char ln_ac_id[20];
	char acc_hrt[6];
	char wrk_sts[2];
	
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
	/* ���ݱ��������˻���� */
	ret = pub_base_CurToSeqn(g_pub_tx.cur_no , &ac_seqn );
	if ( ret )
		{
			sprintf(acErrMsg,"���ú������ݱ��������˻���Ŵ���!");
			WRITEMSG
			ERR_DEAL;	
		}
	sprintf(acErrMsg,"���ݱ��������˻���� PASS!");
	WRITEMSG
		
	/* ����֤�����ͺ�֤������ȡ�ÿͻ��ż��ͻ����� */
	ret = pub_cif_CifCheck( g_pub_tx.id_type , g_pub_tx.id_no , &cif_no_id , 
							cif_type_id  );
	if ( ret )
		{
			sprintf(acErrMsg,"���ú���pub_cif_CifCheck����!");
			WRITEMSG
			ERR_DEAL;	
		}
	sprintf(acErrMsg, "ȡ���Ŀͻ���Ϊ[%ld] ", cif_no_id );
	WRITEMSG				
	sprintf(acErrMsg,"����֤�����ͺ�֤������ȡ�ÿͻ��ż��ͻ����� PASS!");
	WRITEMSG
		
	/* ��� �����ʺ� */	
	if ( strcmp( ac_no , ac_no_back ))	
  	{
		ret = pub_ln_AcCheck( ac_no_back , ac_seqn , &cif_no_back );
		if( ret )
		{
			sprintf(acErrMsg,"���ú���pub_ln_AcCheck����!");
			WRITEMSG
			ERR_DEAL;	
		}

		if ( cif_no_back != cif_no_id  )
		{
			sprintf(acErrMsg,"����֤������Ŀͻ���������˺��пͻ��Ų���");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L019");
			ERR_DEAL;
		}
	}
	sprintf(acErrMsg,"��� �����ʺ� PASS!");
	WRITEMSG
	
	/* �����˻�ID */
	ret = pub_base_disid_cltid( ac_no_back , &g_ln_mst.repay_ac_id );
	if( ret )
		{
			sprintf(acErrMsg,"������ʾ�˺�ת���˻�ID��������!");
			WRITEMSG
			ERR_DEAL;	
		}	
	sprintf(acErrMsg,"���ú����ó��Ļ����˻�IDΪ[%ld]",g_ln_mst.repay_ac_id);
	WRITEMSG	
	sprintf(acErrMsg,"��� �����˻�ID PASS!");
	WRITEMSG
				
	ret = Ln_mst_Sel( g_pub_tx.reply , &ln_mst_tmp , " ac_id=%ld and ac_seqn=%d " , g_ln_mst.ac_id , g_ln_mst.ac_seqn );		
	if (ret) {return ret;}
	
	/* ��� ��ͬ�� */
	ret = Ln_mst_Sel( g_pub_tx.reply , &ln_mst , 
					" pact_no='%s' " , g_ln_mst.pact_no );
	if( ret!=0 && ret!=100)
	{
		sprintf(acErrMsg,"��ȡ�������ļ���Ϣ�쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L021");
		ERR_DEAL;
	}else if ( ret == 0 )
		{			
			if( strcmp(ln_mst.pact_no , ln_mst_tmp.pact_no) && strcmp( ln_mst.ac_sts , "9" ) )
			{
			sprintf(acErrMsg,"��ͬ���Ѵ���![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L020");
			ERR_DEAL;
			}			
		}
	sprintf(acErrMsg,"��� ��ͬ�� PASS!");
	WRITEMSG
	
	/* �����Ʒ���Լ��(����) */
	strcpy( type , "1" );
	strcpy(g_ln_mst.prdt_no,g_pub_tx.prdt_code);	/* �����Ʒ��� */
	
	ret = pub_ln_check( type );
	if( ret )
		{
			sprintf(acErrMsg,"���ô����Ʒ���Լ�麯������!");
			WRITEMSG
			ERR_DEAL;	
		}
	sprintf(acErrMsg,"�����Ʒ���Լ��(����) PASS!");
	WRITEMSG
			
	/* �ͻ������Ƿ�Ϻ���Ʒ���� */
	ret = pub_base_check_cltprdt( cif_type_id, g_pub_tx.prdt_code );
	if( ret )
		{
			sprintf(acErrMsg,"���ÿͻ������Ƿ�Ϻ���Ʒ���庯������!");
			WRITEMSG
			ERR_DEAL;	
		}	
  	
	g_ln_mst.bal = 0;
	sprintf(acErrMsg,"�ͻ������Ƿ�Ϻ���Ʒ������ PASS!");
	WRITEMSG
				 	
	/* ȡ�����Ʒ������Ϣ */

	ret = Ln_parm_Sel( g_pub_tx.reply , &g_ln_parm, "prdt_no='%s'", 
						g_ln_mst.prdt_no);
		
	if( ret==100 )
	{
		sprintf( acErrMsg,"�����Ʒ�������޴˼�¼ erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
		return 1;
	}else if( ret !=0 )
		{
    		sprintf( acErrMsg,"ȡ�����Ʒ�������쳣 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
    		return -1;
    		}
	sprintf(acErrMsg,"ȡ�����Ʒ������Ϣ PASS!");
	WRITEMSG	
		
	/* ���´������ļ� */
    ret = Ln_mst_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d" ,g_ln_mst.ac_id , g_ln_mst.ac_seqn );
    if ( ret )
    {
        sprintf( acErrMsg, "ִ��Ln_mst_Dec_Upd��!ret=[%d]",ret );
        WRITEMSG
        ERR_DEAL
    }
    
    ret = Ln_mst_Fet_Upd( &g_ln_mst,g_pub_tx.reply );   
    if ( ret == 100 )
    {
        sprintf( acErrMsg, "�������ļ����޴˼�¼" );
        WRITEMSG
        ERR_DEAL
    }else if ( ret )
    {
        sprintf( acErrMsg, "ִ��Ln_mst_Fet_Upd����!ret=[%d]",ret );
        WRITEMSG
        ERR_DEAL
    }

	g_ln_mst.cif_no = cif_no_id;   		/* �ͻ��� */
	g_ln_mst.opn_date = p_ln_mst.opn_date;	/* �������� */
	strcpy( g_ln_mst.pact_no , p_ln_mst.pact_no );	/* ��ͬ�� */
	g_ln_mst.amt_lmt = p_ln_mst.amt_lmt;	/* �ƻ����Ŷ� */
	g_ln_mst.bal = p_ln_mst.bal;	/* �ƻ����Ŷ� */	
	g_ln_mst.rate = p_ln_mst.rate;		/* �������� */	
	g_ln_mst.over_rate = p_ln_mst.over_rate;	/* �������� */
	strcpy(g_ln_mst.repay_type,p_ln_mst.repay_type);	/* ����֧����ʽ */
	strcpy(g_ln_mst.cal_code , p_ln_mst.cal_code);		/* ָ����ϵ���� */		
	g_ln_mst.mtr_date = p_ln_mst.mtr_date;		/* �������� */
	strcpy(g_ln_mst.intst_type,p_ln_mst.intst_type);	/* ��Ϣ���� */	
	strcpy(g_ln_mst.name, p_ln_mst.name);		/* ����� */	
	
	
	ret = Ln_mst_Upd_Upd( g_ln_mst, g_pub_tx.reply );
	if ( ret == -239 )
	{
		sprintf( acErrMsg, "�޸Ĵ������ļ���,�����ظ���¼!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L031");
		ERR_DEAL
	}else if( ret )
		{
			sprintf( acErrMsg,"ִ��Ln_mst_Upd_Upd��!ret=[%d]",ret );
			WRITEMSG
			ERR_DEAL
		}
	Ln_mst_Clo_Upd();
	sprintf(acErrMsg,"���´������ļ� PASS!");
	WRITEMSG	
	
	/* Pub_tx�ṹ��ֵ */
	
	g_pub_tx.cif_no = cif_no_id;			/* �ͻ��� */
	strcpy ( g_pub_tx.ac_no , ln_ac_id );	/* �˺� */
	g_pub_tx.ac_id = g_ln_mst.ac_id;		/* �˻�ID */
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
	strcpy ( g_pub_tx.intst_type , g_ln_mst.intst_type );	/* ��Ϣ���� */	
	strcpy ( g_pub_tx.add_ind , "1" );		/* ����:0��1��2���� */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* ��ת:1��2ת */	
	/* �Ƿ��ӡ����(�������۱�־):1��0�� */
	strcpy ( g_pub_tx.prt_ind , "0" );	
	/* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� */
	strcpy ( g_pub_tx.hst_ind , "0" );

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

	
	/* ��� */



	
	sprintf(acErrMsg,"��� PASS!");
	WRITEMSG	

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"������ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"�����ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	
	memset (&ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&p_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&ln_mst_tmp, 0x00, sizeof(struct ln_mst_c));
	memset (&mdm_prdt_rel, 0x00, sizeof(struct mdm_prdt_rel_c));
	
	return 0;
	
}

int data_set_struct()
{    
	int ret=0;
	sprintf(acErrMsg,"���븳ֵ����");
	WRITEMSG
	
	get_zd_long("0440" , &g_ln_mst.opn_date);	/* �������� */
	get_zd_data("0670",g_pub_tx.id_type);		/* ֤������ */
	get_zd_data("0620",g_pub_tx.id_no);			/* ֤������ */
	get_zd_data("0210",g_pub_tx.cur_no);		/* ���� */	
	get_zd_data("0380",ac_no_back);				/* �����ʺ� */
	get_zd_data("0630",g_ln_mst.pact_no);		/* ��ͬ�� */	
	get_zd_double("0430",&g_ln_mst.amt_lmt);	/* �ƻ����Ŷ� */
	get_zd_double("0430",&g_ln_mst.bal);    	/* �ƻ����Ŷ� */	
	get_zd_double("0840",&g_ln_mst.rate);		/* �������� */	
	get_zd_double("0850",&g_ln_mst.over_rate);	/* �������� */
	get_zd_data("0230",g_pub_tx.prdt_code);		/* ��Ʒ���� */
	get_zd_data("0680",rate_knd);		    	/* ����ȡ�÷�ʽ */
	get_zd_data("0690",g_ln_mst.repay_type);	/* ����֧����ʽ */
	get_zd_data("0610",g_ln_mst.cal_code);		/* ָ����ϵ���� */		
	get_zd_long("0450",&g_ln_mst.mtr_date);		/* �������� */
	get_zd_data("0710",g_ln_mst.intst_type);	/* ��Ϣ���� */	
	get_zd_data("0270",g_ln_mst.name);	    	/* ����� */			
	get_zd_data("0310",ac_no); 	            	/* �����˺� */
	
	/* ���ݴ����ʺŲ�ѯ�˺Ž��ʹ�ϵ��ȡ���˻�ID */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel , " ac_no='%s' " , ac_no );			
	if( ret==100 )
	{
		sprintf( acErrMsg,"ƾ֤(����)���˻���ϵ���޴˼�¼ erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"O001" );
		return 1;
	} else if( ret !=0 )
		{
    			sprintf( acErrMsg,"ȡƾ֤(����)���˻���ϵ���쳣erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
    			return 1;
    	}
	sprintf(acErrMsg,"ȡ�����˻�IDΪ[%ld]",g_mdm_ac_rel.ac_id);
	WRITEMSG
	
	g_ln_mst.ac_id = g_mdm_ac_rel.ac_id;		/* �����˻�ID */	
	get_zd_int("0350",&g_ln_mst.ac_seqn);		/* ������� */	
	get_zd_long("0440" , &p_ln_mst.opn_date); 	/* �������� */
	get_zd_data("0630",p_ln_mst.pact_no);		/* ��ͬ�� */	
	get_zd_double("0430",&p_ln_mst.amt_lmt);	/* �ƻ����Ŷ� */
	get_zd_double("0430",&p_ln_mst.bal);    	/* �ƻ����Ŷ� */	
	get_zd_double("0840",&p_ln_mst.rate);		/* �������� */	
	get_zd_double("0850",&p_ln_mst.over_rate);	/* �������� */
	get_zd_data("0690",p_ln_mst.repay_type);	/* ����֧����ʽ */
	get_zd_data("0610",p_ln_mst.cal_code);		/* ָ����ϵ���� */		
	get_zd_long("0450",&p_ln_mst.mtr_date);		/* �������� */
	get_zd_data("0710",p_ln_mst.intst_type);	/* ��Ϣ���� */	
	get_zd_data("0270",p_ln_mst.name);	    	/* ����� */				
	p_ln_mst.ac_id = g_mdm_ac_rel.ac_id;		/* �����˻�ID */	
	get_zd_int("0350",&p_ln_mst.ac_seqn);		/* ������� */				
	return 0;
}

