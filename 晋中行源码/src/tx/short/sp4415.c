/*************************************************
* 文 件 名:    sp4415.c
* 功能描述：   
*              由委托贷款账号取信息
*
* 作    者:    zgf
* 完成日期：   2013年05月22日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/	  
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "prdt_ac_id_c.h"
#include "cif_id_code_rel_c.h"
#include "mdm_attr_c.h"

sp4415()  
{ 		
	int ret=0;
	int i=0;
	char ac_no[20];
	int ac_seqn;
	int flag=0, redflag=0;
	char flag1[2];
	char tx_code[5];
	double lx=0.00, yqlx=0.00;	
	double vbjlx,vbnlx,vbylx,vfllx;
	char wherelist[200];
	char tmpstr[100];
	char filename[100];
	char dc_flg[2];
	char dc_inf[5];
	char dc_flg1[2];
	char dc_inf1[5];
	char dc_flg2[2];
	char dc_inf2[5];
	char hqcpbh[4] = {0};
	
	char ac_type[2],tmp_memo[100],tmpname[100],prdt_name[31];
	long cif_no=0, sysdate=0;
	char cur_no[5],cif_type[2],note_no[20],name[61];
	char tmp_inf[3] = {0};	
	FILE *fp;			
	struct ln_mst_c		ln_mst_tmp;
	struct ln_mst_c		g_ln_mst;
	struct mdm_ac_rel_c mdm_ac_rel_tmp;
	struct dd_mst_c 	dd_mst_tmp;
	struct cif_basic_inf_c cif_basic_inf;
	struct dd_parm_c dd_parm_tmp;
	struct dic_data_c vdic_data;
	struct prdt_ac_id_c prdt_ac_id_c;
	struct cif_id_code_rel_c g_cif_id_code_rel;
	struct mdm_attr_c g_mdm_attr;
		
	/** 数据初始化 **/
	memset(&ln_mst_tmp , 0x00 , sizeof(struct ln_mst_c) );
	memset(&g_ln_mst , 0x00 , sizeof(struct ln_mst_c) );
	memset(&dd_mst_tmp , 0x00 , sizeof(struct dd_mst_c) );
	memset(&mdm_ac_rel_tmp , 0x00 , sizeof(struct mdm_ac_rel_c) );
	memset( &cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c) );
	memset( &dd_parm_tmp,0x00,sizeof(struct dd_parm_c) );
	memset( &vdic_data,0x00,sizeof(struct dic_data_c) );
	memset( &prdt_ac_id_c,0x00,sizeof(struct prdt_ac_id_c) );
	memset( &g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c) );
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/** 取值、赋值 **/	
	get_zd_data("1081", ac_no);	/* 贷款帐号 */
	pub_base_old_acno(ac_no);  	/** 对旧帐号的处理 **/
	set_zd_data("1081", ac_no);	/* 处理旧帐号 */	
	get_zd_data("0650", tx_code); /* 交易代码 */
	get_zd_long("0440", &sysdate); /* 系统时间 */

	/* 根据贷款帐号查询账号介质关系表取出账户ID */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel , " ac_no='%s' " , ac_no );				
	if( ret==100 )
	{
		sprintf( acErrMsg,"凭证(介质)与账户关系表无此记录 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L113" );
		ERR_DEAL
	} else if( ret !=0 )
	{
  	sprintf( acErrMsg,"取凭证(介质)与账户关系表异常erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
    ERR_DEAL
  }
	sprintf(acErrMsg,"取出的账户ID为[%ld]",g_mdm_ac_rel.ac_id);
	WRITEMSG	
	
	/* 根据贷款账户ID查询委托贷款主文件个数 */
	flag = sql_count("ln_mst", " ac_id=%ld  and prdt_no in ('3Y1','3Y2','3Y3','3Y4','3Y5','3Y6','3Y7','3Y8','3Y9','3YA','3YB')" , g_mdm_ac_rel.ac_id);
	sprintf(acErrMsg,"根据贷款账户ID查询委托贷款主文件个数[%d]",flag);
	WRITEMSG	
	if( flag==0 )
  {
		set_zd_int( "108G", redflag );	/* 是否输入序号标志 1-唯一记录*/
 		sprintf(acErrMsg,"贷款主文件表中无此委托贷款记录[%d]",flag);
		WRITEMSG
 		strcpy( g_pub_tx.reply, "L002");
 		goto ErrExit;
  }	
	if( flag==1 )	
	{
		ret = Ln_mst_Sel( g_pub_tx.reply , &ln_mst_tmp , " ac_id=%ld and prdt_no in ('3Y1','3Y2','3Y3','3Y4','3Y5','3Y6','3Y7','3Y8','3Y9','3YA','3YB') " , g_mdm_ac_rel.ac_id );			
		if( ret==100 )
		{
			sprintf( acErrMsg,"贷款主文件表中无此委托贷款记录 erro code=[%d]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L002" );
			ERR_DEAL
		} else if( ret !=0 )
		{
    		sprintf( acErrMsg,"取贷款主文件异常 erro code=[%d]",ret);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"D103" );
    		ERR_DEAL
    }	
    sprintf(acErrMsg,"取贷款主文件PASS");
		WRITEMSG 
	
		/* 贷款状态为销户时，不能做交易 */
    if ( ln_mst_tmp.ac_sts[0]=='*' )
    {
    	sprintf(acErrMsg,"此贷款已销户!");
      WRITEMSG
      strcpy(g_pub_tx.reply,"L035");
      goto ErrExit;
    }
	
	  if( ln_mst_tmp.ac_sts[0] =='1' )
    {
    	sprintf( acErrMsg,"!!!!!!!!!!![%ld]",g_pub_tx.tx_date);
      WRITEMSG
      if ( ln_mst_tmp.mtr_date >= g_pub_tx.tx_date )
      {
      	flag1[0]='1';    /* 正常 */
      }else{
        flag1[0]='2';    /* 展期 */
      }
    }

    if( ln_mst_tmp.ac_sts[0] =='2' || ln_mst_tmp.ac_sts[0] =='3' ||ln_mst_tmp.ac_sts[0] =='4')
    {
      flag1[0]='3';    /* 逾期 */
    }	
		
	ret = Dd_mst_Sel( g_pub_tx.reply , &dd_mst_tmp , " ac_id=%ld " , ln_mst_tmp.repay_ac_id );	
	if( ret==100 )
	{
		sprintf( acErrMsg,"还款主文件表中无此记录 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L002" );
		ERR_DEAL
	} 
	else if( ret !=0 )
	{
  	sprintf( acErrMsg,"取还款主文件异常 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
    ERR_DEAL
  }	
  strcpy(hqcpbh , dd_mst_tmp.prdt_no);
  sprintf(acErrMsg,"取还款主文件PASS");
	WRITEMSG 
					
	/* 根据贷款帐号查询账号介质关系表取出账户ID */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &mdm_ac_rel_tmp , " ac_id=%ld " , dd_mst_tmp.ac_id );				
	if( ret==100 )
	{
		sprintf( acErrMsg,"凭证(介质)与账户关系表无此记录 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L113" );
		ERR_DEAL
	} 
	else if( ret !=0 )
	{
  	sprintf( acErrMsg,"取凭证(介质)与账户关系表异常erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
    ERR_DEAL
  }
	sprintf(acErrMsg,"取出的账户AC_NO为[%ld]",mdm_ac_rel_tmp.ac_no);
	WRITEMSG
	vtcp_log("bbbbbbbbbbbbbbbb [%s]",mdm_ac_rel_tmp.note_no);
	/* 根据贷款帐号查询账号介质关系表取出账户ID */
	ret = Dd_parm_Sel( g_pub_tx.reply , &dd_parm_tmp , " prdt_no='%s' " , dd_mst_tmp.prdt_no );				
	if( ret==100 )
	{
		sprintf( acErrMsg,"凭证(介质)与账户关系表无此记录 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L113" );
		ERR_DEAL
	} else if( ret !=0 )
	{
  	sprintf( acErrMsg,"取凭证(介质)与账户关系表异常erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
    ERR_DEAL
  }
	sprintf(acErrMsg,"取出的账户类型cif_type为[%s]",dd_parm_tmp.cif_type);
	WRITEMSG

	strcpy(cif_type,dd_parm_tmp.cif_type);
	strcpy(ac_type,cif_type);
	
	/***开始下载菜单***/
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");

	if( ac_type[0]=='2' )
	{
		if( strcmp(tmp_inf,"01" ) )	/* 有初值 */
		{
			ret=Dic_data_Sel( g_pub_tx.reply,&vdic_data,"pr_code='tran_memo' and pr_seq='%s' " , tmp_inf);
			if( ret==100 )
			{
				strcpy( tmp_inf , "01" );
			}else if(ret){
				goto ErrExit;
			}else{
				pub_base_strpack(vdic_data.pr_seq);
				pub_base_strpack(vdic_data.content);
				set_zd_data("102L",vdic_data.pr_seq);
				fprintf(fp,"102L|%-20s|%s|\n",vdic_data.content,vdic_data.pr_seq);
			}
		}
		if( strcmp(tmp_inf,"01")==0)
		{
			/* 若无初值 */
			/***下载对公摘要列表***/
			ret=Dic_data_Dec_Sel( g_pub_tx.reply,"pr_code='tran_memo'");
			if( ret ) goto ErrExit;

			while(1)
			{
				ret = Dic_data_Fet_Sel(&vdic_data,g_pub_tx.reply);
				if( ret==100 ) break;
				if( ret ) goto ErrExit;

				pub_base_strpack(vdic_data.pr_seq);
				pub_base_strpack(vdic_data.content);
				if(i==0)
				{
					set_zd_data("102L",vdic_data.pr_seq);
					strcpy(tmp_memo,vdic_data.pr_seq);
				}
				fprintf(fp,"102L|%-20s|%s|\n",vdic_data.content,vdic_data.pr_seq);
				i++;
			}
			Dic_data_Clo_Sel();
		}
	}
	vtcp_log("aaaaaaaaaaaaaaaaaaaaaaaaaa[%s]",g_mdm_ac_rel.mdm_code);
  /*if( strcmp(g_mdm_ac_rel.mdm_code,NULL_MDM_CODE) && ac_type[0]=='1')*/
  if( strcmp(mdm_ac_rel_tmp.mdm_code,NULL_MDM_CODE) && ac_type[0]=='1')
	{
		memset( &g_mdm_attr, 0x00, sizeof(struct mdm_attr_c) );
		ret=Mdm_attr_Sel(g_pub_tx.reply,&g_mdm_attr,"mdm_code='%s'", \
						 mdm_ac_rel_tmp.mdm_code);
		if( ret )
		{
			sprintf(acErrMsg,"查询介质属性表错误![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "B004");
			goto ErrExit;
	
		}
		vtcp_log("[%s][%d]aaaaaaa[%s]",__FILE__,__LINE__,g_mdm_attr.note_type );	
		strcpy(note_no,g_mdm_attr.note_type);
		/***选项列表***/
		ret = Dic_data_Dec_Sel( g_pub_tx.reply, \
							 "pr_code='per_dety' and pr_seq='%s'",note_no);
		if( ret ) goto ErrExit;
		i=0;
		while(1)
		{
			ret = Dic_data_Fet_Sel(&vdic_data,g_pub_tx.reply);
			if( ret==100 ) break;
			if( ret ) goto ErrExit;

			pub_base_strpack(vdic_data.pr_seq);
			pub_base_strpack(vdic_data.content);

			if( i==0 )
			{
				set_zd_data("1023",vdic_data.pr_seq);
				strcpy(note_no,vdic_data.pr_seq);
			}
			fprintf(fp,"1023|%-20s|%s|\n",vdic_data.content,vdic_data.pr_seq);
			i++;
		}
		Dic_data_Clo_Sel();
	}
	else
	{
		if( ac_type[0]=='2' )			/*下载对公取款凭证类型*/
		{
			/***选项列表***/
			i=0;
			ret=Dic_data_Dec_Sel( g_pub_tx.reply, \
				"pr_code='pub_taty' and pr_seq[1,3]<>'001' order by pr_seq");
			if( ret ) goto ErrExit;

			while(1)
			{
				ret = Dic_data_Fet_Sel(&vdic_data,g_pub_tx.reply);
				if( ret==100 ) break;
				if( ret ) goto ErrExit;

				pub_base_strpack(vdic_data.pr_seq);
				pub_base_strpack(vdic_data.content);

				if( i==0 )
				{
					strcpy(note_no,vdic_data.pr_seq);
					set_zd_data("1023",vdic_data.pr_seq);
				}
		vtcp_log("[%s][%d]aaaaaaa[%s][%s]",__FILE__,__LINE__,vdic_data.content,vdic_data.pr_seq );	
				
				fprintf(fp,"1023|%-20s|%s|\n",vdic_data.content,vdic_data.pr_seq);
				i++;
			}
			Dic_data_Clo_Sel();
		}
	}
	fclose(fp);
	set_zd_data(DC_FILE_SND,"2");
	/***选项列表下载完毕***/	
		
	/* 输出变量 */
	sprintf(acErrMsg,"进入输出变量");
	WRITEMSG
	redflag=1;
	set_zd_int( "1082", ln_mst_tmp.ac_seqn );	/* 贷款账户序号 */
	
	set_zd_int( "108G", redflag );	/* 是否输入序号标志 1-唯一记录*/	
		

vtcp_log("name is [%s]",ln_mst_tmp.name);
vtcp_log("bal is [%.2lf]",ln_mst_tmp.bal);
vtcp_log("huan ac_no is [%s]",g_mdm_ac_rel.ac_no);
vtcp_log("huan name is [%s]",dd_mst_tmp.name);
vtcp_log("huan bal is [%.2lf]",dd_mst_tmp.bal);
vtcp_log("chanpin num is [%s]",dd_mst_tmp.prdt_no );
vtcp_log("zhlx is [%s]",dd_parm_tmp.cif_type);
vtcp_log("ic_date is [%ld]",ln_mst_tmp.ic_date);
vtcp_log("note_type is [%s]",g_mdm_attr.note_type);
			set_zd_double("0410",ln_mst_tmp.bal);
			set_zd_data("0310",mdm_ac_rel_tmp.ac_no);
			/*set_zd_data("0310","5004692900019");
			set_zd_data("0310","1000055200017");*/
			set_zd_data("0250",dd_mst_tmp.name);
			set_zd_data("0260",ln_mst_tmp.name);
			set_zd_data("0270",dd_mst_tmp.name);
			set_zd_double("0400",dd_mst_tmp.bal);
			set_zd_data("0700",dd_parm_tmp.cif_type);
			set_zd_data("0320",mdm_ac_rel_tmp.note_no);		/*凭证号码*/
			set_zd_data("0900",g_mdm_attr.note_type);		/*凭证种类*/
			
			set_zd_data("1025",mdm_ac_rel_tmp.draw_uncon_yn); /*是否任意支取*/
			set_zd_data("0710",mdm_ac_rel_tmp.draw_pwd_yn);	/*密码支取*/
      set_zd_data("1027",mdm_ac_rel_tmp.qry_pwd);  /*查询密码*/
      set_zd_data("1028",mdm_ac_rel_tmp.draw_pwd); /*支取密码*/
      set_zd_long("0450",ln_mst_tmp.ic_date);  /**获取取息日期**/
      set_zd_data("0370",ln_mst_tmp.trust_no);/* 获取委托协议编号*/
      vtcp_log("trust_no is [%s]",ln_mst_tmp.trust_no);
      /*特殊标志*/
      set_zd_data("102B",mdm_ac_rel_tmp.draw_seal_yn); /*是否凭印鉴支取*/
			if(flag==1&&mdm_ac_rel_tmp.draw_id_yn[0]=='N')
			{
	 			set_zd_data("0690","N");                        /*证件支取*/
				set_zd_data("0720",g_cif_id_code_rel.id_type);  /*证件类型*/
				set_zd_data("0330",g_cif_id_code_rel.id_no);    /*证件号码*/
			}
			else if(flag==1)
			{
				set_zd_data("0690","Y");                        /*证件支取*/
				set_zd_data("0720",mdm_ac_rel_tmp.id_type);       /*证件类型*/
				set_zd_data("0330",mdm_ac_rel_tmp.id_no);         /*证件号码*/
			}
			else
			{
				set_zd_data("0690",mdm_ac_rel_tmp.draw_id_yn);    /*证件支取*/
				set_zd_data("0720",mdm_ac_rel_tmp.id_type);       /*证件类型*/
				set_zd_data("0330",mdm_ac_rel_tmp.id_no);         /*证件号码*/
			}
		
			vtcp_log("委托贷款余额[%f]",ln_mst_tmp.bal);
			vtcp_log("最近一笔交易日期[%d]",ln_mst_tmp.lst_date);
			vtcp_log("还息日期[%d]",g_pub_tx.tx_date);
			vtcp_log("系统日期[%d]",sysdate);

/**
* 完成时间：  2003年01月14日
*
* 参    数：
*     输  入：beg_date   long     起始日期(上笔发生日期)
*             end_date   ilong    终止日期(截至日期（交易日期）)
*                 day_type       char     计息天数类型（0按实际天数、1按30天计算）
*             acm_type   char     积数计算类型（1每日累计2变动累计3变动累计倒算
*             bal        double   交易前余额(倒算利息用)
*             amt        double   金额(金额应传入发生额，且增为正减为负)
*             acm        double   利息积数
*(倒算利息用)
* intst_type计息类型 0不计息1利随本清2按日计息3按月计息4按季计息5按年计息
* intst_mon计息月份
* intst_date计息日期
*     输  出: acm        double   利息积数
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*

int pub_base_CalAcm_Ln( beg_date,end_date,day_type,acm_type,bal,amt,acm
        ,intst_type,intst_moon,intst_date )

********************************************************************/
			
			/***只有未结利息***/
			if(g_pub_tx.tx_date < ln_mst_tmp.mtr_date)
			{
				ret = 0;
      	ret = pub_base_CalAcm_Ln( ln_mst_tmp.lst_date,g_pub_tx.tx_date,
                                                "0","2",ln_mst_tmp.bal,0.00,
                                                &g_ln_mst.intst_acm,"3",0,0 );
 				if( ret )
      	{
      		sprintf(acErrMsg,"调用函数计算积数错误!");
        	WRITEMSG
        	return -1;
      	}
				vtcp_log("计算出的未结利息积数[%f]",g_ln_mst.intst_acm);
				vtcp_log("原有未结利息积数[%f]",ln_mst_tmp.intst_acm);
      	vtcp_log("总未结利息积数[%f]",ln_mst_tmp.intst_acm + g_ln_mst.intst_acm);
      	lx = ln_mst_tmp.rate * (g_ln_mst.intst_acm + ln_mst_tmp.intst_acm) / 30000;
      	lx = pub_base_PubDround_1(lx);
      	if ( pub_base_CompDblVal(lx,0.00)<0 )
      	{
        	sprintf(acErrMsg,"计算未结利息错误!");
      		WRITEMSG
        	goto ErrExit;
      	}
      	vtcp_log("只有未结利息金额[%f]",lx);		
				set_zd_double("0560",lx);
				sprintf(acErrMsg,"只有未结利息!");
				WRITEMSG
				goto OkExit;
			}else if( ln_mst_tmp.ic_date > ln_mst_tmp.mtr_date) /**只有逾期利息**/
			{
				memset(&g_ln_mst , 0x00 , sizeof(struct ln_mst_c) );
				ret = 0;
      	ret = pub_base_CalAcm_Ln( ln_mst_tmp.ic_date,g_pub_tx.tx_date,
                                                "0","2",ln_mst_tmp.bal,0.00,
                                                &g_ln_mst.intst_acm,"3",0,0 );
 				if( ret )
      	{
      		sprintf(acErrMsg,"调用函数计算积数错误!");
        	WRITEMSG
        	return -1;
      	}
				vtcp_log("计算出的逾期利息积数[%f]",g_ln_mst.intst_acm);
				vtcp_log("原有未结利息积数[%f]",ln_mst_tmp.intst_acm);
      	vtcp_log("总逾期利息积数[%f]",ln_mst_tmp.intst_acm + g_ln_mst.intst_acm);
      	/*** 计算应还逾期利息 ***/
      	lx = ln_mst_tmp.over_rate * (g_ln_mst.intst_acm + ln_mst_tmp.intst_acm) / 30000;
      	lx = pub_base_PubDround_1(lx);
      	if ( pub_base_CompDblVal(lx,0.00)<0 )
      	{
        	sprintf(acErrMsg,"计算未结利息错误!");
      		WRITEMSG
        	goto ErrExit;
      	}
      	vtcp_log("逾期利息金额[%f]",lx);		
				set_zd_double("0560",lx);
				set_zd_double("0570",lx);	
				sprintf(acErrMsg,"只有逾期!");
				WRITEMSG
				goto OkExit;
			}
			else /**算未结+逾期利息之和**/
			{		/**算未结利息部分**/	
				memset(&g_ln_mst , 0x00 , sizeof(struct ln_mst_c) );
				ret = 0;
        ret = pub_base_CalAcm_Ln( ln_mst_tmp.ic_date,ln_mst_tmp.mtr_date,
                                                "0","2",ln_mst_tmp.bal,0.00,
                                                &g_ln_mst.intst_acm,"3",0,0 );
 				if( ret )
      	{
      		sprintf(acErrMsg,"调用函数计算积数错误!");
        	WRITEMSG
        	return -1;
      	}
      	vtcp_log("未逾期利息积数g_ln_mst.intst_acm=[%f],ln_mst_tmp.intst_acm=[%f]",g_ln_mst.intst_acm,ln_mst_tmp.intst_acm);
      	lx = ln_mst_tmp.rate * (g_ln_mst.intst_acm ) / 30000;
      	lx = pub_base_PubDround_1(lx);
      	if ( pub_base_CompDblVal(lx,0.00)<0 )
      	{
        	sprintf(acErrMsg,"计算未结利息错误!");
      		WRITEMSG
        	goto ErrExit;
      	}
      	/**算逾期利息部分**/
      	memset(&g_ln_mst , 0x00 , sizeof(struct ln_mst_c) );
				ret = 0;
        ret = pub_base_CalAcm_Ln( ln_mst_tmp.mtr_date,g_pub_tx.tx_date,
                                                "0","2",ln_mst_tmp.bal,0.00,
                                                &g_ln_mst.intst_acm,"3",0,0 );
 				if( ret )
      	{
      		sprintf(acErrMsg,"调用函数计算积数错误!");
        	WRITEMSG
        	return -1;
     		}
      	vtcp_log("逾期利息积数[%f]",g_ln_mst.intst_acm);
      	yqlx = ln_mst_tmp.over_rate * g_ln_mst.intst_acm  / 30000;
      	yqlx = pub_base_PubDround_1(yqlx);
      	if ( pub_base_CompDblVal(yqlx,0.00)<0 )
      	{
        	sprintf(acErrMsg,"计算逾期利息错误!");
      		WRITEMSG
        	goto ErrExit;
      	}
      	vtcp_log("未结利息部分利息金额[%f]",lx);
      	vtcp_log("逾期利息部分利息金额[%f]",yqlx);
      	lx = lx + yqlx;
      	vtcp_log("总利息金额[%f]",lx);
				set_zd_double("0560",lx);		
				set_zd_double("0570",yqlx);	
				sprintf(acErrMsg,"既有未结利息又有逾期利息，利息之和!");
				WRITEMSG
 				goto OkExit;
 		}
	}   	
  redflag=9;
	set_zd_int( "108G", redflag );	/* 是否输入序号标志 1-唯一记录*/	 		
		
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}    
        
