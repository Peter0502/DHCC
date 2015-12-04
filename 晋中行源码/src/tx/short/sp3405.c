/**************************************************
* 文 件 名:  sp3405.c
* 功能描述： 查询客户凭证交易(现金支票 转账支票)
*				支持内部账所以 if(mdm_ac_rel.ac_seqn=0) ac_seqn=0
* 作    者:  zhanghb
* 完成日期： 2009-10-13 9:23:14
*
*insert into tx_def (tx_code,tx_name,bit_map,log_ind,tx_type,wrk_sts) values ('3405','查询客户凭证交易', '10000000000000000000000000000100000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000000100','1','6','2',null,null);
*insert into tx_flow_def (tx_code,flow_code,sub_tx_code,sub_tx_rslt) values ('3405','0','3405','#$');
*insert into tx_sub_def (sub_tx_code,sub_tx_name,tx_func,acct_ind,chnl_ind) values ('3405','查询客户凭证交易','sp3405','0','0');
*
* 修改记录：
* 日    期:	
* 修 改 人:
* 修改内容:
*************************************************/
#define EXTERN
#include "public.h"
#include "note_mst_c.h"
#include "note_mst_hst_c.h"
#include "note_cheq_mst_c.h"
#include "note_cheq_hst_c.h"
#include "note_parm_c.h"
#include "mdm_ac_rel_c.h"
/*--- 晋中没有 暂时封掉 modify by wzs 20120213
#include "yj_mo_loss_hst_c.h"  */
#include "mo_loss_c.h"
int sp3405()
{
	struct	mdm_ac_rel_c	g_mdm_ac_rel;
	/*--- 晋中没有此表 暂时封掉 modify by wzs 20120213
	struct	yj_mo_loss_hst_c	g_yj_mo_loss_hst; ---*/
	struct	mo_loss_c	g_Mo_loss;
	char		NoteNo[17];			/*凭证号*/
	char		Note_tmp[17];
	char		NoteType[4];		/*凭证种类*/
	char		ActNo[25];
	char		sts[51];					/* 支票状态 */
	char		Txno[5];
	int			ret	= 0;
	int			ac_seqn = 1;
	long    	ac_id = 0;
	long 		Date = 0;
	long        sub_ac_seqn=0;

	memset(&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	/*--- 晋中没有此表暂时封掉 mofify by wzs 20120213	
	memset(&g_yj_mo_loss_hst, 0x00, sizeof(struct yj_mo_loss_hst_c)); ---*/
	memset(&g_Mo_loss, 0x00, sizeof(struct mo_loss_c));
	memset(NoteNo,0x00,sizeof(NoteNo));
	memset(NoteType,0x00,sizeof(NoteType));
	memset(ActNo,0x00,sizeof(ActNo));
	memset(Note_tmp,0x00,sizeof(Note_tmp));
	memset(sts,0x00,sizeof(sts));
	memset(Txno,0x00,sizeof(Txno));
		/*	
	get_zd_data("0580",NoteNo);
		*/
	get_zd_data("0580",Note_tmp);
	get_zd_data("0300",ActNo);
	get_zd_data("0890",NoteType);
	get_zd_data("0480",Txno);
	get_zd_long("0540",&sub_ac_seqn);
	
	pub_base_strpack(ActNo);
	pub_base_old_acno(ActNo);
	/* g_pub_tx.sub_ac_seqn=sub_ac_seqn;*/
	sprintf(NoteNo,"%016s",Note_tmp);	
	vtcp_log("--[%s][%d]look cAcno=[%s]", __FILE__, __LINE__, NoteNo);
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'", ActNo);
	if (ret == 100) {
		vtcp_log("--[%s][%d]look cAcno=[%s]", __FILE__, __LINE__, ActNo);
		sprintf(acErrMsg, "介质帐号关系不存在该帐号!");
		WRITEMSG
			strcpy(g_pub_tx.reply, "P102");
		goto ErrExit;
	}
	if (ret) {
		vtcp_log("--[%s][%d]look cAcno=[%s]", __FILE__, __LINE__, ActNo);
		sprintf(acErrMsg, "取介质帐号关系表出错!");
		WRITEMSG
			strcpy(g_pub_tx.reply, "W011");
		goto ErrExit;
	}
	if(g_mdm_ac_rel.ac_seqn==0)
		ac_seqn=0;
	ret = pub_com_ChkAcCheq( g_mdm_ac_rel.ac_id,ac_seqn,NoteType, NoteNo, NoteNo, sts );
	vtcp_log("--[%s][%d]look sts=[%s]ret=[%d]", __FILE__, __LINE__, sts,ret);
	if ( ret != 0 )
	{
		sprintf( acErrMsg, "客户无此凭证或凭证状态不一致!ac_id=[%d],note_type=[%s],cheq_beg_no=[%s],cheq_end_no=[%s]",g_mdm_ac_rel.ac_id,NoteType,NoteNo, NoteNo);
		WRITEMSG
			/*strcpy( g_pub_tx.reply, "N080" );*/
		goto ErrExit;
	}if ( sts[0] == '0' )
	{
		sprintf( acErrMsg, "客户无此凭证!sts=[%s]", sts);
		WRITEMSG
		    strcpy( g_pub_tx.reply, "N080" );
		goto ErrExit;
	} else if ( sts[0] == '2' )
	{
		sprintf( acErrMsg, "此凭证已挂失!sts=[%s]", sts);
		WRITEMSG
		    strcpy( g_pub_tx.reply, "N081" );
		goto ErrExit;
	} else if ( sts[0] == '3' )
	{
		sprintf( acErrMsg, "此凭证已回收!sts=[%s]", sts);
		WRITEMSG
		    strcpy( g_pub_tx.reply, "N082" );
		goto ErrExit;
	} else if ( sts[0] == '4' )
	{
		sprintf( acErrMsg, "此凭证已作废!sts=[%s]", sts);
		WRITEMSG
		    strcpy( g_pub_tx.reply, "N083" );
		goto ErrExit;
	} else if ( sts[0] == '5' )
	{
		sprintf( acErrMsg, "此凭证已使用!sts=[%s]", sts);
		WRITEMSG
		strcpy( g_pub_tx.reply, "N084" );
		goto ErrExit;
	}
	/*--- 晋中没有此表暂时封掉 modify by wzs 20120213 	
	ret = Yj_mo_loss_hst_Sel(g_pub_tx.reply, &g_yj_mo_loss_hst, "note_type='%s' and note_no='%s'",NoteType,NoteNo);
	if (ret == 0) {
		vtcp_log("--[%s][%d]look txno=[%s]", __FILE__, __LINE__, Txno);
		if(strcmp(Txno,"2405")==0)
		{
			vtcp_log("--[%s][%d]look NoteNo=[%s]", __FILE__, __LINE__, NoteNo);
			sprintf(acErrMsg, "该凭证已经印鉴挂失!");
			set_zd_data(DC_GET_MSG,acErrMsg);
			strcpy(g_pub_tx.reply,"8888");
			WRITEMSG
			goto ErrExit;
		}else	if(strcmp(Txno,"2502") !=0 && strcmp(Txno,"5841") != 0 && strcmp(Txno,"5801") != 0 )			*/	/*5841:普通贷记业务录入 2502:对公和内部账记账*/
	/*		{
				get_zd_long("0440",&Date);
				vtcp_log("--[%s][%d]look Date=[%d] qf_date=[%d]", __FILE__, __LINE__, Date,g_yj_mo_loss_hst.qf_date);
				if(Date != g_yj_mo_loss_hst.qf_date)
				{
					sprintf(acErrMsg, "出票日期与印挂中登记的签发日期不符!");
					set_zd_data(DC_GET_MSG,acErrMsg);
					strcpy(g_pub_tx.reply,"8888");
					WRITEMSG
					goto ErrExit;
				}
			}
	}
	else if (ret!=100) 
	{
		vtcp_log("--[%s][%d]look NoteNo=[%s]", __FILE__, __LINE__, NoteNo);
		sprintf(acErrMsg, "取Yj_mo_loss_hst表出错!");
		set_zd_data(DC_GET_MSG,acErrMsg);
		strcpy(g_pub_tx.reply,"8888");
		WRITEMSG
		goto ErrExit;
	}
	else if (ret == 100)
	{	
		vtcp_log("--[%s][%d]look Txno=[%s]", __FILE__, __LINE__, Txno);
		if(strcmp(Txno,"2405") !=0 )				
		{
			ret = Mo_loss_Sel(g_pub_tx.reply, &g_Mo_loss, "ac_no='%s'",ActNo);
			if(ret == 0)
			{
				if(g_Mo_loss.loss_ind[0] == '5' && g_Mo_loss.unloss_ind[0]=='0')
				{
					sprintf(acErrMsg, "该帐号已印挂但该凭证未作印挂登记!");
					strcpy(g_pub_tx.reply,"P514");
					WRITEMSG
					goto ErrExit;
				}
				else
					goto OkExit;
			}
			else if(ret!=100)
			{
				sprintf(acErrMsg, "查询挂失登记簿失败!");
				set_zd_data(DC_GET_MSG,acErrMsg);
				strcpy(g_pub_tx.reply,"8888");
				WRITEMSG
				goto ErrExit;
			}
		}
	}
	*/
OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "查询成功![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "查询失败![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
