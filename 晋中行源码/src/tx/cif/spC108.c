/*************************************************
* �� �� ��: spC108.c
* �����������ͻ�������Ϣ¼��
*
* ��    ��: andy
* ������ڣ�2004��01��17��
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

static struct cif_basic_inf_c	cif_basic_inf;
static struct cif_lifsty_inf_c	cif_lifsty_inf;
static struct cif_lifsty_inf_c	g_cif_lifsty_inf;

spC108()
{
	int ret=0;

	/** ��ʼ��ȫ�ֱ��� **/
    pub_base_sysinit();

    /** ���ݳ�ʼ�� **/
    memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	memset (&cif_lifsty_inf,0x00,sizeof(struct cif_lifsty_inf_c));
	memset (&g_cif_lifsty_inf,0x00,sizeof(struct cif_lifsty_inf_c));

    /** ȡֵ����ֵ **/
	get_zd_long("0280",&cif_lifsty_inf.cif_no);		/*�ͻ�ID*/
	get_zd_data("0670",cif_lifsty_inf.life_type);	/*�ͻ���ʽ���*/
	get_zd_data("0220",cif_lifsty_inf.life_detail); /*�ͻ����ʽ��ϸ*/

	/** �������� **/
	/* ���ͻ����Ƿ��Ѿ����� */
	ret=Cif_basic_inf_Sel( g_pub_tx.reply,&cif_basic_inf,
						"cif_no=%ld",cif_lifsty_inf.cif_no);
	if( ret==100)
	{
		sprintf(acErrMsg,"�޴˿ͻ��ţ�����!![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
		ERR_DEAL;
	}
	else if( ret )
	{
	 	sprintf(acErrMsg,"��ȡ�ͻ�������Ϣ�쳣!![%s]",g_pub_tx.reply);
        WRITEMSG
        strcpy(g_pub_tx.reply,"C006");
        ERR_DEAL;
    }

	/*�����������ʽ�����е����ʽ��ϸ�Ƿ��ظ�*/
	ret=Cif_lifsty_inf_Dec_Sel(g_pub_tx.reply,"cif_no=%ld order by lifsty_no", \
							   cif_lifsty_inf.cif_no );
	if( ret )
	{
		sprintf(acErrMsg,"DECLARE Cif_lifsty_inf_Dec_Sel�쳣![%s]",
				g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C039");
		ERR_DEAL;
	}

	while(1)
	{
		ret =Cif_lifsty_inf_Fet_Sel(&g_cif_lifsty_inf,g_pub_tx.reply);
		if( ret==100 ) 
		{
			cif_lifsty_inf.lifsty_no=g_cif_lifsty_inf.lifsty_no+1;
			break;
		}	
		else if( ret )
		{
			sprintf(acErrMsg,"FETCH Cif_email_inf_Dec_Sel�α��쳣![%s]",
					g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C038");
			ERR_DEAL;
		}

		pub_base_strpack(g_cif_lifsty_inf.life_type);
		pub_base_strpack(cif_lifsty_inf.life_type);
		pub_base_strpack(g_cif_lifsty_inf.life_detail);
		pub_base_strpack(cif_lifsty_inf.life_detail);

		if (!strcmp(g_cif_lifsty_inf.life_type,cif_lifsty_inf.life_type)&&!strcmp(g_cif_lifsty_inf.life_detail,cif_lifsty_inf.life_detail))
		{
			sprintf(acErrMsg,"�ÿͻ��Ѵ����������ʽ!![%s]");
			WRITEMSG
			strcpy(g_pub_tx.reply,"C040");
			ERR_DEAL;
		}
	}

	Cif_lifsty_inf_Clo_Sel ();

	/* �Ǽǿͻ����ʽ��Ϣ�� */
	ret=Cif_lifsty_inf_Ins(cif_lifsty_inf);
	if( ret )
	{
		sprintf(acErrMsg,"�Ǽǿͻ����ʽ��ʧ��![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C022");
		goto ErrExit;
	}

	/*�Ǽǽ�����ˮ*/
 	if( pub_ins_trace_log() )
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
        WRITEMSG
        goto ErrExit;
    }

	set_zd_long("0350",cif_lifsty_inf.lifsty_no);

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ͻ����ʽ��Ϣ¼��ɹ���[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
    sprintf(acErrMsg,"�ͻ����ʽ��Ϣ¼��ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}


