/***************************************************************
* �� �� ��:     spD307.c
* ����������	����˵���ӡ����
* ��    ��:     jack
* ������ڣ�    2004��1��13��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_prot_ac_rel_c.h"

spD307()
{
	char ac_type[2];
	FILE *fp;
	char filename[50],tmpfile[100];
	char ac_id_type[2];
	long tmp_date;
	double tmp_bal;
	char tmp_prdt[5];
	int	tmp_seqn;
	struct mo_prot_ac_rel_c g_mo_prot_ac_rel;

	g_reply_int=0;

	memset(&g_mo_prot_ac_rel,0x00,sizeof(struct mo_prot_ac_rel_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data("0310",g_pub_tx.ac_no);			/*ȡ�ʺ�*/
	get_zd_int("0340",&g_pub_tx.ac_seqn);		/*ȡ�ʻ����*/

	g_reply_int=pub_base_disid_cltid(g_pub_tx.ac_no,&g_pub_tx.ac_id);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"������ʾ�ʺź����ȡ�˻�IDʧ��!");
		WRITEMSG
		strcpy (g_pub_tx.reply,"W055");
		goto ErrExit;
	}

	g_reply_int = Mo_prot_ac_rel_Dec_Sel(g_pub_tx.reply,
		"main_ac_id=%ld and main_ac_seqn=%d and sts='1'",
		g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	if (g_reply_int)
	{
		sprintf(acErrMsg,"DECLARE Mo_prot_ac_Dec_Sel ERROR!!");
		WRITEMSG
		strcpy (g_pub_tx.reply,"W055");
		goto ErrExit;
	}

	pub_base_GenDwnFilName(filename);
	pub_base_strpack(filename);

	sprintf(tmpfile,"%s/%s",getenv("FILDIR"),filename);
	fp=fopen(tmpfile,"w");

	if (fp==NULL)
	{
		sprintf(acErrMsg,"���ɴ�����ʱ���ļ�����![%s]",tmpfile);
		WRITEMSG
		strcpy( g_pub_tx.reply, "P157");
		goto ErrExit;
	}

	fprintf(fp,"�˺�|�˻����|��Ʒ����|��������|���|\n");

	while(1)
	{
		g_reply_int = Mo_prot_ac_rel_Fet_Sel(&g_mo_prot_ac_rel,g_pub_tx.reply);
		if (g_reply_int==100)break;
		if (g_reply_int )
		{
			sprintf(acErrMsg,"FETCH Mo_prot_ac_Dec_Sel ERROR!!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"W055");
			goto ErrExit;
		}

		/*��ѯ��ʾ�˺�*/
		g_reply_int = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,
			"ac_id=%ld and ac_seqn=%d",g_mo_prot_ac_rel.app_ac_id,
			g_mo_prot_ac_rel.app_ac_seqn);	
		if (g_reply_int && g_reply_int!=100)
		{
			sprintf(acErrMsg,"��ѯ�����ʺŶ��˱��쳣!![%d]",g_reply_int);
			WRITEMSG
			strcpy (g_pub_tx.reply,"W055");
			goto ErrExit;
		}
		else if (g_reply_int==100)
		{
			tmp_seqn=ALL_SEQN;
			g_reply_int = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,
			"ac_id=%ld and ac_seqn=%d",g_mo_prot_ac_rel.app_ac_id,tmp_seqn);	
			if (g_reply_int && g_reply_int!=100)
			{
				sprintf(acErrMsg,"��ѯ�����ʺŶ��˱��쳣!![%d]",g_reply_int);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W055");
				goto ErrExit;
			}
			else if( g_reply_int==100 )
			{
			sprintf(acErrMsg,"�����ʺŶ��ձ����ڸñʼ�¼!![%d]",g_reply_int);
			WRITEMSG
			vtcp_log("[%d],[%d]",g_mo_prot_ac_rel.app_ac_id,tmp_seqn);
			strcpy (g_pub_tx.reply,"W055");
			goto ErrExit;
			}
		}

		/*�������ļ�,���˻���������*/
		g_reply_int = pub_base_idseqn_file(g_mo_prot_ac_rel.app_ac_id,g_mo_prot_ac_rel.app_ac_seqn,ac_id_type);
		if (g_reply_int)
		{
			sprintf(acErrMsg,"���˻�ID���˻����ȡ���ļ������쳣!![%d]",
					g_reply_int);
			WRITEMSG
			strcpy (g_pub_tx.reply,"W055");
			goto ErrExit;
		}

		if (ac_id_type[0]=='1')
		{
			tmp_bal=g_dd_mst.bal;
			tmp_date=g_dd_mst.opn_date;
			strcpy(tmp_prdt,g_dd_mst.prdt_no);
		}
		else if (ac_id_type[0]=='2')
		{
			tmp_bal=g_td_mst.bal;
			tmp_date=g_td_mst.opn_date;
			strcpy(tmp_prdt,g_td_mst.prdt_no);
		}

		fprintf(fp,"%s|%d|%s|%ld|%lf|\n",g_mdm_ac_rel.ac_no,
				g_mo_prot_ac_rel.app_ac_seqn,tmp_prdt,tmp_date,tmp_bal);
		
	}

	fclose(fp);

	set_zd_data(DC_FILE_SND,"1");

	Mo_prot_ac_rel_Clo_Sel ();

    /*** �Ǽǽ�����ˮ ***/
	if( pub_ins_trace_log() )
	{
	 	sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
	  	WRITEMSG
	    goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��ƴ�ӡ�ɹ�!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"��ƴ�ӡʧ��!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
