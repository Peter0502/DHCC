/*************************************************
* �� �� ��:  sp9867.c
* ���������� �����ڲ��˺Ų�ѯ����
*
* ��    ��:  jane
* ������ڣ� 2003��3��9�� 
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"

int sp9867()
{
	struct	in_mst_c	sIn_mst_c;		/* �ڲ�̨�����ļ� */
    	struct 	mdm_ac_rel_c 	sMdm_ac_rel_c;		/* �������˻���ϵ�� */
    	int ret;

    	memset(&sIn_mst_c,0x00,sizeof(struct in_mst_c));
    	memset(&sMdm_ac_rel_c,0x00,sizeof(struct mdm_ac_rel_c));

    	get_zd_data("0330",sMdm_ac_rel_c.ac_no);

    	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel_c,"ac_no = '%s' \
          			and ac_seqn= 0 ",sMdm_ac_rel_c.ac_no);
    	if( ret == 100 )
	{
		sprintf( acErrMsg, "�������˻���ϵ���в����ڴ��ʺ�!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B117" );
                goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel��![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

    	ret = In_mst_Sel(g_pub_tx.reply,&sIn_mst_c,"ac_id =%ld and ac_seqn = 0",\
          		sMdm_ac_rel_c.ac_id);
    	if( ret == 100 )
	{
		sprintf( acErrMsg, "�ڲ�̨�����ļ��в����ڴ��ʺ�!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "A029" );
                goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "ִ��In_mst_Sel��![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	set_zd_data( "0250", sIn_mst_c.name); /*����*/

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
