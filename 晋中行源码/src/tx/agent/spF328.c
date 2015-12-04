/*************************************************
* �� �� ��: spF328.c
* ���������������⳥����
*
* ��    ��: mike
* �������: 2004��08��16��
* �޸�����:
* �� �� ��:
* �޸�����:
*
*************************************************/
#define EXTERN
#include "public.h"
#include "protect_note_c.h"
#include "mo_bail_rate_c.h"

spF328()  
{ 	
	double	tx_amt1,tx_amt2,tx_amt3,tx_amt4;
	struct	protect_note_c pro_note;
	int	ret=0;
	int	flag;
	char	ys_ac_no[24];
	struct	mo_bail_rate_c		sMoBailRate;	/* ��֤�����ʵǼǱ� */
	struct	mo_bail_rate_c		sMoBailRate1;	/* ��֤�����ʵǼǱ� */
	long     day=0;
	long     HQday=0;
	double	HQ_rate=0.00;
	double	HQdealval=0.00;
	char	intst_flag[2];

	/** ���ݳ�ʼ�� **/
	flag=1;
	tx_amt1=0.00;
	tx_amt2=0.00;
	tx_amt3=0.00;
	tx_amt4=0.00;
	memset( &pro_note, 0x00, sizeof(struct protect_note_c));
	memset( ys_ac_no , 0x00, sizeof( ys_ac_no ));
	memset( &sMoBailRate, 0x00, sizeof( struct mo_bail_rate_c ) );
	memset( &sMoBailRate1, 0x00, sizeof( struct mo_bail_rate_c ) );
	memset( &g_pub_intst,0x00,sizeof(struct S_pub_intst) );

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
	get_zd_data  ("1231", ys_ac_no);
	get_zd_double("0840",&pro_note.over_rate);
	strcpy(g_pub_tx.ac_no,pro_note.pro_ac_no);
	g_pub_tx.cif_no=pro_note.pro_cif_no;
	strcpy(g_pub_tx.beg_note_no,pro_note.pact_no);
	g_pub_tx.tx_amt1=pro_note.sum_amt;
	g_pub_tx.tx_amt2=pro_note.amt;
	g_pub_tx.tx_amt3=pro_note.bail_amt;
	g_pub_tx.tx_amt4=pro_note.amends_amt;
	g_pub_tx.rate=pro_note.ratio;
	strcpy(g_pub_tx.brf, "�����⳥");
vtcp_log("RATIO ratio = [%f] ratio=[%f][%s->%d]",pro_note.ratio,g_pub_tx.rate,__FILE__,__LINE__);

	/**---- hao ---- 20051113 ----- ���ӵ�����֤���Ϣ ---**/
	strcpy(intst_flag,"0");
	if( pub_base_CompDblVal(pro_note.bail_amt,0.00)>0 )
	{
	   ret=Mo_bail_rate_Dec_Upd(g_pub_tx.reply,"bail_type='2' and pact_no='%s' and sts='1'",pro_note.pact_no);
   		if( ret )
   		{
   			sprintf( acErrMsg, "ִ��Mo_bail_rate_Dec_Upd��![%d][%s]",ret,pro_note.pact_no);
        	WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
        	goto ErrExit;
   		}
		ret = Mo_bail_rate_Fet_Upd(&sMoBailRate,g_pub_tx.reply);
		if( ret==100 )	
		{
			g_pub_intst.dealval=0.00;
			g_pub_intst.tax_intst=0.00;					/** Ӧ˰��Ϣ **/
			g_pub_intst.factval=0.00;					/** ʵ����Ϣ **/
			g_pub_intst.val=0.00;						/** ��Ϣ˰ **/
			g_pub_intst.keepval=0.00;					/** ��ֵ��Ϣ **/
			set_zd_data("0153","52101");                /** ��֤����Ϣ **/
			set_zd_double("102P",g_pub_intst.dealval);
			strcpy(intst_flag,"0");
		}
   		else if( ret )
   		{
   			sprintf( acErrMsg, "ִ��Mo_bail_rate_Fet_Upd��![%d][%s]",ret,pro_note.pact_no);
        	WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
        	goto ErrExit;
   		}
		else
		{
			strcpy(intst_flag,"1");
			memcpy( &sMoBailRate1, &sMoBailRate, sizeof( struct mo_bail_rate_c ) );
			ret = pub_base_getllz(HQLLBH,"01",g_pub_tx.tx_date,&HQ_rate);
			if( ret )
			{
				sprintf( acErrMsg, "ȡ���ʴ���!" );
				WRITEMSG
				sprintf( g_pub_tx.reply, "O195" );
				goto ErrExit;	
			}

			/** �ж��Ƿ��� **/
			if( g_pub_tx.tx_date<sMoBailRate.end_date ) 	/** ��ǰ֧ȡ **/
			{
				/** �������� **/
				if( pub_base_CalDays(sMoBailRate.beg_date,g_pub_tx.tx_date,"0",&HQday) )
   				{
   					sprintf( acErrMsg, "������������!");
       				WRITEMSG
       				goto ErrExit;
   				}
				day=0;
			}
			else
			{
				/** �������� **/
				if( pub_base_CalDays(sMoBailRate.beg_date,sMoBailRate.end_date,"0",&day) )
   				{
   					sprintf( acErrMsg, "������������!");
       				WRITEMSG
       				goto ErrExit;
   				}
				if( pub_base_CalDays(sMoBailRate.end_date,g_pub_tx.tx_date,"0",&HQday) )
   				{
   					sprintf( acErrMsg, "������������!");
       				WRITEMSG
       				goto ErrExit;
   				}
   			}

			sprintf(acErrMsg,"HAO --- look --- HQday=[%d],day=[%d]",HQday,day);
			/** ������Ϣ **/
			if( pub_base_CompDblVal(pro_note.bail_amt,sMoBailRate.amt) > 0 )
			{
       			HQdealval = HQ_rate * sMoBailRate.amt * HQday / L360 / L100;
       			g_pub_intst.dealval = sMoBailRate.ic_rate * sMoBailRate.amt * day / L360 / L100;
				g_pub_intst.dealval = g_pub_intst.dealval + HQdealval;
				sMoBailRate1.amt=0.00;
			}
			else
			{
       			HQdealval = HQ_rate * pro_note.bail_amt * HQday / L360 / L100;
       			g_pub_intst.dealval = sMoBailRate.ic_rate * pro_note.bail_amt * day / L360 / L100;
				g_pub_intst.dealval = g_pub_intst.dealval + HQdealval;
				sMoBailRate1.amt=sMoBailRate.amt-pro_note.bail_amt;
				sMoBailRate.amt=pro_note.bail_amt;
			}
			g_pub_intst.dealval = pub_base_PubDround_1(g_pub_intst.dealval);

			/** ���±�֤���Ϣ��� **/
			sMoBailRate.intst_date=g_pub_tx.tx_date;
			sMoBailRate.end_trace_no=g_pub_tx.trace_no;
			sMoBailRate.intst_amt=g_pub_intst.dealval;
			strcpy(sMoBailRate.sts,"*");
			ret = Mo_bail_rate_Upd_Upd(sMoBailRate,g_pub_tx.reply);
			if( ret )
   			{
   				sprintf( acErrMsg, "ִ��Mo_bail_rate_Upd_Upd��![%d][%s]",ret,pro_note.pact_no);
       		 	WRITEMSG
				sprintf( g_pub_tx.reply, "B334" );
       		 	goto ErrExit;
   			}
			Mo_bail_rate_Clo_Upd();

			/** �����µı�֤����Ϣ�ǼǱ� **/
			if( pub_base_CompDblVal(sMoBailRate1.amt,0.00) > 0 )
			{
				ret = Mo_bail_rate_Ins(sMoBailRate1,g_pub_tx.reply);
				if( ret )
   				{
   					sprintf( acErrMsg, "ִ��Mo_bail_rate_Ins��![%d]",ret);
        			WRITEMSG
					sprintf( g_pub_tx.reply, "B334" );
        			goto ErrExit;
   				}
			}
			g_pub_intst.tax_intst=g_pub_intst.dealval;	/** Ӧ˰��Ϣ **/
			g_pub_intst.factval=g_pub_intst.dealval;	/** ʵ����Ϣ **/
			g_pub_intst.val=0.00;						/** ��Ϣ˰ **/
			g_pub_intst.keepval=0.00;					/** ��ֵ��Ϣ **/

			/*pro_note.bail_amt=pro_note.bail_amt+g_pub_intst.dealval;*/
			set_zd_data("0153","52101");                /** ��֤����Ϣ **/
			set_zd_double("102P",g_pub_intst.dealval);
		}
	}
	strcpy(g_pub_tx.beg_note_no,pro_note.pact_no);
	if( intst_flag[0]=='1' )		/*** ��֤���Ϣ�� ***/
	{
		strcpy(g_pub_tx.note_type,"Y");
		if ( pub_base_CompDblVal(g_pub_intst.dealval)<0 )
		{
			sprintf(acErrMsg,"������Ϣ�������!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D158");
			goto ErrExit;
		}
		if ( pubf_acct_proc("F326") )
		{
			sprintf(acErrMsg,"�Ǽǵ�����֤������ˮ����!");
			WRITEMSG
			goto ErrExit;
		}
	}
	/**---------- hao ---------------- end ------------**/

	if (pub_ins_trace_log())
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!!=[%s][%d]",pro_note.pact_no,ret);
		WRITEMSG
		goto ErrExit;
	}

	/**---- hao ---- 20051113 ----- ���ӵ�����֤����Ϣ��ͻ��� ---**/
	if( intst_flag[0]=='1' )		/*** ��֤���Ϣ�� ***/
	{
		set_zd_data("1011",sMoBailRate.ac_no);
		set_zd_int ("1012",1);
		set_zd_data("1016","�� ��Ϣ�ʻ�");
		set_zd_data("101A","01");
		set_zd_data("101B","2");                /*ת��*/
		set_zd_double("1013",g_pub_intst.dealval);
		if ( pub_base_my_func("D099","1",g_pub_tx.reply))
		{
			sprintf(acErrMsg,"�� �������ʻ���![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	}

	sprintf(acErrMsg," ac_no1=[%s]",ys_ac_no);
	WRITEMSG

	/*** ҵ����ʼ ***/
	sprintf(acErrMsg,"�⳥��֤���� amends_amt=[%lf] bail_amt=[%lf]",
		pro_note.amends_amt,pro_note.bail_amt);
	WRITEMSG

	/** �⳥�����ڱ�֤�����Ҫ���ֻ��� **/
	if ( pro_note.amends_amt > pro_note.bail_amt )
	{
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",pro_note.pro_ac_no);
		if( ret==100 )
		{
			sprintf( acErrMsg,"�����˺Ŷ��ձ��в����ڴ˼�¼!!erro code=[%d]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"O001" );
			goto ErrExit;
   		}
		else if( ret )
		{
				sprintf( acErrMsg,"��ѯ�����ʺŶ��ձ��쳣!!erro code=[%d]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"O001" );
			goto ErrExit;
   		}

		if(g_mdm_ac_rel.ac_seqn==ALL_SEQN)g_mdm_ac_rel.ac_seqn=1;

		/* ������ʾ�ʺź��˺���Ų�ѯ���ļ� */
		ret=pub_base_disac_file(pro_note.pro_ac_no, g_mdm_ac_rel.ac_seqn, g_pub_tx.ac_type);
		if( ret )
		{
			sprintf( acErrMsg,"������ʾ�ʺź��˺���Ų�ѯ���ļ�erro code=[%d]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"O001" );
			goto ErrExit;
   		}
		tx_amt1 = g_dd_mst.bal - ( pro_note.amends_amt - pro_note.bail_amt ) -1 ;

		sprintf(acErrMsg," amends_amt=[%lf] bail_amt=[%lf] bal=[%lf] tx_amt1=[%lf]",
			pro_note.amends_amt,pro_note.bail_amt,g_dd_mst.bal,tx_amt1);
		WRITEMSG

		/** �������ʻ������� ��Ҫ���  **/
		if ( tx_amt1 > 0.00 )
		{
			tx_amt1 = pro_note.amends_amt - pro_note.bail_amt; /* ��������201������ */
			tx_amt2 = 0.00;
			flag=2; /* ��֤�𲻹��ۡ�������201�ʻ����� */
		}
		else
		{
			tx_amt1 = g_dd_mst.bal - 1; /* ��������201������ */
			tx_amt2 = pro_note.amends_amt - pro_note.bail_amt - g_dd_mst.bal + 1; /* �渶��� */
			flag=3; /* ��֤����������201�ʻ��������� ��Ҫ�渶 */
		}
		tx_amt3 = pro_note.amends_amt; /* 26203 ������ */
		tx_amt4 = 0.00; /* ��������201������ */
	}
	else
	{
		tx_amt3 = pro_note.bail_amt; /* 26203 ������ */
		tx_amt4 = pro_note.bail_amt - pro_note.amends_amt; /* ��������201������ */
	}
sprintf(acErrMsg,"CHKJE amt1=[%lf ] amt2=[%lf] amt3=[%lf] amt4=[%lf]",
	tx_amt1,tx_amt2,g_pub_tx.tx_amt3,g_pub_tx.tx_amt4);
WRITEMSG
	ret = sql_execute("UPDATE protect_note SET protect_sts='1' WHERE pact_no='%s'",pro_note.pact_no);
	if ( ret )
	{
		sprintf(acErrMsg,"UPDATE ERROR pact_no=[%s][%d]",pro_note.pact_no,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"H020");
		goto ErrExit;
	}

	/*************************
	*  1 ��25102 
	*************************/
	set_zd_data("1021",pro_note.bail_ac_no);
	set_zd_int ("1022",1);
	set_zd_data("102Y","�� ��֤���ʻ�");
	set_zd_data("102J","01");
	set_zd_data("102K","2");					/*ת��*/
	set_zd_data("102O","0");					/*�Ƿ��ӡ����(�������۱�־):1��0��*/
	set_zd_double("102F",pro_note.bail_amt);
	if ( pub_base_my_func("D003","1",g_pub_tx.reply))
	{
		sprintf(acErrMsg,"�� ��֤���ʻ�D003��![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

sprintf(acErrMsg,"CHKJE amt1=[%lf ] amt2=[%lf] amt3=[%lf] amt4=[%lf]",
	tx_amt1,tx_amt2,g_pub_tx.tx_amt3,g_pub_tx.tx_amt4);
WRITEMSG

	/*************************
	*  2 ��������201 
	*************************/
	if ( flag == 2 || flag == 3 )
	{
		set_zd_data("1021",pro_note.pro_ac_no);
		set_zd_data("102Y","�� �������ʻ�");
		set_zd_double("102F",tx_amt1);
		if ( pub_base_my_func("D003","1",g_pub_tx.reply))
		{
			sprintf(acErrMsg,"�� �������ʻ�D003��![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	}
sprintf(acErrMsg,"CHKJE amt1=[%lf ] amt2=[%lf] amt3=[%lf] amt4=[%lf]",
	tx_amt1,tx_amt2,g_pub_tx.tx_amt3,g_pub_tx.tx_amt4);
WRITEMSG
	/***********************
	*  3 ����13601 
	************************/
	if ( flag == 3 )
	{
		set_zd_data("1201","13601");
		set_zd_data("120A","�� ���");
		set_zd_data("1204","01");
		set_zd_data("1205","2");
		set_zd_double("1208",tx_amt2);
		if ( pub_base_my_func("A016","1",g_pub_tx.reply))
		{
			sprintf(acErrMsg,"�� ���A016��![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	}
sprintf(acErrMsg,"CHKJE amt1=[%lf ] amt2=[%lf] amt3=[%lf] amt4=[%lf]",
	tx_amt1,tx_amt2,g_pub_tx.tx_amt3,g_pub_tx.tx_amt4);
WRITEMSG
	/**************************
	*  4 ������Ӧ��Ӧ����26203 
	***************************/
	sprintf(acErrMsg,"QTYS ac_no1=[%s]",ys_ac_no);
	WRITEMSG
	set_zd_data("1211","26203");
	set_zd_data("121A","�� ����Ӧ��Ӧ��");
	set_zd_data("1214","01");
	set_zd_data("1215","2");
    set_zd_double("1218",tx_amt3);
	if ( pub_base_my_func("A017","1",g_pub_tx.reply))
	{
		sprintf(acErrMsg,"�� ����Ӧ��Ӧ��A017��![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}
sprintf(acErrMsg,"CHKJE amt1=[%lf ] amt2=[%lf] amt3=[%lf] amt4=[%lf]",
	tx_amt1,tx_amt2,g_pub_tx.tx_amt3,g_pub_tx.tx_amt4);
WRITEMSG
	/**************************
	*  5 ������Ӧ��Ӧ����26203 
	***************************/
	set_zd_data("1201","26203");
	set_zd_data("120A","�� ����Ӧ��Ӧ��");
	set_zd_data("1204","01");
	set_zd_data("1205","2");
    set_zd_double("1208",tx_amt3);
	if ( pub_base_my_func("A016","1",g_pub_tx.reply))
	{
		sprintf(acErrMsg,"�� ����Ӧ��Ӧ����![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}
	/************************
	*  6 ���������ʻ�201
	*************************/
	if ( flag == 1 && tx_amt4 !=0.00 )
	{
		set_zd_data("1011",pro_note.pro_ac_no);
		set_zd_int ("1012",1);
		set_zd_data("1016","�� �������ʻ�");
		set_zd_data("101A","01");
		set_zd_data("101B","2");                /*ת��*/
		set_zd_double("1013",tx_amt4);
		if ( pub_base_my_func("D099","1",g_pub_tx.reply))
		{
			sprintf(acErrMsg,"�� �������ʻ���![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	}
	/*****************************
	*  7 ���������ʻ�201 �⳥���
	******************************/
	set_zd_data("1011",pro_note.ben_ac_no);
	set_zd_int ("1012",1);
	set_zd_data("1016","�� �������ʻ�");
	set_zd_data("101A","01");
	set_zd_data("101B","2");                /*ת��*/
	set_zd_double("1013",pro_note.amends_amt);
	if ( pub_base_my_func("D099","1",g_pub_tx.reply))
	{
		sprintf(acErrMsg,"�� �������ʻ���![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}


	if ( flag == 3 )
	{
		/*****************************
		*  8 �����ڴ���--����������13001
		******************************/
		if ( yq_chk(tx_amt3,pro_note,tx_amt2))
		{
			sprintf(acErrMsg,"�����ڴ���--����������![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		/*****************************
		*  9 �������渶13601
		******************************/
		set_zd_data("1211","13601");
		set_zd_data("121A","�������渶");
		set_zd_data("1214","01");
		set_zd_data("1215","2");
   		set_zd_double("1218",tx_amt2);
		if ( pub_base_my_func("A017","1",g_pub_tx.reply))
		{
			sprintf(acErrMsg,"�����渶A016��![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	}
	set_zd_double("1208",pro_note.amt);
	set_zd_data("1201","618");			
	set_zd_data("120A","��������ע��");
	set_zd_data("1204","01");			
	set_zd_data("1205","2");			
	if ( pub_base_my_func("A016","1",g_pub_tx.reply))
	{
		sprintf(acErrMsg,"�� ��������ע����![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�����⳥����OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"�����⳥ErrExit!!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

int yq_chk(double amt_lmt,struct protect_note_c pro_note,double tx_amt )
{
	/* �ڶ����֣����е��ͬʱ�����ڴ�� */
	double	basic_rate=0.00;
	char	cCifNo[9];
	char	cAcNo[25];
	int		ret;

vtcp_log("0  OVER_328 [%lf]=[%lf]",pro_note.amt,pro_note.over_rate);
	memset( cCifNo, 0x00, sizeof( cCifNo ) );
vtcp_log(" OVER_328 [%lf]=[%lf]",pro_note.amt,pro_note.over_rate);

	/* ȡ�ж����ڴ����Ʒ���:��������com_parm��ȡ��(��pubf_base_GetParm.c�еĺ���) */
	ret = pub_base_GetParm_Str( "BHYQ", 1, g_pub_tx.prdt_code );
	if( ret )
	{
		sprintf( acErrMsg, "ȡ�жҵ��ת���ڴ����Ʒ��Ŵ�!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B265" );
		goto ErrExit;
	}
			
	ret = Ln_parm_Sel( g_pub_tx.reply , &g_ln_parm , "prdt_no='%s' " , g_pub_tx.prdt_code );
	if(ret)	
	{
		sprintf(acErrMsg,"ȡ���������Ϣʧ��!");
		WRITEMSG
		goto ErrExit;	
	}
			
	/* ��ȡ��Ʊ���˺ŵ�ǰ��λ���ͻ��� */
	strncpy( cCifNo, pro_note.pro_ac_no, 8 );
	cCifNo[8] = '\0';
	g_pub_tx.cif_no = atol( cCifNo );

sprintf( acErrMsg, "cif_no=[%ld]", g_pub_tx.cif_no );
WRITEMSG
sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
WRITEMSG
	g_ln_mst.opn_date = g_pub_tx.tx_date;			/* �������� */		
	strcpy( g_ln_mst.pact_no, pro_note.pact_no );	/* ��ͬ�� */	
	g_ln_mst.amt_lmt = tx_amt;				/* ��ͬ�ܶ�  */
vtcp_log(" OVER_328 [%lf]=[%lf]",pro_note.amt,pro_note.over_rate);
	g_ln_mst.over_rate = pro_note.over_rate;		/* ��������:¼��ʱ����ķ�Ϣ���� */
	g_ln_mst.fine_rate = pro_note.over_rate;		/* ��Ϣ����:¼��ʱ����ķ�Ϣ���� */
	strcpy( g_pub_tx.cur_no, "01");				/* ���� */
	strcpy( g_ln_mst.repay_type, g_ln_parm.ln_pay_type );	/* ����֧����ʽ */	
	g_ln_mst.mtr_date = g_pub_tx.tx_date;			/* ��������:���ڵ������� */
	strcpy( g_pub_tx.intst_type, g_ln_parm.intst_type );	/* ��Ϣ���� */	
	strcpy( g_pub_tx.name, pro_note.pro_name );	/* �����:�жһ�Ʊ��Ʊ�˻��� */		
sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
WRITEMSG			
			
	ret = pub_base_getllz( g_ln_parm.rate_no, g_ln_parm.cur_no, g_pub_tx.tx_date , &basic_rate );
	if( ret )
	{
		sprintf(acErrMsg,"���ø������ʱ��ȡ�����ʺ�������!");
		WRITEMSG
		return -1;	
	}
	basic_rate = basic_rate/1.2;/* ת���������� */
sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
WRITEMSG			
	/* ���ʸ����������� */
	g_ln_mst.over_rate_flt = pub_base_PubDround_1( g_ln_mst.over_rate*100/basic_rate);
	g_ln_mst.fine_rate_flt = pub_base_PubDround_1( g_ln_mst.fine_rate*100/basic_rate);
		
	strcpy ( g_pub_tx.ac_wrk_ind,"0110" );
	strcpy ( g_pub_tx.ac_id_type , "3" );		/* �˺����� 3-���� */
	strcpy ( g_pub_tx.add_ind , "1" );		/* ����1�� */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* ��ת:1��2ת */	
	strcpy ( g_pub_tx.prt_ind , "0" );		/* �Ƿ��ӡ����(�������۱�־)0�� */
	strcpy ( g_pub_tx.hst_ind , "1" );		/* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� */
	strcpy( g_pub_tx.brf,"�����������" );		
	
	TRACE
sprintf( acErrMsg, "cif_no=[%ld]", g_pub_tx.cif_no );
WRITEMSG
	g_pub_tx.ac_id =0 ;
	/***�����˺�***/
	if( pub_acct_opn_cif_mdm() )
	{
		sprintf(acErrMsg,"�����˺Ŵ���!");
		WRITEMSG
		goto ErrExit;
	}
sprintf(acErrMsg,"�����˺�[%s]",g_pub_tx.ac_no);
WRITEMSG			
sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
WRITEMSG
			
	strcpy(cAcNo, g_pub_tx.ac_no);			/* �Ǽǳжһ�Ʊ������Ϣ�Ǽǲ��� */
	TRACE
	/* Pub_tx�ṹ��ֵ */
	strcpy ( g_pub_tx.ac_wrk_ind,"0110" );
	strcpy ( g_pub_tx.ac_id_type , "3" );		/* �˺����� 3-���� */
	strcpy ( g_pub_tx.add_ind , "1" );		/* ����1�� */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* ��ת:1��2ת */	
	strcpy ( g_pub_tx.prt_ind , "0" );		/* �Ƿ��ӡ����(�������۱�־)0�� */
	strcpy ( g_pub_tx.hst_ind , "1" );		/* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� */
	g_pub_tx.svc_ind = 3001;
	strcpy( g_pub_tx.brf,"�����" );
	TRACE
sprintf( acErrMsg, "cif_no=[%ld]", g_pub_tx.cif_no );
WRITEMSG
sprintf( acErrMsg, "ac_id=[%ld]", g_pub_tx.ac_id );
WRITEMSG
	g_pub_tx.tx_amt1 = 0.00;
	/***�����˻�ID***/
	if( pub_acct_opn_ac() )
	{
		sprintf(acErrMsg,"�����˺�ID����!");
		WRITEMSG
		goto ErrExit;
	}
sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
WRITEMSG
	/* �������֣����ڴ���ſ�裺13001���ڴ��� */
	sprintf(acErrMsg,"�����ʺ�[%s]",g_pub_tx.ac_no);
	WRITEMSG
	
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �����˻���� */
	sprintf(acErrMsg,"�����˻����[%d]",g_pub_tx.ac_seqn);
	WRITEMSG	
	
	g_pub_tx.tx_amt1 = g_ln_mst.amt_lmt;	/* ������ */
	strcpy( g_pub_tx.ct_ind, "2");						/* ��ת��־ */
	sprintf(acErrMsg,"��ת��־![%s]",g_pub_tx.ct_ind);
	WRITEMSG
sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
WRITEMSG			
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
	if( ret ) {return ret;}
sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
WRITEMSG			
	strcpy ( g_pub_tx.add_ind , "1" );		/* ����:0��1��2���� */	
	
	/* �Ƿ��ӡ����(�������۱�־):1��0�� */
	strcpy ( g_pub_tx.prt_ind , "0" );	
	/* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� */
	strcpy ( g_pub_tx.hst_ind , "1" );
	g_pub_tx.svc_ind = 3010 ;
	strcpy( g_pub_tx.brf,"��������" );
	
	sprintf(acErrMsg,"Pub_tx�ṹ��ֵ PASS!");
	WRITEMSG
sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
WRITEMSG			
	/*** ���׼��ʴ��� ***/
	strcpy( g_pub_tx.sub_tx_code, "L104" );
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"���׼��ʴ������!");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"���׼��ʴ��� PASS!");
	WRITEMSG
	
	/* �ǼǴ���ǼǱ� */
	ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.opn_date,g_ln_mst.mtr_date,"1",g_ln_mst.bal,0.00,"0",0 );	
	if( ret )
	{
	    sprintf(acErrMsg,"�ǼǴ���ǼǱ�ʧ��");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L082");
		goto ErrExit;
	}						
	sprintf(acErrMsg,"�ǼǴ���ǼǱ� PASS!");
	WRITEMSG
			
	set_zd_data("0210",g_ln_parm.cur_no);/* �Ա��ָ�ֵ���ǻ����ʱ��dc_entry���õ� */
			
	/* ��130����Ƽ��� */
	set_zd_data( "0210", "01" );    
	set_zd_double( "0430", g_pub_tx.tx_amt1 );
	strcpy( g_pub_tx.sub_tx_code, "L104" );
	sprintf(acErrMsg,"__zyzyzy__sub_tx_code=[%s]",g_pub_tx.sub_tx_code);
	WRITEMSG
	ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
	if (ret != 0)
	{
		sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
		WRITEMSG
		goto ErrExit;
	}
	return 0;
ErrExit:
	return 1;
}
