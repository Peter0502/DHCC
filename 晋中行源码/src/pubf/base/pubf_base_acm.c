#include "find_debug.h" 
/*************************************************
* �� �� ��: pubf_base_acm.c
* ����������   
*              pub_base_CalAcm          �������(������Ƿֻ���)
*              pub_base_Get_SectIntst   ����ֶλ�����Ϣ
*              pub_base_CalAcm_Ln       �������(����Ƿֻ���)
*
* ��    ��:    lance
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
#include "com_sect_acm_c.h"
#include "com_parm_c.h"
#include "ln_reg_c.h"
#include "ln_acm_reg_c.h"
#include "mo_sxc_c.h"
#include "mo_sxc_hst_c.h"

double pub_base_PubDround_1 ( double lx ); /*��Ϣ��������*/
double pub_base_floor(double number);
/**********************************************************************
* �� �� ����  pub_base_CalAcm
* �������ܣ�  �������
* ��    �ߣ�  lance
* ���ʱ�䣺  2003��01��14��
*
* ��    ���� 
*     ��  �룺beg_date   long	  ��ʼ����(�ϱʷ�������)   
*      	      end_date   ilong	  ��ֹ����(�������ڣ��������ڣ�) 
*	      	  day_type	 char	  ��Ϣ�������ͣ�0��ʵ��������1��30����㣩
*             acm_type   char     �����������ͣ�1ÿ���ۼ�2�䶯�ۼ�3�䶯�ۼƵ���
*             bal        double   ����ǰ���(������Ϣ��)
*             amt        double   ���(���Ӧ���뷢�������Ϊ����Ϊ��)
*             acm        double   ��Ϣ����
*(������Ϣ��)     
* intst_type��Ϣ���� 0����Ϣ1���汾��2���ռ�Ϣ3���¼�Ϣ4������Ϣ5�����Ϣ
* intst_mon��Ϣ�·�
* intst_date��Ϣ����
*     ��  ��: acm        double   ��Ϣ����
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/
int pub_base_CalAcm( beg_date,end_date,day_type,acm_type,bal,amt,acm,intst_type,intst_moon,intst_date )
 long beg_date ;
 long end_date ;
 char day_type[2] ; 
 char acm_type[2] ;
 double bal ;
 double amt ;
 double *acm ;
 char intst_type[2];
 long intst_moon ;
 long intst_date ;
{
	long days;/* ���� */
	int ret;
	long end_acm_day; /* ��Ϣ���� */
	double vamt;
	double vacm;
	
	struct mo_sxc_c      sMo_sxc;
  struct mo_sxc_hst_c  sMo_sxc_hst;
  memset(&sMo_sxc,     0x00, sizeof(sMo_sxc));
  memset(&sMo_sxc_hst, 0x00, sizeof(sMo_sxc_hst));
    
	sprintf(acErrMsg,"����Ϊ[%lf], day_type [%s], acm_type [%s], amt=[%lf]", 
		*acm, day_type, acm_type, amt);
	WRITEMSG	

	days=0;
	vamt=bal;
	vtcp_log("[%s] [%d], �鿴�����������[%lf]", __FILE__, __LINE__, vamt);
	
	if ( acm_type[0]=='0' )	/* ���� */
	{
TRACE
		return 0;
	}
	else if ( acm_type[0]=='1' )	/* ÿ���ۼ� */
	{
TRACE
		return 0;
	}
	else if ( acm_type[0]=='2' )	/* �䶯�ۼ� */
	{
			/* �������� */	
			vtcp_log("beg_date=[%ld],end_date=[%ld],day_type=[%s]",beg_date,end_date,day_type);
			ret = pub_base_CalDays( beg_date , end_date , day_type , &days );
			if ( ret )
			{
				sprintf(acErrMsg,"���ü���������������!");
				WRITEMSG
				return 1;	
			}
			sprintf(acErrMsg,"���ü����������� PASS!����Ϊ[%ld]",days);
			WRITEMSG
			
	}
	else if ( acm_type[0]=='3' )	/* �䶯�ۼƵ��� */
	{
        /* ����Ϣ���� */
		TRACE
		/* �����ȡ �����汾�岻�ܱ䶯�ۼƵ������ ****
        if (intst_type[0] == '1')
        {
             sprintf(acErrMsg,"���汾�岻�ܱ䶯�ۼƵ������!!");
             WRITEMSG
             return 1;
        }
		***********************************************/

		vamt=amt;
			pub_base_next_intst_date(intst_type,intst_moon,
				intst_date,&end_acm_day);
		vtcp_log( "NEXT INTST[%d]",end_acm_day );
			if ( end_acm_day<=19000101 )
			{
				sprintf(acErrMsg,"��Ϣ����Ϊ[%ld]",end_acm_day);
				WRITEMSG					
				days=0;
			}
			else
			{
				/* �������� */	
				ret = pub_base_CalDays( end_date,end_acm_day,day_type,&days );
				if ( ret )
				{
					sprintf(acErrMsg,"���ü���������������!");
					WRITEMSG
					return 1;	
				}
				days = days + 1;
				sprintf(acErrMsg,"���ü����������� PASS!����Ϊ[%ld]",days);
				WRITEMSG	
			}
	}
	else
	{
		sprintf(acErrMsg,"�������ʽ����[%s]!",acm_type);
		WRITEMSG
		strcpy( g_pub_tx.reply,"P146" );
		return 1;	
	}

	/**20030617 rob**/
	if( days<0 ) days=0;

	/* ������� */
	vacm=*acm;
	
	sprintf(acErrMsg,"���Ĵ�ҵ���ж� g_pub_tx.tx_code==[%s] g_pub_tx.sub_tx_code==[%s] g_dd_mst.ac_id==[%ld]",g_pub_tx.tx_code, g_pub_tx.sub_tx_code,g_dd_mst.ac_id);
			WRITEMSG
	/**��ǩԼ���Ĵ�ҵ�񣬹�����ʱ��ȥ���Ĵ���� add by zqbo 20141120***/
  if( (strcmp(g_pub_tx.tx_code,"Z157")==0 || strcmp(g_pub_tx.tx_code,"2316")==0 || strcmp(g_pub_tx.tx_code,"2313")==0 || 
    	strcmp(g_pub_tx.tx_code,"8314")==0 || strcmp(g_pub_tx.tx_code,"8315")==0 || strcmp(g_pub_tx.tx_code,"2314")==0 || 
    	strcmp(g_pub_tx.tx_code,"2315")==0 || strcmp(g_pub_tx.sub_tx_code,"D313")==0 || strcmp(g_pub_tx.sub_tx_code,"D316")==0 || 
    	strcmp(g_pub_tx.tx_code,"2201")==0 || strcmp(g_pub_tx.tx_code,"2202")==0 || strcmp(g_pub_tx.sub_tx_code,"D099")==0 || strcmp(g_pub_tx.sub_tx_code,"D003")==0)
    	&& g_dd_mst.ac_id > 0) /**��ֹ��������Ƕ�׳���ͬʱ���Ч��**/
  {
      ret = Mo_sxc_Sel(g_pub_tx.reply, &sMo_sxc, "ac_id=%ld and ac_seqn=%d and sts='1'", g_dd_mst.ac_id, g_dd_mst.ac_seqn);
      if(ret != 100 && ret != 0)
      {
          sprintf(acErrMsg,"Mo_sxc_Dec_Sel����[%d]!",ret);
          WRITEMSG
          strcpy( g_pub_tx.reply,"P146" );
          return 1; 
      }
      else if(ret == 0) /**������ǩԼ����**/
      {
          if( pub_base_CompDblVal(vamt, g_dd_mst.bal) == 0)
          {
              vamt -= sMo_sxc.bal;    /** ��ȥ���Ĵ���� **/
              vtcp_log("[%s] [%d], ac_id[%ld]���[%lf]	[%lf]", __FILE__, __LINE__, sMo_sxc.ac_id, sMo_sxc.bal, vamt);
          }
          if(sMo_sxc.flag[0]=='0')   /***���������޸ĵ��շ���ҵ���־***/
          {
              ret = sql_execute("update mo_sxc set flag='1' where ac_id=%ld and ac_seqn=%d and sts='1'", g_dd_mst.ac_id, g_dd_mst.ac_seqn);
              if(ret)
              {
                  sprintf(acErrMsg,"sql_execute(update mo_sxc...) ����[%d]!",ret);
                  WRITEMSG
                  strcpy( g_pub_tx.reply,"P146" );
                  return 1;
              }
              vtcp_log("[%s] [%d], ac_id[%ld]���[%lf]�鿴�����������[%lf]", __FILE__, __LINE__, sMo_sxc.ac_id, sMo_sxc.bal, vamt);
          }
      }/**���򲻴���**/
  }
    
	/****�Ƿֲ������***/
	sprintf(acErrMsg,"amt[%.2lf] floor(amt)[%.2lf]",vamt,pub_base_floor(vamt));
	WRITEMSG
	vacm += days*pub_base_floor(vamt);
	sprintf(acErrMsg,"����Ϊ[%lf]days[%d]amt[%.2lf]",vacm,days,vamt);
	WRITEMSG
	/* �������� */
	vacm= pub_base_PubDround_1( vacm );	
	sprintf(acErrMsg,"����Ϊ[%lf]",vacm);
	WRITEMSG
	
	/***�������Ĵ�ǩԼʱ������岹�����ɻ���Ϊ���� ֱ��ȡ0 zqbo 20141120***/
  if(sMo_sxc.sts[0] == '1' && pub_base_CompDblVal(vacm,0.00) < 0)
  {
      vacm = 0.00;
  }
  sprintf(acErrMsg,"----����Ϊ[%lf]----",vacm);
  WRITEMSG
  
	*acm =vacm;
	return 0;
}
/**�����´ν�Ϣ����**/
int pub_base_next_intst_date(intst_type,intst_moon,intst_date,end_acm_day)
 char intst_type[2];
 long intst_moon;
 long intst_date;
 long *end_acm_day;
{
	long vy,vm,vd;
	long vy1,vm1,vd1;
	long next_date,tmpdate;

sprintf(acErrMsg,"�����´ν�Ϣ����[%s][%d][%d]",intst_type,intst_moon,intst_date);
WRITEMSG

	vy=g_pub_tx.tx_date/10000;
	vm=g_pub_tx.tx_date/100%100;
	vd=g_pub_tx.tx_date%100;

/* intst_type��Ϣ���� 0����Ϣ1���汾��2���ռ�Ϣ3���¼�Ϣ4������Ϣ5�����Ϣ*/
	switch(intst_type[0])
	{
		case '0':
			*end_acm_day=0;
			break;
		case '1':
			sprintf(acErrMsg,"------> ��������Ϊ : [%ld]", g_pub_tx.mach_date);
			WRITEMSG
			*end_acm_day=g_pub_tx.mach_date;
			break;
		case '2':
			*end_acm_day=g_pub_tx.tx_date;
			break;
		case '3':
			if( intst_date==99 )
			{
				vd1=1; vy1=vy;
				if( vm==12 ) { vm1=1; vy1++; }
				else { vm1=vm+1; }
				next_date=vy1*10000+vm1*100+vd1;
				pub_base_deadlineD( next_date,-1,&tmpdate );
				if( tmpdate%100>vd ) *end_acm_day=tmpdate;
				else
				{
					if( vm==12 ) { vm1=1; vy1++; }
					next_date=vy1*10000+vm1*100+vd1;
					pub_base_deadlineD( next_date,-1,end_acm_day );
				}
			}
			else
			{
				if( vd<intst_date )
					*end_acm_day=vy*10000+vm*100+intst_date;
				else
				{
					if( vm==12 ) { vm1=1; vy1=vy+1; }
					*end_acm_day=vy1*10000+vm1*100+intst_date;
				}
			}
			break;
		case '4':
			break;
		case '5':
			if( intst_date==99 )
			{
				vy1=vy; vm1=intst_moon; vd1=1; 
				if( vm1==12 ) { vm1=1; vy1++; }
				else { vm1=vm1+1; }
				next_date=vy1*10000+vm1*100+vd1;
				pub_base_deadlineD( next_date,-1,&tmpdate );
				if( g_pub_tx.tx_date<tmpdate ) *end_acm_day=tmpdate;
				else
				{
					*end_acm_day=tmpdate+10000;
				}
			}
			else
			{
				tmpdate=vy*10000+intst_moon*100+intst_date;
				if( g_pub_tx.tx_date<tmpdate ) *end_acm_day=tmpdate;
				else
				{
					*end_acm_day=tmpdate+10000;
				}
			}
			break;
		default :
			*end_acm_day=0;
			break;
	}

sprintf(acErrMsg,"�����´ν�Ϣ����over[%d]",*end_acm_day);
WRITEMSG
	return 0;
}

/**********************************************************************
* �� �� ����  pub_base_Get_SectIntst
* �������ܣ�  ����ֶλ�����Ϣ
* ��    �ߣ�  lance
* ���ʱ�䣺  2003��01��14��
*
* ��    ����  
*     ��  �룺ac_id   	  long 	  �˻�id   
*      	      ac_seqn     int	  �˻����
*             intst_type  char    ��Ϣ���ࣨ0���1����� 2�������ǷϢ3�������ǷϢ4�����5͸֧��
*             mode        int     ����ģʽ 0�޸����ݿ�1���޸����ݿ�
*      
*     ��  ��: intst	  double  ��Ϣ����
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/
int pub_base_Get_SectIntst( long ac_id , int ac_seqn , 
	char intst_type[2] , double *intst , int mode )
{
	int ret;	
	
	struct com_sect_acm_c	com_sect_acm;	
	struct ln_acm_reg_c	ln_acm_reg;
	
	MEMSET_DEBUG( &com_sect_acm, 0x00, sizeof(struct com_sect_acm_c) );
	MEMSET_DEBUG( &ln_acm_reg, 0x00, sizeof(struct ln_acm_reg_c) );
		
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
		vtcp_log("[%s][%d]com_sect_acm.intst_acm=[%lf]com_sect_acm.rate==[%ld]\n",__FILE__,__LINE__,com_sect_acm.intst_acm,com_sect_acm.rate);
		*intst += com_sect_acm.intst_acm*com_sect_acm.rate/30/1000;		

		if( intst_type[0]=='1'||intst_type[0]=='2'||intst_type[0]=='3'||intst_type[0]=='4' )
		{

		if( !mode )
		{
			strcpy( com_sect_acm.sts , "1" );	
			ret = Com_sect_acm_Upd_Upd( com_sect_acm, g_pub_tx.reply  );
			if ( ret )
			{
				sprintf( acErrMsg,"Com_sect_acm_Upd_Upd ����[%d]",ret);
				WRITEMSG
				return 1;
			}
			TRACE
			ret= Ln_mst_Sel( g_pub_tx.reply, &g_ln_mst , "ac_id=%ld and ac_seqn=%d ",com_sect_acm.ac_id,
																		com_sect_acm.ac_seqn );
			if( ret ) return 1;

			/* ��Ӵ����Ϣ�����ǼǱ� */
			ln_acm_reg.ac_id = com_sect_acm.ac_id;
			ln_acm_reg.ac_seqn = com_sect_acm.ac_seqn;
			strcpy( ln_acm_reg.br_no, g_ln_mst.opn_br_no );
			ln_acm_reg.beg_date = com_sect_acm.beg_date;
			ln_acm_reg.end_date = com_sect_acm.end_date;
			ln_acm_reg.cal_date = g_pub_tx.tx_date;
			ln_acm_reg.rate = com_sect_acm.rate;
			ln_acm_reg.intst_acm = com_sect_acm.intst_acm;
			ln_acm_reg.part_intst_acm = com_sect_acm.intst_acm;
			ln_acm_reg.intst = com_sect_acm.intst_acm*com_sect_acm.rate/30/1000;
			strcpy( ln_acm_reg.intst_type , com_sect_acm.intst_type );
			strcpy( ln_acm_reg.type , "0" );	/*�ɷֶλ�����ȡ��*/
			ln_acm_reg.trace_no = g_pub_tx.trace_no;
			TRACE
			sprintf( acErrMsg,"ln_acm_reg.intst=[%lf]",ln_acm_reg.intst);
			WRITEMSG
			ret = Ln_acm_reg_Ins( ln_acm_reg , g_pub_tx.reply );
			if( ret )	return 1;
			TRACE
						
		}
		}
	}
	
	Com_sect_acm_Clo_Upd( );
	sprintf(acErrMsg,"���·ֶλ�������״̬��־ PASS");
	WRITEMSG
	*intst = pub_base_PubDround_1( *intst );
vtcp_log("[%s][%d]g_ln_mst.opn_br_no=[%s]\n",__FILE__,__LINE__,g_ln_mst.opn_br_no);
										 
	return 0;	
}
int pub_base_CalAcm_tt( beg_date,end_date,day_type,acm_type,bal,amt,acm
	,intst_type,intst_moon,intst_date )
 long beg_date ;
 long end_date ;
 char day_type[2] ; 
 char acm_type[2] ;
 double bal ;
 double amt ;
 double *acm ;
 char intst_type[2];
 long intst_moon ;
 long intst_date ;
{
	long days;/* ���� */
	int ret;
	long end_acm_day; /* ��Ϣ���� */
	double vamt;
	double vacm;

	sprintf(acErrMsg, "FUNCTIONS %s bd[%ld] ed[%ld]  t[%lf] [%ld] {%s}{%s}", __FUNCTION__, beg_date, end_date, *acm, (long)acm, day_type, acm_type);	
	WRITEMSG
sprintf(acErrMsg, "FUNCTIONS %s bd[%ld] ed[%ld]  t[%lf] [%ld] {%s}{%s}", __FUNCTION__, beg_date, end_date, *acm, (long)acm, day_type, acm_type);
   WRITEMSG

	sprintf(acErrMsg,"����acmΪ[%lf][%ld], day_type [%s], acm_type [%s], amt=[%lf], bal[%lf] beg_date[%d] end_date[%d]", 
		*acm, (long)acm, day_type, acm_type, amt, bal, beg_date, end_date);
	WRITEMSG	
	days=0;
	vamt=bal;
	
	if ( acm_type[0]=='0' )	/* ���� */
	{
TRACE
		return 0;
	}
	else if ( acm_type[0]=='1' )	/* ÿ���ۼ� */
	{
TRACE
		return 0;
	}
	else if ( acm_type[0]=='2' )	/* �䶯�ۼ� */
	{
			/* �������� */	
TRACE
			ret = pub_base_CalDays( beg_date , end_date , day_type , &days );
			if ( ret )
			{
				sprintf(acErrMsg,"���ü���������������!");
				WRITEMSG
				return 1;	
			}
			sprintf(acErrMsg,"���ü����������� PASS!����Ϊ[%ld]",days);
			WRITEMSG
			
	}
	else if ( acm_type[0]=='3' )	/* �䶯�ۼƵ��� */
	{
        /* ����Ϣ���� */
		TRACE
		/* �����ȡ �����汾�岻�ܱ䶯�ۼƵ������ ****
        if (intst_type[0] == '1')
        {
             sprintf(acErrMsg,"���汾�岻�ܱ䶯�ۼƵ������!!");
             WRITEMSG
             return 1;
        }
		***********************************************/

		vamt=amt;
			pub_base_next_intst_date(intst_type,intst_moon,
				intst_date,&end_acm_day);
		vtcp_log( "NEXT INTST[%d]",end_acm_day );
			if ( end_acm_day<=19000101 )
			{
				sprintf(acErrMsg,"��Ϣ����Ϊ[%ld]",end_acm_day);
				WRITEMSG					
				days=0;
			}
			else
			{
				/* �������� */	
				ret = pub_base_CalDays( end_date,end_acm_day,day_type,&days );
				if ( ret )
				{
					sprintf(acErrMsg,"���ü���������������!");
					WRITEMSG
					return 1;	
				}
				days = days + 1;
				sprintf(acErrMsg,"���ü����������� PASS!����Ϊ[%ld]",days);
				WRITEMSG	
			}
	}
	else
	{
		sprintf(acErrMsg,"�������ʽ����[%s]!",acm_type);
		WRITEMSG
		strcpy( g_pub_tx.reply,"P146" );
		return 1;	
	}

	/**20030617 rob**/
	if( days<0 ) days=0;

	/* ������� */
	vacm=*acm;
	vacm+=days*vamt;
	sprintf(acErrMsg,"����Ϊ[%lf]days[%d]amt[%.2lf]",vacm,days,vamt);
	WRITEMSG
	/* �������� */
	vacm= pub_base_PubDround_1( vacm );	
	sprintf(acErrMsg,"����Ϊ[%lf]",vacm);
	WRITEMSG	
	/**/		 
	*acm =vacm;
	return 0;
}
/**********************************************************************
* �� �� ����  pub_base_CalAcm_Ln
* �������ܣ�  �������
* ��    �ߣ�  lance
* ���ʱ�䣺  2003��01��14��
*
* ��    ���� 
*     ��  �룺beg_date   long	  ��ʼ����(�ϱʷ�������)   
*      	      end_date   ilong	  ��ֹ����(�������ڣ��������ڣ�) 
*	      	  day_type	 char	  ��Ϣ�������ͣ�0��ʵ��������1��30����㣩
*             acm_type   char     �����������ͣ�1ÿ���ۼ�2�䶯�ۼ�3�䶯�ۼƵ���
*             bal        double   ����ǰ���(������Ϣ��)
*             amt        double   ���(���Ӧ���뷢�������Ϊ����Ϊ��)
*             acm        double   ��Ϣ����
*(������Ϣ��)     
* intst_type��Ϣ���� 0����Ϣ1���汾��2���ռ�Ϣ3���¼�Ϣ4������Ϣ5�����Ϣ
* intst_mon��Ϣ�·�
* intst_date��Ϣ����
*     ��  ��: acm        double   ��Ϣ����
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/
int pub_base_CalAcm_Ln( beg_date,end_date,day_type,acm_type,bal,amt,acm
	,intst_type,intst_moon,intst_date )
 long beg_date ;
 long end_date ;
 char day_type[2] ; 
 char acm_type[2] ;
 double bal ;
 double amt ;
 double *acm ;
 char intst_type[2];
 long intst_moon ;
 long intst_date ;
{

	long days;/* ���� */
	int ret;
	long end_acm_day; /* ��Ϣ���� */
	double vamt;
	double vacm;

sprintf(acErrMsg, "FUNCTIONS %s bd[%ld] ed[%ld]  t[%lf] [%ld] {%s}{%s}", __FUNCTION__, beg_date, end_date, *acm, (long)acm, day_type, acm_type);
   WRITEMSG

	sprintf(acErrMsg,"����acmΪ[%lf][%ld], day_type [%s], acm_type [%s], amt=[%lf], bal[%lf] beg_date[%d] end_date[%d]", 
		*acm, (long)acm, day_type, acm_type, amt, bal, beg_date, end_date);
	WRITEMSG	
	days=0;
	vamt=bal;
	
	if ( acm_type[0]=='0' )	/* ���� */
	{
TRACE
		return 0;
	}
	else if ( acm_type[0]=='1' )	/* ÿ���ۼ� */
	{
TRACE
		return 0;
	}
	else if ( acm_type[0]=='2' )	/* �䶯�ۼ� */
	{
			/* �������� */	
TRACE
			ret = pub_base_CalDays( beg_date , end_date , day_type , &days );
			if ( ret )
			{
				sprintf(acErrMsg,"���ü���������������!");
				WRITEMSG
				return 1;	
			}
			sprintf(acErrMsg,"���ü����������� PASS!����Ϊ[%ld]",days);
			WRITEMSG
			
	}
	else if ( acm_type[0]=='3' )	/* �䶯�ۼƵ��� */
	{
        /* ����Ϣ���� */
		TRACE
		/* �����ȡ �����汾�岻�ܱ䶯�ۼƵ������ ****
        if (intst_type[0] == '1')
        {
             sprintf(acErrMsg,"���汾�岻�ܱ䶯�ۼƵ������!!");
             WRITEMSG
             return 1;
        }
		***********************************************/

		vamt=amt;
			pub_base_next_intst_date(intst_type,intst_moon,
				intst_date,&end_acm_day);
		vtcp_log( "NEXT INTST[%d]",end_acm_day );
			if ( end_acm_day<=19000101 )
			{
				sprintf(acErrMsg,"��Ϣ����Ϊ[%ld]",end_acm_day);
				WRITEMSG					
				days=0;
			}
			else
			{
				/* �������� */	
				ret = pub_base_CalDays( end_date,end_acm_day,day_type,&days );
				if ( ret )
				{
					sprintf(acErrMsg,"���ü���������������!");
					WRITEMSG
					return 1;	
				}
				days = days + 1;
				sprintf(acErrMsg,"���ü����������� PASS!����Ϊ[%ld]",days);
				WRITEMSG	
			}
	}
	else
	{
		sprintf(acErrMsg,"�������ʽ����[%s]!",acm_type);
		WRITEMSG
		strcpy( g_pub_tx.reply,"P146" );
		return 1;	
	}

	/**20030617 rob**/
	if( days<0 ) days=0;

	/* ������� */
	vacm=*acm;
	vacm+=days*vamt;
	sprintf(acErrMsg,"����Ϊ[%lf]days[%d]amt[%.2lf]",vacm,days,vamt);
	WRITEMSG
	/* �������� */
	vacm= pub_base_PubDround_1( vacm );	
	sprintf(acErrMsg,"����Ϊ[%lf]",vacm);
	WRITEMSG	
	/**/		 
	*acm =vacm;
	return 0;
}
