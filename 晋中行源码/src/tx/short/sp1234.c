/*************************************************
* �� �� ��: sp1234.c
* �������������������Ϣ���в������� agent_info
* ��    ��: wudawei
* ������ڣ�2015��03��20��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "agent_info_c.h"

sp1234()  
{ 		
	int ret=0, i=0;
	struct agent_info_c sAgent_info;
	char ac_no[20],id_no[21],id_type[2],phone[30],tmp[60];
	char fldstr[2][30];
	/*
			$FD27=#AGENTNAME;
			$FD68=#ATCETYPE;
			$FD63=#ATCENUM;			
			$FD83=#ATCEPHONE;
	*/
	memset(&sAgent_info,0x00,sizeof(struct agent_info_c));
	pub_base_sysinit();
	memset(ac_no,0x00,sizeof(ac_no));
	memset(id_no,0x00,sizeof(id_no));
	memset(id_type,0x00,sizeof(id_type));
	memset(phone,0x00,sizeof(phone));
	memset(fldstr,0x00,sizeof(fldstr));
	memset(tmp,0x00,sizeof(tmp));
	/** ȡֵ����ֵ **/	
	get_zd_data("0300",ac_no);						/*�ʺ�*/
	/**get_zd_data("0270",agent_name); **/					/*����������*/
	get_zd_data("0680",id_type);					/*֤������*/
	get_zd_data("0630",id_no);					    /*֤������*/
	get_zd_data("0830",tmp);					    /*�绰����*/
	
	for( i=0; i<2; i++ )
	{
			ret=pub_base_cut_str( tmp,fldstr[i],"|",i+1 );
			vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );
	}

	strcpy(sAgent_info.ac_no, ac_no);
	strcpy(sAgent_info.type, "1");    /*��IC�� ������*/
	strcpy(sAgent_info.agent_name, fldstr[1]); 
	strcpy(sAgent_info.id_type, id_type);
	strcpy(sAgent_info.id_no, id_no);
	strcpy(sAgent_info.phone, fldstr[0]);
	
	 vtcp_log("sAgent_info.ac_no=��%s��",sAgent_info.ac_no);
	 vtcp_log("sAgent_info.agent_name=��%s��",sAgent_info.agent_name);
	 vtcp_log("sAgent_info.id_type=��%s��",sAgent_info.id_type);
	 vtcp_log("sAgent_info.id_no=��%s��",sAgent_info.id_no);
	 vtcp_log("sAgent_info.phone=��%s��",sAgent_info.phone);

	strcpy(sAgent_info.br_no,  g_pub_tx.tx_br_no); 
	strcpy(sAgent_info.tel, g_pub_tx.tel);
	sAgent_info.tx_date = g_pub_tx.tx_date ;

	if(strlen(sAgent_info.agent_name)!=0){
		vtcp_log("��������˵Ǽǲ���..." );
	 ret = Agent_info_Ins( sAgent_info,   g_pub_tx.reply );
                        if( ret )
                        {
                                sprintf(acErrMsg,"�ǼǴ�������Ϣ�����![%d]",ret);
                                WRITEMSG
                                goto ErrExit;
                        }
     vtcp_log("�����˵Ǽǳɹ�..." );
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
   
