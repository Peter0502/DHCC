/*************************************************************
* �� �� ��: rpt9AB.c
* ������������û�зֻ��Ŀ�Ŀ����ǰ�Ľ�����ϸ
*	    Ҫ��ǰgl_sub���е������� ��ʷ�е�һ�������
* ��    ��: dadary
* �������: 2007��1��17��
*
*ע��:�˳���ÿ�����в���Ҫ���ڷ��̺�ִ�У������Ѿ�����˻�����
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
#include "dc_log_rz_c.h"
#include "acc_hrt_log_c.h"
#include "com_item_c.h"
#include "gl_sub_c.h"


rpt9AB()
{
	int ret ;
	struct acc_hrt_log_c 		 s_acc_hrt_log;
	struct dc_log_rz_c		 s_dc_log_rz;
	struct com_item_c		 s_com_item;
	struct gl_sub_c			 s_gl_sub;
	struct com_sys_parm_c		 s_com_sys_parm;
	double d_bal			=	0.0;
	
	long   beg_tx_date		=	0;	/** > �������ʼ���ڣ������Ѿ����ڵ���������**/
	long   end_tx_date		=	0;	/** <= ����Ľ������ڣ������Ѿ����ڵ���������**/
	
	char pre_br_no[6];
	char pre_acc_hrt[6];
	long pre_tx_date = 0;


	memset(pre_br_no,0x00,sizeof(pre_br_no));
	memset(pre_acc_hrt,0x00,sizeof(pre_acc_hrt));
	
	memcpy(pre_br_no,"-----",5);
	memcpy(pre_acc_hrt,"-----",5);
	vtcp_log("[%s][%d]��ʼ���е���û�зֻ��ʵĿ�Ŀ����ϸ!\n",__FILE__,__LINE__);
	
	
	ret = sql_begin(); /*������*/
    	if( ret != 0 )
    	{
    	    sprintf( acErrMsg, "BEGIN WORK error!!!" );
    	    WRITEMSG
    	    goto ErrExit;
    	}
    	
    	/**------- ��ʼ��ȫ�ֱ��� --------**/
    	pub_base_sysinit();
	
			/**��ȡ��ʼ����***/
	ret = sql_max_long_xier ("acc_hrt_log","tx_date",&beg_tx_date,"1=1");
	ERR_DEAL
	
	
	ret = Com_sys_parm_Sel( g_pub_tx.reply, &s_com_sys_parm, "1=1" );
	ERR_DEAL
	
	end_tx_date = s_com_sys_parm.lst_date;
	
	printf("[%s][%d]���������� ����[%ld]-- С�ڻ��ߵ���[%ld]�ı�������!\n",__FILE__,__LINE__,beg_tx_date,end_tx_date);
	vtcp_log("[%s][%d]���������� ����[%ld]-- С�ڻ��ߵ���[%ld]�ı�������!\n",__FILE__,__LINE__,beg_tx_date,end_tx_date);
	
	ret = Dc_log_rz_Dec_Sel(g_pub_tx.reply,"acc_hrt in ( select acc_hrt from com_item where acc_knd='3') and sts='0' and tx_date>%ld and tx_date<=%ld order by tx_opn_br_no,acc_hrt,tx_date,trace_no,trace_cnt",beg_tx_date,end_tx_date);
	
	/* rem by LiuHuafeng 2007-2-2 15:23
	ret = Dc_log_rz_Dec_Sel(g_pub_tx.reply,"acc_hrt in ( select acc_hrt from com_item where acc_knd='3' and sts='0')  order by tx_tx_br_no,acc_hrt,tx_date,trace_no,trace_cnt");
	******replace by next line ****************
	ret = Dc_log_rz_Dec_Sel(g_pub_tx.reply,"acc_hrt in ( select acc_hrt from com_item where acc_knd='3' and sts='0')  order by tx_opn_br_no,acc_hrt,tx_date,trace_no,trace_cnt");
	****/
	while(1)
	{
		memset(&s_dc_log_rz,0x00,sizeof(s_dc_log_rz));
		memset(&s_com_item, 0x00,sizeof(s_com_item));
		memset(&s_acc_hrt_log,0x00,sizeof(s_acc_hrt_log));
			
		ret = Dc_log_rz_Fet_Sel(&s_dc_log_rz,g_pub_tx.reply);
		if(ret && ret!=100)
		{
			vtcp_log("[%s][%d]���ݿ��������!\n",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"P001");
			goto ErrExit;
		}
		else if(ret==100)
			break;
		/* add by LiuHuafeng 
		s_dc_log_rz.amt = s_dc_log_rz.amt *100;
		 end by LiuHuafeng */	
		ret = Com_item_Sel(g_pub_tx.reply,&s_com_item,"acc_hrt='%s'",s_dc_log_rz.acc_hrt);
		if(ret)
		{
			vtcp_log("[%s][%d]�����Բ����ˣ������Ҳ����������Ŀ��!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"P001");
			goto ErrExit;
		}
		
		if(s_com_item.acc_knd[0]!='3') /***������� �����ֻ��Ŀ�Ŀ ֱ������***/
				continue;
		
		/**��ȡ��Ŀ��ǰ��Ŀ�����***/
		/**Ϊ�˱���������������ű仯��ʱ��ҲҪִ��������䣬����һ�����������������������������һ����ͬ�Ŀ�Ŀ�з���**/
		if(memcmp(pre_acc_hrt,s_dc_log_rz.acc_hrt,5) ||memcmp(pre_br_no,s_dc_log_rz.tx_opn_br_no,5))
		{
			printf("[%s][%d]��Ŀ�ű仯��!�������!\n",__FILE__,__LINE__);
			memset(&s_gl_sub, 0 , sizeof(s_gl_sub));
			/* rem by LiuHuafeng 2007-2-2 17:01
			ret = Gl_sub_Sel(g_pub_tx.reply,&s_gl_sub,"acc_hrt='%s' and br_no='%s' and \"date\"=%ld",
				s_dc_log_rz.acc_hrt,s_dc_log_rz.tx_opn_br_no,s_dc_log_rz.tx_date);
			******************replace by next line*****************************************/
			ret = Gl_sub_Sel(g_pub_tx.reply,&s_gl_sub,"acc_hrt='%s' and br_no='%s'",
				s_dc_log_rz.acc_hrt,s_dc_log_rz.tx_opn_br_no);
			if(ret && ret != 100)
			{
				vtcp_log("[%s][%d]����֧�������ļ������!\n",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"D104");
				goto ErrExit;
			}
			d_bal = 0.0;
			
			
			if(s_com_item.dc_ind[0]=='0' || s_com_item.dc_ind[0]=='1')
			{
				d_bal = s_gl_sub.ldd_bal-s_gl_sub.lcd_bal;
			}
			else if(s_com_item.dc_ind[0]=='2')
			{
				d_bal = s_gl_sub.lcd_bal- s_gl_sub.ldd_bal;
			}
			memcpy(pre_acc_hrt,s_dc_log_rz.acc_hrt,5);
			memcpy(pre_br_no,  s_dc_log_rz.tx_opn_br_no,5);
		}	
		/***��ʼ׼����������****/
		s_acc_hrt_log.tx_date		=		s_dc_log_rz.tx_date    ;
		s_acc_hrt_log.trace_no          =		s_dc_log_rz.trace_no   ;      
		s_acc_hrt_log.trace_cnt         =		s_dc_log_rz.trace_cnt  ;      
		memcpy(s_acc_hrt_log.entry_code,s_dc_log_rz.entry_code,sizeof(s_acc_hrt_log.entry_code)-1);       
		s_acc_hrt_log.dc_seqn           =		s_dc_log_rz.dc_seqn  ;        
		memcpy(s_acc_hrt_log.opn_br_no    ,s_dc_log_rz.opn_br_no    ,sizeof(s_acc_hrt_log.opn_br_no )-1);     
		memcpy(s_acc_hrt_log.tx_br_no     ,s_dc_log_rz.tx_br_no     ,sizeof(s_acc_hrt_log.tx_br_no )-1);    
		memcpy(s_acc_hrt_log.tx_opn_br_no ,s_dc_log_rz.tx_opn_br_no ,sizeof(s_acc_hrt_log.tx_opn_br_no )-1);    
		memcpy(s_acc_hrt_log.tx_tx_br_no  ,s_dc_log_rz.tx_tx_br_no  ,sizeof(s_acc_hrt_log.tx_tx_br_no )-1);    
		memcpy(s_acc_hrt_log.cur_no       ,s_dc_log_rz.cur_no       ,sizeof(s_acc_hrt_log.cur_no )-1);    
		memcpy(s_acc_hrt_log.acc_hrt      ,s_dc_log_rz.acc_hrt      ,sizeof(s_acc_hrt_log.acc_hrt )-1);    
		memcpy(s_acc_hrt_log.dc_ind       ,s_dc_log_rz.dc_ind       ,sizeof(s_acc_hrt_log.dc_ind )-1);    
		memcpy(s_acc_hrt_log.ct_ind       ,s_dc_log_rz.dc_ind       ,sizeof(s_acc_hrt_log.dc_ind )-1);    
		memcpy(s_acc_hrt_log.note_type    ,s_dc_log_rz.note_type    ,sizeof(s_acc_hrt_log.note_type )-1);    
		memcpy(s_acc_hrt_log.note_no      ,s_dc_log_rz.note_no      ,sizeof(s_acc_hrt_log.note_no )-1);          
		s_acc_hrt_log.amt               =		s_dc_log_rz.amt     ;         
		      
		memcpy( s_acc_hrt_log.tel         ,s_dc_log_rz.tel           ,sizeof(s_acc_hrt_log.tel )-1);   
		memcpy( s_acc_hrt_log.sts         ,s_dc_log_rz.sts           ,sizeof(s_acc_hrt_log.sts )-1);   
		memcpy( s_acc_hrt_log.print_flag  ,"0"           	     ,sizeof(s_acc_hrt_log.print_flag )-1); 
		s_acc_hrt_log.pagecnt		=		0;


/**
		printf("%s,%d ���㿪ʼ------����[%s]��Ŀ[%s],com_item.dc_ind=[%c],dc_log.dc_ind=[%c],dbal=[%.2f],amt=[%.2f]\n",
			__FILE__,__LINE__,s_dc_log_rz.tx_tx_br_no,s_dc_log_rz.acc_hrt,s_com_item.dc_ind[0],s_dc_log_rz.dc_ind[0],d_bal,s_dc_log_rz.amt);
**/
		if(s_com_item.dc_ind[0]=='0')/***��������ԵĿ�Ŀ***/
		{
			printf("[%s][%d]���Կ�Ŀ��ʼ�������!\n",__FILE__,__LINE__);
			if(s_dc_log_rz.dc_ind[0]=='1')/*���ԣ���+**/
				d_bal += s_dc_log_rz.amt;
			else if(s_dc_log_rz.dc_ind[0]=='2')/*���ԣ���-***/
				d_bal -= s_dc_log_rz.amt;
			if(memcmp(s_dc_log_rz.tx_opn_br_no,"50101",5)==0 && memcmp(s_dc_log_rz.acc_hrt,"16202",5)==0)
			{
				vtcp_log("%s,%d��ʼ���� d_bal=[%f],amt=[%f]",__FILE__,__LINE__,d_bal,s_dc_log_rz.amt);
			}
		}
		else/***��������������Ŀ��ͬ����ӣ�������**/
		{
			printf("[%s][%d]���Կ�Ŀ��ʼ�������!\n",__FILE__,__LINE__);
			if(s_dc_log_rz.dc_ind[0]!=s_com_item.dc_ind[0])
				d_bal -= s_dc_log_rz.amt;
			else
				d_bal += s_dc_log_rz.amt;
			if(memcmp(s_dc_log_rz.tx_opn_br_no,"50101",5)==0 && memcmp(s_dc_log_rz.acc_hrt,"16202",5)==0)
			{
				vtcp_log("%s,%d��ʼ���� d_bal=[%f],amt=[%f]",__FILE__,__LINE__,d_bal,s_dc_log_rz.amt);
			}
		}
		
		/*********************��д��дacc_hrt_log�Ľ��*********************/
		s_acc_hrt_log.bal =	d_bal;        
		
		printf("�������-------����[%s]��Ŀ[%s],com_item.dc_ind=[%c],dc_log.dc_ind=[%c],dbal=[%.2f],amt=[%.2f]\n",
			s_dc_log_rz.tx_opn_br_no,s_dc_log_rz.acc_hrt,s_com_item.dc_ind[0],s_dc_log_rz.dc_ind[0],d_bal,s_dc_log_rz.amt);
		
		if(memcmp(s_acc_hrt_log.acc_hrt,"16202",5)==0 && memcmp(s_acc_hrt_log.tx_opn_br_no,"50101",5)==0 )
		{
		vtcp_log("%s,%d tx_date=%d,amt=[%f], bal=[%f]",__FILE__,__LINE__,s_acc_hrt_log.tx_date,s_acc_hrt_log.amt,s_acc_hrt_log.bal);
		}
		ret = Acc_hrt_log_Ins(s_acc_hrt_log,g_pub_tx.reply);
		if(ret)
		{
			vtcp_log("[%s][%d]�����¼ʧ����!ret==[%d]\n",__FILE__,__LINE__,ret);
			strcpy(g_pub_tx.reply,"D104");
			Acc_hrt_log_Debug(&s_acc_hrt_log);
			goto ErrExit;
		}
		if(memcmp(s_acc_hrt_log.acc_hrt,"16202",5)==0 && memcmp(s_acc_hrt_log.tx_opn_br_no,"50101",5)==0 )
		{
			printf("%s,%d sleep,sleep \n",__FILE__,__LINE__);
			sleep(10);
		}
	}
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"������־��ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
    if (strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"G011");
    }
	sprintf(acErrMsg,"������־��ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;	
} 
