/**********************************************************/
/*                �����ʽ����������                    */
/*                                                        */
/*                                                        */
/*                                                        */
/*                                                        */
/*                                                        */
/*                                                        */
/*                                                        */
/**********************************************************/
#define ERR_DEAL if(ret){sprintf(acErrMsg,"ERROR");WRITEMSG goto ErrExit;}
#include <stdio.h>
#include <math.h>
#define EXTERN
#define  PG_CNT   45
#include "public.h"
#include "prdt_ac_id_c.h"
#include "trace_log_c.h"
#include "com_item_c.h"
#include "gl_mst_c.h"
#include "gl_sub_c.h"
#include "zjgl_mst_c.h"
#include "zjgl_mst_hst_c.h"
#include "dc_acc_rel_c.h"
#include "in_parm_c.h"
#include "in_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "com_item_add_c.h"
#include "mo_bank_acc_po_c.h"
#include "mo_bail_rate_c.h"
#include "impa_dep_reg_c.h"
#include "com_parm_c.h"
#include "mdm_ac_rel_c.h"
#include "in_parm_c.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"
#include "in_mst_c.h"

extern char *pub_base_daynumLAT(char *, int);
char *pubf_acct_acctoact(char *,char *);

int ipubf_acct_zjgl_trance(char *in_opn_br,char *cur_no,char *in_acc_no,double *in_txamt,double *in_bal)
{
    int    ret = 0;
    int    i=0;
    double dtxamt = 0;
    double dbal = 0;
    double zc_bal = 0;
    double o_tx_amt1 = 0;
    char   chk_acc_hrt[6];
    struct zjgl_mst_c wp_zjgl_mst;
    struct com_item_add_c com_item_add;
    struct com_item_c com_item;

    dtxamt = *in_txamt;
    dbal = *in_bal;


    ret = Com_item_Sel(g_pub_tx.reply,&com_item,"acc_no='%s'",in_acc_no);
    	    ERR_DEAL
    memset(chk_acc_hrt,0,sizeof(chk_acc_hrt));
    strcpy(chk_acc_hrt,com_item.acc_hrt);
    pub_base_strpack(chk_acc_hrt);
    /** �ȼ������ʽ��Ƿ�͸֧, �ϼ���ĿҲ�ж� **/
    for(i=0;i<3;i++)
    {
          ret = Com_item_Sel(g_pub_tx.reply,&com_item,"acc_hrt='%s'",chk_acc_hrt);
    	    ERR_DEAL
          
          ret=pub_acct_UpdZjLimAmt(com_item.acc_no,in_opn_br,cur_no,dtxamt);
          ERR_DEAL
          
          if(!strcmp(com_item.up_acc_hrt,com_item.acc_hrt))
          	 break;
          
          memset(chk_acc_hrt,0,sizeof(chk_acc_hrt));
          strcpy(chk_acc_hrt,com_item.up_acc_hrt);
          pub_base_strpack(chk_acc_hrt);          
    }

    /** ����Ҫ�޸ĵļ�¼ **/
    ret = Zjgl_mst_Dec_Upd( g_pub_tx.reply , "opn_br_no='%s' and acc_no='%s' and cur_no='%s'",\
                            in_opn_br,in_acc_no,cur_no );
    ERR_DEAL

    ret = Zjgl_mst_Fet_Upd(&wp_zjgl_mst, g_pub_tx.reply );
    ERR_DEAL
  
    Zjgl_mst_Debug(&wp_zjgl_mst); 
 
    if(pub_base_CompDblVal(dtxamt,0.00)!=0)
    {
        zc_bal = wp_zjgl_mst.dr_bal - wp_zjgl_mst.cr_bal;
        zc_bal+=dtxamt;
    }else if(pub_base_CompDblVal(dbal,0.00)!=0)
    {
        zc_bal=dbal;
    }else{
        sprintf(acErrMsg,"[%s][%d]���׽�������ͬʱΪ��!",__FILE__,__LINE__);
        WRITEMSG 
/*        goto ErrExit; delete by liuxuan*/
    }

    /** ����,�ۼӻ��� **/
    ret = pubf_acct_zjgl_acm(&wp_zjgl_mst);
    ERR_DEAL
   
    /** �޸����,������� **/
    if(pub_base_CompDblVal(zc_bal,0.00)>=0)
    {
        wp_zjgl_mst.ys_dr_bal = wp_zjgl_mst.dr_bal;
        wp_zjgl_mst.ys_cr_bal = wp_zjgl_mst.cr_bal;
        wp_zjgl_mst.dr_bal = zc_bal;
        wp_zjgl_mst.cr_bal = 0;
    }else{
    	  wp_zjgl_mst.ys_dr_bal = wp_zjgl_mst.dr_bal;
        wp_zjgl_mst.ys_cr_bal = wp_zjgl_mst.cr_bal;
        wp_zjgl_mst.cr_bal = 0-zc_bal;
        wp_zjgl_mst.dr_bal = 0;
    }

    if(pub_base_CompDblVal(dtxamt,0.00)!=0)
    {
        *in_bal = zc_bal;
    }else if(pub_base_CompDblVal(dbal,0.00)!=0)
    {
        *in_txamt=(wp_zjgl_mst.dr_bal-wp_zjgl_mst.cr_bal)-(wp_zjgl_mst.ys_dr_bal-wp_zjgl_mst.ys_cr_bal);
    }else{
        sprintf(acErrMsg,"[%s][%d]���׽�������ͬʱΪ��!",__FILE__,__LINE__);
        WRITEMSG 
        /*goto ErrExit; delete by liuxuan*/
    }


    /** �޸���ϸ�ʱ���,ҳ�� **/
    wp_zjgl_mst.hst_cnt++;
    if(wp_zjgl_mst.hst_cnt%PG_CNT==0)
        wp_zjgl_mst.hst_pg++;

    ret = Zjgl_mst_Upd_Upd(wp_zjgl_mst, g_pub_tx.reply );
    ERR_DEAL

    Zjgl_mst_Clo_Upd();
	
    /** ׼��g_pub_tx���� **/
    o_tx_amt1 = g_pub_tx.tx_amt1;
    g_pub_tx.tx_amt1 = *in_txamt;
    strcpy(g_pub_tx.opn_br_no,wp_zjgl_mst.opn_br_no);

    /** ������ϸ�� **/
    ret = pub_ins_zjgl_hst(wp_zjgl_mst);
    if ( ret )
    {
        sprintf(acErrMsg,"[%s][%d]�Ǽ��ʽ������ϸ����[%d]!",__FILE__,__LINE__,ret);
        WRITEMSG
        goto ErrExit;
    }

    /** ����������ܵǼ���ˮ **
    if(!memcmp(g_pub_tx.tel,"9999",sizeof(g_pub_tx.tel)-1))
    if( pub_ins_trace_log() )
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
        WRITEMSG
        goto ErrExit;
    }
	*/
    
    g_pub_tx.tx_amt1 = o_tx_amt1;

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"���¿����ʽ���ɹ�!");
    WRITEMSG
    return 0;
ErrExit:
    sprintf(acErrMsg,"���¿����ʽ���˴���ʧ��![%s]",g_pub_tx.reply);
    WRITEMSG
    return 1;
}

int pub_acct_UpdZjLimAmt(char * acc_no,char *in_opn_br,char *cur_no,double dtxamt)
{
	  int ret = 0;
	  struct zjgl_mst_c wp_zjgl_mst;
    struct com_item_add_c com_item_add;
    struct com_item_c com_item;

	memset(&wp_zjgl_mst,'\0',sizeof(wp_zjgl_mst)); 
	memset(&com_item_add,'\0',sizeof(com_item_add));
	memset(&com_item,'\0',sizeof(com_item));

	vtcp_log("[%s][%d]acc_no is [%s]\n",__FILE__,__LINE__,acc_no);
	  /** ����Ҫ�޸ĵļ�¼ **/
    ret = Zjgl_mst_Dec_Upd( g_pub_tx.reply , "opn_br_no='%s' and acc_no='%s' and cur_no='%s'",\
                            in_opn_br,acc_no,cur_no );
    if(ret==100) goto OkExit;
    ERR_DEAL

    ret = Zjgl_mst_Fet_Upd(&wp_zjgl_mst, g_pub_tx.reply );
    if(ret==100) goto OkExit;	/* added by liuxuan 2006-11-23 */
    ERR_DEAL
    
	   /** �ж��ʻ��Ƿ��޶������� **/
    if(  wp_zjgl_mst.lim_term[0] != '0' && \
    	   g_pub_tx.tx_date>=wp_zjgl_mst.lim_beg_date && \
    	   g_pub_tx.tx_date<=wp_zjgl_mst.lim_end_date)   	
    {
    	    if(pub_base_CompDblVal(dtxamt,0.00)==0){
               sprintf(acErrMsg,"[%s][%d]��͸֧�����׽���Ϊ��!",__FILE__,__LINE__);
               WRITEMSG 
               goto ErrExit;
           }
    	
    	     ret = Com_item_add_Sel(g_pub_tx.reply,&com_item_add,"acc_hrt='%s'",wp_zjgl_mst.acc_hrt);
    	     ERR_DEAL
    	     
    	     ret = Com_item_Sel(g_pub_tx.reply,&com_item,"acc_hrt='%s'",wp_zjgl_mst.acc_hrt);
    	     ERR_DEAL
    	     
    	     if(com_item.dc_ind[0]=='0')
    	     	  com_item.dc_ind[0]='1';
    	     	  
    	   vtcp_log("[%s][%d]zjgl.lim_bal[%.2f]zjgl.lim_amt[%.2f]   ddtxamt[%.2f]\n",__FILE__,__LINE__, wp_zjgl_mst.lim_bal, wp_zjgl_mst.lim_amt, dtxamt);
    	     if(com_item_add.add_ind[0] == com_item.dc_ind[0])
    	     {
    	     	vtcp_log("[%s][%d]�����������[%.2f]\n",__FILE__,__LINE__,dtxamt);
    	     	   if(dtxamt>0.005)
    	     	   {
    	     	   	    /** �ʻ��Ѿ����޶�� **/
                   if(wp_zjgl_mst.lim_bal+dtxamt-wp_zjgl_mst.lim_amt>0.005)
                   {
                   	    sprintf(acErrMsg,"[%s][%d]�ÿ�Ŀ��ʹ�ý��޶�!",__FILE__,__LINE__);
                        WRITEMSG 
                        goto ErrExit;
                   }                                 
    	     	   }
    	     }else{
    	     	vtcp_log("[%s][%d]���Ƽ������[%.2f]\n",__FILE__,__LINE__,dtxamt);
    	     	  if(dtxamt< -0.005)
    	     	  {
    	     	   	    /** �ʻ��Ѿ����޶�� **/
                   if(wp_zjgl_mst.lim_bal+dtxamt-wp_zjgl_mst.lim_amt<-0.005)
                   {
                   	    sprintf(acErrMsg,"[%s][%d]�ÿ�Ŀ��ʹ�ý��޶�!",__FILE__,__LINE__);
                        WRITEMSG 
                        goto ErrExit;
                   }                                 
    	     	  }
    	     }

           /** �޸��ۼƿ��ƽ�� **/
    	     wp_zjgl_mst.lim_bal+=dtxamt;
    	     
    	     ret = Zjgl_mst_Upd_Upd(wp_zjgl_mst, g_pub_tx.reply );
           ERR_DEAL          
    }
    Zjgl_mst_Clo_Upd();
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"����ʽ��޶����!");
    WRITEMSG
    return 0;
ErrExit:
    strcpy(g_pub_tx.reply,"0001");
    sprintf(acErrMsg,"����ʽ��޶�ʧ��![%s]",g_pub_tx.reply);
    WRITEMSG
    return 1;
}

int pub_ins_zjgl_hst(struct zjgl_mst_c g_zjgl_mst)
{
    int ret;

    struct zjgl_mst_hst_c zjgl_mst_hst_c;
    memset(&zjgl_mst_hst_c,0x00,sizeof(struct zjgl_mst_hst_c));

    zjgl_mst_hst_c.trace_no=g_pub_tx.trace_no;
    zjgl_mst_hst_c.trace_cnt=g_pub_tx.trace_cnt;
    strcpy(zjgl_mst_hst_c.opn_br_no,g_pub_tx.opn_br_no);
    strcpy(zjgl_mst_hst_c.tx_br_no,g_pub_tx.tx_br_no);
    strcpy(zjgl_mst_hst_c.acc_hrt,g_zjgl_mst.acc_hrt);
    strcpy(zjgl_mst_hst_c.acc_no,g_zjgl_mst.acc_no);
    strcpy(zjgl_mst_hst_c.tx_code,g_pub_tx.tx_code);
    strcpy(zjgl_mst_hst_c.sub_tx_code,g_pub_tx.sub_tx_code);

    strcpy(zjgl_mst_hst_c.add_ind,"0");

    zjgl_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
	zjgl_mst_hst_c.dr_bal=g_zjgl_mst.dr_bal;
	zjgl_mst_hst_c.cr_bal=g_zjgl_mst.cr_bal;
    zjgl_mst_hst_c.dr_intst_acm=g_zjgl_mst.dr_intst_acm;
    zjgl_mst_hst_c.cr_intst_acm=g_zjgl_mst.cr_intst_acm;
    zjgl_mst_hst_c.tx_date=g_pub_tx.tx_date;
    zjgl_mst_hst_c.tx_time=g_pub_tx.tx_time;
    strcpy(zjgl_mst_hst_c.cur_no,g_zjgl_mst.cur_no);
    strcpy(zjgl_mst_hst_c.note_type,g_pub_tx.note_type);
    strcpy(zjgl_mst_hst_c.note_no,g_pub_tx.beg_note_no);

    pub_base_unprt_brf( zjgl_mst_hst_c.brf );

    strcpy(zjgl_mst_hst_c.tel,g_pub_tx.tel);
    strcpy(zjgl_mst_hst_c.chk,g_pub_tx.chk);
    strcpy(zjgl_mst_hst_c.auth,g_pub_tx.auth);
    get_zd_long( DC_TRACE_CNT , &zjgl_mst_hst_c.trace_cnt);
    zjgl_mst_hst_c.trace_cnt++;

    if( pub_base_CompDblVal(zjgl_mst_hst_c.tx_amt,0.00)!=0)
    {
        zjgl_mst_hst_c.hst_cnt=g_zjgl_mst.hst_cnt;
        ret=Zjgl_mst_hst_Ins(zjgl_mst_hst_c,g_pub_tx.reply);
        if( ret )
        {
            sprintf(acErrMsg,"�Ǽ��ʽ������ϸ�ʴ���[%d]!",ret);
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

/** ����,�ۼӻ��� **/
int pubf_acct_zjgl_acm(struct zjgl_mst_c *wp_zjgl_mst)
{
    int    ret = 0;
    long    days = 0;
    double vacm = 0;
    double vamt = 0;
	  vtcp_log("aaaaaaaaaaaaaaaaaaa2222aaaaaaaaa[%ld]aaaa=[%ld]\n",g_pub_tx.tx_date,wp_zjgl_mst->lst_date);
    ret = pub_base_CalDays( wp_zjgl_mst->lst_date , g_pub_tx.tx_date , "0" , &days );
    if ( ret )
    {
        sprintf(acErrMsg,"���ü���������������!");
        WRITEMSG
        goto ErrExit;
    }
    sprintf(acErrMsg,"���ü����������� PASS!����Ϊ[%ld]",days);
    WRITEMSG
    vtcp_log("+++++++++++1111++++++++==[%ld]\n",days);
    vamt = wp_zjgl_mst->dr_bal-wp_zjgl_mst->cr_bal;
    vtcp_log("==========3333==========[%.2lf]\n",vamt);
    /*
    vacm = days*pub_base_floor(vamt);
    */
    vacm=days*floor(vamt);
    vtcp_log("==========3333==========[%.2lf]\n",floor(vamt));
    sprintf(acErrMsg,"[%s][%d]vacm=[%.2lf]",__FILE__,__LINE__,vacm);
    WRITEMSG
    if(pub_base_CompDblVal(vacm,0.00)>=0)
    {
        wp_zjgl_mst->dr_intst_acm+=vacm;
    }else{
        wp_zjgl_mst->cr_intst_acm+=(0-vacm);
    }
    sprintf(acErrMsg,"[%s][%d] wp_zjgl_mst->cr_intst_acm[%.2lf]",__FILE__,__LINE__,wp_zjgl_mst->cr_intst_acm);
    WRITEMSG
    /** �޸��ϱʷ������� **/
    wp_zjgl_mst->lst_date = g_pub_tx.tx_date;

OkExit:
    return 0;
ErrExit:
    return 1;
}

/** ������Ϣ���� **/
int pubf_base_zjgl_intst(struct zjgl_mst_c *wd_zjgl_mst,struct S_pub_intst *s_pub_intst)
{
    int    ret = 0;
    double dacm = 0.00;
    double rate_val = 0.00;
    double dealval=0.00;
    double factval=0.00;
    double val=0.00;
    double keepval=0.00;
    char   buf[51];

    ret = pubf_acct_zjgl_acm(wd_zjgl_mst);
    if ( ret )
    {
        sprintf(acErrMsg,"[%s][%d]�����������!",__FILE__,__LINE__);
        WRITEMSG
        goto ErrExit;
    }
  
    /** �õ���Ϣ���� **/
    dacm = wd_zjgl_mst->dr_intst_acm-wd_zjgl_mst->cr_intst_acm;
    
    wd_zjgl_mst->dr_intst_acm = 0;
    wd_zjgl_mst->cr_intst_acm = 0;
    
    /** ȡ���� **/
	vtcp_log("[%s][%d]rate_type is [%s]\n",__FILE__,__LINE__,wd_zjgl_mst->rate_type);
    ret = pub_base_getllz(wd_zjgl_mst->rate_type,"01",g_pub_tx.tx_date,&rate_val);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
        WRITEMSG
        memset(g_pub_tx.reply, 0x0, sizeof(g_pub_tx.reply));
        strcpy(g_pub_tx.reply,"W110");
        return ret;
    }
	vtcp_log("[%s][%d]����rate[%.4f],type[%s]\n",__FILE__,__LINE__,rate_val,wd_zjgl_mst->rate_type);

    dealval = rate_val * dacm / L360 /L100;

    /** ��������,��ȷ���� **/
    memset(buf,0,sizeof(buf));
    sprintf(buf,"%.0f",(dealval+0.005)*100);
    dealval = atof(buf)/100;

    /* ����Ϣ�ṹ��ֵ */
    s_pub_intst->dealval = dealval;
    s_pub_intst->factval = factval;
    s_pub_intst->val = val;
    s_pub_intst->keepval = keepval;

OkExit:
    return 0;
ErrExit:
    return 1;

}

/** ���ݿ�Ŀ���ض�Ӧ�ʺ�           **/
/** ֻ�޿��ڲ��ʵĵ��ʻ���Ŀ       **/
char *pubf_acct_acctoact(char *acc,char *opnbr)
{
	int  ret = 0;
    char cacno[8];
    char copbr[8];
    char *tmp;
    
    struct com_item_c   com_item;
    struct dc_acc_rel_c dc_acc_rel;
    struct in_parm_c    in_parm;
    struct in_mst_c     in_mst;
    struct dd_parm_c    dd_parm;
    struct dd_mst_c     dd_mst;
    struct mdm_ac_rel_c mdm_ac_rel;
    memset(cacno,0,sizeof(cacno));
    memset(copbr,0,sizeof(copbr));
    memset(tmp,0,sizeof(tmp));
    strncpy(cacno,acc,sizeof(cacno)-1);
    strncpy(copbr,opnbr,sizeof(copbr)-1);
    
    /** ����Ŀ���ҿ�Ŀ������ **/
	  ret = Com_item_Sel(g_pub_tx.reply,&com_item," acc_no='%s' ",cacno);
	  if(ret)
	  {
		    sprintf(acErrMsg,"[%s][%d]����!acc_no=[%s],sqlcode=[%d]",__FILE__,__LINE__,cacno,ret);
		    WRITEMSG
		    return(cacno);
	  }

	  /** ����Ŀ�����ֲ��һ�ƴ��� **/
	  ret = Dc_acc_rel_Sel(g_pub_tx.reply,&dc_acc_rel," acc_hrt='%s' ",com_item.acc_hrt);
	  if(ret)
	  {
		    sprintf(acErrMsg,"[%s][%d]����!acc_no=[%s],sqlcode=[%d]",__FILE__,__LINE__,com_item.acc_hrt,ret);
		    WRITEMSG 
		    return(cacno);
	  }
    
	if(com_item.in_ind[0] == 'I')	/** �ڲ��� **/
	{
			/** �����ڲ������� **/
			ret = In_parm_Sel(g_pub_tx.reply,&in_parm," dc_code='%s' ",dc_acc_rel.dc_code);
			if(ret)
			{
				sprintf(acErrMsg,"[%s][%d]����!sqlcode=[%d]",__FILE__,__LINE__,ret);
					WRITEMSG 
				return(cacno);
			}
			
			/** ����������Ʒ���ʻ�ID **/
			ret = In_mst_Sel(g_pub_tx.reply,&in_mst," prdt_no='%s' and opn_br_no='%s' and sts = '1'",in_parm.prdt_no,copbr);
			if(ret)
			{
				sprintf(acErrMsg,"[%s][%d]����!sqlcode=[%d]",__FILE__,__LINE__,ret);
					WRITEMSG 
				return(cacno);
			}
			
			/** �����ʻ�ID�ҵ��ʺ� **/
			ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel," ac_id='%ld' ",in_mst.ac_id);
			if(ret)
			{
				sprintf(acErrMsg,"[%s][%d]����!sqlcode=[%d]",__FILE__,__LINE__,ret);
					WRITEMSG 
				return(cacno);
			}
	}else{
			sprintf(acErrMsg,"[%s][%d]�ʻ����ʹ���!sqlcode=[%d]",__FILE__,__LINE__,ret);
			WRITEMSG 
			return(cacno);
	}
		memcpy(tmp,mdm_ac_rel.ac_no, sizeof(mdm_ac_rel.ac_no));
    return(tmp);
}

int pub_acct_LogOutChkForCDHP()
{
	int  ret = 0;
	int  cnt = 0;
	char cDate[9];
	long cNextDate = 0;

	struct mo_bail_rate_c mo_bail_rate;
	struct impa_dep_reg_c impa_dep_reg;
	struct mdm_ac_rel_c mdm_ac_rel;
	struct dd_mst_c dd_mst;
	struct td_mst_c td_mst;
	struct com_parm_c com_parm;
	struct mo_bank_acc_po_c mo_bank_acc_po;

	memset(cDate,0,sizeof(cDate));
	memset(&mo_bank_acc_po,0,sizeof(mo_bank_acc_po));

	/** ������һ���������� **/
	sprintf(cDate,"%ld",g_pub_tx.tx_date);
	ret = pub_base_dayvalicheck(atol(cDate));
	if(ret)
	{
		sprintf(acErrMsg,"[%s][%d]���ڴ���!date=[%s]",__FILE__,__LINE__,cDate);
		WRITEMSG 
		goto ErrExit;
	}
	cNextDate = pub_base_daynumLAT(cDate, 1);
	/*
	ret = pub_base_dayvalicheck(atol(cNextDate));
	*/
	if(ret)
	{
		sprintf(acErrMsg,"[%s][%d]���ڴ���!date=[%s]",__FILE__,__LINE__,cDate);
		WRITEMSG 
		goto ErrExit;
	}

	/** �жϵ����Ƿ��е��ڳжһ�Ʊ **/
	ret = Mo_bank_acc_po_Dec_Sel(g_pub_tx.reply,"matr_date<='%ld' and acc_po_ind='0' \
	      and payer_br_no='%s'",cNextDate,g_pub_tx.tx_br_no);
	if(ret)
	{
		sprintf(acErrMsg,"[%s][%d]���ҵ��ڳжһ�Ʊ����!sqlcode=[%d]",__FILE__,__LINE__,ret);
		WRITEMSG 
		goto ErrExit;
	}
	while(1)
	{
			/** �жϵ����Ƿ��е��ڳжһ�Ʊ **/
			ret = Mo_bank_acc_po_Fet_Sel(&mo_bank_acc_po,g_pub_tx.reply);
			if(ret == 100) break;
			if(ret)
			{
				sprintf(acErrMsg,"[%s][%d]���ҵ��ڳжһ�Ʊ����!sqlcode=[%d]",__FILE__,__LINE__,ret);
				WRITEMSG 
				goto ErrExit;
			}

			/** ��鸶�����ʻ��Ƿ���� **/
			ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",mo_bank_acc_po.payer_ac_no);
			if(ret)
			{
				sprintf(acErrMsg,"[%s][%d]�������ʺŴ���!sqlcode=[%d]",__FILE__,__LINE__,ret);
				WRITEMSG 
				goto ErrExit;
			}
			cnt = sql_count("dd_mst","ac_id='%d'",mdm_ac_rel.ac_id);
			if(cnt!=1)
			{
				sprintf(acErrMsg,"[%s][%d]�������ʺŴ���!cnt=[%d]",__FILE__,__LINE__,cnt);
				WRITEMSG 
				goto ErrExit;
			}


			/** ���136��243�ʻ��Ƿ���� **/
			cnt = sql_count("in_mst","opn_br_no='%s' and sts='1' and prdt_no='905' ",g_pub_tx.tx_br_no);
			if(cnt!=1)
			{
				sprintf(acErrMsg,"[%s][%d]�жҵ���ʻ�����!cnt=[%d]",__FILE__,__LINE__,cnt);
				WRITEMSG 
				goto ErrExit;
			}

			ret = Com_parm_Sel(g_pub_tx.reply,&com_parm,"parm_code='%s' and parm_seqn=10 ",g_pub_tx.tx_br_no);
			if(ret)
			{
				sprintf(acErrMsg,"[%s][%d]�жҸ�����ʱ�ʻ�����!cnt=[%d]",__FILE__,__LINE__,ret);
				WRITEMSG 
				goto ErrExit;
			}

			/** ���жҸ�����ʱ�ʻ��Ƿ���� **/
			ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",com_parm.val);
			if(ret)
			{
				sprintf(acErrMsg,"[%s][%d]�жҸ�����ʱ�ʺŴ���!sqlcode=[%d]",__FILE__,__LINE__,ret);
				WRITEMSG 
				goto ErrExit;
			}
			ret = Dd_mst_Sel(g_pub_tx.reply,&dd_mst,"ac_id='%d'",mdm_ac_rel.ac_id);
			if(ret)
			{
				sprintf(acErrMsg,"[%s][%d]�жҸ�����ʱ�ʺŴ���!sqlcode=[%d]",__FILE__,__LINE__,ret);
				WRITEMSG 
				goto ErrExit;
			}
			if(dd_mst.ac_sts[0]!='1')
			{
				sprintf(acErrMsg,"[%s][%d]�жҸ�����ʱ�ʺ�״̬����!ac_sts=[%s]",__FILE__,__LINE__,dd_mst.ac_sts);
				WRITEMSG 
				goto ErrExit;
			}
			if(dd_mst.hold_sts[0]!='0')
			{
				sprintf(acErrMsg,"[%s][%d]�жҸ�����ʱ�ʺ�״̬����!hold_sts=[%s]",__FILE__,__LINE__,dd_mst.hold_sts);
				WRITEMSG 
				goto ErrExit;
			}

			/*******************************************************/
			/**1.����Ǳ�֤���ʻ���鱣֤����Ϣ�����ʻ��Ƿ񱻶��� **/
			/**  ��������᲻����ǩ��,�����ϱ������Ļ���һ������  **/
			/**  ״̬�Ļ����ʻ����ڳжұ�֤����Ϣ����             **/
			/**2.�����֤���ʻ��������mo_bank_acc_po��bail_amt   **/
			/**  ������ϵ�ͻ��Ա�֤����в���(5801)���޸�bail_amt **/
			/**  ��߲�������֤���ʻ����                         **/
			/*******************************************************/
			if(mo_bank_acc_po.bail_amt > 0.005)
			{
					/** ��鱣֤���ʻ��Ƿ���� **/
					ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",mo_bank_acc_po.bail_ac_no);
					if(ret)
					{
						sprintf(acErrMsg,"[%s][%d]�������ʺŴ���!sqlcode=[%d]",__FILE__,__LINE__,ret);
						WRITEMSG 
						goto ErrExit;
					}
					ret = Dd_mst_Sel(g_pub_tx.reply,&dd_mst,"ac_id='%d'",mdm_ac_rel.ac_id);
					if(ret)
					{
						sprintf(acErrMsg,"[%s][%d]�������ʺŴ���!sqlcode=[%d]",__FILE__,__LINE__,ret);
						WRITEMSG 
						goto ErrExit;
					}

					/** ��鱣֤���ʻ�����Ƿ񹻿� **/
					if(dd_mst.bal<mo_bank_acc_po.bail_amt)
					{
						sprintf(acErrMsg,"[%s][%d]��֤������!bal=[%f],bail_amt=[%f]",__FILE__,__LINE__,\
						        dd_mst.bal,mo_bank_acc_po.bail_amt);
						WRITEMSG 
						goto ErrExit;
					}

					/** ��鱣֤����Ϣ�����ʻ�״̬�Ƿ����� **/
					ret = Mo_bail_rate_Sel(g_pub_tx.reply,&mo_bail_rate,"pact_no='%s'",mo_bank_acc_po.pact_no);
					if(ret)
					{
						sprintf(acErrMsg,"[%s][%d]��֤��û��������ά��!sqlcode=[%d]",__FILE__,__LINE__,ret);
						WRITEMSG 
						goto ErrExit;
					}
					
					ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",mo_bail_rate.ac_no);
					if(ret)
					{
						sprintf(acErrMsg,"[%s][%d]��֤����Ϣ�ʺŴ���!sqlcode=[%d]",__FILE__,__LINE__,ret);
						WRITEMSG 
						goto ErrExit;
					}
					ret = Dd_mst_Sel(g_pub_tx.reply,&dd_mst,"ac_id='%d'",mdm_ac_rel.ac_id);
					if(ret)
					{
						sprintf(acErrMsg,"[%s][%d]��֤����Ϣ�ʺŴ���!sqlcode=[%d]",__FILE__,__LINE__,ret);
						WRITEMSG 
						goto ErrExit;
					}

					if(dd_mst.ac_sts[0]!='1')
					{
						sprintf(acErrMsg,"[%s][%d]��֤����Ϣ�ʺ�״̬����!ac_sts=[%s]",__FILE__,__LINE__,dd_mst.ac_sts);
						WRITEMSG 
						goto ErrExit;
					}
					if(dd_mst.hold_sts[0]!='0' && dd_mst.hold_sts[0]!='2' && dd_mst.hold_sts[0]!='3')
					{
						sprintf(acErrMsg,"[%s][%d]��֤����Ϣ�ʺ�״̬����!hold_sts=[%s]",__FILE__,__LINE__,dd_mst.hold_sts);
						WRITEMSG 
						goto ErrExit;
					}
					
			}

			/***********************************************/
			/** �������Ѻ�浥�����浥���Ƿ����        **/
			/** ���������,�����ϱ������Ļ���             **/
			/** �޸�mo_bank_acc_po���tot_mort_amtΪ��    **/
			/** ���ϱ�һ����ȷ�Ķ����ʺŹ�����impa_dep_reg**/
			/***********************************************/
			if(mo_bank_acc_po.tot_mort_amt>0.005 && mo_bank_acc_po.acc_type[0] == '2')
			{
					/** ��鸶���ʻ��Ƿ���� **/
					ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",mo_bank_acc_po.payer_ac_no);
					if(ret)
					{
						sprintf(acErrMsg,"[%s][%d]�����ʺŴ���!sqlcode=[%d]",__FILE__,__LINE__,ret);
						WRITEMSG 
						goto ErrExit;
					}
					ret = Dd_mst_Sel(g_pub_tx.reply,&dd_mst,"ac_id='%d'",mdm_ac_rel.ac_id);
					if(ret)
					{
						sprintf(acErrMsg,"[%s][%d]�����ʺŴ���!sqlcode=[%d]",__FILE__,__LINE__,ret);
						WRITEMSG 
						goto ErrExit;
					}
					if(dd_mst.ac_sts[0]!='1')
					{
						sprintf(acErrMsg,"[%s][%d]�����ʺ�״̬����!ac_sts=[%s]",__FILE__,__LINE__,dd_mst.ac_sts);
						WRITEMSG 
						goto ErrExit;
					}
					if(dd_mst.hold_sts[0]!='0' && dd_mst.hold_sts[0]!='3')
					{
						sprintf(acErrMsg,"[%s][%d]�����ʺ�״̬����!hold_sts=[%s]",__FILE__,__LINE__,dd_mst.hold_sts);
						WRITEMSG 
						goto ErrExit;
					}
					ret = Impa_dep_reg_Dec_Sel(g_pub_tx.reply,"pact_no='%s'",mo_bank_acc_po.pact_no);
					if(ret)
					{
						sprintf(acErrMsg,"[%s][%d]���ҵ�Ѻ����������!sqlcode=[%d]",__FILE__,__LINE__,ret);
						WRITEMSG 
						goto ErrExit;
					}
					while(1)
					{
							/** �жϵ����Ƿ��е��ڳжһ�Ʊ **/
							ret = Impa_dep_reg_Fet_Sel(&impa_dep_reg,g_pub_tx.reply);
							if(ret == 100) break;
							if(ret)
							{
								sprintf(acErrMsg,"[%s][%d]���ҵ�Ѻ����������!sqlcode=[%d]",__FILE__,__LINE__,ret);
								WRITEMSG 
								goto ErrExit;
							}
							ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",impa_dep_reg.mort_ac_no);
							if(ret)
							{
								sprintf(acErrMsg,"[%s][%d]��Ѻ�����ʺŴ���!sqlcode=[%d]",__FILE__,__LINE__,ret);
								WRITEMSG 
								goto ErrExit;
							}
							ret = Td_mst_Sel(g_pub_tx.reply,&td_mst,"ac_id='%d'",mdm_ac_rel.ac_id);
							if(ret)
							{
								sprintf(acErrMsg,"[%s][%d]��Ѻ�����ʺŴ���!sqlcode=[%d]",__FILE__,__LINE__,ret);
								WRITEMSG 
								goto ErrExit;
							}
							if(td_mst.ac_sts[0]!='1')
							{
								sprintf(acErrMsg,"[%s][%d]��Ѻ�����ʺ�״̬����!ac_sts=[%s]",__FILE__,__LINE__,dd_mst.ac_sts);
								WRITEMSG 
								goto ErrExit;
							}

					}
					Impa_dep_reg_Clo_Sel();
			}
		
	}
	Mo_bank_acc_po_Clo_Sel();

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�жһ�Ʊ���ڼ��ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}

