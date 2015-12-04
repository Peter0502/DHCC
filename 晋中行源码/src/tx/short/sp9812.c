/*************************************************
* �� �� ��:  sp9812.c
* ���������� ������ʾ�˺Ų�ѯ����
*
* ��    ��:  
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
#include "com_item_c.h"

int sp9812()
{
    struct in_mst_c sIn_mst_c;
    struct dd_mst_c sDd_mst_c;
    struct mdm_ac_rel_c sMdm_ac_rel_c;
	struct com_item_c com_item_c;
    int ret;
   
    memset(&sIn_mst_c,0x0,sizeof(struct in_mst_c));
    memset(&sDd_mst_c,0x0,sizeof(struct dd_mst_c));
    memset(&sMdm_ac_rel_c,0x0,sizeof(struct mdm_ac_rel_c));
	memset(&com_item_c, 0x0, sizeof(struct com_item_c));

    get_zd_data("0300",sMdm_ac_rel_c.ac_no);

    sprintf(acErrMsg,"ac_no=[%s]!! ",sMdm_ac_rel_c.ac_no);
    WRITEMSG
	if (strlen(sMdm_ac_rel_c.ac_no) > 7)
	{
    	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel_c,"ac_no = '%s' ",sMdm_ac_rel_c.ac_no);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"��ѯ�Է��˺Ŵ���!! [%d]",ret);
        	WRITEMSG
        	strcpy(g_pub_tx.reply, "B004");
        	goto ErrExit;
    	}
		if( sMdm_ac_rel_c.ac_seqn=9999 )
			sMdm_ac_rel_c.ac_seqn=1;			/** ȡ����������ʺ���Ϣ ***/

		if( sMdm_ac_rel_c.ac_no[0]=='9' && strlen(sMdm_ac_rel_c.ac_no)!=19 )
		{
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
		set_zd_data( "0270", sIn_mst_c.name); /*�Է�����*/
		}
		else
		{
    	ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst_c,"ac_id =%ld and ac_seqn=%d",
          	sMdm_ac_rel_c.ac_id,sMdm_ac_rel_c.ac_seqn);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"��ѯ�Է���������!! [%d]",ret);
        	WRITEMSG
        	strcpy(g_pub_tx.reply, "B005");
        	goto ErrExit;
    	}
    	sprintf(acErrMsg,"name=[%s]!! ",sDd_mst_c.name);
    	WRITEMSG
		set_zd_data( "0270", sDd_mst_c.name); /*�Է�����*/
		}
    } 
	else
	{
		ret = Com_item_Sel(g_pub_tx.reply,&com_item_c,"acc_no='%s'",
				sMdm_ac_rel_c.ac_no);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg,"��ѯ��Ŀ�����!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"B172");
			goto ErrExit;
		}
		else if (ret == 100)
		{
			sprintf(acErrMsg,"�ÿ�Ŀ������!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"B173");
			goto ErrExit;
		}
    	sprintf(acErrMsg,"acc_name=[%s]!! ",com_item_c.acc_name);
    	WRITEMSG
		set_zd_data( "0270", com_item_c.acc_name); /*�Է�����*/
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
