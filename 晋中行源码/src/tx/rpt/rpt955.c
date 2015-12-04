/*************************************************************
* �� �� ��: rpt955.c
* �������������ж������˻���Ϣ�ͻ��ص�
*
* �������: 2012��03��05��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:

insert into com_eod_exec values('1730','R955','rpt955','�������˻���Ϣ�ͻ��ص�','1111111111','5','3','21','21','2','Y','Y','N','2','Y');
**************************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define  EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "com_branch_c.h"
#include "sub_intst_hst_c.h"
#include "dd_parm_c.h"
#include "dc_acc_rel_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "sub_dd_mst_c.h"

struct com_sys_parm_c  s_com_sys_parm;
struct sub_intst_hst_c ssub_intst_hst;
struct sub_intst_hst_c osub_intst_hst;
struct mdm_ac_rel_c    s_mdm_ac_rel;
struct dd_mst_c        s_dd_mst;
struct sub_dd_mst_c    s_sub_dd_mst;

char cr_acc_hrt[8];
char dr_acc_hrt[8];

int  page=0;
int  line=0;
int  line_file=0;
char vpfm[21];
char t_str[41];
int  opncnt;
long dBeg_date=0;
FILE *fp;
int  ret = 1;

int get_ratio_cxxhqd(int bli,int bll,char str[100]);

rpt955()
{
	int flag=0;
	int ttlnum;
	long tmp_date;
	int dMon=0;
	int dYear=0;
	int dDay=0;
	char fname[100];
	char filename[100];
	char wherelist[1024];  /**��ѯ����**/
	char tmpstr[100];
	char tmp_str[60];
	char old_br_no[6];
	long intst_date=0;

	memset( wherelist,0,sizeof(wherelist) );
	memset( &osub_intst_hst,0,sizeof(osub_intst_hst));
	memset( &s_com_sys_parm , 0x00 , sizeof(struct com_sys_parm_c));
	strcpy( old_br_no , "*****" );

	ret=sql_begin(); /*������*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

	/**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();

	strcpy( filename , "RPT955");
	strcpy(vpfm, "RPT955");
	dMon=g_pub_tx.tx_date/100%100;
	dYear=g_pub_tx.tx_date/10000;
	dDay=g_pub_tx.tx_date%100;
	if(dMon%3>0)
	{
		vtcp_log("%s,%d,��ǰ�·ݷǽ�Ϣ�·ݲ�����Ϣ�嵥!",__FILE__,__LINE__);
		goto OkExit;
	}
	if(dMon/3==1)
	{
			dBeg_date=(dYear-1)*10000+12*100+21;
		}else if(dMon/3==2)
		{
			dBeg_date=dYear*10000+3*100+21;
		}else if(dMon/3==3)
		{
			dBeg_date=dYear*10000+6*100+21;
		}else{
			dBeg_date=dYear*10000+9*100+21;
		}

	ret = Sub_intst_hst_Dec_Sel(g_pub_tx.reply,"ac_ind='A' and pt_ind='1' and intst_date>%ld  and intst_date<=%ld order by br_no,ac_no,sub_ac_seqn,intst_date",dBeg_date,g_pub_tx.tx_date );
	ERR_DEAL
	ttlnum=0;
	while(1)
	{
		memset(&ssub_intst_hst,0,sizeof(ssub_intst_hst));
		memset(&s_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
		memset(&s_dd_mst,0x00,sizeof(struct dd_mst_c));
		memset(&s_sub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));
		ret = Sub_intst_hst_Fet_Sel( &ssub_intst_hst , g_pub_tx.reply );
		if( ret==100 )
		{
			fclose(fp);
			break;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"��ȡ��Ϣ���쳣����!");
			WRITEMSG
			goto ErrExit;
		}

		/* �����任ʱ */
		pub_base_strpack(ssub_intst_hst.br_no );
		if( strcmp( ssub_intst_hst.br_no , old_br_no ) )
		{
			/* ��һ������ */
			if( strcmp( old_br_no , "*****" ) )
			{
				ret = print_tail();
				fclose(fp);
			}
			strcpy( old_br_no , ssub_intst_hst.br_no );
			page=1;
			line_file=0;
			/* ɾ��ԭ�ļ� */
			ret = pub_rpt_rmfile(ssub_intst_hst.br_no , filename , 0 );
			ERR_DEAL
			/* ���ļ� */
			ret = pub_rpt_openfile( &fp, ssub_intst_hst.br_no , filename );
			ERR_DEAL
		}

		ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&s_mdm_ac_rel,"ac_no='%s'",ssub_intst_hst.ac_no);
		if ( ret == 100 )
		{
			vtcp_log("[%s][%d]�����˻����ձ�û�в鵽���˺�![%d] �˺�:[%s]",__FILE__,__LINE__,ret,ssub_intst_hst.ac_no);
			goto ErrExit;
		}
		else if( ret && ret!=100 )
		{
			vtcp_log("[%s][%d]�����˻����ձ�![ret]",__FILE__,__LINE__,ret);
			goto ErrExit;
		}

		ret = Dd_mst_Sel(g_pub_tx.reply,&s_dd_mst,"ac_seqn=%d and ac_id=%ld",s_mdm_ac_rel.ac_seqn,s_mdm_ac_rel.ac_id);
		if ( ret == 100 )
		{
			vtcp_log("[%s][%d]���ڴ�����ļ�û�в鵽��ac_id![%d] ac_id:[%s]",__FILE__,__LINE__,ret,s_mdm_ac_rel.ac_id);
			goto ErrExit;
		}
		else if( ret && ret!=100 )
		{
			vtcp_log("[%s][%d]��ѯ���ڴ�����ļ�����![ret]",__FILE__,__LINE__,ret);
			goto ErrExit;
		}

		/* ��ӡ��Ϣ */
		if( osub_intst_hst.ac_no == ssub_intst_hst.ac_no )
		{
			ssub_intst_hst.ic_date = osub_intst_hst.intst_date;
		}
		memcpy(&osub_intst_hst,&ssub_intst_hst,sizeof(osub_intst_hst));

		ret = Sub_dd_mst_Sel(g_pub_tx.reply,&s_sub_dd_mst,"ac_no='%s' and sub_ac_seqn=%d ",\
			ssub_intst_hst.ac_no,ssub_intst_hst.sub_ac_seqn);
		if( ret )
		{
			sprintf(acErrMsg, "��ѯ���˻����ļ�����! [%s][%d][%d]",ssub_intst_hst.ac_no,ssub_intst_hst.sub_ac_seqn,ret);
			WRITEMSG
			goto ErrExit;
		}

		if(s_dd_mst.ac_sts[0]=='*')
		{
			continue;
		}
		else if(s_sub_dd_mst.ac_sts[0]=='*')
		{
			continue;
		}

		ret = print_body();
		ERR_DEAL
		line_file++;
		if( line_file>=2 )	/* ���ڻ�ҳ */
		{
			fprintf(fp,"\f");
			page++;
			line_file = 0;

		}
		ttlnum++;
	}
	Sub_intst_hst_Clo_Sel();

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ɶ������˻���Ϣ�ͻ��ص��ɹ�!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;

ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"���ɶ������˻���Ϣ�ͻ��ص�ʧ��!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	if( !strcmp(g_pub_tx.reply,"0000") || strlen(g_pub_tx.reply) == 0 )
	{
		strcpy(g_pub_tx.reply,"G010");
	}
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_cxxhqd);
	ERR_DEAL

GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* ��ӡ��β */
int print_tail( )
{
	char vrec[3];

	strcpy( vrec,"CC" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* ��ӡ���� */
int print_body( )
{
	char vrec[3];

	strcpy( vrec,"HH" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL

GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* ��ֵ���� */
int get_ratio_cxxhqd( bli,bll,str )
int bli,bll;
char str[100];
{
	char vhm[101];
	char vstr[101];
	char l_kmm[31];
	char amt_tmp[21];
	char tmp_inf[21];
	char fmt[10];
	int  season;/*����**/
	int i=0;
	long tmpdate=0;
	long intdays=0;
	char tmpname[51];
	char tmpac_no[51];
	struct com_branch_c sCom_branch;
	struct mdm_ac_rel_c s_mdm_ac_rel;
	struct dd_parm_c 	s_dd_parm;
	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	memset(&s_mdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c));
	memset(&s_dd_parm,0x00,sizeof(struct dd_parm_c));

	switch( bli )
	{
		case 'A': /* ��Ϣ���� */
			pub_base_deadlineD(g_pub_tx.tx_date,-1,&tmpdate);
			sprintf( t_str , "%4d��%2d��%2d��" , tmpdate/10000 , tmpdate%10000/100 , tmpdate%100 );
			sprintf(fmt,"%%-%ds",bll);
			sprintf( str , fmt , t_str );
			break;
		case 'B': /* �������� */
			sprintf(fmt,"%%-%ds",bll);
			sprintf( str , fmt , ssub_intst_hst.br_no );
			break;
		case 'C': /* �������� */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",ssub_intst_hst.br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "��ѯ����������������! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( sCom_branch.br_name );
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , ssub_intst_hst.br_no);
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'E': /* �������˺� */
			ret=Sub_dd_mst_Sel(g_pub_tx.reply,&s_sub_dd_mst,"ac_no='%s' and sub_ac_seqn=%d ",\
				ssub_intst_hst.ac_no,ssub_intst_hst.sub_ac_seqn);
			if( ret )
			{
				sprintf(acErrMsg, "ȡ�������˻�����! [%d][%d]", s_dd_mst.ac_id,ret);
				WRITEMSG
				goto ErrExit;
			}
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , s_sub_dd_mst.sub_ac_no);
			break;
		case 'F': /* ��Ʒ���� */
			ret=Dd_parm_Sel(g_pub_tx.reply,&s_dd_parm,"prdt_no='%s' ",s_dd_mst.prdt_no);
			if( ret )
			{
				sprintf(acErrMsg, "ȡ�˻���Ʒ���ʹ���! [%d][%d]", s_dd_mst.ac_id,ret);
				WRITEMSG
				goto ErrExit;
			}
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , s_dd_parm.title);
			break;
		case 'G': /* ��Ϣ��ʽ */
			ret=Sub_dd_mst_Sel(g_pub_tx.reply,&s_sub_dd_mst,"ac_no='%s' and sub_ac_seqn=%d ",\
				ssub_intst_hst.ac_no,ssub_intst_hst.sub_ac_seqn);
			if( ret )
			{
				sprintf(acErrMsg, "ȡ�������˻�����! [%d][%d]", s_dd_mst.ac_id,ret);
				WRITEMSG
				goto ErrExit;
			}
			memset(tmp_inf,0x00,sizeof(tmp_inf));
			if(s_sub_dd_mst.intst_type[0]=='0'){ strcpy(tmp_inf,"����Ϣ"); }
			else if(s_sub_dd_mst.intst_type[0]=='1') { strcpy(tmp_inf,"���汾��"); }
			else if(s_sub_dd_mst.intst_type[0]=='2') { strcpy(tmp_inf,"���ռ�Ϣ"); }
			else if(s_sub_dd_mst.intst_type[0]=='3') { strcpy(tmp_inf,"���¼�Ϣ"); }
			else if(s_sub_dd_mst.intst_type[0]=='4') { strcpy(tmp_inf,"������Ϣ"); }
			else if(s_sub_dd_mst.intst_type[0]=='5') { strcpy(tmp_inf,"�����Ϣ"); }
			else
			{
				vtcp_log("��Ϣ���ʹ��� �޴�����:[%s] [%s] [%d]",s_sub_dd_mst.intst_type,__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"D112");
				goto ErrExit;
			}
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , tmp_inf );
			break;
		case 'L': /*����*/
			sprintf( amt_tmp, "%.5lf" , ssub_intst_hst.rate );
			sprintf(fmt,"%%-%ds",bll);
			sprintf( str, fmt, amt_tmp );
			break;
		case 'M': /*����*/
			ret=Sub_dd_mst_Sel(g_pub_tx.reply,&s_sub_dd_mst,"ac_no='%s' and sub_ac_seqn=%d ",\
				ssub_intst_hst.ac_no,ssub_intst_hst.sub_ac_seqn);
			if( ret )
			{
				sprintf(acErrMsg, "ȡ�������˻���������! [%s][%d][%d][%d]",\
					ssub_intst_hst.ac_no,ssub_intst_hst.sub_ac_seqn,ret);
				WRITEMSG
				goto ErrExit;
			}
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , s_sub_dd_mst.name);
			break;
		case 'N': /*��Ϣ����*/

			sprintf( amt_tmp, "%.2lf" , ssub_intst_hst.bic_acm );
			pub_rpt_flttomon( amt_tmp,amt_tmp );

			sprintf(fmt,"%%-%ds",bll);
			sprintf( str, fmt, amt_tmp );
			break;
		case 'J': /* ��Ϣ */
			sprintf( amt_tmp, "%.2lf" , ssub_intst_hst.fct_intst );
			pub_rpt_flttomon( amt_tmp,amt_tmp );

			sprintf(fmt,"%%-%ds",bll);
			sprintf( str, fmt, amt_tmp );
			break;
		case 'P': /*�ڼ�����*/
			pub_base_deadlineD(g_pub_tx.tx_date,-1,&tmpdate);
			season = (int )tmpdate%10000/100 /4 +1;
			vtcp_log("[%s][%d]���ǵ�%d����!\n",__FILE__,__LINE__,season);
			sprintf(str,"%1d",season);
			break;
		case 'S': /* ��ʼ���� */
			memset(tmp_inf,0x00,sizeof(tmp_inf));
			sprintf(tmp_inf,"%ld",ssub_intst_hst.ic_date);
			sprintf(fmt,"%%-%ds",bll);
			sprintf( str , fmt , tmp_inf );
			break;
		case 'Q': /* �������� */
			memset(tmp_inf,0x00,sizeof(tmp_inf));
			sprintf(tmp_inf,"%ld",ssub_intst_hst.intst_date-1);
			sprintf(fmt,"%%-%ds",bll);
			sprintf( str , fmt , tmp_inf );
			break;
		case 'R': /* ���� */
			ret=pub_base_CalDays(ssub_intst_hst.ic_date,ssub_intst_hst.intst_date,"0",&intdays);
			if (ret != 0)
			{
				sprintf(acErrMsg, "�����Ϣ��������! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			memset(tmp_inf,0x00,sizeof(tmp_inf));
			sprintf(tmp_inf,"%ld",intdays);
			sprintf(fmt,"%%-%ds",bll);
			sprintf( str , fmt , tmp_inf );
			break;

		default :
			memset( str,' ',bll );
			break;
	}

GoodExit:
	return 0;
ErrExit:
	return 1;
}

