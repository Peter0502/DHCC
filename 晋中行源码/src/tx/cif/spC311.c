/*************************************************
* �� �� ��: spC311.c
* ����������
		�ͻ��������ڲ�Ʒ��Ϣά��
*
* ��    ��: andy
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
#include "cif_oth_prdt_inf_c.h"

	static struct cif_basic_inf_c		cif_basic_inf;
	static struct cif_oth_prdt_inf_c		cif_oth_prdt_inf;
	static struct cif_oth_prdt_inf_c		g_cif_oth_prdt_inf;
	
	char old_alias[30],tbwhere[50],memo[41];
	char sts[2];

spC311()
{
	int ret=0;
	int cmp_code=0;
	double lbalance=0.0;
        double lmon_pay=0.0; 
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();

	/** ���ݳ�ʼ�� **/
	memset(&cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&cif_oth_prdt_inf,0x00,sizeof(struct cif_oth_prdt_inf_c));
	memset(&g_cif_oth_prdt_inf,0x00,sizeof(struct cif_oth_prdt_inf_c));

	/** ȡֵ����ֵ **/
	get_zd_data("0680",sts);								/*��������*/
	get_zd_long("0280",&g_cif_oth_prdt_inf.cif_no);			/*�ͻ���*/
	get_zd_long("0490",&g_cif_oth_prdt_inf.inf_no);			/*�������*/

	get_zd_data("0670",g_cif_oth_prdt_inf.unit_type);   	/*��������*/
	get_zd_data("0910",g_cif_oth_prdt_inf.unit_no);     	/*��������*/
	get_zd_data("0660",g_cif_oth_prdt_inf.prdt_code);   	/*��Ʒ����*/
	get_zd_data("0210",g_cif_oth_prdt_inf.pro_cur_no);  	/*��Ʒ����*/
	get_zd_data("0300",g_cif_oth_prdt_inf.ac_no);       	/*��Ʒ����*/
	get_zd_double("0390",&lbalance);       /*������*/
	get_zd_double("0400",&lmon_pay);       /*�»����*/
        vtcp_log("[%s][%d][%.2f]",__FILE__,__LINE__,lbalance);
        vtcp_log("[%s][%d][%.2f]",__FILE__,__LINE__,lmon_pay);
	/** �������� **/
	switch(sts[0])
	{
		case '1':										/*1-�޸�*/
                             vtcp_log("hehehe1");
				ret=Cif_oth_prdt_inf_Dec_Upd(g_pub_tx.reply,
				 	"cif_no=%ld and inf_no=%d",g_cif_oth_prdt_inf.cif_no,
					g_cif_oth_prdt_inf.inf_no);

                             vtcp_log("hehehe2");
			 	if (ret)
			 	{
                             vtcp_log("hehehe3");
					sprintf(acErrMsg,"DECLARE Cif_oth_prdt_inf_rel�α��쳣![%s]",
						   g_pub_tx.reply);
				  	WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
                             vtcp_log("hehehe4");
				ret=Cif_oth_prdt_inf_Fet_Upd(&g_cif_oth_prdt_inf,g_pub_tx.reply);
                             vtcp_log("hehehe5");
				if (ret)
				{
                             vtcp_log("hehehe[6]");
				   	sprintf(acErrMsg,"FETCH Cif_oth_prdt_inf�α��쳣![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				g_cif_oth_prdt_inf.balance=lbalance;
				g_cif_oth_prdt_inf.mon_pay=lmon_pay;
                             vtcp_log("hehehe[61]");
				ret=Cif_oth_prdt_inf_Upd_Upd(g_cif_oth_prdt_inf,g_pub_tx.reply);
                             vtcp_log("hehehe[62]");
				if (ret)
				{
                             vtcp_log("hehehe[63]");
    				sprintf(acErrMsg,"UPDATE Cif_oth_prdt_inf�α��쳣![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
                             vtcp_log("hehehe[64]");
				Cif_oth_prdt_inf_Clo_Upd( );
                             vtcp_log("hehehe[65]");
				break;
		case '2':										/*2-ɾ��*/
				/*ɾ���ͻ��������ڲ�Ʒ��Ϣ������*/
				sprintf(tbwhere,"cif_no=%ld and inf_no=%d",
						g_cif_oth_prdt_inf.cif_no,g_cif_oth_prdt_inf.inf_no);
				ret =deldb("cif_oth_prdt_inf",tbwhere);
				if( ret )
				{
					 sprintf(acErrMsg,"ɾ���ͻ��������ڲ�Ʒ��Ϣ���쳣!![%s]",
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
    sprintf(acErrMsg,"�ͻ��������ڲ�Ʒ��Ϣά���ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"�ͻ��������ڲ�Ʒ��Ϣά��ʧ��![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

