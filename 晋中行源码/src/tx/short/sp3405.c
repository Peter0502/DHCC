/**************************************************
* �� �� ��:  sp3405.c
* ���������� ��ѯ�ͻ�ƾ֤����(�ֽ�֧Ʊ ת��֧Ʊ)
*				֧���ڲ������� if(mdm_ac_rel.ac_seqn=0) ac_seqn=0
* ��    ��:  zhanghb
* ������ڣ� 2009-10-13 9:23:14
*
*insert into tx_def (tx_code,tx_name,bit_map,log_ind,tx_type,wrk_sts) values ('3405','��ѯ�ͻ�ƾ֤����', '10000000000000000000000000000100000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000000100','1','6','2',null,null);
*insert into tx_flow_def (tx_code,flow_code,sub_tx_code,sub_tx_rslt) values ('3405','0','3405','#$');
*insert into tx_sub_def (sub_tx_code,sub_tx_name,tx_func,acct_ind,chnl_ind) values ('3405','��ѯ�ͻ�ƾ֤����','sp3405','0','0');
*
* �޸ļ�¼��
* ��    ��:	
* �� �� ��:
* �޸�����:
*************************************************/
#define EXTERN
#include "public.h"
#include "note_mst_c.h"
#include "note_mst_hst_c.h"
#include "note_cheq_mst_c.h"
#include "note_cheq_hst_c.h"
#include "note_parm_c.h"
#include "mdm_ac_rel_c.h"
/*--- ����û�� ��ʱ��� modify by wzs 20120213
#include "yj_mo_loss_hst_c.h"  */
#include "mo_loss_c.h"
int sp3405()
{
	struct	mdm_ac_rel_c	g_mdm_ac_rel;
	/*--- ����û�д˱� ��ʱ��� modify by wzs 20120213
	struct	yj_mo_loss_hst_c	g_yj_mo_loss_hst; ---*/
	struct	mo_loss_c	g_Mo_loss;
	char		NoteNo[17];			/*ƾ֤��*/
	char		Note_tmp[17];
	char		NoteType[4];		/*ƾ֤����*/
	char		ActNo[25];
	char		sts[51];					/* ֧Ʊ״̬ */
	char		Txno[5];
	int			ret	= 0;
	int			ac_seqn = 1;
	long    	ac_id = 0;
	long 		Date = 0;
	long        sub_ac_seqn=0;

	memset(&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	/*--- ����û�д˱���ʱ��� mofify by wzs 20120213	
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
		sprintf(acErrMsg, "�����ʺŹ�ϵ�����ڸ��ʺ�!");
		WRITEMSG
			strcpy(g_pub_tx.reply, "P102");
		goto ErrExit;
	}
	if (ret) {
		vtcp_log("--[%s][%d]look cAcno=[%s]", __FILE__, __LINE__, ActNo);
		sprintf(acErrMsg, "ȡ�����ʺŹ�ϵ�����!");
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
		sprintf( acErrMsg, "�ͻ��޴�ƾ֤��ƾ֤״̬��һ��!ac_id=[%d],note_type=[%s],cheq_beg_no=[%s],cheq_end_no=[%s]",g_mdm_ac_rel.ac_id,NoteType,NoteNo, NoteNo);
		WRITEMSG
			/*strcpy( g_pub_tx.reply, "N080" );*/
		goto ErrExit;
	}if ( sts[0] == '0' )
	{
		sprintf( acErrMsg, "�ͻ��޴�ƾ֤!sts=[%s]", sts);
		WRITEMSG
		    strcpy( g_pub_tx.reply, "N080" );
		goto ErrExit;
	} else if ( sts[0] == '2' )
	{
		sprintf( acErrMsg, "��ƾ֤�ѹ�ʧ!sts=[%s]", sts);
		WRITEMSG
		    strcpy( g_pub_tx.reply, "N081" );
		goto ErrExit;
	} else if ( sts[0] == '3' )
	{
		sprintf( acErrMsg, "��ƾ֤�ѻ���!sts=[%s]", sts);
		WRITEMSG
		    strcpy( g_pub_tx.reply, "N082" );
		goto ErrExit;
	} else if ( sts[0] == '4' )
	{
		sprintf( acErrMsg, "��ƾ֤������!sts=[%s]", sts);
		WRITEMSG
		    strcpy( g_pub_tx.reply, "N083" );
		goto ErrExit;
	} else if ( sts[0] == '5' )
	{
		sprintf( acErrMsg, "��ƾ֤��ʹ��!sts=[%s]", sts);
		WRITEMSG
		strcpy( g_pub_tx.reply, "N084" );
		goto ErrExit;
	}
	/*--- ����û�д˱���ʱ��� modify by wzs 20120213 	
	ret = Yj_mo_loss_hst_Sel(g_pub_tx.reply, &g_yj_mo_loss_hst, "note_type='%s' and note_no='%s'",NoteType,NoteNo);
	if (ret == 0) {
		vtcp_log("--[%s][%d]look txno=[%s]", __FILE__, __LINE__, Txno);
		if(strcmp(Txno,"2405")==0)
		{
			vtcp_log("--[%s][%d]look NoteNo=[%s]", __FILE__, __LINE__, NoteNo);
			sprintf(acErrMsg, "��ƾ֤�Ѿ�ӡ����ʧ!");
			set_zd_data(DC_GET_MSG,acErrMsg);
			strcpy(g_pub_tx.reply,"8888");
			WRITEMSG
			goto ErrExit;
		}else	if(strcmp(Txno,"2502") !=0 && strcmp(Txno,"5841") != 0 && strcmp(Txno,"5801") != 0 )			*/	/*5841:��ͨ����ҵ��¼�� 2502:�Թ����ڲ��˼���*/
	/*		{
				get_zd_long("0440",&Date);
				vtcp_log("--[%s][%d]look Date=[%d] qf_date=[%d]", __FILE__, __LINE__, Date,g_yj_mo_loss_hst.qf_date);
				if(Date != g_yj_mo_loss_hst.qf_date)
				{
					sprintf(acErrMsg, "��Ʊ������ӡ���еǼǵ�ǩ�����ڲ���!");
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
		sprintf(acErrMsg, "ȡYj_mo_loss_hst�����!");
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
					sprintf(acErrMsg, "���ʺ���ӡ�ҵ���ƾ֤δ��ӡ�ҵǼ�!");
					strcpy(g_pub_tx.reply,"P514");
					WRITEMSG
					goto ErrExit;
				}
				else
					goto OkExit;
			}
			else if(ret!=100)
			{
				sprintf(acErrMsg, "��ѯ��ʧ�Ǽǲ�ʧ��!");
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
	sprintf(acErrMsg, "��ѯ�ɹ�![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "��ѯʧ��![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
