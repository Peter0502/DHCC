/*************************************************
* �� �� ��: spC304.c
* ����������   
*           ���˿ͻ���Ϣά��
*
* ��    ��: andy
* �������: 2004��02��05��
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
#include "cif_per_inf_c.h"

static struct cif_per_inf_c    	 cif_per_inf;
static struct cif_per_inf_c    	 g_cif_per_inf;
static struct cif_basic_inf_c    cif_basic_inf;

char memo[41],tbwhere[50];
char sts[2];
		
spC304()  
{ 	
	int ret=0;
	int cmp_code=0;
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ���ݳ�ʼ�� **/
	memset (&cif_per_inf, 0x00, sizeof(struct cif_per_inf_c));
	memset (&g_cif_per_inf, 0x00, sizeof(struct cif_per_inf_c));
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));

	/** ȡֵ����ֵ **/
	get_zd_long("0280",&g_cif_per_inf.cif_no);			/* �ͻ��� */
	get_zd_data("0720",sts);							/* �������� */
	get_zd_data( "0270",g_cif_per_inf.name);            /*�ͻ�����*/
	get_zd_data( "0670",g_cif_per_inf.sex);             /*�ͻ��Ա�*/
	get_zd_long( "0450",&g_cif_per_inf.n_bir);          /*ũ������*/
	get_zd_long( "0440",&g_cif_per_inf.g_bir);          /*��������*/
	get_zd_data( "0810",g_cif_per_inf.birth_adr);       /*������*/
	get_zd_data( "0230",g_cif_per_inf.nationality);     /*��������*/
	get_zd_data( "0210",g_cif_per_inf.folk);            /*�������*/
	get_zd_data( "0220",g_cif_per_inf.language);        /*���Դ���*/
	get_zd_data( "0710",g_cif_per_inf.edu);             /*���ѧ��*/
	get_zd_data( "0890",g_cif_per_inf.headship);        /*ְ��*/
	get_zd_data( "0900",g_cif_per_inf.technical_post);  /*ְ��*/
	get_zd_data( "0650",g_cif_per_inf.clerk_no);        /*ְԱ���*/
	get_zd_double( "0400",&g_cif_per_inf.mon_income);   /*������*/
	get_zd_double( "0410",&g_cif_per_inf.h_mon_income); /*��ͥ������*/
	get_zd_long( "0500",&g_cif_per_inf.home_cnt);        /*������ͥ��Ա��Ŀ*/
	get_zd_long( "0290",&g_cif_per_inf.mar_date);       /*�������*/
	get_zd_data( "0680",g_cif_per_inf.marriage);        /*����״��*/
	get_zd_data( "0690",g_cif_per_inf.health);          /*����״��*/
	get_zd_data( "0700",g_cif_per_inf.religion);        /*�ڽ�����*/
	get_zd_data( "0240",g_cif_per_inf.country);         /*��ס����*/

	/** �������� **/
	vtcp_log("sts is [%s]",sts);
	switch(sts[0])
	{
		case '1':									/*�޸�*/
				ret=Cif_per_inf_Dec_Upd(g_pub_tx.reply,
				   			"cif_no=%ld ",g_cif_per_inf.cif_no);
			    if (ret)
			    {
				   sprintf(acErrMsg,"DECLARE Cif_per_inf_rel�α��쳣![%s]",
				   g_pub_tx.reply);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
			    }
			    ret=Cif_per_inf_Fet_Upd(&cif_per_inf,g_pub_tx.reply);
			    if (ret)
			    {
				   sprintf(acErrMsg,"FETCH Cif_per_inf�α��쳣![%s]",
				   g_pub_tx.reply);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
			    }
				strcpy(g_cif_per_inf.rowid,cif_per_inf.rowid);
				ret=Cif_per_inf_Upd_Upd(g_cif_per_inf,g_pub_tx.reply);
				if (ret)
				{
					sprintf(acErrMsg,"UPDATE Cif_per_inf�α��쳣![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				Cif_per_inf_Clo_Upd( );
				break;
		case '2':									/*ɾ��*/

				/*ɾ���ͻ�������Ϣ������*/
				sprintf(tbwhere,"cif_no=%ld",g_cif_per_inf.cif_no);
				ret =deldb("cif_per_inf",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"ɾ�����˿ͻ�������Ϣ���¼�쳣!![%s]",ret);
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
	sprintf(acErrMsg,"�Ǽǽ�����ˮ PASS!");
	WRITEMSG
		
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"���˿ͻ�������Ϣά���ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"���˿ͻ�������Ϣά��ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
