/*************************************************
* 文 件 名:  spE405.c
* 功能描述： 拆借查询
*
* 作    者:  mike
* 完成日期： 2004年3月05日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ass_mst_c.h"
#include "com_branch_c.h"
#include "zjgl_mst_c.h"
		int  ttlnum1=0;			/**读取的总条数**/
		char filename1[100];
		FILE *fp1;
int spE405()
{
    int ret;
    long lpact_no=0;
    struct ass_mst_c ass_mst;
    struct com_branch_c sComBranch;
		char filename[100];
		char wherelist[1024];  /**查询条件**/
		char tmpstr[512];
		char cSign[5];
		char prt_flag[2];
		char cBr_no[6];
		char cStat[9];
		int ttlnum=0;			/**读取的总条数**/
		FILE *fp;
    memset(&ass_mst,0x0,sizeof(struct ass_mst_c));
    memset(&sComBranch,0x0,sizeof(struct com_branch_c));
		memset( wherelist,0x0,sizeof(wherelist) );
		memset( tmpstr,0x0,sizeof(tmpstr) );
		memset( prt_flag,0x0,sizeof(prt_flag) );
		memset( cBr_no,0x0,sizeof(cBr_no) );
		memset( filename,0x0,sizeof(filename) );
		memset(cSign,0x0,sizeof(cSign));
		memset(cStat,0x0,sizeof(cStat));
    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_long("0470",&lpact_no);
    get_zd_data("0670",prt_flag);
    get_zd_data("0910",cBr_no);
	  vtcp_log("开始呵呵呵![%s][%s]\n",prt_flag,cBr_no);

 if(prt_flag[0]!='1')
 {  
  	  vtcp_log("开始呵呵呵![%s][%s]\n",prt_flag,cBr_no);

    if(lpact_no)
    {
    			sprintf( tmpstr," pact_no=%ld and",lpact_no );
					strcat( wherelist,tmpstr );

    }
    /**Add by hxc 091123 晋中商行要求只允许各机构只查询本机构，清算中心(10001)可查询所有信息 Begin**/
     if( strlen(cBr_no) && memcmp(cBr_no,"10001",5))
   {
    			sprintf( tmpstr," br_no='%s' and",cBr_no );
					strcat( wherelist,tmpstr );
   }
   /**Add by hxc 091123 晋中商行要求只允许各机构只查询本机构，清算中心(10001)可查询所有信息 End**/
    strcat(wherelist," 1=1 ORDER BY br_no,pact_no") ;
    ret=Ass_mst_Dec_Sel(g_pub_tx.reply,wherelist);
    ERR_DEAL  
    ttlnum=0;
    while(1)
		{
				memset(cSign,0x0,sizeof(cSign));
				memset(cStat,0x0,sizeof(cStat));	
				memset(&ass_mst,0x0,sizeof(struct ass_mst_c));
				ret=Ass_mst_Fet_Sel(&ass_mst, g_pub_tx.reply);
				if( ret==100 ) break;
				ERR_DEAL
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
					fprintf( fp,"~合同号|拆借机构|标　　志|$余　　额|$昨日余额|@起　息　日|上笔发生日|到　期　日|@利　　率|@逾期利率|@已结利息|@积  数|@状　　态|\n" );
				}
			   	if(ass_mst.sign[0]=='1')
			   	{
			   		memcpy(cSign,"拆出",4);
			   	}
			   	else
			   	{
			   		memcpy(cSign,"上存",4);
			   	}
			   	if(ass_mst.sts[0]=='*')
			   	{
			   		memcpy(cStat,"已经结清",8);
			   	}
			   	else
			   	{
			   		memcpy(cStat,"尚未结清",8);
			   	}
			   	
			   	fprintf(fp, "%ld|%s|%s|%.2lf|%.2lf|%ld|%ld|%ld|%.4f|%.4f|%.2lf|%.2lf|%s|\n",ass_mst.pact_no,ass_mst.sub_br_no,cSign,ass_mst.bal,ass_mst.ys_bal,ass_mst.ic_date,ass_mst.lst_date,ass_mst.end_date,ass_mst.rate,ass_mst.over_rate,ass_mst.intst,ass_mst.intst_acm,cStat);
		ttlnum++;
   
	}
	ret=Ass_mst_Clo_Sel();
	ERR_DEAL

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
 } 
 else /***开始打印***/
 {
	vtcp_log("开始呵呵呵!\n");
	ret=prt_dzd(cBr_no,lpact_no);
	if( !ttlnum1 )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
	 		fclose(fp1);
  		set_zd_data( DC_FILE_SND,"1" );
	}
 } 
  /*
    ret = Ass_mst_Sel(g_pub_tx.reply,&ass_mst,"pact_no=%ld",ass_mst.pact_no);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"查询考核主文件错误!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O104");
        goto ErrExit;
    }   
    else if (ret == 100)
    {
        sprintf(acErrMsg,"没有该合同号的系统内部拆借!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O111");
        goto ErrExit;
    }
    if (ass_mst.sts[0] == '*')
    {
        sprintf(acErrMsg,"该合同号的内部拆借已经结清!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O112");
        goto ErrExit;
    }
   
    ret=Com_branch_Sel( g_pub_tx.reply , &sComBranch ,
             "br_no='%s'" , ass_mst.sub_br_no );
    if( ret != 0 )
    {
        sprintf( acErrMsg, "取公共机构码表错!\
            br_no=[%s]", g_pub_tx.tx_br_no );
        WRITEMSG
        strcpy( g_pub_tx.reply, "S019" );
        goto    ErrExit;
    }


    set_zd_data("0910",ass_mst.sub_br_no);
    set_zd_data("0250",sComBranch.br_name);
    set_zd_double("0390",ass_mst.bal);
    set_zd_long("0450",ass_mst.end_date);
    set_zd_data("0380",ass_mst.br_no);
    if (ass_mst.sign[0] == '1')
    {
        set_zd_data("0370","拆出");
    }
    else
    {
        set_zd_data("0370","上存");
    }
    set_zd_data("0670",ass_mst.sign);
    set_zd_long("0440",ass_mst.ic_date);
    sprintf(acErrMsg,"考核机构=[%s]",ass_mst.br_no);
    WRITEMSG
   */
OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
int prt_dzd(char * br_no, long pact_no)
{
    struct ass_mst_c	 s_ass_mst;
    struct zjgl_mst_c	 s_zjgl_mst;
  	char wherelist[1024];  /**查询条件**/
		char tmpstr[512];
  	char wherelist1[1024];  /**查询条件**/
		char tmpstr1[512];
		int ret=0;
		char tmp_bal[21];
    memset(&s_ass_mst,0x00,sizeof(s_ass_mst));
    memset(&wherelist,0x00,sizeof(wherelist));
    memset(&tmpstr,0x00,sizeof(tmpstr));
    memset(&tmp_bal,0x00,sizeof(tmp_bal));
    memset(&wherelist1,0x00,sizeof(wherelist1));
    memset(&tmpstr1,0x00,sizeof(tmpstr1));
    memset(&s_zjgl_mst,0x00,sizeof(s_zjgl_mst));
	  vtcp_log("jinrule!\n");
	 if(pact_no)
   {
    			sprintf( tmpstr," pact_no=%ld and",pact_no );
					strcat( wherelist,tmpstr );
   }
   pub_base_strpack(br_no);
   if( strlen(br_no) )
   {
    			sprintf( tmpstr," br_no='%s' and",br_no );
					strcat( wherelist,tmpstr );
   }
    strcat(wherelist," sts!='*' and 1=1 ORDER BY br_no,pact_no") ;
	  ret=Ass_mst_Dec_Sel(g_pub_tx.reply,wherelist);
    ERR_DEAL  
    ttlnum1=0;
    while(1)
		{
			  vtcp_log("开始查询了!\n");
				memset(&s_ass_mst,0x0,sizeof(struct ass_mst_c));
				ret=Ass_mst_Fet_Sel(&s_ass_mst, g_pub_tx.reply);
				if( ret==100 )
				{
	 		       fclose(fp1);
				     break;
			  }
				ERR_DEAL
				if( !ttlnum1 )
				{
					pub_base_AllDwnFilName( filename1 );
					fp1 = fopen( filename1,"w" );
					if( fp1==NULL )
					{
						sprintf(acErrMsg," open file [%s] error ",filename1 );
						WRITEMSG
						strcpy( g_pub_tx.reply,"S047" );
						goto ErrExit;
					}
					/**显示列标题**/
				}
          memset(&tmp_bal,0x00,sizeof(tmp_bal));
					sprintf(tmp_bal,"%.2lf",s_ass_mst.bal);
				  pub_rpt_flttomon(tmp_bal,tmp_bal);
				  char dc_ind[3];
				  memset(dc_ind,0x00,sizeof(dc_ind));
				  if(s_ass_mst.sign[0]=='1')
				  {
				  	strcpy(dc_ind,"借");
				  }
				  else
				  {
				  	strcpy(dc_ind,"贷");
				  }
          /*fprintf( fp1,"^[@^[3^P^\v1\n");*/
          fprintf( fp1," \n\n\n" );
          fprintf( fp1,"                                                       晋中银行对帐单\n" );
          fprintf( fp1,"            合同号: %ld                                                                                                  \n",s_ass_mst.pact_no );
          fprintf( fp1,"            机构号: %5s                                                                        打印日期: %8ld            \n",s_ass_mst.br_no,g_pub_tx.tx_date);
          fprintf( fp1,"           ┍━━━━┯━━━┯━━━━━━━━━━━━━━┯━━━━━┯━━━━┯━━━┯━━━━━━━━━━━━━━┑\n" );
          fprintf( fp1,"           │ 日   期│ 借贷 │    银  行  帐  余  额      │   未达   │ 日   期│ 借贷 │    银  行  帐  余  额      │\n" );
          fprintf( fp1,"           ┝━━━━┿━━━┿━━━━━━━━━━━━━━┥          ┝━━━━┿━━━┿━━━━━━━━━━━━━━┥\n" );
          fprintf( fp1,"           │%8ld│ %2s   │       %21s│   帐项   │        │      │                            │\n",s_ass_mst.lst_date,dc_ind,tmp_bal);
          fprintf( fp1,"           ┝━━━━┿━━━┿━━━━━━━━━━━━━━┿━━┯━━┿━━━━┿━━━┿━━━━━━━━━━━━━━┥\n" );
          fprintf( fp1,"           │        │      │                            │ 银 │ 企 │        │      │                            │\n" );
          fprintf( fp1,"           ┝━━━━┿━━━┿━━━━━━━━━━━━━━┥ 行 │ 业 ┝━━━━┿━━━┿━━━━━━━━━━━━━━┥\n" );
          fprintf( fp1,"           │        │      │                            │ 未 │ 未 │        │      │                            │\n" );
          fprintf( fp1,"           ┝━━━━┿━━━┿━━━━━━━━━━━━━━┥ 达 │ 达 ┝━━━━┿━━━┿━━━━━━━━━━━━━━┥\n" );
          fprintf( fp1,"           │        │      │                            │ 帐 │ 帐 │        │      │                            │\n" );
          fprintf( fp1,"           ┝━━━━┿━━━┿━━━━━━━━━━━━━━┥ 项 │ 项 ┝━━━━┿━━━┿━━━━━━━━━━━━━━┥\n" );
          fprintf( fp1,"           │        │      │                            │    │    │        │      │                            │\n" );
      /*  fprintf( fp1,"           ┝━━━━┿━━━┿━━━━━━━━━━━━━━┥    │    ┝━━━━┿━━━┿━━━━━━━━━━━━━━┥\n" );
          fprintf( fp1,"           │        │      │                            │    │    │        │      │                            │\n" );
          fprintf( fp1,"           ┝━━━━┿━━━┿━━━━━━━━━━━━━━┥    │    ┝━━━━┿━━━┿━━━━━━━━━━━━━━┥\n" );
          fprintf( fp1,"           │        │      │                            │    │    │        │      │                            │\n" );*/
          fprintf( fp1,"           ┝━━━━┿━━━┿━━━━━━━━━━━━━━┿━━┷━━┿━━━━┿━━━┿━━━━━━━━━━━━━━┥\n" );
          fprintf( fp1,"           │调整以后│      │                            │          │调整以后│      │                            │\n" );
          fprintf( fp1,"           ┕━━━━┷━━━┷━━━━━━━━━━━━━━┷━━━━━┷━━━━┷━━━┷━━━━━━━━━━━━━━┙\n" );
          fprintf( fp1,"                                                                                                                         \n" );
          fprintf( fp1,"             1.请企业认真核对银行帐，并于七日内将此对账单第一联盖章后送回晋中市        ┌┄┄┄┄┄┄┄┄┄┄┄┄┐      \n" );
          fprintf( fp1,"                                                                                       ┆                        ┆      \n" );
          fprintf( fp1,"               商业银行。                                                              ┆                        ┆      \n" );
          fprintf( fp1,"                                                                                       ┆     企  业  签  章     ┆      \n" );
          fprintf( fp1,"                                                                                       ┆                        ┆      \n" );
          fprintf( fp1,"                                                                                       ┆                        ┆      \n" );
          fprintf( fp1,"                                                                                       └┄┄┄┄┄┄┄┄┄┄┄┄┘      \n" );
          fprintf( fp1,"                                                                                                                          \n" );       
          fprintf( fp1," \n" );
   		    ttlnum1++;
          if(ttlnum1%2==0)      
          fprintf( fp1," \f" );
	}
	ret=Ass_mst_Clo_Sel();
   pub_base_strpack(br_no);
   if( strlen(br_no) )
   {
    			sprintf( tmpstr1," opn_br_no='%s' and",br_no);
					strcat( wherelist1,tmpstr1 );
   }
    strcat(wherelist1," substr(acc_no,1,3)='407' and 1=1 ORDER BY opn_br_no,acc_no") ;
	  ret=Zjgl_mst_Dec_Sel(g_pub_tx.reply,wherelist1);
    ERR_DEAL  
    ttlnum1=0;
    while(1)
		{
			  vtcp_log("开始查询了!\n");
				memset(&s_zjgl_mst,0x0,sizeof(struct zjgl_mst_c));
				ret=Zjgl_mst_Fet_Sel(&s_zjgl_mst, g_pub_tx.reply);
				if( ret==100 )
				{
	 		       fclose(fp1);
				     break;
			  }
				ERR_DEAL
				if( !ttlnum1 )
				{
					pub_base_AllDwnFilName( filename1 );
					fp1 = fopen( filename1,"a+" );
					if( fp1==NULL )
					{
						sprintf(acErrMsg," open file [%s] error ",filename1 );
						WRITEMSG
						strcpy( g_pub_tx.reply,"S047" );
						goto ErrExit;
					}
					/**显示列标题**/
				}
           memset(&tmp_bal,0x00,sizeof(tmp_bal));
					sprintf(tmp_bal,"%.2lf",s_zjgl_mst.dr_bal);
				  pub_rpt_flttomon(tmp_bal,tmp_bal);

          /*fprintf( fp1,"^[@^[3^P^\v1\n");*/
          fprintf( fp1," \n\n\n" );
          fprintf( fp1,"                                                          晋中银行对帐单\n" );
          fprintf( fp1,"            科目号: %7s                                                                                                  \n",s_zjgl_mst.acc_no );
          fprintf( fp1,"            机构号: %5s                                                                        打印日期: %8ld            \n",s_zjgl_mst.opn_br_no,g_pub_tx.tx_date);
          fprintf( fp1,"           ┍━━━━┯━━━┯━━━━━━━━━━━━━━┯━━━━━┯━━━━┯━━━┯━━━━━━━━━━━━━━┑\n" );
          fprintf( fp1,"           │ 日   期│ 借贷 │    银  行  帐  余  额      │   未达   │ 日   期│ 借贷 │    银  行  帐  余  额      │\n" );
          fprintf( fp1,"           ┝━━━━┿━━━┿━━━━━━━━━━━━━━┥          ┝━━━━┿━━━┿━━━━━━━━━━━━━━┥\n" );
          fprintf( fp1,"           │%8ld│ 借   │       %21s│   帐项   │        │      │                            │\n",s_zjgl_mst.lst_date,tmp_bal);
          fprintf( fp1,"           ┝━━━━┿━━━┿━━━━━━━━━━━━━━┿━━┯━━┿━━━━┿━━━┿━━━━━━━━━━━━━━┥\n" );
          fprintf( fp1,"           │        │      │                            │ 银 │ 企 │        │      │                            │\n" );
          fprintf( fp1,"           ┝━━━━┿━━━┿━━━━━━━━━━━━━━┥ 行 │ 业 ┝━━━━┿━━━┿━━━━━━━━━━━━━━┥\n" );
          fprintf( fp1,"           │        │      │                            │ 未 │ 未 │        │      │                            │\n" );
          fprintf( fp1,"           ┝━━━━┿━━━┿━━━━━━━━━━━━━━┥ 达 │ 达 ┝━━━━┿━━━┿━━━━━━━━━━━━━━┥\n" );
          fprintf( fp1,"           │        │      │                            │ 帐 │ 帐 │        │      │                            │\n" );
          fprintf( fp1,"           ┝━━━━┿━━━┿━━━━━━━━━━━━━━┥ 项 │ 项 ┝━━━━┿━━━┿━━━━━━━━━━━━━━┥\n" );
          fprintf( fp1,"           │        │      │                            │    │    │        │      │                            │\n" );
        /*fprintf( fp1,"           ┝━━━━┿━━━┿━━━━━━━━━━━━━━┥    │    ┝━━━━┿━━━┿━━━━━━━━━━━━━━┥\n" );
          fprintf( fp1,"           │        │      │                            │    │    │        │      │                            │\n" );
          fprintf( fp1,"           ┝━━━━┿━━━┿━━━━━━━━━━━━━━┥    │    ┝━━━━┿━━━┿━━━━━━━━━━━━━━┥\n" );
          fprintf( fp1,"           │        │      │                            │    │    │        │      │                            │\n" );*/
          fprintf( fp1,"           ┝━━━━┿━━━┿━━━━━━━━━━━━━━┿━━┷━━┿━━━━┿━━━┿━━━━━━━━━━━━━━┥\n" );
          fprintf( fp1,"           │调整以后│      │                            │          │调整以后│      │                            │\n" );
          fprintf( fp1,"           ┕━━━━┷━━━┷━━━━━━━━━━━━━━┷━━━━━┷━━━━┷━━━┷━━━━━━━━━━━━━━┙\n" );
          fprintf( fp1,"                                                                                                                         \n" );
          fprintf( fp1,"             1.请企业认真核对银行帐，并于七日内将此对账单第一联盖章后送回晋中市        ┌┄┄┄┄┄┄┄┄┄┄┄┄┐      \n" );
          fprintf( fp1,"                                                                                       ┆                        ┆      \n" );
          fprintf( fp1,"               商业银行。                                                              ┆                        ┆      \n" );
          fprintf( fp1,"                                                                                       ┆     企  业  签  章     ┆      \n" );
          fprintf( fp1,"                                                                                       ┆                        ┆      \n" );
          fprintf( fp1,"                                                                                       ┆                        ┆      \n" );
          fprintf( fp1,"                                                                                       └┄┄┄┄┄┄┄┄┄┄┄┄┘      \n" );
          fprintf( fp1,"                                                                                                                         \n" );       
          fprintf( fp1," \n" );
   		    ttlnum1++;
          if(ttlnum1%2==0)      
          fprintf( fp1," \f" );
	}
	ret=Zjgl_mst_Clo_Sel();
OkExit:	
	  return 0;
ErrExit:
    return 1;
}
