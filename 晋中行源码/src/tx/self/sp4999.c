/*************************************************
* �� �� ��:  sp4999.c
*����ģ�� :  ��ƽ̨�Ľӿڳ��� 
* ���������� ��Ա������
*
* ��    ��:  shaoliguang
* ������ڣ� 
* �������ã�������Ҫ����Ա����ĳ��ϣ�����ε�¼��һЩ���˽��׵���������Ա�������ڲ�����ˮ������¼�Ǽǲ���
* 
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
  insert into tx_def values('6231','ƽ̨��Ա������','10000000000000000000000010000000000000000000000000000000000000000011100000000000000000000001000000000000000000000000000000000100','0','6','0','3',null);
  insert into tx_flow_def  values ('6231', 0, '4999', '#$');
  insert into tx_sub_def  values ('4999', 'ƽ̨��Ա������', 'sp4999', '0', '0');
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "com_tel_c.h"

sp4999()
{
    int   iRet=0;
    char  cTel[7];  /*��Ա��*/
    char  cPsw[17]; /*��Ա����*/
    char  cBrf[51]; /*ժҪ*/
    
    memset( cTel, 0x00 ,sizeof( cTel ));
    memset( cPsw, 0x00 ,sizeof( cPsw ));
    memset( cBrf, 0x00 ,sizeof( cBrf ));
    
    struct com_tel_c sCom_tel;
    memset(&sCom_tel,0x00,sizeof(struct com_tel_c));
    
    /*��ʼ������*/
    pub_base_sysinit();
    vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
    set_zd_long("0460",g_pub_tx.tx_date);
    /*��ȡ��ƽ̨������ֵ*/
    get_zd_data("0070",cTel);
    get_zd_data("0790",cPsw);
    get_zd_data("0810",cBrf);
    
    /*vtcp_log("%s,%d ����=[%s]\n",__FILE__,__LINE__,cPsw);*/

    vtcp_log("%s,%d ��Ա��=[%s]\n",__FILE__,__LINE__,cTel);
    WRITEMSG

    iRet = Com_tel_Dec_Sel(g_pub_tx.reply, " tel='%s' ", cTel);
    if (iRet)
    {
       sprintf(acErrMsg, "��Ա������[%s]", cTel);
       WRITEMSG
       strcpy(g_pub_tx.reply, "O120");
       goto ErrExit;
    }
    iRet = Com_tel_Fet_Sel(&sCom_tel,g_pub_tx.reply);
    if (iRet)
    {
       sprintf(acErrMsg, "��Ա������[%s]", cTel);
       WRITEMSG
       strcpy(g_pub_tx.reply, "O120");
       goto ErrExit;
    }

    /*��Ա������ֿ�ֵ,�˴����� */
    if( !strcmp(pub_base_strpack(sCom_tel.pwd),'') )
    {
      sprintf(acErrMsg, "����Ϊ��,����Ƽ�����ϵ!![%s][%s]", cPsw,sCom_tel.pwd);
      WRITEMSG
      strcpy(g_pub_tx.reply, "O270");
           goto ErrExit;
    }
    /*���������麯��*/
    if (pub_base_DesChk(g_pub_tx.tx_date, " ",cPsw,sCom_tel.pwd)) 
    {
         strcpy(g_pub_tx.reply, "AT04");
         sprintf(acErrMsg, "���벻��ȷ!!");
         WRITEMSG
         goto ErrExit;
    }
    
    Com_tel_Clo_Sel();

OkExit:
     strcpy(g_pub_tx.reply, "0000");
     sprintf(acErrMsg, "������֤�ɹ�:[%s]", g_pub_tx.reply);
     WRITEMSG
          set_zd_data("0120", g_pub_tx.reply);
     return 0;
ErrExit:
     sprintf(acErrMsg, "������֤ʧ�ܣ�[%s]", g_pub_tx.reply);
     WRITEMSG
          set_zd_data("0120", g_pub_tx.reply);
     return 1;
}

