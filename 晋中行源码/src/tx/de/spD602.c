/***************************************************************
* �� �� ��: spD602.c
* ����������֧ȡ��Ϣά��
* ��    ��: mike
* ������ڣ�2003��3��9��
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
#include "card.h"
#include "mo_infrm_c.h"
#include "prdt_ac_id_c.h"
#include "com_parm_c.h"
#include "cif_id_code_rel_c.h"

spD602()
{   
	int ret; 
	char ac_no[20],ac_type[2],intst_type[2];
	long tmp_date1,tmp_date2;
	double tmp_amt;
	long date=0;
	char id_type[2];
	char id_no[21];
        char option[2];   /* 20110725 add by yanqq  ������־׷�� */
	char fname[100]={'0'};
	struct mo_infrm_c	g_mo_infrm;
	struct prdt_ac_id_c	g_prdt_ac_id;
	struct com_parm_c sCom_parm;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_mo_infrm,0x00,sizeof(struct mo_infrm_c));
	memset(&sCom_parm, 0x0, sizeof(struct com_parm_c));
	memset(id_type, 0x0, sizeof(id_type));
	memset(id_no, 0x0, sizeof(id_no));
	memset(option,0x0,sizeof(option));    /* 20110725 add by yanqq  ������־׷�� */

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	
	set_zd_long("0460",g_pub_tx.tx_date);/**add by spj 20140721**/
	
	get_zd_data("0300",g_pub_tx.ac_no);          	/* �˺� */
	get_zd_data("0670",g_pub_tx.draw_pwd_yn);      	/* �����־ */
	get_zd_data("0680", g_pub_tx.id_type); 			/* ֤������ */
	get_zd_data("0620", g_pub_tx.id_no);   			/* ֤������ */
	get_zd_data("0690",option);                     /* 20110725 add by yanqq ������־ */

	if( g_pub_tx.ac_no[0]=='5' )
	{
		sprintf(acErrMsg,"�����봢���˻�!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O164");
		goto ErrExit;
	}

	/*�����ʻ�����*/
	ret = Mdm_ac_rel_Dec_Upd(g_pub_tx.reply, "ac_no='%s'",g_pub_tx.ac_no);
	if( ret )
	{
		sprintf(acErrMsg,"�α궨�����!");
		WRITEMSG
		goto ErrExit;
	}

	ret = Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�˺Ų�����!");
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

	if( g_mdm_ac_rel.draw_pwd_yn[0]=='N' )
	{
		/* add by xxx at 20030726 **/
		ret = Com_parm_Sel( g_pub_tx.reply, &sCom_parm, "parm_code='ZJJY' and \
							parm_seqn=0");
		if( ret )
		{
			sprintf(acErrMsg, "��ѯ�������������!! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}

		date = atol(sCom_parm.val);
		sprintf(acErrMsg, "beg_date=[%ld], date=[%ld]", g_mdm_ac_rel.beg_date,
			date);
		WRITEMSG
		if (g_mdm_ac_rel.beg_date >= date)
		{
			ret = pub_cif_AcToIdNoType_jy(g_mdm_ac_rel.ac_id, 
				g_mdm_ac_rel.ac_seqn, id_type, id_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "��ѯ�ͻ������е�֤�����ͺ�֤���������");
				WRITEMSG
				goto ErrExit;
			}

			sprintf(acErrMsg, "-------> id_type=[%s] [%s]", 
				g_pub_tx.id_type, id_type);
			WRITEMSG
			if (strncmp(id_type, g_pub_tx.id_type, 1) != 0)
			{
				sprintf(acErrMsg, "֤�����Ͳ���!! [%s]", g_pub_tx.id_type);
				WRITEMSG
				strcpy(g_pub_tx.reply, "P444");
				goto ErrExit;
			}

			strcpy(g_mdm_ac_rel.id_type, g_pub_tx.id_type);

			sprintf(acErrMsg, "-------> id_no=[%s] [%s]", 
				g_pub_tx.id_no, id_no);
			WRITEMSG
			if (strcmp(id_no, g_pub_tx.id_no) != 0)
			{
				sprintf(acErrMsg, "֤�����벻��ȷ!! [%s]", g_pub_tx.id_no);
				WRITEMSG
				strcpy(g_pub_tx.reply, "P445");
				goto ErrExit;
			}
			strcpy(g_mdm_ac_rel.id_no, g_pub_tx.id_no);
		}

		get_zd_data("1027",g_pub_tx.draw_pwd); 			/* ֧ȡ���� */
		pub_base_EnDes(g_pub_tx.tx_date,"",g_pub_tx.draw_pwd);
		strcpy(g_mdm_ac_rel.draw_pwd,g_pub_tx.draw_pwd);
	}
	else
	{
		vtcp_log("[%s][%d]g_pub_tx.ac_no=[%s]\n",__FILE__,__LINE__,g_pub_tx.ac_no);
		vtcp_log("[%s][%d]������־option=[%s]\n",__FILE__,__LINE__,option);
		/***add by hxc 091021 ���п�����ά�����Ĳ�����ԭ����У��***/
		/*20110725 mod by yanqq ����ʱ������ԭ����У��*/
		/*if (memcmp(g_pub_tx.ac_no,CARDHEADCODE,6) != 0 && memcmp(g_pub_tx.ac_no,ICCARDHEADCODE,6) != 0) */
		if (memcmp(option,"1",1))
		{
			if(memcmp(g_pub_tx.ac_no,CARDHEADCODE,6) != 0 && memcmp(g_pub_tx.ac_no,ICCARDHEADCODE,6) != 0)
			{
				/* add by xxx at 20030726 ����У��**/
				get_zd_data("0790",g_pub_tx.draw_pwd); 			/* ԭ֧ȡ���� */
				pub_base_EnDes(g_pub_tx.tx_date,"",g_pub_tx.draw_pwd);
				pub_base_strpack(g_mdm_ac_rel.draw_pwd);
				pub_base_strpack(g_pub_tx.draw_pwd);
				if ( strcmp(g_mdm_ac_rel.draw_pwd, g_pub_tx.draw_pwd) != 0)
				{
					sprintf(acErrMsg, "�������!!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "O123");
					goto ErrExit;
				}
			}
		}
		/***end by hxc 091021 ������ά��������ԭ����У��***/
		
		get_zd_data("1027",g_pub_tx.draw_pwd); 			/* ��֧ȡ���� */
		pub_base_EnDes(g_pub_tx.tx_date,"",g_pub_tx.draw_pwd);
		/*vtcp_log("[%s][%d]g_pub_tx.draw_pwd=[%s]\n",__FILE__,__LINE__,g_pub_tx.draw_pwd);*/
		strcpy(g_mdm_ac_rel.draw_pwd,g_pub_tx.draw_pwd);
	}

	strcpy(g_mdm_ac_rel.draw_pwd_yn,g_pub_tx.draw_pwd_yn);
	ret = Mdm_ac_rel_Upd_Upd(g_mdm_ac_rel,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"�޸Ĵ���!");
		WRITEMSG
		goto ErrExit;
	}

	Mdm_ac_rel_Clo_Upd();

    /*** �Ǽǽ�����ˮ ***/
	if( pub_ins_trace_log() )
	{
	 	sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
	  	WRITEMSG
	    goto ErrExit;
	}
        /*mod by lwb 20140122 */	
	/*pub_mob_sendmail("���������޸ĳɹ�", g_pub_tx.tx_code,g_pub_tx.ac_no,g_pub_tx.add_ind,0.0, 0.0 );*/
	pub_mob_sendmail("���������޸ĳɹ�", g_pub_tx.tx_code,g_pub_tx.ac_no,"1",0.0, 0.0 );
	
	/****add 20120104�����޸ĺ�Ѵ����ļ�ɾ��*/
	sprintf( fname,"rm -f /backup/pass_error/%s*",g_pub_tx.ac_no);
        ret = system( fname );
        if( ret )
        {
        sprintf(acErrMsg,"rm file error [%s] [%d]",fname,ret);
        WRITEMSG
        goto ErrExit;
        }

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"֧ȡ��Ϣά���ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"֧ȡ��Ϣά��ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
/**********************************************************************
 * �� �� ����pub_cif_AcToIdNoType_jy();(�޸İ�)
 * �������ܣ�����֤�����ͺ�֤������ȡ�ÿͻ��ż��ͻ�����
 *			 
 * ����/ʱ�䣺rob/20051010
 * 
 * ������
 *     ���룺	
 *		ac_id		�˻�id
 *		ac_seqn	    �˻����
 *     ���: 	
 *		id_type		֤������		
 *		id_no		֤������
 *
 *          	��Ӧ��
 *   ����ֵ��
 *           ����0�ɹ���
 *           ���ط�0ʧ�ܣ�
 * �޸���ʷ��
 *          
********************************************************************/
int pub_cif_AcToIdNoType_jy(long ac_id, int ac_seqn, char *id_type, char *id_no)
{
	int ret_jy = 0;
	struct prdt_ac_id_c sPrdt_ac_id;
	struct cif_id_code_rel_c sCif_id_code_rel;

	memset(&sPrdt_ac_id, 0x0, sizeof(struct prdt_ac_id_c));
	memset(&sCif_id_code_rel, 0x0, sizeof(struct cif_id_code_rel_c));

	sprintf(acErrMsg, " CGH -------> ac_id=[%ld], ac_seqn=[%d]", 
		ac_id, ac_seqn);
	WRITEMSG
	if( ac_seqn!=9999 )
	{
		ret_jy = Prdt_ac_id_Sel(g_pub_tx.reply, &sPrdt_ac_id, "ac_id=%ld and \
				ac_seqn=%d", ac_id, ac_seqn);
	}else{
		ret_jy = Prdt_ac_id_Sel(g_pub_tx.reply, &sPrdt_ac_id, "ac_id=%ld", ac_id);
	}	
	
	if (ret_jy != 0 && ret_jy != 100)
	{
		sprintf(acErrMsg, "��ѯ��Ʒ�˺Ŷ��ձ����!! [%d]", ret_jy);
		WRITEMSG
		strcpy(g_pub_tx.reply, "W013");
		return ret_jy;
	}
	else if (ret_jy == 100)
	{
		sprintf(acErrMsg, "��Ʒ�˺Ŷ��ձ��в����ڸü�¼!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "W012");
		return ret_jy;
	}

	ret_jy = Cif_id_code_rel_Sel(g_pub_tx.reply, &sCif_id_code_rel, 
		"cif_no=%ld", sPrdt_ac_id.cif_no);
	if (ret_jy != 0 && ret_jy != 100)
	{
		sprintf(acErrMsg, "�ͻ�֤����ͻ��Ŷ��ձ����!! [%d]", ret_jy);
		WRITEMSG
		strcpy(g_pub_tx.reply, "C002");
		return ret_jy;
	}
	else if (ret_jy == 100)
	{
		sprintf(acErrMsg, "�ͻ�֤����ͻ��Ŷ��ձ�û�д˼�¼!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "B115");
		return ret_jy;
	}

	pub_base_strpack(sCif_id_code_rel.id_type);
	pub_base_strpack(sCif_id_code_rel.id_no);
	strcpy(id_type, sCif_id_code_rel.id_type);
	strcpy(id_no, sCif_id_code_rel.id_no);

	return 0;
}
