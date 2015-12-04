/*************************************************************
* �� �� ��:     spD699.c
* ����������    �
* ��    ��:     xxxxxxx
* ������ڣ�    2003��3��15��
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
#include "prdt_ac_id_c.h"

spD699()
{
	int ret=0;
	int num=0;
	char ac_no[20],ac_type[2];
	char name[50],cur_no[2];
	double amt;
	FILE *fp;
	char filename[50];
	char tmpname[101];

	struct prdt_ac_id_c 		g_prdt_ac_id;
	struct dd_mst_c				g_dd_mst;
	struct td_mst_c				g_td_mst;
	struct mdm_ac_rel_c			g_mdm_ac_rel;
	struct dd_parm_c			g_dd_parm;
	struct td_parm_c			g_td_parm;

	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));

	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ȡֵ����ֵ **/
	get_zd_data("0300",g_pub_tx.ac_no);

	/** �������� **/
	/* �����˺�ȡ���˻�ID���ʻ���� */
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,\
	                      "ac_no='%s'",g_pub_tx.ac_no);
	if(ret)
	{
		sprintf(acErrMsg,"��ѯ�������˻���ϵ���[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	g_pub_tx.ac_seqn=g_mdm_ac_rel.ac_seqn;

TRACE		
	if(g_pub_tx.ac_seqn!=9999)
	{
TRACE		
		ret=Prdt_ac_id_Sel(g_pub_tx.reply,&g_prdt_ac_id, \
							"ac_id=%ld and  ac_seqn=%d", \
							g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
		if(ret)
		{
			sprintf(acErrMsg,"��ѯ��Ʒ�˺Ŷ��ձ�ʧ��![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}

TRACE		
		switch(g_prdt_ac_id.ac_id_type[0])
		{
TRACE		
			 case '1':                                   /* ���� */
					ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,\
						"ac_id=%ld and ac_seqn=%d",\
						 g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
TRACE		
					if(ret)
					{	
						sprintf(acErrMsg,"��ѯ���ڴ�����ļ�ʧ��![%d]",ret);
						WRITEMSG
						goto ErrExit;
					}
TRACE		
					strcpy(name,g_dd_mst.name);
					amt=g_dd_mst.bal;
					/*���ݲ�Ʒ��Ų��ұ���*/
					ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,\
									"prdt_no='%s'",g_dd_mst.prdt_no);
					if(ret)
					{
						sprintf(acErrMsg,"��ѯ���ڲ�Ʒ������ʧ��![%d]",ret);
						WRITEMSG
						goto ErrExit;
					}
TRACE		
					strcpy(cur_no,g_dd_parm.cur_no);
					break;
			case '2':
					ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,\
								"ac_id=%ld and ac_seqn=%d",\
								g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
					if(ret)
					{
						sprintf(acErrMsg,"��ѯ�������ļ�ʧ��![%d]",ret);
						WRITEMSG
						goto ErrExit;
					}
					strcpy(name,g_td_mst.name);
					amt=g_td_mst.bal;
				
					ret=Td_parm_Sel(g_pub_tx.reply,&g_td_parm,\
									"prdt_no='%s'",g_td_mst.prdt_no);
					if(ret)
					{
						sprintf(acErrMsg,"��ѯ���ڲ�Ʒ���������![%d]");
						WRITEMSG
						goto ErrExit;
					}
					strcpy(cur_no,g_td_parm.cur_no);
					break;
			default:
					break;
			}
TRACE		
			/*��ӡ��ʾ����*/
			pub_base_GenDwnFilName(filename);
TRACE		
			pub_base_strpack(filename);
			sprintf(tmpname,"%s/%s",getenv("FILDIR"),filename);
TRACE		
			fp=fopen(tmpname,"w");

TRACE		
			fprintf(fp,"~@�˺�|�˺����|@�ͻ���|@����|$���|\n");
			fprintf(fp,"%s|%d|%s|%s|2lf|\n",g_pub_tx.ac_no,g_pub_tx.ac_seqn,\
						name,cur_no,amt);
			fclose(fp);
TRACE		
			set_zd_data(DC_FILE_SND,"1");
		}
		else
		{
			ret=Prdt_ac_id_Dec_Sel(g_pub_tx.reply,"ac_id=%ld",\
									g_mdm_ac_rel.ac_id);
			if(ret)
			{
				sprintf(acErrMsg,"ִ��Prdt_ac_id_Dec_Sel��![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			num=0;
			pub_base_GenDwnFilName(filename);
			pub_base_strpack(filename);
			sprintf(tmpname,"%s/%s",getenv("FILDIR"),filename);
			fp=fopen(tmpname,"w");


			fprintf(fp,"~@�˺�|�˺����|@�ͻ���|@����|$���|\n");
			while(1)
			{
				ret=Prdt_ac_id_Fet_Sel(&g_prdt_ac_id,g_pub_tx.reply);
				if(ret==100&& num==0) 
				{
					sprintf(acErrMsg,"��ѯ��Ʒ�˺Ŷ��ձ��޼�¼![%d]",ret);
					WRITEMSG
					goto ErrExit;
				}

				else if ( ret == 100 && num > 0 ) break;

				else if(ret)
				{
					sprintf(acErrMsg,"ִ��Prdt_ac_id_Fet_Sel��![%d]",ret);
					WRITEMSG
					goto ErrExit;
				}

				g_pub_tx.ac_seqn=g_prdt_ac_id.ac_seqn;


		
		switch(g_prdt_ac_id.ac_id_type[0])
		{
			case '1':									/* ���� */
			ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst, \
							"ac_id=%ld and ac_seqn=%d",\
							g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
			if(ret)
			{
				sprintf(acErrMsg,"��ѯ���ڴ�����ļ�ʧ��![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
TRACE
		sprintf(acErrMsg,"OKOKOKOOKOO");
		WRITEMSG
			strcpy(name,g_dd_mst.name);
			amt=g_dd_mst.bal;
			/*���ݲ�Ʒ��Ų��ұ���*/
			ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, \
							"prdt_no='%s'",g_dd_mst.prdt_no);
			if(ret)
			{
				sprintf(acErrMsg,"��ѯ���ڲ�Ʒ������ʧ��![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			strcpy(cur_no,g_dd_parm.cur_no);
TRACE
		sprintf(acErrMsg,"OKOKOKOOKOO");
		WRITEMSG
			break;
			
			case '2':								/* ���� */
			ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst, \
							"ac_id=%ld and ac_seqn=%d",\
							g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
			if(ret)
			{
				sprintf(acErrMsg,"��ѯ�������ļ�ʧ��![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			strcpy(name,g_td_mst.name);
			amt=g_td_mst.bal;

			ret=Td_parm_Sel(g_pub_tx.reply,&g_td_parm, \
							"prdt_no='%s'",g_td_mst.prdt_no);
			if(ret)
			{
				sprintf(acErrMsg,"��ѯ���ڲ�Ʒ������ʧ��![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			strcpy(cur_no,g_td_parm.cur_no);
			break;
		default:
			break;
		}

TRACE

  	/*��ӡ��ʾ����*/

	fprintf(fp,"%s|%d|%s|%s|2lf|\n",g_pub_tx.ac_no,g_pub_tx.ac_seqn,name,amt);
TRACE
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
	}
	num++;
	/*while ����*/

	Mdm_ac_rel_Clo_Sel();
	}
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��ѯ��ӡ�ɹ�![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"��ѯ��ӡʧ��![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;

}
