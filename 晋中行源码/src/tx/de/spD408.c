/***************************************************************
* �� �� ��: spD408.c
* ������������ӡ���ʵ�����
* ��    ��: mike
* ������ڣ�2004��1��14��
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
#include "dd_mst_hst_c.h"
#include "td_mst_hst_c.h"
#include "ln_mst_hst_c.h"
#include "od_mst_hst_c.h"
#include "in_mst_hst_c.h"
#include "mdm_attr_c.h"
#include "note_parm_c.h"
#include "sub_dd_mst_c.h"
#include "sub_dd_mst_hst_c.h"

#define RECK_PAGE_LENG   50

spD408()
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
	char br_name[51];
	int first=0;
	long tmpdate;
	double tpbal=0.00;

	struct prdt_ac_id_c	g_prdt_ac_id;
	struct dd_mst_hst_c	g_dd_mst_hst;
	struct dd_mst_hst_c	o_dd_mst_hst;
	struct td_mst_hst_c	g_td_mst_hst;
	struct ln_mst_hst_c	g_ln_mst_hst;
	struct od_mst_hst_c	g_od_mst_hst;
	struct in_mst_hst_c	g_in_mst_hst;
	struct mdm_attr_c	g_mdm_attr;
	struct note_parm_c	g_note_parm;
	struct dic_data_c	g_dic_data;
	struct sub_dd_mst_c	s_sub_dd_mst;
	struct sub_dd_mst_hst_c	s_sub_dd_mst_hst;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_od_mst,0x00,sizeof(struct od_mst_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_mdm_attr,0x00,sizeof(struct mdm_attr_c));
	memset(&g_note_parm,0x00,sizeof(struct note_parm_c));
	memset(&g_dic_data,0x00,sizeof(struct dic_data_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data("0670",ac_type);				/*�˻�����*/
	get_zd_data("0210",cur_no);					/*����*/
	get_zd_data("0310",ac_no);					/*ȡ�ʺ�*/
	get_zd_data("0320",sub_ac_no);				/*�������ʺ�*/
	get_zd_int("0340",&ac_seqn);				/*ȡ�ʺ����*/
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

	pub_base_strpack(g_mdm_attr.mdm_name);

	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"w");
	if( fp==NULL )
	{
		sprintf(acErrMsg,"���ļ�[%s]��",tmpname);
		WRITEMSG
		strcpy( g_pub_tx.reply,"P157" );
		goto ErrExit;
	}

	if( ac_type[0]=='1' )							/*** �����ʻ� ***/
	{
		if( g_mdm_attr.ac_num_ind[0]=='N' )
		{
			/*** ��ӡ���»����ʻ����ʵ� ***/
			ac_seqn = atoi(cur_no);					/*** ȡ�˻���� ***/

			/*** �ǿ����˻���ʾ ***/
			if( g_mdm_attr.mdm_type[0]!='3' )
			{
				sprintf(acErrMsg,"����������ʺ�!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"O232");
				goto ErrExit;
			}
		}
		else if( g_mdm_ac_rel.ac_seqn )
		{
			/*** ��ӡ��ͨ�����ʻ����ʵ� ***/
			ac_seqn = g_mdm_ac_rel.ac_seqn;		/*** ȡ�˻���� ***/
		}
		else
		{
			sprintf(acErrMsg,"����������ʺ�!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O232");
			goto ErrExit;
		}
	
		if( date1==date2 ) 
		goto TAG;

		ret = Dd_mst_Sel( g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d", \
						  g_mdm_ac_rel.ac_id,ac_seqn);
		if( ret==100 )
		{
			sprintf(acErrMsg,"δ�ҵ��˻����˻���Ϣ!!");
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

		ret = Dd_parm_Sel( g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'", \
					  		g_dd_mst.prdt_no);
		if( ret==100 )
		{
			sprintf(acErrMsg,"���ڲ������޴˼�¼!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D101");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}

		/**total_cnt=sql_count("dd_mst_hst", "ac_id=%ld and ac_seqn=%d and \
							tx_date>=%ld and tx_date<=%ld", \
							g_mdm_ac_rel.ac_id,ac_seqn,date1,date2);ȥ�� 2611*/
		total_cnt=sql_count("dd_mst_hst", "ac_id=%ld and ac_seqn=%d and \
							tx_date>=%ld and tx_date<=%ld and tx_code<>'2611'", \
							g_mdm_ac_rel.ac_id,ac_seqn,date1,date2);
		if( total_cnt<0 )
		{
			sprintf(acErrMsg,"transfer sql_count function error!!");
			WRITEMSG
			goto ErrExit;
		}

vtcp_log("DDDD:ac_id=%ld and ac_seqn=%d and tx_date>=%ld and tx_date<=%ld order by hst_cnt", g_mdm_ac_rel.ac_id,ac_seqn,date1,date2);
	first=0;
		/*ret=Dd_mst_hst_Dec_Sel(g_pub_tx.reply, \
		"ac_id=%ld and ac_seqn=%d and tx_date>=%ld and tx_date<=%ld order by tx_date,hst_cnt,trace_no,tx_time", g_mdm_ac_rel.ac_id,ac_seqn,date1,date2);
		tx_code='2611' ���� ����ӡ */
		ret=Dd_mst_hst_Dec_Sel(g_pub_tx.reply, \
		"ac_id=%ld and ac_seqn=%d and tx_date>=%ld and tx_date<=%ld and tx_code<>'2611' order by tx_date,hst_cnt,trace_no,tx_time", g_mdm_ac_rel.ac_id,ac_seqn,date1,date2);
		if( ret )
		{
			sprintf(acErrMsg,"Prepare Open Dd_mst_hst_Dec_Sel error!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D101");
			goto ErrExit;
		}

		while(1)
		{
			ret=Dd_mst_hst_Fet_Sel(&g_dd_mst_hst,g_pub_tx.reply);
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

			if( first==0 )
			{
				if( date1%100==1 && g_dd_mst.opn_date<date1 )
				{
					tpbal=0.00;
					tmpdate=date1;
					if( g_dd_mst_hst.add_ind[0]=='0' )
						tpbal=g_dd_mst_hst.bal+g_dd_mst_hst.tx_amt;
					if( g_dd_mst_hst.add_ind[0]=='1' )
						tpbal=g_dd_mst_hst.bal-g_dd_mst_hst.tx_amt;

					pub_base_getbrname(g_dd_mst_hst.opn_br_no,br_name);
		fprintf(fp,"DZD%s|%s|%s|%.2lf|%ld|%ld|%ld|%s|%s|%.2lf|%s|%s|%d|%s|%s|%s|%s| |\n",
					ac_no,g_dd_mst_hst.tx_br_no,"���½�ת",0.00, tmpdate,0,
					g_dd_mst_hst.hst_cnt,"","",
					tpbal,"1",g_dd_mst.name,
					page_cnt,"","", g_dd_parm.cif_type,br_name);
					hst_cnt++;
				}
				first=1;
				memcpy( &o_dd_mst_hst,&g_dd_mst_hst,sizeof(g_dd_mst_hst));
			}
			else if( o_dd_mst_hst.tx_date/100!=g_dd_mst_hst.tx_date/100 )
			{
				if( i>RECK_PAGE_LENG )
				{
					page_cnt++;
					i = 1;
				}
				else i++;

				pub_base_getbrname(g_dd_mst_hst.opn_br_no,br_name);
				tmpdate=g_dd_mst_hst.tx_date/100*100+1;
		fprintf(fp,"DZD%s|%s|%s|%.2lf|%ld|%ld|%ld|%s|%s|%.2lf|%s|%s|%d|%s|%s|%s|%s| |\n",
				ac_no,g_dd_mst_hst.tx_br_no,"���½�ת",0.00, tmpdate,0,
				g_dd_mst_hst.hst_cnt,"","",
				o_dd_mst_hst.bal,"1",g_dd_mst.name, page_cnt,"","",
				g_dd_parm.cif_type,br_name);
				hst_cnt++;
			}

			if( i>RECK_PAGE_LENG )
			{
				page_cnt++;
				i = 1;
			}
			else i++;
			/*20080123*/
			memset(&g_note_parm,0x00,sizeof(struct note_parm_c));
			/*20080123*/
			if(hst_cnt>0&&g_dd_parm.cif_type[0]=='1')
			{
				strcpy(g_mdm_ac_rel.note_no,"----------------");
				strcpy(g_mdm_attr.mdm_name,"------------");
			}
			else if(g_dd_parm.cif_type[0]!='1')
			{
				strcpy(g_mdm_ac_rel.note_no,g_dd_mst_hst.note_no);
				if( strlen(g_dd_mst_hst.note_type) )
				{
					ret = Note_parm_Sel(g_pub_tx.reply,&g_note_parm, \
					 	 "note_type='%s'",g_dd_mst_hst.note_type);
					if( ret==100 )
					{
						sprintf(acErrMsg,"ȡƾ֤���಻����!!note_type=[%s]",
								g_dd_mst_hst.note_type);
						WRITEMSG
						strcpy(g_mdm_attr.mdm_name, g_dd_mst_hst.note_type);
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

			pub_base_strpack(g_mdm_ac_rel.note_no);
			if( strlen(g_mdm_ac_rel.note_no)>12 )
			{
				strcpy( g_dd_mst_hst.note_no,g_mdm_ac_rel.note_no+strlen(g_mdm_ac_rel.note_no)-12 );
				strcpy( g_mdm_ac_rel.note_no,g_dd_mst_hst.note_no );
			}

			pub_base_strpack(g_dd_mst.name);
			pub_base_dic_show_str(add_ind,"deta_type",g_dd_mst_hst.add_ind);
			pub_base_getbrname(g_dd_mst_hst.opn_br_no,br_name);

			fprintf(fp,"DZD%s|%s|%s|%.2lf|%ld|%ld|%ld|%s|%s|%.2lf|%s|%s|%d|%s|%s|%s|%s|%s|\n",
			ac_no,g_dd_mst_hst.tx_br_no,add_ind,g_dd_mst_hst.tx_amt,
			g_dd_mst_hst.tx_date,g_dd_mst_hst.tx_time,
			g_dd_mst_hst.hst_cnt,g_dd_mst_hst.brf,g_dd_mst_hst.tel,
			g_dd_mst_hst.bal,g_dd_mst_hst.add_ind,g_dd_mst.name,
			page_cnt,g_mdm_ac_rel.note_no,g_mdm_attr.mdm_name,
			g_dd_parm.cif_type,br_name,g_dd_mst_hst.tx_code);
			memcpy( &o_dd_mst_hst,&g_dd_mst_hst,sizeof(g_dd_mst_hst));
			hst_cnt++;
		}

		Dd_mst_hst_Clo_Sel();
	}
	else if(ac_type[0]=='2')					/*** �����ʻ� ***/
	{
		if( g_mdm_attr.ac_num_ind[0]=='N' )
		{
			/*** ��ӡ���˻������µĶ��ڻ����ʵ� ***/
			if( !strncmp(g_mdm_ac_rel.mdm_code,"0016",4) ||
				!strncmp(g_mdm_ac_rel.mdm_code,"0020",4) )
			{
				ret=Td_mst_hst_Dec_Sel(g_pub_tx.reply, \
					 "ac_id=%ld and tx_date>=%ld and tx_date<=%ld \
					 order by ac_seqn,tx_date,tx_time", \
					 g_mdm_ac_rel.ac_id,date1,date2);
				if( ret )
				{
					sprintf(acErrMsg,"Prepare Open Td_mst_hst_Dec_Sel error!!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"D101");
					goto ErrExit;
				}

				while(1)
				{
					ret=Td_mst_hst_Fet_Sel(&g_td_mst_hst,g_pub_tx.reply);
					if( ret==100 )
					{
						if(hst_cnt==0)
						{
							sprintf(acErrMsg,"���ʻ�û��Ҫ��ӡ�ļ�¼!! \
								ac_id=%ld and tx_date>=%ld and tx_date<=%ld \
								order by ac_seqn,tx_date",
								g_mdm_ac_rel.ac_id,date1,date2);
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

					ret = Td_mst_Sel( g_pub_tx.reply,&g_td_mst, \
									 "ac_id=%ld and ac_seqn=%d", \
									 g_td_mst_hst.ac_id,g_td_mst_hst.ac_seqn);
					if( ret==100 )
					{
						sprintf(acErrMsg,"δ�ҵ��˶����˻���Ϣ!!");
						WRITEMSG
						strcpy(g_pub_tx.reply,"D101");
						goto ErrExit;
					}
					else if( ret )
					{
						sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
						WRITEMSG
						strcpy(g_pub_tx.reply,"D103");
						goto ErrExit;
					}

					ret = Td_parm_Sel( g_pub_tx.reply,&g_td_parm,"prdt_no='%s'",
										g_td_mst.prdt_no);
					if( ret )
					{
						sprintf(acErrMsg,"δ�ҵ��˶����˻���Ϣ!!");
						WRITEMSG
						strcpy(g_pub_tx.reply,"D101");
						goto ErrExit;
					}
					else if( ret )
					{
						sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
						WRITEMSG
						strcpy(g_pub_tx.reply,"D103");
						goto ErrExit;
					}

					if( g_td_parm.term_type[0]=='Y' ) 
					g_td_parm.term = g_td_parm.term*12 ;

					sprintf(term,"%02d",g_td_parm.term);
					strcat(term,"��");

			    	/*** �ļ�˳��
						 ��������|�ʺ�|�˻����|����|��������|����|��Ϣ��| \
						 ������|֧ȡ��|���|����|��Ϣ|��Ա|ҳ��|������־| \
						 ���ʴ���|��������
					***/
					fprintf(fp,"YBT%ld|%s|%d|%s|%s|%s|%ld|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%d|%s|%s|%s| |\n",
					g_td_mst_hst.tx_date,ac_no,g_td_mst.ac_seqn,g_td_mst.name,
					g_td_mst_hst.brf,term,g_td_mst.ic_date,
					g_td_mst.mtr_date,0,g_td_mst_hst.tx_amt,g_td_mst.rate,
					g_td_mst_hst.intst,g_td_mst_hst.tel,1,g_td_mst_hst.add_ind,
					g_mdm_ac_rel.mdm_code,g_td_mst_hst.opn_br_no);

					hst_cnt++;
				}

				Td_mst_hst_Clo_Sel( );
	
				set_zd_data("0670","1");
			}
			else
			{
				sprintf(acErrMsg,"�����붨���˻�!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"O233");
				goto ErrExit;
			}
		}
		else if( !g_mdm_ac_rel.ac_seqn )
		{
			/*** ��ӡ��ͨ���ڻ����ʵ� ***/
			ac_seqn = 0;

			if( date1==date2 ) 
			goto TAG;

			ret = Td_mst_Sel(g_pub_tx.reply,&g_td_mst, \
							"ac_id=%ld and ac_seqn=%d", \
							g_mdm_ac_rel.ac_id,ac_seqn);
			if( ret==100 )
			{
				sprintf(acErrMsg,"δ�ҵ��˶����˻���Ϣ error!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D101");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"D103");
				goto ErrExit;
			}

			total_cnt = sql_count("td_mst_hst","ac_id=%ld and ac_seqn=%d",
									g_mdm_ac_rel.ac_id,ac_seqn);
			if( total_cnt<0 )
			{
				sprintf(acErrMsg,"transfer sql_count function error!!");
				WRITEMSG
				goto ErrExit;
			}

			page_cnt = total_cnt/RECK_PAGE_LENG;
			page_cnt++ ;

			ret = Td_mst_hst_Dec_Sel(g_pub_tx.reply,
			"ac_id=%ld and ac_seqn=%d and tx_date>=%ld and tx_date<=%ld order by hst_cnt",g_mdm_ac_rel.ac_id,ac_seqn,date1,date2);
			if( ret )
			{
				sprintf(acErrMsg,"Prepare Td_mst_hst_Dec_Sel error!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D101");
				goto ErrExit;
			}

			while(1)
			{
				ret=Td_mst_hst_Fet_Sel(&g_td_mst_hst,g_pub_tx.reply);
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
					strcpy(g_pub_tx.reply,"W051");
					goto ErrExit;
				}

				if(hst_cnt>0)
				{
					strcpy(g_mdm_ac_rel.note_no,"----------------");
					strcpy(g_mdm_attr.mdm_name,"------------");
				}

				pub_base_dic_show_str( add_ind,"deta_type", \
									   g_td_mst_hst.add_ind);
				pub_base_strpack(g_td_mst_hst.brf);
			pub_base_strpack(g_mdm_ac_rel.note_no);
			if( strlen(g_mdm_ac_rel.note_no)>12 )
			{
				strcpy( g_dd_mst_hst.note_no,g_mdm_ac_rel.note_no+strlen(g_mdm_ac_rel.note_no)-12 );
				strcpy( g_mdm_ac_rel.note_no,g_dd_mst_hst.note_no );
			}

				fprintf(fp,"DZD%s|%s|%s|%.2lf|%ld|%ld|%ld|%s|%s|%.2lf|%s|%s|%d|%s|%s|%s|%s|\n",
					ac_no,g_td_mst_hst.tx_br_no,add_ind,g_td_mst_hst.tx_amt,g_td_mst_hst.tx_date,g_td_mst_hst.tx_time,g_td_mst_hst.hst_cnt,g_td_mst_hst.brf,g_td_mst_hst.tel,g_td_mst_hst.bal,g_td_mst_hst.add_ind,g_td_mst.name,page_cnt,g_mdm_ac_rel.note_no,g_mdm_attr.mdm_name,g_td_parm.cif_type,g_td_mst_hst.tx_code);
				hst_cnt++;
			}
			Td_mst_hst_Clo_Sel();
		}
		else
		{
			sprintf(acErrMsg,"�����붨���˻�!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O233");
			goto ErrExit;
		}
	}
	else if(ac_type[0]=='3')							/*** �������ʻ� ***/
	{
		if( date1==date2 ) 
		goto TAG;

		total_cnt=sql_count("sub_dd_mst_hst", "ac_no='%s' and sub_ac_no='%s'  and tx_date>=%ld and tx_date<=%ld", ac_no,sub_ac_no,date1,date2);
		if(total_cnt<0)
		{
			sprintf(acErrMsg,"transfer sql_count function error!!");
			WRITEMSG
			goto ErrExit;
		}

vtcp_log("DDDD:ac_no='%s' and sub_ac_no='%s' and tx_date>=%ld and tx_date<=%ld order by hst_cnt", ac_no,sub_ac_no,date1,date2);
		ret=Sub_dd_mst_Sel(g_pub_tx.reply,&s_sub_dd_mst,"ac_no='%s' and sub_ac_no='%s' ",ac_no,sub_ac_no);
		if( ret )
		{
			sprintf(acErrMsg,"Prepare Open Dd_mst_hst_Dec_Sel error!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D101");
			goto ErrExit;
		}

		ret=Sub_dd_mst_hst_Dec_Sel(g_pub_tx.reply, \
		"ac_no='%s' and sub_ac_no='%s' and tx_date>=%ld and tx_date<=%ld order by hst_cnt",ac_no,sub_ac_no,date1,date2);
		if( ret )
		{
			sprintf(acErrMsg,"Prepare Open Dd_mst_hst_Dec_Sel error!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D101");
			goto ErrExit;
		}

		count=0;
		while(1)
		{
			memset(&s_sub_dd_mst_hst,0x00,sizeof(struct sub_dd_mst_hst_c));
			ret=Sub_dd_mst_hst_Fet_Sel(&s_sub_dd_mst_hst,g_pub_tx.reply);
			if( ret==100 )
			{
				if(count==0)
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
				sprintf(acErrMsg,"ȡ�������ʺ���ϸ�ʳ���!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"W050");
				goto ErrExit;
			}
			count++;

			if(i==RECK_PAGE_LENG)
			{
				page_cnt++;
				i=0;
			}
			else
				i++;

			strcpy(g_mdm_ac_rel.note_no,"----------------");
			strcpy(g_mdm_attr.mdm_name,"------------");
			/**pub_base_dic_show_str(add_ind,"deta_type",g_dd_mst_hst.add_ind);**/
			if( s_sub_dd_mst_hst.add_ind[0]=='0' )
				strcpy(add_ind,"����");
			else
				strcpy(add_ind,"����");

			pub_base_getbrname(s_sub_dd_mst_hst.opn_br_no,br_name);
			pub_base_strpack(g_mdm_ac_rel.note_no);
			if( strlen(g_mdm_ac_rel.note_no)>12 )
			{
				strcpy( g_dd_mst_hst.note_no,g_mdm_ac_rel.note_no+strlen(g_mdm_ac_rel.note_no)-12 );
				strcpy( g_mdm_ac_rel.note_no,g_dd_mst_hst.note_no );
			}
			fprintf(fp,"DZD%s-%s|%s|%s|%.2lf|%ld|%ld|%ld|%s|%s|%.2lf|%s|%s|%d|%s|%s|5|%s|%s| |\n",
			ac_no,sub_ac_no,s_sub_dd_mst_hst.tx_br_no,add_ind,s_sub_dd_mst_hst.tx_amt,
			s_sub_dd_mst_hst.tx_date,s_sub_dd_mst_hst.tx_time,
			s_sub_dd_mst_hst.hst_cnt,s_sub_dd_mst_hst.brf,s_sub_dd_mst_hst.tel,
			s_sub_dd_mst_hst.bal,s_sub_dd_mst_hst.add_ind,s_sub_dd_mst.name,
			page_cnt,g_mdm_ac_rel.note_no,g_mdm_attr.mdm_name,br_name,
			"");
			hst_cnt++;
		}
		Sub_dd_mst_hst_Clo_Sel();
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

	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��ӡ���ʵ��ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;

TAG:
	/*** ������ͬ��ӡ���ʻص� ***/
	if( ac_type[0]=='1' )
	{
		ret = Dd_mst_Sel( g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d", \
						  g_mdm_ac_rel.ac_id,ac_seqn);
		if( ret==100 )
		{
			sprintf(acErrMsg,"����������ʺ�!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O232");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"���һ����˻���Ϣ!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D101");
			goto ErrExit;
		}

		if( g_pub_tx.tx_date==date1 ) 
			g_dd_mst_hst.bal = g_dd_mst.bal;
		else
		{
			ret = Dd_mst_hst_Sel( g_pub_tx.reply,&g_dd_mst_hst, "ac_id=%ld and ac_seqn=%d and tx_date<=%ld order by tx_date desc, hst_cnt desc,trace_no desc , tx_time desc", g_mdm_ac_rel.ac_id, ac_seqn, date1 );
			if( ret==100 )
			{
				sprintf(acErrMsg,"δ�ҵ��˻����˻���Ϣ!!ac_id=[%ld],ac_seqn=[%d],tx_date=[%ld]",g_mdm_ac_rel.ac_id, ac_seqn, date1);
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

		fprintf(fp,"DZHD%s|%s|%.2lf|%ld|\n",ac_no,g_dd_mst.name, \
					g_dd_mst_hst.bal,date1);
	}
	else if( ac_type[0]=='2' )
	{
		ret = Td_mst_Sel( g_pub_tx.reply,&g_td_mst,"ac_id=%ld", \
							g_mdm_ac_rel.ac_id);
		if( ret==100 )
		{
			sprintf(acErrMsg,"�����붨���ʺ�!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O233");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"δ�ҵ��˶����˻���Ϣ!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D101");
			goto ErrExit;
		}

		if( g_pub_tx.tx_date==date1 ) 
			g_td_mst_hst.bal = g_td_mst.bal;
		else
		{
			ret = Td_mst_hst_Sel( g_pub_tx.reply,&g_td_mst_hst, \
			 		"ac_id=%ld and ac_seqn=%d and tx_date<=%ld \
			  		order by tx_date desc,hst_cnt desc,tx_time desc", \
			  		g_mdm_ac_rel.ac_id,ac_seqn, date1);
			if( ret==100 )
			{
				sprintf(acErrMsg,"δ�ҵ��˶����˻���Ϣ!!");
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

		fprintf(fp,"DZHD%s|%s|%.2lf|%ld|\n",ac_no,g_td_mst.name, \
					g_td_mst_hst.bal,date1);
	}
	else if( ac_type[0]=='3' )
	{
		ret = Sub_dd_mst_Sel(g_pub_tx.reply,&s_sub_dd_mst,"ac_no='%s' and sub_ac_no='%s'",ac_no,sub_ac_no);
		if( ret )
		{
			sprintf(acErrMsg,"�����������˻���Ϣ!![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D101");
			goto ErrExit;
		}

		if( g_pub_tx.tx_date==date1 ) s_sub_dd_mst_hst.bal = s_sub_dd_mst.bal;
		else
		{
			ret = Sub_dd_mst_hst_Sel( g_pub_tx.reply,&s_sub_dd_mst_hst, \
			  	"ac_no='%s' and sub_ac_no='%s' and tx_date<=%ld \
			 	 order by tx_date desc,tx_time desc",ac_no,sub_ac_no, date1);
			if( ret==100 )
			{
				sprintf(acErrMsg,"δ�ҵ��˻����˻���Ϣ!!");
				WRITEMSG
				ret = Sub_dd_mst_hst_Sel( g_pub_tx.reply,&s_sub_dd_mst_hst, \
			  		"ac_no='%s' and sub_ac_no='%s' and tx_date>=%ld \
			 	 	order by tx_date,tx_time",ac_no,sub_ac_no, date1);
				if( ret==100 )
					s_sub_dd_mst_hst.bal=s_sub_dd_mst.bal;
				if( ret==0 )
					s_sub_dd_mst_hst.bal=s_sub_dd_mst_hst.bal-s_sub_dd_mst_hst.tx_amt;
			}
			if( ret && ret!=100 )
			{
				sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"D103");
				goto ErrExit;
			}
		}
		fprintf(fp,"DZHD%s-%s|%s|%.2lf|%ld|\n",ac_no,sub_ac_no,s_sub_dd_mst.name,s_sub_dd_mst_hst.bal,date1);
	}
	else
	{
		sprintf(acErrMsg,"�Ǵ�ȡ�����ʺ�!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
		goto ErrExit;
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
