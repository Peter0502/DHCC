/*************************************************
* �� �� ��: spC310.c
* ����������
		�ͻ�������Ϣά��
*
* ��    ��: 
* ������ڣ�2004��01��26��
* �޸ļ�¼��
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2.
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include "stdio.h"
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "cif_basic_inf_c.h"
#include "cif_lifsty_inf_c.h"

	static struct cif_basic_inf_c		cif_basic_inf;
	static struct cif_lifsty_inf_c		cif_lifsty_inf;
	static struct cif_lifsty_inf_c		g_cif_lifsty_inf;
	
	char old_alias[30],tbwhere[50],memo[41];
	char sts[2];

spC310()
{
	int ret=0;
	int cmp_code=0;

	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();

	/** ���ݳ�ʼ�� **/
	memset(&cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&cif_lifsty_inf,0x00,sizeof(struct cif_lifsty_inf_c));
	memset(&g_cif_lifsty_inf,0x00,sizeof(struct cif_lifsty_inf_c));

	/** ȡֵ����ֵ **/
	get_zd_data("0680",sts);								/*��������*/
	get_zd_long("0280",&g_cif_lifsty_inf.cif_no);			/*�ͻ���*/
	get_zd_long("0340",&g_cif_lifsty_inf.lifsty_no);			/*�������*/

	get_zd_data("0670",g_cif_lifsty_inf.life_type);			/*���ʽ����*/
	get_zd_data("0220",g_cif_lifsty_inf.life_detail);		/*���ʽ��ϸ*/

	/** �������� **/
	switch(sts[0])
	{
		case '1':										/*1-�޸�*/
				ret=Cif_lifsty_inf_Dec_Upd(g_pub_tx.reply,
				 	"cif_no=%ld and lifsty_no=%d",g_cif_lifsty_inf.cif_no,
					g_cif_lifsty_inf.lifsty_no);
			 	if (ret)
			 	{
					sprintf(acErrMsg,"DECLARE Cif_lifsty_inf_rel�α��쳣![%s]",
						   g_pub_tx.reply);
				  	WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				ret=Cif_lifsty_inf_Fet_Upd(&cif_lifsty_inf,g_pub_tx.reply);
				if (ret)
				{
				   	sprintf(acErrMsg,"FETCH Cif_lifsty_inf�α��쳣![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				
			memcpy(g_cif_lifsty_inf.rowid, cif_lifsty_inf.rowid,sizeof(g_cif_lifsty_inf.rowid));
			
				ret=Cif_lifsty_inf_Upd_Upd(g_cif_lifsty_inf,g_pub_tx.reply);
				if (ret)
				{
    				sprintf(acErrMsg,"UPDATE Cif_lifsty_inf�α��쳣![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				Cif_lifsty_inf_Clo_Upd( );
				break;
		case '2':										/*2-ɾ��*/
				/*ɾ���ͻ�������ʷ��Ϣ������*/
				sprintf(tbwhere,"cif_no=%ld and lifsty_no=%d",
						g_cif_lifsty_inf.cif_no,g_cif_lifsty_inf.lifsty_no);
				ret =deldb("cif_lifsty_inf",tbwhere);
				if( ret )
				{
					 sprintf(acErrMsg,"ɾ���ͻ�������Ϣ���¼�쳣!![%s]",
							ret);
					 WRITEMSG
					 strcpy(g_pub_tx.reply,"C053");
					 ERR_DEAL;
				}
				break;
		default:
				break;
	}

    /* �Ǽǽ�����ˮ */
    if( pub_ins_trace_log() )
    {
         sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
         WRITEMSG
         goto ErrExit;
    }

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�ͻ�������Ϣά���ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"�ͻ�������Ϣά��ʧ��![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

