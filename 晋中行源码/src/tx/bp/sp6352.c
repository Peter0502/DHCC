/***************************************************************************
�ļ�����: �й������ڼ���ά��
��������: 
��    �ߣ�
������ڣ� 
�޸ļ�¼�� 
��    ��: 
�� �� ��:
�޸�����:                                            
��    ע: �˽���û��ȡ������                                           
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
    
    long     lBegdate =0;  /**��ʼ����**/
    long     lEnddate =0;  /**��ֹ����**/
    long     ldays=0;      /**��������**/
    
    char     cType[2];
    char     cT[2];
    char     cDatecode[ 4];  /**�ڼ��մ���**/
    char     cDatename[11];  /**�ڼ�������**/
    char     cFilename[101];  
    char     cEffect[7];
    char     cTmpstr[201];
    char     wherelist[501];      /** ��ѯ���� **/
    struct   com_parm_holiday_c  sCom_holiday;
    
    /****������ʼ��****/
    memset(&sCom_holiday, 0 , sizeof(struct com_parm_holiday_c));
    memset(cDatecode    , 0 , sizeof(cDatecode));
    memset(cDatename    , 0 , sizeof(cDatename));
    memset(cFilename    , 0 , sizeof(cFilename));
    memset(cTmpstr      , 0 , sizeof(cTmpstr));
    memset(cEffect      , 0 , sizeof(cEffect));
    memset(wherelist    , 0 , sizeof(wherelist));
    memset(cType        , 0 , sizeof(cType));
    memset(cT           , 0 , sizeof(cT));
       
    /***��ʼ��ȫ�ֱ���****/
    pub_base_sysinit();
 
   /****����ǰ̨����ֵ****/
    
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
    /*** cT ��1 ���ӣ�2�޸�***/
    if('1'==cT[0])/*** cT ��1 ����***/
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
                 vtcp_log("[%s],[%d] ����ʧ��!",__FILE__,__LINE__);
                 strcpy(g_pub_tx.reply,"D103");
                 goto ErrExit;
               }
        
        vtcp_log("[%s][%d]========������������",__FILE__,__LINE__);
    	}
    	else 
    if('2'==cT[0])/*** cT ��2�޸�***/
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
                  vtcp_log("[%s],[%d] �����α�ʧ��!",__FILE__,__LINE__);
                  strcpy(g_pub_tx.reply,"D101");
                  goto ErrExit;
                }
        
        memset(&sCom_holiday , 0 , sizeof(sCom_holiday));
        ret=Com_parm_holiday_Fet_Upd(&sCom_holiday,g_pub_tx.reply);
        vtcp_log("[%s],[%d],[%d]*****************\n",__FILE__,__LINE__,ret);
        if(100 == ret) {
                 vtcp_log("[%s],[%d] δ�ҵ���¼!",__FILE__,__LINE__);
                 strcpy(g_pub_tx.reply,"D103");
                 goto ErrExit;
               }
        if(ret){
                 vtcp_log("[%s],[%d] ȡֵʧ��!",__FILE__,__LINE__);
                 strcpy(g_pub_tx.reply,"D103");
                 goto ErrExit;
               }
        /***ֻ�޸�״̬ ��ʱ
        strcpy(sCom_holiday.date_code,cDatecode);
    		strcpy(sCom_holiday.date_name,cDatename);
    		sCom_holiday.beg_date=lBegdate;
    		sCom_holiday.end_date=lEnddate;
    		***/
    		strcpy(sCom_holiday.type,cType);
    		
    		sCom_holiday.days=pub_base_CalTrueDays( lBegdate , lEnddate );
        
        Com_parm_holiday_Upd_Upd(sCom_holiday,g_pub_tx.reply);
        if(ret){
                  vtcp_log("[%s],[%d] ����ʧ��!",__FILE__,__LINE__);
                  strcpy(g_pub_tx.reply,"D101");
                  goto ErrExit;
                }
        Com_parm_holiday_Clo_Upd();
        vtcp_log("[%s][%d]��¼�޸Ľ���rdCnt=[%d]��!\n",__FILE__,__LINE__,rdCnt);
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
             vtcp_log("[%s][%d]---------δ�ҵ����������ļ�¼[%d]!\n",__FILE__,__LINE__,pkgcnt);
             strcpy(g_pub_tx.reply,"S049");
             goto ErrExit;
         }
         
         ret=Com_parm_holiday_Dec_Sel(g_pub_tx.reply,wherelist);
         vtcp_log("[%s],[%d],[%d]*****************\n",__FILE__,__LINE__,ret);
         if(ret){
             vtcp_log("[%s],[%d] �����α�ʧ��!",__FILE__,__LINE__);
             strcpy(g_pub_tx.reply,"D101");
             goto ErrExit;
         }
         while(1){
             memset(&sCom_holiday , 0 , sizeof(sCom_holiday));
             ret=Com_parm_holiday_Fet_Sel(&sCom_holiday,g_pub_tx.reply);
             vtcp_log("[%s],[%d],[%d]*****************\n",__FILE__,__LINE__,ret);
             if(100 == ret) break;
             if(ret){
                 vtcp_log("[%s],[%d] ȡֵʧ��!",__FILE__,__LINE__);
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
                 fprintf( fp, "~@�ڼ��մ���|�ڼ�������|��ʼ����|��ֹ����|�ܹ�����|�Ƿ���Ч|��ע|\n");
             }
             memset(cEffect      , 0 , sizeof(cEffect));
             if(sCom_holiday.type[0]=='0')
             {
                 memcpy(cEffect,"δ��Ч",sizeof(cEffect)-1);
             }
             else if(sCom_holiday.type[0]=='1')
             {
                 memcpy(cEffect,"����Ч",sizeof(cEffect)-1);
             }
             else
             {
                 memcpy(cEffect,"��  ��",sizeof(cEffect)-1);
             }
             fprintf( fp, "%-3s|%-10s|%-8ld|%-8ld|%-6ld|%-1s|%-60s|%1s|\n",sCom_holiday.date_code,sCom_holiday.date_name,sCom_holiday.beg_date,\
                            sCom_holiday.end_date,sCom_holiday.days,cEffect,sCom_holiday.filler,sCom_holiday.type);
             rdCnt ++;
         }
         vtcp_log("[%s][%d]��ѯ�����¼���rdCnt=[%d]!\n",__FILE__,__LINE__,rdCnt);
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
         vtcp_log("[%s][%d]========�����ѯ����",__FILE__,__LINE__);
       }
    
    vtcp_log("[%s][%d]========�������",__FILE__,__LINE__);
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
       
