/*************************************************
* �� �� ��: spL102.c
* ����������   
*              �ſ�ƻ�
*
* ��    ��:    bob
* ������ڣ�   2003��01��10��
* �޸ļ�¼��   
*     1. ��    ��:2003-03-25
*        �� �� ��:lance
*        �޸�����:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#define EXTERN
#include "public.h"
#include "ln_pay_pln_c.h"

static char dd_ac_no[20];
static int  dd_ac_seqn;

spL102()  
{ 	
	
	int ret=0,flag=0;
	char filename[100];
	char fldstr[6][100];
	double amt=0.00,atm_tmp=0.00;
	long date=0;
	char tmpstr[1024];
	FILE *fp;
	int i=0;
	
    struct ln_pay_pln_c	ln_pay_pln;
    struct ln_pay_pln_c	ln_pay_pln_v;	

	memset (&ln_pay_pln,0x00,sizeof(struct ln_pay_pln_c) );	
	memset (&ln_pay_pln_v,0x00,sizeof(struct ln_pay_pln_c) );	
		
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	sprintf(acErrMsg,"��ʼ��ȫ�ֱ��� PASS!");
	WRITEMSG	
	
	/** ���ݳ�ʼ�� **/
	if ( data_init_struct() )	ERR_DEAL;
	sprintf(acErrMsg,"���ݳ�ʼ�� PASS!");
	WRITEMSG
	
	/** ȡֵ����ֵ **/
	if ( data_set_struct() )    ERR_DEAL;
	sprintf(acErrMsg,"ȡֵ����ֵ PASS!");
	WRITEMSG	
	
	sprintf(acErrMsg,"��ȡ�����Ϣ[%s][%d]",g_pub_tx.ac_no,g_pub_tx.ac_seqn);
	WRITEMSG		
	/* ���ø��ݴ�����ʾ�˺ź��˺����ȡ�������ļ�����*/
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
						&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
	if( ret )
	{
		sprintf(acErrMsg,"��ȡ�����Ϣerror");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"ac_id[%d] [%d]",g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	WRITEMSG
	
	/* �жϴ����Ƿ�������״̬ */
	if ( g_ln_mst.ac_sts[0]!='1' )
	{
		sprintf(acErrMsg,"����ڷ�����״̬,�������˽���!",
										g_ln_parm.pay_pln_type);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L079");
		goto ErrExit;
	}
		
	/* �жϷſ�ƻ����� N�� M�ֶ� */
	if ( g_ln_parm.pay_pln_type[0]!='M' )
	{
		sprintf(acErrMsg,"�ſ�ƻ����ͷ��ֶ�[%s],�������˽���!",
										g_ln_parm.pay_pln_type);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L075");
		goto ErrExit;
	}	
	/* �����¼ */
	ret = Ln_pay_pln_Dec_Sel( g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d and \
								pay_ind='2'",g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	if( ret==0 )
	{
		ret=sql_execute("delete from ln_pay_pln where ac_id=%ld and \
				ac_seqn=%d and pay_ind='2'",g_ln_mst.ac_id,g_ln_mst.ac_seqn);
		if(ret)		goto ErrExit;
	}else if( ret&&ret!=100 )
	{
		goto ErrExit;
	}
	/* ȡ�������˻�ID */
	ret = pub_base_currentdis_file(dd_ac_no,dd_ac_seqn,&g_dd_mst);
	if( ret )
	{	
		sprintf(acErrMsg,"ȡ�������ļ���������");
		WRITEMSG
		goto ErrExit;
	}	
	Gl_file_first=0;	
	pub_base_AllDwnFilName( filename );
	fp = fopen( filename,"r" );
	if( fp==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename );
		strcpy( g_pub_tx.reply,"S047" );
	}
	while(1)
	{
		fgets( tmpstr,1024,fp );
		if( feof(fp) ) break;

		for( i=0;i<6;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
				vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
		}
		atm_tmp+=atof(fldstr[4]);
		amt=atof(fldstr[4]);
		date=atol(fldstr[3]);
		vtcp_log("%.2lf,%.2lf,%s",amt,atof(fldstr[4]),fldstr[4]);
		
		/* ������Ƿ��� */
		ret=pub_base_CompDblVal(g_ln_mst.amt_lmt,g_ln_mst.ttl_prvd_amt+atm_tmp);
		if(ret<0)
		{
			sprintf(acErrMsg,"�ƻ��ſ������,��������ſ��޶�![%lf][%lf]",
								g_ln_mst.amt_lmt,g_ln_mst.ttl_prvd_amt+atm_tmp);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L076");
			goto ErrExit;
		}
		ln_pay_pln.ac_id=g_ln_mst.ac_id;
		ln_pay_pln.ac_seqn=g_ln_mst.ac_seqn;
		ln_pay_pln.de_ac_id=g_dd_mst.ac_id;
		ln_pay_pln.de_ac_seqn=dd_ac_seqn;
		ln_pay_pln.ttl_amt=amt;
		ln_pay_pln.wrk_date=g_pub_tx.tx_date;
		ln_pay_pln.beg_date=date;
		strcpy(ln_pay_pln.pay_ind,"2");
																	
		/* ��ӻ���ƻ��� */	
		ret = Ln_pay_pln_Ins( ln_pay_pln , g_pub_tx.reply );
		if(ret)
		{
			sprintf(acErrMsg,"�ǼǷſ�ƻ���ʧ��");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L077");
			goto ErrExit;
		}
		/* Pub_tx�ṹ��ֵ */
		strcpy ( g_pub_tx.ac_wrk_ind,"0" );
		g_pub_tx.tx_amt1 = 0.00 ;
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
		strcpy ( g_pub_tx.add_ind , "1" );		/* ����1�� */	
		strcpy ( g_pub_tx.ct_ind , "2" );		/* ��ת:1��2ת */	
		strcpy ( g_pub_tx.prt_ind , "0" );	/* �Ƿ��ӡ����(�������۱�־)0�� */
		strcpy ( g_pub_tx.hst_ind , "1" );  /* ����ϸ�ʱ�־:1�ռ��� 2���յ����� 															3���ջ����� */
		g_pub_tx.svc_ind = 3060 ;
		strcpy( g_pub_tx.brf,"����ſ�ƻ�" );
		sprintf(acErrMsg,"Pub_tx�ṹ��ֵ PASS!");
		WRITEMSG
	
		/*** ���׼��ʴ��� ***/
		if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"���׼��ʴ������!");
			WRITEMSG
			goto ErrExit;
		}
		sprintf(acErrMsg,"���׼��ʴ��� PASS!");
		WRITEMSG

	}
	vtcp_log("amt [%.2lf]",amt);	
	fclose(fp);
	
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�ſ�ƻ��ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"�ſ�ƻ�ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	
	return 0;
	
}

int data_set_struct()
{    
	sprintf(acErrMsg,"���븳ֵ����");
	WRITEMSG
	
	g_ln_mst.opn_date = g_pub_tx.tx_date;	/* �������� */
	get_zd_data("0370",g_pub_tx.ac_no);		/* �����˺� */
	get_zd_int("0340",&g_pub_tx.ac_seqn);	/* �����˺���� */
	get_zd_data("0310",dd_ac_no);			/* ����˺� */
	get_zd_int("0350",&dd_ac_seqn);			/* ����˺���� */					
	return 0;
}

