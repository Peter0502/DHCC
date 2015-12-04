/****************************************************************************************
* �� �� ��:  spE603.c
* ���������� ��Ա�깤����
*            ����С���Ƿ���δ���͡����˽��ף���Ա�Ƿ����и������Է���δ����Ľ��Ӽ�¼;
*            ��Ա�Ƿ�����Ӧ�����뵫��δ����Ľ��Ӽ�¼, �Ƿ��и���û��������ļ�¼
*            ���֧�в����Լ�4��ͷ��Ŀ, ��������ֻ���Լ�408�Ŀ�Ŀ
*            lvl=6�Ĺ�Ա����ǩ��
* ��    ��:  andy
* ������ڣ� 2004��3��9��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��: xxx/20040712
*   �޸�����: ͬ�ǿ���
*   ��    ��: 20070412
*   �� �� ��: wanglei 
*   �޸�����: ��Աȯ�������������������Ƿ���ͬ������ͬ������ǩ��
*   ��    ��: 20070428
*   �� �� ��: wanglei 
*   �޸�����: Ҫ���Ա����ȯ������������������Ƚϣ�����ͬ������ǩ��
******************************************************************************************/
#include <stdio.h>
#include <math.h>
#include "public.h"
#include "note_mst_c.h"
#include "note_mst_hst_c.h"
#include "com_tel_c.h"
#include "com_tel_connect_c.h"
#include "com_parm_c.h"
#include "dc_log_c.h"
#include "trace_log_c.h"
#include "cash_mst_c.h"
#define  EXTERN


int spE603()
{
	double d_amt=0.00, c_amt=0.00;
	int	ret=0;
	int mcnt=0, zcnt=0;
	double tmp_amt1=0.00, tmp_amt2=0.00, tmp_amt3=0.00, tmp_amt4=0.00;
	double dCert=0.00;
	double dBal=0.00;  /****98��Ա�����****/
	char tmp_str[256],tmp_600[600];

	struct	com_tel_c	g_com_tel;
	struct	com_tel_connect_c	g_com_tel_connect;
	struct  com_parm_c sCom_parm;
	struct  dc_log_c sDc_log;
	struct  trace_log_c sTrace_log;
	struct  cash_mst_c cash_mst;
	struct  st_money_dtl money_box;

	memset(&g_com_tel,0x00,sizeof(struct com_tel_c));
	memset(&g_com_tel_connect,0x00,sizeof(struct com_tel_connect_c));
	memset(tmp_str, 0x00, sizeof(tmp_str));
	memset(&sDc_log, 0x00, sizeof(struct dc_log_c));
	memset(&sTrace_log, 0x00, sizeof(struct trace_log_c));
	memset(&cash_mst, 0x00, sizeof(struct cash_mst_c));
	memset(&money_box, 0x00, sizeof(struct st_money_dtl));
	memset(tmp_600,'\0',sizeof(tmp_600));

	get_zd_data("0950",tmp_600); /****�õ���ǰ̨������ȯ������****/
	strcpy((char *)&money_box,tmp_600);

	/* ��ʼ���ṹ */
	pub_base_sysinit();

	/* add by LiuHuafeng ����Ƿ���δ����δ���͵����� */
	
	{
		char cTel[7];
		memset(cTel, 0 , sizeof(cTel));
		int iCnt=0;
		get_zd_data("0070",cTel);
		iCnt=sql_count("hv_zf","cmtno in ('100','101','102','103','105','108','121') and hv_sts='0' and chk='%s'",cTel);
		if(iCnt>0)
		{
			memset(acErrMsg, 0 , sizeof(acErrMsg));
			sprintf( acErrMsg, 
				"������%d�ʸ���δ���ʹ��ҵ��",iCnt);
			set_zd_data("0130",acErrMsg);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O155");
			goto ErrExit;
		}
	}
	
	/* end by LiuHuafeng 20061229 */
	/*����Ա�Ƿ����и������Է���δ����Ľ��Ӽ�¼*/
	ret=Com_tel_connect_Sel(g_pub_tx.reply,&g_com_tel_connect,"tel='%s' and ind in('0','A')",g_pub_tx.tel);
	if(ret==0)
	{
		sprintf( acErrMsg, 
				"��Ա�и����Ľ��Ӽ�¼,���Է���Ա��δ����,��������!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O048");
		goto ErrExit;
	}else if(ret&&ret!=100){
		goto ErrExit;
	}

	
	/*����Ա�Ƿ�����Ӧ�����뵫��δ����Ľ��Ӽ�¼*/
	ret=Com_tel_connect_Sel(g_pub_tx.reply,&g_com_tel_connect,"tw_tel='%s' and ind in('0','A')",g_pub_tx.tel);
	if(ret==0)
	{
		sprintf( acErrMsg, "��Ա��Ӧ������Ľ��Ӽ�¼����δ����,��������!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O047");
		goto ErrExit;
	}else if(ret && ret!=100){
		goto ErrExit;
	}

	/* add by xxx at 20040907 **/
	/* ����Ƿ��и���û��������ļ�¼ */
	ret = sql_count("note_mst", "tel='%s' and sts = '2'", g_pub_tx.tel);
	if (ret < 0)
	{
		sprintf(acErrMsg, "ͳ��note_mst�����!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D101");
		goto ErrExit;
	}
	if (ret > 0)
	{
		sprintf(acErrMsg, "�е�������״̬��ƾ֤!![%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "N095");
		goto ErrExit;
	}

	/* add by xxx end **/
	
	/**add by spj for vtm 20140610 beg**/
	ret = sql_count("vtm_note_mst", "tw_tel='%s' and ind = '2'", g_pub_tx.tel);
	if (0 > ret)
	{
		sprintf(acErrMsg, "�������ݿ�[vtm_note_mst]����!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "AT03");
		goto ErrExit;
	}
	else if (0 < ret)
	{
		sprintf(acErrMsg, "�ù�Ա����δ���յĿ�!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "A096");
		goto ErrExit;
	}
	/**add by spj for vtm 20140610 end**/

	/* wanglei begin 20070412 */
	ret = Cash_mst_Sel(g_pub_tx.reply,&cash_mst,"tel='%s'",g_pub_tx.tel);
	if(ret)
	{
		sprintf( acErrMsg, "��ѯ�ֽ�Ǽǲ������!![%d]",ret );
		WRITEMSG
		goto ErrExit;
	}
	/****�˶�ȯ������     98��Ա���������****/
	if(memcmp(g_pub_tx.tel+2,"98",2) != 0)
	{
		if((atol(money_box.money100) != cash_mst.t10000) || (atol(money_box.money50) != cash_mst.t5000) || \
		   (atol(money_box.money20)  != cash_mst.t2000) || (atol(money_box.money10)  != cash_mst.t1000) || \
		   (atol(money_box.money5)   != cash_mst.t500)  || (atol(money_box.money2)   != cash_mst.t200) || \
		   (atol(money_box.money1)   != cash_mst.t100)  || (atol(money_box.money05)  != cash_mst.t50) || \
		   (atol(money_box.money02)  != cash_mst.t20)   || (atol(money_box.money01)  != cash_mst.t10) || \
		   (atol(money_box.money005) != cash_mst.t5)    || (atol(money_box.money002) != cash_mst.t2) || \
		   (atol(money_box.money001) != cash_mst.t1) )
		{
			vtcp_log("10000[%d],5000[%d],2000[%d],1000[%d],500[%d],200[%d],100[%d],50[%d],20[%d],10[%d],5[%d],2[%d],1[%d]\n",cash_mst.t10000,cash_mst.t5000,cash_mst.t2000,cash_mst.t1000,cash_mst.t500,cash_mst.t200,cash_mst.t100,cash_mst.t50,cash_mst.t20,cash_mst.t10,cash_mst.t5,cash_mst.t2,cash_mst.t1);


				sprintf(acErrMsg,"��Ա����ȯ���������̨��¼����!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "M039");
				goto ErrExit;
		}
	}else{/****98��Աǩ��ʱ���˶����****/
		get_zd_double("0400",&dBal);
		if(pub_base_CompDblVal(dBal, cash_mst.bal) != 0)
		{
			sprintf(acErrMsg,"98��Ա�ֽ�����! dbal=[%.2f] bal= [%.2f]",dBal,cash_mst.bal);
			WRITEMSG
			strcpy(g_pub_tx.reply, "M040");
			goto ErrExit;
		}
	}

	dCert = cash_mst.t10000*100.0+cash_mst.t5000*50.0+cash_mst.t2000*20+cash_mst.t1000*10+ \
	cash_mst.t500*5+cash_mst.t200*2+cash_mst.t100*1+cash_mst.t50*0.5+cash_mst.t20*0.2+ \
	cash_mst.t10*0.1+cash_mst.t5*0.05+cash_mst.t2*0.02+cash_mst.t1*0.01;

	/****wanglei 20070426   ������98��Աȯ��ϼ��ܶ����ֽ�����ȵ����****/
	if(memcmp(g_pub_tx.tel+2,"98",2) != 0)
	{
		if(pub_base_CompDblVal(dCert, cash_mst.bal) != 0)
		{
			sprintf(acErrMsg,"��Աȯ��ϼ��ܶ����ֽ�����! cert=[%.2f] bal= [%.2f]",dCert,cash_mst.bal);
			WRITEMSG
			strcpy(g_pub_tx.reply, "M038");
			goto ErrExit;
		}
	}

	/* wanglei end 20070412 */

  /*****/
  ret=sql_count("com_tel","br_no='%s' and csts='0' and tel not like '%99'",g_pub_tx.tx_br_no,g_pub_tx.tel);
  if(ret==1)
  {
  	set_zd_int("0680",ret);
  }
  /*****/
	/*����Ա״̬*/
	ret=Com_tel_Sel(g_pub_tx.reply,&g_com_tel,"tel='%s'",
					g_pub_tx.tel);
	if(ret)
	{
		sprintf( acErrMsg, "��ѯ��Ա��Ϣ�����!!" );
		WRITEMSG
		goto ErrExit;
	}

	if( g_com_tel.lvl[0]=='6' )
	{
		sprintf( acErrMsg, "������[%s]!!",g_pub_tx.tel );
		WRITEMSG
		strcpy(g_pub_tx.reply,"S080");
		goto ErrExit;
	}

	if(g_com_tel.csts[0]=='2')
	{
		sprintf( acErrMsg, "��Ա�Ѿ�ǩ��!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O046");
		goto ErrExit;
	}

	if(g_com_tel.csts[0]=='3')
	{
		sprintf( acErrMsg, "�ù�Ա�ѱ�����!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O063");
		goto ErrExit;
	}

	if(g_com_tel.csts[0]=='4')
	{
		sprintf( acErrMsg, "�ù�Ա�ѱ�ɾ��!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O062");
		goto ErrExit;
	}

	/* ���4��ͷ��Ŀ�����Լ���,�������Ŀɼ�408 **/
	d_amt=0;
if(memcmp(g_pub_tx.tx_br_no,QS_BR_NO,5)!=0)
{
	ret = sql_sum_double("dc_log", "amt", &d_amt, "tx_date=%ld and tel='%s' \
		and sts='0' and substr(acc_hrt,1,3) in('407','408','416') and tx_opn_br_no='%s' ",g_pub_tx.tx_date, g_pub_tx.tel,g_pub_tx.tx_br_no);
}
else{
	ret = sql_sum_double("dc_log", "amt", &d_amt, "tx_date=%ld and tel='%s' \
		and sts='0' and substr(acc_hrt,1,3) in('407','416')",g_pub_tx.tx_date, g_pub_tx.tel);
}
	if (ret != 0)
	{
vtcp_log("%d@%s ret=%d",__LINE__, __FILE__, ret);
		sprintf(acErrMsg, "ͳ��4��ͷ��Ŀ���������!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

		/*vtcp_log("d_amt is[%.2f] ",d_amt);*/
	if (pub_base_CompDblVal(d_amt, 0.00) != 0)
	{
		sprintf(acErrMsg, "�����Լ�4��ͷ��Ŀ [%.2f] [%.2f]",d_amt, c_amt);
		WRITEMSG
		strcpy(g_pub_tx.reply, "P081");
		goto ErrExit;
	}

	/*�޸Ĺ�Ա״̬Ϊǩ��*/
	ret=sql_execute("update com_tel set csts='%s' where tel='%s'",
					 "2",g_pub_tx.tel);
	if(ret)
	{
		sprintf( acErrMsg, "���¹�Ա״̬����!!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O049");
		goto ErrExit;
	}

	/* add by xxx at 20040820 */
	/* ���ù�Ա�Ļ����ˮ�Ƿ�ƽ�� */
	/*ȡ���ս跽����������*/ 
	tmp_amt1 = 0.00;
	tmp_amt2 = 0.00;
	tmp_amt3 = 0.00;
	tmp_amt4 = 0.00;
	ret=Dc_log_Dec_Sel(g_pub_tx.reply, "dc_ind='1' and tx_tx_br_no='%s' and \
		sts='0' and acc_hrt[1]<'6' and tel='%s'", g_pub_tx.tx_br_no,
		g_pub_tx.tel); 
	if(ret) 
	{ 
		sprintf( acErrMsg, "Declare dc_log error!!" ); 
		WRITEMSG 
		goto ErrExit; 
	} 
	
	while(1) 
	{ 
		ret=Dc_log_Fet_Sel(&sDc_log, g_pub_tx.reply); 
		if(ret==100)break; 
		else if(ret) 
		{ 
			sprintf( acErrMsg, "Fetch dc_log record error!!" ); 
			WRITEMSG 
			goto ErrExit; 
		} 
		
		if( sDc_log.ct_ind[0]=='1' ) tmp_amt3+=sDc_log.amt; 
		tmp_amt1+=sDc_log.amt; 
	} 
	Dc_log_Clo_Sel(); 
	
	/*ȡ���մ�������������*/ 
	ret=Dc_log_Dec_Sel(g_pub_tx.reply, "dc_ind='2' and tx_tx_br_no='%s' and \
		sts='0' and acc_hrt[1]<'6' and tel='%s'",g_pub_tx.tx_br_no,
		g_pub_tx.tel); 
	if(ret) 
	{ 
		sprintf( acErrMsg, "Declare dc_log error!!" ); 
		WRITEMSG 
		goto ErrExit; 
	}

	while(1) 
	{ 
		ret=Dc_log_Fet_Sel(&sDc_log,g_pub_tx.reply); 
		if(ret==100)break; 
		else if(ret) 
		{ 
			sprintf( acErrMsg, "Fetch dc_log record error!!" ); 
			WRITEMSG 
			goto ErrExit; 
		} 
		if( sDc_log.ct_ind[0]=='1' ) tmp_amt4+=sDc_log.amt; 
		tmp_amt2+=sDc_log.amt; 
	} 
	Dc_log_Clo_Sel();

	/*����Ʒ�¼����Ƿ�ƽ��*/ 
	ret=pub_base_CompDblVal(tmp_amt1,tmp_amt2); 
	if(ret==1) 
	{ 
		sprintf(tmp_str,"%.2lf>%.2lf ���Ϊ%.2lf",tmp_amt1,tmp_amt2,tmp_amt1-tmp_amt2); 
		set_zd_data(DC_GET_MSG,tmp_str); 
		sprintf( acErrMsg, "���ս跽��������ڴ���������!!" ); 
		WRITEMSG 
		strcpy(g_pub_tx.reply,"O255"); 
		goto ErrExit; 
	} 
	else if(ret==-1) 
	{ 
		sprintf(tmp_str,"%.2lf>%.2lf,���Ϊ%.2lf",tmp_amt1,tmp_amt2,tmp_amt1-tmp_amt2); 
		set_zd_data(DC_GET_MSG,tmp_str); 
		sprintf( acErrMsg, "���ս跽������С�ڴ���������!!" ); 
		WRITEMSG 
		strcpy(g_pub_tx.reply,"O255"); 
		goto ErrExit; 
	}

	/*����Ʒ�¼����Ƿ�ƽ��*/ 
	ret=pub_base_CompDblVal(tmp_amt3,tmp_amt4); 
	if(ret==1) 
	{ 
		sprintf(tmp_str,"%.2lf>%.2lf,���Ϊ%.2lf",tmp_amt3,tmp_amt4,tmp_amt3-tmp_amt4); 
		set_zd_data(DC_GET_MSG,tmp_str); 
		sprintf( acErrMsg, "�����ֽ�跽��������ڴ���������!!" ); 
		WRITEMSG 
		strcpy(g_pub_tx.reply,"O256"); 
		goto ErrExit; 
	} 
	else if(ret==-1) 
	{ 
		sprintf(tmp_str,"%.2lf>%.2lf,���Ϊ%.2lf",tmp_amt3,tmp_amt4,tmp_amt3-tmp_amt4); 
		set_zd_data(DC_GET_MSG,tmp_str); 
		sprintf( acErrMsg, "�����ֽ�跽������С�ڴ���������!!" ); 
		WRITEMSG 
		strcpy(g_pub_tx.reply,"O256"); 
		goto ErrExit; 
	}
  
	set_zd_data("0670","0");				/*����ǩ��*/
	
OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    vtcp_log("[%s][%d]Before return: reply is[%s]",__FILE__,__LINE__,g_pub_tx.reply);
    return 1;
}
