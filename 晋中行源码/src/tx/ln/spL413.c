/*************************************************
* �� �� ��: spL413.c
* ����������   
*              ί�д��Ϣ
*
* ��    ��:    zgf
* ������ڣ�   2013��05��28��
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
#include "cif_basic_inf_c.h"

int 	ret=0;
long	ln_ic_date=0;	/* �����ϴν������� */ 
long  pri_end_date=0;	/* ��ֹ���� */
long	need_pay_beg_day=0;
char flag_typ[2];	/* �������ͱ�־ */
spL413()  
{ 	
	int ret =0;
	char cif_type[2];
	char clear_sts[2];
	char print_str[31];

			
	struct cif_basic_inf_c cif_basic_inf;
	
	memset( &cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c) );
			
	TRACE
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	sprintf(acErrMsg,"��ʼ��ȫ�ֱ��� PASS!");
	WRITEMSG	
	
	/** ���ݳ�ʼ�� **/
	if ( data_init_struct() )	
		ERR_DEAL;
		
	sprintf(acErrMsg,"���ݳ�ʼ�� PASS!");
	WRITEMSG
		
	/** ȡֵ����ֵ **/
	if ( data_get_struct() )
		ERR_DEAL;
		
	sprintf(acErrMsg,"ȡֵ����ֵ PASS!");
	WRITEMSG
			
	/* �ͻ��ص�--���ܻص� */	
	ret = print_LNHDHZ( );
	if(ret)
	{
		sprintf(acErrMsg,"print_LNHDHZ,ʧ������!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}
	TRACE

OkExit:
    sprintf(acErrMsg,"ί�д��Ϣ�ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
        sprintf(acErrMsg,"ί�д��Ϣʧ��![%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        return 1;
}
    
int data_init_struct()
{
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	
	return 0;
}
int data_get_struct()
{    
	int ret=0;
	double tmp_tx_amt=0.00;

	sprintf(acErrMsg,"���븳ֵ����");
	WRITEMSG
	
	/* ί�д��Ϣ������Ϣ��¼ */
	ret = pay_acm_intst();
	if( ret )
	{
		sprintf(acErrMsg,"����pay_acm_intst�����쳣����,�������Ļ�����ϵ!" );
		WRITEMSG
		if( strlen(g_pub_tx.reply)==0 )
	  {
	     strcpy( g_pub_tx.reply,"L193" );
	  }
		return 1;
	}
	 
  /*���۴���˰���ڲ���D413*/ 
  /*    
  strcpy(g_pub_tx.ac_no,"26505"); 
  get_zd_double("0420",&g_pub_tx.tx_amt1);
	strcpy(g_pub_tx.ct_ind,"2");
	strcpy(g_pub_tx.brf,"ί�л�Ϣ����˰");
  
	strcpy( g_pub_tx.ac_id_type,"9" ); 
  strcpy( g_pub_tx.ac_get_ind,"00" ); 
  strcpy( g_pub_tx.ac_wrk_ind,"000" ); 
  strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no);
  strcpy(g_pub_tx.add_ind,"1");
  strcpy( g_pub_tx.prt_ind,"0" ); 
  g_pub_tx.svc_ind=9001;  
  strcpy(g_pub_tx.hst_ind,"1"); 
	*/
	/***����жϣ�Ϊ0�����л�Ϣ����**/  	
	/*
  ret = pay_in_lo_intst( &g_pub_tx.tx_amt1);
	if( ret )
	{
		sprintf(acErrMsg,"���ý���жϺ����쳣����,�������Ļ�����ϵ!" );
		WRITEMSG
		if( strlen(g_pub_tx.reply)==0 )
	  {
	     strcpy( g_pub_tx.reply,"L193" );
	  }
		return 1;
	}

  ret=pub_acct_trance();
  if( ret )
  {
      sprintf(acErrMsg,"���ô��۴���˰L413����ʧ��!");
      WRITEMSG
      return 1;
  } 
	sprintf(acErrMsg,"���۴���˰L413,���׼��ʴ��� PASS!");
	WRITEMSG 
*/	    
	/*ȡ���D003��ֵ*/
	get_zd_data("1021",g_pub_tx.ac_no);  /*�˺� */
	sprintf(acErrMsg,"ί�д��Ϣȡ���˺�![%s]",g_pub_tx.ac_no);
	WRITEMSG											
	get_zd_double("102F",&g_pub_tx.tx_amt1);  /*���׽�� */
	sprintf(acErrMsg,"ί�д��Ϣȡ����![%.2lf]",g_pub_tx.tx_amt1);
	WRITEMSG
	get_zd_data("1023",g_pub_tx.note_type);  /*ƾ֤���� */
	sprintf(acErrMsg,"ȡ���˺�ƾ֤����![%s]",g_pub_tx.note_type);
	WRITEMSG
	get_zd_data("1024",g_pub_tx.beg_note_no);  /*ƾ֤���� */
	sprintf(acErrMsg,"ȡ���˺�ƾ֤����![%s]",g_pub_tx.beg_note_no);
	WRITEMSG
	/*��˽֤�����͸�ֵ*/
	get_zd_data("102F",g_pub_tx.id_type);  /*֤������ */
	sprintf(acErrMsg,"֤������![%s]",g_pub_tx.id_type);
	WRITEMSG
	get_zd_data("102A",g_pub_tx.id_no);  /*֤������ */
	sprintf(acErrMsg,"֤������![%s]",g_pub_tx.id_no);
	WRITEMSG
	
	set_zd_data( "102J","01" ); /*����*/
	set_zd_data( "102K","2" ); /* ��ת��־*/
	set_zd_data("102B","Y"); 
	set_zd_data("102Y","ί�д�����Ϣ�黹");
	
	/*����D099��ֵ*/
	get_zd_data("1011",g_pub_tx.ac_no);  /*�˺� */
	sprintf(acErrMsg,"ί�д��Ϣ����˺�![%s]",g_pub_tx.ac_no);
	WRITEMSG
	get_zd_double("1013",&g_pub_tx.tx_amt1);  /*��� */
	sprintf(acErrMsg,"ί�д��Ϣ�����![%.2lf]",g_pub_tx.tx_amt1);
	WRITEMSG
	set_zd_int("1012",1);
	set_zd_data("101A","01");
	set_zd_data("101B","2");
	set_zd_data("101C","031");				/***ժҪ***/	
	set_zd_data("1016","ί�л�Ϣ");				/***ժҪ***/	
	
	sprintf(acErrMsg,"ί�д��Ϣ1111111111111 pass");
	WRITEMSG	

	return 0;
}

/* ί�д��Ϣ����жϣ�д����Ǽǲ� */
int pay_in_lo_intst( double *amt )	
{
	double acm_intst=0.00;
	int flag;			/* ����/�����־ */
	
	double bal=0.00;
	
	bal = *amt;

	sprintf(acErrMsg,"����ί�д��Ϣ---���[%lf]",bal);
	WRITEMSG
	
	ret = pub_base_CompDblVal( bal , 0.00 );
	if (ret==0)
	{
		sprintf(acErrMsg,"ί�д��Ϣ���Ϊ�㣬���ù黹." );	
		WRITEMSG		
	        strcpy( g_pub_tx.reply,"L069" );
	        return 1;
	}	
	return 0;
	
}

/* ί�д��Ϣ������Ϣ��¼ */
int pay_acm_intst()	
{

	double acm_intst=0.00;
	int flag;			/* ����/�����־ */
	
	double bal=0.00;
	get_zd_data("1081",g_pub_tx.ac_no);  /*ί�д����˺� */
	get_zd_double("102F",&g_pub_tx.tx_amt1);
	bal = g_pub_tx.tx_amt1;

	sprintf(acErrMsg,"����ί�д��Ϣ---���[%lf]",bal);
	WRITEMSG
	
	ret = pub_base_CompDblVal( bal , 0.00 );
	if (ret==0)
	{
		sprintf(acErrMsg,"ί�д��Ϣ���Ϊ�㣬���ù黹." );	
		WRITEMSG		
	        strcpy( g_pub_tx.reply,"L069" );
	        return 1;
	}
	
	/* ���ݴ����ʺŲ�ѯ�˺Ž��ʹ�ϵ��ȡ���˻�ID */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel , " ac_no='%s' " , g_pub_tx.ac_no );				
	if( ret==100 )
	{
		sprintf( acErrMsg,"ƾ֤(����)���˻���ϵ���޴˼�¼ erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L113" );
		return 1;
	} else if( ret !=0 )
	{
  	sprintf( acErrMsg,"ȡƾ֤(����)���˻���ϵ���쳣erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
    return 1;
  }
	sprintf(acErrMsg,"ȡ�����˻�IDΪ[%ld]",g_mdm_ac_rel.ac_id);
	WRITEMSG	
	
	ret = Ln_mst_Sel( g_pub_tx.reply , &g_ln_mst , " ac_id=%ld " , g_mdm_ac_rel.ac_id );			
	if( ret==100 )
	{
			sprintf( acErrMsg,"�������ļ������޴�ί�д����¼ erro code=[%d]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L002" );
			return 1;
	} else if( ret !=0 )
	{
    		sprintf( acErrMsg,"ȡ�������ļ��쳣 erro code=[%d]",ret);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"D103" );
    		return 1;
  }	
  sprintf(acErrMsg,"ȡ�������ļ�PASS");
	WRITEMSG 
	
	
	/* �������Ϣ�����ж� */
	flag = pub_ln_GetIntstTyp ( g_ln_mst , g_ln_parm );
	sprintf( flag_typ , "%d" , flag+1 ); 
	
	sprintf(acErrMsg,"ly�������Ϣ����[%s]",flag_typ );	
	WRITEMSG	
	
	get_zd_data("1081",g_pub_tx.ac_no);  /*ί�д����˺� */
  sprintf(acErrMsg,"ί�д��Ϣ�����˺�![%s]",g_pub_tx.ac_no);
	WRITEMSG
	strcpy(g_pub_tx.ct_ind,"2");
	strcpy(g_pub_tx.brf,"ί�д�����Ϣ�黹");
  
	strcpy( g_pub_tx.ac_id_type,"3" ); /*�˻�����Ϊ�ڲ�*/
  strcpy( g_pub_tx.ac_get_ind,"00" ); /*������δ��ȡ�ֻ�*/
  strcpy(g_pub_tx.note_type,"299");/*������־*/
  strcpy( g_pub_tx.beg_note_no,"null" ); /*������*/
  strcpy( g_pub_tx.prt_ind,"0" ); /*������*/
  strcpy(g_pub_tx.hst_ind,"1"); /*�ռ�����ϸ*/
  strcpy ( g_pub_tx.ac_wrk_ind , "0000" );/* �������ˮ,��ϸ*/		
	strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
	strcpy ( g_pub_tx.add_ind , "0" );		/* ����:0��1��2���� */	
	g_pub_tx.svc_ind=3011;
	sprintf(acErrMsg,"���˳ɹ��󣬸��´�����л����ֶμ�������������ֶ� START!"); 
	WRITEMSG  
	/*д����Ǽǲ�*/	
	ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"2",bal,g_ln_mst.intst_acm,"0",2 );
	if( ret ) 
		return 1;
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"���׼��ʴ������!");
		WRITEMSG
		return 1;
	}	
	sprintf(acErrMsg,"���׼��ʴ��� PASS!");
	WRITEMSG	
	
	return 0;
}

/* ��ӡ--�ͻ��ص�--��Ϣ�͸������ܻص� */
int  print_LNHDHZ( )	
{
	TRACE
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char over_rate[11];
	TRACE
    	char filename[51],tmpname[101];
    	long start_day=0;
    	char start_day_str[15];
    	double tol_amt=0.00;
     	double lx_tol=0.00;
    	double fl_tol=0.00;     	   	
    	 
    	FILE *fp;
 	TRACE   		
	pub_base_AllDwnFilName( tmpname );
	TRACE
	fp=fopen(tmpname,"a");
    if( fp == NULL )
    {
		sprintf(acErrMsg,"�ļ���:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNHDLX���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
    }
  	TRACE
  	      	
	/* ��ֵ���� */
	strcpy( tx_type , "ί�д��Ϣ" );			

	/* ȷ����ʼ���� */
	sprintf(acErrMsg,"lyȷ����ʼ����:[%ld]!",need_pay_beg_day);
	WRITEMSG	
	start_day = need_pay_beg_day;
	if( start_day )
	{
		sprintf( start_day_str, "%4d-%02d-%02d", start_day/10000, start_day%10000/100, start_day%100 );
	}else{
		strcpy( start_day_str, "");
	}	

	sprintf( rate , "%8.5lf��" , g_ln_mst.rate );
	sprintf( over_rate , "%8.5lf��" , g_ln_mst.over_rate);
	TRACE		
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( over_rate );
	TRACE			
	/*  1.�������͡�  3.��ݺš�  5.��Ʒ���� 	*/
	/*  7.��ʼ���ڡ�  10.�ۼƻ���     */
	/*  12.�������ʡ� 13.�������ʡ�*/
	fprintf(fp,"LNHDHZ%s|%s|%s|%ld|%.2lf|%s|%s|\n",
	tx_type , g_ln_mst.pact_no ,  g_ln_parm.title ,
  g_ln_mst.ic_date  , tol_pay ,
  rate , over_rate );	
	TRACE
vtcp_log("��ô����ʾ�أ�");	
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
	TRACE				
	return 0;
}	