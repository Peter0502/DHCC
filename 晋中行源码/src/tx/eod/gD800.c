/*************************************************************
* 文 件 名: gD800.c
* 功能描述：单位协议存款户计息程序 ligl 20060918
**************************************************************/
#define MYSQLERR if( ret ) {\
	strcpy( g_pub_tx.reply,"AT03" );\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
	WRITEMSG\
	goto ErrExit;\
	}
#define MYERR if( ret ) {\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
	WRITEMSG\
	goto ErrExit;\
	}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "com_branch_c.h"
#include "com_eod_exec_c.h"
#include "intst_hst_c.h"
#include "ac_ac_rel_c.h"
#include "dd_mst_hst_c.h"
#include "dc_acc_rel_c.h"
struct com_sys_parm_c com_sys_parm;
extern struct com_eod_exec_c grz_com_eod_exec;/*全局,用于向子程序传递控制序列*/

gD800()
{
		int ret = 0;
    struct td_mst_c td_mst_c;
    struct dd_mst_c dd_mst_c;
    struct td_mst_c o_td_mst;
    struct td_parm_c td_parm_c;
    struct com_branch_c com_branch_c;
		struct S_pub_intst sum_intst;
		long first=0;
    memset(&td_mst_c,0x0,sizeof(struct td_mst_c));
    
    memset(&dd_mst_c,0x0,sizeof(struct dd_mst_c));
    
    memset(&o_td_mst,0x00,sizeof(struct td_mst_c));
    
    memset(&g_pub_tx,0x0,sizeof(g_pub_tx));
    
    memset(&td_parm_c,0x0,sizeof(struct td_parm_c));
    
    memset(&com_branch_c,0x00,sizeof(struct com_branch_c));
    
    memset(&g_pub_intst,0x0,sizeof(g_pub_intst));
    
    memset(&sum_intst,0x0,sizeof(sum_intst));
    
		show_log( "-----------begin--------------");
    ret=sql_begin(); /*打开事务*/
		MYSQLERR
    /**------- 初始化全局变量 --------**/
    pub_base_sysinit();
		ret=pub_base_GetSysparm(&com_sys_parm);
		MYERR
		g_pub_tx.tx_date=com_sys_parm.sys_date;
    ret =Td_mst_Dec_Sel(g_pub_tx.reply,"ac_sts='1' and (prdt_no='257' or prdt_no='258') and opn_br_no in(select br_no from com_branch where substr(spe_ind,2,1)='0') order by opn_br_no,prdt_no");
		MYSQLERR
    while(1)
    {
        memset(&td_mst_c,0x0,sizeof(struct td_mst_c));
        ret = Td_mst_Fet_Sel(&td_mst_c, g_pub_tx.reply);
				if( ret==100 ) break;
				MYSQLERR
        strcpy(td_parm_c.prdt_no,td_mst_c.prdt_no);
        ret = Td_parm_Sel( g_pub_tx.reply,&td_parm_c,"prdt_no='%s'",td_parm_c.prdt_no);
        if ( ret )
        {
            	sprintf(acErrMsg,"查询定期产品表错误[%s]!!!",td_mst_c.prdt_no);
            	WRITEMSG
            	goto ErrExit;
        }
				if( pub_base_CompDblVal(td_mst_c.bal,0.00)<0 )
				{
        	memcpy(&o_td_mst,&td_mst_c,sizeof(td_mst_c));
					vtcp_log("FFFFFFFFFFFFFFFFFFFFFF");
					continue;
				}
				ret=sub_cal_intst(td_mst_c,td_parm_c);
				MYERR
    }
    Td_mst_Clo_Sel();

	set_zd_data(DC_CODE,"");
	set_zd_double("102P",0.00 );
	set_zd_double("102Q",0.00 );
  set_zd_data("102J","");
  memset(&g_pub_intst,0x0,sizeof(g_pub_intst));
	ret=sql_execute("update com_branch set spe_ind=substr(spe_ind,1,1)||0||substr(spe_ind,3,6) where 1=1");
MYERR                                   

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"对定期协议户计息程序成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"对定期协议户计息程序失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
int sub_cal_intst( td_mst_c,td_parm_c)
 struct td_mst_c td_mst_c;
 struct td_parm_c td_parm_c;
{
	long caldate,tmp_end_date=0;
	int ret=0,tmpday=0;
	double tmpacm=0.00,tmpbal=0.00;
	double lx=0.00;
	double lxs=0.00;
	double sdsl=0.00;
	char num[51];
	char tmptype[2];
	double intst_acm=0.00;
	struct intst_hst_c	T_intst_hst;
	struct dd_mst_c tmp_dd_mst;
	struct td_mst_c tmp_td_mst;
	struct ac_ac_rel_c ac_ac_rel;
	struct dd_mst_c g_dd_mst;
	memset( &ac_ac_rel,0x00,sizeof(struct ac_ac_rel_c));
	memset(	&T_intst_hst,0x00,sizeof(struct intst_hst_c));
  memset( &g_pub_intst,0x0,sizeof(g_pub_intst));
  memset( &g_dd_mst,0x00,sizeof(g_dd_mst));
  memset( &tmp_dd_mst,0x00,sizeof(struct dd_mst_c));
  memset( &tmp_td_mst,0x00,sizeof(struct td_mst_c));
	memcpy( &g_td_mst,&td_mst_c,sizeof(g_td_mst));
	memcpy( &g_td_parm,&td_parm_c,sizeof(g_td_parm));
	ret=Ac_ac_rel_Sel(g_pub_tx.reply,&ac_ac_rel,"ac_id=%ld",g_td_mst.ac_id);
	MYERR
	ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld",ac_ac_rel.ac_id1);
	MYERR
	/**计算利息积数**/
	/*-----------------------------------------------------------
   if( pub_base_CalAcm(g_td_mst.lst_date,g_pub_tx.tx_date,
            g_td_parm.intst_term_type,g_td_parm.acm_calc_mode,
            g_td_mst.bal,0.00,&g_td_mst.intst_acm,
            g_td_mst.intst_type,g_td_parm.intst_mon,g_td_parm.intst_day))
   {
            sprintf(acErrMsg,"计算利息积数错误!");
            WRITEMSG
            goto ErrExit;
   }
   */
   /*计算天数*/
   ret = pub_base_CalDays( g_td_mst.lst_date , g_pub_tx.tx_date , g_td_parm.intst_term_type , &caldate);
			if ( ret )
			{
				sprintf(acErrMsg,"调用计算天数函数错误!");
				WRITEMSG
				return 1;	
			}
		tmpacm=g_td_mst.intst_acm;
		sprintf(acErrMsg,"acm积数为[%lf]",tmpacm);
		WRITEMSG	
		sprintf(acErrMsg,"date积数为[%ld]",caldate);
		WRITEMSG	
		memset(num,0,sizeof(num));
		sprintf(num,"%.0f",g_td_mst.bal);
		tmpbal= atof(num);
		tmpacm += caldate * tmpbal;
		sprintf(acErrMsg,"acm积数为[%lf]",tmpacm);
		WRITEMSG
	/* 四舍五入 */
	tmpacm= pub_base_PubDround_1(tmpacm);	
	sprintf(acErrMsg,"积数为[%lf]",tmpacm);
	WRITEMSG	
	 g_td_mst.intst_acm=tmpacm;
   sprintf(acErrMsg,"计算利息前 intst_acm=[%lf]",g_td_mst.intst_acm);
   WRITEMSG
   g_pub_tx.ac_id_type[0] = '2';
   /**计算利息**/
   lx = g_td_mst.rate * g_td_mst.intst_acm/ 360 / 100;
   g_pub_intst.dealval=pub_base_PubDround_1(lx);
   sprintf(acErrMsg,"利息为=[%.2lf] ",g_pub_intst.dealval);
   WRITEMSG
   sprintf(acErrMsg,"计算利息后 intst_acm=[%lf] ",g_td_mst.intst_acm);
   WRITEMSG   
	 T_intst_hst.bic_bal=g_td_mst.bal;
	 T_intst_hst.bic_acm=g_td_mst.intst_acm;
	 T_intst_hst.fct_intst=g_pub_intst.dealval;
	 T_intst_hst.intst_tax=g_pub_intst.val;
	 T_intst_hst.ic_date=g_td_mst.ic_date;
		
		strcpy( g_pub_tx.ac_no,"");
		g_pub_tx.ac_id=g_td_mst.ac_id;
		g_pub_tx.ac_seqn=g_td_mst.ac_seqn;
		strcpy( g_pub_tx.cur_no,g_td_parm.cur_no);
		strcpy( g_pub_tx.prdt_code,g_td_parm.prdt_no);
		strcpy( g_pub_tx.ct_ind,"2" );
   	g_td_mst.intst_acm = 0.00;
   	td_mst_c.intst_acm = 0.00;
		strcpy( g_pub_tx.ac_wrk_ind,"0200000J1" );
		g_pub_tx.svc_ind=1012;
		strcpy( g_pub_tx.hst_ind,"1" );
		strcpy( g_pub_tx.add_ind,"1" );
		strcpy( g_pub_tx.ac_get_ind,"011" );
		strcpy( g_pub_tx.brf,"结息" );
		g_pub_tx.tx_amt1=g_pub_intst.dealval;
   	g_td_mst.ic_date = g_pub_tx.tx_date;
   	g_td_mst.lst_date = g_pub_tx.tx_date;
		ret=Td_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%d and ac_seqn=%d",g_td_mst.ac_id,g_td_mst.ac_seqn);
		MYERR
		ret=Td_mst_Fet_Upd(&tmp_td_mst,g_pub_tx.reply);
		MYERR
		ret=Td_mst_Upd_Upd(g_td_mst,g_pub_tx.reply);
		Td_mst_Clo_Upd();	
		strcpy(g_pub_tx.opn_br_no,g_dd_mst.opn_br_no);	
			if(g_pub_tx.hst_ind[0]=='1')		/*日间入明细*/
			{
				ret=pub_ins_td_hst();			
				MYERR
			}
		strcpy(g_pub_tx.cur_no,g_td_parm.cur_no);
		strcpy(g_pub_tx.dc_code,g_td_parm.dc_code);
		strcpy(g_pub_tx.prdt_code,g_td_mst.prdt_no);
		strcpy(g_pub_tx.name,g_td_mst.name);
		g_pub_tx.cif_no=g_td_mst.cif_no;
		/*set_zd_data(DC_CODE,g_pub_tx.dc_code);
		get_zd_long( DC_TRACE_CNT , &g_pub_tx.trace_cnt);
    g_pub_tx.trace_cnt++;
    set_zd_long( DC_TRACE_CNT , g_pub_tx.trace_cnt);
    */
		ret=Dd_mst_Dec_Upd( g_pub_tx.reply ,"ac_id=%d and ac_seqn=%d" ,
			g_dd_mst.ac_id,g_dd_mst.ac_seqn );
		MYERR
		ret=Dd_mst_Fet_Upd( &tmp_dd_mst , g_pub_tx.reply);
		MYERR
		g_dd_mst.bal+=g_pub_tx.tx_amt1;
		g_dd_mst.acbo_bal+=g_pub_tx.tx_amt1;
		g_dd_mst.hst_bal+=g_pub_tx.tx_amt1;
		ret=Dd_mst_Upd_Upd( g_dd_mst , g_pub_tx.reply);
		Dd_mst_Clo_Upd();
		
		strcpy( g_pub_tx.ac_no,"");
		g_pub_tx.ac_id=g_dd_mst.ac_id;
		g_pub_tx.ac_seqn=g_dd_mst.ac_seqn;
		
		strcpy( g_pub_tx.cur_no,"01");
		strcpy( g_pub_tx.prdt_code,g_dd_mst.prdt_no);
		strcpy( g_pub_tx.ct_ind,"2" );
		strcpy( g_pub_tx.ac_wrk_ind,"0200000J1" );
		g_pub_tx.svc_ind=1012;
		strcpy( g_pub_tx.hst_ind,"1" );
		strcpy( g_pub_tx.add_ind,"1" );
		strcpy( g_pub_tx.ac_get_ind,"011" );
		strcpy( g_pub_tx.brf,"协议结息转入" );
		g_pub_tx.tx_amt1=g_pub_intst.dealval;	
		strcpy(g_pub_tx.opn_br_no,g_dd_mst.opn_br_no);	
		if(g_pub_tx.hst_ind[0]=='1')		/*日间入明细*/
		{
				vtcp_log("aaaa %.2lf,%.2lf,%.2lf\n",g_dd_mst.bal,g_dd_mst.acbo_bal,g_dd_mst.hst_bal);
				ret=pub_insert_dd_hst(g_dd_mst);
				MYERR
		}
		/*--- hao --- @20050915@ ---- 登记结息登记薄(要求在数据库中保留明细) --放定期帐号--*/
		strcpy(T_intst_hst.br_no,g_td_mst.opn_br_no);
		T_intst_hst.ac_id=g_td_mst.ac_id;
		T_intst_hst.ac_seqn=g_td_mst.ac_seqn;
		strcpy(T_intst_hst.prdt_no,g_td_mst.prdt_no);
		strcpy(T_intst_hst.cur_no,g_td_parm.cur_no);
		T_intst_hst.aic_bal=g_td_mst.bal;
		T_intst_hst.aic_acm=g_td_mst.intst_acm;
		T_intst_hst.intst_date=g_pub_tx.tx_date;
		strcpy(T_intst_hst.ac_ind,"1");
		strcpy(T_intst_hst.pt_ind,"1");
		T_intst_hst.rate=g_td_mst.rate;
		ret=Intst_hst_Ins(T_intst_hst,g_pub_tx.reply);
		MYERR
		ret=intst_acct(g_dd_mst,g_td_mst,g_pub_tx.tx_amt1);
		MYERR
	return 0;
ErrExit:
	return 1;
}
int pub_insert_dd_hst(struct dd_mst_c dd_mst)
{
	int ret;

	struct dd_mst_hst_c	dd_mst_hst_c;
	memset(&dd_mst_hst_c,0x00,sizeof(struct dd_mst_hst_c));

	dd_mst_hst_c.trace_no=g_pub_tx.trace_no;
	dd_mst_hst_c.trace_cnt=g_pub_tx.trace_cnt;
	strcpy(dd_mst_hst_c.opn_br_no,g_pub_tx.opn_br_no);	
	strcpy(dd_mst_hst_c.tx_br_no,g_pub_tx.tx_br_no);	
	dd_mst_hst_c.ac_id=g_pub_tx.ac_id;
	dd_mst_hst_c.ac_seqn=g_pub_tx.ac_seqn;
	strcpy(dd_mst_hst_c.tx_code,g_pub_tx.tx_code);	
	strcpy(dd_mst_hst_c.sub_tx_code,g_pub_tx.sub_tx_code);	
	vtcp_log("aaaa %.2lf,%.2lf,%.2lf\n",dd_mst.bal,dd_mst.acbo_bal,dd_mst.hst_bal);
	if( g_pub_tx.add_ind[0]=='2' || g_pub_tx.add_ind[0]=='0' 
		|| g_pub_tx.add_ind[0]=='3' )
		strcpy(dd_mst_hst_c.add_ind,"0");	
	else
		strcpy(dd_mst_hst_c.add_ind,"1");	
    strcpy(dd_mst_hst_c.ct_ind,g_pub_tx.ct_ind);
	dd_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
	dd_mst_hst_c.bal=dd_mst.hst_bal;
	dd_mst_hst_c.acbo_bal=dd_mst.acbo_bal;
	dd_mst_hst_c.intst_acm=dd_mst.intst_acm;
	dd_mst_hst_c.tx_date=g_pub_tx.tx_date;
	dd_mst_hst_c.tx_time=g_pub_tx.tx_time;
	vtcp_log("aaaa %.2lf,%.2lf,%.2lf\n",dd_mst.bal,dd_mst.acbo_bal,dd_mst.hst_bal);
	strcpy(dd_mst_hst_c.note_type,g_pub_tx.note_type);	
	strcpy(dd_mst_hst_c.note_no,g_pub_tx.beg_note_no);
	pub_base_unprt_brf( dd_mst_hst_c.brf );	
	strcpy(dd_mst_hst_c.tel,g_pub_tx.tel);	
	strcpy(dd_mst_hst_c.chk,g_pub_tx.chk);	
	strcpy(dd_mst_hst_c.auth,g_pub_tx.auth);	
	get_zd_long( DC_TRACE_CNT , &dd_mst_hst_c.trace_cnt);
	dd_mst_hst_c.trace_cnt++;

	if( ( pub_base_CompDblVal(dd_mst_hst_c.tx_amt,0.00) 
		|| g_pub_tx.ac_wrk_ind[2]=='1') && g_pub_tx.hst_ind[0]=='1' )
	{
		dd_mst.hst_cnt++;
		dd_mst_hst_c.hst_cnt=dd_mst.hst_cnt;
		ret=Dd_mst_hst_Ins(dd_mst_hst_c,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"登记活期明细帐错误[%d]!",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P127");
			goto ErrExit;
		}
	}

OkExit:	
	return 0;
ErrExit:
	return 1;
}
int intst_acct( struct dd_mst_c dd_mst_c,struct td_mst_c td_mst_c,double sum_intst)
{
	int ret=0;
	struct dd_parm_c dd_parm;
	struct td_parm_c td_parm;
	struct dc_acc_rel_c dc_acc_rel;
	memset(&dd_parm,0x0,sizeof(struct dd_parm_c));
	memset(&td_parm,0x0,sizeof(struct td_parm_c));
	memset(&dc_acc_rel,0x0,sizeof(struct dc_acc_rel_c));
	strcpy(g_pub_tx.tx_br_no,dd_mst_c.opn_br_no);
	strcpy(g_pub_tx.opn_br_no,dd_mst_c.opn_br_no);
	strcpy(g_pub_tx.cur_no,"01");
  strcpy(g_pub_tx.ac_wrk_ind,"0000000J");
	g_pub_tx.svc_ind=1005;
	strcpy( g_pub_tx.ct_ind,"2" );
	strcpy( g_pub_tx.add_ind,"1" );
	strcpy( g_pub_tx.prdt_code,dd_mst_c.prdt_no );
	vtcp_log("bbbbb%s\n",g_pub_tx.prdt_code);
	ret=Dd_parm_Sel(g_pub_tx.reply,&dd_parm,"prdt_no='%s' ",dd_mst_c.prdt_no);
	MYERR
	set_zd_data(DC_CODE,dd_parm.dc_code);
	strcpy( g_pub_tx.ac_no,"");
	g_pub_tx.ac_id=0;
	g_pub_tx.ac_seqn=0;
	strcpy( g_pub_tx.note_type,"" );
	strcpy( g_pub_tx.beg_note_no,"");
	g_pub_tx.tx_amt1=sum_intst;
	strcpy( g_pub_tx.no_show,"0" );

		ret=pub_ins_trace_log();
		if( ret ) goto ErrExit;

	set_zd_data("101A","01");
	set_zd_data("101B","2");
	set_zd_double("1013",sum_intst);

        ret = pubf_acct_proc("D099");
		if( ret ) goto ErrExit;
			
	/*-----------------------------------------------------*/	
		strcpy(g_pub_tx.tx_br_no,dd_mst_c.opn_br_no);
		strcpy(g_pub_tx.opn_br_no,dd_mst_c.opn_br_no);
		vtcp_log("KKKKK,g_pub_tx.ac_no[%s]-----------------",g_pub_tx.ac_no);
		ret=Td_parm_Sel(g_pub_tx.reply,&td_parm,"prdt_no='%s' ",td_mst_c.prdt_no);
		MYERR
		memset(&dc_acc_rel,0x0,sizeof(struct dc_acc_rel_c));
		ret=Dc_acc_rel_Sel(g_pub_tx.reply,&dc_acc_rel,"dc_code='%s' and data_code='0153' ",td_parm.dc_code);
			strcpy( g_pub_tx.ac_no,dc_acc_rel.acc_hrt);
		/*get_zd_data("0153",g_pub_tx.ac_no);*/
		vtcp_log("KKKKK,g_pub_tx.ac_no[%s]-----------------",g_pub_tx.ac_no);
       	g_pub_tx.ac_id = 0;
       	g_pub_tx.ac_seqn = 0;
        strcpy(g_pub_tx.ac_get_ind,"00");
        strcpy(g_pub_tx.ac_id_type,"9");
        strcpy(g_pub_tx.add_ind,"0");		/** 不是减少，是借方 **/
    	strcpy(g_pub_tx.ac_wrk_ind,"0000000J");
        strcpy(g_pub_tx.hst_ind,"1");
        g_pub_tx.svc_ind=9001;
        g_pub_tx.tx_amt1 = sum_intst;
		strcpy(g_pub_tx.brf,"对公结息");

        ret=pub_acct_trance();
        if( ret )
        {
            sprintf(acErrMsg,"利息支出科目记账错误!!");
            WRITEMSG
            goto ErrExit;
        }

        set_zd_data("1204",g_pub_tx.cur_no);
        set_zd_data("1205",g_pub_tx.ct_ind);
        set_zd_double("1208",g_pub_tx.tx_amt1);
        ret = pubf_acct_proc("A016");
        if (ret != 0)
        {
            sprintf(acErrMsg,"会计记帐不成功!!");
            WRITEMSG
            goto ErrExit;
        }

OkExit:
	return 0;
ErrExit:
	return 1;
}
