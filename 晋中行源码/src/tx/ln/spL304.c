/*************************************************
* �� �� ��: spL304.c
* ����������   
*              ������Ϣ����
*
* ��    ��:    lance
* ������ڣ�   2003��01��22��
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
#include "ln_reg_c.h"
#include "ln_lo_c.h"
#include "ln_pay_pln_c.h"
#include "ln_auth_c.h"

static struct ln_lo_c ln_lo;
static struct ln_reg_c		ln_reg;	
static struct ln_pay_pln_c	pri_ln_pay_pln;

double print_tol_amt=0.00;
double in_lo_beg_date= 0.00, out_lo_beg_date= 0.00, cmpd_lo_beg_date= 0.00;
double 	amt_in_lo=0.00, amt_out_lo=0.00, amt_cmpd_lo=0.00;
long    pri_end_date=0;	/* ��ֹ���� */
int ret=0;
char print_flag[11];/*��һλ:��ͨ�������;�ڶ�λ:��ͨ�������;����λ:��ͨ�����;*/
			

spL304()  
{ 	
	double amt_tmp=0.00;
	int pay_flag=0;
			
	struct S_ln_intst vln_intst[LN_INTST_TYPS];
	struct ln_auth_c	sLn_auth;
	int i=0;
	int flag=0;
	
	double amt1=0.00,amt2=0.00;

	memset( vln_intst,0,sizeof(vln_intst) );
	memset(&sLn_auth,0x00,sizeof(sLn_auth));
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	sprintf(acErrMsg,"��ʼ��ȫ�ֱ��� PASS!");
	WRITEMSG	
	
	/** ���ݳ�ʼ�� **/
	if ( data_init_struct() )	ERR_DEAL
	sprintf(acErrMsg,"���ݳ�ʼ�� PASS!");
	WRITEMSG
		
	/** ȡֵ����ֵ **/
	if ( data_get_struct() )        ERR_DEAL
	sprintf(acErrMsg,"ȡֵ����ֵ PASS!");
	WRITEMSG
	
		
	/* ��ɳ��ҵ���У�������� ****/
	/* ������״̬�Ƿ�Ϊ���� 	
	if( g_ln_mst.ac_sts[0]!='4' )
	{
		sprintf(acErrMsg,"����״̬�Ǵ���,���ܺ���![%s]",g_ln_parm.ac_sts);
		WRITEMSG
		strcpy( g_pub_tx.reply,"L056" );
		ERR_DEAL
	}
	sprintf(acErrMsg,"������״̬�Ƿ�Ϊ���� PASS!");
	WRITEMSG
	*****************/
	
	/** ��ͨ���� **/
	if ( g_ln_parm.ln_pay_type[0]!='3' && g_ln_parm.ln_pay_type[0]!='4' )
	{
		/* ������ļ���ǷϢ���֮���Ƿ�Ϊ0 */	
		if ( !pub_base_CompDblVal(g_ln_mst.in_lo_intst+g_ln_mst.out_lo_intst
											+g_ln_mst.cmpd_lo_intst , 0) )
		{
			sprintf(acErrMsg,"����ǷϢΪ0,���ܺ���![%s]",g_ln_parm.ac_sts);
			WRITEMSG
			strcpy( g_pub_tx.reply,"L074" );
			ERR_DEAL
		}
		sprintf(acErrMsg,"������ļ���ǷϢ���֮���Ƿ�Ϊ0 PASS!");
		WRITEMSG
		
		/* ������ļ���ǷϢ���֮���Ƿ�ȫ���� */	
		if ( pub_base_CompDblVal(g_ln_mst.in_lo_intst+g_ln_mst.out_lo_intst
											+g_ln_mst.cmpd_lo_intst , g_pub_tx.tmp_amt1) )
		{
			sprintf(acErrMsg,"��Ƿ��Ϣ����һ���Ժ���![%s]",g_ln_parm.ac_sts);
			WRITEMSG
			strcpy( g_pub_tx.reply,"L232" );
			ERR_DEAL
		}
		sprintf(acErrMsg,"��Ƿ��Ϣ����һ���Ժ��� PASS!");
		WRITEMSG		
		
		/* ����Ƿ������ȫ�黹���� */
		flag = pub_base_CompDblVal(g_ln_mst.in_lo_intst+g_ln_mst.out_lo_intst
								+g_ln_mst.cmpd_lo_intst , g_pub_tx.tmp_amt1);
		if (flag<0)	/* ��������� */ 
		{
			sprintf(acErrMsg,"���������![%lf][%lf]", g_ln_mst.in_lo_intst
			+g_ln_mst.out_lo_intst+g_ln_mst.cmpd_lo_intst,g_pub_tx.tmp_amt1);
			WRITEMSG
			strcpy( g_pub_tx.reply,"L219" );
			ERR_DEAL				
		}	
		sprintf(acErrMsg,"����Ƿ������ȫ�黹���� PASS!");
		WRITEMSG
		
		/* ����Ƿ������ȫ�黹 */
		flag = pub_base_CompDblVal(g_ln_mst.in_lo_intst+g_ln_mst.out_lo_intst
								+g_ln_mst.cmpd_lo_intst , g_pub_tx.tmp_amt1);
		if (!flag)	/* ������ȫ��� */ 
		{
			ret = pub_ln_CleanAcm ( g_ln_mst.ac_id , g_ln_mst.ac_seqn , "1" );
			if (ret)
			{
				sprintf(acErrMsg,"���ú����������ʧ��");
				WRITEMSG
				ERR_DEAL	
			}
			strcpy ( g_pub_tx.intst_type , "0" );	/* ��Ϣ���� */			
			
		}		
		sprintf(acErrMsg,"����Ƿ������ȫ�黹 PASS!");
		WRITEMSG
			
		/* ȷ������ǷϢ�Ļ����� */
		i=0;
		vln_intst[i].intst_amt=g_ln_mst.in_lo_intst; vln_intst[i].type=2;
		i++;
		vln_intst[i].intst_amt=g_ln_mst.out_lo_intst; vln_intst[i].type=4;
		i++;
		vln_intst[i].intst_amt=g_ln_mst.cmpd_lo_intst; vln_intst[i].type=6;
		i++;
		
		ret=pub_ln_OrderPay( &vln_intst,g_ln_parm.pay_seq_code,
												&g_pub_tx.tmp_amt1 );
		if( ret ) goto ErrExit;
	
		g_pub_tx.tx_amt1=0.00;
		g_pub_tx.tx_amt2=vln_intst[0].pay_amt;
		g_pub_tx.tx_amt3=vln_intst[1].pay_amt;
		g_pub_tx.tx_amt4=vln_intst[2].pay_amt;
		
		amt_in_lo= g_pub_tx.tx_amt2;
		amt_out_lo= g_pub_tx.tx_amt3;
		amt_cmpd_lo= g_pub_tx.tx_amt4;
		pri_end_date = g_ln_mst.ic_date;
		
		sprintf(acErrMsg,"pay_amt[%.2lf][%.2lf][%.2lf][%.2lf]",
		g_pub_tx.tx_amt1,g_pub_tx.tx_amt2,g_pub_tx.tx_amt3,g_pub_tx.tx_amt4 );
		WRITEMSG
		
		/*д����Ǽǲ�*/
		ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"2",
									g_pub_tx.tx_amt2,g_ln_mst.in_lo_acm,"0",1 );
		if( ret ) goto ErrExit;
		in_lo_beg_date = g_pub_tx.acm_beg_date ;
		
		ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"3",
								g_pub_tx.tx_amt3,g_ln_mst.out_lo_acm,"0",1 );
		if( ret ) goto ErrExit;
		out_lo_beg_date = g_pub_tx.acm_beg_date ;
		ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"4",
								g_pub_tx.tx_amt4,g_ln_mst.cmpd_lo_acm,"0",1 );
		if( ret ) goto ErrExit;
		cmpd_lo_beg_date = g_pub_tx.acm_beg_date ;	
					
		set_zd_double( "108F" , g_pub_tx.tx_amt2 );	/* ���� */
		set_zd_double( "108H" , g_pub_tx.tx_amt3 );	/* ���� */
		set_zd_double( "108I" , g_pub_tx.tx_amt4 );	/* ���� */			
		set_zd_double( "108L" , g_pub_tx.tx_amt4+g_pub_tx.tx_amt3 );/* ����+���� ������Ϣ��Ŀ */			
		
		/* Pub_tx�ṹ��ֵ */
		g_pub_tx.ac_id = g_ln_mst.ac_id;		/* �˻���� */
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
		strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
		strcpy ( g_pub_tx.add_ind , "0" );		/* ����:0��1��2���� */	
		strcpy ( g_pub_tx.ct_ind , "2" );		/* ��ת:1��2ת */	
		strcpy ( g_pub_tx.prt_ind , "0" );/*�Ƿ��ӡ����(�������۱�־):1��0�� */
		strcpy ( g_pub_tx.hst_ind , "1" );/*����ϸ:1�ռ���2���յ�����
												3���ջ�����*/
		g_pub_tx.svc_ind = 3050;			/* ������Ϣ���� */	
		strcpy ( g_pub_tx.brf,"������Ϣ����" );
	
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
		
		/*��ӡ����*/
		/* ���ڴ�Ʊ--������Ϣ���� */
		if( pub_base_CompDblVal(g_pub_tx.tx_amt2, 0.00))	
		{
			print_flag[0]='1';
			ret = print_LNHXBN( );			
			if(ret)
			{
				sprintf(acErrMsg,"print_LNHXBN,ʧ������!" );
				WRITEMSG
				strcpy(g_pub_tx.reply,"L207");
				goto ErrExit;
			}	
			TRACE	
		}else{
			print_flag[0]='*';
		}		
		/* ���ڴ�Ʊ--������Ϣ���� */
		if( pub_base_CompDblVal(g_pub_tx.tx_amt3, 0.00))	
		{		
			print_flag[1]='1';
			ret = print_LNHXBW( );			
			if(ret)
			{
				sprintf(acErrMsg,"print_LNHXBW,ʧ������!" );
				WRITEMSG
				strcpy(g_pub_tx.reply,"L207");
				goto ErrExit;
			}	
			TRACE
		}else{
			print_flag[1]='*';
		}
		/* ���ڴ�Ʊ--������Ϣ���� */
		if( pub_base_CompDblVal(g_pub_tx.tx_amt4, 0.00))	
		{
			print_flag[2]='1';	
			ret = print_LNHXFL( );			
			if(ret)
			{
				sprintf(acErrMsg,"print_LNHXFL,ʧ������!" );
				WRITEMSG
				strcpy(g_pub_tx.reply,"L207");
				goto ErrExit;
			}	
			TRACE	
		}else{
			print_flag[2]='*';
		}				
	}
	/*��δ���ǰ��Һ���*/
	if( 1==0 )
	{
		/* ȡ����ƻ����У����ڵ������� */
		ret = Ln_pay_pln_Sel( g_pub_tx.reply , &pri_ln_pay_pln , "ac_id=%ld and ac_seqn=%d " , g_ln_mst.ac_id , g_ln_mst.ac_seqn );
		if( ret ) {return ret;}
		
		return 0;
		/* ���Ҵ��� */	
		ret = Ln_lo_Dec_Upd ( g_pub_tx.reply , 
					"ac_id=%ld and ac_seqn=%d  and pay_type='0'", 
					 g_ln_mst.ac_id,g_ln_mst.ac_seqn );
		if ( ret )
		{
			sprintf( acErrMsg, "Ln_lo_Dec_Sel��!ret=[%d]", ret);
			WRITEMSG		
			goto ErrExit;	
		}
		sprintf( acErrMsg, "ac_id[%ld]ac_seqn=[%d]", 
								g_ln_mst.ac_id,g_ln_mst.ac_seqn);
		WRITEMSG				

		while(1)
		{
			ret = Ln_lo_Fet_Upd( &ln_lo, g_pub_tx.reply );
	   		if( ret && ret!=100 )
	 		{
		 		sprintf( acErrMsg,"fetch Ln_lo_Fet_Sel error code = [%d]",ret);
		 		WRITEMSG
		 		goto ErrExit;
	 		}else if( ret==100 ){
				TRACE
				break;	
			}
			TRACE
		 	sprintf( acErrMsg,"���뻹Ƿ��֣�����");
		 	WRITEMSG
			/* ����� */
			/* ��ϵͳ���ݣ���Ԥ�����Ѿ�����ڡ�����Ҫ�� ��133 ��705*/
  			/* �����--ֻ������Ƿ��Ϣ */
			amt_tmp = ln_lo.pay_lo_intst + g_pub_tx.tmp_amt1 ;
			 
			/* �������ڹ黹���ں�,����ʣ�� */
			if( pub_base_CompDblVal(ln_lo.lo_intst, amt_tmp)<=0 )
			{
				g_pub_tx.tmp_amt1=g_pub_tx.tmp_amt1-
										(ln_lo.lo_intst-ln_lo.pay_lo_intst );
				ln_lo.pay_lo_intst = ln_lo.lo_intst; /* �ѹ黹��� */	
				sprintf( acErrMsg,"Ƿ����С��(�ѹ黹���+������),\
									ʣ����[%lf]",g_pub_tx.tmp_amt1);
		 		WRITEMSG
			
	           	/* ���� */
	           	if( ln_lo.old_ac_ind[0]=='1' )
	           	{
	           		ret = pub_ln_ln_reg( g_ln_mst,ln_lo.shld_pay_date,
						ln_lo.shld_pay_date,"2",ln_lo.lo_intst,0.00,"0",2 );
					if( ret ) 	return 1;
					amt1 += ln_lo.lo_intst - ln_lo.pay_lo_intst;
	            }
	           	/* ���� */
	            if( ln_lo.old_ac_ind[0]=='2' )
	            {
	            	ret = pub_ln_ln_reg( g_ln_mst,ln_lo.shld_pay_date,
						ln_lo.shld_pay_date,"3",ln_lo.lo_intst,0.00,"0",2 );
					if( ret ) 	return 1;	
					amt2 += ln_lo.lo_intst - ln_lo.pay_lo_intst;
	   	    	}
			}else{	
				/* �պù黹���ڻ��߲��ܻ��屾�� */
				g_pub_tx.tmp_amt1= 0.00;
				/* �ѹ黹��� */
				ln_lo.pay_lo_intst=ln_lo.pay_lo_intst+g_pub_tx.tmp_amt1;
				sprintf( acErrMsg,"Ƿ������ڵ���(�ѹ黹���+������),\
										ʣ����[%lf]",g_pub_tx.tmp_amt1);
		 		WRITEMSG
	            /* ���� */
	           	if( ln_lo.old_ac_ind[0]=='1' )
	            {
	            	amt1+=g_pub_tx.tmp_amt1;
	            }
	            /* ���� */
	            if( ln_lo.old_ac_ind[0]=='2' )
	            {	
	            	amt2+=g_pub_tx.tmp_amt1;	
	            }            	    				 		
		 	}					
	        pay_flag=0;    /*�Ƿ����־*/
	        ret = pub_base_CompDblVal(ln_lo.lo_amt, ln_lo.pay_lo_amt );
	        if( ret ) pay_flag++;  
	        ret = pub_base_CompDblVal(ln_lo.lo_intst, ln_lo.pay_lo_intst );
	        if( ret ) pay_flag++;             
	        ret = pub_base_CompDblVal(ln_lo.pun_intst, ln_lo.pay_pun_intst );
	        if( ret ) pay_flag++;
	        if( pay_flag==0 )     
	        {
	            /* �޸��Ƿ����־ 0-δ���� 1-���� */
	            strcpy( ln_lo.pay_type , "1" );
	            flag++;
	        } 
	        ret = Ln_lo_Upd_Upd(ln_lo, g_pub_tx.reply  );
	        if ( ret )
	        {
	            sprintf( acErrMsg,"Ln_lo_Upd_Upd ����[%d]",ret);
		        WRITEMSG
	            break;
	        }	                	
		}
		Ln_lo_Clo_Upd ();
		sprintf( acErrMsg,"Ƿ�����³ɹ�!!!!");
		WRITEMSG
		
		/* �Ƚϻ��� */	
		g_pub_tx.tmp_amt1 = pub_base_PubDround_1(g_pub_tx.tmp_amt1);
		if ( pub_base_CompDblVal( g_pub_tx.tmp_amt1 , 0.00 ) )
		{
			sprintf(acErrMsg,"��������Ӧ������[%lf]",g_pub_tx.tmp_amt1);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L046");
			ERR_DEAL				
		}
		
		set_zd_double( "1085" , amt1 );
		set_zd_double( "1086" , amt2 );
		
		/* ���˴��� */
	    g_pub_tx.tx_amt2 = amt1;
	    g_pub_tx.tx_amt4 = amt2 ;   
	           
	    /* ����----ֻ����ϵͳ���ݷ�Ϣ�͸����������� */
	    if( pub_base_CompDblVal( g_pub_tx.tx_amt2 , 0.00 ) )        
		{
			g_pub_tx.ac_id = g_ln_mst.ac_id;		/* �˻���� */
			g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */
			strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
			strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
			strcpy ( g_pub_tx.add_ind , "0" );		/* ����:0��1��2���� */	
			strcpy (g_pub_tx.prt_ind,"0");/*�Ƿ��ӡ����(�������۱�־):1��0��*/
			strcpy ( g_pub_tx.hst_ind , "1" );/*����ϸ:1�ռ���2���յ�����
														3���ջ�����*/
			g_pub_tx.svc_ind = 3071 ;	
			strcpy ( g_pub_tx.brf,"�����ݰ�����Ϣ����" );
			sprintf(acErrMsg,"Pub_tx�ṹ��ֵ PASS!");
			WRITEMSG
			
			/*** ���׼��ʴ���(��������Ϣ)***/
			if( pub_acct_trance() )
			{
				sprintf(acErrMsg,"���׼��ʴ������!");
				WRITEMSG
				goto ErrExit;
			}
			sprintf(acErrMsg,"���׼��ʴ���(��������Ϣ) PASS!");
			WRITEMSG	                
	     }
	     if( pub_base_CompDblVal( g_pub_tx.tx_amt4 , 0.00 ) )        
		 {       
			g_pub_tx.ac_id = g_ln_mst.ac_id;		/* �˻���� */
			g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */
			strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
			strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
			strcpy ( g_pub_tx.add_ind , "0" );		/* ����:0��1��2���� */	
			strcpy (g_pub_tx.prt_ind,"0");/*�Ƿ��ӡ����(��������):1��0��*/
			strcpy ( g_pub_tx.hst_ind , "1" );/*����ϸ:1�ռ���2���յ�����
															3���ջ�����*/
			g_pub_tx.svc_ind = 3072 ;	
			strcpy ( g_pub_tx.brf,"�����ݰ�����Ϣ����" );
			sprintf(acErrMsg,"Pub_tx�ṹ��ֵ PASS!");
			WRITEMSG
			/*** ���׼��ʴ���(�����ݷ�Ϣ)***/			
			if( pub_acct_trance() )
			{
				sprintf(acErrMsg,"���׼��ʴ������!");
				WRITEMSG
				goto ErrExit;
			}
			sprintf(acErrMsg,"���׼��ʴ���(�����ݷ�Ϣ) PASS!");
			WRITEMSG
		  }
	}
	/** begin ��ʼ�����Ŵ������������¼ add by chenchao 2011-8-7 11:14:49 **/
	ret = Ln_auth_Dec_Upd(g_pub_tx.reply,"pact_no = '%s' and type='11' and sts in ('X','B') ",g_ln_mst.pact_no);
	if(ret)
	{
		sprintf( acErrMsg, "Ln_auth_Dec_Upd����!ret=[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
		goto ErrExit;	
	}
	ret = Ln_auth_Fet_Upd(&sLn_auth,g_pub_tx.reply);
	if(ret && ret != 100)
	{
		sprintf( acErrMsg, "Ln_auth_Fet_Upd����!ret=[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D102");
		goto ErrExit;	
	}else if (ret == 100)
	{
		sprintf( acErrMsg, "û���ҵ�������¼!ret=[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L175");
		goto ErrExit;
	}
	if(sLn_auth.sts[0] == 'B' || g_ln_mst.bal < 0.005 || sLn_auth.amt<0.005)
	{
		sLn_auth.sts[0] = '1';			/** ����Ҳ�Ѿ�����,��Ϊ����״̬ **/
	}else
	{
		sLn_auth.sts[0] = 'L';			/** ����δ����,��Ϊ��Ϣ����״̬ **/
	}
	ret =  Ln_auth_Upd_Upd(sLn_auth,g_pub_tx.reply);
	if(ret)
	{
		sprintf( acErrMsg, "����������¼����!ret=[%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L175");
		goto ErrExit;
	}
	Ln_auth_Clo_Upd();
	/** end �����Ŵ������������¼ add by chenchao 2011-8-7 11:14:49 **/
	set_zd_data("0630",print_flag);
	
OkExit:
    set_zd_data("0210",g_ln_parm.cur_no);/* �Ա��ָ�ֵ */
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"������Ϣ����[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"������Ϣ����[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset (&ln_lo, 0x00, sizeof(struct ln_lo_c)); 		
	memset (&ln_reg, 0x00, sizeof(struct ln_reg_c));
	memset (&pri_ln_pay_pln, 0x00, sizeof(struct ln_pay_pln_c));
	return 0;
}
int data_get_struct()
{    
	int ret=0;

	sprintf(acErrMsg,"���븳ֵ����");
	WRITEMSG
	
	get_zd_data("1081",g_pub_tx.ac_no);			/* �ʺ� */
	get_zd_int("1082",&g_pub_tx.ac_seqn);			/* �˻���� */

	get_zd_double("108B",&g_pub_tx.tmp_amt1);		/* ������ */
	print_tol_amt= g_pub_tx.tmp_amt1;
	get_zd_data("108C",g_pub_tx.note_type);			/* ƾ֤���� */
	get_zd_data("108D",g_pub_tx.beg_note_no);		/* ��ʼƾ֤���� */
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no );

	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
		&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
	if( ret ) {return ret;}

	/*ret = pub_base_check_mdm_sts( g_mdm_ac_rel ) *������״̬*
	if( ret ) return ret; */
					
	return 0;
}

/* ��ӡ--���������Ʊ--������Ϣ���� */
int print_LNHXBN( )
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
		sprintf(acErrMsg,"LNHXBN���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* ��ֵ���� */
	strcpy( tx_type , "������Ϣ" );		
	/* ȷ����ʼ���� */
	start_day= in_lo_beg_date;
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
	
	/* ���׽�� */
	tol_amt = amt_in_lo;

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
	fprintf(fp,"LNHXBN%s|%s|%s|%s|%s|%.2lf|%s|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%ld|%s|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	tol_amt , start_day_str , pri_end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate ,g_pub_tx.trace_no,ln_acc_hrt);	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}
/* ��ӡ--���������Ʊ--������Ϣ���� */
int print_LNHXBW( )
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
		sprintf(acErrMsg,"LNHXBW���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* ��ֵ���� */
	strcpy( tx_type , "������Ϣ" );			
	/* ȷ����ʼ���� */
	start_day= out_lo_beg_date;
		
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
	
	/* ���׽�� */
	tol_amt = amt_out_lo;

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
	fprintf(fp,"LNHXBW%s|%s|%s|%s|%s|%.2lf|%s|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%ld|%s|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	tol_amt , start_day_str , pri_end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate ,g_pub_tx.trace_no,ln_acc_hrt);	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}
/* ��ӡ--���������Ʊ--������Ϣ���� */
int print_LNHXFL( )
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
		sprintf(acErrMsg,"LNHXFL���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* ��ֵ���� */
	strcpy( tx_type , "������Ϣ" );		
	/* ȷ����ʼ���� */
	start_day= cmpd_lo_beg_date;
		
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
	
	/* ���׽�� */
	tol_amt = amt_cmpd_lo;

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
	fprintf(fp,"LNHXFL%s|%s|%s|%s|%s|%.2lf|%s|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%ld|%s|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	tol_amt , start_day_str , pri_end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate ,g_pub_tx.trace_no,ln_acc_hrt);	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}
