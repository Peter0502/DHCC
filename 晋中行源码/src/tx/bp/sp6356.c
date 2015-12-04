/***************************************************************************
文件名称: sp6356.c                                                      
功能描述: 行内兑付状态本票查询 
作    者：xyy   2008-5-6 15:27
完成日期： 
修改记录： 
日    期: 
修 改 人:
修改内容:                                            
备    注: 此交易没有取消交易                                           
***************************************************************************/
#define EXTERN
#include "stdio.h"
#include "string.h"

#include "stdlib.h"
#include "public.h"
#include "cashier_c.h"
#include "bplosthis_c.h"
#include "lv_define.h"

sp6356()
{
    struct cashier_c  cashier_g;
    int    ret=0;
    int    pkgcnt=0;
    int    rdCnt=0;
    
    long   lSdate=0;  /**起始日期**/
    long   lEdate=0;  /**结束日期**/
    char   cPono[21]; /**本票号码**/
    char   cKinbr[6]; /**行号**/
    char   cTmpstr[201];
    
    char   filename[101];  
    char   tmpstr[61];        
    char   wherelist[501];          /** 查询条件 **/
    FILE   *fp;
      
    /****变量初始化****/
    memset(&cashier_g,  0 , sizeof(struct cashier_c));
    memset(cPono,       0 , sizeof(cPono));
    memset(cKinbr,      0 , sizeof(cKinbr));
    memset(filename,    0 , sizeof(filename));
    memset(tmpstr,      0 , sizeof(tmpstr));
    memset(wherelist,   0 , sizeof(wherelist));
       
    /***初始化全局变量****/
    pub_base_sysinit();
 
    /****接收前台传的值****/
    
    get_zd_long("0440",&lSdate);
    get_zd_long("0450",&lEdate);
    get_zd_data("0620",cPono);
    get_zd_data("0910",cKinbr);
 
    if(lSdate){
        sprintf(tmpstr," cashday>=%ld and ",lSdate); 
        strcat(wherelist,tmpstr);
    }    
    if(lEdate){
        sprintf(tmpstr," cashday<=%ld and ",lEdate); 
        strcat(wherelist,tmpstr);
    }
    if(0!=strlen(cPono)){
        sprintf(tmpstr," pono='0000%s' and",cPono);  /* 20110422 [00000000->0000] */
        strcat(wherelist,tmpstr);
    }
    
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
        sprintf( cTmpstr,"brno='%s' and ",cKinbr);
        strcat(wherelist,cTmpstr);
    }
    
    sprintf(tmpstr," stat in('2','3') and ");
    strcat(wherelist,tmpstr);
    
    sprintf(wherelist+strlen(wherelist), " 1=1");
    vtcp_log("wherelist[%s]\n",  wherelist);
    pkgcnt=sql_count("cashier",wherelist);
    
    if(pkgcnt<=0){
        vtcp_log("[%s][%d]---------未找到符合条件的记录!\n",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"S049");
        goto ErrExit;
    }
    ret=Cashier_Dec_Sel(g_pub_tx.reply,wherelist);
    if(ret){
        vtcp_log("[%s],[%d] 定义游标失败!",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"D101");
        goto ErrExit;
    }
    while(1){
        ret=Cashier_Fet_Sel(&cashier_g,g_pub_tx.reply);
        if(100 == ret) break;
        if(ret){
            vtcp_log("[%s],[%d] 取值失败!",__FILE__,__LINE__);
            strcpy(g_pub_tx.reply,"D103");
            goto ErrExit;
        }
        if(!rdCnt){
            pub_base_AllDwnFilName( filename );
            fp = fopen( filename,"w" );
            if( fp==NULL ){
                sprintf(acErrMsg," open file [%s] error ",filename );
                WRITEMSG
                strcpy( g_pub_tx.reply,"S047" );
                goto ErrExit;
            }
            fprintf( fp, "~@本票号码|机构号码|发起行行号|出票金额|付款人帐号|付款人户名|收款人帐号|收款人户名|兑付行行号|兑付行行名|最后兑付行号|最后兑付行名|最后兑付帐号|最后兑付名字|当前状态|本票类别|签发日期|兑付日期|结清日期|本票到期日期|密押|录入操作员|签发操作员|兑付操作员|\n");
        }
        fprintf( fp, "%-20s|%-8s|%-13s|%15.2f|%-25s|%-40s|%-25s|%-40s|%-13s|%-40s|%-13s|%-40s|%-25s|%-40s|%-2s|%-2s|%8ld|%8ld|%8ld|%8ld|%20s|%4s|%4s|%4s|\n",
cashier_g.pono+4,cashier_g.brno,cashier_g.orbrno,
cashier_g.avbal,cashier_g.payactno,cashier_g.payname,cashier_g.cashactno,cashier_g.cashname,cashier_g.cashbrno,cashier_g.cashbrname,
cashier_g.holdbrno,cashier_g.holdbrname,cashier_g.holdactno,cashier_g.holdname,cashier_g.stat,cashier_g.bptype,cashier_g.signday,
cashier_g.cashday,cashier_g.jqday,cashier_g.endday,cashier_g.passwd,cashier_g.optlrno,cashier_g.chktlrno,cashier_g.cashtlrno);              
        
        rdCnt ++;
        vtcp_log("[%s],[%d],[%d]*****************ZK\n",__FILE__,__LINE__,rdCnt);
    }
    vtcp_log("[%s][%d]查询输出记录完毕!\n",__FILE__,__LINE__);
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
    
    Cashier_Clo_Sel();
                                                         
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
