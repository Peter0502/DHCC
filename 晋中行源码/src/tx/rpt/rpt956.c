/*************************************************************
* �� �� ��: rpt956.c
* �������������ж������˻���ҳ��
*
* ��    ��: z.s.wang
* �������: 2012��03��09��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
insert into com_eod_exec values ('6005','R956','rpt956','�������˻���ҳ��','1111111111','1','01','01','2','Y','Y','N','2','Y');
**************************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define  EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "com_branch_c.h"
#include "xqdz_c.h"
#include "sub_dd_mst_c.h"
#include "sub_dd_mst_hst_c.h"

#define MAX_CNT 50  /* ������ҳ����Ŀ�� */

FILE   *fp_full;
FILE   *fp_not_full;
long   tmp_bgn_date;
long   page_cnt;
long   tmp_flag;
double dD_amt;
double dJ_amt;
char   page_title[64+1];
struct com_sys_parm_c s_com_sys_parm;
struct sub_dd_mst_c   s_sub_dd_mst;
struct xqdz_c         s_xqdz;

rpt956()
{
	fp_full=NULL;
	fp_not_full=NULL;
	int ret=-1;
	int  i=0;
	int  tmp_cnt=0;
	int  tmp_page=0;
	long tmp_line=0;
	char filename[20+1];
	char filename_not_full[20+1];
	char tmp_filename_full[128+1];
	char tmp_filename_not_full[128+1];
	char old_br_no[5+1];
	char old_ac_no[20];

	memset(filename,0x00,sizeof(filename));
	memset(filename_not_full,0x00,sizeof(filename_not_full));
	memset(tmp_filename_full,0x00,sizeof(tmp_filename_full));
	memset(tmp_filename_not_full,0x00,sizeof(tmp_filename_not_full));
	memset(old_br_no,0x00,sizeof(old_br_no));
	memset(page_title,0x00,sizeof(page_title));
	memset(&s_com_sys_parm,0x00,sizeof(struct com_sys_parm_c));
	memset(&s_sub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));
	memset(&s_xqdz,0x00,sizeof(struct xqdz_c));

	strcpy( filename , "RPT956");
	strcpy( filename_not_full,"RPT956_NOT_FULL");

	ret=sql_begin(); /*������*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

	/**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();

	vtcp_log("---���ɱ���rpt956(�������˻���ҳ��)��ʼ---");
	ret=Com_sys_parm_Sel(g_pub_tx.reply,&s_com_sys_parm,"1=1");
	if(ret)
	{
		vtcp_log("��ѯϵͳ���ڳ���! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
		goto ErrExit;
	}

	tmp_bgn_date=s_com_sys_parm.lst_date/100*100+1;
	vtcp_log("�õ���ϵͳ���� sys_date:[%ld] tmp_bgn_date:[%ld] [%s] [%d]",\
		s_com_sys_parm.lst_date,tmp_bgn_date,__FILE__,__LINE__);

	ret=Sub_dd_mst_Dec_Sel(g_pub_tx.reply,"1=1 order by br_no,ac_no,sub_ac_no");
	if(ret)
	{
		vtcp_log("����Sub_dd_mst��ѯ�������� [%d] [%s] [%d]",ret,__FILE__,__LINE__);
		goto ErrExit;
	}

	while(1)
	{
		tmp_flag=0;
		dD_amt=0.0;
		dJ_amt=0.0;
		memset(&s_sub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));
		memset(page_title,0x00,sizeof(page_title));
		ret=Sub_dd_mst_Fet_Sel(&s_sub_dd_mst,g_pub_tx.reply);
		if(ret==100)
		{
			vtcp_log("��ȡ���ݽ���.[%s] [%d]",__FILE__,__LINE__);
			break;
		}
		else if(ret)
		{
			vtcp_log("��ȡ�����쳣.[%d] [%s] [%d]",ret,__FILE__,__LINE__);
			goto ErrExit;
		}
		page_cnt=s_sub_dd_mst.hst_pg;
		if(tmp_bgn_date%10000==101)
		{
			page_cnt=0;
		}
		if(strcmp(old_br_no,s_sub_dd_mst.br_no))
		{
			if(fp_full != NULL) fclose(fp_full);
			if(fp_not_full != NULL) fclose(fp_not_full);
			memset(tmp_filename_full,0x00,sizeof(tmp_filename_full));
			memset(tmp_filename_not_full,0x00,sizeof(tmp_filename_not_full));
			sprintf(tmp_filename_full,"%s/report/%s/%s.txt",getenv("HOME"),s_sub_dd_mst.br_no,filename);
			sprintf(tmp_filename_not_full,"%s/report/%s/%s.txt",getenv("HOME"),s_sub_dd_mst.br_no,filename_not_full);
			vtcp_log("filename_full is:[%s] [%s] [%d]",tmp_filename_full,__FILE__,__LINE__);
			vtcp_log("filename_not_full is:[%s] [%s] [%d]",tmp_filename_not_full,__FILE__,__LINE__);
			pub_rpt_rmfile(s_sub_dd_mst.br_no,filename,0 );
			pub_rpt_rmfile(s_sub_dd_mst.br_no,filename_not_full,0 );
			strcpy(old_br_no,s_sub_dd_mst.br_no);
		}

		if(strcmp(old_ac_no,s_sub_dd_mst.ac_no))
		{
			memset(&s_xqdz,0x00,sizeof(struct xqdz_c));
			ret=Xqdz_Sel(g_pub_tx.reply,&s_xqdz,"xqdz100='%s'",s_sub_dd_mst.ac_no);
			if(ret)
			{
				vtcp_log("��ѯС����Ϣ�쳣! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
				goto ErrExit;
			}
		}

		tmp_cnt=sql_count("sub_dd_mst_hst","sub_ac_no='%s' and tx_date>=%ld  and tx_date <=%ld",\
			s_sub_dd_mst.sub_ac_no,tmp_bgn_date,s_com_sys_parm.lst_date);

		if(tmp_cnt==0)
		{
			vtcp_log("û�����������ļ�¼,[%s] [%s] [%d]",s_sub_dd_mst.sub_ac_no,__FILE__,__LINE__);
			continue;
		}
		else if(tmp_cnt<MAX_CNT)
		{
			if(fp_not_full == NULL)
			{
				fp_not_full=fopen(tmp_filename_not_full,"w+");
			}

			strcpy(page_title,"                            ��������δ��ҳ��\n");
			/* �����ֱ�Ӵ�ӡ��not_full�ļ����� */
			ret=rpt956_print(fp_not_full,s_sub_dd_mst.sub_ac_no,0,&tmp_line);
			if(ret)
			{
				vtcp_log("��ӡ���������ĵ��������ʳ���! [%s] [%d]",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"L207");
				goto ErrExit;
			}
		}
		else
		{
			tmp_line=0;
			tmp_page=tmp_cnt/MAX_CNT;

			if(fp_full == NULL)
			{
				fp_full=fopen(tmp_filename_full,"w+");
			}
			if(tmp_cnt%MAX_CNT != 0 && fp_not_full == NULL)
			{
				fp_not_full=fopen(tmp_filename_not_full,"w+");
			}

			strcpy(page_title,"                            ����������ҳ��\n");

			for(i=0;i<tmp_page;i++)
			{
				/* ��Ŀ����MAX_CNT �ʹ�ӡһҳ */
				ret=rpt956_print(fp_full,s_sub_dd_mst.sub_ac_no,tmp_line,&tmp_line);
				if(ret)
				{
					vtcp_log("��ӡ���������ĵ��������ʳ���! [%s] [%d]",__FILE__,__LINE__);
					strcpy(g_pub_tx.reply,"L207");
					goto ErrExit;
				}
			}

			if(tmp_cnt%MAX_CNT != 0)
			{
				memset(page_title,0x00,sizeof(page_title));
				strcpy(page_title,"                            ��������δ��ҳ��\n");
				/* �����ֱ�Ӵ�ӡ��not_full�ļ����� */
				ret=rpt956_print(fp_not_full,s_sub_dd_mst.sub_ac_no,tmp_line,&tmp_line);
				if(ret)
				{
					vtcp_log("��ӡ���������ĵ��������ʳ���! [%s] [%d]",__FILE__,__LINE__);
					strcpy(g_pub_tx.reply,"L207");
					goto ErrExit;
				}
			}
		}

		if(page_cnt != s_sub_dd_mst.hst_pg)
		{
			ret=sql_execute("update sub_dd_mst set hst_pg=%ld where sub_ac_no='%s'",page_cnt,s_sub_dd_mst.sub_ac_no);
			if(ret)
			{
				vtcp_log("����sub_dd_mst��hst_pgʧ��![%d] [%s] [%d]",ret,__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"P434");
				goto ErrExit;
			}
		}
	}
	Sub_dd_mst_Clo_Sel();
	vtcp_log("---���ɱ���rpt956(�������˻���ҳ��)����---");

OkExit:
	vtcp_log("[%s][%d]�ύ���ݿ�!\n",__FILE__,__LINE__);
	sql_commit();   /*--�ύ����--*/
	vtcp_log("---���ɶ������˻���ҳ�� �ɹ� ---,[%s] [%d]",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data("0120",g_pub_tx.reply);
	return (0);

ErrExit:
	Sub_dd_mst_Clo_Sel();
	if(fp_full != NULL) fclose(fp_full);
	if(fp_not_full != NULL) fclose(fp_not_full);
	vtcp_log("---���ɶ������˻���ҳ�� ʧ�� !!! ---,[%s] [%d]",__FILE__,__LINE__);
	if( strlen(g_pub_tx.reply)==0 || !strcmp(g_pub_tx.reply,"0000"))
	{
		strcpy(g_pub_tx.reply,"L207");
	}
	set_zd_data("0120",g_pub_tx.reply);
	return (1);
}

int rpt956_print(FILE *fp,char* sub_ac_no,long icnt_bgn,long* ocnt_end)
{
	int ret=-1;
	int cnt=0;
	double dBal=0.0;
	char j_str[13+1];
	char d_str[13+1];
	struct sub_dd_mst_hst_c s_sub_dd_mst_hst;
	memset(j_str,0x00,sizeof(j_str));
	memset(d_str,0x00,sizeof(d_str));
	memset(&s_sub_dd_mst_hst,0x00,sizeof(struct sub_dd_mst_hst_c));

	ret=Sub_dd_mst_hst_Dec_Sel(g_pub_tx.reply,"sub_ac_no='%s' and tx_date>=%ld \
		and tx_date <=%ld and hst_cnt>=%ld",sub_ac_no,tmp_bgn_date,s_com_sys_parm.lst_date,icnt_bgn);
	if(ret)
	{
		vtcp_log("����Sub_dd_mst_hst����. [%d] [%s] [%d]",ret,__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D101");
		goto ErrExit;
	}

	while(1)
	{
		char ct_flag[2+1];
		char brf[64+1];
		memset(j_str,0x00,sizeof(j_str));
		memset(d_str,0x00,sizeof(d_str));
		memset(ct_flag,0x00,sizeof(ct_flag));
		memset(brf,0x00,sizeof(brf));
		struct sub_dd_mst_hst_c s_sub_dd_mst_hst;
		memset(&s_sub_dd_mst_hst,0x00,sizeof(struct sub_dd_mst_hst_c));
		ret=Sub_dd_mst_hst_Fet_Sel(&s_sub_dd_mst_hst,g_pub_tx.reply);
		if(ret==100 || cnt == MAX_CNT)
		{
			fprintf(fp,"�����������ة��������������������ة��������ة����������������ة��������������ة��������������ة��������������ة�������\n");
			fprintf(fp,"��ҳ�跽С��:%-16.2f      ��ҳ����С��:%-16.2f    \n",dJ_amt,dD_amt);
			fprintf(fp,"%c\n",'\f');
			*ocnt_end=s_sub_dd_mst_hst.hst_cnt;
			break;
		}
		else if(ret)
		{
			vtcp_log("��ȡSub_dd_mst_hst����. [%d] [%s] [%d]",ret,__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D101");
			goto ErrExit;
		}

		if(cnt==0)
		{
			page_cnt++;
			fprintf(fp,"\n");
			fprintf(fp,page_title);
			fprintf(fp,"\n");
			fprintf(fp,"С������:%-50s\n",s_xqdz.xqdz15);
			fprintf(fp,"���˻�:%-20.20s     ���˻�:%-20.20s   ����:%-30.30s   ��%3dҳ\n",s_sub_dd_mst.ac_no,s_sub_dd_mst.sub_ac_no,s_sub_dd_mst.name,page_cnt);
			fprintf(fp,"�����������Щ��������������������Щ��������Щ����������������Щ��������������Щ��������������Щ��������������Щ�������\n");
			fprintf(fp,"���������ک�    ժ        Ҫ    ��ƾ֤���੦    ƾ֤����    ��  �跽������  ��  ����������  ��   ��    ��   ����  Ա��\n");
			fprintf(fp,"�����������੤�������������������੤�������੤���������������੤�������������੤�������������੤�������������੤������\n");

			if(s_sub_dd_mst_hst.add_ind[0]=='0')
			{
				dBal=s_sub_dd_mst_hst.bal+s_sub_dd_mst_hst.tx_amt;
			}
			else
			{
				dBal=s_sub_dd_mst_hst.bal-s_sub_dd_mst_hst.tx_amt;
			}

			if(s_sub_dd_mst_hst.hst_cnt==1)
			{
			}
			else if(tmp_bgn_date%10000==101 && page_cnt==1 )
			{
				fprintf(fp,"��        ��������              ��        ��                ��              ��              ��%14.2f��      ��\n",dBal);
			}
			else
			{
				if(tmp_flag==0)
				{
					fprintf(fp,"��        ������ҳ              ��        ��                ��              ��              ��%14.2f��      ��\n",dBal);
				}
				else
				{
					fprintf(fp,"��%8d������ҳ              ��        ��                ��%14.2f��%14.2f��%14.2f��      ��\n",s_sub_dd_mst_hst.tx_date,dJ_amt,dD_amt,dBal);
				}
			}
		}
		if(s_sub_dd_mst_hst.ct_ind[0]=='1')
		{
			strcpy(ct_flag,"��");
		}
		else if(s_sub_dd_mst_hst.ct_ind[0]=='2')
		{
			strcpy(ct_flag,"ת");
		}

		if(s_sub_dd_mst_hst.add_ind[0]=='0')
		{
			sprintf(j_str,"%s%12.2f",ct_flag,s_sub_dd_mst_hst.tx_amt);
			dJ_amt+=s_sub_dd_mst_hst.tx_amt;
		}
		else
		{
			sprintf(d_str,"%s%12.2f",ct_flag,s_sub_dd_mst_hst.tx_amt);
			dD_amt+=s_sub_dd_mst_hst.tx_amt;
		}

		Del_half_wrod(s_sub_dd_mst_hst.brf,brf);
		fprintf(fp,"��%8d��%-20.20s��%8.8s��%16.16s��%14.14s��%14.14s��%14.2f��%6.6s��\n",
		s_sub_dd_mst_hst.tx_date,brf,s_sub_dd_mst_hst.note_type,s_sub_dd_mst_hst.note_no,j_str,d_str,s_sub_dd_mst_hst.bal,s_sub_dd_mst_hst.tel);
		cnt++;
	}
	tmp_flag++;
	Sub_dd_mst_hst_Clo_Sel();

OkExit:
	return(0);
ErrExit:
	Sub_dd_mst_hst_Clo_Sel();
	return(-1);
}
