/*************************************************
* �� �� ��: spL202.c
* ����������   
*              ���Ϣ
*
* ��    ��:    lance
* ������ڣ�   2003��07��28��
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
#include "ln_acm_reg_c.h"
#include "cif_basic_inf_c.h"

char	repay_ind[9];
char 	ct_flag[2];
int 	ret=0;
int 	in_lo=0,out_lo=0,cmpd_lo=0,bal_acm=0;
int 	cmpd_acm=0,bal_acm_in=0,bal_acm_out=0;
double 	amt_in_lo=0.00 , amt_out_lo=0.00, amt_cmpd_lo=0.00,amt_bal_acm=0.00;
double	amt_cmpd_acm=0.00,amt_bal_acm_in=0.00,amt_bal_acm_out=0.00;
long	ln_ic_date=0;	/* �����ϴν������� */
long	in_lo_beg_date=0 , out_lo_beg_date=0 , cmpd_lo_beg_date=0; 
long	bal_acm_beg_date=0, cmpd_acm_beg_date=0 ,acm_beg_date=0;
char	acm_ind[2];
long    pri_end_date=0;	/* ��ֹ���� */
long	need_pay_beg_day=0;
double	print_tol_amt=0.00;
char flag_typ[2];	/* �������ͱ�־ */

	struct ln_acm_reg_c ln_acm_reg;

spL202()  
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
	if ( data_init_struct() )	ERR_DEAL;
	sprintf(acErrMsg,"���ݳ�ʼ�� PASS!");
	WRITEMSG
		
	/** ȡֵ����ֵ **/
	if ( data_get_struct() )        ERR_DEAL;
	sprintf(acErrMsg,"ȡֵ����ֵ PASS!");
	WRITEMSG
	
	/* ȡ�ͻ����� */
	ret=Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf,"cif_no=%ld",
														g_ln_mst.cif_no);
	if( ret )
	{
		goto ErrExit;
	}
	if( cif_basic_inf.type[0]=='1' || cif_basic_inf.type[0]=='5' )
	{
		strcpy( cif_type , "0" );	/* ��˽ */
	}else{
		strcpy( cif_type , "1" );	/* �Թ� */
	}		
	set_zd_data("108P" , cif_type);	
	set_zd_double("108L",g_pub_tx.tmp_amt1);	/* �ܻ����� */
	sprintf(acErrMsg,"����˳��108J[%s]������[%.2lf]",repay_ind,
													g_pub_tx.tmp_amt1);
	WRITEMSG

	/* ��Ϣ���� */
	ret = ln_pay_seq( repay_ind , &g_pub_tx.tmp_amt1 );
	if( ret )
	{
		sprintf(acErrMsg,"���û�Ϣ�����쳣����,�������Ļ�����ϵ!" );
		WRITEMSG
		goto ErrExit;
	}	
	sprintf(acErrMsg,"���û�Ϣ����PASS!" );
	WRITEMSG	
	
	/* �ж����� */	
	if( pub_base_CompDblVal( g_pub_tx.tmp_amt1 , 0.00 ) )
	{
		sprintf(acErrMsg,"���������Ƿ����,����!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L205");
		goto ErrExit;
	}		
	
	/* ����Ƿ��� */
	ret = pub_ln_chk_clear(g_ln_mst,g_ln_parm,clear_sts);
	if(ret)	goto ErrExit;
	if( pub_base_CompDblVal( g_ln_mst.gage_amt , 0.00 ) )
	{			
		set_zd_data("108O",clear_sts);
		/* �����е�ѺƷ */
	}else{
		set_zd_data("108O","*");
		/* �����е�ѺƷ */
	}
	
	set_zd_data("0210",g_ln_parm.cur_no);/* �Ա��ָ�ֵ */

	/* ��ӡ���� */
	if( acm_ind[0]=='1' )
	{
		pri_end_date = g_pub_tx.tx_date;
	}else{
		pri_end_date = g_ln_mst.ic_date;
	}
			
	/* ����ʼ���ڸ�ֵ ���� ��Ϊ99999999 ���򲻱� */
	if( !in_lo_beg_date )
	{
		in_lo_beg_date = 99999999;
	}
	if( !out_lo_beg_date )
	{
		out_lo_beg_date = 99999999;
	}
	if( !cmpd_lo_beg_date )
	{
		cmpd_lo_beg_date = 99999999;
	}
	if( !bal_acm_beg_date )
	{
		bal_acm_beg_date = 99999999;
	}
	if( !cmpd_acm_beg_date )
	{
		cmpd_acm_beg_date = 99999999;
	}
	if( !acm_beg_date )
	{
		acm_beg_date = 99999999;
	}
	/* ������������ֵΪ0  �������в���ȷ��beg_dateΪ19000101*/
	if( in_lo_beg_date==19000101 )
	{
		in_lo_beg_date = 0;
	}
	if( out_lo_beg_date==19000101 )
	{
		out_lo_beg_date = 0;
	}
	if( cmpd_lo_beg_date==19000101 )
	{
		cmpd_lo_beg_date = 0;
	}
	if( bal_acm_beg_date==19000101 )
	{
		bal_acm_beg_date = 0;
	}
	if( cmpd_acm_beg_date==19000101 )
	{
		cmpd_acm_beg_date = 0;
	}
	if( acm_beg_date==19000101 )
	{
		acm_beg_date = 0;
	}

	sprintf(acErrMsg,"in_lo_beg_date[%ld]",in_lo_beg_date );
	WRITEMSG
	sprintf(acErrMsg,"out_lo_beg_date[%ld]",out_lo_beg_date );
	WRITEMSG
	sprintf(acErrMsg,"cmpd_lo_beg_date[%ld]",cmpd_lo_beg_date );
	WRITEMSG
	sprintf(acErrMsg,"bal_acm_beg_date[%ld]",bal_acm_beg_date );
	WRITEMSG
	sprintf(acErrMsg,"cmpd_acm_beg_date[%ld]",cmpd_acm_beg_date );
	WRITEMSG
	sprintf(acErrMsg,"acm_beg_date[%ld]",acm_beg_date );
	WRITEMSG			

	/* ת�˽��˵�,�ֽ�ɿ */
	ret = print_LNDF( );
	if(ret)		
	{
		sprintf(acErrMsg,"print_LNDF,ʧ������!" );
		WRITEMSG		
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}
	TRACE
	
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
	
	/* �ͻ��ص�--��Ϣ�ص� *
	if( pub_base_CompDblVal( in_lo+out_lo+bal_acm , 0.00 ) )* �黹����ǷϢ������ǷϢ�����������Ϣ *	
	{	
		ret = print_LNHDLX( );
		if(ret)
		{
			sprintf(acErrMsg,"print_LNHDLX,ʧ������!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L207");
			goto ErrExit;
		}
	}
	TRACE
	*******/
	
	/* �ͻ��ص�--�����ص� *
	if( pub_base_CompDblVal( cmpd_lo+cmpd_acm , 0.00 ) )	* �黹����ǷϢ������������Ϣ *
	{
		ret = print_LNHDFL( );		
		if(ret)
		{
			sprintf(acErrMsg,"print_LNHDFL,ʧ������!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L207");
			goto ErrExit;
		}
	}
	TRACE
	*****/
	
	/* ���黹�˱������ */
	if( bal_acm )
	{
		if( flag_typ[0]=='2' )	/* ����� */
		{
			bal_acm_in = 1;
		}else if ( flag_typ[0]=='3' ){	/* ����� */
			bal_acm_out = 1;			
		}
	}
	TRACE	
	/* ���ڴ�Ʊ--���ڲ��� */
	if(pub_base_CompDblVal(in_lo+bal_acm_in, 0.00))	/*�黹����ǷϢ�������������� */	
	{
		ret = print_LNBKBBN( );			
		if(ret)
		{
			sprintf(acErrMsg,"print_LNBKBBN,ʧ������!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L207");
			goto ErrExit;
		}
	}	
	TRACE	
	/* ���ڴ�Ʊ--���ⲿ�� */
	if( pub_base_CompDblVal(out_lo+bal_acm_out, 0.00))/*�黹����ǷϢ�������������� */	
	{
		ret = print_LNBKBBW( );			
		if(ret)
		{
			sprintf(acErrMsg,"print_LNBKBBW,ʧ������!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L207");
			goto ErrExit;
		}
	}
	TRACE	
	/* ���ڴ�Ʊ--�������� */
	if( pub_base_CompDblVal(cmpd_lo+cmpd_acm, 0.00))/*�黹����ǷϢ����������*/	
	{
		ret = print_LNBKBFL( );			
		if(ret)
		{
			sprintf(acErrMsg,"print_LNBKBFL,ʧ������!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L207");
			goto ErrExit;
		}
	}
	TRACE
	/* ���ڴ�Ʊ-������Ϣ�� */
	if( pub_base_CompDblVal( bal_acm+cmpd_acm , 0.00 ))	/* �������,�������� */
	{
		ret = print_LNJS( );				
		if(ret)
		{
			sprintf(acErrMsg,"print_LNJS,ʧ������!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L207");
			goto ErrExit;
		}
	}	

	/*pub_mob_sendmail_ln("�黹��Ϣ",g_pub_tx.tx_code,g_mdm_ac_rel.ac_no,g_pub_tx.add_ind,print_tol_amt,0.0,0.0);*/
	pub_mob_sendmail_ln("�黹��Ϣ",g_pub_tx.tx_code,g_ln_mst.pact_no,g_pub_tx.add_ind,print_tol_amt,0.0,0.0);
	
	TRACE	
	if( ct_flag[0]=='1' )	/** �ֽ� **/
		goto ErrExit1;
	else if( ct_flag[0]=='2' ) /*ת��*/
		goto ErrExit2;
    else if (ct_flag[0] == '3')		/*���*/
        goto ErrExit3;
    else if (ct_flag[0] == '8')		/*�ۻ�*/
        goto ErrExit8;

ErrExit:
    sprintf(acErrMsg,"���Ϣʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
ErrExit1:
	strcpy(g_pub_tx.reply,"0001");
	sprintf(acErrMsg,"���Ϣ�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit2:
	strcpy(g_pub_tx.reply,"0002");
	sprintf(acErrMsg,"���Ϣ�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit3:
    strcpy(g_pub_tx.reply,"0003");
    sprintf(acErrMsg,"���Ϣ�ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;	
ErrExit8:
    strcpy(g_pub_tx.reply,"0008");
    sprintf(acErrMsg,"���Ϣ�ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;	
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

	sprintf(acErrMsg,"���븳ֵ����");
	WRITEMSG
	
	get_zd_data("1081",g_pub_tx.ac_no);			/* �ʺ� */
	get_zd_int("1082",&g_pub_tx.ac_seqn);		/* �˻���� */
	get_zd_data("108A",g_pub_tx.tmp_ind);			/* ���汾�� */
	get_zd_double("108B",&g_pub_tx.tmp_amt1);			/* ������ */
	print_tol_amt = g_pub_tx.tmp_amt1 ;
	get_zd_data("108C",g_pub_tx.note_type);			/* ƾ֤���� */
	get_zd_data("108D",g_pub_tx.beg_note_no);			/* ��ʼƾ֤���� */
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no );
	get_zd_data("108J",repay_ind);
	sprintf(acErrMsg,"˳��[%s]",repay_ind);
	WRITEMSG
	
	get_zd_data("0660",g_pub_tx.ct_ind);			/* ��ת��־ */
	get_zd_data("0660",ct_flag);				/* ��ת��־ */	
	sprintf(acErrMsg,"��ת��־![%s]",g_pub_tx.ct_ind);
	WRITEMSG

	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
		&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
	if( ret ) 
	{
		sprintf(acErrMsg,"��ȡ�����Ϣerror");
		WRITEMSG
		return ret;
	}
	ln_ic_date = g_ln_mst.ic_date;		/* ��Ϣ���� */
	
	/* ȷ������ǷϢ���� */
	ret = sql_min_long( "ln_reg" , "reg_date_beg" , &need_pay_beg_day , "ac_id=%ld and ac_seqn=%d  and type in('2','3','4') and sts='0' " , g_ln_mst.ac_id,g_ln_mst.ac_seqn);
	if( ret )	return 1;
	
	/* ��Ϊ19000101,���������� */
	if( need_pay_beg_day==19000101 )
	{
		need_pay_beg_day = 0;
	}
	sprintf(acErrMsg,"need_pay_beg_day=[%ld]",need_pay_beg_day);
	WRITEMSG	
		
	/* �޸���С�黹���Ϊ 0.00 */
	g_ln_parm.min_intst = 0.00 ;

	/*ret = pub_base_check_mdm_sts( g_mdm_ac_rel ) *������״̬*
	if( ret ) return ret; */

	ret = pub_ln_check_sts( g_ln_mst,0 ); /*�����״̬*/
	if( ret ) 
	{
		sprintf(acErrMsg,"�����״̬error");
		WRITEMSG
		return ret;
	}
					
	if( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
	{
		sprintf(acErrMsg,"���Ҵ���ڴ˴���![%s]",g_ln_parm.ln_pay_type);
		WRITEMSG
		strcpy( g_pub_tx.reply,"L038" );
		return 1;
	}

	g_pub_tx.ac_id = g_ln_mst.ac_id;	/* �˻���� */
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */
	strcpy ( g_pub_tx.ac_get_ind,"10" );  /*�Ѿ���ȡ��mdm_ac_rel*/
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
	strcpy ( g_pub_tx.prt_ind , "0" );	/*�Ƿ��ӡ����(�������۱�־):1��0�� */
	strcpy ( g_pub_tx.hst_ind , "1" );	/*����ϸ:1�ռ���2���յ�����3���ջ�����*/

	return 0;
}

/* �ܿس��� */
int ln_pay_seq( char *pay_seq , double *tol_amt )
{
	int k=0;

	char sts[2];
	long beg_date_tmp=0;
	
	double amt=0.00;
	double need_pay=0.00;
	
	amt = *tol_amt;
	
	g_ln_parm.min_intst =0.00;
	
	sprintf(acErrMsg,"���뻹Ϣ�ܿ��ܽ��[%lf]˳��[%s]",amt,pay_seq);
	WRITEMSG
	
	if( pub_base_CompDblVal( g_ln_parm.min_intst , g_pub_tx.tmp_amt1 )>0 )
	{
		need_pay = g_pub_tx.tmp_amt1 - g_ln_parm.min_intst ;
	}
	
	if( strlen( pay_seq )==0 )
	{
		sprintf(acErrMsg,"��¼�뻹��˳��!" );
		WRITEMSG
        	strcpy( g_pub_tx.reply,"L191" );
		return 1;
	}
					
	for( k=0 ; k<5 ; k++ )
	{
		TRACE
		if( pay_seq[k]!='\0' )
		{
			TRACE
			g_pub_tx.tx_amt1 =0.00;
			g_pub_tx.tx_amt2 =0.00;
			g_pub_tx.tx_amt3 =0.00;
			g_pub_tx.tx_amt4 =0.00;		
			g_pub_tx.acm_beg_date = 0;
			
			sprintf( sts , "%c" , pay_seq[k] );
			sprintf(acErrMsg,"���û�Ϣ����ǰ���[%lf]˳��[%s]",amt,sts);
			WRITEMSG	
			ret = pay_intst( sts , &amt );
			if( ret )
			{
				sprintf(acErrMsg,"���û�Ϣ�����쳣����,�������Ļ�����ϵ!" );
				WRITEMSG
				if( strlen(g_pub_tx.reply)==0 )
	        		{
		        		strcpy( g_pub_tx.reply,"L192" );
					
				}
				return 1;
			}
			
			if( pub_base_CompDblVal( amt , 0.00 ) <= 0 )
			{
				sprintf(acErrMsg,"�޻������ˡ�amt[%.2lf]!", amt);
				WRITEMSG
				break;
			}				
		}	
	}
	/* ���Ƕ�δ�ﵽ��ͻ������ */
	if( pub_base_CompDblVal (need_pay , 0.00 ) )
	{
		return 0;
	}
	
	*tol_amt = amt;
	return 0;
}
				

/* ��Ϣ���� */
/* ��ڣ�
   ��Ϣ���� 1-����ǷϢ 2-����ǷϢ 3-����ǷϢ 4-δ����Ϣ
   ������
*/
int pay_intst( char *type , double *amt )	
{
	double	pay_amt=0.00;
	double	min_pay=0.00;	
	
	pay_amt=*amt;
	
	sprintf(acErrMsg,"���þ��廹Ϣ����ǰ���[%lf]˳��[%s]",pay_amt,type);
	WRITEMSG	
	
	if( type[0]=='1' )	/* ����ǷϢ */
	{
		TRACE
		ret = pay_in_lo_intst( &pay_amt );
		if( ret )
		{
			sprintf(acErrMsg,"���ñ���ǷϢ��Ϣ�����쳣����,�������Ļ�����ϵ!" );
			WRITEMSG
			if( strlen(g_pub_tx.reply)==0 )
	        	{
	        		strcpy( g_pub_tx.reply,"L193" );
	        	}
			return 1;
		}
		in_lo=1;
						
	}
	if( type[0]=='2' )	/* ����ǷϢ */
	{
		TRACE
		ret = pay_out_lo_intst( &pay_amt );
		if( ret )
		{
			sprintf(acErrMsg,"���ñ���ǷϢ��Ϣ�����쳣����,�������Ļ�����ϵ!" );
			WRITEMSG
	        	if( strlen(g_pub_tx.reply)==0 )
	        	{
	        		strcpy( g_pub_tx.reply,"L194" );
			}
			return 1;
		}
		out_lo=1;				
	}
	if( type[0]=='3' )	/* ����ǷϢ */
	{
		TRACE
		ret = pay_cmpd_lo_intst( &pay_amt );
		if( ret )
		{
			sprintf(acErrMsg,"���ø���ǷϢ��Ϣ�����쳣����,�������Ļ�����ϵ!" );
			WRITEMSG
			if( strlen(g_pub_tx.reply)==0 )
	        	{
	        		strcpy( g_pub_tx.reply,"L195" );
			}
			return 1;
		}
		cmpd_lo=1;				
	}
	/****
	if( type[0]=='4' )	* ������� *
	{
		TRACE
		ret = pay_bal_acm_intst( &pay_amt  );
		if( ret )
		{
			sprintf(acErrMsg,"���ñ��������Ϣ�����쳣����,�������Ļ�����ϵ!" );
			WRITEMSG
	        	if( strlen(g_pub_tx.reply)==0 )
	        	{
	        		strcpy( g_pub_tx.reply,"L196" );
	        	}
			return 1;
		}
		bal_acm=1;				
	}
	if( type[0]=='5' )	* �������� *
	{
		TRACE
		ret = pay_cmpd_acm_intst( &pay_amt );
		if( ret )
		{
			sprintf(acErrMsg,"���ø���������Ϣ�����쳣����,�������Ļ�����ϵ!" );
			WRITEMSG
	        	if( strlen(g_pub_tx.reply)==0 )
	        	{
	        		strcpy( g_pub_tx.reply,"L197" );
	        	}
			return 1;
		}
		cmpd_acm=1;				
	}	
	
	****/
	if( type[0]=='4' )	/* δ����Ϣ */
	{
		TRACE
		strcpy( acm_ind , "1" );

		/* ��ʱ���� *  �������del by lance 20031103ҹ
		if( g_ln_mst.ac_id!=100354509 )
		{
			strcpy( g_pub_tx.reply,"L208" );
			return 1;
		}
		*******/

		ret = pay_acm_intst( &pay_amt  );
		if( ret )
		{
			sprintf(acErrMsg,"����δ����Ϣ��Ϣ�����쳣����,�������Ļ�����ϵ!" );
			WRITEMSG
	        	if( strlen(g_pub_tx.reply)==0 )
	        	{
	        		strcpy( g_pub_tx.reply,"L208" );
	        	}
			return 1;
		}				
	}
		
				
	*amt = pay_amt ;
	
	return 0;
}

/* ��Ϣ--����ǷϢ */
int pay_in_lo_intst( double *amt )	
{
	double bal=0.00;
	
	bal = *amt;

	sprintf(acErrMsg,"���뻹Ϣ--����ǷϢ[%lf]",bal);
	WRITEMSG
	
	ret = pub_base_CompDblVal( g_ln_mst.in_lo_intst , 0.00 );
	if (ret==0)
	{
		sprintf(acErrMsg,"����ǷϢΪ��,���ù黹!���������û���ƻ���." );	
		WRITEMSG		
	        strcpy( g_pub_tx.reply,"L201" );
	        return 1;
	}
		
	/* ȷ�������� */
	/* �������������߸պù��� */
	if( pub_base_CompDblVal( g_ln_mst.in_lo_intst , bal ) >=0 )
	{
		g_pub_tx.tx_amt2 = bal ;
		bal = 0.00;
	}else{
		g_pub_tx.tx_amt2 = g_ln_mst.in_lo_intst ;
		bal = bal - g_ln_mst.in_lo_intst;
	}
	
	/*д����Ǽǲ�*/
	ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"2",g_pub_tx.tx_amt2,0.00,"0",1 );
	if( ret ) 	return 1;			

	in_lo_beg_date = g_pub_tx.acm_beg_date ;
	sprintf(acErrMsg,"����ǷϢ��ʼ����[%ld]",g_pub_tx.acm_beg_date);
	WRITEMSG	
		
	strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
	strcpy ( g_pub_tx.add_ind , "0" );	/* ����:0��1��2���� */	
	g_pub_tx.svc_ind=3050;
	strcpy( g_pub_tx.brf, "");
	/*** ���׼��ʴ��� ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"���׼��ʴ������!");
		WRITEMSG
		return 1;
	}
	sprintf(acErrMsg,"���׼��ʴ��� PASS!");
	WRITEMSG
		
	*amt = bal ;
	set_zd_double("108F",g_pub_tx.tx_amt2);
	amt_in_lo=g_pub_tx.tx_amt2;
		
	return 0;
	
}

/* ��Ϣ--����ǷϢ */
int pay_out_lo_intst( double *amt )	
{
	double bal=0.00;
	
	bal = *amt;
	
	sprintf(acErrMsg,"���뻹Ϣ--����ǷϢ[%lf]",bal);
	WRITEMSG
	ret = pub_base_CompDblVal( g_ln_mst.out_lo_intst , 0.00 );
	if (ret==0)
	{
		sprintf(acErrMsg,"����ǷϢΪ��,���ù黹!���������û���ƻ���." );	
		WRITEMSG		
	        strcpy( g_pub_tx.reply,"L200" );
	        return 1;
	}
	
	/* ȷ�������� */
	/* �������������߸պù��� */
	if( pub_base_CompDblVal( g_ln_mst.out_lo_intst , bal ) >=0 )
	{
		g_pub_tx.tx_amt3 = bal ;
		bal = 0.00;
	}else{
		g_pub_tx.tx_amt3 = g_ln_mst.out_lo_intst ;
		bal = bal - g_ln_mst.out_lo_intst;
	}
	
	/*д����Ǽǲ�*/
	ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"3",g_pub_tx.tx_amt3,0.00,"0",1 );
	if( ret ) 	return 1;	
			
	out_lo_beg_date = g_pub_tx.acm_beg_date ;
	sprintf(acErrMsg,"����ǷϢ��ʼ����[%ld]",g_pub_tx.acm_beg_date);
	WRITEMSG
		
	strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
	strcpy ( g_pub_tx.add_ind , "0" );	/* ����:0��1��2���� */	
	g_pub_tx.svc_ind=3050;
	strcpy( g_pub_tx.brf, "");	
	/*** ���׼��ʴ��� ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"���׼��ʴ������!");
		WRITEMSG
		return 1;
	}
	sprintf(acErrMsg,"���׼��ʴ��� PASS!");
	WRITEMSG
		
	*amt = bal ;
	set_zd_double("108H",g_pub_tx.tx_amt3);
	amt_out_lo=g_pub_tx.tx_amt3;
	
	return 0;
	
}

/* ��Ϣ--����ǷϢ */
int pay_cmpd_lo_intst( double *amt )	
{
	double bal=0.00;
	
	bal = *amt;
	
	sprintf(acErrMsg,"���뻹Ϣ--����ǷϢ[%lf]",bal);
	WRITEMSG
		
	ret = pub_base_CompDblVal( g_ln_mst.cmpd_lo_intst , 0.00 );
	if (ret==0)
	{
		sprintf(acErrMsg,"����ǷϢΪ��,���ù黹!���������û���ƻ���." );	
		WRITEMSG		
	        strcpy( g_pub_tx.reply,"L199" );
	        return 1;
	}
	
	/* ȷ�������� */
	/* �������������߸պù��� */
	if( pub_base_CompDblVal( g_ln_mst.cmpd_lo_intst , bal ) >=0 )
	{
		g_pub_tx.tx_amt4 = bal ;
		bal = 0.00;
	}else{
		g_pub_tx.tx_amt4 = g_ln_mst.cmpd_lo_intst ;
		bal = bal - g_ln_mst.cmpd_lo_intst;
	}
	
	/*д����Ǽǲ�*/
	ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"4",g_pub_tx.tx_amt4,0.00,"0",1 );
	if( ret ) 	return 1;	

	cmpd_lo_beg_date = g_pub_tx.acm_beg_date ;
	sprintf(acErrMsg,"����ǷϢ��ʼ����[%ld]",g_pub_tx.acm_beg_date);
	WRITEMSG
			
	strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
	strcpy ( g_pub_tx.add_ind , "0" );	/* ����:0��1��2���� */	
	g_pub_tx.svc_ind=3050;
	strcpy( g_pub_tx.brf, "");	
	/*** ���׼��ʴ��� ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"���׼��ʴ������!");
		WRITEMSG
		return 1;
	}
	sprintf(acErrMsg,"���׼��ʴ��� PASS!");
	WRITEMSG
		
	*amt = bal ;
	set_zd_double("108I",g_pub_tx.tx_amt4);
	amt_cmpd_lo=g_pub_tx.tx_amt4;
			
	return 0;
	
}

/* ��Ϣ--������� */
int pay_bal_acm_intst( double *amt )	
{
	double bal=0.00;
	double acm_intst=0.00;
	int flag;			/* ����/�����־ */
			
	bal = *amt;
	
	sprintf(acErrMsg,"���뻹Ϣ--�������[%lf]",bal);
	WRITEMSG
		
	/* �������������Ϣ */
	ret = pub_ln_CalIntst ( g_ln_mst , g_ln_parm , "0" , &acm_intst , 0 );
	if( ret )	return 1;
	
	/* ������Ƿ�Ϊ�� */
	ret = pub_base_CompDblVal( acm_intst , 0.00 );
	if (ret==0)
	{
		sprintf(acErrMsg,"���������ϢΪ��,���ù黹!���������û���ƻ���." );	
		WRITEMSG		
	        strcpy( g_pub_tx.reply,"L198" );
	        return 1;
	}
	
	/* ������Ƿ����һ�λ��� */
	/* ȷ�������� */
	/* �������������߸պù��� */
	
	if( pub_base_CompDblVal( acm_intst , bal ) >0 ){
		
		sprintf(acErrMsg,"���������Ϣ����һ�λ���!���������û���ƻ���." );	
		WRITEMSG		
	        strcpy( g_pub_tx.reply,"L202" );
	        return 1;		
	}else if( pub_base_CompDblVal( acm_intst , bal ) ==0 )
	{
		g_pub_tx.tx_amt1 = acm_intst ;
		bal = 0.00;
	}else{
		g_pub_tx.tx_amt1 = acm_intst ;
		bal = bal - acm_intst;
	}
		
	
	/* �黹���������������Ϣ���� */
	/* �������Ϣ�����ж� */	
	flag = pub_ln_GetIntstTyp ( g_ln_mst , g_ln_parm );
	sprintf( flag_typ , "%d" , flag+1 ); 


	/*д����Ǽǲ�*/
	ret = pub_ln_ln_reg(g_ln_mst,ln_ic_date,g_pub_tx.tx_date,flag_typ,acm_intst,g_ln_mst.intst_acm,"0",2);
	if( ret ) return 1;
	
	bal_acm_beg_date = ln_ic_date ;
	sprintf(acErrMsg,"���������ʼ����[%ld]",ln_ic_date);
	WRITEMSG
	
	strcpy( g_pub_tx.ac_get_ind,"10");	/*�Ѿ���ȡ��mdm_ac_rel*/
	strcpy( g_pub_tx.ac_id_type,"3" );	/* �˺����� 3-���� */
	strcpy( g_pub_tx.add_ind,"0" );		/* ����:0��1��2���� */	
	g_pub_tx.svc_ind=3011;
	strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0�� */
	strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����3���ջ�����*/
	strcpy( g_pub_tx.brf,"���������" );

	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"���׼��ʴ������!");
		WRITEMSG
		return 1;
	}
	sprintf(acErrMsg,"���׼��ʴ��� PASS!");
	WRITEMSG
				
	*amt = bal ;
	set_zd_double("108K",acm_intst);

	if( flag==1 )
	{
		set_zd_double("108M",acm_intst);	/* ���� */		
		amt_bal_acm=acm_intst;
		amt_bal_acm_in=acm_intst;

	}
	if( flag==2 )
	{
		set_zd_double("108N",acm_intst);	/* ���� */
		amt_bal_acm=acm_intst;
		amt_bal_acm_out=acm_intst;
	}
			
	return 0;	
}

/* ��Ϣ--�������� */
int pay_cmpd_acm_intst( double *amt )	
{
	double	vbnlx=0.00,vbwlx=0.00,vfllx=0.00;
	double bal=0.00;
			
	bal = *amt;
	
	sprintf(acErrMsg,"���뻹Ϣ--��������[%lf]",bal);
	WRITEMSG
		
	/* ����/����/��������������Ϣ */		
	ret = pub_ln_CalIntst( g_ln_mst,g_ln_parm,"1",&vbnlx,0 );
	if( ret ) return 1;

	ret = pub_ln_CalIntst( g_ln_mst,g_ln_parm,"2",&vbwlx,0 );
	if( ret ) return 1;

	ret = pub_ln_CalIntst( g_ln_mst,g_ln_parm,"3",&vfllx,0 );
	if( ret ) return 1;
	
	cmpd_acm_beg_date = ln_ic_date ;
	sprintf(acErrMsg,"����������ʼ����[%ld]",ln_ic_date);
	WRITEMSG
	
	/* ������Ƿ�Ϊ�� */
	ret = pub_base_CompDblVal( vbnlx+vbwlx+vfllx , 0.00 );
	if (ret==0)
	{
		sprintf(acErrMsg,"����������ϢΪ��,���ù黹!���������û���ƻ���." );	
		WRITEMSG		
	        strcpy( g_pub_tx.reply,"L203" );
	        return 1;
	}
	
	/* ������Ƿ����һ�λ��� */
	/* ȷ�������� */
	/* �������������߸պù��� */
	
	if( pub_base_CompDblVal( vbnlx+vbwlx+vfllx , bal ) >0 ){
		
		sprintf(acErrMsg,"����������Ϣ����һ�λ���!���������û���ƻ���." );	
		WRITEMSG		
	        strcpy( g_pub_tx.reply,"L204" );
	        return 1;		
	}else if( pub_base_CompDblVal( vbnlx+vbwlx+vfllx , bal ) ==0 )
	{
		g_pub_tx.tx_amt2=vbnlx;
		g_pub_tx.tx_amt3=vbwlx;
		g_pub_tx.tx_amt4=vfllx;
		bal = 0.00;
	}else{
		g_pub_tx.tx_amt2=vbnlx;
		g_pub_tx.tx_amt3=vbwlx;
		g_pub_tx.tx_amt4=vfllx;
		bal = bal - (vbnlx+vbwlx+vfllx);
	}
	
	/*д����Ǽǲ�*/
	ret = pub_ln_ln_reg( g_ln_mst,ln_ic_date,g_pub_tx.tx_date,"4",vbnlx,g_ln_mst.in_lo_acm,"0",2 );
	if( ret ) return 1;

	ret = pub_ln_ln_reg( g_ln_mst,ln_ic_date,g_pub_tx.tx_date,"4",vbwlx,g_ln_mst.out_lo_acm,"0",2 );
	if( ret ) return 1;
	
	ret = pub_ln_ln_reg( g_ln_mst,ln_ic_date,g_pub_tx.tx_date,"4",vfllx,g_ln_mst.cmpd_lo_acm,"0",2 );
	if( ret ) return 1;
	
	strcpy ( g_pub_tx.ac_wrk_ind , "0000" );/* �������ˮ,��ϸ*/		
	strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
	strcpy ( g_pub_tx.add_ind , "0" );		/* ����:0��1��2���� */	
	strcpy ( g_pub_tx.brf,"����Ϣ���������ĸ���" );
	g_pub_tx.svc_ind=3051;

	vtcp_log("=========[%.2lf][%.2lf][%.2lf]",vbnlx,vbwlx,vfllx);
	/*** ���׼��ʴ��� ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"���׼��ʴ������!");
		WRITEMSG
		return 1;
	}
			
	sprintf(acErrMsg,"���׼��ʴ��� PASS!");
	WRITEMSG
				
	*amt = bal ;
	set_zd_double("108E",vbnlx+vbwlx+vfllx);
	amt_cmpd_acm=vbnlx+vbwlx+vfllx;		
	return 0;	
}

/* ��Ϣ--δ����Ϣ */
int pay_acm_intst( double *amt )	
{
	double bal=0.00;
	double acm_intst=0.00;
	double	vbnlx=0.00,vbwlx=0.00,vfllx=0.00;
	int flag;			/* ����/�����־ */
			
	bal = *amt;
	
	sprintf(acErrMsg,"���뻹Ϣ--δ����Ϣ[%lf]",bal);
	WRITEMSG
		
	/* �������������Ϣ */
	ret = pub_ln_CalIntst ( g_ln_mst , g_ln_parm , "0" , &acm_intst , 0 );
	if( ret )	return 1;
	
	sprintf(acErrMsg,"�������������Ϣ--δ����Ϣ[%lf][%lf][%lf][%lf]",acm_intst,vbnlx,vbwlx,vfllx);
	WRITEMSG
		
	acm_beg_date = ln_ic_date ;
	sprintf(acErrMsg,"δ����Ϣ��ʼ����[%ld]",ln_ic_date);
	WRITEMSG
	cmpd_acm_beg_date = ln_ic_date ;
	sprintf(acErrMsg,"δ�Ḵ����ʼ����[%ld]",cmpd_acm_beg_date);
	WRITEMSG
	bal_acm_beg_date = ln_ic_date ;
	sprintf(acErrMsg,"���������ʼ����[%ld]",ln_ic_date);
	WRITEMSG
			
	/* ����/����/��������������Ϣ */		
	ret = pub_ln_CalIntst( g_ln_mst,g_ln_parm,"1",&vbnlx,0 );
	if( ret ) return 1;
	sprintf(acErrMsg,"�������������Ϣ--δ����Ϣ[%lf][%lf][%lf][%lf]",acm_intst,vbnlx,vbwlx,vfllx);
	WRITEMSG
	
	ret = pub_ln_CalIntst( g_ln_mst,g_ln_parm,"2",&vbwlx,0 );
	if( ret ) return 1;
	sprintf(acErrMsg,"�������������Ϣ--δ����Ϣ[%lf][%lf][%lf][%lf]",acm_intst,vbnlx,vbwlx,vfllx);
	WRITEMSG
	ret = pub_ln_CalIntst( g_ln_mst,g_ln_parm,"3",&vfllx,0 );
	if( ret ) return 1;
	sprintf(acErrMsg,"�������������Ϣ--δ����Ϣ[%lf][%lf][%lf][%lf]",acm_intst,vbnlx,vbwlx,vfllx);
	WRITEMSG	
	
	if( pub_base_CompDblVal( acm_intst , 0.00 ))
	{
		bal_acm=1;
	}
	if( pub_base_CompDblVal( vbnlx+vbwlx+vfllx , 0.00 ))
	{
		cmpd_acm=1;
	}	
	
	/* ������Ƿ�Ϊ�� */
	ret = pub_base_CompDblVal( acm_intst+vbnlx+vbwlx+vfllx , 0.00 );
	if (ret==0)
	{
		sprintf(acErrMsg,"δ�������ϢΪ��,���ù黹!���������û���ƻ���." );	
		WRITEMSG		
	        strcpy( g_pub_tx.reply,"L209" );
	        return 1;
	}
	
	/* ������Ƿ����һ�λ��� */
	/* ȷ�������� */
	/* �������������߸պù��� */
	
	if( pub_base_CompDblVal( acm_intst+vbnlx+vbwlx+vfllx , bal ) >0 ){
		
		sprintf(acErrMsg,"δ�������Ϣ����һ�λ���!���������û������.��[%lf]bal[%lf]",acm_intst+vbnlx+vbwlx+vfllx , bal );	
		WRITEMSG		
	        strcpy( g_pub_tx.reply,"L210" );
	        return 1;		
	}else if( pub_base_CompDblVal( acm_intst+vbnlx+vbwlx+vfllx , bal ) ==0 )
	{
		g_pub_tx.tx_amt1 = acm_intst ;
		g_pub_tx.tx_amt2=vbnlx;
		g_pub_tx.tx_amt3=vbwlx;
		g_pub_tx.tx_amt4=vfllx;
		
		bal = 0.00;
	}else{
		g_pub_tx.tx_amt1 = acm_intst ;		
		g_pub_tx.tx_amt2=vbnlx;
		g_pub_tx.tx_amt3=vbwlx;
		g_pub_tx.tx_amt4=vfllx;
		
		bal = bal - (acm_intst+vbnlx+vbwlx+vfllx);
	}
		
	/* �������Ϣ�����ж� */
	flag = pub_ln_GetIntstTyp ( g_ln_mst , g_ln_parm );
	sprintf( flag_typ , "%d" , flag+1 ); 
	
	sprintf(acErrMsg,"ly�������Ϣ����[%s]",flag_typ );	
	WRITEMSG	
	

	/*д����Ǽǲ�*/
	ret = pub_ln_ln_reg(g_ln_mst,ln_ic_date,g_pub_tx.tx_date,flag_typ,acm_intst,g_ln_mst.intst_acm,"0",2);
	if( ret ) return 1;
	
	ret = pub_ln_ln_reg( g_ln_mst,ln_ic_date,g_pub_tx.tx_date,"4",vbnlx,g_ln_mst.in_lo_acm,"0",2 );
	if( ret ) return 1;

	ret = pub_ln_ln_reg( g_ln_mst,ln_ic_date,g_pub_tx.tx_date,"4",vbwlx,g_ln_mst.out_lo_acm,"0",2 );
	if( ret ) return 1;
	
	ret = pub_ln_ln_reg( g_ln_mst,ln_ic_date,g_pub_tx.tx_date,"4",vfllx,g_ln_mst.cmpd_lo_acm,"0",2 );
	if( ret ) return 1;
		
	/*** �黹����������������Ϣ���� ***/
	vtcp_log("=========[%.2lf][%.2lf][%.2lf]",vbnlx,vbwlx,vfllx);
	vtcp_log("=========[%.2lf][%.2lf][%.2lf]",g_pub_tx.tx_amt2,g_pub_tx.tx_amt3,g_pub_tx.tx_amt4);
		
	strcpy ( g_pub_tx.ac_wrk_ind , "0000" );/* �������ˮ,��ϸ*/		
	strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
	strcpy ( g_pub_tx.add_ind , "0" );		/* ����:0��1��2���� */	
	strcpy ( g_pub_tx.brf,"����Ϣ���������ĸ���" );
	g_pub_tx.svc_ind=3051;
	
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"���׼��ʴ������!");
		WRITEMSG
		return 1;
	}
			
	sprintf(acErrMsg,"���׼��ʴ��� PASS!");
	WRITEMSG
	set_zd_double("108E",vbnlx+vbwlx+vfllx);	
	amt_cmpd_acm=vbnlx+vbwlx+vfllx;	

	/* �黹���������������Ϣ���� */
	vtcp_log("=========[%.2lf]",acm_intst );
	vtcp_log("=========[%.2lf]",g_pub_tx.tx_amt1 );

	strcpy( g_pub_tx.ac_get_ind,"10");	/*�Ѿ���ȡ��mdm_ac_rel*/
	strcpy( g_pub_tx.ac_id_type,"3" );	/* �˺����� 3-���� */
	strcpy( g_pub_tx.add_ind,"0" );		/* ����:0��1��2���� */	
	if( flag==1 )
	{
		g_pub_tx.svc_ind=3011;
	}else if( flag==2 ){
		g_pub_tx.svc_ind=3012;	
	}	
	strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0�� */
	strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����3���ջ�����*/
	strcpy( g_pub_tx.brf,"���������" );

	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"���׼��ʴ������!");
		WRITEMSG
		return 1;
	}
	sprintf(acErrMsg,"���׼��ʴ��� PASS!");
	WRITEMSG

	set_zd_double("108K",acm_intst);	/* ����δ����Ϣ */

	if( flag==1 )
	{
		set_zd_double("108M",acm_intst);	/* ���� */		
		amt_bal_acm=acm_intst;
		amt_bal_acm_in=acm_intst;
	}
	if( flag==2 )
	{
		set_zd_double("108N",acm_intst);	/* ���� */
		amt_bal_acm=acm_intst;
		amt_bal_acm_out=acm_intst;
	}
					
	*amt = bal ;

	return 0;	
}

/* ��ӡ--ת�˽��˵�,�ֽ�ɿ */
int  print_LNDF()
{ 
    	char tx_type[9];

    	char filename[51],tmpname[101];
    	FILE *fp;
    
    	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"�ļ���:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNDF���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
	
	strcpy( tx_type , "���Ϣ" );	
	
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( tx_type );
			
	/* 1.�����˺š�	     2.������	      3.���͡�    4.�ܻ����� 	*/
	fprintf(fp,"LNDF%s|%s|%s|%.2lf|\n",g_pub_tx.ac_no , g_ln_mst.name , tx_type , print_tol_amt );	
	
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
		
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
	strcpy( tx_type , "���Ϣ" );		
	
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
		
	/* ǷϢ�ܶ� */ 
	ret = sql_sum_double( "ln_reg" , "lo_bal" , &tol_lo , "ac_id=%ld and ac_seqn=%d and reg_date_beg>=%ld and type in('2','3','4')" , g_pub_tx.ac_id,g_pub_tx.ac_seqn,start_day);
	if( ret )	return 1;
	TRACE
	/* �ۼƻ��� */
	ret = sql_sum_double( "ln_reg" , "pay_bal" , &tol_pay , "ac_id=%ld and ac_seqn=%d and reg_date_beg>=%ld and type in('2','3','4')" , g_pub_tx.ac_id,g_pub_tx.ac_seqn,start_day);
	if( ret )	return 1;
	TRACE
	/* ��Ƿ��� */		
	/***
	tol_need_pay = tol_lo - tol_pay;
	****/
	/***��Ϊ�����ļ���ȡֵ****/
	tol_need_pay = g_ln_mst.in_lo_intst + g_ln_mst.out_lo_intst + g_ln_mst.cmpd_lo_intst ;
	if( pub_base_CompDblVal( tol_need_pay , 0.00 ) ==0 )
	{
		tol_need_pay= 0.00;
	}
	/* ���׽����ϸ */
	lx_tol = amt_in_lo + amt_out_lo + amt_bal_acm ;	/* ��Ϣ */
	fl_tol = amt_cmpd_lo + amt_cmpd_acm  ;		/* ���� */
	
	sprintf( rate , "%8.5lf��" , g_ln_mst.rate );
	sprintf( over_rate , "%8.5lf��" , g_ln_mst.over_rate);
	TRACE		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( over_rate );
	TRACE			
	/*  1.�������͡�  2.�����˺š�  3.��ݺš�   4.�������  5.��Ʒ���� 	*/
	/*  6.���׽�  7.��ʼ���ڡ�  8.��ֹ���ڡ� 9.ǷϢ�ܶ 10.�ۼƻ���     */
	/* 11.��Ƿ�� 12.�������ʡ� 13.�������ʡ�14.��ˮ�š�   15.��ϸ��Ϣ  */
	/* 16.��ϸ����  */
	fprintf(fp,"LNHDHZ%s|%s|%s|%s|%s|%.2lf|%s|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%ld|%.2lf|%.2lf|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	print_tol_amt , start_day_str , pri_end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate , g_pub_tx.trace_no , lx_tol ,
	fl_tol );	
	TRACE
vtcp_log("��ô����ʾ�أ���tx_type=[%s]ac_no=[%s]pact_no[%s]name[%s]",tx_type,g_pub_tx.ac_no,g_ln_mst.pact_no,g_ln_mst.name);	
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
	TRACE				
	return 0;
}	
/* ��ӡ--�ͻ��ص�--��Ϣ�ص� */
int  print_LNHDLX( )	
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
    	double tol_amt=0.00;
    	 
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
	strcpy( tx_type , "���Ϣ" );		
	/* ȷ����ʼ���� */
	/******
	if( flag_typ[0]=='2' )	* ����� *
	{
		ret = min_day( in_lo_beg_date , out_lo_beg_date , bal_acm_beg_date , &start_day );
		if( ret )	return 1;		
	}else if ( flag_typ[0]=='3' ){	* ����� *
		ret = min_day( in_lo_beg_date , out_lo_beg_date , bal_acm_beg_date , &start_day );
		if( ret )	return 1;					
	}
	*****/
	ret = sql_min_long( "ln_reg" , "reg_date_beg" , &start_day , "ac_id=%ld and ac_seqn=%d  and type in('2','3') " , g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	if( ret )	return 1;
	TRACE
		
	/* ǷϢ�ܶ� */ 
	ret = sql_sum_double( "ln_reg" , "lo_bal" , &tol_lo , "ac_id=%ld and ac_seqn=%d and reg_date_beg>=%ld and type in('2','3')" , g_pub_tx.ac_id,g_pub_tx.ac_seqn,start_day);
	if( ret )	return 1;
	TRACE
	/* �ۼƻ��� */
	ret = sql_sum_double( "ln_reg" , "pay_bal" , &tol_pay , "ac_id=%ld and ac_seqn=%d and reg_date_beg>=%ld and type in('2','3')" , g_pub_tx.ac_id,g_pub_tx.ac_seqn,start_day);
	if( ret )	return 1;
	TRACE
	/* ��Ƿ��� */		
	/***
	tol_need_pay = tol_lo - tol_pay;
	****/
	/***��Ϊ�����ļ���ȡֵ****/
	tol_need_pay = g_ln_mst.in_lo_intst + g_ln_mst.out_lo_intst;	
	
	/* ���׽�� */
	tol_amt = amt_in_lo + amt_out_lo + amt_bal_acm ;
	
	sprintf( rate , "%8.5lf��" , g_ln_mst.rate );
	sprintf( over_rate , "%8.5lf��" , g_ln_mst.over_rate);
	TRACE		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( over_rate );
	TRACE			
	/*  1.�������͡�  2.�����˺š�  3.��ݺš�   4.�������  5.��Ʒ���� 	*/
	/*  6.���׽�  7.��ʼ���ڡ�  8.��ֹ���ڡ� 9.ǷϢ�ܶ 10.�ۼƻ���     */
	/* 11.��Ƿ�� 12.�������ʡ� 13.�������ʡ�14.��ˮ�� */
	fprintf(fp,"LNHDLX%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	tol_amt , start_day , pri_end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate , g_pub_tx.trace_no );	
	TRACE	
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
	TRACE				
	return 0;
}

/* ��ӡ--�ͻ��ص�--�����ص� */
int  print_LNHDFL( )	
{
  	TRACE  
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	
	char rate[11];
	char over_rate[11];
	long start_day=0;
	
	TRACE
    	char filename[51],tmpname[101];
    	FILE *fp;
     	double tol_amt=0.00;
    	    	
  	TRACE  	
	pub_base_AllDwnFilName( tmpname );
	TRACE
	
	sprintf(acErrMsg,"�ļ���:[%s]!",tmpname);
	WRITEMSG
		
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"�ļ���:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNHDFL���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        TRACE
        	
	/* ��ֵ���� */
	strcpy( tx_type , "�����" );		
	TRACE
	/* ȷ����ʼ���� */		
	/*** 
	sprintf(acErrMsg,"cmpd_acm_beg_date:[%ld]!",cmpd_acm_beg_date);
	WRITEMSG
	sprintf(acErrMsg,"cmpd_lo_beg_date:[%ld]!",cmpd_lo_beg_date);
	WRITEMSG��
	
	ret = min_day( cmpd_acm_beg_date , cmpd_lo_beg_date , 99999999 , &start_day );
	if( ret )	return 1;					
	*****/
	
	ret = sql_min_long( "ln_reg" , "reg_date_beg" , &start_day , "ac_id=%ld and ac_seqn=%d  and type='4' " , g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	if( ret )	return 1;
	TRACE
		
	/* ǷϢ�ܶ� */ 
	ret = sql_sum_double( "ln_reg" , "lo_bal" , &tol_lo , "ac_id=%ld and ac_seqn=%d and reg_date_beg>=%ld and type='4' " , g_pub_tx.ac_id,g_pub_tx.ac_seqn,start_day);
	if( ret )	return 1;
	TRACE
	/* �ۼƻ��� */
	ret = sql_sum_double( "ln_reg" , "pay_bal" , &tol_pay , "ac_id=%ld and ac_seqn=%d and reg_date_beg>=%ld and type='4' " , g_pub_tx.ac_id,g_pub_tx.ac_seqn,start_day);
	if( ret )	return 1;
	TRACE
	/* ��Ƿ��� */		
	/***
	tol_need_pay = tol_lo - tol_pay;
	****/
	/***��Ϊ�����ļ���ȡֵ****/
	tol_need_pay = g_ln_mst.cmpd_lo_intst ;	

	/* ���׽�� */
	tol_amt = amt_cmpd_lo + amt_cmpd_acm  ;

	sprintf( rate , "%8.5lf��" , g_ln_mst.rate );
	sprintf( over_rate , "%8.5lf��" , g_ln_mst.over_rate);
	TRACE
			
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( over_rate );
	TRACE
		
	/*  1.�������͡�  2.�����˺š�  3.��ݺš�   4.�������  5.��Ʒ���� 	*/
	/*  6.���׽�  7.��ʼ���ڡ�  8.��ֹ���ڡ� 9.ǷϢ�ܶ 10.�ۼƻ���     */
	/* 11.��Ƿ�� 12.�������ʡ� 13.�������ʡ�14.��ˮ��*/
	fprintf(fp,"LNHDFL%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	tol_amt , start_day , pri_end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate ,g_pub_tx.trace_no);	
	TRACE
		
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");		
	TRACE
	return 0;
}

/* ��ӡ--���ڴ�Ʊ--���ڲ��� */
int print_LNBKBBN( )
{
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char over_rate[11];
	
	int ret_tmp=0;
	long start_day=0;
	char start_day_str[15];
	char ln_acc_hrt[8];
	char data_code[5];
			
    	char filename[51],tmpname[101];
    	FILE *fp;
      	double tol_amt=0.00;
    	   	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"�ļ���:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNBKBBN���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* ��ֵ���� */
	strcpy( tx_type , "������Ϣ" );		
	/* ȷ����ʼ���� */
	sprintf(acErrMsg,"���ڣ�����flag_typ=[%s]!",flag_typ);
	WRITEMSG	
	sprintf(acErrMsg,"in_lo_beg_date=[%ld],bal_acm_beg_date=[%ld]",in_lo_beg_date,bal_acm_beg_date);
	WRITEMSG
	
	if( acm_ind[0]=='1' )	/* �黹��δ����Ϣ */
	{
		if( flag_typ[0]=='2' )	/* ����� */
		{
			ret = min_day( 99999999 , in_lo_beg_date , bal_acm_beg_date , &start_day );
			if( ret )	return 1;		
		}else if ( flag_typ[0]=='3' ){	/* ����� */
			ret = min_day( 99999999 , in_lo_beg_date , 99999999 , &start_day );
			if( ret )	return 1;					
		}
	}else{
		ret = min_day( 99999999 , in_lo_beg_date , 99999999 , &start_day );
		if( ret )	return 1;
	}	
	/* ת�����ַ���,�����������ڲ�ȷ����,����ʾ */
	if( start_day )
	{
		sprintf( start_day_str, "%4d-%02d-%02d", start_day/10000, start_day%10000/100, start_day%100 );
	}else{
		strcpy( start_day_str, "");
	}	

	/* ȡ��Ŀ�� */
	strcpy( data_code , "0155" );	/* ���ڿ�Ŀ */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"ȡ�����Ŀ��ʧ��!");
		WRITEMSG		
		return 1;
	}
	
	/* ǷϢ�ܶ� */ 
	ret = sql_sum_double( "ln_reg" , "lo_bal" , &tol_lo , " ac_id=%ld and ac_seqn=%d and reg_date_beg>=%ld and type='2' " , g_pub_tx.ac_id,g_pub_tx.ac_seqn,start_day);
	if( ret )	return 1;
	/* �ۼƻ��� */
	ret = sql_sum_double( "ln_reg" , "pay_bal" , &tol_pay , "ac_id=%ld and ac_seqn=%d and reg_date_beg>=%ld and type='2' " , g_pub_tx.ac_id,g_pub_tx.ac_seqn,start_day);
	if( ret )	return 1;
	/* ��Ƿ��� */		
	/***
	tol_need_pay = tol_lo - tol_pay;
	****/
	/***��Ϊ�����ļ���ȡֵ****/
	tol_need_pay = g_ln_mst.in_lo_intst ;	
	if( pub_base_CompDblVal( tol_need_pay , 0.00 )==0 )
	{
		tol_need_pay= 0.00;
	}
		
	/* ���׽�� */
	tol_amt = amt_in_lo + amt_bal_acm_in  ;

	sprintf( rate , "%8.5lf��" , g_ln_mst.rate );
	sprintf( over_rate , "%8.5lf��" , g_ln_mst.over_rate);

		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( over_rate );
	pub_base_strpack( ln_acc_hrt );	
			
	/*  1.�������͡�  2.�����˺š�  3.��ݺš�   4.�������  5.��Ʒ���� 	*/
	/*  6.���׽�  7.��ʼ���ڡ�  8.��ֹ���ڡ� 9.ǷϢ�ܶ 10.�ۼƻ���     */
	/* 11.��Ƿ�� 12.�������ʡ� 13.�������ʡ�14.��ˮ�š�   15.��Ŀ��*/
	fprintf(fp,"LNBKBBN%s|%s|%s|%s|%s|%.2lf|%s|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%ld|%s|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	tol_amt , start_day_str , pri_end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate ,g_pub_tx.trace_no,ln_acc_hrt);	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	
	
/* ��ӡ--���ڴ�Ʊ--���ⲿ�� */
int print_LNBKBBW( )
{
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char over_rate[11];
	
	long start_day=0;
	char start_day_str[15];
	
	char ln_acc_hrt[8];
	char data_code[5];
	
	int ret_tmp=0;		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	
    	double tol_amt=0.00;
    	     	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"�ļ���:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNBKBBW���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* ��ֵ���� */
	strcpy( tx_type , "������Ϣ" );		
	sprintf(acErrMsg,"���ڣ�����flag_typ=[%s]!",flag_typ);
	WRITEMSG	
	sprintf(acErrMsg,"out_lo_beg_date=[%ld],bal_acm_beg_date=[%ld]",out_lo_beg_date,bal_acm_beg_date);
	WRITEMSG	
	/* ȷ����ʼ���� */
	if( acm_ind[0]=='1' )	/* �黹��δ����Ϣ */
	{
		TRACE
		if( flag_typ[0]=='2' )	/* ����� */
		{
			TRACE
			ret = min_day( 99999999 , out_lo_beg_date , 99999999 , &start_day );
			if( ret )	return 1;		
		}else if ( flag_typ[0]=='3' ){	/* ����� */
			TRACE
			ret = min_day( 99999999 , out_lo_beg_date , bal_acm_beg_date , &start_day );
			if( ret )	return 1;					
		}
	}else{
		TRACE
		ret = min_day( 99999999 , out_lo_beg_date , 99999999 , &start_day );
		if( ret )	return 1;
	}	
	/* ת�����ַ���,�����������ڲ�ȷ����,����ʾ */
	if( start_day )
	{
		sprintf( start_day_str, "%4d-%02d-%02d", start_day/10000, start_day%10000/100, start_day%100 );
	}else{
		strcpy( start_day_str, "");
	}	
	
	/* ȡ��Ŀ�� */
	strcpy( data_code , "0156" );	/* �����Ŀ */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"ȡ�����Ŀ��ʧ��!");
		WRITEMSG		
		return 1;
	}	

	/* ǷϢ�ܶ� */ 
	ret = sql_sum_double( "ln_reg" , "lo_bal" , &tol_lo , "ac_id=%ld and ac_seqn=%d and reg_date_beg>=%ld and type='3' " , g_pub_tx.ac_id,g_pub_tx.ac_seqn,start_day);
	if( ret )	return 1;
	/* �ۼƻ��� */
	ret = sql_sum_double( "ln_reg" , "pay_bal" , &tol_pay , "ac_id=%ld and ac_seqn=%d and reg_date_beg>=%ld and type='3' " , g_pub_tx.ac_id,g_pub_tx.ac_seqn,start_day);
	if( ret )	return 1;
	/* ��Ƿ��� */		
	/***
	tol_need_pay = tol_lo - tol_pay;
	****/
	/***��Ϊ�����ļ���ȡֵ****/
	tol_need_pay =  g_ln_mst.out_lo_intst ;	
	if( pub_base_CompDblVal( tol_need_pay , 0.00 )==0 )
	{
		tol_need_pay= 0.00;
	}
		
	/* ���׽�� */
	tol_amt = amt_out_lo + amt_bal_acm_out  ;

	sprintf( rate , "%8.5lf��" , g_ln_mst.rate );
	sprintf( over_rate , "%8.5lf��" , g_ln_mst.over_rate);
		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( over_rate );
	pub_base_strpack( ln_acc_hrt );	
			
	/*  1.�������͡�  2.�����˺š�  3.��ݺš�   4.�������  5.��Ʒ���� 	*/
	/*  6.���׽�  7.��ʼ���ڡ�  8.��ֹ���ڡ� 9.ǷϢ�ܶ 10.�ۼƻ���     */
	/* 11.��Ƿ�� 12.�������ʡ� 13.�������ʡ�14.��ˮ�š�   15.��Ŀ��*/
	fprintf(fp,"LNBKBBW%s|%s|%s|%s|%s|%.2lf|%s|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%ld|%s|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	tol_amt , start_day_str , pri_end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate ,g_pub_tx.trace_no,ln_acc_hrt);	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}
	
/* ��ӡ--���ڴ�Ʊ--�������� */
int print_LNBKBFL( )
{
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char over_rate[11];
	int ret_tmp=0;	
	long start_day=0;
	char start_day_str[15];
	char ln_acc_hrt[8];
	char data_code[5];
		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	
     	double tol_amt=0.00;
    	    	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"�ļ���:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNBKBFL���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* ��ֵ���� */
	strcpy( tx_type , "������Ϣ" );		
	/* ȷ����ʼ���� */
	if( acm_ind[0]=='1' )	/* �黹��δ����Ϣ */
	{
		if( flag_typ[0]=='2' )	/* ����� */
		{
			ret = min_day( cmpd_acm_beg_date , cmpd_lo_beg_date , 99999999 , &start_day );
			if( ret )	return 1;		
		}else if ( flag_typ[0]=='3' ){	/* ����� */
			ret = min_day( cmpd_acm_beg_date , cmpd_lo_beg_date , 99999999 , &start_day );
			if( ret )	return 1;					
		}
	}else{
		ret = min_day( 99999999 , cmpd_lo_beg_date , 99999999 , &start_day );
		if( ret )	return 1;
	}	
	/* ת�����ַ���,�����������ڲ�ȷ����,����ʾ */
	if( start_day )
	{
		sprintf( start_day_str, "%4d-%02d-%02d", start_day/10000, start_day%10000/100, start_day%100 );
	}else{
		strcpy( start_day_str, "");
	}	
	
	/* ȡ��Ŀ�� */
	strcpy( data_code , "0157" );	/* ������Ŀ */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"ȡ�����Ŀ��ʧ��!");
		WRITEMSG		
		return 1;
	}	
	/* ǷϢ�ܶ� */ 
	ret = sql_sum_double( "ln_reg" , "lo_bal" , &tol_lo , "ac_id=%ld and ac_seqn=%d and reg_date_beg>=%ld and type='4' " , g_pub_tx.ac_id,g_pub_tx.ac_seqn,start_day);
	if( ret )	return 1;
	/* �ۼƻ��� */
	ret = sql_sum_double( "ln_reg" , "pay_bal" , &tol_pay , "ac_id=%ld and ac_seqn=%d and reg_date_beg>=%ld and type='4' " , g_pub_tx.ac_id,g_pub_tx.ac_seqn,start_day);
	if( ret )	return 1;
	/* ��Ƿ��� */		
	/***
	tol_need_pay = tol_lo - tol_pay;
	****/
	/***��Ϊ�����ļ���ȡֵ****/
	tol_need_pay = g_ln_mst.cmpd_lo_intst ;	
	if( pub_base_CompDblVal( tol_need_pay , 0.00 )==0 )
	{
		tol_need_pay= 0.00;
	}
		
	/* ���׽�� */
	tol_amt = amt_cmpd_lo + amt_cmpd_acm  ;

	sprintf( rate , "%8.5lf��" , g_ln_mst.rate );
	sprintf( over_rate , "%8.5lf��" , g_ln_mst.over_rate);
		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( over_rate );
		
	/*  1.�������͡�  2.�����˺š�  3.��ݺš�   4.�������  5.��Ʒ���� 	*/
	/*  6.���׽�  7.��ʼ���ڡ�  8.��ֹ���ڡ� 9.ǷϢ�ܶ 10.�ۼƻ���   */
	/* 11.��Ƿ�� 12.�������ʡ� 13.�������ʡ�14.��ˮ�š�   15.��Ŀ��*/
	fprintf(fp,"LNBKBFL%s|%s|%s|%s|%s|%.2lf|%s|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%ld|%s|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	tol_amt , start_day_str , pri_end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate ,g_pub_tx.trace_no,ln_acc_hrt);	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}

/* ��ӡ--������Ϣ�� */
int print_LNJS( )
{
	char tmp_type[2];
	long true_days;	
	char ln_intst_type[11];
	char acm_intst_type[21];

    	char filename[51],tmpname[101];
    	FILE *fp;
    	
	memset( &ln_acm_reg , 0x00 , sizeof(struct ln_acm_reg_c) );
		
    	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"�ļ���:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNJS���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        
	ret = Ln_acm_reg_Dec_Sel( g_pub_tx.reply , "trace_no=%ld ORDER BY intst_type" , g_pub_tx.trace_no );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Ln_acm_reg_Dec_Sel��!ret=[%d]", ret);
		WRITEMSG		
		return 1;	
	}
	while(1)
	{
		ret = Ln_acm_reg_Fet_Sel( &ln_acm_reg, g_pub_tx.reply );
   		if( ret && ret!=100 )
 		{
	 		sprintf( acErrMsg,"fetch Ln_acm_reg_Fet_Sel error code = [%d]",ret);
	 		WRITEMSG
	 		return 1;
 		}else if( ret==100 ){
			break;
		}
		
		pub_base_dic_show_str(ln_intst_type,"ln_intst_knd",g_ln_parm.intst_type);	/* ��Ϣ���� */
		
	 	sprintf( acErrMsg,"fetch Ln_acm_reg_Fet_Sel error code = [%d]",ret);
	 	WRITEMSG
	 					
		pub_base_dic_show_str(acm_intst_type,"ln_acm_type",ln_acm_reg.intst_type);	/* ��Ϣ���� */				
		true_days = pub_base_CalTrueDays(ln_acm_reg.beg_date,ln_acm_reg.end_date);	/* �������� */

		pub_base_strpack(g_ln_parm.title);
		pub_base_strpack(g_pub_tx.ac_no);		
		pub_base_strpack(ln_intst_type);
		pub_base_strpack(g_ln_mst.name);
		pub_base_strpack(acm_intst_type);
										
		/* 1.��Ʒ���ơ�	     2.�˺š�	      3.��Ϣ��ʽ��    4.������	      5.��Ϣ���͡� */
		/* g_ln_parm.title , g_pux_tx.ac_no , ln_intst_type , g_ln_mst.name , acm_intst_type ,*/
		/* 6.��ʼ���ڡ�	         7.��ֹ���ڡ�	       8.������    9.������		  10.���ʡ�	    11.��� */
		/* ln_acm_reg.beg_date , ln_acm_reg.end_date , true_days , ln_acm_reg.intst_acm , ln_acm_reg.rate , ln_acm_reg.intst */
		fprintf(fp,"LNJS%s|%s|%s|%s|%s|%ld|%ld|%ld|%.2lf|%.5lf|%.2lf|\n",
		g_ln_parm.title , g_pub_tx.ac_no , ln_intst_type , g_ln_mst.name , acm_intst_type , 
		ln_acm_reg.beg_date , ln_acm_reg.end_date , true_days , ln_acm_reg.intst_acm , ln_acm_reg.rate , 
		ln_acm_reg.intst);	
	}	
	Ln_acm_reg_Clo_Sel ();
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}

/* ������������ ���ط���������Сֵ*/	
int min_day( long day1 , long day2 , long day3 , long *min_day )
{
	long tmp_day=0 ;
	
	
	if( day1 <= day2 )
	{
		tmp_day = day1;
	}else{
		tmp_day = day2;
	}
	if( tmp_day <= day3 )
	{
		tmp_day = tmp_day;
	}else{
		tmp_day = day3;
	}
	
	*min_day = tmp_day;
	
	return 0;
}	
