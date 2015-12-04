/*************************************************
* �� �� ��: sp6681.c
* �����������˽�������Ŵ�֧����������
*
* ��    ��: chenchao
* ������ڣ�2011-7-20 9:50:43
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:

insert into tx_def values('6681','�Ŵ�֧����������','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
insert into tx_flow_def values('6681','0','6681','#$');
insert into tx_sub_def values ('6681','�Ŵ�֧����������','sp6681','0','0');

*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cms_rollback_c.h"
#include "draw_loan_hst_c.h"
#include "mdm_ac_rel_c.h"

int sp6681()
{
	int	iRet = 0;
	char	cGet_data[201];
	
	struct	draw_loan_hst_c	sDraw_loan_hst;
	struct	mdm_ac_rel_c	sMdm_ac_rel;
	struct	cms_rollback_c	sCms_rollback;
	
	memset(&sDraw_loan_hst,0x00,sizeof(sDraw_loan_hst));
	memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	memset(cGet_data,0x00,sizeof(cGet_data));
	
	pub_base_sysinit();
	
	get_zd_data("0670",  sDraw_loan_hst.st_flag    );	/** ֧ȡ��ʽ             **/
	get_zd_data("0370",  cGet_data    );			/** ��ݺ�               **/
	zip_space(cGet_data);
	memcpy(sDraw_loan_hst.pact_no,cGet_data,sizeof(sDraw_loan_hst.pact_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_data("0300",  cGet_data);			/** ��Ȩ֧��Э���       **/
	zip_space(cGet_data);
	memcpy(sDraw_loan_hst.draw_no,cGet_data,sizeof(sDraw_loan_hst.draw_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
/*****
	get_zd_data("0380",  sDraw_loan_hst.ac_no      );	** �����Ӧ�Ĵ���˺�   
	zip_space(cGet_data);
	memcpy(sDraw_loan_hst.draw_no,cGet_data,sizeof(sDraw_loan_hst.draw_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
*****/
	
	
	get_zd_double("0400",  &sDraw_loan_hst.draw_amt   );	/** ֧ȡ���             **/
	get_zd_data("0320",  cGet_data);			/** ί���տ��˺�         **/
	zip_space(cGet_data);
	memcpy(sDraw_loan_hst.trust_ac_no,cGet_data,sizeof(sDraw_loan_hst.trust_ac_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	
	get_zd_data("0250", cGet_data );	/** ί���տ��         **/
	zip_space(cGet_data);
	memcpy(sDraw_loan_hst.trust_name,cGet_data,sizeof(sDraw_loan_hst.trust_name)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	sDraw_loan_hst.sts[0] = '1';				/** ״̬Ϊ��֧ȡ״̬ **/
	iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_id in (select save_ac_id from ln_mst where pact_no='%s' )",sDraw_loan_hst.pact_no);
	if(iRet)
	{
		sprintf( acErrMsg, "��ѯ���ʹ�ϵ�����!pact_no[%s]ret=[%d]" ,sDraw_loan_hst.pact_no ,iRet);
		WRITEMSG
		set_zd_data(DC_GET_MSG,"�˽�ݺ�δ�����������֧�����벻�ܼ���!!!");
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;
	}
	
	memcpy(sDraw_loan_hst.ac_no,sMdm_ac_rel.ac_no,sizeof(sDraw_loan_hst.ac_no)-1);
	sDraw_loan_hst.ac_id = sMdm_ac_rel.ac_id;
	sDraw_loan_hst.approve_date = g_pub_tx.tx_date;
	
	iRet = Draw_loan_hst_Ins(sDraw_loan_hst,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]����֧����ϸ��ʧ��.����ret=[%d]",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"����֧����ϸ��ʧ��!!!");
		goto ErrExit;
	}
	
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	get_zd_data("0580",cGet_data);	/** ��Ȩ���� **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	memcpy(sCms_rollback.beg_table," draw_loan_hst ",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'I';
	sprintf(sCms_rollback.beg_sql," sts='1' and draw_no='%s' ",sDraw_loan_hst.draw_no);
	sCms_rollback.sts[0] = '0';
	get_zd_data("1230",sCms_rollback.filler);
	iRet = Cms_rollback_Ins(sCms_rollback,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]����ع��ǼǱ�����.����iRet=[%d]",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"����ع��ǼǱ�����!!!"); 
		goto ErrExit;
	}
	
OkExit:
	sprintf( acErrMsg, "�Ŵ�֧�������ɹ�!!!");
	WRITEMSG
	strcpy( g_pub_tx.reply, "0000" );
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4))
		memcpy(g_pub_tx.reply,"H034",4);
	sprintf( acErrMsg, "�Ŵ�֧������ʧ��[%s]", g_pub_tx.reply );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
