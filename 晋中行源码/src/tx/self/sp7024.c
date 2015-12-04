/**********************************************************************************
�������ƣ��������˽���sp7024.c	 
����������ֻ����ת���࣬�������ֽ���
�����ļ���hvps_check_��������_��ˮ��.txt
�ļ���ʽ��
	���|Ψһ��ʶ|ϵͳ���|��������|��������|ί������|֧���������|ƽ̨����|ƽ̨��ˮ|�������к�|������ʶ|���Ʊ�ʶ|���׽��|�����־|���˺�|���˻���|�ڶ��˺�|ƾ֤����|ƾ֤����|���׻���|����Ա��|���ļ�������|���ļ�����ˮ|���˱�ʶ|������ʶ|������|�����ѱ�ʶ|�����ѳ���|�����������Ŀ|

�����Ķ��ʽ���ļ�����txtĿ¼�� hvps_check_��������_��ˮ��.ret
���˽���ļ���ʽ��
	��ϸ���|Ψһ��ʶ|ϵͳ���|��������|���˱�ʶ|�������˽��|���ļ�������|���ļ�����ˮ|�����˺�|���˻���|����ԭ��|

insert into tx_def values('7024','��������','10000000000000000000000000000000000000000000000000000000000000000001000000000000010000000000000000000000000000000000000000000100','1','1','2','3','');
insert into tx_flow_def values('7024','0','7024','#$');
insert into tx_sub_def values('7024','��������','sp7024','0','0');
***********************************************************************************/

#define DEWZGD "41101"
#define XEWZGD "41102"
#define DELZGD "41201"
#define XELZGD "41202"

#include <errno.h>
#include "public.h"
#include "com_sys_parm_c.h"
#include "chnl_batch_acct_c.h"
#include "chnl_single_cnaps_c.h"
#include "chnl_compare_acct_c.h"

static int compare_info_acct(struct chnl_compare_acct_c stCompare,char *gz_br_no,char *ac_no,char *gz_reason);
static int compare_exit_acct(struct chnl_compare_acct_c stCompare);

sp7024()
{
	int i = 0;
	int	j = 0;
	int	k = 0;
	int	l = 0;
	int iRet = 0;
	long	iCount = 0;
	long	pt_date = 0;
	long	lCom_swno = 0;
	long	lTot_num = 0;	/*�ǼǶ��˵ǼǱ����ܱ���*/
	long	lFact_num = 0;	/*�����ܱ���*/
	long	lCount_num = 0;	/*������ܱ���*/
	char	sBrf[21] = {0};
	char	sCmd[50] = {0};
	char	sSeqno[11];
	char	sAc_no[33] = {0};	/*���˺�*/
	char	sGz_br_no[6] = {0};	/*���˻���*/
	char	sGz_reason[2] = {0};	/*����ԭ��*/
	char	sSysCode[5] = {0};
	char	sTmpstr[1024] = {0};
	char	sCheckDate[9] = {0};
	char	sCom_result[11] = {0};
	char	sFilename[60] = {0};
	char	sAll_filename[248] = {0};/***ȫ·��**/
	char	sFilename_rt[400] = {0};
	char	sFilename_ret[400] = {0};
	char	sFldstr[30][100] = {0};
	struct com_sys_parm_c com_sys_parm = {0};
	struct chnl_batch_acct_c stBatch = {0};	/*�������˵Ǽǲ�*/
	struct chnl_single_cnaps_c stSingle = {0};	/*���ʼ��˵Ǽǲ�*/
	struct chnl_compare_acct_c stCompare = {0};	/*���˵Ǽǲ�*/
	struct chnl_compare_acct_c stCompareQ = {0};	/*���˼��˵Ǽǲ�*/
	char *p=NULL;
	FILE *fp=NULL;
	FILE *new_fp=NULL;
	
	pub_base_sysinit();
	
	vtcp_log("[%s],[%d],���ڡ���ˮ,tx_date=[%d]trace_no=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date,g_pub_tx.trace_no);
	vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
	set_zd_long("0460",g_pub_tx.tx_date);
	
	/********��ȡƽ̨����***************/
	get_zd_data("0100",sSysCode); 	/*ϵͳ���*/
	get_zd_long("0440",&iCount);	/****��������*****/
	sprintf(sCheckDate,"%d",iCount);
	vtcp_log("[%s][%d]�������� sCheckDate=[%s]",__FILE__,__LINE__,sCheckDate);
	get_zd_long("0450",&pt_date);/****ƽ̨����*****/
	vtcp_log("[%s][%d]ƽ̨���� pt_date=[%d]",__FILE__,__LINE__,pt_date);
	get_zd_long("0480",&lCount_num);/****�ܱ���*****/
	vtcp_log("[%s][%d]�ܱ��� lCount_num=[%d]",__FILE__,__LINE__,lCount_num);
	get_zd_long("0520", &lCom_swno);
	vtcp_log("[%s][%d]ƽ̨��ˮ lCom_swno=[%d]",__FILE__,__LINE__,lCom_swno);
	get_zd_data("0810",sBrf);
	pub_base_strpack(sBrf);
	get_zd_data("0830",sFilename);/**�����ļ���***/
	
	iRet = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm,"1=1");
	if(iRet){
		sprintf(acErrMsg," ��ѯϵͳ���ڴ��� ");
		WRITEMSG
		goto ErrExit;
	}
	/*if(pt_date != com_sys_parm.sys_date)
	{
		sprintf(acErrMsg," ����������������ڲ����!!ƽ̨��%ld�� ���ġ�%ld�� ",pt_date,com_sys_parm.sys_date);
		WRITEMSG
		goto ErrExit;
	}*/
	/******���ļ�������ļ��Ƿ����������ļ����ݲ�����˵ǼǱ�*******/
	pub_base_strpack(sFilename);
	pub_base_strpack(sAll_filename);
	sprintf(sAll_filename,"%s/txt/%s",getenv("HOME"),sFilename);
	vtcp_log("sFilename[%s][%s][%d]",sFilename,__FILE__,__LINE__);
	vtcp_log("sAll_filename[%s][%s][%d]",sAll_filename,__FILE__,__LINE__);
	fp = fopen(sAll_filename,"r");
	if (fp == NULL)
	{
		sprintf(acErrMsg," ���ļ����� [%s]  ",sFilename);
		strcpy(g_pub_tx.reply,"S047");
		goto ErrExit;
	}
	while (1){
		memset(sTmpstr,0x0,sizeof(sTmpstr));
		p = fgets(sTmpstr,1024,fp);/*ȡ��һ����¼*/
		if (feof(fp))
		{
			vtcp_log("[%s],[%d]�ļ�������ȡ���ˣ�",__FILE__,__LINE__);
			break;
		}
		pub_base_strpack_all(sTmpstr);
		vtcp_log("wkwk==========1");
		memset(sFldstr,0x0,sizeof(sFldstr));
		for (i = 0; i < 30; i++)
		{
			iRet = pub_base_cut_str(sTmpstr,sFldstr[i],"|",i+1);
			vtcp_log("[%d] [%s]",i+1,sFldstr[i]);
			pub_base_strpack_all(sFldstr[i]);
		}
		/**********************************************************
		sFldstr[0] ��ϸ���          sFldstr[15] ���˺Ż���
		sFldstr[1] Ψһ��ʶ          sFldstr[16] �ڶ��˺�
		sFldstr[2] ϵͳ���          sFldstr[17] ƾ֤����
		sFldstr[3] ��������          sFldstr[18] ƾ֤����
		sFldstr[4] ��������          sFldstr[19] ���׻���
		sFldstr[5] ί������          sFldstr[20] ����Ա��
		sFldstr[6] ֧�����          sFldstr[21] ��������
		sFldstr[7] ƽ̨����          sFldstr[22] ������ˮ
		sFldstr[8] ƽ̨��ˮ          sFldstr[23] ���˱�ʾ  ��ʱ����
		sFldstr[9] �������к�        sFldstr[24] ������ʶ
		sFldstr[10] ������ʾ         sFldstr[25] �շѱ�ʶ
		sFldstr[11] ���׽��         sFldstr[26] �����ѱ�ʶ
		sFldstr[12] ���Ʊ�ʶ         sFldstr[27] �����ѽ��
		sFldstr[13] �����ʾ         sFldstr[28] �����ѳ���
		sFldstr[14] ���˺�           sFldstr[29] �����������Ŀ
		************************************************************/
		iRet = Chnl_compare_acct_Dec_Upd(g_pub_tx.reply,"un_marking = '%s' and "
			"syscode = '%s' and com_date = '%s'",sFldstr[1],sFldstr[2],sFldstr[3]);
		if (iRet){
			vtcp_log("[%s],[%d]�����α����,iRet=[%d]",__FILE__,__LINE__,iRet);
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		iRet = Chnl_compare_acct_Fet_Upd(&stCompare,g_pub_tx.reply);
		if (iRet == 100 || iRet == 1403){
			vtcp_log("[%s],[%d]���ݿ�û�м�¼��ǼǶ��˵ǼǱ�,iRet=��%d��",__FILE__,__LINE__,iRet);
			memset(&stCompare,0x00,sizeof(stCompare));
			stCompare.detail_no = atoi(sFldstr[0]);
			memcpy(stCompare.un_marking,sFldstr[1],strlen(sFldstr[1]));
			memcpy(stCompare.syscode,sFldstr[2],strlen(sFldstr[2]));
			memcpy(stCompare.com_date,sFldstr[3],strlen(sFldstr[3]));
			memcpy(stCompare.code_type,sFldstr[4],strlen(sFldstr[4]));
			memcpy(stCompare.wt_date,sFldstr[5],strlen(sFldstr[5]));
			stCompare.orderno = atoi(sFldstr[6]);
			stCompare.pt_date = atoi(sFldstr[7]);
			stCompare.pt_trace_no = atoi(sFldstr[8]);
			memcpy(stCompare.or_br_no,sFldstr[9],strlen(sFldstr[9]));
			memcpy(stCompare.sndrcvflag,sFldstr[10],strlen(sFldstr[10]));
			stCompare.tx_amt = strtod(sFldstr[11],NULL)/100;
			vtcp_log("stCompare.tx_amt��ֵ��[%.2f][%s][%d]",stCompare.tx_amt,__FILE__,__LINE__);
			memcpy(stCompare.con_marking,sFldstr[12],strlen(sFldstr[12]));
			memcpy(stCompare.bor_lend,sFldstr[13],strlen(sFldstr[13]));
			memcpy(stCompare.main_acc_no,sFldstr[14],strlen(sFldstr[14]));
			memcpy(stCompare.main_acc_nm,sFldstr[15],strlen(sFldstr[15]));
			memcpy(stCompare.snd_acc_no,sFldstr[16],strlen(sFldstr[16]));
			memcpy(stCompare.note_type,sFldstr[17],strlen(sFldstr[17]));
			memcpy(stCompare.note_no,sFldstr[18],strlen(sFldstr[18]));
			memcpy(stCompare.tx_br_no,sFldstr[19],strlen(sFldstr[19]));
			memcpy(stCompare.teller,sFldstr[20],strlen(sFldstr[20]));
			stCompare.tx_date = atoi(sFldstr[21]);
			stCompare.trace_no = atoi(sFldstr[22]);
			memcpy(stCompare.com_stat,sFldstr[23],strlen(sFldstr[23]));
			memcpy(stCompare.batch_detail,sFldstr[24],strlen(sFldstr[24]));
			memcpy(stCompare.chrgflag,sFldstr[25],strlen(sFldstr[25]));
			memcpy(stCompare.chrg_ind,sFldstr[26],strlen(sFldstr[26]));
			stCompare.chrg_amt = strtod(sFldstr[27],NULL)/100;
			vtcp_log("stCompare.chrg_amt��ֵ��[%.2f][%s][%d]",stCompare.chrg_amt,__FILE__,__LINE__);
			memcpy(stCompare.chrgacno,sFldstr[28],strlen(sFldstr[28]));
			memcpy(stCompare.feeaccno,sFldstr[29],strlen(sFldstr[29]));
			stCompare.com_pt_date = pt_date;
			stCompare.com_swno = lCom_swno;
			
			iRet = Chnl_compare_acct_Ins(stCompare,g_pub_tx.reply);
			if (iRet){
				Chnl_compare_acct_Debug(&stCompare);
				sprintf(acErrMsg,"������˵Ǽǲ�����[%d]",iRet);
				WRITEMSG
				Chnl_compare_acct_Clo_Upd();
				goto ErrExit;
			}
			vtcp_log("[%s],[%d]��,lTot_num=��%d���ʵǼ���ɣ�",__FILE__,__LINE__,lTot_num);
			
		}else if (iRet){
			sprintf(acErrMsg,"��ѯChnl_compare_acct�����%d��",iRet);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			Chnl_compare_acct_Clo_Upd();
			goto ErrExit;
		}else{
			vtcp_log("[%s],[%d]���ݿ��м�¼����¶��˵ǼǱ�,iRet=��%d��",__FILE__,__LINE__,iRet);
			memset(&stCompare,0x00,sizeof(stCompare));
			/*stCompare.detail_no = atoi(sFldstr[0]);*/
			memcpy(stCompare.un_marking,sFldstr[1],strlen(sFldstr[1]));
			memcpy(stCompare.syscode,sFldstr[2],strlen(sFldstr[2]));
			memcpy(stCompare.com_date,sFldstr[3],strlen(sFldstr[3]));
			memcpy(stCompare.code_type,sFldstr[4],strlen(sFldstr[4]));
			memcpy(stCompare.wt_date,sFldstr[5],strlen(sFldstr[5]));
			stCompare.orderno = atoi(sFldstr[6]);
			stCompare.pt_date = atoi(sFldstr[7]);
			stCompare.pt_trace_no = atoi(sFldstr[8]);
			memcpy(stCompare.or_br_no,sFldstr[9],strlen(sFldstr[9]));
			memcpy(stCompare.sndrcvflag,sFldstr[10],strlen(sFldstr[10]));
			stCompare.tx_amt = strtod(sFldstr[11],NULL)/100;
			vtcp_log("stCompare.tx_amt��ֵ��[%.2f][%s][%d]",stCompare.tx_amt,__FILE__,__LINE__);
			memcpy(stCompare.con_marking,sFldstr[12],strlen(sFldstr[12]));
			memcpy(stCompare.bor_lend,sFldstr[13],strlen(sFldstr[13]));
			memcpy(stCompare.main_acc_no,sFldstr[14],strlen(sFldstr[14]));
			memcpy(stCompare.main_acc_nm,sFldstr[15],strlen(sFldstr[15]));
			memcpy(stCompare.snd_acc_no,sFldstr[16],strlen(sFldstr[16]));
			memcpy(stCompare.note_type,sFldstr[17],strlen(sFldstr[17]));
			memcpy(stCompare.note_no,sFldstr[18],strlen(sFldstr[18]));
			memcpy(stCompare.tx_br_no,sFldstr[19],strlen(sFldstr[19]));
			memcpy(stCompare.teller,sFldstr[20],strlen(sFldstr[20]));
			stCompare.tx_date = atoi(sFldstr[21]);
			stCompare.trace_no = atoi(sFldstr[22]);
			memcpy(stCompare.com_stat,sFldstr[23],strlen(sFldstr[23]));
			memcpy(stCompare.batch_detail,sFldstr[24],strlen(sFldstr[24]));
			memcpy(stCompare.chrgflag,sFldstr[25],strlen(sFldstr[25]));
			memcpy(stCompare.chrg_ind,sFldstr[26],strlen(sFldstr[26]));
			stCompare.chrg_amt = strtod(sFldstr[27],NULL)/100;
			vtcp_log("stCompare.chrg_amt��ֵ��[%.2f][%s][%d]",stCompare.chrg_amt,__FILE__,__LINE__);
			memcpy(stCompare.chrgacno,sFldstr[28],strlen(sFldstr[28]));
			memcpy(stCompare.feeaccno,sFldstr[29],strlen(sFldstr[29]));
			stCompare.com_pt_date = pt_date;
			stCompare.com_swno = lCom_swno;
			
			iRet = Chnl_compare_acct_Upd_Upd(stCompare,g_pub_tx.reply);
			if (iRet){
        		Chnl_compare_acct_Debug(&stCompare);
				sprintf(acErrMsg,"������˵Ǽǲ�����[%d]",iRet);
				WRITEMSG
				Chnl_compare_acct_Clo_Upd();
				goto ErrExit;
			}
			vtcp_log("[%s],[%d]��,lTot_num=��%d���ʸ�����ɣ�",__FILE__,__LINE__,lTot_num);
		}
		Chnl_compare_acct_Clo_Upd();
		lTot_num ++;
		vtcp_log("[%s],[%d]��,lTot_num=��%d���ʵǼ���ɣ�",__FILE__,__LINE__,lTot_num);
		/*continue;*/
	}
	fclose(fp);
	
	vtcp_log("[%s][%d]sFilename��ֵ��[%s],",__FILE__,__LINE__,sFilename);
	/******ȡ�����ļ������********/
	for ( i = 0; i < strlen(sFilename); i++ )
	{
		if(sFilename[i] == '_') 
		{
			j++;
			if(j == 3) k=i+1;
		}
		if(sFilename[i] == '.')
		{
			l = i + 1;
		}
	}
	vtcp_log("I��ֵ��[%d][%s][%d]",i,__FILE__,__LINE__);
	vtcp_log("J��ֵ��[%d][%s][%d]",j,__FILE__,__LINE__);
	vtcp_log("K��ֵ��[%d][%s][%d]",k,__FILE__,__LINE__);
	vtcp_log("L��ֵ��[%d][%s][%d]",l,__FILE__,__LINE__);
	memset(sSeqno,0x00,sizeof(sSeqno));
	memcpy(sSeqno,sFilename + k,l - 1 - k);
	vtcp_log("[%s][%d]cSeq��ֵ��[%s]",__FILE__,__LINE__,sSeqno);
	
	/*****���ɶ��˽���ļ�**********/
	sprintf(sFilename_ret,"%s/txt/CNAPS2_check_%ld_%s.ret",getenv("HOME"),
		com_sys_parm.sys_date,sSeqno);
	sprintf(sFilename_rt,"CNAPS2_check_%ld_%s.ret",com_sys_parm.sys_date,sSeqno);
	vtcp_log("[%s][%d]sFilename_ret=[%s]",__FILE__,__LINE__,sFilename_ret);
	vtcp_log("[%s][%d]sFilename_rt=[%s]",__FILE__,__LINE__,sFilename_rt);
	new_fp = fopen(sFilename_ret,"w");
	if (new_fp == NULL)
	{
		vtcp_log("[%s][%d]���ļ�[%s]��",__FILE__,__LINE__,sFilename_rt);
		strcpy(g_pub_tx.reply,"P157");
		goto ErrExit;
	}
	iRet = Chnl_compare_acct_Dec_Upd(g_pub_tx.reply,"syscode='%s' and com_date='%s'"
		" and com_pt_date=%d and (com_flag = '4' or com_flag is null)",sSysCode,sCheckDate,pt_date);
	if (iRet){
		vtcp_log("�����α����,%d",iRet);
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	while (1){
		memset(&g_pub_tx,0x00,sizeof(g_pub_tx));
		pub_base_sysinit();
		memset(sCom_result,0x00,sizeof(sCom_result));
		memset(&stCompareQ,0x00,sizeof(struct chnl_compare_acct_c));
		iRet = Chnl_compare_acct_Fet_Upd(&stCompareQ,g_pub_tx.reply);
		if (iRet){
			if (iRet == 100 || iRet == 1403){
				vtcp_log("���˵ǼǱ���û�иñʼ�¼,%d",iRet);
				if (lFact_num != 0){
					strcpy(g_pub_tx.reply,"0000");
				}
				break;
			}else{
				vtcp_log("fet����,%d",iRet);
				strcpy(g_pub_tx.reply,"D103");
				Chnl_compare_acct_Clo_Upd();
				goto ErrExit;
			}
		}
		if (stCompareQ.batch_detail[0]=='0'){
			iRet = Chnl_single_cnaps_Dec_Upd(g_pub_tx.reply,"chnl_type='3' and "
				"bbmk='%s' and pt_date=%d and pt_trace_no=%d ",
				stCompareQ.syscode,stCompareQ.pt_date,stCompareQ.pt_trace_no);
			if (iRet){
				vtcp_log("�����α����,%d",iRet);
				strcpy(g_pub_tx.reply,"D103");
				Chnl_compare_acct_Clo_Upd();
				goto ErrExit;
			}
			iRet = Chnl_single_cnaps_Fet_Upd(&stSingle,g_pub_tx.reply);
			if (iRet == 100 || iRet == 1403){
				/*Chnl_single_cnaps_Clo_Upd();*/
				if (stCompareQ.com_stat[0] == '2'){
					vtcp_log("���ʼ��˵ǼǱ��޼�¼�������ˣ�com_stat[%s]",stCompareQ.com_stat);
					/*��Ϊ���˶��ˣ�����û�м�¼��û�м��ˣ��������˴����ӿ�δд��*/
					iRet = compare_info_acct(stCompareQ,sGz_br_no,sAc_no,sGz_reason);
					if (iRet){
						vtcp_log("�Զ����˴���,��%d��",iRet);
						stCompareQ.com_flag[0] = '4';
						strcpy(sCom_result,"���˴�");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]����ʧ�ܣ�",__FILE__,__LINE__);
					}else{
						vtcp_log("[%s][%d]cAc_no=[%s],cGz_br_no=[%s],cGz_reason=[%s]",
							__FILE__,__LINE__,sAc_no,sGz_br_no,sGz_reason);
						if (strlen(sGz_reason) != 0){
							stCompareQ.com_flag[0] = '2';
							strcpy(stCompareQ.gz_br_no,sGz_br_no);
							strcpy(stCompareQ.gz_acct_no,sAc_no);
							strcpy(stCompareQ.gz_reasen,sGz_reason);
							strcpy(sCom_result,"���˲���");
							stSingle.sts[0]='2';
							vtcp_log("[%s][%d]���˲��ˣ�",__FILE__,__LINE__);
						}else{
							stCompareQ.com_flag[0] = '1';
							strcpy(sCom_result,"��������");
							stSingle.sts[0]='1';
							vtcp_log("[%s][%d]�������ˣ�",__FILE__,__LINE__);
						}
						stCompareQ.tx_date = g_pub_tx.tx_date;
						stCompareQ.trace_no = g_pub_tx.trace_no;
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]gz_br_no[%s]",__FILE__,__LINE__,stCompareQ.gz_br_no);
						vtcp_log("[%s][%d]gz_acct_no[%s]",__FILE__,__LINE__,stCompareQ.gz_acct_no);
						vtcp_log("[%s][%d]gz_reasen[%s]",__FILE__,__LINE__,stCompareQ.gz_reasen);
						vtcp_log("[%s][%d]tx_date[%d]",__FILE__,__LINE__,stCompareQ.tx_date);
						vtcp_log("[%s][%d]trace_no[%d]",__FILE__,__LINE__,stCompareQ.trace_no);
						vtcp_log("[%s][%d]����д�ļ���ɣ�",__FILE__,__LINE__);
					}
				}else{
					vtcp_log("���ʼ��˵ǼǱ���û�иñʼ�¼,%d",iRet);
					stCompareQ.com_flag[0] = '4';
					strcpy(sCom_result,"���˴�");
					iWriteFile(new_fp,stCompareQ);
					vtcp_log("[%s][%d]���˴�,����!",__FILE__,__LINE__);
				}
				strcpy(g_pub_tx.reply,"0000");
			}else if (iRet){
				vtcp_log("fet����,%d",iRet);
				Chnl_single_cnaps_Clo_Upd();
				strcpy(g_pub_tx.reply,"D103");
				Chnl_compare_acct_Clo_Upd();
				goto ErrExit;
			}else{/*�������˽��:0-����ƽ  1-�������� 2-���˲��� 3-�ѳ��� 4-���˴�*/
				if (stCompareQ.com_stat[0]=='0'){/*���˶���*/
					if (stSingle.sts[0]=='0'||stSingle.sts[0]=='4'){
						stCompareQ.com_flag[0] = '4';
						strcpy(sCom_result,"���˴�");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]���˴�,����!",__FILE__,__LINE__);
					}else{
						stCompareQ.com_flag[0] = '0';
						strcpy(sCom_result,"����ƽ");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]����ƽ,�����!",__FILE__,__LINE__);
					}
				}else if(stCompareQ.com_stat[0]=='1'){/*��������*/
					if (!memcmp(stSingle.sts,"4",1)){
						stCompareQ.com_flag[0] = '0';
						strcpy(sCom_result,"����ƽ");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]����ƽ,�����!",__FILE__,__LINE__);
					}else{
						stCompareQ.com_flag[0] = '4';
						strcpy(sCom_result,"���˴�");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]���˴�,����!",__FILE__,__LINE__);
					}
				}else if (stCompareQ.com_stat[0]=='2'){/*���˶���*/
					if (stSingle.sts[0]=='1'||stSingle.sts[0]=='2'||
						stSingle.sts[0]=='3'||stSingle.sts[0]=='5'||
						stSingle.sts[0]=='6')
					{
						stCompareQ.com_flag[0] = '1';
						strcpy(sCom_result,"��������");
						iWriteFile(new_fp,stCompareQ);
						stCompareQ.tx_date = stSingle.tx_date;
						stCompareQ.trace_no = stSingle.trace_no;
						vtcp_log("[%s][%d]tx_date[%d]",__FILE__,__LINE__,stCompareQ.tx_date);
						vtcp_log("[%s][%d]trace_no[%d]",__FILE__,__LINE__,stCompareQ.trace_no);
						vtcp_log("[%s][%d]�Ѽ���,����!",__FILE__,__LINE__);
					}else{
						stCompareQ.com_flag[0] = '4';
						strcpy(sCom_result,"���˴�");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]���˴�,����!",__FILE__,__LINE__);
					}
				}else if (!memcmp(stCompareQ.com_stat,"3",1)){/*��������*/
					if (!memcmp(stSingle.sts,"4",1)){
						stCompareQ.com_flag[0] = '0';
						strcpy(sCom_result,"����ƽ");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]�ѳ���,����!",__FILE__,__LINE__);
					}else{/*��Ҫ�����������������ӿ�Ϊ������*/
						iRet = compare_exit_acct(stCompareQ);
						if (iRet){
							vtcp_log("�Զ����˴���,��%d��",iRet);
							stCompareQ.com_flag[0] = '4';
							strcpy(sCom_result,"����ʧ��");
							iWriteFile(new_fp,stCompareQ);
							vtcp_log("[%s][%d]�Զ�����ʧ��,����!",__FILE__,__LINE__);
							/**add by zhaoyue 20140714 Ϊ�������һ���ԣ������˳�**/
							goto ErrExit;
							/**add by zhaoyue 20140714 end**/
						}else{
							stCompareQ.com_flag[0] = '3';
							strcpy(sCom_result,"���˳���");
							stCompareQ.tx_date =0;
							stCompareQ.trace_no =0;
							stCompareQ.tx_date = g_pub_tx.tx_date;
							stCompareQ.trace_no = g_pub_tx.trace_no;
							iWriteFile(new_fp,stCompareQ);
							stSingle.sts[0]='4';
							vtcp_log("[%s][%d]tx_date[%d]",__FILE__,__LINE__,stCompareQ.tx_date);
							vtcp_log("[%s][%d]trace_no[%d]",__FILE__,__LINE__,stCompareQ.trace_no);
							vtcp_log("[%s][%d]���˳ɹ�,�����!",__FILE__,__LINE__);
						}
					}
				}else{
					vtcp_log("ƽ̨�Ķ��˱�ʶ����ȷ��com_stat=��%s��",stCompareQ.com_stat);
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}
				memcpy(stSingle.dz_ind,"1",1);
				vtcp_log("����״̬,Dz_ind=��%s��",stSingle.dz_ind);
				strcpy(stSingle.fill1,sCom_result);
				vtcp_log("�ڡ�%d����,sCom_result=��%s��com_stat=��%s��",
				lFact_num,sCom_result,stCompareQ.com_stat);
				
				iRet = Chnl_single_cnaps_Upd_Upd(stSingle,g_pub_tx.reply);
				if (iRet)
				{
					Chnl_single_cnaps_Clo_Upd();
					Chnl_compare_acct_Clo_Upd();
					sprintf(acErrMsg,"�������ݿ�ʧ�ܣ�[%d]",iRet);
					WRITEMSG
					strcpy(g_pub_tx.reply,"P102");
					goto ErrExit;
				}
				vtcp_log("[%s],[%d],���µ��ʼ��˱���ɣ���[%d]��",__FILE__,__LINE__,lFact_num);
			}
			Chnl_single_cnaps_Clo_Upd();
			vtcp_log("%s,%d,found g_pub_tx.reply=[%s]",__FILE__,__LINE__,g_pub_tx.reply);
			vtcp_log("[%s],[%d]tx_date=[%d],trace_no=[%d]",__FILE__,__LINE__,stCompareQ.tx_date,stCompareQ.trace_no);
			vtcp_log("[%s],[%d],���˱�ʶ:[%s]",__FILE__,__LINE__,stCompareQ.com_flag);
			vtcp_log("[%s],[%d],���˽������:[%s]",__FILE__,__LINE__,sCom_result);
			memcpy(stCompareQ.com_result,sCom_result,strlen(sCom_result));
			vtcp_log("[%s][%d]gz_br_no[%s]",__FILE__,__LINE__,stCompareQ.gz_br_no);
			vtcp_log("[%s][%d]gz_acct_no[%s]",__FILE__,__LINE__,stCompareQ.gz_acct_no);
			vtcp_log("[%s][%s]gz_reasen[%s]",__FILE__,__LINE__,stCompareQ.gz_reasen);
			
			iRet = Chnl_compare_acct_Upd_Upd(stCompareQ,g_pub_tx.reply);
			if (iRet)
			{
				Chnl_compare_acct_Clo_Upd();
				sprintf(acErrMsg,"�������ݿ�ʧ�ܣ�[%d]",iRet);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P102");
				goto ErrExit;
			}
			vtcp_log("[%s],[%d],���¶��˵ǼǱ���ɣ���[%d]��",__FILE__,__LINE__,lFact_num);
			
			lFact_num++;
			continue;
		}else if (stCompareQ.batch_detail[0]=='1'){
			iRet = Chnl_batch_acct_Dec_Upd(g_pub_tx.reply,"dz_ref = '%s'",stCompareQ.un_marking);
			if (iRet){
				vtcp_log("�����α����,%d",iRet);
				strcpy(g_pub_tx.reply,"D103");
				Chnl_compare_acct_Clo_Upd();
				goto ErrExit;
			}
			iRet = Chnl_batch_acct_Fet_Upd(&stBatch,g_pub_tx.reply);
			if (iRet == 100 || iRet == 1403){
				/*Chnl_batch_acct_Clo_Upd();*/
				if (stCompareQ.com_stat[0]=='2'){
					vtcp_log("�������˵ǼǱ���û�иñʼ�¼,���в��˴�����%d��",iRet);
					/*��Ϊ���˶��ˣ�����û�м�¼��û�м��ˣ��������˴����ӿ�δд��*/
					iRet = compare_info_acct(stCompareQ,sGz_br_no,sAc_no,sGz_reason);
					if (iRet){
						vtcp_log("�Զ����˴���,��%d��",iRet);
						stCompareQ.com_flag[0]='4';
						strcpy(sCom_result,"���˴���");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]���˴���,����!",__FILE__,__LINE__);
					}else{
						vtcp_log("[%s][%d]cAc_no=[%s],cGz_br_no=[%s],cGz_reason=[%s]",
							__FILE__,__LINE__,sAc_no,sGz_br_no,sGz_reason);
						if (strlen(sGz_reason) != 0){
							stCompareQ.com_flag[0]='2';
							strcpy(stCompareQ.gz_br_no,sGz_br_no);
							strcpy(stCompareQ.gz_acct_no,sAc_no);
							strcpy(stCompareQ.gz_reasen,sGz_reason);
							strcpy(sCom_result,"���˲���");
							vtcp_log("[%s][%d]���˲���,�����!",__FILE__,__LINE__);
						}else{
							stCompareQ.com_flag[0]='1';
							strcpy(sCom_result,"��������");
							vtcp_log("[%s][%d]��������,�����!",__FILE__,__LINE__);
						}
						stCompareQ.tx_date = g_pub_tx.tx_date;
						stCompareQ.trace_no = g_pub_tx.trace_no;
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]tx_date[%d]",__FILE__,__LINE__,stCompareQ.tx_date);
						vtcp_log("[%s][%d]trace_no[%d]",__FILE__,__LINE__,stCompareQ.trace_no);
						vtcp_log("[%s][%d]gz_br_no[%s]",__FILE__,__LINE__,stCompareQ.gz_br_no);
						vtcp_log("[%s][%d]gz_acct_no[%s]",__FILE__,__LINE__,stCompareQ.gz_acct_no);
						vtcp_log("[%s][%d]gz_reasen[%s]",__FILE__,__LINE__,stCompareQ.gz_reasen);
						vtcp_log("[%s][%d]�Ѽ���,����!",__FILE__,__LINE__);
					}
				}else{
					vtcp_log("���ʼ��˵ǼǱ���û�иñʼ�¼,%d",iRet);
					stCompareQ.com_flag[0]='4';
					strcpy(sCom_result,"���˴�");
					iWriteFile(new_fp,stCompareQ);
					vtcp_log("[%s][%d]���˴�,����!",__FILE__,__LINE__);
				}
				strcpy(g_pub_tx.reply,"0000");
			}else if (iRet){
				vtcp_log("fet����,%d",iRet);
				Chnl_batch_acct_Clo_Upd();
				Chnl_compare_acct_Clo_Upd();
				strcpy(g_pub_tx.reply,"D103");
				goto ErrExit;
			}else{
				/*�������˽��:0-����ƽ  1-�������� 2-���˲��� 3-�ѳ��� 4-���˴�*/
				if (stCompareQ.com_stat[0]=='0'){/*���˶���*/
					if (stBatch.sts[0]=='0'||stBatch.sts[0]=='4'){
						stCompareQ.com_flag[0]='4';
						strcpy(sCom_result,"���˴�");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]���˴�,����!",__FILE__,__LINE__);
					}else{
						stCompareQ.com_flag[0]='0';
						strcpy(sCom_result,"����ƽ");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]����ƽ,�����!",__FILE__,__LINE__);
					}
				}else if(stCompareQ.com_stat[0]=='1'){/*��������*/
					if (stBatch.sts[0]=='4'){
						stCompareQ.com_flag[0]='0';
						strcpy(sCom_result,"����ƽ");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]����ƽ,�����!",__FILE__,__LINE__);
					}else{
						stCompareQ.com_flag[0]='4';
						strcpy(sCom_result,"���˴�");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]���˴�,����!",__FILE__,__LINE__);
					}
				}else if (stCompareQ.com_stat[0]=='2'){/*���˶���*/
					if (stBatch.sts[0]=='1' || stBatch.sts[0]=='2' || 
						stBatch.sts[0]=='3' || stBatch.sts[0]=='5' || 
						stBatch.sts[0]=='6')
					{
						stCompareQ.com_flag[0]='1';
						strcpy(sCom_result,"��������");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]��������,�����!",__FILE__,__LINE__);
					}else{
						/*�������û�м��������в��˴����ӿ���δ������������Ҫдô����*/
						vtcp_log("���ʼ��˵ǼǱ���û�иñʼ�¼,%d",iRet);
						stCompareQ.com_flag[0]='4';
						strcpy(sCom_result,"���˴�");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]���˴�,����!",__FILE__,__LINE__);
					}
				}else if (stCompareQ.com_stat[0]=='3'){/*��������*/
					if (stBatch.sts[0]=='4'){
						stCompareQ.com_flag[0]='0';
						strcpy(sCom_result,"����ƽ");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]����ƽ,�����!",__FILE__,__LINE__);
					}else{/*��Ҫ�����������������ӿ�Ϊ������*/
						iRet = compare_exit_acct(stCompareQ);
						if (iRet){
							vtcp_log("�Զ����˴���,��%d��",iRet);
							stCompareQ.com_flag[0]='4';
							strcpy(sCom_result,"����ʧ��");
							vtcp_log("[%s][%d]����ʧ��,����!",__FILE__,__LINE__);
							/**add by zhaoyue 20140714 Ϊ�������һ���ԣ������˳�**/
							goto ErrExit;
							/**add by zhaoyue 20140714 end**/
						}else{
							stCompareQ.com_flag[0]='3';
							stCompareQ.tx_date = g_pub_tx.tx_date;
							stCompareQ.trace_no = g_pub_tx.trace_no;
							strcpy(sCom_result,"�ѳ���");
							stBatch.sts[0]='4';
							vtcp_log("[%s][%d]�ѳ���,�����!",__FILE__,__LINE__);
						}
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]tx_date[%d]",__FILE__,__LINE__,stCompareQ.tx_date);
						vtcp_log("[%s][%d]trace_no[%d]",__FILE__,__LINE__,stCompareQ.trace_no);
					}
				}else{
					vtcp_log("ƽ̨�Ķ��˱�ʶ����ȷ��com_stat=��%s��",stCompareQ.com_stat);
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}
				vtcp_log("�ڡ�%d����,sCom_result=��%s��com_stat=��%s��",
					lFact_num,sCom_result,stCompareQ.com_stat);
				stBatch.dz_ind[0] = '1';
				vtcp_log("����״̬,Dz_ind=��%s��",stBatch.dz_ind);
				
				iRet = Chnl_batch_acct_Upd_Upd(stBatch,g_pub_tx.reply);
				if (iRet)
				{
					Chnl_batch_acct_Clo_Upd();
					Chnl_compare_acct_Clo_Upd();
					sprintf(acErrMsg,"�������ݿ�ʧ�ܣ�[%d]",iRet);
					WRITEMSG
					strcpy(g_pub_tx.reply,"P102");
					goto ErrExit;
				}
				vtcp_log("[%s],[%d],�����������˵ǼǱ����[%d]��",__FILE__,__LINE__,lFact_num);
			}
			Chnl_batch_acct_Clo_Upd();
			vtcp_log("[%s],[%d],g_pub_tx.reply=[%s]",__FILE__,__LINE__,g_pub_tx.reply);
			vtcp_log("[%s],[%d]tx_date=[%d],trace_no=[%d]",__FILE__,__LINE__,stCompareQ.tx_date,stCompareQ.trace_no);
			vtcp_log("[%s],[%d],���˱�ʶ:[%s]",__FILE__,__LINE__,stCompareQ.com_flag);
			vtcp_log("[%s],[%d],���˽������:[%s]",__FILE__,__LINE__,sCom_result);
			
			/*strcpy(stCompareQ.gz_br_no,sGz_br_no);
			strcpy(stCompareQ.gz_acct_no,sAc_no);
			strcpy(stCompareQ.gz_reasen,sGz_reason);*/
			memcpy(stCompareQ.com_result,sCom_result,strlen(sCom_result));
			vtcp_log("[%s][%d]gz_br_no[%s]",__FILE__,__LINE__,stCompareQ.gz_br_no);
			vtcp_log("[%s][%d]gz_acct_no[%s]",__FILE__,__LINE__,stCompareQ.gz_acct_no);
			vtcp_log("[%s][%d]gz_reasen[%s]",__FILE__,__LINE__,stCompareQ.gz_reasen);
			
			iRet = Chnl_compare_acct_Upd_Upd(stCompareQ,g_pub_tx.reply);
			if (iRet)
			{
				Chnl_compare_acct_Clo_Upd();
				sprintf(acErrMsg,"�������ݿ�ʧ�ܣ�[%d]",iRet);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P102");
				goto ErrExit;
			}
			vtcp_log("[%s],[%d],���¶��˵ǼǱ���ɣ���[%d]��",__FILE__,__LINE__,lFact_num);
			
			lFact_num++;
			continue;
			
		}else{
			vtcp_log("������ʶ����ȷ��batch_detail=��%s��",stCompareQ.batch_detail);
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		vtcp_log("%s,%d,found g_pub_tx.reply=[%s]",__FILE__,__LINE__,g_pub_tx.reply);
	}
	Chnl_compare_acct_Clo_Upd();
	vtcp_log("%s,%d,�����ܱ���=[%d]",__FILE__,__LINE__,lFact_num);
	if (new_fp == NULL){
		vtcp_log("�����Ѷ��˻�δ����ҵ��");
		strcpy(g_pub_tx.reply,"D104");
		goto ErrExit;
	}
	fclose(new_fp);
	memset(sCmd,0x0,sizeof(sCmd));
	sprintf(sCmd,"%s/bin/send_back.sh %s",getenv("HOME"),sFilename_rt);
	iRet = system(sCmd);
	if(iRet){
		sprintf(acErrMsg,"�ش��ļ�����[%d]",iRet);
		WRITEMSG
		strcpy(g_pub_tx.reply,"S047");
		goto ErrExit;
	}
	set_zd_data("0820",sFilename_rt);
	vtcp_log("%s,%d,sFilename_rt=[%s]!",__FILE__,__LINE__,sFilename_rt);
	
	vtcp_log("[%s][%d],g_pub_tx.reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);  
	
OkExit:
	/*sql_commit();
	sql_begin();*/
	vtcp_log("%s,%d,�������!",__FILE__,__LINE__);
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return(0);
ErrExit:
	/*sql_rollback();
	sql_begin();*/
	if(strlen(g_pub_tx.reply)==0 || !strcmp(g_pub_tx.reply,"0000"))
	{
		strcpy(g_pub_tx.reply,"D103");
	}
	vtcp_log("%s,%d,��������ʧ��!",__FILE__,__LINE__);
	if(new_fp!=NULL)
	fclose(new_fp);
	if(fp!=NULL)
	fclose(fp);
	WRITEMSG
	return(1);
}
	
/*==============================================================================
**�������ܣ�ƽ̨�ͺ��Ķ���ʱ�Զ�����
**���������
**	1�������ʾ��
**		DBIT--��ǣ�CRDT--����
**	2��������
**		stCompare.main_acc_no--���˺ţ�stCompare.main_acc_nm--���˺Ż���
**		stCompare.snd_acc_no--�ڶ��˺ţ�stCompare.tx_amt--���׽�
**		stCompare.tx_br_no--���׽ṹ��
**�����������������ò����������������崦�����£�
**	1��������˻���Ϣ�Ƿ���������˺š�������һ��������й��˴���
**	2����Ϊǿ�ƹ��˵Ľ������������˻���Ϣֱ����ǿ�ƹ��˴���
**	3��������˻���Ϣ��ȫ��ȷ��ֱ�����ˣ�ǿ�ƹ��˵ĳ��⣩��
**	4���Ǽǵ��ʻ��������˵ǼǱ���
================================================================================*/
static int compare_info_acct(struct chnl_compare_acct_c stCompare,char *gz_br_no,char *ac_no,char *gz_reason)
{
	vtcp_log("[%s],[%d],��û�н����˺�����compare_info_acct��!",__FILE__,__LINE__);
	int	iRet=0;
	int	iParm_seqn = 0;	/*com_parm �� �������*/
	char	sGdkm[6] = {0};  /**���ɿ�Ŀ**/
	char	cAc_no1[33] = {0};	/*���˺�*/
	char	cAc_name1[61] = {0};	/*����*/
	char	cGz_ind[2] = {0};	/*���˱�־*/
	char	cGz_br_no[6] = {0};	/*���˻���*/
	char	cGz_reason[2] = {0};	/*����ԭ��*/
	char    cXjinFlag[2] = {0};	/*�ֽ����־*/
	struct	chnl_batch_acct_c stBatch = {0};	/*�������˵Ǽǲ�*/
	struct	chnl_single_cnaps_c stSingle = {0};	/*���ʼ��˵Ǽǲ�*/
	vtcp_log("��%s����%d��,found g_pub_tx.reply=[%s]",__FILE__,__LINE__,g_pub_tx.reply);
	strcpy(g_pub_tx.reply,"0000");
	/***��***/
	g_pub_tx.ac_id = 0;
	g_pub_tx.ac_seqn = 0;
	strcpy(g_pub_tx.beg_note_no,"");
	g_pub_tx.tx_amt1 = stCompare.tx_amt;
	cGz_ind[0] = stCompare.con_marking[5];
	vtcp_log("[%s][%d]���˱�ʶcGz_ind=[%s]",__FILE__,__LINE__,cGz_ind);
	strcpy(cAc_no1,stCompare.main_acc_no);
	strcpy(cAc_name1,stCompare.main_acc_nm);
	strcpy(cGz_br_no,stCompare.tx_br_no);
	if (!memcmp(cXjinFlag,"X",1)){
		iParm_seqn = 299;
	}else{
		if (!memcmp(stCompare.syscode,"HVPS",4))
		{
			iParm_seqn = 6;
		}else{
			iParm_seqn = 8;
		}
	}
	iRet = pubf_com_CheckGZ(cAc_no1,cAc_name1,cGz_ind,cGz_br_no,cGz_reason,iParm_seqn);
	if (iRet){
	  sprintf(acErrMsg,"%s,%d,���˼�麯��ʧ��",__FILE__,__LINE__);
	  WRITEMSG
	  goto ErrExit;	
	}
	vtcp_log("[%s][%d]cAc_no1=[%s],cAc_name1=[%s],cGz_br_no=[%s],cGz_reason=[%s]",
		__FILE__,__LINE__,cAc_no1,cAc_name1,cGz_br_no,cGz_reason);
	strcpy(gz_br_no,cGz_br_no);
	strcpy(ac_no,cAc_no1);
	strcpy(gz_reason,cGz_reason);
	vtcp_log("[%s][%d]gz_br_no=[%s],ac_no=[%s],gz_reason=[%s]",
		__FILE__,__LINE__,gz_br_no,ac_no,gz_reason);
	memcpy(g_pub_tx.reply,"0000",4);
	memset(g_pub_tx.ac_no,0x00,sizeof(g_pub_tx.ac_no));
	strcpy(g_pub_tx.ac_no,cAc_no1);
	if (cAc_no1[0] == '9' || strlen(cAc_no1) <10){/*--�ڲ��� --*/
		
		strcpy(g_pub_tx.ac_id_type,"9");		 /*	�˻�����Ϊ�ڲ� */
		strcpy(g_pub_tx.ac_get_ind,"00");		/* ������δ��ȡ�ֻ�	*/
		g_pub_tx.svc_ind = 9001;
		strcpy(g_pub_tx.add_ind,"1"); 
		memset(g_pub_tx.sub_tx_code,0x00,sizeof(g_pub_tx.sub_tx_code));
		strcpy(g_pub_tx.sub_tx_code,"A017");
		set_zd_data(DC_SUB_TX_CODE,g_pub_tx.sub_tx_code);
		set_zd_data("1214","01");
		set_zd_data("1215","2");
		set_zd_double("1218",g_pub_tx.tx_amt1);
	}else{
		strcpy(g_pub_tx.ac_id_type,"1");		 /*	�˻�����Ϊ���� */
		strcpy(g_pub_tx.ac_get_ind,"00");		/* ������δ��ȡ�ֻ�	*/
		g_pub_tx.svc_ind = 1001;
		strcpy(g_pub_tx.add_ind,"1"); 
		memset(g_pub_tx.sub_tx_code,0x00,sizeof(g_pub_tx.sub_tx_code));
		strcpy(g_pub_tx.sub_tx_code,"D099");
		set_zd_data(DC_SUB_TX_CODE,g_pub_tx.sub_tx_code);
		
		strcpy(g_pub_tx.prt_ind,"0");
		set_zd_data("1017","0");
		
		set_zd_data("101A","01");
		set_zd_data("101B","2");
		set_zd_double("1013",g_pub_tx.tx_amt1);
	}
	strcpy(g_pub_tx.ct_ind,"2");
	strcpy(g_pub_tx.ac_wrk_ind,"001001001");
	strcpy(g_pub_tx.hst_ind,"1");
	strcpy(g_pub_tx.brf,"���˲���");
	iRet = pub_acct_trance();
	if (iRet){
		sprintf(acErrMsg,"���ô�ȡ������ʧ��[%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}
	iRet = pubf_acct_proc(g_pub_tx.sub_tx_code);
	if (iRet){
		sprintf(acErrMsg,"���ô�ȡ������ʧ��[%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}
	/****��***/
	g_pub_tx.ac_id = 0;		 /*	test clean */
	g_pub_tx.ac_seqn = 0;
	strcpy(g_pub_tx.beg_note_no,"");
	strcpy(g_pub_tx.ac_no,stCompare.snd_acc_no);
	g_pub_tx.tx_amt1 = stCompare.tx_amt;
	
	if (g_pub_tx.ac_no[0]	== '9' || strlen(g_pub_tx.ac_no) < 10){ /*--�ڲ���--*/
		
		strcpy(g_pub_tx.ac_id_type,"9");		 /*	�˻�����Ϊ�ڲ� */
		strcpy(g_pub_tx.ac_get_ind,"00");		/* ������δ��ȡ�ֻ�	*/
		g_pub_tx.svc_ind = 9001;
		strcpy(g_pub_tx.add_ind,"0");
		memset(g_pub_tx.sub_tx_code,0x00,sizeof(g_pub_tx.sub_tx_code));
		strcpy(g_pub_tx.sub_tx_code,"A016");
		set_zd_data(DC_SUB_TX_CODE,g_pub_tx.sub_tx_code);
		set_zd_data("1204","01");
		set_zd_data("1205","2");
		set_zd_double("1208",g_pub_tx.tx_amt1);
	}else{
		strcpy(g_pub_tx.ac_id_type,"1");		 /*	�˻�����Ϊ���� */
		strcpy(g_pub_tx.ac_get_ind,"00");		/* ������δ��ȡ�ֻ�	*/
		g_pub_tx.svc_ind = 1001;
		strcpy(g_pub_tx.add_ind,"0"); 
		memset(g_pub_tx.sub_tx_code,0x00,sizeof(g_pub_tx.sub_tx_code));
		strcpy(g_pub_tx.sub_tx_code,"D003");
		set_zd_data(DC_SUB_TX_CODE,g_pub_tx.sub_tx_code);
		strcpy(g_pub_tx.prt_ind,"0");
		set_zd_data("102O","0");
		set_zd_data("102J","01");
		set_zd_data("102K","2");
		set_zd_double("102F",g_pub_tx.tx_amt1);
		set_zd_double("102P",0.00);
		set_zd_double("102Q",0.00);
	}
	strcpy(g_pub_tx.ct_ind,"2");
	strcpy(g_pub_tx.ac_wrk_ind,"001001001");
	strcpy(g_pub_tx.hst_ind,"1");
	strcpy(g_pub_tx.brf,"���˲���");
	iRet = pub_acct_trance();
	if (iRet){
		sprintf(acErrMsg,"���ô�ȡ������ʧ��[%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}
	iRet = pubf_acct_proc(g_pub_tx.sub_tx_code);
	if (iRet){
		sprintf(acErrMsg,"���ô�ȡ������ʧ��[%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}
	g_pub_tx.ac_id = 0;
	g_pub_tx.ac_seqn = 0;
	g_pub_tx.tx_amt1 = 0.00;		/* ������	*/
	strcpy(g_pub_tx.ac_id_type,"0");		 /*	�˻�������Ϊ���� */
	
	/*add fupengfei 2014/7/19 12:41:33 ���˹��ɼ���begin*/
	if(!memcmp(stCompare.syscode,"HVPS",4))
	{
		strcpy(sGdkm,DELZGD);
	}
	if(!memcmp(stCompare.syscode,"BEPS",4))
	{
		strcpy(sGdkm,XELZGD);
	}
	{
		memset(g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code));
		memset(g_pub_tx.brf, 0x0, sizeof(g_pub_tx.brf));
		memset(g_pub_tx.ac_no, 0x0, sizeof(g_pub_tx.ac_no));
		memset(g_pub_tx.add_ind, 0x0, sizeof(g_pub_tx.add_ind));

		memset(g_pub_tx.opn_br_no, 0x0, sizeof(g_pub_tx.opn_br_no));
		strncpy(g_pub_tx.opn_br_no, stCompare.tx_br_no ,sizeof(g_pub_tx.opn_br_no)-1);

		vtcp_log("%s,%d,g_pub_tx.opn_br_no=[%s]!",__FILE__,__LINE__,g_pub_tx.opn_br_no);
		strcpy(g_pub_tx.brf,"���˼���");
		
		g_pub_tx.svc_ind=9001;
		strcpy(g_pub_tx.sub_tx_code, "A016");
		strcpy(g_pub_tx.ac_id_type,"9");
		g_pub_tx.ac_seqn=0;

		strcpy(g_pub_tx.ac_no,sGdkm);
		g_pub_tx.tx_amt1=stCompare.tx_amt;
		strcpy(g_pub_tx.add_ind,"0"); 
		strcpy(g_pub_tx.hst_ind,"1"); /**�ռ�����ϸ**/
		strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
		strcpy(g_pub_tx.ac_get_ind, "00");
		strcpy(g_pub_tx.note_type,"");
		strcpy(g_pub_tx.beg_note_no,"");
	
		if ( pub_acct_trance() )
		{
			sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
			WRITEMSG
			goto ErrExit;
		}

		set_zd_data("1204","01");
		set_zd_data("1205",g_pub_tx.ct_ind);
		set_zd_double("1208",g_pub_tx.tx_amt1);

		if ( pubf_acct_proc(g_pub_tx.sub_tx_code) )
		{
			sprintf(acErrMsg,"�Ǽǻ����ˮ����!");
			WRITEMSG
			goto ErrExit;
		}

		/* ���ɴ��� */
		memset(g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code));
		memset(g_pub_tx.brf, 0x0, sizeof(g_pub_tx.brf));
		memset(g_pub_tx.ac_no, 0x0, sizeof(g_pub_tx.ac_no));
		memset(g_pub_tx.add_ind, 0x0, sizeof(g_pub_tx.add_ind));

		g_pub_tx.svc_ind=9001;
		strcpy(g_pub_tx.sub_tx_code, "A017");
		strcpy(g_pub_tx.ac_id_type,"9");
		g_pub_tx.ac_seqn=0;

		strcpy(g_pub_tx.ac_no,sGdkm);
		g_pub_tx.tx_amt1=stCompare.tx_amt;
		strcpy(g_pub_tx.add_ind,"1"); 
		strcpy(g_pub_tx.hst_ind,"1"); /**�ռ�����ϸ**/
		strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
		strcpy(g_pub_tx.ac_get_ind, "00");
		strcpy(g_pub_tx.note_type,"");
		strcpy(g_pub_tx.beg_note_no,"");
	
		if ( pub_acct_trance() )
		{
			sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
			WRITEMSG
			goto ErrExit;
		}

		set_zd_data("1214","01");
		set_zd_data("1215",g_pub_tx.ct_ind);
		set_zd_double("1218",g_pub_tx.tx_amt1);

		if ( pubf_acct_proc(g_pub_tx.sub_tx_code) )
		{
			sprintf(acErrMsg,"�Ǽǻ����ˮ����!");
			WRITEMSG
			goto ErrExit;
		}
	}
	/*add fupengfei 2014/7/19 12:41:33 ���˹��ɼ���end*/
	
	if (stCompare.batch_detail[0] == '0'){/**�Ǽǵ��ʼ��˵ǼǱ�***/
		
		stSingle.tx_date = g_pub_tx.tx_date;
		sprintf(stSingle.tx_time,"%6ld",g_pub_tx.tx_time);
		stSingle.trace_no = g_pub_tx.trace_no;
		memcpy(stSingle.cur_no,"01",2);
		memcpy(stSingle.ac_no,stCompare.main_acc_no,sizeof(stSingle.ac_no)-1);
		memcpy(stSingle.ac_no1,stCompare.main_acc_no,sizeof(stSingle.ac_no1)-1);
		memcpy(stSingle.ac_no2,stCompare.snd_acc_no,sizeof(stSingle.ac_no2)-1);
		memcpy(stSingle.g_ac_no2,stCompare.gz_acct_no,sizeof(stSingle.g_ac_no2)-1);
		memcpy(stSingle.tx_code,g_pub_tx.tx_code,sizeof(stSingle.tx_code)-1);
		memcpy(stSingle.tx_br_no,g_pub_tx.tx_br_no,sizeof(stSingle.tx_br_no)-1);
		memcpy(stSingle.opn_br_no,g_pub_tx.opn_br_no,sizeof(stSingle.opn_br_no)-1);
		memcpy(stSingle.tel,g_pub_tx.tel,sizeof(stSingle.tel)-1);
		get_zd_data("0100",stSingle.bbmk);
		memcpy(stSingle.ttyn,stSingle.bbmk,sizeof(stSingle.ttyn)-1);
		stSingle.pt_date = stCompare.pt_date;
		vtcp_log("[%s][%d],ƽ̨����=[%d]",__FILE__,__LINE__,stSingle.pt_date);
		sprintf(stSingle.pt_trace_no ,"%ld" ,stCompare.pt_trace_no);
		vtcp_log("[%s][%d],ƽ̨��ˮ=[%s]",__FILE__,__LINE__,stSingle.pt_trace_no);
		stSingle.tx_amt = stCompare.tx_amt;
		stSingle.ct_ind[0] = '2';
		memcpy(stSingle.brf,"���˼���",sizeof(stSingle.brf)-1);
		stSingle.sts[0] = '1';
		stSingle.dz_ind[0] = '1';
		stSingle.chnl_type[0] = '3';
		
		iRet = Chnl_single_cnaps_Ins(stSingle,g_pub_tx.reply);
		if (iRet){
			sprintf(acErrMsg,"�Ǽǵ��ʼ��˵ǼǱ�����!!!");
			WRITEMSG
			Chnl_single_cnaps_Debug(&stSingle);
			strcpy(g_pub_tx.reply,"D107");
			goto ErrExit;
		}
		vtcp_log("[%s],[%d],�Ǽǵ��ʼ��˵ǼǱ���ɣ�stSingle��!",__FILE__,__LINE__);
	}
	if (stCompare.batch_detail[0] == '1'){/*�����ˣ��Ǽ��������˵Ǽǲ�*/
		
		stBatch.tx_date = g_pub_tx.tx_date;
		sprintf(stBatch.tx_time,"%ld",g_pub_tx.tx_time);
		get_zd_data("0830",stBatch.batch_ref);
		strcpy(stBatch.dz_ref,stCompare.un_marking);
		stBatch.trace_no = g_pub_tx.trace_no;
		strcpy(stBatch.cur_no,"01");
		strcpy(stBatch.ac_no,stCompare.main_acc_no);
		strcpy(stBatch.ac_no1,stCompare.main_acc_no);
		strcpy(stBatch.ac_no2,stCompare.snd_acc_no);
		strcpy(stBatch.g_ac_no2,stCompare.gz_acct_no);
		strcpy(stBatch.tx_code,g_pub_tx.tx_code);
		strcpy(stBatch.tx_br_no,g_pub_tx.tx_br_no);
		strcpy(stBatch.opn_br_no,g_pub_tx.opn_br_no);
		strcpy(stBatch.tel,g_pub_tx.tel);
		get_zd_data("0100",stBatch.bbmk);
		strcpy(stBatch.ttyn,g_pub_tx.tty);
		stBatch.pt_date = stCompare.pt_date;
		get_zd_data("0170",stBatch.pt_time);
		sprintf(stBatch.pt_trace_no ,"%ld" ,stCompare.pt_trace_no); 
		stBatch.tx_amt=stCompare.tx_amt;
		strcpy(stBatch.brf,"���˲���");
		stBatch.ct_ind[0]='2';
		stBatch.sts[0]='1';
		stBatch.dz_ind[0]='1';
		
		iRet = Chnl_batch_acct_Ins(stBatch,g_pub_tx.reply);
		if (iRet){
        	Chnl_batch_acct_Debug(&stBatch);
			sprintf(acErrMsg,"�����������˵Ǽǲ�����[%d]",iRet);
			WRITEMSG
			goto ErrExit;
		}
		vtcp_log("[%s],[%d],�����������˵Ǽǲ���ɣ�stBatch��!",__FILE__,__LINE__);
	}
	vtcp_log("����Ҫ��gz_br_no=[%s],ac_no[%s],gz_reason=[%s]",gz_br_no,ac_no,gz_reason);
	vtcp_log("[%s],[%d],���˺���ִ�������𣿣�compare_info_acct��!",__FILE__,__LINE__);
	
	OkExit:
		vtcp_log("��%s����%d��,found g_pub_tx.reply=[%s]",__FILE__,__LINE__,g_pub_tx.reply);
		return 0;
	ErrExit:
		return 1;
}
/*==============================================================================
**�������ܣ�ƽ̨�ͺ��Ķ���ʱ�Զ�����
**���������
**	1�������ʾ��
**		DBIT--��ǣ�CRDT--����
**	2��������
**		stCompare.main_acc_no--���˺ţ�stCompare.main_acc_nm--���˺Ż���
**		stCompare.snd_acc_no--�ڶ��˺ţ�stCompare.tx_amt--���׽�
**		stCompare.tx_br_no--���׽ṹ��
**�����������������ò����������������崦�����£�
**	1�����µ��ʻ��������˵ǼǱ���
================================================================================*/
static int compare_exit_acct(struct chnl_compare_acct_c stCompare)
{
	vtcp_log("[%s],[%d],������˺�����compare_exit_acct�����������",__FILE__,__LINE__);
	int	iRet = 0;
	char	sGdkm[6] = {0};  /***���ɿ�Ŀ**/
	struct	chnl_batch_acct_c stBatch = {0};	/*�������˵Ǽǲ�*/
	struct	chnl_single_cnaps_c stSingle = {0};	/*���ʼ��˵Ǽǲ�*/
	
	vtcp_log("[%s],[%d],���ڡ���ˮ,tx_date=[%d]trace_no=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date,g_pub_tx.trace_no);
	pub_base_sysinit();
	vtcp_log("[%s],[%d],���ڡ���ˮ,tx_date=[%d]trace_no=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date,g_pub_tx.trace_no);
	vtcp_log("��%s����%d��,found g_pub_tx.reply=[%s]",__FILE__,__LINE__,g_pub_tx.reply);
	strcpy(g_pub_tx.reply,"0000");
	/****��***/
	g_pub_tx.ac_id = 0;		 /*	test clean */
	g_pub_tx.ac_seqn = 0;
	strcpy(g_pub_tx.beg_note_no,"");
	strcpy(g_pub_tx.ac_no,stCompare.main_acc_no);
	g_pub_tx.tx_amt1 = -1*stCompare.tx_amt;
	if (g_pub_tx.ac_no[0]	== '9' || strlen(g_pub_tx.ac_no) < 10){ /*--�ڲ���--*/
		
		strcpy(g_pub_tx.ac_id_type,"9");		 /*	�˻�����Ϊ�ڲ� */
		strcpy(g_pub_tx.ac_get_ind,"00");		/* ������δ��ȡ�ֻ�	*/
		g_pub_tx.svc_ind = 9001;
		strcpy(g_pub_tx.add_ind,"0");
		memset(g_pub_tx.sub_tx_code,0x00,sizeof(g_pub_tx.sub_tx_code));
		strcpy(g_pub_tx.sub_tx_code,"A016");
		set_zd_data(DC_SUB_TX_CODE,g_pub_tx.sub_tx_code);
		set_zd_data("1204","01");
		set_zd_data("1205","2");
		set_zd_double("1208",g_pub_tx.tx_amt1);
	}else{
		strcpy(g_pub_tx.ac_id_type,"1");		 /*	�˻�����Ϊ���� */
		strcpy(g_pub_tx.ac_get_ind,"00");		/* ������δ��ȡ�ֻ�	*/
		g_pub_tx.svc_ind = 1001;
		strcpy(g_pub_tx.add_ind,"0"); 
		memset(g_pub_tx.sub_tx_code,0x00,sizeof(g_pub_tx.sub_tx_code));
		strcpy(g_pub_tx.sub_tx_code,"D003");
		set_zd_data(DC_SUB_TX_CODE,g_pub_tx.sub_tx_code);
		strcpy(g_pub_tx.prt_ind,"0");
		set_zd_data("102O","0");
		set_zd_data("102J","01");
		set_zd_data("102K","2");
		set_zd_double("102F",g_pub_tx.tx_amt1);
		set_zd_double("102P",0.00);
		set_zd_double("102Q",0.00);
	}
	strcpy(g_pub_tx.ct_ind,"2");
	strcpy(g_pub_tx.ac_wrk_ind,"001001001");
	strcpy(g_pub_tx.hst_ind,"1");
	strcpy(g_pub_tx.brf,"���˳���");
	iRet = pub_acct_trance();
	if (iRet){
		sprintf(acErrMsg,"���ô�ȡ������ʧ��[%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}
	iRet = pubf_acct_proc(g_pub_tx.sub_tx_code);
	if (iRet){
		sprintf(acErrMsg,"���ô�ȡ������ʧ��[%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s],[%d],��ͻ��ˣ��跽���ɹ�!",__FILE__,__LINE__);
	/***��***/
	g_pub_tx.ac_id = 0;
	g_pub_tx.ac_seqn = 0;
	strcpy(g_pub_tx.beg_note_no,"");
	memset(g_pub_tx.ac_no,0x00,sizeof(g_pub_tx.ac_no));
	strcpy(g_pub_tx.ac_no,stCompare.snd_acc_no);
	g_pub_tx.tx_amt1 = -1*stCompare.tx_amt;
	if(!memcmp(g_pub_tx.ac_no,"41403",5))
	{
		strcpy(g_pub_tx.opn_br_no,"10001");
	}
	if (g_pub_tx.ac_no[0] == '9' || strlen(g_pub_tx.ac_no) <10){/*--�ڲ��� --*/
		
		strcpy(g_pub_tx.ac_id_type,"9");		 /*	�˻�����Ϊ�ڲ� */
		strcpy(g_pub_tx.ac_get_ind,"00");		/* ������δ��ȡ�ֻ�	*/
		g_pub_tx.svc_ind = 9001;
		strcpy(g_pub_tx.add_ind,"1"); 
		memset(g_pub_tx.sub_tx_code,0x00,sizeof(g_pub_tx.sub_tx_code));
		strcpy(g_pub_tx.sub_tx_code,"A017");
		set_zd_data(DC_SUB_TX_CODE,g_pub_tx.sub_tx_code);
		set_zd_data("1214","01");
		set_zd_data("1215","2");
		set_zd_double("1218",g_pub_tx.tx_amt1);
	}else{
		strcpy(g_pub_tx.ac_id_type,"1");		 /*	�˻�����Ϊ���� */
		strcpy(g_pub_tx.ac_get_ind,"00");		/* ������δ��ȡ�ֻ�*/
		g_pub_tx.svc_ind = 1001;
		strcpy(g_pub_tx.add_ind,"1"); 
		memset(g_pub_tx.sub_tx_code,0x00,sizeof(g_pub_tx.sub_tx_code));
		strcpy(g_pub_tx.sub_tx_code,"D099");
		set_zd_data(DC_SUB_TX_CODE,g_pub_tx.sub_tx_code);
		
		strcpy(g_pub_tx.prt_ind,"0");
		set_zd_data("1017","0");
		
		set_zd_data("101A","01");
		set_zd_data("101B","2");
		set_zd_double("1013",g_pub_tx.tx_amt1);
	}
	strcpy(g_pub_tx.ct_ind,"2");
	strcpy(g_pub_tx.ac_wrk_ind,"001001001");
	strcpy(g_pub_tx.hst_ind,"1");
	strcpy(g_pub_tx.brf,"���˳���");
	iRet = pub_acct_trance();
	if (iRet){
		sprintf(acErrMsg,"���ô�ȡ������ʧ��[%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}
	iRet = pubf_acct_proc(g_pub_tx.sub_tx_code);
	if (iRet){
		sprintf(acErrMsg,"���ô�ȡ������ʧ��[%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}
	
	g_pub_tx.ac_id = 0;
	g_pub_tx.ac_seqn = 0;
	g_pub_tx.tx_amt1 = 0.00;		/*������*/
	strcpy(g_pub_tx.ac_id_type,"0");		 /*	�˻�������Ϊ���� */
	
	/*add  fupengfei  2014/6/30 21:22:44 �����ѡ����ɳ���begin*/
	if (stCompare.chrgflag[0] == '1')
	{
		/******��������*******/
		memset(g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code));
		memset(g_pub_tx.brf, 0x0, sizeof(g_pub_tx.brf));
		memset(g_pub_tx.ac_no, 0x0, sizeof(g_pub_tx.ac_no));
		memset(g_pub_tx.add_ind, 0x0, sizeof(g_pub_tx.add_ind));
		g_pub_tx.ac_id=0;
		g_pub_tx.ac_seqn=0;
		strncpy(g_pub_tx.tx_br_no,stCompare.tx_br_no,sizeof(g_pub_tx.tx_br_no)-1);
		strncpy(g_pub_tx.opn_br_no,stCompare.tx_br_no,sizeof(g_pub_tx.opn_br_no)-1);
	
		vtcp_log("[%s],[%d],���Ƚ��׻��� 2g_pub_tx.tx_br_no=[%s]!",__FILE__,__LINE__,g_pub_tx.tx_br_no);
		vtcp_log("[%s],[%d],���ȿ������� 2g_pub_tx.opn_br_no=[%s]!",__FILE__,__LINE__,g_pub_tx.opn_br_no);
		/*by YangMeng 20140630 end*/
		strcpy(g_pub_tx.ac_no, stCompare.feeaccno); /* �����ѿ�Ŀ */
		g_pub_tx.tx_amt1 = -1*stCompare.chrg_amt ;
		strcpy(g_pub_tx.ac_id_type, "9");	/* �˻�����Ϊ�ڲ� */
		strcpy(g_pub_tx.ac_get_ind, "00");
		g_pub_tx.svc_ind = 9001;
		strcpy(g_pub_tx.add_ind, "1");
	
		strcpy(g_pub_tx.sub_tx_code, "A017");
	
		set_zd_data("1214","01");
		set_zd_data("1215","2");
		set_zd_double("1218",g_pub_tx.tx_amt1);
		strcpy(g_pub_tx.brf,"����������");/***ժҪ***/
		strcpy(g_pub_tx.ct_ind,"2");/**��ת��־**/
		strcpy(g_pub_tx.ac_wrk_ind,"001001001");
		strcpy(g_pub_tx.hst_ind,"1");/***�ռ�����ϸ***/
		
		iRet = pub_acct_trance();
		if (iRet) {
			sprintf(acErrMsg, "���ô�ȡ������ʧ��[%s]", g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
		iRet = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (iRet) {
			sprintf(acErrMsg, "���ô�ȡ������ʧ��[%s]", g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
		vtcp_log("[%s],[%d],g_pub_tx.reply=[%s]!",__FILE__,__LINE__,g_pub_tx.reply);
		
		/********��������*************/
		memset(g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code));
		memset(g_pub_tx.brf, 0x0, sizeof(g_pub_tx.brf));
		memset(g_pub_tx.ac_no, 0x0, sizeof(g_pub_tx.ac_no));
		memset(g_pub_tx.add_ind, 0x0, sizeof(g_pub_tx.add_ind));
		g_pub_tx.ac_id=0;
		
		if (stCompare.chrg_ind[0] == '2') /*�����ѣ�ת��*/
		{
			strcpy(g_pub_tx.ac_no, stCompare.main_acc_no); 
		}
		else if (stCompare.chrg_ind[0] == '1') /*�����ѣ��ֽ𲻳���*/
		{
			/*��ȷ��*/
		}
		g_pub_tx.tx_amt1 = -1*stCompare.chrg_amt;
		if (g_pub_tx.ac_no[0]	== '9' || strlen(g_pub_tx.ac_no) < 10){ /*--�ڲ���--*/
		
		strcpy(g_pub_tx.ac_id_type,"9");		 /*	�˻�����Ϊ�ڲ� */
		strcpy(g_pub_tx.ac_get_ind,"00");		/* ������δ��ȡ�ֻ�	*/
		g_pub_tx.svc_ind = 9001;
		strcpy(g_pub_tx.add_ind,"0");
		memset(g_pub_tx.sub_tx_code,0x00,sizeof(g_pub_tx.sub_tx_code));
		strcpy(g_pub_tx.sub_tx_code,"A016");
		set_zd_data(DC_SUB_TX_CODE,g_pub_tx.sub_tx_code);
		set_zd_data("1204","01");
		set_zd_data("1205","2");
		set_zd_double("1208",g_pub_tx.tx_amt1);
		}else{
			strcpy(g_pub_tx.ac_id_type,"1");		 /*	�˻�����Ϊ���� */
			strcpy(g_pub_tx.ac_get_ind,"00");		/* ������δ��ȡ�ֻ�	*/
			g_pub_tx.svc_ind = 1001;
			strcpy(g_pub_tx.add_ind,"0"); 
			memset(g_pub_tx.sub_tx_code,0x00,sizeof(g_pub_tx.sub_tx_code));
			strcpy(g_pub_tx.sub_tx_code,"D003");
			set_zd_data(DC_SUB_TX_CODE,g_pub_tx.sub_tx_code);
			strcpy(g_pub_tx.prt_ind,"0");
			set_zd_data("102O","0");
			set_zd_data("102J","01");
			set_zd_data("102K","2");
			set_zd_double("102F",g_pub_tx.tx_amt1);
			set_zd_double("102P",0.00);
			set_zd_double("102Q",0.00);
		}
		strcpy(g_pub_tx.ct_ind,"2");
		strcpy(g_pub_tx.ac_wrk_ind,"001001001");
		strcpy(g_pub_tx.hst_ind,"1");
		strcpy(g_pub_tx.brf,"���˳���");
	
		iRet = pub_acct_trance();
		if (iRet) {
			sprintf(acErrMsg, "���ô�ȡ������ʧ��[%s]", g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
		iRet = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (iRet) {
			sprintf(acErrMsg, "���ô�ȡ������ʧ��[%s]", g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	}
	
	if(!memcmp(stCompare.syscode,"HVPS",4))
	{
		strcpy(sGdkm,DEWZGD);
	}
	if(!memcmp(stCompare.syscode,"BEPS",4))
	{
		strcpy(sGdkm,XEWZGD);
	}
	{
		memset(g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code));
		memset(g_pub_tx.brf, 0x0, sizeof(g_pub_tx.brf));
		memset(g_pub_tx.ac_no, 0x0, sizeof(g_pub_tx.ac_no));
		memset(g_pub_tx.add_ind, 0x0, sizeof(g_pub_tx.add_ind));

		memset(g_pub_tx.opn_br_no, 0x0, sizeof(g_pub_tx.opn_br_no));
		strncpy(g_pub_tx.opn_br_no, stCompare.tx_br_no ,sizeof(g_pub_tx.opn_br_no)-1);

		vtcp_log("%s,%d,g_pub_tx.opn_br_no=[%s]!",__FILE__,__LINE__,g_pub_tx.opn_br_no);
		strcpy(g_pub_tx.brf,"���˳���");
		
		g_pub_tx.svc_ind=9001;
		strcpy(g_pub_tx.sub_tx_code, "A016");
		strcpy(g_pub_tx.ac_id_type,"9");
		g_pub_tx.ac_seqn=0;

		strcpy(g_pub_tx.ac_no,sGdkm);
		g_pub_tx.tx_amt1=-1*stCompare.tx_amt;
		strcpy(g_pub_tx.add_ind,"0"); 
		strcpy(g_pub_tx.hst_ind,"1"); /**�ռ�����ϸ**/
		strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
		strcpy(g_pub_tx.ac_get_ind, "00");
		strcpy(g_pub_tx.note_type,"");
		strcpy(g_pub_tx.beg_note_no,"");
	
		if ( pub_acct_trance() )
		{
			sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
			WRITEMSG
			goto ErrExit;
		}

		set_zd_data("1204","01");
		set_zd_data("1205",g_pub_tx.ct_ind);
		set_zd_double("1208",g_pub_tx.tx_amt1);

		if ( pubf_acct_proc(g_pub_tx.sub_tx_code) )
		{
			sprintf(acErrMsg,"�Ǽǻ����ˮ����!");
			WRITEMSG
			goto ErrExit;
		}

		/* ���ɴ��� */
		memset(g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code));
		memset(g_pub_tx.brf, 0x0, sizeof(g_pub_tx.brf));
		memset(g_pub_tx.ac_no, 0x0, sizeof(g_pub_tx.ac_no));
		memset(g_pub_tx.add_ind, 0x0, sizeof(g_pub_tx.add_ind));

		g_pub_tx.svc_ind=9001;
		strcpy(g_pub_tx.sub_tx_code, "A017");
		strcpy(g_pub_tx.ac_id_type,"9");
		g_pub_tx.ac_seqn=0;

		strcpy(g_pub_tx.ac_no,sGdkm);
		g_pub_tx.tx_amt1=-1*stCompare.tx_amt;
		strcpy(g_pub_tx.add_ind,"1"); 
		strcpy(g_pub_tx.hst_ind,"1"); /**�ռ�����ϸ**/
		strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
		strcpy(g_pub_tx.ac_get_ind, "00");
		strcpy(g_pub_tx.note_type,"");
		strcpy(g_pub_tx.beg_note_no,"");
	
		if ( pub_acct_trance() )
		{
			sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
			WRITEMSG
			goto ErrExit;
		}

		set_zd_data("1214","01");
		set_zd_data("1215",g_pub_tx.ct_ind);
		set_zd_double("1218",g_pub_tx.tx_amt1);

		if ( pubf_acct_proc(g_pub_tx.sub_tx_code) )
		{
			sprintf(acErrMsg,"�Ǽǻ����ˮ����!");
			WRITEMSG
			goto ErrExit;
		}
	}
	/*add  fupengfei  2014/6/30 21:22:44 �����ѡ����ɳ��� end*/
	vtcp_log("[%s],[%d],���˺���ִ�������𣿣�compare_exit_acct��!",__FILE__,__LINE__);
	vtcp_log("[%s],[%d],����,tx_date=[%d]trace_no=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date,g_pub_tx.trace_no);
	
	OkExit:
		vtcp_log("��%s��,��%d��,found g_pub_tx.reply=[%s]",__FILE__,__LINE__,g_pub_tx.reply);
		return 0;
	ErrExit:
		return 1;
}
int iWriteFile(FILE	*new_fp,struct chnl_compare_acct_c stCompare)
{
	fprintf(new_fp, "%d|",stCompare.detail_no);
	fprintf(new_fp, "%s|",stCompare.un_marking);
	fprintf(new_fp, "%s|",stCompare.syscode);
	fprintf(new_fp, "%s|",stCompare.code_type);
	fprintf(new_fp, "%s|",stCompare.com_stat);
	fprintf(new_fp, "%s|",stCompare.com_flag);
	fprintf(new_fp, "%d|",stCompare.tx_date);
	fprintf(new_fp, "%d|",stCompare.trace_no);
	fprintf(new_fp, "%s|",stCompare.gz_br_no);
	fprintf(new_fp, "%s|",stCompare.gz_acct_no);
	fprintf(new_fp, "%s|",stCompare.gz_reasen);
	fprintf(new_fp, "%s|",stCompare.sndrcvflag);
	fprintf(new_fp,"\n");
	vtcp_log("��%s��,��%d��,д�����ļ����!",__FILE__,__LINE__);
	return 0;
}

