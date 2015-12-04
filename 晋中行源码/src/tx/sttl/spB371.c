/*************************************************
* �� �� ��:  spB371.c
* ���������� ��֤������ά�����ܡ�
*
* ��    ��:  xxx
* ������ڣ� 2003��11��2��
*
* �޸ļ�¼�� 
*   ��    ��:2011-8-10 13:58:07
*   �� �� ��:chenchao 
*   �޸�����: ���ӱ�֤���˺ź�Э��Ĺ���
*   ��    �ڣ�2011-8-22 
*   �� �� �ˣ�gongliangliang
*   �޸����ݣ������Ŵ��Ѿ�������У�飬���Ĳ���Ҫ���������������������ʵ���֤��ע��������֤����
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_bail_rate_c.h"
#include "mo_bank_acc_po_c.h"
#include "protect_note_c.h"
#include "com_parm_c.h"
#include "dd_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "mo_sg_cash_po_c.h"
#include "dd_parm_c.h"
int spB371()
{
	double	dRate=0.00;
	struct	mo_bail_rate_c	sMoBailRate;		/* ��֤�����ʵǼǱ� */
	struct	mo_bail_rate_c	sMoBailRate1;		/* ��֤�����ʵǼǱ� */
	struct	dd_mst_c	sDdMst,sDd_mst;			/** �������ļ� **/
	struct	mo_bank_acc_po_c 	sMo_bank_acc_po;/* �жһ�Ʊ������Ϣ�Ǽǲ� */
	struct	mo_sg_cash_po_c		sMoSgCashPo;
	struct	mdm_ac_rel_c	sMdmAcRel;		/** ���ʹ�ϵ  ***/
	struct  com_parm_c      sComParm;           /* �������� */
	char	czm[2],tmpstr[16];
	char	cOpenBrno[9];			/* �������ʺſ������� */
	int	ret;
	struct  dd_parm_c	sDd_parm;
	
	/* ���ݳ�ʼ�� */
	memset( &sDd_parm,0x00,sizeof(sDd_parm));
	memset( &sMoBailRate, 0x00, sizeof( struct mo_bail_rate_c ) );
	memset( &sMoBailRate1, 0x00, sizeof( struct mo_bail_rate_c ) );
	memset( &sMo_bank_acc_po, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );
	pub_base_sysinit();
	memset( &sDd_mst, 0x00, sizeof( sDd_mst ) );
	
	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0680", czm);				/* ������ */
	strcpy(sMoBailRate.br_no,g_pub_tx.tx_br_no);
	get_zd_data( "0670", sMoBailRate.bail_type );		/* ��֤������ */
	get_zd_data( "0330", sMoBailRate.pact_no );		/* Э���� */
	get_zd_data( "0320", sMoBailRate.bail_ac_no );		/* ��֤���ʺ� */
	get_zd_data( "0310", sMoBailRate.ac_no );		/* ��Ϣ�ʺ� */
	get_zd_double( "0390",&sMoBailRate.amt );		/* ��֤���� */
	get_zd_long( "0440", &sMoBailRate.beg_date );		/* ��Ϣ���� */
	get_zd_long( "0450", &sMoBailRate.end_date );		/* �������� */
	get_zd_double( "0840",&sMoBailRate.ic_rate );		/* ���� */
	sMoBailRate.in_trace_no = g_pub_tx.trace_no;		/* �Ǽ���ˮ�� */
	sMoBailRate.intst_date=0;				/* ��Ϣ���� */

	/** ���Ҿ��ʺ� **/
	pub_base_old_acno( sMoBailRate.ac_no );
	pub_base_old_acno( sMoBailRate.bail_ac_no );
	

	
	ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id in (select ac_id from mdm_ac_rel where ac_no='%s')",sMoBailRate.bail_ac_no);
	if( ret )
	{
		sprintf( acErrMsg, "ִ��Dd_mst_Sel��![%d][%s]",ret,sMoBailRate.bail_ac_no);
		WRITEMSG
		sprintf( g_pub_tx.reply, "B334" );
		goto ErrExit;
	}
	if(memcmp(sDd_mst.prdt_no,"131",3) && memcmp(sDd_mst.prdt_no,"142",3) )
	{
		sprintf( acErrMsg, "�Ǳ�֤���˻��������ô˽���![%d][%s]",ret,sMoBailRate.bail_ac_no);
		WRITEMSG
		sprintf( g_pub_tx.reply, "B334" );
		goto ErrExit;
	}
	if(sMoBailRate.bail_type[0] == '2')	/**  ������֤����ø��·ֻ�����ʽ** modify chenchao 2012/3/20 13:27:41 **/
	{
		if( memcmp(sDd_mst.prdt_no,"142",3) )
		{
			sprintf( acErrMsg, "�Ǳ�֤���˻��������ô˽���![%s]",sMoBailRate.bail_ac_no);
			WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
			goto ErrExit;
		}
		if(pub_base_CompDblVal(sDd_mst.intst_acm,0.00)>0)
		{
			sprintf( acErrMsg, "������Ϊ�㣬������������Ϣ![%lf]",sDd_mst.intst_acm);
			WRITEMSG
			sprintf( g_pub_tx.reply, "D147" );
			goto ErrExit;
		}
		ret = Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no='%s'",sDd_mst.prdt_no);
		if( ret )
		{
			sprintf( acErrMsg, "Dd_parm_Sel![%d][%s]",ret,sMoBailRate.bail_ac_no);
			WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
			goto ErrExit;
		}
		ret = pub_base_getllz(sDd_parm.rate_no,"01",g_pub_tx.tx_date,&dRate);
		if( ret )
		{
			sprintf( acErrMsg, "ȡ���ʴ���!" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "O195" );
			goto ErrExit;	
		}
		/** add by chenchao 2011-8-10 17:36:03 ���н����Ŵ����� **/
		ret = sql_execute("update ln_auth set sts='1' where pay_ac_no ='%s' and type='12' and sts='X' ",sMoBailRate.bail_ac_no);
		if(ret)
		{
			sprintf( acErrMsg, "����ˮ��־��" );
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data("0300",sMoBailRate.bail_ac_no);
		set_zd_data("0340","1");
		set_zd_double("0290",(sMoBailRate.ic_rate/dRate)-1); 
		strcpy( g_pub_tx.reply, "0001" );
		goto OkExit;
	}
	if( sMoBailRate.beg_date>=sMoBailRate.end_date )
	{
	   	sprintf( acErrMsg, "��Ϣ���ڲ��ܴ��ڵǼǵ�������!beg_date=[%ld],end_date[%ld]",sMoBailRate.beg_date,sMoBailRate.end_date);
		WRITEMSG
		sprintf( g_pub_tx.reply, "B350" );
		goto ErrExit;
	}
	/* �����Ϣ�ʺŵ���Ч�ԣ��������ʺ�id */
	ret = pub_base_ChkAc( sMoBailRate.ac_no, 1, cOpenBrno, &g_pub_tx.ac_id );
	if ( ret )
	{
		sprintf( acErrMsg, "��鸶���ʺ���Ч�Դ�" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B339" );
		goto ErrExit;	
	}
	ret=pub_base_ChkAc( sMoBailRate.bail_ac_no, 1, cOpenBrno, &g_pub_tx.ac_id );
	if ( ret )
	{
		sprintf( acErrMsg, "��鸶���ʺ���Ч�Դ�" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B339" );
		goto ErrExit;	
	}

	vtcp_log("��֤�𿪻�����:=[%s]",cOpenBrno);
	/* ��������Ƿ񳬹��޶� */
	ret = Com_parm_Sel(g_pub_tx.reply,&sComParm,"parm_code='BZJLL' and parm_seqn=1 ");
	if( ret )
	{
		sprintf( acErrMsg, "ȡ�������������!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "G001" );
		goto ErrExit;	
	}
	ret = pub_base_getllz(sComParm.val,"01",g_pub_tx.tx_date,&sMoBailRate1.ic_rate);
	if( ret )
	{
		sprintf( acErrMsg, "ȡ���ʴ���!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "O195" );
		goto ErrExit;	
	}
	sMoBailRate.ic_rate=sMoBailRate.ic_rate * L100;
	sMoBailRate1.ic_rate=sMoBailRate1.ic_rate * L100;
	/*******edit by gongliangliang 20110822 ���������Ŵ�
	if( sMoBailRate.ic_rate<-0.0005 )
	{
		sprintf( acErrMsg, "��ǰ���ʵ�����͸�������!!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "A005" );
		goto ErrExit;	
	}
	if( pub_base_CompDblVal(sMoBailRate.ic_rate,sMoBailRate1.ic_rate)>0 )
	{
		sprintf( acErrMsg, "��ǰ���ʸ�����߸�������!!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "A006" );
		goto ErrExit;	
	}
	*********end by gongliangliang 20110822 ���������Ŵ�**********/
	sMoBailRate.ic_rate=sMoBailRate.ic_rate / L100;
	sMoBailRate1.ic_rate=sMoBailRate1.ic_rate / L100;

	if( czm[0]=='0' )			/** �Ǽ� **/
	{
		/** add by chenchao 2011-8-10 13:20:15 �����Ϣ������ά�������  ***/
		/* ��鱣֤���ʺŵ���Ч��:�����˺š��˻�״̬���� */
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",sMoBailRate.bail_ac_no );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "��֤���˺Ų�����![%s]", sMoBailRate.bail_ac_no );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B195" );
			goto ErrExit;
		}else if( ret )
		{
			sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel����![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		
		ret = Dd_mst_Sel( g_pub_tx.reply, &sDdMst, "ac_id=%ld and ac_seqn=%d",sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "����ı�֤���˺ŷǻ����˺�!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B196" );
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "ִ��Dd_mst_Sel����![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		if( sDdMst.ac_sts[0] != '1' )
		{
			sprintf( acErrMsg, "�˱�֤���˺�״̬������!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B197" );
			goto ErrExit;
		}
		if(sMoBailRate.bail_type[0] == '1')
		{
			/* ���ݳж�Э���Ų������гжһ�Ʊ������Ϣ�ǼǱ� */
			ret = Mo_bank_acc_po_Sel( g_pub_tx.reply, &sMo_bank_acc_po, "pact_no='%s'", sMoBailRate.pact_no );
			if( ret == 0 )
			{
				sprintf( acErrMsg, "�˳ж�Э�����Ѵ���!" );
				WRITEMSG
				sprintf( g_pub_tx.reply, "B191" );
				goto ErrExit;
			}else if( ret != 100 && ret )
			{
				sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Sel��![%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
			memcpy(sMo_bank_acc_po.pact_no,sMoBailRate.pact_no,sizeof(sMo_bank_acc_po.pact_no)-1);
			/* ���ɴ���Ǽǵ����ҲֵǼǱ����Ѵ汣֤���ܽ��һ���� */
			ret = Mo_sg_cash_po_Sel( g_pub_tx.reply, &sMoSgCashPo, "pact_no='%s'", sMo_bank_acc_po.pact_no );
			if( ret == 0 )
			{
				sprintf( acErrMsg, "�˳ж�Э�����Ѵ���!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B191" );
				goto ErrExit;
			}else if( ret !=0 && ret != 100 )
			{
				sprintf( acErrMsg, "ִ��Mo_sg_cash_po_Sel��![%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
			/* �ǼǱ��ҲֵǼǲ� */
			strcpy( sMoSgCashPo.pact_no, sMo_bank_acc_po.pact_no );/* �ж�Э���� */
			strcpy( sMoSgCashPo.br_no, g_pub_tx.tx_br_no );		/* ���׻��� */
			sMoSgCashPo.safeg_sum = sMoBailRate.amt;		/* �Ѵ汣֤���ܽ�� */
			sMoSgCashPo.pickup_sum = 0.00;
			
			ret = Mo_sg_cash_po_Ins( sMoSgCashPo, g_pub_tx.reply );
			if ( ret )
			{
				Mo_sg_cash_po_Debug(&sMoSgCashPo);
				sprintf( acErrMsg, "ִ��Mo_sg_cash_po_Ins����!ret=[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
			/* �Ǽǻ�Ʊ�ǼǱ� */
			strcpy( sMo_bank_acc_po.bail_ac_no, sMoBailRate.bail_ac_no );	/* ��֤���ʺ� */
			sMo_bank_acc_po.bail_amt = sMoBailRate.amt;		/* ��֤���� */
			strcpy( sMo_bank_acc_po.acc_po_sts, "A" );       	/* �ж�Э��״̬��A �汣֤�� */
			sMo_bank_acc_po.req_date = g_pub_tx.tx_date;		/* ��ʱ��¼�汣֤������ */
			
			ret = pub_sttl_RegMoBankAccPo( sMo_bank_acc_po );
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��pub_sttl_RegMoBankAccPo����!ret=[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
		}
		/*** end add by chenchao 2011-8-10 13:20:54 ****/
		
		strcpy( sMoBailRate.sts, "1" );						/* ����״̬0¼��*/
		sMoBailRate.end_trace_no=0;
		sMoBailRate.intst_amt=0;
		sMoBailRate.seq=0;
		if( sMoBailRate.bail_type[0]=='1' )					/** �жұ�֤�� **/
		{
			strcpy(g_pub_tx.brf,"�Ǽǳжұ�֤������");
			ret=sql_count("mo_bail_rate","pact_no='%s' and bail_type='1' and sts='1' ", sMoBailRate.pact_no );
		}
		else											/** ������֤�� **/
		{
			strcpy(g_pub_tx.brf,"�Ǽǵ�����֤������");
			ret=sql_count("mo_bail_rate","pact_no='%s' and bail_type='2' and sts='1' ", sMoBailRate.pact_no );
		}
		if( ret )
	   	{
	   		sprintf( acErrMsg, "��Э�����Ѿ��Ǽ�![%d][%s]",ret,sMoBailRate.pact_no );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B333" );
			goto ErrExit;
		}
		ret=Mo_bail_rate_Ins(sMoBailRate,g_pub_tx.reply);
		if( ret )
	   	{
	   		sprintf( acErrMsg, "�ǼǱ�֤�����ʵǼǱ�����![%d][%s]",ret,sMoBailRate.pact_no );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
			goto ErrExit;
		}
	}
	else if( czm[0]=='1' || czm[0]=='2' )		/** �޸�,ɾ�� **/ 
	{
		if( sMoBailRate.bail_type[0]=='1' )				/** �жұ�֤�� **/
		{
			strcpy(tmpstr,"�жұ�֤������");
			ret=Mo_bail_rate_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and bail_type='1' and sts='1' ", sMoBailRate.pact_no );
		}
		else
		{
			strcpy(tmpstr,"������֤������");
			ret=Mo_bail_rate_Dec_Upd( g_pub_tx.reply,"pact_no='%s' and bail_type='2' and sts='1' ", sMoBailRate.pact_no );
		}
		if( ret )
	   	{
	   		sprintf( acErrMsg, "�ǼǱ�֤�����ʵǼǱ�����![%d][%s]",ret,sMoBailRate.pact_no );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
			goto ErrExit;
		}
		ret=Mo_bail_rate_Fet_Upd(&sMoBailRate,g_pub_tx.reply);
		if( ret )
	   	{
	   		sprintf( acErrMsg, "�ǼǱ�֤�����ʵǼǱ�����![%d][%s]",ret,sMoBailRate.pact_no );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
			goto ErrExit;
		}
		if( sMoBailRate.sts[0]!='1' )
	   	{
	   		sprintf( acErrMsg, "�ǼǱ�֤�����ʵǼǱ�״̬������![%s]",sMoBailRate.sts );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B335" );
			goto ErrExit;
		}

		if( czm[0]=='1' )
		{
			strcpy(sMoBailRate.br_no,g_pub_tx.tx_br_no);
			get_zd_data( "0670", sMoBailRate.bail_type );		/* ��֤������ */
			get_zd_data( "0330", sMoBailRate.pact_no );		/* Э���� */
			get_zd_data( "0320", sMoBailRate.bail_ac_no );		/* ��֤���ʺ� */
			get_zd_data( "0310", sMoBailRate.ac_no );		/* ��Ϣ�ʺ� */
			get_zd_double( "0390",&sMoBailRate.amt );		/* ��֤���� */
			get_zd_long( "0440", &sMoBailRate.beg_date );		/* ��Ϣ���� */
			get_zd_long( "0450", &sMoBailRate.end_date );		/* �������� */
			get_zd_double( "0840",&sMoBailRate.ic_rate );		/* ���� */
			sMoBailRate.in_trace_no = g_pub_tx.trace_no;		/* �Ǽ���ˮ�� */
			strcpy(g_pub_tx.brf,"�޸�");
			ret=Mo_bail_rate_Upd_Upd(sMoBailRate,g_pub_tx.reply);
			if( ret )
	   		{
	   			sprintf( acErrMsg, "�ǼǱ�֤�����ʵǼǱ�����![%d]",ret );
				WRITEMSG
				sprintf( g_pub_tx.reply, "B334" );
				goto ErrExit;
			}
			ret = sql_execute("update mo_bank_acc_po set bail_amt='%lf',bail_amt_ratio=bail_amt*100/sign_amt where pact_no='%s' ",sMoBailRate.amt,sMoBailRate.pact_no);
			if(ret)
			{
				sprintf( acErrMsg, "�޸ĳж�Э������![%d]",ret );
				WRITEMSG
				sprintf( g_pub_tx.reply, "B334" );
				goto ErrExit;
			}
			ret = sql_execute("update mo_sg_cash_po set safeg_sum=%lf where pact_no='%s' ",sMoBailRate.pact_no,sMoBailRate.amt);
			if(ret)
			{
				sprintf( acErrMsg, "���ҲֵǼǱ�����![%d]",ret );
				WRITEMSG
				sprintf( g_pub_tx.reply, "B334" );
				goto ErrExit;
			}
		}
		else
		{
			strcpy(g_pub_tx.brf,"ɾ��");
			ret=Mo_bail_rate_Del_Upd(sMoBailRate,g_pub_tx.reply);
			if( ret )
	   		{
	   			sprintf( acErrMsg, "�ǼǱ�֤�����ʵǼǱ�����![%d]",ret );
				WRITEMSG
				sprintf( g_pub_tx.reply, "B334" );
				goto ErrExit;
			}
			if(sMoBailRate.bail_type[0]=='1')
			{
				ret = sql_execute("update  mo_bank_acc_po set acc_po_sts='C',bail_amt=0,bail_amt_ratio=0 where pact_no='%s' and acc_po_sts in ('A','0','1','2')",sMoBailRate.pact_no);/** ��Ϊɾ��״̬ **/
				if(ret)
				{
					sprintf( acErrMsg, "ɾ���ж�Э������![%d]",ret );
					WRITEMSG
					sprintf( g_pub_tx.reply, "B334" );
					goto ErrExit;
				}
				ret = sql_execute("delete from mo_sg_cash_po where pact_no='%s' ",sMoBailRate.pact_no);
				if(ret)
				{
					sprintf( acErrMsg, "���ҲֵǼǱ�����![%d]",ret );
					WRITEMSG
					sprintf( g_pub_tx.reply, "B334" );
					goto ErrExit;
				}
			}
		}
		strcat(g_pub_tx.brf,tmpstr);
		Mo_bail_rate_Clo_Upd();
	}
	else
	{
		sprintf( acErrMsg, "���������ʹ���!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B332" );
		goto ErrExit;
	}
	
	/* ����ˮ��־ */
	strcpy(g_pub_tx.ac_no,sMoBailRate.ac_no);
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}
	/** add by chenchao 2011-8-10 17:36:03 ���н����Ŵ����� **/
	ret = sql_execute("update ln_auth set sts='1' where pact_no='%s' and type='12' and sts='X' ",sMoBailRate.pact_no);
	if(ret)
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}
	strcpy( g_pub_tx.reply, "0000" );
OkExit:
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4) == 0)
		strcpy( g_pub_tx.reply, "H034" );
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
