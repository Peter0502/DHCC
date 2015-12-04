/*********************************************************************
* �� �� �� :  V1.0
* �������� :  ���Ϊ
* ��    �� :  20140613
* ����ģ�� :  �����ϵά��������Ϣ��ѯ
* �������� :  sp9028.c
* �������� :
* �����б� :
* ʹ��ע�� :
* �޸ļ�¼ :
�������ã�
insert into tx_def (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS)
values ('9028', '�����ϵ��ѯ', '10000000000000000000000000000000000000000000000000000000000000000000000000000000100000000001000000000000000000000000000000000000', '1', '4', '2');

insert into tx_flow_def (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT)
values ('9028', 0, '9028', '#$');

insert into tx_sub_def (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND)
values ('9028', '�����ϵ��ѯ', 'sp9028', '0', '0');
***********************************************************************/
#define EXTERN
#include "public.h"
#include "tel_save_rel_c.h"

int sp9028()
{
		int ret = 0;
    struct 	tel_save_rel_c		tel_save_rel;		/* ��Ա������Ϣ�� */
    memset ( &tel_save_rel, 0x00, sizeof( struct tel_save_rel_c ) );
    
		 /**��ʼ�����ýṹ**/
		 if (pub_base_sysinit())
		 {
		     vtcp_log("��ʼ�����ýṹ����!!");
		     strcpy(g_pub_tx.reply, "CU06");
		     goto ErrExit;
		 }
    
    /* ��ǰ̨��Ļȡֵ */
		get_zd_data( "0370", tel_save_rel.ac_no );		/* �����˺� */
		get_zd_data( "0680", tel_save_rel.tx_type );	/* ������ 1�ʺ� 2�ͻ���*/
		get_zd_long( "0290", &tel_save_rel.cif_no ); 
  
		if(tel_save_rel.tx_type[0] == '1')
		{
				ret = Tel_save_rel_Sel( g_pub_tx.reply , &tel_save_rel , "ac_no='%s' and sts='1'" , tel_save_rel.ac_no );
				if( ret && ret!=100 )
				{
						sprintf( acErrMsg,"��ȡ�����ϵʧ��,�������Ļ�����ϵ!ret=[%d]",ret);
			                WRITEMSG
			                strcpy( g_pub_tx.reply, "S048" );
			                goto ErrExit;
			  }else if ( ret==100 ){
			  		sprintf( acErrMsg,"���˺Ų�����,����!");
			                WRITEMSG
			                strcpy( g_pub_tx.reply, "C115" );
			                goto ErrExit;
			  }
			  set_zd_data( "0920", tel_save_rel.tel );
			  set_zd_data( "0810", tel_save_rel.brf );
		}else if(tel_save_rel.tx_type[0] == '2')
		{
				ret = Tel_save_rel_Sel( g_pub_tx.reply , &tel_save_rel , "cif_no='%ld' and sts='1'" , tel_save_rel.cif_no );
				if( ret && ret!=100 )
				{
						sprintf( acErrMsg,"��ȡ�����ϵʧ��,�������Ļ�����ϵ!ret=[%d]",ret);
			                WRITEMSG
			                strcpy( g_pub_tx.reply, "S048" );
			                goto ErrExit;
			  }else if ( ret==100 ){
			  		sprintf( acErrMsg,"�ÿͻ��Ų�����,����!");
			                WRITEMSG
			                strcpy( g_pub_tx.reply, "C051" );
			                goto ErrExit;
			  }
			  set_zd_data( "0920", tel_save_rel.tel );
			  set_zd_data( "0810", tel_save_rel.brf );
		}
OkExit:
    strcpy(g_pub_tx.reply, "0000");
    vtcp_log("success !!: reply is[%s]", g_pub_tx.reply);
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    vtcp_log("fail !!: reply is[%s]", g_pub_tx.reply);
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}
 