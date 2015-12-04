/*************************************************
* �� �� ��:  sp5924.c
* ���������� ��Ʊ��ӡ
* ��    ��:  ligl
* ������ڣ� 2006��10��18��
* �޸ļ�¼�� 
*   ��    ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define  EXTERN
#include <string.h>
#include "public.h"
#include "hv_pub.h"
#include "hv_define.h"
#include "cif_basic_inf_c.h"
#include "prdt_ac_id_c.h"
#include "trace_log_c.h"
#include "trace_log_bk_c.h"
sp5924()
{
	int ret=0,i=0;
	char cBr_no[6];  											/*������*/
	long lTx_date=0;											/*��������*/
	char cTel[5];
	long lNexttrace_no=0;
	char cSe_type[2];/*����״̬*/
	char cTx_code[5];/*��Ʊ����*/
	char cPrtflag[2];
	char cAc_no1[24];
	char cAc_no2[24];
	char wherelist[1024];  /**��ѯ����**/
	struct prdt_ac_id_c prdt_ac_id1;
	struct cif_basic_inf_c cif_basic_inf1;
	struct mdm_ac_rel_c mdm_ac_rel1;
	struct prdt_ac_id_c prdt_ac_id2;
	struct cif_basic_inf_c cif_basic_inf2;
	struct mdm_ac_rel_c mdm_ac_rel2;
	struct trace_log_c Trace_log;
	struct trace_log_bk_c Trace_log_bk;
	memset(&Trace_log,0x0,sizeof(struct trace_log_c));
	memset(&Trace_log_bk,0x0,sizeof(struct trace_log_bk_c));
	memset(cBr_no     ,0, sizeof(cBr_no));
	memset(cTx_code   ,0, sizeof(cTx_code));
  memset(cSe_type,0,sizeof(cSe_type));
  memset(cPrtflag,0,sizeof(cPrtflag));
  memset(cAc_no1,0,sizeof(cAc_no1));
  memset(cAc_no2,0,sizeof(cAc_no2));
  memset(&prdt_ac_id1,0x0,sizeof(prdt_ac_id1));
  memset(&cif_basic_inf1,0x0,sizeof(cif_basic_inf1));
  memset(&mdm_ac_rel1,0x0,sizeof(mdm_ac_rel1));
   memset(&prdt_ac_id2,0x0,sizeof(prdt_ac_id2));
  memset(&cif_basic_inf2,0x0,sizeof(cif_basic_inf2));
  memset(&mdm_ac_rel2,0x0,sizeof(mdm_ac_rel2));
	vtcp_log("sp5924��ʼ��[%s][%d]",__FILE__,__LINE__);
	
	/**��ʼ����**/
	pub_base_sysinit();
	get_zd_data("0030",cBr_no);
	get_zd_data("0670",cPrtflag);
	get_zd_long("0440",&lTx_date);
	get_zd_long("0530",&lNexttrace_no);
	get_zd_data("0490",cTx_code);
	get_zd_data("0680",cSe_type);
	get_zd_data("0300",cAc_no1);
	get_zd_data("0310",cAc_no2);
	pub_base_old_acno(cAc_no1);
	pub_base_old_acno(cAc_no2);
	vtcp_log("%ld",g_pub_tx.tx_date);
if(lTx_date==g_pub_tx.tx_date)
{
	vtcp_log("AP MSG: ��ʼ�������ƾ֤��ӡ����![%s][%d]",__FILE__,__LINE__);
	vtcp_log("AP MSG: �õ�Ҫ��ӡ�ļ�¼![%s][%d]",__FILE__,__LINE__);
			ret = Trace_log_Sel(g_pub_tx.reply,&Trace_log, " trace_no=%ld and tx_date=%ld and opn_br_no='%s' and tx_br_no!='%s' and no_show!='1' ",lNexttrace_no,lTx_date,cBr_no,cBr_no);
			vtcp_log("[%s][%d]",__FILE__,__LINE__);
			if ( ret==100 )
			{
					sprintf(acErrMsg,"�޴˽���");
					WRITEMSG
					goto	ErrExit;
			}
			else if ( ret )
			{
					sprintf(acErrMsg,"��ѯ���׻�����Ϣ����[%d]",ret);
					WRITEMSG
					goto ErrExit;
			}		
			vtcp_log("��ѯ�ɹ���  [%s][%d]",__FILE__,__LINE__);
			/*���´��֧������*/
			vtcp_log("���´��֧������[%s][%d]",__FILE__,__LINE__);	
			ret = sql_execute("update trace_log set no_show= '2' where trace_no=%ld and tx_date=%ld and opn_br_no='%s' and tx_br_no!='%s'",lNexttrace_no,lTx_date,cBr_no,cBr_no);
			vtcp_log("[%s][%d]vvvvvvvvvvvvvvvvvvvvvv\n",__FILE__,__LINE__);
			if (ret)
			{
				sprintf(acErrMsg,"���´��֧�����׻�����Ϣ����[%d]",ret);
				WRITEMSG
				goto ErrExit;
			}		
}
else
{
			vtcp_log("AP MSG: ��ʼ�������ƾ֤��ӡ����![%s][%d]",__FILE__,__LINE__);
			vtcp_log("AP MSG: �õ�Ҫ��ӡ�ļ�¼![%s][%d]",__FILE__,__LINE__);
			ret = Trace_log_rz_Sel(g_pub_tx.reply,&Trace_log_bk, " trace_no=%ld and tx_date=%ld and opn_br_no='%s' and tx_br_no!='%s' and no_show!='1' ",lNexttrace_no,lTx_date,cBr_no,cBr_no);
			vtcp_log("[%s][%d]",__FILE__,__LINE__);
			if ( ret==100 )
			{
					sprintf(acErrMsg,"�޴˽���");
					WRITEMSG
					goto	ErrExit;
			}
			else if ( ret )
			{
					sprintf(acErrMsg,"��ѯ���׻�����Ϣ����[%d]",ret);
					WRITEMSG
					goto ErrExit;
			}		
			vtcp_log("��ѯ�ɹ���  [%s][%d]",__FILE__,__LINE__);
			/*���´��֧������*/
			vtcp_log("���´��֧������[%s][%d]",__FILE__,__LINE__);	
			ret = sql_execute("update trace_log_rz set no_show= '2' where trace_no=%ld and tx_date=%ld and opn_br_no='%s' and tx_br_no!='%s'",lNexttrace_no,lTx_date,cBr_no,cBr_no);
			vtcp_log("[%s][%d]vvvvvvvvvvvvvvvvvvvvvv\n",__FILE__,__LINE__);
			if (ret)
			{
				sprintf(acErrMsg,"���´��֧�����׻�����Ϣ����[%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
}
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
