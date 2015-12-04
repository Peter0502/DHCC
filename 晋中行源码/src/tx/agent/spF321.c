/*************************************************
* �� �� ��: spF321.c
* �����������ǼǱ����Ǽǲ�
*
* ��    ��: rob
* �������: 2004��08��13��
* �޸�����:
* �� �� ��:
* �޸�����:
*
*************************************************/
#define EXTERN
#include "public.h"
#include "protect_note_c.h"

spF321()  
{ 	
	struct	protect_note_c pro_note;
	struct	protect_note_c pro_note1;
	int	ret=0;

	memset( &pro_note, 0x00, sizeof(struct protect_note_c));
	memset( &pro_note1, 0x00, sizeof(struct protect_note_c));
sprintf(acErrMsg,"�����Ǽǿ�ʼ !" );
WRITEMSG
	pub_base_sysinit();
	pro_note.trace_no=g_pub_tx.trace_no;
	pro_note.tx_date=g_pub_tx.tx_date;
	strcpy(pro_note.tx_br_no,g_pub_tx.tx_br_no);
	strcpy(pro_note.tel,g_pub_tx.tel);

	get_zd_data  ("0300", pro_note.pro_ac_no);
	get_zd_long  ("0280",&pro_note.pro_cif_no);
	get_zd_data  ("0310", pro_note.ben_ac_no);
	get_zd_long  ("0290",&pro_note.ben_cif_no);
	get_zd_data  ("0320", pro_note.bail_ac_no);
	get_zd_data  ("0330", pro_note.pact_no);
	get_zd_long  ("0440",&pro_note.opn_date);
	get_zd_long  ("0480",&pro_note.time_limt);
	get_zd_long  ("0450",&pro_note.end_date);
	get_zd_double("0390",&pro_note.sum_amt);
	get_zd_double("0400",&pro_note.amt);
	get_zd_double("0410",&pro_note.bail_amt);
	get_zd_double("0850",&pro_note.ratio);
	get_zd_data  ("0220", pro_note.mort_typ);
	get_zd_data  ("0370", pro_note.mort_ac_no);
	get_zd_data  ("0961", pro_note.pro_name);
	get_zd_data  ("0962", pro_note.pro_addr);
	get_zd_data  ("0963", pro_note.ben_name);
	get_zd_data  ("0964", pro_note.ben_addr);
	get_zd_double("0420",&pro_note.amends_amt);
	get_zd_double("0840",&pro_note.over_rate);
	get_zd_data  ("0670", pro_note.protect_typ);
	vtcp_log(" ratio = [%f] ratio=[%f]",pro_note.ratio,g_pub_tx.rate);
	pro_note.protect_sts[0]='0';
	strcpy(g_pub_tx.ac_no,pro_note.pro_ac_no);
	g_pub_tx.cif_no=pro_note.pro_cif_no;
	strcpy(g_pub_tx.beg_note_no,pro_note.pact_no);
	g_pub_tx.tx_amt1=pro_note.sum_amt;
	g_pub_tx.tx_amt2=pro_note.amt;
	g_pub_tx.tx_amt3=pro_note.bail_amt;
	g_pub_tx.tx_amt4=pro_note.amends_amt;
	g_pub_tx.rate=pro_note.ratio;

	strcpy(g_pub_tx.brf, "���߱���");
	if (pub_ins_trace_log())
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!!=[%s][%d]",pro_note.pact_no,ret);
		WRITEMSG
		/**
		strcpy(g_pub_tx.reply,"");
		**/
		goto ErrExit;
	}

	/** ��鱣֤���ʺţ�����Ƿ���ȷ **/
	ret = Protect_note_Dec_Upd(g_pub_tx.reply,"pact_no='%s' ",pro_note.pact_no); 
	if ( ret )
	{
		sprintf(acErrMsg,"ȡ�����ǼǱ����� [%d]",ret );
		WRITEMSG
		goto ErrExit;
	}
	ret = Protect_note_Fet_Upd(&pro_note1,g_pub_tx.reply);
	if ( ret==100 )	
	{
		sprintf(acErrMsg,"������6305���״��뱣֤��! [%d]",ret );
		WRITEMSG
		strcpy(g_pub_tx.reply,"B352");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"ȡ�����ǼǱ����� [%d]",ret );
		WRITEMSG
		goto ErrExit;
	}

	if( pro_note1.protect_sts[0]!='A' )
	{
		sprintf(acErrMsg,"�����ǼǱ�״̬�Ǵ��뱣֤��״̬! [%s]",pro_note1.protect_sts );
		WRITEMSG
		strcpy(g_pub_tx.reply,"B349");
		goto ErrExit;
	}
	pub_base_strpack(pro_note1.bail_ac_no);
	if( strcmp(pro_note1.bail_ac_no,pro_note.bail_ac_no) )
	{
		sprintf( acErrMsg, "��֤���ʺŴ���![%s][%s]",pro_note1.bail_ac_no,pro_note.bail_ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"B258");
		goto ErrExit;
	}
	if( pub_base_CompDblVal(pro_note1.bail_amt,pro_note.bail_amt)!=0 )
	{
		sprintf( acErrMsg, "��֤�������![%lf][%lf]",pro_note1.bail_amt,pro_note.bail_amt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"B330");
		goto ErrExit;
	}

	/** ������һ��Ϣ�� **/
	ret = pub_base_deadlineM( pro_note.opn_date, 3 ,&pro_note.nex_date );
	if ( ret )
	{
		sprintf(acErrMsg,"net_date error [%d] [%d]",pro_note.opn_date,ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Protect_note_Upd_Upd(pro_note,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"�޸ı����ǼǱ�����![%d]",ret );
		WRITEMSG
		goto ErrExit;
	}
	Protect_note_Clo_Upd();

	/***
	ret = Protect_note_Ins( pro_note , g_pub_tx.reply );
	if ( ret )
	{
		sprintf(acErrMsg,"insert into protect_note error [%d]",ret );
		WRITEMSG
		goto ErrExit;
	}
	***/
	vtcp_log(" ratio = [%f] ratio=[%f]",pro_note.ratio,g_pub_tx.rate);

	get_zd_double("0430",&g_pub_tx.tx_amt1);			/**������**/
	g_pub_tx.tx_amt2 = pro_note.amt*pro_note.ratio/100;/** ������ = �������*�����ѱ��� **/
	set_zd_double("1001",0.00);
	set_zd_data("1011",pro_note.bail_ac_no);
	set_zd_int ("1012",1);
	set_zd_data("1016","�� ������֤���ʻ�");
	set_zd_data("101A","01");
	set_zd_data("101B","2");				/*ת��*/
    set_zd_double("1013",pro_note.bail_amt);


OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�ǼǱ�������OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"�ǼǱ���ErrExit!!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
