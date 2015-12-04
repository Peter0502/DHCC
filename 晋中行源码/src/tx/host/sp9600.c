/*************************************************
* �� �� ��: sp9600.c
* ��������������ƽ̨�Ĺ�Ա��¼
*
* ��    ��: rob
* ������ڣ�20030314
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "com_tel_c.h"

int sp9600( )
{
    int ret;
    struct com_tel_c com_tel_c,com_tel;

    memset(&com_tel_c,0x0,sizeof(struct com_tel_c));
    memset(&com_tel,0x0,sizeof(struct com_tel_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }
    
    get_zd_data("0920",com_tel_c.tel);
    get_zd_data("0790",com_tel_c.pwd);

    pub_base_strpack(com_tel_c.tel);
    pub_base_strpack(com_tel_c.pwd);

    /* ����Ա���Ƿ�Ψһ */
    ret = Com_tel_Sel(g_pub_tx.reply,&com_tel,"tel='%s'",com_tel_c.tel);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"��ѯ��Ա��Ϣ�����!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"D103");
        goto ErrExit;
    }
    else if (ret == 100)
    {
        sprintf(acErrMsg,"�ù�Ա�Ų�����!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O120");
        goto ErrExit;
    }
	
    /* ���ù�Ա�Ƿ�ǩ��??? */
    /* ������� */
    ret = pub_base_DesChk(g_pub_tx.tx_date,"",com_tel_c.pwd,com_tel.pwd);
	/*vtcp_log("JJJ com_tel_c.pwd=[%d][%s]",strlen(com_tel_c.pwd),com_tel_c.pwd);*/
    if (ret != 0)
    {
        sprintf(acErrMsg,"�������!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O123");
        goto ErrExit;
    }
       
    strcpy(g_pub_tx.ac_no,com_tel_c.tel);
    strcpy(g_pub_tx.brf,"����ƽ̨��Ա��¼");

	/****
    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!!");
        WRITEMSG
        goto ErrExit;
    }
	****/

       
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"����ƽ̨��Ա��¼�ɹ� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"����ƽ̨��Ա��¼ʧ�� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
