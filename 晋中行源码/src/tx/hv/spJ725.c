/***************************************************************
* �� �� ��: spJ725.c
* ������������Ʊȫ��Ҹ�֪ͨ
*			����ǩ���Ǽǲ��е���Ϣ
*
* ��    ��: jane
* ������ڣ�2004��07��01��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "hv_pub.h"
#include "hv_define.h"
#include "com_tel_c.h"
#include "find_debug.h"

spJ725()
{
	struct mbfebillopnreg_c billopnreg_v;
	struct mbfebrbkno_c brbkno_v;
	struct com_tel_c sComTel;

	memset( &billopnreg_v, 0x00, sizeof( struct mbfebillopnreg_c ) );

	/*ǩ���Ǽǲ�����*/
	billopnreg_v.amount = atof( mbfetran.remittanceamnt ) / 100;    /*��Ʊ���*/
	pub_base_strpack( mbfetran.payeracc );
	pub_base_old_acno( mbfetran.payeracc );
	g_reply_int = Mbfebillopnreg_Dec_Upd( g_pub_tx.reply, "billsts='2' and payeropenaccbkcode='%s' and signdate='%s' and billcode='%s' and billofxchngseal='%s' and payeracc='%s' and amount='%lf'", mbfetran.payeropenaccbkcode, mbfetran.warrantdate, mbfetran.warrantno, mbfetran.billofxchngseal, mbfetran.payeracc, billopnreg_v.amount );
	if( g_reply_int )
	{
		sprintf(acErrMsg,"��Ʊȫ��Ҹ�֪ͨ���ǩ���Ǽǲ������α����ݿ����ʧ��[%d]", g_reply_int);
		WRITEMSG
		goto ErrExit;
	}
	g_reply_int = Mbfebillopnreg_Fet_Upd( &billopnreg_v, g_pub_tx.reply );
	if( g_reply_int == 100 )
	{
		strcpy( mbfetran.procstate,"5E" );
		sprintf(acErrMsg,"��Ʊȫ��Ҹ�֪ͨ�Ǳ���ǩ������δ������Ʊ");
		WRITEMSG
		Mbfebillopnreg_Clo_Upd();
		strcpy( mbfetran.prilevel, "1" );    /*��������״̬0:�ɹ�����1:������*/
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf(acErrMsg,"��Ʊȫ��Ҹ�֪ͨ���ǩ���Ǽǲ�ȡ�α����ݿ����ʧ��[%d]", g_reply_int);
		WRITEMSG
		goto ErrExit;
	}

	strcpy( mbfetran.prilevel, "0" );    /*��������״̬0:�ɹ�����1:������*/

	strcpy( billopnreg_v.billsts, "*" );    /*��Ʊ״̬*:����*/
	strcpy( billopnreg_v.useind, "2" );    /*����״̬3:ȫ��Ҹ�*/
	billopnreg_v.canceldate = g_pub_tx.tx_date;    /*��������*/
	billopnreg_v.cancelflowno = g_pub_tx.trace_no;    /*������ˮ��*/
	strcpy( billopnreg_v.cancel_tel, g_pub_tx.tel );    /*��������Ա*/
	billopnreg_v.prmptpaydate = atol( mbfetran.notepaydate );    /*��ʾ��������*/
	billopnreg_v.repudiationamnt = atof( mbfetran.factclramnt ) / 100;    /*ʵ�ʽ�����*/
	billopnreg_v.redundantamnt = atof( mbfetran.amount ) / 100;    /*������*/
	strcpy( billopnreg_v.recipientopenaccbk, mbfetran.recipientopenaccbk );    /*����Ʊ�˿����к�*/
	strcpy( billopnreg_v.lastbillholderacc, mbfetran.lastbillholderacc );    /*����Ʊ���˺�*/
	strcpy( billopnreg_v.lastbillholdername, mbfetran.lastbillholdername );    /*����Ʊ������*/
	/*�Զ���Ҵ���*/
	if( billopnreg_v.los_rpt_ind[0] == '1' )
	{
		strcpy( billopnreg_v.los_rpt_ind, "2" );    /*��ʧ��־2:���*/
		billopnreg_v.clos_rpt_date = g_pub_tx.tx_date;    /*�������*/
		strcpy( billopnreg_v.clos_rpt_tel, g_pub_tx.tel );    /*��Ҳ���Ա*/
	}

	g_reply_int = Mbfebillopnreg_Upd_Upd( billopnreg_v, g_pub_tx.reply );
	if( g_reply_int )
	{
		sprintf(acErrMsg,"��Ʊȫ��Ҹ�֪ͨ���ǩ���Ǽǲ�ִ���α����ݿ����ʧ��[%d]", g_reply_int);
		WRITEMSG
		goto ErrExit;
	}

	Mbfebillopnreg_Clo_Upd();

	if( pub_mbfe_bmreg() ) goto ErrExit;

	if( mbfetran.prilevel[0] == '0' )
	{
		/*�����ʽ��׻����źͲ���Ա��*/
		memset( &brbkno_v, 0x00, sizeof( struct mbfebrbkno_c ) );
		g_reply_int = Mbfebrbkno_Sel( g_pub_tx.reply, &brbkno_v, "bkno='%s'", mbfetran.payeropenaccbkcode );
		if( g_reply_int )
		{
			sprintf(acErrMsg,"��ѯ���ʽ����ж�Ӧ���������ݿ����ʧ��[%d]",g_reply_int);
			WRITEMSG
			return -1;
		}
		strcpy( g_pub_tx.tx_br_no, brbkno_v.brno );
		set_zd_data( "0030", g_pub_tx.tx_br_no );
		memset( &sComTel, 0x00, sizeof( struct com_tel_c ) );
		g_reply_int = Com_tel_Sel( g_pub_tx.reply, &sComTel, "br_no='%s' and lvl='4'", brbkno_v.brno );
		if( g_reply_int )
		{
			sprintf(acErrMsg,"��ѯ���ʽ����ж�Ӧ���������ݿ����ʧ��[%d]",g_reply_int);
			WRITEMSG
			return -1;
		}
		strcpy( g_pub_tx.tel, sComTel.tel );
		set_zd_data( "0070", g_pub_tx.tel );

		/*���㿪����Ʊǩ�������ƴ滧*/
		g_reply_int = set_bill_opnrmv( billopnreg_v.amount );
		if( g_reply_int )
		{
			sprintf(acErrMsg,"���㿪����Ʊǩ�������ƴ滧����[%d]", g_reply_int);
			WRITEMSG
			goto ErrExit;
		}
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���»�Ʊǩ���Ǽǲ��ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"���»�Ʊǩ���Ǽǲ�ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

/************************************************
*     �Ǽǻ�Ʊȫ��Ҹ�֪ͨ��                    *
************************************************/
int pub_mbfe_bmreg()
{
	struct mbfebillmsgreg_c mbfebillmsgreg_v;

	memset( &mbfebillmsgreg_v, 0x00, sizeof( struct mbfebillmsgreg_c ) );

	if( get_serialno_val( "13", mbfebillmsgreg_v.msssno ) );
	strcpy( mbfebillmsgreg_v.sts, mbfetran.prilevel );
	mbfebillmsgreg_v.consigndate=atol(mbfetran.consigndate);
	strcpy( mbfebillmsgreg_v.rdficode, mbfetran.rdficode );
	strcpy( mbfebillmsgreg_v.payeropenaccbkcode, mbfetran.payeropenaccbkcode );
	strcpy( mbfebillmsgreg_v.payeracc, mbfetran.payeracc );
	strcpy( mbfebillmsgreg_v.payername, mbfetran.payername );
	strcpy( mbfebillmsgreg_v.payeraddr, mbfetran.payeraddr );
	mbfebillmsgreg_v.billdate = atol( mbfetran.warrantdate );
	strcpy( mbfebillmsgreg_v.billcode, mbfetran.warrantno );
	strcpy( mbfebillmsgreg_v.billofxchngseal, mbfetran.billofxchngseal );
	mbfebillmsgreg_v.remittanceamnt = atof( mbfetran.remittanceamnt )/100;
	mbfebillmsgreg_v.repudiationamnt = atof( mbfetran.factclramnt )/100;
	strcpy( mbfebillmsgreg_v.rdficenter, mbfetran.rdficenter );
	strcpy( mbfebillmsgreg_v.recipientopenaccbk, mbfetran.recipientopenaccbk );
	strcpy( mbfebillmsgreg_v.lastbillholderacc, mbfetran.lastbillholderacc );
	strcpy( mbfebillmsgreg_v.lastbillholdername, mbfetran.lastbillholdername );
	mbfebillmsgreg_v.prmptpaydate = atol( mbfetran.notepaydate );
	strcpy( mbfebillmsgreg_v.strinfo, mbfetran.strinfo );

	sprintf( acErrMsg, "mbfebillmsgreg: 1n[%s] 2n[%ld] 3n[%s] 4n[%s] 5n[%s] 6n[%s] 7n[%s] 8n[%ld] 9[%s] 10[%s] 11n[%f] 12[%f] 13n[%s] 14[%s] 15n[%s] 16[%s] 17[%ld] 18[%s]", mbfebillmsgreg_v.msssno, mbfebillmsgreg_v.consigndate, mbfebillmsgreg_v.rdficode, mbfebillmsgreg_v.payeropenaccbkcode, mbfebillmsgreg_v.payeracc, mbfebillmsgreg_v.payername, mbfebillmsgreg_v.payeraddr, mbfebillmsgreg_v.billdate, mbfebillmsgreg_v.billcode, mbfebillmsgreg_v.billofxchngseal, mbfebillmsgreg_v.remittanceamnt, mbfebillmsgreg_v.repudiationamnt, mbfebillmsgreg_v.rdficenter, mbfebillmsgreg_v.recipientopenaccbk, mbfebillmsgreg_v.lastbillholderacc, mbfebillmsgreg_v.lastbillholdername, mbfebillmsgreg_v.prmptpaydate, mbfebillmsgreg_v.strinfo );
	WRITEMSG

	g_reply_int=Mbfebillmsgreg_Ins(mbfebillmsgreg_v,g_pub_tx.reply);
	if ( g_reply_int )
	{
		sprintf(acErrMsg,"�Ǽǻ�Ʊȫ��Ҹ�֪ͨ�����ݿ����ʧ��[%d]",g_reply_int);
		WRITEMSG
		return -1;
	}

	return 0;
}
