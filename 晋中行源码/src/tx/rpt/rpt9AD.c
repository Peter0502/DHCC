/*************************************************************
* �� �� ��: rpt9AD.c
* �������������������ϸ���ڲ�����ϸ��
* ��    ��: dadary
* �������: 2007��1��24��
* ˵    ��: Ϊ�˴ﵽ���õ�Ч������������ÿ�����µ�ִ�У�����
*           	     
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define extern
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \ 
		}
#include "public.h"
#include "com_sys_parm_c.h"
#include "tx_def_c.h"
#include "com_branch_c.h"
#include "acno_hst_log_c.h"
#include "dd_mst_hst_c.h"
#include "in_mst_hst_c.h"
#include "ln_mst_hst_c.h"

long   beg_tx_date		=	0;	/** > �������ʼ���ڣ������Ѿ����ڵ���������**/
long   end_tx_date		=	0;	/** <= ����Ľ������ڣ������Ѿ����ڵ���������**/
int ret ;

rpt9AD()
{
	printf("[%s][%d]��ʼ���������ϸ���Լ��ڲ�����ϸ��!\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]��ʼ���������ϸ���Լ��ڲ�����ϸ��!\n",__FILE__,__LINE__);
	struct com_sys_parm_c		 s_com_sys_parm;
	
	float  d_bal			=	0.0;
	
	
	/**��ʼ����**/
	ret=sql_begin(); 
	if( ret )
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}
	
	/* ��ʼ��ȫ�ֱ��� */
	pub_base_sysinit( );
    
    	
	/**��ȡ��ʼ����***/
	ret = sql_max_long_xier ("acno_hst_log","tx_date",&beg_tx_date,"1=1");
	ERR_DEAL
	
	/***�����Ҫǿ�Ƶ���ĳ���������������������д���Ϳ����� ��ע�������� ǰ����յ�  �ǵõ�����ɺ�ɾ�����ĸĶ�***/
	beg_tx_date=20070430;
	/******�ͼ�һ�оͿ�����****/
	
	ret = Com_sys_parm_Sel( g_pub_tx.reply, &s_com_sys_parm, "1=1" );
	ERR_DEAL
	
	end_tx_date = s_com_sys_parm.lst_date;
	end_tx_date=20070630;
	
	printf("[%s][%d]���������� ����[%ld]-- С�ڻ��ߵ���[%ld]�ı�������!\n",__FILE__,__LINE__,beg_tx_date,end_tx_date);
	vtcp_log("[%s][%d]���������� ����[%ld]-- С�ڻ��ߵ���[%ld]�ı�������!\n",__FILE__,__LINE__,beg_tx_date,end_tx_date);
	
	
	/***�����ݳ���*****/
	
	/*if(insert_dd_hst())	goto ErrExit;
	
	if(insert_in_hst())	goto ErrExit;	*/
		
	if(insert_ln_hst()) goto ErrExit;
	
	
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�������/�ڲ���ϸ�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
    if (strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"G011");
    }
	sprintf(acErrMsg,"�������/�ڲ���ϸʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;	
} 

int insert_dd_hst()
{
	vtcp_log("[%s][%d]��ʼ���������ʷ��ϸ!\n",__FILE__,__LINE__);
	
	struct acno_hst_log_c	s_acno_hst_log;
	struct dd_mst_hst_c	s_dd_mst_hst;
	struct dd_mst_c		s_dd_mst;
	
	long	pre_ac_id = 99999999;
	long	l_hst_cnt = 0;	/**��ȡacno_hst_log���е�ǰ����ӡ���������**/
	long	t_hst_cnt = 0;	/**ȷ����ϸ����һ��һ�����ӱ���ͬ������**/
	float	bal	  = 0.0;
	int	num	  = 0;	/*��¼һ�²���ļ�¼������**/
	
	ret = Dd_mst_hst_Dec_Sel(g_pub_tx.reply,"tx_date>%ld and tx_date<=%ld order by ac_id,tx_date,hst_cnt",beg_tx_date,end_tx_date);
	ERR_DEAL
	
	while(1)
	{
		memset(&s_dd_mst_hst,0x00,sizeof(s_dd_mst_hst));
		memset(&s_acno_hst_log,0x00,sizeof(s_acno_hst_log));
		
		
		ret = Dd_mst_hst_Fet_Sel(&s_dd_mst_hst,g_pub_tx.reply);
		if(ret && ret!=100)
		{
			vtcp_log("[%s][%d]����������ϸ�����!\n",__FILE__,__LINE__);
			return -1;
		}
		else if(ret==100)	break;
		
		/***��������ű仯,����dd_mst���ȡ��������,��Ϊ��ϸ�����ֵļ�¼��������д�Ĳ���***/
		if(pre_ac_id!=s_dd_mst_hst.ac_id)
		{
			memset(&s_dd_mst,0x00,sizeof(s_dd_mst));
			
			ret = Dd_mst_Sel(g_pub_tx.reply,&s_dd_mst,"ac_id=%ld",s_dd_mst_hst.ac_id);
			ERR_DEAL
			
			/**��acno_hst_log���л�ȡ���ʺŵ���ϸ���***/
			ret = sql_max_long_xier ("acno_hst_log","hst_cnt",&l_hst_cnt,"ac_id=%ld",s_dd_mst_hst.ac_id);
			ERR_DEAL
			
			printf("[%s][%d]�����ʻ�[%ld]��ǰ��ӡ����[%ld]\n",__FILE__,__LINE__,s_dd_mst_hst.ac_id,l_hst_cnt);
			
			t_hst_cnt = l_hst_cnt;
			
			pre_ac_id = s_dd_mst_hst.ac_id;
			
			
		}
		
		/***��ϸ��ŵ���***/
		t_hst_cnt ++;
		
		
		/***��ʼ׼������������***/
		memcpy(s_acno_hst_log.ac_type,"1",sizeof(s_acno_hst_log.ac_type)-1);	/*�ʻ����� 1����  2 ����  3 ����***/
		s_acno_hst_log.ac_id	=	s_dd_mst_hst.ac_id;
		s_acno_hst_log.ac_seqn	=	s_dd_mst_hst.ac_seqn;
		s_acno_hst_log.hst_cnt	=	t_hst_cnt;
		s_acno_hst_log.tx_date	=	s_dd_mst_hst.tx_date;
		memcpy(s_acno_hst_log.br_no,s_dd_mst.opn_br_no,sizeof(s_acno_hst_log.br_no)-1);
		memcpy(s_acno_hst_log.note_type,s_dd_mst_hst.note_type,sizeof(s_acno_hst_log.note_no)-1);
		memcpy(s_acno_hst_log.note_no,	s_dd_mst_hst.note_no,sizeof(s_acno_hst_log.note_no)-1);
		memcpy(s_acno_hst_log.tel,	s_dd_mst_hst.tel,sizeof(s_acno_hst_log.tel)-1);
		memcpy(s_acno_hst_log.print_flag,"0",sizeof(s_acno_hst_log.print_flag)-1);
		memcpy(s_acno_hst_log.brf,	s_dd_mst_hst.brf,sizeof(s_acno_hst_log.brf)-1);
		
		/**������**/
		s_acno_hst_log.pagecnt= 0 ;	/*��ǰ��¼���ڴ�ӡ�еĵڼ�ҳ**/
		s_acno_hst_log.tx_amt = s_dd_mst_hst.tx_amt;
		s_acno_hst_log.bal    = s_dd_mst_hst.bal;
		
		if(s_dd_mst_hst.add_ind[0]=='0')	/*0 ��ʾ�跽**/
			memcpy(s_acno_hst_log.dc_ind,"1",sizeof(s_acno_hst_log.dc_ind)-1);
		else if(s_dd_mst_hst.add_ind[0]=='1')	/*1 ��ʾ����**/
			memcpy(s_acno_hst_log.dc_ind,"2",sizeof(s_acno_hst_log.dc_ind)-1);
		
		
		ret = Acno_hst_log_Ins(s_acno_hst_log,g_pub_tx.reply);
		if(ret)
		{
			vtcp_log("[%s][%d]�����¼ʧ����![%d]\n",__FILE__,__LINE__,ret);
			strcpy(g_pub_tx.reply,"D104");
			Acno_hst_log_Debug(&s_acno_hst_log);
			goto ErrExit;
		}
		printf("[%s][%d]�������[%ld] ��ϸ��[%ld]���ǵ�[%d]����¼!\n",__FILE__,__LINE__,s_acno_hst_log.ac_id,
			s_acno_hst_log.hst_cnt,num);
		
		num ++;
	}
	
OkExit:
	vtcp_log("[%s][%d]������ϸ����ɹ�!��������[%d]\n",__FILE__,__LINE__,num);
	return 0;
ErrExit:
	vtcp_log("[%s][%d]������ϸ����ʧ��!\n",__FILE__,__LINE__);
	return 1;
	
}


int insert_in_hst()
{
	vtcp_log("[%s][%d]��ʼ�����ڲ���ʷ��ϸ!\n",__FILE__,__LINE__);
	
	struct acno_hst_log_c	s_acno_hst_log;
	struct in_mst_hst_c	s_in_mst_hst;
	struct in_mst_c		s_in_mst;
	
	long	pre_ac_id = 99999999;
	long	l_hst_cnt = 0;	/**��ȡacno_hst_log���е�ǰ����ӡ���������**/
	long	t_hst_cnt = 0;	/**ȷ����ϸ����һ��һ�����ӱ���ͬ������**/
	float	bal	  = 0.0;
	int	num	  = 0;	/*��¼һ�²���ļ�¼������**/
	
	ret = In_mst_hst_Dec_Sel(g_pub_tx.reply,"tx_date>%ld and tx_date<=%ld order by ac_id,tx_date,hst_cnt",beg_tx_date,end_tx_date);
	ERR_DEAL
	
	while(1)
	{
		memset(&s_in_mst_hst,0x00,sizeof(s_in_mst_hst));
		memset(&s_acno_hst_log,0x00,sizeof(s_acno_hst_log));
		
		
		ret = In_mst_hst_Fet_Sel(&s_in_mst_hst,g_pub_tx.reply);
		if(ret && ret!=100)
		{
			vtcp_log("[%s][%d]�����ڲ���ϸ�����!\n",__FILE__,__LINE__);
			return -1;
		}
		else if(ret==100)	break;
		
		
		printf("[%s][%d]�����ʺ�[%ld] ��ţ�[%d]\n",__FILE__,__LINE__,s_in_mst_hst.ac_id,s_in_mst_hst.hst_cnt);
		/***��������ű仯,����in_mst���ȡ��������,��Ϊ��ϸ�����ֵļ�¼��������д�Ĳ���***/
		if(pre_ac_id!=s_in_mst_hst.ac_id)
		{
			memset(&s_in_mst,0x00,sizeof(s_in_mst));
			
			ret = In_mst_Sel(g_pub_tx.reply,&s_in_mst,"ac_id=%ld",s_in_mst_hst.ac_id);
			ERR_DEAL
			
			/**��acno_hst_log���л�ȡ���ʺŵ���ϸ���***/
			ret = sql_max_long_xier ("acno_hst_log","hst_cnt",&l_hst_cnt,"ac_id=%ld",s_in_mst_hst.ac_id);
			ERR_DEAL
			
			printf("[%s][%d]�ڲ��ʻ�[%ld]��ǰ��ӡ����[%ld]\n",__FILE__,__LINE__,s_in_mst_hst.ac_id,l_hst_cnt);
			
			t_hst_cnt = l_hst_cnt;
			
			pre_ac_id = s_in_mst_hst.ac_id;
			
			
		}
		
		/***��ϸ��ŵ���***/
		t_hst_cnt ++;
		
		
		/***��ʼ׼������������***/
		memcpy(s_acno_hst_log.ac_type,"3",sizeof(s_acno_hst_log.ac_type)-1);	/*�ʻ����� 1�ڲ�  2 ����  3 ����***/
		s_acno_hst_log.ac_id	=	s_in_mst_hst.ac_id;
		s_acno_hst_log.ac_seqn	=	s_in_mst_hst.ac_seqn;
		s_acno_hst_log.hst_cnt	=	t_hst_cnt;
		s_acno_hst_log.tx_date	=	s_in_mst_hst.tx_date;
		memcpy(s_acno_hst_log.br_no,s_in_mst.opn_br_no,sizeof(s_acno_hst_log.br_no)-1);
		memcpy(s_acno_hst_log.note_type,s_in_mst_hst.note_type,sizeof(s_acno_hst_log.note_no)-1);
		memcpy(s_acno_hst_log.note_no,	s_in_mst_hst.note_no,sizeof(s_acno_hst_log.note_no)-1);
		memcpy(s_acno_hst_log.tel,	s_in_mst_hst.tel,sizeof(s_acno_hst_log.tel)-1);
		memcpy(s_acno_hst_log.print_flag,"0",sizeof(s_acno_hst_log.print_flag)-1);
		memcpy(s_acno_hst_log.brf,	s_in_mst_hst.brf,sizeof(s_acno_hst_log.brf)-1);
		
		/**������**/
		s_acno_hst_log.pagecnt= 0 ;	/*��ǰ��¼���ڴ�ӡ�еĵڼ�ҳ**/
		s_acno_hst_log.tx_amt = s_in_mst_hst.tx_amt;
		s_acno_hst_log.bal    = s_in_mst_hst.bal;
		
		if(s_in_mst_hst.add_ind[0]=='0')	/*0 ��ʾ�跽**/
			memcpy(s_acno_hst_log.dc_ind,"1",sizeof(s_acno_hst_log.dc_ind)-1);
		else if(s_in_mst_hst.add_ind[0]=='1')	/*1 ��ʾ����**/
			memcpy(s_acno_hst_log.dc_ind,"2",sizeof(s_acno_hst_log.dc_ind)-1);
		
		
		ret = Acno_hst_log_Ins(s_acno_hst_log,g_pub_tx.reply);
		if(ret)
		{
			vtcp_log("[%s][%d]�����¼ʧ����![%d]\n",__FILE__,__LINE__,ret);
			strcpy(g_pub_tx.reply,"D104");
			Acno_hst_log_Debug(&s_acno_hst_log);
			goto ErrExit;
		}
		printf("[%s][%d]�����ڲ�[%ld] ��ϸ��[%ld]���ǵ�[%d]����¼!\n",__FILE__,__LINE__,s_acno_hst_log.ac_id,
			s_acno_hst_log.hst_cnt,num);
		
		num ++;
	}
	
OkExit:
	vtcp_log("[%s][%d]�ڲ���ϸ����ɹ�!��������[%d]\n",__FILE__,__LINE__,num);
	return 0;
ErrExit:
	vtcp_log("[%s][%d]�ڲ���ϸ����ʧ��!\n",__FILE__,__LINE__);
	return 1;
	
}

int insert_ln_hst()
{
	vtcp_log("[%s][%d]��ʼ���������ʷ��ϸ!\n",__FILE__,__LINE__);
	
	struct acno_hst_log_c	s_acno_hst_log;
	struct ln_mst_hst_c	s_ln_mst_hst;
	struct ln_mst_c		s_ln_mst;
	
	long	pre_ac_id = 99999999;
	long	l_hst_cnt = 0;	/**��ȡacno_hst_log���е�ǰ����ӡ���������**/
	long	t_hst_cnt = 0;	/**ȷ����ϸ����һ��һ�����ӱ���ͬ������**/
	float	bal	  = 0.0;
	int	num	  = 0;	/*��¼һ�²���ļ�¼������**/
	
	ret = Ln_mst_hst_Dec_Sel(g_pub_tx.reply,"tx_date>%ld and tx_date<=%ld order by ac_id,tx_date,hst_cnt",beg_tx_date,end_tx_date);
	ERR_DEAL
	
	while(1)
	{
		memset(&s_ln_mst_hst,0x00,sizeof(s_ln_mst_hst));
		memset(&s_acno_hst_log,0x00,sizeof(s_acno_hst_log));
		
		
		ret = Ln_mst_hst_Fet_Sel(&s_ln_mst_hst,g_pub_tx.reply);
		if(ret && ret!=100)
		{
			vtcp_log("[%s][%d]����������ϸ�����!\n",__FILE__,__LINE__);
			return -1;
		}
		else if(ret==100)	break;
		
		printf("[%s][%d]�����ʺ�[%ld] ��ţ�[%d]\n",__FILE__,__LINE__,s_ln_mst_hst.ac_id,s_ln_mst_hst.hst_cnt);
		/***��������ű仯,����ln_mst���ȡ��������,��Ϊ��ϸ�����ֵļ�¼��������д�Ĳ���***/
		if(pre_ac_id!=s_ln_mst_hst.ac_id)
		{
			memset(&s_ln_mst,0x00,sizeof(s_ln_mst));
			
			ret = Ln_mst_Sel(g_pub_tx.reply,&s_ln_mst,"ac_id=%ld",s_ln_mst_hst.ac_id);
			ERR_DEAL
			
			/**��acno_hst_log���л�ȡ���ʺŵ���ϸ���***/
			ret = sql_max_long_xier ("acno_hst_log","hst_cnt",&l_hst_cnt,"ac_id=%ld",s_ln_mst_hst.ac_id);
			ERR_DEAL
			
			printf("[%s][%d]�����ʻ�[%ld]��ǰ��ӡ����[%ld]\n",__FILE__,__LINE__,s_ln_mst_hst.ac_id,l_hst_cnt);
			
			t_hst_cnt = l_hst_cnt;
			
			pre_ac_id = s_ln_mst_hst.ac_id;
			
			
		}
		
		/***��ϸ��ŵ���***/
		t_hst_cnt ++;
		
		
		/***��ʼ׼������������***/
		memcpy(s_acno_hst_log.ac_type,"2",sizeof(s_acno_hst_log.ac_type)-1);	/*�ʻ����� 1�ڲ�  2 ����  3 ����***/
		s_acno_hst_log.ac_id	=	s_ln_mst_hst.ac_id;
		s_acno_hst_log.ac_seqn	=	s_ln_mst_hst.ac_seqn;
		s_acno_hst_log.hst_cnt	=	t_hst_cnt;
		s_acno_hst_log.tx_date	=	s_ln_mst_hst.tx_date;
		memcpy(s_acno_hst_log.br_no,s_ln_mst.opn_br_no,sizeof(s_acno_hst_log.br_no)-1);
		memcpy(s_acno_hst_log.note_type,s_ln_mst_hst.note_type,sizeof(s_acno_hst_log.note_no)-1);
		memcpy(s_acno_hst_log.note_no,	s_ln_mst_hst.note_no,sizeof(s_acno_hst_log.note_no)-1);
		memcpy(s_acno_hst_log.tel,	s_ln_mst_hst.tel,sizeof(s_acno_hst_log.tel)-1);
		memcpy(s_acno_hst_log.print_flag,"0",sizeof(s_acno_hst_log.print_flag)-1);
		memcpy(s_acno_hst_log.brf,	s_ln_mst_hst.brf,sizeof(s_acno_hst_log.brf)-1);
		
		/**������**/
		s_acno_hst_log.pagecnt= 0 ;	/*��ǰ��¼���ڴ�ӡ�еĵڼ�ҳ**/
		s_acno_hst_log.tx_amt = s_ln_mst_hst.tx_amt;
		s_acno_hst_log.bal    = s_ln_mst_hst.bal;
		
		if(s_ln_mst_hst.add_ind[0]=='0')	/*0 ��ʾ�跽**/
			memcpy(s_acno_hst_log.dc_ind,"1",sizeof(s_acno_hst_log.dc_ind)-1);
		else if(s_ln_mst_hst.add_ind[0]=='1')	/*1 ��ʾ����**/
			memcpy(s_acno_hst_log.dc_ind,"2",sizeof(s_acno_hst_log.dc_ind)-1);
		
		ret = Acno_hst_log_Ins(s_acno_hst_log,g_pub_tx.reply);
		if(ret)
		{
			vtcp_log("[%s][%d]�����¼ʧ����![%d]\n",__FILE__,__LINE__,ret);
			strcpy(g_pub_tx.reply,"D104");
			Acno_hst_log_Debug(&s_acno_hst_log);
			goto ErrExit;
		}
		printf("[%s][%d]�������[%ld] ��ϸ��[%ld]���ǵ�[%d]����¼!\n",__FILE__,__LINE__,s_acno_hst_log.ac_id,
			s_acno_hst_log.hst_cnt,num);
		
		num ++;
		
		}
	
OkExit:
	vtcp_log("[%s][%d]������ϸ����ɹ�!��������[%d]\n",__FILE__,__LINE__,num);
	return 0;
ErrExit:
	vtcp_log("[%s][%d]������ϸ����ʧ��!\n",__FILE__,__LINE__);
	return 1;
	
}