/*************************************************
* 文 件 名: spC305.c
* 功能描述：   
*           公司客户信息维护
*
* 作    者: andy
* 完成日期: 2004年02月05日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "cif_basic_inf_c.h"
#include "cif_cop_inf_c.h"

static struct cif_cop_inf_c    	 cif_cop_inf, g_cif_cop_inf;
static struct cif_basic_inf_c    cif_basic_inf;

char memo[41],tbwhere[50];
char sts[2];
		
spC305()  
{ 	
	int ret=0;
	int cmp_code=0;
	
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/** 数据初始化 **/
	memset (&cif_cop_inf, 0x00, sizeof(struct cif_cop_inf_c));
	memset (&g_cif_cop_inf, 0x00, sizeof(struct cif_cop_inf_c));
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	memset(sts,0x00,sizeof(sts));

	/** 取值、赋值 **/
	get_zd_long("0280",&g_cif_cop_inf.cif_no);			/* 客户号 */
	vtcp_log("[%s] [%d] cif_no =[%ld]\n",__FILE__,__LINE__,g_cif_cop_inf.cif_no);
	get_zd_data( "0620",g_cif_cop_inf.crd_no);			/*代码证号*/
	vtcp_log("[%s] [%d] crd_no =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.crd_no);
	get_zd_data( "0630",g_cif_cop_inf.license);			/*营业执照*/
	vtcp_log("[%s] [%d] license =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.license);
	get_zd_data( "0300",g_cif_cop_inf.units_license);   /*登记证号*/
	vtcp_log("[%s] [%d] units_license =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.units_license);
	get_zd_data( "0310",g_cif_cop_inf.g_tax_no);		/*国税号*/
	vtcp_log("[%s] [%d] g_tax_no =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.g_tax_no);
	get_zd_data( "0320",g_cif_cop_inf.d_tax_no);		/*地税号*/
	vtcp_log("[%s] [%d] d_tax_no =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.d_tax_no);
	get_zd_data( "0580",g_cif_cop_inf.credit_no);		/*贷款卡号*/
	vtcp_log("[%s] [%d] credit_no =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.credit_no);
	/**前台未传此字段 delete by wanglei 20070409
	get_zd_data( "0660",g_cif_cop_inf.eco_attri);       *经济性质*
	**/
	get_zd_data( "0670",g_cif_cop_inf.ent_attri);       /*企业性质*/
	vtcp_log("[%s] [%d] ent_attri =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.ent_attri);
	/**前台未传此字段 delete by wanglei 20070409
	get_zd_data( "0220",g_cif_cop_inf.cif_scopes);      *所属领域*
	**/
	get_zd_data( "0680",g_cif_cop_inf.belong_bank);     /*归属行业(人行)*/
	vtcp_log("[%s] [%d] belong_bank =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.belong_bank);
	get_zd_data( "0690",g_cif_cop_inf.belong_stat);     /*归属行业(统计局)*/
	vtcp_log("[%s] [%d] belong_stat =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.belong_stat);
	/**前台未传此字段 delete by wanglei 20070409
	get_zd_data( "0700",g_cif_cop_inf.region);          *地域级别*
	**/
	get_zd_data( "0710",g_cif_cop_inf.mana_orga_moda);  /*经营形式*/
	vtcp_log("[%s] [%d] mana_orga_moda =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.mana_orga_moda);
	get_zd_data( "0720",g_cif_cop_inf.bd_corp_orga_moda);/*企业形式*/
	vtcp_log("[%s] [%d] bd_corp_orga_moda =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.bd_corp_orga_moda);
	/**前台未传此字段 delete by wanglei 20070409
	get_zd_data( "0230",g_cif_cop_inf.corp_act_mode);   *运作方式*
	**/
	get_zd_long( "0440",&g_cif_cop_inf.reg_date);        /*注册日期*/
	vtcp_log("[%s] [%d] reg_date =[%ld]\n",__FILE__,__LINE__,g_cif_cop_inf.reg_date);
	get_zd_data( "0890",g_cif_cop_inf.reg_country);    	/*注册国家*/
	vtcp_log("[%s] [%d] reg_country =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.reg_country);
	get_zd_data( "0210",g_cif_cop_inf.cur_no);          /*注册币种*/
	vtcp_log("[%s] [%d] cur_no =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.cur_no);
	get_zd_double( "0400",&g_cif_cop_inf.reg_fund);      /*注册资金*/
	vtcp_log("[%s] [%d] reg_fund =[%.2f]\n",__FILE__,__LINE__,g_cif_cop_inf.reg_fund);
	get_zd_double( "0410",&g_cif_cop_inf.paicl_up_capital);/*实收资本*/
	vtcp_log("[%s] [%d] paicl_up_capital =[%.2f]\n",__FILE__,__LINE__,g_cif_cop_inf.paicl_up_capital);
	get_zd_data( "0330",g_cif_cop_inf.ap_qualife);      /*法人资格*/
	vtcp_log("[%s] [%d] ap_qualife =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.ap_qualife);
	get_zd_data( "0810",g_cif_cop_inf.artificial_person);/*法人名称*/
	vtcp_log("[%s] [%d] artificial_person =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.artificial_person);
	get_zd_data( "0820",g_cif_cop_inf.ap_id);           /*法人身份证号*/
	vtcp_log("[%s] [%d] ap_id =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.ap_id);
	get_zd_data( "0900",g_cif_cop_inf.corp_size);       /*企业规模*/
	vtcp_log("[%s] [%d] corp_size =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.corp_size);
	/**前台未传此字段 delete by wanglei 20070409
	get_zd_data( "0240",g_cif_cop_inf.corp_mana_sts);   *经营状态*
	**/
	get_zd_data( "0640",g_cif_cop_inf.credit);          /*信誉度*/
	vtcp_log("[%s] [%d] credit =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.credit);
	get_zd_data( "0250",g_cif_cop_inf.supervior);       /*上级单位*/
	vtcp_log("[%s] [%d] supervior =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.supervior);
       /**26域使用错误，导致数据丢失 modify 20120601
	get_zd_data( "0260",g_cif_cop_inf.director);         *主管单位*
       **/
	get_zd_data( "0830",g_cif_cop_inf.director);         /*主管单位*/
	vtcp_log("[%s] [%d] director =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.director);
	get_zd_data("0930",sts);							/* 操作类型 */
	vtcp_log("[%s] [%d] sts =[%s]\n",__FILE__,__LINE__,sts);
	vtcp_log("[%s] [%d] sts =[%c]\n",__FILE__,__LINE__,sts[0]);
	/** 处理流程 **/
	switch(sts[0])
	{
		case '1':									/*修改*/
				ret=Cif_cop_inf_Dec_Upd(g_pub_tx.reply,
				   			"cif_no=%ld ",g_cif_cop_inf.cif_no);
	vtcp_log("[%s] [%d] stsaaaaaaaaaaaa[0] =[%c]\n",__FILE__,__LINE__,sts[0]);
			    if (ret)
			    {
				   sprintf(acErrMsg,"DECLARE Cif_cop_inf_rel游标异常![%s]",
				   g_pub_tx.reply);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
			    }
			    ret=Cif_cop_inf_Fet_Upd(&cif_cop_inf,g_pub_tx.reply);
			    if (ret)
			    {
				   sprintf(acErrMsg,"FETCH Cif_cop_inf游标异常![%s]",
				   g_pub_tx.reply);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
			    }
			    strcpy(g_cif_cop_inf.rowid,cif_cop_inf.rowid);
				vtcp_log("update success g_rowid[%s],c_rowid[%s]",g_cif_cop_inf.rowid,cif_cop_inf.rowid);
	vtcp_log("[%s] [%d] upd credit_no =[%s]\n",__FILE__,__LINE__,g_cif_cop_inf.credit_no);
	/*get_zd_data( "0580",g_cif_cop_inf.credit_no);*/		/*贷款卡号*/
			    ret=Cif_cop_inf_Upd_Upd(g_cif_cop_inf,g_pub_tx.reply);
				if (ret)
				{
					sprintf(acErrMsg,"UPDATE Cif_cop_inf游标异常![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				Cif_cop_inf_Clo_Upd( );
				
		  /* add begin 网银客户信息同步 lzy 20141204 */
			char today[9]={0};
			/*char nextday[9]={0};*/
			long nextday = 0;
			char filename[60]={0};
			char pcfilename[256]={0};
			char cFtpfile[256]={0};
			FILE *fp = NULL;
			
			apitoa(g_pub_tx.tx_date,8,today);
			/*memcpy(nextday,pub_base_daynumLAT(today,1),8);*/
			nextday = pub_base_daynumLAT(today,1);

			sprintf( filename,"CorpCustSyn%ld.txt",nextday);			
			sprintf( pcfilename,"%s/%s",getenv("FILDIR"),filename );
			vtcp_log("%s %d:: pcfilename[%ld]",__FILE__,__LINE__,pcfilename);
			fp=fopen(pcfilename,"at");
			if( fp==NULL )
			{
				vtcp_log("[%s],[%d] 创建新文件[%s]",__LINE__,__FILE__,pcfilename);
				fp = fopen(pcfilename, "wt");
				if (fp == NULL)
				{
					vtcp_log("[%s],[%d] open file[%s] error",__LINE__,__FILE__,pcfilename);
					strcpy(g_pub_tx.reply,"SM83");
					goto ErrExit;
				}
			}
			/**fprintf(fp, "客户号,证件类型,证件号码,名称,地址,邮编,电话,法人代表\n");**/
			fprintf(fp, "%ld,,,,,,,%s\n",g_cif_cop_inf.cif_no,g_cif_cop_inf.artificial_person);
			fclose(fp);
				
			sprintf(cFtpfile,"%s/bin/ftp_to_wyzt.sh %s",getenv("WORKDIR"),filename);
			vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
			ret=system(cFtpfile);
			if (ret)
				{
					vtcp_log("[%s][%d] ftp文件[%s]到网银中台出错[%d]", __FILE__, __LINE__,filename,ret);
					strcpy(g_pub_tx.reply,"S047");
					goto ErrExit;
				}	
			vtcp_log("[%s][%d] ftp文件[%s]到网银中台成功", __FILE__, __LINE__,filename);				
		  /* add end 网银客户信息同步 lzy 20141204 */
		  
				break;
		case '2':									/*删除*/

				/*删除公司客户信息表内容*/
				sprintf(tbwhere,"cif_no=%ld",g_cif_cop_inf.cif_no);
				ret =deldb("cif_cop_inf",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"删除公司客户基本信息表记录异常!![%s]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C053");
				   ERR_DEAL;
				}
				break;
		default:
				break;
	}

	/* 登记交易流水 */
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"登记交易流水 PASS!");
	WRITEMSG
		
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"公司客户基本信息维护成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"公司客户基本信息维护失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
