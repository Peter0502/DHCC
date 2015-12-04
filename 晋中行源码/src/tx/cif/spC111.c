/*************************************************
* �� �� ��: spC111.c
* ���������������ͻ���Ϣ¼��
* ��    ��: andy
* ������ڣ�2004��05��28��
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
#include "cif_magn_inf_c.h"
#include "cif_id_code_rel_c.h"

	static struct cif_basic_inf_c    cif_basic_inf;
	static struct cif_magn_inf_c      cif_magn_inf;
	static struct cif_id_code_rel_c	 g_cif_id_code_rel;    

	char credit[8];
	char corp_size[4];
	char corp_act_mode[4];	
	char corp_mana_sts[4];
	char cif_no[9];
		
spC111()  
{ 	
	int ret=0;
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	sprintf(acErrMsg,"��ʼ��ȫ�ֱ��� PASS!");
	WRITEMSG	
	
	/** ���ݳ�ʼ�� **/
	if ( data_init_struct() )	goto ErrExit;
	sprintf(acErrMsg,"���ݳ�ʼ�� PASS!");
	WRITEMSG
		
	/** ȡֵ����ֵ **/
	if ( data_set_struct() )        goto ErrExit;
	sprintf(acErrMsg,"ȡֵ����ֵ PASS!");
	WRITEMSG
		
	/** �������� **/
	/* ���ͻ����Ƿ��Ѿ����� */
	ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
						" cif_no=%ld " , cif_magn_inf.cif_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"�޴˿ͻ��ţ�����![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"��ȡ�ͻ�������Ϣ�쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		goto ErrExit;
	}
	sprintf(acErrMsg,"���ͻ����Ƿ��Ѿ����� PASS!");
	WRITEMSG
	
	/* ���ͻ����Ƿ��ǻ����ͻ��� */
	sprintf( cif_no , "%ld" , cif_magn_inf.cif_no );
	if ( cif_basic_inf.type[0]!='3' ) 
	{
		sprintf(acErrMsg,"�ǻ����ͻ���!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C092");
		goto ErrExit;
	}		
	sprintf(acErrMsg,"���ͻ����Ƿ��ǻ����ͻ��� PASS!");
	WRITEMSG

	/* ����֤�� */	
	/**			������Ҫ�󲻵ǼǶ����֤����Ϣ  Cif_id_code_rel �����ֻ����һ����Ӧ֤����Ϣ wudw 20130228	
	if(strlen(cif_magn_inf.units_license))
	{
		ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel, \
			"cif_no=%ld and id_type='8'",cif_magn_inf.cif_no);
		if(ret==100)
		{
			g_cif_id_code_rel.cif_no=cif_magn_inf.cif_no;
			strcpy(g_cif_id_code_rel.id_type,"8");
			strcpy(g_cif_id_code_rel.id_no,cif_magn_inf.units_license);

			ret=Cif_id_code_rel_Ins(g_cif_id_code_rel,&g_pub_tx.reply);
			if( ret && ret!=-239 )
			{
				sprintf(acErrMsg,"�Ǽǿͻ�֤����ͻ��Ŷ��ձ�ʧ��![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C009");
				goto ErrExit;
			}else if( ret==-239 ){
				sprintf(acErrMsg,"�˴���֤�ѵǼ�,����![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C093");
				goto ErrExit;
			}
		}
		else if(ret)
		{
			sprintf(acErrMsg,"��ѯ�ͻ�֤����ͻ��Ŷ��ձ�ʧ��!,ret=[%d]",ret);
			WRITEMSG
			ERR_DEAL;
		}		
	}*/
	/* ��ҵ����֤�� */
	/*if(strlen(cif_magn_inf.ap_qualife))
	{
		ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel, \
			"cif_no=%ld and id_type='B'",cif_magn_inf.cif_no);
		if(ret==100)
		{
			g_cif_id_code_rel.cif_no=cif_magn_inf.cif_no;
			strcpy(g_cif_id_code_rel.id_type,"B");
			strcpy(g_cif_id_code_rel.id_no,cif_magn_inf.ap_qualife);

			ret=Cif_id_code_rel_Ins(g_cif_id_code_rel,&g_pub_tx.reply);
			if( ret && ret!=-239 )
			{
				sprintf(acErrMsg,"�Ǽǿͻ�֤����ͻ��Ŷ��ձ�ʧ��![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C009");
				goto ErrExit;
			}else if( ret==-239 ){
				sprintf(acErrMsg,"����ҵ����֤�ѵǼ�,����![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C095");
				goto ErrExit;
			}
		}
		else if(ret)
		{
			sprintf(acErrMsg,"��ѯ�ͻ�֤����ͻ��Ŷ��ձ�ʧ��!,ret=[%d]",ret);
			WRITEMSG
			ERR_DEAL;
		}		
	}*/

	ret = Cif_magn_inf_Sel(g_pub_tx.reply,&cif_magn_inf,"cif_no=%ld", \
			cif_magn_inf.cif_no);
	if(ret==100)
	{
		/* �Ǽǻ����ͻ���Ϣ�� */
		ret = Cif_magn_inf_Ins( cif_magn_inf ,g_pub_tx.reply); /*******���ӷ����� 20131028***/
		if( ret )
		{
			sprintf(acErrMsg,"�Ǽǻ����ͻ���Ϣ��ʧ��![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		sprintf(acErrMsg,"�Ǽǻ����ͻ���Ϣ�� PASS!");
		WRITEMSG
	}else if(!ret)
	{
		sprintf(acErrMsg,"�ÿͻ��ѵǼǻ����ͻ���Ϣ![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C087");
		goto ErrExit;
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
    sprintf(acErrMsg,"�����ͻ���Ϣ¼��ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"�����ͻ���Ϣ¼��ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	memset (&cif_magn_inf, 0x00, sizeof(struct cif_magn_inf_c));
	memset (&g_cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));
	return 0;
}

int data_set_struct()
{    
	sprintf(acErrMsg,"���븳ֵ����");
	WRITEMSG
	
	get_zd_long("0280",&cif_magn_inf.cif_no);		/* �ͻ��� */
	get_zd_data("0620",cif_magn_inf.units_license);		/* ����֤�� */		
	get_zd_data("0330",cif_magn_inf.ap_qualife);	/* ����֤�� */	
	get_zd_data("0810",cif_magn_inf.artificial_person);/* ��ҵ�������� */				
	get_zd_data("0820",cif_magn_inf.ap_id);	   	/* �������֤���� */
	get_zd_data("0580",cif_magn_inf.credit_no);		/* ����� */
	get_zd_data("0710",cif_magn_inf.own_attri);	/* ��������ʽ */
	get_zd_data("0670",cif_magn_inf.ent_attri);		/* ��ҵ���� */	
	get_zd_data("0640",credit);				/* ������ */
	sprintf(acErrMsg,"��Ļȡ�� [%s]",credit);
	WRITEMSG
	
	pub_base_strpack ( credit );
	sprintf(acErrMsg,"ѹ���� [%s]",credit);
	WRITEMSG
	
	strncpy( cif_magn_inf.credit , credit , 2);
	sprintf(acErrMsg,"�޸ĳ���λ�� [%s]",credit);
	WRITEMSG
		
	get_zd_data("0250",cif_magn_inf.supervior);	/* �ϼ���λ */	
	get_zd_data("0260",cif_magn_inf.director);	/* ���ܵ�λ */	
	get_zd_data("0270",cif_magn_inf.financer);	/* �����ʽ������λ */	
	get_zd_data("0830",cif_magn_inf.opn_bank);	/* �����˺ſ����� */	
	get_zd_double("0410",&cif_magn_inf.opn_amt);	/* �����ʽ� */	
	return 0;
}
