/*************************************************
* 文 件 名:  sp8506.c
* 功能描述： 支行总账查询
*
* 作    者:  xyy
* 完成日期： 
*
* 修改记录: 修改为查询支行今日总帐 ?* 日   期:
* 修 改 人:  Jarod
* 修改内容:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "gl_sub_c.h"
#include "com_item_c.h"
#include "dc_log_c.h"

int sp8506()
{
	struct com_item_c sCom_item;
	struct gl_sub_c vgl_sub;
	struct gl_sub_c f_gl_sub;
	struct gl_sub_c f_gl_suba;
	char filename[100];
	char wherelist[1024];  /**查询条件**/
	char tmpstr[512];
	char cAcc_no[8];
	char cAcc_hrt[6];
	int ttlnum=0;			/**读取的总条数**/
	int i=0;
	int ret=0;
	FILE *fp;
	ttlnum=0;
	struct dc_log_c sDc_log;
	double dr_amt=0.00; /*借方发生额*/
	double cr_amt=0.00; /*贷方发生额*/
	double cdr_amt=0.00, ccr_amt=0.00, tdr_amt=0.00, tcr_amt=0.00;
	int cdr_cnt=0, ccr_cnt=0, tdr_cnt=0, tcr_cnt=0;
	double tmp_amt=0.00;

	memset(&vgl_sub,0x00,sizeof(struct gl_sub_c));
	memset(&f_gl_sub,0x00,sizeof(struct gl_sub_c));
	memset(&f_gl_suba,0x00,sizeof(struct gl_sub_c));
	memset(&sDc_log,0x00,sizeof(struct dc_log_c));
	memset(cAcc_no   ,0,sizeof(cAcc_no));
	memset(cAcc_hrt  ,0,sizeof(cAcc_hrt));
	memset( &sCom_item,0,sizeof(struct com_item_c) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( filename,0,sizeof(filename) );
	memset( tmpstr,0,sizeof(tmpstr) );
		
	pub_base_sysinit();

	get_zd_data("0910",f_gl_sub.br_no );
	get_zd_data("0210",f_gl_sub.cur_no );
	get_zd_data("0640",cAcc_no );
	get_zd_long("0290",&f_gl_sub.date );
	get_zd_long("0440",&f_gl_suba.date );
	vtcp_log("科目号:[%s][%d]cAcc_no=[%s]",__FILE__,__LINE__,cAcc_no);
	
	/*首先取科目的属性从com_item*/   
	ret=Com_item_Dec_Sel(g_pub_tx.reply,"acc_no like '%s%%%%' order by acc_no ",cAcc_no);
	if(ret)
	{
		sprintf(acErrMsg,"声明com_item游标错误!![%d]",ret);
		WRITEMSG
		goto ErrExit;	
	} 
	while(1) /*1 while begin */ 
	{
    		memset( &sCom_item,0,sizeof(struct com_item_c) );
		memset( wherelist,0,sizeof(wherelist) );
		memset( filename,0,sizeof(filename) );
		memset( tmpstr,0,sizeof(tmpstr) );
		memset(&vgl_sub,0x00,sizeof(struct gl_sub_c));
		memset(&sDc_log,0x00,sizeof(struct dc_log_c));
		
		ret=Com_item_Fet_Sel(&sCom_item,g_pub_tx.reply);
		if(ret == 100) break;
		if(ret)
		{
			sprintf(acErrMsg,"FETCH com_item错误!![%d]",ret);
			WRITEMSG
			goto ErrExit;	
		}
		/*组合查询条件*/
		if(strlen(f_gl_sub.br_no))
		{
			sprintf(tmpstr," br_no='%s' and ",f_gl_sub.br_no);
			strcat(wherelist,tmpstr); 
		}
		if(strlen(f_gl_sub.cur_no))
		{
			sprintf(tmpstr," cur_no='%s' and ",f_gl_sub.cur_no);
			strcat(wherelist,tmpstr);
		}			 
		if(strlen(sCom_item.acc_hrt))
		{
			sprintf(tmpstr," acc_hrt='%s' and ",sCom_item.acc_hrt);
			strcat(wherelist,tmpstr);
		}
		/*	 gl_sub里每次都是更新date,所以这个date的条件是没用的
		if(f_gl_sub.tx_date != 0 && f_gl_suba.tx_date!=0)
		{
			sprintf(tmpstr," tx_date=%ld and ",f_gl_sub.tx_date,f_gl_suba.tx_date);
			strcat(wherelist,tmpstr);
		}
		*/
		sprintf(tmpstr," 1=1 ");
		strcat(wherelist,tmpstr);
		/*查询支行主文件 */
		vtcp_log("gl_sub's WHERELIST:[%s]  [%s]--[%d]",wherelist,__FILE__,__LINE__);	
		ret=Gl_sub_Dec_Sel(g_pub_tx.reply,wherelist);
		if(ret)
		{
			sprintf(acErrMsg,"声明gl_sub游标出错!![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}	
		while(1) /*2  gl_sub while*/
		{
			memset(&vgl_sub,0x00,sizeof(struct gl_sub_c));
			memset( wherelist,0,sizeof(wherelist) );
			memset( tmpstr,0,sizeof(tmpstr) );
		
			ret=Gl_sub_Fet_Sel(&vgl_sub,g_pub_tx.reply);
			if(ret == 100) break;
			if(ret)
			{
				sprintf(acErrMsg,"FETCH gl_sub出错!![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
	    		if( !ttlnum )
	    		{
	    			pub_base_AllDwnFilName( filename );
	    			fp = fopen( filename,"w" );
	    			if( fp==NULL )
	    			{
	    				sprintf(acErrMsg," open file [%s] error ",filename );
	    				WRITEMSG
	    				strcpy( g_pub_tx.reply,"S047" );
	    				goto ErrExit;
	    			}
                	
	    			/**显示列标题**/
	    			fprintf( fp,"~@科目|@科目名称|@币种|交易日期|$当前借方余额|$当前贷方余额|$昨日借方余额|$昨日贷方余额|$本日借笔数|$本日贷笔数|$本日借发生额|$本日贷发生额|本日现金借笔数|本日现金贷笔数|$本日现金借发生额|$本日现金贷发生额|$旬初借方余额|$旬初贷方余额|$本旬借方笔数|$本旬贷方笔数|$本旬借方发生额|$本旬贷方发生额|$月初借余额|$月初贷余额|$本月借笔|$本月贷笔数|$本月借发生额|$本月贷发生额|$季初借余额|$季初贷余额|$本季借笔数|$本季贷笔数|$本季借发生额|$本季贷发生额|$年初借余额|$年初贷余额|$本年借笔数|$本年贷笔数|$本年借发生额|$本年贷发生额|\n" );
	    		}

			strcpy(cAcc_hrt, vgl_sub.acc_hrt);
			vtcp_log("while %s, %c", cAcc_hrt, cAcc_hrt[strlen(cAcc_hrt)-1]);
			while(cAcc_hrt[strlen(cAcc_hrt)-1] == '0' ) cAcc_hrt[strlen(cAcc_hrt)-1] = 0x0;
			dr_amt=0.00; /*借方发生额*/
			cr_amt=0.00; /*贷方发生额*/
			cdr_cnt = 0; ccr_cnt = 0; tdr_cnt = 0; tcr_cnt = 0; cdr_amt= 0.0; ccr_amt = 0.0; tdr_amt = 0.0; tcr_amt = 0.0;
			tmp_amt=0.00;
			if(!strcmp(vgl_sub.br_no,"10000"))
			{
				sprintf(tmpstr,"  cur_no='%s' and dc_ind='1' and sts='0' ",vgl_sub.cur_no);
			}
			else
			{
				sprintf(tmpstr," tx_opn_br_no='%s' and  cur_no='%s' and dc_ind='1' and sts='0' ",vgl_sub.br_no,vgl_sub.cur_no);
			}
				/**sprintf(wherelist+strlen(wherelist), "%s and acc_hrt like '%s%%%%'", tmpstr, vgl_sub.acc_hrt);**/
				sprintf(wherelist+strlen(wherelist), "%s and acc_hrt in (select acc_hrt from com_item where acc_no like '%s%%%%') ", tmpstr, sCom_item.acc_no);
			/*strcat(wherelist,tmpstr);*/

			ret=sql_sum_double("dc_log","amt",&dr_amt,wherelist);
			if(ret)
			{
				sprintf(acErrMsg,"sql_sum_double DC_LOG 出错!![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			vtcp_log("\n%d@%s...tmpstr(%s) \n", __LINE__, __FILE__, tmpstr);
			/**if((ret = (tdr_cnt=sql_count_sum_double("dc_log", "amt", &tdr_amt, "acc_hrt like '%s%%%%' and %s and ct_ind != '1'", cAcc_hrt, tmpstr)))<0) ERR_DEAL
			if((ret = (cdr_cnt=sql_count_sum_double("dc_log", "amt", &cdr_amt, "acc_hrt like '%s%%%%' and %s and ct_ind = '1'", cAcc_hrt, tmpstr)))<0) ERR_DEAL**/
			if((ret = (tdr_cnt=sql_count_sum_double("dc_log", "amt", &tdr_amt, "acc_hrt in (select acc_hrt from com_item where acc_no like '%s%%%%') and %s and ct_ind != '1'", sCom_item.acc_no, tmpstr)))<0) ERR_DEAL
			if((ret = (cdr_cnt=sql_count_sum_double("dc_log", "amt", &cdr_amt, "acc_hrt in (select acc_hrt from com_item where acc_no like '%s%%%%') and %s and ct_ind = '1'", sCom_item.acc_no, tmpstr)))<0) ERR_DEAL
			vtcp_log("\ndc_log's WHERELIST 取借方发生额: (%s)[%s]  [%s]--[%d], dr_amt[%.2lf] ret%d",cAcc_hrt, wherelist,__FILE__,__LINE__, dr_amt, ret);	
			/*得到科目今日的发生额后,计算科目今日的借方余额*/
			if(vgl_sub.dc_ind[0] == '1')
			{
				vgl_sub.dr_bal+=dr_amt;
			}
			else if(vgl_sub.dc_ind[0] == '2')
			{
				vgl_sub.cr_bal-=dr_amt;
			}
			else if(vgl_sub.dc_ind[0] == '0')
			{
				if(sCom_item.roll_ind[0] == 'Y') /*扎差*/
				{
					tmp_amt=vgl_sub.dr_bal-vgl_sub.cr_bal+dr_amt;
					vgl_sub.dr_bal=0.00;
					vgl_sub.cr_bal=0.00;
					if(tmp_amt>0.001)
						vgl_sub.dr_bal=tmp_amt;
					else
						vgl_sub.cr_bal=-tmp_amt;
				}
				else if(sCom_item.roll_ind[0] == 'N') /*不扎差*/
				{
					vgl_sub.dr_bal+=dr_amt;	
				}
			}	
			memset( wherelist,0,sizeof(wherelist) );
			memset( tmpstr,0,sizeof(tmpstr) );
			tmp_amt=0.00;
			vgl_sub.tddr_bal+=dr_amt;
			if(!strcmp(vgl_sub.br_no,"10000"))
			{
				sprintf(tmpstr,"  cur_no='%s' and dc_ind='2' and sts='0' ",vgl_sub.cur_no);
			}
			else
			{
				sprintf(tmpstr," tx_opn_br_no='%s' and  cur_no='%s' and dc_ind='2' and sts='0' ",vgl_sub.br_no,vgl_sub.cur_no);
			}
			/**	
			sprintf(wherelist+strlen(wherelist), " %s and acc_hrt like '%s%%%%' ", tmpstr, vgl_sub.acc_hrt);
			**/
			sprintf(wherelist+strlen(wherelist), "%s and acc_hrt in (select acc_hrt from com_item where acc_no like '%s%%%%') ", tmpstr, sCom_item.acc_no);
			/*strcat(wherelist,tmpstr);*/
			ret=sql_sum_double("dc_log","amt",&cr_amt,wherelist);
			if(ret)
			{
				sprintf(acErrMsg,"sql_sum_double DC_LOG 出错!![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			/**
			if((ret = (tcr_cnt=sql_count_sum_double("dc_log", "amt", &tcr_amt, "acc_hrt like '%s%%%%' and %s and ct_ind != '1'", cAcc_hrt, tmpstr)))<0) ERR_DEAL
			if((ret = (ccr_cnt=sql_count_sum_double("dc_log", "amt", &ccr_amt, "acc_hrt like '%s%%%%' and %s and ct_ind = '1'", cAcc_hrt, tmpstr)))<0) ERR_DEAL
			**/
			if((ret = (tcr_cnt=sql_count_sum_double("dc_log", "amt", &tcr_amt, "acc_hrt in (select acc_hrt from com_item where acc_no like '%s%%%%') and %s and ct_ind != '1'", sCom_item.acc_no, tmpstr)))<0) ERR_DEAL
			if((ret = (ccr_cnt=sql_count_sum_double("dc_log", "amt", &ccr_amt, "acc_hrt in (select acc_hrt from com_item where acc_no like '%s%%%%') and %s and ct_ind = '1'", sCom_item.acc_no, tmpstr)))<0) ERR_DEAL
			vtcp_log("dc_log's WHERELIST (%s)取贷方发生额:[%s]  [%s]--[%d] cr_amt[%.2lf] ret %d \n tcr_cnt %d ccr_cnt %d tcr_amt %.2lf tdr_amt %.2lf",cAcc_hrt, wherelist,__FILE__,__LINE__, cr_amt, ret, tcr_cnt, ccr_cnt, tcr_amt, tdr_amt);	
			vgl_sub.rdd_cnt=tdr_cnt;
			vgl_sub.rcd_cnt=tcr_cnt;
			vgl_sub.rdd_amt=tdr_amt;
			vgl_sub.rcd_amt=tcr_amt;
                	
			vgl_sub.cdd_cnt=cdr_cnt;
			vgl_sub.ccd_cnt=ccr_cnt;
			vgl_sub.cdd_amt=cdr_amt;
			vgl_sub.ccd_amt=ccr_amt;


			/*得到科目今日的发生额后,计算科目今日的贷方余额*/
			
			if(vgl_sub.dc_ind[0] == '1')
			{
				vgl_sub.dr_bal-=cr_amt;
			}
			else if(vgl_sub.dc_ind[0] == '2')
			{
				vgl_sub.cr_bal+=cr_amt;
			}
			else if(vgl_sub.dc_ind[0] == '0')
			{
				if(sCom_item.roll_ind[0] == 'Y') /*扎差*/
				{
					tmp_amt=vgl_sub.dr_bal-vgl_sub.cr_bal-cr_amt;
					vgl_sub.dr_bal=0.00;
					vgl_sub.cr_bal=0.00;
					if(tmp_amt>0.001)
						vgl_sub.dr_bal=tmp_amt;
					else
						vgl_sub.cr_bal=-tmp_amt;
				}
				else if(sCom_item.roll_ind[0] == 'N') /*不扎差*/
				{
					vgl_sub.cr_bal+=cr_amt;	
				}
			}
		vtcp_log("计算完成后:vgl_sub.dr_bal:[%lf],vgl_sub.cr_bal[%lf] [%s]--[%d]",vgl_sub.dr_bal,vgl_sub.cr_bal,__FILE__,__LINE__);	
	    	if(vgl_sub.date<-2147000000) vgl_sub.date=0;
	    	if(vgl_sub.dr_bal<-2147000000) vgl_sub.dr_bal=0.0;
	    	if(vgl_sub.cr_bal<-2147000000) vgl_sub.cr_bal=0.0;
	    	if(vgl_sub.ldd_bal<-2147000000) vgl_sub.ldd_bal=0.0;
	    	if(vgl_sub.lcd_bal<-2147000000) vgl_sub.lcd_bal=0.0;
	    	if(vgl_sub.rdd_cnt<-2147000000) vgl_sub.rdd_cnt=0.0;
	    	if(vgl_sub.rcd_cnt<-2147000000) vgl_sub.rcd_cnt=0.0;
	    	if(vgl_sub.rdd_amt<-2147000000) vgl_sub.rdd_amt=0.0;
	    	if(vgl_sub.rcd_amt<-2147000000) vgl_sub.rcd_amt=0.0;
	    	if(vgl_sub.cdd_cnt<-2147000000) vgl_sub.cdd_cnt=0;
	    	if(vgl_sub.ccd_cnt<-2147000000) vgl_sub.ccd_cnt=0;
	    	if(vgl_sub.cdd_amt<-2147000000) vgl_sub.cdd_amt=0.0;
	    	if(vgl_sub.ccd_amt<-2147000000) vgl_sub.ccd_amt=0.0;
	    	if(vgl_sub.tddr_bal<-2147000000) vgl_sub.tddr_bal=0.0;
	    	if(vgl_sub.tdcr_bal<-2147000000) vgl_sub.tdcr_bal=0.0;
	    	if(vgl_sub.tddr_cnt<-2147000000) vgl_sub.tddr_cnt=0.0;
	    	if(vgl_sub.tdcr_cnt<-2147000000) vgl_sub.tdcr_cnt=0.0;
	    	if(vgl_sub.tddr_amt<-2147000000) vgl_sub.tddr_amt=0.0;
	    	if(vgl_sub.tdcr_amt<-2147000000) vgl_sub.tdcr_amt=0.0;
	    	if(vgl_sub.mdr_bal<-2147000000) vgl_sub.mdr_bal=0.0;
	    	if(vgl_sub.mcr_bal<-2147000000) vgl_sub.mcr_bal=0.0;
	    	if(vgl_sub.mdr_cnt<-2147000000) vgl_sub.mdr_cnt=0.0;
	    	if(vgl_sub.mcr_cnt<-2147000000) vgl_sub.mcr_cnt=0.0;
	    	if(vgl_sub.mdr_amt<-2147000000) vgl_sub.mdr_amt=0.0;
	    	if(vgl_sub.mcr_amt<-2147000000) vgl_sub.mcr_amt=0.0;
	    	if(vgl_sub.qdr_bal<-2147000000) vgl_sub.qdr_bal=0.0;
	    	if(vgl_sub.qcr_bal<-2147000000) vgl_sub.qcr_bal=0.0;
	    	if(vgl_sub.qdr_cnt<-2147000000) vgl_sub.qdr_cnt=0.0;
	    	if(vgl_sub.qcr_cnt<-2147000000) vgl_sub.qcr_cnt=0.0;
	    	if(vgl_sub.qdr_amt<-2147000000) vgl_sub.qdr_amt=0.0;
	    	if(vgl_sub.qcr_amt<-2147000000) vgl_sub.qcr_amt=0.0;
	    	if(vgl_sub.ydr_bal<-2147000000) vgl_sub.ydr_bal=0.0;
	    	if(vgl_sub.ycr_bal<-2147000000) vgl_sub.ycr_bal=0.0;
	    	if(vgl_sub.ydr_cnt<-2147000000) vgl_sub.ydr_cnt=0.0;
	    	if(vgl_sub.ycr_cnt<-2147000000) vgl_sub.ycr_cnt=0.0;
	    	if(vgl_sub.ydr_amt<-2147000000) vgl_sub.ydr_amt=0.0;
	    	if(vgl_sub.ycr_amt<-2147000000) vgl_sub.ycr_amt=0.0;
     
	    	fprintf( fp, "%s|%s|%s|%ld|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%.2lf|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|%ld|%ld|%.2lf|%.2lf|\n",sCom_item.acc_no,sCom_item.acc_name,vgl_sub.cur_no,vgl_sub.date,vgl_sub.dr_bal,vgl_sub.cr_bal,vgl_sub.ldd_bal,vgl_sub.lcd_bal,vgl_sub.rdd_cnt,vgl_sub.rcd_cnt,vgl_sub.rdd_amt,vgl_sub.rcd_amt,vgl_sub.cdd_cnt,vgl_sub.ccd_cnt,vgl_sub.cdd_amt,vgl_sub.ccd_amt,vgl_sub.tddr_bal,vgl_sub.tdcr_bal,vgl_sub.tddr_cnt,vgl_sub.tdcr_cnt,vgl_sub.tddr_amt,vgl_sub.tdcr_amt,vgl_sub.mdr_bal,vgl_sub.mcr_bal,vgl_sub.mdr_cnt,vgl_sub.mcr_cnt,vgl_sub.mdr_amt,vgl_sub.mcr_amt,vgl_sub.qdr_bal,vgl_sub.qcr_bal,vgl_sub.qdr_cnt,vgl_sub.qcr_cnt,vgl_sub.qdr_amt,vgl_sub.qcr_amt,vgl_sub.ydr_bal,vgl_sub.ycr_bal,vgl_sub.ydr_cnt,vgl_sub.ycr_cnt,vgl_sub.ydr_amt,vgl_sub.ycr_amt);
	    	ttlnum++;
	
		}/*2 while*/
		ret=Gl_sub_Clo_Sel();
		if(ret)
		{
			sprintf(acErrMsg,"CLOSE gl_sub错误!![%d]",ret);
			WRITEMSG
			goto ErrExit;	
		}
	}/*1 end of  while*/
	ret=Com_item_Clo_Sel();
	if(ret)
	{
		sprintf(acErrMsg,"CLOSE com_item错误!![%d]",ret);
		WRITEMSG
		goto ErrExit;	
	}
     
	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);

		set_zd_data( DC_FILE_SND,"1" );
	}

GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
