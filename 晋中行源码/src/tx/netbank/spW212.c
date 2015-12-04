/*********************************************************************
* �� �� �� :  
* �������� :  lixiang
* ��    �� :  2014/11/01
* ����ģ�� :  ������¼��ϸ��ѯ
* �������� :  spW212.c
* �������� :
* �����б� :
* ʹ��ע�� :
�������ã�
insert into tx_def values ('5063', '������¼��ϸ��ѯ', '10000000000000000000110010010011100000111111111110000000000001000111111100000000000110000000100000011000000000000000000000000000', '0','5', '2');
insert into tx_flow_def values ('5063', 0, 'W212', '#$');
insert into tx_sub_def values ('W212', '������¼��ϸ��ѯ', 'spW212', '0', '0');
***********************************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "ln_mst_c.h"
#include "ln_parm_c.h"
#include "ln_mst_hst_c.h"

int spW212()
{
	struct mdm_ac_rel_c mdm_ac_rel_1;	/*�������˻���ϵ*/
	struct mdm_ac_rel_c mdm_ac_rel_2;	/*�������˻���ϵ*/
	struct ln_mst_c	ln_mst_1;		/*�������ļ�*/
	struct ln_mst_hst_c ln_mst_hst_1;	/*������ϸ*/
	
	char loanAccount[22];		/*�����˺�*/
	char quryType[2];		/*��ѯ����*/
	char beginDate[10];		/*��ʼ����*/
	char endDate[10];		/*��ֹ����*/
	char turnPageBeginPos[10];	/*��ѯ��ʼ��¼��*/
	char turnPageShowNum[10];	/*�����¼��*/
	char sw_traceno[12];		/*ƽ̨��ˮ*/
	char tx_date[12];		/*ƽ̨����*/
	char whereList[1024];		/*��ѯ����*/
	char buf[256];
	char filename[60];
	char pcfilename[256];
	char cFtpfile[256];
	FILE *fp=NULL;
	int ret;
	int cnt=1;
	int Product=0;
	int recordNo_1=0;
	int recordNo=0;		/*��¼��*/
	
	memset(&mdm_ac_rel_1,0x00,sizeof(mdm_ac_rel_1));
	memset(&mdm_ac_rel_2,0x00,sizeof(mdm_ac_rel_2));
	memset(&ln_mst_1,0x00,sizeof(ln_mst_1));
	memset(&ln_mst_hst_1,0x00,sizeof(ln_mst_hst_1));
	memset(loanAccount,0x00,sizeof(loanAccount));
	memset(quryType,0x00,sizeof(quryType));
	memset(beginDate,0x00,sizeof(beginDate));
	memset(endDate,0x00,sizeof(endDate));
	memset(turnPageBeginPos,0x00,sizeof(turnPageBeginPos));
	memset(turnPageShowNum,0x00,sizeof(turnPageShowNum));
	memset(sw_traceno, 0x00, sizeof(sw_traceno));
	memset(tx_date, 0x00, sizeof(tx_date));
	memset(whereList, 0x00, sizeof(whereList));
	memset(buf, 0x00, sizeof(buf));
	memset(filename,0x00,sizeof(filename));
	memset(pcfilename,0x00,sizeof(pcfilename));
	memset(cFtpfile,0x00,sizeof(cFtpfile));
	
	/**��ʼ�����ýṹ**/
	if (pub_base_sysinit())
	{
	    vtcp_log("��ʼ�����ýṹ����!!");
	    strcpy(g_pub_tx.reply, "CU06");
	    goto ErrExit;
	}
	
	get_zd_data("0380",loanAccount);
	get_zd_data("0050",tx_date);
	get_zd_data("0520",sw_traceno);
	get_zd_data("0440",beginDate);
	get_zd_data("0450",endDate);
	vtcp_log("%s,%d �����˺�=[%s]\n",__FILE__,__LINE__,loanAccount);
	vtcp_log("%s %d ::ƽ̨����tx_date=%s",__FILE__,__LINE__,tx_date);
	vtcp_log("%s %d ::ƽ̨��ˮsw_traceno=%s",__FILE__,__LINE__,sw_traceno);
	vtcp_log("%s %d ::��ʼ����beginDate=%s",__FILE__,__LINE__,beginDate);
	vtcp_log("%s %d ::��ֹ����endDate=%s",__FILE__,__LINE__,endDate);
	
	/*��ѯ��ʼ��¼��*/
	get_zd_data("0280",turnPageBeginPos);
	if(!strlen(turnPageBeginPos))
	{
		strcpy(g_pub_tx.reply,"9999");
		set_zd_data("0130","��ѯ��ʼֵ����");
		vtcp_log("%s %d ::[%s]��ѯ��ʼֵ����turnPageBeginPos=%ld",__FILE__,__LINE__,g_pub_tx.reply,turnPageBeginPos);
		goto ErrExit;
	}
	vtcp_log("%s %d ::��ѯ��ʼ��¼��turnPageBeginPos=%s",__FILE__,__LINE__,turnPageBeginPos);

	/*�����¼��*/
	get_zd_data("0290",turnPageShowNum);	
	if(!strlen(turnPageShowNum))
	{
		strcpy(g_pub_tx.reply,"9999");
		set_zd_data("0130","�����¼������");
		vtcp_log("%s %d ::[%s]�����¼������turnPageShowNum=%s",__FILE__,__LINE__,g_pub_tx.reply,turnPageShowNum);
		goto ErrExit;
	}
	/* �����ļ� */
	sprintf( filename,"5063netbank%s%s",tx_date,sw_traceno );
	sprintf( pcfilename,"%s/%s",getenv("FILDIR"),filename );
	vtcp_log("%s %d:: pcfilename[%s]  ",__FILE__,__LINE__,pcfilename);
	fp=fopen(pcfilename,"w");
	if( fp==NULL )
	{
	    vtcp_log("%s %d::open file [%s] error ",__FILE__,__LINE__,pcfilename);
	    strcpy(g_pub_tx.reply,"P157");
	    goto ErrExit;
	}
	/*�ļ�ͷ*/
	fprintf(fp, "�����˺�|�����|��������|����ʱ��|������|���|��ת��־|�����˺�|ժҪ|\n");
	
	/*���ݴ����˺Ż�ȡ�����˺�ID;mdm_ac_rel_1.ac_id*/
	ret = -1;
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_1,"ac_no='%s'",loanAccount);
	if(ret)
	{
		vtcp_log("%s %d::Mdm_ac_rel_Sel [%d] error ",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"W011");
		goto ErrExit;	
	}
	
	/*ͨ�������˻�ID��ȡ�����˺�ID�ʹ����:ln_mst_1.repay_ac_id��ln_mst_1.name*/
	ret=-1;
	ret=Ln_mst_Sel(g_pub_tx.reply,&ln_mst_1,"ac_id='%d'",mdm_ac_rel_1.ac_id);
	if(ret==100)
	{
		set_zd_data("0120","D004");
		set_zd_data("0130","���˺�Ϊ�Ǵ����˺�");
		goto ErrExit;
	}
	else if(ret)
	{
		vtcp_log("%s %d::Mdm_ac_rel_Sel [%d] error ",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"W011");
		goto ErrExit;	
	}
	
	/*ͨ�������˺�ID��ȡ�����˺�mdm_ac_rel_2.ac_no*/
	ret = -1;
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_2,"ac_id='%d'",ln_mst_1.repay_ac_id);
	if(ret)
	{
		vtcp_log("%s %d::Mdm_ac_rel_Sel [%d] error ",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"W011");
		goto ErrExit;	
	}
	
	/*==================���ɲ�ѯ����whereList*=================*/
	
	/*����ʱ������־λ0*/
	sprintf(buf,"ac_id='%ld' and add_ind='0' ",mdm_ac_rel_1.ac_id);
	strcat( whereList,buf);
	
	/*��ѯ��ʼ����*/
	if(strlen(beginDate))
	{
	memset(buf, 0x00, sizeof(buf));
	sprintf( buf,"  and tx_date>='%s' ",beginDate );
	strcat( whereList,buf);
	}
	
	/*��ѯ��ֹ����*/
	if(strlen(endDate))
	{
	memset(buf, 0x00, sizeof(buf));
	sprintf( buf,"  and tx_date<='%s' ",endDate );
	strcat( whereList,buf);
	}
	
	/*��ȡ��ѯ����*/
	get_zd_data("0310",quryType);
	if(strlen(quryType))
	{
		if(strcmp(quryType,"1")==0)
		{
			memset(buf, 0x00, sizeof(buf));
			sprintf( buf,"  and ln_tx_type='0' order by tx_date desc , tx_time desc");
			strcat( whereList,buf);
		}
		else if(strcmp(quryType,"2")==0)
		{
			memset(buf, 0x00, sizeof(buf));
			sprintf( buf,"  and ln_tx_type in ('1','2') order by tx_date desc , tx_time desc");
			strcat( whereList,buf);	
		}
		else
		{
			strcpy(g_pub_tx.reply,"9999");
			set_zd_data("0130","��ѯ��������");
			vtcp_log("%s %d ::[%s]��ѯ��������turnPageBeginPos=%ld",__FILE__,__LINE__,g_pub_tx.reply,quryType);
			goto ErrExit;
			
		}
	}	
	
	/*ͨ�������˻�ID��ȡ�����¼��ϸ*/
	recordNo_1=sql_count("ln_mst_hst",whereList);
		set_zd_int("0450",recordNo_1);
		vtcp_log("[%s][%d]::������¼��ϸ��¼����Ϊ[%d]",__FILE__,__LINE__,recordNo_1);
		if ( recordNo_1 == 0 )
		{
			vtcp_log("[%s][%d]::������¼��ϸ��¼����Ϊ0,ֱ�ӷ���.",__FILE__,__LINE__);
			fclose(fp);
			goto OkExit;	
		}
	ret = -1;
	ret=Ln_mst_hst_Dec_Sel(g_pub_tx.reply,whereList);
	if(ret)
	{
		vtcp_log("%s %d::Ln_mst_Dec_Sel [%d] error ",__FILE__,__LINE__,ret);
	 	strcpy(g_pub_tx.reply,"D102");
	 	set_zd_data("0130","�����α����");
	 	fclose(fp);
	 	goto ErrExit;		
	}
	else
	{
		while(1)
		{	
			ret=-1;
			ret=Ln_mst_hst_Fet_Sel(&ln_mst_hst_1,g_pub_tx.reply);
			if(ret == 100)
			{
				vtcp_log("%s %d::ȡ���ڴ����ϸ�α����[%d]",__FILE__,__LINE__,ret);
				break;	
			}
			 else if(0 != ret && 100 != ret)
			{
				vtcp_log("%s %d::Ln_mst_Fet_Sel error[%d]",__FILE__,__LINE__,ret);
				strcpy(g_pub_tx.reply,"D001");
				set_zd_data("0130","��ѯ����");
				Ln_mst_hst_Clo_Sel();
				fclose(fp);
				goto ErrExit;
			}
			else
	 		{
	 			if(cnt < atoi(turnPageBeginPos))
	 			{
	 				cnt++;
	 				continue;	
	 			}
	 			if(!strcmp(ln_mst_hst_1.ct_ind,"1"))
	 			{
	 				/*д�ļ�:�����˺�|�����|��������|����ʱ��|������|���|��ת��־|�����˺�|ժҪ|*/
					fprintf(fp, "%20s|%60s|%ld|%ld|%16.2f|%16.2f|%2s|%20s|%25s|\n",\
					loanAccount,ln_mst_1.name,ln_mst_hst_1.tx_date,ln_mst_hst_1.tx_time,ln_mst_hst_1.tx_amt,\
					ln_mst_hst_1.bal,ln_mst_hst_1.ct_ind,"�ֽ𻹿�",ln_mst_hst_1.brf);
	 					
	 			}
	 			else
	 			{
					/*д�ļ�:�����˺�|�����|��������|����ʱ��|������|���|��ת��־|�����˺�|ժҪ|*/
					fprintf(fp, "%20s|%60s|%ld|%ld|%16.2f|%16.2f|%2s|%20s|%25s|\n",\
					loanAccount,ln_mst_1.name,ln_mst_hst_1.tx_date,ln_mst_hst_1.tx_time,ln_mst_hst_1.tx_amt,\
					ln_mst_hst_1.bal,ln_mst_hst_1.ct_ind,mdm_ac_rel_2.ac_no,ln_mst_hst_1.brf);
				}
				/*��¼��*/
	 			++recordNo; 
	 			vtcp_log("%s %d::��¼��recordNo=%d",__FILE__,__LINE__,recordNo);
	 			if(recordNo >= atoi(turnPageShowNum))
	 			{
	 				break;	
	 			}
			}
		}
	}
	Ln_mst_hst_Clo_Sel();
	fclose(fp);
	fp=NULL;
	
	/*ftp�����ļ�*/
	vtcp_log("%s,%d filename=[%s]\n",__FILE__,__LINE__,filename);
	set_zd_data("0260",filename);
	
	sprintf(cFtpfile,"%s/bin/ftp_to_nsw.sh %s %s %s",getenv("WORKDIR"),get_env_info("NSW_FTP_USER"),\
		get_env_info("NSW_FTP_PWD"),filename);
	vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
	ret = -1;
	ret=system(cFtpfile);
	if (ret)
	{
		vtcp_log("[%s][%d] ftp�ļ�[%s]��ƽ̨����[%d]", __FILE__, __LINE__,filename,ret);
		strcpy(g_pub_tx.reply,"9999");
		set_zd_data("0130","�ļ����ݳ���");
		goto ErrExit;
	}
	
	vtcp_log("[%s][%d] ftp�ļ�[%s]��ƽ̨�ɹ�", __FILE__, __LINE__,filename);
	
   
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data("0120","0000");
	set_zd_data("0130"," ������¼��ϸ��ѯ�ɹ�");
	vtcp_log("success !!: 5063 reply is[%s]", g_pub_tx.reply);
	return 0;
ErrExit:
	vtcp_log("fail !!: reply is[%s]", g_pub_tx.reply);
	return 1;
}
