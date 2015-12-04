/*************************************************
* �� �� ��:    sp9821.c
* ����������   
*              ���ݴ����˺�,�˻����ȡ�����Ҵ�����Ϣ
*
* ��    ��:    lance
* ������ڣ�   2003��01��20��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "ln_lo_c.h"
#include "ln_pay_pln_c.h"
	
sp9821()  
{ 		
	int ret=0;
	int flag=0, redflag=0;
	double to_amt=0.00;	/* ����Ӧ����� */
	double tol=0.00,tol_lo=0.00,pay_amt=0.00;
	
	struct ln_mst_c		v;
	struct mdm_ac_rel_c		w;
	struct ln_lo_c		ln_lo;
	struct ln_lo_c		ln_lo_tmp;
	struct ln_pay_pln_c	ln_pay_pln;
	
	
	/** ���ݳ�ʼ�� **/
	memset(&ln_lo , 0x00 , sizeof(struct ln_lo_c) );
	memset(&ln_lo_tmp , 0x00 , sizeof(struct ln_lo_c) );
	memset(&ln_pay_pln , 0x00 , sizeof(struct ln_pay_pln_c) );
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ȡֵ����ֵ **/	
	
	get_zd_data( "1091" , w.ac_no );	/* �����ʺ� */
	pub_base_old_acno( w.ac_no );  	/** �Ծ��ʺŵĴ��� **/
	set_zd_data("1091",w.ac_no);	/* ������ʺ� */

	get_zd_long( "1092" , &v.ac_seqn );	/* �ʺ���� */

	sprintf( acErrMsg," senf acno [%s] [%ld]",w.ac_no,v.ac_seqn );
	WRITEMSG
	
	/* ���ݴ����ʺŲ�ѯ�˺Ž��ʹ�ϵ��ȡ���˻�ID */
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel," ac_no='%s' ",w.ac_no);
	if( ret==100 )
	{
		sprintf( acErrMsg,"ƾ֤(����)���˻���ϵ���޴˼�¼ erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"O001" );
		ERR_DEAL
	} 
	else if( ret )
	{
    	sprintf( acErrMsg,"ȡƾ֤(����)���˻���ϵ���쳣erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
    	ERR_DEAL
    }

	sprintf(acErrMsg,"ȡ�����˻�IDΪ[%ld]seqn[%d]",
		g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
	WRITEMSG	
	
	if( g_mdm_ac_rel.ac_seqn!=ALL_SEQN && g_mdm_ac_rel.ac_seqn!=v.ac_seqn)
	{
    	sprintf( acErrMsg,"[%d]!=[%d]",g_mdm_ac_rel.ac_seqn,v.ac_seqn);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
    	ERR_DEAL
	}
	else
	{
		TRACE
		ret = Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst," ac_id=%ld and ac_seqn=%d ",
			g_mdm_ac_rel.ac_id,v.ac_seqn );
		if( ret==100 )
		{
			sprintf( acErrMsg,"�������ļ������޴˼�¼ erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L002" );
			ERR_DEAL
		} 
		else if( ret ) { ERR_DEAL }	

		ret = Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm," prdt_no='%s' ",
			g_ln_mst.prdt_no);
		if( ret==100 )
		{
			sprintf( acErrMsg,"�������ļ������޴˼�¼ erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"W030" );
			ERR_DEAL
		} 
		else if( ret ) { ERR_DEAL }	

		/* �����������Ƿ��ǰ��Ҵ��� */
		if ( g_ln_parm.ln_pay_type[0]!='3' && g_ln_parm.ln_pay_type[0]!='4' )
		{
			sprintf( acErrMsg,"�˴���ǰ��Ҵ���[%s]",g_ln_parm.ln_pay_type);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L042" );
			ERR_DEAL
		}	
		sprintf(acErrMsg,"�����������Ƿ��ǰ��Ҵ��� PASS!");
		WRITEMSG		
		TRACE
		/* ��ѯ�����Ƿ�ſ� */ 
		ret = pub_base_CompDblVal( g_ln_mst.bal, 0.00 );
		if( ret==0 )
		{
    			sprintf( acErrMsg,"�˰��Ҵ�����δ�ſ�!");
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L109" );		
			goto ErrExit;
		}		
		sprintf(acErrMsg,"�������Ƿ�ſ� PASS!");
		WRITEMSG		
		
		
		/* ��ѯ���Ҵ���Ƿ����е�Ƿ����Ϣ */	
		ret = Ln_lo_Dec_Sel( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d and \
							pay_type='0'" , g_mdm_ac_rel.ac_id,v.ac_seqn );
		if(ret)
		{
				sprintf( acErrMsg, "ִ��Ln_lo_Dec_Sel��!ret=[%d]", ret);
				WRITEMSG		
				return(1);	
		}			
		TRACE
		flag=0;	/* ��Ƿ���� */
		while(1)
		{
			ret = Ln_lo_Fet_Sel( &ln_lo_tmp, g_pub_tx.reply );
   			if( ret && ret!=100 )
 			{
	 			sprintf( acErrMsg,"fetch Ln_lo_Fet_Sel error code = [%d]",ret);
	 			WRITEMSG
	 			return 1;
 			}
			else if( ret==100 )
			break;
			
			flag++;
			ln_lo.lo_amt+=ln_lo_tmp.lo_amt; 	/* Ƿ��� */
			ln_lo.lo_intst+=ln_lo_tmp.lo_intst; 	/* Ƿ��Ϣ */
			ln_lo.pun_intst+=ln_lo_tmp.pun_intst; 	/* ��Ϣ */
			pay_amt+=ln_lo_tmp.pay_lo_amt+ln_lo_tmp.pay_lo_intst+ln_lo_tmp.pay_pun_intst;/* �ѻ���� */
		}
		
		Ln_lo_Clo_Sel ();
		TRACE
		/* ȡ������Ӧ����� */
  		ret = Ln_pay_pln_Sel( g_pub_tx.reply , &ln_pay_pln , "ac_id=%ld and ac_seqn=%d"
					 , g_mdm_ac_rel.ac_id,v.ac_seqn );
		if( ret==100 )
		{
			sprintf(acErrMsg,"�����ƻ�����Ϣ���޴���Ϣ![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L043");
			goto ErrExit;
		}else if( ret )
		  {
			sprintf(acErrMsg,"��ȡ�����ƻ�����Ϣ�쳣![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		  }		  
		sprintf(acErrMsg,"ȡ�����ƻ�����Ϣ PASS");
		WRITEMSG	
		TRACE
		sprintf(acErrMsg,"beg_date[%ld]tx_date[%ld]",ln_pay_pln.beg_date,g_pub_tx.tx_date);
		WRITEMSG			
		if ( ln_pay_pln.beg_date > g_pub_tx.tx_date )
		{
			sprintf(acErrMsg,"Ԥ���ѻ���!");
			WRITEMSG
			ln_pay_pln.curr_amt=0.00;	/* ����Ӧ������ */
			ln_pay_pln.curr_intst=0.00;/* ����Ӧ����Ϣ */
		}
		if ( ln_pay_pln.curr_cnt == ln_pay_pln.ttl_cnt && ln_pay_pln.pay_ind[0]=='1' )
		{
			to_amt = 0.00;
		}else{
			TRACE
			to_amt = ln_pay_pln.curr_amt+ln_pay_pln.curr_intst; /* ����Ӧ����� */
		}
		/* ���ܽ�� */
		pay_amt=ln_lo.pay_lo_amt+ln_lo.pay_lo_intst+ln_lo.pay_pun_intst;
		tol_lo=ln_lo.lo_amt+ln_lo.lo_intst+ln_lo.pun_intst-pay_amt;/* Ƿ���ܼ� */

		tol=tol_lo+to_amt;	/* �ܼ� */ 
		TRACE
		/* ��� */ 
		set_zd_double("1094",g_ln_mst.bal);/* ����� */
		set_zd_long("109E",ln_pay_pln.ttl_cnt);	/* ������ */
		set_zd_long("109F",ln_pay_pln.curr_cnt-1);	/* �ѻ����� */

		set_zd_int("1096",flag);	/* ��Ƿ���� */
		set_zd_double("1097",ln_lo.lo_amt+ln_lo.lo_intst);/* ��Ƿ�ܶ� */
		set_zd_double("1098",ln_lo.pun_intst);/* ��Ϣ */		
		set_zd_double("109G",tol_lo);/* Ƿ���ܼ� */	
		
		set_zd_double("1095",to_amt);	/* ���ڽ�� */			
		set_zd_double("109I",ln_pay_pln.curr_amt);	/* ����Ӧ������ */		
		set_zd_double("109J",ln_pay_pln.curr_intst);	/* ����Ӧ����Ϣ */							
		set_zd_double("109H",tol);	/* �ܼƽ�� */		
		
		set_zd_data("1093",g_ln_mst.name);/* ���� */			
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
   
