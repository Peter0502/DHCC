/*************************************************
* �� �� ��:  sp9866.c
* ���������� �����ڲ��˺Ų�ѯ����(���в���Ϊ�ֽ��ո�����)
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
#include "com_item_c.h"

int sp9866()
{
	struct	in_mst_c	sIn_mst_c;		/* �ڲ�̨�����ļ� */
    	struct 	mdm_ac_rel_c 	sMdm_ac_rel_c;		/* �������˻���ϵ�� */
    	struct 	com_item_c 	sCom_item_c;		/* ������Ŀ�� */
    	int ret;

    	memset(&sIn_mst_c,0x00,sizeof(struct in_mst_c));
    	memset(&sMdm_ac_rel_c,0x00,sizeof(struct mdm_ac_rel_c));
    	memset(&sCom_item_c,0x00,sizeof(struct com_item_c));

    	get_zd_data("0300",sMdm_ac_rel_c.ac_no);
sprintf(acErrMsg,"__xxxxxx111__ac_no=[%s]",sMdm_ac_rel_c.ac_no);
WRITEMSG    	
    	pub_base_old_acno( sMdm_ac_rel_c.ac_no );   		/* �Ծ��ʺŵĴ��� */
sprintf(acErrMsg,"__XXxxxx222__ac_no=[%s]",sMdm_ac_rel_c.ac_no);
WRITEMSG
/*	110��Ŀ���ڸ�Ϊ���˻���Ŀ�����Դ˶γ�����2003.7.31���� jane
	if( strcmp( sMdm_ac_rel_c.ac_no, "110" ) == 0 )		* �����ÿ�Ŀ��110���� *
	{
		ret = Com_item_Sel( g_pub_tx.reply, &sCom_item_c, "acc_no = '%s'","110" );
    		if( ret == 100 )
		{
			sprintf( acErrMsg, "������Ŀ���в����ڴ˿�Ŀ��!" );
                	WRITEMSG
                	strcpy( g_pub_tx.reply, "O074" );
               		goto ErrExit;
       		}else if( ret )
       		{
       			sprintf( acErrMsg, "ִ��Com_item_Sel��![%d]", ret );
                	WRITEMSG
                	goto ErrExit;
       		}

		set_zd_data( "0270", sCom_item_c.acc_name ); 	*����*
	}*/

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel_c,"ac_no = '%s'",sMdm_ac_rel_c.ac_no);
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

	set_zd_data( "0270", sIn_mst_c.name); /*����*/
	set_zd_data("0300",sMdm_ac_rel_c.ac_no);		/* ������ʺ� */
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
