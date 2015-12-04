/***************************************************************
* �� �� ��:     pubf_acct_entry.c
* ����������
*		�������ȶ���ķ�¼�ǼǷ�¼��ˮ���޸��ڲ��ֻ����
*
* ��    ��:	xxx
* ������ڣ�    2003��10��21��
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
#include "svrpub_c.h"
#include "tx_dc_rel_c.h"
#include "tx_dc_br_rel_c.h"
#include "dc_log_c.h"
#include "dc_branch_c.h"
#include "dc_entry_c.h"
#include "dc_acc_rel_c.h"
#include "com_sys_parm_c.h"
#include "com_item_c.h"
#include "gl_mst_c.h"
#include "com_item_c.h"
#include "com_item_add_c.h"
#include "zjgl_mst_c.h"
pubf_acct_proc(char *sub_tx_code)
{
    
	struct tx_dc_rel_c dc_rel;
	memset(&dc_rel,0x00,sizeof(struct tx_dc_rel_c));
	
	vtcp_log("���������������[%s][%d],sub_tx_code [%s]\n",__FILE__,__LINE__,sub_tx_code);
	vtcp_log("%s,%d tx_br_no=[%s],opn_br_no=[%s]",__FILE__,__LINE__,g_pub_tx.tx_br_no,g_pub_tx.opn_br_no);
	if(memcmp(sub_tx_code,"SF00",4)==0
		||memcmp(sub_tx_code,"SF01",4)==0
	/* rem by LiuHuafeng 20070117
		||memcmp(sub_tx_code,"SF02",4)==0
	******SF02��������ʺŵ�����Ҫ���ʺ�Ϊ��****************************/
		||memcmp(sub_tx_code,"N001",4)==0)	/* ��ƾ֤�ý��׻��� modified by liuxuan 2006-12-13*/
	{
		vtcp_log("ע��ע��[%s][%d]���ӽ���[%s]���׻����Ϳ�������һ��\n",__FILE__,__LINE__,sub_tx_code);
		memcpy(g_pub_tx.opn_br_no,g_pub_tx.tx_br_no,sizeof(g_pub_tx.opn_br_no));
	}
	
	RespCode=pubf_acct_init();
	if ( RespCode )
	{
		sprintf(acErrMsg,"��ʼ������!!RespCode=[%s]",RtCode);
		WRITEMSG
		return 1;
	}

	RespCode=Tx_dc_rel_Sel(RtCode,&dc_rel,
		"sub_tx_code='%s' and dc_code='0000'",sub_tx_code);
	if ( RespCode )
	{
		sprintf(acErrMsg,"��ȡ��Ʒ��ƹ�ϵ�����!!RespCode=[%ld][%s]",
			RespCode,sub_tx_code);
		WRITEMSG
		strcpy ( RtCode , "S001" );
		strcpy ( g_pub_tx.reply , "S001" );
		return 1;
	}

	vtcp_log("%s,%d tx_br_no=[%s],opn_br_no=[%s]",__FILE__,__LINE__,g_pub_tx.tx_br_no,g_pub_tx.opn_br_no);
	if ( pubf_acct_entry(sub_tx_code,dc_rel.entry_code) )
	{
		vtcp_log("��¼��ˮ��������");
		WRITEMSG
		strcpy ( RtCode , g_pub_tx.reply );
		return 1;
	}

	return 0;
}
static pubf_acct_init()
{
	struct dc_acc_rel_c acc_rel;

	memset(&acc_rel,0x00,sizeof(struct dc_acc_rel_c));

	get_zd_data(DC_CODE,acc_rel.dc_code);

	sprintf(acErrMsg,"DC_INIT [%s]",acc_rel.dc_code);
	WRITEMSG
	sprintf(acErrMsg,"DC_INIT [%s]",DC_CODE);
	WRITEMSG
	
	if( !strncmp(acc_rel.dc_code,"ACHR",4) )
		return 0;

	RespCode=Dc_acc_rel_Dec_Sel(RtCode,"dc_code='%s'",acc_rel.dc_code);
	if ( RespCode )
	{
		sprintf(acErrMsg,"��ȡ��ƴ�����ձ����!!RespCode=[%ld][%s]",
			RespCode,acc_rel.dc_code);
		WRITEMSG
		strcpy ( RtCode , "S050" );
		strcpy ( g_pub_tx.reply , "S050" );
		return 1;
	}
	while(1)
	{
		RespCode=Dc_acc_rel_Fet_Sel(&acc_rel,RtCode);
		if ( RespCode <0 )
		{
			Dc_acc_rel_Clo_Sel ();
			sprintf(acErrMsg,"��ȡ��ƴ�����ձ����!!RespCode=[%ld][%s]",
				RespCode,acc_rel.dc_code);
			WRITEMSG
			strcpy ( RtCode , "S051" );
			strcpy ( g_pub_tx.reply , "S051" );
		}
		else if ( RespCode >0 )
		{
			break;
		}
		set_zd_data(acc_rel.data_code,acc_rel.acc_hrt);

		sprintf(acErrMsg,"DC_INIT [%s][%s]",acc_rel.data_code,acc_rel.acc_hrt);
		WRITEMSG
	}
	Dc_acc_rel_Clo_Sel ();

	return 0;
}
static pubf_acct_entry(char *sub_tx_code,char *entry_code)
{
	struct dc_log_c dc_log;
	struct dc_branch_c dc_branch;
	struct tx_dc_br_rel_c br_rel;

	vtcp_log("%s,%d tx_br_no=[%s],opn_br_no=[%s]",__FILE__,__LINE__,g_pub_tx.tx_br_no,g_pub_tx.opn_br_no);
	memset(&dc_log,0x00,sizeof(struct dc_log_c));
	memset(&dc_branch,0x00,sizeof(struct dc_branch_c));
	memset(&br_rel,0x00,sizeof(struct tx_dc_br_rel_c));

	dc_log.trace_no=g_pub_tx.trace_no;
	get_zd_long( DC_DATE , &dc_log.tx_date);
	get_zd_long( DC_TRACE_CNT , &g_pub_tx.trace_cnt);
	dc_log.trace_cnt=g_pub_tx.trace_cnt;
	strcpy(dc_log.cur_no,g_pub_tx.cur_no);
	vtcp_log("[%s][%d]dc_log=[%s]",__FILE__,__LINE__,dc_log.cur_no);
	strcpy(dc_log.entry_code,entry_code);
	if(Tx_dc_br_rel_Sel(g_pub_tx.reply,
		&br_rel,"tx_br_no='%s'",g_pub_tx.tx_br_no))
	{
		vtcp_log("ȡ��ƻ��������[%s]",g_pub_tx.tx_br_no);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "S029" );
		return 1;
	}

	RespCode=Dc_branch_Sel(g_pub_tx.reply,
		&dc_branch,"br_no='%s'",br_rel.dc_br_no);
	if ( RespCode )
	{
		vtcp_log("ȡ��ƻ��������[%s]",br_rel.dc_br_no);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "S032");
		return 1;
	}
	strcpy(dc_log.tx_br_no,br_rel.dc_br_no);
	if ( strncmp ( g_pub_tx.tx_br_no , g_pub_tx.opn_br_no , 5 ) )
	{
		if(Tx_dc_br_rel_Sel(g_pub_tx.reply,
			&br_rel,"tx_br_no='%s'",g_pub_tx.opn_br_no))
		{
			vtcp_log("��ѯ�������ƻ������ձ����!! [%s]",g_pub_tx.opn_br_no);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "S029" );
			return 1;
		}
		RespCode=Dc_branch_Sel(g_pub_tx.reply,
			&dc_branch,"br_no='%s'",br_rel.dc_br_no);
		if ( RespCode )
		{
			vtcp_log("ȡ��ƻ��������[%s]",br_rel.dc_br_no);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "S032");
			return 1;
		}
		strcpy(dc_log.opn_br_no,br_rel.dc_br_no);
	}
	else
	{
		strcpy(dc_log.opn_br_no,dc_log.tx_br_no);
	}
	vtcp_log("%s,%d tx_br_no=[%s],opn_br_no=[%s]",__FILE__,__LINE__,g_pub_tx.tx_br_no,g_pub_tx.opn_br_no);

	strcpy(dc_log.tx_tx_br_no,g_pub_tx.tx_br_no);
	strcpy(dc_log.tx_opn_br_no,g_pub_tx.opn_br_no);
	strcpy(dc_log.ct_ind,g_pub_tx.ct_ind);
	strcpy(dc_log.note_type,g_pub_tx.note_type);
	/**** modify 20110521
	strcpy(dc_log.note_no,g_pub_tx.beg_note_no);
	****/
	memcpy(dc_log.note_no,g_pub_tx.beg_note_no,16);
	strcpy(dc_log.sts,"0");

	/** �жһ�Ʊ��Ʊ���⴦�������������������ҵ�񣬽��׻�����Ϊ�������� **/
	if(!strncmp(g_pub_tx.tx_br_no,QS_BR_NO,5) && \
	   !strncmp(g_pub_tx.tx_code,"4334",4))
	{
		if( !strncmp(g_pub_tx.sub_tx_code,"D003",4) ||\
		    !strncmp(g_pub_tx.sub_tx_code,"D099",4) )
		{
				memset(dc_log.tx_tx_br_no,0,sizeof(dc_log.tx_tx_br_no));
				strcpy(dc_log.tx_tx_br_no,dc_log.tx_opn_br_no);
		}
	}

	if ( dc_entry_proc(&dc_log) )
	{
		vtcp_log("�����Զ���¼��ˮ����");
		WRITEMSG
		return 1;
	}

	if ( strncmp ( dc_log.opn_br_no , dc_log.tx_br_no , 5 ) )
	{
		if ( dc_entry_branch ( sub_tx_code , dc_log ) )
		{
			vtcp_log("�������¼����");
			WRITEMSG
			return 1;
		}
	}

	return 0;
}

static dc_entry_proc(dc_log)
struct dc_log_c *dc_log;
{
	int ret=0;
	int tcnt=0;/* add by martin ����trace_log 2008/12/25 13:42:26*/
	char ttmp[3];
	struct dc_entry_c dc_entry;

	memset(&dc_entry,0x00,sizeof(struct dc_entry_c));

	if ( Dc_entry_Dec_Sel(g_pub_tx.reply,
		"entry_code='%s' order by entry_code,seqn",dc_log->entry_code) )
	{
		vtcp_log ( "�����Զ���¼�����α����" );
		WRITEMSG
		strcpy ( g_pub_tx.reply , "S030" );
		return 1;
	}
	if ( Dc_entry_Fet_Sel ( &dc_entry , g_pub_tx.reply ) )
	{
		vtcp_log ( "ȡ���Զ���¼�������[%s]",dc_log->entry_code );
		WRITEMSG
		strcpy ( g_pub_tx.reply , "S031" );
		return 1;
	}
	while (1)
	{
		pub_base_strpack(dc_entry.amt_no);
		dc_entry.amt_no[strlen(dc_entry.amt_no)]='\0';
			vtcp_log ( "�ǼǷ�¼��ˮtcnt=[%d],dc_log->amt[%lf]",tcnt,dc_log->amt );
			WRITEMSG
		/* add by martin ����trace_log 2008/12/25 13:42:26*/
		if ( dc_log_getentry ( dc_log , dc_entry ) )
		{
			vtcp_log ( "�ǼǷ�¼��ˮ����" );
			WRITEMSG
			return 1;
		}
		if( tcnt>0 &&( dc_log->amt<-0.005 || dc_log->amt>0.005 ) )
			{
				vtcp_log ( "�ǼǷ�¼��ˮ�ɹ�",tcnt,dc_log->amt );
				sprintf(ttmp,"%d",tcnt);
				if(strlen(g_pub_tx.brf)+strlen(ttmp)<=20)/**2009/1/20 20:56:46 add by martin ���Ӷ���brf���ȵ��ж�**/
					{
				    strcat(g_pub_tx.brf,ttmp );
			    }
				/* modify  by martin 2008/12/28 17:25:31 ��Ӱ�쳷������ˮ���Խ����Ϊ0.00 */
	      ret=pub_ins_ln_trace_log(0.00);
	      if( ret ) 
	      {
	   	    vtcp_log ( "�Ǽ�ҵ����ˮ����" );
			    WRITEMSG
	   	    return 1;
	      }
	      dc_log->trace_cnt=g_pub_tx.trace_cnt;
	   }
	   /* end add by martin ����trace_log 2008/12/25 13:42:26*/
		if ( dc_log_entry ( dc_log , dc_entry ) )
		{
			vtcp_log ( "�ǼǷ�¼��ˮ����" );
			WRITEMSG
			return 1;
		}
		if( dc_log->amt<-0.005 || dc_log->amt>0.005 )/* add by martin ����trace_log 2008/12/25 13:42:26*/
			{
		   tcnt++;
		  
	    vtcp_log ( "�ǼǷ�¼��ˮtcnt=[%d]",tcnt );
			WRITEMSG
			}	
		ret = Dc_entry_Fet_Sel ( &dc_entry , g_pub_tx.reply ) ;
		if( ret==100 ) break;
		else if( ret )
		{
			vtcp_log ( "ȡ��Ʒ�¼����" );
			WRITEMSG
			return 1;
		}
	}
	Dc_entry_Clo_Sel();

	vtcp_log ( "�Զ���¼�����˳�g_pub_tx.reply[%s]",g_pub_tx.reply );
	memcpy(g_pub_tx.reply, "0000", 4);/*��������������ӦΪ��0000��Added by YangMeng 201400704*/
	vtcp_log ( "�Զ���¼�����˳�g_pub_tx.reply[%s]",g_pub_tx.reply );
	
	return 0;
}
static dc_log_entry(dc_log,dc_entry)
struct dc_log_c *dc_log;
struct dc_entry_c dc_entry;
{
	struct com_sys_parm_c sys_parm;
	char data_code[5];
	char tmpkmh[8];
	struct com_item_c vcom_item;
	char tcode[5];
	int ret=0;

	memset(&sys_parm,0x00,sizeof(struct com_sys_parm_c));
vtcp_log("%s,%d, ACC_HRT[%s][%s][%s]",__FILE__,__LINE__,dc_log->acc_hrt,dc_log->cur_no,dc_entry.brf);

	dc_log->dc_seqn=dc_entry.seqn;
	pub_base_strpack( dc_entry.cur_no );
	vtcp_log("%s,%d dc_entry.acc_hrt=[%s]",__FILE__,__LINE__,dc_entry.acc_hrt);
	if( strlen(dc_entry.cur_no)==4 )
	{
		get_zd_data(dc_entry.cur_no,dc_log->cur_no);
		vtcp_log("[%s][%d]dc_log->cur_no=[%s]",__FILE__,__LINE__,dc_log->cur_no);
	}
	else
	{
		dc_log->cur_no[0]=dc_entry.cur_no[0];
		dc_log->cur_no[1]=dc_entry.cur_no[1];
		dc_log->dc_ind[1]='\0';
		vtcp_log("[%s][%d]dc_log->cur_no=[%s]",__FILE__,__LINE__,dc_log->cur_no);
	}

	vtcp_log("%s,%d dc_entry.acc_hrt=[%s]",__FILE__,__LINE__,dc_entry.acc_hrt);
	/**ȡ�ÿ�Ŀ������**/
	if ( dc_entry.acc_hrt[0]!='@' )
	{
		vtcp_log("%s,%d dc_entry.acc_hrt=[%s]",__FILE__,__LINE__,dc_entry.acc_hrt);
		strcpy ( dc_log->acc_hrt , dc_entry.acc_hrt );
	}
	else
	{
		strncpy(data_code,dc_entry.acc_hrt+1,4);
		data_code[4]='\0';
		get_zd_data(data_code,dc_log->acc_hrt);
		dc_log->acc_hrt[5]='\0';
	}
	pub_base_strpack( dc_entry.dc_ind );
	if( strlen(dc_entry.dc_ind)==4 )
	{
		get_zd_data ( dc_entry.dc_ind , dc_log->dc_ind );
	}
	else
	{
		dc_log->dc_ind[0]=dc_entry.dc_ind[0];
		dc_log->dc_ind[1]='\0';
	}

	pub_base_strpack( dc_entry.ct_ind );
	if( strlen(dc_entry.ct_ind)==4 )
	{
		get_zd_data ( dc_entry.ct_ind , dc_log->ct_ind );
		if( dc_log->ct_ind[0]!='1' ) dc_log->ct_ind[0]='2';
	}
	else
	{
		dc_log->ct_ind[0]=dc_entry.ct_ind[0];
		dc_log->ct_ind[1]='\0';
	}
	if ( dc_entry.ct_type[0] == '1' )
	{
		if ( strncmp ( dc_log->opn_br_no , dc_log->tx_br_no , 5 ) )
		{
			strcpy ( dc_log->ct_ind , "2" );
		}
	}

	if ( pub_acct_dc_get_amt ( dc_entry.amt_no , &dc_log->amt ) )
	{
		vtcp_log("ȡ�ý�����");
		WRITEMSG
		return 1;
	}
	vtcp_log( "---->xxx[%lf]",dc_log->amt );
		get_zd_data( DC_TEL, dc_log->tel );
	
	if( dc_log->amt<-0.005 || dc_log->amt>0.005 )
	{
		if( !strcmp(dc_log->tx_tx_br_no,"99999") 
			|| !strcmp(dc_log->tx_tx_br_no,TOT_BR_NO ))
		{
			strcpy( g_pub_tx.reply,"S043" );
			vtcp_log ("�ǼǴ��� S043");
			WRITEMSG
			return 1;
		}
sprintf(acErrMsg,"ACC_HRT[%s][%s][%s]",dc_log->acc_hrt,dc_log->cur_no,dc_entry.brf);
WRITEMSG

		if ( pubf_ass_sy( dc_log->acc_hrt,dc_log->dc_ind,dc_log->cur_no,
			dc_log->ct_ind, dc_log->tx_opn_br_no, dc_log->tx_tx_br_no, 
			dc_log->amt, dc_entry  ) )
		{
			vtcp_log ("�Ǽǿ��˴���");
			WRITEMSG
			return 1;
		}
		g_acc_amt[dc_log->dc_ind[0]-'1']+=dc_log->amt;
		sprintf(acErrMsg,"ACC_HRT[%s][%s][%s][%.2lf]",dc_log->acc_hrt,dc_log->cur_no,dc_entry.brf,dc_log->amt);
WRITEMSG
		/** add by LiuHuafeng 2006-11-2 17:20 ���Ӷ�ָ���Ŀ�Ŀ�Ŀ���,������������ */
		/* 1- ���Ŀ�Ŀ�Ƿ���Ҫ�����ʽ������� */
		{
			struct com_item_add_c sCom_item_add;
			double dAvbal=0.0,dTxamt=0.0;
			struct zjgl_mst_c sZjgl_mst;
			char cMx_no[8];
			char cEj_no[6];
			char cYj_no[4];
			memset(cMx_no, 0 ,sizeof(cMx_no));
			memset(cEj_no, 0 ,sizeof(cEj_no));
			memset(cYj_no, 0 ,sizeof(cYj_no));
			memset(&sZjgl_mst, 0 , sizeof(sZjgl_mst));
			dAvbal=0.0;
			dTxamt=0.0;
			memset(&sCom_item_add, 0 , sizeof(sCom_item_add));
			g_reply_int=Com_item_add_Sel(g_pub_tx.reply,&sCom_item_add,"acc_hrt='%s'",dc_log->acc_hrt);
			if(g_reply_int==0)
			{
				/* ����һ�� */
				if(sCom_item_add.add_ind[0]!='0')
				{
					vtcp_log("%s,%d acchtr==[%s]����ǵײ��Ŀ,������ܿ��ƺ���",__FILE__,__LINE__,dc_log->acc_hrt);
					struct com_item_c sCom_item;
					memset(&sCom_item, 0 , sizeof(sCom_item));
					g_reply_int=Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_hrt='%s'",dc_log->acc_hrt);
					if(g_reply_int==0 && sCom_item.sub_acc_yn[0]=='N' && memcmp(dc_log->tx_opn_br_no,CW_BR_NO,5))
					{
						g_reply_int=Zjgl_mst_Sel(g_pub_tx.reply,&sZjgl_mst,"opn_br_no='%s' and cur_no='%s' and acc_no='%s'",dc_log->tx_opn_br_no,dc_log->cur_no,dc_log->acc_hrt);
						if(g_reply_int==0)
						{
							vtcp_log("%s,%d acchtr==[%s]�ǵײ��Ŀ,������ܿ��ƺ���",__FILE__,__LINE__,dc_log->acc_hrt);
							/* 2-��ײ��Ŀ��Ҫ����������ʽ������� */
							dAvbal=0.0;
							/** ����������Ϳ�Ŀ����ͬ����������ȡΪ����,˫�Կ�Ŀ�Խ跽Ϊ�� */
							dTxamt=dc_log->amt;
							if(sCom_item.dc_ind[0]=='0' || sCom_item.dc_ind[0]=='1')
							{
								/* �跽��Ŀ */
								if(dc_log->dc_ind[0]=='2') 
									dTxamt=-dc_log->amt;
							}
							else
							{
								/* ������Ŀ */
								if(dc_log->dc_ind[0]!='2') 
									dTxamt=-dc_log->amt;
							}
							g_reply_int=ipubf_acct_zjgl_trance(dc_log->tx_opn_br_no,dc_log->cur_no,sCom_item.acc_no,&dTxamt,&dAvbal);
							if(g_reply_int)
							{
								vtcp_log("%s,%d �ʽ������ƴ��� RET=[%d]",__FILE__,__LINE__,g_reply_int);
								sprintf(acErrMsg,"��Ŀ%s���������޶�,�������������ϵ",sCom_item.acc_no);
								set_zd_data("0130",acErrMsg);
								WRITEMSG
								return 1;
							}
						}
					}
				}
			}
			memset(&sCom_item_add, 0 , sizeof(sCom_item_add));
			sprintf(cYj_no,"%.3s",dc_log->acc_hrt);
			vtcp_log("+++++++cYj_no is [%s]++++++++++\n",cYj_no);
			g_reply_int=Com_item_add_Sel(g_pub_tx.reply,&sCom_item_add,"acc_no='%s'",cYj_no);
			/* �ж�һ���Ŀ��ƽ�� */
			if(g_reply_int==0)
			{
				if(sCom_item_add.add_ind[0]!='0')
				{
					struct com_item_c sCom_item;
					memset(&sCom_item, 0 , sizeof(sCom_item));
					vtcp_log("%s,%d acchtr==[%s]�ж�һ����Ŀ��������ܿغ���",__FILE__,__LINE__,dc_log->acc_hrt);
					g_reply_int=Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_no='%s'",cYj_no);
					if(g_reply_int==0 && sCom_item.sub_acc_yn[0]=='Y' && memcmp(dc_log->tx_opn_br_no,CW_BR_NO,5))
					{
						g_reply_int=Zjgl_mst_Sel(g_pub_tx.reply,&sZjgl_mst,"opn_br_no='%s' and cur_no='%s' and acc_no='%s'",dc_log->tx_opn_br_no,dc_log->cur_no,dc_log->acc_hrt);
						if(g_reply_int==0)
						{
							vtcp_log("%s,%d acchtr==[%s]�Ƿǵײ��һ����Ŀ,������ܿ��ƺ���",__FILE__,__LINE__,dc_log->acc_hrt);
							dAvbal=0.0;
							/** ����������Ϳ�Ŀ����ͬ����������ȡΪ����,˫�Կ�Ŀ�Խ跽Ϊ�� */
							dTxamt=dc_log->amt;
							if(sCom_item.dc_ind[0]=='0' || sCom_item.dc_ind[0]=='1')
							{
								/* �跽��Ŀ */
								if(dc_log->dc_ind[0]=='2') 
									dTxamt=-dc_log->amt;
							}
							else
							{
								/* ������Ŀ */
								if(dc_log->dc_ind[0]!='2') 
									dTxamt=-dc_log->amt;
							}
							g_reply_int=ipubf_acct_zjgl_trance(dc_log->tx_opn_br_no,dc_log->cur_no,cYj_no,&dTxamt,&dAvbal);
							if(g_reply_int)
							{
								vtcp_log("%s,%d �ʽ������ƴ��� RET=[%d]",__FILE__,__LINE__,g_reply_int);
								sprintf(acErrMsg,"��Ŀ%s���������޶�,�������������ϵ",sCom_item.acc_no);
								set_zd_data("0130",acErrMsg);
								WRITEMSG
								return 1;
							}
						}
					}
				}
			}
		}
		sprintf(acErrMsg,"ACC_HRT[%s][%s][%s][%.2lf]",dc_log->acc_hrt,dc_log->cur_no,dc_entry.brf,dc_log->amt);
WRITEMSG		
		/** end by LiuHuafeng 2006-11-2 17:21 
vtcp_log ( "1111tttttttttttttttttt=[%s]",dc_log->trace_no );
vtcp_log ( "2222yyyyyyyyyyyyyyyyyy=[%s]",dc_log->entry_code );
vtcp_log ( "3333uuuuuuuuuuuuuuuuuu=[%s]",g_pub_tx.reply);*/
		if ( Dc_log_Ins ( *dc_log , g_pub_tx.reply ) )
		{
			vtcp_log ("�ǼǷ�¼��ˮ����");
			WRITEMSG
			return 1;
		}

		if ( Com_sys_parm_Sel ( g_pub_tx.reply , &sys_parm , "%s" , "1=1" ) )
		{
			vtcp_log ("�ǼǷ�¼��ˮ����");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "S005" );
			return 1;
		}
		if ( sys_parm.real_time_ind[0]=='Y' )
		{
			if ( gl_mst_proc ( *dc_log ) )
			{
				vtcp_log ("���»�����ļ�����");
				WRITEMSG
				return 1;
			}
		}

	}

	return 0;
}
/***************************************************
*  LiuHuafeng 20060823 �����������ĸ�������:
*  ����amt_no�еĶ�Ӧ��8583���е�ֵ����һ��������
*  ���ҽ���֧�ּӼ������� 
***************************************************/
pub_acct_dc_get_amt ( char *amt_no , double *amt )
{
	int i,j,len;
	char data_code[5];
	double tmpamt,op;

	*amt=0.00;
	tmpamt=0.00;
	op=1.00;

sprintf(acErrMsg,"amt_mo[%s]",amt_no);
WRITEMSG
	len=strlen(amt_no);
	for(i=0;i<len;i++)
	{
		switch(amt_no[i])
		{
			case '+':
				op=1.00;
				continue;
			case '-':
				op=-1.00;
				continue;
			default:
				break;
		}
		for(j=0;j<4;j++)
		{
			if ( (amt_no[i]>'9' || amt_no[i]<'0')
				&& (amt_no[i]>'Z' || amt_no[i]<'A') )
			{
                strcpy ( g_pub_tx.reply , "S042" );
				vtcp_log("�����붨�����");
				WRITEMSG
				return 1;
			}
			data_code[j]=amt_no[i];
			i++;
		}
		i--;
        data_code[4]='\0';

		if ( get_zd_double ( data_code , &tmpamt )==NULL )
		{
			/**
            strcpy ( g_pub_tx.reply , "S042" );
			vtcp_log("���ݱ��붨�����[%s]",data_code);
			WRITEMSG
			return 1;
			**/
		}
vtcp_log("%s,%d sjbm[%s] amt=[%lf] op = [%lf]",__FILE__,__LINE__,data_code,tmpamt,op);
		/*tmpamt+=op*tmpamt;*/
		*amt+=op*tmpamt;
vtcp_log("%s,%d UUUUUUUU amt=[%lf] op = [%lf]",__FILE__,__LINE__,*amt,op);
	}

vtcp_log("%s,%d last amt=[%lf] op = [%lf]",__FILE__,__LINE__,*amt,op);
	return 0;
}
static dc_entry_branch ( sub_tx_code , dc_log )
struct dc_log_c *dc_log;
{
	struct tx_dc_rel_c dc_rel;
	char opn_br_no[6];
	char tx_br_no[6];

	memset(&dc_rel,0x00,sizeof(struct tx_dc_rel_c));

	strcpy ( opn_br_no , dc_log->opn_br_no );
	strcpy ( tx_br_no , dc_log->tx_br_no );
	strcpy ( dc_rel.dc_code , "BR01" );
	RespCode=Tx_dc_rel_Sel(g_pub_tx.reply,&dc_rel,
		"sub_tx_code='%s' AND dc_code='%s'",sub_tx_code,dc_rel.dc_code);
	if ( RespCode )
	{
		vtcp_log("��ȡͨ�Ҳ�Ʒ��ƹ�ϵ�����");
		strcpy ( g_pub_tx.reply , "S001" );
		WRITEMSG
		return 1;
	}
	strcpy(dc_log->entry_code,dc_rel.entry_code);
	if ( dc_entry_proc(dc_log) )
	{
		vtcp_log("�����Զ���¼��ˮ����");
		WRITEMSG
		return 1;
	}
	strcpy ( dc_rel.dc_code , "BR02" );
	RespCode=Tx_dc_rel_Sel(g_pub_tx.reply,&dc_rel,
		"sub_tx_code='%s' AND dc_code='%s'",sub_tx_code,dc_rel.dc_code);
	if ( RespCode )
	{
		vtcp_log("��ȡͨ�Ҳ�Ʒ��ƹ�ϵ�����");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "S001" );
		return 1;
	}
	strcpy(dc_log->opn_br_no,dc_log->tx_br_no);
	strcpy(dc_log->entry_code,dc_rel.entry_code);
	if ( dc_entry_proc(dc_log) )
	{
		vtcp_log("�����Զ���¼��ˮ����");
		WRITEMSG
		return 1;
	}

/****xxx 20030312
	while(1)
	{
		if ( dc_entry_opn_up_br(dc_log,opn_br_no,sub_tx_code) )
		{
			vtcp_log("�����������ϼ���¼����");
			WRITEMSG
			return 1;
		}
		if ( dc_entry_tx_up_br(dc_log,tx_br_no,sub_tx_code) )
		{
			vtcp_log("�����׻����ϼ���¼����");
			WRITEMSG
			return 1;
		}
		if ( !strncmp ( opn_br_no , tx_br_no , 5 ) )
		{
			break;
		}
	}
	****/

	return 0;
}
static dc_entry_opn_up_br(dc_log,opn_br_no,sub_tx_code)
struct dc_log_c *dc_log;
char *opn_br_no,*sub_tx_code;
{
	struct dc_branch_c dc_branch;
	struct tx_dc_rel_c dc_rel;

	memset(&dc_branch,0x00,sizeof(struct dc_branch_c));
	memset(&dc_rel,0x00,sizeof(struct tx_dc_rel_c));

	RespCode=Dc_branch_Sel(g_pub_tx.reply,&dc_branch,"br_no'%s'",opn_br_no);
	if ( RespCode )
	{
		vtcp_log("ȡ��ƻ��������[%s]",opn_br_no);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "S032");
		return 1;
	}
	strcpy ( dc_rel.dc_code , "BR03" );
	RespCode=Tx_dc_rel_Sel(g_pub_tx.reply,&dc_rel,
		"sub_tx_code='%s' AND dc_code='%s'",sub_tx_code,dc_rel.dc_code);
	if ( RespCode )
	{
		vtcp_log("��ȡ��Ʒ��ƹ�ϵ�����[%s][%s]",sub_tx_code,dc_rel.dc_code);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "S001" );
		return 1;
	}
	strcpy(dc_log->opn_br_no,dc_branch.up_br_no);
	RespCode=Dc_branch_Sel(g_pub_tx.reply,
		&dc_branch,"br_no'%s'",dc_branch.up_br_no);
	if ( RespCode )
	{
		vtcp_log("ȡ��ƻ��������[%s]",dc_branch.up_br_no);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "S032");
		return 1;
	}
	strcpy(dc_log->entry_code,dc_rel.entry_code);
	if ( dc_entry_proc(dc_log) )
	{
		vtcp_log("�����Զ���¼��ˮ����");
		WRITEMSG
		return 1;
	}
	strcpy ( opn_br_no , dc_log->opn_br_no );

	return 0;
}
static dc_entry_tx_up_br(dc_log,tx_br_no,sub_tx_code)
struct dc_log_c *dc_log;
char *tx_br_no,*sub_tx_code;
{
	struct dc_branch_c dc_branch;
	struct tx_dc_rel_c dc_rel;

	memset(&dc_branch,0x00,sizeof(struct dc_branch_c));
	memset(&dc_rel,0x00,sizeof(struct tx_dc_rel_c));

	RespCode=Dc_branch_Sel(g_pub_tx.reply,&dc_branch,"br_no'%s'",tx_br_no);
	if ( RespCode )
	{
		vtcp_log("ȡ��ƻ��������[%s]",tx_br_no);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "S032");
		return 1;
	}
	strcpy ( dc_rel.dc_code , "BR04" );
	RespCode=Tx_dc_rel_Sel(g_pub_tx.reply,&dc_rel,
		"sub_tx_code='%s' AND dc_code='%s'",sub_tx_code,dc_rel.dc_code);
	if ( RespCode )
	{
		vtcp_log("��ȡ��Ʒ��ƹ�ϵ�����[%s][%s]",sub_tx_code,dc_rel.dc_code);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "S001" );
		return 1;
	}
	strcpy(dc_log->opn_br_no,dc_branch.up_br_no);
	RespCode=Dc_branch_Sel(g_pub_tx.reply,
		&dc_branch,"br_no'%s'",dc_branch.up_br_no);
	if ( RespCode )
	{
		vtcp_log("ȡ��ƻ��������[%s]",dc_branch.up_br_no);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "S032");
		return 1;
	}
	strcpy(dc_log->entry_code,dc_rel.entry_code);
	if ( dc_entry_proc(dc_log) )
	{
		vtcp_log("�����Զ���¼��ˮ����");
		WRITEMSG
		return 1;
	}
	strcpy ( tx_br_no , dc_log->opn_br_no );

	return 0;
}
static gl_mst_proc ( dc_log )
struct dc_log_c dc_log;
{
	struct com_item_c com_item;
	struct gl_mst_c gl_mst;

	memset(&com_item,0x00,sizeof(struct com_item_c));
	memset(&gl_mst,0x00,sizeof(struct gl_mst_c));

	if (Com_item_Sel(g_pub_tx.reply,&com_item,"acc_hrt='%s'",dc_log.acc_hrt))
	{
		strcpy ( g_pub_tx.reply , "S033" );
		vtcp_log("��Ŀ�����޴˿�Ŀ������[%s]",dc_log.acc_hrt);
		WRITEMSG
		return 1;
	}
	if ( com_item.sub_acc_yn[0]=='Y' )
	{
		strcpy ( g_pub_tx.reply , "S036" );
		vtcp_log("�˿�Ŀ�����ַǼ��ʿ�Ŀ[%s]",dc_log.acc_hrt);
		WRITEMSG
		return 1;
	}
	vtcp_log("[%s][%d]dc_log.cur_no=[%s]",__FILE__,__LINE__,dc_log.cur_no);
	if ( com_item.fore_ind[0]=='1'&&strncmp(dc_log.cur_no,"01",2) )
	{
		strcpy ( g_pub_tx.reply , "S036" );
		vtcp_log("�˿�Ŀ�����ַ���ҿ�Ŀ[%s]",dc_log.acc_hrt);
		WRITEMSG
		return 1;
	}
	if ( com_item.fore_ind[0]=='2'&&!strncmp(dc_log.cur_no,"01",2) )
	{
		strcpy ( g_pub_tx.reply , "S036" );
		vtcp_log("�˿�Ŀ�����ַ�����ҿ�Ŀ[%s]",dc_log.acc_hrt);
		WRITEMSG
		return 1;
	}

	RespCode=Gl_mst_Dec_Upd(g_pub_tx.reply,
		"br_no='%s' and cur_no='%s' and acc_hrt='%s'",
		dc_log.opn_br_no,dc_log.cur_no,dc_log.acc_hrt);
	if ( RespCode )
	{
		strcpy ( g_pub_tx.reply , "S034" );
		vtcp_log("�����������ļ��α��");
		WRITEMSG
		return 1;
	}

	RespCode=Gl_mst_Fet_Upd(&gl_mst,g_pub_tx.reply);
	if ( RespCode <0 )
	{
		strcpy ( g_pub_tx.reply , "S035" );
		vtcp_log("�����������ļ��α��");
		WRITEMSG
		Gl_mst_Clo_Upd();
		return 1;
	}
	else if ( RespCode==0 )
	{
		if ( gl_mst_val ( gl_mst , dc_log , com_item ) )
		{
			vtcp_log("���������ļ���ֵ��");
			WRITEMSG
			Gl_mst_Clo_Upd();
			return 1;
		}
		if ( Gl_mst_Upd_Upd ( gl_mst , g_pub_tx.reply ) )
		{
			vtcp_log("�����������ļ���");
			WRITEMSG
			Gl_mst_Clo_Upd();
			return 1;
		}
		Gl_mst_Clo_Upd();
	}
	else
	{
		Gl_mst_Clo_Upd();
		gl_mst_init ( &gl_mst , dc_log , com_item ) ;
		if ( gl_mst_val ( &gl_mst , dc_log , com_item ) )
		{
			vtcp_log("���������ļ���ֵ��");
			WRITEMSG
			return 1;
		}
		if ( Gl_mst_Ins ( gl_mst , g_pub_tx.reply ) )
		{
			strcpy ( g_pub_tx.reply , "S038" );
			vtcp_log("���������ļ���");
			WRITEMSG
			return 1;
		}
	}

	return 0;
}
static gl_mst_init ( gl_mst , dc_log , com_item )
struct gl_mst_c *gl_mst;
struct dc_log_c dc_log;
struct com_item_c com_item;
{
	strcpy ( gl_mst->br_no , dc_log.opn_br_no );
	strcpy ( gl_mst->cur_no , dc_log.cur_no );
	strcpy ( gl_mst->acc_hrt , dc_log.acc_hrt );
	get_zd_long ( "0070" , &gl_mst->date );
	strcpy ( gl_mst->dc_ind , com_item.dc_ind );
	strcpy ( gl_mst->up_acc_hrt , com_item.up_acc_hrt );
	gl_mst->dr_bal=0.00;
	gl_mst->cr_bal=0.00;
	gl_mst->ldd_bal=0.00;
	gl_mst->lcd_bal=0.00;
	gl_mst->rdd_cnt=0;
	gl_mst->rcd_cnt=0;
	gl_mst->rdd_amt=0.00;
	gl_mst->rcd_amt=0.00;
	gl_mst->cdd_cnt=0;
	gl_mst->ccd_cnt=0;
	gl_mst->cdd_amt=0.00;
	gl_mst->ccd_amt=0.00;
	gl_mst->tddr_bal=0.00;
	gl_mst->tdcr_bal=0.00;
	gl_mst->tddr_cnt=0;
	gl_mst->tdcr_cnt=0;
	gl_mst->tddr_amt=0.00;
	gl_mst->tdcr_amt=0.00;
	gl_mst->mdr_bal=0.00;
	gl_mst->mcr_bal=0.00;
	gl_mst->mdr_cnt=0;
	gl_mst->mcr_cnt=0;
	gl_mst->mdr_amt=0.00;
	gl_mst->mcr_amt=0.00;
	gl_mst->qdr_bal=0.00;
	gl_mst->qcr_bal=0.00;
	gl_mst->qdr_cnt=0;
	gl_mst->qcr_cnt=0;
	gl_mst->qdr_amt=0.00;
	gl_mst->qcr_amt=0.00;
	gl_mst->ydr_bal=0.00;
	gl_mst->ycr_bal=0.00;
	gl_mst->ydr_cnt=0;
	gl_mst->ycr_cnt=0;
	gl_mst->ydr_amt=0.00;
	gl_mst->ycr_amt=0.00;
}
static gl_mst_val ( gl_mst , dc_log , com_item )
struct gl_mst_c *gl_mst;
struct dc_log_c dc_log;
struct com_item_c com_item;
{
	double tmpamt;

	if ( dc_log.dc_ind[0]=='1' )
	{
		gl_mst->rdd_cnt+=1;
		gl_mst->rdd_amt+=dc_log.amt;
		gl_mst->tddr_cnt+=1;
		gl_mst->tddr_amt+=dc_log.amt;
		gl_mst->mdr_cnt+=1;
		gl_mst->mdr_amt+=dc_log.amt;
		gl_mst->qdr_cnt+=1;
		gl_mst->qdr_amt+=dc_log.amt;
		gl_mst->ydr_cnt+=1;
		gl_mst->ydr_amt+=dc_log.amt;
		if ( dc_log.ct_ind[0]=='1' )
		{
			gl_mst->cdd_cnt+=1;
			gl_mst->cdd_amt+=dc_log.amt;
		}
		if ( gl_mst->dc_ind[0]=='1' )
		{
			gl_mst->dr_bal+=dc_log.amt;
			if (gl_mst->dr_bal<0.00&&com_item.od_ind[0]=='Y')
			{
				strcpy ( g_pub_tx.reply , "S037" );
				WRITEMSG
				return 1;
			}
		}
		else if ( gl_mst->dc_ind[0]=='2' )
		{
			gl_mst->cr_bal-=dc_log.amt;
			if (gl_mst->cr_bal<0.00&&com_item.od_ind[0]=='Y')
			{
				strcpy ( g_pub_tx.reply , "S037" );
				WRITEMSG
				return 1;
			}
		}
		else
		{
			tmpamt=gl_mst->dr_bal-gl_mst->cr_bal+dc_log.amt;
			if ( tmpamt < 0.00 )
			{
				gl_mst->dr_bal=0.00;
				gl_mst->cr_bal-=tmpamt;
			}
			else
			{
				gl_mst->cr_bal=0.00;
				gl_mst->dr_bal-=tmpamt;
			}
		}
	}
	else
	{
		gl_mst->rcd_cnt+=1;
		gl_mst->rcd_amt+=dc_log.amt;
		gl_mst->tdcr_cnt+=1;
		gl_mst->tdcr_amt+=dc_log.amt;
		gl_mst->mcr_cnt+=1;
		gl_mst->mcr_amt+=dc_log.amt;
		gl_mst->qcr_cnt+=1;
		gl_mst->qcr_amt+=dc_log.amt;
		gl_mst->ycr_cnt+=1;
		gl_mst->ycr_amt+=dc_log.amt;
		if ( dc_log.ct_ind[0]=='1' )
		{
			gl_mst->ccd_cnt+=1;
			gl_mst->ccd_amt+=dc_log.amt;
		}
		if ( gl_mst->dc_ind[0]=='1' )
		{
			gl_mst->dr_bal-=dc_log.amt;
			if (gl_mst->dr_bal<0.00&&com_item.od_ind[0]=='Y')
			{
				strcpy ( g_pub_tx.reply , "S037" );
				WRITEMSG
				return 1;
			}
		}
		else if ( gl_mst->dc_ind[0]=='2' )
		{
			gl_mst->cr_bal+=dc_log.amt;
			if (gl_mst->cr_bal<0.00&&com_item.od_ind[0]=='Y')
			{
				strcpy ( g_pub_tx.reply , "S037" );
				WRITEMSG
				return 1;
			}
		}
		else
		{
			tmpamt=gl_mst->dr_bal-gl_mst->cr_bal-dc_log.amt;
			if ( tmpamt < 0.00 )
			{
				gl_mst->dr_bal=0.00;
				gl_mst->cr_bal-=tmpamt;
			}
			else
			{
				gl_mst->cr_bal=0.00;
				gl_mst->dr_bal-=tmpamt;
			}
		}
	}

	return 0;
}
/**
���dc_log->amtֵ ȡdc_log_entry ǰ�벿
**/
static dc_log_getentry(dc_log,dc_entry)
struct dc_log_c *dc_log;
struct dc_entry_c dc_entry;
{
	struct com_sys_parm_c sys_parm;
	char data_code[5];
	char tmpkmh[8];
	struct com_item_c vcom_item;
	char tcode[5];
	int ret=0;

	memset(&sys_parm,0x00,sizeof(struct com_sys_parm_c));
vtcp_log("%s,%d, ACC_HRT[%s][%s][%s]",__FILE__,__LINE__,dc_log->acc_hrt,dc_log->cur_no,dc_entry.brf);

	dc_log->dc_seqn=dc_entry.seqn;
	pub_base_strpack( dc_entry.cur_no );
	vtcp_log("%s,%d dc_entry.acc_hrt=[%s]",__FILE__,__LINE__,dc_entry.acc_hrt);
	if( strlen(dc_entry.cur_no)==4 )
	{
		get_zd_data(dc_entry.cur_no,dc_log->cur_no);
		vtcp_log("[%s][%d]dc_log->cur_no=[%s]",__FILE__,__LINE__,dc_log->cur_no);
	}
	else
	{
		dc_log->cur_no[0]=dc_entry.cur_no[0];
		dc_log->cur_no[1]=dc_entry.cur_no[1];
		dc_log->dc_ind[1]='\0';
		vtcp_log("[%s][%d]dc_log->cur_no=[%s]",__FILE__,__LINE__,dc_log->cur_no);
	}

	vtcp_log("%s,%d dc_entry.acc_hrt=[%s]",__FILE__,__LINE__,dc_entry.acc_hrt);
	/**ȡ�ÿ�Ŀ������**/
	if ( dc_entry.acc_hrt[0]!='@' )
	{
		vtcp_log("%s,%d dc_entry.acc_hrt=[%s]",__FILE__,__LINE__,dc_entry.acc_hrt);
		strcpy ( dc_log->acc_hrt , dc_entry.acc_hrt );
	}
	else
	{
		strncpy(data_code,dc_entry.acc_hrt+1,4);
		data_code[4]='\0';
		get_zd_data(data_code,dc_log->acc_hrt);
		dc_log->acc_hrt[5]='\0';
	}
	pub_base_strpack( dc_entry.dc_ind );
	if( strlen(dc_entry.dc_ind)==4 )
	{
		get_zd_data ( dc_entry.dc_ind , dc_log->dc_ind );
	}
	else
	{
		dc_log->dc_ind[0]=dc_entry.dc_ind[0];
		dc_log->dc_ind[1]='\0';
	}

	pub_base_strpack( dc_entry.ct_ind );
	if( strlen(dc_entry.ct_ind)==4 )
	{
		get_zd_data ( dc_entry.ct_ind , dc_log->ct_ind );
		if( dc_log->ct_ind[0]!='1' ) dc_log->ct_ind[0]='2';
	}
	else
	{
		dc_log->ct_ind[0]=dc_entry.ct_ind[0];
		dc_log->ct_ind[1]='\0';
	}
	if ( dc_entry.ct_type[0] == '1' )
	{
		if ( strncmp ( dc_log->opn_br_no , dc_log->tx_br_no , 5 ) )
		{
			strcpy ( dc_log->ct_ind , "2" );
		}
	}

	if ( pub_acct_dc_get_amt ( dc_entry.amt_no , &dc_log->amt ) )
	{
		vtcp_log("ȡ�ý�����");
		WRITEMSG
		return 1;
	}
	
	return 0;
}
