#include "find_debug.h" 
/*************************************************
* �� �� ��:    pubf_comm_net.c
* ����������
*              tx_chk     ͨѶ��麯��
*
* ��    ��:    jack
* ������ڣ�   2003��12��16��
* �޸ļ�¼��
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2.
*************************************************/
# include <stdio.h>
#define EXTERN
# include "public.h"
# include "svrpub_c.h"
# include "com_tel_c.h"
# include "com_branch_c.h"
# include "tx_def_c.h"

  char cl_addr[16];

/********************************************/
int tx_chk()
{
    struct  com_tel_c   com_tel;
    struct  tx_def_c   tx_def;
    struct  com_branch_c    com_branch;
    char    auth_tel[5];
    char    tx_code[5];
    char    br_no[6];
    char    tel[5];
    char    sTerm[5];
	MEMSET_DEBUG(tel, 0, sizeof(tel));
	MEMSET_DEBUG(auth_tel, 0, sizeof(auth_tel));
	MEMSET_DEBUG(tx_code, 0, sizeof(tx_code));
	MEMSET_DEBUG(br_no, 0, sizeof(br_no));
    get_zd_data(DC_TEL,tel);
    get_zd_data(DC_CHK,auth_tel);
    get_zd_data(DC_TX_CODE,tx_code);
    get_zd_data(DC_TX_BR_NO,br_no);

	/****Modified by SSH,2006.6.12,�����9998,���������͹�Ա��ϵ
	**            ��Ϊ������׾���Ҫ�õ�������Ϣ              ***/
	if(memcmp(tx_code,"9998",4)==0){
		return(0);
	}
	vtcp_log("[%s][%d]����tx_def�ļ����\n",__FILE__,__LINE__);
/** chenggongxun 200901007 ������ʱע�� begin
    if(memcmp(tx_code,"6202",4)==0)
    {
        vtcp_log("tx_code===[%s]",tx_code);
        vtcp_log("tel===[%s]",tel);
        vtcp_log("auth_tel===[%s]",auth_tel);
        vtcp_log("br_no===[%s]",br_no);
		MEMCPY_DEBUG(tel,"5004",sizeof(tel)-1);
		MEMCPY_DEBUG(auth_tel,"5001",sizeof(auth_tel)-1);
		MEMCPY_DEBUG(br_no, QS_BR_NO,sizeof(br_no)-1);
	}
 chenggongxun 200901007 ������ʱע�� end **/

    RespCode=Com_tel_Sel(RtCode,&com_tel,"br_no='%s' and tel='%s'",br_no,tel);
    if( RespCode )
    {
		sprintf(acErrMsg,"FIND COM_TEL brno[%s]tel[%s]error[%d]",
			br_no,tel,RespCode);
		WRITEMSG
        strcpy(RtCode,"S009");
		/**ADD BY ZHGF �����µĽ������������ݿⷵ�ز���1403�Ļ���tx_chk����һ������ֵ9��cnvtin.c���ж�������ݿⱨ��������Ļ�ִ��exit(1)ɱ���÷���*/
		if(RespCode!=100){
			return 9;
		}
        return -1;
    }

    if(com_tel.csts[0]!='0')
    {
		/* ����Ա����ʱ,�Ŵ�����ֱ������ BY WYW20140530*/
		memset(sTerm,0x00,sizeof(sTerm));
		get_zd_data(DC_TTY,sTerm);    	
        if(com_tel.csts[0]=='4')
        {
			TRACE
            strcpy(RtCode,"S011");
            return -1;
        }
        if(com_tel.csts[0]=='3')
        {
        	/* 20140603 BY WYW */
        	if(memcmp(sTerm,"XDXT",4))
        	{
        		vtcp_log("%s,%d,�Ŵ�ҵ������״̬�¿�ͨ��!",__FILE__,__LINE__);
        		TRACE
        		strcpy(RtCode,"S012");
        		return -1;
        	}
        }
		if(Tx_def_Sel(RtCode,&tx_def,"tx_code='%s' AND wrk_sts!='2'",tx_code))
        {
            if(com_tel.csts[0]=='2'||com_tel.csts[0]=='1')
            {
            	/* 20140603 BY WYW �Ŵ����׷�ǩ��״̬Ҳ�������� */
        		if(memcmp(sTerm,"XDXT",4))
        		{
        			vtcp_log("%s,%d,�Ŵ�ҵ������״̬�¿�ͨ��!",__FILE__,__LINE__);
        			strcpy(RtCode,"S013");
        			return -1;
        		}
            }
            else
            {
				TRACE
                strcpy(RtCode,"S014");
                return -1;
            }
        }
        else
        {
            if(com_tel.csts[0]!='2'&&com_tel.csts[0]!='1')
            {
				TRACE
                strcpy(RtCode,"S014");
                return -1;
            }
        }
    }
	else{
		if(memcmp(tx_code,"5504",4)==0)		/* ��Ա���ӱ�����ǩ��״̬���� jinzhong*/
        {
				vtcp_log("[%s][%d]��Ա���ӽ��ױ�����ǩ��״̬����\n",__FILE__,__LINE__);
                strcpy(RtCode,"P089");
                return -1;
		}
    }
    if(strncmp(tel,auth_tel,4)&&auth_tel[0]!='\0'&&strcmp(auth_tel,""))
    {
        RespCode=Com_tel_Sel(RtCode,&com_tel,
			"br_no='%s' and tel='%s'",br_no,auth_tel );
        if( RespCode )
        {
				TRACE
            strcpy(RtCode,"S010");
            return -1;
        }
        if(com_tel.csts[0]!='0')
        {
            if(com_tel.csts[0]=='4')
            {
				TRACE
                strcpy(RtCode,"S015");
                return -1;
            }
            else if(com_tel.csts[0]=='3')
            {
				TRACE
                strcpy(RtCode,"S016");
                return -1;
            }
            else if(com_tel.csts[0]=='2'||com_tel.csts[0]=='1')
			{
				TRACE
                strcpy(RtCode,"S017");
                return -1;
			}
			else
            {
				TRACE
                strcpy(RtCode,"S018");
                return -1;
            }
        }
    }
    RespCode = Com_branch_Sel(RtCode,&com_branch," br_no='%s' ",br_no );
    if( RespCode )
    {
        sprintf(acErrMsg,"ϵͳ���������! br_no=[%s] ",br_no);
        WRITEMSG
        strcpy(RtCode,"S019");
        return -1;
    }
vtcp_log("br[%s]code[%s]",br_no,tx_code);
	if(Tx_def_Sel(RtCode,&tx_def,"tx_code='%s' AND wrk_sts='0'",tx_code))
    {
        if(com_branch.wrk_sts[0]!='1')
		{
				TRACE
		vtcp_log("[%s][%d]�����Ѿ�ǩ�˲�����ҵ��!\n",__FILE__,__LINE__);
            strcpy(RtCode,"S020");
            return -1;
        }
    }
    if(com_branch.br_type[0]=='*')
	{
        TRACE;
        strcpy(RtCode,"S021");
        return -1;
    }
    pub_base_strpack(com_branch.ipaddr);
    pub_base_strpack(cl_addr);
    if(strncmp(com_branch.ipaddr,cl_addr,strlen(com_branch.ipaddr))
		&& strncmp(com_branch.ipaddr,"#",1) )
    {
        TRACE;
        sprintf(acErrMsg," [%s]",cl_addr);
        WRITEMSG
        strcpy(RtCode,"S022");
        return -1;
    }

    return 0;
}
