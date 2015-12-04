/*************************************************
* �� �� ��: spC320.c
* �������������֤��֤�Ǽ�
* 
* ��    ��: houyi
* ������ڣ�20120323

�˲�����
	00	������ݺ���������һ�£��Ҵ�����Ƭ	
	01	������ݺ���������һ�£�����������Ƭ	
	02	������ݺ�����ڣ�����������ƥ��	
	03	������ݺ��벻����	
	04	��������	
	05	���������Ϣ�޷���ʵ
	06	���������Ϣ���
	07  ���꼰�������ϲ�����
insert into tx_def values('1320','���֤��֤ά��','10000000000000000000010000000000000000000000000000000000000001000001000000000000000000000000000000000000000000000000000000000000','1','4','0','0',0.00);
insert into tx_flow_def values('1320',0,'1320','#$');
insert into tx_sub_def values('1320','���֤��֤�Ǽ�','spC320','0','0');
*************************************************/
#include "stdio.h"
#define EXTERN
#include "public.h"
#include "cif_id_code_rel_c.h"
#include "cif_per_inf_c.h"
#include "cif_email_inf_c.h"
#include "cif_addr_inf_c.h"
#include "cif_inf_c.h"
#include "cif_sfhc_inf_c.h"

spC320()
{
	struct cif_id_code_rel_c sCif_id_code_rel;
	struct cif_per_inf_c     sCif_per;
	struct cif_email_inf_c   sCif_email;
	struct cif_addr_inf_c    sCif_addr_inf;
	struct cif_inf_c         sCif_inf;
	struct cif_sfhc_inf_c    sCif_sfhc_inf;

	char   phone1[61];
	char   phone2[61];
	char   result[3];
	char   post_code[11];
	char   addr [61];
	
	memset(&sCif_id_code_rel,0x00,sizeof(sCif_id_code_rel));
	memset(&sCif_per,        0x00,sizeof(sCif_per));
	memset(&sCif_email,      0x00,sizeof(sCif_email));
	memset(&sCif_addr_inf,  0x00,sizeof(sCif_addr_inf));
	memset(&sCif_inf,       0x00,sizeof(sCif_inf));
	memset(&sCif_sfhc_inf,0x00,sizeof(sCif_sfhc_inf));

	memset(phone1,0x00,sizeof(phone1));
	memset(phone2,0x00,sizeof(phone2));
	memset(result,0x00,sizeof(result));
	memset(post_code,0x00,sizeof(post_code));
	memset(addr,0x00,sizeof(addr));
	
	pub_base_sysinit();
	
	get_zd_data("0680",sCif_id_code_rel.id_type);/**֤������**/
	get_zd_data("0620",sCif_id_code_rel.id_no);/**֤������**/
	get_zd_data("0220",result);/**�˲���**/
	get_zd_data("0580",phone1);/**�̶��绰**/
	get_zd_data("0590",phone2);/**�ͻ��ֻ�**/
	get_zd_data("0260",addr);/**�ͻ���ַ**/
	get_zd_data("0730",post_code);/**��������**/

	
	g_reply_int=Cif_id_code_rel_Sel(g_pub_tx.reply,&sCif_id_code_rel,"id_type='%s' and id_no='%s'",sCif_id_code_rel.id_type,sCif_id_code_rel.id_no);
	if(g_reply_int)
	{
		vtcp_log("[%s][%d] ��ѯ cif_id_code_rel ����� %d",__FILE__,__LINE__,g_reply_int);
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	g_reply_int=Cif_sfhc_inf_Dec_Upd(g_pub_tx.reply,"cif_no=%ld",sCif_id_code_rel.cif_no);
	if(g_reply_int)
	{
		vtcp_log("[%s][%d] ���� Cif_sfhc_inf �����α���� %d",__FILE__,__LINE__,g_reply_int);
		strcpy(g_pub_tx.reply,"D101");
		goto ErrExit;
	}

	/***���˿ͻ���Ϣ��ά��***/
	g_reply_int=Cif_sfhc_inf_Fet_Upd(&sCif_sfhc_inf,g_pub_tx.reply);
	if(g_reply_int==100)
	{
		sCif_sfhc_inf.cif_no=sCif_id_code_rel.cif_no;
		strcpy(sCif_sfhc_inf.country,result);
		strcpy(sCif_sfhc_inf.clerk_no,g_pub_tx.tel);
		sCif_sfhc_inf.mar_date=g_pub_tx.tx_date;
		g_reply_int=Cif_sfhc_inf_Ins(sCif_sfhc_inf,g_pub_tx.reply);
		if(g_reply_int)
		{
			vtcp_log("[%s][%d] ���� Cif_sfhc_inf ��¼���� %d",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"D107");
			goto ErrExit;
		}
		
		g_reply_int=sql_execute("update cif_inf set sts='2' where cif_no=%ld", sCif_id_code_rel.cif_no);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"����cif_inf ʧ�ܣ�");
			WRITEMSG
			goto ErrExit;
		}
		
		Cif_sfhc_inf_Clo_Upd();
		goto Ok1;
	}
	else if(g_reply_int)
	{
		vtcp_log("[%s][%d] ȡ Cif_sfhc_inf ���¼�¼���� %d",__FILE__,__LINE__,g_reply_int);
		strcpy(g_pub_tx.reply,"D102");
		goto ErrExit;
	}

	strcpy(sCif_sfhc_inf.country,result);
	strcpy(sCif_sfhc_inf.clerk_no,g_pub_tx.tel);
	sCif_sfhc_inf.mar_date=g_pub_tx.tx_date;
	g_reply_int=Cif_sfhc_inf_Upd_Upd(sCif_sfhc_inf,g_pub_tx.reply);
	if(g_reply_int)
	{
		vtcp_log("[%s][%d] ���� Cif_sfhc_inf ��¼���� %d",__FILE__,__LINE__,g_reply_int);
		strcpy(g_pub_tx.reply,"D105");
		goto ErrExit;
	}
	Cif_sfhc_inf_Clo_Upd();

Ok1:
	g_reply_int=Cif_addr_inf_Dec_Upd(g_pub_tx.reply,"cif_no=%ld and addr_type='2' order by cif_no,addr_seqn desc",sCif_id_code_rel.cif_no);
	if(g_reply_int)
	{
		vtcp_log("[%s][%d] ���� Cif_addr_inf �����α���� %d",__FILE__,__LINE__,g_reply_int);
		strcpy(g_pub_tx.reply,"D101");
		goto ErrExit;
	}

	/***�ͻ���ַ��Ϣ��ά��***/
	g_reply_int=Cif_addr_inf_Fet_Upd(&sCif_addr_inf,g_pub_tx.reply);
vtcp_log("[%s][%d] g_reply_int=[%d]",__FILE__,__LINE__,g_reply_int);
Cif_addr_inf_Debug(&sCif_addr_inf);
	if(g_reply_int==100)
	{
		sCif_addr_inf.cif_no=sCif_id_code_rel.cif_no;
		sCif_addr_inf.addr_type[0]='2';
		strcpy(sCif_addr_inf.post_code,post_code);
		strcpy(sCif_addr_inf.addr,addr);
		sCif_addr_inf.addr_seqn=1+sql_count("cif_addr_inf","cif_no=%ld",sCif_id_code_rel.cif_no);
		
		g_reply_int=Cif_addr_inf_Ins(sCif_addr_inf,g_pub_tx.reply);
		if(g_reply_int)
		{
			vtcp_log("[%s][%d] ���� Cif_addr_inf ��¼���� %d",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"D107");
			goto ErrExit;
		}
		Cif_addr_inf_Clo_Upd();
		goto Ok2;
	}
	else if(g_reply_int)
	{
		vtcp_log("[%s][%d] ȡ Cif_addr_inf ���¼�¼���� %d",__FILE__,__LINE__,g_reply_int);
		strcpy(g_pub_tx.reply,"D102");
		goto ErrExit;
	}
	sCif_addr_inf.cif_no=sCif_id_code_rel.cif_no;
	sCif_addr_inf.addr_type[0]='2';
	strcpy(sCif_addr_inf.post_code,post_code);
	strcpy(sCif_addr_inf.addr,addr);
	g_reply_int=Cif_addr_inf_Upd_Upd(sCif_addr_inf,g_pub_tx.reply);
	if(g_reply_int)
	{
		vtcp_log("[%s][%d] ���� Cif_addr_inf ��¼���� %d",__FILE__,__LINE__,g_reply_int);
		strcpy(g_pub_tx.reply,"D105");
		goto ErrExit;
	}
	Cif_addr_inf_Clo_Upd();
Ok2:
	if(strlen(phone1)>0)
	{
		g_reply_int=Cif_email_inf_Dec_Upd(g_pub_tx.reply,"cif_no=%ld and addr_type='1' ",sCif_id_code_rel.cif_no);
		if(g_reply_int)
		{
			vtcp_log("[%s][%d] ���� Cif_email_inf �����α���� %d",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"D101");
			goto ErrExit;
		}
		
		/***�ͻ����ӵ�ַ��Ϣ��-�̶��绰ά��***/
		g_reply_int=Cif_email_inf_Fet_Upd(&sCif_email,g_pub_tx.reply);
		if(g_reply_int=100)
		{
			sCif_email.cif_no=sCif_id_code_rel.cif_no;
			strcpy(sCif_email.email,phone1);
			sCif_email.addr_type[0]='1';
			sCif_email.addr_seqn=1+sql_count("cif_email_inf","cif_no=%ld",sCif_id_code_rel.cif_no);
			g_reply_int=Cif_email_inf_Ins(sCif_email,g_pub_tx.reply);
			if(g_reply_int)
			{
				vtcp_log("[%s][%d] ���� Cif_email_inf ��¼���� %d",__FILE__,__LINE__,g_reply_int);
				strcpy(g_pub_tx.reply,"D107");
				goto ErrExit;
			}
			Cif_email_inf_Clo_Upd();
			goto Ok3;
		}
		else if(g_reply_int)
		{
			vtcp_log("[%s][%d] ȡ Cif_addr_inf ���¼�¼���� %d",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"D102");
			goto ErrExit;
		}
		strcpy(sCif_email.email,phone1);
		g_reply_int=Cif_email_inf_Upd_Upd(sCif_email,g_pub_tx.reply);
		if(g_reply_int)
		{
			vtcp_log("[%s][%d] ���� Cif_email_inf ��¼���� %d",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"D105");
			goto ErrExit;
		}
		Cif_email_inf_Clo_Upd();
	}
Ok3:
	if(strlen(phone2)>0)
	{
		g_reply_int=Cif_email_inf_Dec_Upd(g_pub_tx.reply,"cif_no=%ld and addr_type='5' ",sCif_id_code_rel.cif_no);
		if(g_reply_int)
		{
			vtcp_log("[%s][%d] ���� Cif_email_inf �����α���� %d",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"D101");
			goto ErrExit;
		}

		/***�ͻ����ӵ�ַ��Ϣ��-�ֻ���ά��***/
		g_reply_int=Cif_email_inf_Fet_Upd(&sCif_email,g_pub_tx.reply);
		if(g_reply_int=100)
		{
			sCif_email.cif_no=sCif_id_code_rel.cif_no;
			strcpy(sCif_email.email,phone2);
			sCif_email.addr_type[0]='5';
			sCif_email.addr_seqn=1+sql_count("cif_email_inf","cif_no=%ld",sCif_id_code_rel.cif_no);
			g_reply_int=Cif_email_inf_Ins(sCif_email,g_pub_tx.reply);
			if(g_reply_int)
			{
				vtcp_log("[%s][%d] ���� Cif_email_inf ��¼���� %d",__FILE__,__LINE__,g_reply_int);
				strcpy(g_pub_tx.reply,"D107");
				goto ErrExit;
			}
			Cif_email_inf_Clo_Upd();
			goto Ok4;
		}
		else if(g_reply_int)
		{
			vtcp_log("[%s][%d] ȡ Cif_email_inf ���¼�¼���� %d",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"D102");
			goto ErrExit;
		}
		strcpy(sCif_email.email,phone2);
		g_reply_int=Cif_email_inf_Upd_Upd(sCif_email,g_pub_tx.reply);
		if(g_reply_int)
		{
			vtcp_log("[%s][%d] ���� Cif_email_inf ��¼���� %d",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"D105");
			goto ErrExit;
		}
		Cif_email_inf_Clo_Upd();
	}
Ok4:
        strcpy(g_pub_tx.brf,"���֤�˲���ά��");
	g_reply_int=pub_ins_trace_log();
	if(g_reply_int)
	{
		vtcp_log("[%s] [%d] �Ǽǽ�����ˮʧ�� [%d] ",__FILE__,__LINE__,g_reply_int);
		strcpy(g_pub_tx.reply,"P126");
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	Cif_sfhc_inf_Clo_Upd();
	set_zd_data("0120",g_pub_tx.reply);
	return g_reply_int;
}
