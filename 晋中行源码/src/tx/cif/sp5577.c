/******************************************************
* �� �� ��:  sp5577.c
* ���������� �ϲ��ͻ��� 
* ����Ŀ��:  ���ڣ����ڣ����� �˻��������˽���
* ��    ��:  ����
* ������ڣ� 20100102
insert into tx_def values('5577','�ϲ��ͻ���','10000000000000000000000000010100000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000','1','6','0','0',0)
insert into tx_flow_def values('5577',0,'5577','#$')
insert into tx_sub_def values('5577','�ϲ��ͻ���','sp5577','0','0')
insert into rsp_code values('C222','�˿ͻ����Ѿ�ע�����޷����кϲ�!','');
*******************************************************/
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"
#include "ln_mst_c.h"
#include "prdt_ac_id_c.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "cif_merge_log_c.h"

sp5577()
{
	int    cnt=0;
       /** add by wudw 20120726 **/
	int    Cif_No=0;
        int    trace_ac_id=0;
        int   trace_ac_seqn=0;
       /** end **/
	char   ac_no[20];
	struct mdm_ac_rel_c sMdm;
	struct dd_mst_c     sDd;
	struct td_mst_c     sTd;
	struct ln_mst_c     sLn;
	struct prdt_ac_id_c sPrd;
	struct cif_basic_inf_c sCif;
	struct cif_basic_inf_c sCif_basic_inf;
	struct cif_id_code_rel_c sCif_id_code_rel;
	struct cif_id_code_rel_c sCifIdCodeRel;
	/*wudawei 20140512*/
	struct cif_merge_log_c sCif_merge_log;
	/*wudawei 20140512*/
	FILE *fp = NULL;
	char acFileName[100];
	char acFileContent[110];/** �洢��ȡ���ļ������� **/

	memset(acFileName, 0x00, sizeof(acFileName));
	memset(acFileContent,0x00, sizeof(acFileContent));
	memset(ac_no,0x00,sizeof(ac_no));
	memset(&sMdm,0x00,sizeof(sMdm));
	memset(&sDd,0x00,sizeof(sDd));
	memset(&sTd,0x00,sizeof(sTd));
	memset(&sLn,0x00,sizeof(sLn));
	memset(&sPrd,0x00,sizeof(sPrd));
	memset(&sCif,0x00,sizeof(sCif));
	memset(&sCif_basic_inf,0x00,sizeof(sCif_basic_inf));
	memset(&sCif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
	memset(&sCifIdCodeRel,0x00,sizeof(struct cif_id_code_rel_c));

	pub_base_sysinit();
	get_zd_data("0300",ac_no);
	get_zd_long("0280",&(sCif.cif_no));
	pub_base_old_acno(ac_no);
	Gl_file_first = 0;
	pub_base_AllDwnFilName(acFileName);
	fp = fopen(acFileName,"r");
	if (NULL == fp)
	{
	  sprintf(acErrMsg, "open file:[%s] error", acFileName);
	  strcpy(g_pub_tx.reply, "S047");
	  WRITEMSG
	  goto ErrExit;
	}

	/** add begin by yanqq 20120731 **/
	/** ��ȡ�¿ͻ��Ŷ�Ӧ�Ŀͻ�״̬��Ϣ **/
	g_reply_int=Cif_basic_inf_Sel(g_pub_tx.reply,&sCif_basic_inf,"cif_no=%ld",sCif.cif_no);
	if(g_reply_int)
	{
		vtcp_log("[%s][%d]���ݿ��¼�쳣���������Ļ�����ϵ[%d]",__FILE__,__LINE__,g_reply_int);
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	if(g_reply_int==0)
	{
		if(sCif_basic_inf.sts[0]=='2')
		{
			vtcp_log("[%s][%d]�˿ͻ����Ѿ�ע�����޷����кϲ�![%d]",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"C222");
			goto ErrExit;
		}
	}
	/** add end **/

	/** ��ȡ�¿ͻ��Ŷ�Ӧ�Ŀͻ�֤����Ϣ **/
	g_reply_int=Cif_id_code_rel_Sel(g_pub_tx.reply,&sCifIdCodeRel,"cif_no=%ld",sCif.cif_no);
	if(g_reply_int)
	{
		vtcp_log("[%s][%d]���ݿ��¼�쳣���������Ļ�����ϵ[%d]",__FILE__,__LINE__,g_reply_int);
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	while (1)
	{
		if (NULL != fgets(acFileContent,sizeof(acFileContent) - 1,fp))
		{
			pub_base_cut_str(acFileContent,ac_no,"|",1);
			pub_base_strpack(ac_no);
			pub_base_old_acno(ac_no);
		}
		else
		{
			break;
		}
                Cif_No=0;
		trace_ac_id=0;
                trace_ac_seqn=0;
		/**����ac_no��ȡac_id **/
		g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm,"ac_no='%s'",ac_no);
		if(g_reply_int)
		{
			vtcp_log("[%s][%d]��ѯ���ݿ����[%d]",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"C114");
			goto ErrExit;
		}
		else if(g_reply_int==100)
		{
			vtcp_log("[%s][%d]���˺��ڸ�ϵͳ�в�����",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D104");
			goto ErrExit;
		}
                trace_ac_id=sMdm.ac_id;
		/** ��ȡac_id��Ӧ�Ŀͻ���cif_no **/
#if 0
		g_reply_int = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrd,"ac_id=%ld",sMdm.ac_id);
		vtcp_log("[%s],[%d] ac_id=%d sprd.cif_no=%d",__FILE__,__LINE__,sPrd.ac_id,sPrd.cif_no);
		if(g_reply_int)
		{
			vtcp_log("[%s][%d]��ѯ���ݿ����[%d]",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"C114");
			goto ErrExit;
		}
#endif
                
		g_reply_int = Prdt_ac_id_Dec_Sel(g_pub_tx.reply, "ac_id=%ld", sMdm.ac_id);
		if(g_reply_int)
		{
			vtcp_log("[%s][%d]�������α�����������Ļ�����ϵ", __FILE__, __LINE__);
			strcpy(g_pub_tx.reply, "D101");
			goto ErrExit;
		}

		g_reply_int = Prdt_ac_id_Fet_Sel(&sPrd, g_pub_tx.reply);
		if(g_reply_int)
		{
			vtcp_log("[%s][%d]���ݿ��¼�쳣���������Ļ�����ϵ[%d]", __FILE__, __LINE__, g_reply_int);
			strcpy(g_pub_tx.reply, "D103");
			goto ErrExit;
		}		
		vtcp_log("[%s],[%d] sprd.cif_no=%d",__FILE__,__LINE__,sPrd.cif_no);
                vtcp_log("[%s],[%d] sprd.ac_id_type=%s",__FILE__,__LINE__,sPrd.ac_id_type);
		Cif_No=sPrd.cif_no;
                if(strcmp(sPrd.ac_id_type,"1")==0)
                {
                        trace_ac_seqn=1;
                }
                vtcp_log("[%s],[%d] trace_ac_seqn=%d",__FILE__,__LINE__,trace_ac_seqn);
		/** ��ѯ�ͻ����µ�ac_id **/
		g_reply_int=Prdt_ac_id_Dec_Sel(g_pub_tx.reply,"cif_no=%ld",sPrd.cif_no);
		if(g_reply_int)
		{
			vtcp_log("[%s][%d]�������α�����������Ļ�����ϵ",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D101");
			goto ErrExit;
		}

		while(1)
		{
			g_reply_int=Prdt_ac_id_Fet_Sel(&sPrd,g_pub_tx.reply);
			vtcp_log("%s,%d��ӡ�ͻ���=%ld",__FILE__,__LINE__,sPrd.cif_no);
			if(g_reply_int==100)
			{
				vtcp_log("[%s][%d]��ѯ���",__FILE__,__LINE__);
				break;
			}
			else if(g_reply_int)
			{
				vtcp_log("[%s][%d]���ݿ��¼�쳣���������Ļ�����ϵ[%d]",__FILE__,__LINE__,g_reply_int);
				strcpy(g_pub_tx.reply,"D103");
				goto ErrExit;
			}

			/** ͳ��Ǩ�Ƶ��˺����� **/
			cnt++;
			vtcp_log("%s,%d,Cnt=��%d��",__FILE__,__LINE__,cnt);

			switch(sPrd.ac_id_type[0])
			{
				case '1':
					/** ���²�Ʒ��ϸ���еĿͻ��� **/
					g_reply_int=Dd_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d  ",sPrd.ac_id,sPrd.ac_seqn);
					if(g_reply_int&&g_reply_int!=100)
					{
						vtcp_log("[%s][%d]�������α�����������Ļ�����ϵ[%d]",__FILE__,__LINE__,g_reply_int);
						strcpy(g_pub_tx.reply,"D101");
						goto ErrExit;
					}
					g_reply_int=Dd_mst_Fet_Upd(&sDd,g_pub_tx.reply);
					if(g_reply_int)
					{
						vtcp_log("[%s][%d]���ݿ��¼�쳣���������Ļ�����ϵ[%d]",__FILE__,__LINE__,g_reply_int);
						strcpy(g_pub_tx.reply,"D102");
						goto ErrExit;
					}
					/**
if(sDd.cif_no==sCif.cif_no)
					{
						vtcp_log("[%s][%d]���˺����ڸÿͻ���������Ǩ��[sDd.cif_no=%ld][scif.cif_no=%ld]",__FILE__,__LINE__,sDd.cif_no,sCif.cif_no);
						Prdt_ac_id_Clo_Sel();
						Dd_mst_Clo_Upd();
						strcpy(g_pub_tx.reply,"F035 ");
						goto ErrExit;
					}
**/
					sDd.cif_no=sCif.cif_no;
					g_reply_int=Dd_mst_Upd_Upd(sDd,g_pub_tx.reply);
					if(g_reply_int&&g_reply_int!=100)
					{
						vtcp_log("[%s][%d]����������ݿ��¼�����������Ļ�����ϵ[%d]",__FILE__,__LINE__,g_reply_int);
						strcpy(g_pub_tx.reply,"D105");
						goto ErrExit;
					}
					Dd_mst_Clo_Upd();
					break;
				case '2':
					g_reply_int=Td_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d  ",sPrd.ac_id,sPrd.ac_seqn);
					if(g_reply_int&&g_reply_int!=100)
					{
						vtcp_log("[%s][%d]�������α�����������Ļ�����ϵ[%d]",__FILE__,__LINE__,g_reply_int);
						strcpy(g_pub_tx.reply,"D101");
						goto ErrExit;
					}
					g_reply_int=Td_mst_Fet_Upd(&sTd,g_pub_tx.reply);
					if(g_reply_int==100)
					{
						Td_mst_Clo_Upd();
						break;
					}
					else if(g_reply_int)
					{
						vtcp_log("[%s][%d]���ݿ��¼�쳣���������Ļ�����ϵ[%d]",__FILE__,__LINE__,g_reply_int);
						strcpy(g_pub_tx.reply,"D102");
						goto ErrExit;
					}
					/**
					if(sTd.cif_no==sCif.cif_no)
					{
						vtcp_log("[%s][%d]���˺����ڸÿͻ���������Ǩ��",__FILE__,__LINE__);
						Prdt_ac_id_Clo_Sel();
						Td_mst_Clo_Upd();
						strcpy(g_pub_tx.reply,"F035 ");
						goto ErrExit;
					}
					**/
					sTd.cif_no=sCif.cif_no;
					g_reply_int=Td_mst_Upd_Upd(sTd,g_pub_tx.reply);
					if(g_reply_int)
					{
						vtcp_log("[%s][%d]������α�����������Ļ�����ϵ[%d]",__FILE__,__LINE__,g_reply_int);
						strcpy(g_pub_tx.reply,"D103");
						goto ErrExit;
					}
					Td_mst_Clo_Upd();
					break;
				case '3':
					g_reply_int=Ln_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d  ",sPrd.ac_id,sPrd.ac_seqn);
					if(g_reply_int&&g_reply_int!=100)
					{
						vtcp_log("[%s][%d]�������α�����������Ļ�����ϵ[%d]",__FILE__,__LINE__,g_reply_int);
						strcpy(g_pub_tx.reply,"D101");
						goto ErrExit;
					}
					g_reply_int=Ln_mst_Fet_Upd(&sLn,g_pub_tx.reply);
					if(g_reply_int==100)
					{
						Ln_mst_Clo_Upd();
						break;
					}
					else if(g_reply_int)
					{
						vtcp_log("[%s][%d]���ݿ��¼�쳣���������Ļ�����ϵ[%d]",__FILE__,__LINE__,g_reply_int);
						strcpy(g_pub_tx.reply,"D102");
						goto ErrExit;
					}
                                        /**
					if(sLn.cif_no==sCif.cif_no)
					{
						vtcp_log("[%s][%d]���˺����ڸÿͻ���������Ǩ��",__FILE__,__LINE__);
						Prdt_ac_id_Clo_Sel();
						Ln_mst_Clo_Upd();
						strcpy(g_pub_tx.reply,"F035 ");
						goto ErrExit;
					}
                                        **/
					sLn.cif_no=sCif.cif_no;
					g_reply_int=Ln_mst_Upd_Upd(sLn,g_pub_tx.reply);
					if(g_reply_int)
					{
						vtcp_log("[%s][%d]������α�����������Ļ�����ϵ[%d]",__FILE__,__LINE__,g_reply_int);
						strcpy(g_pub_tx.reply,"D103");
						goto ErrExit;
					}
					Ln_mst_Clo_Upd();
					break;
				default:
					if(cnt) break;
					else
					{
						vtcp_log("[%s][%d]��������",__FILE__,__LINE__);
						strcpy(g_pub_tx.reply,"LS57");
						goto ErrExit;
					}
			}
			
			/** add by yangxingkai 2012-5-22 ����mdm_ac_rel�е�֤�����ͺ�֤���� **/
			g_reply_int=sql_execute("update mdm_ac_rel set id_type='%s',id_no='%s' where ac_id='%d'",sCifIdCodeRel.id_type,sCifIdCodeRel.id_no,sPrd.ac_id);
			vtcp_log("[%s],[%d]  update mdm_ac_rel set id_type='%s',id_no='%s' where ac_id='%d'",__FILE__,__LINE__,sCifIdCodeRel.id_type,sCifIdCodeRel.id_no,sPrd.ac_id);
			if(g_reply_int)
			{
				vtcp_log("[%s][%d]�������ݿ��¼�쳣���������Ļ�����ϵ[%d]",__FILE__,__LINE__,g_reply_int);
				strcpy(g_pub_tx.reply,"D105");
				goto ErrExit;
			}
			/** end add by yangxingkai **/

		}

		Prdt_ac_id_Clo_Sel();

		/**add by xk 2012-2-13 14:38:51**/
		/**����֤�����͡�֤�������ѯ�ͻ���**/ 
	        /**add by wudw 20120725
		g_reply_int=Cif_id_code_rel_Sel(g_pub_tx.reply,&sCif_id_code_rel,"id_type='%s'and id_no='%s'",sMdm.id_type,sMdm.id_no);
		if (g_reply_int == 100)
		{
			sprintf(acErrMsg, "�ͻ�֤����ͻ��Ŷ��ձ����ڸü�¼,id_type=%s,id_no=%s",sMdm.id_type,sMdm.id_no,__FILE__,__LINE__,g_reply_int);
			WRITEMSG
				strcpy(g_pub_tx.reply, "D104");
			goto ErrExit;
		}
		else if (g_reply_int)
		{
			sprintf(acErrMsg, "���Ҽ�¼����[%d]", g_pub_tx);
			WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
			goto ErrExit;
		}
                add by wudw 20120725 end **/
		/** ��ѯ�ͻ��ţ�����ͻ����µ��˺Ŷ�Ǩ�ƣ���ע����������ʾ������δǨ�Ƶ��˺� **/
		memset(&sPrd,0x00,sizeof(sPrd));
		g_reply_int=Prdt_ac_id_Sel(g_pub_tx.reply,&sPrd,"cif_no=%ld",Cif_No);
		if(g_reply_int==100)
		{
			g_reply_int=sql_execute("update cif_basic_inf set sts='2' where cif_no = %ld",Cif_No);
			if(g_reply_int)
			{
				vtcp_log("[%s][%d]�������ݿ��¼�쳣���������Ļ�����ϵ[%d]",__FILE__,__LINE__,g_reply_int);
				strcpy(g_pub_tx.reply,"D105");
				goto ErrExit;
			}
		}
		else if(g_reply_int==0)
		{
			vtcp_log("%s,%d[�ÿͻ����»���δǨ�Ƶ��˻��������Ǩ��!!!]",__FILE__,__LINE__);
			set_zd_data("0700","1");/**����70���ֵ����ʾδǨ�Ƶ��˻�**/
		}
		else
		{
			sprintf(acErrMsg, "���Ҽ�¼����[%d]", g_pub_tx);
			WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
			goto ErrExit;
		}
		/**end  by xk 2012-2-13 14:38:54**/
		/** д�ϲ���־�� wudawei 20140512  **/
		memset(&sCif_merge_log,0x00,sizeof(sCif_merge_log));
		sCif_merge_log.cif_no_old = Cif_No;
		sCif_merge_log.cif_no_new = sCif.cif_no;
		sCif_merge_log.tx_date = g_pub_tx.tx_date;
		sCif_merge_log.tx_time = g_pub_tx.tx_time;
		strncpy(sCif_merge_log.tel , g_pub_tx.tel ,4);
		g_reply_int = Cif_merge_log_Ins( sCif_merge_log ,g_pub_tx.reply); 
		if( g_reply_int )
		{
		 sprintf(acErrMsg,"�ǼǺϲ��ͻ�����־��ʧ��![%d]",g_reply_int);
		 WRITEMSG
		 goto ErrExit;
		}
		/** end wudawei 20140512  **/
	}

	/** �����¿ͻ��ŵ���Ϣ,��Ҫ���޸��˿������� **/
	g_reply_int=Cif_basic_inf_Dec_Upd(g_pub_tx.reply,"cif_no=%ld",sCif.cif_no);
	if(g_reply_int)
	{
		vtcp_log("[%s][%d]�������α�����������Ļ�����ϵ[%d]",__FILE__,__LINE__,g_reply_int);
		strcpy(g_pub_tx.reply,"D101");
		goto ErrExit;
	}
	g_reply_int=Cif_basic_inf_Fet_Upd(&sCif,g_pub_tx.reply);
	if(g_reply_int==100)
	{
		vtcp_log("[%s][%d]���ݿ��¼�쳣���������Ļ�����ϵ",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D102");
		goto ErrExit;
	}
	else if(g_reply_int)
	{
		vtcp_log("[%s][%d]���ݿ��¼�쳣���������Ļ�����ϵ[%d]",__FILE__,__LINE__,g_reply_int);
		strcpy(g_pub_tx.reply,"D102");
		goto ErrExit;
	}
	sCif.cif_seqn+=cnt;
	g_reply_int=Cif_basic_inf_Upd_Upd(sCif,g_pub_tx.reply);
	if(g_reply_int)
	{
		vtcp_log("[%s][%d]������α�����������Ļ�����ϵ[%d]",__FILE__,__LINE__,g_reply_int);
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	Cif_basic_inf_Clo_Upd();

#if 0
	memset(&sCif_id_code_rel,0x0,sizeof(sCif_id_code_rel));
	g_reply_int=Cif_id_code_rel_Sel(g_pub_tx.reply,&sCif_id_code_rel,"cif_no=%ld",sCif.cif_no);
	if(g_reply_int)
	{
		vtcp_log("[%s][%d]���ݿ��¼�쳣���������Ļ�����ϵ[%d]",__FILE__,__LINE__,g_reply_int);
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	g_reply_int=sql_execute("update mdm_ac_rel set id_type='%s',id_no='%s' where ac_id='%d'",sCif_id_code_rel.id_type,sCif_id_code_rel.id_no,sMdm.ac_id);
	if(g_reply_int)
	{
		vtcp_log("[%s][%d]�������ݿ��¼�쳣���������Ļ�����ϵ[%d]",__FILE__,__LINE__,g_reply_int);
		strcpy(g_pub_tx.reply,"D105");
		goto ErrExit;
	}
#endif 
        strcpy(g_pub_tx.ac_no,ac_no);
        vtcp_log("[%s][%d] ac_id=[%d]\n",__FILE__,__LINE__,trace_ac_id);
        g_pub_tx.ac_id=trace_ac_id;
        g_pub_tx.ac_seqn=trace_ac_seqn;
        strcpy(g_pub_tx.brf,"�ϲ��ͻ���");
	g_reply_int=pub_ins_trace_log();
	if(g_reply_int)
	{
		vtcp_log("[%s][%d][%d]",__FILE__,__LINE__,g_reply_int);
		strcpy(g_pub_tx.reply,"S024");
		goto ErrExit;
	}
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	set_zd_data("0120",g_pub_tx.reply);
	return g_reply_int;
}
