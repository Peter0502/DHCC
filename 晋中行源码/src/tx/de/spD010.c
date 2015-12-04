/***************************************************************
* �� �� ��:     spD010.c
* ����������    �Թ����ڿ����Ż�ת���˺�����Ϊ���Ż��˺�����ac_no-cnt
*
* ��    ��:     hhb
* ������ڣ�    2011��8��11��
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

spD010()
{
	int fun_mainacno(char *acno),fun_sonacno(char *acno1,char *acno2);
	char og_acno,om_acno,main_acno[30];
	memset(&og_acno,0x00,sizeof(og_acno));
	memset(&om_acno,0x00,sizeof(om_acno));
	memset(main_acno,0x00,sizeof(main_acno));

	pub_base_sysinit();

	get_zd_data("0670",&og_acno);/**�Ƿ񿪼��Ż�,1 ��, 0 ��**/
	get_zd_data("0680",&om_acno);/**�Ƿ����ʻ�,1 ��, 0 ��**/
	get_zd_data("0300",main_acno);/**���ʻ�**/
	get_zd_data("1167",g_pub_tx.ac_no);

	pub_base_strpack(g_pub_tx.ac_no);
	pub_base_strpack(main_acno);
	vtcp_log("[%s][%d] og_acno=[%c],om_acno=[%c],ac_no=[%s],main_acno=[%s]\n",__FILE__,__LINE__,og_acno,om_acno,g_pub_tx.ac_no,main_acno);

	if(og_acno=='1' && om_acno=='1')
		g_reply_int=fun_mainacno(g_pub_tx.ac_no);
	if(og_acno=='1' && om_acno=='0')
		g_reply_int=fun_sonacno(g_pub_tx.ac_no,main_acno);
	if(g_reply_int)
		{
			sprintf(acErrMsg,"�˺�ת��ʧ��![%d][%s]",g_reply_int,g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

	vtcp_log("[%s][%d] ac_no=[%s],main_acno=[%s]\n",__FILE__,__LINE__,g_pub_tx.ac_no,main_acno);
	set_zd_data("1167",g_pub_tx.ac_no);

OkExit:
        strcpy(g_pub_tx.reply,"0000");
        sprintf(acErrMsg,"�����Ż��˺�ת���ɹ�![%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        return 0;
ErrExit:
        sprintf(acErrMsg,"�����Ż��˺�ת��ʧ��![%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        return 1;
}
/**�����ʻ��Ӻ���**/
int fun_mainacno(char *acno)
{
	g_reply_int=0;
	g_reply_int=Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'",acno);
	if(g_reply_int)
		{
			sprintf(acErrMsg,"�������ݿ��α�ʧ��![%d][%s]",g_reply_int,g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	g_reply_int=Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
	if(g_reply_int)
		{
			sprintf(acErrMsg,"���ݿ��α�ȡֵʧ��![%d][%s]",g_reply_int,g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	strcat(g_mdm_ac_rel.ac_no,"-1");
	strcpy(acno,g_mdm_ac_rel.ac_no);
	g_reply_int=Mdm_ac_rel_Upd_Upd(g_mdm_ac_rel,g_pub_tx.reply);
	if(g_reply_int)
		{
			sprintf(acErrMsg,"���ݿ���¼�¼ʧ��![%d][%s]",g_reply_int,g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	g_reply_int=Mdm_ac_rel_Clo_Upd();
	if(g_reply_int)
		{
			sprintf(acErrMsg,"���ݿ�ر��α�ʧ��![%d][%s]",g_reply_int,g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/**�����˻��Ӻ���**/
int fun_sonacno(char *acno1,char *acno2)
{
	int cnt=0;
	g_reply_int=0;
	char tail[20],head[30];
	memset(tail,0x00,sizeof(tail));
	memset(head,0x00,sizeof(head));
	if(!strlen(acno2))
	{
		strcpy(acErrMsg,"���˺Ŵ�!");
		strcpy(g_pub_tx.reply,"C115");
		WRITEMSG
		goto ErrExit;
	}
	memcpy(head,acno2,strlen(acno2)-2);
	cnt=sql_count("mdm_ac_rel","substr(ac_no,0,%d)='%s'",strlen(head),head);
	if(!cnt)
	{
		strcpy(acErrMsg,"���˺Ŵ�!");
                strcpy(g_pub_tx.reply,"C115");
                WRITEMSG
                goto ErrExit;
	}
	cnt++;
	sprintf(tail,"-%d",cnt);
	strcat(head,tail);
	g_reply_int=Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'",acno1);
	if(g_reply_int)
		{
			sprintf(acErrMsg,"�������ݿ��α�ʧ��![%d][%s]",g_reply_int,g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	g_reply_int=Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
	if(g_reply_int)
		{
			sprintf(acErrMsg,"���ݿ��α�ȡֵʧ��![%d][%s]",g_reply_int,g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	strcpy(g_mdm_ac_rel.ac_no,head);
	strcpy(acno1,g_mdm_ac_rel.ac_no);
	g_reply_int=Mdm_ac_rel_Upd_Upd(g_mdm_ac_rel,g_pub_tx.reply);
	if(g_reply_int)
		{
			sprintf(acErrMsg,"���ݿ���¼�¼ʧ��![%d][%s]",g_reply_int,g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	g_reply_int=Mdm_ac_rel_Clo_Upd();
	if(g_reply_int)
		{
			sprintf(acErrMsg,"���ݿ�ر��α�ʧ��![%d][%s]",g_reply_int,g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
OkExit:
	return 0;
ErrExit:
	return 1;
}
