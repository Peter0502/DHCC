/*************************************************
* �� �� ��:    sp5666.c
* ��������:   
*              ȡ�ù���ͨ�����ʻ�
*
* ��    ��:    wanglei
* �������:    2010��06��29��
* ���׶���:
	insert into tx_def values ('5666','ȡ�ù���ͨ�����ʻ�','10000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
	insert into tx_flow_def values ('5666',0,'5666','#$');
	insert into tx_sub_def values ('5666','ȡ�ù���ͨ�����ʻ�','sp5666','0','0');
* �޸ļ�¼:    
*     1. ��    ��: 
*        �� �� ��: 
*        �޸�����: 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "com_parm_c.h"

	
sp5666()  
{ 		
	/** ���ݳ�ʼ�� **/
	struct com_parm_c      com_parm; 

	int  ret =0;	
	char cCode[4];
	char cSeqn[3];
	int  iSeqn=0;

	memset (&com_parm, 0x00, sizeof(struct com_parm_c));
	memset (cCode, 0x00, sizeof(cCode));
	memset (cSeqn, 0x00, sizeof(cSeqn));
	
	/** ȡֵ����ֵ **/	
	get_zd_data( "0210" , cSeqn );  /* ������� */
	get_zd_data( "0230" , cCode );  /* �������� */
	iSeqn = atoi(cSeqn);
	vtcp_log("[%s][%d] cSEQN=[%s]",__FILE__,__LINE__,cSeqn);
	vtcp_log("[%s][%d] iSEQN=[%d]",__FILE__,__LINE__,iSeqn);
	vtcp_log("[%s][%d] CODE=[%s]",__FILE__,__LINE__,cCode);
	
					  
	/* ���ݿͻ��Ų�ѯ�ͻ����� */			  
	ret = Com_parm_Sel( g_pub_tx.reply , &com_parm , " parm_code= '%s'  and parm_seqn = %d " , cCode,iSeqn);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�������������޴˼�¼![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"G001");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"ȡ�����������쳣![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O005");
		goto ErrExit;
	}

	set_zd_data("0370",com_parm.val);/**�ͻ�����**/

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
