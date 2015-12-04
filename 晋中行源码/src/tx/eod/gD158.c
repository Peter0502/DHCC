/***************************************************************
* �� �� ��:  gD158.c
* ��������:  ���ӹ���ת��
* ��    ��:  lq

* ������ڣ�    
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
*
**************************************************************/
#define MYSQLERR if( ret ) {\
	strcpy( g_pub_tx.reply,"AT03" );\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
	WRITEMSG\
	goto ErrExit;\
	}
#define MYERR if( ret ) {\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
	WRITEMSG\
	goto ErrExit;\
	}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "com_eod_exec_c.h"
#include "mo_qzgl_c.h"
#include "mo_qzgl_hst_c.h"
gD158()
{
		int ret = 0;
		int iErr_flag=0;
		int iErr_flag_1=0;
		double dd_bal_1;
		double dd_bal_2;
		char cReturn[32];
		char tx_date[9];
		char stmp[3];
    struct dd_mst_c dd_mst1;
    struct dd_mst_c dd_mst2;
    struct mo_qzgl_c sMo_qzgl;
    struct mo_qzgl_hst_c sMo_qzgl_hst;
    memset(&dd_mst1,0x0,sizeof(struct dd_mst_c));
    memset(&dd_mst2,0x0,sizeof(struct dd_mst_c));
    memset(&sMo_qzgl,0x0,sizeof(struct mo_qzgl_c));
    memset(&sMo_qzgl_hst,0x0,sizeof(struct mo_qzgl_hst_c));
    memset(&g_pub_tx,0x0,sizeof(g_pub_tx));
    vtcp_log("[%s][%d],++++++++++++\n",__FILE__,__LINE__);
    
    memset(tx_date,0x00,sizeof(tx_date));
		sprintf(tx_date, "%ld", g_pub_tx.tx_date); 
		
		memset(stmp,0x00,sizeof(stmp));
		strcpy(stmp, tx_date+6);
		if(strncmp(stmp ,"8", 2)!=0 && strncmp(stmp ,"18", 2)!=0 && strncmp(stmp ,"28", 2)!=0)
		{
			vtcp_log("����8,18,28�� ���������ӻ���[%s]",tx_date);
			goto OkExit;
	  }
    ret=sql_begin(); /*������*/
		MYSQLERR
    /**------- ��ʼ��ȫ�ֱ��� --------**/
    pub_base_sysinit();
    if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
		{
			sprintf(acErrMsg,"ȡ������ˮ�Ŵ� [%d]",g_pub_tx.trace_no);
			WRITEMSG
			goto ErrExit;
		}
		set_zd_long( DC_TRACE_NO,g_pub_tx.trace_no );
		set_zd_long( DC_TRACE_CNT,1 );
		strcpy(g_pub_tx.tx_code,"����ת��");

    ret = Mo_qzgl_Dec_Sel(g_pub_tx.reply,"1=1 and sts='1' ");
		MYSQLERR
    while(1)
    {
    	iErr_flag=0;
			iErr_flag_1=0;
	    memset(&dd_mst1,0x0,sizeof(struct dd_mst_c));
	    memset(&dd_mst2,0x0,sizeof(struct dd_mst_c));
	    memset(cReturn,0x00,sizeof(cReturn));

	    double	tmpamt=0;
	    long		sys_date_tmp=0;
	    long		hz_date_tmp=0;
	    
	    sys_date_tmp = g_pub_tx.tx_date/100;
	    hz_date_tmp = g_pub_tx.tx_date%100;
	     vtcp_log("[%s][%d],AAAhz_date_tmp=[%ld]\n",__FILE__,__LINE__,hz_date_tmp);
	    ret = Mo_qzgl_Fet_Sel(&sMo_qzgl,g_pub_tx.reply);
	    if(ret==100)
	    {
	      break;
	    }
	    MYERR

			if((sMo_qzgl.hz_date <= hz_date_tmp))
			{
    	
	  	  vtcp_log("[%s][%d],++++++1++++++dd_mst1=[%ld],dd_mst2=[%ld],sts=[%s]\n",__FILE__,__LINE__,dd_mst1.ac_id,dd_mst2.ac_id,sMo_qzgl.sts);
	  	  if(sMo_qzgl.sts[0]=='*')
	  	  	continue;
	  	  
	  	  tmpamt = sMo_qzgl.qy_amt;
	  	  
	  	  ret=Mo_qzgl_hst_Sel(g_pub_tx.reply,&sMo_qzgl_hst,"child_ac_id=%ld and part_ac_id=%ld and tx_date like '%ld%%'",sMo_qzgl.child_ac_id,sMo_qzgl.part_ac_id,sys_date_tmp);
	  	  if(ret && ret!=100)
    	  {
    	    strcpy(g_pub_tx.reply, "D102");
    	    sprintf( acErrMsg, "Mo_qzgl_hst_Sel����!!![%d]", ret );
    	    WRITEMSG
    	    goto ErrExit;
    	  }
    	  else if(ret==100)
    	 	{
    	 		struct mdm_ac_rel_c g_mdm_ac_rel;
    	 		struct mdm_ac_rel_c g_mdm_ac_rel_1;
					memset(&g_mdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
					memset(&g_mdm_ac_rel_1,0x0,sizeof(struct mdm_ac_rel_c));
					ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld",sMo_qzgl.part_ac_id);
					if ( ret )
					{
						sprintf( cReturn, "�˻�״̬�쳣" );
						sprintf( acErrMsg, "�����˺Ż�����Ϣ��" );
						WRITEMSG
						iErr_flag = 1;
						goto TrLog;
					}
					if(g_mdm_ac_rel.note_sts[0]=='*'  )
					{
						strcpy( cReturn, "����״̬" );
						sprintf( acErrMsg, "����״̬");
						WRITEMSG
						iErr_flag = 1;
						goto TrLog;
					}
					if(g_mdm_ac_rel.note_sts[0]=='1'  )
					{
						strcpy( cReturn, "��ʧ״̬" );
						sprintf( acErrMsg, "�ѹ�ʧ");
						WRITEMSG
						iErr_flag = 1;
						goto TrLog;
					}
					
			    ret=Dd_mst_Sel(g_pub_tx.reply,&dd_mst1,"ac_id=%ld",sMo_qzgl.part_ac_id);
					if ( ret )
					{
						sprintf( cReturn, "�˻�״̬�쳣" );
						sprintf( acErrMsg, "�����˺Ż�����Ϣ��" );
						WRITEMSG
						iErr_flag = 1;
						goto TrLog;
					}
					pub_base_strpack( dd_mst1.ac_sts );
					if ( dd_mst1.ac_sts[0] == '*' )
					{
						strcpy( cReturn, "����״̬" );
						sprintf( acErrMsg, "������");
						WRITEMSG
						iErr_flag = 1;
						goto TrLog;
					}
					if ( dd_mst1.hold_sts[0]=='1' )
					{
						strcpy( cReturn, "��ȫ����״̬" );
						sprintf( acErrMsg, "��ȫ����");
						WRITEMSG
						iErr_flag = 1;
						goto TrLog;
					}
					if ( dd_mst1.hold_sts[0]=='2' )
					{
						strcpy( cReturn, "ֻ������״̬" );
						sprintf( acErrMsg, "ֻ������");
						WRITEMSG
						iErr_flag = 1;
						goto TrLog;
					}
					if(dd_mst1.sttl_type[0]=='S')
					{
						strcpy( cReturn, "�˻�״̬�쳣" );
						sprintf( acErrMsg, "�û�Ϊ�ǽ��㻧,����ת��");
						WRITEMSG
						iErr_flag = 1;
						goto TrLog;
					}
					dd_bal_1 = dd_mst1.bal - dd_mst1.ctl_amt ;
					if(dd_mst1.hold_sts[0]=='3')
					{
						dd_bal_1 = dd_bal_1 - dd_mst1.hold_amt;
					}
					vtcp_log("dd_bal_1=[%lf],tmpamt[%lf]",dd_bal_1,tmpamt);
			    if(pub_base_CompDblVal(dd_bal_1,tmpamt)<0)
			    {
						strcpy( cReturn, "����" );
						sprintf( acErrMsg, "����");
						WRITEMSG
						iErr_flag = 1;
						goto TrLog;
					}
					
			    
					ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel_1,"ac_id=%ld",sMo_qzgl.child_ac_id);
					if ( ret )
					{
						sprintf( cReturn, "�˻�״̬�쳣" );
						sprintf( acErrMsg, "�����˺Ż�����Ϣ��" );
						WRITEMSG
						iErr_flag_1 = 1;
						goto TrLog;
					} 
					if(g_mdm_ac_rel_1.note_sts[0]=='*'  )
					{
						strcpy( cReturn, "����״̬" );
						sprintf( acErrMsg, "������");
						WRITEMSG
						iErr_flag_1 = 1;
						goto TrLog;
					}
					if(g_mdm_ac_rel_1.note_sts[0]=='1'  )
					{
						strcpy( cReturn, "��ʧ״̬" );
						sprintf( acErrMsg, "�ѹ�ʧ");
						WRITEMSG
						iErr_flag_1 = 1;
						goto TrLog;
					}
					
			    ret=Dd_mst_Sel(g_pub_tx.reply,&dd_mst2,"ac_id=%ld",sMo_qzgl.child_ac_id);
					if ( ret )
					{
						sprintf( cReturn, "�˻�״̬�쳣" );
						sprintf( acErrMsg, "�����˺Ż�����Ϣ��" );
						WRITEMSG
						iErr_flag_1 = 1;
						goto TrLog;
					}
					pub_base_strpack( dd_mst2.ac_sts );
					if ( dd_mst2.ac_sts[0] == '*' )
					{
						strcpy( cReturn, "����״̬" );
						sprintf( acErrMsg, "������");
						WRITEMSG
						iErr_flag_1 = 1;
						goto TrLog;
					}
					if ( dd_mst2.hold_sts[0]=='1' )
					{
						strcpy( cReturn, "��ȫ����״̬" );
						sprintf( acErrMsg, "��ȫ����");
						WRITEMSG
						iErr_flag_1 = 1;
						goto TrLog;
					}
					if(dd_mst2.sttl_type[0]=='S')
					{
						strcpy( cReturn, "�˻�״̬�쳣" );
						sprintf( acErrMsg, "�û�Ϊ�ǽ��㻧,����ת��");
						WRITEMSG
						iErr_flag_1 = 1;
						goto TrLog;
					}
	TrLog:	  
			  	vtcp_log("[%s][%d],++++++2++++++dd_mst1=[%ld],dd_mst2=[%ld]\n",__FILE__,__LINE__,dd_mst1.ac_id,dd_mst2.ac_id);
			    if(iErr_flag == 1)
			    {
						vtcp_log("[%s][%d] ��ĸ�˺�%ld����",__FILE__,__LINE__,dd_mst1.ac_id);
						/*������֪ͨ���� wudawei 20151127*/
						/*��β��%s��%d��%d��%dʱ%02d�����㣬���ӹ���ת��ʧ�ܣ�������ʱ��ֵ��*/
						pub_mob_sendmail_bat( "1", cReturn, "G158",  g_mdm_ac_rel.ac_no, "Q", tmpamt, dd_mst1.bal, 0);
						/*������֪ͨ���� wudawei 20151127*/
			    	continue;
			    }
			     if(iErr_flag_1 == 1)
			    {
						vtcp_log("[%s][%d] ���˺�״̬�쳣",__FILE__,__LINE__);
						/*������֪ͨ���� wudawei 20151127*/
						/*��β��%s��%d��%d��%dʱ%02d�����㣬���ӹ���ת��ʧ�ܣ�������ʱ��ֵ��*/
						pub_mob_sendmail_bat( "2", cReturn, "G158",  g_mdm_ac_rel_1.ac_no, "Q", tmpamt, dd_mst2.bal, 0);
						/*������֪ͨ���� wudawei 20151127*/
			    	continue;
			    }
			    if (iErr_flag==0 && iErr_flag_1==0)
			    {
			    	/**����ת��**/
			    	vtcp_log("[%s][%d],++++++3++++++dd_mst1=[%ld],dd_mst2=[%ld]\n",__FILE__,__LINE__,dd_mst1.ac_id,dd_mst2.ac_id);
			    	ret=sub_change_acct(dd_mst1,dd_mst2,tmpamt );
			    	MYERR
			    }
			    vtcp_log("[%s][%d],++++++4++++++dd_mst1=[%ld],dd_mst2=[%ld]\n",__FILE__,__LINE__,dd_mst1.ac_id,dd_mst2.ac_id);
			  }
			}
    }
    Mo_qzgl_Clo_Sel();
    set_zd_data(DC_CODE,"");
		set_zd_double("102P",0.00 );
		set_zd_double("102Q",0.00 );
    set_zd_data("102J","");
    vtcp_log("[%s][%d],++++++5++++++dd_mst1=[%ld],dd_mst2=[%ld]\n",__FILE__,__LINE__,dd_mst1.ac_id,dd_mst2.ac_id);

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ӹ����Զ���ת����ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
  if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
      strcpy(g_pub_tx.reply,"G009");
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"���ӹ����Զ���ת����ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int sub_change_acct(struct dd_mst_c dd_mst1,struct dd_mst_c dd_mst2,double amt)
{
		int ret=0;
		struct mdm_ac_rel_c vmdm_ac_rel;
		struct mo_qzgl_hst_c sMo_qzgl_hst;
		memset(&vmdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
		memset(&sMo_qzgl_hst,0x0,sizeof(sMo_qzgl_hst));
		/*����*/
		/**�踸ĸ��**/
		memset(&vmdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
		g_pub_tx.tx_amt1=amt;
		strcpy(g_pub_tx.opn_br_no,dd_mst1.opn_br_no);
		strcpy(g_pub_tx.tx_br_no,dd_mst1.opn_br_no);
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&vmdm_ac_rel,"ac_id=%ld and (note_sts = '0' or note_sts = '1')",dd_mst1.ac_id);
		if(ret)
			goto ErrExit;
		vtcp_log("aaaaaaaaaaa[%s][%lf]\n",vmdm_ac_rel.ac_no,amt);
		memcpy(g_pub_tx.ac_no,vmdm_ac_rel.ac_no,sizeof(g_pub_tx.ac_no));
		g_pub_tx.ac_id = dd_mst1.ac_id;
		g_pub_tx.ac_seqn =1;
		strcpy( g_pub_tx.ac_get_ind,"00");	/*�Ѿ���ȡ��mdm_ac_rel*/
		strcpy( g_pub_tx.ac_id_type,"1" );	/* �˺����� 1-��� */
		strcpy( g_pub_tx.add_ind,"0" );		/* ����:0�� */	
		strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת*/	
		strcpy( g_pub_tx.prt_ind,"0" );		/*�Ƿ��ӡ����(�������۱�־):1��0�� */
		strcpy( g_pub_tx.hst_ind,"1" );		/*����ϸ:1�ռ���2���յ�����3���ջ�����*/        
		strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ����*/	
		g_pub_tx.svc_ind=1001;	   
		strcpy ( g_pub_tx.brf,"���ӹ����Զ�ת��" );
		strcpy(g_pub_tx.tx_code,"D158");
		strcpy(g_pub_tx.sub_tx_code,"D003");
		/*** ���׼��ʴ��� ***/
		if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"���׼��ʴ������!");
			WRITEMSG
			goto ErrExit;
		}
		/* ���л�Ƽ��� */
		set_zd_data("102J","01");         
		set_zd_double("102F",g_pub_tx.tx_amt1);                     

		
		strcpy(g_pub_tx.prdt_code,dd_mst1.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
			WRITEMSG
			goto ErrExit;
		}
		
		/**����ͯ��**/
		memset(&vmdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&vmdm_ac_rel,"ac_id=%ld and (note_sts = '0' or note_sts = '1')",dd_mst2.ac_id);
		if(ret) goto ErrExit;
		memcpy(g_pub_tx.ac_no,vmdm_ac_rel.ac_no,sizeof(g_pub_tx.ac_no));
		g_pub_tx.ac_id = dd_mst2.ac_id;
		g_pub_tx.ac_seqn = 1 ;
		strcpy(g_pub_tx.opn_br_no,dd_mst2.opn_br_no);
		strcpy( g_pub_tx.ac_get_ind,"00");	/*�Ѿ���ȡ��mdm_ac_rel*/
		strcpy( g_pub_tx.ac_id_type,"1" );	/* �˺����� 1-��� */
		strcpy( g_pub_tx.add_ind,"1" );		/* ����:0�� */	
		strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת*/	
		strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0�� */
		strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����3���ջ�����*/        
		strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ����*/	
		g_pub_tx.svc_ind=1001;	   
		strcpy ( g_pub_tx.brf,"���ӹ����Զ�ת��" );
		strcpy(g_pub_tx.tx_code,"D158");
		strcpy(g_pub_tx.sub_tx_code,"D099");

		/*** ���׼��ʴ��� ***/
		if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"���׼��ʴ������!");
			WRITEMSG
			goto ErrExit;
		}
		/* ���л�Ƽ��� */
		set_zd_data("101A","01");         
		set_zd_double("1013",g_pub_tx.tx_amt1);                     
		strcpy(g_pub_tx.prdt_code,dd_mst2.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
			WRITEMSG
			goto ErrExit;
		}
		
		sMo_qzgl_hst.child_ac_id	= dd_mst2.ac_id;
		sMo_qzgl_hst.part_ac_id		= dd_mst1.ac_id;
		strcpy(sMo_qzgl_hst.tx_br_no,  g_pub_tx.tx_br_no);
    sMo_qzgl_hst.trace_no			= g_pub_tx.trace_no;
    sMo_qzgl_hst.tx_date			= g_pub_tx.tx_date;
    sMo_qzgl_hst.tx_amt				= amt;
    strcpy(sMo_qzgl_hst.tx_tel, g_pub_tx.tel);
    sMo_qzgl_hst.child_bal   = dd_mst2.bal+amt;
    ret = Mo_qzgl_hst_Ins(sMo_qzgl_hst, g_pub_tx.reply);
    if(ret)
    {
        strcpy(g_pub_tx.reply, "D102");
        sprintf( acErrMsg, "Mo_qzgl_hst_Ins����!!![%d]", ret );
        WRITEMSG
        Mo_qzgl_Clo_Sel();
        goto ErrExit;
    }
		
		return 0;
ErrExit:
	return 1;
}
 
