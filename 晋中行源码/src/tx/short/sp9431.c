/*************************************************
* 文 件 名: sp9431.c
* 功能描述：根据账号回显账户序号及信息[公共转账取款快查]
* 作    者:
* 完成日期：
* 修改记录：
*     1. 日    期:
*        修 改 人:
*        修改内容:
* 注意： 网银要用到40域，请不要用40域做任何操作
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include "string.h"
#define  EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "com_item_c.h"
#include "cif_id_code_rel_c.h"
#include "mdm_attr_c.h"
#include "gl_mst_c.h"
#include "gl_sub_c.h"
#include "hv_brno_orno_c.h"
#include "card.h"

sp9431()
{
    int         i = 0, ret = 0, flag = 0, tag = 0;
    long        ac_seqn = 0;
    double      bal=0.00;
    long        cif_no = 0;
    char        ac_no[20], cur_no[5], cif_type[2], note_no[20], name[61], tmp_inf[3];
    char        ac_type[2], tmp_memo[100], tmpname[100], prdt_name[31];
    FILE        *fp = NULL;
    char        in_out = '0';     /**0.科目  内部账 1.对公活期账户**/
    double      s_amt=0.00;
    char        br_no[20];
    char spe_ind[7];
    char        opn_br_no[6];
	int         countno =0;

    struct prdt_ac_id_c       prdt_ac_id_c;
    struct cif_id_code_rel_c  g_cif_id_code_rel;
    struct mdm_attr_c         g_mdm_attr;
    struct dic_data_c         vdic_data;
    struct hv_brno_orno_c     hv_brno_orno;
    struct com_item_c         g_com_item;
    struct com_item_c         sCom_item;
    struct gl_mst_c           g_gl_mst;
    struct gl_sub_c           g_gl_sub;
    struct dd_mst_c		sDd_mst;

    memset(spe_ind,'0',sizeof(spe_ind) - 1);
    spe_ind[6] = 0;
    memset(&vdic_data,    0x00 , sizeof(struct dic_data_c));
    memset(&sDd_mst,      0x00,sizeof(sDd_mst));
    memset(&g_mdm_attr,   0x00,sizeof(g_mdm_attr));
    memset(&hv_brno_orno, 0x00 , sizeof(struct hv_brno_orno_c));
    memset(&g_gl_sub,     0x00,sizeof(struct gl_sub_c));
    memset(&g_com_item,   0x00, sizeof(struct com_item_c));
    memset(&sCom_item,   0x00, sizeof(struct com_item_c));

    memset(br_no,         0x00, sizeof(br_no));
    memset(ac_no,         0x00, sizeof(ac_no));
    memset(cur_no,        0x00, sizeof(cur_no));
    memset(cif_type,      0x00, sizeof(cif_type));
    memset(note_no,       0x00, sizeof(note_no));
    memset(name,          0x00, sizeof(name));
    memset(tmp_inf,       0x00, sizeof(tmp_inf));
    memset(ac_type,       0x00, sizeof(ac_type));
    memset(tmp_memo,      0x00, sizeof(tmp_memo));
    memset(prdt_name,     0x00, sizeof(prdt_name));
    memset(tmpname,       0x00, sizeof(tmpname));
    memset(opn_br_no,     0x00,sizeof(opn_br_no));

    pub_base_sysinit();

    get_zd_data("0030",opn_br_no);
    get_zd_data("1021", ac_no);
    get_zd_data("0020", br_no);

    pub_base_old_acno(ac_no);     /**对旧账号的处理**/
    set_zd_data("0320",ac_no);    /*旧账号处理后传到前台*/
    vtcp_log(" %s, %d, ac_no[%s] ",__FILE__,__LINE__,ac_no);
    get_zd_data("102L", tmp_inf);     /* 摘要 */
    i = strlen(ac_no);
    if (i == 3 || i == 5 || i == 7)
    {	/* 科目与内部账号 */
		in_out = '0';
		set_zd_data("0690", &in_out);
		ret = Hv_brno_orno_Sel(g_pub_tx.reply,&hv_brno_orno,"br_no='%s'",opn_br_no);
		if ( ret )
		{
		     sprintf(acErrMsg,"查开户行行号记录不存在!![%s]",ac_no);
		     WRITEMSG
		     strcpy(g_pub_tx.reply,"M003");
		     goto ErrExit;
		}
		vtcp_log(" %s, %d, 行号[%s] ",__FILE__,__LINE__,hv_brno_orno.or_br_no);
		/* 根据科目号查询科目名称 */
		ret = Com_item_Sel(g_pub_tx.reply, &g_com_item, "acc_no='%s'", ac_no);
		if (ret == 100)
		{
		     sprintf(acErrMsg, "该科目号不存在!!acc_hrt=[%s]", g_in_parm.acc_hrt);
		     WRITEMSG
		     strcpy(g_pub_tx.reply, "O074");
		     goto ErrExit;
		}
		else if (ret)
		{
		     sprintf(acErrMsg, "查询公共科目表异常!!ret=[%d]", ret);
		     WRITEMSG
		     strcpy(g_pub_tx.reply, "X007");
		     goto ErrExit;
		}
		if (g_com_item.acc_knd[0] != '3')
		{
		     sprintf(acErrMsg, "该科目下有账户或有子科目!!acc_knd=[%s]", g_com_item.acc_knd);
		     WRITEMSG
		     strcpy(g_pub_tx.reply, "P406");
		     goto ErrExit;
		}
		/* 根据科目号查询余额及借贷方向 */
		ret = Gl_sub_Sel(g_pub_tx.reply, &g_gl_sub, "acc_hrt='%s' and br_no like '%%%s%%'",g_com_item.acc_hrt, br_no);
		if (ret == 100)
		{
		     sprintf(acErrMsg, "该科目号不存在!!acc_hrt=[%s]", g_com_item.acc_hrt);
		     WRITEMSG
		     strcpy(g_pub_tx.reply, "O075");
		     goto ErrExit;
		}
		else if (ret)
		{
		     sprintf(acErrMsg, "查询公共科目表异常!!ret=[%d]", ret);
		     WRITEMSG
		     strcpy(g_pub_tx.reply, "X007");
		     goto ErrExit;
		}
		s_amt = 0.00;
		ret = sql_sum_double("dc_log", "amt", &s_amt, "acc_hrt='%s' and tx_opn_br_no like '%%%s%%' and dc_ind = '1'", g_com_item.acc_hrt, br_no);
		if (0 != ret && 100 != ret)
		{
		     sprintf(acErrMsg, "查询dc_log表异常!!ret=[%d]", ret);
		     WRITEMSG
		     goto ErrExit;
		}
		vtcp_log(" %d, %s, s_amt[%.2lf] ", __LINE__, __FILE__, s_amt);
		g_gl_sub.dr_bal += s_amt;
		s_amt = 0;
		ret = sql_sum_double("dc_log", "amt", &s_amt, "acc_hrt='%s' and tx_opn_br_no like '%%%s%%' and dc_ind != '1'", g_com_item.acc_hrt, br_no);
		if (0 != ret && 100 != ret)
		{
		     sprintf(acErrMsg, "查询dc_log表异常!!ret=[%d]", ret);
		     WRITEMSG
		     goto ErrExit;
		}
		g_gl_sub.cr_bal += s_amt;
		vtcp_log(" %s, %d, cr_bal[%lf] s_amt[%.2lf] ",__FILE__,__LINE__,g_gl_sub.cr_bal,s_amt);
		set_zd_double("1203", g_gl_sub.dr_bal);     /* 余额 */
		/* 科目借贷类型 */
		if (g_com_item.dc_ind[0] == '1')
		{
		     set_zd_data("0220", "借");
		     g_gl_sub.dr_bal -= g_gl_sub.cr_bal;
		     if (g_gl_sub.dr_bal < 0.005)
		     {
		          g_gl_sub.dr_bal = 0;
		     }
		     set_zd_double("0410", g_gl_sub.dr_bal);
		     set_zd_double("0390", g_gl_sub.dr_bal);     /* 当前借方余额 */
		     set_zd_double("1203", g_gl_sub.dr_bal);     /* 余额 */
		     set_zd_double("102I", g_gl_sub.dr_bal);     /* 余额 */
		}
		else if (g_com_item.dc_ind[0] == '2')
		{
		     set_zd_data("0220", "贷");
		     g_gl_sub.cr_bal -= g_gl_sub.dr_bal;
		     if (g_gl_sub.dr_bal < 0.005)
		     {
		          g_gl_sub.cr_bal = 0;
		     }
		     set_zd_double("0410", g_gl_sub.cr_bal);
		     set_zd_double("0390", g_gl_sub.cr_bal);     /* 当前贷方余额 */
		     set_zd_double("1203", g_gl_sub.cr_bal);     /* 余额 */
		     set_zd_double("102I", g_gl_sub.cr_bal);     /* 余额 */
		}
		else if (g_com_item.dc_ind[0] == '0')
		{	/* 双方 */
			ret = pub_base_CompDblVal(g_gl_sub.dr_bal, g_gl_sub.cr_bal);
			if (ret == 1)
			{
			     set_zd_data("0220", "借");
			     g_gl_sub.dr_bal -= g_gl_sub.cr_bal;
			     set_zd_double("0410", g_gl_sub.dr_bal);
			     set_zd_double("0390", g_gl_sub.dr_bal);     /* 当前借方余额 */
			     set_zd_double("1203", g_gl_sub.dr_bal);     /* 余额 */
			     set_zd_double("102I", g_gl_sub.dr_bal);     /* 余额 */
			}
			else if (ret == -1)
			{
			     set_zd_data("0220", "贷");
			     g_gl_sub.cr_bal -= g_gl_sub.dr_bal;
			     set_zd_double("0410", g_gl_sub.cr_bal);
			     set_zd_double("0390", g_gl_sub.cr_bal);     /* 当前贷方余额 */
			     set_zd_double("1203", g_gl_sub.cr_bal);     /* 余额 */
			     set_zd_double("102I", g_gl_sub.cr_bal);     /* 余额 */
			}
		}
		set_zd_data("0250", g_com_item.acc_name);             /* 户名 */
		set_zd_data("0210", g_gl_sub.cur_no);                 /* 币种 */

		/**以下是FD120的赋值**/
		set_zd_data("1201", ac_no);
		set_zd_data("1202", g_com_item.acc_name);             /* 户名 */
		/* 1203余额上面已添加 */
		set_zd_data("1204", g_gl_sub.cur_no);                 /* 币种 */
		set_zd_data("1205", "2");                             /* 现转标志:2 转 1 现 */

		/* 为了和检查账户时保持一致，这里增加对102域部分信息赋值 */
		set_zd_data("102H", g_com_item.acc_name);
		set_zd_data("102J", g_gl_sub.cur_no);                 /* 币种 */
		set_zd_data("102K", "2");                             /* 现转标志:2 转 1 现 */
		strcpy(cif_type, "2");        /* 当对公户类型处理 */
		strcpy(ac_type, cif_type);
		set_zd_data("102M", ac_type);
		set_zd_data("1029", "N");
		set_zd_data("1026", "N");
		strcpy(prdt_name, "科目账:");
		strcat(prdt_name, g_com_item.acc_name);
		set_zd_data("102V", prdt_name);

		set_zd_data("0520",hv_brno_orno.or_br_no);             /* 传送行号 */
		set_zd_data("0020",g_pub_tx.tx_br_no);
    }
    else
    {
		ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'", ac_no);
		if (ret)
		{
		     sprintf(acErrMsg, "介质账号对照表不存在该记录!![%s]", ac_no);
		     WRITEMSG
		     strcpy(g_pub_tx.reply, "M003");
		     goto ErrExit;
		}
		set_zd_data("0020",g_mdm_ac_rel.opn_br_no);
		set_zd_data("0880",g_mdm_ac_rel.draw_pwd_chg);
		/*
		if(g_mdm_ac_rel.ac_seqn==9999)
		{
		ret = sql_count("td_mst", "ac_id=%ld and ac_sts!='*'", g_mdm_ac_rel.ac_id);
		if (ret < 0)
		{
		         sprintf(acErrMsg, "sql_count error!! [%d]", ret);
		         WRITEMSG
		         goto ErrExit;
		}
		countno=ret;
		ret = sql_count("dd_mst", "ac_id=%ld and ac_sts!='*'", g_mdm_ac_rel.ac_id);
		if (ret < 0)
		{
		         sprintf(acErrMsg, "sql_count error!! [%d]", ret);
		         WRITEMSG
		         goto ErrExit;
		}
		countno=countno+ret;
		if (countno == 0)
		{
			set_zd_data();
		}
		}
		*/
		memcpy(g_cif_id_code_rel.id_type,g_mdm_ac_rel.id_type,sizeof(g_cif_id_code_rel.id_type)-1);
		memcpy(g_cif_id_code_rel.id_no,g_mdm_ac_rel.id_no,sizeof(g_cif_id_code_rel.id_no)-1);
		ret = Hv_brno_orno_Sel(g_pub_tx.reply,&hv_brno_orno,"br_no='%s'",g_mdm_ac_rel.opn_br_no);
		if ( ret )
		{
		     sprintf(acErrMsg,"查开户行行号记录不存在!![%s]",ac_no);
		     WRITEMSG
		     strcpy(g_pub_tx.reply,"M003");
		     goto ErrExit;
		}
		vtcp_log(" %s, %d, ac_no[%s] or_br_no[%s] ",__FILE__,__LINE__,ac_no,hv_brno_orno.or_br_no);
		if(!memcmp(ac_no,CARDHEADCODE,6) || !memcmp(ac_no,"622135",6) || !memcmp(ac_no,"940027",6))
		{
			ret = Prdt_ac_id_Sel(g_pub_tx.reply, &prdt_ac_id_c, "ac_id=%ld and ac_id_type='1'",g_mdm_ac_rel.ac_id);
			if (ret)
			{
			     sprintf(acErrMsg, "产品账号对照表不存在该记录!! [%d]", ret);
			     WRITEMSG
			     strcpy(g_pub_tx.reply, "M003");
			     goto ErrExit;
			}
			in_out = '2';
			set_zd_data("0690", &in_out);
			set_zd_data("1270","1");
		    vtcp_log(" %s, %d, in_out[%c] ",__FILE__,__LINE__,in_out);
		}
		else
		{
			ret = Prdt_ac_id_Sel(g_pub_tx.reply, &prdt_ac_id_c, "ac_id=%ld order by ac_seqn", g_mdm_ac_rel.ac_id);
			if (ret)
			{
			     sprintf(acErrMsg, "产品账号对照表不存在该记录!! [%d]", ret);
			     WRITEMSG
			     strcpy(g_pub_tx.reply, "M003");
			     goto ErrExit;
			}
			if (prdt_ac_id_c.ac_id_type[0] == '1')
			{
			     in_out = '2';
			     set_zd_data("0690", &in_out);
			}
			else if (prdt_ac_id_c.ac_id_type[0] == '9')
			{
			     in_out = '1';
			     set_zd_data("0690", &in_out);
			}
			vtcp_log(" %s, %d, in_out[%c] ",__FILE__,__LINE__,in_out);
		}

		/* 增加转账对内部账户的支持 **/
		if ((prdt_ac_id_c.prdt_no[0] != '1' && prdt_ac_id_c.prdt_no[0] != '2') && prdt_ac_id_c.prdt_no[0] != '9')
		{
		     sprintf(acErrMsg, "非存取款类账号! [%d]", ret);
		     WRITEMSG
		     strcpy(g_pub_tx.reply, "O166");
		     goto ErrExit;
		}
		set_zd_data("0930",prdt_ac_id_c.ac_id_type);
		ret = Mdm_attr_Sel(g_pub_tx.reply, &g_mdm_attr, "mdm_code='%s'", g_mdm_ac_rel.mdm_code);
		if (ret == 100)
		{
		     sprintf(acErrMsg, "介质不存在! ");
		     WRITEMSG
		     strcpy(g_pub_tx.reply, "M003");
		     goto ErrExit;
		}
		else if (ret)
		{
		     sprintf(acErrMsg, "查找出错! ");
		     WRITEMSG
		     strcpy(g_pub_tx.reply, "D103");
		     goto ErrExit;
		}
        /* 多账户介质的情况 */
		if(memcmp(ac_no,CARDHEADCODE,6) && memcmp(ac_no,"622135",6) && memcmp(ac_no,"940027",6))
        {
		 	/*
        	if (g_mdm_attr.ac_num_ind[0] == 'N' || !memcmp(g_mdm_attr.mdm_code,"0024",4))
		 	*/
            if (g_mdm_attr.ac_num_ind[0] == 'N')
            {     /* 卡折双开户特殊处理 0024 */
                  ret = sql_count("prdt_ac_id", "ac_id=%ld", g_mdm_ac_rel.ac_id);
                  if (ret < 0)
                  {
                       sprintf(acErrMsg, "sql_count error!! [%d]", ret);
                       WRITEMSG
                       goto ErrExit;
                  }
                  else if (ret == 1)
                  {
                       ac_seqn = prdt_ac_id_c.ac_seqn;
                  }
                  else if (ret > 1)
                  {
                       set_zd_long("102N", prdt_ac_id_c.ac_seqn);
                       ac_seqn = prdt_ac_id_c.ac_seqn;
                       set_zd_long("1022", ac_seqn);
                       /**tag = 1; **/
                       /* 若是多账户介质取活期的序号 */
                  }
             }
             else
             {
                  ac_seqn = g_mdm_ac_rel.ac_seqn;
             }
        }
        else
        {
              ac_seqn=prdt_ac_id_c.ac_seqn;
        }
        set_zd_long("1022", ac_seqn);
		/* 对与所有的磁条卡介质只要是换证了，状态不是正常都不能办理业务
        if(!memcmp(g_mdm_ac_rel.mdm_code, "0020", sizeof(g_mdm_ac_rel.mdm_code)-1) || !memcmp(g_mdm_ac_rel.mdm_code, "0024", sizeof(g_mdm_ac_rel.mdm_code)-1))
		*/
		if(g_mdm_attr.mdm_type[0]=='3')
        {
              if (g_mdm_ac_rel.note_sts[0] != '0')
              {
                   sprintf(acErrMsg, "卡户状态不正常! [%d]", ret);
                   WRITEMSG
                   strcpy(g_pub_tx.reply, "M022");
                   set_zd_data("0200" , g_mdm_ac_rel.note_sts);
                   goto ErrExit;
              }
        }
        if (tag == 0)
        {
             set_zd_data("0230",prdt_ac_id_c.prdt_no);
             /*
             if( prdt_ac_id_c.prdt_no[0]=='1' )
             {
                  ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst, "ac_id=%ld and ac_seqn=%ld",prdt_ac_id_c.ac_id,ac_seqn);
                  if( ret )
                  {
                       sprintf(acErrMsg,"read dd_mst error" );
                       WRITEMSG
                       goto ErrExit;
                  }
                  strcpy(name,g_dd_mst.name);
                  cif_no = g_dd_mst.cif_no;
                  bal = g_dd_mst.bal;
			   vtcp_log("%s %d g_dd_mst.ac_sts[%s] g_mdm_ac_rel.note_sts[%s]",__FILE__,__LINE__,g_dd_mst.ac_sts[0],g_mdm_ac_rel.note_sts[0]);
                  if(g_dd_mst.ac_sts[0]=='*' && g_mdm_ac_rel.note_sts[0]=='*')
                  {
                       sprintf(acErrMsg,"该账号已销户!! [%d]",ret);
                       WRITEMSG
                       strcpy(g_pub_tx.reply,"P245");
                       goto ErrExit;
                  }
                  ret = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'",g_dd_mst.prdt_no);
                  if( ret )
                  {
                       sprintf(acErrMsg,"read dd_mst error" );
                       WRITEMSG
                       goto ErrExit;
                  }
                  strcpy(prdt_name,g_dd_parm.title);
                  strcpy(cur_no,g_dd_parm.cur_no);
                  strcpy(cif_type,g_dd_parm.cif_type);
             }
             else
             {
                  sprintf(acErrMsg,"请输入活期账号，其他账户不能作转账方![%s]",prdt_ac_id_c.prdt_no);
                  WRITEMSG
                  strcpy(g_pub_tx.reply, "O227");
                  goto ErrExit;
             }
             strcpy(ac_type,cif_type);
             set_zd_data("102M",ac_type);
             ret = Cif_id_code_rel_Sel( g_pub_tx.reply,&g_cif_id_code_rel,"cif_no=%ld",cif_no);
             if( ret )
             {
                  sprintf(acErrMsg,"查询cif_id_code_rel错误![%d][%ld]",ret,cif_no);
                  WRITEMSG
                  strcpy(g_pub_tx.reply, "G102");
                  goto ErrExit;
             }
             */

             /* 增加对内部账户的支持 **/
             switch (prdt_ac_id_c.prdt_no[0])
             {
                   case '1':
                        ret = Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst, "ac_id=%ld and ac_seqn=%ld",prdt_ac_id_c.ac_id, ac_seqn);
                        if (ret)
                        {
                             sprintf(acErrMsg, "read dd_mst error");
                             WRITEMSG
                             goto ErrExit;
                        }
                        strcpy(name, g_dd_mst.name);
                        cif_no = g_dd_mst.cif_no;
                        /**bal = g_dd_mst.bal; **/
                        ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=(select app_ac_id from mo_prot_ac_rel where main_ac_id=%ld and sts='1') ",g_dd_mst.ac_id);
                        if(ret==100)/**没有协定理财**/
                        {
                        	bal = g_dd_mst.bal;
                        }
                        else if(ret)/**查询错误**/
                        {
                             sprintf(acErrMsg, "read dd_mst error");
                             WRITEMSG
                             goto ErrExit;
                        }
                        else
                        {
                        	bal=g_dd_mst.bal+sDd_mst.bal;
                        }
                        if (g_dd_mst.ac_sts[0] == '*')
                        {
                             sprintf(acErrMsg, "该账号已销户!! [%d]", ret);
                             WRITEMSG
                             strcpy(g_pub_tx.reply, "P245");
                             goto ErrExit;
                        }
                        ret = Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm,"prdt_no='%s'", g_dd_mst.prdt_no);
                        if (ret)
                        {
                             sprintf(acErrMsg, "read dd_mst error");
                             WRITEMSG
                             goto ErrExit;
                        }
                        strcpy(prdt_name, g_dd_parm.title);
                        strcpy(cur_no, g_dd_parm.cur_no);
                        strcpy(cif_type, g_dd_parm.cif_type);

                        strcpy(ac_type, cif_type);
                        set_zd_data("102M", ac_type);
                        set_zd_data("0990",g_dd_mst.ac_sts);
                        set_zd_data("0700",g_dd_mst.sttl_type);         /*结算类型*/
                        set_zd_data("0710",g_dd_mst.hold_sts);
                        set_zd_data("0910", g_dd_mst.opn_br_no);
                        set_zd_double("0430",bal);/* 如果协定理财，应加理财户的余额 */
			/*
                        set_zd_double("0420",g_dd_mst.bal-g_dd_mst.hold_amt-g_dd_mst.ctl_amt);**chenchao 大额返回的是可用余额**
			*/
                        set_zd_double("0420",bal-g_dd_mst.hold_amt-g_dd_mst.ctl_amt);/**chenchao 大额返回的是可用余额**/
                        set_zd_data("0670",g_dd_mst.hold_sts);        /*返回冻结状态*/
			set_zd_data("0730",g_dd_mst.od_ind);          /*透支标志*/
                        /*
                        ret = Cif_id_code_rel_Sel( g_pub_tx.reply,&g_cif_id_code_rel, "cif_no=%ld",cif_no);
                        if( ret )
                        {
                             sprintf(acErrMsg,"查询cif_id_code_rel错误![%d][%ld]",ret,cif_no);
                             WRITEMSG
                             strcpy(g_pub_tx.reply, "G102");
                             goto ErrExit;
                        }
                        */
			 /* 返回账户属性 */
                        ret = Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_hrt = (select acc_hrt from dc_acc_rel where dc_code='%s' and data_code='0152') ",g_dd_parm.dc_code);
                        if( ret )
                        {
                             sprintf(acErrMsg,"查询com_item错误!ret[%d]",ret);
                             WRITEMSG
                             strcpy(g_pub_tx.reply, "G102");
                             goto ErrExit;
                        }
                        set_zd_data("0965",sCom_item.dc_ind);
                        break;
                   case '2':
                        ret = Td_mst_Sel(g_pub_tx.reply, &g_td_mst, "ac_id=%ld and ac_seqn=%ld",prdt_ac_id_c.ac_id, ac_seqn);
                        if (ret)
                        {
                             sprintf(acErrMsg, "read dd_mst error");
                             WRITEMSG
                             goto ErrExit;
                        }
                        strcpy(name, g_td_mst.name);
                        cif_no = g_td_mst.cif_no;
                        bal = g_td_mst.bal;
                        if (g_td_mst.ac_sts[0] == '*' && g_mdm_ac_rel.note_sts[0] == '*')
                        {
                             sprintf(acErrMsg, "该账号已销户!! [%d]", ret);
                             WRITEMSG
                             strcpy(g_pub_tx.reply, "P245");
                             goto ErrExit;
                        }
                        /* 若是完全冻结返回标志 */
                        if(g_td_mst.hold_sts[0] =='1' || g_td_mst.hold_sts[0]=='2')
                        {
                             set_zd_data("0720","1");
                        }
                        ret = Td_parm_Sel(g_pub_tx.reply, &g_td_parm,"prdt_no='%s'", g_td_mst.prdt_no);
                        if (ret)
                        {
                             sprintf(acErrMsg, "read dd_mst error");
                             WRITEMSG
                             goto ErrExit;
                        }
                        strcpy(prdt_name, g_td_parm.title);
                        strcpy(cur_no, g_td_parm.cur_no);
                        strcpy(cif_type, g_td_parm.cif_type);
                        strcpy(ac_type, cif_type);
                        set_zd_data("0910", g_td_mst.opn_br_no);
                        set_zd_data("102M", ac_type);
                        set_zd_double("0430",g_td_mst.bal);
                        set_zd_double("0420",g_td_mst.bal-g_td_mst.hold_amt-g_td_mst.ctl_amt);
                        /*
                        ret = Cif_id_code_rel_Sel( g_pub_tx.reply,&g_cif_id_code_rel,"cif_no=%ld",cif_no);
                        if( ret )
                        {
                             sprintf(acErrMsg,"查询cif_id_code_rel错误![%d][%ld]",ret,cif_no);
                             WRITEMSG
                             strcpy(g_pub_tx.reply, "G102");
                             goto ErrExit;
                        }
                        */
					 /* 返回账户属性 ****/
                        ret = Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_hrt = (select acc_hrt from dc_acc_rel where dc_code='%s' and data_code='0152') ",g_td_parm.dc_code);
                        if( ret )
                        {
                             sprintf(acErrMsg,"查询com_item错误!ret[%d]",ret);
                             WRITEMSG
                             strcpy(g_pub_tx.reply, "G102");
                             goto ErrExit;
                        }
                        set_zd_data("0965",sCom_item.dc_ind);
                        break;
                   case '9':
                        ret = In_mst_Sel(g_pub_tx.reply, &g_in_mst,  "ac_id=%ld and ac_seqn=%d", prdt_ac_id_c.ac_id, ac_seqn);
                        if (ret)
                        {
                             sprintf(acErrMsg, "read dd_mst error");
                             WRITEMSG
                             goto ErrExit;
                        }
                        strcpy(name, g_in_mst.name);
                        bal = g_in_mst.bal;
                        if (g_in_mst.sts[0] == '*')
                        {
                             sprintf(acErrMsg, "该账号已销户!! [%d]", ret);
                             WRITEMSG
                             strcpy(g_pub_tx.reply, "P245");
                             goto ErrExit;
                        }
                        ret = In_parm_Sel(g_pub_tx.reply, &g_in_parm,"prdt_no='%s'", g_in_mst.prdt_no);
                        if (ret)
                        {
                             sprintf(acErrMsg, "read dd_mst error");
                             WRITEMSG
                             goto ErrExit;
                        }
                        strcpy(prdt_name, g_in_parm.prdt_mo);
                        strcpy(cur_no, g_in_parm.cur_no);
                        strcpy(cif_type, "2");       /** 当对公户类型处理 **/
                        set_zd_data("0990",g_in_mst.sts);
                        strcpy(ac_type, cif_type);
                        set_zd_data("102M", ac_type);
                        set_zd_data("0710","0");
                        set_zd_data("0910", g_in_mst.opn_br_no);
                        g_mdm_ac_rel.draw_id_yn[0] = 'N';            /** 因为内部账户没有值，所以赋一个 **/
                        set_zd_double("0430",g_in_mst.bal);
                        set_zd_double("0420",g_in_mst.bal-g_in_mst.hold_amt);
                        set_zd_data("0730",g_in_mst.od_ind);          /*透支标志*/
					 /* 返回账户属性 */
                        ret = Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_hrt = '%s' ",g_in_parm.acc_hrt);
                        if( ret )
                        {
                             sprintf(acErrMsg,"查询com_item错误!ret[%d]",ret);
                             WRITEMSG
                             strcpy(g_pub_tx.reply, "G102");
                             goto ErrExit;
                        }
                        set_zd_data("0965",sCom_item.dc_ind);
                        break;
                   default:
                        sprintf(acErrMsg, "请输入活期账号，其他账户不能作转账方![%s]",prdt_ac_id_c.prdt_no);
                        WRITEMSG
                        strcpy(g_pub_tx.reply, "O227");
                        goto ErrExit;
             }
             pub_base_strpack(name);
             vtcp_log(" %s， %d， name[%s] ", __FILE__, __LINE__, name);
             set_zd_data("0250", name);
             set_zd_double("0390", bal);
             /* set_zd_data("102C", "N"); ** 是否支付密码器 */
             set_zd_data("102H", name);                                  /* 户名 */
             set_zd_data("102V", prdt_name);                             /* 产品名称 */
             set_zd_data("102J", cur_no);                                /* 币种 */
		/* 将现转标志默认为转账 */
             set_zd_data("102K", "2");                                   /* 默认现转 */
             set_zd_data("0680", "1");
             set_zd_data("0920",g_mdm_ac_rel.mdm_code);
             set_zd_data("0980",g_mdm_ac_rel.note_sts);
             set_zd_double("102I", bal);                                 /* 余额 */
             set_zd_data("1024", g_mdm_ac_rel.note_no);                  /* 凭证号码 */
             set_zd_data("1025", g_mdm_ac_rel.draw_uncon_yn);            /* 是否任意支取 */
             set_zd_data("1026", g_mdm_ac_rel.draw_pwd_yn);              /* 密码支取 */
             set_zd_data("1027", g_mdm_ac_rel.qry_pwd);                  /* 查询密码 */
             set_zd_data("1028", g_mdm_ac_rel.draw_pwd);                 /* 支取密码 */
             /* 特殊标志 */
             set_zd_data("102B", g_mdm_ac_rel.draw_seal_yn);             /* 是否凭印鉴支取 */
             if (flag == 1 && g_mdm_ac_rel.draw_id_yn[0] == 'N')
             {
                  set_zd_data("1029", "N");                              /* 证件支取 */
                  set_zd_data("102G", g_cif_id_code_rel.id_type);        /* 证件类型 */
                  set_zd_data("102A", g_cif_id_code_rel.id_no);          /* 证件号码 */
             } else if (flag == 1) {
                  set_zd_data("1029", "Y");                              /* 证件支取 */
                  set_zd_data("102G", g_mdm_ac_rel.id_type);             /* 证件类型 */
                  set_zd_data("102A", g_mdm_ac_rel.id_no);               /* 证件号码 */
             } else {
                  set_zd_data("1029", g_mdm_ac_rel.draw_id_yn);          /* 证件支取 */
                  set_zd_data("102G", g_mdm_ac_rel.id_type);             /* 证件类型 */
                  set_zd_data("102A", g_mdm_ac_rel.id_no);               /* 证件号码 */
             }
             set_zd_data("0250", name);
             set_zd_data("0620", g_mdm_ac_rel.id_no);
             set_zd_data("0520",hv_brno_orno.or_br_no);                  /* 传送行号 **/
        }
    }
    /* 开始下载菜单 */
    pub_base_AllDwnFilName(tmpname);
    fp = fopen(tmpname, "a");
vtcp_log(" %s, %d, ac_type[%s] ",__FILE__,__LINE__,ac_type);
    if (ac_type[0] == '2')
    {
         if (strcmp(tmp_inf, "01"))
         {     /* 有初值 */
              ret = Dic_data_Sel(g_pub_tx.reply, &vdic_data, "pr_code='tran_memo' and pr_seq='%s' ", tmp_inf);
              if (ret == 100) {
                   strcpy(tmp_inf, "01");
              } else if (ret) {
                   goto ErrExit;
              } else {
                   pub_base_strpack(vdic_data.pr_seq);
                   pub_base_strpack(vdic_data.content);
                   set_zd_data("102L", vdic_data.pr_seq);
                   fprintf(fp, "102L|%-20s|%s|\n", vdic_data.content, vdic_data.pr_seq);
              }
         }
         if (strcmp(tmp_inf, "01") == 0)
         {	/* 若无初值 */
              ret = Dic_data_Dec_Sel(g_pub_tx.reply, "pr_code='tran_memo'");
              if (ret)
              {
                   goto ErrExit;
              }
              while (1)
              {
                   ret = Dic_data_Fet_Sel(&vdic_data, g_pub_tx.reply);
                   if (ret == 100)
                        break;
                   if (ret)
                        goto ErrExit;

                   pub_base_strpack(vdic_data.pr_seq);
                   pub_base_strpack(vdic_data.content);
                   if (i == 0) {
                        set_zd_data("102L", vdic_data.pr_seq);
                        strcpy(tmp_memo, vdic_data.pr_seq);
                   }
                   fprintf(fp, "102L|%-20s|%s|\n", vdic_data.content, vdic_data.pr_seq);
                   i++;
              }
              Dic_data_Clo_Sel();
         }
    }
    if (strcmp(g_mdm_ac_rel.mdm_code, NULL_MDM_CODE) && ac_type[0] == '1')
    {
         memset(&g_mdm_attr, 0x00, sizeof(struct mdm_attr_c));
         ret = Mdm_attr_Sel(g_pub_tx.reply, &g_mdm_attr, "mdm_code='%s'", g_mdm_ac_rel.mdm_code);
         if (ret)
         {
              sprintf(acErrMsg, "查询介质属性表错误![%d]", ret);
              WRITEMSG
              strcpy(g_pub_tx.reply, "B004");
              goto ErrExit;
         }
         strcpy(note_no, g_mdm_attr.note_type);

	  /*根据凭证种类找凭证名称
         ret = Dic_data_Dec_Sel(g_pub_tx.reply, "pr_code='per_dety' and pr_seq='%s'", note_no);
	  */
         ret = Dic_data_Dec_Sel(g_pub_tx.reply, "pr_code='note_type' and pr_seq='%s'", note_no);
         if (ret)
              goto ErrExit;
         i = 0;
         while (1)
         {
              ret = Dic_data_Fet_Sel(&vdic_data, g_pub_tx.reply);
              if (ret == 100)
                   break;
              if (ret)
                   goto ErrExit;

              pub_base_strpack(vdic_data.pr_seq);
              pub_base_strpack(vdic_data.content);

              if (i == 0)
              {
                   set_zd_data("1023", vdic_data.pr_seq);
                   strcpy(note_no, vdic_data.pr_seq);
              }
              fprintf(fp, "1023|%-20s|%s|\n", vdic_data.content, vdic_data.pr_seq);
              i++;
         }
         Dic_data_Clo_Sel();
    }
    else
    {
         if (ac_type[0] == '2')
         {     /* 下载对公取款凭证类型 */
              /***选项列表***/
              i = 0;
              ret = Dic_data_Dec_Sel(g_pub_tx.reply,"pr_code='pub_taty' and pr_seq[1,3]<>'001' order by pr_seq");
              if (ret)
                   goto ErrExit;
              while (1)
              {
                   ret = Dic_data_Fet_Sel(&vdic_data, g_pub_tx.reply);
                   if (ret == 100)
                        break;
                   if (ret)
                        goto ErrExit;

                   pub_base_strpack(vdic_data.pr_seq);
                   pub_base_strpack(vdic_data.content);

                   if (i == 0) {
                        strcpy(note_no, vdic_data.pr_seq);
                        set_zd_data("1023", vdic_data.pr_seq);
                   }
                   fprintf(fp, "1023|%-20s|%s|\n", vdic_data.content, vdic_data.pr_seq);
                   i++;
              }
              Dic_data_Clo_Sel();
         }
    }
    fclose(fp);
    set_zd_data(DC_FILE_SND, "1");
    set_zd_data("1021", ac_no);
    set_zd_data("1023",g_mdm_attr.note_type);

    double tmp_bal = 0;
    get_zd_double("0410", &tmp_bal);
    if (tmp_bal < 0)
    {
         vtcp_log(" %s, %d, bal[%lf]<0 ",__FILE__,__LINE__,tmp_bal);
         tmp_bal = 0;
         set_zd_double("0410", tmp_bal);
    }
    tmp_bal = 0;
    get_zd_double("0390", &tmp_bal);
    if (tmp_bal < 0)
    {
         vtcp_log(" %s, %d, bal[%lf]<0 ",__FILE__,__LINE__,tmp_bal);
         tmp_bal = 0;
         set_zd_double("0390", tmp_bal);
    }
    tmp_bal = 0;
    get_zd_double("102I", &tmp_bal);
    if (tmp_bal < 0)
    {
         vtcp_log(" %s, %d, bal[%lf]<0 ",__FILE__,__LINE__,tmp_bal);
         tmp_bal = 0;
         set_zd_double("102I", tmp_bal);
    }
	vtcp_log(" %s, %d, spe_ind[%s] ",__FILE__,__LINE__,spe_ind);
        /* spe_ind[4]=g_mdm_ac_rel.kt_flag[4]; */    /* 通存通兑 */
        set_zd_data("102U",spe_ind);    /* 特殊标志 */
	set_zd_data("102C", g_mdm_ac_rel.pwd_mach_yn); /** 是否支付密码器 */
	set_zd_data("102B", g_mdm_ac_rel.draw_seal_yn); /** 凭印鉴 */
	set_zd_data("1024", g_mdm_ac_rel.note_no);      /* 凭证号码 */
	fclose(fp);
	fp = NULL;
OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	if(fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
	sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
