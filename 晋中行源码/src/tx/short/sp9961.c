/*************************************************
* �� �� ��:  sp9961.c
* ���������� �����ʺţ�����֤����Ϣ
*
* ��    ��:  mike
* ������ڣ� 2003��07��05��
*
* �޸ļ�¼�� 
*   ��   ��:20130326
*   �� �� ��:
*   �޸�����:ʹ����ǰ��λ��ICCARDHEADCODE����CARDHEADCODE������67����Ϊ��ʱ��ʧ״̬������
ʹ����ǰ��λ��ICCARDHEADCODE����CARDHEADCODE������67������Ϊ��ʱ��ʧ״̬����
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mdm_attr_c.h"
#include "card.h"

int sp9961()
{
	int ret=0;
	char name[51],flag[2];

	struct mdm_attr_c	g_mdm_attr;

	memset( &g_mdm_ac_rel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &g_mdm_attr, 0x00, sizeof( struct mdm_attr_c ) );

	get_zd_data( "0300", g_pub_tx.ac_no );		/* �ʺ� */
	pub_base_old_acno( g_pub_tx.ac_no );        /**�Ծ��ʺŵĴ���**/
	
	get_zd_data("0670",flag);
	vtcp_log("[%s][%d]  0670[%s]\n",__FILE__,__LINE__,flag);

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'", \
						 g_pub_tx.ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�����ڸ��ʺ�!!ac_no=[%s]",g_pub_tx.ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҵ���");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	/*** ������Ҫ���ʧ״̬������ 20130710 ���Ϊ
	if(( memcmp(g_pub_tx.ac_no,ICCARDHEADCODE,6)!=0  && memcmp(g_pub_tx.ac_no,CARDHEADCODE,6)!=0 ) || strlen(flag))
	{
	if( g_mdm_ac_rel.note_sts[0]=='1' )
			{
		sprintf(acErrMsg,"�ý����ѹ�ʧ!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P170");
		goto ErrExit;
			}
	}
	else
	***/
  if( g_mdm_ac_rel.note_sts[0]=='2' )
	{
		sprintf(acErrMsg,"�ý����ѹ�ʧ��֤!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P171");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='3' )
	{
		sprintf(acErrMsg,"�ý����ѱ���֤!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M022");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='4' )
	{
		sprintf(acErrMsg,"�ý����Ѳ��ỻ֤!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D215");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='*' )
	{
		sprintf(acErrMsg,"�ý���������������!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M012");
		goto ErrExit;
	}

	ret = Mdm_attr_Sel( g_pub_tx.reply,&g_mdm_attr,"mdm_code='%s'", \
						 g_mdm_ac_rel.mdm_code);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�����ڸ��ʺ�!!ac_no=[%s]",g_pub_tx.ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҵ���!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/*** ���಻��������֤��֧ȡ ***/
	/**modified by ligl2008 2006-12-18 17:42 ��Ҳ��Ҫ֤��֧ȡ**/
	/*if( g_mdm_attr.mdm_type[0]=='3' || g_mdm_attr.mdm_type[0]=='4' || \ 
		g_mdm_attr.mdm_type[0]=='5' )
	{
		sprintf(acErrMsg,"�����˻�����������֤��֧ȡ!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D227");
		goto ErrExit;
	}*/

	ret = pub_base_getacname( g_mdm_ac_rel.ac_id, name );
	if( ret )
	{
		sprintf(acErrMsg,"ȡ����������!");
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("sp9961 ������[%s][%d][%s]ret=[%d]id_no=[%s]",__FILE__,__LINE__,name,ret,g_mdm_ac_rel.id_no);

	set_zd_data( "0300", g_pub_tx.ac_no );				/* �˺� */
	set_zd_data( "0250", name );						/* ���� */
	set_zd_data( "0680", g_mdm_ac_rel.draw_id_yn );		/* ֤��֧ȡ */
	set_zd_data( "0670", g_mdm_ac_rel.id_type );		/* ֤������ */
    set_zd_data( "0620", g_mdm_ac_rel.id_no );          /*֤����*/
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
