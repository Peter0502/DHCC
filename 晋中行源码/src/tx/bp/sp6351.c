/***************************************************************************
文件名称: sp6351.c                                                      
功能描述: 银行本票小额支付信息查询 
作    者：
完成日期： 
修改记录： 
日    期: 
修 改 人:
修改内容:                                            
备    注:                                          
***************************************************************************/
#include "stdio.h"
#include "string.h"

#include "stdlib.h"
#include "public.h"
#include "lv_pkgreg_c.h"
#include "lv_param_c.h"
#include "bplosthis_c.h"
#include "lv_define.h"

sp6351()
{
    struct lv_pkgreg_c lv_pkgreg;
    struct lv_param_c  lv_param;
    int    ret=0;
    int    ipkgcnt=0;
    int    rdCnt=0;
    
    long   lSdate=0;         /**起始日期**/
    long   lEdate=0;         /**结束日期**/
    
    char   Parname[31];     
    char   clwflag[6];       /**来往标识**/
    char   cLvstat[30];      /**交易状态**/
    char   cHzstat[32];      /**回执状态**/
    char   cKinbr[BRNO_LEN+1];
    char   filename[101];  
    char   cTmpstr[201];
    char   cPkg_ind[2];
    char   cLw_ind[2];
    char   wherelist[501];   /**查询条件**/
    FILE   *fp;
      
    /****初始化变量****/
    memset(&lv_param  , 0 , sizeof(struct lv_param_c));
    memset(&lv_pkgreg , 0 , sizeof(struct lv_pkgreg_c));
    memset(filename   , 0 , sizeof(filename));
    memset(cTmpstr    , 0 , sizeof(cTmpstr));
    memset(cKinbr     , 0 , sizeof(cKinbr));
    memset(clwflag    , 0 , sizeof(clwflag));
    memset(cLvstat    , 0 , sizeof(cLvstat));
    memset(cHzstat    , 0 , sizeof(cHzstat));
    memset(wherelist  , 0 , sizeof(wherelist));
    memset(Parname  , 0 , sizeof(Parname));
    memset(cPkg_ind  , 0 , sizeof(cPkg_ind));
    memset(cLw_ind   , 0 , sizeof(cLw_ind));
       
    /***初始化全局变量****/
    pub_base_sysinit();
   /****接收前台传的值****/
    vtcp_log("[%s][%d]sp6351开始啦\n",__FILE__,__LINE__);
    get_zd_long("0440",&lSdate);
    vtcp_log("[%s],[%d]起始日期:[%d]\n",__FILE__,__LINE__,lSdate);
    get_zd_long( "0450",&lEdate);
    vtcp_log("[%s],[%d]截至日期:[%d]\n",__FILE__,__LINE__,lEdate);
    get_zd_data("0910",cKinbr);
    vtcp_log("[%s][%d]交易机构为[%s]\n",__FILE__,__LINE__,cKinbr);
    get_zd_data("0720",cPkg_ind);
    vtcp_log("[%s][%d]业务类型[%s]\n",__FILE__,__LINE__,cPkg_ind);
    get_zd_data("0710",cLw_ind);
    vtcp_log("[%s][%d]来往标志[%s]\n",__FILE__,__LINE__,cLw_ind);
    if(memcmp(g_pub_tx.tx_br_no,LV_CZ_QSBRNO,5)!=0 )
    {
    	  if(strlen(cKinbr)==0)
    	  {
            vtcp_log("[%s][%d][%s]非清算中心查询交易机构不能为空!\n",__FILE__,__LINE__,g_pub_tx.tx_br_no);
            strcpy(g_pub_tx.reply,"P075");
            goto ErrExit;
        }
        else if(memcmp(g_pub_tx.tx_br_no,cKinbr,5)!=0)
        {
            vtcp_log("[%s][%d][%s]清算中心,交易机构[%.5s]!\n",__FILE__,__LINE__,LV_CZ_QSBRNO,g_pub_tx.tx_br_no);
            strcpy(g_pub_tx.reply,"P075");
            goto ErrExit;
        }
    }
    
    /***行号***/
    memset(cTmpstr    , 0 , sizeof(cTmpstr));
    if(cKinbr[0]!=0x00)
    {
        sprintf( cTmpstr,"br_no='%s' and ",cKinbr);
        strcat(wherelist,cTmpstr);
    }
    /**来往标志**/
    memset(cTmpstr    , 0 , sizeof(cTmpstr));
    if(cLw_ind[0]!='0')
    {
    	  sprintf( cTmpstr,"lw_ind='%s' and  ",cLw_ind);
        strcat(wherelist,cTmpstr);
    }
    /**业务类型**/
    memset(cTmpstr    , 0 , sizeof(cTmpstr));
    if(cPkg_ind[0]!='0')
    {
    	  if(cPkg_ind[0]=='1')
    	  {
    	      sprintf( cTmpstr,"pkgno='004' and ");
    	  }
    	  else
    	  {
    	  	  sprintf( cTmpstr,"pkgno='010' and ");
    	  }
        strcat(wherelist,cTmpstr);
    }
    else
    {
    	  sprintf( cTmpstr,"pkgno in ('004','010') and  ");
        strcat(wherelist,cTmpstr);
    }
    memset(cTmpstr    , 0 , sizeof(cTmpstr));

    sprintf(cTmpstr," wt_date>=%ld and wt_date<=%ld  and txnum='30103' and ",lSdate,lEdate);

    strcat(wherelist,cTmpstr);
    sprintf(wherelist+strlen(wherelist), " 1=1 order by wt_date,orderno,packid");
    vtcp_log("[%s][%d]wherelist[%s]\n",__FILE__,__LINE__,wherelist);
    ipkgcnt=sql_count("lv_pkgreg",wherelist);
    
    if(ipkgcnt<=0)
    {
        vtcp_log("[%s][%d]---------未找到符合条件的记录!\n",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"S049");
        goto ErrExit;
    }
    
    ret=Lv_pkgreg_Dec_Sel(g_pub_tx.reply,wherelist);
    vtcp_log("[%s],[%d],[%d]*****************\n",__FILE__,__LINE__,ret);
    if(ret)
    {
        vtcp_log("[%s],[%d] 定义游标失败!",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"D101");
        goto ErrExit;
    }
    while(1)
    {
    	memset(&lv_param  , 0 , sizeof(struct lv_param_c));
    	memset(cHzstat    , 0 , sizeof(cHzstat));
    	memset(Parname  , 0 , sizeof(Parname));
    	memset(clwflag    , 0 , sizeof(clwflag));
    	memset(cLvstat    , 0 , sizeof(cLvstat));
        strcpy(cHzstat,"TPLY");
        vtcp_log("[%s],[%d]*****************\n",__FILE__,__LINE__);
        ret=Lv_pkgreg_Fet_Sel(&lv_pkgreg,g_pub_tx.reply);
        vtcp_log("[%s],[%d],[%d]*****************\n",__FILE__,__LINE__,ret);
        if(100 == ret) break;
        if(ret){
            vtcp_log("[%s],[%d] 取值失败!",__FILE__,__LINE__);
            strcpy(g_pub_tx.reply,"D103");
            goto ErrExit;
        }
        vtcp_log("[%s],[%d],[%d]*****************\n",__FILE__,__LINE__,rdCnt);
        if(!rdCnt)
        {
            pub_base_AllDwnFilName( filename );
            fp = fopen( filename,"w" );
            if( fp==NULL ){
                sprintf(acErrMsg," open file [%s] error ",filename );
                WRITEMSG
                strcpy( g_pub_tx.reply,"S047" );
                goto ErrExit;
            }
            fprintf( fp, "~@pkg编号|发起行行号|交易序号|接收行行号|来包日期|委托日期|打包日期|交易金额|收款人帐号|收款人名称|应答日期|回执状态|交易机构|交易状态|包序号|来往标识|\n");
        }
        switch(lv_pkgreg.lv_sts[0])
        {
            case '0':
                  strcpy(cLvstat,"无效状态");
                  break;
            case '1':
                  strcpy(cLvstat,"往账录入");
                  break;
            case '2':
                  strcpy(cLvstat,"往账复核，挂帐");
                  break;
            case '3':
                  strcpy(cLvstat,"往账复核，记帐");
                  break;
            case '4':
                  strcpy(cLvstat,"往账发送");
                  break;
            case '5':
                  strcpy(cLvstat,"往账拒绝");
                  break;
            case '6':
                  strcpy(cLvstat,"往账扎差");
                  break;
            case '7':
                  strcpy(cLvstat,"往账排队");
                  break;
            case '8':
                  strcpy(cLvstat,"往账撤销等待");
                  break;
            case '9':
                  strcpy(cLvstat,"往账撤销");
                  break;
            case 'A':
                  strcpy(cLvstat,"往账退回、冲正、止付等待");
                  break;
            case 'B':
                  strcpy(cLvstat,"往账退回");
                  break;
            case 'C':
                  strcpy(cLvstat,"往账已冲正");
                  break;
            case 'D':
                  strcpy(cLvstat,"往账已止付");
                  break;
            case 'G':
                  strcpy(cLvstat,"往账异常挂帐");
                  break;
            case 'H':
                  strcpy(cLvstat,"临时接收,不记帐");
                  break;
            case 'I':
                  strcpy(cLvstat,"正式接收，正常记帐");
                  break;
            case 'J':
                  strcpy(cLvstat,"全国押错 -挂支行");
                  break;
            case 'K':
                  strcpy(cLvstat,"地方押错 -挂支行");
                  break;
            case 'L':
                  strcpy(cLvstat,"全国押和地方押都错");
                  break;
            case 'M':
                  strcpy(cLvstat,"信息不符，清算中心挂帐");
                  break;
            case 'N':
                  strcpy(cLvstat,"信息不符，支行挂帐");
                  break;      
            case 'O':
                  strcpy(cLvstat,"来账退回等待");
                  break;
            case 'P':
                  strcpy(cLvstat,"来账退回");
                  break;
            case 'Q':
                  strcpy(cLvstat,"已做同意应答且未发送");
                  break;
            case 'R':
                  strcpy(cLvstat,"已做同意应答且已发送");
                  break;
            case 'S':
                  strcpy(cLvstat,"已做不同意应答且未发送");
                  break;
            case 'T':
                  strcpy(cLvstat,"已做不同意应答且已发送");
                  break;
            case 'U':
                  strcpy(cLvstat,"来账已冲正");
                  break; 
            case 'V':
                  strcpy(cLvstat,"来账已止付");
                  break;
            case 'W':
                  strcpy(cLvstat,"来账已付款");
                  break;
            case 'Z':
                  strcpy(cLvstat,"来账异常挂帐");
                  break;
            default:
                  vtcp_log("难道出去了");
                  break;
        }
       
        if(lv_pkgreg.lw_ind[0]=='1')
        {
            strcpy(clwflag,"往账"); 
        }    
        if(lv_pkgreg.lw_ind[0]=='2')
        {
            strcpy(clwflag,"来账"); 
        }    
        vtcp_log("[%s][%d]lv_pkgreg.rcpstat=[%s]\n",__FILE__,__LINE__,lv_pkgreg.rcpstat);
        
        strcat(cHzstat,lv_pkgreg.rcpstat);
        ret=Lv_param_Sel(g_pub_tx.reply,&lv_param,"par_code='%s'",cHzstat);
        
        if (ret==100)
        {
            
            vtcp_log("[%s],[%d] 无回执状态!",__FILE__,__LINE__);
            strcpy(Parname,"无回执状态");
            
        }
        else if (ret)
        {
            vtcp_log("[%s],[%d] 查询Lv_param失败!",__FILE__,__LINE__);
            strcpy(g_pub_tx.reply,"D101");
            goto ErrExit;
        }
        else if (ret==0)
        {
        	strcpy(Parname,lv_param.par_name);
        	
        }
        fprintf( fp, "%-3s|%-13s|%-8s|%-13s|%-8ld|%-8ld|%-8ld|%.2f|%-20s|%-32s|%-8ld|%-30s|%-7s|%-30s|%-8s|%-6s|\n",
        lv_pkgreg.pkgno,
        lv_pkgreg.or_br_no,
        lv_pkgreg.orderno,
        lv_pkgreg.ac_br_no,  
        lv_pkgreg.in_date,
        lv_pkgreg.wt_date,
        lv_pkgreg.pack_date,
        lv_pkgreg.tx_amt,
        lv_pkgreg.cash_ac_no,
        lv_pkgreg.cash_name,
        lv_pkgreg.resp_date,
        Parname,
        lv_pkgreg.br_no,
        cLvstat,
        lv_pkgreg.packid,
        clwflag);           
      
        rdCnt ++;
        vtcp_log("[%s],[%d],[%d]*****************\n",__FILE__,__LINE__,rdCnt);
        
    }
    vtcp_log("[%s][%d]查询记录完毕!\n",__FILE__,__LINE__);
    if( !rdCnt )
    {
        strcpy( g_pub_tx.reply,"S049" );
        goto ErrExit;
    }
    else
    {
        fclose(fp);
        set_zd_data( DC_FILE_SND,"1" );
    }
    
    Lv_pkgreg_Clo_Sel();
                                                           
    vtcp_log("[%s][%d]========程序结束",__FILE__,__LINE__);
    
OkExit:
        strcpy(g_pub_tx.reply,"0000");
        sprintf(acErrMsg,"Before OK return: reply is[%s]\n",g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        return 0;
ErrExit:
        sprintf(acErrMsg,"errcode: reply is[%s]\n",g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        return 1;
}
