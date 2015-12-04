/*************************************************
* �� �� ��: spLT12.c
* ����������   
*              �ɴ���ǷϢ�黹���� 
*
* ��    ��:    wanglei
* ������ڣ�   2007��05��25��
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
#include "intrtmpreg_c.h"

		
spLT12()
{ 
	int    ret=0;
	int    iAc_seqn=0;
	int    iOwe_seqn=0;  /****�������****/
	char   cAc_no[20];
	char   cStat[2];     /****�����־: 1 δ�� 2 ���ֻ��� 3 ����****/
	double dTx_amt=0.00,dOwe_bal=0.00;
	char   newAc_no[20];   /* add by yanqq 20111221 ���ʺŶ�Ӧ�����ʺ� */

	static	struct intrtmpreg_c	intrtmpreg;

	memset (&intrtmpreg, 0x00, sizeof(struct intrtmpreg_c));
	memset(&g_mdm_ac_rel,0x00,sizeof(g_mdm_ac_rel));
	memset(&g_ln_mst,0x00,sizeof(g_ln_mst));
	memset(&g_ln_parm,0x00,sizeof(g_ln_parm));
	memset(cAc_no,0x00,sizeof(cAc_no));
	memset(cStat,0x00,sizeof(cStat));
	memset(newAc_no,0x00,sizeof(newAc_no));   /* add by yanqq 20111221 ���ʺŶ�Ӧ�����ʺ� */
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	vtcp_log("[%s][%d]>>>>>>>>",__FILE__,__LINE__);
	/** ȡֵ����ֵ **/
	get_zd_data("0300",cAc_no);
	get_zd_data("0300",newAc_no);    /* add by yanqq 20111221 ���ʺŶ�Ӧ�����ʺ� */
	vtcp_log("[%s][%d]�����ʺţ�ac_no===[%s],newAc_no=[%s]",__FILE__,__LINE__,cAc_no,newAc_no);
	get_zd_int("0340",&iAc_seqn);
	vtcp_log("[%s][%d]ac_seqn===[%d]",__FILE__,__LINE__,iAc_seqn);
	get_zd_int("0770",&iOwe_seqn);   /****Ƿ�����****/
	vtcp_log("[%s][%d]seq===[%d]",__FILE__,__LINE__,iOwe_seqn);
	get_zd_double("0400",&dTx_amt);  /****��ǷϢ���****/
	vtcp_log("[%s][%d]tx_amt===[%.2lf]",__FILE__,__LINE__,dTx_amt);

	sprintf(acErrMsg,"ȡֵ����ֵ PASS!");
	WRITEMSG
	vtcp_log("[%s][%d]ac_no===[%s]",__FILE__,__LINE__,cAc_no);
	vtcp_log("[%s][%d]ac_seqn===[%d]",__FILE__,__LINE__,iAc_seqn);
	vtcp_log("[%s][%d]tx_amt===[%.2lf]",__FILE__,__LINE__,dTx_amt);

	/* add beg by yanqq 20111221 ���ʺ��¾��ʺ�ת�� */
	pub_base_old_acno( newAc_no );           /****�����ʺŴ���****/
	vtcp_log("[%s][%d]�¾��ʺ�ת����ac_no===[%s],newAc_no=[%s]",__FILE__,__LINE__,cAc_no,newAc_no);
	/* add end  by yanqq 20111221 */

	/* ȡ������Ϣ */
	/* ret = pub_ln_lndis_file(cAc_no,iAc_seqn,&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); del by yanqq 20111221 ��Ӧ�����ʺŽ��м�� */
	ret = pub_ln_lndis_file(newAc_no,iAc_seqn,&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm );
	if( ret )
		goto ErrExit;

	ret = Intrtmpreg_Sel(g_pub_tx.reply,&intrtmpreg,"actno = '%s' and actnoseq = %d",cAc_no,iOwe_seqn);
	if(ret == 100)
	{
		sprintf(acErrMsg,"�ϴ���ǷϢ�Ǽǲ����޴˱ʼ�¼![%s]",cAc_no); 
		WRITEMSG
		strcpy(g_pub_tx.reply,"L410");
		goto ErrExit;
	}
	else if(ret)
	{
		sprintf(acErrMsg,"��ѯ�ϴ���ǷϢ�Ǽǲ�����![%d]",ret); 
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	dOwe_bal = intrtmpreg.avbal;  /****�õ�ǷϢ���****/

	/****��״̬Ϊ '3' ˵��������¼ǷϢ�Ѿ�����****/
	if( intrtmpreg.stat[0] == '3')
	{
		sprintf(acErrMsg,"����ǷϢ�ѻ���!"); 
		WRITEMSG
		strcpy(g_pub_tx.reply,"L411");
		goto ErrExit;
	}


	/* ������Ƿ��� */
	ret = pub_base_CompDblVal( dOwe_bal , dTx_amt );
	if( ret < 0 )
	{
		sprintf(acErrMsg,"���������Ƿ����[%.2lf][%.2lf]",dOwe_bal,dTx_amt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"B108");
		goto ErrExit;
	}
	else if(ret > 0)
	{
		cStat[0] = '2';
	}
	else
	{
		cStat[0] = '3';
	}

	/****����ǷϢ�Ǽǲ��Ľ���״̬****/
	ret = Intrtmpreg_Dec_Upd(g_pub_tx.reply,"actno = '%s' and actnoseq = %d",cAc_no,iOwe_seqn);
	if(ret )
	{
		sprintf(acErrMsg,"��ѯ�ϴ���ǷϢ�Ǽǲ�����![%d]",ret); 
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	ret = Intrtmpreg_Fet_Upd(&intrtmpreg,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"��ѯ�ϴ���ǷϢ�Ǽǲ�����![%d]",ret); 
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	strcpy(intrtmpreg.actno,cAc_no);
	intrtmpreg.actnoseq = iOwe_seqn;
	strcpy(intrtmpreg.stat,cStat);
	intrtmpreg.avbal = dOwe_bal - dTx_amt;

	ret = Intrtmpreg_Upd_Upd(intrtmpreg,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"�����ϴ���ǷϢ�Ǽǲ�����![%d]",ret); 
		WRITEMSG
		strcpy(g_pub_tx.reply,"D105");
		goto ErrExit;
	}
	Intrtmpreg_Clo_Upd();


OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�ϴ���ǷϢ����ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"�ϴ���ǷϢ����ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

