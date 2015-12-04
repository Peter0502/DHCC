/***************************************************************************
文件名称: 中国法定节假日维护
功能描述: 
作    者：
完成日期： 
修改记录： 
日    期: 
修 改 人:
修改内容:                                            
备    注: 此交易没有取消交易                                           
***************************************************************************/
#define extern
#include "stdio.h"
#include "string.h"

#include "stdlib.h"
#include "public.h"
#include "com_parm_holiday_c.h"
#include "lv_define.h"
#include "lv_pkgreg_c.h"


sp6352()
{
    int      ret=0;
    int      pkgcnt=0;
    int      rdCnt=0;
    
    FILE   *fp;
    
    long     lBegdate =0;  /**开始日期**/
    long     lEnddate =0;  /**终止日期**/
    long     ldays=0;      /**日期天数**/
    
    char     cType[2];
    char     cT[2];
    char     cDatecode[ 4];  /**节假日代码**/
    char     cDatename[11];  /**节假日名称**/
    char     cFilename[101];  
    char     cEffect[7];
    char     cTmpstr[201];
    char     wherelist[501];      /** 查询条件 **/
    struct   com_parm_holiday_c  sCom_holiday;
    
    /****变量初始化****/
    memset(&sCom_holiday, 0 , sizeof(struct com_parm_holiday_c));
    memset(cDatecode    , 0 , sizeof(cDatecode));
    memset(cDatename    , 0 , sizeof(cDatename));
    memset(cFilename    , 0 , sizeof(cFilename));
    memset(cTmpstr      , 0 , sizeof(cTmpstr));
    memset(cEffect      , 0 , sizeof(cEffect));
    memset(wherelist    , 0 , sizeof(wherelist));
    memset(cType        , 0 , sizeof(cType));
    memset(cT           , 0 , sizeof(cT));
       
    /***初始化全局变量****/
    pub_base_sysinit();
 
   /****接收前台传的值****/
    
    get_zd_data("0920",cDatecode);    
    get_zd_data( "0870",cDatename);
    get_zd_long( "0460",&lBegdate);
    get_zd_long( "0470",&lEnddate);
    get_zd_data( "0670",cType);
    get_zd_data( "0680",cT);
    vtcp_log("[%s],[%d] cDatename[%s]\n",__FILE__,__LINE__,cDatename);
    vtcp_log("[%s],[%d] cDatecode[%s]\n",__FILE__,__LINE__,cDatecode);      
    vtcp_log("[%s],[%d] lBegdate [%d]\n",__FILE__,__LINE__,lBegdate );
    vtcp_log("[%s],[%d] lEnddate [%d]\n",__FILE__,__LINE__,lEnddate );
    vtcp_log("[%s],[%d] cType [%s]\n",__FILE__,__LINE__,cType );
    vtcp_log("[%s],[%d] cT [%s]\n",__FILE__,__LINE__,cT );
    /*** cT ：1 增加，2修改***/
    if('1'==cT[0])/*** cT ：1 增加***/
    	{
    		memset(&sCom_holiday , 0 , sizeof(sCom_holiday));
    		
    		strcpy(sCom_holiday.date_code,cDatecode);
    		strcpy(sCom_holiday.date_name,cDatename);
    		sCom_holiday.beg_date=lBegdate;
    		sCom_holiday.end_date=lEnddate;
    		strcpy(sCom_holiday.type,cType);
    		
    		sCom_holiday.days=pub_base_CalTrueDays( lBegdate , lEnddate );
    		
    		
        ret=Com_parm_holiday_Ins(sCom_holiday,g_pub_tx.reply);
        vtcp_log("[%s],[%d],[%d]*****************\n",__FILE__,__LINE__,ret);
        if(ret){
                 vtcp_log("[%s],[%d] 新增失败!",__FILE__,__LINE__);
                 strcpy(g_pub_tx.reply,"D103");
                 goto ErrExit;
               }
        
        vtcp_log("[%s][%d]========程序新增结束",__FILE__,__LINE__);
    	}
    	else 
    if('2'==cT[0])/*** cT ：2修改***/
    	{
    		if(strlen(cDatecode)!=0){
             sprintf(cTmpstr," date_code='%s' and ",cDatecode); 
             strcat(wherelist,cTmpstr);
         }  
         if(strlen(cDatename)!=0){
             sprintf(cTmpstr," date_name='%s' and ",cDatename); 
             strcat(wherelist,cTmpstr);
         }
         if(lBegdate!=0){
             sprintf(cTmpstr," beg_date='%ld' and ",lBegdate); 
             strcat(wherelist,cTmpstr);
         }    
         if(lEnddate!=0){
             sprintf(cTmpstr," end_date='%ld' and ",lEnddate); 
             strcat(wherelist,cTmpstr);
         }
         
         sprintf(wherelist+strlen(wherelist), " 1=1 order by beg_date,end_date");
         vtcp_log("wherelist[%s]\n",  wherelist);
         
    		ret=Com_parm_holiday_Dec_Upd(g_pub_tx.reply,wherelist);
         vtcp_log("[%s],[%d],[%d]*****************\n",__FILE__,__LINE__,ret);
         if(ret){
                  vtcp_log("[%s],[%d] 定义游标失败!",__FILE__,__LINE__);
                  strcpy(g_pub_tx.reply,"D101");
                  goto ErrExit;
                }
        
        memset(&sCom_holiday , 0 , sizeof(sCom_holiday));
        ret=Com_parm_holiday_Fet_Upd(&sCom_holiday,g_pub_tx.reply);
        vtcp_log("[%s],[%d],[%d]*****************\n",__FILE__,__LINE__,ret);
        if(100 == ret) {
                 vtcp_log("[%s],[%d] 未找到纪录!",__FILE__,__LINE__);
                 strcpy(g_pub_tx.reply,"D103");
                 goto ErrExit;
               }
        if(ret){
                 vtcp_log("[%s],[%d] 取值失败!",__FILE__,__LINE__);
                 strcpy(g_pub_tx.reply,"D103");
                 goto ErrExit;
               }
        /***只修改状态 暂时
        strcpy(sCom_holiday.date_code,cDatecode);
    		strcpy(sCom_holiday.date_name,cDatename);
    		sCom_holiday.beg_date=lBegdate;
    		sCom_holiday.end_date=lEnddate;
    		***/
    		strcpy(sCom_holiday.type,cType);
    		
    		sCom_holiday.days=pub_base_CalTrueDays( lBegdate , lEnddate );
        
        Com_parm_holiday_Upd_Upd(sCom_holiday,g_pub_tx.reply);
        if(ret){
                  vtcp_log("[%s],[%d] 更新失败!",__FILE__,__LINE__);
                  strcpy(g_pub_tx.reply,"D101");
                  goto ErrExit;
                }
        Com_parm_holiday_Clo_Upd();
        vtcp_log("[%s][%d]记录修改结束rdCnt=[%d]次!\n",__FILE__,__LINE__,rdCnt);
    	}
    	else
    	{
    	
         if(strlen(cDatecode)!=0){
             sprintf(cTmpstr," date_code='%s' and ",cDatecode); 
             strcat(wherelist,cTmpstr);
         }  
         if(strlen(cDatename)!=0){
             sprintf(cTmpstr," date_name='%s' and ",cDatename); 
             strcat(wherelist,cTmpstr);
         }
         if(lBegdate!=0){
             sprintf(cTmpstr," beg_date>='%ld' and ",lBegdate); 
             strcat(wherelist,cTmpstr);
         }    
         if(lEnddate!=0){
             sprintf(cTmpstr," end_date<='%ld' and ",lEnddate); 
             strcat(wherelist,cTmpstr);
         }
         
         sprintf(wherelist+strlen(wherelist), " 1=1 order by beg_date,end_date");
         vtcp_log("wherelist[%s]\n",  wherelist);
         pkgcnt=sql_count("com_parm_holiday",wherelist);
         if(pkgcnt<=0){
             vtcp_log("[%s][%d]---------未找到符合条件的记录[%d]!\n",__FILE__,__LINE__,pkgcnt);
             strcpy(g_pub_tx.reply,"S049");
             goto ErrExit;
         }
         
         ret=Com_parm_holiday_Dec_Sel(g_pub_tx.reply,wherelist);
         vtcp_log("[%s],[%d],[%d]*****************\n",__FILE__,__LINE__,ret);
         if(ret){
             vtcp_log("[%s],[%d] 定义游标失败!",__FILE__,__LINE__);
             strcpy(g_pub_tx.reply,"D101");
             goto ErrExit;
         }
         while(1){
             memset(&sCom_holiday , 0 , sizeof(sCom_holiday));
             ret=Com_parm_holiday_Fet_Sel(&sCom_holiday,g_pub_tx.reply);
             vtcp_log("[%s],[%d],[%d]*****************\n",__FILE__,__LINE__,ret);
             if(100 == ret) break;
             if(ret){
                 vtcp_log("[%s],[%d] 取值失败!",__FILE__,__LINE__);
                 strcpy(g_pub_tx.reply,"D103");
                 goto ErrExit;
             }
             vtcp_log("[%s],[%d]rdCnt=[%d]\n",__FILE__,__LINE__,rdCnt);
             if(!rdCnt){
                 pub_base_AllDwnFilName( cFilename );
                 vtcp_log("[%s],[%d] cFilename=[%s]\n",__FILE__,__LINE__,cFilename);
                 fp = fopen( cFilename,"w" );
                 if( fp==NULL ){
                      sprintf(acErrMsg," open file [%s] error ",cFilename );
                      WRITEMSG
                      strcpy( g_pub_tx.reply,"S047" );
                      goto ErrExit;
                 }
                 fprintf( fp, "~@节假日代码|节假日名称|起始日期|终止日期|总共天数|是否生效|备注|\n");
             }
             memset(cEffect      , 0 , sizeof(cEffect));
             if(sCom_holiday.type[0]=='0')
             {
                 memcpy(cEffect,"未生效",sizeof(cEffect)-1);
             }
             else if(sCom_holiday.type[0]=='1')
             {
                 memcpy(cEffect,"已生效",sizeof(cEffect)-1);
             }
             else
             {
                 memcpy(cEffect,"错  误",sizeof(cEffect)-1);
             }
             fprintf( fp, "%-3s|%-10s|%-8ld|%-8ld|%-6ld|%-1s|%-60s|%1s|\n",sCom_holiday.date_code,sCom_holiday.date_name,sCom_holiday.beg_date,\
                            sCom_holiday.end_date,sCom_holiday.days,cEffect,sCom_holiday.filler,sCom_holiday.type);
             rdCnt ++;
         }
         vtcp_log("[%s][%d]查询输出记录完毕rdCnt=[%d]!\n",__FILE__,__LINE__,rdCnt);
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
         Com_parm_holiday_Clo_Sel();
         vtcp_log("[%s][%d]========程序查询结束",__FILE__,__LINE__);
       }
    
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
       
