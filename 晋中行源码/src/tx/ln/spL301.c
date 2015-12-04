/*************************************************
* �� �� ��: spL301.c
* ����������   
*              ����չ��
*
* ��    ��:    lance
* ������ڣ�   2003��01��17��
* �޸ļ�¼��   
*     1. ��    ��: 20030819
*        �� �� ��: jane
*        �޸�����: �����޸�ɾ������
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "ln_expand_c.h"
#include "ln_auth_c.h"
	
	static long exp_end_date;
	static double exp_rate;
	static	char	exp_ind_v[2];
	static struct ln_expand_c	ln_expand;
	long	old_mtr_date=0;
	char 	old_rate[11];
	char 	old_exp_ind[5];
	double	acm_rate=0.00;
		
		
spL301()  
{ 		
	int ret=0;
	int flag=0;
	double ln_rate=0.00;
	char ln_flag[2];

	char oprt_type[2];
	get_zd_data( "0700", oprt_type );    /*0-¼�� 1-�޸� 2-ɾ��*/
	
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
	
	/* ������״̬ */
	if ( strcmp( g_ln_mst.ac_sts,"1") )
	{
		sprintf( acErrMsg,"�ô����˻�״̬������");
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L018" );
		ERR_DEAL
	}
	sprintf(acErrMsg,"������״̬ PASS!");
	WRITEMSG
		
	/* ����������Ƿ�Ϊ�� */
	if( pub_base_CompDblVal( g_ln_mst.bal , 0.00 )==0 )	
	{
		sprintf( acErrMsg,"�������Ϊ��,�������˽���!");
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L216" );
		ERR_DEAL
	}
	sprintf(acErrMsg,"��������� PASS!");
	WRITEMSG	
						
	/* ������չ�ڴ��� */
	if ( g_ln_parm.exp_ind[0]=='N' )
	{
		sprintf( acErrMsg,"���ִ����Ʒ������չ��[%s]",g_ln_mst.prdt_no);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L028" );
		ERR_DEAL		
	}else if ( atoi(g_ln_mst.exp_ind) >= atoi(g_ln_parm.exp_ind) && oprt_type[0] == '0' )
		{
			sprintf( acErrMsg,"�ô���Ѿ�������չ��������");
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L029" );
			ERR_DEAL
		}					
	sprintf(acErrMsg,"������չ�ڴ��� PASS!");
	WRITEMSG	
	if( ( oprt_type[0] == '1' || oprt_type[0] == '2' ) && g_ln_mst.exp_ind[0] == '0' )
	{
		strcpy( g_pub_tx.reply, "L225" );
       	sprintf( acErrMsg, "�޸�չ����Ϣ:�˴�����δչ��!");
       	WRITEMSG
       	ERR_DEAL
	}
	
	if( oprt_type[0] == '0' )
	{
		long date=0;
		date=pub_base_daynumSUB(g_ln_mst.mtr_date,10);
		sprintf(acErrMsg,"now[%d],min[%d],max[%d]",g_pub_tx.tx_date,g_ln_mst.mtr_date,date);
    WRITEMSG
    vtcp_log("չ��:mtr_date=[%ld],tx_date=[%ld] date=[%ld] %s,%d",g_ln_mst.mtr_date,g_pub_tx.tx_date,date,__FILE__,__LINE__);
		if (g_pub_tx.tx_date>g_ln_mst.mtr_date||g_pub_tx.tx_date<date)
         {
            strcpy( g_pub_tx.reply, "L332" );
            sprintf(acErrMsg,"��ǰ����û����չ��������,����¼��now[%d],min[%d],max[%d]",g_pub_tx.tx_date,g_ln_mst.mtr_date,date);
            WRITEMSG
            ERR_DEAL
        }
		exp_ind_v[0] = exp_ind_v[0] + 1;
		/* �ǼǴ���չ�ڵǼǲ� */
		ln_expand.ac_id = g_ln_mst.ac_id;	/* �˻�ID */
		ln_expand.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */
		strcpy( ln_expand.exp_br_no , g_pub_tx.tx_br_no );/* չ�ڻ����� */
		ln_expand.exp_amt = g_ln_mst.bal;	/* չ�ڽ�� */
		ln_expand.exp_rate = exp_rate;		/* չ������ */
		
		if ( g_ln_mst.exp_ind[0] ='0' )
		{
			ln_expand.exp_beg_date = g_ln_mst.mtr_date;/* չ����ʼ�� */
		}else{
			ln_expand.exp_beg_date = g_ln_mst.exp_mtr_date;/* չ����ʼ�� */
	     	}
		
		ln_expand.exp_mtr_date = exp_end_date;	/* չ�ڵ����� */
		
		strcpy( ln_expand.exp_ind , exp_ind_v );/* չ�ڱ�־(0δչ��1-9�ѱ�ʾչ�ڴ���) */
		
		ln_expand.rgst_date = g_pub_tx.tx_date;	/* �Ǽ����� */
		strcpy( ln_expand.tel , g_pub_tx.tel );	/* ����Ա */
		ln_expand.trace_no=g_pub_tx.trace_no; /* ��ˮ�� */
		
		ret = Ln_expand_Ins(ln_expand , g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"�ǼǴ���չ�ڵǼǲ�ʧ��![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L030");
			goto ErrExit;
		}			
		sprintf(acErrMsg,"�ǼǴ���չ�ڵǼǲ� PASS!");
		WRITEMSG
		/* ��ֶλ��� */	
		/* ȡ��ֶλ���������� */
		ret = pub_ln_GetRate ( g_ln_mst , g_ln_parm , &acm_rate );
		if( ret )	
		{
			sprintf(acErrMsg,"ȡ��������ʧ��!");
			WRITEMSG
			ERR_DEAL
		}
		ret=pub_ln_Ins_sect_rate( g_ln_mst, g_ln_parm, acm_rate );
		if( ret )	
		{
			sprintf(acErrMsg,"�ǼǷֶλ�����ʧ��!");
			WRITEMSG
			ERR_DEAL
		}		
		
		strcpy( g_pub_tx.brf,"����չ��" );	
		/****
		g_pub_tx.svc_ind = 3065;			* ����չ��(�������ı�) *
		****/
		g_pub_tx.svc_ind = 3063;
		ret = sql_execute( "UPDATE ln_auth SET sts='1' WHERE pact_no ='%s' and sts='X' and type='02'",g_ln_mst.pact_no );/* 20110811 ����sts״̬Ϊ1 չ��*/
		if( ret < 0 )
		{
        	sprintf( acErrMsg, "����ln_auth�����!ret=[%d]",ret );
        	WRITEMSG
        	ERR_DEAL
		}
	}/*������¼��Ĵ���*/
	else if( oprt_type[0] == '1' )
	{
	  if (g_pub_tx.tx_date>g_ln_mst.exp_mtr_date)
	     {
			strcpy( g_pub_tx.reply, "L331" );
            sprintf(acErrMsg,"��ǰ���ڴ���չ������,�����޸�");
            WRITEMSG
            ERR_DEAL
		}
		ret = Ln_expand_Dec_Upd( g_pub_tx.reply, "ac_id=%d and ac_seqn=%d and exp_ind='%s'", \
			g_ln_mst.ac_id, g_ln_mst.ac_seqn, g_ln_mst.exp_ind );
		if( ret )
		{
        	sprintf( acErrMsg, "ִ��Ln_expand_Dec_Upd��!ret=[%d]",ret );
        	WRITEMSG
        	ERR_DEAL
		}
	
		memset( &ln_expand, 0x00, sizeof( struct ln_expand_c ) );
		ret = Ln_expand_Fet_Upd( &ln_expand, g_pub_tx.reply );
    	if ( ret == 100 )
    	{
        	sprintf( acErrMsg, "����չ�ڵǼǲ����޴˼�¼" );
        	WRITEMSG
        	ERR_DEAL
    	}else if ( ret )
    	{
        	sprintf( acErrMsg, "ִ��Ln_expand_Fet_Upd����!ret=[%d]",ret );
        	WRITEMSG
        	ERR_DEAL
    	}
	
		ln_expand.exp_rate = exp_rate;		/* չ������ */
		ln_expand.exp_mtr_date = exp_end_date;	/* չ�ڵ����� */
		ln_expand.rgst_date = g_pub_tx.tx_date;	/* �Ǽ����� */
		strcpy( ln_expand.tel , g_pub_tx.tel );	/* ����Ա */
		ln_expand.trace_no=g_pub_tx.trace_no; /* ��ˮ�� */
			
		ret = Ln_expand_Upd_Upd( ln_expand, g_pub_tx.reply );
		if( ret )
		{
			sprintf( acErrMsg,"ִ��Ln_expand_Upd_Upd��!ret=[%d]",ret );
			WRITEMSG
			ERR_DEAL
		}
	
		Ln_expand_Clo_Upd();
	
		strcpy( g_pub_tx.brf,"����չ���޸�" );	
		g_pub_tx.svc_ind = 3065;			/* ����չ��(�������ı�) */
		
		/*�жϵ�ǰ���״̬*/
		ret = pub_ln_check_acsts( ln_flag );
		if(ret)		ERR_DEAL
		/*����ǰ���״̬Ϊ"չ��",�ǼǷֶλ���*/
		if(ln_flag[0]=='Z')	
		{
			/*ȡ��ǰ�������ֵ*/
			ret=pub_ln_GetRate ( g_ln_mst, g_ln_parm, &ln_rate );
			if(ret)		ERR_DEAL
			/*�ǼǷֶλ���*/
			ret=pub_ln_Ins_sect_rate( g_ln_mst, g_ln_parm, ln_rate );
			if(ret)		ERR_DEAL
			g_pub_tx.svc_ind = 3063;			/* ����չ��(��������) */						
		}
	}/*�������޸�*/
	else if( oprt_type[0] == '2' )
	{
		sprintf(acErrMsg,"���������[%ld]",g_ln_mst.exp_mtr_date);
		sprintf(acErrMsg,"ϵͳ��ǰ����dddd[%ld]",g_pub_tx.tx_date);
		WRITEMSG
		if (g_pub_tx.tx_date>=g_ln_mst.exp_mtr_date)
         {
			strcpy( g_pub_tx.reply, "L330" );
			sprintf(acErrMsg,"��ǰ���ڴ���չ�����ڣ�����ɾ��");
			WRITEMSG
			ERR_DEAL
		}
		ret = sql_execute( "delete from ln_expand where ac_id=%d and ac_seqn=%d and exp_ind='%s'", \
			g_ln_mst.ac_id, g_ln_mst.ac_seqn, g_ln_mst.exp_ind );
		if( ret < 0 )
		{
        	sprintf( acErrMsg, "ɾ��չ�ڵǼǲ����ݿ��������!ret=[%d]",ret );
        	WRITEMSG
        	ERR_DEAL
		}
		strcpy( g_pub_tx.brf,"����չ��ɾ��" );	
		g_pub_tx.svc_ind = 3065;			/* ����չ��(�������ı�) */	
		
		/*�жϵ�ǰ���״̬*/
		ret = pub_ln_check_acsts( ln_flag );
		if(ret)		ERR_DEAL
		/*����ǰ���״̬Ϊ"չ��",�ǼǷֶλ���*/
		if(ln_flag[0]=='Z')	
		{
			/*ȡ��ǰ�������ֵ*/
			ret=pub_ln_GetRate ( g_ln_mst, g_ln_parm, &ln_rate );
			if(ret)		ERR_DEAL
			/*�ǼǷֶλ���*/
			ret=pub_ln_Ins_sect_rate( g_ln_mst, g_ln_parm, ln_rate );
			if(ret)		ERR_DEAL
			g_pub_tx.svc_ind = 3063;			/* ����չ��(��������) */						
		}

		exp_ind_v[0] = exp_ind_v[0] - 1;
		if( exp_ind_v[0] == '0' )
		{
			exp_rate = 0;
			exp_end_date = 0;
		}
		else
		{
			memset( &ln_expand, 0x00, sizeof( struct ln_expand_c ) );
			ret = Ln_expand_Sel( g_pub_tx.reply, &ln_expand, "ac_id=%d and ac_seqn=%d and exp_ind='%s'", \
				g_ln_mst.ac_id, g_ln_mst.ac_seqn, exp_ind_v );
			if( ret && ret != 100 )
			{
        		sprintf( acErrMsg, "��ѯչ�ڵǼǲ����ݿ��������!ret=[%d]",ret );
        		WRITEMSG
        		ERR_DEAL
			}
			else if( ret == 100 )
			{
        		sprintf( acErrMsg, "ɾ���ǵ�һ��չ����Ϣʱ��ѯ�ϴ�չ�ڼ�¼������!ret=[%d]",ret );
        		WRITEMSG
        		ERR_DEAL
			}
			exp_rate = ln_expand.exp_rate;
			exp_end_date = ln_expand.exp_mtr_date;
		}
	}/*������ɾ��*/

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

	g_ln_mst.exp_rate = exp_rate ;			/* չ������ */
	sprintf(acErrMsg,"չ������[%lf]",g_ln_mst.exp_rate);
	WRITEMSG
	
	g_ln_mst.exp_mtr_date = exp_end_date;		/* ��������� */
	sprintf(acErrMsg,"���������[%ld]",g_ln_mst.exp_mtr_date);
	WRITEMSG
	
	strcpy( g_ln_mst.exp_ind , exp_ind_v );	/* չ�ڱ�־ */
	sprintf(acErrMsg,"չ�ڱ�־[%s]", g_ln_mst.exp_ind );
	WRITEMSG
	
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
	/*����¼����޸�ʱ���*/	
	if( oprt_type[0] == '0' || oprt_type[0] == '1' )
	{
		ret = pub_ln_check( "2" );
		if( ret )
		{
			sprintf( acErrMsg,"����չ�ڼ�����!ret=[%d]",ret );
			WRITEMSG
			ERR_DEAL
		}	
	}	
	
	/* Pub_tx�ṹ��ֵ */
	strcpy ( g_pub_tx.add_ind , "1" );		/* ����:0��1��2���� */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* ��ת:1��2ת */	
	/* �Ƿ��ӡ����(�������۱�־):1��0�� */
	strcpy ( g_pub_tx.prt_ind , "0" );	
	/* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� */
	strcpy ( g_pub_tx.hst_ind , "1" );

	strcpy( g_pub_tx.ac_wrk_ind , "0110000" );	
	/*** ���׼��ʴ��� ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"���׼��ʴ������!");
		WRITEMSG
		ERR_DEAL
	}
	sprintf(acErrMsg,"���׼��ʴ��� PASS!");
	WRITEMSG
	
	/* ���ڴ�Ʊ--����չ�ڴ�Ʊ */
	if( oprt_type[0] == '0' || oprt_type[0] == '1' )
	{
		ret = print_LNBKZQ( );			
		if(ret)
		{
			sprintf(acErrMsg,"print_LNBKZQ,ʧ������!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L207");
			goto ErrExit;
		}	
	}

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"����չ�ڳɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"����չ��ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{	
	memset ( &ln_expand , 0x00 , sizeof(struct ln_expand_c) );
	return 0;	
}

int data_set_struct()
{    
	int ret;
	sprintf(acErrMsg,"���븳ֵ����");
	WRITEMSG
			
	get_zd_data("0370",g_pub_tx.ac_no);		/* �����ʺ� */
	sprintf(acErrMsg,"�����ʺ�[%s]",g_pub_tx.ac_no);
	WRITEMSG
	pub_base_old_acno(g_pub_tx.ac_no);
	get_zd_int( "0340" , &g_pub_tx.ac_seqn);	/* �����˻���� */
	sprintf(acErrMsg,"�����˻����[%d]",g_pub_tx.ac_seqn);
	WRITEMSG	
	
	get_zd_long("0450",&exp_end_date);		/* ��������� */	
	get_zd_double("0840",&exp_rate);		/* չ������ */

	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
				&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
	if( ret ) {return ret;}
	
	if( g_ln_mst.exp_mtr_date && g_ln_mst.exp_ind[0]!='0' )
	{
		old_mtr_date = g_ln_mst.exp_mtr_date;
		strcpy( old_exp_ind , "(չ)" );
		sprintf( old_rate , "%8.5lf��" , g_ln_mst.exp_rate );		
	}else{
		old_mtr_date = g_ln_mst.mtr_date;
		strcpy( old_exp_ind , "" );
		sprintf( old_rate , "%8.5lf��" , g_ln_mst.rate );
	}	
	
	strcpy( exp_ind_v, g_ln_mst.exp_ind );
					
	return 0;
}
/* ��ӡ--����չ�ڴ�Ʊ */
int print_LNBKZQ( )
{
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char over_rate[11];
	char exp_ind[5];
	long exp_start_day=0;
		
	char ln_acc_hrt[8];
	char data_code[5];
	int ret_tmp=0;
	long end_date =0;
		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"�ļ���:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNBKZQ���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* ��ֵ���� */
	strcpy( tx_type , "�����" );	
	
	/* ȡ��Ŀ�� */
	strcpy( data_code , "0152" );	/* �����Ŀ */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"ȡ�����Ŀ��ʧ��!");
		WRITEMSG		
		return 1;
	}		
	
	/* Ƿ���ܶ� */	
	tol_lo = g_ln_mst.ttl_prvd_amt ;
	
	/* �ۼƻ��� */	
	tol_pay = g_ln_mst.ttl_prvd_amt - g_ln_mst.bal;
			
	/* ��Ƿ���� */		
	tol_need_pay = g_ln_mst.bal;

	if( g_ln_mst.exp_mtr_date && g_ln_mst.exp_ind[0]!='0' )
	{
		end_date = g_ln_mst.exp_mtr_date;
		strcpy( exp_ind , "(չ)" );
		sprintf( rate , "%8.5lf��" , g_ln_mst.exp_rate );		
	}else{
		end_date = g_ln_mst.mtr_date;
		strcpy( exp_ind , "" );
		sprintf( rate , "%8.5lf��" , g_ln_mst.rate );
	}
		
	sprintf( rate , "%8.5lf��" , exp_rate );
	sprintf( over_rate , "%8.5lf��" , g_ln_mst.over_rate);
	/* ����չ����ʼ����  */
	pub_base_deadlineD( old_mtr_date , 1 , 	&exp_start_day );
	
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( over_rate );
		
	/*  1.�������͡�  2.�����˺š�  3.��ݺš�   4.�������  5.��Ʒ���� 	*/
	/*  6.���׽�  7.�������ڡ�  8.�������ڡ� 9.Ƿ���ܶ 10.�ۼƻ���     */
	/* 11.��Ƿ���� 12.�������ʡ� 13.�������ʡ�14.��Ŀ�š�   15.չ�ڱ�־  */
	/* 16.��ˮ�š�   17.չ�����ʡ� 18.չ����ʼ�ա�19.չ����ֹ��  */
	fprintf(fp,"LNBKZQ%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%ld|%s|%ld|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	0.00 , g_ln_mst.opn_date , old_mtr_date , tol_lo , tol_pay ,
	tol_need_pay , old_rate , over_rate , ln_acc_hrt, old_exp_ind , 
	g_pub_tx.trace_no , rate ,  exp_start_day ,end_date);	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	
