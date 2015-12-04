/*************************************************
* �� �� ��:  spW010.c
* ���������� ������ϸ���˵���ϸ��Ϣ��ѯ 
*
* ��    ��:  hzh 
* ������ڣ� 2013��6��19�� 
*
* �޸ļ�¼�� 
* ��   ��:	
* �� �� ��:
* �޸�����:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "dd_mst_hst_c.h"
#include "mdm_ac_rel_c.h"
#include "td_mst_hst_c.h"
#include "com_branch_c.h"
#include "mdm_unprt_hst_c.h"
#include "dd_mst_c.h"
#include "chnl_self_trade_c.h"
extern char *get_env_info(char *infoname);
spW010()
{
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct dd_mst_hst_c vdd_mst_hst;
	struct td_mst_hst_c vtd_mst_hst;
	struct td_mst_hst_c f_td_mst_hst;
	struct td_mst_hst_c b_td_mst_hst;
	struct dd_mst_hst_c f_dd_mst_hst;
	struct dd_mst_hst_c b_dd_mst_hst;
	struct dd_mst_hst_c f_dd_mst_hsta;
	struct com_branch_c sCom_branch;
	struct mdm_unprt_hst_c sMdm_unprt_hst;
	struct dd_mst_c g_dd_mst;
	struct dd_mst_c f_dd_mst;
	struct td_mst_c f_td_mst;
	struct td_mst_c g_td_mst;
	struct chnl_self_trade_c g_chnl_self_trade;

	char            add_ind[3];
	char            ct_ind[5];
	char            ac_no3[25];
	char            f_note_type9[41];
	char            f_acno0[25];
	char            filename[100];
	char            pcfilename[100];
	char            wherelist[1024];	/**��ѯ����**/
	char            tmpstr[512];
	char            tmpmo[512];
	char			sPc_flag[2];   /* �Թ���˽��־ */
	char            fieldlist[1024];	/**Ҫ��ѯ���ֶ��б�**/
	char            tablelist[128];	/**Ҫ��ѯ�ı���**/
	char            titstr[1024];
	int             ttlnum = 0;	/**��ȡ��������**/
	int             ttlnum2 = 0;  /**��Ҫ�����ʼ��¼�ſ�ʼ��ȡ��������**/
	int             i = 0;
	int             ret = 0, dPage = 0;
	double					tmpbal = 0.00;
	FILE           *fp;
	char            tmp_date[9];
	char            tmp_time[7];
	int             currnum = 0;
	int 						count = 0;
	int							sqlcnt = 0;
	long            qs_date;
	long            jz_date;
	char            bz[4];
	char            flag[3];
	char            td_mdm[5];
	char            tx_date[8+1];
	char            sw_traceno[10+1];
	char            cFtpfile[200];
	char            sDf_ac[30];
	char            sDf_ac_name[100];

	memset(&f_td_mst_hst, 0x00, sizeof(f_td_mst_hst));
	memset(&b_td_mst_hst, 0x00, sizeof(b_td_mst_hst));
	memset(add_ind, 0x00, sizeof(add_ind));
	memset(sPc_flag,0,sizeof(sPc_flag));
	memset(ct_ind, 0x00, sizeof(ct_ind));
	memset(ac_no3, 0x00, sizeof(ac_no3));
	memset(f_note_type9, 0x00, sizeof(f_note_type9));
	memset(f_acno0, 0x00, sizeof(f_acno0));
	memset(&vdd_mst_hst, 0x00, sizeof(vdd_mst_hst));
		memset(&vtd_mst_hst, 0x00, sizeof(vtd_mst_hst));
	memset(&f_dd_mst_hst, 0x00, sizeof(f_dd_mst_hst));
	memset(&b_dd_mst_hst, 0x00, sizeof(b_dd_mst_hst));
	memset(&f_dd_mst_hsta, 0x00, sizeof(f_dd_mst_hsta));
	memset(&sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel));
	memset(&sCom_branch, 0x00, sizeof(sCom_branch));
	memset(&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset(&g_td_mst, 0x00, sizeof(struct td_mst_c));
	memset(&f_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset(&f_td_mst, 0x00, sizeof(struct td_mst_c));
	memset(&g_chnl_self_trade, 0x00, sizeof(struct chnl_self_trade_c));
	memset(tmpstr, 0x00, sizeof(tmpstr));
	memset(filename, 0x00, sizeof(filename));
	memset(pcfilename, 0x00, sizeof(pcfilename));
	memset(tmpmo, 0x00, sizeof(tmpmo));
	memset(titstr, 0x00, sizeof(titstr));
	memset(tmp_date, 0x00, sizeof(tmp_date));
	memset(tmp_time, 0x00, sizeof(tmp_time));
	memset(&g_pub_tx, 0, sizeof(g_pub_tx));
	memset(wherelist, 0, sizeof(wherelist));
	memset(fieldlist, 0, sizeof(fieldlist));
	memset(tablelist, 0, sizeof(tablelist));
	memset(bz, 0, sizeof(bz));
	memset(td_mdm, 0, sizeof(td_mdm));
	memset(tx_date, 0, sizeof(tx_date));
	memset(sw_traceno, 0, sizeof(sw_traceno));
	memset(cFtpfile, 0, sizeof(cFtpfile));
	memset(sDf_ac, 0, sizeof(sDf_ac));
	memset(sDf_ac_name, 0, sizeof(sDf_ac_name));
	pub_base_sysinit();

	get_zd_data("0300", f_acno0);
	pub_base_old_acno(f_acno0);	/** �������˺�**/
	get_zd_long("0440", &qs_date);
	get_zd_long("0450", &jz_date);

	get_zd_data("0710", flag);	/* ��ѯ��־ */
	get_zd_data("0660",sPc_flag);
	vtcp_log("LINE %d FILE %s  flag[%.3s]", __LINE__, __FILE__, flag);
	get_zd_data("0050",tx_date);
	get_zd_data("0520",sw_traceno);

	/**��ѯ**/
	pub_base_strpack(f_acno0);
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'", f_acno0);
	ERR_DEAL
	   
	memcpy( td_mdm ,sMdm_ac_rel.mdm_code, 4);
	vtcp_log("[%s],[%d],leijianhui=[%s]=[%s]", __FILE__, __LINE__, td_mdm,sMdm_ac_rel.mdm_code);
	
	ret = Dd_mst_Sel(&g_pub_tx.reply, &g_dd_mst, "ac_id=%ld and ac_seqn=%ld", sMdm_ac_rel.ac_id,sMdm_ac_rel.ac_seqn);
	if ( ret==100 )
	{
			sprintf(acErrMsg,"��ѯ�������ļ��쳣[%d]",ret);
			WRITEMSG
			strcpy (g_pub_tx.reply,"W014");
			return 1;
	}
	else if(ret)
	{
			sprintf(acErrMsg,"��ѯ�������ļ��쳣[%d]",ret);
			WRITEMSG
			    strcpy (g_pub_tx.reply,"W015");
			return 1;
	}
	else
	{
		set_zd_data("0250",g_dd_mst.name);
		vtcp_log("[%s],[%d],�˻�����Ϊ[%s]", __FILE__, __LINE__, g_dd_mst.name);

		/*	set_zd_double("0390",g_dd_mst.bal); */
	}
		
	vtcp_log("[%s],[%d],�����˻����Ϊ[%.2f]", __FILE__, __LINE__, g_dd_mst.bal);

 		count=sql_count("dd_mst_hst","ac_id=%ld and ac_seqn=%ld and tx_date>=%ld and tx_date<=%ld ", sMdm_ac_rel.ac_id,sMdm_ac_rel.ac_seqn,qs_date,jz_date);
	  vtcp_log("[%s],[%d],11112222count=[%d]", __FILE__, __LINE__,count);
	  if(count == 0)
	  {
	  	/*�к����begin*/
			ret = Dd_mst_hst_Sel(&g_pub_tx.reply, &b_dd_mst_hst,
			   "ac_id=%ld and ac_seqn=%ld and  tx_date>%ld  order by hst_cnt asc",g_dd_mst.ac_id, g_dd_mst.ac_seqn, jz_date);
			vtcp_log("[%s],[%d],ret=[%d]", __FILE__, __LINE__,ret);
			if ( ret==100 )
			{
				set_zd_double("0430",g_dd_mst.bal); /*����ϸ��û����˵���к����Ҫ�ӻ��ڶ����ļ���ȡ*/
				set_zd_double("0390",g_dd_mst.bal); /*����ϸ��û����˵����ǰ���Ҫ�ӻ��ڶ����ļ���ȡ*/
				vtcp_log("[%s],[%d],����ϸ��û����˵�����Ҫ�ӻ��ڶ����ļ���ȡg_dd_mst.bal=[%f]", __FILE__, __LINE__,g_dd_mst.bal);
			}
			else if(ret)
			{
				sprintf(acErrMsg,"��ѯ������ϸ���쳣[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W050");
				return 1;
			}
			else
			{
				if(memcmp(b_dd_mst_hst.add_ind,"1",1)==0)
					{
						b_dd_mst_hst.bal=b_dd_mst_hst.bal-b_dd_mst_hst.tx_amt;
						set_zd_double("0430",b_dd_mst_hst.bal); /*�к����*/
						set_zd_double("0390",b_dd_mst_hst.bal); /*��ǰ���*/
					}
				else
					{
						b_dd_mst_hst.bal=b_dd_mst_hst.bal+b_dd_mst_hst.tx_amt;
						set_zd_double("0430",b_dd_mst_hst.bal); /*�к����*/
						set_zd_double("0390",b_dd_mst_hst.bal); /*��ǰ���*/
					}
				vtcp_log("[%s],[%d],�к��˻����tmpbalΪ[%.2f]", __FILE__, __LINE__, b_dd_mst_hst.bal);
			
		  }		
			/*�к����end*/
	  }
		ret = Dd_mst_hst_Dec_Sel(g_pub_tx.reply, "ac_id=%ld and tx_date>=%ld and tx_date<=%d order by hst_cnt asc", sMdm_ac_rel.ac_id, qs_date, jz_date);
		vtcp_log("[%s],[%d],ret=[%d]", __FILE__, __LINE__,ret);
		ERR_DEAL

		ttlnum = 0;
		sprintf( filename,"6013netbank%s%s.txt",tx_date,sw_traceno );
		sprintf( pcfilename,"%s/%s",getenv("FILDIR"),filename );
		/**pub_base_AllDwnFilName(filename);**/
		fp = fopen(pcfilename, "w");
		if (fp == NULL)
	  {
			sprintf(acErrMsg, " open file [%s] error ", pcfilename);
			WRITEMSG
			strcpy(g_pub_tx.reply, "S047");
			vtcp_log("[%s],[%d]",__LINE__,__FILE__);
			goto ErrExit;
		}
		vtcp_log("[%s],[%d],����ȫ·���ļ���Ϊ[%s],�ļ���Ϊ[%s]", __FILE__, __LINE__, pcfilename,filename);
		
						vtcp_log("[%s],[%d],f_acno0=[%s]", __FILE__, __LINE__, f_acno0);
				/**��ʾ�б���**/
				/**if('2'==sPc_flag[0])**/
				if(memcmp(f_acno0,"5",1))
				{
					vtcp_log("[%s],[%d],qqqqq", __FILE__, __LINE__);
				  fprintf(fp, "������ˮ��|��������|�������|���׽��|�˻����|�Է��˺�|�Է��˻���|ժҪ|\n");
				}
				else
				{	
					vtcp_log("[%s],[%d],wwwww", __FILE__, __LINE__);
					fprintf(fp, "������ˮ��|��������|�������|���׽��|�˻����|�Է��˺�|�Է��˻���|ժҪ\n");
				}

				
		while (ttlnum <= count) 
		{
		ret = Dd_mst_hst_Fet_Sel(&vdd_mst_hst, g_pub_tx.reply);
		vtcp_log("[%s],[%d],ret=[%d]", __FILE__, __LINE__,ret);
		if (ret == 100)
		{
			ttlnum++;
			break;
		}
			vtcp_log("[%s],[%d],2222222", __FILE__, __LINE__);
			ERR_DEAL
			ttlnum++;
			vtcp_log("[%s],[%d],2222222ttlnum=[%d]", __FILE__, __LINE__,ttlnum);
			/*��һ�������ǰ���*/
			
			if(ttlnum == 1)
		{
			if(memcmp(vdd_mst_hst.add_ind,"1",1)==0)
			{
				tmpbal=vdd_mst_hst.bal-vdd_mst_hst.tx_amt;
				set_zd_double("0390",tmpbal); /*��ǰ���*/
			}
			else
			{
				tmpbal=vdd_mst_hst.bal+vdd_mst_hst.tx_amt;
				set_zd_double("0390",tmpbal); /*��ǰ���*/
			}
	  }

	  /*��һ�������ǰ���*/
	  if(ttlnum == count)
	  {
	  	set_zd_double("0430",vdd_mst_hst.bal); /*�к����*/
	  }
	 
		sprintf(tmp_date, "%ld", vdd_mst_hst.tx_date);
		sprintf(tmp_time, "%d", vdd_mst_hst.tx_time);
		strcpy(bz, "CNY");
	/*	ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'", vdd_mst_hst.tx_br_no);
		if (100 != ret)
			ERR_DEAL
		*/

		/**��ѯ�Է��˺źͶԷ��˻�����**/
	/*	if(memcmp(vdd_mst_hst.tx_code,"6203",4) == 0)
				{
					ret = Chnl_self_trade_Sel(&g_pub_tx.reply, &g_chnl_self_trade,
					"tx_date=%ld and trace_no=%ld",
					vdd_mst_hst.tx_date,vdd_mst_hst.trace_no);
					if ( ret==100 )
					{
					sprintf(acErrMsg,"��ѯ�����豸������ˮ��¼���޴���ˮ���޷�ȡ���Է��˻���Ϣ[%d]",ret);
					WRITEMSG
					}
					else if(ret)
					{
					sprintf(acErrMsg,"��ѯ�����豸������ˮ�쳣[%d]",ret);
					WRITEMSG
					strcpy (g_pub_tx.reply,"W015");
					return 1;
					}
					strcpy(sDf_ac_name,"����");
					strcpy(sDf_ac,"����");
				}
			else 
				{
					strcpy(sDf_ac_name,"����");
					strcpy(sDf_ac,"����");
				}*/
			/**��ѯ�Է��˺źͶԷ��˻�����**/
			
			strcpy(sDf_ac_name,"����");
			strcpy(sDf_ac,"����");
				if(memcmp(f_acno0,"5",1))
				{
					vtcp_log("[%s],[%d],aaaaa", __FILE__, __LINE__);
					fprintf(fp, "%20d|%6d|%1s|%17.2f|%17.2f|%20s|%50s|%60s|\n", vdd_mst_hst.hst_cnt,vdd_mst_hst.tx_date, vdd_mst_hst.add_ind, vdd_mst_hst.tx_amt, vdd_mst_hst.bal,sDf_ac, sDf_ac_name, vdd_mst_hst.brf);
	        vtcp_log("��ʼ��ӡ��%d|%d|%06d|%s|%17.2f|%17.2f|%50s|\n", vdd_mst_hst.hst_cnt,vdd_mst_hst.tx_date, vdd_mst_hst.tx_time, vdd_mst_hst.add_ind, vdd_mst_hst.tx_amt, vdd_mst_hst.bal, vdd_mst_hst.brf);
					}
				else{
					vtcp_log("[%s],[%d],sssss", __FILE__, __LINE__);
		      fprintf(fp, "%20d|%6d|%s|%17.2f|%17.2f|%20s|%30s|%30s|\n", vdd_mst_hst.hst_cnt,vdd_mst_hst.tx_date, vdd_mst_hst.add_ind, vdd_mst_hst.tx_amt, vdd_mst_hst.bal, sDf_ac, sDf_ac_name,vdd_mst_hst.brf);
	        vtcp_log("��ʼ��ӡ��%d|%d|%s|%17.2f|%17.2f|%s|%20s|%50s|\n", vdd_mst_hst.hst_cnt,vdd_mst_hst.tx_date, vdd_mst_hst.add_ind, vdd_mst_hst.tx_amt, vdd_mst_hst.bal, sDf_ac, sDf_ac_name, vdd_mst_hst.brf);
				}
		}

		

		ret = Dd_mst_hst_Clo_Sel();
		vtcp_log("[%s],[%d],ret=[%d]", __FILE__, __LINE__,ret);
		ERR_DEAL
		vtcp_log("[%s],[%d],fp=[%s]", __FILE__, __LINE__,fp);
			fclose(fp);
		vtcp_log("[%s],[%d],6666", __FILE__, __LINE__);

			/**set_zd_data(DC_FILE_SND, "1");**/
			set_zd_int("0480", count);
			set_zd_data("0260",filename);
			sprintf(cFtpfile,"%s/bin/ftp_to_nsw.sh %s %s %s",getenv("WORKDIR"),get_env_info("NSW_FTP_USER"),get_env_info("NSW_FTP_PWD"),filename);
			vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
			ret=system(cFtpfile);
			if (ret)
			{
			sprintf(acErrMsg,"ftp�ļ�[%s]��ƽ̨����[%d]",filename,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P157");
			goto ErrExit;
			}
			sprintf(acErrMsg,"ftp�ļ�[%s]��ƽ̨�ɹ�",filename);
			WRITEMSG
		
	



GoodExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before bad return: reply [%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}

