/***************************************************************
* �� �� ��: spD013.c
* ����������ȡ��
* ��    ��: jack
* ������ڣ�2004��1��6��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "cif_id_code_rel_c.h"


spD013()
{
	int  ret=0;
	char c_fd93[1];
	char tmpname[101],ct_name[5],tmp_ac_no[7],term[3],id_name[21];
	char intst_type[17],memo[51],draw_type[21],auto_depo[9];
	char br_name[31],br_tele[21];
	FILE *fp;
	int	 tmp_tx_date=0,TmpTerm=0,TmpAcSeqn=0;
	double tmp_tx_amt=0.00;
	struct S_pub_intst				TmpIntst;
	struct td_mst_c					sTd_mst;
	struct cif_id_code_rel_c		g_cif_id_code_rel;
	char acc_no[8];

	memset(c_fd93, 0 , sizeof(c_fd93));
	memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));
	memset(&TmpIntst,0x00,sizeof(struct S_pub_intst));
	memset(&g_pub_intst,0x00,sizeof(g_pub_intst) );
	g_reply_int=0;

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	get_zd_data("1021",g_pub_tx.ac_no);
	get_zd_int("1022",&g_pub_tx.ac_seqn);
	get_zd_data("0930",c_fd93);
	sprintf(acErrMsg,"ac_no=[%s],ac_seqn=[%d]",g_pub_tx.ac_no,g_pub_tx.ac_seqn);
	WRITEMSG

	pub_base_old_acno( g_pub_tx.ac_no );

	get_zd_data("1023",g_pub_tx.note_type);
	get_zd_data("1024",g_pub_tx.beg_note_no);
	get_zd_data("1024",g_pub_tx.end_note_no);
	get_zd_data("102T",g_pub_tx.new_note_no);
	get_zd_data("1025",g_pub_tx.draw_uncon_yn);	
	get_zd_data("1026",g_pub_tx.draw_pwd_yn);	
	get_zd_data("1027",g_pub_tx.qry_pwd);
	get_zd_data("1028",g_pub_tx.draw_pwd);
	get_zd_data("1029",g_pub_tx.draw_id_yn);	
	get_zd_data("102G",g_pub_tx.id_type);	
	get_zd_data("102A",g_pub_tx.id_no);	
	get_zd_data("102B",g_pub_tx.draw_seal_yn);	
	get_zd_data("102C",g_pub_tx.pwd_mach_yn);	
	get_zd_data("102D",g_pub_tx.draw_mach);	
	get_zd_long("102E",&g_pub_tx.mach_date);
	get_zd_double("102F",&g_pub_tx.tx_amt1);
	get_zd_data("102J",g_pub_tx.cur_no);
	get_zd_data("102K",g_pub_tx.ct_ind);	
	get_zd_data("102Y",g_pub_tx.brf);	
	sprintf(acErrMsg,"cur_no=[%s],ct_ind=[%s]",g_pub_tx.cur_no,g_pub_tx.ct_ind);
	vtcp_log("%s,%d, cur_no=[%s],ct_ind=[%s]",__FILE__,__LINE__,g_pub_tx.cur_no,g_pub_tx.ct_ind);


	WRITEMSG
	get_zd_data("102O",g_pub_tx.prt_ind); /*�Ƿ���۱�־*/

	if( !strlen(g_pub_tx.brf) )
		strcpy( g_pub_tx.brf,"ȡ��" );

	if(g_pub_tx.ct_ind[0]=='1')
		strcpy( memo,"ȡ��" );
	else 
		strcpy( memo,"תȡ" );

	strcpy(g_pub_tx.add_ind,"0"); /**����**/
	strcpy(g_pub_tx.hst_ind,"1"); /**�ռ�����ϸ**/
	
	if (g_pub_tx.ct_ind[0]!='1' && g_pub_tx.ct_ind[0]!='2' 
		&& g_pub_tx.ct_ind[0]!='3' && g_pub_tx.ct_ind[0]!='4' )
	{
		vtcp_log("CCCCC[%s]",g_pub_tx.ct_ind);
		TRACE
		strcpy( g_pub_tx.reply,"P156" );
vtcp_log("%s,%d, tx_amt1=[%s],mach_date=[%s],",__FILE__,__LINE__,g_pub_tx.tx_amt1,g_pub_tx.mach_date);
vtcp_log("%s,%d, note_type=[%s],id_no=[%s],",__FILE__,__LINE__,g_pub_tx.note_type,g_pub_tx.id_no);

		goto ErrExit;
	}
	if (g_pub_tx.ct_ind[0] != '1')
	{
		strcpy(g_pub_tx.ct_ind,"2");
	}
	/**���ƾ֤���㲻����ˮ���㲻����ϸ�����֧ȡ��ʽ**/
	if(c_fd93[0]=='9')/* ����ǿ�в���Ҫ������� */	
	strcpy(g_pub_tx.ac_wrk_ind,"1000"); 
	else
	strcpy(g_pub_tx.ac_wrk_ind,"1001"); 
	g_pub_tx.svc_ind=1001; /**��ȡ��***/

	get_zd_data("0191",g_pub_tx.using_ind);
	if( g_pub_tx.using_ind[0]=='1' )
	{
		get_zd_data("0192",g_pub_tx.ac_wrk_ind);
		get_zd_int("0193",&g_pub_tx.svc_ind);
	}
	sprintf(acErrMsg, "[%s][%d]useing_ind=[%c]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.using_ind[0],g_pub_tx.svc_ind);
	WRITEMSG
	sprintf(acErrMsg,"BEG D003[%s][%s][%s][%d]",g_pub_tx.ac_no,
		g_pub_tx.using_ind,g_pub_tx.ac_wrk_ind,g_pub_tx.svc_ind);
	WRITEMSG
	/** ���� **/

	TmpAcSeqn = g_pub_tx.ac_seqn;
	
	sprintf( acErrMsg, "-------------->����ȥ��ƾ֤����Ϊ[%s]", g_pub_tx.note_type );
	WRITEMSG

	sprintf(acErrMsg, "*********��������[%ld]svc_ind=[%d]", g_pub_tx.tx_date,g_pub_tx.svc_ind);
	WRITEMSG
	vtcp_log("<<<<<<<<<<<ac_no[%s]", g_pub_tx.ac_no);
	if(strlen(g_pub_tx.ac_no)==3 ||strlen(g_pub_tx.ac_no)==5 ||strlen(g_pub_tx.ac_no)==7)
	{
		goto OkExit;
	}
	g_reply_int=pub_acct_trance_check();
	if( g_reply_int )
	{
		sprintf(acErrMsg,"���ô�ȡ�����ؼ�麯��ʧ��!");
		WRITEMSG
		goto ErrExit;
	}

	set_zd_int("1022",g_pub_tx.ac_seqn);	/* ������һ��102����ʻ�����ֶ� */

	sprintf(acErrMsg,"Ӧ����Ϣ[%.2lf] ˰[%.2lf] �ܸ�[%.2lf]",
			g_pub_intst.dealval+g_pub_intst.keepval, g_pub_intst.val,
	g_pub_tx.tx_amt1+g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val);
	WRITEMSG

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"ȡ��ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"ȡ��ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
