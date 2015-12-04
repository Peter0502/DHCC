/*=========================================================================
*�� �� ��: sp7026.c
*��������:�Թ�������ҵ��ί����;��С�����ʲ�ѯ��ӡ����顢����С������ƾ֤��;
*
*
* insert into tx_def values('7026','�ͻ�ƾ֤���/��ƾ֤','11000000000010010001000010000000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000100','0','6','0','3',null);
* insert into tx_flow_def  values ('7026', 0, '7026', '#$');
* insert into tx_sub_def  values ('7026', '�ͻ�ƾ֤���/��ƾ֤', 'sp7026', '0', '0');
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
	char sAc_no[30];  /** �˺� **/
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
		sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
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
  	
  	
  	if (sFlag[0] == '1')		/*У��ͻ�����Աƾ֤*/
  	{
  		iRet = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", g_pub_tx.note_type);
		if( iRet == 100 )
		{
		    sprintf(acErrMsg,"ȡƾ֤����note_parm���!note_type=[%s]",g_pub_tx.note_type);
		    WRITEMSG
		    strcpy( g_pub_tx.reply, "N001" );
		    goto ErrExit;
		}
		else if ( iRet )
		{
		    sprintf( acErrMsg, "ִ��Note_parm_Sel����![%d]", iRet );
		    WRITEMSG
		    goto ErrExit;
		}
		if (sNoteParm.ctl_no_ind[0] == 'N')
		{
			goto OkExit;	/* ��������룬ֱ�ӷ�����ȷ0 */
		}
		if (sNoteParm.sell_ind[0] == 'N' )	/*��Աƾ֤*/
		{
			vtcp_log("%s,%d,��ʼ����Աƾ֤",__FILE__,__LINE__);
			strcpy( sNoteMst.tel, sTel );
			strcpy( sNoteMst.note_type, g_pub_tx.note_type );
			strcpy( sNoteMst.br_no, g_pub_tx.tx_br_no );
			vtcp_log("%s,%d,sNoteMst.br_no===[%s]",__FILE__,__LINE__,sNoteMst.br_no);
			strcpy( sNoteMst.beg_note_no, g_pub_tx.beg_note_no );
			strcpy( sNoteMst.end_note_no, g_pub_tx.end_note_no );
			iRet = pub_com_ChkNoteHead( g_pub_tx.note_type, g_pub_tx.beg_note_no , sTel );	
			if ( iRet )
			{
				sprintf( acErrMsg, "�����ʼƾ֤����ͷ��" );
				WRITEMSG
				goto ErrExit;
			}
			iRet = pub_com_ChkNoteHead( g_pub_tx.note_type, g_pub_tx.end_note_no , sTel );	
			if ( iRet )
			{
				sprintf( acErrMsg, "�����ֹƾ֤����ͷ��" );
				WRITEMSG
				goto ErrExit;
			}
			iRet = pub_com_CalNoteCnt( g_pub_tx.note_type, g_pub_tx.beg_note_no, g_pub_tx.end_note_no );
			if ( iRet == -1 ) 
			{
				sprintf( acErrMsg, "����ƾ֤������!" );
				WRITEMSG
				goto ErrExit;   
			}
			sNoteMst.cnt = iRet;
			vtcp_log("%s,%d,sNoteMst.cnt===[%d]",__FILE__,__LINE__,sNoteMst.cnt);
			iRet=pub_com_ChkNoteUse( g_pub_tx.note_type );
			if( iRet==-1 )
			{
				sprintf( acErrMsg, "��Աƾ֤������!" );
				WRITEMSG
				goto ErrExit;
			}
			/* ����Ա�Ƿ��д�ƾ֤ */
			iRet=pub_com_ChkNote( "D", sNoteMst );
			if( iRet==-1 )
			{
				sprintf( acErrMsg, "ƾ֤״̬������!" );
				WRITEMSG
				goto ErrExit;
			}
			
			iRet = pub_com_ChkNoteMin( g_pub_tx.note_type, g_pub_tx.beg_note_no, sTel );
			if( iRet == -1 )
			{
				sprintf( acErrMsg, "ƾ֤��С�ż�����!" );
				WRITEMSG
				goto ErrExit;
			}
			else if( iRet == 2 )  /* ������С���룬������С���� */
			{
				sprintf( acErrMsg, "ƾ֤�Ų�����С��!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "N014" );
				sprintf(sTmp, "��С��%s", g_pub_tx.beg_note_no);
				set_zd_data("0950", sTmp);
				goto ErrExit;
			}
 
			
		}
		else if(sNoteParm.sell_ind[0] == 'Y')	/*�ͻ�ƾ֤*/
		{
			vtcp_log("%s,%d,��ʼ���ͻ�ƾ֤",__FILE__,__LINE__);
			iRet = pub_com_ChkBoeNote( g_pub_tx.note_type, g_pub_tx.beg_note_no, g_pub_tx.end_note_no);
			if ( iRet == -1 )
			{
				sprintf( acErrMsg, "���������,����ͷʧ��!ret=[%d]",iRet );
				WRITEMSG
				goto ErrExit;		
			}
			pub_base_old_acno(sAc_no);
			
			zip_space(sAc_no);
			
			iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",sAc_no);
			if (iRet)
			{
				vtcp_log("%s,%d,ac_no[%s]����mdm_ac_rel����!",__FILE__,__LINE__,sAc_no);
				strcpy(g_pub_tx.reply,"C114");
				goto ErrExit;
			}
			iRet=pub_com_ChkAcCheq( g_mdm_ac_rel.ac_id, g_mdm_ac_rel.ac_seqn, g_pub_tx.note_type, g_pub_tx.beg_note_no, g_pub_tx.end_note_no, sts );
			if( iRet == -1 )
			{
				sprintf( acErrMsg, "�ͻ��޴�ƾ֤��ƾ֤״̬��һ��!ac_id=[%d],\
				         note_type=[%s],cheq_beg_no=[%s],cheq_end_no=[%s]",\
				         g_mdm_ac_rel.ac_id,g_pub_tx.note_type,g_pub_tx.beg_note_no, g_pub_tx.end_note_no );
				WRITEMSG
				goto ErrExit;
			}
			if ( sts[0] == '0' )
			{
				sprintf( acErrMsg, "�ͻ��޴�ƾ֤!sts=[%s]", sts);	
				WRITEMSG
				strcpy( g_pub_tx.reply, "N080" );
				goto ErrExit;
			}else if ( sts[0] == '2' )
			{
				sprintf( acErrMsg, "��ƾ֤�ѹ�ʧ!sts=[%s]", sts);	
				WRITEMSG
				strcpy( g_pub_tx.reply, "N081" );
				goto ErrExit;
			}else if ( sts[0] == '3' )
			{
				sprintf( acErrMsg, "��ƾ֤�ѻ���!sts=[%s]", sts);	
				WRITEMSG
				strcpy( g_pub_tx.reply, "N082" );
				goto ErrExit;
			}else if ( sts[0] == '4' )
			{
				sprintf( acErrMsg, "��ƾ֤������!sts=[%s]", sts);	
				WRITEMSG
				strcpy( g_pub_tx.reply, "N083" );
				goto ErrExit;
			}else if ( sts[0] == '5' )
			{
				sprintf( acErrMsg, "��ƾ֤��ʹ��!sts=[%s]", sts);	
				WRITEMSG
				strcpy( g_pub_tx.reply, "N084" );
				goto ErrExit;
			}
			
		}
  		
  	}
  	if (sFlag[0] == '2')		/*���ͻ�����Աƾ֤*/
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
				vtcp_log("%s,%d,ac_no[%s]����mdm_ac_rel����!",__FILE__,__LINE__,sAc_no);
				strcpy(g_pub_tx.reply,"C114");
				goto ErrExit;
			}
			
			if(g_mdm_ac_rel.ac_seqn==0)
		  		g_mdm_ac_rel.ac_seqn=1;
		}
	  		
		iRet=pub_com_NoteUse(g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn,g_pub_tx.note_type,g_pub_tx.beg_note_no,g_pub_tx.end_note_no,sTel);
		if(iRet)
		{
		        sprintf(acErrMsg,"����ƾ֤����!");
		        WRITEMSG
		        goto ErrExit;
		}
		
		/*������һ������ƾ֤*/
		iRet = pub_com_GetNoteMin(g_pub_tx.note_type,sNextVocnum,sTel);
		if(iRet)
		{
		        sprintf(acErrMsg,"û��ƾ֤��!");
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

