/*************************************************
* �� �� ��:  sp9827.c
* ���������� ������ʾ�˺Ų�ѯ����
*
* ��    ��:  rob
* ������ڣ� 2003��1��22�� 
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"

int sp9827()
{
    struct in_mst_c sIn_mst_c;
    struct mdm_ac_rel_c sMdm_ac_rel_c;
    int ret;
   
    memset(&sIn_mst_c,0x0,sizeof(struct in_mst_c));
    memset(&sMdm_ac_rel_c,0x0,sizeof(struct mdm_ac_rel_c));

    get_zd_data("0310",sMdm_ac_rel_c.ac_no);

    sprintf(acErrMsg,"ac_no=[%s]!! ",sMdm_ac_rel_c.ac_no);
    WRITEMSG

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
    
   
    
    sprintf(acErrMsg,"name=[%s]!! ",sIn_mst_c.name);
    WRITEMSG
	set_zd_data( "0250", sIn_mst_c.name); /*�Է�����*/

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
