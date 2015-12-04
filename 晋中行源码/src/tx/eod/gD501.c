/*************************************************************
* �� �� ��: gD501.c
* ��������: ������ҽԺPOS����Ѯĩ���� 
* ��    ��: ����ӯ
* �������: 20150114
* insert into com_eod_exec values (126, 'D501', 'gD501', '��ҽԺPOS���뻮ת', '1111111111', '3', '0', '99', '99', '0', 'Y', 'Y', 'N', '2', 'Y');
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#define MYSQLERR if(ret) {strcpy( g_pub_tx.reply,"AT03" ); sprintf(acErrMsg,"SQL ERR [%d]",ret); WRITEMSG goto ErrExit;}
#define  EXTERN
#define  MRCHNO  "059175080620001" /*��������ҽԺ�̻����*/
#define  LIMITAMT 5000.00  /*�ڲ��˻��������*/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "in_mst_c.h"
#include "march_info_c.h"
#include "com_sys_parm_c.h"

gD501()
{
	int ret=0;
	double amt=0.00;
	struct com_sys_parm_c com_sys_parm_c;
	struct dd_mst_c dd_mst_c;
	struct dd_mst_c dd_mst_s;
	struct in_mst_c in_mst_c;	
	struct mdm_ac_rel_c  smdm_ac_rel;
	struct mdm_ac_rel_c  smdm_ac_rel2;
	struct march_info_c  smarch_info;
	
	memset(&dd_mst_c,0x00,sizeof(struct dd_mst_c) );
	memset(&dd_mst_s,0x00,sizeof(struct dd_mst_c) );
	memset(&in_mst_c,0x00,sizeof(struct in_mst_c) );
	memset(&com_sys_parm_c, 0x00, sizeof(struct com_sys_parm_c));
	memset(&smdm_ac_rel, 0x00, sizeof(smdm_ac_rel));
	memset(&smdm_ac_rel2, 0x00, sizeof(smdm_ac_rel2));
	memset(&smarch_info, 0x00, sizeof(smarch_info));

	ret = sql_begin();	/* ������ */
	MYSQLERR

	/* ��ѯϵͳ������ */
	ret = Com_sys_parm_Sel(g_pub_tx.reply, &com_sys_parm_c, "1=1");
	MYSQLERR
	
	g_pub_tx.tx_date = com_sys_parm_c.sys_date;
	strcpy(g_pub_tx.cur_no, RMB);
	strcpy(g_pub_tx.tx_code, "D501");
	strcpy(g_pub_tx.sub_tx_code, "D501");
	g_pub_tx.trace_cnt = 0;
	strcpy(g_pub_tx.ct_ind, "2");
	strcpy(g_pub_tx.tel, "9999");
	
	/*ȡ������ˮ��*/
	if (pub_base_PubQlsh(&g_pub_tx.trace_no, g_pub_tx.reply))
	{
		sprintf(acErrMsg, "ȡ������ˮ�ų���! trace_no=[%ld]",g_pub_tx.trace_no);
		WRITEMSG
		goto ErrExit;
	}

	vtcp_log("��������ҽԺPOS����Ѯĩ���ʼ");
	/*��������*/
	strncpy(smarch_info.mrchno,MRCHNO,sizeof(smarch_info.mrchno)-1);
	ret = March_info_Sel(g_pub_tx.reply,&smarch_info,"mrchno = '%s'",smarch_info.mrchno);
	if ( ret )                                                                     
	{
		strcpy( g_pub_tx.reply,"AT06" );
		sprintf(acErrMsg,"��ѯ�̻���������,ret[%d],�̻����[%s]",ret,smarch_info.mrchno);
		WRITEMSG
		goto ErrExit;
	}
  
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&smdm_ac_rel," ac_no='%s' ", smarch_info.mrchacno_ls);
	if ( ret )                                                                     
	{
		strcpy( g_pub_tx.reply,"AT06" );
		sprintf(acErrMsg,"��ѯ�����˻���ϵ�����,ret[%d],�˺�[%s]",ret,smarch_info.mrchacno_ls);
		WRITEMSG
		goto ErrExit;
	}

	ret=Dd_mst_Sel(g_pub_tx.reply,&dd_mst_s," ac_id='%ld' ", smdm_ac_rel.ac_id);
	if ( ret )
	{
		strcpy( g_pub_tx.reply,"AT06" );
		sprintf(acErrMsg,"��ѯ�����˻��������,ret[%d],�˻�ID[%s]",ret,smdm_ac_rel.ac_id);
		WRITEMSG
		goto ErrExit;
	}

	if(dd_mst_s.ac_sts[0] != '1')
	{
		sprintf(acErrMsg,"�����˻�״̬������,״̬Ϊ[%s]", dd_mst_s.ac_sts);
		WRITEMSG
		goto ErrExit;
	}
	if(dd_mst_s.hold_sts[0]=='1')
	{
		sprintf(acErrMsg, "�����˻�Ϊ��ȫ����״̬!");
		WRITEMSG
		goto ErrExit;
	}
	
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&smdm_ac_rel2," ac_no='%s' ", smarch_info.mrchacno);
	if ( ret )                                                                     
	{
		strcpy( g_pub_tx.reply,"AT06" );
		sprintf(acErrMsg,"��ѯ�����˻���ϵ�����,ret[%d],�˺�[%s]",ret,smarch_info.mrchacno);
		WRITEMSG
		goto ErrExit;
	}
	
	ret=Dd_mst_Sel(g_pub_tx.reply,&dd_mst_c, " ac_id='%ld' ",smdm_ac_rel2.ac_id);
	if ( ret )
	{
		sprintf(acErrMsg,"��ѯ�����������,ret[%d],�˻�ID[%d]",ret,smdm_ac_rel2.ac_id);
		WRITEMSG
		goto ErrExit;
	}
	
	if(dd_mst_c.ac_sts[0] != '1')
	{
		sprintf(acErrMsg,"�Թ��˻�״̬������,״̬Ϊ[%s]", dd_mst_c.ac_sts);
		WRITEMSG
		goto ErrExit;
	}
	if(dd_mst_c.hold_sts[0]=='1')
	{
		sprintf(acErrMsg, "�Թ��˻�Ϊ��ȫ����״̬!");
		WRITEMSG
		goto ErrExit;
	}
	
	if( pub_base_CompDblVal(dd_mst_s.bal,LIMITAMT) != 1 )
	{
		sprintf(acErrMsg,"�����˻����ﲻ�������޶�,���軮��!�����޶�[%.2f],�˻����[%.2f]",LIMITAMT,dd_mst_s.bal);
		WRITEMSG
		goto OkExit;
	}
	  
	vtcp_log("��ʼ�����˻�ת������");
	strcpy(g_pub_tx.sub_tx_code,"D003");
	strcpy(g_pub_tx.tx_br_no,dd_mst_s.opn_br_no);
	strcpy(g_pub_tx.opn_br_no,dd_mst_s.opn_br_no);
	strcpy(g_pub_tx.cur_no,"01");
	strcpy(g_pub_tx.ac_no,smarch_info.mrchacno_ls);
	g_pub_tx.ac_id=dd_mst_s.ac_id;
	g_pub_tx.ac_seqn=1;
	strcpy( g_pub_tx.ac_id_type,"1" );
	g_pub_tx.tx_amt1=dd_mst_s.bal-LIMITAMT;
	vtcp_log("[%s][%d]���λ�ת���[%.2f]",__FILE__,__LINE__,dd_mst_s.bal-LIMITAMT);
	strcpy(g_pub_tx.add_ind,"0"); 
	strcpy(g_pub_tx.hst_ind,"1"); /**�ռ�����ϸ**/
	strcpy(g_pub_tx.ac_wrk_ind,"0000000011");
	g_pub_tx.svc_ind=1001; /**�������***/
	strcpy(g_pub_tx.ac_get_ind,"00");
	strcpy(g_pub_tx.ct_ind,"2");	/*** ת�� ***/
	strcpy(g_pub_tx.brf,"��ҽԺPOS��תת��");
	strcpy( g_pub_tx.prt_ind,"0" );
	
	if ( pub_acct_trance() )
	{
		sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
		WRITEMSG
		return 1;
	}
	set_zd_data( "102J", "01" );
	set_zd_data( "102K", "2" );
	set_zd_double( "102F", g_pub_tx.tx_amt1 );
		
	if ( pubf_acct_proc("D003") )
	{
		sprintf(acErrMsg,"�Ǽǻ����ˮ����!");
		WRITEMSG
		return 1;
	}

	vtcp_log("��ʼ�Թ��˻�ת�����");
	strcpy(g_pub_tx.sub_tx_code,"D099");
	strcpy(g_pub_tx.ac_no,smarch_info.mrchacno);
	strcpy(g_pub_tx.tx_br_no,smdm_ac_rel2.opn_br_no);
	strcpy(g_pub_tx.opn_br_no,smdm_ac_rel2.opn_br_no);
	strcpy(g_pub_tx.cur_no,"01");
	g_pub_tx.ac_id=smdm_ac_rel2.ac_id;
	g_pub_tx.tx_amt1=dd_mst_s.bal-LIMITAMT;
	g_pub_tx.ac_seqn=1;
	strcpy( g_pub_tx.ac_id_type,"1" );
	strcpy(g_pub_tx.ac_get_ind,"00");
	strcpy( g_pub_tx.prt_ind,"0" );
	strcpy(g_pub_tx.add_ind,"1"); 
	strcpy(g_pub_tx.hst_ind,"1"); /**�ռ�����ϸ**/
	strcpy(g_pub_tx.ac_wrk_ind,"0000000011");
	g_pub_tx.svc_ind=1001; /**�������***/
	strcpy(g_pub_tx.ct_ind,"2");	/*** ת�� ***/
	strcpy(g_pub_tx.brf,"��ҽԺPOS��תת��");
	
	if ( pub_acct_trance() )
	{
		sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
		WRITEMSG
		goto ErrExit;
	}
	set_zd_data("101A","01");
	set_zd_data("101B","2");
	set_zd_double("1013",g_pub_tx.tx_amt1);

	if ( pubf_acct_proc("D099") )
	{
		sprintf(acErrMsg,"�Ǽǻ����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	sql_commit();		/*--�ύ����--*/
	strcpy(g_pub_tx.reply, "0000");
	set_zd_data("0120", "0000");
	sprintf(acErrMsg, "��������ҽԺPOS����Ѯĩ����ɹ�![%s]", g_pub_tx.reply);
	WRITEMSG
	return 0;
	
ErrExit:
	sql_rollback();  /*--����ع�--*/
	if (strcmp(g_pub_tx.reply, "0000") == 0 || g_pub_tx.reply[0] == '\0') 
	{
		strcpy(g_pub_tx.reply, "G009");
	}
	set_zd_data("0120", g_pub_tx.reply);
	sprintf(acErrMsg, "��������ҽԺPOS����Ѯĩ����ʧ��![%s]", g_pub_tx.reply);
	WRITEMSG
	return 1;
}