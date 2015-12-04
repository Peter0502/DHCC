/*************************************************
* �� �� ��: sp9616.c
* ����������ǿ��ǩ�˹�Ա
*
* ��    ��: rob 
* ������ڣ�20030719
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "com_tel_c.h"
#include "com_tel_connect_c.h"
#include "com_parm_c.h"
#include "dc_log_c.h"
#include "tel_control_c.h"

int sp9616( )
{
	double d_amt=0.00, c_amt=0.00;
	int	ret=0;
	double tmp_amt1=0.00, tmp_amt2=0.00, tmp_amt3=0.00, tmp_amt4=0.00;
	char tmp_str[256];

	struct	com_tel_c				g_com_tel;
	struct	com_tel_connect_c		g_com_tel_connect;
	struct  com_parm_c sCom_parm;
	struct  dc_log_c sDc_log;
	struct tel_control_c sTel_control;

	memset(&g_com_tel,0x00,sizeof(struct com_tel_c));
	memset(&g_com_tel_connect,0x00,sizeof(struct com_tel_connect_c));
	memset(tmp_str, 0x0, sizeof(tmp_str));
	memset(&sDc_log, 0x0, sizeof(struct dc_log_c));
	memset(&sTel_control, 0x0, sizeof(struct tel_control_c));

	/* ��ʼ���ṹ */
	pub_base_sysinit();

	get_zd_data("0920", g_pub_tx.tel);

	sprintf(acErrMsg, "ǿ��ǩ�˹�Ա��Ϊ:[%s]", g_pub_tx.tel);
	WRITEMSG

	/*����Ա״̬*/
	ret=Com_tel_Sel(g_pub_tx.reply,&g_com_tel,"tel='%s'",
		g_pub_tx.tel);
	if(ret)
	{
		sprintf( acErrMsg, "��ѯ��Ա��Ϣ�����!!" );
		WRITEMSG
		goto ErrExit;
	}

	strcpy(g_pub_tx.tx_br_no, g_com_tel.br_no);
	strcpy(sTel_control.br_no, g_com_tel.br_no);
	strcpy(sTel_control.tel, g_com_tel.tel);
	sTel_control.tx_date = g_pub_tx.tx_date;
	sTel_control.sts[0] = '0';
	strcpy(sTel_control.msg, "�ù�Ա��������δǩ�˶�������!!");
	ret = Tel_control_Ins(sTel_control, g_pub_tx.reply);
	if ( ret )
	{
		sprintf(acErrMsg, "insert into tel_control error [%d]", ret);
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

	/* add by rob at 20030907 **/
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
	/* add by rob end **/

	/* ��ͬ�ǵĿ��� add by rob at 20030711 */
	ret = Com_parm_Sel(g_pub_tx.reply, &sCom_parm, "parm_code='TCKM' and \
			parm_seqn = 1");
	if (ret != 0 && ret != 100)
	{
		sprintf(acErrMsg, "��ѯ�������������!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	else if (ret == 100)
	{
		sprintf(acErrMsg, "����������û�иù��˿�Ŀ!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "O228");
		goto ErrExit;
	}

	/* �������ˮ 26102 ��Ŀ����Ƿ�Ϊ�� *
	**/

	ret = sql_sum_double("dc_log", "amt", &d_amt, "tx_date=%ld and tel='%s' \
		and sts='0' and dc_ind='1' and acc_hrt='%s'", g_pub_tx.tx_date, 
		g_pub_tx.tel, sCom_parm.val);
	if (ret != 0)
	{
		sprintf(acErrMsg, "ͳ��������ʿ�Ŀ�跽������!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	ret = sql_sum_double("dc_log", "amt", &c_amt, "tx_date=%ld and tel='%s' \
		and sts='0' and dc_ind='2' and acc_hrt='%s'", g_pub_tx.tx_date, 
		g_pub_tx.tel, sCom_parm.val);
	if (ret != 0)
	{
		sprintf(acErrMsg, "ͳ��������ʿ�Ŀ����������!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	if (pub_base_CompDblVal(d_amt, c_amt) != 0)
	{
		sprintf(acErrMsg, "ͬ��������ʿ�Ŀ��ƽ [%lf] [%lf]",d_amt, c_amt);
		WRITEMSG
		strcpy(g_pub_tx.reply, "O230");
		goto ErrExit;
	}

	ret = sql_count("mo_city", "tx_br_no='%s' and tel='%s' and \
		sts='2'", g_pub_tx.tx_br_no, g_pub_tx.tel);
	if (ret < 0)
	{
		sprintf(acErrMsg, "ͳ��ͬ�ǵǼǲ�����!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	else if (ret > 0)
	{
		sprintf(acErrMsg, "�ù�Ա���������ӡƱ��û�к���!! ");
		WRITEMSG
		set_zd_data("0130", "�ù�Ա���������ӡƱ��û�к���!!");
		strcpy(g_pub_tx.reply, "D258");
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

	/* add by rob at 20030820 */
	/* ���ù�Ա�Ļ����ˮ�Ƿ�ƽ�� */
	/*ȡ���ս跽����������*/ 
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
		sprintf(tmp_str,"%.2lf>%.2lf",tmp_amt1,tmp_amt2); 
		set_zd_data(DC_GET_MSG,tmp_str); 
		sprintf( acErrMsg, "���ս跽��������ڴ���������!!" ); 
		WRITEMSG 
		strcpy(g_pub_tx.reply,"O255"); 
		goto ErrExit; 
	} 
	else if(ret==-1) 
	{ 
		sprintf(tmp_str,"%.2lf<%.2lf",tmp_amt1,tmp_amt2); 
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
		sprintf(tmp_str,"%.2lf>%.2lf",tmp_amt3,tmp_amt4); 
		set_zd_data(DC_GET_MSG,tmp_str); 
		sprintf( acErrMsg, "�����ֽ�跽��������ڴ���������!!" ); 
		WRITEMSG 
		strcpy(g_pub_tx.reply,"O256"); 
		goto ErrExit; 
	} 
	else if(ret==-1) 
	{ 
		sprintf(tmp_str,"%.2lf<%.2lf",tmp_amt3,tmp_amt4);
		set_zd_data(DC_GET_MSG,tmp_str); 
		sprintf( acErrMsg, "�����ֽ�跽������С�ڴ���������!!" ); 
		WRITEMSG 
		strcpy(g_pub_tx.reply,"O256"); 
		goto ErrExit; 
	}


OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"ǿ��ǩ�˹�Ա�ɹ� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"ǿ��ǩ�˹�Աʧ�� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
