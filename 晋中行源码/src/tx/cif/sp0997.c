/*************************************************
* �� �� ��: sp0997.c
* ���������������ͻ�ʶ��ID
* ��    ��: andy
* ������ڣ�2004��01��06��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#include "stdlib.h" 
#define TX_CODE "0990"
#define EXTERN
#include "public.h"
#include "pub_tx.h"
#include "trace_log_c.h"
#include "cif_basic_inf_c.h"
#include "cif_alias_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "cif_email_inf_c.h"
#include "cif_addr_inf_c.h"

static struct cif_alias_inf_c    cif_alias_inf;
static struct cif_basic_inf_c    cif_basic_inf;
static struct cif_id_code_rel_c  cif_id_code_rel;
static struct cif_addr_inf_c  	 cif_addr_inf;
static struct cif_email_inf_c  	 cif_email_inf;

sp0997()
{ 	
	int ret=0;
	int lkg_flag=0;
	long cif_no;
	long bir_date;
	char sex[2];
	char tmp_andy[11];

	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();

	/** ���ݳ�ʼ�� **/
	if ( data_init_struct() )	ERR_DEAL;

	/** ȡֵ����ֵ **/
	if ( data_set_struct() )        ERR_DEAL;
	vtcp_log("[%s][%d] cif_basic_inf.name = [%s]\n",cif_basic_inf.name);
	vtcp_log("[%s][%d] cif_addr_inf.addr = [%s]\n",cif_addr_inf.addr);

	vtcp_log("%d@%s   ȡ��ˮ�� \n", __LINE__, __FILE__);
	if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
	{
		sprintf(acErrMsg,"ȡ������ˮ�Ŵ� [%d]",g_pub_tx.trace_no);
		WRITEMSG
		goto ErrExit;
	}

	vtcp_log("%d@%s   END ȡ��ˮ�� \n", __LINE__, __FILE__);

	vtcp_log("[%s][%d] cif_basic_inf.name = [%s]\n",cif_basic_inf.name);
	if (iCharacterType(cif_addr_inf.addr,sizeof(cif_addr_inf.addr),'g'))
	{
		sprintf(acErrMsg,"�ͻ���ַ���зǷ��ַ�![%s]",g_pub_tx.reply);
		WRITEMSG
			strcpy(g_pub_tx.reply,"C128");
		goto ErrExit;
	}
	vtcp_log("[%s][%d] cif_addr_inf.addr = [%s]\n",cif_addr_inf.addr);

	/** �������� **/
	/* ���ͻ�֤�����ͣ�֤�������Ƿ��Ѿ����� */
	ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&cif_id_code_rel, 
		" id_type='%s' and id_no='%s'",cif_id_code_rel.id_type ,
		cif_id_code_rel.id_no );
	if ( ret==0 )
	{
		sprintf(acErrMsg,"�ÿͻ��Ѿ�ӵ�пͻ�ID![%s]",g_pub_tx.reply);
		WRITEMSG
			strcpy(g_pub_tx.reply,"C001");
		goto ErrExit;
	}
	else if ( ret!=100 )
	{
		sprintf(acErrMsg,"ȡ�ͻ�֤����Ϣ�쳣![%s]",g_pub_tx.reply);
		WRITEMSG
			strcpy(g_pub_tx.reply,"C002");
		goto ErrExit;
	}
	vtcp_log("%s,%d type=[%s]",__FILE__,__LINE__,cif_basic_inf.type);
	if (atoi(cif_basic_inf.type)!=1)
	{
		/*���ͻ������Ƿ��Ѿ�����*/
		vtcp_log("%s,%d name=[%s]",__FILE__,__LINE__);
		ret=Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf,"name='%s'and type!='%s' and cif_no<60000000",cif_basic_inf.name,"1");
		if (ret==0)
		{
			sprintf(acErrMsg,"�û����Ѵ���[%s]",g_pub_tx.reply);
			WRITEMSG
				strcpy(g_pub_tx.reply,"C220");
			goto ErrExit;
		}
		else if ( ret!=100 )
		{
			sprintf(acErrMsg,"ȡ�û�����Ϣ�쳣![%s]",g_pub_tx.reply);
			WRITEMSG
				strcpy(g_pub_tx.reply,"C221");
			goto ErrExit;
		}
	}
	/* ���ɿͻ�id */
	switch(atoi(cif_basic_inf.type))
	{
	case 9: /* �ڲ�Ա�� */
		ret = pub_cif_CrePer_cif_no( &cif_no );
		if ( ret )
		{
			sprintf(acErrMsg,"���ɿͻ�idʧ��!");
			WRITEMSG
				strcpy(g_pub_tx.reply,"C003");
			goto ErrExit;
		}
		break;	  	
	case 1: /* ���� */
		ret = pub_cif_CrePer_cif_no( &cif_no );
		if ( ret )
		{
			sprintf(acErrMsg,"���ɿͻ�idʧ��!");
			WRITEMSG
				strcpy(g_pub_tx.reply,"C003");
			goto ErrExit;
		}
		break;
	case 2: /* ��˾ */
	case 3: /* ���� */
	case 4: /* ���ڻ��� */
		ret = pub_cif_CreCom_cif_no( &cif_no );
		if ( ret )
		{
			sprintf(acErrMsg,"���ɿͻ�idʧ��!");
			WRITEMSG
				strcpy(g_pub_tx.reply,"C003");
			goto ErrExit;
		}
		break;		
	}	
	cif_basic_inf.cif_no = cif_no ;
	cif_id_code_rel.cif_no = cif_no ;	

	/* �Ǽǿͻ�֤����ͻ��Ŷ��ձ� */
	ret = Cif_id_code_rel_Ins( cif_id_code_rel,g_pub_tx.reply );
	vtcp_log("[%s][%d]ret===[%d]\n",__FILE__,__LINE__,ret);
	if ( ret )
	{
		vtcp_log("[%s][%d]ret===[%d]\n",__FILE__,__LINE__,ret);
		sprintf(acErrMsg,"�Ǽǿͻ�֤����ͻ��Ŷ��ձ����![%d]",ret);
		vtcp_log("[%s][%d]ret===[%d]\n",__FILE__,__LINE__,ret);
		WRITEMSG
			vtcp_log("[%s][%d]ret===[%d]\n",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"C004");
		vtcp_log("[%s][%d]ret===[%d]\n",__FILE__,__LINE__,ret);
		goto ErrExit;
		vtcp_log("[%s][%d]ret===[%d]\n",__FILE__,__LINE__,ret);
	}
	vtcp_log("[%s][%d]ret===[%d]\n",__FILE__,__LINE__,ret);

	/* �Ǽǿͻ�������Ϣ�� */
	cif_basic_inf.crt_date = g_pub_tx.tx_date ; /* �������� */
	cif_basic_inf.crt_time = g_pub_tx.tx_time ; /* ����ʱ�� */
	strcpy( cif_basic_inf.sts , "1" ) ;         /* �ͻ�״̬��1���� 2ע�� */
	cif_basic_inf.cif_seqn = 0 ;                /* �ͻ���� */

	ret = Cif_basic_inf_Ins( cif_basic_inf,g_pub_tx.reply );
	if ( ret )
	{
		sprintf(acErrMsg,"�Ǽǿͻ�������Ϣ�����![%d]",ret);
		WRITEMSG
			strcpy(g_pub_tx.reply,"C005");
		goto ErrExit;
	}
	strcpy(cif_alias_inf.alias,cif_basic_inf.name);		/* �ͻ����� */
	cif_alias_inf.cif_no = cif_basic_inf.cif_no ; 		/* �� �� �� */
	cif_alias_inf.alias_seqn = 1 ;                		/* ������� */
	ret=Cif_alias_inf_Ins(cif_alias_inf,g_pub_tx.reply);
	if ( ret )
	{
		sprintf(acErrMsg,"�Ǽǿͻ�������Ϣ�����![%d]",ret);
		WRITEMSG
			strcpy(g_pub_tx.reply,"C083");
		goto ErrExit;
	}
	/* �Ǽǽ�����ˮ */
	if ( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
			goto ErrExit;
	}
	if (cif_basic_inf.type[0]=='1'||cif_basic_inf.type[0]=='5')
	{
		strcpy(cif_addr_inf.addr_type,"2");
		if (!strncmp(cif_email_inf.email,"13",2)&&strlen(cif_email_inf.email)==11)
			strcpy(cif_email_inf.addr_type,"5");
		else
			strcpy(cif_email_inf.addr_type,"1");
	}
	else if (cif_basic_inf.type[0]=='2'||cif_basic_inf.type[0]=='3'||cif_basic_inf.type[0]=='4')
	{
		strcpy(cif_addr_inf.addr_type,"1");
		strcpy(cif_email_inf.addr_type,"2");
	}
	cif_addr_inf.cif_no=cif_no;
	cif_email_inf.cif_no=cif_no;
	cif_addr_inf.addr_seqn=1;
	cif_email_inf.addr_seqn=1;
	strcpy(cif_addr_inf.country,"01");
	strcpy(cif_email_inf.link_man,cif_basic_inf.name);

	ret=Cif_email_inf_Ins(cif_email_inf,g_pub_tx.reply);
	if (ret)
	{
		Cif_email_inf_Debug(&cif_email_inf);
		sprintf(acErrMsg,"�Ǽǵ��ӵ�ַ��Ϣ�����![%d]",ret);
		WRITEMSG
			strcpy(g_pub_tx.reply,"C024");
		goto ErrExit;
	}
	ret=Cif_addr_inf_Ins(cif_addr_inf,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"�Ǽǵ�ַ��Ϣ�����![%d]",ret);
		WRITEMSG
			strcpy(g_pub_tx.reply,"C023");
		goto ErrExit;
	}
	TRACE

	/* ������� */
	set_zd_long( "0280", cif_no );
	set_zd_data("0160",TX_CODE);
	sprintf(acErrMsg,"�ͻ���[%ld]",cif_no);
	WRITEMSG

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�����ͻ�ʶ��ID�ɹ���[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	vtcp_log("[%s][%d]FUCK!ret==[%d]\n",__FILE__,__LINE__,ret);
	sprintf(acErrMsg,"�����ͻ�ʶ��IDʧ�ܣ�[%s]",g_pub_tx.reply);
	vtcp_log("[%s][%d]FUCK!ret==[%d]\n",__FILE__,__LINE__,ret);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

int data_init_struct()
{
	memset(&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	memset(&cif_alias_inf, 0x00, sizeof(struct cif_alias_inf_c));
	memset(&cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));
	memset(&cif_id_code_rel, 0 ,sizeof(struct cif_id_code_rel_c));
	memset(&cif_addr_inf   , 0 ,sizeof(struct cif_addr_inf_c));
	memset(&cif_email_inf  , 0 ,sizeof(struct cif_email_inf_c));
	return 0;
}

int data_set_struct()
{
	get_zd_data("0670",cif_basic_inf.type);		/* �ͻ����� */
	get_zd_data("0680",cif_id_code_rel.id_type);/* ֤������ */
	get_zd_data("0620",cif_id_code_rel.id_no);	/* ֤������ */
	get_zd_data("0250",cif_basic_inf.name);		/* �ͻ����� */
	strcpy(cif_basic_inf.local,"1");			/* ���ر�־1-����2-���3-���� */
	strcpy(cif_basic_inf.poten,"Y");			/* Ǳ�ڱ�־Y-Ǳ�� N-��Ǳ��*/
	strcpy(cif_basic_inf.shareholder_ind,"N");	/* ���йɶ�Y-�� N-�� */
	strcpy(cif_basic_inf.lvl,"4");
	/*�ͻ�����1 �׽𿨿ͻ� 2�𿨿ͻ� 3�����ͻ� 4��ͨ�ͻ� */

	get_zd_data("0260",cif_email_inf.email);	/*��ϵ��Ϣ*/
	get_zd_data("0270",cif_addr_inf.addr);		/*��ַ��Ϣ*/
	return 0;
}
