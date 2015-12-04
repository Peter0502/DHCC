/***************************************************************
* �� �� ��: spD604.c
* ����������֤����Ϣά��
* ��    ��: mike
* ������ڣ�2003��3��9��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"

spD604()
{   
	int ret; 

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data("0300",g_pub_tx.ac_no);          	/* �˺� */
	get_zd_data("0680",g_pub_tx.draw_id_yn);      	/* ֤��֧ȡ */
	get_zd_data("0670",g_pub_tx.id_type); 	     	/* ֤������ */
	get_zd_data("0620",g_pub_tx.id_no); 	     	/* ֤������ */

	if( g_pub_tx.ac_no[0]=='5' )
	{
		sprintf(acErrMsg,"�����봢���˻�!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O164");
		goto ErrExit;
	}

	if( g_pub_tx.draw_id_yn[0]=='Y' && !strlen(g_pub_tx.id_no) )
  	{
	   sprintf(acErrMsg,"֤�����벻��Ϊ��!");
	   WRITEMSG
	   strcpy(g_pub_tx.reply,"O220");
	   goto ErrExit;
	}

	/*�����ʻ�����*/
	ret = Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'",g_pub_tx.ac_no);
  	if( ret )
  	{
	   sprintf(acErrMsg,"�����˻����ձ��в����ڸü�¼");
	   WRITEMSG
	   strcpy(g_pub_tx.reply,"W010");
	   goto ErrExit;
	}

	ret = Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
  	if( ret==100 )
  	{
	   sprintf(acErrMsg,"�����˻����ձ��в����ڸü�¼");
	   WRITEMSG
	   strcpy(g_pub_tx.reply,"M003");
	   goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	pub_base_strpack(g_mdm_ac_rel.id_no);
	pub_base_strpack(g_pub_tx.id_no);

	if( g_mdm_ac_rel.draw_id_yn[0]=='N' && g_pub_tx.draw_id_yn[0]=='N' )
  	{
	   sprintf(acErrMsg,"δ���κ��޸�!");
	   WRITEMSG
	   strcpy(g_pub_tx.reply,"O224");
	   goto ErrExit;
	}

	if( g_mdm_ac_rel.draw_id_yn[0]=='Y' && g_pub_tx.draw_id_yn[0]=='Y' )
	{
		if( (g_mdm_ac_rel.id_type[0]==g_pub_tx.id_type[0]) && \
			!strcmp(g_mdm_ac_rel.id_no,g_pub_tx.id_no) )
  		{
	   		sprintf(acErrMsg,"δ���κ��޸�!");
	  		WRITEMSG
	   		strcpy(g_pub_tx.reply,"O224");
	  		goto ErrExit;
		}
	}

	if(g_pub_tx.draw_id_yn[0]=='N')
		strcpy(g_mdm_ac_rel.draw_id_yn,g_pub_tx.draw_id_yn);
	else
	{
		strcpy(g_mdm_ac_rel.draw_id_yn,g_pub_tx.draw_id_yn);
		strcpy(g_mdm_ac_rel.id_type,g_pub_tx.id_type);
		strcpy(g_mdm_ac_rel.id_no,g_pub_tx.id_no);
	}

	ret = Mdm_ac_rel_Upd_Upd(g_mdm_ac_rel,g_pub_tx.reply);
  	if( ret )
  	{
	   sprintf(acErrMsg,"�����˻����ձ��в����ڸü�¼");
	   WRITEMSG
	   strcpy(g_pub_tx.reply,"W010");
	   goto ErrExit;
	}

	Mdm_ac_rel_Clo_Upd();

    /*** �Ǽǽ�����ˮ ***/
	if( pub_ins_trace_log() )
	{
	 	sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
	  	WRITEMSG
	    goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"֤����Ϣά���ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"֤����Ϣά��ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
