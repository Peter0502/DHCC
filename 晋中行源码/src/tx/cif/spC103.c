/*************************************************
* �� �� ��: spC103.c
* ������������˾�ͻ���Ϣ¼��
* ��    ��: andy
* ������ڣ�2004��01��10��
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
#include "cif_cop_inf_c.h"
#include "cif_id_code_rel_c.h"

	static struct cif_basic_inf_c    cif_basic_inf;
	static struct cif_cop_inf_c      cif_cop_inf;
	static struct cif_id_code_rel_c	 g_cif_id_code_rel;    

	char credit[8];
	char corp_size[4];
	char corp_act_mode[4];	
	char corp_mana_sts[4];
	char cif_no[9];
		
spC103()  
{ 	
	int ret=0;
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	sprintf(acErrMsg,"��ʼ��ȫ�ֱ��� PASS!");
	WRITEMSG	
	
	/** ���ݳ�ʼ�� **/
	if ( data_init_struct() )	ERR_DEAL;
	sprintf(acErrMsg,"���ݳ�ʼ�� PASS!");
	WRITEMSG
		
	/** ȡֵ����ֵ **/
	if ( data_set_struct() )        ERR_DEAL;
	sprintf(acErrMsg,"ȡֵ����ֵ PASS!");
	WRITEMSG
		
	/** �������� **/
	/* ���ͻ����Ƿ��Ѿ����� */
	ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
						" cif_no=%ld " , cif_cop_inf.cif_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"�޴˿ͻ��ţ�����![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
		ERR_DEAL;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"��ȡ�ͻ�������Ϣ�쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		ERR_DEAL;
	}
	sprintf(acErrMsg,"���ͻ����Ƿ��Ѿ����� PASS!");
	WRITEMSG
	
	/* ���ͻ����Ƿ��ǶԹ��ͻ��� */
	sprintf( cif_no , "%ld" , cif_cop_inf.cif_no );
	if ( cif_no[0] != '5' )
	{
		sprintf(acErrMsg,"�ǶԹ��ͻ���!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C010");
		ERR_DEAL;
	}		
	sprintf(acErrMsg,"���ͻ����Ƿ��ǶԹ��ͻ��� PASS!");
	WRITEMSG
	/* ����֤�� */
	/**			������Ҫ�󲻵ǼǶ����֤����Ϣ  Cif_id_code_rel �����ֻ����һ����Ӧ֤����Ϣ wudw 20130228
	if(strlen(cif_cop_inf.crd_no))
	{
			memset (&g_cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));
		ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel, \
			"cif_no=%ld and id_type='8'",cif_cop_inf.cif_no);
		if(ret==100)
		{
			g_cif_id_code_rel.cif_no=cif_cop_inf.cif_no;
			strcpy(g_cif_id_code_rel.id_type,"8");
			strcpy(g_cif_id_code_rel.id_no,cif_cop_inf.crd_no);

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
				sprintf(acErrMsg,"�˴���֤[%s][%s]",cif_cop_inf.crd_no,g_cif_id_code_rel.id_no);
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
	}  */
	/* Ӫҵִ�� */
	/* if(strlen(cif_cop_inf.license))
	{
			memset (&g_cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));
		vtcp_log("PASSPASSPASSPASS");
		ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel,
			"cif_no=%ld and id_type='A'",cif_cop_inf.cif_no);
		if(ret==100)
		{
			g_cif_id_code_rel.cif_no=cif_cop_inf.cif_no;
			strcpy(g_cif_id_code_rel.id_type,"A");
			strcpy(g_cif_id_code_rel.id_no,cif_cop_inf.license);

			ret=Cif_id_code_rel_Ins(g_cif_id_code_rel,&g_pub_tx.reply);
			if(ret&&ret!=-239)
			{
				sprintf(acErrMsg,"�Ǽǿͻ�֤����ͻ��Ŷ��ձ�ʧ��![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C009");
				goto ErrExit;
			}else if(ret==-239){
				sprintf(acErrMsg,"��Ӫҵִ���ѵǼ�,����!");
				WRITEMSG
				sprintf(acErrMsg,"Ӫҵִ��[%s][%s]",cif_cop_inf.license,g_cif_id_code_rel.id_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C091");
				goto ErrExit;
			}

		}
		else if(ret)
		{
			sprintf(acErrMsg,"��ѯ�ͻ�֤����ͻ��Ŷ��ձ�ʧ��!,ret=[%d]",ret);
			WRITEMSG
			ERR_DEAL;
		}		
	} */
	/* ��ҵ��λ�Ǽ����֤ */
	/*if(strlen(cif_cop_inf.units_license))
	{
			memset (&g_cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));
		vtcp_log("PASSPASSPASSPASS");
		ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel, \
			"cif_no=%ld and id_type='9'",cif_cop_inf.cif_no);
		if(ret==100)
		{
			g_cif_id_code_rel.cif_no=cif_cop_inf.cif_no;
			strcpy(g_cif_id_code_rel.id_type,"9");
			strcpy(g_cif_id_code_rel.id_no,cif_cop_inf.units_license);

			ret=Cif_id_code_rel_Ins(g_cif_id_code_rel,&g_pub_tx.reply);
			if(ret&&ret!=-239)
			{
				sprintf(acErrMsg,"�Ǽǿͻ�֤����ͻ��Ŷ��ձ�ʧ��![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C009");
				goto ErrExit;
			}else if(ret==-239){
				sprintf(acErrMsg,"�˾�Ӫ���֤�ѵǼ�,����!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"C094");
				goto ErrExit;
			}

		}
		else if(ret)
		{
			sprintf(acErrMsg,"��ѯ�ͻ�֤����ͻ��Ŷ��ձ�ʧ��!,ret=[%d]",ret);
			WRITEMSG
			ERR_DEAL;
		}		
	}
	**/
	ret = Cif_cop_inf_Sel(g_pub_tx.reply,&cif_cop_inf,"cif_no=%ld", \
			cif_cop_inf.cif_no);
	if(ret==100)
	{
		/* �Ǽǹ�˾�ͻ���Ϣ�� */
		ret = Cif_cop_inf_Ins( cif_cop_inf );
		if( ret )
		{
			sprintf(acErrMsg,"�Ǽǹ�˾�ͻ���Ϣ��ʧ��![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		sprintf(acErrMsg,"�Ǽǹ�˾�ͻ���Ϣ�� PASS!");
		WRITEMSG
	}
	else if(!ret)
	{
		sprintf(acErrMsg,"�ÿͻ��ѵǼǹ�˾�ͻ���Ϣ![%d]",ret);
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
    sprintf(acErrMsg,"��˾�ͻ���Ϣ¼��ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"��˾�ͻ���Ϣ¼��ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	memset (&cif_cop_inf, 0x00, sizeof(struct cif_cop_inf_c));
	memset (&g_cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));
	return 0;
}

int data_set_struct()
{    
	sprintf(acErrMsg,"���븳ֵ����");
	WRITEMSG
	
	get_zd_long("0280",&cif_cop_inf.cif_no);		/* �ͻ��� */
	get_zd_long("0440",&cif_cop_inf.reg_date);		/* ע������ */
	get_zd_data("0890",cif_cop_inf.reg_country);	/* ע����� */
	get_zd_double("0400",&cif_cop_inf.reg_fund);	/* ע���ʽ� */	
	get_zd_data("0210",cif_cop_inf.cur_no);			/* ע����� */
	get_zd_data("0620",cif_cop_inf.crd_no);			/* ����֤�� */	
	get_zd_data("0300",cif_cop_inf.units_license);	/* ��ҵ��λ�Ǽ����֤ */		
	get_zd_data("0310",cif_cop_inf.g_tax_no);		/* ��˰�Ǽ�֤ */	
	get_zd_data("0320",cif_cop_inf.d_tax_no);		/* ��˰�Ǽ�֤ */	
	get_zd_data("0580",cif_cop_inf.credit_no);		/* ����� */
	get_zd_data("0630",cif_cop_inf.license);		/* Ӫҵִ�� */	
	/*
	get_zd_data("0660",cif_cop_inf.eco_attri);	* �������� *	
	*/
	strcpy(cif_cop_inf.eco_attri,"");
	get_zd_data("0670",cif_cop_inf.ent_attri);		/* ��ҵ���� */	
	
	get_zd_data("0640",credit);						/* ������ */
	sprintf(acErrMsg,"��Ļȡ�� [%s]",credit);
	WRITEMSG
	
	pub_base_strpack ( credit );
	sprintf(acErrMsg,"ѹ���� [%s]",credit);
	WRITEMSG
	
	strncpy( cif_cop_inf.credit , credit , 2);
	sprintf(acErrMsg,"�޸ĳ���λ�� [%s]",credit);
	WRITEMSG	
	
	get_zd_data("0700",cif_cop_inf.region);		/* ���򼶱� */	
	get_zd_data("0250",cif_cop_inf.supervior);	/* �ϼ���λ */	
	get_zd_data("0260",cif_cop_inf.director);	/* ���ܵ�λ */	
	get_zd_data("0680",cif_cop_inf.belong_bank);/* ������ҵ(���л���) */	
	get_zd_data("0690",cif_cop_inf.belong_stat);/* ������ҵ(ͳ�ƾֻ���) */
	get_zd_data("0710",cif_cop_inf.mana_orga_moda);	/* ��Ӫ��ʽ */
	get_zd_data("0720",cif_cop_inf.bd_corp_orga_moda);/* ��ҵ��ʽ */
		
	get_zd_data("0900",corp_size);			/* ��ҵ��ģ */	
	pub_base_strpack ( corp_size );
	strncpy( cif_cop_inf.corp_size , corp_size , 2);
	
	get_zd_data("0230",corp_act_mode);		/* ������ʽ */	
	pub_base_strpack ( corp_act_mode );
	strncpy( cif_cop_inf.corp_act_mode , corp_act_mode , 2);
	
	get_zd_double("0410",&cif_cop_inf.paicl_up_capital);/* ʵ���ʱ� */				
	get_zd_data("0240",corp_mana_sts);		/* ��Ӫ״̬ */	
	pub_base_strpack ( corp_mana_sts );
	strncpy( cif_cop_inf.corp_mana_sts , corp_mana_sts , 2);	
					
	get_zd_data("0330",cif_cop_inf.ap_qualife);	/* �����ʸ� */	
	get_zd_data("0810",cif_cop_inf.artificial_person);/* �������� */				
	get_zd_data("0820",cif_cop_inf.ap_id);	   	/* �������֤���� */
	/*
	get_zd_data("0220",cif_cop_inf.cif_scopes);	   	* �������� *
	*/
	strcpy(cif_cop_inf.cif_scopes,"");
	return 0;
}
