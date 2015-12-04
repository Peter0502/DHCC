#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_jl_bal_c.h"

/**********************************************************************
* �� �� ����  pub_eod_ye_bal
* �������ܣ�  ������������
********************************************************************/
int pub_eod_ye_bal ( ye_bal , bal , ac_id_tmp , ac_seqn_tmp)
 double ye_bal ;
 double bal ;
 long ac_id_tmp ;
 int ac_seqn_tmp;
{
  	int ret;

  	struct mo_jl_bal_c	mo_jl_bal;	
   	struct mo_jl_bal_c	mo_jl_bal_new;	
    	
	memset( &mo_jl_bal,0x00,sizeof(struct mo_jl_bal_c) );	
	memset( &mo_jl_bal_new,0x00,sizeof(struct mo_jl_bal_c) );	
	
	if( pub_base_CompDblVal( ye_bal , bal )==0 )
	{
		/* ����ޱ仯������ */
		return 0;
	}
	else
	{
		/* ����б仯ʱ */
		/* ��ѯ�����鿴�Ƿ��м�¼ */
		ret = Mo_jl_bal_Dec_Upd( g_pub_tx.reply , 
			"ac_id=%ld and ac_seqn=%d and end_date=99999999" , 
			ac_id_tmp , ac_seqn_tmp );
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Mo_jl_bal_Dec_Upd��!ret=[%d]", ret);
			WRITEMSG					
			return(1);
		}

		ret = Mo_jl_bal_Fet_Upd( &mo_jl_bal , g_pub_tx.reply );
		if( ret==100 )
		{
			TRACE
			Mo_jl_bal_Clo_Upd();
		}
		else if( ret )
		{
			sprintf( acErrMsg, "ִ��Mo_jl_bal_Fet_Upd��!ret=[%d]", ret);
			WRITEMSG					
			return(1);
		}
		else 
		{
			TRACE
			/* �޸������е�������(��ǰ���ڵ�ǰһ��) */
			mo_jl_bal.end_date = pub_base_daynumSUB( g_pub_tx.tx_date , 1 );
			mo_jl_bal.bal = ye_bal;
			
			ret = Mo_jl_bal_Upd_Upd( mo_jl_bal , g_pub_tx.reply);
			if ( ret )
			{
				sprintf( acErrMsg,"����prdt_ac_idʱ���� [%d]",ret);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"D105" );
				return 1;
			}
						
			Mo_jl_bal_Clo_Upd();
		}			
		
		/* ���һ���µļ�¼ */
		mo_jl_bal_new.ac_id = ac_id_tmp;
		mo_jl_bal_new.ac_seqn = ac_seqn_tmp;
		mo_jl_bal_new.bal = bal;
		mo_jl_bal_new.beg_date = g_pub_tx.tx_date;
		mo_jl_bal_new.end_date = 99999999;
		
		ret = Mo_jl_bal_Ins( mo_jl_bal_new , g_pub_tx.reply );
		if (ret )	
		{
			sprintf( acErrMsg, "�����������ʱ�쳣����!ret=[%d]", ret);
			WRITEMSG			
			return 1;
		}
	}
			
	return 0;
}

