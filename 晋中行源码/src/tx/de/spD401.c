/***************************************************************
* �� �� ��: spD401.c
* ��������: ��ʰ����
* ��    ��: mike
* �������: 2004��1��10��
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_coll_c.h"

spD401()
{
	int ret=0;

	/*����ṹ*/
	struct mo_coll_c	mo_coll_c;
	struct mo_coll_c	g_mo_coll;

	/*��սṹ*/
	memset(&g_mo_coll,0x00,sizeof(struct mo_coll_c));
	memset(&mo_coll_c,0x00,sizeof(struct mo_coll_c));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}

	/*ȡֵ����ֵ*/
	get_zd_data("0670",mo_coll_c.sts);				/*�����־*/
	get_zd_data("1021",mo_coll_c.ac_no);			/*��/�ʺ�*/
	strcpy(g_pub_tx.ac_no,mo_coll_c.ac_no);
	get_zd_data("0250",mo_coll_c.name);				/*����*/

	get_zd_data("1142",g_pub_tx.draw_pwd_yn);		/*�Ƿ�ƾ��*/
	get_zd_data("1144",g_pub_tx.draw_pwd);			/*֧ȡ����*/
	get_zd_data("1145",g_pub_tx.draw_id_yn);		/*֤��֧ȡ*/
	get_zd_data("1148",g_pub_tx.id_type);			/*֤������*/
	get_zd_data("1146",g_pub_tx.id_no);				/*֤������*/

	mo_coll_c.wrk_date = g_pub_tx.tx_date; 
	mo_coll_c.trance_no = g_pub_tx.trace_no; 

	if( mo_coll_c.sts[0]=='1' )			 			/*1 - ��ʰ*/
	{
		mo_coll_c.pick_date = g_pub_tx.tx_date;
		strcpy(mo_coll_c.pick_br_no ,g_pub_tx.tx_br_no); 
		strcpy(mo_coll_c.pick_tel , g_pub_tx.tel); 

		mo_coll_c.ret_date = 0; 
		strcpy(mo_coll_c.ret_br_no, ""); 
		strcpy(mo_coll_c.ret_tel, ""); 

		ret = Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'",mo_coll_c.ac_no);
		if( ret==100 )
		{
			sprintf(acErrMsg,"����ȡ�����˺Ŷ��ձ��α����![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D101");
			goto ErrExit;
		}

		ret = Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
		if( ret==100 )
		{
			sprintf(acErrMsg,"�����˺Ŷ��ձ��в����ڸ��˻�![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D104");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}

		if( g_mdm_ac_rel.coll_sts[0]=='1' )
		{
			sprintf(acErrMsg,"���˻��Ѿ����ڼ�ʰ״̬![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"W038");
			goto ErrExit;
		}
		else 
			strcpy(g_mdm_ac_rel.coll_sts,"1");

		ret = Mdm_ac_rel_Upd_Upd(g_mdm_ac_rel,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"�޸Ľ����˺Ŷ��ձ����![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D105");
			goto ErrExit;
		}

		Mdm_ac_rel_Clo_Upd( );


		/*�ǼǼ�ʰ�ǼǱ�*/
		ret = Mo_coll_Ins(mo_coll_c,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"�ǼǼ�ʰ�ǼǱ�����![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"W041");
			goto ErrExit;
		}
	}
	else if( mo_coll_c.sts[0]=='2' )				/* 2-�黹 */
	{
		ret = Mo_coll_Dec_Upd( g_pub_tx.reply,"ac_no='%s' and sts='1'", \
						   g_pub_tx.ac_no );
		if( ret==100 )
		{
			sprintf(acErrMsg,"�����ڸ��˻��ļ�ʰ��¼![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"W039");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		ret = Mo_coll_Fet_Upd(&g_mo_coll,g_pub_tx.reply);
		if(ret&&ret!=100)
		{
			sprintf(acErrMsg,"���ݿ�ʧ��[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
			
		}
		else if(ret==100)
		{
			sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		if( strncmp(g_pub_tx.tx_br_no,g_mo_coll.pick_br_no,5) )
		{
			sprintf(acErrMsg,"������δ��ʰ�ý��ʣ��뵽��ʰ�а���黹![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D199");
			goto ErrExit;
		}

		mo_coll_c.pick_date = 0;
		strcpy(mo_coll_c.pick_br_no,""); 
		strcpy(mo_coll_c.pick_tel, ""); 

		mo_coll_c.ret_date = g_pub_tx.tx_date; 
		strcpy(mo_coll_c.ret_br_no , g_pub_tx.tx_br_no); 
		strcpy(mo_coll_c.ret_tel , g_pub_tx.tel); 

		/*****�����ٵ�*****/
		memcpy(mo_coll_c.rowid,g_mo_coll.rowid,sizeof(mo_coll_c.rowid)-1);
		
		ret = Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'",mo_coll_c.ac_no);
		if( ret )
		{
			sprintf(acErrMsg,"����ȡ�����˺Ŷ��ձ��α����![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D101");
			goto ErrExit;
		}

		ret = Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
		if( ret==100 )
		{
			sprintf(acErrMsg,"�����˺Ŷ��ձ��в����ڸ��˻�![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D104");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}

		if( g_mdm_ac_rel.coll_sts[0]=='0' )
		{
			sprintf(acErrMsg,"�����ڸ��˻��ļ�ʰ��¼![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"W039");
			goto ErrExit;
		}
		else
		{
			/*����֧ȡ��ʽ*/
			ret=pub_base_check_draw("001");
			if (ret)
			{
				sprintf(acErrMsg,"֧ȡ��ʽ����ȷ![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}

			/*״̬�޸�Ϊ����*/
			strcpy(g_mdm_ac_rel.coll_sts,"0");
		}

		ret = Mdm_ac_rel_Upd_Upd(g_mdm_ac_rel,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"�޸Ľ����˺Ŷ��ձ����![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D105");
			goto ErrExit;
		}

		Mdm_ac_rel_Clo_Upd( );

		/*�ǼǼ�ʰ�ǼǱ�*/
		ret = Mo_coll_Upd_Upd(mo_coll_c,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"�ǼǼ�ʰ�ǼǱ�����![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"W041");
			goto ErrExit;
		}

		set_zd_data("0680",g_mdm_ac_rel.note_sts);	/*���ع�ʧ״̬*/
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
	sprintf(acErrMsg,"��ʰ����ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"��ʰ����ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

