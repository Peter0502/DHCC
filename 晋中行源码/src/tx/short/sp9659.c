/*************************************************
* �� �� ��:  sp9659.c
* ���������� ��֤���֤����У��λ
*
* ��    ��:  ZHGF 
* ������ڣ� 2008��2��22��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
#include <string.h>
#define EXTERN
#include "public.h"
#define ERR_DEAL if(ret){\
              sprintf(acErrMsg,"[%s],[%d],sqlcode=[%d]",__FILE__,__LINE__,ret);\
              WRITEMSG \
              goto ErrExit;\
             }
int sp9659()
{
	int             ret;
	char            id_no[20];
	char						cOutBuf[20];
	/* ��ʼ�� */
	memset(id_no, 0x00, sizeof(id_no));
	memset(cOutBuf,0x00,sizeof(cOutBuf));
	/* ǰ̨ȡֵ */
	get_zd_data("0620", id_no);	/* ֤������ */
	vtcp_log("[%s][%d]��������֤Ϊ[%s]\n",__FILE__,__LINE__,id_no);
	ret = iPub_base_SFZYZ(id_no, cOutBuf);
        if(ret == -1)
        {
	        sprintf(acErrMsg,"���֤У��ʧ��",g_pub_tx.reply);
                WRITEMSG
                strcpy(g_pub_tx.reply,"CD02");
                vtcp_log("%s,%d ���֤У��ʧ��,У��λΪ[%d]",__FILE__,__LINE__,ret);
                goto ErrExit;
          }
	vtcp_log("%s,%d,cOutBuf[%s]",__FILE__,__LINE__,cOutBuf);
	set_zd_data("0620",cOutBuf);
OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
