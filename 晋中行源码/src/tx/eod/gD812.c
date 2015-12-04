/*************************************************************
* �� �� ��: gD812.c
* ����������һ��ͨ����Ϣ����
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
#include "ac_yht_rel_c.h"
gD812()
{
		int ret = 0;
    struct dd_mst_c dd_mst1;
    struct dd_mst_c dd_mst2;
    struct dd_parm_c dd_parm_c;
    struct ac_yht_rel_c ac_yht_rel;
    double lmt_amt=0.0;
    double zz_lmt=0.0;
    memset(&dd_mst1,0x0,sizeof(struct dd_mst_c));
    memset(&dd_mst2,0x0,sizeof(struct dd_mst_c));
    memset(&ac_yht_rel,0x0,sizeof(struct ac_yht_rel_c));
    memset(&g_pub_tx,0x0,sizeof(g_pub_tx));
    memset(&dd_parm_c,0x0,sizeof(struct dd_parm_c));
    vtcp_log("[%s][%d],++++++++++++\n",__FILE__,__LINE__);
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
	strcpy(g_pub_tx.tx_code,"D812");

    ret = Ac_yht_rel_Dec_Sel(g_pub_tx.reply,"1=1");
		MYSQLERR
    while(1)
    {
        memset(&dd_mst1,0x0,sizeof(struct dd_mst_c));
        memset(&dd_mst2,0x0,sizeof(struct dd_mst_c));
        lmt_amt=0.0;
        zz_lmt=0.0;
        double tmpamt=0.0;
        ret = Ac_yht_rel_Fet_Sel(&ac_yht_rel,g_pub_tx.reply);
        if(ret==100)
        {
          break;
        }
        MYERR
        vtcp_log("[%s][%d],++++++++++++dd_mst1=[%ld],dd_mst2=[%ld],lmt_amt=[%lf],zz_lmt=[%lf]\n",__FILE__,__LINE__,dd_mst1.ac_id,dd_mst2.ac_id,ac_yht_rel.lmt_amt,ac_yht_rel.zz_lmt);
        ret=Dd_mst_Sel(g_pub_tx.reply,&dd_mst1,"ac_id=%ld",ac_yht_rel.ac_id);
        MYERR
		vtcp_log("[%s][%d] ��1=[%s] ��2=[%s]",__FILE__,__LINE__,dd_mst1.hold_sts,dd_mst2.hold_sts);
        if(dd_mst1.ac_sts[0]!='1') /***���ڻ�***/
        {
		vtcp_log("[%s][%d] ��1=[%s] ��2=[%s]",__FILE__,__LINE__,dd_mst1.hold_sts,dd_mst2.hold_sts);
          continue;
        }
        if(dd_mst1.hold_sts[0]!='0')
        {
		vtcp_log("[%s][%d] ��1=[%s] ��2=[%s]",__FILE__,__LINE__,dd_mst1.hold_sts,dd_mst2.hold_sts);
        	continue;
        }
        ret=Dd_mst_Sel(g_pub_tx.reply,&dd_mst2,"ac_id=%ld",ac_yht_rel.yht_ac_id);
        if(dd_mst2.ac_sts[0]!='1') /***һ��ͨ***/
        {
		vtcp_log("[%s][%d] ��1=[%s] ��2=[%s]",__FILE__,__LINE__,dd_mst1.hold_sts,dd_mst2.hold_sts);
        	continue;
        }
        if(dd_mst2.hold_sts[0]!='0' && dd_mst2.hold_sts[0]!='2')
        {
		vtcp_log("[%s][%d] ��1=[%s] ��2=[%s]",__FILE__,__LINE__,dd_mst1.hold_sts,dd_mst2.hold_sts);
        	continue;
        }
		vtcp_log("[%s][%d] ��1=[%s] ��2=[%s]",__FILE__,__LINE__,dd_mst1.hold_sts,dd_mst2.hold_sts);
      
        tmpamt=ac_yht_rel.lmt_amt+ac_yht_rel.zz_lmt;
    
        vtcp_log("[%s][%d],%%%%****$$$$$$$$$$$$++++++++++++dd_mst1=[%ld],dd_mst2=[%ld],lmt_amt=[%lf],zz_lmt=[%lf]\n",__FILE__,__LINE__,dd_mst1.ac_id,dd_mst2.ac_id,ac_yht_rel.lmt_amt,ac_yht_rel.zz_lmt);
        if(pub_base_CompDblVal(dd_mst1.bal,tmpamt)<0)
        {
		vtcp_log("[%s][%d] ��1=[%s] ��2=[%s]",__FILE__,__LINE__,dd_mst1.hold_sts,dd_mst2.hold_sts);
        	continue;
        }
        else
        {
        	/**����ת��**/
         vtcp_log("[%s][%d],++++++++++++dd_mst1=[%ld],dd_mst2=[%ld],lmt_amt=[%lf],zz_lmt=[%lf]\n",__FILE__,__LINE__,dd_mst1.ac_id,dd_mst2.ac_id,ac_yht_rel.lmt_amt,ac_yht_rel.zz_lmt);

		vtcp_log("[%s][%d] ��1=[%s] ��2=[%s]",__FILE__,__LINE__,dd_mst1.hold_sts,dd_mst2.hold_sts);
        	ret=sub_change_acct(dd_mst1,dd_mst2,dd_mst1.bal - ac_yht_rel.lmt_amt );
        	MYERR
        }
		vtcp_log("[%s][%d] ��1=[%s] ��2=[%s]",__FILE__,__LINE__,dd_mst1.hold_sts,dd_mst2.hold_sts);
             vtcp_log("[%s][%d],++++++++++++dd_mst1=[%ld],dd_mst2=[%ld],lmt_amt=[%lf],zz_lmt=[%lf]\n",__FILE__,__LINE__,dd_mst1.ac_id,dd_mst2.ac_id,ac_yht_rel.lmt_amt,ac_yht_rel.zz_lmt);
   
    }
    Ac_yht_rel_Clo_Sel();
    set_zd_data(DC_CODE,"");
		set_zd_double("102P",0.00 );
		set_zd_double("102Q",0.00 );
    set_zd_data("102J","");
            vtcp_log("[%s][%d],++++++++++++dd_mst1=[%ld],dd_mst2=[%ld],lmt_amt=[%lf],zz_lmt=[%lf]\n",__FILE__,__LINE__,dd_mst1.ac_id,dd_mst2.ac_id,ac_yht_rel.lmt_amt,ac_yht_rel.zz_lmt);

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��˽���ڻ���Ϣ����ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
        strcpy(g_pub_tx.reply,"G009");
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"��˽���ڻ���Ϣ����ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
} 
int sub_change_acct(struct dd_mst_c dd_mst1,struct dd_mst_c dd_mst2,double amt)
{
		int ret=0;
		struct mdm_ac_rel_c vmdm_ac_rel;
		memset(&vmdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
		/*����*/
		/**����ۻ�**/
		memset(&vmdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
		g_pub_tx.tx_amt1=amt;
		strcpy(g_pub_tx.note_type,"812");
		strcpy(g_pub_tx.opn_br_no,dd_mst1.opn_br_no);
		strcpy(g_pub_tx.tx_br_no,dd_mst1.opn_br_no);
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&vmdm_ac_rel,"ac_id=%ld and (note_sts = '0' or note_sts = '1')",dd_mst1.ac_id);
		if(ret) goto ErrExit;
		vtcp_log("aaaaaaaaaaa[%s][%lf]\n",vmdm_ac_rel.ac_no,amt);
		memcpy(g_pub_tx.ac_no,vmdm_ac_rel.ac_no,sizeof(g_pub_tx.ac_no));
		g_pub_tx.ac_id = dd_mst1.ac_id;
		g_pub_tx.ac_seqn =1;
		strcpy( g_pub_tx.ac_get_ind,"00");	/*�Ѿ���ȡ��mdm_ac_rel*/
		strcpy( g_pub_tx.ac_id_type,"1" );	/* �˺����� 1-��� */
		strcpy( g_pub_tx.add_ind,"0" );		/* ����:0�� */	
		strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת*/	
		strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0�� */
		strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����3���ջ�����*/        
		strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ����*/	
		g_pub_tx.svc_ind=1001;	   
		strcpy ( g_pub_tx.brf,"һ��ͨת" );	
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
		
		/**��һ��ͨ��**/
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
		strcpy ( g_pub_tx.brf,"һ��ͨת" );	
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
			return 0;
ErrExit:
	return 1;
}
 
