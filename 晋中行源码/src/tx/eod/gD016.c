/*************************************************************
* �� �� ��: gD016.c
* ����������������ƴ���
*
* ��    ��: jack
* �������: 2003��3��22��
*
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#define MYRETERR if(ret){ \
        sprintf(acErrMsg,"sql erro!! [%d]",ret); \
        WRITEMSG \
        return ret; \
	}

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "mo_prdt_prot_c.h"


gD016()
{
    int ret = 0;
    struct com_sys_parm_c com_sys_parm_c;
	struct mo_prdt_prot_c mo_prdt_prot_c;

    memset(&com_sys_parm_c, 0x00, sizeof(struct com_sys_parm_c));
	memset(&mo_prdt_prot_c, 0x00 ,sizeof(struct mo_prdt_prot_c));
	memset(&g_pub_tx, 0x00 ,sizeof(struct pub_tx));

    ret=sql_begin(); /*������*/
	MYRETERR

    /**------- ��ʼ��ȫ�ֱ��� --------**/
	memset ( &g_pub_tx , 0x00 , sizeof(struct pub_tx) );
   
    /* ��ѯϵͳ������ */
    ret = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm_c,"1=1");
	MYRETERR

    g_pub_tx.tx_date = com_sys_parm_c.sys_date;
	strcpy(g_pub_tx.cur_no,RMB);
	strcpy(g_pub_tx.tx_code,"Z016");
	strcpy(g_pub_tx.sub_tx_code,"Z016");
	g_pub_tx.trace_cnt=0;
	strcpy(g_pub_tx.ct_ind,"2");
	strcpy(g_pub_tx.brf,"Э������");
	strcpy(g_pub_tx.tel,"9999");

	if( pub_base_end_year(g_pub_tx.tx_date)==0 )	/** ��ĩ **/
	{
		ret = Mo_prdt_prot_Dec_Sel1(g_pub_tx.reply,
		"sts='1' and prot_type in (select prot_type from prdt_prot_parm where turn_term_type in ('1','2','3') and end_date>%d) order by tx_br_no,prot_type",
			g_pub_tx.tx_date );
	}
	else if( pub_base_end_month(g_pub_tx.tx_date)==0 )	/** ��ĩ **/
	{
		ret=Mo_prdt_prot_Dec_Sel1(g_pub_tx.reply,
		"sts='1' and prot_type in (select prot_type from prdt_prot_parm where turn_term_type in ('1','2') and end_date>%d ) order by tx_br_no,prot_type ",
			g_pub_tx.tx_date );
	}
	else
	{
		ret = Mo_prdt_prot_Dec_Sel1(g_pub_tx.reply,
		"sts='1' and prot_type in (select prot_type from prdt_prot_parm where substr(turn_term_type,1,1)='1' and end_date>%d) order by tx_br_no,prot_type ",
			g_pub_tx.tx_date );
	}
	MYRETERR

	while(1)
	{
		memset(&mo_prdt_prot_c, 0x00 ,sizeof(struct mo_prdt_prot_c));
		
		ret = Mo_prdt_prot_Fet_Sel1(&mo_prdt_prot_c,g_pub_tx.reply);
		if( ret==100 )	break;
		else if( ret )
		{
			sprintf(acErrMsg,"mo_prdt_prot_fet_select erro!! [%d]",ret);
			WRITEMSG
			return ret;
		}
		vtcp_log("[%s][%d] ���ʺ�id==[%ld] ",__FILE__,__LINE__,mo_prdt_prot_c.main_ac_id);

		strcpy(g_pub_tx.opn_br_no,mo_prdt_prot_c.tx_br_no);
		strcpy(g_pub_tx.tx_br_no,mo_prdt_prot_c.tx_br_no);
		pub_base_PubQlsh_Trans(&g_pub_tx.trace_no);
		if( g_pub_tx.trace_no<0 )
		{
			sprintf(acErrMsg,"ȡ������ˮ��!");
			WRITEMSG
			goto ErrExit;
		}
		vtcp_log("[%s][%d] ���ʺ�id==[%ld] ",__FILE__,__LINE__,mo_prdt_prot_c.main_ac_id);
		ret = pub_eod_prot(mo_prdt_prot_c);
		if (ret != 0)
		{
			sprintf(acErrMsg,"������ƺ�������!!");
			WRITEMSG
			goto ErrExit;
		}
		vtcp_log("[%s][%d] ���ʺ�id==[%ld] ",__FILE__,__LINE__,mo_prdt_prot_c.main_ac_id);
	}
	Mo_prdt_prot_Clo_Sel();

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"������ƴ���ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");
    }
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"������ƴ���ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
