/*****************************************************
* �� �� ��:  sp9559.c
* ���������� ͬ�ǽ��׵�ǰ̨У����������������к��Ƿ���ȷ,��ȡ�����׻����Ľ����к�
* �� �� ��:  changetab
* ��    ��:  Liuxuan
* ������ڣ� 2006��8��17�� 
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "changetab_c.h"

int sp9559()
{
	int ret=0;
	char or_br_no[11],kinbr[6];
	struct changetab_c changetab;    
	
	memset(or_br_no,'\0',sizeof(or_br_no));
	memset(&changetab,0x00,sizeof(changetab));
 	vtcp_log("sp9559��ʼ��[%s][%d]",__FILE__,__LINE__);

	pub_base_sysinit();

	memset(kinbr,'\0',sizeof(kinbr));
	get_zd_data("0020",kinbr);
	get_zd_data("0860",or_br_no);
	or_br_no[10]='\0';
   	pub_base_strpack(or_br_no);	
	vtcp_log("[%s][%d]  or_br_no=[%s],kinbr=[%s]",__FILE__,__LINE__,or_br_no,kinbr);
	ret = Changetab_Sel(g_pub_tx.reply,&changetab, "change_no='%s'",or_br_no);
	if ( ret )
	{
		sprintf(acErrMsg,"��ѯ��������[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}	
	
 vtcp_log("[%s][%d]  br_name_f=[%s]",__FILE__,__LINE__,changetab.br_name);

 set_zd_data("0961",changetab.br_name);  /* ����ȫ��: ֧��ϵͳ���� */
/*
	memset(&changetab,'\0',sizeof(changetab));
	ret = Changetab_Sel(g_pub_tx.reply,&changetab, "br_no='%s'",kinbr);
	if ( ret )
	{
		sprintf(acErrMsg,"��ѯ���׻����Ľ����кų���[%d][%s]",ret,kinbr);
		WRITEMSG
		goto ErrExit;
	}	
*/
 vtcp_log("[%s][%d]  br_name_f=[%s]",__FILE__,__LINE__,changetab.br_name);
 set_zd_data("0870",changetab.change_no);  /* �����ж�Ӧ�Ľ����к� */
 set_zd_data("0810",changetab.br_name);  /* �����ж�Ӧ�Ľ����к� */

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

