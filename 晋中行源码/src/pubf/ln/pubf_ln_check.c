/*************************************************
* �� �� ��: pubf_ln_check.c
* ����������   
* 			pub_ln_check     �����Ʒ���Լ��
* 			pub_ln_chk_clear �鿴�����Ƿ���
*			pub_ln_lndis_file���ݴ�����ʾ�˺ź��˺����ȡ�������ļ�
*			pub_ln_lnid_file ���ݴ����˺�ID���˺����ȡ�������ļ�
*			pub_ln_check_sts �������˻�״̬
*			pub_ln_check_stsdate �������˻�״̬(����ٽ�״̬)
*
* ��    ��:    rob
* ������ڣ�   2004��12��28��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2. 
*************************************************/
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "mo_infrm_c.h"
#include "ln_reg_c.h"
#include "ln_lo_c.h"
#include "dc_acc_rel_c.h"
#include "com_sect_acm_c.h"
#include "com_item_c.h"
/**********************************************************************
* �� �� ����  pub_ln_check
* �������ܣ�  �����Ʒ���Լ��,���̨�������Ƿ�Ϻ���Ʒ���������Ҫ��
* ��    �ߣ�  rob
* ���ʱ�䣺  2004��12��28��
*
* ��    ���� 
*     ��  �룺tx_type   char(1)   ��������(1-���� 2-չ�� 3-���� 4-����)    
*      
*     ��  ��:           char(4)   ��Ӧ��
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/
int pub_ln_check(char *tx_type)
{
	struct mo_infrm_c mo_infrm_c;

	int ret = 0;
	long min_day , max_day ;
	double min_rate , max_rate , rate;
	double rate_val,over_rate_val,fine_rate_val;
	long infrm_date;

	memset( &g_ln_parm,0x00,sizeof(struct ln_parm_c) );
	vtcp_log("%s,%d ",__FILE__,__LINE__);
    /* ���ݲ�Ʒ���������Ʒ�������鿴�Ƿ��и��ֲ�Ʒ */
	ret = Ln_parm_Sel( g_pub_tx.reply , &g_ln_parm, "prdt_no='%s'", 
						g_ln_mst.prdt_no);
		
	if( ret==100 )
	{
		vtcp_log("%s,%d g_ln_mst.prdt_no=[%s] ",__FILE__,__LINE__,g_ln_mst.prdt_no);
		sprintf( acErrMsg,"�����Ʒ�������޴˼�¼ erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
		return 1;
	}else if( ret !=0 )
		{
    		sprintf( acErrMsg,"ȡ�����Ʒ�������쳣 erro code=[%d]",ret);
			vtcp_log("%s,%d g_ln_mst.prdt_no=[%s] ",__FILE__,__LINE__,g_ln_mst.prdt_no);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
    		return -1;
    		}
	switch( atoi(tx_type) )
	{
	  case 1:	/* ����ʱ��� */

		/* ��鿪�������Ƿ����������ں�ֹ������֮�� */	
		if ( (g_ln_parm.beg_date > g_ln_mst.opn_date) || \
			(g_ln_mst.opn_date > g_ln_parm.end_date))
		{	
	        strcpy(acErrMsg,"�������ڲ������������ں�ֹ������֮��!");
		    WRITEMSG
		    strcpy ( g_pub_tx.reply , "A001" );
		    goto ErrExit;
		}
		
		/* ���������С���ޣ�������� */	
		
		if ( g_ln_parm.term_type[0]=='Y' )      /* �������ͣ��� */
		{	
			 TRACE
			g_ln_parm.min_term = g_ln_parm.min_term*12;
			g_ln_parm.max_term = g_ln_parm.max_term*12;
			
			pub_base_deadlineM( g_ln_mst.opn_date,g_ln_parm.min_term,&min_day);
			pub_base_deadlineM( g_ln_mst.opn_date,g_ln_parm.max_term,&max_day);
		}
		else if ( g_ln_parm.term_type[0]=='M' ) /* �������ͣ��� */
          {
			 TRACE
			 pub_base_deadlineM( g_ln_mst.opn_date,g_ln_parm.min_term,&min_day);
			 pub_base_deadlineM( g_ln_mst.opn_date,g_ln_parm.max_term,&max_day);
		  }
		else if ( g_ln_parm.term_type[0]=='D' )  /* �������ͣ��� */
		  {
			 TRACE
			pub_base_deadlineD( g_ln_mst.opn_date,g_ln_parm.min_term,&min_day);
			pub_base_deadlineD( g_ln_mst.opn_date,g_ln_parm.max_term,&max_day);
		  }
			 TRACE
		if ( g_ln_mst.mtr_date<min_day )
		  {
              	sprintf( acErrMsg,"��������С�ڹ涨����С����![%ld][%ld][%s]",
							g_ln_mst.mtr_date , min_day , g_ln_parm.term_type );
				WRITEMSG
				strcpy ( g_pub_tx.reply , "L003" );
				goto ErrExit;
		  }
        if ( g_ln_mst.mtr_date>max_day )
		  {
              	sprintf( acErrMsg,"�������޴��ڹ涨���������![%ld][%ld][%s]",
							g_ln_mst.mtr_date , max_day , g_ln_parm.term_type );
				WRITEMSG
				strcpy ( g_pub_tx.reply , "L004" );
				goto ErrExit;
		  }
	sprintf(acErrMsg,"���������С���ޣ��������(����) PASS!");
	WRITEMSG	
	
		/* ���ò�Ʒ�����������޶� */
		if( pub_base_CompDblVal( g_ln_parm.min_bal , 0.00)>0 )
		{
			if ( g_ln_mst.bal<g_ln_parm.min_bal )
			{
				strcpy(acErrMsg,"���������������");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A003" );
				goto ErrExit;
			}
		}
		if( pub_base_CompDblVal( g_ln_parm.min_bal , 0.00)>0 )		
		{
			if ( g_ln_mst.bal>g_ln_parm.max_bal)
			{
				strcpy(acErrMsg,"������������޶");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A004" );
				goto ErrExit;
			}
		}
	sprintf(acErrMsg,"���ò�Ʒ�����������޶�(����) PASS!");
	WRITEMSG	
	
		/* ��������Ƿ񳬹��涨�����ϸ����¸���Ľ���*/
		/* ���ú����������ʱ��ȡ������ */
	   /***ȡ���ʱ�Ŷ�Ӧ������***/
	   if( g_ln_mst.opn_date>=20030827 )
	   {
		   ret = pub_base_getllz(g_ln_parm.rate_no,g_ln_parm.cur_no,
						g_pub_tx.tx_date,&rate_val);
		   if( ret ) goto ErrExit;
		   sprintf(acErrMsg,"ת��ǰ����[%lf]",rate_val);
		   WRITEMSG
	
		   rate_val = rate_val/1.2;/* ת���������� */ 
		   sprintf(acErrMsg,"ת������������[%lf]",rate_val);
		   WRITEMSG
	
		   /** ȡ�������� **/
		   if( g_ln_parm.over_rate_type[0]=='0' )/* ȡ�������ʱ�� */
		   {
		   		TRACE
		   		ret = pub_base_getllz(g_ln_parm.over_rate_no,g_ln_parm.cur_no,
								g_pub_tx.tx_date,&over_rate_val);
		   		if( ret ) goto ErrExit;					
		   		over_rate_val = over_rate_val/1.2;/* ת���������� */
		   }else{
				TRACE
				over_rate_val = g_ln_mst.rate ;
		   }
		   /** ȡ��Ϣ���� **/
		   if( g_ln_parm.fine_rate_type[0]=='0' )/* ȡ�������ʱ�� */
		   {
		   		ret = pub_base_getllz(g_ln_parm.fine_rate_no,g_ln_parm.cur_no,
								g_pub_tx.tx_date,&fine_rate_val);
		   		if( ret ) goto ErrExit;					
		   		fine_rate_val = fine_rate_val/1.2;/* ת���������� */
		   }else{
				fine_rate_val = g_ln_mst.rate ;
		   }
			sprintf(acErrMsg,"��������[%s]",g_ln_parm.over_rate_no);
			WRITEMSG
			sprintf(acErrMsg,"��Ϣ����[%s]",g_ln_parm.fine_rate_no);
			WRITEMSG		
			sprintf(acErrMsg,"[%s]",g_ln_parm.cur_no);
			WRITEMSG
		 	sprintf(acErrMsg,"[%ld]",g_pub_tx.tx_date);
			WRITEMSG
	
	
			if( g_ln_parm.get_rate_type[0]=='0' ) /**��������**/
			{
				g_ln_mst.rate=rate_val;
			}else{		/**�������ʼ���ϸ��¸�����**/  
				/*�������ʼ��*/
				if( pub_base_CompDblVal(g_ln_mst.rate,
						rate_val*( (100+g_ln_parm.max_flt_ratio)/100.00 ) )>0 )
				{
					vtcp_log("=[%d][%s]",g_ln_parm.max_flt_ratio,g_ln_mst.prdt_no );
					sprintf(acErrMsg,"current rate is too high[%lf][%lf][%d] ",
						rate_val,rate_val*((100+g_ln_parm.max_flt_ratio)/100.00)
								,g_ln_parm.max_flt_ratio );
					WRITEMSG
					strcpy ( g_pub_tx.reply , "A006" );
					goto ErrExit;
				}
	            sprintf(acErrMsg,"���������ȷʱ[%lf][%lf][%d] ", rate_val,
				rate_val*((100+g_ln_parm.max_flt_ratio)/100.00) ,g_ln_parm.max_flt_ratio );
				WRITEMSG
	
				if( pub_base_CompDblVal(g_ln_mst.rate,
						rate_val*((100+g_ln_parm.min_flt_ratio)/100.00) )<0 )
				{
				sprintf(acErrMsg,"������ʴ���ʱ[%lf][%lf][%d] ", rate_val,
			 rate_val*((100+g_ln_parm.min_flt_ratio)/100.00) ,g_ln_parm.min_flt_ratio );
							 WRITEMSG
					strcpy ( g_pub_tx.reply , "A005" );
					goto ErrExit;
				}
				sprintf(acErrMsg,"���������ȷʱ[%lf][%lf][%d] ", rate_val,
			rate_val*((100+g_ln_parm.min_flt_ratio)/100.00) ,g_ln_parm.min_flt_ratio );
				WRITEMSG
	
				/**�������ʼ��**/
				if( pub_base_CompDblVal(g_ln_mst.over_rate,
						over_rate_val*(100+g_ln_parm.over_rate_max)/100.00 )>0 )
				{
					vtcp_log("=[%d][%s]",g_ln_parm.over_rate_max,g_ln_mst.prdt_no );
					sprintf(acErrMsg,"current rate is too high[%lf][%d][%lf] ",
					g_ln_mst.over_rate,g_ln_parm.over_rate_max,
					over_rate_val*(100+g_ln_parm.over_rate_max)/100.00
								 );
					WRITEMSG
					strcpy ( g_pub_tx.reply , "L062" );
					goto ErrExit;
				}
				if( pub_base_CompDblVal(g_ln_mst.over_rate,
						over_rate_val*((100+g_ln_parm.over_rate_min)/100.00) )<0 )
				{
					sprintf(acErrMsg,"over_rate rate is too high[%lf]over_rate_val=[%lf]g_ln_mst.rate=[%lf]!!",
							over_rate_val*((100+g_ln_parm.over_rate_min)/100.00),over_rate_val,g_ln_mst.over_rate);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "L061" );
					goto ErrExit;
				}
				
				/**��Ϣ���ʼ��**/
				if( pub_base_CompDblVal(g_ln_mst.fine_rate,
						fine_rate_val*(100+g_ln_parm.fine_rate_max)/100.00 )>0 )
				{
					vtcp_log("=[%d][%s]",g_ln_parm.fine_rate_max,g_ln_mst.prdt_no );
					sprintf(acErrMsg,"current rate is too high[%lf][%d][%lf] ",
					g_ln_mst.fine_rate,g_ln_parm.fine_rate_max,
					fine_rate_val*(100+g_ln_parm.fine_rate_max)/100.00
								 );
					WRITEMSG
					strcpy ( g_pub_tx.reply , "L163" );
					goto ErrExit;
				}
				if( pub_base_CompDblVal(g_ln_mst.fine_rate,
						fine_rate_val*((100+g_ln_parm.fine_rate_min)/100.00) )<0 )
				{
					sprintf(acErrMsg,"current rate is too low[%lf][%lf]!!",
							g_ln_mst.fine_rate,fine_rate_val*((100+g_ln_parm.fine_rate_min)/100.00));
					WRITEMSG
					strcpy ( g_pub_tx.reply , "L162" );
					goto ErrExit;
				}						
			}
			sprintf(acErrMsg,"�������/����/��Ϣ����(����) PASS!");
			WRITEMSG
		}
	
		/* ��鿪�������������� (���ú���)
		if ( g_ln_parm.opn_lmt == 000 )
		{
			goto ErrExit;
		}
		*/
		break;
	sprintf(acErrMsg,"��鿪��������������(����) PASS!");
	WRITEMSG	
	
   case 2:   /* չ��ʱ��� */
   		/*���ɷ�չ�ڱ�־*/
		if( g_ln_parm.exp_ind[0]=='N' )
    		{
			strcpy(acErrMsg,"�ô����Ʒ������չ��!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L007" );
		    	goto ErrExit;
		}else{
			if( atoi(g_ln_mst.exp_ind)>=(atoi(g_ln_parm.exp_ind)+1) )
		        {
				strcpy(acErrMsg,"�ô����Ʒ�ѳ���չ�ڴ���������չ��!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "L008" );
				goto ErrExit;
			}
		}

		/* ������չ�ں��Ƿ񳬹������������� */	
		
		if ( g_ln_parm.term_type[0]=='Y' )     /* �������ͣ��� */
		{	
			g_ln_parm.max_term = g_ln_parm.max_term*12;
			pub_base_deadlineM( g_ln_mst.opn_date,g_ln_parm.max_term,&max_day);
		}
		else if ( g_ln_parm.term_type[0]=='M' ) /* �������ͣ��� */
          {
			 pub_base_deadlineM( g_ln_mst.opn_date,g_ln_parm.max_term,&max_day);
		  }
		else if ( g_ln_parm.term_type[0]=='D' ) /* �������ͣ��� */
		  {
			pub_base_deadlineD( g_ln_mst.opn_date,g_ln_parm.max_term,&max_day);
		  }

        if ( g_ln_mst.mtr_date>max_day )
		  {
                strcpy( acErrMsg,"�������޴��ڹ涨���������!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "L004" );
				goto ErrExit;
		  }

		/* ��������Ƿ񳬹��涨�����ϸ����¸���Ľ���*/
		/* ���ú����������ʱ��ȡ������(ֻȡ����) */
		/* չ�����ʱ�����ڴ����ͬ���� */
		/******************8
		min_rate=g_ln_mst.rate-g_ln_parm.min_flt_ratio;
		max_rate=g_ln_mst.rate+g_ln_parm.max_flt_ratio;
	
		if ( g_ln_mst.rate < min_rate )
		{
			strcpy(acErrMsg,"����չ�����ʵ��ڹ涨��������ʣ�");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L009" );
			goto ErrExit;
		}
		else if ( g_ln_mst.rate > max_rate )
		{
			strcpy(acErrMsg,"����չ�����ʸ��ڹ涨��������ʣ�");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L010" );
			goto ErrExit;
		}
		***********************/
		if( pub_base_CompDblVal(g_ln_mst.exp_rate,g_ln_mst.rate )<0 )
		{
			sprintf(acErrMsg,"չ�����ʹ���[%lf][%lf]!!",g_ln_mst.exp_rate,g_ln_mst.rate);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L124" );
			goto ErrExit;
		}		
		break;

   case 3:   /* ����ʱ��� */

     if( g_pub_tx.tx_date < g_ln_mst.mtr_date )
	  {
        ret = Mo_infrm_Sel(g_pub_tx.reply, &mo_infrm_c , \
				  "ac_id=%ld and ac_seqn=%d and prm_draw_date=%ld" , \
			       g_ln_mst.ac_id , g_ln_mst.ac_seqn , g_pub_tx.tx_date );

     	  if( ret==100 )
			{
				sprintf( acErrMsg,"֪ͨ�ǼǱ����޴˼�¼ erro code=[%d]",ret);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"L011" );
				goto ErrExit;
			}else if( ret !=0 )
				{
					sprintf( acErrMsg,"ȡ֪ͨ�ǼǱ����쳣 erro code=[%d]",ret);
					WRITEMSG
					strcpy( g_pub_tx.reply ,"D103" );
			    	goto ErrExit;
				}
		
     if( g_pub_tx.tx_amt1> g_ln_parm.fdrq_lmt_amt )
 	 {
		  strcpy(acErrMsg,"���������ǰ�����޶�!");
		  WRITEMSG
		  strcpy ( g_pub_tx.reply , "L012" );
		  goto ErrExit;
	 }
	}

		break;

   case 4:   /* ����ʱ��� */   

		/* ������������������� (���ú���) */
		if ( g_ln_parm.cls_lmt == 000 )
		{
			goto ErrExit;
		}		

		break;

   default: /* ����������ڲ�������  */
	   
		strcpy( acErrMsg,"����������ڲ�������" );
		WRITEMSG
		goto ErrExit;

}

 return 0;

/* ������� */
ErrExit:

		return 1;

}
/**********************************************************************
* �� �� ����  pub_ln_lndis_file
* �������ܣ�  ���ݴ�����ʾ�˺ź��˺����ȡ�������ļ�
* ��    �ߣ�  rob
* ���ʱ�䣺  2004��12��30��
*
* ��    ���� 
*     ��  �룺  ac_no       char(19)  ��ʾ�˺�  	
*               ac_seqn     smallint  �˺����	       
*
*     ��  ��:                         �����ʺŶ����ļ��ṹ
*                                     �������ļ��ṹ
*                                     ��������ļ��ṹ
*                           char(4)   ��Ӧ��
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/
int pub_ln_lndis_file(char *ac_no,int ac_seqn , 
	struct mdm_ac_rel_c *mdm_ac_rel,struct ln_mst_c *ln_mst,
	struct ln_parm_c *ln_parm )
{
    int ret = 0 ;
    
    /* ������ʾ�˺ţ���ƾ֤(����)���˻���ϵmdm_ac_rel���в�ѯ��ȡ���˻�ID */
	sprintf( acErrMsg,"!!!![%s]",ac_no);
	WRITEMSG
	sprintf( acErrMsg,"!!!![%s]",ac_no);
	WRITEMSG

	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , mdm_ac_rel, "ac_no='%s'", ac_no );
	if( ret==100 )
	{
		sprintf( acErrMsg,"ƾ֤(����)���˻���ϵ���޴˼�¼[%d][%s]",ret,ac_no);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"O001" );
		return 1;
	} 
	else if( ret )
	{
    	sprintf( acErrMsg,"ȡƾ֤(����)���˻���ϵ���쳣[%d][%s]",ret,ac_no);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
    	return -1;
    }
    	        
	/******
	if( mdm_ac_rel->ac_seqn && ac_seqn && ac_seqn!=mdm_ac_rel->ac_seqn )
	{
    	sprintf( acErrMsg,"�˻�����쳣mdm[%d]input[%d]",
			ac_seqn,mdm_ac_rel->ac_seqn);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D033" );
    	return -1;
	}
	****/

   	/* ȡ�������ļ���Ϣ */
	ret = Ln_mst_Sel(g_pub_tx.reply, ln_mst , "ac_id=%ld and ac_seqn=%d" ,
				        mdm_ac_rel->ac_id,ac_seqn );
	if( ret==100 )
	{
		sprintf( acErrMsg,"�������ļ������޴˼�¼ erro code=[%d]",ret);
		WRITEMSG
		sprintf( acErrMsg,"mdm_ac_rel->ac_id=[%ld]ac_seqn=[%d]",mdm_ac_rel->ac_id,ac_seqn);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L002" );
		return 1;

	}
	else if( ret )
	{
   		sprintf( acErrMsg,"ȡ�������ļ��쳣 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
   		return -1;
   	}

   	/* ȡ���������Ϣ */
	ret = Ln_parm_Sel(g_pub_tx.reply, ln_parm , " prdt_no='%s' " ,
				        ln_mst->prdt_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"��������ļ����޴˼�¼ [%s][%d]",ln_mst->prdt_no,ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
		return 1;
	} 
	else if( ret )
	{
   		sprintf( acErrMsg,"ȡ��������ļ��쳣 [%s][%d]",ln_mst->prdt_no,ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
   		return -1;
   	}
  return 0 ;
}


/**********************************************************************
* �� �� ����  pub_ln_lnid_file
* �������ܣ�  ���ݴ����˺�ID���˺����ȡ�������ļ�
* ��    �ߣ�  rob
* ���ʱ�䣺  2004��12��30��
*
* ��    ���� 
*     ��  �룺  ac_id       integer   �˺�ID  	
*               ac_seqn     smallint  �˺����	       
*
*     ��  ��:                         �������ļ��ṹ
*                           char(4)   ��Ӧ��
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/
int pub_ln_lnid_file ( long ac_id , int ac_seqn , struct ln_mst_c *ln_mst )
{

	int ret = 0 ;
	
	memset( ln_mst, 0x00, sizeof( struct ln_mst_c ) );
    	        
    /* ȡ�������ļ���Ϣ */
	ret = Ln_mst_Sel(g_pub_tx.reply, ln_mst , "ac_id=%ld and ac_seqn=%d" ,
			        ac_id,ac_seqn );
	if( ret==100 )
	{
		sprintf( acErrMsg,"�������ļ������޴˼�¼ erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L002" );
		return 1;
	} 
	else if( ret )
	{
    	sprintf( acErrMsg,"ȡ�������ļ��쳣 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
    	return -1;
    }
  return 0 ;
}
/**********************************************************************
* �� �� ����  pub_ln_check_sts
* �������ܣ�  �������˻�״̬
* ��    �ߣ�  rob
* ���ʱ�䣺  2004��04��30��
*
* ��    ���� 
*     ��  �룺  ln_mst_c       �������ļ��ṹ   	
*               mode     int   ���ģʽ	       
*
*     ��  ��:       
*
*     ����ֵ�� 0 �ɹ�
*             -1 ʧ��
* 
* �޸���ʷ��
*                   
********************************************************************/
int pub_ln_check_sts( struct ln_mst_c ln_mst,int mode )
{
	int ret=0;

	if( ln_mst.ac_sts[0]=='*' )
	{
    	sprintf( acErrMsg,"ȡ����״̬�쳣[%s]",ln_mst.ac_sts);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L035" );
    	return -1;
	}
	if( ln_mst.ac_sts[0]=='9' )
	{
    	sprintf( acErrMsg,"ȡ����״̬�쳣[%s]",ln_mst.ac_sts);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L036" );
    	return -1;
	}
	if( ln_mst.ac_sts[0]!='1' && ln_mst.ac_sts[0]!='2' 
		&& ln_mst.ac_sts[0]!='3' && ln_mst.ac_sts[0]!='4' && ln_mst.ac_sts[0]!='5' )
	{
    	sprintf( acErrMsg,"ȡ����״̬�쳣[%s]",ln_mst.ac_sts);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L036" );
    	return -1;
	}

	return 0;
}
/**********************************************************************
* �� �� ����  pub_ln_check_stsdate
* �������ܣ�  �������˻�״̬(����ٽ�״̬)
* ��    �ߣ�  rob
* ���ʱ�䣺  2004��04��30��
*
* ��    ���� 
*     ��  �룺 	       
*
*     ��  ��:       
*
*     ����ֵ�� 2���������һ��,�������
*		3���������һ��,��Ĵ���
* 
* �޸���ʷ��
*                   
********************************************************************/
int pub_ln_check_stsdate( )
{
	int ret=0;
	long date,dqr; 
	
	/* ת���ڱ�־ */
	if ( g_ln_parm.turn_ill_ind[0]=='n' )	
	{	
		return -1;
	}
	/* �ж�չ�ڱ�־ */
	if ( g_ln_mst.exp_ind[0]!='0' )
	{
		date=g_ln_mst.exp_mtr_date;
	}else{
		date=g_ln_mst.mtr_date;
	}
	/* ��ǰ״̬Ϊ���� */
	if ( g_ln_mst.ac_sts[0]=='1' )
	{
		
	vtcp_log("JJJJJJJJJJJJJJJgpub_tx.txdate=[%d]",g_pub_tx.tx_date);
		if( date<=g_pub_tx.tx_date )
		
		/***
		if( date==g_pub_tx.tx_date )		
		****/
		{
			return 2;
		}else{
			return -1;
		}
	}
	/**add by ligl2006-9-25 15:57**/
	/*��ǰ״̬Ϊ����90��**/
	if(g_ln_mst.ac_sts[0]=='2')
	{
     ret=pub_base_deadlineD(date,g_ln_parm.turn_ill_term,&dqr );
     if(ret)	return -1;
     sprintf( acErrMsg,"date[%ld]turn_ill_term[%d]dqr[%ld]",
										date,g_ln_parm.turn_ill_term,dqr);
			WRITEMSG							
		if( dqr==g_pub_tx.tx_date )
     {
				return 5;
		 }
		 else
		 {
				return -1;
		 }		
	}
	/**��ǰ״̬Ϊ����90����**/
	if ( g_ln_mst.ac_sts[0]=='5' )
	{
		ret=pub_base_deadlineD( date, g_ln_parm.turn_ill_term, &dqr );
		if(ret) return -1;
		sprintf( acErrMsg,"date[%ld]turn_ill_term[%d]dqr[%ld]",
										date,g_ln_parm.turn_ill_term,dqr);
		WRITEMSG
		if( dqr==g_pub_tx.tx_date )		
		{
				return 3;
		}else
		{
				return -1;
		}
	}
	/**end**/
	
	/* ��ǰ״̬Ϊ���� */
	/**if ( g_ln_mst.ac_sts[0]=='2' )
	{
		ret=pub_base_deadlineD( date, g_ln_parm.turn_ill_term, &dqr );
		if(ret) return -1;
		sprintf( acErrMsg,"date[%ld]turn_ill_term[%d]dqr[%ld]",
										date,g_ln_parm.turn_ill_term,dqr);
		WRITEMSG	**/	
		/******
		if( dqr<=g_pub_tx.tx_date )
		******/
		/**if( dqr==g_pub_tx.tx_date )		
		{
			return 3;
		}else{
			return -1;
		}
	}**/
	return -1;		
}
/**********************************************************************
* �� �� ����  pub_ln_check_acsts
* �������ܣ�  ���������˻�״̬
* ��    �ߣ�  rob
* ���ʱ�䣺  2004��04��30��
*
* ��    ���� 
*     ��  �룺 	      
*
*     ��  ��:  ln_flag 		1-����
*				Z-չ��(��ĸZ)
*				2-����     
*				3-����
*				4-����
*
*     ����ֵ�� 
* 
* �޸���ʷ��
*                   
********************************************************************/	
int pub_ln_check_acsts( char *ln_flag )
{ 		
	strcpy( ln_flag , g_ln_mst.ac_sts );
	/* ��ǰ״̬Ϊ���� */
	if ( g_ln_mst.ac_sts[0]=='1' )
	{
		if ( g_ln_mst.exp_ind[0]!='0' && g_pub_tx.tx_date>g_ln_mst.mtr_date )
		{
			strcpy( ln_flag , "Z" );		
		}
	}

	return 0;		
}		
/**********************************************************************
* �� �� ����  pub_ln_chk_clear
* �������ܣ�  �жϴ����Ƿ��ѻ���
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��06��07��
*
* ��    ����
*     ��  �룺   p_ln_mst            �������ļ��ṹ
*		 		 p_ln_parm 	    	 ��������ṹ
*      
*     ��  ��:    clear_flg	     char(1)	 �Ƿ����־	0-���� 1-δ����
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��	
*                   
********************************************************************/
int pub_ln_chk_clear(struct ln_mst_c p_ln_mst, struct ln_parm_c p_ln_parm, 
												char *clear_flg )
{
  	int ret=0;

  	struct ln_reg_c	ln_reg;	
  	struct ln_lo_c	ln_lo;
  	
	memset( &ln_reg,0x00,sizeof(struct ln_reg_c) );	
	memset( &ln_lo,0x00,sizeof(struct ln_lo_c) );	
	
	/* ��ͨ���� */
	if( p_ln_parm.ln_pay_type[0]!='3' && p_ln_parm.ln_pay_type[0]!='4' )
	{
		/* ��ѯ�������ļ����Ƿ���Ƿ���¼ */
		/* ������Ϣ */
		if( p_ln_mst.intst_type[0]=='0' )
		{
			if( pub_base_CompDblVal( p_ln_mst.bal , 0.00) )
			{
				strcpy( clear_flg ,"1" );	/*��Ƿ��*/
				return 0;
			}else{
				strcpy( clear_flg ,"0" );   /*��Ƿ��*/
				return 0;
			}
		}
		/* �������㸴�� */
		if( p_ln_mst.cmpd_intst_ind[0]=='N' )
		{
			if( pub_base_CompDblVal( p_ln_mst.bal , 0.00) ||
				pub_base_CompDblVal( p_ln_mst.intst_acm , 0.00) ||
				pub_base_CompDblVal( p_ln_mst.in_lo_intst , 0.00) ||
				pub_base_CompDblVal( p_ln_mst.out_lo_intst , 0.00) ||
				pub_base_CompDblVal( p_ln_mst.cmpd_lo_intst , 0.00) )
			{
				strcpy( clear_flg ,"1" );   /*��Ƿ��*/
				return 0;
			}else{
				strcpy( clear_flg ,"0" );   /*��Ƿ��*/
				return 0;
			}
		}

		if( pub_base_CompDblVal( p_ln_mst.bal , 0.00) ||
		    pub_base_CompDblVal( p_ln_mst.intst_acm , 0.00) ||
		    pub_base_CompDblVal( p_ln_mst.in_lo_intst , 0.00) ||
		    pub_base_CompDblVal( p_ln_mst.in_lo_acm , 0.00) ||
		    pub_base_CompDblVal( p_ln_mst.out_lo_intst , 0.00) ||
		    pub_base_CompDblVal( p_ln_mst.out_lo_acm , 0.00) ||
		    pub_base_CompDblVal( p_ln_mst.cmpd_lo_intst , 0.00) ||
		    pub_base_CompDblVal( p_ln_mst.cmpd_lo_acm , 0.00) )
		{	
			strcpy( clear_flg , "1" );	/* ��Ƿ�� */
			return 0;
		}else{
			strcpy( clear_flg ,"0" );   /*��Ƿ��*/
			return 0;
		}
		 	
		/* ��ѯ����Ǽǲ�,�鿴�Ƿ���δ����¼ *
		ret = Ln_reg_Sel ( g_pub_tx.reply , &ln_reg , "ac_id=%ld and \
					ac_seqn=%d and sts='0'", p_ln_mst.ac_id, p_ln_mst.ac_seqn);
		if ( ret && ret!=100 )
		{
			sprintf( acErrMsg, "ִ��Ln_reg_Sel��!ret=[%d]", ret);
			WRITEMSG					
			return 1;
		}else if( ret==100 ){
	 		sprintf( acErrMsg,"��ǷϢ��¼!!");
	 		WRITEMSG
			strcpy( clear_flg , "0" );	* ��Ƿ�� *
 		}else if( !ret ){ 
			strcpy( clear_flg , "1" );	* ��Ƿ�� *
			return 0;
		}
		*****/
	}
	
	/* ���Ҵ��� */	
	if( p_ln_parm.ln_pay_type[0]=='3' || p_ln_parm.ln_pay_type[0]=='4' )
	{
		/* ��ѯ�������ļ����Ƿ���Ƿ���¼ */
		if( pub_base_CompDblVal( p_ln_mst.bal , 0.00) )
		{
			strcpy( clear_flg , "1" );	/* ��Ƿ�� */
			return 0;
		}
		/* ��ѯ����Ƿ���,�鿴�Ƿ���δ����¼ */						
		ret = Ln_lo_Sel( g_pub_tx.reply, &ln_lo, "ac_id=%ld and ac_seqn=%d \
					and pay_type='0'", p_ln_mst.ac_id , p_ln_mst.ac_seqn );
		if ( ret && ret!=100 )
		{
			sprintf( acErrMsg, "ִ��Ln_lo_Sel��!ret=[%d]", ret);
			WRITEMSG					
			return 1;	
		}else if( ret==100 ){
	 		sprintf( acErrMsg,"��ǷϢ��¼!!");
	 		WRITEMSG
			strcpy( clear_flg , "0" );	/* ��Ƿ�� */
 		}else if( !ret ){ 
			strcpy( clear_flg , "1" );	/* ��Ƿ�� */
			return 0;
		}
	}		
			
	return 0;
}
/**********************************************************************
* �� �� ����  pub_ln_check_mtr_date
* �������ܣ�  �жϴ����˻��Ƿ���
* ��    �ߣ�  rob
* ���ʱ�䣺  2004��04��30��
*
* ��    ���� 
*     ��  �룺 	      
*
*     ��  ��:      
*
*     ����ֵ��1���������һ��
*		     2���������һ��
*		     -1----������ 
* 
* �޸���ʷ��
*                   
********************************************************************/	
int pub_ln_check_mtr_date( )
{
	int ret=0;
	long date,dqr; 
	
	/* �ж�չ�ڱ�־ */
	if ( g_ln_mst.exp_ind[0]!='0' )
	{
		TRACE
		date=g_ln_mst.exp_mtr_date;
	}else{
		TRACE
		date=g_ln_mst.mtr_date;
	}
	TRACE
	/* ��ǰ״̬Ϊ���� */
	if ( g_ln_mst.ac_sts[0]=='1' )
	{
	TRACE
		if( date==g_pub_tx.tx_date )
		{
		TRACE
			return 1;
		}else{
		TRACE
			return -1;
		}
	}
	/* ��ǰ״̬Ϊ���� 90����*/
	if ( g_ln_mst.ac_sts[0]=='2' )
	{
		TRACE
		ret=pub_base_deadlineD( date, g_ln_parm.turn_ill_term, &dqr );
		if(ret) return -2;
		TRACE
		sprintf( acErrMsg,"date[%ld]turn_ill_term[%d]dqr[%ld]",date,g_ln_parm.turn_ill_term,dqr);
		WRITEMSG		
		if( dqr==g_pub_tx.tx_date )
		{
		TRACE
			return 2;
		}else{
		TRACE
			return -1;
		}
	}
	/**��ǰ״̬Ϊ���� 90����**/
	if(g_ln_mst.ac_sts[0]=='5')
  {
  	TRACE
		ret=pub_base_deadlineD( date, g_ln_parm.turn_ill_term, &dqr );
		if(ret) return -5;
		TRACE
		sprintf( acErrMsg,"date[%ld]turn_ill_term[%d]dqr[%ld]",date,g_ln_parm.turn_ill_term,dqr);
		WRITEMSG		
		if( dqr==g_pub_tx.tx_date )
		{
		TRACE
			return 5;
		}else{
		TRACE
			return -1;
		}
  }
  /**eend**/
		TRACE
	return -1;		
}		

/**********************************************************************
* �� �� ����  pub_ln_get_acc_hrt
* �������ܣ�  �������ݱ���ȡ�����Ŀ��
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��07��29��
*
* ��    ����
*     ��  �룺   p_ln_mst            �������ļ��ṹ
*		 p_ln_parm 	     ��������ṹ
*                data_code           ���ݱ���
*      
*     ��  ��:    acc_hrt	     char(8)	 ��Ŀ��
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��	
*                   
********************************************************************/
int pub_ln_get_acc_hrt( struct ln_mst_c p_ln_mst , struct ln_parm_c p_ln_parm , char *data_code , char *acc_hrt )
{
	int ret =0;
	char ln_dc_code[5];
	
	struct dc_acc_rel_c	dc_acc_rel_tmp;
	struct com_item_c com_item_tmp;
	memset( &com_item_tmp  , 0x00, sizeof(struct com_item_c));
	memset( &dc_acc_rel_tmp , 0x00 , sizeof( struct dc_acc_rel_c ) );
	
	if( p_ln_mst.ac_sts[0]=='1' )/* ���� */
	{
	TRACE
		strcpy(ln_dc_code,p_ln_parm.dc_code);	
	}
	if( p_ln_mst.ac_sts[0]=='2' )/* ����90���� */
	{
	TRACE
		strcpy(ln_dc_code,p_ln_parm.over_dc_code);	
	}
	if(p_ln_mst.ac_sts[0]=='5')/* ����90���� */
	{
		TRACE
		strcpy(ln_dc_code,p_ln_parm.over90_dc_code);
		
	}
	if( p_ln_mst.ac_sts[0]=='3' )/* ���� */
	{
	TRACE
		strcpy(ln_dc_code,p_ln_parm.sla_dc_code);	
	}
	if( p_ln_mst.ac_sts[0]=='4' )/* ���� */
	{
	TRACE
		strcpy(ln_dc_code,p_ln_parm.bad_dc_code);	
	}
	
	ret = Dc_acc_rel_Sel(g_pub_tx.reply , &dc_acc_rel_tmp , "dc_code='%s' and data_code='%s' ",ln_dc_code, data_code );
	if ( ret )
	{
		sprintf(acErrMsg,"��ȡ��ƴ�����ձ����!!RespCode=[%ld][%s]",
			ret,ln_dc_code);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "S050" );
		return 1;
	}
	ret = Com_item_Sel(g_pub_tx.reply,&com_item_tmp,"acc_hrt='%s'",dc_acc_rel_tmp.acc_hrt);
	if(ret)
	{
		sprintf(acErrMsg,"��ȡ��ƴ�����ձ����!!RespCode=[%ld][%s]",
			ret,ln_dc_code);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "S050" );
		return 1;
	}
	strcpy( acc_hrt , com_item_tmp.acc_hrt);
	
	return 0;
}	
/**********************************************************************
* �� �� ����  pub_ln_Get_SectAcm
* �������ܣ�  ����ֶλ���
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��08��30��
*
* ��    ����  
*     ��  �룺ac_id   	  long 	  �˻�id   
*      	      ac_seqn     int	  �˻����
*             intst_type  char    ��Ϣ���ࣨ0���1����� 2�������ǷϢ3�������ǷϢ4�����5͸֧��
*      
*     ��  ��: acm	  double  ��Ϣ����
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/
int pub_ln_Get_SectAcm( long ac_id , int ac_seqn , 
	char intst_type[2] , double *acm )
{
	int ret;	
	
	struct com_sect_acm_c	com_sect_acm;	
	
	memset( &com_sect_acm, 0x00, sizeof(struct com_sect_acm_c) );
		
	/* ���·ֶλ�������״̬��־ */
	ret = Com_sect_acm_Dec_Upd( g_pub_tx.reply, 
		"ac_id=%ld and ac_seqn=%d and intst_type='%s' and sts='0' ", 
		ac_id , ac_seqn , intst_type);
	if( ret )
	{
		sprintf( acErrMsg,"Prepare Com_sect_acm_Dec_Sel �α����[%d]",ret);
		WRITEMSG
		return 1;
	}	
	
	while(1)
	{
		ret = Com_sect_acm_Fet_Upd( &com_sect_acm, g_pub_tx.reply );
		if( ret==100 ) break;
   		if( ret )
 		{
	 		sprintf( acErrMsg,"fetch com_sect_acm error code = [%d]",ret);
	 		WRITEMSG
	 		strcpy( g_pub_tx.reply ,"D103" );
	 		return 1;
 		}
					
		/* ����Ϣ */
		*acm += com_sect_acm.intst_acm;
	}
	
	Com_sect_acm_Clo_Upd( );
	sprintf(acErrMsg,"���·ֶλ�������״̬��־ PASS[%ld]",*acm);
	WRITEMSG
	*acm = pub_base_PubDround_1( *acm );
										 
	return 0;	
}
