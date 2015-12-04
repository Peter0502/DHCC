/***************************************************************
* �� �� ��: spD999.c
* �����������������ʵ�
* ��    ��: 
* ������ڣ�2003��8��14��
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
#include "ln_mst_hst_c.h"
#include "mdm_attr_c.h"
#include "note_parm_c.h"

#define RECK_PAGE_LENG   50

spL999()
{
	int ret=0,i=0,count=0;
	int total_cnt=0,page_cnt=1,tmp_page_cnt=0;
	char ac_no[25],sub_ac_no[25];
	long date1,date2;
	int	 ac_seqn=0;
	int	 hst_cnt=0;					/***��ϸ�ʴ�***/
	FILE *fp;
	char tmpname[101];
	char add_ind[5];
	char cur_no[3];
	char term[5];
	char ac_type[2];
	char dc_flag[2];
	char ln_tx_type[9];
	double d_amt=0.00,c_amt=0.00;

	struct prdt_ac_id_c	g_prdt_ac_id;
	struct ln_mst_hst_c	g_ln_mst_hst;
	struct mdm_attr_c	g_mdm_attr;
	struct note_parm_c	g_note_parm;
	struct dic_data_c	g_dic_data;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_mdm_attr,0x00,sizeof(struct mdm_attr_c));
	memset(&g_note_parm,0x00,sizeof(struct note_parm_c));
	memset(&g_dic_data,0x00,sizeof(struct dic_data_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit( );

	get_zd_data("0670",ac_type);				/*��������*/
	get_zd_data("0210",cur_no);					/*����*/
	get_zd_data("0310",ac_no);					/*ȡ�ʺ�*/
	get_zd_long("0440",&date1);					/*ȡ��ʼ����*/
	get_zd_long("0450",&date2);					/*ȡ��ֹ����*/

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�����˻����ձ��в����ڸü�¼");
		WRITEMSG
		strcpy(g_pub_tx.reply,"W010");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	ret = Mdm_attr_Sel( g_pub_tx.reply,&g_mdm_attr,"mdm_code='%s'", \
						g_mdm_ac_rel.mdm_code);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�����ڸý������Ͷ���ret==[%d]",ret);
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

	g_reply_int = Prdt_ac_id_Sel( g_pub_tx.reply, &g_prdt_ac_id, " ac_id=%ld ", 
							  	  g_mdm_ac_rel.ac_id );
	if( g_reply_int == 100 )
	{
		sprintf( acErrMsg, "���˺Ų�����!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "���Ҵ���[%d]",g_reply_int );
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if( g_prdt_ac_id.prdt_no[0]!='3' )
	{
		sprintf( acErrMsg, "�Ǵ����˺�" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L114");
		goto ErrExit;
	}

	g_reply_int = Ln_mst_Sel( g_pub_tx.reply, &g_ln_mst, " ac_id=%ld ", 
							  g_mdm_ac_rel.ac_id );
	if( g_reply_int == 100 )
	{
		sprintf( acErrMsg, "�����˺Ų�����!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "���Ҵ���[%d]",g_reply_int );
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	pub_base_strpack( g_mdm_attr.mdm_name );

	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"w");
	if( fp==NULL )
	{
		sprintf(acErrMsg,"���ļ�[%s]��",tmpname);
		WRITEMSG
		strcpy( g_pub_tx.reply,"P157" );
		goto ErrExit;
	}

	/*** ������ͬ��ӡ���ʻص� ***/
	if( date1==date2 ) 
	{
		if( g_pub_tx.tx_date==date1 ) 
			g_ln_mst_hst.bal = g_ln_mst.bal;
		else
		{
			ret = Ln_mst_hst_Sel( g_pub_tx.reply,&g_ln_mst_hst, \
			  					"ac_id=%ld and ac_seqn=0 and tx_date<=%ld \
								 order by tx_date desc,tx_time desc", \
								 g_mdm_ac_rel.ac_id, date1 );
			if( ret==100 )
			{
				sprintf(acErrMsg,"δ�ҵ����������Ĵ����˻���Ϣ!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"L224");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"D103");
				goto ErrExit;
			}
		}

		fprintf(fp,"DZHD%s|%s|%.2lf|%ld|\n",ac_no,g_ln_mst.name, \
					g_ln_mst_hst.bal,date1);
	}
	else
	{
		total_cnt=sql_count("ln_mst_hst", "ac_id=%ld and ac_seqn=0 and \
							tx_date>=%ld and tx_date<=%ld", \
							g_mdm_ac_rel.ac_id,date1,date2);
		if( total_cnt<=0 )
		{
			sprintf(acErrMsg,"transfer sql_count function error!!");
			WRITEMSG
			goto ErrExit;
		}

		if( ac_type[0]=='*' )
		{
			ret=Ln_mst_hst_Dec_Sel( g_pub_tx.reply, \
									"ac_id=%ld and ac_seqn=0 and tx_date>=%ld \
									and tx_date<=%ld order by hst_cnt", \
									g_mdm_ac_rel.ac_id,date1,date2 );
		}
		else 
		{
			ret=Ln_mst_hst_Dec_Sel( g_pub_tx.reply, \
									"ac_id=%ld and ac_seqn=0 and tx_date>=%ld \
									and tx_date<=%ld and ln_tx_type='%s' order \
									by hst_cnt", g_mdm_ac_rel.ac_id,date1, \
									date2,ac_type );
		}
		if( ret )
		{
			sprintf(acErrMsg,"Prepare Open Ln_mst_hst_Dec_Sel error!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D101");
			goto ErrExit;
		}

		while(1)
		{
			ret=Ln_mst_hst_Fet_Sel(&g_ln_mst_hst,g_pub_tx.reply);
			if( ret==100 )
			{
				if(hst_cnt==0)
				{
					sprintf(acErrMsg,"���ʻ�û��Ҫ��ӡ�ļ�¼!!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"O218");
					goto ErrExit;
				}
				else break;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"ȡ������ϸ�ʳ���!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"W050");
				goto ErrExit;
			}
			
			/*�跽������*/
			if(g_ln_mst_hst.ln_tx_type[0]=='0' && g_ln_mst_hst.add_ind[0]=='1' )
			{
				d_amt=g_ln_mst_hst.tx_amt;
				c_amt=0.00;
				strcpy(dc_flag,"0");
			}else{
				/*����������*/
				c_amt=g_ln_mst_hst.tx_amt;
				d_amt=0.00;
				strcpy(dc_flag,"1");
			}				
			
			while(1)
			{
				if(i==RECK_PAGE_LENG)
				{
					page_cnt++;
					i=0;
				}
				else
					i++;
				break;
			}				

			if( hst_cnt>0 && g_ln_parm.cif_type[0]=='1' )
			{
				strcpy(g_mdm_ac_rel.note_no,"----------------");
				strcpy(g_mdm_attr.mdm_name,"------------");
			}
			else if(g_ln_parm.cif_type[0]!='1')
			{
				strcpy(g_mdm_ac_rel.note_no,g_ln_mst_hst.note_no);
				if( strlen(g_ln_mst_hst.note_type) )
				{
					ret = Note_parm_Sel(g_pub_tx.reply,&g_note_parm, \
					 	 "note_type='%s'",g_ln_mst_hst.note_type);
					if( ret==100 )
					{
						sprintf(acErrMsg,"ȡƾ֤���಻����!!note_type=[%s]",
								g_ln_mst_hst.note_type);
						WRITEMSG
						strcpy(g_mdm_attr.mdm_name, g_ln_mst_hst.note_type);
					}
					else if( ret )
					{
						sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
						WRITEMSG
						strcpy(g_pub_tx.reply,"D103");
						goto ErrExit;
					}
					else 
						strcpy(g_mdm_attr.mdm_name,g_note_parm.name);
				}
				else 
					strcpy(g_mdm_attr.mdm_name,g_note_parm.name);
			}
			if(g_ln_mst_hst.ln_tx_type [0]== '8' )
			{
				   continue;
			}

			pub_base_strpack(g_ln_mst.name);
			pub_base_dic_show_str(add_ind,"deta_type",g_ln_mst_hst.add_ind);
			pub_base_dic_show_str(ln_tx_type,"ln_tx_type_hst",g_ln_mst_hst.ln_tx_type);

			fprintf(fp,"DDZD%s|%s|%s|%.2lf|%ld|%ld|%ld|%s|%s|%.2lf|%s|%s|%d|%s|%s|%s|%s|%.2lf|%.2lf|%s|\n",
			ac_no,g_ln_mst_hst.tx_br_no,add_ind,g_ln_mst_hst.tx_amt,g_ln_mst_hst.tx_date,
			g_ln_mst_hst.tx_time,g_ln_mst_hst.hst_cnt,g_ln_mst_hst.brf,g_ln_mst_hst.tel,g_ln_mst_hst.bal,
			g_ln_mst_hst.add_ind,g_ln_mst.name,page_cnt,g_mdm_ac_rel.note_no,g_mdm_attr.mdm_name,
			g_ln_parm.cif_type,ln_tx_type,d_amt,c_amt,dc_flag);
			hst_cnt++;
		}

		Ln_mst_hst_Clo_Sel();
	}

	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");

    /*** �Ǽǽ�����ˮ ***/
	if( pub_ins_trace_log() )
	{
	 	sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
	  	WRITEMSG
	    goto ErrExit;
	}


OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��ӡ���ʵ��ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"��ӡ���ʵ�ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
