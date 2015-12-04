/*************************************************
* �� �� ��:  sp9972.c
* ���������� �����С���ʺ� (��������)
*
* ��    ��:  rob
* ������ڣ� 2003��8��06��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mdm_attr_c.h"

int sp9972()
{
	int	ret;
	struct mdm_attr_c mdm_attr_c;
	struct mdm_ac_rel_c v_mdm_ac_rel;
	struct mdm_ac_rel_c sMdm_ac_rel;

	memset(&mdm_attr_c, 0x0, sizeof(struct mdm_attr_c));
	memset(&v_mdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c));
	memset(&sMdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c));

	pub_base_sysinit();
	/*ǰ̨ȡֵ*/
	get_zd_data("0300",g_pub_tx.crd_no);
	pub_base_old_acno(g_pub_tx.crd_no); 
	get_zd_data("0310",g_pub_tx.ac_no); 
	pub_base_old_acno(g_pub_tx.ac_no);

	get_zd_data("0590",g_pub_tx.beg_note_no);  
	get_zd_data("0590",g_pub_tx.end_note_no); 
	pub_base_strpack( g_pub_tx.beg_note_no );
	if( !strlen(g_pub_tx.beg_note_no) )
		goto OkExit;

	/* ��ѯ�����ʺŶ��ձ� */
	pub_base_strpack(g_pub_tx.crd_no);
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'",
		g_pub_tx.crd_no);
	if (ret != 0 && ret != 100)
	{
		sprintf(acErrMsg, "��ѯ�����ʺŶ��ձ����!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "P147");
		goto ErrExit;
	}
	else if (ret == 100)
	{
		sprintf(acErrMsg, "�����ʺŶ��ձ��в����ڸü�¼!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "B117");
		goto ErrExit;
	}
	strcpy(g_pub_tx.mdm_code, sMdm_ac_rel.mdm_code);

	/*** ��ȡ�������� ***/
	ret=Mdm_attr_Sel(&g_pub_tx.reply,&mdm_attr_c," mdm_code='%s' ", \
					 g_pub_tx.mdm_code);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�ý������Բ�����![%d][%s]",
			g_reply_int,g_pub_tx.mdm_code);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P132");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"ȡ���������쳣![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P133");
		goto ErrExit;
	}

	strcpy(g_pub_tx.note_type,mdm_attr_c.note_type);
	strcpy(g_pub_tx.end_note_no,g_pub_tx.beg_note_no);

vtcp_log("PQAAA[%s]",g_pub_tx.beg_note_no );TRACE
	sprintf(acErrMsg,"mdm_attr_c.note_type=[%s]",mdm_attr_c.note_type);
	WRITEMSG

	/*��Ҫƾ���ſ������翨�ࣺ����ʱ��Ҫ���뿨��*/
	if( mdm_attr_c.no_ind[0]=='Y' )
	{
		if( !strlen(g_pub_tx.ac_no) )
		{
			sprintf(acErrMsg,"��Ҫ���뿨��");
			WRITEMSG
			strcpy( g_pub_tx.reply,"D111" );
			goto ErrExit;
		}

		sprintf(acErrMsg,"����Ŀ���Ϊ[%s]",g_pub_tx.ac_no);
		WRITEMSG

		/**��鿨���Ƿ��Ѿ�����**/
		ret=Mdm_ac_rel_Sel( g_pub_tx.reply,&v_mdm_ac_rel,
			"ac_no='%s'",g_pub_tx.ac_no ) ;
		if( ret && ret!=100 ) goto ErrExit;
		else if( !ret )
		{
			sprintf(acErrMsg,"��Ҫ���뿨��[%s],ret=[%d]",g_pub_tx.ac_no,ret);
			WRITEMSG
			strcpy( g_pub_tx.reply,"D113" );
			goto ErrExit;
		}
		strcpy( g_pub_tx.ac_no,g_pub_tx.ac_no );

		pub_base_strpack(g_pub_tx.beg_note_no);
		if( strlen(g_pub_tx.ac_no) && !strlen(g_pub_tx.beg_note_no) )
		{
			/*** ���ݿ���ȡƾ֤�� ***/
			pub_card_CardToNote(g_pub_tx.ac_no,g_pub_tx.beg_note_no);
			strcpy(g_pub_tx.end_note_no,g_pub_tx.beg_note_no);
vtcp_log("PQAAA[%s]",g_pub_tx.end_note_no );
		}
vtcp_log("PAAAA[%s]",g_pub_tx.end_note_no );
	}

	/* ��Ҫƾ֤���� */
	if( mdm_attr_c.only_ind[0]=='Y' ) 
	{
vtcp_log("PBBBBBBBBBBBBB[%s]",g_pub_tx.note_type );
		ret=pub_com_ChkNoteMin( g_pub_tx.note_type,g_pub_tx.beg_note_no,
							 g_pub_tx.tel);
		if( ret )
		{
			goto ErrExit;
		}
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

