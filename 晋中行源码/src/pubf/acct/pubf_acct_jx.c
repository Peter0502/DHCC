
#define ERR_DEAL if(ret){sprintf(acErrMsg,"ERROR");WRITEMSG goto ErrExit;}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "card.h"
#include "prdt_ac_id_c.h"
#include "trace_log_c.h"
#include "dd_mst_hst_c.h"
#include "td_mst_hst_c.h"
#include "ln_mst_hst_c.h"
#include "in_mst_hst_c.h"
#include "mdm_unprt_hst_c.h"
#include "mdm_attr_c.h"
#include "note_parm_c.h"
#include "com_item_c.h"
#include "gl_mst_c.h"
#include "tx_dc_br_rel_c.h"
#include "mo_prdt_prot_c.h"
#include "mo_prot_ac_rel_c.h"
#include "card_consum_lmt_c.h"
#include "remark_ctl_c.h"

static struct mdm_attr_c v_mdm_attr;
static struct prdt_ac_id_c prdt_ac_id_c;
static struct mdm_attr_c mdm_attr_c;
static struct note_parm_c note_parm_c;
static struct card_consum_lmt_c g_card_consum_lmt;
static	struct ln_mst_c ln_mst_acm;
static	struct ln_mst_c o_td_mst;
static int wdr_flag=0;
static int partflag=0;

pub_acct_trance_jx()
{
	int ret=0;
	int ass_ind=0;
    int factday,lastday;
	struct pub_tx v_pub_tx;

	memset(&prdt_ac_id_c,0x00,sizeof(struct prdt_ac_id_c));
	memset(&mdm_attr_c,0x00,sizeof(struct mdm_attr_c));
	memset(&note_parm_c,0x00,sizeof(struct note_parm_c));
	wdr_flag=0;

	/**��ʼ�����ļ�����**/
	pub_base_strpack(g_pub_tx.ac_no);
	g_pub_tx.ct_ind[0]='2';

   	sprintf(acErrMsg,"g_pub_tx.acno= [%s] id=[%ld] seqn=[%d]",
		g_pub_tx.ac_no,g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	WRITEMSG
	sprintf(acErrMsg,"������ʹ������[%.2lf][%.2lf]",
		g_pub_tx.tx_amt1,g_ln_mst.bal );
	WRITEMSG

		/**��ʼ�����ļ�����**/
		ret=jx_sub_ac();			/* ûɶ��������� */
		if( ret==100 ) return 0; /***TEMP�����˺�***/
		ERR_DEAL

		/***��鴦��ֻ���***/
		ret=jx_dd_mst();
		ERR_DEAL

		/***�����۴���***/
		/**����Ҫ����δ������ϸ��-wangwk-20110119**/
		if((memcmp(g_pub_tx.ac_no,CARDHEADCODE,6) != 0) && (memcmp(g_pub_tx.ac_no,ICCARDHEADCODE,6) != 0))
		{
			ret=jx_unprt_hst_ins(g_pub_tx.ac_id_type);
		}else
		{
			vtcp_log("%s,%d,������Ҫ��Ϣ�Ǽ�",__FILE__,__LINE__);
		}
		/**���Ľ���-wangwk-20110119**/
		ERR_DEAL

	set_zd_data(DC_CODE,g_pub_tx.dc_code);

		get_zd_long( DC_TRACE_CNT , &g_pub_tx.trace_cnt);
                g_pub_tx.trace_cnt++;
                set_zd_long( DC_TRACE_CNT , g_pub_tx.trace_cnt);


OKExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"ҵ����ϵͳ���˴���ɹ�!");
	WRITEMSG
	return 0;
	
ErrExit:
	sprintf(acErrMsg,"ҵ����ϵͳ���˴���ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	return 1;
}
jx_sub_ac()
{
	struct mdm_ac_rel_c v_mdm_ac_rel;
	struct prdt_ac_id_c prdt_ac_id_c;
	struct prdt_ac_id_c s_prdt_ac_id;
	struct mdm_attr_c s_mdm_attr;
	char v_ac_get_ind[2];
	int ret=0;

	v_ac_get_ind[0]=g_pub_tx.ac_get_ind[0];

	pub_base_strpack(g_pub_tx.ac_no);

	g_pub_tx.ac_id=g_dd_mst.ac_id;
	g_pub_tx.ac_seqn=g_dd_mst.ac_seqn;

	strcpy(g_pub_tx.opn_br_no,g_dd_mst.opn_br_no);
	set_zd_data("0020", g_pub_tx.opn_br_no);

	/*** ȡ�˻�����,��Ʒ���� ����Ѿ�ȷ���˾Ͳ�����***/
	strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
	strcpy(g_pub_tx.ac_id_type,"1");

	return 0;
}
int jx_dd_mst()
{
	int ret;
	double tmpje=0.00;
	double tmpbal=0.00;
	struct dd_mst_c	TmpDd_mst;
	char msg[51];
	struct	remark_ctl_c rem;
	int chk_ctl_flag=0;

	memset(&rem,0x00,sizeof(struct remark_ctl_c));
	/*** ���û��ڲ�Ʒ���������麯�� ***/

	sprintf(acErrMsg,"bal[%lf][%lf][%lf][%lf]",
		g_dd_mst.ctl_amt,g_dd_mst.hold_amt,g_pub_tx.tx_amt1,g_dd_mst.bal);
	WRITEMSG

	strcpy(g_pub_tx.cur_no,g_dd_parm.cur_no);
	strcpy(g_pub_tx.dc_code,g_dd_parm.dc_code);
	strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);

	strcpy(g_pub_tx.name,g_dd_mst.name);
	g_pub_tx.cif_no=g_dd_mst.cif_no;

	if( g_dd_mst.ac_sts[0]=='3' )
	{
		sprintf(acErrMsg,"���˻��ѹ�ʧ����!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O174");
		goto ErrExit;
	}
	else if( g_dd_mst.ac_sts[0]=='5' )
	{
		sprintf(acErrMsg,"���˻�������ʱ����״̬!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O173");
		goto ErrExit;
	}
	else if( g_dd_mst.ac_sts[0]=='*' )
	{
		sprintf(acErrMsg,"���˻�������������!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P245");
		goto ErrExit;
	}

	vtcp_log("------look------[%s]",g_mdm_ac_rel.ac_no);
	/*ȡ�������Э������*/
	if( g_pub_tx.add_ind[0]=='0' )
	{
		g_dd_mst.bal=g_dd_mst.bal-g_pub_tx.tx_amt1;
		g_dd_mst.acbo_bal=g_dd_mst.acbo_bal-g_pub_tx.tx_amt1;
		
		if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
		{
			g_dd_mst.hst_bal=g_dd_mst.hst_bal-g_pub_tx.tx_amt1;
		}
	}
	else if( g_pub_tx.add_ind[0]=='1' )		/*���*/
	{
		g_dd_mst.bal=g_dd_mst.bal+g_pub_tx.tx_amt1;
		g_dd_mst.acbo_bal=g_dd_mst.acbo_bal+g_pub_tx.tx_amt1;
		/*** ��������Ϣ�����ĺ��� ***/

		if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
		{
			g_dd_mst.hst_bal=g_dd_mst.hst_bal+g_pub_tx.tx_amt1;
		}
	}
	else
	{
		sprintf(acErrMsg,"��ȡ��־����g_pub_tx.add_ind=[%s]!",g_pub_tx.add_ind);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P106");
		goto ErrExit;
	}
	vtcp_log("------look------[%s]",g_mdm_ac_rel.ac_no);

	g_dd_mst.lst_date=g_pub_tx.tx_date;

vtcp_log("XXXXXXX[%.2lf]",g_dd_mst.bal);
	if( g_dd_mst.bal<-0.005 && g_pub_tx.add_ind[0]!='1' )
	{
		strcpy(g_pub_tx.reply,"A022" );
		goto ErrExit;
	}

	if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
	{
		ret=pub_ins_dd_hst();
		ERR_DEAL
	}

/****
pub_mob_sendmail( g_pub_tx.tx_code,g_mdm_ac_rel.ac_no,g_pub_tx.add_ind,g_pub_tx.tx_amt1, g_dd_mst.bal );
***/
	vtcp_log("------look------[%s]",g_mdm_ac_rel.ac_no);
OkExit:
	return 0;
ErrExit:
	return 1;
}
int jx_unprt_hst_ins(char ac_type[2])
{
	int ret;
	struct mdm_unprt_hst_c	mdm_unprt_hst_c;

	memset(&mdm_unprt_hst_c,0x00,sizeof(struct mdm_unprt_hst_c));

		mdm_unprt_hst_c.trace_no=g_pub_tx.trace_no;
		mdm_unprt_hst_c.trace_cnt=g_pub_tx.trace_cnt;
		strcpy(mdm_unprt_hst_c.tx_code,g_pub_tx.tx_code);
		strcpy(mdm_unprt_hst_c.tx_br_no,g_pub_tx.tx_br_no);
		mdm_unprt_hst_c.ac_id=g_pub_tx.ac_id;
		mdm_unprt_hst_c.ac_seqn=g_pub_tx.ac_seqn;
		strcpy(mdm_unprt_hst_c.add_ind,g_pub_tx.add_ind);
		mdm_unprt_hst_c.tx_amt=g_pub_tx.tx_amt1;

		if( ac_type[0]=='1' )
			mdm_unprt_hst_c.bal=g_dd_mst.hst_bal;
		else if( ac_type[0]=='2' )
			mdm_unprt_hst_c.bal=g_td_mst.hst_bal;
		else if( ac_type[0]=='3' )
			mdm_unprt_hst_c.bal=g_ln_mst.hst_bal;
		else if( ac_type[0]=='9' )
			mdm_unprt_hst_c.bal=g_in_mst.hst_bal;
		else
		{
			sprintf(acErrMsg,"�Ǽ�δ��ӡ��ϸ���˺����ʹ���[%d]!",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P131");
			goto ErrExit;
		}
		mdm_unprt_hst_c.tx_date=g_pub_tx.tx_date;
		strcpy(mdm_unprt_hst_c.tel,g_pub_tx.tel);
		
		pub_base_unprt_brf(mdm_unprt_hst_c.brf);
		
		/*strncpy(mdm_unprt_hst_c.brf,g_pub_tx.brf,10);*/

		if ( ( pub_base_CompDblVal(mdm_unprt_hst_c.tx_amt,0.00) || \
				g_pub_tx.ac_wrk_ind[2]=='1') )
		{
			ret=Mdm_unprt_hst_Ins(mdm_unprt_hst_c,g_pub_tx.reply);
			if( ret )
			{
				sprintf(acErrMsg,"�Ǽ�δ��ӡ��ϸ�ʴ���[%d]!",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P131");
				goto ErrExit;
			}
		}

OkExit:
	return 0;
ErrExit:
	return 1;
}
