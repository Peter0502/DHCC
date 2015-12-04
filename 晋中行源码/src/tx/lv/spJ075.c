/*****************************************************************
交易名称:清算中心多方协议号维护（1--查询，2--增加，3--修改,4--删除）
		 
FUNCTION For XiaoE
Create on 2005-12-10
    (为达到最佳浏览效果，请调整行宽至132)
 ****************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "stdlib.h"
#include "RBS_DEFINE.h"
#include "cn_define.h"
#include "apl_itf.h"
#include "apl_incl.h"
#include "apl_glob.h"
#include "gs_incl.h"
#include "pb_incl.h"
#include "wd_incl.h"
#include "wd_extn.h"
#include "dg_incl.h"
#include "dg_cj_incl.h"

#include "lv_define.h"


EXEC SQL INCLUDE sqlca;
EXEC SQL INCLUDE sqlda;

static struct
  {   
  	  char   optype	      ;     /**  操作类型    **/ 
  	  char	 xyno	   [60];    /**	协议号  	**/
	  char   cashactno [32];    /**	收款帐号	    **/
	  char   cashname  [60];    /**	收款户名**/
	  char   flag          ;    /**  协议状态标志    **/
	  char   filler	   [60];    /**	备注    **/
  } tis;
static struct 
 {
 	  char	 filename  [16];
 	  char	 flag		   ;    /**  成功标志	**/
      char	 xyno	   [60];    /**	协议号  	**/
	  char   cashactno [32];    /**	收款帐号	    **/
	  char   cashname  [60];    /**	收款户名**/
	  char   stsflag       ;    /**  协议状态标志    **/
	  char   filler	   [60];    /**	备注    **/
 }tos;
static struct wd_lv_dfxym_area wd_lv_dfxym;
static struct wd_hvuniontab_area wd_hvuniontab;
EXEC SQL BEGIN DECLARE SECTION;
    char    temxyno[61];
	static struct wd_lv_dfxyd_area wd_lv_dfxyd;
EXEC SQL END DECLARE SECTION;
int dh65610_Work()
{
   dfxym_Initial();
   printf("[%d]++++++++++++\n",__LINE__);
   if(it_txcom.txrsut!=TX_SUCCESS){
        gs100_error();
        return(-1);
    }
   dfxym_Verify();
   printf("[%d]++++++++++++\n",__LINE__);
   if(it_txcom.txrsut!=TX_SUCCESS){
        gs100_error();
        return(-1);
    }
   memcpy(tos.filename,"6561",sizeof(tos.filename)); 
   if(tis.optype=='1')        
   {
      	dfxym_query();
      	if(it_txcom.txrsut != TX_SUCCESS){
            gs100_error();
            return(-1);
    	}
    	tos.flag='1';
   }              
   if(tis.optype=='2') 		
   {
   	    dfxym_insert();
   	    if(it_txcom.txrsut != TX_SUCCESS){
            gs100_error();
            return(-1);
    	}
    	tos.flag='2';
   }
   
   if(tis.optype=='3')     
   {
   		dfxym_update();
   		if(it_txcom.txrsut != TX_SUCCESS){
            gs100_error();
            return(-1);
    	}
    	tos.flag='3';
   }
   if(tis.optype=='4')     
   {
   		dfxym_delete();
   		if(it_txcom.txrsut != TX_SUCCESS){
            gs100_error();
            return(-1);
    	}
    	tos.flag='4';
   }
   printf("[%s][%d]数据处理完毕\n"__FILE__,__LINE__);
   dfxym_PutMsg();
   if(it_txcom.txrsut!=TX_SUCCESS){
           gs100_error();
           return(-1);
    }
    printf("+++++++++程序处理完毕++++++++\n");
	return(0);
}

/**   全局变量的初始化和数据传入	**/
dfxym_Initial()
{
    printf("[%s][%d]变量初始化!\n",__FILE__,__LINE__);
    strcpy(it_txcom.prgnm,"6561");
    /*  初始化tis,TOTA结构*/
    memset(&wd_lv_dfxym,'\0',sizeof(wd_lv_dfxym));
    memset(&tis,'\0',sizeof(tis));
    memcpy(&tis,it_tita.labtex.text,sizeof(tis));  
    memset(&tos,' ',sizeof(tos));
    memcpy(wd_lv_dfxym.xyno,tis.xyno,sizeof(wd_lv_dfxym.xyno)-1);
    printf("[%s][%d]你输入的协议号是[%s]\n",__FILE__,__LINE__,wd_lv_dfxym.xyno);
    wf_updflg.upd_acmr = FLAG_OFF;
    wf_updflg.upd_xdtl = FLAG_OFF;
    wf_updflg.upd_tlrf = FLAG_OFF;
    wf_updflg.upd_vocdtl = FLAG_OFF;
    printf("[%s][%d]你输入的协议号是[%s]\n",__FILE__,__LINE__,wd_lv_dfxym.xyno);
	return(0);
 }
int dfxym_Verify()
{
	if(memcmp(TITA.kinbr,LV_CZ_QSBRNO,BRNO_LEN)!=0){
		printf("%s,%d,非清算中心不允许做此交易\n",__FILE__,__LINE__);
		it_txcom.txrsut=TX_REJECT;
		strncpy(it_totw.labtex.text,"非清算中心不允许做此交易!",40);
		return(-1);
	}
	return(0);
}
/*查询数据 */
dfxym_query()
{
    memcpy(wd_lv_dfxym.xyno,tis.xyno,sizeof(wd_lv_dfxym.xyno)-1);
    if((it_txcom.rtncd=DbsLV_DFXYM(DBS_FIND,&wd_lv_dfxym))!=DB_OK)                                                     
    {        
        DbsLV_DFXYMDebug(&wd_lv_dfxym); 
        printf("[%s][%d]find lv_dfxym error! sqlcode=[%d]xmh=[%c] \n",__FILE__,__LINE__,it_txcom.rtncd,wd_lv_dfxym.xyno); 
        it_txcom.txrsut = TX_REJECT; 
        sprintf(it_totw.labtex.text, "没有你输入的协议号![L%d]",__LINE__); 
        gs100_error();                                                    
        return(-1);     
    }
    printf("[%s][%d]小额协议号查询!\n",__FILE__,__LINE__); 	
}
/*增加数据 */
dfxym_insert()
{
    if(tis.flag=='1')
    {
        it_txcom.txrsut = TX_REJECT; 
        sprintf(it_totw.labtex.text, "开一个作废的协议没有意义![L%d]",__LINE__); 
        gs100_error();                                                    
        return(-1);
    }
    wd_lv_dfxym.flag[0]=tis.flag;
    
    memcpy(wd_lv_dfxym.cashactno,tis.cashactno,sizeof(wd_lv_dfxym.cashactno)-1);
    
    memcpy(wd_lv_dfxym.cashname,tis.cashname,sizeof(wd_lv_dfxym.cashname)-1);
    memcpy(wd_lv_dfxym.xyno,tis.xyno,sizeof(wd_lv_dfxym.xyno)-1);
    memcpy(wd_lv_dfxym.brno,TITA.kinbr,sizeof(wd_lv_dfxym.brno)-1);
    memcpy(wd_lv_dfxym.lastday,it_txcom.tbsdy,sizeof(wd_lv_dfxym.lastday)-1);
    memcpy(wd_lv_dfxym.tlrno,TITA.tlrno,sizeof(wd_lv_dfxym.tlrno)-1);
    memcpy(wd_lv_dfxym.filler,tis.filler,sizeof(wd_lv_dfxym.filler)-1); 
    printf("[%s][%d] flag=[%s[[%c] and filler=[%s]\n",__FILE__,__LINE__,wd_lv_dfxym.flag,tis.flag,wd_lv_dfxym.filler);
    printf("[%s][%d]  wd_lv_dfxym=[%s]\n",__FILE__,__LINE__,&wd_lv_dfxym); 
    if((it_txcom.rtncd=DbsLV_DFXYM(DBS_FIND,&wd_lv_dfxym))==DB_OK)                                                   
    {  
        DbsLV_DFXYMDebug(&wd_lv_dfxym); 
        printf("[%s][%d]xyno=[%s]sqlcode=[%d]\n",__FILE__,__LINE__,wd_lv_dfxym.xyno,it_txcom.rtncd);
        it_txcom.txrsut = TX_REJECT; 
        sprintf(it_totw.labtex.text, "lv_dfxym中已经有此协议号![L%d]",__LINE__); 
        gs100_error();                                                    
        return(-1);     
    }
    if(it_txcom.rtncd)
    {
        if(it_txcom.rtncd==1403)
        {        
            EXEC SQL insert into lv_dfxym values(:wd_lv_dfxym); 
            printf("[%d]+++++++++++++++++++++++++++++?????????!!!!!!!!!\n",__LINE__); 
            if(sqlca.sqlcode)
            {
                printf("insert lv_dfxym sqlca.sqlcode is [%d],[%d]\n",sqlca.sqlcode,__LINE__);
                it_txcom.txrsut = TX_REJECT;
                sprintf(it_totw.labtex.text, "插入lv_dfxym表错误![L%d]", __LINE__);
                return(-1);
            } 
        }
        else
        {
            it_txcom.txrsut = TX_REJECT; 
            sprintf(it_totw.labtex.text, "数据库错误![L%d]",__LINE__); 
            gs100_error();                                                    
            return(-1);
        }
    }
}
/*修改数据 */
dfxym_update()
{
    memset(temxyno,'\0',sizeof(temxyno));
    memset(&wd_lv_dfxyd,'\0',sizeof(wd_lv_dfxyd));
    memcpy(wd_lv_dfxym.xyno,tis.xyno,sizeof(wd_lv_dfxym.xyno)-1);
    printf("[%d]++++++++++++\n",__LINE__);
    if((it_txcom.rtncd=DbsLV_DFXYM(DBS_LOCK,&wd_lv_dfxym))!= DB_OK)                                                     
    {       
        DbsLV_DFXYMDebug(&wd_lv_dfxym); 
        printf("[%s][%d]xyno=[%s]sqlcode=[%d]\n",__FILE__,__LINE__,wd_lv_dfxym.xyno,it_txcom.rtncd);
        it_txcom.txrsut = TX_REJECT; 
        sprintf(it_totw.labtex.text, "lv_dfxym锁定表失败[L%d]",__LINE__); 
        gs100_error();                                                    
        return(-1);     
    }
    if(memcmp(wd_lv_dfxym.brno,TITA.kinbr,sizeof(wd_lv_dfxym.brno)-1) )
    {
        it_txcom.txrsut = TX_REJECT; 
        sprintf(it_totw.labtex.text, "只能录入行修改，不可改成他行![L%d]",__LINE__); 
        gs100_error();                                                    
        return(-1);
    }
    wd_lv_dfxym.flag[0]=tis.flag;
    
    memcpy(wd_lv_dfxym.cashactno,tis.cashactno,sizeof(wd_lv_dfxym.cashactno)-1);
    memcpy(wd_lv_dfxym.cashname,tis.cashname,sizeof(wd_lv_dfxym.cashname)-1);
    memcpy(wd_lv_dfxym.xyno,tis.xyno,sizeof(wd_lv_dfxym.xyno)-1);
    memcpy(wd_lv_dfxym.brno,TITA.kinbr,sizeof(wd_lv_dfxym.brno)-1);
    memcpy(wd_lv_dfxym.lastday,it_txcom.tbsdy,sizeof(wd_lv_dfxym.lastday)-1);
    memcpy(wd_lv_dfxym.tlrno,TITA.tlrno,sizeof(wd_lv_dfxym.tlrno)-1);
    memcpy(wd_lv_dfxym.filler,tis.filler,sizeof(wd_lv_dfxym.filler)-1);     
    if((it_txcom.rtncd=DbsLV_DFXYM(DBS_UPDATE,&wd_lv_dfxym))!=DB_OK)                                                     
    {        
        DbsLV_DFXYMDebug(&wd_lv_dfxym); 
        printf("[%s][%d]update lv_dfxym error sqlcode=[%d]\n",__FILE__,__LINE__,it_txcom.rtncd); 
        it_txcom.txrsut = TX_REJECT; 
        sprintf(it_totw.labtex.text, "lv_dfxym更新失败[L%d]",__LINE__); 
        gs100_error();                                                    
        return(-1);     
    }
    if(tis.flag=='1')  
    {
        memcpy(temxyno,wd_lv_dfxym.xyno,sizeof(temxyno)-1);
        EXEC SQL DECLARE cur_dfxyd cursor for select * from lv_dfxyd  where xyno=:temxyno;
        memcpy(temxyno,wd_lv_dfxym.xyno,sizeof(temxyno)-1);
        EXEC SQL open cur_dfxyd;
        if(sqlca.sqlcode)
	    {
	    	printf("[%s][%d]Open cursor error!sqlcode=[%d]\n",__FILE__,__LINE__,sqlca.sqlcode);
	    	it_txcom.txrsut = TX_REJECT;
	    	sprintf(it_totw.labtex.text, "打开游标错误![L%d]",__LINE__); 
	    	gs100_error();
	    	return(-1);
	    }
        while(1)
        {
            memset(&wd_lv_dfxyd,'\0',sizeof(wd_lv_dfxyd));
	        EXEC SQL FETCH cur_dfxyd
	            into :wd_lv_dfxyd;
	        if(sqlca.sqlcode==1403)
	        {
	            break;
	        }
	        if(sqlca.sqlcode!=0)
	        {
	            it_txcom.txrsut=TX_REJECT;
	            memcpy(it_totw.labtex.text,"fetch lv_dfxyd的数据表错误!",40);
	            printf("[%s][%d]errcode=[%d]\n",__FILE__,__LINE__,sqlca.sqlcode);
	            return(-1);
	        }
		    EXEC SQL update lv_dfxyd set flag='1' where xyno=:temxyno;
            printf("[%d]+++++++++++++++++++++++!!!!!!!!![%s]\n",__LINE__,wd_lv_dfxyd.xyno);
        }
	    EXEC SQL CLOSE cur_dfxyd;
    }
}
dfxym_delete()
{
   memset(temxyno,'\0',sizeof(temxyno));
    memset(&wd_lv_dfxyd,'\0',sizeof(wd_lv_dfxyd));
    memcpy(wd_lv_dfxym.xyno,tis.xyno,sizeof(wd_lv_dfxym.xyno)-1);
    if((it_txcom.rtncd=DbsLV_DFXYM(DBS_LOCK,&wd_lv_dfxym))!=DB_OK)                                                     
    {       
        DbsLV_DFXYMDebug(&wd_lv_dfxym); 
        printf("[%s][%d]xyno=[%s]sqlcode=[%d]\n",__FILE__,__LINE__,wd_lv_dfxym.xyno,it_txcom.rtncd);
        it_txcom.txrsut = TX_REJECT; 
        sprintf(it_totw.labtex.text, "lv_dfxym锁定表失败[L%d]",__LINE__); 
        gs100_error();                                                    
        return(-1);     
    }

    if(!memcmp(wd_lv_dfxym.brno,TITA.kinbr,sizeof(wd_lv_dfxym.brno)-1) )
    {
        it_txcom.txrsut = TX_REJECT; 
        sprintf(it_totw.labtex.text, "只能录入行修改，不可改成他行![L%d]",__LINE__); 
        gs100_error();                                                    
        return(-1);
    }
     
    wd_lv_dfxym.flag[0]=tis.flag;
    memcpy(wd_lv_dfxym.cashactno,tis.cashactno,sizeof(wd_lv_dfxym.cashactno)-1);
    memcpy(wd_lv_dfxym.cashname,tis.cashname,sizeof(wd_lv_dfxym.cashname)-1);
    memcpy(wd_lv_dfxym.xyno,tis.xyno,sizeof(wd_lv_dfxym.xyno)-1);
    memcpy(wd_lv_dfxym.brno,TITA.kinbr,sizeof(wd_lv_dfxym.brno)-1);
    memcpy(wd_lv_dfxym.lastday,it_txcom.tbsdy,sizeof(wd_lv_dfxym.lastday)-1);
    memcpy(wd_lv_dfxym.tlrno,TITA.tlrno,sizeof(wd_lv_dfxym.tlrno)-1);
    memcpy(wd_lv_dfxym.filler,tis.filler,sizeof(wd_lv_dfxym.filler)-1);     
    if((it_txcom.rtncd=DbsLV_DFXYM(DBS_DELETE,&wd_lv_dfxym))!=DB_OK)                                                     
    {        
        DbsLV_DFXYMDebug(&wd_lv_dfxym); 
        printf("[%s][%d]delete lv_dfxym error sqlcode=[%d]\n",__FILE__,__LINE__,it_txcom.rtncd); 
        it_txcom.txrsut = TX_REJECT; 
        sprintf(it_totw.labtex.text, "lv_dfxym删除失败[L%d]",__LINE__); 
        gs100_error();                                                    
        return(-1);     
    } 
    memcpy(temxyno,wd_lv_dfxym.xyno,sizeof(temxyno)-1);
    EXEC SQL DECLARE cur_dfxyd2 cursor for select * from lv_dfxyd  where xyno=temxyno;
    EXEC SQL open cur_dfxyd2;
    if(sqlca.sqlcode)
    {
    	printf("[%s][%d]Open cursor error!sqlcode=[%d]\n",__FILE__,__LINE__,sqlca.sqlcode);
    	it_txcom.txrsut = TX_REJECT;
    	sprintf(it_totw.labtex.text, "打开游标错误![L%d]",__LINE__); 
    	gs100_error();
    	return(-1);
    }
    while(1)
    {
        memset(&wd_lv_dfxyd,'\0',sizeof(wd_lv_dfxyd));
        EXEC SQL FETCH cur_dfxyd2
            into :wd_lv_dfxyd;
        if(sqlca.sqlcode==1403)
        {
            break;
        }
        if(sqlca.sqlcode!=0)
        {
            it_txcom.txrsut=TX_REJECT;
            memcpy(it_totw.labtex.text,"查询卡的数据表错误!",40);
            printf("[%s][%d]errcode=[%d]\n",__FILE__,__LINE__,sqlca.sqlcode);
            return(-1);
        }
        EXEC SQL update lv_dfxyd2 set flag='1' where xyno=:temxyno;
        printf("[%d]+++++++++++++++++++++++!!!!!!!!![%s]\n",__LINE__,wd_lv_dfxyd.xyno);
    }
	EXEC SQL CLOSE cur_dfxyd2;
}
dfxym_PutMsg()
{
    /**准备返回数据	**/
    memcpy(tos.xyno,wd_lv_dfxym.xyno,sizeof(tos.xyno));
    printf("[%s][%d]xyno=====[%s] \n",__FILE__,__LINE__,tos.xyno);
    memcpy(tos.cashactno,wd_lv_dfxym.cashactno,sizeof(tos.cashactno));
    printf("[%s][%d]pactno=====[%s] \n",__FILE__,__LINE__,tos.cashactno); 
    memcpy(tos.cashname,wd_lv_dfxym.cashname,sizeof(tos.cashname));	  
    tos.stsflag=wd_lv_dfxym.flag[0];
    printf("[%s][%d]stsflag=====[%c]\n",__FILE__,__LINE__,tos.stsflag);  
    gs42_mvbasic();
    if(tis.optype=='1')
    {
    	TOTW.msgend = '8';	
    }
    else
    {
    	TOTW.msgend = '1';
    }
    printf("[%s][%d] \n",__FILE__,__LINE__);
    TOTW.msgtype = 'l';
    memcpy(TOTW.msgno, "6561", sizeof(TOTW.msgno));
    apitoa(TOTA_LABEL_LENGTH+sizeof(tos),sizeof(TOTW.msglng), TOTW.msglng);
    printf("[%s][%d]  \n",__FILE__,__LINE__);  
    memset(it_totw.labtex.text ,' ', sizeof(it_totw.labtex.text));  
    memcpy(it_totw.labtex.text , &tos, sizeof(tos)); 
    cs81_mvtota(); 
}

