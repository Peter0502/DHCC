/*************************************************
* �� �� ��:  gD500.c
* ���������� ������ϸ���˵���ϸ��Ϣ��ѯ 
*
* ��    ��:  wuhaifan
* ������ڣ� 2014��10��29�� 
*
* �޸ļ�¼�� 
* ��   ��:	
* �� �� ��:
* �޸�����:
*************************************************/
#define EXTERN
#include "public.h"
#include <stdio.h>
#include <string.h>
#include "dd_mst_hst_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "com_sys_parm_c.h"
#include "netbank_yqdz_c.h"


extern char *get_env_info(char *infoname);
gD500()
{
	struct mdm_ac_rel_c sMdm_ac_rel;   	/*�������˻���ϵ*/
	struct dd_mst_hst_c vdd_mst_hst;  	 /*���ڴ����ϸ*/
	struct dd_mst_c g_dd_mst;		/*���ڴ�����ļ�*/
	struct com_sys_parm_c d_com_sys_parm;	/*����ϵͳ����*/
	struct netbank_yqdz_c d_netbank_yqdz;	/*����������˱�*/
	
	memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
	memset(&vdd_mst_hst,0x00,sizeof(vdd_mst_hst));
	memset(&g_dd_mst,0x00,sizeof(g_dd_mst));
	memset(&d_com_sys_parm,0x00,sizeof(d_com_sys_parm));
	memset(&d_netbank_yqdz,0x00,sizeof(d_netbank_yqdz));
	/*
	vtcp_log("%s %d::vdd_mst_hst.bal=[%f] error ",__FILE__,__LINE__,vdd_mst_hst.bal);
	*/
	long t_date=0;	/*����ϵͳ����-ϵͳ����*/
	long t_tmp=0;
	char tmp[7]={0};
	
	char filename[100];
	char pcfilename[100];
	char cFtpfile[100];
	char total[1024];
	
	double beginBalance=0.00;	/*��ǰ���*/
	double endBalance=0.00;	/*�к����*/
	char oppositeAccount[32+1];	/*�Է��˺�*/
	char oppositeAccountName[60+1];	/*�Է��˻���*/	
	FILE *fp = NULL;
	int cnt;	/*������*/
	
	memset(filename,0x00,sizeof(filename));
	memset(pcfilename,0x00,sizeof(pcfilename));
	memset(cFtpfile,0x00,sizeof(cFtpfile));
	memset(oppositeAccount,0x00,sizeof(oppositeAccount));
	memset(oppositeAccountName,0x00,sizeof(oppositeAccountName));
	memset(total,0x00,sizeof(total));
	
	int ret = -1;

	ret=sql_begin(); /*������*/
	if( ret ) 
	{
		vtcp_log("[%s],[%d] ������ʧ��!!!",__LINE__,__FILE__);
		goto ErrExit;
	}

    	/**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();
	
	/*ȡ��ǰ����:����ϵͳ������*/
	ret = -1;
	ret=Com_sys_parm_Sel( g_pub_tx.reply,&d_com_sys_parm, "1=1" );
	if(ret)
	{
		vtcp_log("%s %d::Com_sys_parm_Sel [%d] error ",__FILE__,__LINE__,ret);
	 	strcpy(g_pub_tx.reply,"O072");
	 	goto ErrExit;	
	}
	
	t_tmp=d_com_sys_parm.lst_date;
	vtcp_log("%s %d::t_date = [%ld]",__FILE__,__LINE__,t_tmp);
	t_date=t_tmp/100;
	
	/*�����ļ�*/
	sprintf( filename,"CorBillSyn%06d.TXT",t_date);
	sprintf( pcfilename,"%s/%s",getenv("FILDIR"),filename );
	/**pub_base_AllDwnFilName(filename);**/
	fp = fopen(pcfilename, "w");
	if (fp == NULL)
	{
		strcpy(g_pub_tx.reply, "P157");
		set_zd_data("0130","�����ļ�����");
		vtcp_log("[%s],[%d] open file [%s] error",__LINE__,__FILE__,pcfilename);
		goto ErrExit;
	}
	vtcp_log("[%s],[%d],����ȫ·���ļ���Ϊ[%s],�ļ���Ϊ[%s]", __FILE__, __LINE__, pcfilename,filename);
	/*�ļ�ͷ*/
	/**fprintf(fp, "�˺�|�˻�����|������ˮ��|��������|�������|���׽��|�˻����|�Է��˺�|�Է��˻���|ժҪ|��ǰ���|�к���|\n");**/
	
	/*��ѯ�����˺�:����������˱�*/
	ret = -1;
	ret=Netbank_yqdz_Dec_Sel(g_pub_tx.reply,"sts='0'");
	if(ret)
	{
		vtcp_log("%s %d::Netbank_yqdz_Dec_Sel [%d] error ",__FILE__,__LINE__,ret);
	 	strcpy(g_pub_tx.reply,"D102");
	 	set_zd_data("0130","���α����");
	 	fclose(fp);
	 	goto ErrExit;	
	}

	while(1)
	{
		cnt = 1;/*ÿһ���˺ŵĽ�������*/
		beginBalance=0.00;
		endBalance=0.00;
		
		memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
		memset(&vdd_mst_hst,0x00,sizeof(vdd_mst_hst));
		memset(&g_dd_mst,0x00,sizeof(g_dd_mst));
		memset(&d_netbank_yqdz,0x00,sizeof(d_netbank_yqdz));
		
		ret = -1;
		ret=Netbank_yqdz_Fet_Sel(&d_netbank_yqdz,g_pub_tx.reply);
		if(ret == 1403 || ret == 100)
		{
			vtcp_log("%s %d::������Ҫ��ѯ���˺�[%d]",__FILE__,__LINE__,ret);
			break;	
		}
		 else if(0 != ret && 100 != ret)
		{
			vtcp_log("%s %d::Netbank_yqdz_Fet_Sel error[%d]",__FILE__,__LINE__,ret);
			strcpy(g_pub_tx.reply,"D001");
			set_zd_data("0130","��ѯ����");
			Netbank_yqdz_Clo_Sel();
			fclose(fp);
			goto ErrExit;
		}
		
		/*��ѯ�˺�*/
		ret = -1;
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_id='%ld'",d_netbank_yqdz.ac_id);
		if(ret)
		{
			vtcp_log("%s %d::Mdm_ac_rel_Sel [%d] error ",__FILE__,__LINE__,ret);
		 	strcpy(g_pub_tx.reply,"W011");
		 	Netbank_yqdz_Clo_Sel();
			fclose(fp);
		 	goto ErrExit;	
		}
		
		/**��ѯ�Է��˺źͶԷ��˻�����**/
		strcpy(oppositeAccount,"����");
		strcpy(oppositeAccountName,"����");
		
		/*��ѯ���ڴ�����ļ�*/
		ret = -1;
		ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id='%ld'",d_netbank_yqdz.ac_id);
		if(ret)
		{
			vtcp_log("%s %d::Dd_mst_Sel [%d] error ",__FILE__,__LINE__,ret);
		 	strcpy(g_pub_tx.reply,"AT06");
		 	set_zd_data("0130","��ѯ���ڴ�����ļ�����");
		 	Netbank_yqdz_Clo_Sel();
			fclose(fp);
		 	goto ErrExit;	
		}
		
		int count=0;
		count=sql_count("dd_mst_hst","ac_id='%ld' and substr(tx_date,0,6)='%d' ",d_netbank_yqdz.ac_id,t_date);	
		if(count < 0)
		{
			vtcp_log("%s %d::sql_count [%d] error ",__FILE__,__LINE__,count);
		 	strcpy(g_pub_tx.reply,"D101");	
		 	Netbank_yqdz_Clo_Sel();
			fclose(fp);
		 	goto ErrExit;
		}
		if(count == 0)
		{
		 	/*��ǰ���:��ѯ���ڴ����ϸ����ǰ�����һ�ʽ��ף���ǰ���=���*/
		 	memset(&vdd_mst_hst,0x00,sizeof(vdd_mst_hst));
			ret = -1;
			ret=Dd_mst_hst_Sel(g_pub_tx.reply,&vdd_mst_hst,"ac_id='%ld' and substr(tx_date,0,6)<'%d' order by hst_cnt desc",d_netbank_yqdz.ac_id,t_date);
			if(ret == 100 || ret == 1403)
			{
			 	vtcp_log("%s %d::����Ӧ��¼ ",__FILE__,__LINE__);	
			}
			else if(ret)
			{
				vtcp_log("%s %d::Dd_mst_hst_Sel [%d] error ",__FILE__,__LINE__,ret);
			 	strcpy(g_pub_tx.reply,"W050");
			 	Netbank_yqdz_Clo_Sel();
				fclose(fp);
			 	goto ErrExit;	
			}
			
			vtcp_log("%s %d::vdd_mst_hst.bal='%16.2f' ",__FILE__,__LINE__,vdd_mst_hst.bal);
			fprintf(fp, "%20s|%40s|%12s|%8s|%1s|%16s|%16s|%6s|%6s|%20s|%16.2f|%16.2f|\n",\ 
				sMdm_ac_rel.ac_no,g_dd_mst.name,'','','','','',oppositeAccount,oppositeAccountName,'',vdd_mst_hst.bal,vdd_mst_hst.bal);	
		 	vtcp_log("%s %d::���˺ű����޽��� ",__FILE__,__LINE__);	
		 	continue;
			 		
		}
		else
		{
			/*��ǰ���:��ѯ���ڴ����ϸ���µ�һ�ʽ��ף���ǰ���=�������+-���׽��*/
			memset(&vdd_mst_hst,0x00,sizeof(vdd_mst_hst));
			ret = -1;
			ret=Dd_mst_hst_Sel(g_pub_tx.reply,&vdd_mst_hst,"ac_id='%ld' and substr(tx_date,0,6)='%d' order by hst_cnt asc",d_netbank_yqdz.ac_id,t_date);
			if(ret == 100 || ret == 1403)
			{
				vtcp_log("%s %d::����Ӧ��¼ ",__FILE__,__LINE__);	
			}
			else if(ret)
			{
				vtcp_log("%s %d::Dd_mst_hst_Sel [%s] error ",__FILE__,__LINE__,ret);
			 	strcpy(g_pub_tx.reply,"W050");
			 	Netbank_yqdz_Clo_Sel();
				fclose(fp);
			 	goto ErrExit;	
			}
			
			/*add_ind������־��0���� 1����*/
			if(0 == memcmp(vdd_mst_hst.add_ind,"0",1))
			{
				beginBalance=vdd_mst_hst.bal+vdd_mst_hst.tx_amt;	
			}
			else
			{
				beginBalance=vdd_mst_hst.bal-vdd_mst_hst.tx_amt;	
			}	
		}
		
		/*��ѯ���ڴ����ϸ*/
		ret = -1;
		ret=Dd_mst_hst_Dec_Sel(g_pub_tx.reply,"ac_id='%ld' and substr(tx_date,0,6)='%d' order by hst_cnt desc",d_netbank_yqdz.ac_id,t_date);
		if(ret)
		{
			vtcp_log("%s %d::Dd_mst_hst_Dec_Sel [%d] error ",__FILE__,__LINE__,ret);
		 	strcpy(g_pub_tx.reply,"T026");
		 	set_zd_data("0130","���α����");
		 	Netbank_yqdz_Clo_Sel();
		 	fclose(fp);
		 	goto ErrExit;	
		}

		while(1)
		{
			memset(&vdd_mst_hst,0x00,sizeof(vdd_mst_hst));	
			ret = -1;
			ret=Dd_mst_hst_Fet_Sel(&vdd_mst_hst,g_pub_tx.reply);
			if(ret == 1403 || ret == 100)
			{
				vtcp_log("%s %d::������Ҫ��ѯ�ļ�¼[%d]",__FILE__,__LINE__,ret);
				break;	
			}
			else if(0 != ret && 100 != ret)
			{
				vtcp_log("%s %d::Dd_mst_hst_Fet_Sel error[%d]",__FILE__,__LINE__,ret);
				strcpy(g_pub_tx.reply,"D001");
				set_zd_data("0130","��ѯ����");
				Netbank_yqdz_Clo_Sel();
				Dd_mst_hst_Clo_Sel();
				fclose(fp);
				goto ErrExit;
			}
			
			/*�к����:��ѯ���ڴ����ϸ�������һ�ʽ��ף���ǰ���=�������+-���׽��*/
			if(cnt == 1)
			{
				endBalance=vdd_mst_hst.bal;
			}
			cnt++;
			
			fprintf(fp, "%20s|%40s|%12d|%08d|%1s|%16.2f|%16.2f|%6s|%6s|%20s|%16.2f|%16.2f|\n",\ 
				sMdm_ac_rel.ac_no,g_dd_mst.name,vdd_mst_hst.trace_no,vdd_mst_hst.tx_date,\
				vdd_mst_hst.add_ind,vdd_mst_hst.tx_amt,vdd_mst_hst.bal,oppositeAccount,\
				oppositeAccountName,vdd_mst_hst.brf,beginBalance,endBalance);
		 
			/**vtcp_log("��ʼ��ӡ��%s|%s|%d|%d|%s|%16.2f|%16.2f|%s|%s|%s|%16.2f|%16.2f|\n",\ 
				sMdm_ac_rel.ac_no,g_dd_mst.name,vdd_mst_hst.trace_no,vdd_mst_hst.tx_date,\
				vdd_mst_hst.add_ind,vdd_mst_hst.tx_amt,vdd_mst_hst.bal,oppositeAccount,\
				oppositeAccountName,vdd_mst_hst.brf,beginBalance,endBalance);**/
		}

		Dd_mst_hst_Clo_Sel();	
			
	}	

	Netbank_yqdz_Clo_Sel();
	fclose(fp);

	/*ftp�����ļ�*/
	ret = -1;	
	sprintf(cFtpfile,"%s/bin/ftp_to_wyzt.sh %s",getenv("WORKDIR"),filename);
	vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
	ret=system(cFtpfile);
	if (ret)
	{
		vtcp_log("[%s][%d] ftp�ļ�[%s]��������̨����[%d]", __FILE__, __LINE__,filename,ret);
		strcpy(g_pub_tx.reply,"9999");
		set_zd_data("0130"," ftp�����ļ���������̨����");
		goto ErrExit;
	}
	
	vtcp_log("[%s][%d] ftp�ļ�[%s]��������̨�ɹ�", __FILE__, __LINE__,filename);
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data("0120","0000");
	set_zd_data("0130"," ������ϸ���˵���ϸ��Ϣ��ѯ�ɹ���");
	vtcp_log("success !!: gD500 reply is[%s]", g_pub_tx.reply);
	return 0;
ErrExit:
	vtcp_log("fail !!: reply is[%s]", g_pub_tx.reply);
	return 1;
}
