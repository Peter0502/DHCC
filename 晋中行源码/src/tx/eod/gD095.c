/*************************************************
* �� �� ��: gD095.c
* �������������ɰ��Ҵ���24���»���״̬
*
* ��    ��: lance
* �������: 2003��10��31��
* �޸�����:
* �� �� ��:
* �޸�����:
*
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#define EXTERN
#include "public.h"
#include <errno.h>
#include "ln_reg_c.h"
#include "ln_lo_c.h"
#include "ln_paystatus_c.h"


gD095()
{
	int ret=0;
	int str_flag=0;
	int sum_i=0;
		
	long today=0;
	long date_24=0;
	long date_24_fro=0;
	long date_24_bak=0;
	long date_24_yy=0;
	long date_24_mm=0;
	long fir_spay_date=0;
	long date_bz=0;
	long lo_days=0;
	long yy=0;
	long mm=0;
	long dd=0;
	
			
	struct ln_reg_c	S_ln_reg;
	struct ln_lo_c	S_ln_lo;	
	struct ln_paystatus_c	S_ln_paystatus;	
	
	memset(&S_ln_reg,0x00,sizeof(struct ln_reg_c));	
	memset(&S_ln_lo,0x00,sizeof(struct ln_lo_c));			
	
	ret=sql_begin(); /*������*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

   	/* ��ʼ��ȫ�ֱ��� */
   	pub_base_sysinit();
   	
 	/** ÿ���������� **/
	if( g_pub_tx.tx_date%100!=1 )
	goto OkExit;  
		
	pub_base_deadlineD(g_pub_tx.tx_date,-1,&today);
	sprintf(acErrMsg,"���� today=[%ld]",today);
	WRITEMSG
		
	ret = Eod_ln_mst_Dec_Sel(g_pub_tx.reply," repay_type in('3','4') ");
	if (ret != 0)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}	
	while(1)
	{
		ret = Eod_ln_mst_Fet_Sel(&g_ln_mst, g_pub_tx.reply);
		if (ret&&ret!=100 )
		{
			sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}else if( ret==100 ){
			break;
		}
		sprintf(acErrMsg,"�� [%d]��",sum_i);
		WRITEMSG
		sum_i++;
		memset(&S_ln_paystatus,0x00,sizeof(struct ln_paystatus_c));		
		/* ȷ��24����ǰ���� */
		date_24=(today/10000-2)*10000+today%10000;
		date_24_yy=date_24/10000;
		date_24_mm=date_24%10000/100;	
			
		sprintf( acErrMsg,"24����ǰ������Ϊ[%ld]",date_24);
		WRITEMSG	
		for( str_flag=0; str_flag<24; str_flag++ )
		{
			/*ȷ��ÿ���µ���ʼ��ֹ����*/
			date_24_mm++;
			if( date_24_mm==13 )
			{
				date_24_yy++;
				date_24_mm=1;
			}
			date_24_fro=date_24_yy*10000+date_24_mm*100+01;
			if( date_24_mm==1||date_24_mm==3||date_24_mm==5||date_24_mm==7||date_24_mm==8||date_24_mm==10||date_24_mm==12 )
			{
				date_24_bak=date_24_yy*10000+date_24_mm*100+31;
			}else{
				date_24_bak=date_24_yy*10000+date_24_mm*100+30;
			}
			if( date_24_mm==2 )
			{
				if( pub_base_chk_leap(date_24_yy) )
				{
					date_24_bak=date_24_yy*10000+date_24_mm*100+28;	
				}else{
					date_24_bak=date_24_yy*10000+date_24_mm*100+29;	
				}
			}			
			
			if( g_ln_mst.opn_date> date_24_bak )/*��δ����*/
			{
				S_ln_paystatus.pay_sts[str_flag]='/';
			}else if( g_ln_mst.opn_date>= date_24_fro && g_ln_mst.opn_date<= date_24_bak ){	/*���¿���*/
				S_ln_paystatus.pay_sts[str_flag]='*';
			}else if( g_ln_mst.opn_date< date_24_fro ){ /*�ѿ���*/	
				
				/*��ѯ�����Ƿ����*/
				if( pub_base_CompDblVal( g_ln_mst.bal,0.00)<=0  )
				{
					S_ln_paystatus.pay_sts[str_flag]='C';
					S_ln_paystatus.clear_sts[0]='1';	
				}else{
					/*�Էǰ��½�Ϣ���Ĵ���*/
					ret=Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm,"prdt_no='%s'",g_ln_mst.prdt_no);
					if(ret)	 goto ErrExit;
					
					
					ret=pub_base_get_ymd(&yy,&mm,&dd,date_24_bak,&date_bz);
					if(ret) goto ErrExit;
					if( g_ln_parm.pay_term_type[0]=='Y' && date_bz!=10 ){
						S_ln_paystatus.pay_sts[str_flag]='*';
					}else if( g_ln_parm.pay_term_type[0]=='Q' && ( date_bz!=10 && date_bz!=6 && date_bz!=8 ) ){
						S_ln_paystatus.pay_sts[str_flag]='*';
					}else{			
						/*�鿴����Ƿ������*/
						ret=sql_min_long("ln_lo","shld_pay_date",&fir_spay_date,"ac_id=%ld and ac_seqn=%d and pay_type='0'",g_ln_mst.ac_id,g_ln_mst.ac_seqn);
						if(ret)	goto ErrExit;
						
						if( fir_spay_date> date_24_bak )/*��δǷ��*/
						{
							S_ln_paystatus.pay_sts[str_flag]='N';
						}else{/*��Ƿ��*/	
							/*����Ƿ������*/
							if( fir_spay_date==0 )
							{
								S_ln_paystatus.pay_sts[str_flag]='N';
							}else{	
								lo_days=pub_base_CalTrueDays( fir_spay_date,date_24_bak );
								if( lo_days<=30 )
								{
									S_ln_paystatus.pay_sts[str_flag]='1';
								}else if( lo_days<=60 ){
									S_ln_paystatus.pay_sts[str_flag]='2';
								}else if( lo_days<=90 ){
									S_ln_paystatus.pay_sts[str_flag]='3';
								}else if( lo_days<=120 ){
									S_ln_paystatus.pay_sts[str_flag]='4';
								}else if( lo_days<=150 ){
									S_ln_paystatus.pay_sts[str_flag]='5';
								}else if( lo_days<=180 ){																																
									S_ln_paystatus.pay_sts[str_flag]='6';
								}else{
									S_ln_paystatus.pay_sts[str_flag]='7';	
								}
							}					
						}		    	
					}
				}
			}
		}
		
		S_ln_paystatus.ac_id=g_ln_mst.ac_id;
		S_ln_paystatus.ac_seqn=g_ln_mst.ac_seqn;
		if(S_ln_paystatus.clear_sts[0]!='1')
		{
			S_ln_paystatus.clear_sts[0]='0';
		}
                ret = Ln_paystatus_Ins(S_ln_paystatus, g_pub_tx.reply);
                if (ret)
                {
                    sprintf(acErrMsg,"Insert 24�´������������! ret=[%d]",
                            ret);
                    WRITEMSG
                    goto ErrExit;
                }
	}		
	Eod_ln_mst_Clo_Sel();				

OkExit:
	sql_commit();
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"----->����24�°��Ҵ���������ɹ���[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
    
ErrExit:
	sql_rollback();
	sprintf(acErrMsg,"----->����24�°��Ҵ���������ʧ�ܣ�[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
