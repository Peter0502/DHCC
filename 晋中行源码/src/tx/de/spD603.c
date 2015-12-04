/***************************************************************
* �� �� ��: spD603.c
* ����������֧��������Ϣ����
* ��    ��: mike
* ������ڣ�2003��06��09��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "paymach_mana_inf_c.h"
#include "prdt_ac_id_c.h"

spD603()
{   
	int ret=0; 
	char opt_code[2];
	char tmp_ac_no[128];
	char inf[50];										/*������Ӧ��Ϣ*/

	struct paymach_mana_inf_c	g_paymach_mana_inf;
	struct prdt_ac_id_c			g_prdt_ac_id;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_paymach_mana_inf,0x00,sizeof(struct paymach_mana_inf_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data("0750",tmp_ac_no);  			        	/* �˺� */
	get_zd_data("0730",g_paymach_mana_inf.id_no );      	/* ���ߺ��� */
	get_zd_data("0070",g_paymach_mana_inf.oper_no ); 		/* tel_no */
	get_zd_data("0670",opt_code );					      	/* �����־ */
								/*0�����ӣ�1��ɾ����2��ͣ�ã�3�����ã�4��ע��*/
	get_zd_long("0440",&g_paymach_mana_inf.pr_date );  	 	/* �������� */

	sprintf(acErrMsg,"---------->��ǰ���˺�Ϊ[%s]", tmp_ac_no );
	WRITEMSG
	memcpy( g_paymach_mana_inf.ac_no, tmp_ac_no+12, 20 );
	g_paymach_mana_inf.ac_no[20] = '\0';
   	sprintf(acErrMsg,"---------->�غ���˺�Ϊ[%s]", g_paymach_mana_inf.ac_no );
	WRITEMSG
	
	if ( g_paymach_mana_inf.ac_no[0] == '0' )
	{
		memset( g_paymach_mana_inf.ac_no, 0x0, sizeof(g_paymach_mana_inf.ac_no) );
		memcpy( g_paymach_mana_inf.ac_no, tmp_ac_no+19, 13 );
		g_paymach_mana_inf.ac_no[13] = '\0';
   		sprintf(acErrMsg,"---------->�غ���˺�Ϊ[%s]", g_paymach_mana_inf.ac_no );
		WRITEMSG
	}

	if( opt_code[0] != '0' )
	{   		
		pub_base_old_acno( g_paymach_mana_inf.ac_no );
	}

	switch(opt_code[0])									
	{		
		case '0':										/***0-����***/
				strcpy(g_paymach_mana_inf.flag,"0");	/***0-����***/
														/***1-ͣ��***/
														/***4-ע��***/
				ret = Paymach_mana_inf_Ins(g_paymach_mana_inf,g_pub_tx.reply);
  				if( ret )
  				{
	   				sprintf(acErrMsg,"�Ǽ�֧��������Ϣ��������!");
	  				WRITEMSG
	   				strcpy(g_pub_tx.reply,"O202");
	   				goto ErrExit;
				}

				/***�޸�֧�������־***/
				ret = Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'", \
										 g_paymach_mana_inf.ac_no);
  				if( ret )
  				{
	   				sprintf(acErrMsg,"�����α����!");
	  				WRITEMSG
	   				goto ErrExit;
				}

				ret = Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
  				if( ret==100 )
  				{
	   				sprintf(acErrMsg,"�ʺŲ�����!");
	  				WRITEMSG
	   				strcpy(g_pub_tx.reply,"M003");
	   				goto ErrExit;
				}
				else if( ret )
				{
					sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}

				strcpy(g_mdm_ac_rel.pwd_mach_yn,"Y");

				ret = Mdm_ac_rel_Upd_Upd(g_mdm_ac_rel,g_pub_tx.reply);
  				if( ret )
  				{
	   				sprintf(acErrMsg,"�޸ļ�¼����!");
	  				WRITEMSG
	   				goto ErrExit;
				}

				Mdm_ac_rel_Clo_Upd( );
				break;
		case '1':										/***1-ɾ��***/

				ret = sql_count("paymach_mana_inf","ac_no='%s' and id_no='%s'",
							g_paymach_mana_inf.ac_no,g_paymach_mana_inf.id_no);
				if( ret<0 )
  				{
	   				sprintf(acErrMsg,"����sql_count����!");
	  				WRITEMSG
	   				goto ErrExit;
				}
				else if( ret==0 )
  				{
	   				sprintf(acErrMsg,"�����ڸ��ʻ��ͻ��߹�ϵ!ac_no='%s' and id_no='%s'",g_paymach_mana_inf.ac_no,g_paymach_mana_inf.id_no);
	  				WRITEMSG
	   				strcpy(g_pub_tx.reply,"O207");
	   				goto ErrExit;
				}

				ret = sql_execute("delete from paymach_mana_inf where ac_no='%s' and id_no='%s'",g_paymach_mana_inf.ac_no,g_paymach_mana_inf.id_no);
  				if( ret )
  				{
	   				sprintf(acErrMsg,"ɾ��֧��������Ϣ������¼����!");
	  				WRITEMSG
	   				strcpy(g_pub_tx.reply,"O203");
	   				goto ErrExit;
				}
				vtcp_log("--------INSERT PAYMACHINE TABLE SUCCESS--------");

				/***�޸�֧�������־***/
				ret = Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'", \
										 g_paymach_mana_inf.ac_no);
  				if( ret )
  				{
	   				sprintf(acErrMsg,"�����α����!");
	  				WRITEMSG
	   				goto ErrExit;
				}

				ret = Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
  				if( ret==100 )
  				{
	   				sprintf(acErrMsg,"�ʺŲ�����!");
	  				WRITEMSG
	   				strcpy(g_pub_tx.reply,"M003");
	   				goto ErrExit;
				}
				else if( ret )
				{
					sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}

				strcpy(g_mdm_ac_rel.pwd_mach_yn,"N");

				ret = Mdm_ac_rel_Upd_Upd(g_mdm_ac_rel,g_pub_tx.reply);
  				if( ret )
  				{
	   				sprintf(acErrMsg,"�޸ļ�¼����!");
	  				WRITEMSG
	   				goto ErrExit;
				}

				Mdm_ac_rel_Clo_Upd( );
				break;
		case '2':										/***2-ͣ��***/
				ret = sql_execute("update paymach_mana_inf set flag='1' where id_no='%s' and flag='0'",g_paymach_mana_inf.id_no);
  				if( ret )
  				{
	   				sprintf(acErrMsg,"����֧��������Ϣ������¼����!");
	  				WRITEMSG
	   				strcpy(g_pub_tx.reply,"O204");
	   				goto ErrExit;
				}
				vtcp_log("--------UPDATE PAYMACHINE TABLE SUCCESS--------");
				break;
		case '3':										/***3-����***/
				ret = sql_execute("update paymach_mana_inf set flag='0' where id_no='%s' and flag='1'",g_paymach_mana_inf.id_no);
  				if( ret )
  				{
	   				sprintf(acErrMsg,"����֧��������Ϣ������¼����!");
	  				WRITEMSG
	   				strcpy(g_pub_tx.reply,"O204");
	   				goto ErrExit;
				}
				vtcp_log("--------UPDATE PAYMACHINE TABLE SUCCESS--------");
				break;
		case '4':										/***4-ע��***/
				ret = sql_count("paymach_mana_inf","id_no='%s'", \
								 g_paymach_mana_inf.id_no);
				if(ret<0)
  				{
	   				sprintf(acErrMsg,"����sql_count����!");
	  				WRITEMSG
	   				goto ErrExit;
				}
				else if(ret==0)
  				{
	   				sprintf(acErrMsg,"�����ڸû���!");
	  				WRITEMSG
	   				strcpy(g_pub_tx.reply,"O208");
	   				goto ErrExit;
				}

				ret = sql_execute("delete from paymach_mana_inf where id_no='%s'",g_paymach_mana_inf.id_no);
  				if( ret )
  				{
	   				sprintf(acErrMsg,"ɾ��֧��������Ϣ������¼����!");
	  				WRITEMSG
	   				strcpy(g_pub_tx.reply,"O203");
	   				goto ErrExit;
				}
				vtcp_log("--------DELETE PAYMACHINE TABLE SUCCESS--------");
				break;
		default:
	   			sprintf(acErrMsg,"֧�����봦�����Ͳ�����!!");
	  			WRITEMSG
	   			strcpy(g_pub_tx.reply,"O205");
	   			goto ErrExit;
	}

    /*** �Ǽǽ�����ˮ ***/
	if( pub_ins_trace_log() )
	{
	 	sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
	  	WRITEMSG
	    goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"֧���������ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"֧���������ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	pub_base_regetinf(g_pub_tx.reply,inf);
	set_zd_data("0130",inf);
	return 1;
}
