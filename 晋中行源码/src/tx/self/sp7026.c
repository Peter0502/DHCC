/*=========================================================================
*文 件 名: sp7026.c
*功能描述:对公户，销业务委托书;大小额来帐查询打印，检查、销大小额来帐凭证号;
*
*
* insert into tx_def values('7026','客户凭证检查/销凭证','11000000000010010001000010000000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000100','0','6','0','3',null);
* insert into tx_flow_def  values ('7026', 0, '7026', '#$');
* insert into tx_sub_def  values ('7026', '客户凭证检查/销凭证', 'sp7026', '0', '0');
*
=========================================================================*/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "note_parm_c.h"
#include "note_mst_c.h"

int sp7026()
{
	int  iRet = 0;
	char sAc_no[30];  /** 账号 **/
	char sTel[10];
	char sNextVocnum[17];
	char sFlag[2];
	char sts[51];
	char sTmp[200];
	struct mdm_ac_rel_c g_mdm_ac_rel;
	struct note_parm_c sNoteParm;
	struct note_mst_c sNoteMst;
	
	
	memset(sFlag, 0x00, sizeof(sFlag));
	memset(sTel, 0x00, sizeof(sTel));
	memset(&g_mdm_ac_rel, 0x00, sizeof(g_mdm_ac_rel));
	memset(sAc_no, 0x00, sizeof(sAc_no));
	memset(sNextVocnum, 0x00, sizeof(sNextVocnum));
	memset(&sNoteParm, 0x00, sizeof(sNoteParm));
	memset(sts, 0x00, sizeof(sts));
	memset(sTmp, 0x00, sizeof(sTmp));
	
	iRet = pub_base_sysinit();
	if(iRet)
	{
		sprintf(acErrMsg,"初始化公用结构错误!!");
		WRITEMSG
		goto ErrExit;
	}
	
	
	get_zd_data("0300", sAc_no);
	vtcp_log("%s,%d,sAc_no===[%s]",__FILE__,__LINE__,sAc_no);
	get_zd_data("0070", sTel);
	vtcp_log("%s,%d,sTel===[%s]",__FILE__,__LINE__,sTel);
	get_zd_data("0890",g_pub_tx.note_type);
	vtcp_log("%s,%d,note_type===[%s]",__FILE__,__LINE__,g_pub_tx.note_type);
  	get_zd_data("0580",g_pub_tx.beg_note_no);
  	vtcp_log("%s,%d,beg_note_no===[%s]",__FILE__,__LINE__,g_pub_tx.beg_note_no);
  	get_zd_data("0580",g_pub_tx.end_note_no);
  	get_zd_data("0990",sFlag);
  	vtcp_log("%s,%d,sFlag===[%s]",__FILE__,__LINE__,sFlag);
  	zip_space(sAc_no);
  	
  	
  	if (sFlag[0] == '1')		/*校验客户、柜员凭证*/
  	{
  		iRet = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", g_pub_tx.note_type);
		if( iRet == 100 )
		{
		    sprintf(acErrMsg,"取凭证参数note_parm表错!note_type=[%s]",g_pub_tx.note_type);
		    WRITEMSG
		    strcpy( g_pub_tx.reply, "N001" );
		    goto ErrExit;
		}
		else if ( iRet )
		{
		    sprintf( acErrMsg, "执行Note_parm_Sel错误![%d]", iRet );
		    WRITEMSG
		    goto ErrExit;
		}
		if (sNoteParm.ctl_no_ind[0] == 'N')
		{
			goto OkExit;	/* 不管理号码，直接返回正确0 */
		}
		if (sNoteParm.sell_ind[0] == 'N' )	/*柜员凭证*/
		{
			vtcp_log("%s,%d,开始检查柜员凭证",__FILE__,__LINE__);
			strcpy( sNoteMst.tel, sTel );
			strcpy( sNoteMst.note_type, g_pub_tx.note_type );
			strcpy( sNoteMst.br_no, g_pub_tx.tx_br_no );
			vtcp_log("%s,%d,sNoteMst.br_no===[%s]",__FILE__,__LINE__,sNoteMst.br_no);
			strcpy( sNoteMst.beg_note_no, g_pub_tx.beg_note_no );
			strcpy( sNoteMst.end_note_no, g_pub_tx.end_note_no );
			iRet = pub_com_ChkNoteHead( g_pub_tx.note_type, g_pub_tx.beg_note_no , sTel );	
			if ( iRet )
			{
				sprintf( acErrMsg, "检查起始凭证冠子头错" );
				WRITEMSG
				goto ErrExit;
			}
			iRet = pub_com_ChkNoteHead( g_pub_tx.note_type, g_pub_tx.end_note_no , sTel );	
			if ( iRet )
			{
				sprintf( acErrMsg, "检查终止凭证冠子头错" );
				WRITEMSG
				goto ErrExit;
			}
			iRet = pub_com_CalNoteCnt( g_pub_tx.note_type, g_pub_tx.beg_note_no, g_pub_tx.end_note_no );
			if ( iRet == -1 ) 
			{
				sprintf( acErrMsg, "计算凭证数量错!" );
				WRITEMSG
				goto ErrExit;   
			}
			sNoteMst.cnt = iRet;
			vtcp_log("%s,%d,sNoteMst.cnt===[%d]",__FILE__,__LINE__,sNoteMst.cnt);
			iRet=pub_com_ChkNoteUse( g_pub_tx.note_type );
			if( iRet==-1 )
			{
				sprintf( acErrMsg, "柜员凭证检查出错!" );
				WRITEMSG
				goto ErrExit;
			}
			/* 检查柜员是否有此凭证 */
			iRet=pub_com_ChkNote( "D", sNoteMst );
			if( iRet==-1 )
			{
				sprintf( acErrMsg, "凭证状态不正常!" );
				WRITEMSG
				goto ErrExit;
			}
			
			iRet = pub_com_ChkNoteMin( g_pub_tx.note_type, g_pub_tx.beg_note_no, sTel );
			if( iRet == -1 )
			{
				sprintf( acErrMsg, "凭证最小号检查出错!" );
				WRITEMSG
				goto ErrExit;
			}
			else if( iRet == 2 )  /* 不是最小号码，返回最小号码 */
			{
				sprintf( acErrMsg, "凭证号不是最小号!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "N014" );
				sprintf(sTmp, "最小号%s", g_pub_tx.beg_note_no);
				set_zd_data("0950", sTmp);
				goto ErrExit;
			}
 
			
		}
		else if(sNoteParm.sell_ind[0] == 'Y')	/*客户凭证*/
		{
			vtcp_log("%s,%d,开始检查客户凭证",__FILE__,__LINE__);
			iRet = pub_com_ChkBoeNote( g_pub_tx.note_type, g_pub_tx.beg_note_no, g_pub_tx.end_note_no);
			if ( iRet == -1 )
			{
				sprintf( acErrMsg, "检查连续性,冠子头失败!ret=[%d]",iRet );
				WRITEMSG
				goto ErrExit;		
			}
			pub_base_old_acno(sAc_no);
			
			zip_space(sAc_no);
			
			iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",sAc_no);
			if (iRet)
			{
				vtcp_log("%s,%d,ac_no[%s]查找mdm_ac_rel错误!",__FILE__,__LINE__,sAc_no);
				strcpy(g_pub_tx.reply,"C114");
				goto ErrExit;
			}
			iRet=pub_com_ChkAcCheq( g_mdm_ac_rel.ac_id, g_mdm_ac_rel.ac_seqn, g_pub_tx.note_type, g_pub_tx.beg_note_no, g_pub_tx.end_note_no, sts );
			if( iRet == -1 )
			{
				sprintf( acErrMsg, "客户无此凭证或凭证状态不一致!ac_id=[%d],\
				         note_type=[%s],cheq_beg_no=[%s],cheq_end_no=[%s]",\
				         g_mdm_ac_rel.ac_id,g_pub_tx.note_type,g_pub_tx.beg_note_no, g_pub_tx.end_note_no );
				WRITEMSG
				goto ErrExit;
			}
			if ( sts[0] == '0' )
			{
				sprintf( acErrMsg, "客户无此凭证!sts=[%s]", sts);	
				WRITEMSG
				strcpy( g_pub_tx.reply, "N080" );
				goto ErrExit;
			}else if ( sts[0] == '2' )
			{
				sprintf( acErrMsg, "此凭证已挂失!sts=[%s]", sts);	
				WRITEMSG
				strcpy( g_pub_tx.reply, "N081" );
				goto ErrExit;
			}else if ( sts[0] == '3' )
			{
				sprintf( acErrMsg, "此凭证已回收!sts=[%s]", sts);	
				WRITEMSG
				strcpy( g_pub_tx.reply, "N082" );
				goto ErrExit;
			}else if ( sts[0] == '4' )
			{
				sprintf( acErrMsg, "此凭证已作废!sts=[%s]", sts);	
				WRITEMSG
				strcpy( g_pub_tx.reply, "N083" );
				goto ErrExit;
			}else if ( sts[0] == '5' )
			{
				sprintf( acErrMsg, "此凭证已使用!sts=[%s]", sts);	
				WRITEMSG
				strcpy( g_pub_tx.reply, "N084" );
				goto ErrExit;
			}
			
		}
  		
  	}
  	if (sFlag[0] == '2')		/*销客户、柜员凭证*/
  	{
	  	if (sAc_no[0] == '0' && strlen(sAc_no) == 1)
	  	{
	  		g_mdm_ac_rel.ac_id = 0;
	  		g_mdm_ac_rel.ac_seqn = 0;
	  	}
	  	else
	  	{
			pub_base_old_acno(sAc_no);
			
			zip_space(sAc_no);
			
			iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",sAc_no);
			if (iRet)
			{
				vtcp_log("%s,%d,ac_no[%s]查找mdm_ac_rel错误!",__FILE__,__LINE__,sAc_no);
				strcpy(g_pub_tx.reply,"C114");
				goto ErrExit;
			}
			
			if(g_mdm_ac_rel.ac_seqn==0)
		  		g_mdm_ac_rel.ac_seqn=1;
		}
	  		
		iRet=pub_com_NoteUse(g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn,g_pub_tx.note_type,g_pub_tx.beg_note_no,g_pub_tx.end_note_no,sTel);
		if(iRet)
		{
		        sprintf(acErrMsg,"处理凭证出错!");
		        WRITEMSG
		        goto ErrExit;
		}
		
		/*返回下一个可用凭证*/
		iRet = pub_com_GetNoteMin(g_pub_tx.note_type,sNextVocnum,sTel);
		if(iRet)
		{
		        sprintf(acErrMsg,"没有凭证了!");
		        WRITEMSG
		        goto ErrExit;
		}
		set_zd_data("0961",sNextVocnum);
	}
OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);	
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;	
}

