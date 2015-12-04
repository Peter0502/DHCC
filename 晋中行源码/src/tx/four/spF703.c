/***********************************************************
* �� �� ��: spF703.c
* ������������ػ������
* ��    ��: ChenMing
* ������ڣ�200h6��1��11��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��: 
* �޸�����:	
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "card.h"
spF703()
{
	int ret = 0;
	char cCur_no[3];
	char cCity[5];
	char cPswd[7];
	char cAcno_qs[25];
	int iAc_seq = 0;

	memset(cAcno_qs, 0 , sizeof(cAcno_qs));
	memset(cCity,0x0,sizeof(cCity));
	memset(cCur_no,0x0,sizeof(cCur_no));
	memset(cPswd,0x0,sizeof(cPswd));
    memset(&g_dd_mst,0x0,sizeof(struct dd_mst_c));
    memset(&g_mdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data("0360",cCity);
	
/*  Remed by ChenMing for test 
	if( strcmp(cCity, "4302") != 0)
	{
			strcpy(g_pub_tx.reply,"F001");
			sprintf(acErrMsg,"�Ǳ��п�,�����ڱ���������ѯ![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
	}
*/

    get_zd_data("0370",g_mdm_ac_rel.ac_no);
    get_zd_data("0210",cCur_no);
    get_zd_data("0800",cPswd);
    iAc_seq = atoi(cCur_no);

    iAc_seq = 1;
    
    sprintf(acErrMsg,"----->iAc_seq====[%d]!", iAc_seq );
    WRITEMSG
   
    sprintf(acErrMsg,"----->ac_no=[%s],pass[%s]!",g_mdm_ac_rel.ac_no,cPswd);
    WRITEMSG
    
    ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &g_mdm_ac_rel, \
				"ac_no='%s'", g_mdm_ac_rel.ac_no );
    if(  ret == 100 )
    {
       	sprintf(acErrMsg,"----------->��ѯ���˺Ų�����!! [%d]",ret);
       	WRITEMSG
       	strcpy(g_pub_tx.reply, "B004");
       	goto ErrExit;
    }
	if( ret ) 
	{
       	sprintf(acErrMsg,"------->��ѯ�˺���Ϣ����!! [%d]", ret );
       	WRITEMSG
       	strcpy(g_pub_tx.reply, "B004");
       	goto ErrExit;
	}

	pub_base_EnDes( g_pub_tx.tx_date, "", cPswd );
	if ( strcmp( g_mdm_ac_rel.draw_pwd, cPswd ) )
	{
       	sprintf(acErrMsg,"�����!");
       	WRITEMSG
       	strcpy(g_pub_tx.reply, "M005");
       	goto ErrExit;
	}

	ret = Dd_mst_Sel( g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d", \
			g_mdm_ac_rel.ac_id, iAc_seq );
    if( ret )
    {
        sprintf(acErrMsg,"��ѯ������������!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "B005");
        goto ErrExit;
    }
	/* begin add by LiuHuafeng 20070204 */
	ret = pubf_card_chk_msr(1,g_mdm_ac_rel.ac_no);
	if( ret ) 
	{
       	sprintf(acErrMsg,"%s,%d�����ڿ��Ǽǲ��в�����!! [%d]",__FILE__,__LINE__,ret );
       	WRITEMSG
       	strcpy(g_pub_tx.reply, "B004");
       	goto ErrExit;
	}
	/* �Կ��Ǽǲ����д��� */
	{
		char cTradetype[4];/* ������� */
		int iSelflag=0;  /*������֯��־ */
		int atm_qy_free_flag=0;/* atm��ѯ��ѱ�־ 0��� 1�շ� */
		int pos_qy_free_flag=0;/* pos ��ѯ��ѱ�־ 0��� 1�շ� */
		int pos_tx_free_flag=0;/* pos ������ѱ�־ 0��� 1�շ� */
		int atm_tx_free_flag=0;/* atm ������ѱ�־ 0��� 1�շ� */
		int iDscpt=0;/* ��ת��ѯ��־ */

		iSelflag=0;  /*������֯��־ */                        
		atm_qy_free_flag=0;/* atm��ѯ��ѱ�־ 0��� 1�շ� */  
		pos_qy_free_flag=0;/* pos ��ѯ��ѱ�־ 0��� 1�շ� */ 
		pos_tx_free_flag=0;/* pos ������ѱ�־ 0��� 1�շ� */ 
		atm_tx_free_flag=0;/* atm ������ѱ�־ 0��� 1�շ� */ 
		iDscpt=0;/* ��ת��ѯ��־ */                           

		memset(cTradetype, 0 , sizeof(cTradetype));
		iGetTradeType(cTradetype,&iSelflag,&iDscpt);
		g_reply_int=pub_card_reg(g_pub_tx.tx_amt1,cTradetype,'0',&atm_qy_free_flag,
			&pos_qy_free_flag,&pos_tx_free_flag,&atm_tx_free_flag,iSelflag,iDscpt);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"�Ǽǿ��Ǽǲ�����");
			strncpy(g_pub_tx.reply,"AT06",4);
			WRITEMSG
			vtcp_log("%s,%d �������׵Ǽǲ����� ret=[%d]",__FILE__,__LINE__,g_reply_int);
			goto ErrExit;
		}
		/* ����ATM��������ѱ�־ */
		if(iSelflag==0)
		{
			/* ��������֯�ڵĶ�����ȡ���� */
			vtcp_log("%s,%d �ͻ������������������շ�",__FILE__,__LINE__);
			set_zd_data("0680","2");
		}
		else
		{
			if(atm_qy_free_flag==0)
			{
				/* ������ */
				set_zd_data("0680","0");
				vtcp_log("%s,%d ��ϲ�������",__FILE__,__LINE__);
			}
			else
			{
				set_zd_data("0680","1");
				vtcp_log("%s,%d �շ�û������",__FILE__,__LINE__);
			}
		}
	}
	/* �����ڲ�������� */
	ret = pub_base_GetParm_Str(g_pub_tx.tx_br_no,PARM_CARD_QS,cAcno_qs);
	if (ret)
	{
		memset(cAcno_qs, 0 , sizeof(cAcno_qs));
		strncpy(cAcno_qs,TESTCARD,sizeof(cAcno_qs)-1);
		vtcp_log("%s,%d û���ҵ�������Ĵ��̫ԭ���Ϻ����㣩�����������Ĭ���ʻ�[%s]",__FILE__,__LINE__,cAcno_qs);
	}
	set_zd_data("1211",cAcno_qs);
	set_zd_data("1214",g_pub_tx.cur_no);
	set_zd_data("1214","2");/* ת�� */
	set_zd_double("1218",g_pub_tx.tx_amt1);
	set_zd_data("1219",g_pub_tx.brf);
	/* ׼������������֧����Ŀ 0.1Ԫ 52701*/
	memset(cAcno_qs, 0 , sizeof(cAcno_qs));
	ret = pub_base_GetParm_Str(g_pub_tx.tx_br_no,PARM_CARD_OPEN_FEE,cAcno_qs);
	if (ret)
	{
		memset(cAcno_qs, 0 , sizeof(cAcno_qs));
		strncpy(cAcno_qs,"52701",sizeof(cAcno_qs)-1);
		vtcp_log("%s,%d û���ҵ�������֧����Ŀ����������Ĭ���ʻ�=%s",__FILE__,__LINE__,cAcno_qs);
	}
	set_zd_data("0640",cAcno_qs);
	/* ׼�����е渶�����ѿ�Ŀ 0.1Ԫ  139�ʺ� */
	memset(cAcno_qs, 0 , sizeof(cAcno_qs));
	ret = pub_base_GetParm_Str(g_pub_tx.tx_br_no,PARM_CARD_USER_FEE,cAcno_qs);
	if (ret)
	{
		memset(cAcno_qs, 0 , sizeof(cAcno_qs));
		strncpy(cAcno_qs,"13902",sizeof(cAcno_qs)-1);
		vtcp_log("%s,%d û���ҵ�����ʻ�����������Ĭ���ʻ�[%s]",__FILE__,__LINE__,cAcno_qs);
	}
	set_zd_data("0330",cAcno_qs);
	
	/* end by LiuHuafeng 20070204 */

	set_zd_data("0250",g_dd_mst.name);
	set_zd_data( "0270", g_dd_mst.name );
	set_zd_double("0420",g_dd_mst.bal);

	/* ���� ������� �� ���տ������ ���� ʵ����� FD74 */
	ret = iSetAdditional_Amounts( g_dd_mst.bal, g_dd_mst.bal);  /* ���� FD74 ʵ����� */
	if(ret)
	{
		/* rem by LiuHuafeng 2007-2-8 10:44 
      return -1;
		* end by LiuHuafeng 2007-2-8 10:44 */
		vtcp_log("%s,%d ���ý����ִ���",__FILE__,__LINE__);
		strncpy(g_pub_tx.reply,"AT06",4);
		goto ErrExit;
	}
OKExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��ѯ����ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"��ѯ����ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
