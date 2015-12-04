/***************************************************************
* �� �� ��:     pubf_acct_opn_ac.c
* ����������	���˺�
*              	1�������˻���ţ�
*              	2���Ǽǲ�Ʒ�˺Ŷ��ձ�
*              	3���Ǽ����ļ���
*              	4���Ǽǿ������Ǽǲ���
*              	5���Ǽǽ�����ˮ��
*
* ��    ��:     xxx
* ������ڣ�    2003��1��6��
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
#include "ac_seqn_id_c.h"
#include "prdt_ac_id_c.h"
#include "mdm_attr_c.h"
#include "cif_prdt_rel_c.h"
#include "com_item_c.h"
#include "ac_ac_rel_c.h"

char cif_type;

pub_acct_opn_ac()
{
	int ret;

/*	char cif_type[2];
*/
	struct ac_seqn_id_c	ac_seqn_id_c;
	struct prdt_ac_id_c	prdt_ac_id_c;
	struct mdm_attr_c	mdm_attr_c;
	struct dd_mst_c	v_dd_mst;
	struct cif_prdt_rel_c	g_cif_prdt_rel;
	memset(&ac_seqn_id_c,0x00,sizeof(struct ac_seqn_id_c));
	memset(&prdt_ac_id_c,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_cif_prdt_rel,0x00,sizeof(struct cif_prdt_rel_c));
	ret=0;

	/***�����˺ż��***/
	ret=pub_sub_init();
	if( ret ) goto ErrExit;
	
	sprintf(acErrMsg,"�˺�[%s]",g_pub_tx.ac_no);
	WRITEMSG	/**ȡ�����˺ű�**/
	
	ret=Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s' ",
		g_pub_tx.ac_no);
	if( ret ) 
	{
		sprintf(acErrMsg,"ȡ�����˺Ŷ��ձ��[%s]",g_pub_tx.ac_no);
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("%s %d ",__FILE__,__LINE__);
	ret=Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
	if( ret==100 )
	{
		sprintf(acErrMsg,"��ʾ�˺Ų�����![%s]",g_pub_tx.ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	else if( ret ) 
	{ 
		sprintf(acErrMsg," ȡ��ʾ�˺ų���![%s][%d]",g_pub_tx.ac_no,ret);
		WRITEMSG
		goto ErrExit; 
	}

	ret=pub_sub_chk_mdm();
	if( ret )
	{
		sprintf(acErrMsg," ------�����ʳ���------ " );
		WRITEMSG
		goto ErrExit;
	}

	strcpy(g_pub_tx.opn_br_no,g_mdm_ac_rel.opn_br_no);

	g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;
	strcpy(g_pub_tx.mdm_code,g_mdm_ac_rel.mdm_code);

	/*** ��ȡ�������� ***/
	ret=Mdm_attr_Sel(&g_pub_tx.reply,&mdm_attr_c," mdm_code='%s'", \
					 g_pub_tx.mdm_code);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�ý������Բ�����![%d][%s]",
			g_reply_int,g_pub_tx.mdm_code);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P132");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"ȡ���������쳣![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P133");
		goto ErrExit;
	}

	/*** Add By xxx **/
	/*** ���˻������ж�״̬ ***/
	if( mdm_attr_c.ac_num_ind[0]=='N' )
	{
		if( g_mdm_ac_rel.main_ind[0]!='1' && mdm_attr_c.mdm_type[0]=='3' )
		{
			sprintf(acErrMsg,"�����������¿���!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D220");
			goto ErrExit;
		}

		if( g_mdm_ac_rel.coll_sts[0]=='1' )
		{
			if( pub_base_checkcollyn( g_mdm_ac_rel.ac_no ) )
			{
				sprintf(acErrMsg,"���ü�ʰ��麯������");
				WRITEMSG
				goto ErrExit;
			}
		}

		switch( g_mdm_ac_rel.note_sts[0] )
		{
			case '1':
				sprintf(acErrMsg,"�ý����ѹ�ʧ!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"M009");
				goto ErrExit;
			case '2':
				sprintf(acErrMsg,"�ý����ѹ�ʧ��֤!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P171");
				goto ErrExit;
			case '3':
				sprintf(acErrMsg,"�ý���������֤����!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"M022");
				goto ErrExit;
			case '4':
				sprintf(acErrMsg,"�ý����ѱ�����!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"O217");
				goto ErrExit;
			case '5':
				sprintf(acErrMsg,"�ý����ѳ���!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"M023");
				goto ErrExit;
			case '*':
				sprintf(acErrMsg,"�ý�������!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P172");
				goto ErrExit;
			default:
				break;
		}
	}

	/***����һ��ͨ��ӡ������3***/
	if( !strncmp(mdm_attr_c.mdm_code,"0016",4) && !strncmp(g_pub_tx.tx_code,"2104",4) )
	{	
	vtcp_log("aaaaaaaaa[%d]\n",g_mdm_ac_rel.prt_line);
	g_mdm_ac_rel.prt_line+=3;
	vtcp_log("aaaaaaaaaa[%d]\n",g_mdm_ac_rel.prt_line);
	}
	/****
	else if( !strncmp(mdm_attr_c.mdm_code,"0016",4) )
	{	
	vtcp_log("bbbbbbbbb[%d]\n",g_mdm_ac_rel.prt_line);
	g_mdm_ac_rel.prt_line++;
	vtcp_log("bbbbbbbbb[%d]\n",g_mdm_ac_rel,prt_line);
	}
***/
	strcpy(g_pub_tx.note_type,mdm_attr_c.note_type);
	strcpy(g_pub_tx.beg_note_no,g_mdm_ac_rel.note_no);
	strcpy(g_pub_tx.end_note_no,g_mdm_ac_rel.note_no);

	if(mdm_attr_c.ac_num_ind[0]=='Y')
	{
		g_reply_int= Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld", \
									g_pub_tx.ac_id);
		if( !g_reply_int )
		{
			sprintf(acErrMsg,"�ý���ֻ����һ���˻�![%d]",g_reply_int);
			WRITEMSG
			sprintf(acErrMsg,"ac_id[%d]",g_pub_tx.ac_id);
			WRITEMSG
						
			strcpy(g_pub_tx.reply,"P244");
			goto ErrExit;
		}
		else if( g_reply_int&&g_reply_int!=100 )
		{
			sprintf(acErrMsg,"���Ҵ���![%d]",g_reply_int);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
	}
	/***FINISH***/

	/*** �����˻���� ***/
	if( g_pub_tx.prdt_code[0]=='1' ) /**���ڣ�ÿ����һ������**/
	{
		ret=pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);
		if( ret ) 
		{
			sprintf(acErrMsg,"ת��ų��� 1 " );
			WRITEMSG
			goto ErrExit;
		}
		ret=Dd_mst_Sel(g_pub_tx.reply,&v_dd_mst,
			"ac_id=%ld and ac_seqn=%d ",g_pub_tx.ac_id,g_pub_tx.ac_seqn );
		if( ret && ret!=100 ) goto ErrExit;
		else if( ret==0 )
		{
			sprintf(acErrMsg,"�˺��Ѿ�����id[%ld]seqn[%d]!ret[%d]",
					g_pub_tx.ac_id,g_pub_tx.ac_seqn,ret);
			WRITEMSG
			strcpy( g_pub_tx.reply,"P164" );
			goto ErrExit;
		}
		if( g_mdm_ac_rel.ac_seqn!=ALL_SEQN ) /**�Ƕ໧����ʹ�����**/
		{
			g_mdm_ac_rel.ac_seqn=g_pub_tx.ac_seqn;
		}
	}
	else if( g_mdm_ac_rel.ac_seqn==ALL_SEQN ) /**�໧����ʹ�����**/
	{
		ret=pub_base_CrtAcSeqn( g_pub_tx.ac_id , &g_pub_tx.ac_seqn );
		if( ret )
		{
			sprintf(acErrMsg,"�˺����ID���쳣����![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P140");
			goto ErrExit;
		}
		/***����һ��ͨÿ���������ɿ�26�����ʻ����������¶���һ��ͨ�ʻ�***/
		if(!strcmp(mdm_attr_c.mdm_code, "0016") && g_pub_tx.ac_seqn == 226)
		{
			sprintf(acErrMsg,"����һ��ͨ�����ѳ�����󿪻������뿪��һ��ͨ�ʻ�!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P510");
			goto ErrExit;
		}
	}
	else if( ac_seqn_id_c.ac_seqn_id!=ZERO_SEQN ) /**������ͳһ���**/
	{
		g_pub_tx.ac_seqn=SINGLE_SEQN;
		g_mdm_ac_rel.ac_seqn=g_pub_tx.ac_seqn;
	}
	else
	{
		sprintf(acErrMsg,"�ý��ʲ���������˻�![%d]",ac_seqn_id_c.ac_seqn_id);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P143");
		goto ErrExit;
	}

	if(mdm_attr_c.ac_num_ind[0]=='Y')
		g_mdm_ac_rel.book_bal=g_pub_tx.tx_amt1;
	else
		g_mdm_ac_rel.book_bal=0.00;
	ret=Mdm_ac_rel_Upd_Upd(g_mdm_ac_rel,g_pub_tx.reply);
	if( ret ) 
	{
		sprintf(acErrMsg,"�޸Ľ����˺Ŷ��ձ����" );
		WRITEMSG
		goto ErrExit;
	}

	ret=Mdm_ac_rel_Clo_Upd();
	if( ret ) 
		goto ErrExit;

	sprintf(acErrMsg,"���������[%d]",g_pub_tx.ac_seqn);
	WRITEMSG
	
	/*** �Ǽǲ�Ʒ�˻����ձ� ***/
	/****�Ѿ���Ϊ��ͼ**
	prdt_ac_id_c.ac_id=g_pub_tx.ac_id;
	prdt_ac_id_c.ac_seqn=g_pub_tx.ac_seqn;
	strncpy(prdt_ac_id_c.ac_id_type,g_pub_tx.prdt_code,1);
	strcpy(prdt_ac_id_c.prdt_no,g_pub_tx.prdt_code);
	ret=Prdt_ac_id_Ins(prdt_ac_id_c,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"�Ǽǲ�Ʒ�˻�ID���ձ����![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P144");
		goto ErrExit;
	}
	*********/

	/*** �Ǽ����ļ� ***
	if( g_pub_tx.add_ind[0]!='1' )
	{
		sprintf(acErrMsg,"��ȡ��־����g_pub_tx.add_ind=[%s]!",g_pub_tx.add_ind);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P106");
		goto ErrExit;
	}
	*/

	/**** ADD BY xxx.05.03.23 ****/
	/**** �Ǽǿͻ���Ʒ��ϵ�� ****/
	if( g_pub_tx.prdt_code[0]=='1'||g_pub_tx.prdt_code[0]=='2'|| \
		g_pub_tx.prdt_code[0]=='3' )
	{
		g_cif_prdt_rel.cif_no=g_pub_tx.cif_no;
		strcpy(g_cif_prdt_rel.prdt_code,g_pub_tx.prdt_code);
		g_cif_prdt_rel.ac_id=g_pub_tx.ac_id;
		strcpy(g_cif_prdt_rel.relation_no,"1");			/*��ϵ����1-����*/
		strcpy(g_cif_prdt_rel.relation_flag,"1");		/*״̬1-����2-���*/
		g_cif_prdt_rel.addr_seri=0;						/*�˵���ַ*/
		g_cif_prdt_rel.crt_date=g_pub_tx.tx_date;		/*��������*/
		g_cif_prdt_rel.cls_date=0;						/*�������*/

		ret = Cif_prdt_rel_Ins(g_cif_prdt_rel,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg, "Insert cif_prdt_rel error !! [%d]", ret);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "C080" );
			goto ErrExit;
		} 
	}
	/****FINISH****/

	switch(g_pub_tx.prdt_code[0])
	{
		case '1' :	
			ret=pub_ins_dd_mst();
			if( ret )
			{
				sprintf(acErrMsg,"�Ǽǻ������ļ�����![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			break;
		case '2' :	
			ret=pub_ins_td_mst();
			if( ret )
			{
				sprintf(acErrMsg,"�ǼǶ������ļ�����![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			break;
		case '3' :	
			ret=pub_ins_ln_mst();
			if( ret )
			{
				sprintf(acErrMsg,"�ǼǴ������ļ�����![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			break;
		case '9' :	
			ret=pub_ins_in_mst();
			if( ret )
			{
				sprintf(acErrMsg,"�Ǽ��ڲ����ļ�����![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			break;
		default  :	
			sprintf(acErrMsg,"�˻����ʹ�!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P104");
			goto ErrExit;
	}
	if((memcmp(g_pub_tx.tx_code,"2104",4)==0)&&strlen(g_mdm_ac_rel.ac_no)==13)
	{
	vtcp_log("-=========g_pub_tx.ac_id_type=[%s],ac_no=[%s]",g_pub_tx.ac_id_type,g_mdm_ac_rel.ac_no);
		ret=pub_unprt_hst_ins(g_pub_tx.ac_id_type);
		if(ret)
		{
			sprintf(acErrMsg,"ERROR");
			WRITEMSG 
			goto ErrExit;
		}
	}
/*** �����۴��� ***/
	if(g_pub_tx.prt_ind[0]=='1')
	{
	ret=pub_base_patch_book(mdm_attr_c);
		if( ret<0 )
		{
			sprintf(acErrMsg,"�����۴���!");
			WRITEMSG
			goto ErrExit;
		}
	}

	/*** �Ǽǽ�����ˮ ***/
	if( g_pub_tx.prdt_code[0]!='3' )
	{
		if( pub_ins_trace_log() )
		{
			sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
			WRITEMSG
			goto ErrExit;
		}
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���˻��ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"���˻�ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int pub_ins_dd_mst()
{
	int ret;
	double rate_val;

	g_dd_mst.ac_id=g_pub_tx.ac_id;
	vtcp_log("-==========ac_type=[%s]",g_dd_mst.ac_type);
	g_dd_mst.ac_seqn=g_pub_tx.ac_seqn;
	strcpy(g_dd_mst.opn_br_no,g_pub_tx.opn_br_no);
	strcpy(g_dd_mst.prdt_no,g_pub_tx.prdt_code);
	vtcp_log("-==========g_pub_tx.prdt_code=[%s]",g_pub_tx.prdt_code);
	g_dd_mst.bal=g_pub_tx.tx_amt1;
	g_dd_mst.acbo_bal=g_pub_tx.tx_amt1;
	g_dd_mst.ys_bal=0.00;	
	g_dd_mst.hst_bal=g_pub_tx.tx_amt1;	
	g_dd_mst.rate=g_pub_tx.rate;
	g_dd_mst.flt_ratio=0.00; /**˭�ӵ�??**/
	g_dd_mst.opn_date=g_pub_tx.tx_date;
	g_dd_mst.ic_date=g_pub_tx.tx_date;
	g_dd_mst.lst_date=g_pub_tx.tx_date;

	/***Add by xxx***/
	/***�Թ���������ȡ�֡��Թ�һ�㻧����ȡ��***/
	if( g_dd_parm.cif_type[0]=='1' || g_pub_tx.ac_type[0]=='1' )
		g_dd_mst.cash_ind[0]='Y';
	else if(g_dd_parm.cif_type[0]!='1' && g_pub_tx.ac_type[0]=='2')
		g_dd_mst.cash_ind[0]='N';
	else 
		g_dd_mst.cash_ind[0]=g_dd_parm.cash_ind[0];
	/***Finish***/

	g_dd_mst.odtt_ind[0]='0';
	strcpy( g_dd_mst.cal_code,g_pub_tx.cal_code );
/****for xxxxxxxx****/
	if( !strncmp(g_dd_mst.prdt_no,"142",3) )
		strcpy( g_dd_mst.cal_code,"61099" );
	else if( cif_type=='1' || cif_type=='5' )
		g_dd_mst.cal_code[0]='1';
	else if( cif_type=='2' )
		g_dd_mst.cal_code[0]='2';
	else if( cif_type=='3' || cif_type=='4' || cif_type=='6' )
		g_dd_mst.cal_code[0]='3';
	else 
		g_dd_mst.cal_code[0]='2';


	/***�������**/
	/*��鿪�������Ƿ����������ں���ֹ����֮��*/	
	if ( (g_dd_parm.beg_date > g_dd_mst.opn_date) || \
		(g_dd_mst.opn_date > g_dd_parm.end_date))
	{	
		sprintf(acErrMsg,"����Ч��[%d]not in[%d][%d]",
			g_dd_mst.opn_date,g_dd_parm.beg_date,g_dd_parm.end_date);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "A001" );
		goto ErrExit;
	}

	if( (g_dd_parm.cif_type[0]=='1' &&  cif_type!='1' && cif_type!='5' ) ||
		(g_dd_parm.cif_type[0]=='2' 
			&&  cif_type!='2' && cif_type!='3' && cif_type!='4' ) ||
			(g_dd_parm.cif_type[0]=='5' &&  cif_type!='5' )  )
	{
		sprintf( acErrMsg,"�ͻ����Ͳ���parm[%s] [%c]",
			g_dd_parm.cif_type,cif_type );
		WRITEMSG
		strcpy ( g_pub_tx.reply , "D112" );
		goto ErrExit;
	}

	/**�˻�����**/
	if( g_pub_tx.ac_type[0]=='S' || g_pub_tx.ac_type[0]=='D' )
	{
		strcpy(g_dd_mst.sttl_type,g_pub_tx.ac_type);
		strcpy(g_dd_mst.ac_type,"5");
		vtcp_log("-=666666==g_dd_mst.ac_type=[%s]",g_dd_mst.ac_type);
		
	}
	else
	{
		strcpy(g_dd_mst.ac_type,g_pub_tx.ac_type);
		strcpy(g_dd_mst.sttl_type,"D");
	}

	/*���ò�Ʒ��Ϳ���������߿������*/
	if ( g_pub_tx.ac_wrk_ind[4]=='1' && 
		pub_base_CompDblVal(g_dd_mst.bal,0.00)==0 )
	{
		;
	}
	else if ( pub_base_CompDblVal(g_dd_mst.bal,g_dd_parm.min_opn_amt)<0 )
	{
		sprintf(acErrMsg,"low [%.2lf]<[%.2lf]! ",
			g_dd_mst.bal,g_dd_parm.min_opn_amt);
		WRITEMSG
		if( pub_base_CompDblVal(g_dd_mst.bal,0.00)==0 )
		{
			sprintf(acErrMsg,"���ڿ��������");
			WRITEMSG
		}
		else
		{
			strcpy ( g_pub_tx.reply , "A003" );
			goto ErrExit;
		}
	}
	else if( pub_base_CompDblVal(g_dd_mst.bal,g_dd_parm.max_opn_amt)>0
		&& pub_base_CompDblVal(g_dd_parm.max_opn_amt,0.00)!=0 )
	{
		sprintf(acErrMsg,"open bal overflow!! ");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "A004" );
		goto ErrExit;
	}

	/*��鿪��ͨ�ұ�־,ͨ��ֻҵ����������ڻ�ƻ����޹�*/
	if ( g_dd_parm.thr_opn_ind[0]=='N' && 
		strcmp(g_pub_tx.opn_br_no,g_pub_tx.tx_br_no ) )
	{
		sprintf(acErrMsg,"thr_opn_ind is N,open failed!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "A002" );
		goto ErrExit;
	}

	/*��������Ƿ񳬹��涨�����ϸ����¸���Ľ���*/
	/***ȡ���ʱ�Ŷ�Ӧ������***/
	ret = pub_base_getllz(g_dd_parm.rate_no,g_dd_parm.cur_no,
		g_pub_tx.tx_date,&rate_val);
	if( ret ) goto ErrExit;

	if( g_dd_parm.rate_type[0]=='0' && g_pub_tx.ac_type[0]!='6' )/**��������**/
		g_dd_mst.rate=rate_val;
	else if( g_dd_parm.rate_type[0]=='2' || g_pub_tx.ac_type[0]=='6' ) /**��������(Э����Ʒ)**/
		g_dd_mst.rate=g_pub_tx.rate;
	else /**�������ʼ���ϸ��¸�����**/
	{
		if( pub_base_CompDblVal(g_dd_mst.rate,
			rate_val*(1+g_dd_parm.min_flt_ratio) )>0 )
		{
			sprintf(acErrMsg,"current rate is too high!! ");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A006" );
			goto ErrExit;
		}
		if( pub_base_CompDblVal(g_dd_mst.rate,
			rate_val*(1-g_dd_parm.min_flt_ratio) )<0 )
		{
			sprintf(acErrMsg,"current rate is too high!! ");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A005" );
			goto ErrExit;
		}
	}

	/* ��鿪�������������� (���ú���) *
	g_dd_parm.opn_lmt */

	/**����������**/

	if(!strcmp(g_pub_tx.intst_type,""))
	{
		strcpy(g_pub_tx.intst_type,g_dd_parm.intst_type);
	}
	strcpy(g_dd_mst.intst_type,g_pub_tx.intst_type);

	/*** ������Ϣ�����ĺ��� ***/
	g_dd_mst.intst_acm=0.00;
	if( pub_base_CalAcm(g_pub_tx.tx_date,g_pub_tx.tx_date,
		g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
		0.00,g_pub_tx.tx_amt1,&g_dd_mst.intst_acm,
		g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
	{
		sprintf(acErrMsg,"������Ϣ��������!");
		WRITEMSG
		goto ErrExit;
	}

sprintf(acErrMsg,"intst_acm=[%lf]",g_dd_mst.intst_acm);
WRITEMSG
vtcp_log("-==========ac_type=[%s]",g_dd_mst.ac_type);

	g_dd_mst.hst_cnt=0;
	g_dd_mst.hst_pg=0;
	/** �����־��ֵ **/
	if( g_dd_parm.opn_aplic_ind[0]=='Y' && \
		(g_dd_mst.ac_type[0]=='1' || g_dd_mst.ac_type[0]=='2' || \
		 g_dd_mst.ac_type[0]=='3' || g_dd_mst.ac_type[0]=='4') )
			strcpy(g_dd_mst.ac_sts,"0");
	else
		strcpy(g_dd_mst.ac_sts,"1");
	if(memcmp(g_dd_parm.prdt_no,"145",3)==0)/***���ʻ� 2007-6-8 9:32*/
	{
		strcpy(g_dd_mst.ac_sts,"1");
	}
	strcpy(g_dd_mst.hold_sts,"0");
	g_dd_mst.hold_amt=0.00;
	g_dd_mst.ctl_amt=0.00;
	strcpy(g_dd_mst.od_ind,g_dd_parm.od_aplic_ind);	
	strcpy(g_dd_mst.draw_pwd,"N");

	g_dd_mst.cif_no=g_pub_tx.cif_no;
	strcpy(g_dd_mst.name,g_pub_tx.name);
	/**ʹ�Թ��������Ѿ������Ŀͻ�,�����ٿ������� modify by wanglei 20061120**/
	if( g_pub_tx.ac_type[0]=='1' )
	{
		ret=sql_count("dd_mst","cif_no=%ld and ac_type='1' and ac_sts!='*'",g_pub_tx.cif_no,g_dd_mst.ac_sts);
		if( ret )
		{
			sprintf(acErrMsg,"�ÿͻ��Ѵ��ڻ����˻�[%ld][%d]", \
					g_pub_tx.cif_no,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C082");
			goto ErrExit;
		}
	}

	ret=pub_ins_dd_hst();
	if( ret )
	{
		sprintf(acErrMsg,"�Ǽǻ�����ϸ�ļ�����![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

sprintf(acErrMsg,"���Ϊ:[%lf]",g_dd_mst.bal);
WRITEMSG

	ret=Dd_mst_Ins(g_dd_mst,g_pub_tx.reply);	
	vtcp_log("debug begin\n");
		 Dd_mst_Debug(&g_dd_mst);
	if( ret )
	{
		sprintf(acErrMsg,"�Ǽǻ��ڲ�Ʒ���ļ�����![%d]",ret);
		WRITEMSG
		 Dd_mst_Debug(&g_dd_mst);
		strcpy(g_pub_tx.reply,"P144");
		goto ErrExit;
	}
	/*** �Ǽǿ������Ǽǲ� ***/
	if( pub_base_InsMo_opn_cls(g_dd_mst.name,"","1") )
	{
		sprintf(acErrMsg,"�Ǽǿ������Ǽǲ�����!");
		WRITEMSG
		goto ErrExit;
	}

	return 0;
ErrExit:
	return 1;
}

int pub_ins_td_mst()
{
	int ret;
    int dedr_intvl;
	double rate_val;
	long l_term=0;
        struct ac_ac_rel_c      g_ac_ac_rel;
        struct mdm_ac_rel_c     t_mdm_ac_rel;
	memset(&g_ac_ac_rel,0x00,sizeof(struct ac_ac_rel_c));
	memset(&t_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

vtcp_log("INNNNNNNNNNNNNNNNNNNNNNNNN");
	/*** ǰֻ̨�����˵��˻����ʵĽ�� ***/
	/*** �����10�����ϱ��뿪���ִ浥 ***/
	/*if( (!strcmp(g_td_parm.dc_code,"2151") || !strcmp(g_td_parm.dc_code,"2158"))
		&& strcmp(g_td_parm.prdt_no,"20C")
		&& pub_base_CompDblVal(g_pub_tx.tx_amt1,100000.00)>=0 
		&& strcmp(g_pub_tx.mdm_code,"0014") )
	{
		sprintf(acErrMsg,"����������ȡ����ʮ��Ԫ�����ô������ִ浥![%s][%lf]%s",
			g_td_parm.prdt_no,g_pub_tx.tx_amt1,g_pub_tx.mdm_code);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "P419" );
		goto ErrExit;
	}
*/
	/*** �����10�����²��������ִ浥 ***/
	/*if( (!strcmp(g_td_parm.dc_code,"2151") || !strcmp(g_td_parm.dc_code,"2158"))
		&& strcmp(g_td_parm.prdt_no,"20C")
		&& pub_base_CompDblVal(g_pub_tx.tx_amt1,100000.00)<0 
		&& !strcmp(g_pub_tx.mdm_code,"0014") )
	{
		sprintf(acErrMsg,"����������ȡС��ʮ��Ԫ�����ô������ִ浥![%s][%lf]%s",
			g_td_parm.prdt_no,g_pub_tx.tx_amt1,g_pub_tx.mdm_code);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "P423" );
		goto ErrExit;
	}*/

	g_td_mst.ac_id=g_pub_tx.ac_id;
	g_td_mst.ac_seqn=g_pub_tx.ac_seqn;
	strcpy(g_td_mst.opn_br_no,g_pub_tx.opn_br_no);
	pub_base_strpack(g_td_parm.br_no);
	if( strlen(g_td_parm.br_no)==5 )
	{
		strcpy(g_td_mst.opn_br_no,g_td_parm.br_no);
		strcpy(g_pub_tx.opn_br_no,g_td_parm.br_no);
	}
	strcpy(g_td_mst.prdt_no,g_pub_tx.prdt_code);
	g_td_mst.bal=g_pub_tx.tx_amt1;
	g_td_mst.acbo_bal=g_pub_tx.tx_amt1;
	g_td_mst.ys_bal=0.00;	
	g_td_mst.hst_bal=g_pub_tx.tx_amt1;	
	g_td_mst.rate=0.00;
	g_td_mst.flt_ratio=0.00;

	g_td_mst.opn_date=g_pub_tx.tx_date;
	g_td_mst.ic_date=g_pub_tx.tx_date;
	g_td_mst.lst_date=g_pub_tx.tx_date;
	g_td_mst.book_line=g_mdm_ac_rel.prt_line;

		/**���㵽������**/
	if(memcmp(g_td_parm.prdt_no,"25A",3)==0)/*����Э��ͬҵ���� add by martin 2009/7/22 10:59:38*/
		{
			/*����ֱ�Ӹ��Ƶ�����*/
		}
	else
		{/* begin by martin 2009/7/22 11:00:30*/
	if (g_td_parm.td_type[0] != '4')
	{
		if( g_td_parm.term_type[0]=='D' )
		{
			l_term=g_td_parm.term;
			pub_base_deadlineD(g_td_mst.opn_date,l_term,&g_td_mst.mtr_date);
		}
		if( g_td_parm.term_type[0]=='Y' )
		{
			g_td_parm.term_type[0]='M';
			g_td_parm.term*=12;
		}
		if( g_td_parm.term_type[0]=='M' )
		{
			l_term=g_td_parm.term;
			pub_base_deadlineM(g_td_mst.opn_date,l_term,&g_td_mst.mtr_date);
		}
	}
	else
	{
		g_td_mst.mtr_date = 0;
	}
   }/* end by martin 2009/7/22 11:00:30*/
	g_pub_tx.mach_date=g_td_mst.mtr_date;

	sprintf(acErrMsg,"term_type[%s] term[%d][%d]opn[%d]mtr[%d]",
		g_td_parm.term_type,l_term,g_td_parm.term,
		g_td_mst.opn_date,g_td_mst.mtr_date);
	WRITEMSG

	/***�������**/
		/*��鿪�������Ƿ����������ں���ֹ����֮��*/	
		if ( (g_td_parm.beg_date > g_td_mst.opn_date) || \
			(g_td_mst.opn_date > g_td_parm.end_date))
		{	
			sprintf(acErrMsg,"����Ч��[%d]not in[%d][%d]",
				g_td_mst.opn_date,g_td_parm.beg_date,g_td_parm.end_date);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A001" );
			goto ErrExit;
		}
TRACE
		if( (g_td_parm.cif_type[0]=='1' &&  cif_type!='1' && cif_type!='5' ) ||
			(g_td_parm.cif_type[0]=='2' 
				&&  cif_type!='2' && cif_type!='3' && cif_type!='4' ) ||
			(g_td_parm.cif_type[0]=='5' &&  cif_type!='5' )  )
		{
			sprintf( acErrMsg,"�ͻ����Ͳ���parm[%s] [%c]",
				g_td_parm.cif_type,cif_type );
			WRITEMSG
			strcpy ( g_pub_tx.reply , "D112" );
			goto ErrExit;
		}

TRACE
		/*���ò�Ʒ��Ϳ���������߿������*/
		if ( g_pub_tx.ac_wrk_ind[4]=='1' && 
			pub_base_CompDblVal(g_td_mst.bal,0.00)==0 )
		{
			;
		}
TRACE
		if ( pub_base_CompDblVal(g_td_mst.bal,g_td_parm.min_opn_amt)<0 )
		{
			sprintf(acErrMsg,"open bal is too low!!  \
			g_td_mst.bal==================================[%.2lf], \
			g_td_parm.min_opn_amt=========================[%.2lf]", \
			g_td_mst.bal,g_td_parm.min_opn_amt);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A003" );
			goto ErrExit;
		}
		else if( pub_base_CompDblVal(g_td_mst.bal,g_td_parm.max_opn_amt)>0
			&& pub_base_CompDblVal(g_td_parm.max_opn_amt,0.00)!=0 )
		{
			sprintf(acErrMsg,"open bal overflow!! ");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A004" );
			goto ErrExit;
		}

TRACE
		/*��鿪��ͨ�ұ�־,ͨ��ֻҵ����������ڻ�ƻ����޹�*/
		if ( g_td_parm.thr_opn_ind[0]=='N' && 
			strcmp(g_pub_tx.opn_br_no,g_pub_tx.tx_br_no ) )
		{
			sprintf(acErrMsg,"thr_opn_ind is N,open failed!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A002" );
			goto ErrExit;
		}

TRACE
		/*��������Ƿ񳬹��涨�����ϸ����¸���Ľ���*/
		/***ȡ���ʱ�Ŷ�Ӧ������***/
		ret = pub_base_getllz(g_td_parm.rate_no,g_td_parm.cur_no,
			g_pub_tx.tx_date,&rate_val);
		if( ret ) goto ErrExit;

		if( g_td_parm.rate_type[0]=='0' ) /**��������**/
			g_td_mst.rate=rate_val;
		else /**�������ʼ���ϸ��¸�����**/
		{
			g_td_mst.rate=g_pub_tx.rate;
			vtcp_log("jack -- look g_pub_tx.rate=[%lf][%s][%d]",g_pub_tx.rate,__FILE__,__LINE__);

			/*if( pub_base_CompDblVal(g_td_mst.rate,
				rate_val*(1+g_td_parm.min_flt_ratio) )>0 )
			{
				sprintf(acErrMsg,"current rate is too high!! ");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A006" );
				goto ErrExit;
			}
			if( pub_base_CompDblVal(g_td_mst.rate,
				rate_val*(1-g_td_parm.min_flt_ratio) )<0 )
			{
				sprintf(acErrMsg,"current rate is too high!! \
						g_td_mst.rate=====================[%.2lf], \
						rate_val==========================[%.2lf]", \
						g_td_mst.rate, rate_val);
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A005" );
				goto ErrExit;
			}*/
		}

TRACE
		/* ��鿪�������������� (���ú���) *
		g_td_parm.opn_lmt */

	/**����������**/
    if (g_td_parm.td_type[0]=='1' || g_td_parm.td_type[0]=='2'
        || g_td_parm.td_type[0] == '3' || g_td_parm.td_type[0]=='7' )
    {
        /* �����ȡ ������ȡ �汾ȡϢ �����ܷ������� */
        if (g_td_parm.intvl_type[0] == 'Y')
        {
            dedr_intvl=g_td_parm.dedr_intvl * 12;
        }
        else if (g_td_parm.intvl_type[0] == 'Q')
        {
            dedr_intvl=g_td_parm.dedr_intvl * 4;
        } 
        else if (g_td_parm.intvl_type[0] == 'M')
        {
            dedr_intvl=g_td_parm.dedr_intvl;
        }
        else if (g_td_parm.intvl_type[0] == 'D')
        {
            /* ֻ�������µ������� */
            dedr_intvl = g_td_parm.dedr_intvl / 30;
        }
        else 
        {
            sprintf(acErrMsg,"���ڲ�Ʒ���еļ�����ʹ���!!");
            WRITEMSG
            goto ErrExit;
        }

        if (g_td_parm.term_type[0] == 'Y')
        {
            g_td_mst.ttl_cnt = g_td_parm.term * 12 / dedr_intvl;
        }
        else if (g_td_parm.term_type[0] == 'M')
        {
            g_td_mst.ttl_cnt = g_td_parm.term / dedr_intvl;
        }
        else if (g_td_parm.term_type[0] == 'D')
        {
            g_td_mst.ttl_cnt = g_td_parm.term / 30 / dedr_intvl;
        }
    }
    
  /*** begin.gyf.20060918 ��Ϊ������ȡ������Ϣ��Ҫ��������������������ģ����ǰ **/
	if(!strcmp(g_pub_tx.intst_type,""))
	{
		strcpy(g_pub_tx.intst_type,g_td_parm.intst_type);
	}
	strcpy(g_td_mst.intst_type,g_pub_tx.intst_type);

	/* ������Ϣ�����ĺ��� NEED�������Ҫ�Ľ�����Ϊ��ͬ��ʽҪ��ͬ���ڴ��� */
	g_td_mst.intst_acm=0.00;
TRACE
    if (g_td_parm.td_type[0] == '1' || g_td_parm.td_type[0] == '2' ||
		g_td_parm.td_type[0] == '7' )
    /* �����ȡ ������ȡ */
    {
		if( pub_base_CalAcm(g_pub_tx.tx_date,g_pub_tx.tx_date,
			g_td_parm.intst_term_type,g_td_parm.acm_calc_mode,
			0.00,g_pub_tx.tx_amt1,&g_td_mst.intst_acm,
			g_td_mst.intst_type,g_td_parm.intst_mon,g_td_parm.intst_day) )
		{
			sprintf(acErrMsg,"������Ϣ��������!");
			WRITEMSG
			goto ErrExit;
		}
    }
   /*** end.gyf.20060918 ****/
    
	/**�˻�����**/
	switch( g_td_parm.td_type[0] )
	{
		case '1': /*�����ȡ*/
		case '7': /*��������*/
			g_td_mst.td_amt=g_pub_tx.tx_amt1;
            g_td_mst.tx_cnt = g_td_mst.tx_cnt + 1;
            sprintf(acErrMsg,"tx_cnt=[%d] ",g_td_mst.tx_cnt);
            WRITEMSG
			break;
		case '2': /*������ȡ*/
			/**NEED ��Ҫ����**/
			      g_td_mst.td_amt=g_pub_tx.tx_amt1;
            g_td_mst.tx_cnt = g_td_mst.tx_cnt + 1;
            
            /* begin.gyf.20060918 */
            /* ������Ϣ */
            ret = pub_base_CalIntst(&g_pub_intst);
            if (ret != 0)
            {
                sprintf(acErrMsg,"����汾ȡϢ��Ϣ����!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"P220");
                goto ErrExit;
            }
            sprintf(acErrMsg,"�汾ȡϢ��ϢΪ:[%lf]",g_pub_intst.factval);
            WRITEMSG
            g_td_mst.td_amt+=g_pub_intst.factval ;
            g_td_mst.td_amt/=g_td_mst.ttl_cnt;
            break;
            /* end.gyf.20060918 */
		case '3': /*�汾ȡϢ*/

			/**NEED ��Ҫ����**/
            /* ������Ϣ */
            ret = pub_base_CalIntst(&g_pub_intst);
            if (ret != 0)
            {
                sprintf(acErrMsg,"����汾ȡϢ��Ϣ����!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"P220");
                goto ErrExit;
            }
            sprintf(acErrMsg,"�汾ȡϢ��ϢΪ:[%lf]",g_pub_intst.factval);
            WRITEMSG
			g_td_mst.td_amt=g_pub_intst.factval / g_td_mst.ttl_cnt;

			break;
		default :
			g_td_mst.td_amt=0.00;
			break;
	}

  /*** begin.gyf.20060918 ��Ϊ������ȡ������Ϣ��Ҫ��������������������ģ����ǰ 
	if(!strcmp(g_pub_tx.intst_type,""))
	{
		strcpy(g_pub_tx.intst_type,g_td_parm.intst_type);
	}
	strcpy(g_td_mst.intst_type,g_pub_tx.intst_type);

	/ ������Ϣ�����ĺ��� NEED�������Ҫ�Ľ�����Ϊ��ͬ��ʽҪ��ͬ���ڴ���/
	g_td_mst.intst_acm=0.00;
TRACE
    if (g_td_parm.td_type[0] == '1' || g_td_parm.td_type[0] == '2' ||
		g_td_parm.td_type[0] == '7' )
    /�����ȡ ������ȡ /
    {
		if( pub_base_CalAcm(g_pub_tx.tx_date,g_pub_tx.tx_date,
			g_td_parm.intst_term_type,g_td_parm.acm_calc_mode,
			0.00,g_pub_tx.tx_amt1,&g_td_mst.intst_acm,
			g_td_mst.intst_type,g_td_parm.intst_mon,g_td_parm.intst_day) )
		{
			sprintf(acErrMsg,"������Ϣ��������!");
			WRITEMSG
			goto ErrExit;
		}
    }
    end.gyf.20060918 ****/

	g_td_mst.hst_cnt=0;
	/** �����־��ֵ **/
	strcpy(g_td_mst.ac_sts,"1");
	strcpy(g_td_mst.hold_sts,"0");
	g_td_mst.hold_amt=0.00;
	g_td_mst.ctl_amt=0.00;
	strcpy(g_td_mst.tfr_ind,g_pub_tx.tmp_ind);
	g_td_mst.cif_no=g_pub_tx.cif_no;
	strcpy(g_td_mst.name,g_pub_tx.name);
/****for xxxxxxxx****/
	if( cif_type=='1' || cif_type=='5' )
		g_td_mst.cal_code[0]='1';
	else if( cif_type=='2' )
		g_td_mst.cal_code[0]='2';
	else if( cif_type=='3' || cif_type=='4' || cif_type=='6' )
		g_td_mst.cal_code[0]='3';
	else 
		g_td_mst.cal_code[0]='2';

vtcp_log("INNNNNNNNNNNNNNNNNNNNNNNNN");
TRACE
		ret=pub_ins_td_hst();
		if( ret )
		{
			sprintf(acErrMsg,"�ǼǶ�����ϸ�ļ�����![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}

TRACE
	ret=Td_mst_Ins(g_td_mst,g_pub_tx.reply);	
	if( ret )
	{
		sprintf(acErrMsg,"�ǼǶ��ڲ�Ʒ���ļ�����![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P144");
		goto ErrExit;
	}
	vtcp_log("jack -- AAAAAAAAAAAAAAlook g_pub_tx.rate=[%lf][%s][%d]",g_pub_tx.rate,__FILE__,__LINE__);
        /***********���ΪЭ������ʺ������һ���Թ����ڻ� add by ligl 20060915**************/
        if(g_td_parm.td_type[0]=='5')/*Э����*/
        {
		vtcp_log("jack -- AAAAAAAAAAAAAAlook g_pub_tx.rate=[%lf][%s][%d]",g_pub_tx.rate,__FILE__,__LINE__);
          	
          	ret=Mdm_ac_rel_Sel(&g_pub_tx.reply,&t_mdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no1);
		vtcp_log("jack -- AAAAAAAAAAAAAAlook g_pub_tx.rate=[%ld][%s][%d]",t_mdm_ac_rel.ac_id,__FILE__,__LINE__);
	 	if( ret==100 )
	 	{
			sprintf(acErrMsg,"�����ʺŹ�ϵ�����ڸü�¼![%d][%s]",ret,g_pub_tx.ac_no1);
			WRITEMSG
			strcpy(g_pub_tx.reply,"B117");
			goto ErrExit;
		}
		else if(ret)
		{
			sprintf(acErrMsg,"ȡ�����ʺŹ�ϵ���쳣![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L015");
			goto ErrExit;
		}
		vtcp_log("jack -- AAAAAAAAAAAAAAlook g_pub_tx.rate=[%lf][%s][%d]",g_pub_tx.rate,__FILE__,__LINE__);
		g_ac_ac_rel.ac_id=g_td_mst.ac_id;
		vtcp_log("jack -- AAAAAAAAAAAAAAlook g_pub_tx.rate=[%lf][%s][%d]",g_pub_tx.rate,__FILE__,__LINE__);
		g_ac_ac_rel.ac_id1=t_mdm_ac_rel.ac_id;
		ret=Ac_ac_rel_Ins(g_ac_ac_rel,g_pub_tx.reply);	
		if( ret )
		{
			sprintf(acErrMsg,"�Ǽ��ʻ����������![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P144");
			goto ErrExit;
		}	
        }
        /******************end******************/
	/*** �Ǽǿ������Ǽǲ� ***/
	if( pub_base_InsMo_opn_cls(g_td_mst.name,"","1") )
	{
		sprintf(acErrMsg,"�Ǽǿ������Ǽǲ�����!");
		WRITEMSG
		goto ErrExit;
	}

	return 0;
ErrExit:
	return 1;
}

/*****����ֻ�����*************/
int pub_ins_ln_mst()
{
	int ret;
	int pay_ac_seqn;
	long cif_no,ac_id;
	char v_cif_type[2];
	double rate_val,over_rate_val,fine_rate_val;
	double min_rate , max_rate , rate;
	long min_day , max_day ;
	
	g_ln_mst.ac_id=g_pub_tx.ac_id;
	g_ln_mst.ac_seqn=g_pub_tx.ac_seqn;
	strcpy(g_ln_mst.opn_br_no,g_pub_tx.opn_br_no); 	/**��������**/
	strcpy(g_ln_mst.prdt_no,g_pub_tx.prdt_code);
	/*g_ln_mst.amt_lmt			*�����޶�*/
	g_ln_mst.bal=g_pub_tx.tx_amt1;
	g_ln_mst.ys_bal=0.00;	
	g_ln_mst.hst_bal=0.00;	
	g_ln_mst.ttl_prvd_amt=g_pub_tx.tx_amt1; /*�ۼƷ��Ž��*/

	/*g_ln_mst.rate;  ����*/
	/*g_ln_mst.over_rate;  ��������*/
	g_ln_mst.intst_acm=0.00;

	g_ln_mst.in_lo_intst=0.00;  /**ǷϢ**/
	g_ln_mst.in_lo_acm=0.00;
	g_ln_mst.out_lo_intst=0.00;
	g_ln_mst.out_lo_acm=0.00;
	g_ln_mst.cmpd_lo_intst=0.00;
	g_ln_mst.cmpd_lo_acm=0.00;

	/**g_ln_mst.cal_code ָ����ϵ���븶ֵ **/
/****for xxxxxxxx****/
	if( cif_type=='1' || cif_type=='5' )
		g_ln_mst.cal_code[0]='1';
	else if( cif_type=='2' )
		g_ln_mst.cal_code[0]='2';
	else if( cif_type=='3' || cif_type=='4' || cif_type=='6' )
		g_ln_mst.cal_code[0]='3';
	else 
		g_ln_mst.cal_code[0]='2';

	g_ln_mst.opn_date=g_pub_tx.tx_date;
	g_ln_mst.ic_date=g_pub_tx.tx_date;
	g_ln_mst.lst_date=g_pub_tx.tx_date;
	/**g_ln_mst.mtr_date ; ��������**/

	if(!strcmp(g_pub_tx.intst_type,""))
	{
		strcpy(g_pub_tx.intst_type,g_ln_parm.intst_type);
	}
	strcpy(g_ln_mst.intst_type,g_pub_tx.intst_type); 

    if(!strcmp(g_pub_tx.intst_type,""))
	{
		strcpy(g_ln_mst.repay_type,g_ln_parm.ln_pay_type);
	}
	g_ln_mst.hst_cnt=0;
	g_ln_mst.hst_pg=0;
	/** �����˺Ÿ�ֵ ���**/

	/** չ�ڸ�ֵ **/
	strcpy(g_ln_mst.exp_ind,"0");
	g_ln_mst.exp_rate=0.00;
	g_ln_mst.exp_mtr_date=0;

	strcpy(g_ln_mst.ac_sts,g_ln_parm.ac_sts);
	strcpy(g_ln_mst.five_sts,"1");

	g_ln_mst.cif_no=g_pub_tx.cif_no;  /** �ͻ��Ÿ�ֵ **/
	strcpy(g_ln_mst.name,g_pub_tx.name);
	/**g_ln_mst.pact_no ��ͬ�Ÿ�ֵ **/
	strcpy( g_pub_tx.cur_no,g_ln_parm.cur_no );

		/* ��� ��ͬ�� */
		ret = Ln_mst_Sel( g_pub_tx.reply , &g_ln_mst , 
					" pact_no='%s' and ac_sts!='9' " , g_ln_mst.pact_no );
		if( ret && ret!=100)
		{
			sprintf(acErrMsg,"��ȡ�������ļ���Ϣ�쳣![%s][%s]",g_ln_mst.pact_no,g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
		else if ( ret == 0 )
		{
			sprintf(acErrMsg,"��ͬ���Ѵ���![%s][%s]",g_ln_mst.pact_no,g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L020");
			goto ErrExit;
		}

	/***�������**/
		/*��鿪�������Ƿ����������ں���ֹ����֮��*/	
		if ( (g_ln_parm.beg_date > g_ln_mst.opn_date) || \
			(g_ln_mst.opn_date > g_ln_parm.end_date))
		{	
			sprintf(acErrMsg,"����Ч��[%d]not in[%d][%d]",
				g_ln_mst.opn_date,g_ln_parm.beg_date,g_ln_parm.end_date);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A001" );
			goto ErrExit;
		}

		if( (g_ln_parm.cif_type[0]=='1' &&  cif_type!='1' && cif_type!='5' ) ||
			(g_ln_parm.cif_type[0]=='2' 
				&&  cif_type!='2' && cif_type!='3' && cif_type!='4' ) ||
			(g_ln_parm.cif_type[0]=='5' &&  cif_type!='5' )  )
		{
			if( cif_type!='\0' )
			{
			sprintf( acErrMsg,"NEED�ͻ����Ͳ���parm[%s] [%c]",
				g_ln_parm.cif_type,cif_type );
			WRITEMSG
			strcpy ( g_pub_tx.reply , "D112" );
			goto ErrExit;
			}
		}

		/***���޼��***/
		/* ���������С���ޣ�������� */	
		
		if ( g_ln_parm.term_type[0]=='Y' )      /* �������ͣ��� */
		{	
			g_ln_parm.min_term = g_ln_parm.min_term*12;
			g_ln_parm.max_term = g_ln_parm.max_term*12;
			
			pub_base_deadlineM( g_ln_mst.opn_date,g_ln_parm.min_term,&min_day);
			pub_base_deadlineM( g_ln_mst.opn_date,g_ln_parm.max_term,&max_day);
		}
		else if ( g_ln_parm.term_type[0]=='M' ) /* �������ͣ��� */
         	 {
			 TRACE
			 pub_base_deadlineM( g_ln_mst.opn_date,g_ln_parm.min_term,&min_day);
			 pub_base_deadlineM( g_ln_mst.opn_date,g_ln_parm.max_term,&max_day);
		  }
		else if ( g_ln_parm.term_type[0]=='D' )  /* �������ͣ��� */
		  {
			pub_base_deadlineD( g_ln_mst.opn_date,g_ln_parm.min_term,&min_day);
			pub_base_deadlineD( g_ln_mst.opn_date,g_ln_parm.max_term,&max_day);
		  }
		  sprintf(acErrMsg,"������[%ld]������[%ld]",g_ln_mst.opn_date,g_ln_mst.mtr_date);
		  WRITEMSG		  
		  sprintf(acErrMsg,"��С����[%ld]�������[%ld]",min_day,max_day);
		  WRITEMSG
		  /*lpj20151123 ��Ϊ�����ӵĶԹ������Ʒ�������������ͣ����ԶԹ���������������޼��*/
		if(strncmp(g_ln_mst.prdt_no,"3G",2)!=0)
		{
		if ( g_ln_mst.mtr_date<min_day )
		  {
			sprintf(acErrMsg,"[%ld][%ld]",min_day,max_day);
			WRITEMSG
			strcpy( acErrMsg,"��������С�ڹ涨����С����!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L003" );
			goto ErrExit;
		  }
       		 if ( g_ln_mst.mtr_date>max_day )
		  {
                strcpy( acErrMsg,"�������޴��ڹ涨���������!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "L004" );
				goto ErrExit;
		  }
		sprintf(acErrMsg,"���������С���ޣ��������(����) PASS!");
		WRITEMSG	
	}
		/*���ò�Ʒ��ͽ�����߽����ʱ���޶�*/
		if ( pub_base_CompDblVal(g_ln_mst.amt_lmt,g_ln_parm.min_bal)<0 )
		{
			sprintf(acErrMsg,"low [%.2lf]<[%.2lf]! ",
				g_ln_mst.amt_lmt,g_ln_parm.min_bal);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A003" );
			goto ErrExit;
		}
		else if( pub_base_CompDblVal(g_ln_mst.amt_lmt,g_ln_parm.max_bal)>0
			&& pub_base_CompDblVal(g_ln_parm.max_bal,0.00)!=0 )
		{
			sprintf(acErrMsg,"open bal overflow!! ");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A004" );
			goto ErrExit;
		}

		/** �����˺Ÿ�ֵ �����ǻ���**/
		/* ���ݱ��������˻���� */
		/**ret = pub_base_CurToSeqn( g_pub_tx.cur_no , &pay_ac_seqn );	***/	
		ret = pub_base_CurToSeqn( g_ln_parm.cur_no , &pay_ac_seqn );
		if ( ret )
		{
			sprintf(acErrMsg,"���ú������ݱ��������˻���Ŵ���!");
			WRITEMSG
			goto ErrExit;
		}
		g_ln_mst.repay_ac_id=0;
		pub_base_strpack( g_pub_tx.ac_no1);
		if( strlen(g_pub_tx.ac_no1) )
		{
			ret = pub_ln_AcCheck( g_pub_tx.ac_no1,pay_ac_seqn,
				&cif_no,&g_ln_mst.repay_ac_id );
			sprintf(acErrMsg,"�����ʺ�!!!![%s][%d]",g_pub_tx.ac_no1,pay_ac_seqn);
   			WRITEMSG	
			sprintf(acErrMsg,"�����ʺ�id!!!![%ld]",g_ln_mst.repay_ac_id);
   			WRITEMSG
			if( ret )
			{
				sprintf(acErrMsg,"���ú���pub_ln_AcCheck����!");
				WRITEMSG
				goto ErrExit;
			}
		/** ������Ƿ�һ���� **/
		/*
			if ( cif_no != g_pub_tx.cif_no )
			{
				sprintf(acErrMsg,"���ݿ��Ŀͻ���[%d]�뻹Ϣ���ͻ���[%d]����",
					g_pub_tx.cif_no,cif_no );
				WRITEMSG
				strcpy(g_pub_tx.reply,"L019");
				goto ErrExit;
			 }*/
		}
		/**��������֤���Ϳ����Ƿ����**/
		if( strlen(g_pub_tx.id_type) )
		{
			if( pub_cif_CifCheck(g_pub_tx.id_type,g_pub_tx.id_no,
				&cif_no,v_cif_type) )
			{
				sprintf(acErrMsg,"�ÿͻ��ڿͻ�����δ�Ǽ�![%s][%s]",
					g_pub_tx.id_type,g_pub_tx.id_no);
				WRITEMSG
				goto ErrExit;
			}
			/**������Ƿ�һ����**/
			/*if ( cif_no != g_pub_tx.cif_no )
			{
				sprintf(acErrMsg,"����֤���Ŀͻ���[%d]�뻹Ϣ���ͻ���[%d]����",
					g_pub_tx.cif_no,cif_no );
				WRITEMSG
				strcpy(g_pub_tx.reply,"L019");
				goto ErrExit;
			}*/
		}
	   /***ȡ���ʱ�Ŷ�Ӧ������***/
	   ret = pub_base_getllz(g_ln_parm.rate_no,g_ln_parm.cur_no,
					g_pub_tx.tx_date,&rate_val);
	   if( ret ) goto ErrExit;
	   sprintf(acErrMsg,"ת��ǰ����[%lf]",rate_val);
	   WRITEMSG

	   rate_val = rate_val/1.2;/* ת���������� */ 
	   sprintf(acErrMsg,"ת������������[%lf]",rate_val);
	   WRITEMSG

	   
	   if( g_ln_parm.over_rate_type[0]=='0' )/* ȡ�������ʱ�� */
	   {
	   		ret = pub_base_getllz(g_ln_parm.over_rate_no,g_ln_parm.cur_no,
							g_pub_tx.tx_date,&over_rate_val);
	   		if( ret ) goto ErrExit;					
	   		over_rate_val = over_rate_val/1.2;/* ת���������� */
	   }else{
			over_rate_val = g_ln_mst.rate ;
	   }
	   /** ȡ��Ϣ���� **/
	   if( g_ln_parm.fine_rate_type[0]=='0' )/* ȡ�������ʱ�� */
	   {
	   		ret = pub_base_getllz(g_ln_parm.fine_rate_no,g_ln_parm.cur_no,
							g_pub_tx.tx_date,&fine_rate_val);
	   		if( ret ) goto ErrExit;					
	   		fine_rate_val = fine_rate_val/1.2;/* ת���������� */
	   }else{
			fine_rate_val = g_ln_mst.rate ;
	   }
		sprintf(acErrMsg,"��������[%s]",g_ln_parm.over_rate_no);
		WRITEMSG
		sprintf(acErrMsg,"��Ϣ����[%s]",g_ln_parm.fine_rate_no);
		WRITEMSG
		sprintf(acErrMsg,"[%s]",g_ln_parm.cur_no);
		WRITEMSG
	 	sprintf(acErrMsg,"[%ld]",g_pub_tx.tx_date);
		WRITEMSG


		if( g_ln_parm.get_rate_type[0]=='0' ) /**��������**/
		{
			g_ln_mst.rate=rate_val;
		}else{		/**�������ʼ���ϸ��¸�����**/  
			/*�������ʼ��*/
			/* modified by liuxuan 2010-4-16 �Ŵ��������ۺ�ҵ�񲻹�
			if( pub_base_CompDblVal(g_ln_mst.rate,
					rate_val*( (100+g_ln_parm.max_flt_ratio)/100.00 ) )>0 )
			{
				vtcp_log("=[%d][%s]",g_ln_parm.max_flt_ratio,g_ln_mst.prdt_no );
				sprintf(acErrMsg,"current rate is too high[%lf][%lf][%d] ",
					rate_val,rate_val*((100+g_ln_parm.max_flt_ratio)/100.00)
							,g_ln_parm.max_flt_ratio );
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A006" );
				goto ErrExit;
			}
            sprintf(acErrMsg,"���������ȷʱ[%lf][%lf][%d] ", rate_val,
			rate_val*((100+g_ln_parm.max_flt_ratio)/100.00) ,g_ln_parm.max_flt_ratio );
			WRITEMSG

			if( pub_base_CompDblVal(g_ln_mst.rate,
					rate_val*((100+g_ln_parm.min_flt_ratio)/100.00) )<0 )
			{
			sprintf(acErrMsg,"������ʴ���ʱ[%lf][%lf][%d] ", rate_val,
		 rate_val*((100+g_ln_parm.min_flt_ratio)/100.00) ,g_ln_parm.min_flt_ratio );
						 WRITEMSG
				strcpy ( g_pub_tx.reply , "A005" );
				goto ErrExit;
			}
			sprintf(acErrMsg,"���������ȷʱ[%lf][%lf][%d] ", rate_val,
		rate_val*((100+g_ln_parm.min_flt_ratio)/100.00) ,g_ln_parm.min_flt_ratio );
			WRITEMSG
			*/
			/**�������ʼ��**/
			/*
			if( pub_base_CompDblVal(g_ln_mst.over_rate,
					over_rate_val*(100+g_ln_parm.over_rate_max)/100.00 )>0 )
			{
				vtcp_log("=[%d][%s]",g_ln_parm.over_rate_max,g_ln_mst.prdt_no );
				sprintf(acErrMsg,"current rate is too high[%lf][%d][%lf] ",
				g_ln_mst.over_rate,g_ln_parm.over_rate_max,
				over_rate_val*(100+g_ln_parm.over_rate_max)/100.00
							 );
				WRITEMSG
				strcpy ( g_pub_tx.reply , "L062" );
				goto ErrExit;
			}
			if( pub_base_CompDblVal(g_ln_mst.over_rate,
					over_rate_val*((100+g_ln_parm.over_rate_min)/100.00) )<0 )
			{
				sprintf(acErrMsg,"current rate is too high[%lf][%lf]!!",
						g_ln_mst.over_rate,
						over_rate_val*((100+g_ln_parm.over_rate_min)/100.00));
				WRITEMSG
				strcpy ( g_pub_tx.reply , "L061" );
				goto ErrExit;
			}			
			*/
			/**��Ϣ���ʼ��**/
			/*
			if( pub_base_CompDblVal(g_ln_mst.fine_rate,
					fine_rate_val*(100+g_ln_parm.fine_rate_max)/100.00 )>0 )
			{
				vtcp_log("=[%d][%s]",g_ln_parm.fine_rate_max,g_ln_mst.prdt_no );
				sprintf(acErrMsg,"current rate is too high[%lf][%d][%lf] ",
				g_ln_mst.fine_rate,g_ln_parm.fine_rate_max,
				fine_rate_val*(100+g_ln_parm.fine_rate_max)/100.00
							 );
				WRITEMSG
				strcpy ( g_pub_tx.reply , "L163" );
				goto ErrExit;
			}
			if( pub_base_CompDblVal(g_ln_mst.fine_rate,
					fine_rate_val*((100+g_ln_parm.fine_rate_min)/100.00) )<0 )
			{
				sprintf(acErrMsg,"current rate is too high[%lf]!!",
						fine_rate_val*((100+g_ln_parm.fine_rate_min)/100.00));
				WRITEMSG
				strcpy ( g_pub_tx.reply , "L162" );
				goto ErrExit;
			}
			*/
		}

		/* ��鿪�������������� (���ú���) *
		g_dd_parm.opn_lmt */
		TRACE
		if( pub_ins_ln_hst( 0 ) )
		{
			sprintf( acErrMsg,"�ǼǴ����Ʒ��ϸ����!" );
			WRITEMSG
			goto ErrExit;
		}
		TRACE
	/**����������**/
	ret=Ln_mst_Ins(g_ln_mst,g_pub_tx.reply);	
	if( ret )
	{
		Ln_mst_Debug(&g_ln_mst);



		sprintf(acErrMsg,"�ǼǴ����Ʒ���ļ�����![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L220");
		goto ErrExit;
	}
TRACE
	/*** �Ǽǿ������Ǽǲ� ***/
	if( pub_base_InsMo_opn_cls(g_ln_mst.name,"","1") )
	{
		sprintf(acErrMsg,"�Ǽǿ������Ǽǲ�����!");
		WRITEMSG
		goto ErrExit;
	}
TRACE
	return 0;
ErrExit:
	return 1;
}

int pub_ins_in_mst()
{
	int ret;
	double rate_val;
	struct com_item_c v_com_item;
	memset(&v_com_item,0x00,sizeof(struct com_item_c));

	g_in_mst.ac_id=g_pub_tx.ac_id;
	g_in_mst.ac_seqn=g_pub_tx.ac_seqn;
	strcpy(g_in_mst.opn_br_no,g_pub_tx.opn_br_no);
	strcpy(g_in_mst.prdt_no,g_pub_tx.prdt_code);
	g_in_mst.bal=g_pub_tx.tx_amt1;
	g_in_mst.ys_bal=0.00;
	g_in_mst.hst_bal=g_pub_tx.tx_amt1;	
	g_in_mst.rate=g_pub_tx.rate;
	g_in_mst.over_rate=g_pub_tx.over_rate;
	if( g_in_mst.over_rate<0.005 && g_in_mst.over_rate>-0.005 )
		g_in_mst.over_rate=g_in_mst.rate;
	g_in_mst.opn_date=g_pub_tx.tx_date;
	g_in_mst.ic_date=g_pub_tx.tx_date;
	g_in_mst.lst_date=g_pub_tx.tx_date;
	g_in_mst.end_date=g_pub_tx.mach_date;
	strcpy(g_in_mst.intst_knd,g_pub_tx.tmp_ind);

	/***�������**/
		/*��鿪�������Ƿ����������ں���ֹ����֮��*/	
		if ( (g_in_parm.star_date > g_in_mst.opn_date) || \
			(g_in_mst.opn_date > g_in_parm.stop_date))
		{	
			sprintf(acErrMsg,"����Ч��[%d]not in[%d][%d]",
				g_in_mst.opn_date,g_in_parm.star_date,g_in_parm.stop_date);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A001" );
			goto ErrExit;
		}

		/*���ò�Ʒ��ͽ�����߽��*/
		/****NEED�����Ƿ�Ҫ��**
		if( pub_base_CompDblVal(g_in_mst.bal,g_in_parm.min_amt)<0 )
		{
			sprintf(acErrMsg,"open bal is too low!! ========================
					g_in_mst.bal=[%.2lf],g_in_parm.min_amt=[%.2lf]", \
					g_in_mst.bal,g_in_parm.min_amt);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A003" );
			goto ErrExit;
		}
		else if( pub_base_CompDblVal(g_in_mst.bal,g_in_parm.max_amt)>0 )
		{
			sprintf(acErrMsg,"open bal overflow!! ");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A004" );
			goto ErrExit;
		}
		**/

		/***���������޵ļ��***/
		/***NEED***/
		if( g_in_mst.end_date<=g_in_mst.opn_date && g_in_mst.end_date!=0 )
		{
			sprintf(acErrMsg,"�����մ�[%d] [%d]! ",
				g_in_mst.end_date,g_in_mst.opn_date );
			WRITEMSG
			strcpy ( g_pub_tx.reply , "B028" );
			goto ErrExit;
		}

		/*��������Ƿ񳬹��涨�����ϸ����¸���Ľ���*/
		/***ȡ���ʱ�Ŷ�Ӧ������***/
		sprintf(acErrMsg,"����rate_no[%s]cur_no[%s]tx_date[%ld]",
				g_in_parm.rate_no,g_in_parm.cur_no,
			g_pub_tx.tx_date );
			WRITEMSG		
		ret = pub_base_getllz(g_in_parm.rate_no,g_in_parm.cur_no,
			g_pub_tx.tx_date,&rate_val);
		if( ret ) goto ErrExit;

		/**�ڲ�������ȫ�����䣬���ʼ���ϸ��¸�����**
			if( pub_base_CompDblVal(g_in_mst.rate,
				rate_val*(1+g_in_parm.max_flot_rate) )>0 )
			{
				sprintf(acErrMsg,"rate[%lf] is too high [%lf][%lf]!! ",
					g_in_mst.rate,rate_val,g_in_parm.max_flot_rate);
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A006" );
				goto ErrExit;
			}
			if( pub_base_CompDblVal(g_in_mst.rate,
				rate_val*(1-g_in_parm.min_flot_rate) )<0 )
			{
				sprintf(acErrMsg,"current rate is too high!! ");
				WRITEMSG
sprintf(acErrMsg,"g_in_mst.rate=[%lf],min_flot_rate=[%lf],rate_val=[%lf]",
        g_in_mst.rate,g_in_parm.min_flot_rate,rate_val);
WRITEMSG
				strcpy ( g_pub_tx.reply , "A005" );
				goto ErrExit;
			}
			**********/

		/***ȡ��Ϣ���ʱ�Ŷ�Ӧ������***/
		ret = pub_base_getllz(g_in_parm.fine_rate_no,g_in_parm.cur_no,
			g_pub_tx.tx_date,&rate_val);
		if( ret ) goto ErrExit;

		/**�ڲ�������ȫ�����䣬���ʼ���ϸ��¸�����**/
		sprintf(acErrMsg, "-----> max=[%lf], min=[%lf] , current=[%lf]",
			rate_val*(1+g_in_parm.max_flot_rate), 
			rate_val*(1-g_in_parm.min_flot_rate), g_in_mst.over_rate);
		WRITEMSG

		/*********
			if( pub_base_CompDblVal(g_in_mst.over_rate,
				rate_val*(1+g_in_parm.max_flot_rate) )>0 )
			{
				sprintf(acErrMsg,"current rate is too high!! ");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A006" );
				goto ErrExit;
			}
			if( pub_base_CompDblVal(g_in_mst.over_rate,
				rate_val*(1-g_in_parm.min_flot_rate) )<0 )
			{
				sprintf(acErrMsg,"current rate is too high!! ");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A005" );
				goto ErrExit;
			}
		**************/

		/* ��鿪�������������� (���ú���) *
		g_in_parm.opn_lmt */

	/** xxx ���ÿ�Ŀ�Ƿ��Ƕ��û���Ŀ**/
	ret=Com_item_Sel(g_pub_tx.reply,&v_com_item,"acc_hrt='%s'",g_in_parm.acc_hrt);
	if( ret )
	{
		sprintf(acErrMsg,"com_item_sel erro [%s][%d]!!",g_in_parm.acc_hrt,ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "O074" );
		goto ErrExit;
	}
	if( v_com_item.acc_knd[0]!='2' )
	{
		sprintf(acErrMsg,"�ÿ�Ŀ�������˺�!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P302");
		goto ErrExit;
	}
	if(v_com_item.sub_acc_yn[0]=='Y')
	{
		sprintf(acErrMsg,"�ÿ�Ŀ�����¼���Ŀ!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P301");
		goto ErrExit;
	}
	if( strlen(g_pub_tx.name)==0 )
		strcpy(g_pub_tx.name,v_com_item.acc_name);
	/**����������**/

	strcpy(g_pub_tx.add_ind,"1");
	/*** ������Ϣ�����ĺ��� Ĭ��Ϊ0��ʵ������2�䶯�ۻ��� ***/
	g_in_mst.intst_acm=0.00;
	if( pub_base_CalAcm(g_pub_tx.tx_date,g_pub_tx.tx_date,"0","2",
		0.00,g_pub_tx.tx_amt1,&g_in_mst.intst_acm,
		g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) )
	{
		sprintf(acErrMsg,"������Ϣ��������!");
		WRITEMSG
		goto ErrExit;
	}
	/***/

	/**** add by xxx 20030715 ***
	if (g_pub_tx.add_ind[0] == '0') * �� *
	{
		if( v_com_item.dc_ind[0]=='2' ) 
		{ 
			*** ���ü�����Ϣ�������� ***
			if( pub_base_CalAcm(g_pub_tx.tx_date,g_pub_tx.tx_date, "0", "2",
				0.00,-g_pub_tx.tx_amt1,&g_in_mst.intst_acm, 
				g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) ) 
			{ 
				sprintf(acErrMsg,"������Ϣ��������!"); 
				WRITEMSG 
				goto ErrExit; 
			} 
			
			strcpy(g_pub_tx.add_ind,"0"); 
		} 
		else
		{
			*** ���ü�����Ϣ�������� *** 
			if( pub_base_CalAcm(g_pub_tx.tx_date,g_pub_tx.tx_date, "0", "2",
				0.00,g_pub_tx.tx_amt1,&g_in_mst.intst_acm, 
				g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) ) 
			{ 
				sprintf(acErrMsg,"������Ϣ��������!"); 
				WRITEMSG 
				goto ErrExit; 
			} 
			
			strcpy(g_pub_tx.add_ind,"1");
		}

	}
	else if (g_pub_tx.add_ind[0] == '1') * �� *
	{
		if( v_com_item.dc_ind[0]=='2' ) 
		{ 
			*** ���ü�����Ϣ�������� ***
			if( pub_base_CalAcm(g_pub_tx.tx_date,g_pub_tx.tx_date, "0", "2",
				0.00,g_pub_tx.tx_amt1,&g_in_mst.intst_acm, 
				g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) ) 
			{ 
				sprintf(acErrMsg,"������Ϣ��������!"); 
				WRITEMSG 
				goto ErrExit; 
			} 

			strcpy(g_pub_tx.add_ind,"1"); 
		} 
		else 
		{
			*** ���ü�����Ϣ�������� ***
			if( pub_base_CalAcm(g_pub_tx.tx_date,g_pub_tx.tx_date, "0", "2",
				0.00,-g_pub_tx.tx_amt1,&g_in_mst.intst_acm, 
				g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) ) 
			{ 
				sprintf(acErrMsg,"������Ϣ��������!"); 
				WRITEMSG 
				goto ErrExit; 
			} 

			strcpy(g_pub_tx.add_ind,"0");
		}
	}
	else 
	{ 
		sprintf(acErrMsg,"��ȡ��־����g_pub_tx.add_ind=[%s]!",
				g_pub_tx.add_ind); 
		WRITEMSG 
		strcpy(g_pub_tx.reply,"P106"); 
		goto ErrExit; 
	}
	*** add by xxx end ****/


	g_in_mst.hst_cnt=0;
	g_in_mst.hst_pg=0;
	strcpy(g_in_mst.sts,"1");
	g_in_mst.hold_amt=0.00;
	/* modified by liuxuan �Ƿ��͸֧ȡ��Ŀ������ ��ǰ��ֱ�Ӹ���N*/
	g_in_mst.od_ind[0]=v_com_item.od_ind[0];
	/* modify over */
	strcpy(g_in_mst.name,g_pub_tx.name);

	ret=pub_ins_in_hst();
	if( ret )
	{
		sprintf(acErrMsg,"�Ǽ��ڲ���ϸ�ļ�����![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	ret=In_mst_Ins(g_in_mst,g_pub_tx.reply);	
	if( ret )
	{
		sprintf(acErrMsg,"�Ǽ��ڲ���Ʒ���ļ�����![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P144");
		goto ErrExit;
	}

	/*** �Ǽǿ������Ǽǲ� ***/
	if( pub_base_InsMo_opn_cls(g_in_mst.name,"",g_in_mst.sts) )
	{
		sprintf(acErrMsg,"�Ǽǿ������Ǽǲ�����!");
		WRITEMSG
		goto ErrExit;
	}

	return 0;
ErrExit:
	return 1;
}

static int pub_sub_init()
{
	int ret;
	/**��ȡ��Ʒ����**/
	switch(g_pub_tx.prdt_code[0])
	{
		case '1' :	
			ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,
				"prdt_no='%s'",g_pub_tx.prdt_code);
			strcpy( g_pub_tx.cur_no,g_dd_parm.cur_no);
			strcpy( g_pub_tx.dc_code,g_dd_parm.dc_code);
			break;
		case '2' :	
			ret=Td_parm_Sel(g_pub_tx.reply,&g_td_parm,
				"prdt_no='%s'",g_pub_tx.prdt_code);
			strcpy( g_pub_tx.cur_no,g_td_parm.cur_no);
			strcpy( g_pub_tx.dc_code,g_td_parm.dc_code);
			break;
		case '3' :	
			ret=Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm,
				"prdt_no='%s'",g_pub_tx.prdt_code);
			strcpy( g_pub_tx.cur_no,g_ln_parm.cur_no);
			strcpy( g_pub_tx.dc_code,g_ln_parm.dc_code);
			break;
		case '9' :	
			ret=In_parm_Sel(g_pub_tx.reply,&g_in_parm,
				"prdt_no='%s'",g_pub_tx.prdt_code);
			strcpy( g_pub_tx.cur_no,g_in_parm.cur_no);
			strcpy( g_pub_tx.dc_code,g_in_parm.dc_code);
			break;
		default  :	
			sprintf(acErrMsg,"�˻����ʹ�[%s]!",g_pub_tx.prdt_code);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P104");
			goto ErrExit;
	}
	if( ret==100 )
	{
		sprintf(acErrMsg,"��Ʒ���벻����![%s]",g_pub_tx.prdt_code);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P145");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"ȡ��Ʒ���������![%s][%d]",g_pub_tx.prdt_code,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P146");
		goto ErrExit;
	}

	set_zd_data(DC_CODE,g_pub_tx.dc_code );

	return 0;
ErrExit:
	return 1;
}
static int pub_sub_chk_mdm()
{
	char v_cif_type[2];
	int ret;

	/****����****/
	if( g_pub_tx.tx_date>g_mdm_ac_rel.end_date )
	{
		sprintf(acErrMsg,"�ý����Ѿ����ڣ�������!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P148");
		goto ErrExit;
	}

	/*** ���ʲ�Ʒ��麯��,���ò�Ʒ������Ƿ�ƥ�� ***/
	/* if( strcmp(g_mdm_ac_rel.mdm_code,NULL_MDM_CODE) ) */
	if( g_pub_tx.ac_wrk_ind[0]=='9' )
		;
	else
	{
		if( pub_base_check_promedi(g_pub_tx.prdt_code,
			g_mdm_ac_rel.mdm_code,NULL_MDM,NULL_MDM) )
		{
			sprintf(acErrMsg,"�ý��ʲ��ܴ˲�Ʒ[%s][%s]!",
				g_mdm_ac_rel.mdm_code,g_pub_tx.prdt_code);
			WRITEMSG
			goto ErrExit;
		}
	}

	/**�ͻ����ļ��**/
	if( g_pub_tx.prdt_code[0]!='9' )
	{
		vtcp_log("[%s][%d][%s]\n",__FILE__,__LINE__,g_mdm_ac_rel.id_type);
		pub_base_strpack( g_mdm_ac_rel.id_type );
		if( strlen(g_mdm_ac_rel.id_type) )
		{
			if( pub_cif_CifCheck(g_mdm_ac_rel.id_type,g_mdm_ac_rel.id_no,
				&g_pub_tx.cif_no,v_cif_type) )
			{
				sprintf(acErrMsg,"�ÿͻ��ڿͻ�����δ�Ǽ�![%s][%s]",
					g_pub_tx.id_type,g_pub_tx.id_no);
				WRITEMSG
				goto ErrExit;
			}
			 vtcp_log("[%s][%d][%ld]\n",__FILE__,__LINE__,g_pub_tx.cif_no);

		}
		else
			g_pub_tx.cif_no=0;
		vtcp_log("[%s][%d][%ld]\n",__FILE__,__LINE__,g_pub_tx.cif_no);
	}
	else
		g_pub_tx.cif_no=0;

	cif_type=v_cif_type[0];
	if(g_pub_tx.cif_no==0)/*���ʻ�2007-6-8 9:33**/
  {
  	cif_type='2';
  }
	g_pub_tx.ac_id_type[0]=g_pub_tx.prdt_code[0];
	g_pub_tx.ac_id_type[1]='\0';
 vtcp_log("[%s][%d][%ld]\n",__FILE__,__LINE__,g_pub_tx.cif_no);
	return 0;
ErrExit:
	return 1;
}
