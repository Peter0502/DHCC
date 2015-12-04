/********************
* �� �� ��: gD207.c
* ��������: ���� jz_super ������ȡ
*
* ��    ��: zyl
* �������: 20110316
* ��    ע: ����ĺܶ��趨,ֻ�����ڽ�������.
* δ���...Ҫ�޸�Ϊ *.pc ��src/tx/eodec/gD206.pc
********************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define extern
#include <math.h>  
#include "public.h"
#include <sys/timeb.h> 
#include <time.h> 

#include "jz_super_c.h"

#include "trace_log_c.h"
#include "dc_log_c.h"
#include "com_branch_c.h"
#include "dd_mst_c.h"
#include "in_mst_c.h"
#include "ln_mst_c.h"
#include "prdt_ac_id_c.h"
#include "note_parm_c.h"
#include "note_mst_hst_c.h"
#include "dd_parm_c.h"
#include "td_parm_c.h"
#include "note_cheq_mst_c.h"
#include "com_chrg_hst_c.h"

gD207()
{
	/**���ݶ���**/
	int  ret = 0;
	int  count = 0;
	FILE *fp = NULL;
	FILE *pz = NULL;
	long filesize = 0;
	int  flag = 0;
	char tmpstr[256];
	char superstr[1024];
	char tmpfilename[64];
	long tdate = 0;
	long tno = 0;
	char Tmpbuf[256];
	char tx_date[9];
	char trace_no[9];
	char filename[64];
	long tmpdate = 0;
	long tmpno = 0;
	struct jz_super_c g_jz_super;
	struct trace_log_c g_trace_log;
	struct trace_log_c sTrace_log;
	struct note_parm_c g_note_parm;
	struct note_mst_hst_c g_note_mst_hst;
	struct prdt_ac_id_c sPrdtacid;
	struct com_branch_c g_com_branch;
	struct note_cheq_mst_c g_note_cheq_mst;
	struct com_chrg_hst_c g_com_chrg_hst;

	memset(&g_jz_super,0x00,sizeof(struct jz_super_c));
	memset(&g_trace_log,0x00,sizeof(struct trace_log_c));
	memset(&g_note_parm,0x00,sizeof(struct note_parm_c));
	memset(&g_note_mst_hst,0x00,sizeof(struct note_mst_hst_c));
	memset(&sTrace_log,0x00,sizeof(struct trace_log_c));
	memset(&sPrdtacid,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_com_branch,0x00,sizeof(struct com_branch_c));
	memset(&g_note_cheq_mst,0x00,sizeof(struct note_cheq_mst_c));
	memset(&g_com_chrg_hst,0x00,sizeof(struct com_chrg_hst_c));
	memset(tmpfilename,0x00,sizeof(tmpfilename));
	memset(Tmpbuf,0x00,sizeof(Tmpbuf));
	memset(tx_date,0x00,sizeof(tx_date));
	memset(trace_no,0x00,sizeof(trace_no));
	memset(filename,0x00,sizeof(filename));
	
	memset(tmpstr,0x00,sizeof(tmpstr));
	memset(superstr,0x00,sizeof(superstr));
	/****/

	g_reply_int=sql_begin(); /*������*/
	if(g_reply_int)
	{
		vtcp_log("[%s][%d]������ʧ�ܣ�������",__FILE__,__LINE__ );
		strcpy(g_pub_tx.reply,"S003");
		goto ErrExit;
	}
	/**��ʼ��**/
	pub_base_sysinit();
	
	/**�������ļ� jz_super.txt ���� 57�ַ�  trace_log.tx_date|trace_log.trace_no|filename  **/
	/**��ÿ�й̶�Ϊ57���ַ�**/
	/**�������ļ�**/
	if( (pz = fopen("/home/devht/etc/jz_super.txt","ab+")) == NULL)
	{
		vtcp_log("[%s][%d]�ļ���ȡʧ��..",__FILE__,__LINE__ );
		strcpy(g_pub_tx.reply,"L206");
		goto ErrExit;
	}
	/**��ȡ�ļ���С**/
	fseek(pz,0l,2);
	filesize = ftell(pz);
	vtcp_log("[%s][%d] filesize=[%ld]\n",__FILE__,__LINE__,filesize);
	
	if(filesize < 57 || filesize%57 != 0 ) /**�ļ���С**/
	{
		if(filesize == 0)
		{
			flag = 1;
		}
		if(filesize%57 != 0)
		{
			vtcp_log("[%s][%d]�����ļ�jz_super���ݳ���..",__FILE__,__LINE__ );
			strcpy(g_pub_tx.reply,"L206");
			goto ErrExit;
		}
	}
	else
	{
		fseek(pz,-57l,2);   /**ָ���������һ��**/
		fgets(tmpstr,256,pz);
		vtcp_log("zylsee34 length=%d",strlen(tmpstr));
	}
	/****/
	if(strlen(tmpstr) == 57 || flag == 1)
	{
		/**��tmpstr�ָ�**/
		pub_base_cut_str(tmpstr,tx_date,"|",1);
		pub_base_cut_str(tmpstr,trace_no,"|",2);
		pub_base_cut_str(tmpstr,filename,"|",3);
		tdate = atol(tx_date);
		tno = atol(trace_no);
		vtcp_log("[%s][%d] tx_date=[%s]",__FILE__,__LINE__,tx_date);
		vtcp_log("[%s][%d] trace_no=[%s]",__FILE__,__LINE__,trace_no);
		vtcp_log("[%s][%d] filename=[%s]",__FILE__,__LINE__,filename);
		vtcp_log("[%s][%d] tx_date=[%ld]",__FILE__,__LINE__,tdate);
		vtcp_log("[%s][%d] trace_no=[%ld]",__FILE__,__LINE__,tno);
		/****/
		g_trace_log.tx_date=tdate;
		g_trace_log.trace_no=tno;
		/**���ļ� /home/devht/data/filename ���ڴ洢��ȡ��������**/
		memset(filename,0x00,sizeof(filename));
		sprintf(filename,"/home/devht/data/jz_super_201103240000");
		if( (fp = fopen(filename,"ab+")) == NULL)
		{
			vtcp_log("[%s][%d]�ļ���ȡʧ��..",__FILE__,__LINE__ );
			strcpy(g_pub_tx.reply,"L206");
			goto ErrExit;
		}
		/**���ݷָ�õ������� ��ʼ��ȡ trace_log ѭ��**/
		/**if(strlen(tmpstr) == 57) ��ִ��  tmpstr�Ѿ����ֽ�..strlen!=57 haha**/

		if(flag == 1)
		{
			vtcp_log("[%s][%d],aaaseee3333",__FILE__,__LINE__);
			ret = Trace_log_Dec_Sel(g_pub_tx.reply,"trace_cnt = 1 order by tx_date,trace_no,trace_cnt ");
			if(ret)
			{
		    vtcp_log("[%s][%d],Trace_log_Dec_Sel=[%d]\n",__FILE__,__LINE__,ret);
				strcpy(g_pub_tx.reply,"D101");
				goto ErrExit;
			}
		}
		else
		{
			vtcp_log("[%s][%d],aaasee",__FILE__,__LINE__);
			ret = Trace_log_Dec_Sel(g_pub_tx.reply,"tx_date= %ld and trace_no > %ld and trace_cnt = 1 order by tx_date,trace_no,trace_cnt ",tdate,tno);
			if(ret)
			{
		    vtcp_log("[%s][%d],Trace_log_Dec_Sel=[%d]\n",__FILE__,__LINE__,ret);
				strcpy(g_pub_tx.reply,"D101");
				goto ErrExit;
			}
		}
		while(1)
		{
			  /**��ʱ g_trace_log δ��.������� Fet��ret=100 ��.���,���뵽 jz_super.txt��ʱ.����.00000000|00000000|***  ����..**/
				tmpdate = g_trace_log.tx_date;
				tmpno = g_trace_log.trace_no;
		    memset(&g_trace_log,0x00,sizeof(struct trace_log_c));
		    ret = Trace_log_Fet_Sel(&g_trace_log,g_pub_tx.reply);
		    if(ret == 100)
		    {
		      vtcp_log("[%s][%d] ret = 100",__FILE__,__LINE__);
		    	break;
		    }
		    else if(ret)
		    {
		    	vtcp_log("[%s][%d],Trace_log_Fet_Sel=[%d]\n",__FILE__,__LINE__,ret);
					strcpy(g_pub_tx.reply,"D102");
					goto ErrExit;
		    }
		    /**������ȡ ��ϸ **/
				struct timeb timebuffer;
				struct tm timenow;
				ftime( &timebuffer );
				memcpy(&timenow,localtime(&timebuffer.time),sizeof(struct tm));
				memset(Tmpbuf,0x00,sizeof(Tmpbuf));
				count++;
				sprintf(Tmpbuf,"aaaaaaaa_jz_super_%4d%02d%02d%02d%02d%02d%03d%03d",timenow.tm_year+1900,timenow.tm_mon+1,timenow.tm_mday,timenow.tm_hour,timenow.tm_min,timenow.tm_sec,timebuffer.millitm,count);
		    if(count == 999)
		    {
		    	count = 0;
		    }
		    strcpy(g_jz_super.uuid,Tmpbuf);/**uuid ��������..**/
		    sprintf(Tmpbuf,"%ld",g_trace_log.tx_date);
		    strcpy(g_jz_super.trandate,Tmpbuf);
		    strcpy(g_jz_super.areano,g_trace_log.opn_br_no);
		    strcpy(g_jz_super.tellerid,g_trace_log.tel);
		    memset(Tmpbuf,0x00,sizeof(Tmpbuf));
		    sprintf(Tmpbuf,"%ld",g_trace_log.trace_no);
		    strcpy(g_jz_super.serseqno,Tmpbuf);
		    strcpy(g_jz_super.ywcode,g_trace_log.tx_code);
		    strcpy(g_jz_super.businesssort,"");/**ҵ������ ͨ��ͨ�Ҳ�Ҫ��.**/
		    
		    /**���׷�ʽ �ֽ�ת��**/
		    if(g_trace_log.ct_ind[0] == '1')
		    {
		    	strcpy(g_jz_super.dealmode,"�ֽ�");
		    }
		    else if(g_trace_log.ct_ind[0] == '2')
		    {
		    	strcpy(g_jz_super.dealmode,"ת��");
		    }
		    else
		    {
		    	strcpy(g_jz_super.dealmode,"");
		  	}
		  	/**ƾ֤����**/
		  	if(strlen(g_trace_log.note_type) == 3)
		  	{
		  		ret = Note_parm_Sel(g_pub_tx.reply,&g_note_parm,"note_type='%s'",g_trace_log.note_type);
		  		if(ret)
		  		{
		    		vtcp_log("[%s][%d],Note_parm_Sel ret=[%d]\n",__FILE__,__LINE__,ret);
						strcpy(g_pub_tx.reply,"D103");
						goto ErrExit;
		  		}
		  		strcpy(g_jz_super.warrantname,g_note_parm.name);
		  	}
		  	else
		  	{
		  		strcpy(g_jz_super.warrantname,"");
		  	}
		  	/**7105 ���� ���鷳**/
		  	ret = Note_mst_hst_Sel(g_pub_tx.reply,&g_note_mst_hst,"note_type='%s' and beg_note_no <= '%s' and end_note_no >='%s' and tel='%s' ",g_trace_log.note_type,g_trace_log.note_no,g_trace_log.note_no,g_trace_log.tel);
		  	if(ret == 100)
		  	{
		  		strcpy(g_jz_super.count,"");
		  		strcpy(g_jz_super.startno,"");
		  		strcpy(g_jz_super.endno,"");
		  	}
		  	else if(ret)
		  	{
		    	vtcp_log("[%s][%d],Note_mst_hst_Sel ret=[%d]\n",__FILE__,__LINE__,ret);
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
		  	}
		  	else
		  	{
		  		memset(Tmpbuf,0x00,sizeof(Tmpbuf));
		  		sprintf(Tmpbuf,"%d",g_note_mst_hst.note_cnt);
		  		strcpy(g_jz_super.count,Tmpbuf);
		  		strcpy(g_jz_super.startno,g_note_mst_hst.beg_note_no);
		  		strcpy(g_jz_super.endno,g_note_mst_hst.end_note_no);		  		
		  	}

		  	/**���� ��,��**/
		  	ret = Note_cheq_mst_Sel(g_pub_tx.reply,&g_note_cheq_mst,"ac_id=%ld and note_type='%s' and beg_note_no <= '%s' and end_note_no >='%s'",g_trace_log.ac_id,g_trace_log.note_type,g_trace_log.note_no,g_trace_log.note_no);
		  	if(ret == 100)
		  	{
		  		/**�ǳ�����ʽ**/
		  		strcpy(g_jz_super.unit,"");
		  		strcpy(g_jz_super.flatcost,"");
		  		strcpy(g_jz_super.poundage,"");
		  	}
		  	else if(ret)
		  	{
		    	vtcp_log("[%s][%d],Note_cheq_mst_Sel ret=[%d]\n",__FILE__,__LINE__,ret);
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
		  	}
		  	if(g_note_cheq_mst.grp_ind[0] == 'Y')
		  	{
		  		strcpy(g_jz_super.unit,"��");
		  	}
		  	else if(g_note_cheq_mst.grp_ind[0] == 'N')
		  	{
		  		strcpy(g_jz_super.unit,"��");
		  	}

		  	/**������,������**/
		  	ret = Com_chrg_hst_Sel(g_pub_tx.reply,&g_com_chrg_hst,"trace_no=%ld and tx_date=%ld and chrg_rate_code ='PZ1'",g_note_mst_hst.trace_no,g_note_mst_hst.tx_date);
		  	if(ret == 100)
		  	{
		  		strcpy(g_jz_super.poundage,"");
		  	}
		  	else if(ret)
		  	{
		    	vtcp_log("[%s][%d],Com_chrg_hst_Sel ret=[%d]\n",__FILE__,__LINE__,ret);
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
		  	}
		  	else
		  	{
		  		memset(Tmpbuf,0x00,sizeof(Tmpbuf));
		  		sprintf(Tmpbuf,"%17.2f",g_com_chrg_hst.chrg_amt);
		  		strcpy(g_jz_super.poundage,Tmpbuf);
		  	}
		  	memset(&g_com_chrg_hst,0x00,sizeof(struct com_chrg_hst_c));
		  	ret = Com_chrg_hst_Sel(g_pub_tx.reply,&g_com_chrg_hst,"trace_no=%ld and tx_date=%ld and chrg_rate_code ='PZ2'",g_note_mst_hst.trace_no,g_note_mst_hst.tx_date);
		  	if(ret == 100)
		  	{
		  		strcpy(g_jz_super.flatcost,"");
		  	}
		  	else if(ret)
		  	{
		    	vtcp_log("[%s][%d],Com_chrg_hst_Sel ret=[%d]\n",__FILE__,__LINE__,ret);
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
		  	}
		  	else
		  	{
		  		memset(Tmpbuf,0x00,sizeof(Tmpbuf));
		  		sprintf(Tmpbuf,"%17.2f",g_com_chrg_hst.chrg_amt);
		  		strcpy(g_jz_super.flatcost,Tmpbuf);
		  	}
		  	/**���� �����˺��� �������֧��ȡֵ**/
		  	/**��Ŀ���� �Ƿ��ϴ�Ӱ���.**/
		  	strcpy(g_jz_super.accode,"");
		  	strcpy(g_jz_super.isup,"");
		  	strcpy(g_jz_super.state,"00");
		    /**trace_log�� amt ��ֵ..�п���add_ind=0 ʱ 1 add_ind=1ʱ 1000 ��ôȡ?**/
				if(g_trace_log.add_ind[0] == '1')
				{
					memset(Tmpbuf,0x00,sizeof(Tmpbuf));
					sprintf(Tmpbuf,"%17.2f",g_trace_log.amt);
					strcpy(g_jz_super.tranamt,Tmpbuf);
					strcpy(g_jz_super.mondyb,pub_rpt_pcask(g_trace_log.amt));/**��д**/
					ret = Trace_log_Dec_Upd(g_pub_tx.reply,"tx_date=%ld and trace_no = %ld and add_ind = '0' ",g_trace_log.tx_date,g_trace_log.trace_no);
					if(ret)
					{
		    		vtcp_log("[%s][%d],Trace_log_Dec_Upd=[%d]\n",__FILE__,__LINE__,ret);
						strcpy(g_pub_tx.reply,"D103");
						goto ErrExit;
					}
					ret = Trace_log_Fet_Upd(&sTrace_log,g_pub_tx.reply);
					
					if(ret == 100)
					{
						/**��������Ϣ ֻ���տ���**/
						strcpy(g_jz_super.faccno,"");
						strcpy(g_jz_super.faccname,"");
						strcpy(g_jz_super.faccbank,"");
						/**���� accountname ֤���� idcode �����˵� mdm_ac_rel.note_no **/
						strcpy(g_jz_super.accountname,"");
						strcpy(g_jz_super.idcode,"");
						strcpy(g_jz_super.savetype,"");
						/**�տ�����Ϣ ��Ŀʱ?? **/
						strcpy(g_jz_super.saccno,g_trace_log.ac_no);
						/*find Saccname*/
						/**(strlen(g_trace_log.ac_no)==10 && g_trace_log.ac_no[0]=='5') ���Թ�Աac_no**/
						/**in_mst dd_mst td_mst ac_no����Ϊ13or19 ln_mst ac_no is 10or13 **/
						if((strlen(g_trace_log.ac_no)==10 && g_trace_log.ac_no[0]=='5') ||strlen(g_trace_log.ac_no)==13 || strlen(g_trace_log.ac_no)==19)
						{
							memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
							ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no = '%s'",g_trace_log.ac_no);
		    			if(ret)
		    			{
		    				vtcp_log("[%s][%d],Mdm_ac_rel_Sel ret=[%d]\n",__FILE__,__LINE__,ret);
								strcpy(g_pub_tx.reply,"D103");
								goto ErrExit;
		    			}
							ret = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdtacid,"ac_id=%ld",g_mdm_ac_rel.ac_id);
		    			if(ret)
		    			{
		    				vtcp_log("[%s][%d],Prdt_ac_id_Sel ret=[%d]\n",__FILE__,__LINE__,ret);
								strcpy(g_pub_tx.reply,"D103");
								goto ErrExit;
		    			}
		    			if(sPrdtacid.ac_id_type[0] == '1')/**����**/
		    			{
		    				memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
								ret = Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst, "ac_id=%ld", g_mdm_ac_rel.ac_id);
								if(ret == 100) 
								{
									sprintf(acErrMsg, "���ҿͻ��š�����ʧ��!");
									WRITEMSG
									strcpy(g_pub_tx.reply, "M003");
									goto ErrExit;
								}
								else if(ret) 
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								strcpy(g_jz_super.saccname,g_dd_mst.name);
								
		    			}
		    			else if(sPrdtacid.ac_id_type[0] == '2')/**����**/
		    			{
		    				memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
								ret = Td_mst_Sel(g_pub_tx.reply, &g_td_mst, "ac_id=%ld", g_mdm_ac_rel.ac_id);
								if(ret == 100) 
								{
									sprintf(acErrMsg, "���ҿͻ��š�����ʧ��!");
									WRITEMSG
									strcpy(g_pub_tx.reply, "M003");
									goto ErrExit;
								}
								else if(ret) 
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								strcpy(g_jz_super.saccname,g_td_mst.name);
		    			}
		    			else if(sPrdtacid.ac_id_type[0] == '3')/**����**/
		    			{
		    				memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
								ret = Ln_mst_Sel(g_pub_tx.reply, &g_ln_mst, "ac_id=%ld", g_mdm_ac_rel.ac_id);
								if(ret == 100) 
								{
									sprintf(acErrMsg, "���ҿͻ��š�����ʧ��!");
									WRITEMSG
									strcpy(g_pub_tx.reply, "M003");
									goto ErrExit;
								}
								else if(ret) 
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								strcpy(g_jz_super.saccname,g_ln_mst.name);
		    			}
		    			else if(sPrdtacid.ac_id_type[0] == '9')/**�ڲ�**/
		    			{
		    				memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
								ret = In_mst_Sel(g_pub_tx.reply, &g_in_mst, "ac_id=%ld", g_mdm_ac_rel.ac_id);
								if(ret == 100) 
								{
									sprintf(acErrMsg, "���ҿͻ��š�����ʧ��!");
									WRITEMSG
									strcpy(g_pub_tx.reply, "M003");
									goto ErrExit;
								}
								else if(ret) 
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								strcpy(g_jz_super.saccname,g_in_mst.name);
		    			}
		    			else
		    			{
		    				vtcp_log("[%s][%d],�˻����ʹ��� ac_id_type=[%s]",__FILE__,__LINE__,sPrdtacid.ac_id_type);
								strcpy(g_pub_tx.reply, "P410");
								goto ErrExit;
		    			}
						}
						else
						{
		    			vtcp_log("[%s][%d] ac_no=[%s] \n",__FILE__,__LINE__,g_trace_log.ac_no);
							/**
							strcpy(g_pub_tx.reply,"D103");
							goto ErrExit;
							**/
						}
						/*find Saccbank*/
						if(strlen(g_trace_log.opn_br_no) == 5)/**�������� 5λ�����к�**/
						{
							memset(&g_com_branch,0x00,sizeof(struct com_branch_c));
							ret = Com_branch_Sel(g_pub_tx.reply, &g_com_branch, "br_no='%s'", g_trace_log.opn_br_no);
							if(ret == 100) 
							{
								sprintf(acErrMsg, "Com_branch_Selʧ��!");
								WRITEMSG
								strcpy(g_pub_tx.reply, "M003");
								goto ErrExit;
							}
							else if(ret) 
							{
									sprintf(acErrMsg, "����ʧ��! [%d]", ret);
									WRITEMSG
									strcpy(g_pub_tx.reply, "D103");
									goto ErrExit;
							}
							strcpy(g_jz_super.saccbank,g_com_branch.br_name);
						}
						else
						{
		    			vtcp_log("[%s][%d] opn_br_no=[%s] \n",__FILE__,__LINE__,g_trace_log.opn_br_no);
							strcpy(g_pub_tx.reply,"D103");
							goto ErrExit;
						}
					}
					else if(ret)
					{
		    		vtcp_log("[%s][%d],Trace_log_Fet_Upd=[%d]\n",__FILE__,__LINE__,ret);
						strcpy(g_pub_tx.reply,"D103");
						goto ErrExit;
					}
					else
					{
						/**��������Ϣ**/
						strcpy(g_jz_super.faccno,sTrace_log.ac_no);
						/*find Faccname*/
						if((strlen(g_trace_log.ac_no)==10 && g_trace_log.ac_no[0]=='5') ||strlen(sTrace_log.ac_no)==13 || strlen(sTrace_log.ac_no)==19)
						{
							memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
							ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no = '%s'",sTrace_log.ac_no);
		    			if(ret)
		    			{
		    				vtcp_log("[%s][%d],Mdm_ac_rel_Sel ret=[%d]\n",__FILE__,__LINE__,ret);
								strcpy(g_pub_tx.reply,"D103");
								goto ErrExit;
		    			}
							ret = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdtacid,"ac_id=%ld",g_mdm_ac_rel.ac_id);
		    			if(ret)
		    			{
		    				vtcp_log("[%s][%d],Prdt_ac_id_Sel ret=[%d]\n",__FILE__,__LINE__,ret);
								strcpy(g_pub_tx.reply,"D103");
								goto ErrExit;
		    			}
		    			if(sPrdtacid.ac_id_type[0] == '1')/**����**/
		    			{
		    				memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
								ret = Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst, "ac_id=%ld", g_mdm_ac_rel.ac_id);
								if(ret == 100) 
								{
									sprintf(acErrMsg, "���ҿͻ��š�����ʧ��!");
									WRITEMSG
									strcpy(g_pub_tx.reply, "M003");
									goto ErrExit;
								}
								else if(ret) 
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								strcpy(g_jz_super.faccname,g_dd_mst.name);
								/**������Ϣ**/
								ret = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no = '%s'",g_dd_mst.prdt_no);
								if(ret)
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								if(g_dd_parm.cif_type[0] == '1')
								{
									strcpy(g_jz_super.savetype,g_dd_parm.title);
								}
								else
								{
									strcpy(g_jz_super.savetype,"");
								}
		    			}
		    			else if(sPrdtacid.ac_id_type[0] == '2')/**����**/
		    			{
		    				memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
								ret = Td_mst_Sel(g_pub_tx.reply, &g_td_mst, "ac_id=%ld", g_mdm_ac_rel.ac_id);
								if(ret == 100) 
								{
									sprintf(acErrMsg, "���ҿͻ��š�����ʧ��!");
									WRITEMSG
									strcpy(g_pub_tx.reply, "M003");
									goto ErrExit;
								}
								else if(ret) 
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								strcpy(g_jz_super.faccname,g_td_mst.name);
								/**������Ϣ**/
								ret = Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no = '%s'",g_td_mst.prdt_no);
								if(ret)
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								if(g_td_parm.cif_type[0] == '1')
								{
									strcpy(g_jz_super.savetype,g_td_parm.title);
								}
								else
								{
									strcpy(g_jz_super.savetype,"");
								}
		    			}
		    			else if(sPrdtacid.ac_id_type[0] == '3')/**����**/
		    			{
		    				memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
								ret = Ln_mst_Sel(g_pub_tx.reply, &g_ln_mst, "ac_id=%ld", g_mdm_ac_rel.ac_id);
								if(ret == 100) 
								{
									sprintf(acErrMsg, "���ҿͻ��š�����ʧ��!");
									WRITEMSG
									strcpy(g_pub_tx.reply, "M003");
									goto ErrExit;
								}
								else if(ret) 
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								strcpy(g_jz_super.faccname,g_ln_mst.name);
		    			}
		    			else if(sPrdtacid.ac_id_type[0] == '9')/**�ڲ�**/
		    			{
		    				memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
								ret = In_mst_Sel(g_pub_tx.reply, &g_in_mst, "ac_id=%ld", g_mdm_ac_rel.ac_id);
								if(ret == 100) 
								{
									sprintf(acErrMsg, "���ҿͻ��š�����ʧ��!");
									WRITEMSG
									strcpy(g_pub_tx.reply, "M003");
									goto ErrExit;
								}
								else if(ret) 
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								strcpy(g_jz_super.faccname,g_in_mst.name);
		    			}
		    			else
		    			{
		    				vtcp_log("[%s][%d],�˻����ʹ��� ac_id_type=[%s]",__FILE__,__LINE__,sPrdtacid.ac_id_type);
								strcpy(g_pub_tx.reply, "P410");
								goto ErrExit;
		    			}
						}
						else
						{
		    			vtcp_log("[%s][%d] ac_no=[%s] \n",__FILE__,__LINE__,sTrace_log.ac_no);
							/**strcpy(g_pub_tx.reply,"D103");
							goto ErrExit;**/
						}
						/**���� accountname ֤���� idcode �����˵� mdm_ac_rel.note_no **/
						strcpy(g_jz_super.accountname,g_jz_super.faccname);
						strcpy(g_jz_super.idcode,g_mdm_ac_rel.note_no);
						
						/*find Faccbank*/
						if(strlen(sTrace_log.opn_br_no) == 5)/**�������� 5λ�����к�**/
						{
							memset(&g_com_branch,0x00,sizeof(struct com_branch_c));
							ret = Com_branch_Sel(g_pub_tx.reply, &g_com_branch, "br_no='%s'", sTrace_log.opn_br_no);
							if(ret == 100) 
							{
								sprintf(acErrMsg, "Com_branch_Selʧ��!");
								WRITEMSG
								strcpy(g_pub_tx.reply, "M003");
								goto ErrExit;
							}
							else if(ret) 
							{
									sprintf(acErrMsg, "����ʧ��! [%d]", ret);
									WRITEMSG
									strcpy(g_pub_tx.reply, "D103");
									goto ErrExit;
							}
							strcpy(g_jz_super.faccbank,g_com_branch.br_name);
						}
						else
						{
		    			vtcp_log("[%s][%d] opn_br_no=[%s] \n",__FILE__,__LINE__,sTrace_log.opn_br_no);
							strcpy(g_pub_tx.reply,"D103");
							goto ErrExit;
						}
						/**�տ�����Ϣ**/
						strcpy(g_jz_super.saccno,g_trace_log.ac_no);
						/*find Saccname*/
						if((strlen(g_trace_log.ac_no)==10 && g_trace_log.ac_no[0]=='5') ||strlen(g_trace_log.ac_no)==13 || strlen(g_trace_log.ac_no)==19)
						{
							memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
							ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no = '%s'",g_trace_log.ac_no);
		    			if(ret)
		    			{
		    				vtcp_log("[%s][%d],Mdm_ac_rel_Sel ret=[%d]\n",__FILE__,__LINE__,ret);
								strcpy(g_pub_tx.reply,"D103");
								goto ErrExit;
		    			}
							ret = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdtacid,"ac_id=%ld",g_mdm_ac_rel.ac_id);
		    			if(ret)
		    			{
		    				vtcp_log("[%s][%d],Prdt_ac_id_Sel ret=[%d]\n",__FILE__,__LINE__,ret);
								strcpy(g_pub_tx.reply,"D103");
								goto ErrExit;
		    			}
		    			if(sPrdtacid.ac_id_type[0] == '1')/**����**/
		    			{
		    				memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
								ret = Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst, "ac_id=%ld", g_mdm_ac_rel.ac_id);
								if(ret == 100) 
								{
									sprintf(acErrMsg, "���ҿͻ��š�����ʧ��!");
									WRITEMSG
									strcpy(g_pub_tx.reply, "M003");
									goto ErrExit;
								}
								else if(ret) 
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								strcpy(g_jz_super.saccname,g_dd_mst.name);
		    			}
		    			else if(sPrdtacid.ac_id_type[0] == '2')/**����**/
		    			{
		    				memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
								ret = Td_mst_Sel(g_pub_tx.reply, &g_td_mst, "ac_id=%ld", g_mdm_ac_rel.ac_id);
								if(ret == 100) 
								{
									sprintf(acErrMsg, "���ҿͻ��š�����ʧ��!");
									WRITEMSG
									strcpy(g_pub_tx.reply, "M003");
									goto ErrExit;
								}
								else if(ret) 
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								strcpy(g_jz_super.saccname,g_td_mst.name);
		    			}
		    			else if(sPrdtacid.ac_id_type[0] == '3')/**����**/
		    			{
		    				memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
								ret = Ln_mst_Sel(g_pub_tx.reply, &g_ln_mst, "ac_id=%ld", g_mdm_ac_rel.ac_id);
								if(ret == 100) 
								{
									sprintf(acErrMsg, "���ҿͻ��š�����ʧ��!");
									WRITEMSG
									strcpy(g_pub_tx.reply, "M003");
									goto ErrExit;
								}
								else if(ret) 
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								strcpy(g_jz_super.saccname,g_ln_mst.name);
		    			}
		    			else if(sPrdtacid.ac_id_type[0] == '9')/**�ڲ�**/
		    			{
		    				memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
								ret = In_mst_Sel(g_pub_tx.reply, &g_in_mst, "ac_id=%ld", g_mdm_ac_rel.ac_id);
								if(ret == 100) 
								{
									sprintf(acErrMsg, "���ҿͻ��š�����ʧ��!");
									WRITEMSG
									strcpy(g_pub_tx.reply, "M003");
									goto ErrExit;
								}
								else if(ret) 
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								strcpy(g_jz_super.saccname,g_in_mst.name);
		    			}
		    			else
		    			{
		    				vtcp_log("[%s][%d],�˻����ʹ��� ac_id_type=[%s]",__FILE__,__LINE__,sPrdtacid.ac_id_type);
								strcpy(g_pub_tx.reply, "P410");
								goto ErrExit;
		    			}
						}
						else
						{
		    			vtcp_log("[%s][%d] ac_no=[%s] \n",__FILE__,__LINE__,g_trace_log.ac_no);
							/**strcpy(g_pub_tx.reply,"D103");
							goto ErrExit;**/
						}
						/*find Saccbank*/
						if(strlen(g_trace_log.opn_br_no) == 5)/**�������� 5λ�����к�**/
						{
							memset(&g_com_branch,0x00,sizeof(struct com_branch_c));
							ret = Com_branch_Sel(g_pub_tx.reply, &g_com_branch, "br_no='%s'", g_trace_log.opn_br_no);
							if(ret == 100) 
							{
								sprintf(acErrMsg, "Com_branch_Selʧ��!");
								WRITEMSG
								strcpy(g_pub_tx.reply, "M003");
								goto ErrExit;
							}
							else if(ret) 
							{
									sprintf(acErrMsg, "����ʧ��! [%d]", ret);
									WRITEMSG
									strcpy(g_pub_tx.reply, "D103");
									goto ErrExit;
							}
							strcpy(g_jz_super.saccbank,g_com_branch.br_name);
						}
						else
						{
		    			vtcp_log("[%s][%d] opn_br_no=[%s] \n",__FILE__,__LINE__,g_trace_log.opn_br_no);
							strcpy(g_pub_tx.reply,"D103");
							goto ErrExit;
						}
					}
				}
				else
				{
					ret = Trace_log_Dec_Upd(g_pub_tx.reply,"tx_date=%ld and trace_no = %ld and add_ind = '1' ",g_trace_log.tx_date,g_trace_log.trace_no);
					if(ret)
					{
		    		vtcp_log("[%s][%d],Trace_log_Dec_Upd=[%d]\n",__FILE__,__LINE__,ret);
						strcpy(g_pub_tx.reply,"D103");
						goto ErrExit;
					}
					ret = Trace_log_Fet_Upd(&sTrace_log,g_pub_tx.reply);
					
					if(ret == 100)
					{
						memset(Tmpbuf,0x00,sizeof(Tmpbuf));
						sprintf(Tmpbuf,"%17.2f",g_trace_log.amt);
						strcpy(g_jz_super.tranamt,Tmpbuf);
						strcpy(g_jz_super.mondyb,pub_rpt_pcask(g_trace_log.amt));/**��д**/
						/**add_ind=0 ���� add_ind=1 ������.**/
						/**��������Ϣ**/
						strcpy(g_jz_super.faccno,g_trace_log.ac_no);
						/*find faccname*/
						if((strlen(g_trace_log.ac_no)==10 && g_trace_log.ac_no[0]=='5') ||strlen(g_trace_log.ac_no)==13 || strlen(g_trace_log.ac_no)==19)
						{
							memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
							ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no = '%s'",g_trace_log.ac_no);
		    			if(ret)
		    			{
		    				vtcp_log("[%s][%d],Mdm_ac_rel_Sel ret=[%d]\n",__FILE__,__LINE__,ret);
								strcpy(g_pub_tx.reply,"D103");
								goto ErrExit;
		    			}
							ret = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdtacid,"ac_id=%ld",g_mdm_ac_rel.ac_id);
		    			if(ret)
		    			{
		    				vtcp_log("[%s][%d],Prdt_ac_id_Sel ret=[%d]\n",__FILE__,__LINE__,ret);
								strcpy(g_pub_tx.reply,"D103");
								goto ErrExit;
		    			}
		    			if(sPrdtacid.ac_id_type[0] == '1')/**����**/
		    			{
		    				memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
								ret = Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst, "ac_id=%ld", g_mdm_ac_rel.ac_id);
								if(ret == 100) 
								{
									sprintf(acErrMsg, "���ҿͻ��š�����ʧ��!");
									WRITEMSG
									strcpy(g_pub_tx.reply, "M003");
									goto ErrExit;
								}
								else if(ret) 
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								strcpy(g_jz_super.faccname,g_dd_mst.name);
								/**������Ϣ**/
								ret = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no = '%s'",g_dd_mst.prdt_no);
								if(ret)
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								if(g_dd_parm.cif_type[0] == '1')
								{
									strcpy(g_jz_super.savetype,g_dd_parm.title);
								}
								else
								{
									strcpy(g_jz_super.savetype,"");
								}
								
		    			}
		    			else if(sPrdtacid.ac_id_type[0] == '2')/**����**/
		    			{
		    				memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
								ret = Td_mst_Sel(g_pub_tx.reply, &g_td_mst, "ac_id=%ld", g_mdm_ac_rel.ac_id);
								if(ret == 100) 
								{
									sprintf(acErrMsg, "���ҿͻ��š�����ʧ��!");
									WRITEMSG
									strcpy(g_pub_tx.reply, "M003");
									goto ErrExit;
								}
								else if(ret) 
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								strcpy(g_jz_super.faccname,g_td_mst.name);
								/**������Ϣ**/
								ret = Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no = '%s'",g_td_mst.prdt_no);
								if(ret)
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								if(g_td_parm.cif_type[0] == '1')
								{
									strcpy(g_jz_super.savetype,g_td_parm.title);
								}
								else
								{
									strcpy(g_jz_super.savetype,"");
								}
		    			}
		    			else if(sPrdtacid.ac_id_type[0] == '3')/**����**/
		    			{
		    				memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
								ret = Ln_mst_Sel(g_pub_tx.reply, &g_ln_mst, "ac_id=%ld", g_mdm_ac_rel.ac_id);
								if(ret == 100) 
								{
									sprintf(acErrMsg, "���ҿͻ��š�����ʧ��!");
									WRITEMSG
									strcpy(g_pub_tx.reply, "M003");
									goto ErrExit;
								}
								else if(ret) 
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								strcpy(g_jz_super.faccname,g_ln_mst.name);
		    			}
		    			else if(sPrdtacid.ac_id_type[0] == '9')/**�ڲ�**/
		    			{
		    				memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
								ret = In_mst_Sel(g_pub_tx.reply, &g_in_mst, "ac_id=%ld", g_mdm_ac_rel.ac_id);
								if(ret == 100) 
								{
									sprintf(acErrMsg, "���ҿͻ��š�����ʧ��!");
									WRITEMSG
									strcpy(g_pub_tx.reply, "M003");
									goto ErrExit;
								}
								else if(ret) 
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								strcpy(g_jz_super.faccname,g_in_mst.name);
		    			}
		    			else
		    			{
		    				vtcp_log("[%s][%d],�˻����ʹ��� ac_id_type=[%s]",__FILE__,__LINE__,sPrdtacid.ac_id_type);
								strcpy(g_pub_tx.reply, "P410");
								goto ErrExit;
		    			}
						}
						else
						{
		    			vtcp_log("[%s][%d] ac_no=[%s] \n",__FILE__,__LINE__,g_trace_log.ac_no);
							/**strcpy(g_pub_tx.reply,"D103");
							goto ErrExit;**/
						}
						/*find faccbank*/
						if(strlen(g_trace_log.opn_br_no) == 5)/**�������� 5λ�����к�**/
						{
							memset(&g_com_branch,0x00,sizeof(struct com_branch_c));
							ret = Com_branch_Sel(g_pub_tx.reply, &g_com_branch, "br_no='%s'", g_trace_log.opn_br_no);
							if(ret == 100) 
							{
								sprintf(acErrMsg, "Com_branch_Selʧ��!");
								WRITEMSG
								strcpy(g_pub_tx.reply, "M003");
								goto ErrExit;
							}
							else if(ret) 
							{
									sprintf(acErrMsg, "����ʧ��! [%d]", ret);
									WRITEMSG
									strcpy(g_pub_tx.reply, "D103");
									goto ErrExit;
							}
							vtcp_log("[%s][%d] zylseeee1",__FILE__,__LINE__);
							strcpy(g_jz_super.faccbank,g_com_branch.br_name);
						}
						else
						{
		    			vtcp_log("[%s][%d] opn_br_no=[%s] \n",__FILE__,__LINE__,g_trace_log.opn_br_no);
							strcpy(g_pub_tx.reply,"D103");
							goto ErrExit;
						}
						/**���� accountname ֤���� idcode �����˵� mdm_ac_rel.note_no **/
						strcpy(g_jz_super.accountname,g_jz_super.faccname);
						strcpy(g_jz_super.idcode,g_mdm_ac_rel.note_no);
						/**�տ�����Ϣ ������**/

						strcpy(g_jz_super.saccno,"");
						strcpy(g_jz_super.saccname,"");
						strcpy(g_jz_super.saccbank,"");
						vtcp_log("[%s][%d] zylseeee1.2",__FILE__,__LINE__);
					}
					else if(ret)
					{
		    		vtcp_log("[%s][%d],Trace_log_Sel=[%d]\n",__FILE__,__LINE__,ret);
						strcpy(g_pub_tx.reply,"D103");
						goto ErrExit;
					}
					else
					{
						memset(Tmpbuf,0x00,sizeof(Tmpbuf));
						sprintf(Tmpbuf,"%17.2f",sTrace_log.amt);
						strcpy(g_jz_super.tranamt,Tmpbuf);
						vtcp_log("[%s][%d] zylseeee1.3",__FILE__,__LINE__);
						strcpy(g_jz_super.mondyb,pub_rpt_pcask(sTrace_log.amt));/**��д**/
						/**��������Ϣ**/
						strcpy(g_jz_super.faccno,g_trace_log.ac_no);
						/*find Faccname*/
						if((strlen(g_trace_log.ac_no)==10 && g_trace_log.ac_no[0]=='5') ||strlen(g_trace_log.ac_no)==13 || strlen(g_trace_log.ac_no)==19)
						{
							memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
							ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no = '%s'",g_trace_log.ac_no);
		    			if(ret)
		    			{
		    				vtcp_log("[%s][%d],Mdm_ac_rel_Sel ret=[%d]\n",__FILE__,__LINE__,ret);
								strcpy(g_pub_tx.reply,"D103");
								goto ErrExit;
		    			}
							ret = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdtacid,"ac_id=%ld",g_mdm_ac_rel.ac_id);
		    			if(ret)
		    			{
		    				vtcp_log("[%s][%d],Prdt_ac_id_Sel ret=[%d]\n",__FILE__,__LINE__,ret);
								strcpy(g_pub_tx.reply,"D103");
								goto ErrExit;
		    			}
		    			if(sPrdtacid.ac_id_type[0] == '1')/**����**/
		    			{
		    				memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
								ret = Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst, "ac_id=%ld", g_mdm_ac_rel.ac_id);
								if(ret == 100) 
								{
									sprintf(acErrMsg, "���ҿͻ��š�����ʧ��!");
									WRITEMSG
									strcpy(g_pub_tx.reply, "M003");
									goto ErrExit;
								}
								else if(ret) 
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								strcpy(g_jz_super.faccname,g_dd_mst.name);
								/**������Ϣ**/
								ret = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no = '%s'",g_dd_mst.prdt_no);
								if(ret)
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								if(g_dd_parm.cif_type[0] == '1')
								{
									strcpy(g_jz_super.savetype,g_dd_parm.title);
								}
								else
								{
									strcpy(g_jz_super.savetype,"");
								}
		    			}
		    			else if(sPrdtacid.ac_id_type[0] == '2')/**����**/
		    			{
		    				memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
								ret = Td_mst_Sel(g_pub_tx.reply, &g_td_mst, "ac_id=%ld", g_mdm_ac_rel.ac_id);
								if(ret == 100) 
								{
									sprintf(acErrMsg, "���ҿͻ��š�����ʧ��!");
									WRITEMSG
									strcpy(g_pub_tx.reply, "M003");
									goto ErrExit;
								}
								else if(ret) 
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								strcpy(g_jz_super.faccname,g_td_mst.name);
								/**������Ϣ**/
								ret = Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no = '%s'",g_td_mst.prdt_no);
								if(ret)
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								if(g_td_parm.cif_type[0] == '1')
								{
									strcpy(g_jz_super.savetype,g_td_parm.title);
								}
								else
								{
									strcpy(g_jz_super.savetype,"");
								}
		    			}
		    			else if(sPrdtacid.ac_id_type[0] == '3')/**����**/
		    			{
		    				memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
								ret = Ln_mst_Sel(g_pub_tx.reply, &g_ln_mst, "ac_id=%ld", g_mdm_ac_rel.ac_id);
								if(ret == 100) 
								{
									sprintf(acErrMsg, "���ҿͻ��š�����ʧ��!");
									WRITEMSG
									strcpy(g_pub_tx.reply, "M003");
									goto ErrExit;
								}
								else if(ret) 
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								strcpy(g_jz_super.faccname,g_ln_mst.name);
		    			}
		    			else if(sPrdtacid.ac_id_type[0] == '9')/**�ڲ�**/
		    			{
		    				memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
								ret = In_mst_Sel(g_pub_tx.reply, &g_in_mst, "ac_id=%ld", g_mdm_ac_rel.ac_id);
								if(ret == 100) 
								{
									sprintf(acErrMsg, "���ҿͻ��š�����ʧ��!");
									WRITEMSG
									strcpy(g_pub_tx.reply, "M003");
									goto ErrExit;
								}
								else if(ret) 
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								strcpy(g_jz_super.faccname,g_in_mst.name);
		    			}
		    			else
		    			{
		    				vtcp_log("[%s][%d],�˻����ʹ��� ac_id_type=[%s]",__FILE__,__LINE__,sPrdtacid.ac_id_type);
								strcpy(g_pub_tx.reply, "P410");
								goto ErrExit;
		    			}
						}
						else
						{
		    			vtcp_log("[%s][%d] ac_no=[%s] \n",__FILE__,__LINE__,g_trace_log.ac_no);
							/**strcpy(g_pub_tx.reply,"D103");
							goto ErrExit;**/
						}
						/*find Faccbank*/
						if(strlen(g_trace_log.opn_br_no) == 5)/**�������� 5λ�����к�**/
						{
							memset(&g_com_branch,0x00,sizeof(struct com_branch_c));
							ret = Com_branch_Sel(g_pub_tx.reply, &g_com_branch, "br_no='%s'", g_trace_log.opn_br_no);
							if(ret == 100) 
							{
								sprintf(acErrMsg, "Com_branch_Selʧ��!");
								WRITEMSG
								strcpy(g_pub_tx.reply, "M003");
								goto ErrExit;
							}
							else if(ret) 
							{
									sprintf(acErrMsg, "����ʧ��! [%d]", ret);
									WRITEMSG
									strcpy(g_pub_tx.reply, "D103");
									goto ErrExit;
							}
							strcpy(g_jz_super.faccbank,g_com_branch.br_name);
						}
						else
						{
		    			vtcp_log("[%s][%d] opn_br_no=[%s] \n",__FILE__,__LINE__,g_trace_log.opn_br_no);
							strcpy(g_pub_tx.reply,"D103");
							goto ErrExit;
						}
						/**���� accountname ֤���� idcode �����˵� mdm_ac_rel.note_no **/
						strcpy(g_jz_super.accountname,g_jz_super.faccname);
						strcpy(g_jz_super.idcode,g_mdm_ac_rel.note_no);
						/**�տ�����Ϣ**/
						strcpy(g_jz_super.saccno,sTrace_log.ac_no);
						/*find Saccname*/
						if((strlen(g_trace_log.ac_no)==10 && g_trace_log.ac_no[0]=='5') ||strlen(sTrace_log.ac_no)==13 || strlen(sTrace_log.ac_no)==19)
						{
							memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
							ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no = '%s'",sTrace_log.ac_no);
		    			if(ret)
		    			{
		    				vtcp_log("[%s][%d],Mdm_ac_rel_Sel ret=[%d]\n",__FILE__,__LINE__,ret);
								strcpy(g_pub_tx.reply,"D103");
								goto ErrExit;
		    			}
							ret = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdtacid,"ac_id=%ld",g_mdm_ac_rel.ac_id);
		    			if(ret)
		    			{
		    				vtcp_log("[%s][%d],Prdt_ac_id_Sel ret=[%d]\n",__FILE__,__LINE__,ret);
								strcpy(g_pub_tx.reply,"D103");
								goto ErrExit;
		    			}
		    			if(sPrdtacid.ac_id_type[0] == '1')/**����**/
		    			{
		    				memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
								ret = Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst, "ac_id=%ld", g_mdm_ac_rel.ac_id);
								if(ret == 100) 
								{
									sprintf(acErrMsg, "���ҿͻ��š�����ʧ��!");
									WRITEMSG
									strcpy(g_pub_tx.reply, "M003");
									goto ErrExit;
								}
								else if(ret) 
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								strcpy(g_jz_super.saccname,g_dd_mst.name);
		    			}
		    			else if(sPrdtacid.ac_id_type[0] == '2')/**����**/
		    			{
		    				memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
								ret = Td_mst_Sel(g_pub_tx.reply, &g_td_mst, "ac_id=%ld", g_mdm_ac_rel.ac_id);
								if(ret == 100) 
								{
									sprintf(acErrMsg, "���ҿͻ��š�����ʧ��!");
									WRITEMSG
									strcpy(g_pub_tx.reply, "M003");
									goto ErrExit;
								}
								else if(ret) 
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								strcpy(g_jz_super.saccname,g_td_mst.name);
		    			}
		    			else if(sPrdtacid.ac_id_type[0] == '3')/**����**/
		    			{
		    				memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
								ret = Ln_mst_Sel(g_pub_tx.reply, &g_ln_mst, "ac_id=%ld", g_mdm_ac_rel.ac_id);
								if(ret == 100) 
								{
									sprintf(acErrMsg, "���ҿͻ��š�����ʧ��!");
									WRITEMSG
									strcpy(g_pub_tx.reply, "M003");
									goto ErrExit;
								}
								else if(ret) 
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								strcpy(g_jz_super.saccname,g_ln_mst.name);
		    			}
		    			else if(sPrdtacid.ac_id_type[0] == '9')/**�ڲ�**/
		    			{
		    				memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
								ret = In_mst_Sel(g_pub_tx.reply, &g_in_mst, "ac_id=%ld", g_mdm_ac_rel.ac_id);
								if(ret == 100) 
								{
									sprintf(acErrMsg, "���ҿͻ��š�����ʧ��!");
									WRITEMSG
									strcpy(g_pub_tx.reply, "M003");
									goto ErrExit;
								}
								else if(ret) 
								{
										sprintf(acErrMsg, "����ʧ��! [%d]", ret);
										WRITEMSG
										strcpy(g_pub_tx.reply, "D103");
										goto ErrExit;
								}
								strcpy(g_jz_super.saccname,g_in_mst.name);
		    			}
		    			else
		    			{
		    				vtcp_log("[%s][%d],�˻����ʹ��� ac_id_type=[%s]",__FILE__,__LINE__,sPrdtacid.ac_id_type);
								strcpy(g_pub_tx.reply, "P410");
								goto ErrExit;
		    			}
						}
						else
						{
		    			vtcp_log("[%s][%d] ac_no=[%s] \n",__FILE__,__LINE__,sTrace_log.ac_no);
							/**strcpy(g_pub_tx.reply,"D103");
							goto ErrExit;
							**/
						}
						/*find Saccbank*/
						if(strlen(sTrace_log.opn_br_no) == 5)/**�������� 5λ�����к�**/
						{
							memset(&g_com_branch,0x00,sizeof(struct com_branch_c));
							ret = Com_branch_Sel(g_pub_tx.reply, &g_com_branch, "br_no='%s'", sTrace_log.opn_br_no);
							if(ret == 100) 
							{
								sprintf(acErrMsg, "Com_branch_Selʧ��!");
								WRITEMSG
								strcpy(g_pub_tx.reply, "M003");
								goto ErrExit;
							}
							else if(ret) 
							{
									sprintf(acErrMsg, "����ʧ��! [%d]", ret);
									WRITEMSG
									strcpy(g_pub_tx.reply, "D103");
									goto ErrExit;
							}
							strcpy(g_jz_super.saccbank,g_com_branch.br_name);
						}
						else
						{
		    			vtcp_log("[%s][%d] opn_br_no=[%s] \n",__FILE__,__LINE__,sTrace_log.opn_br_no);
							strcpy(g_pub_tx.reply,"D103");
							goto ErrExit;
						}
						
					}
				}
				/**����ȡ����������ϳ� superstr**/
				vtcp_log("[%s][%d] zylseeee1.25",__FILE__,__LINE__);
				sprintf(superstr,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",g_jz_super.uuid,g_jz_super.trandate,g_jz_super.areano,g_jz_super.tellerid,g_jz_super.serseqno,g_jz_super.ywcode,g_jz_super.tranamt,g_jz_super.faccno,g_jz_super.faccname,g_jz_super.faccbank,g_jz_super.saccno,g_jz_super.saccname,g_jz_super.saccbank,g_jz_super.mondyb,g_jz_super.accountname,g_jz_super.idcode,g_jz_super.savetype,g_jz_super.businesssort,g_jz_super.dealmode,g_jz_super.warrantname,g_jz_super.unit,g_jz_super.count,g_jz_super.startno,g_jz_super.endno,g_jz_super.flatcost,g_jz_super.poundage,g_jz_super.accode,g_jz_super.isup,g_jz_super.state);
				vtcp_log("[%s][%d] [%s]",__FILE__,__LINE__,superstr);
				/**��superstrд���ļ� /home/devht/data/filename filenamesȡ����,ʱ��,jz_super���**/
				Trace_log_Clo_Upd();
				fputs(superstr,fp);
				vtcp_log("[%s][%d] zylseeee1.27",__FILE__,__LINE__);
		}
		Trace_log_Clo_Sel();
		
		vtcp_log("[%s][%d] zylseeee1.28",__FILE__,__LINE__);
		/**ѭ����ȡ���� �����һ�� trace_log.tx_date trace_log.trace_no filename д�������ļ�**/
		/**�����ļ�β,д��¼**/
		fseek(pz,0l,2);
		memset(Tmpbuf,0x00,sizeof(Tmpbuf));
		
		sprintf(Tmpbuf,"%08d|%08d|%s\n",tmpdate,tmpno,filename);
		fputs(Tmpbuf,pz);
		vtcp_log("[%s][%d] zylseeee1.29",__FILE__,__LINE__);
	}
	else
	{
		vtcp_log("[%s][%d]�ļ���ȡʧ��..",__FILE__,__LINE__ );
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	
	fclose(fp);
	fclose(pz);	
	
OkExit:
	sql_commit();
  strcpy(g_pub_tx.reply,"0000");
  vtcp_log("������ȡ�ɹ�! " );
  set_zd_data("0120",g_pub_tx.reply);
  return 0;
    
ErrExit:
	sql_rollback();
  vtcp_log("������ȡʧ�� [%s]",g_pub_tx.reply);
  set_zd_data("0120",g_pub_tx.reply);
  return 1;
}

/** pubf_base_file.c���Ѿ�����
pub_base_cut_str( char *sstr,char *dstr,char *mode,int num )
**/
