/*************************************************
* �� �� ��: sp6673.c
* �����������˽�������Ŵ���������
*
* ��    ��: chenchao
* ������ڣ�2011-7-20 9:50:43
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:

insert into tx_def values('6673','�Ŵ���������','10000000000000000000000000000000000000000000000000000000010000000010000000000000000000000000000000000000000000000000000000000000','0','4','0');
insert into tx_flow_def values('6673','0','6673','#$');
insert into tx_sub_def values ('6673','�Ŵ���������','sp6673','0','0');

*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cms_rollback_c.h"
#include "mo_bank_acc_po_c.h"

int sp6673()
{
	int	iRet = 0;
	int	iCount = 0;
	double	dSumPoAmt=0.00;
	char	cCms_no[30];	/** �Ŵ���Ȩ��� **/
	

	struct cms_rollback_c sCms_rollback;
	struct mo_bank_acc_po_c sMo_bank_acc_po;
	
	memset(cCms_no,0x00,sizeof(cCms_no));
	memset(&sCms_rollback,0x00,sizeof( sCms_rollback));
	memset(&sMo_bank_acc_po,0x00,sizeof( sMo_bank_acc_po));
	
	pub_base_sysinit();
	get_zd_data("0580",cCms_no);
	pub_base_strpack(cCms_no);
	vtcp_log("[%s][%d]�����Ŵ���������.......�������Ϊ[%s]",__FILE__,__LINE__,cCms_no);
	iRet = Cms_rollback_Dec_Upd( g_pub_tx.reply, " cms_sq_no = '%s' ", cCms_no );
	if(iRet)
	{
		vtcp_log("[%s][%d]�����α����!!!",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "D101" );
		set_zd_data(DC_GET_MSG,"�����α����!!!");
		goto ErrExit;
	}
	while(1)
	{
		memset(&sCms_rollback,0x00,sizeof( sCms_rollback));
		iRet = Cms_rollback_Fet_Upd(&sCms_rollback, g_pub_tx.reply );
		if( iRet==100 )
		{
			if(iCount)
			{
				break;
			}else
			{
				vtcp_log("[%s][%d]δ�ҵ�Ҫ�����ļ�¼.����",__FILE__,__LINE__);
				strcpy( g_pub_tx.reply, "XD00" );
				set_zd_data(DC_GET_MSG,"û���ҵ�������¼!!!");
				goto ErrExit;
			}
		}else if(iRet)
		{
			vtcp_log("[%s][%d]δ�ҵ�Ҫ�����ļ�¼.����",__FILE__,__LINE__);
			strcpy( g_pub_tx.reply, "AT06" );
			set_zd_data(DC_GET_MSG,"���Ҽ�¼ʧ��!!!");
			goto ErrExit;
		}
		if(sCms_rollback.sts[0] == '1')
		{
			vtcp_log("[%s][%d]����Ȩ���ѳ���",__FILE__,__LINE__);
			strcpy( g_pub_tx.reply, "AT06" );
			set_zd_data(DC_GET_MSG,"����Ȩ���ѳ���!!!");
			goto ErrExit;
		}
		iRet = sql_count(sCms_rollback.beg_table,"%s",sCms_rollback.beg_sql);
		if(iRet > 1)
		{
			vtcp_log("[%s][%d]��������Ϊ[%d],����ʧ��,����ԭ��!!!",__FILE__,__LINE__,iRet);
			strcpy( g_pub_tx.reply, "H007" );
			set_zd_data(DC_GET_MSG,"����ʧ��!!!");
			goto ErrExit;
		}
		if(sCms_rollback.do_type[0] == 'I')	/** �Ŵ�ԭʼΪ�������,����Ϊɾ������  **/
		{
			vtcp_log("[%s][%d]Ҫִ�еĳ������Ϊ[Delete * from %s where %s]",__FILE__,__LINE__,sCms_rollback.beg_table , sCms_rollback.beg_sql);
			iRet = sql_execute2("Delete from %s where %s ",sCms_rollback.beg_table , sCms_rollback.beg_sql);
			if(iRet)
			{
				vtcp_log("[%s][%d]ɾ�����¼ʧ��",__FILE__,__LINE__);
				strcpy( g_pub_tx.reply, "H007" );
				set_zd_data(DC_GET_MSG,"ɾ�����¼ʧ��");
				goto ErrExit;
			}
		}else if(sCms_rollback.do_type[0] == 'U')	/** �Ŵ�ԭʼΪ���²���,����Ϊ���²���  **/
		{
			vtcp_log("[%s][%d]Ҫִ�еĳ������Ϊ[Update %s set %s where %s ]",__FILE__,__LINE__,sCms_rollback.beg_table , sCms_rollback.beg_sql);
			iRet = sql_execute2("Update %s set %s where %s ",sCms_rollback.beg_table, sCms_rollback.rolbk_sql,sCms_rollback.beg_sql);
			if(iRet)
			{
				vtcp_log("[%s][%d]���±��¼ʧ��",__FILE__,__LINE__);
				strcpy( g_pub_tx.reply, "D105" );
				set_zd_data(DC_GET_MSG,"���±��¼ʧ��");
				goto ErrExit;
			}
			vtcp_log("[%s][%d]beg_table=[%s]",__FILE__,__LINE__,sCms_rollback.beg_table);
			/** ����жһ�Ʊ��������Ĳ���ͬ��״̬��Э��������� ***/
			if(memcmp(sCms_rollback.beg_table,"mo_bank_acc_po",14)==0)
			{
				char cTemp[61];
				memset(cTemp,0x00,sizeof(cTemp));
				memcpy(cTemp,sCms_rollback.beg_sql,27);
				iRet = sql_sum_double( "mo_bank_po","po_amt", &dSumPoAmt, "%s and po_sts='1'",cTemp);
				if( iRet )
				{
					sprintf( acErrMsg, "ִ��sql_sum_double����![%d]", iRet );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"ִ�����ݿ��ѯʧ�ܣ�");
					goto ErrExit;
				}
				iRet = Mo_bank_acc_po_Sel(g_pub_tx.reply,&sMo_bank_acc_po,"%s and acc_po_sts in ('A','B','0','1','2')",sCms_rollback.beg_sql);
				if( iRet )
				{
					sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Sel����![%d]", iRet );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"ִ�����ݿ��ѯʧ�ܣ�");
					goto ErrExit;
				}
				if(pub_base_CompDblVal(sMo_bank_acc_po.sign_amt,dSumPoAmt) ==0 )
				{
					iRet = sql_execute2("Update mo_bank_acc_po set acc_po_sts='2' where %s and acc_po_sts in ('A','B','0','1')",sCms_rollback.beg_sql);
					if(iRet)
					{
						vtcp_log("[%s][%d]���±��¼ʧ��",__FILE__,__LINE__);
						strcpy( g_pub_tx.reply, "D105" );
						set_zd_data(DC_GET_MSG,"���±��¼ʧ��");
						goto ErrExit;
					}
				}
			}
		}
		sCms_rollback.sts[0] = '2';/** ��Ϊ�ѳ��� **/
		iRet = Cms_rollback_Upd_Upd(sCms_rollback,g_pub_tx.reply);
		if(iRet)
		{
			vtcp_log("[%s][%d]����ʧ��[%d]!!!",__FILE__,__LINE__,iRet);
			strcpy( g_pub_tx.reply, "H007" );
			set_zd_data(DC_GET_MSG,"���³����ǼǱ�ʧ��!!!");
			goto ErrExit;
		}
		iCount ++;
	}
	Cms_rollback_Clo_Upd();
	vtcp_log("[%s][%d]��������ɹ�.....",__FILE__,__LINE__);

OkExit:
	sprintf( acErrMsg, "�����ɹ�!!!" );
	WRITEMSG
	set_zd_data("0670","0");	/**�����ɹ� **/
	strcpy( g_pub_tx.reply, "0000" );
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4))
		memcpy(g_pub_tx.reply,"H034",4);
	sprintf( acErrMsg, "����ʧ��reply[%s]", g_pub_tx.reply );
	WRITEMSG
	set_zd_data("0670","1");	/**����ʧ�� **/
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
