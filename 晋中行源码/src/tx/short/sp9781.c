/*************************************************
* �� �� ��:  sp9708.c
* ���������� �ɿ��Ż��Ի���
*
* ��    ��:  rob 
* ������ڣ� 2003��2��21��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"

int sp9781()
{
	int	ret;
	char card_no[20],tmp_name[50];
	struct 	prdt_ac_id_c   g_prdt_ac_id;			/*��Ʒ�˻��ṹ*/

	/*���*/
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));

	/*ǰ̨ȡֵ*/
	get_zd_data("0370",card_no);       				/*����*/

   	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",card_no);
   	if( ret==100 )
   	{
        sprintf( acErrMsg,"�޴˿��ţ�����!card_no=[%s]", card_no );
        WRITEMSG
        strcpy( g_pub_tx.reply, "M003" );
        goto ErrExit;
   	}
    else if( ret )
   	{
        sprintf( acErrMsg, "��ȡ�����˻����ձ��쳣![%d]", ret );
        WRITEMSG
        strcpy( g_pub_tx.reply, "W011" );
        goto ErrExit;
    }

	/*�����˻�ID�ҳ��˻�����*/
   	ret=Prdt_ac_id_Sel(g_pub_tx.reply,&g_prdt_ac_id,
						"ac_id=%ld",g_mdm_ac_rel.ac_id);
   	if( ret==100 )
   	{
        sprintf( acErrMsg,"�޴˿��ţ�����!card_no=[%ld]", card_no );
        WRITEMSG
        strcpy( g_pub_tx.reply, "M003" );
        goto ErrExit;
   	}
    else if( ret )
   	{
        sprintf( acErrMsg, "��ȡ��Ʒ�˻����ձ��쳣![%d]", ret );
        WRITEMSG
        strcpy( g_pub_tx.reply, "W013" );
        goto ErrExit;
    }

	switch(g_prdt_ac_id.ac_id_type[0])
	{
		case '1':							/*1-����*/
   				ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,
								"ac_id=%ld",g_prdt_ac_id.ac_id);
   				if( ret==100 )
   				{
        			sprintf( acErrMsg,"���˻�ID������!card_no=[%ld]", 
							card_no );
        			WRITEMSG
        			strcpy( g_pub_tx.reply, "M003" );
        			goto ErrExit;
   				}
    			else if( ret )
   				{	
        			sprintf( acErrMsg, "��ȡ�������ļ��쳣![%d]", ret );
        			WRITEMSG
        			strcpy( g_pub_tx.reply, "W015" );
        			goto ErrExit;
    			}
				strcpy(tmp_name,g_dd_mst.name);
				break;
		case '2':							/*2-����*/
   				ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,
									"ac_id=%ld",g_prdt_ac_id.ac_id);
   				if( ret==100 )
   				{
        			sprintf( acErrMsg,"���˻�ID������!card_no=[%ld]", 
							card_no );
        			WRITEMSG
        			strcpy( g_pub_tx.reply, "M003" );
        			goto ErrExit;
   				}
    			else if( ret )
   				{	
        			sprintf( acErrMsg, "��ȡ�������ļ��쳣![%d]", ret );
        			WRITEMSG
        			strcpy( g_pub_tx.reply, "W017" );
        			goto ErrExit;
    			}
				strcpy(tmp_name,g_td_mst.name);
				break;
		case '3':							/*3-����*/
   				ret=Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,
									"ac_id=%ld",g_prdt_ac_id.ac_id);
   				if( ret==100 )
   				{
        			sprintf( acErrMsg,"���˻�ID������!card_no=[%ld]", 
							card_no );
        			WRITEMSG
        			strcpy( g_pub_tx.reply, "M003" );
        			goto ErrExit;
   				}
    			else if( ret )
   				{	
        			sprintf( acErrMsg, "��ȡ�������ļ��쳣![%d]", ret );
        			WRITEMSG
        			strcpy( g_pub_tx.reply, "W019" );
        			goto ErrExit;
    			}
				strcpy(tmp_name,g_ln_mst.name);
				break;
		default : 	
        		sprintf( acErrMsg, "�ÿ��쳣!![%d]", ret );
        		WRITEMSG
        		strcpy( g_pub_tx.reply, "O024" );
        		goto ErrExit;
				break;
	}

    /* ��ǰ̨��Ļ��ֵ */
	set_zd_data("0250",tmp_name);						/*����*/

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

