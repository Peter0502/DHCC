/***************************************************************
* �� �� ��: spD407.c
* ���������������۽���
* ��    ��: Azz
* ������ڣ�2005��1��13��
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mdm_attr_c.h"

spD407()
{
	int ret=0;
	char ac_type[2];
	int unprt_cnt;
	struct mdm_attr_c	g_mdm_attr;

	memset(&g_mdm_attr,0x00,sizeof(struct mdm_attr_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data("0310",g_pub_tx.ac_no);			/*�ʺ�*/
	g_pub_tx.ac_seqn=DD_BOOK_AC_SEQN;			/*�ʺ����*/

	pub_base_old_acno( g_pub_tx.ac_no );        /**�Ծ��ʺŵĴ���**/

  	ret = Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'", \
						   g_pub_tx.ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�������Ա��в����ڸü�¼[%s]",g_pub_tx.ac_no);
		WRITEMSG
		strcpy (g_pub_tx.reply,"N055");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/*------add by xxx 20050801---for stat-begin------*/
	if( g_mdm_ac_rel.note_sts[0] == '*' )
	{
		sprintf( acErrMsg, "����������!" );
		WRITEMSG
		strcpy (g_pub_tx.reply,"P172");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0] == '1' )
	{
		sprintf( acErrMsg, "��ƾ֤�ѹ�ʧ!" );
		WRITEMSG
		strcpy (g_pub_tx.reply,"N081");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0] != '0' )
	{
		sprintf( acErrMsg, "��ƾ֤״̬������" );
		WRITEMSG
		strcpy (g_pub_tx.reply,"M013");
		goto ErrExit;
	}
	/*------add by xxx 20050801---for stat-end------*/

	if( strncmp(g_mdm_ac_rel.mdm_code,"0010",4) && \
		strncmp(g_mdm_ac_rel.mdm_code,"0011",4) )
	{
		sprintf(acErrMsg,"����������˺�!");
		WRITEMSG
		strcpy (g_pub_tx.reply,"O189");
		goto ErrExit;
	}

  	ret = Mdm_attr_Sel(g_pub_tx.reply,&g_mdm_attr,"mdm_code='%s'", \
					   g_mdm_ac_rel.mdm_code);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�������Ա��в����ڸü�¼[%s]",g_mdm_ac_rel.mdm_code);
		WRITEMSG
		strcpy (g_pub_tx.reply,"N055");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;

	if( g_mdm_attr.prt_ind[0]=='Y' )
	{
		ret = pub_base_patch_book(g_mdm_attr);
		if( ret<0 )
		{
			sprintf(acErrMsg,"���ò����ۺ���ʧ��!");
			WRITEMSG
			goto ErrExit;
		}
		else if( ret==0 )
		{
			sprintf(acErrMsg,"�û�û��δ����ϸ!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O190");
			goto ErrExit;
		}

		set_zd_data("0110","1");
	}
	else if( g_mdm_attr.prt_ind[0]=='N' )
	{
		sprintf(acErrMsg,"�ý���Ϊ�Ǵ�ӡ�����!");
		WRITEMSG
		strcpy (g_pub_tx.reply,"N053");
		goto ErrExit;
	}
	else 
	{
		sprintf(acErrMsg,"�ý��ʴ�ӡ��������ȷ!");
		WRITEMSG
		strcpy (g_pub_tx.reply,"N054");
		goto ErrExit;
	}

    /*** �Ǽǽ�����ˮ ***/
	if( pub_ins_trace_log( ) )
	{
	 	sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
	  	WRITEMSG
	    goto ErrExit;
	}
	unprt_cnt=sql_count("mdm_unprt_hst","ac_id=%ld " ,g_pub_tx.ac_id);
	if (unprt_cnt>0)
		set_zd_data("0200","1");
    else
		set_zd_data("0200","0");
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�����۳ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"������ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
