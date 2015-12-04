/*************************************************
* �� �� ��:  sp9819.c
* ���������� ������ʾ�˺Ų�ѯ���������
*
* ��    ��:  rob
* ������ڣ� 2003��1��17�� 
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"

int sp9819()
{
    struct in_mst_c sIn_mst_c;
    struct mdm_ac_rel_c sMdm_ac_rel_c;
    char ac_no[24];
    int flag;
    int ret;
   
    memset(&sIn_mst_c,0x0,sizeof(struct in_mst_c));
    memset(&sMdm_ac_rel_c,0x0,sizeof(struct mdm_ac_rel_c));
    memset(ac_no,0x0,sizeof(ac_no));

    get_zd_data("0310",ac_no);
    if (strcmp(ac_no,"") != 0)
    {
        strcpy(sMdm_ac_rel_c.ac_no,ac_no);
        sprintf(acErrMsg,"ac_no=[%s] [%s]!! ",sMdm_ac_rel_c.ac_no,ac_no);
        WRITEMSG
        flag = 1;
    }
    
    memset(ac_no,0x0,sizeof(ac_no));
    get_zd_data("0300",ac_no);
    if (strcmp(ac_no,"") != 0)
    {
        strcpy(sMdm_ac_rel_c.ac_no,ac_no);
        sprintf(acErrMsg,"ac_no=[%s] [%s]!! ",sMdm_ac_rel_c.ac_no,ac_no);
        WRITEMSG
        flag = 2;
    }

    ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel_c,"ac_no = '%s' and \
          ac_seqn= 0 ",sMdm_ac_rel_c.ac_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�Է��˺Ŵ���!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "B004");
        goto ErrExit;
    }

    ret = In_mst_Sel(g_pub_tx.reply,&sIn_mst_c,"ac_id =%ld and ac_seqn = 0",
          sMdm_ac_rel_c.ac_id);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�Է���������!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "B005");
        goto ErrExit;
    }
    
   
    if (flag == 1)
    {
	    set_zd_data( "0250", sIn_mst_c.name); /*��軧��*/
	    set_zd_double( "0410", sIn_mst_c.bal); /*�˻����*/
    }
    if (flag == 2)
    {
	    set_zd_data( "0270", sIn_mst_c.name); /*�Է�����*/
	    set_zd_double( "0420", sIn_mst_c.bal); /*�˻����*/
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
