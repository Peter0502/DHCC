/*************************************************************
* �� �� ��: gD037.c
* �����������Ա�֤����Ϣ��ת�����㻧����
*           	��֤���˻���Ϣת������˻���
* ��    ��: CHENGBO	 
* �������: 2013��3��4��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
insert into com_eod_exec values('1042','Z037','gD037','��֤���Ϣ��ת','1111111111','5','3','21','21','2','Y','Y','N','2','Y');
**************************************************************/
#define MYSQLERR if( ret ) {\
	strcpy( g_pub_tx.reply,"AT03" );\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
	WRITEMSG\
	goto ErrExit;\
	}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "intst_hst_c.h"
#include "bzj_js_ac_rel_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"

gD037()
{
	int ret = 0;
	struct intst_hst_c sIntst_hst;
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct dd_mst_c sDd_mst;
	struct mdm_ac_rel_c sMdm_ac_rel_1;
	struct dd_mst_c sDd_mst_1;
	struct bzj_js_ac_rel_c sBzj_js_ac_rel; 

	memset(&sIntst_hst, 0x00, sizeof(struct intst_hst_c));
	memset(&sBzj_js_ac_rel, 0x00, sizeof(struct bzj_js_ac_rel_c));
	memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&sDd_mst_1,0x00,sizeof(struct dd_mst_c));
	memset(&sMdm_ac_rel_1,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_pub_tx, 0, sizeof(g_pub_tx));

    	ret=sql_begin(); /*������*/
	MYSQLERR

    	/**------- ��ʼ��ȫ�ֱ��� --------**/
    	pub_base_sysinit();
	if( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply) )
		goto ErrExit;
	/**g_pub_tx.tx_date=20080621;**/
	strcpy(g_pub_tx.ct_ind,"2");
	strcpy(g_pub_tx.tx_code,"G037");
	g_pub_tx.trace_cnt=0;

    	ret = Bzj_js_ac_rel_Dec_Sel(g_pub_tx.reply," sts='1' order by tx_br_no");
	MYSQLERR

	while(1)
	{
                memset(&sBzj_js_ac_rel,0x00,sizeof(struct bzj_js_ac_rel_c));
		ret = Bzj_js_ac_rel_Fet_Sel(&sBzj_js_ac_rel, g_pub_tx.reply);
		if (ret == 100) break;
		MYSQLERR
	        /*��鱣֤���˻�*/				
		memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
 		ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_id=%ld",sBzj_js_ac_rel.bzj_ac_id);
		MYSQLERR
		memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
		ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld ",sBzj_js_ac_rel.bzj_ac_id);
		MYSQLERR
		if( sMdm_ac_rel.note_sts[0] != '0' ) /*����״̬������*/
		{
			continue;
		}
		if( sDd_mst.ac_sts[0] != '1' ) /*��֤���˻�״̬������*/
		{
			continue;
		}
		if( sDd_mst.hold_sts[0] == '1' ) /*��֤���˻���ȫ����*/
		{
			continue;
		}
		else if( sDd_mst.hold_sts[0] == '2' ) /*��֤���˻�ֻ������*/
		{
			continue;
		}
		memset(&sIntst_hst, 0x00, sizeof(struct intst_hst_c));
                ret = Intst_hst_Sel(g_pub_tx.reply,&sIntst_hst,"ac_id = %ld and  ac_seqn=%d and intst_date=%ld and ac_ind='0' and pt_ind='1' ",sBzj_js_ac_rel.bzj_ac_id,sBzj_js_ac_rel.bzj_ac_seqn,g_pub_tx.tx_date);
		if( ret == 100 )
		{       /*û�н�Ϣ��¼*/
			continue;
		} 	
		MYSQLERR
		
		if( sDd_mst.hold_sts[0] == '3' && pub_base_CompDblVal(sDd_mst.bal - sDd_mst.hold_amt - sDd_mst.ctl_amt,sIntst_hst.fct_intst - sIntst_hst.intst_tax) <= 0 )
		{       /*���ֶ��ᣬ��������*/
			continue;
		}
		/*���ת���˻�*/
		memset(&sMdm_ac_rel_1,0x00,sizeof(struct mdm_ac_rel_c));
 		ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel_1,"ac_id=%ld",sBzj_js_ac_rel.js_ac_id);
		MYSQLERR
		memset(&sDd_mst_1,0x00,sizeof(struct dd_mst_c));
		ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst_1,"ac_id=%ld ",sBzj_js_ac_rel.js_ac_id);
		MYSQLERR
		if( sMdm_ac_rel_1.note_sts[0] != '0' ) /*����״̬������*/
		{
			continue;
		}
		if( sDd_mst_1.ac_sts[0] != '1' ) /*ת���˻�״̬������*/
		{
			continue;
		}
		if( sDd_mst_1.hold_sts[0] == '1' ) /*ת���˻���ȫ����*/
		{
			continue;
		}
		else if( sDd_mst_1.hold_sts[0] == '2' ) /*ת���˻�ֻ������*/
		{
			continue;
		}
		
		/****��ʼ��ת��������****/
		strcpy(g_pub_tx.sub_tx_code,"D003");
		strcpy(g_pub_tx.tx_br_no,sMdm_ac_rel.opn_br_no);
		strcpy(g_pub_tx.opn_br_no,sMdm_ac_rel.opn_br_no);
		strcpy(g_pub_tx.cur_no,sIntst_hst.cur_no);
		strcpy(g_pub_tx.ac_no,sMdm_ac_rel.ac_no);
		g_pub_tx.ac_id = sBzj_js_ac_rel.bzj_ac_id;
		g_pub_tx.ac_seqn = sBzj_js_ac_rel.bzj_ac_seqn;
		strcpy(g_pub_tx.ac_get_ind,"00");
		strcpy(g_pub_tx.ac_id_type,"1");
		strcpy(g_pub_tx.add_ind,"0");
		strcpy( g_pub_tx.prt_ind,"0" );
		strcpy(g_pub_tx.ac_wrk_ind,"0000000011");
		strcpy(g_pub_tx.hst_ind,"1");
		g_pub_tx.svc_ind=1001;
		strcpy(g_pub_tx.brf,"��֤���Ϣת��");
		g_pub_tx.tx_amt1 = sIntst_hst.fct_intst - sIntst_hst.intst_tax;
		
		ret=pub_acct_trance();
		if( ret )
		{
			sprintf(acErrMsg,"ת����֤���ʻ���Ϣ����!!");
			WRITEMSG
			goto ErrExit;
		}
                set_zd_data( "102J", "01" );
                set_zd_data( "102K", "2" );
		set_zd_double( "102F", g_pub_tx.tx_amt1 );
		ret = pubf_acct_proc("D003");
		if( ret )
		{
			sprintf(acErrMsg,"��ת��֤���ʻ���Ϣ����!!");
			WRITEMSG
			goto ErrExit;
		}
		
		/****��ʼ��ת��������****/
		strcpy(g_pub_tx.sub_tx_code,"D099");
		strcpy(g_pub_tx.tx_br_no,sMdm_ac_rel.opn_br_no);
		strcpy(g_pub_tx.opn_br_no,sMdm_ac_rel_1.opn_br_no);
		strcpy(g_pub_tx.cur_no,"01");
		strcpy(g_pub_tx.ac_no,sMdm_ac_rel_1.ac_no);
		g_pub_tx.ac_id = sDd_mst_1.ac_id;
		g_pub_tx.ac_seqn = sDd_mst_1.ac_seqn;
		strcpy(g_pub_tx.ac_get_ind,"00");
		strcpy(g_pub_tx.ac_id_type,"1");
		strcpy( g_pub_tx.prt_ind,"0" );
		strcpy(g_pub_tx.add_ind,"1");
		strcpy(g_pub_tx.ac_wrk_ind,"0000000001");
		strcpy(g_pub_tx.hst_ind,"1");
		g_pub_tx.svc_ind=1001;
		strcpy(g_pub_tx.brf,"��֤���Ϣת��");
		g_pub_tx.tx_amt1 = sIntst_hst.fct_intst - sIntst_hst.intst_tax;

		ret=pub_acct_trance();
		if( ret )
		{
			sprintf(acErrMsg,"ת�뱣֤���ʻ���Ϣ����!!");
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data("101A","01");
		set_zd_data("101B","2");
		set_zd_double("1013",g_pub_tx.tx_amt1);
        	ret = pubf_acct_proc("D099");
		if( ret )
		{
			sprintf(acErrMsg,"��ת��֤���ʻ���Ϣ����!!");
			WRITEMSG
			goto ErrExit;
		}
    	}
	Bzj_js_ac_rel_Clo_Sel();
       
        /*�Ǽ���ˮ*
        ret=pub_ins_trace_log();
        if( ret )
        {
		sprintf(acErrMsg,"��ת��֤���ʻ���Ϣ����!!");
		WRITEMSG
		goto ErrExit;
        } 
	**/

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��֤����Ϣ��ת����ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
	{
		strcpy(g_pub_tx.reply,"G009");
	}
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"��֤����Ϣ��ת����ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

