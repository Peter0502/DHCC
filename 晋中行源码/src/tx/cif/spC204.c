/*************************************************
* �� �� ��: spC204.c
* ������������Ա������Ϣ¼��
*              
*
* ��    ��: ���Ϊ
* ������ڣ�2014��06��13��
* �޸ļ�¼��   
*     1. ��    ��: 
*        �� �� ��: 
*        �޸�����:
insert into tx_def (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS)
values ('1204', '��Ա�����ϵά��', '10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '1', '6', '2');

insert into tx_flow_def (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT)
values ('1204', 0, 'C204', '#$');

insert into tx_sub_def (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND)
values ('C204', '��Ա�����ϵά��', 'spC204', '0', '0');

insert into auth_item (TX_CODE, SEQN, CODE_AC, CODE_SEQN, CODE_AMT, CODE_DC, CODE_CT, CODE_XX, CODE_YY, CODE_ZZ, IND, MO)
values ('1204', 1, '', '', '', '', '', '', '', '', '0000', '��Ա����ά��');

insert into auth_cond (TYPE, NO, TX_CODE, SEQN, MO, COND)
values ('S', 0, '1204', 1, '��Ա����ά��', 'sq(1237)');
*************************************************/
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "tel_save_rel_c.h"
#include "com_tel_c.h"
#include "mdm_ac_rel_c.h"

spC204()  
{
	struct 	tel_save_rel_c		tel_save_rel;		/* ��Ա������Ϣ�� */
	struct 	tel_save_rel_c		tel_save_rel_tmp;	
	struct 	com_tel_c		com_tel_tmp;		/* ��Ա��Ϣ�� */
	struct 	mdm_ac_rel_c		g_mdm_ac_rel		;		
	int ret=0;
	char flag[2];

	memset ( &tel_save_rel, 0x00, sizeof( struct tel_save_rel_c ) );
	memset ( &tel_save_rel_tmp, 0x00, sizeof( struct tel_save_rel_c ) );
	memset ( &g_mdm_ac_rel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset ( &com_tel_tmp, 0x00, sizeof( struct com_tel_c ) );
	memset ( flag, 0x00, sizeof(flag) );
	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0370", tel_save_rel.ac_no );		/* �����˺� */
	get_zd_data( "0920", tel_save_rel.tel );		/* ��Ա���� */
	get_zd_long( "0340",  &tel_save_rel.ac_seqn ); 		/* �˻���� */
	get_zd_data( "0680", tel_save_rel.tx_type );	/* ������ 1�ʺ� 2�ͻ���*/
	get_zd_long( "0290", &tel_save_rel.cif_no ); 
	get_zd_data( "0810", tel_save_rel.brf );  /*  ��ע */
	get_zd_data( "0690", flag );
	
	sprintf( acErrMsg,"spC204 !flag=[%s]",flag);
	WRITEMSG
	
	/* ����Ա���Ƿ���� */
	/* ��Ҫ��ȥ�����׻����ļ�� wudawei 20141008 */
	ret = Com_tel_Sel( g_pub_tx.reply , &com_tel_tmp , "tel='%s' " , tel_save_rel.tel );
	if( ret && ret!=100 )
	{
			sprintf( acErrMsg,"��ȡ��Ա��Ϣʧ��,�������Ļ�����ϵ!ret=[%d]",ret);
                WRITEMSG
                strcpy( g_pub_tx.reply, "C118" );
                goto ErrExit;
  }else if ( ret==100 ){
  		sprintf( acErrMsg,"�ù�Ա������,����!");
                WRITEMSG
                strcpy( g_pub_tx.reply, "C119" );
                goto ErrExit;
  }     	
	
	if(tel_save_rel.tx_type[0] == '1')
	{
		/* ����˺��Ƿ���� ����״̬�Ƿ����� �Ƿ񱾻������ʺ�  */
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel , "ac_no='%s'" , tel_save_rel.ac_no );
		if( ret && ret!=100 )
		{
			sprintf( acErrMsg,"��ȡ�����˺Ŷ��ձ�ʧ��,�������Ļ�����ϵ!ret=[%d]",ret);
		              WRITEMSG
		              strcpy( g_pub_tx.reply, "C114" );
		              goto ErrExit;
		}else if ( ret==100 ){
				sprintf( acErrMsg,"���˺Ų�����,����!");
		              WRITEMSG
		              strcpy( g_pub_tx.reply, "C115" );
		              goto ErrExit;
		}   
		if(strcmp(g_mdm_ac_rel.opn_br_no , g_pub_tx.tx_br_no)!= 0)
		{
					sprintf( acErrMsg,"���ڿ����а����ҵ��!!ret=[%d]",ret);
			            WRITEMSG
		              strcpy( g_pub_tx.reply, "D245" );
		              goto ErrExit;
		}
		if(g_mdm_ac_rel.note_sts[0] != '0') 
		{
			  	sprintf( acErrMsg,"���ʺŽ���״̬������!ret=[%d]",ret);
			            WRITEMSG
		              strcpy( g_pub_tx.reply, "D140" );
		              goto ErrExit;
		}
	}
	
	/* ���� */
	if(flag[0] == '1')
	{
		/* �����ʺ� */
		if(tel_save_rel.tx_type[0] == '1')
		{		
				ret = Tel_save_rel_Sel( g_pub_tx.reply , &tel_save_rel_tmp , "ac_no='%s' and sts='1'" , tel_save_rel.ac_no );
				if(ret == 0)
				{
						sprintf( acErrMsg,"�Ѿ������˴��ʺŵĹ�����Ϣ!ret=[%d]",ret);
			      WRITEMSG
						strcpy( g_pub_tx.reply, "H001" );
			      goto ErrExit;
			  }
				
			  
		} 
		else if(tel_save_rel.tx_type[0] == '2'){
			
				ret = Tel_save_rel_Sel( g_pub_tx.reply , &tel_save_rel_tmp , "cif_no=%ld and sts='1'" , tel_save_rel.cif_no );
				if(ret == 0)
				{
						sprintf( acErrMsg,"�Ѿ������˴��ʺŵĹ�����Ϣ!ret=[%d]",ret);
			      WRITEMSG
						strcpy( g_pub_tx.reply, "H001" );
			      goto ErrExit;
			  }
		}    
		tel_save_rel.tx_date = g_pub_tx.tx_date;
		tel_save_rel.tx_time = g_pub_tx.tx_time;
		tel_save_rel.trace_no = g_pub_tx.trace_no;
		strcpy(tel_save_rel.tx_tel, g_pub_tx.tel);
		strcpy(tel_save_rel.br_no , g_pub_tx.tx_br_no);
		strcpy(tel_save_rel.sts , "1");   	
		/* �Ǽ���������� */			
		ret = Tel_save_rel_Ins( tel_save_rel, g_pub_tx.reply );
		if( ret == -239 )
	    	{
	      	 	sprintf( acErrMsg, "����������Ϣ�ѵǼ�!" );
	      	 	WRITEMSG
	      	 	strcpy( g_pub_tx.reply, "C116" );
	      	 	goto ErrExit;
		}else if ( ret ){
	       		sprintf( acErrMsg, "�Ǽǹ�Ա������Ϣ�ǼǱ��쳣����,�������Ļ�����ϵ!ret=[%d]", ret );
	       		WRITEMSG
	      	 	strcpy( g_pub_tx.reply, "C117" );
	       		goto ErrExit;
		}		
	}
	
	/* ά�� */
	else if(flag[0] == '2')
	{
			if(tel_save_rel.tx_type[0] == '1')
			{		
					ret = Tel_save_rel_Sel( g_pub_tx.reply , &tel_save_rel_tmp , "ac_no='%s' and sts='1'" , tel_save_rel.ac_no );
					if(ret != 0)
					{
							sprintf( acErrMsg,"���˺Ų�����,����!");
			                WRITEMSG
			                strcpy( g_pub_tx.reply, "C115" );
			                goto ErrExit;
				  }
			} 
			else if(tel_save_rel.tx_type[0] == '2')
			{	
					ret = Tel_save_rel_Sel( g_pub_tx.reply , &tel_save_rel_tmp , "cif_no=%ld and sts='1'" , tel_save_rel.cif_no );
					if(ret != 0)
					{
								sprintf( acErrMsg,"�ÿͻ��Ų�����,����!");
			                WRITEMSG
			                strcpy( g_pub_tx.reply, "C051" );
			                goto ErrExit;
				  }
			}    
			tel_save_rel_tmp.tx_date = g_pub_tx.tx_date;
			tel_save_rel_tmp.tx_time = g_pub_tx.tx_time;
			tel_save_rel_tmp.trace_no = g_pub_tx.trace_no;
			strcpy(tel_save_rel_tmp.tx_tel, g_pub_tx.tel);
			strcpy(tel_save_rel_tmp.tel, tel_save_rel.tel);
			strcpy(tel_save_rel_tmp.brf, tel_save_rel.brf);
			/*�޸�*/
			ret = Tel_save_rel_Upd_Upd( tel_save_rel_tmp , g_pub_tx.reply );
			if (ret != 0)
	    {
	        sprintf(acErrMsg,"update error !! [%d]",ret);
	        WRITEMSG
	        Tel_save_rel_Clo_Upd();
	        goto ErrExit;
	    }
	    Tel_save_rel_Clo_Upd();

	}
	
  /* �Ƴ� */
	else if(flag[0] == '3')
	{
			if(tel_save_rel.tx_type[0] == '1')
			{		
					ret = Tel_save_rel_Sel( g_pub_tx.reply , &tel_save_rel_tmp , "ac_no='%s' and sts='1'" , tel_save_rel.ac_no );
					if(ret != 0)
					{
							sprintf( acErrMsg,"���˺Ų�����,����!");
			                WRITEMSG
			                strcpy( g_pub_tx.reply, "C115" );
			                goto ErrExit;
				  }
			} 
			else if(tel_save_rel.tx_type[0] == '2')
			{	
					ret = Tel_save_rel_Sel( g_pub_tx.reply , &tel_save_rel_tmp , "cif_no=%ld and sts='1'" , tel_save_rel.cif_no );
					if(ret != 0)
					{
								sprintf( acErrMsg,"�ÿͻ��Ų�����,����!");
			                WRITEMSG
			                strcpy( g_pub_tx.reply, "C051" );
			                goto ErrExit;
				  }
			}    
			tel_save_rel_tmp.tx_date = g_pub_tx.tx_date;
			tel_save_rel_tmp.tx_time = g_pub_tx.tx_time;
			tel_save_rel_tmp.trace_no = g_pub_tx.trace_no;
			strcpy(tel_save_rel_tmp.tx_tel, g_pub_tx.tel);
			strcpy(tel_save_rel_tmp.sts , "*");  
			/*�޸�sts=*   */
			ret = Tel_save_rel_Upd_Upd( tel_save_rel_tmp , g_pub_tx.reply );
			if (ret != 0)
	    {
	        sprintf(acErrMsg,"update error !! [%d]",ret);
	        WRITEMSG
	        Tel_save_rel_Clo_Upd();
	        goto ErrExit;
	    }
	    Tel_save_rel_Clo_Upd();

	}
	
	  	
	/* ����ˮ��־ */
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�Ǽǹ�Ա������Ϣ�ǼǱ��ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"�Ǽǹ�Ա������Ϣ�ǼǱ�ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
