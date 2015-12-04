/*************************************************
* 文 件 名: pubf_hv_cmt_r.c
* 功能描述: 大额来帐报文处理
*            pub_hv_recv          来报处理
*            pub_hv_cmt_c        拆来报报文
*            pub_hv_regtb        登记登记簿
*
* 作    者:      ChenMing
* 完成日期:   2006年08月17
* 修改记录:
*     1. 日    期:
*          修 改 人:
*          修改内容:
*     2.
*************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "hv_define.h"
#include "hv_pub.h"
#include "hvpack.h"
#include "svrpub_c.h"
#include "com_parm_c.h"

#include "mo_merge_ac_c.h"
#include "prdt_ac_id_c.h"
#include "hv_chargemx_c.h"
#include "hv_chkagemr_c.h"
#include "hv_chkmr_c.h"
#include "hv_chkmrdtl_c.h"
#include "hv_uniontab_c.h"
#include "hv_syspara_c.h"
#include "hv_poinfo_c.h"
#include   "data_dic_c.h"
#include <stdarg.h>
#include "find_debug.h"
#include "hv_81x_c.h"
#include "hv_orno_brno_c.h"


/* begin add by LiuHuafeng 2007-1-11 23:20 */
static char cLogfile_rece[512];
struct PVal_St *Pu_Val_bak_rece;
char *Pub_Buf_bak_rece;
#define LV_RECE_LOG "/home/newsys/usr/lhf/lv_recelog"
/* end by LiuHuafeng 2007-1-11 23:20 */
int lv_fprintf_com(char *filename, char *buf)
{
    FILE           *fp;
    const  long  max_file_len = 32*1024*1024;
    char   tmpbuf[255];
    char   strtime[255];
    char bkfnm[255];


    if(get_file_size(filename) - max_file_len > 0.001){
        sprintf(bkfnm, "%s%.12s.bk", filename, logGetTime(NULL, 0, strtime));
        rename(filename, bkfnm);
    }
    fp = fopen(filename, "a");
    if (fp == NULL) {
        return -1;
    }
    fprintf(fp, buf);
    fclose(fp);
}
void lv_log_com(char *filename, char *fmt,...)
{
    va_list         argptr;
    int             cnt;
    char            buf[1024 * 1024];

    memset(buf, 0x0, sizeof(buf));
    va_start(argptr, fmt);

    cnt = vsprintf(buf, fmt, argptr);
    va_end(argptr);
    strcat(buf, "\n");

    lv_fprintf_com(filename, buf);
}

/***************************************************************************/
/* 函数名称: pub_hv_recv_check_main()                                      */
/* 功能描述: spJ998 大额来帐－挂帐检查  主函数                             */
/* 参数:                                                                   */
/* 返回值:   0/-1                                                          */
/* 创建日期: 2006.08.30.                                                   */
/* 作者:     ChenMing                                                      */
/* 说明:                                                                   */
/***************************************************************************/
int pub_hv_recv_check_main()
{
    int ret = 0;
    char Stat = '';
    char cmtno[4 ];      /** 报文编号**/    
    char cTxday[9];
    THVPACKAGE hv_pack;        

	/**** rem by LiuHuafeng 20070111 no used 
    memset(&mbfetran,0x00,sizeof(struct MBFETRAN));
	****/
    memset(&hv_pack,0x00,sizeof(THVPACKAGE));    
    memset(cmtno,'\0',sizeof(cmtno)); 
    memset(cTxday, 0 , sizeof(cTxday));
    get_fd_data(DC_DATE,cTxday);
    /* begin add by LiuHuafeng 2007-1-11 23:18 */
    sprintf(cLogfile_rece,"%s%s.txt",LV_RECE_LOG,cTxday);   
    {
        int i=0;
        long iLength=0;
        Pu_Val_bak_rece =  (struct PVal_St *)malloc (sizeof(struct PVal_St) *PuVal_num);/**8583备份的共用变量指针***/
        Pub_Buf_bak_rece =(unsigned char *)malloc (sizeof(unsigned char)*Pub_leng+2);/**8583备份的公用变量缓冲区头指针**/
        memset(Pu_Val_bak_rece, 0 , sizeof(struct PVal_St) *PuVal_num);
        memset(Pub_Buf_bak_rece,' ',sizeof(unsigned char)*Pub_leng+2);
        for (i=0,iLength=0;i<PuVal_num;i++) 
        {
            strncpy(Pu_Val_bak_rece[i].sjbm,Pu_Val[i].sjbm,sizeof(Pu_Val_bak_rece[i].sjbm));
            Pu_Val_bak_rece[i].length=Pu_Val[i].length;
            Pu_Val_bak_rece[i].prec=Pu_Val[i].prec;
            Pu_Val_bak_rece[i].lx=Pu_Val[i].lx;
            Pu_Val_bak_rece[i].val=Pub_Buf_bak_rece+iLength;
            iLength+=Pu_Val_bak_rece[i].length;
        }
        lv_log_com(cLogfile_rece,"\n%s,%d 时间=[%d] new record coming ",__FILE__,__LINE__,g_pub_tx.tx_time);
        lv_log_com(cLogfile_rece,"%s,%d 时间=[%d] hv_pack地址=[%ld]",__FILE__,__LINE__,g_pub_tx.tx_time,&hv_pack);
        lv_log_com(cLogfile_rece,"%s,%d pu_val地址=[%ld],pub_buf地址=[%ld],Pu_Val_Creat_add=[%ld]",__FILE__,__LINE__,Pu_Val,Pub_Buf,iGlobal_Pu_Val_Addr);
        lv_log_com(cLogfile_rece,"%s,%d Pu_Val_bak_rece地址=[%ld],Pub_Buf_bak_rece地址=[%ld]",__FILE__,__LINE__,Pu_Val_bak_rece,Pub_Buf_bak_rece);
    }
    g_reply_int = 0;
    /*在这里开始第一次备份原始8583数据，无条件备份 */
    memcpy(Pub_Buf_bak_rece, Pub_Buf,sizeof(unsigned char)*Pub_leng+2);
    g_reply_int=iTest8583_1(__FILE__,__LINE__,"12345678");
    if(g_reply_int!=0)
    {
        /* 检测8583失败,要重新设置8583 */
        g_reply_int=iReset8583_1(__FILE__,__LINE__,"11111110");
        if(g_reply_int !=0){
        memcpy(g_pub_tx.reply,"T065",4);
        sprintf(acErrMsg,"检测8583数据项目失败\n");
        set_zd_data(DC_GET_MSG,"分析来帐报文，报头内容非法");
        WRITEMSG
        return -1;
        }
        else{
        sprintf(acErrMsg,"8583数据恢复成功\n");
        set_zd_data(DC_GET_MSG,"8583数据恢复成功");
        WRITEMSG
        }
    }
    /* end by LiuHuafeng 2007-1-11 23:18 */
    if ( pub_hv_cmt_head(&hv_pack) )   /* 读来帐报文， 分析报头内容是否合法  */
    {
        sprintf(acErrMsg,"分析来帐报文，报头内容非法!");
        MBFEWRITEMSG  
        return -1;
    }
    memcpy(cmtno,hv_pack.CMTCode,3);   /** 得到报文编号 **/              
    vtcp_log("[%s][%d]hv_pack.CMTCode=[%.3s]",__FILE__,__LINE__,cmtno);
    if (pub_hv_check_cmtno(cmtno) != 0)  /** 检查报文编号是否合法 **/ 
    {
        sprintf(acErrMsg,"cmtno=[%s] -------cmtno 错误！", cmtno);
        MBFEWRITEMSG  
        return -1;
    }    
    /*  对报文分类处理                                                              */        
    /*  分三种：1. 支付交易，需要走帐，则需要进行挂帐检查                           */
    /*          2. 非支付交易，不需要走帐，直接交易流程的第4步，登记来帐。          */
    /*          3. 有些报文需要特殊处理                                             */
    switch (atoi(cmtno))
    {
        case 100:     case 101:     
        case 102:     case 103:         
        case 105:     case 108:         
        case 122:     case 123:         
        case 124:     case 725:  
        case 232:  
        case 802:     case 803:
/** dongxy 20070402 增加812和814报文 **/
        case 812:     case 814:
        {
            vtcp_log("%s,%d ",__FILE__,__LINE__);
            pubf_hv_handle_zf_cmt(&hv_pack,cmtno);    
            return 0;
        }
        
        case 301:     case 302:
        case 303:  
        case 313:  /*Added  2006-11-15 14:27  */
        case 314:   
        case 407:     case 408:
        case 417:     case 418:
        case 689:     case 681:
        case 841:  
        {
            iSetHvTxBrNo(&hv_pack);
            pubf_hv_handle_fzf_cmt(cmtno);
            return 0;
        }
    
        case 312:                    
        case 253: case 910:       /*  需要检查是否记帐  */   
        {
            int iJZflg=0;
            vtcp_log("[%s][%d] 11111   到这里了 !!  hv_pack.CMTCode=[%.3s]",__FILE__,__LINE__,cmtno);
            iSetHvTxBrNo(&hv_pack);
            ret = iHvCheck253_910_stat(&hv_pack);
            vtcp_log("[%s][%d] 11111   到这里了 !! ret=[%d]",__FILE__,__LINE__,ret);      
            if ( ret == 1 ) 
            {   /* 返回 1     需要记帐  设置8583  奔DELZ  */             
                pub_hv_set_8583(&hv_pack,cmtno, 0 );                                         
                vtcp_log("%s,%d 需要冲帐",__FILE__,__LINE__);
                strcpy( g_pub_tx.reply, "0000" );  
                return 0; 
            }else if(ret)    /* ret == 100 或 ret < 0 出错了*/  
            {
                sprintf(acErrMsg,"iHvCheck253_910_stat() 中出错了 !");
                MBFEWRITEMSG  
                return -1;          
            } 
            else
            {
               pubf_hv_handle_fzf_cmt(cmtno);     
            }
            return 0;                                
        }
        default: 
            sprintf(acErrMsg,"cmtno=[%s] ------cmtno 错误！", cmtno);
            MBFEWRITEMSG 
            return -1;    
    }  
    return 0;
}


/***************************************************************************/
/* 函数名称: pub_hv_recv_reg_main()                                             */
/* 功能描述: spJ999 大额来帐－登记来帐  主函数                             */
/* 参数:                                                                   */
/* 返回值:   0/-1                                                          */
/* 创建日期: 2006.08.30.                                                   */
/* 作者:     ChenMing                                                      */
/* 说明:                                                                   */
/***************************************************************************/
int pub_hv_recv_reg_main(char* chkcmtno)
{
    THVPACKAGE hv_pack;        
    char    cmtno        [4 ];      /** 报文编号**/    
    vtcp_log("===  spJ999 大额来帐－登记来帐  主函数 开始!!  ===");

    memset(&hv_pack,0x00,sizeof(THVPACKAGE));    
    memset(cmtno,'\0',sizeof(cmtno));    

    {
        int i=0;
        long iLength=0;
        /**
        free(Pu_Val_bak_rece);
        free(Pub_Buf_bak_rece);
        Pu_Val_bak_rece =  (struct PVal_St *)malloc (sizeof(struct PVal_St) *PuVal_num);**8583备份的共用变量指针**
        Pub_Buf_bak_rece =(unsigned char *)malloc (sizeof(unsigned char)*Pub_leng+2); **8583备份的公用变量缓冲区头指针**
        ***/
        memset(Pu_Val_bak_rece, 0 , sizeof(struct PVal_St) *PuVal_num);
        memset(Pub_Buf_bak_rece,' ',sizeof(unsigned char)*Pub_leng+2);
        for (i=0,iLength=0;i<PuVal_num;i++) 
        {
            strncpy(Pu_Val_bak_rece[i].sjbm,Pu_Val[i].sjbm,sizeof(Pu_Val_bak_rece[i].sjbm));
            Pu_Val_bak_rece[i].length=Pu_Val[i].length;
            Pu_Val_bak_rece[i].prec=Pu_Val[i].prec;
            Pu_Val_bak_rece[i].lx=Pu_Val[i].lx;
            Pu_Val_bak_rece[i].val=Pub_Buf_bak_rece+iLength;
            iLength+=Pu_Val_bak_rece[i].length;
        }
        lv_log_com(cLogfile_rece,"%s,%d 时间=[%d] ",__FILE__,__LINE__,g_pub_tx.tx_time);
        lv_log_com(cLogfile_rece,"%s,%d 时间=[%d] hv_pack地址=[%ld]",__FILE__,__LINE__,g_pub_tx.tx_time,&hv_pack);
        lv_log_com(cLogfile_rece,"%s,%d pu_val地址=[%ld],pub_buf地址=[%ld]",__FILE__,__LINE__,Pu_Val,Pub_Buf);
        lv_log_com(cLogfile_rece,"%s,%d Pu_Val_bak_rece地址=[%ld],Pub_Buf_bak_rece地址=[%ld]",__FILE__,__LINE__,Pu_Val_bak_rece,Pub_Buf_bak_rece);
        lv_log_com(cLogfile_rece,"%s,%d 时间=[%d]\n",__FILE__,__LINE__,g_pub_tx.tx_time);
    }
    g_reply_int = 0;
    /*在这里开始第一次备份原始8583数据，无条件备份 */
    memcpy(Pub_Buf_bak_rece, Pub_Buf,sizeof(unsigned char)*Pub_leng+2);
    g_reply_int=iTest8583_1(__FILE__,__LINE__,"87654321");
    if(g_reply_int!=0)
    {
        /* 检测8583失败,要重新设置8583 */
        g_reply_int=iReset8583_1(__FILE__,__LINE__,"22222220");
        if(g_reply_int !=0){
        memcpy(g_pub_tx.reply,"T065",4);
        sprintf(acErrMsg,"检测8583数据项目失败\n");
        set_zd_data(DC_GET_MSG,"分析来帐报文，报头内容非法");
        WRITEMSG
        return -1;
        }
        else{
        sprintf(acErrMsg,"8583数据恢复成功\n");
        set_zd_data(DC_GET_MSG,"8583数据恢复成功");
        WRITEMSG
        }
    }
    /* end by LiuHuafeng 2007-1-11 23:18 */

    /* ====== 读来帐报文，分析来帐报文 1. 分析报头内容是否合法  ==========*/
    if ( pub_hv_cmt_head(&hv_pack) )
    {
        sprintf(acErrMsg,"分析来帐报文，报头内容非法!");
        MBFEWRITEMSG  
            return -1;
    }
    /** 得到报文编号 **/   
    memcpy(cmtno,hv_pack.CMTCode,3);                
    vtcp_log("[%s][%d]hv_pack.CMTCode=[%-3s]",__FILE__,__LINE__,cmtno);

    if (memcmp(cmtno,chkcmtno,3) != 0)
    {
        sprintf(acErrMsg,"cmtno=[%s] chkcmtno=[%s] 不一致！", cmtno,chkcmtno);
        MBFEWRITEMSG  
            return -1;
    }        
    /*  分别处理 批量 和 非批量 业务报文 */
    if ( memcmp(cmtno,"689",3) == 0 )    /* 批量支付业务报文 */
    {        
        vtcp_log("[%s][%d] 批量支付业务报文 cmtno=[%s] ",__FILE__,__LINE__,cmtno);        
        if ( pub_hv_batch_rcv(&hv_pack,cmtno) )  return -1;
    }
    else    /* 非批量业务报文 */
    {
        vtcp_log("[%s][%d] 非批量支付业务报文 cmtno=[%s] ",__FILE__,__LINE__,cmtno);        
        if ( pub_hv_Notbatch_rcv(&hv_pack,cmtno) )  return -1;        
    }
        
    strcpy( g_pub_tx.reply, "0000" ); 
    return 0;
}

/***************************************************************************/
/* 函数名称: pub_hv_cmt_head()                                             */
/* 参数:                                                                   */
/* 功能描述: 读来帐报文，分析报头内容是否合法，设置相应域                  */
/*                                                                         */
/* 返回值:   0/-1                                                          */
/* 创建日期: 2006.08.18                                                    */
/* 作者:     ChenMing                                                      */
/* 说明:                                                                   */
/***************************************************************************/
int pub_hv_cmt_head(THVPACKAGE* hv_pack)
{
    FILE *fcmt;
    char fullfilename[100];        
    char filename[50];
    /* char log_buf[HV_BODY_LENGTH];    报文体数据（60k，报文头＋业务头+报文体） */
    long cmt_len;

    memset( fullfilename,'\0', sizeof(fullfilename) );    
    memset( filename,'\0', sizeof(filename) );
    /*memset( log_buf, '\0', sizeof(log_buf) );*/

    get_zd_data("0250",filename);
    get_zd_long("0530",&cmt_len);
    /* begin add by LiuHuafeng 2007-1-11 23:25 */
    g_reply_int=iTest8583_1(__FILE__,__LINE__,"11111111");
    if(g_reply_int!=0)
    {
        /* 检测8583失败,要重新设置8583 */
        iReset8583_1(__FILE__,__LINE__,"11111112");
        if(g_reply_int !=0){
        memcpy(g_pub_tx.reply,"T065",4);
        sprintf(acErrMsg,"检测8583数据项目失败\n");
        set_zd_data(DC_GET_MSG,"分析来帐报文，报头内容非法");
        WRITEMSG
        return -1;
        }
        else{
        sprintf(acErrMsg,"8583数据恢复成功\n");
        set_zd_data(DC_GET_MSG,"8583数据恢复成功");
        WRITEMSG
        }
    }
    /* end by LiuHuafeng 2007-1-11 23:26 */
    pub_base_strpack( filename );
    if ( strlen(filename) < 1 )
    {
        sprintf(acErrMsg,"来报文件名错");
        MBFEWRITEMSG
        return -1;
    }
    sprintf( fullfilename,"%s/",getenv("FILDIR") );
    memcpy(fullfilename+strlen(fullfilename),filename,sizeof(filename));
    vtcp_log("来报报文文件名:[%s]",fullfilename);  

    /*For Test   100:    cmt_len = 672;  301:    cmt_len = 433;    301:    cmt_len = 14514;    */ 
    cmt_len = HV_BODY_LENGTH -1;            

    fcmt = fopen(fullfilename,"r");
    if (fcmt==NULL)
    {
        sprintf(acErrMsg,"读来报文件[%s]打开文件出错",fullfilename);
        MBFEWRITEMSG
            return -1;
    }
    
    g_reply_int=fread(hv_pack->body,sizeof(char),cmt_len,fcmt);
    if (!g_reply_int)
    {
        fclose(fcmt);
        sprintf(acErrMsg,"读来报文件[%s]读文件出错",fullfilename);
        MBFEWRITEMSG
        return -1;
    }
    fclose(fcmt);

    /* strncpy( log_buf, hv_pack->body, cmt_len );
    log_buf[sizeof(log_buf)-1] = '\0';
    vtcp_log("[%s][%d] 来报报文长[%d]:",__FILE__,__LINE__,strlen(log_buf));
    if(strlen(log_buf) < 2000 )
    {
    vtcp_log("\n%s",log_buf);    
    }
    else
    {
    vtcp_log("来报报文长超过2000，为批量报文");    
    }*/

    /* 取来报报头信息,给 THVPACKAGE 相应报头项赋值 新系统程序中解析报文程序，结构更清晰  */  
    if (split_head(hv_pack))
    {
        vtcp_log("[%s][%d] split_head 分析来报报头错误",__FILE__,__LINE__);
        sprintf(acErrMsg," 大额来帐报文报头分析错误 ! [%s]  ",fullfilename);
        MBFEWRITEMSG
        return -1;        
    }
    /** 得到报文编号 
    memcpy(cmtno,hv_pack.CMTCode,3);                
    vtcp_log("[%s][%d]hv_pack.CMTCode=[%-3s]",__FILE__,__LINE__,cmtno); 
    **/

    return 0;
}

/************************************************************************* 
* 函 数 名: split_head                  
* 功能描述： 分析来报报头
* 作    者:  ChenMing                             
* 完成日期： 2006-8-20                             
* 修改记录：                                      
*   日   期:                                      
*   修 改 人:                                     
*   修改内容:                                      
*************************************************************************/ 
/**/
int split_head(THVPACKAGE* phv_pack)
{
    char tmpstr[7];
    memset(tmpstr,0x00,sizeof(tmpstr));
    memcpy(tmpstr,phv_pack->body,3);
    
    if (strcmp(tmpstr,"{1:"))
    {
        sprintf(acErrMsg,"报头块块标识错误[%s]",tmpstr);
        MBFEWRITEMSG
            return -1;
    }
    memset(tmpstr,0x00,sizeof(tmpstr));
    memcpy(tmpstr,phv_pack->body+3,6);
    phv_pack->length = apatoi(tmpstr,6);

    memcpy(&phv_pack->use,phv_pack->body+9,1);
    memcpy(&phv_pack->mark,phv_pack->body+10,1);

    memcpy(&phv_pack->opeType,phv_pack->body+11,1);
    memcpy(phv_pack->opeStateCode,phv_pack->body+12,3);

    memcpy(phv_pack->flgNo,phv_pack->body+15,8);
    memcpy(phv_pack->reNo,phv_pack->body+23,20);

    memcpy(phv_pack->sendTime,phv_pack->body+43,14);
    memcpy(phv_pack->CMTCode,phv_pack->body+57,3);

    memcpy(phv_pack->workDate,phv_pack->body+60,8);

    /* sprintf(phv_pack->recvtime,"%8ld%06ld",g_pub_tx.tx_date,g_pub_tx.tx_time); */

    memset(tmpstr,0x00,sizeof(tmpstr));
    memcpy(tmpstr,phv_pack->body+68,1);
    if (strcmp(tmpstr,"}"))
    {
        sprintf(acErrMsg,"报头块结束标识错误[%s]",tmpstr);
        MBFEWRITEMSG
        return -1;
    }
    return 0;
}

/************************************************************************* 
* 函 数 名: split_ophead                  
* 功能描述： 分析来帐报文业务头块
* 作    者:  ChenMing                             
* 完成日期： 2006-8-20                             
* 修改记录：                                      
*   日   期:                                      
*   修 改 人:                                     
*   修改内容:                                      
*************************************************************************/ 
int split_ophead(THVPACKAGE* phv_pack,int position)
{
    char tmpstr[4];
    char *ophead;
    ophead = phv_pack->body + position;
    memset(tmpstr,0x00,sizeof(tmpstr));
    memcpy(tmpstr,ophead,3);
    
    if (strcmp(tmpstr,"{2:"))
    {
        if (strcmp(tmpstr,"{3:") == 0)
        {
            vtcp_log("没有业务头块块，直接 split_body()");
            return 100;
        }
        sprintf(acErrMsg,"业务头块块标识错[%s]",tmpstr);
        MBFEWRITEMSG
            return -1;
    }
    /* if(phv_pack->CMTCode[0] == '1' ) */
    memcpy(&(phv_pack->PRI),ophead+6,1);
    memset(tmpstr,0x00,sizeof(tmpstr));
    memcpy(tmpstr,ophead+23,1);
    if (strcmp(tmpstr,"}"))
    {
        sprintf(acErrMsg,"业务头块结束标识错[%s]",tmpstr);
        MBFEWRITEMSG
        return -1;
    }
    return 0;
}

/************************************************************************* 
* 函 数 名: split_body                  
* 功能描述： 分析正文块块标识
* 作    者:  ChenMing                             
* 完成日期： 2006-8-20                             
* 修改记录：                                      
*   日   期:                                      
*   修 改 人:                                     
*   修改内容:                                      
*************************************************************************/ 
int split_body(THVPACKAGE* phv_pack,int position)
{
    char tmpstr[4];
    char tagnum[4];
    char *pack;
    char *ptmp;
    char *tagbuf;
    int taglen;
    char *body;
    
    body = phv_pack->body + position;
    memset(tmpstr,0x00,sizeof(tmpstr));
    memcpy(tmpstr,body,3);
    
    if (strcmp(tmpstr,"{3:"))
    {
        TRACE
        if (tmpstr[0]!='/')
        {
            sprintf(acErrMsg,"正文块块标识错[%s]",tmpstr);
            MBFEWRITEMSG
                return -1;
        }
        vtcp_log("文件类型报文 ??? ");
        /*
        ptmp=body;
        while((ptmp=strchr(ptmp,'/'))!=NULL)
        {
        ptmp++;
        strcpy(mbfetranfilename,ptmp);
        }
        */
        sprintf(acErrMsg,"文件类型报文 ??? [%s]",tmpstr);
        MBFEWRITEMSG    
        return -1;
    }
    
    pack=body+3;
    memset(tmpstr,0x00,sizeof(tmpstr));
    memcpy(tmpstr,pack,1);
    while (tmpstr[0]==':')
    {
        pack=pack+1;
        memset(tagnum,0x00,sizeof(tagnum));
        memcpy(tagnum,pack,3);

        pack=pack+4;
        ptmp=pack;
        taglen=0;
        memset(tmpstr,0x00,sizeof(tmpstr));
        memcpy(tmpstr,ptmp,1);
        while ((tmpstr[0]!=':')&&(tmpstr[0]!='}'))
        {
            taglen++;
            ptmp=ptmp+1;
            memset(tmpstr,0x00,sizeof(tmpstr));
            memcpy(tmpstr,ptmp,1);
        }
        /*
        sprintf(acErrMsg, "TAG [%s]=========taglen [%d]", tagnum,taglen );
        WRITEMSG
        */
        /*   ???? 
        if ( taglen )
        {   
            tagbuf = (char *)malloc((taglen+1)*sizeof(char));
            if (tagbuf==NULL)
            {
                sprintf(acErrMsg,"拆正文TAG字段[:%s:]分配内存出错",tagnum);
                MBFEWRITEMSG
                    return -1;
            }
            memset(tagbuf,0x00,(taglen+1)*sizeof(char));
            memcpy(tagbuf,pack,taglen);
               set_mbfetran(tagnum,tagbuf); 
            free (tagbuf);           
        }*/
        pack=pack+taglen;
        memset(tmpstr,0x00,sizeof(tmpstr));
        memcpy(tmpstr,pack,1);
    }
    if (tmpstr[0]!='}')
    {
        sprintf(acErrMsg,"正文块结束标识错[%s]",tmpstr);
        MBFEWRITEMSG
        return -1;
    }
    return 0;
}

/***************************************************************************/
/* 函数名称: pub_hv_check_cmtno()                                         */
/* 功能描述: 检查cmtno是否合法                                             */
/* 参数:                                                                   */
/* 返回值:   0/-1                                                          */
/* 创建日期: 2006.08.18                                                    */
/* 作者:     ChenMing                                                      */
/* 说明:                                                                   */
/***************************************************************************/
int pub_hv_check_cmtno(char* cmtno)
{
    int i;
    if (strlen(cmtno) != 3)      return -1;     
    for (i = 0; i++; i < 3)
    {
        if ( *(cmtno+i) > '9'  || *(cmtno+i) < '0' ) return -1;
    }
    return 0;    
}

/***************************************************************************/
/* 函数名称: pubf_hv_handle_zf_cmt()                                         */
/* 功能描述: 处理大额来帐支付报文                                              */
/* 参数:                                                                   */
/* 返回值:   0/-1                                                          */
/* 创建日期: 2006.08.18                                                    */
/* 作者:     ChenMing                                                      */
/* 说明:                                                                   */
/***************************************************************************/
int pubf_hv_handle_zf_cmt(THVPACKAGE *hv_pack,char *cmtno)
{
    int  ret = 0;
    int  hangflg = 0;       /*处理标志*/   
    char stat = '';            /** 来账业务状态 **/

    /*  挂帐检查  */     
    /* add by LiuHuafeng 2006-10-1 21:59 设置默认状态为清算 */
    stat= RECEIVE_ACCT_QS;
    ret = pub_hv_hang_check(hv_pack,cmtno,&stat);
    if ( ret == 100 ) 
    {
        char cAc_no[32];
        char cAcno244[32];
        char cKinbr[BRNO_LEN+1];
        memset(cKinbr , 0 , sizeof(cKinbr));
        get_zd_data(DC_TX_BR_NO,cKinbr);
        memset(cAc_no, 0 , sizeof(cAc_no));
        /* 设置默认2440402，2440401 帐号，不用的交易金额设置为0 */
        memset(cAcno244, 0 , sizeof(cAcno244));
        ret=pub_base_GetParm_Str(cKinbr,PARM_HPQF,cAcno244);
        if(ret)
        {
            vtcp_log("%s,%d 没有找到交易机构指定的汇票签发户",__FILE__,__LINE__);
            return ret;
        }
        set_zd_data("0320",cAcno244);
        memset(cAcno244, 0 , sizeof(cAcno244));
        ret=pub_base_GetParm_Str(cKinbr,PARM_HPYC,cAcno244);
        if(ret)
        {
            vtcp_log("%s,%d 没有找到交易机构指定的汇票移存户",__FILE__,__LINE__);
            return ret;
        }
        set_zd_data("0330",cAcno244);
        /* set_zd_double("0420",0.0);*/
        
        /** 设置清算110账号 */
        set_zd_data("0660","2");
        set_zd_data("0720","1");
        ret=pub_base_GetParm_Str(cKinbr,PARM_HVQS,cAc_no);
        if(ret)
        {
            vtcp_log("%s,%d 没有找到交易机构指定的110户",__FILE__,__LINE__);
            return ret;
        }
        set_zd_data("1201",cAc_no);
        set_zd_data("1204","01");
        set_zd_data("1205","2");
        set_zd_data("0720","1");/* 借贷标志 */
        if(memcmp(cmtno,"123",3)==0 || memcmp(cmtno,"124",3)==0 || memcmp(cmtno,"122",3)==0)
        {
            /** 设置汇票挂账帐号 **/
            get_zd_data(DC_TX_BR_NO,cKinbr);
            set_zd_data("0210","01");
            set_zd_data("0680","6");
            set_zd_data("0660","2");
            vtcp_log("%s,%d set zd data 0660==2",__FILE__,__LINE__);
            set_zd_data("0670","2");
            ret=pub_base_GetParm_Str(cKinbr,PARM_HPGZ,cAc_no);
            if(ret)
            {
                vtcp_log("%s,%d 得到HPGZ参数错误，直接设置科目2620401",__FILE__,__LINE__);
                memcpy(cAc_no,"2620401",7);
            }
            set_zd_data("0300",cAc_no);
        }
        else if(memcmp(cmtno,"725",3)==0 )
        {
            /** 设置汇票挂账帐号 **/
            get_zd_data(DC_TX_BR_NO,cKinbr);
            set_zd_data("0210","01");
            set_zd_data("0680","6");
            set_zd_data("0660","2");
            vtcp_log("%s,%d set zd data 0660==2",__FILE__,__LINE__);
            set_zd_data("0670","2");
            ret=pub_base_GetParm_Str(cKinbr,PARM_HPGZ,cAc_no); /* 这里挂帐时随便设一挂帐科目，不走帐 */
            if(ret)
            {
                vtcp_log("%s,%d 得到HPGZ参数错误，直接设置科目2620401",__FILE__,__LINE__);
                memcpy(cAc_no,"2620401",7);
            }
            set_zd_data("0300",cAc_no);
            
            /* 注意，不走帐 一定置为 0  Added by ChenMing 2006-11-5 16:52 */
            set_zd_double("0420",0.0);          
        }
        else
        {
          /* 设置一般挂账帐号 **/
            /** 设置汇票挂账帐号 **/
            get_zd_data(DC_TX_BR_NO,cKinbr);
            set_zd_data("0210","01");
            set_zd_data("0680","6");
            set_zd_data("0660","2");
            vtcp_log("%s,%d set zd data 0660==2",__FILE__,__LINE__);
            set_zd_data("0670","2");
            
            /* 注意，不走帐 一定置为 0  Added by ChenMing 2006-11-5 16:52 */
            set_zd_double("0420",0.0);
            
            ret=pub_base_GetParm_Str(cKinbr,PARM_HVGZ,cAc_no);
            if(ret)
            {
                vtcp_log("%s,%d 得到HVGZ参数错误，直接设置科目2620401",__FILE__,__LINE__);
                memcpy(cAc_no,"2620401",7);
            }
            set_zd_data("0300",cAc_no);
        }
        hangflg = 1;         
    }
    /* begin add by LiuHuafeng 2009-5-13 14:31:32 for 村镇银行 */
    else if(ret == 9999)
    {
        vtcp_log("%s,%d 村镇银行，只进行登记 ",__FILE__,__LINE__);
        strncpy(g_pub_tx.reply,"0010",4);
        set_zd_data("0510",cmtno);
        set_zd_data("0980",&stat);      /** 来账业务状态 **/
        return 0;
    }/* end by LiuHuafeng 2009-5-13 14:31:32 for 村镇银行 */
    else if( ret )
    {
        sprintf(acErrMsg,"挂帐检查时出错");
        MBFEWRITEMSG
        return -1;
    }
    /*  根据挂帐标志，设置 DELZ 相应8583域  */     
    set_zd_data("0980",&stat);      /** 来账业务状态 **/     
    pub_hv_set_8583(hv_pack, cmtno, hangflg);
    return 0;
}

/***************************************************************************/
/* 函数名称: pub_hv_set_8583()                                        */
/* 功能描述:   根据挂帐标志，置相应8583域                                  */
/* 参数:                                                                   */
/* 返回值:   0/-1                                                          */
/* 创建日期: 2006.08.18                                                    */
/* 作者:     ChenMing                                                      */
/* 说明:                                                                   */
/***************************************************************************/
int pub_hv_set_8583(THVPACKAGE *hv_pack, char * cmtno, int hangflg )
{
    double amount_v = 0.0;
    char    cTmpBuf[19];
    char    cTmpBuf802[16]; /*没有RMB 2006-12-1 10:22*/
    char    cTmpBuf803[23]; /*没有RMB 2006-12-1 10:22*/
    char    cAmount[23]; 
    int iRet;
    char cStat[2];
    
    memset(cStat,0x00,sizeof(cStat));      
    memset(cAmount,0x00,sizeof(cAmount));     
    memset(cTmpBuf,0x00,sizeof(cTmpBuf));   
    memset(cTmpBuf802,0x00,sizeof(cTmpBuf802));     
    memset(cTmpBuf803,0x00,sizeof(cTmpBuf803));
         
    vtcp_log(" --- pub_hv_set_8583 Start !! ");

    /* For Test  Print 39 40*/
    double dTx_amt=0.0;
    get_zd_double("0390",&dTx_amt); 
    vtcp_log("%s,%d 0390  dTx_amt=======[%lf]",__FILE__,__LINE__,dTx_amt);
    dTx_amt = 0.0;
    get_zd_double("0400",&dTx_amt); 
    vtcp_log("%s,%d 0400  dTx_amt=======[%lf]",__FILE__,__LINE__,dTx_amt);    
    dTx_amt = 0.0;
    get_zd_double("0420",&dTx_amt); 
    vtcp_log("%s,%d 0420  dTx_amt=======[%lf]",__FILE__,__LINE__,dTx_amt); 
    
    
    /* 253 312 910 已经设置了金额     */
    if ( memcmp(cmtno,"253",3) && memcmp(cmtno,"910",3) && memcmp(cmtno,"312",3)) 
    {
         /* 802 803 232走帐时，找不到汇票帐号出错  所以特殊处理   2006-11-2 19:51 */        
        if (memcmp(cmtno,"802",3) == 0 || memcmp(cmtno,"803",3) == 0 
         || memcmp(cmtno,"232",3) == 0 || memcmp(cmtno,"812",3) == 0 
         || memcmp(cmtno,"814",3) == 0 )
        {
            /* 802走帐时，找不到汇票帐号出错 所以加上下面几行  2006-11-2 19:51 */
            char cKinbr[BRNO_LEN+1];
            char cTmp_ac_no[20];    
      
            memset(cTmp_ac_no, 0 , sizeof(cTmp_ac_no));
            memset(cKinbr, 0  ,sizeof(cKinbr));
            if (memcmp(cmtno,"802",3) == 0 || memcmp(cmtno,"812",3) == 0 || memcmp(cmtno,"814",3) == 0)  
            {
                iHVGETTAG(hv_pack,"33H",cTmpBuf802,sizeof(cTmpBuf802)); 
               
            }
            else if (memcmp(cmtno,"803",3) == 0)  
            {
                iHVGETTAG(hv_pack,"0B4",cTmpBuf803,sizeof(cTmpBuf803)); 
            }
            else
            {
                iHVGETTAG(hv_pack,"32A",cTmpBuf,sizeof(cTmpBuf));          
            }
            
            vtcp_log("[%s][%d] --- 货币金额 --- cTmpBuf=[%s]\n",__FILE__,__LINE__,cTmpBuf);   
            vtcp_log("[%s][%d] --- 货币金额 --- cTmpBuf802=[%s]\n",__FILE__,__LINE__,cTmpBuf802);  
            vtcp_log("[%s][%d] --- 货币金额 --- cTmpBuf803=[%s]\n",__FILE__,__LINE__,cTmpBuf803);                          
            
            get_zd_data(DC_TX_BR_NO,cKinbr);

            iRet=pub_base_GetParm_Str(cKinbr,PARM_HVQS,cTmp_ac_no);
             if(iRet)
             {
               vtcp_log("%s,%d 没有找到交易机构指定的110户",__FILE__,__LINE__);
               return iRet;
             }
             set_zd_data("1201",cTmp_ac_no);
             set_zd_data("1204","01");
             set_zd_data("1205","2");
             /** dongxy 20070402 增加812和814的情况 **/
             if(memcmp(cmtno,"802",3)==0) /***add by liuyue 20061201****/
                 set_zd_data("0720","2");/* 借贷标志 */
             else if(memcmp(cmtno,"812",3)==0)
             {
                 vtcp_log("[%s][%d]-----------是不是赋值了呢!\n",__FILE__,__LINE__);
                 set_zd_data("0720","2");/* 借贷标志 */
             }
             else if(memcmp(cmtno,"814",3)==0)
             {
                 set_zd_data("0720","1");/* 借贷标志 */
             }
             /* begin add by LiuHuafeng 20080419 232已经特殊处理过，这里不需要处理 */
             else if(memcmp(cmtno,"232",3)==0)
             {
                 char cFd72[2];
                 memset(cFd72, 0 , sizeof(cFd72));
                 get_zd_data("0720",cFd72);
                 vtcp_log("%s,%d 232 上边处理了0720域，值是 %s",__FILE__,__LINE__,cFd72);
             } 
             /* end by LiuHuafeng 20080419 */
             else
                 set_zd_data("0720","1");/* 借贷标志 */
             /* 设置默认2440402，2440401 帐号，不用的交易金额设置为0 */
             memset(cTmp_ac_no, 0 , sizeof(cTmp_ac_no));
             iRet=pub_base_GetParm_Str(cKinbr,PARM_HPQF,cTmp_ac_no);
             if(iRet)
             {
               vtcp_log("%s,%d 没有找到交易机构指定的汇票签发户",__FILE__,__LINE__);
               return iRet;
             }
             set_zd_data("0320",cTmp_ac_no);
             memset(cTmp_ac_no, 0 , sizeof(cTmp_ac_no));
             iRet=pub_base_GetParm_Str(cKinbr,PARM_HPYC,cTmp_ac_no);
             if(iRet)
             {
               vtcp_log("%s,%d 没有找到交易机构指定的汇票移存户",__FILE__,__LINE__);
               return iRet;
             }
             set_zd_data("0330",cTmp_ac_no);
             set_zd_double("0420",0.0);
        }
        else if (memcmp(cmtno,"725",3) == 0)
        {
            /* 725不走这两个科目 一定置为 0 */
            memset(cTmpBuf,0x00,sizeof(cTmpBuf)); 
            vtcp_log("[%s][%d] --- 货币金额 --- cAmount=[%.18s] ",__FILE__,__LINE__,cTmpBuf);  
            set_zd_data("0660","2");
            set_zd_data("0720","1"); 
        }else /* 其他报文取32A域 */
        {
            iHVGETTAG(hv_pack,"32A",cTmpBuf,sizeof(cTmpBuf));
            vtcp_log("[%s][%d] --- 货币金额 --- cAmount=[%.18s] ",__FILE__,__LINE__,cTmpBuf);
        }


        vtcp_log("[%s][%d] --- 货币金额 --- cTmpBuf=[%s] ",__FILE__,__LINE__,cTmpBuf);   
        vtcp_log("[%s][%d] --- 货币金额 --- cTmpBuf802=[%s] ",__FILE__,__LINE__,cTmpBuf802);  
        vtcp_log("[%s][%d] --- 货币金额 --- cTmpBuf803=[%s] ",__FILE__,__LINE__,cTmpBuf803);  
        /** dongxy 20070402 增加812和814的情况 **/
        if (memcmp(cmtno,"802",3) == 0 || memcmp(cmtno,"812",3) == 0 || memcmp(cmtno,"814",3) == 0)  
        {
            memcpy(cAmount,cTmpBuf802,15);          /* 货币金额 15位 */
        }else if (memcmp(cmtno,"803",3) == 0)  
        {
            memcpy(cAmount,cTmpBuf803,22);          /* 货币金额 22位 */
        }
        else
        {
            memcpy(cAmount,cTmpBuf+3,15);            /*货币金额*/
        }

        vtcp_log(" --- 货币金额 --- cAmount=[%.15s] ",cAmount);    
        dot_str2dbl(cAmount ,sizeof(cAmount),&amount_v);/* 交易金额  */    
        amount_v = amount_v/100;       
        vtcp_log(" --- 货币金额 --- amount_v=[%lf] ",amount_v);    
        set_zd_double("0390", amount_v );   /* 金额 */      
        set_zd_double("0400", amount_v);  
    }
    
    set_zd_data("0510",cmtno);         

    if ( hangflg == 1 )  /* 挂帐 */
    {
        strcpy( g_pub_tx.reply, "0020" );  
        vtcp_log(" --- 挂帐 --- =[%s][%d] ",__FILE__,__LINE__);        
    }
    else
    {
        strcpy( g_pub_tx.reply, "0000" );
        vtcp_log(" --- 检查正常 --- =[%s][%d] ",__FILE__,__LINE__);           
    }
    
    set_zd_data("0210","01");
    
    /* For Test  Print 39 40 */  
    get_zd_double("0390",&dTx_amt); 
    vtcp_log("%s,%d 0390  dTx_amt=======[%lf]",__FILE__,__LINE__,dTx_amt);
    dTx_amt = 0.0;
    get_zd_double("0400",&dTx_amt); 
    vtcp_log("%s,%d 0400  dTx_amt=======[%lf]",__FILE__,__LINE__,dTx_amt);    
    dTx_amt = 0.0;
    get_zd_double("0420",&dTx_amt); 
    vtcp_log("%s,%d 0420  dTx_amt=======[%lf]",__FILE__,__LINE__,dTx_amt); 

    return 0;
}

/***************************************************************************/
/* 函数名称: pubf_hv_handle_fzf_cmt()                                      */
/* 功能描述: 非支付来帐: 不走帐                                            */
/* 参数:                 直接跳到流程的第4步，插入 hv_fzf 和 hv_fzfhtr 表  */
/* 返回值:   0/-1                                                          */
/* 创建日期: 2006.08.18                                                    */
/* 作者:     ChenMing                                                      */
/* 说明:                                                                   */
/***************************************************************************/
int pubf_hv_handle_fzf_cmt(char* cmtno)
{
    strcpy( g_pub_tx.reply, "0010" );  
    set_zd_data("0510",cmtno);    
    return 0;    
}

/***************************************************************************/
/* 函数名称: pub_hv_batch_rcv()                                            */
/* 功能描述: 大额来帐，批量业务报文处理                                    */
/*           分析批量支付报文, 循环获取批量报文记录，插入相应表            */
/* 参数:                                                                   */
/* 返回值:   0/-1                                                          */
/* 创建日期: 2006.08.18                                                    */
/* 作者:     ChenMing                                                      */
/* 说明:                                                                   */
/***************************************************************************/
int pub_hv_batch_rcv(THVPACKAGE* phv_pack,char* cmtno)
{
    THVPACKAGE hvpack_fetch;    
    int position = 69;
    int iRet = 0;

    if ( split_batchcmt(phv_pack,position) )
    {
        vtcp_log("[%s][%d] split_batchcmt() 分析批量支付报文错误",__FILE__,__LINE__);
        sprintf(acErrMsg," 大额来帐报文批量支付报文分析错误 !  ");
        MBFEWRITEMSG
            return -1;        
    }
    while (1)
    {
        memset(&hvpack_fetch, 0 ,sizeof(hvpack_fetch));    
        iRet = hv_fetchpkg( phv_pack, &hvpack_fetch );
        vtcp_log("[%s][%d] iRet====[%d]",__FILE__,__LINE__,iRet);
        
        if (iRet == 1) /*Fetch完毕*/
        {
            vtcp_log("[%s][%d] iRet====[%d]",__FILE__,__LINE__,iRet);
            break;
        }      
        if (iRet == -1)
        {
            sprintf(acErrMsg,"解析报文错误! ");
            MBFEWRITEMSG
            return -1;           
        }
        iRet = pubf_hv_ins_hv_chkagemr( &hvpack_fetch );
        if (iRet)
        {
            sprintf(acErrMsg,"插入 hv_chkagemr 错误! [%d]  ", iRet);
            MBFEWRITEMSG
            return -1;   
        }
    } /*end while(1)*/         
    return 0;  
}

/***************************************************************************/
/* 函数名称: pub_hv_Notbatch_rcv()                                         */
/* 功能描述: 大额来帐，非批量业务报文处理                                  */
/*                                                                         */
/* 参数:                                                                   */
/* 返回值:   0/-1                                                          */
/* 创建日期: 2006.08.18                                                    */
/* 作者:     ChenMing                                                      */
/* 说明:                                                                   */
/***************************************************************************/
int pub_hv_Notbatch_rcv(THVPACKAGE* phv_pack,char* cmtno )
{
    int position = 69;
    int ret = 0;
    /* 分析来帐报文    2. 分析来帐报文业务头块 */      
    ret = split_ophead(phv_pack,position); 
    position = 93;    
    if ( ret == 100 )  /*没有业务头块，直接 split_body */
    {
        position = 69;
    } 
    else if ( ret != 0 ) 
    {
        vtcp_log("[%s][%d] split_ophead 分析来帐报文业务头块错误",__FILE__,__LINE__);
        sprintf(acErrMsg," 大额来帐报文业务头块分析错误 ! ");
        MBFEWRITEMSG
            return -1;        
    }
    /* 分析来帐报文    3. 分析来帐报文正文块块标识 */  
    vtcp_log("[%s][%d] ret=[%d]  position=[%d] ",__FILE__,__LINE__,ret,position);               
    if ( split_body(phv_pack,position) )
    {
        vtcp_log("[%s][%d] split_body 分析来帐报文正文块块标识错误",__FILE__,__LINE__);
        sprintf(acErrMsg," 大额来帐报文正文块块标识分析错误 !  ");
        MBFEWRITEMSG
            return -1;        
    }    
    vtcp_log("[%s][%d]cmtno=[%.3s]",__FILE__,__LINE__,cmtno);

    /* ============== 一些特殊报文的处理 (一) Tag2T 之前   ======================= */
    if (memcmp(cmtno,"417",3) == 0)      /* 公共数据更新报文  不需要登记 */  
    {
        vtcp_log("pub_hv_handle_CMT417() 开始处理公共数据更新报文!! ");
        if ( pub_hv_handle_CMT417(phv_pack))
        {
            sprintf(acErrMsg,"处理 417 公共数据更新报文 出错 ! ");
            MBFEWRITEMSG
                return -1;  
        }
        return 0;  /* 不需要登记*/
    } 
    if ( memcmp(cmtno,"418",3) == 0 )    /* 系统状态变更通知报文  不需要登记*/
    {        
        vtcp_log("pub_hv_handle_CMT418() 开始处理系统状态变更通知报文!! ");
        if ( pub_hv_handle_CMT418(phv_pack) )
        {
            vtcp_log("[%s][%d] pub_hv_handle_CMT418() 处理系统状态变更通知报文错误",__FILE__,__LINE__);
            sprintf(acErrMsg," 处理系统状态变更通知报文错误 !  ");
            MBFEWRITEMSG
                return -1;    
        }        
        return 0;  /* 不需要登记 */
    }
    if (memcmp(cmtno,"841",3) == 0)      /*  登记 841 大额支付业务核对报文的明细 不需要登记 */  
    {
        vtcp_log("pub_hv_chkmr_reg() 开始处理大额支付业务核对报文的明细!! ");
        if ( pub_hv_chkmr_reg(phv_pack))
        {
            sprintf(acErrMsg,"登记841大额支付业务核对报文的明细出错 ! ");
            MBFEWRITEMSG
                return -1;  
        }
        return 0;  /* 不需要登记*/
    }  
      
    /*  需要登记到 hv_zf hv_fzf 的大额来帐的处理   */
    if ( pub_hv_handle_pkg(phv_pack,cmtno) )
    {
        vtcp_log("[%s][%d] pub_hv_handle_pkg() 处理大额来帐错误",__FILE__,__LINE__);
        sprintf(acErrMsg," 处理大额来帐错误 !  ");
        MBFEWRITEMSG
            return -1;                    
    }
    return 0;

}

/************************************************************************* 
* 函 数 名: split_batchcmt                  
* 功能描述： 分析批量业务报文
* 作    者:  ChenMing                             
* 完成日期： 2006-8-20                             
* 修改记录：                                      
*   日   期:                                      
*   修 改 人:                                     
*   修改内容:                                      
*************************************************************************/ 
int split_batchcmt(THVPACKAGE* phv_pack,int position)
{
    char tmpstr[30];
    char *batchcmt;
    
    batchcmt = phv_pack->body + position;
    memset(tmpstr,0x00,sizeof(tmpstr));
    memcpy(tmpstr,batchcmt,3);
    
    if (strcmp(tmpstr,"{B:"))
    {
        sprintf(acErrMsg,"批量业务头块块标识错[%s]",tmpstr);
        MBFEWRITEMSG
            return -1;
    }
    /* 取批量报文中{B:}中的四个字段 */
    memcpy(&(phv_pack->recvDnum),batchcmt+3,10);
    memcpy(&(phv_pack->recvCnum),batchcmt+13,10);
    memcpy(&(phv_pack->sendDnum),batchcmt+23,10);
    memcpy(&(phv_pack->sendCnum),batchcmt+33,10);
    phv_pack->pos = position+44; /*指向第一条记录.*/
    memset(tmpstr,0x00,sizeof(tmpstr));
    memcpy(tmpstr,batchcmt+43,1);
    if (strcmp(tmpstr,"}"))
    {
        sprintf(acErrMsg,"批量业务头块结束标识错[%s]",tmpstr);
        MBFEWRITEMSG
            return -1;
    }
    return 0;
}

/************************************************************************* 
* 函 数 名: hv_fetchpkg(THVPACKAGE *ihv,THVPACKAGE *ohv)                  
* 功能描述： 得到批量业务的下一条记录      成功0，记录结束1，异常-1 
* 作    者:  ChenMing                             
* 完成日期： 2006-8-20                             
* 修改记录：                                      
*   日   期:                                      
*   修 改 人:                                     
*   修改内容:                                      
*************************************************************************/ 
int pub_hv_fetchpkg(THVPACKAGE *ihv,THVPACKAGE *ohv)
{
    char *tmpp1=NULL;
    char *tmpp2=NULL;
    char tmpIdtno[20];

    if ( ihv->pos >= ihv->length )
    {
        printf("[%s][%d]==记录结束.",__FILE__,__LINE__);
        return 1;
    }
    else
    if ( ihv->pos < SENDCNUM_POS+11 ) /* 102 +11 */
    {
        return -1;
    }
    if ( memcmp(ihv->body+ihv->pos,"{2:",3) !=0 )
    {
        printf("[%s][%d]错误的记录.",__FILE__,__LINE__);
        return -1;
    }
    /*给ohv赋值*/
    tmpp1 = ihv->body+ihv->pos;
    tmpp2 = strchr(tmpp1,'}');
    if (tmpp2==NULL)
    {
        printf("[%s][%d]==查找记录出错，记录不完整.",__FILE__,__LINE__);
        return -1;
    }
    tmpp2++;
    tmpp2 = strchr(tmpp2,'}');
    ohv->length = tmpp2-tmpp1+2+PHEADER_END;
    /*取得报文用途、来往标记、业务类型、业务状态码、报文标识号
    　报文参考号、发送时间、CMT号码、工作日期*/
    memcpy(&(ohv->use),ihv->body+USE,PHEADER_END-USE);
    /*取得优先级和最后16位预留位*/
    memcpy(&(ohv->PRI),ihv->body+ihv->pos+6,OPEHEADER_END-PRI_POS);
    memcpy(ohv->CMTCode,ihv->body+ihv->pos+3,3);
    memcpy(ohv->body,tmpp2-ohv->length+1,ohv->length);

    memcpy(ohv->body+USE,&(ohv->use),PHEADER_END-USE);
    ohv->body[PHEADER_END]='}';
    memcpy(ohv->body,"{1:",3);

    memset(tmpIdtno,'\0',sizeof(tmpIdtno));
    sprintf(tmpIdtno,"%06d",ohv->length);
    memcpy(ohv->body+LENGTH,tmpIdtno,6);

    ohv->body[ohv->length]='\0';
    ihv->pos += tmpp2-tmpp1+1;

    return 0;
}

/***************************************************************************/
/* 函数名称: pub_hv_handle_pkg()                                           */
/* 功能描述: 处理大额来帐中的一条记录                                      */
/* 参数:                                                                   */
/* 返回值:   0/-1                                                          */
/* 创建日期: 2006.08.18                                                    */
/* 作者:     ChenMing                                                      */
/* 说明:                                                                   */
/***************************************************************************/
int pub_hv_handle_pkg(THVPACKAGE* phv_pack,char* cmtno)
{
    HV_PAYIN_AREA hv_payin;
    HV_NOTPAY_AREA hv_notpay;    
    int zfflg = 0;
    int ret = 0;
    char cTempstr[1024];

    memset(&hv_payin,0x00,sizeof(HV_PAYIN_AREA));        
    memset(&hv_notpay,0x00,sizeof(HV_NOTPAY_AREA));         
    vtcp_log("[%s][%d]cmtno=[%.3s]",__FILE__,__LINE__,cmtno);
    
    /* 根据cmtno，由 hv_pack 设置 HV_PAYIN_AREA/HV_NOTPAY_AREA的T字段   */      
    ret = pub_hv_set_AREA(phv_pack, cmtno, &zfflg, &hv_payin, &hv_notpay); 
    if (ret<0)
    {
        sprintf(acErrMsg,"设置相应的AREA的T字段时出错! ");
        MBFEWRITEMSG
        return -1;
    }
    
    vtcp_log(">>>phv_payin->T_hv_sts=[%c]begsts=[%c]",hv_payin.T_hv_sts[0],hv_payin.T_beg_sts[0]);TRACE 
    vtcp_log("[%s][%d] 已设置AREA的T字段,此后可操作T字段",__FILE__,__LINE__);
    vtcp_log("[%s][%d] zfflg=[%d]:",__FILE__,__LINE__,zfflg);    
    vtcp_log("%s,%d,hv_notpay->T_orderno   [%.8s]",__FILE__,__LINE__,hv_notpay.T_orderno );
    vtcp_log("%s,%d,hv_notpay->T_or_br_no  [%.12s]",__FILE__,__LINE__,hv_notpay.T_or_br_no);
    vtcp_log("%s,%d,hv_notpay->T_wt_date   [%s]",__FILE__,__LINE__,hv_notpay.T_wt_date );

    vtcp_log("%s,%d,hv_payin->T_orderno   [%.8s]",__FILE__,__LINE__,hv_payin.T_orderno );
    vtcp_log("%s,%d,hv_payin->T_or_br_no  [%.12s]",__FILE__,__LINE__,hv_payin.T_or_br_no);
    vtcp_log("%s,%d,hv_payin->T_wt_date   [%s]",__FILE__,__LINE__,hv_payin.T_wt_date );

    vtcp_log("%s,%d,  [%.7s]",__FILE__,__LINE__,hv_notpay.T_br_no );
    vtcp_log("[%s][%d]hv_notpay.T_cmtno=[%.3s]",__FILE__,__LINE__,hv_notpay.T_cmtno);        
    /*  ==============支付 与 非支付 分别处理 ================== */
    if ( zfflg == 1)  /* 支付: 登记大额支付来帐 hv_zf 和 hv_zfhtr  */
    {
        /*   给一些字段赋值   */            
        get_fd_data("0030",hv_payin.T_tx_br_no);   /*   机构号   */
        /* begin add by LiuHuafeng 2009-5-13 20:53:22 增加对村镇银行处理 */
        {
            int iRet=0;
            char cReply[5];
            struct hv_orno_brno_c sOrno_brno;
            memset(&sOrno_brno , 0 , sizeof(sOrno_brno));
            memset(cReply, 0 , sizeof(cReply));
            strncpy(sOrno_brno.or_br_no,hv_payin.T_ac_br_no,sizeof(sOrno_brno.or_br_no)-1);
            iRet=Hv_orno_brno_Sel(cReply,&sOrno_brno,"or_br_no='%s'",sOrno_brno.or_br_no);
            if (iRet==0)
            {
		vtcp_log("[%s][%d] br_no=[%s] T_br_no=[%s]",__FILE__,__LINE__,sOrno_brno.br_no,hv_notpay.T_br_no);
                if(strncmp(sOrno_brno.br_no,"9999",4)==0)/**xyy机构号头4位为9999,为村镇银行**/
                {
                    strncpy(hv_payin.T_tx_br_no,sOrno_brno.br_no,BRNO_LEN);
                    vtcp_log("%s,%d 村镇银行，将机构设置为村镇银行号",__FILE__,__LINE__);
                }
            }
            
        }
        /* end by LiuHuafeng 2009-5-13 20:53:51 */
        get_fd_data("0040",hv_payin.T_trace_no);   /*   流水号   */
        
        get_fd_data("0050",hv_payin.T_tx_date);    /*   交易日期   */
        
        get_fd_data("0060",hv_payin.T_tx_time);    /*   交易时间   */
        get_fd_data("0070",hv_payin.T_tel);        /*   柜员号   */

        get_fd_data("0980",hv_payin.T_beg_sts);    /*   beg_sts   */
        hv_payin.T_hv_sts[0]=hv_payin.T_beg_sts[0];/* add by LiuHuafeng 2006-10-1 22:18 */
        hv_payin.T_hv_prt_ind[0] = '0';       /*   打印标志 置 0    */
        hv_payin.T_lw_ind[0] = '2';           /*   来往标志 置 2     */
        memcpy( hv_payin.T_refno, phv_pack->reNo, sizeof(hv_payin.T_refno) ); /*   报文参考号   */

        vtcp_log("[%s][%d] T_tx_time=[%.6s]:",__FILE__,__LINE__,hv_payin.T_tx_time);    
        vtcp_log("[%s][%d] T_hv_prt_ind=[%.1s]:",__FILE__,__LINE__,hv_payin.T_hv_prt_ind);    
        
        memset(cTempstr,0,sizeof(cTempstr));
        memcpy(cTempstr, hv_payin.T_pay_name, sizeof(hv_payin.T_pay_name));
        vtcp_log("[%s][%d] hv_payin.T_pay_name==[%.60s] ",__FILE__,__LINE__,hv_payin.T_pay_name); 
        dbc_zipspace(cTempstr);  /*压缩全角空格 2006-11-7 23:45  */
        memcpy(hv_payin.T_pay_name, cTempstr, sizeof(hv_payin.T_pay_name));
        vtcp_log("[%s][%d] hv_payin.T_pay_name==[%.60s] ",__FILE__,__LINE__,hv_payin.T_pay_name);

        vtcp_log("[%s][%d] hv_payin.T_cash_name==[%.60s] ",__FILE__,__LINE__,hv_payin.T_cash_name); 
        memset(cTempstr,0,sizeof(cTempstr));
        memcpy(cTempstr, hv_payin.T_cash_name, sizeof(hv_payin.T_cash_name));        
        dbc_zipspace(cTempstr);  /*压缩全角空格 2006-11-7 23:45  */
        memcpy(  hv_payin.T_cash_name , cTempstr, sizeof(hv_payin.T_cash_name));        
        vtcp_log("[%s][%d] hv_payin.T_cash_name==[%.60s] ",__FILE__,__LINE__,hv_payin.T_cash_name);

        /* ============== 一些特殊报文的处理 (二) Tag2T 之后   ======================= */
        if ( memcmp(cmtno, "802", 3) == 0)   /*  登记 802 支付业务收费清单报文的明细  */ 
        {
            if ( pub_hv_chargemx_reg(phv_pack, &hv_payin))
            {
                sprintf(acErrMsg,"登记支付业务收费清单报文明细出错 ! ");
                MBFEWRITEMSG
                    return -1;  
            }
        }
        /** dongxy 20070402 增加812和814报文的情况 **/
        if ( memcmp(cmtno, "812", 3) == 0 || memcmp(cmtno, "814", 3) == 0)   /*  登记 81x 支付业务收费清单报文的明细  */ 
        {
            if ( pub_hv_81x_reg(phv_pack, &hv_payin))
            {
                sprintf(acErrMsg,"登记81x支付业务收费清单报文明细出错 ! ");
                MBFEWRITEMSG
                return -1;  
            }
        }
  
        if ( memcmp(cmtno, "108", 3) == 0)   /*  CMT108  退汇支付报文 更改原往帐状态 */ 
        {
            ret = iHv_108_proc(phv_pack, &hv_payin) ;
            if ( ret == 100 ) /*   没有找到原大额支付往帐  */
            {
                vtcp_log("%s,%d, T_orderno=[%s]",__FILE__,__LINE__,hv_payin.T_orderno );
                /* memcpy(hv_payin.T_orderno,"00000000",sizeof(hv_payin.T_orderno));*/
            }else if( ret )
            {
                sprintf(acErrMsg,"退汇支付报文更改原往帐状态出错 ! ");
                MBFEWRITEMSG
                return -1;  
            }
        }
        
     /*  CMT122  银行汇票资金信息报文   CMT123  汇票多余资金划回报文                    */  
     /*  CMT124  汇票资金未用退回报文   更改汇票记录状态                                */   
        if ( memcmp(cmtno, "122", 3)==0||memcmp(cmtno, "123", 3)==0||memcmp(cmtno, "124", 3)==0 ) 
        {
            char cStat[2];
            memset(cStat,0,sizeof(cStat));
            get_fd_data("0980",cStat);
            
            vtcp_log("%s,%d, cStat= [%s]",__FILE__,__LINE__, cStat ); 
            if(cStat[0] != 'I' && cStat[0] != 'H' ) /* 挂帐就不用再处理汇票状态了 */
            {
                ret = iHv_HPCMT_proc(phv_pack) ;
                if ( ret == 100 ) /*   没有找到汇票记录  */
                {
                    vtcp_log("%s,%d, 没有找到汇票记录 继续登记来帐 ! ",__FILE__,__LINE__);
                }else if( ret )
                {
                    sprintf(acErrMsg,"退汇支付报文更改原往帐状态出错 ! ");
                    MBFEWRITEMSG
                    return -1;  
                }        
                vtcp_log("[%s,%d]>>>iHv_HPCMT_proc()  Finished !!! ",__FILE__,__LINE__); 
            }               
        }
        
        vtcp_log(">>>phv_payin->T_hv_sts=[%c]begsts=[%c]",hv_payin.T_hv_sts[0],hv_payin.T_beg_sts[0]); 
        /* ----------  登记大额支付来帐: hv_zf hv_zfhtr  ---------*/
        if (pub_hv_ins_zf(&hv_payin))
        {
            sprintf(acErrMsg,"登记大额支付来帐错误 ! ");
            MBFEWRITEMSG
                return -1;  
        }
    }
    else /*  非支付: 登记大额非支付来帐 hv_fzf 和 hv_fzfhtr */
    {
        /* get_fd_data("0030",hv_notpay.T_br_no);     机构号   */
        vtcp_log("%s,%d,  [%.12s]",__FILE__,__LINE__,hv_notpay.T_ac_br_no );        
        char cAc_br_no[13];
        char cBr_no[8];
        
        memset(cAc_br_no,0,sizeof(cAc_br_no));
        memset(cBr_no,0,sizeof(cBr_no));      
        memcpy(cAc_br_no, hv_notpay.T_ac_br_no, sizeof(cAc_br_no)-1);
        vtcp_log("%s,%d, T_cmtno= [%.12s]",__FILE__,__LINE__,hv_notpay.T_cmtno );          
        ret = GetBr_noByOr_br_no(cAc_br_no,cBr_no);
        if(ret)
        { 
           /* 没有找到cAc_br_no对应的机构号 
           memcpy(cBr_no,QS_BR_NO,sizeof(cBr_no)-1);*/
	   memcpy(cBr_no,QS_BR_NO,sizeof(cBr_no)-1);
        }
        vtcp_log("%s,%d, T_cmtno= [%.12s]",__FILE__,__LINE__,hv_notpay.T_cmtno );          
        memcpy(hv_notpay.T_br_no, cBr_no, sizeof(hv_notpay.T_br_no));       
        vtcp_log("%s,%d, cBr_no== [%.12s]",__FILE__,__LINE__,cBr_no );        
        vtcp_log("%s,%d, hv_notpay.T_br_no== [%.7s]",__FILE__,__LINE__,hv_notpay.T_br_no );      
        vtcp_log("%s,%d, T_cmtno= [%.12s]",__FILE__,__LINE__,hv_notpay.T_cmtno );  

        vtcp_log("[%s][%d] hv_notpay.T_req_content==[%.60s] ",__FILE__,__LINE__,hv_notpay.T_req_content); 
        memset(cTempstr,0,sizeof(cTempstr));
        memcpy(cTempstr, hv_notpay.T_req_content, sizeof(hv_notpay.T_req_content));   
        dbc_zipspace(cTempstr);  /*压缩全角空格 2006-11-7 23:45  */
        memcpy( hv_notpay.T_req_content, cTempstr, sizeof(hv_notpay.T_req_content)); 
        vtcp_log("[%s][%d] hv_notpay.T_req_content==[%.60s] ",__FILE__,__LINE__,hv_notpay.T_req_content);

        vtcp_log("[%s][%d] hv_notpay.T_res_content==[%.60s] ",__FILE__,__LINE__,hv_notpay.T_res_content); 
        memset(cTempstr,0,sizeof(cTempstr));
        memcpy(cTempstr, hv_notpay.T_res_content, sizeof(hv_notpay.T_res_content));  
        dbc_zipspace(cTempstr);  /*压缩全角空格 2006-11-7 23:45  */
        memcpy( hv_notpay.T_res_content, cTempstr, sizeof(hv_notpay.T_res_content)); 
        vtcp_log("[%s][%d] hv_notpay.T_res_content==[%.60s] ",__FILE__,__LINE__,hv_notpay.T_res_content);

        get_fd_data("0040",hv_notpay.T_trace_no);   /*   流水号   */                  
        get_fd_data("0050",hv_notpay.T_tx_date);    /*   交易日期   */           
        get_fd_data("0060",hv_notpay.T_tx_time);    /*   交易时间   */
        get_fd_data("0070",hv_notpay.T_tel);        /*   柜员号   */

        vtcp_log("%s,%d,  [%.8s]",__FILE__,__LINE__,hv_notpay.T_orderno );
        vtcp_log("%s,%d,  [%.12s]",__FILE__,__LINE__,hv_notpay.T_or_br_no);
        vtcp_log("%s,%d,  [%s]",__FILE__,__LINE__,hv_notpay.T_wt_date );
        vtcp_log("%s,%d,  [%s]",__FILE__,__LINE__,hv_notpay.T_o_tx_amt );
        vtcp_log("%s,%d, T_cmtno= [%.12s]",__FILE__,__LINE__,hv_notpay.T_cmtno );  
        
        vtcp_log("[%s][%d] hv_notpay.T_req_br_no=[%.12s] ",__FILE__,__LINE__,hv_notpay.T_req_br_no);  
        vtcp_log("[%s][%d] hv_notpay.T_req_date=[%.8s] ",__FILE__,__LINE__,hv_notpay.T_req_date);  
        vtcp_log("[%s][%d] hv_notpay.T_req_orderno=[%.8s] ",__FILE__,__LINE__,hv_notpay.T_req_orderno);  
       
        hv_notpay.T_hv_prt_ind[0] = '0';
        /* ============== 一些特殊报文的处理 (二) Tag2T 之后   ======================= */  
        /* 253  910 报文的特殊处理 */  
        if ( memcmp(cmtno, "302", 3) == 0)   /*  CMT302  查复报文 更改原往帐状态 */ 
        {
            ret = iHv_302_proc(&hv_notpay) ;
            if ( ret == 100 ) /*   没有找到原查询记录  */
            {
                vtcp_log("%s,%d, 没有找到原查询记录 往下走登记 302 ! ",__FILE__,__LINE__);
            }else if( ret )
            {
                sprintf(acErrMsg,"查复报文更改原查询记录状态出错 ! ");
                MBFEWRITEMSG
                return -1;  
            }
            else
            {
                vtcp_log("%s,%d, 找到原查询记录 处理完毕 ! ",__FILE__,__LINE__);
                return 0;     /* 不插入 hv_fzf hv_fzfhtr */              
            }
        }         
                   
                     
        if ( (memcmp(cmtno,"253",3) == 0) || (memcmp(cmtno,"910",3) == 0) )
        {
            if (iHv253_910_proc( &hv_notpay ))
            {
                vtcp_log(" 253 910 报文的特殊处理出错 " );
                sprintf(acErrMsg,"253 910 报文的特殊处理出错! ");
                WRITEMSG
                return -1;
            } 
            return 0;     /* 不插入 hv_fzf hv_fzfhtr */          
        }
        
        /* 312 报文的特殊处理*/
        if ( memcmp(cmtno,"312",3) == 0 )  
        {
            vtcp_log("%s,%d,hv_notpay->T_ac_br_no  [%.12s]",__FILE__,__LINE__,hv_notpay.T_ac_br_no);          
            vtcp_log("%s,%d,hv_notpay->T_or_br_no  [%.12s]",__FILE__,__LINE__,hv_notpay.T_or_br_no);          
            ret = iHv_312_proc( &hv_notpay);
            /* if(ret == 0)    return 0; 更新了 hv_fzf 不用插入 */  
            if (ret != 0 && ret != 100)
            {
                vtcp_log(" 312 报文的特殊处理出错 " );
                sprintf(acErrMsg,"312 报文的特殊处理出错! ");
                WRITEMSG
                    return -1;
            } 
            /* ret == 100 时，没有查到相应的311， 则需要插入hv_fzf hv_fzfhtr  */
        } 
        
        /* 314 报文的特殊处理*/
        if ( memcmp(cmtno,"314",3) == 0 )  
        {
            ret = iHv_314_proc( &hv_notpay );
            if (ret == 0)    return 0;  /* 更新了 hv_fzf 不用插入 */  
            if (ret != 0 && ret != 100)
            {
                vtcp_log(" 314 报文的特殊处理出错 " );
                sprintf(acErrMsg,"314 报文的特殊处理出错! ");
                WRITEMSG
                    return -1;
            } 
            /* ret == 100 时，没有查到相应的313，则需要插入hv_fzf hv_fzfhtr */
        }  
        
         /* 725  汇票全额兑付通知报文  更改汇票记录状态    */   
        if ( memcmp(cmtno, "725", 3)==0 ) 
        {
            ret = iHv_HPCMT_proc(phv_pack) ;
            if ( ret == 100 ) /*   没有找到汇票记录  */
            {
                vtcp_log("%s,%d, 没有找到汇票记录 继续登记来帐 ! ",__FILE__,__LINE__);
            }else if( ret )
            {
                sprintf(acErrMsg,"退汇支付报文更改原往帐状态出错 ! ");
                MBFEWRITEMSG
                return -1;  
            }
            char cStat;
            get_zd_data("0980",&cStat);
            hv_notpay.T_hv_fzf_sts[0] = cStat;  
        }
        /* ----------  登记大额非支付来帐: hv_fzf hv_fzfhtr  ---------*/
        hv_notpay.T_otype[0] = '1';            /*   大额标志    */              
        hv_notpay.T_hv_prt_ind[0] = '0';       /*   打印标志 置 0    */
        hv_notpay.T_lw_ind[0] = '2';           /*   来往标志 置 2     */
        memcpy( hv_notpay.T_refno, phv_pack->reNo, sizeof(hv_notpay.T_refno) ); /*   报文参考号   */
        vtcp_log("%s,%d, phv_notpayin->T_br_no= [%.7s]",__FILE__,__LINE__,hv_notpay.T_br_no );
        vtcp_log("%s,%d,  [%s]",__FILE__,__LINE__,hv_notpay.T_orderno );
        vtcp_log("%s,%d,  [%s]",__FILE__,__LINE__,hv_notpay.T_or_br_no);
                
        /* pub_hv_print_NOTPAY(&hv_notpay);   */
        if (pub_hv_ins_fzf(&hv_notpay))
        {
            sprintf(acErrMsg,"登记大额非支付来帐错误 ! ");
            MBFEWRITEMSG
                return -1;  
        }                                   
    }   
    return 0;  
}
/***************************************************************************/
/* 函数名称: iHv_108_proc()                                                */
/* 功能描述: 退汇支付报文 更改原往帐状态                                   */
/* 参数:                                                                   */
/* 返回值:   0/-1                                                          */
/* 创建日期: 2006.08.18                                                    */
/* 作者:     ChenMing                                                      */  
/* 说明:                                                                   */   
/***************************************************************************/
int iHv_108_proc( THVPACKAGE* phv_pack, HV_PAYIN_AREA* phv_payin)
{
    int ret;
    struct hv_zf_c hv_zf;

    char cCmtno[4];
    char cWt_date[9];
    char cOrderno[9];
    char cOr_br_no[13];
    memset(cCmtno, 0, sizeof(cCmtno) );
    memset(cWt_date, 0, sizeof(cWt_date) );
    memset(cOrderno, 0, sizeof(cOrderno) );            
    memset(cOr_br_no, 0, sizeof(cOr_br_no) );
    memset(&hv_zf, 0, sizeof(struct hv_zf_c) );    

    memcpy(cCmtno,phv_payin->T_o_cmtno,sizeof(cCmtno)-1 );
    memcpy(cWt_date,phv_payin->T_o_wt_date,sizeof(cWt_date)-1 );    
    memcpy(cOrderno,phv_payin->T_o_orderno,sizeof(cOrderno)-1 );    /*原支付交易序号*/
    memcpy(cOr_br_no,phv_payin->T_ac_br_no,sizeof(cOr_br_no)-1 );    /* 退汇交易的原接收行 */

    vtcp_log(" cCmtno===[%s] ",cCmtno);
    vtcp_log(" cWt_date===[%s] ",cWt_date);
    vtcp_log(" cOrderno===[%s] ",cOrderno);
    vtcp_log(" cOr_br_no===[%s] ",cOr_br_no);

    ret = Hv_zf_Dec_Upd(g_pub_tx.reply," cmtno='%s' and or_br_no='%s' and wt_date=%d and orderno='%s' ", \
        cCmtno, cOr_br_no, apatoi(cWt_date,8), cOrderno);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }
    ret = Hv_zf_Fet_Upd(&hv_zf, g_pub_tx.reply);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"查询支付交易表错误!![%d]",ret);
        WRITEMSG
        return ret;
    }
    else if (ret == 100)
    {
        vtcp_log("[%s][%d] 支付交易表中不存在该记录",__FILE__,__LINE__);
        return ret; /* 没找到则忽略 不报错 */
    }    
    hv_zf.hv_sts[0] = SEND_BACK_ACCT;

    ret = Hv_zf_Upd_Upd(hv_zf, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE COM_BRANCH ERROR ! [%d]",ret);
        WRITEMSG
        return ret;
    } 
       
    Hv_zf_Clo_Upd();  
    vtcp_log("[%s][%d] 更新支付交易表记录成功",__FILE__,__LINE__);    
    return 0;
}


/***************************************************************************/
/* 函数名称: iHv_302_proc()                                                 */
/* 功能描述:                            */
/* 参数:                                                                   */
/* 返回值:   0/-1                                                          */
/* 创建日期: 2006.08.18                                                    */
/* 作者:     ChenMing                                                      */  
/* 说明:                                                                   */   
/***************************************************************************/
int iHv_302_proc( HV_NOTPAY_AREA* phv_notpay )
{
    int ret;
    struct hv_fzf_c hv_fzf;
    char cOr_br_no[13];
    char cWt_date[9];
    char cOrderno[9];

    memset(cOr_br_no, 0, sizeof(cOr_br_no));
    memset(cWt_date, 0, sizeof(cWt_date));   
    memset(cOrderno, 0, sizeof(cOrderno));    

    vtcp_log("[%s][%d] phv_notpay->T_req_br_no=[%.12s] ",__FILE__,__LINE__,phv_notpay->T_req_br_no);  
    vtcp_log("[%s][%d] phv_notpay->T_req_date=[%.8s] ",__FILE__,__LINE__,phv_notpay->T_req_date);  
    vtcp_log("[%s][%d] phv_notpay->T_req_orderno=[%.8s] ",__FILE__,__LINE__,phv_notpay->T_req_orderno);  
      

    memcpy( cOr_br_no,phv_notpay->T_req_br_no,sizeof(cOr_br_no)-1 );  /*原发起行行号*/
    memcpy( cWt_date,phv_notpay->T_req_date,sizeof(cWt_date)-1 );      /*原查询日期*/
    memcpy( cOrderno,phv_notpay->T_req_orderno,sizeof(cOrderno)-1 );   /*原查询书号*/

    vtcp_log(" cOr_br_no=[%d] ", cOr_br_no);  
    vtcp_log(" cWt_date=[%s] ", cWt_date);     
    vtcp_log(" cOrderno=[%s] ", cOrderno);    

    /* 查找hv_fzf表，如果有相应的301就更新， 否则插入 */    

    /* ret = Hv_fzf_Dec_Upd(g_pub_tx.reply, "otype='1' and lw_ind='1' and or_br_no='%s' and wt_date=%d and orderno='%s' and cmtno='%s'", \
        cOr_br_no, atoi(cWt_date), cOrderno,"301");    改成全行唯一非支付交易序号   2006-11-8 19:10 */
    ret = Hv_fzf_Dec_Upd(g_pub_tx.reply, "otype='1' and pay_qs_no='%s' and orderno='%s' and cmtno='%s' ", \
          HV_QS_BR_NO, cOrderno,"301");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        WRITEMSG
            return ret;
    }
    
    memset(&hv_fzf, 0, sizeof(hv_fzf) );
    ret = Hv_fzf_Fet_Upd(&hv_fzf, g_pub_tx.reply);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"查询非支付交易表错误!![%d]",ret);
        WRITEMSG
            return ret;
    }
    else if (ret == 100)
    {
        vtcp_log("[%s][%d] 没有找到对应的 301 报文 !!! ",__FILE__,__LINE__);
        sprintf(acErrMsg," 没有找到对应的 301 报文 !!");
        WRITEMSG
            strcpy(g_pub_tx.reply,"O127");
        return ret;
    }    
    
    if(hv_fzf.hv_fzf_sts[0] == SEND_ACCT_QS)
    {
        vtcp_log("[%s][%d] 该查询书已做查复 !!! ",__FILE__,__LINE__);            
        sprintf(acErrMsg," 该查询书已做查复 !!");
        WRITEMSG
        return -1;
    }
    
    vtcp_log("[%s][%d] 找到了对应的 301 报文 !!! ",__FILE__,__LINE__);
    hv_fzf.res_date = apatoi(phv_notpay->T_wt_date,8); 
    memcpy( hv_fzf.res_br_no, phv_notpay->T_or_br_no, sizeof(hv_fzf.res_br_no)-1 );
    memcpy( hv_fzf.res_content, phv_notpay->T_res_content, sizeof(hv_fzf.res_content)-1 );
    memcpy( hv_fzf.res_orderno, phv_notpay->T_orderno, sizeof(hv_fzf.res_orderno)-1 );
    memcpy( hv_fzf.res_sts, phv_notpay->T_hv_fzf_sts, sizeof(hv_fzf.res_sts)-1 );            
    hv_fzf.hv_fzf_sts[0] = SEND_ACCT_QS; 
    hv_fzf.hv_prt_ind[0] = '0';     

    vtcp_log(" hv_fzf.res_date=[%d] ", hv_fzf.res_date);  
    vtcp_log(" hv_fzf.res_date=[%s] ", hv_fzf.res_br_no);        
    vtcp_log(" hv_fzf.res_date=[%s] ", hv_fzf.res_orderno);          
    vtcp_log(" hv_fzf.res_date=[%s] ",hv_fzf.res_sts);  
    vtcp_log(" hv_fzf.hv_fzf_sts=[%s] ",hv_fzf.hv_fzf_sts);     

    ret = Hv_fzf_Upd_Upd(hv_fzf, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE COM_BRANCH ERROR ! [%d]",ret);
        WRITEMSG
            return ret;
    }    
    
    Hv_fzf_Clo_Upd();  
    vtcp_log("[%s][%d] 更新非支付交易表记录成功",__FILE__,__LINE__);  
    return 0;
}

/***************************************************************************/
/* 函数名称: iHv_312_proc()                                                 */
/* 功能描述:                            */
/* 参数:                                                                   */
/* 返回值:   0/-1                                                          */
/* 创建日期: 2006.08.18                                                    */
/* 作者:     ChenMing                                                      */  
/* 说明:                                                                   */   
/***************************************************************************/
int iHv_312_proc( HV_NOTPAY_AREA* phv_notpay)
{
    int ret;
    struct hv_fzf_c hv_fzf;
    char cOr_br_no[13];
    char cWt_date[9];
    char cOrderno[9];
    struct hv_zf_c sHv_zf;

    memset(cOr_br_no, 0, sizeof(cOr_br_no));
    memset(cWt_date, 0, sizeof(cWt_date));   
    memset(cOrderno, 0, sizeof(cOrderno));    

    vtcp_log("%s,%d,hv_notpay->phv_notpay  [%.12s]",__FILE__,__LINE__,phv_notpay->T_ac_br_no);          
    vtcp_log("%s,%d,hv_notpay->phv_notpay  [%.12s]",__FILE__,__LINE__,phv_notpay->T_or_br_no);          
    
    memcpy( phv_notpay->T_or_br_no,phv_notpay->T_ac_br_no,sizeof(phv_notpay->T_or_br_no) ); 
    memcpy( cOr_br_no,phv_notpay->T_ac_br_no,sizeof(cOr_br_no)-1 );  /*原发起行行号*/
    memcpy( cWt_date,phv_notpay->T_wt_date,sizeof(cWt_date)-1 );      /*原查询日期*/
    memcpy( cOrderno,phv_notpay->T_req_orderno,sizeof(cOrderno)-1 );   /*原查询书号*/

    vtcp_log(" cOr_br_no=[%d] ", cOr_br_no);  
    vtcp_log(" cWt_date=[%s] ", cWt_date); 
    vtcp_log(" cOrderno=[%s] ", cOrderno); 

     /* Add by SSH,2005.1.19, ------------------------------------------
     如果收到允许撤销的报文，应该更改原交易状态并冲账
    如果不允许撤销，则不更改原交易状态而使它保持撤销等待，因为此种状态也可以
    接收CMT253报文，如果一直收不到CMT253报文，因为夜间对账之前会把所有记帐等待和撤销
    等待的记录更新为记帐清算，所以也不会有问题 ----------------------*/

    /* 查找hv_fzf表，如果有相应的311就更新  */    
    /*ret = Hv_fzf_Dec_Upd(g_pub_tx.reply, "otype='1'  and lw_ind='1' and or_br_no='%s' and wt_date=%d and orderno='%s' and cmtno='%s'", \
        cOr_br_no, apatoi(cWt_date,8), cOrderno, "311");  改成全行唯一非支付交易序号  2006-11-8 19:11  */
    ret = Hv_fzf_Dec_Upd(g_pub_tx.reply, "otype='1' and pay_qs_no='%s' and orderno='%s' and cmtno='%s'", \
         HV_QS_BR_NO, cOrderno, "311");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        WRITEMSG
            return ret;
    }
    memset(&hv_fzf, 0, sizeof(hv_fzf) );    
    ret = Hv_fzf_Fet_Upd(&hv_fzf, g_pub_tx.reply);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"查询非支付交易表错误!![%d]",ret);
        WRITEMSG
        return ret;
    }
    else if (ret == 100)
    {
        vtcp_log("[%s][%d] 没有找到对应的 311 报文 !!! ",__FILE__,__LINE__);      
        sprintf(acErrMsg,"非支付交易表中不存在该记录!!");
        WRITEMSG
        return ret;
    } 
       
    vtcp_log("[%s][%d] 找到了对应的 311 报文 !!! ",__FILE__,__LINE__);
    if(hv_fzf.hv_fzf_sts[0] != SEND_ACCT_WAIT)
    {
        vtcp_log("[%s][%d] 原撤销申请状态不正确 !!! ",__FILE__,__LINE__);            
        sprintf(acErrMsg," 原撤销申请状态不正确 !!");
        WRITEMSG
        return -1;
    }
    
    hv_fzf.res_date = apatoi(phv_notpay->T_wt_date,8); 
    memcpy( hv_fzf.res_br_no, phv_notpay->T_or_br_no, sizeof(hv_fzf.res_br_no)-1 );
    memcpy( hv_fzf.res_content, phv_notpay->T_res_content, sizeof(hv_fzf.res_content)-1 );
    memcpy( hv_fzf.res_sts, phv_notpay->T_respcode, sizeof(hv_fzf.res_sts)-1 );   

    if ( phv_notpay->T_respcode[0] == '0' )
    {
        hv_fzf.hv_fzf_sts[0] = SEND_ACCT_QS; 
    }
    else
    {
        hv_fzf.hv_fzf_sts[0] = SEND_REJECT;    
    }
    
    
    hv_fzf.hv_prt_ind[0] = '0'; 
    vtcp_log(" hv_fzf.res_date=[%d] ", hv_fzf.res_date);  
    vtcp_log(" hv_fzf.res_br_no=[%s] ", hv_fzf.res_br_no);        
    vtcp_log(" hv_fzf.res_orderno=[%s] ", hv_fzf.res_orderno);          
    vtcp_log(" hv_fzf.res_sts=[%s] ",hv_fzf.res_sts);  
    vtcp_log(" hv_fzf.hv_fzf_sts=[%s] ",hv_fzf.hv_fzf_sts);   

    ret = Hv_fzf_Upd_Upd(hv_fzf, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE COM_BRANCH ERROR ! [%d]",ret);
        WRITEMSG
            return ret;
    }    
    Hv_fzf_Clo_Upd();  
    vtcp_log("[%s][%d] 更新非支付交易表记录成功",__FILE__,__LINE__);

    /* 因为刘岳在做311时没有改变原交易状态，所以“不能撤销”时不改变原支付交易状态 */
    if(phv_notpay->T_respcode[0] != '0')
    {
        return 0;
    }

    /* 同意撤销: 再根据311找到原支付交易 设置相应状态  */    
    ret=Hv_zf_Dec_Upd(g_pub_tx.reply,"orderno='%s' and or_br_no='%s' and wt_date=%d",
    hv_fzf.o_orderno,hv_fzf.o_or_br_no,hv_fzf.o_wt_date);
    if(ret)
    {
      vtcp_log("%s,%d 支付交易表游标定义错误 ret=[%d]",__FILE__,__LINE__,ret);
      vtcp_log(acErrMsg,"支付交易表游标定义错误!");
      WRITEMSG
      return ret;
    }
    memset(&sHv_zf, 0 , sizeof(sHv_zf));     
    ret=Hv_zf_Fet_Upd(&sHv_zf,g_pub_tx.reply);
    if(ret)
    {
      vtcp_log("%s,%d 读取支付交易表错误 ret=[%d]",__FILE__,__LINE__,ret);
      vtcp_log(acErrMsg,"读取支付交易表错误!");
      WRITEMSG
      return ret;
    }
    
/*  if( sHv_zf.hv_sts[0]==SEND_REJECT || sHv_zf.hv_sts[0]==SEND_ACCT_QS 
     || sHv_zf.hv_sts[0]==SEND_CANCEL_ACCT || sHv_zf.hv_sts[0]== SEND_BACK_ACCT)
    {
      vtcp_log("当前状态是最终状态[%c]不需要再处理",sHv_zf.hv_sts[0]);
    }
    else
    {
      if(phv_notpay->T_resp1[3]!='I')
      {
        if(memcmp(phv_notpay->T_resp1,"CS2O9010",8))
        {
          sHv_zf.hv_sts[0]=SEND_REJECT;
          iHvChangeLzStatForTh(&sHv_zf,'@');
        }
        else
          sHv_zf.hv_sts[0]=SEND_INQUEUE;
      }
      else
      {
        sHv_zf.hv_sts[0]=SEND_ACCT_QS;
        iHvChangeLzStatForTh(&sHv_zf,RECEIVE_BACK_ACCT);
      }
      memcpy(sHv_zf.resp1,phv_notpay->T_resp1,sizeof(sHv_zf.resp1)-1);
    }     */

    sHv_zf.hv_sts[0] = SEND_CANCEL_ACCT;  /*往账撤销记账*/
    ret=Hv_zf_Upd_Upd(sHv_zf,g_pub_tx.reply);
    if(ret)
    {
      vtcp_log("%s,%d 更新支付交易表错误 ret=[%d]",__FILE__,__LINE__,ret);
      vtcp_log(acErrMsg,"更新支付交易表错误");
      WRITEMSG
      return ret;
    }    

    vtcp_log("[%s][%d] 更新原支付交易记录状态成功",__FILE__,__LINE__);
    return 0;
}

/***************************************************************************/
/* 函数名称: iHv_314_proc()                                                */
/* 功能描述:                            */
/* 参数:                                                                   */
/* 返回值:   0/-1                                                          */
/* 创建日期: 2006.08.18                                                    */
/* 作者:     ChenMing                                                      */  
/* 说明:                                                                   */   
/***************************************************************************/
int iHv_314_proc( HV_NOTPAY_AREA* phv_notpay )
{
    int ret;
    struct hv_fzf_c hv_fzf;
    char cOr_br_no[13];
    char cWt_date[9];
    char cOrderno[9];

    memset(cOr_br_no, 0, sizeof(cOr_br_no));
    memset(cWt_date, 0, sizeof(cWt_date));   
    memset(cOrderno, 0, sizeof(cOrderno));    

    memcpy( cOr_br_no,phv_notpay->T_ac_br_no,sizeof(cOr_br_no)-1 );  /*原发起行行号*/
    memcpy( cWt_date,phv_notpay->T_req_date,sizeof(cWt_date)-1 );      /*原退回申请日期*/
    memcpy( cOrderno,phv_notpay->T_req_orderno,sizeof(cOrderno)-1 );   /*原退回申请序号*/

    /* 查找hv_fzf表，如果有相应的313就更新， 否则插入     
    ret = Hv_fzf_Sel( g_pub_tx.reply, &hv_fzf,"otype='1' and lw_ind='1' and or_br_no='%s' and wt_date=%d and orderno='%s' and cmtno='%s'", \
        cOr_br_no, atoi(cWt_date), cOrderno,"313");
    if (ret == 100)
    {
        vtcp_log("[%s][%d] 没有找到对应的 313 报文 !!! ",__FILE__,__LINE__);      
        return ret;
    }
    else
    if (ret!=0)
    {
        sprintf( acErrMsg,"Select hv_fzf table error!![%s][%d][%d]",__FILE__,__LINE__,ret);
        set_zd_data(DC_GET_MSG,"Select hv_fzf table error!");
        WRITEMSG
            return -1;
    }  */
    /*     ret = Hv_fzf_Dec_Upd(g_pub_tx.reply, "otype='1' and or_br_no='%s' and wt_date=%d and orderno='%s' and cmtno='%s'", \
        cOr_br_no, atoi(cWt_date), cOrderno,"313");   */
    ret = Hv_fzf_Dec_Upd(g_pub_tx.reply, "otype='1' and pay_qs_no='%s' and orderno='%s' and cmtno='%s'", \
         HV_QS_BR_NO, cOrderno,"313");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        WRITEMSG
            return ret;
    }

    memset(&hv_fzf, 0 , sizeof(hv_fzf));      
    ret = Hv_fzf_Fet_Upd(&hv_fzf, g_pub_tx.reply);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"查询非支付交易表错误!![%d]",ret);
        WRITEMSG
        return ret;
    }
    else if (ret == 100)
    {
        vtcp_log("[%s][%d] 没有找到对应的 313 报文 !!! ",__FILE__,__LINE__);      
        sprintf(acErrMsg,"没有找到对应的 313 报文!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O127");
        return ret;
    }
    
    vtcp_log("[%s][%d] 找到了对应的 313 报文 !!! ",__FILE__,__LINE__);
    if(hv_fzf.hv_fzf_sts[0] == SEND_ACCT_QS)
    {
        vtcp_log("[%s][%d] 该退回申请已应答 !!! ",__FILE__,__LINE__);            
        sprintf(acErrMsg," 该退回申请已应答 !!");
        WRITEMSG
        return -1;
    }    
    
    hv_fzf.res_date = apatoi(phv_notpay->T_wt_date,8);
    memcpy( hv_fzf.res_br_no, phv_notpay->T_or_br_no, sizeof(hv_fzf.res_br_no)-1 );
    memcpy( hv_fzf.res_sts, phv_notpay->T_respcode, sizeof(hv_fzf.res_sts)-1 );   
    memcpy( hv_fzf.res_content, phv_notpay->T_res_content, sizeof(hv_fzf.res_content)-1 );
    memcpy( hv_fzf.res_orderno, phv_notpay->T_orderno, sizeof(hv_fzf.res_orderno)-1 );
    memcpy( hv_fzf.res_sts, phv_notpay->T_respcode, sizeof(hv_fzf.res_sts)-1 );  
    hv_fzf.hv_fzf_sts[0] = SEND_ACCT_QS; 
    hv_fzf.hv_prt_ind[0] = '0';
    vtcp_log(" hv_fzf.res_date=[%d] ", hv_fzf.res_date);  
    vtcp_log(" hv_fzf.res_br_no=[%s] ", hv_fzf.res_br_no);        
    vtcp_log(" hv_fzf.res_orderno=[%s] ", hv_fzf.res_orderno);          
    vtcp_log(" hv_fzf.res_sts=[%s] ",hv_fzf.res_sts);  
    vtcp_log(" hv_fzf.hv_fzf_sts=[%s] ",hv_fzf.hv_fzf_sts);     

    ret = Hv_fzf_Upd_Upd(hv_fzf, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE COM_BRANCH ERROR ! [%d]",ret);
        WRITEMSG
            return ret;
    }    
    
    Hv_fzf_Clo_Upd();  
    vtcp_log("[%s][%d] 更新非支付交易表记录成功",__FILE__,__LINE__);  
    return 0;
}

/***************************************************************************/
/* 函数名称: pub_hv_set_AREA()                                             */
/* 功能描述: 根据cmtno，设置相应的 AREA 的T字段                            */  
/* 参数:                                                                   */ 
/* 返回值:   0/-1                                                          */  
/* 创建日期: 2006.08.18                                                    */   
/* 作者:     ChenMing                                                      */   
/* 说明:                                                                   */      
/***************************************************************************/
int pub_hv_set_AREA(THVPACKAGE* phv_pack,char* cmtno,int* zfflg, HV_PAYIN_AREA* phv_payin,HV_NOTPAY_AREA* phv_notpay)
{
    if (pub_hv_check_cmtno(cmtno) != 0)
    {
        vtcp_log("[%s][%d] cmtno=[%s] -------cmtno 错误！",__FILE__,__LINE__,cmtno);            
        return -1;
    }                        
    vtcp_log("[%s][%d] cmtno=[%s] ",__FILE__,__LINE__,cmtno);    
    memcpy(phv_payin->T_cmtno,cmtno,sizeof(phv_payin->T_cmtno));
    memcpy(phv_notpay->T_cmtno,cmtno,sizeof(phv_notpay->T_cmtno));        
    switch (atoi(cmtno))
    {
    case 100:         if( pub_com_CMT100_TAG2T(phv_payin,phv_pack) ) return -1;  *zfflg = 1; return 0;
    case 101:         if( pub_com_CMT101_TAG2T(phv_payin,phv_pack) ) return -1;  *zfflg = 1; return 0;
    case 102:         if( pub_com_CMT102_TAG2T(phv_payin,phv_pack) ) return -1;  *zfflg = 1; return 0;
    case 103:         if( pub_com_CMT103_TAG2T(phv_payin,phv_pack) ) return -1;  *zfflg = 1; return 0;
    case 105:         if( pub_com_CMT105_TAG2T(phv_payin,phv_pack) ) return -1;  *zfflg = 1; return 0;
    case 108:         if( pub_com_CMT108_TAG2T(phv_payin,phv_pack) ) return -1;  *zfflg = 1; return 0;
        /****xyy 20060909  
        case 121:         if( pub_com_CMT121_TAG2T(phv_payin,phv_pack) ) return -1;  *zfflg = 1; return 0;*****/
    case 122:         if( pub_com_CMT122_TAG2T(phv_payin,phv_pack) ) return -1;  *zfflg = 1; return 0;
    case 123:         if( pub_com_CMT123_TAG2T(phv_payin,phv_pack) ) return -1;  *zfflg = 1; return 0;
    case 124:         if( pub_com_CMT124_TAG2T(phv_payin,phv_pack) ) return -1;  *zfflg = 1; return 0;
    case 725:         if( pub_com_CMT725_TAG2T(phv_notpay,phv_pack) ) return -1;  *zfflg = 0; return 0;

    case 232:         if( pub_com_CMT232_TAG2T(phv_payin,phv_pack) ) return -1;  *zfflg = 1; return 0;
    case 802:         if( pub_com_CMT802_TAG2T(phv_payin,phv_pack) ) return -1;  *zfflg = 1; return 0;
    case 803:         if( pub_com_CMT803_TAG2T(phv_payin,phv_pack) ) return -1;  *zfflg = 1; return 0;
    /** dongxy 20070402 增加812和814报文 **/
    case 812:
    case 814:         if( pub_com_CMT81x_TAG2T(phv_payin,phv_pack) ) return -1;  *zfflg = 1; return 0;
    case 253:         if( pub_com_CMT253_TAG2T(phv_notpay,phv_pack) ) return -1;  *zfflg = 0; return 0;
    case 301:         if( pub_com_CMT301_TAG2T(phv_notpay,phv_pack) ) return -1;  *zfflg = 0; return 0;
    case 302:         if( pub_com_CMT302_TAG2T(phv_notpay,phv_pack) ) return -1;  *zfflg = 0; return 0;
    case 303:         if( pub_com_CMT303_TAG2T(phv_notpay,phv_pack) ) return -1;  *zfflg = 0; return 0;   
        /*if( iGetHvOrderno(phv_notpay->T_orderno) )      return -1;   
        vtcp_log("[%s][%d] hv_notpay.T_orderno=[%.12s]",__FILE__,__LINE__,phv_notpay->T_orderno);*/

    case 312:         if( pub_com_CMT312_TAG2T(phv_notpay,phv_pack) ) return -1;  *zfflg = 0; return 0; 
    /*Added  2006-11-15 14:27  */
    case 313:         if( pub_com_CMT313_TAG2T(phv_notpay,phv_pack) ) return -1;  *zfflg = 0; return 0; 
    case 314:         if( pub_com_CMT314_TAG2T(phv_notpay,phv_pack) ) return -1;  *zfflg = 0; return 0; 
    case 910:         if( pub_com_CMT910_TAG2T(phv_notpay,phv_pack) ) return -1;  *zfflg = 0; return 0;

    default:          vtcp_log("[%s][%d] cmtno=[%s] 出错了! ",__FILE__,__LINE__,cmtno);   
        return -1;    
    }
    return -1;    
}

/***************************************************************************/
/* 函数名称: pub_hv_ins_zf()                                               */
/* 功能描述: 登记大额支付来帐,插入 hv_zf 和 hv_zfhtr 表                    */
/* 参数:                                                                   */
/* 返回值:   0/-1                                                          */
/* 创建日期: 2006.08.18                                                    */
/* 作者:     ChenMing                                                      */
/* 说明:                                                                   */
/***************************************************************************/
int pub_hv_ins_zf(HV_PAYIN_AREA* phv_payin)
{
    char cTx_amt[20];
    memset(cTx_amt,0,sizeof(cTx_amt));

    /* Begin Added for process Sendco Receco  2006-11-13 15:57 **************/
    int ret=0;
    char cSendco[5];
    char cRececo[5];
    memset(cSendco, 0, sizeof(cSendco));
    memset(cRececo, 0, sizeof(cRececo));    
    memcpy(cSendco, phv_payin->T_sendco, sizeof(cSendco)-1 );
    memcpy(cRececo, phv_payin->T_receco, sizeof(cRececo)-1 );    

    if(strlen(cSendco) < 4)
    {
        ret = iGetSendco(cSendco, phv_payin->T_pay_qs_no);
        if(ret == 0)
        {
            memcpy(phv_payin->T_sendco, cSendco, sizeof(phv_payin->T_sendco));
        }  
    }
    if(strlen(cRececo) < 4)
    {
        ret = iGetSendco(cRececo, phv_payin->T_cash_qs_no);
        if(ret == 100)
        {
            memcpy(phv_payin->T_receco, cRececo, sizeof(phv_payin->T_receco));
        }  
    }
    /* End Added for process Sendco Receco  2006-11-13 15:57 ****************/
    
    
    vtcp_log("%s,%d 开始插入支付表 ",__FILE__,__LINE__);
    vtcp_log(">>>phv_payin->T_hv_sts=[%c]begsts=[%c]",phv_payin->T_hv_sts[0],phv_payin->T_beg_sts[0]);TRACE 
    vtcp_log(">>>phv_payin->T_hv_sts=[%c]",phv_payin->T_hv_sts[0]);TRACE 
    if (iHvUpdZfRecFromPayin(phv_payin,1) != 0)
    {
        vtcp_log("[%s][%d] 插入 hv_zf 表出错！",__FILE__,__LINE__);    
        return -1;
    }
    vtcp_log("[%s][%d] 插入 hv_zf 表成功！",__FILE__,__LINE__);        

    /*dot_str2dbl(phv_payin->T_tx_amt,sizeof(phv_payin->T_tx_amt),&tx_amt);*/ 
    /*   已在iHvUpdZfRecFromPayin()中插入
    memcpy(cTx_amt, phv_payin->T_tx_amt, sizeof(cTx_amt)-1); 
    vtcp_log("[%s,%d] cTx_amt====[%s]",__FILE__,__LINE__,cTx_amt);  
    if (dg_hv_zfhtr(phv_payin,cTx_amt) != 0)
    {
        vtcp_log("[%s][%d] 插入 hv_zfhtr 表出错！",__FILE__,__LINE__);    
        return -1;
    }
    vtcp_log("[%s][%d] 插入 hv_zfhtr 表成功！",__FILE__,__LINE__);  */      
    return 0;
}

/***************************************************************************/
/* 函数名称: pub_hv_ins_fzf()                                              */
/* 功能描述: 登记大额非支付来帐,插入 hv_fzf 和 hv_fzfhtr 表                */
/* 参数:                                                                   */
/* 返回值:   0/-1                                                          */
/* 创建日期: 2006.08.18                                                    */
/* 作者:     ChenMing                                                      */
/* 说明:                                                                   */
/***************************************************************************/
int pub_hv_ins_fzf(HV_NOTPAY_AREA* phv_notpay)
{
    /* Begin Added for process Sendco Receco  2006-11-13 15:57 **************/
    int ret=0;
    char cSendco[5];
    char cRececo[5];
    memset(cSendco, 0, sizeof(cSendco));
    memset(cRececo, 0, sizeof(cRececo));    
    memcpy(cSendco, phv_notpay->T_sendco, sizeof(cSendco)-1 );
    memcpy(cRececo, phv_notpay->T_receco, sizeof(cRececo)-1 );    

    if(strlen(cSendco) < 4)
    {
        ret = iGetSendco(cSendco, phv_notpay->T_pay_qs_no);
        if(ret == 100)
        {
            memcpy(phv_notpay->T_sendco, cSendco, sizeof(phv_notpay->T_sendco));
        }  
    }
    if(strlen(cRececo) < 4)
    {
        ret = iGetSendco(cRececo, phv_notpay->T_cash_qs_no);
        if(ret == 100)
        {
            memcpy(phv_notpay->T_receco, cRececo, sizeof(phv_notpay->T_receco));
        }  
    }
    /* End Added for process Sendco Receco  2006-11-13 15:57 ****************/

    if (iHvUpdFzfRecFromPayin(phv_notpay,1) != 0)
    {
        vtcp_log("[%s][%d] 插入 hv_fzf 表出错！",__FILE__,__LINE__);    
        return -1;
    }
    vtcp_log("[%s][%d] 插入 hv_fzf  表成功！",__FILE__,__LINE__);        
    if (dg_hv_fzfhtr(phv_notpay) != 0)
    {
        vtcp_log("[%s][%d] 插入 hv_fzfhtr 表出错！",__FILE__,__LINE__);    
        return -1;
    }    
    vtcp_log("[%s][%d] 插入 hv_fzf  hv_fzfhtr 表成功！",__FILE__,__LINE__);        
    return 0;
}
/***************************************************************************/
/* 函数名称: pub_hv_hang_check()                                           */
/* 功能描述: 挂账条件检查:  日期、来帐状态、 收款人帐号 户名 帐号状态      */
/* 函数出口：                                                              */ 
/* 参数:                    支付接收行号 ???                               */
/* 返回值:   挂帐return 0  出错:return 1                                   */
/* 创建日期: 2006.08.18                                                    */
/* 作者:     ChenMing                                                      */
/* 说明:                                                                   */
/***************************************************************************/
int pub_hv_hang_check(THVPACKAGE *hv_pack,char *cmtno,char* stat)
{
    int  ret = 0;
    char    payactno    [33];    /** 付款人帐号 **/
    char    cashactno   [33];    /** 收款人帐号 **/
    char    payname     [61];    /** 付款人名称 **/
    char    cashname    [61];    /** 收款人名称 **/   
    char    tx_br_no    [6] ;       /*大额来帐记账机构号*/ 
    char    cP_br_no    [6] ;        /*汇票签发机构号*/ 
    char    tx_tel      [6] ;         /* 柜员号*/      
    char    acc_v       [33];
    char    name_v      [61];
    char    cTempstr    [1024];
    
    int     ac_seqn_v;
    struct  mdm_ac_rel_c mdm_ac_rel_v;
    struct  prdt_ac_id_c prdt_ac_id_v;
    struct  dd_mst_c dd_mst_v;   

    memset(payactno,0x00,sizeof(payactno));    
    memset(cashactno,0x00,sizeof(cashactno));    
    memset(payname,0x00,sizeof(payname));         
    memset(cashname,0x00,sizeof(cashname));         
    memset(tx_br_no,0x00,sizeof(tx_br_no));
    memset(cP_br_no,0x00,sizeof(cP_br_no));    
    memset(tx_tel,0x00,sizeof(tx_tel));
    memset(acc_v,0x00,sizeof(acc_v));
    memset(name_v,0x00,sizeof(name_v));
    
    memset(&mdm_ac_rel_v,0x00,sizeof(mdm_ac_rel_v));
    memset(&prdt_ac_id_v,0x00,sizeof(prdt_ac_id_v));
    memset(&dd_mst_v,0x00,sizeof(dd_mst_v));    

    set_zd_data("0640",HVLZGD);  /* 大额挂帐科目*/
        
    /* 设置默认状态为 9 */
    if (memcmp(hv_pack->opeStateCode, "000", 3 ) == 0 )       { stat[0] = RECEIVE_ACCT_QS;  }  /** 来账记账清算 **/ 
    else if (memcmp(hv_pack->opeStateCode, "001", 3 ) == 0 )  { stat[0] = RECEIVE_NMAC_ERR; }  /** 来账核全国押错 **/  
    else if (memcmp(hv_pack->opeStateCode, "002", 3 ) == 0 )  { stat[0] = RECEIVE_LMAC_ERR; }     /** 来账核地方押错 **/
    vtcp_log("hv_pack->opeStateCode=[%.3s]",hv_pack->opeStateCode);
    vtcp_log("[%s][%d] 来账状态=[%c]",__FILE__,__LINE__,stat[0]);    
    /** ========= 以下设置判断来账状态 stat ============= **/
    ret = iSetHvTxBrNo(hv_pack);    /*  设置机构号 ==> 0030 0020 */  
    if( ret==1)/* 得到开户机构号错误, 设置来帐机构 支行挂账  */
    {
       if(stat[0] ==RECEIVE_NMAC_ERR || stat[0]==RECEIVE_NMAC_ERR)
       {
       }
       else
       {
           stat[0] = RECEIVE_CUSTNAME_ERR;
       }
       sprintf(acErrMsg,"得到开户机构号错误, 设置来帐机构支行挂账!");
       WRITEMSG
    }
    /* begin add by LiuHuafeng 2009-5-13 14:23:18 for 村镇银行 */
    else if(ret == 9999)
    {
        vtcp_log("%s,%d 村镇银行，只登记不处理 ",__FILE__,__LINE__);
        stat[0] = RECEIVE_DATA_ERR;
        return 9999;
    }
    /* end by LiuHuafeng 2009-5-13 14:23:36 */
    else if ( ret ) /* 得到开户机构 来帐机构 错误 清算挂帐 */
    {
        if(stat[0] ==RECEIVE_NMAC_ERR || stat[0]==RECEIVE_NMAC_ERR)
        {
        }
        else
        {
           stat[0] = RECEIVE_DATA_ERR; 
        }
        sprintf(acErrMsg,"得到交易机构号错误,清算挂账!");
        WRITEMSG
    }
    get_zd_data("0030",tx_br_no); /* 由上一步得到的大额来帐记账机构号*/   
    ret = iGetReceTel(tx_br_no);    /*  柜员号 ==> 0070 */
    if ( ret )
    {
        *stat = RECEIVE_DATA_ERR;         
        sprintf(acErrMsg,"得到柜员号错误,挂账!");
        WRITEMSG
        return 100;
    }
    get_zd_data("0070",tx_tel); /* 由上一步得到的柜员号*/
    vtcp_log("%s,%d 得到的操作员是",__FILE__,__LINE__,tx_tel);
    vtcp_log("[%s][%d] 来账状态=[%c]",__FILE__,__LINE__,stat[0]);
    vtcp_log("%s,%d tx_br_no ===[%s] ",__FILE__,__LINE__,tx_br_no);

    get_zd_data("0030",tx_br_no); /* 由上一步得到的大额来帐记账机构号*/   
    iHvGetRece_AcValue(hv_pack, acc_v, name_v); /* 会自动设置好帐号    add by LiuHuafeng 2006-9-1 20:22 */
    pub_base_strpack(acc_v);
    pub_base_strpack(name_v);  
    
    vtcp_log("[%s][%d] name_v==[%.60s] ",__FILE__,__LINE__,name_v); 
    memset(cTempstr,0,sizeof(cTempstr));
    memcpy(cTempstr, name_v, sizeof(name_v)-1);
    dbc_zipspace(cTempstr);  /*压缩全角空格 2006-11-7 23:45  */
    memcpy(name_v, cTempstr, sizeof(name_v)-1);
    vtcp_log("[%s][%d] name_v==[%.60s] ",__FILE__,__LINE__,name_v); 
      
    if (stat[0] != RECEIVE_ACCT_QS ) /** 只有正常的可以入帐，有核押错挂帐 **/
    { 
        char cAc_no[22];
        int iRet;
        memset(cAc_no, 0 , sizeof(cAc_no));
        /* 设置会计科目借贷标志add by LiuHuafeng 20060930 */
        set_zd_data("0660","2");/* 客户帐方向借贷 */
        set_zd_data("0720","1");/* 110 方向借贷 */
        set_zd_data("0300",cAc_no);
        sprintf(acErrMsg,"来账状态不是正常状态stat=[%c],挂账!",stat[0]);
        WRITEMSG
        return 100;
    }
    
    
    /* 查找汇票记录是否存在以及状态是否正确  设置汇票签发机构 2006-10-26 10:06 */
    if ( memcmp(cmtno, "122", 3) == 0 || memcmp(cmtno, "123", 3) == 0||
          memcmp(cmtno, "124", 3) == 0 || memcmp(cmtno, "725", 3) == 0  )      
    {
        char cPay_br_no[13];
        char cPo_no[9];
        struct hv_poinfo_c hv_poinfo;   
           
        /*以下查找签发票据的机构号 */
        memset(cPay_br_no, 0 , sizeof(cPay_br_no));    
        memset(cPo_no, 0 , sizeof(cPo_no));    
        hv_get_tag(hv_pack,"CC4",cPay_br_no);
        hv_get_tag(hv_pack,"21A",cPo_no);  
        vtcp_log("%s,%d cPay_br_no===[%s]",__FILE__,__LINE__,cPay_br_no);    
        vtcp_log("%s,%d cPo_no    ===[%s]",__FILE__,__LINE__,cPo_no);  
            
        memset(&hv_poinfo, 0 , sizeof(hv_poinfo));
        ret = Hv_poinfo_Sel(g_pub_tx.reply, &hv_poinfo, "po_no='%s' and pay_br_no='%s' ", cPo_no, cPay_br_no);
        if (ret == 100)
        {
            vtcp_log("[%s][%d] 没有找到对应的签发汇票，支行挂帐 !!!  ret=[%d] ",__FILE__,__LINE__,ret);      
            sprintf(acErrMsg," 没有找到对应的签发汇票，支行挂帐!!");
            WRITEMSG
            stat[0] = RECEIVE_CUSTNAME_ERR;
            set_zd_data("0980",stat);  
            return 100;  /* 挂帐 */
        }else if (ret != 0)
        {
            sprintf(acErrMsg,"数据库错误 !! [%d]",ret);
            WRITEMSG
            return ret;
        } 
        vtcp_log("%s,%d 汇票签发机构 hv_poinfo.br_no  ===[%s] ",__FILE__,__LINE__,hv_poinfo.br_no);
 
        vtcp_log("[%s][%d] 查找汇票记录状态是否正确 CMTCode==[%.3s] hv_poinfo.po_sts==[%s] ",
        __FILE__,__LINE__,hv_pack->CMTCode,hv_poinfo.po_sts);   
         
        if(memcmp(hv_pack->CMTCode,"122",sizeof(hv_pack->CMTCode) )==0)
        {
            if( hv_poinfo.po_sts[0] == HV_HP_UNLOSS ||
                hv_poinfo.po_sts[0] == HV_HP_LOSS ||  /* 挂失解挂状态  Added by ChenMing  2006-11-22 15:07*/
                hv_poinfo.po_sts[0] == HV_HP_JQ || /* 725先收到 已全额结清 */
                hv_poinfo.po_sts[0] == HV_HP_ASKCASH ) /* 申请兑付 */
            {
                  vtcp_log("[%s][%d] 只有在'申请兑付'或725先收到时可接收! ",__FILE__,__LINE__);               
                  vtcp_log("[%s][%d] 状态正确，可接收! ",__FILE__,__LINE__);   
            }
            else
            {
                Hv_poinfo_Clo_Upd();  
                *stat = RECEIVE_CUSTNAME_ERR;         
                sprintf(acErrMsg,"汇票状态错误,挂账!");
                WRITEMSG
                return 100;
            } 
        }else if(memcmp(hv_pack->CMTCode,"123",sizeof(hv_pack->CMTCode) )==0)
        {
            /***  老刘说  123  124  不用检查了
            if(hv_poinfo.po_sts[0] == HV_HP_UNLOSS ||
                hv_poinfo.po_sts[0] == HV_HP_LOSS ||   挂失解挂状态  Added by ChenMing  2006-11-22 15:07 
                 hv_poinfo.po_sts[0] == HV_HP_ASKCASH || hv_poinfo.po_sts[0] == HV_HP_CASH ||
                hv_poinfo.po_sts[0] == HV_HP_ASKRETURN || hv_poinfo.po_sts[0] == HV_HP_MV_CENTER)  
            {
                  vtcp_log("[%s][%d] 状态正确，可接收! ",__FILE__,__LINE__);   
            }
            else
            {
                Hv_poinfo_Clo_Upd();  
                *stat = RECEIVE_CUSTNAME_ERR; 
                sprintf(acErrMsg,"汇票状态错误,挂账!");
                WRITEMSG
                return 100;
            }
            **/
        }
        else if(memcmp(hv_pack->CMTCode,"124",sizeof(hv_pack->CMTCode) )==0)
        {
            /*  老刘说  123  124  不用检查了
            if(hv_poinfo.po_sts[0] == HV_HP_UNLOSS ||
                hv_poinfo.po_sts[0] == HV_HP_LOSS ||   挂失解挂状态  Added by ChenMing  2006-11-22 15:07
                 hv_poinfo.po_sts[0] == HV_HP_ASKCASH || hv_poinfo.po_sts[0] == HV_HP_CASH ||
                hv_poinfo.po_sts[0] == HV_HP_ASKRETURN || hv_poinfo.po_sts[0] == HV_HP_MV_CENTER )  
            {
                  vtcp_log("[%s][%d] 状态正确，可接收! ",__FILE__,__LINE__);   
            }
            else
            {
                *stat = RECEIVE_CUSTNAME_ERR;         
                sprintf(acErrMsg,"汇票状态错误,挂账!");
                WRITEMSG
                return 100;
            }
            */
        }
        else if(memcmp(hv_pack->CMTCode,"725",sizeof(hv_pack->CMTCode) )==0)
        {
            if( hv_poinfo.po_sts[0] == HV_HP_UNLOSS ||
                hv_poinfo.po_sts[0] == HV_HP_LOSS ||  /* 挂失解挂状态  Added by ChenMing  2006-11-22 15:07*/
                hv_poinfo.po_sts[0] == HV_HP_ASKCASH || 
                hv_poinfo.po_sts[0] == HV_HP_MV_CENTER || 
                hv_poinfo.po_sts[0] == HV_HP_CASH)
            {
                  vtcp_log("[%s][%d] 状态正确，可接收! ",__FILE__,__LINE__); 
            }
            else
            {
                *stat = RECEIVE_DATA_ERR;         
                sprintf(acErrMsg,"汇票状态错误,挂账!");
                WRITEMSG
                return 100;
            }
        }
        else
        {
            sprintf(acErrMsg,"CMTCode== [%s] 不在这里处理! ",hv_pack->CMTCode);
            WRITEMSG
            return -1;
        }

        /* 122使用最后持票人的开户机构，  不用在这里设置机构号了 */
        if(memcmp(hv_pack->CMTCode,"122",sizeof(hv_pack->CMTCode)))  
        {
             /* 检查汇票的签发机构 */
              memcpy(cP_br_no,hv_poinfo.br_no,sizeof(cP_br_no)-1);
              vtcp_log("%s,%d 汇票签发机构 cP_br_no=[%s] tx_br_no=[%s]",__FILE__,__LINE__,cP_br_no,tx_br_no);
             if(memcmp(cP_br_no,tx_br_no,sizeof(cP_br_no)-1))  
             {
                  sprintf(acErrMsg,"注意，这里出现不一致 !! [%d]",ret);
                  WRITEMSG
                  set_zd_data(DC_TX_BR_NO,cP_br_no);
             }
             if(iHvHang_Check_Brno(cP_br_no, tx_br_no))
             {
                 sprintf(acErrMsg,"数据库错误 !! [%d]",ret);
                 WRITEMSG
                 return -1;
             }
             get_zd_data("0030",cP_br_no);  /* 由上一步得到的汇票签发机构*/  
             iGetReceTel(cP_br_no);
             vtcp_log("%s,%d 汇票签发机构 cP_br_no=[%s] tx_br_no=[%s]",__FILE__,__LINE__,cP_br_no,tx_br_no);
        }
        
    }
    
    vtcp_log("acc_v=[%s] name_v=[%s]",acc_v,name_v);
    /** dongxy 20070402 增加812和814的情况 **/
    if ((memcmp(cmtno, "232", 3 ) == 0) || 
        (memcmp(cmtno, "802", 3 ) == 0)  || (memcmp(cmtno, "803", 3 ) == 0) || 
        (memcmp(cmtno, "812", 3 ) == 0)  || (memcmp(cmtno, "814", 3 ) == 0) )
    {
        set_zd_data("0640",HVLZGD);
        vtcp_log("  cmtno=[%.3s] 不需要做下面的检查 ",cmtno);
        return 0;       
    }
    /** add by LiuHuafeng 2006-12-21 12:08 */
    if(memcmp(cmtno,"725",3)==0 && (strlen(acc_v)==3 ||strlen(acc_v)==5 ||strlen(acc_v)==7))
    {
        set_zd_data("0680","6");
        vtcp_log("[%s][%d]来帐是725报文，你可以不用检查客户帐号了!^_^\n",__FILE__,__LINE__);
        return 0;
    }
    /** end by LiuHuafeng 2006-12-21 12:19 */
    /*  大额来帐记账机构号及柜员号赋值  */       
    vtcp_log("[%s][%d] tx_br_no=[%.5s]",__FILE__,__LINE__,tx_br_no);    
    vtcp_log("[%s][%d] tx_tel===[%.5s]",__FILE__,__LINE__,tx_tel);        
    vtcp_log("[%s][%d] acc_v ==[%.32s] ",__FILE__,__LINE__,acc_v);
    vtcp_log("[%s][%d] name_v==[%.60s] ",__FILE__,__LINE__,name_v);      
    pub_base_old_acno( acc_v );    /*旧账号转新账号*/
    vtcp_log("[%s][%d] acc_v ==[%.32s] ",__FILE__,__LINE__,acc_v);
    vtcp_log("[%s][%d] name_v==[%.60s] ",__FILE__,__LINE__,name_v);      

    /* 收款人账号检查开始 */
    memset( &mdm_ac_rel_v, 0x00, sizeof( struct mdm_ac_rel_c ) );
    g_reply_int = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_v,"ac_no='%s'",acc_v);
    if (g_reply_int==100)
    {
        *stat = RECEIVE_CUSTNAME_ERR;
        sprintf(acErrMsg,"大额来账收款人账号[%s]不存在,挂账!",acc_v);
        WRITEMSG
        return 100;
    }
    else if (g_reply_int)
    {
        *stat = RECEIVE_CUSTNAME_ERR;
        sprintf(acErrMsg,"大额来账检查收款人账号[%s]数据库操作失败[%d]",acc_v,g_reply_int);
        WRITEMSG
        return -1;
    }
    /* begin 判断介质状态 add by LiuHuafeng 2006-10-5 10:55*/
    if(mdm_ac_rel_v.note_sts[0]!='0' && mdm_ac_rel_v.note_sts[0]!='1' && mdm_ac_rel_v.note_sts[0]!='3' )
    {
        /* 介质状态不正常挂帐处理 */
        *stat = RECEIVE_CUSTNAME_ERR;
        sprintf(acErrMsg,"大额来账收款人账号[%s]介质状态错误[%s]",acc_v,mdm_ac_rel_v.note_sts[0]);
        WRITEMSG
        return 100;
    }
    /* end by LiuHuafeng 2006-10-5 10:56 */
    memset(&prdt_ac_id_v,0x00,sizeof(struct prdt_ac_id_c));  
    g_reply_int = Prdt_ac_id_Sel( g_pub_tx.reply, &prdt_ac_id_v,"ac_id='%ld' and (ac_id_type='1' or ac_id_type='9')", \
        mdm_ac_rel_v.ac_id );
    if (g_reply_int==100)
    {
        *stat = RECEIVE_CUSTNAME_ERR;   /* 'I'  来账内容不符，含账号、户名错误,清算中心挂帐*/
        sprintf(acErrMsg,"大额来账收款人账号非活期存款户[%s],挂账!",acc_v);
        WRITEMSG
        return 100;
    }
    else if (g_reply_int)
    {
        *stat = RECEIVE_CUSTNAME_ERR;
        sprintf(acErrMsg,"大额来账检查收款人账号[%s]数据库操作失败[%d]",acc_v,g_reply_int);
        WRITEMSG
        return -1;
    }
    /* 根据币种生成账户序号  
    g_reply_int = pub_base_CurToSeqn( "01", &ac_seqn_v );
    if ( g_reply_int )
    {
    sprintf(acErrMsg,"调用函数根据币种生成账户序号错误!");
    WRITEMSG
    return -1;
    } */
    /* 内部帐户不检查户名 */
    /****add by liuyue 20061205 725报文不应该再来检查汇票申请人的帐号了****/
    
    if(memcmp(hv_pack->CMTCode,"725",sizeof(hv_pack->CMTCode)))
    {
        if(prdt_ac_id_v.ac_id_type[0]=='1'  )
        {
            ac_seqn_v  =  1;    
            g_reply_int = Dd_mst_Sel(g_pub_tx.reply,&dd_mst_v,"ac_id='%ld' and ac_seqn='%d'",mdm_ac_rel_v.ac_id,ac_seqn_v);
            if (g_reply_int)
            {
                *stat = RECEIVE_CUSTNAME_ERR;
                sprintf(acErrMsg,"查询大额来账收款人账号活期主文件错sqlcode[%d] ac_id[%ld] ac_seqn[%d]",g_reply_int,mdm_ac_rel_v.ac_id,ac_seqn_v);
                WRITEMSG
                return -1;
            }
            if (dd_mst_v.ac_sts[0] != '1' )
            {
                *stat = RECEIVE_CUSTNAME_ERR;   /*   'I'  来账户名不符，但帐号正确，支行挂帐*/
                sprintf(acErrMsg,"大额来账收款人账号状态不正常ac_sts[%s]hold_sts[%s]",dd_mst_v.ac_sts,dd_mst_v.hold_sts);
                WRITEMSG
                return 100;
            }
            /*begin 判断是否别冻结 add by LiuHuafeng 2006-10-5 10:57 */
            /******change by liuyue 20061128 对于帐号状态的冻结状态 长治 要求只有正常状态才可以正常入帐,大小额一致***/
            /** 修改只有完全冻结挂帐 change by wudawei 20130529 ***/
            if(dd_mst_v.hold_sts[0] == '1')
            {
                /* 帐号id被完全冻结所以要挂帐 */
                *stat = RECEIVE_CUSTNAME_ERR;   /*   'I'  来账户名不符，但帐号正确，支行挂帐*/
                sprintf(acErrMsg,"大额来账收款人账号状态不正常ac_sts[%s]hold_sts[%s]",dd_mst_v.ac_sts,dd_mst_v.hold_sts);
                WRITEMSG
                return 100;
            }
            /* end by LiuHuafeng 2006-10-5 11:23 */
            /* begin 判断是否是结算户 by LiuHuafeng 2006-10-5 11:24 */
            if(dd_mst_v.sttl_type[0]=='S')
            {
                *stat = RECEIVE_CUSTNAME_ERR;   /*   'I'  来账户名不符，但帐号正确，支行挂帐*/
                sprintf(acErrMsg,"大额来账收款人账号非结算户sttl_type[%s]",dd_mst_v.sttl_type);
                WRITEMSG
                return 100;
            }
            /* end by LiuHuafeng 2006-10-5 11:24 */
            pub_base_strpack(dd_mst_v.name);
            pub_base_strpack(name_v);
        
            vtcp_log("[%s][%d] dd_mst_v.name==[%.60s] ",__FILE__,__LINE__,dd_mst_v.name); 
            memset(cTempstr,0,sizeof(cTempstr));
            memcpy(cTempstr, dd_mst_v.name, sizeof(dd_mst_v.name)-1);
            dbc_zipspace(cTempstr);  /*压缩全角空格 2006-11-7 23:45  */
            memcpy(dd_mst_v.name, cTempstr, sizeof(dd_mst_v.name)-1);
            vtcp_log("[%s][%d] dd_mst_v.name==[%.60s] ",__FILE__,__LINE__,dd_mst_v.name); 
        
            vtcp_log("[%s][%d] name_v==[%.60s] ",__FILE__,__LINE__,name_v); 
            memset(cTempstr,0,sizeof(cTempstr));
            memcpy(cTempstr, name_v, sizeof(name_v)-1);
            dbc_zipspace(cTempstr);  /*压缩全角空格 2006-11-7 23:45  */
            memcpy(name_v, cTempstr, sizeof(name_v)-1);
            vtcp_log("[%s][%d] name_v==[%.60s] ",__FILE__,__LINE__,name_v); 
                        
            if (strcmp(dd_mst_v.name,name_v))
            {
                *stat = RECEIVE_CUSTNAME_ERR;   /*   'I'  来账户名不符，但帐号正确，支行挂帐*/
                sprintf(acErrMsg,"大额来账收款人名称不正确,账号ac_id[%ld]户名[%s]错误户名[%s]", dd_mst_v.ac_id,dd_mst_v.name,name_v);
                WRITEMSG
                return 100;
            }
            set_zd_data("0680","1");
        }/* begin add by LiuHuafeng 2006-10-5 10:22 检查内部帐户 */
        else
        {
            struct in_mst_c sIn_mst;
            memset(&sIn_mst, 0  ,sizeof(sIn_mst));
            g_reply_int = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id='%ld' and ac_seqn='0'",mdm_ac_rel_v.ac_id);
            if (g_reply_int)
            {
                *stat = RECEIVE_CUSTNAME_ERR;
                sprintf(acErrMsg,"查询大额来账收款人账号内部帐主文件错sqlcode[%d] ac_id[%ld] ac_seqn[0]",g_reply_int,mdm_ac_rel_v.ac_id);
                WRITEMSG
                return -1;
            }
            if (sIn_mst.sts[0] != '1' )
            {
                *stat = RECEIVE_DATA_ERR;  /* 'H'  来账内容不符，含账号、户名错误,清算中心挂帐*/          
                sprintf(acErrMsg,"大额来账收款人账号状态不正常ac_sts[%s]hold_sts[%s]",dd_mst_v.ac_sts,dd_mst_v.hold_sts);
                WRITEMSG
                return 100;
            }
            set_zd_data("0680","6");/* 设置帐户类型 */
        }
    }
    else
    {
        if(prdt_ac_id_v.ac_id_type[0]=='1' )
            set_zd_data("0680","1");
        else
            set_zd_data("0680","6");
        vtcp_log("[%s][%d]来帐是725报文，你可以不用检查客户帐号了!^_^\n",__FILE__,__LINE__);
    }
    /* end by LiuHuafeng 2006-10-5 10:22 */
    /* 正常则置8583域 */
    set_zd_data("0300", acc_v );  /* 收款人账号 */ 
    set_zd_data("0210", "1");     /* 币种 */

    return 0;    
}

/************************************************************************* 
* 函 数 名: pub_hv_old_acno()                  
* 功能描述： 处理新旧账号
* 作    者:  ChenMing                             
* 完成日期： 2006-8-20                             
* 修改记录：                                      
*   日   期:                                      
*   修 改 人:                                     
*   修改内容:                                      
*************************************************************************/
int pub_hv_old_acno( char * ac_no )
{
    int ret=0;
    struct mo_merge_ac_c mo_merge_ac_c;    
    memset(&mo_merge_ac_c, 0, sizeof(mo_merge_ac_c));

    /**旧账号**/
    if ( strlen(ac_no)==15 )
        ;
    else if ( !strncmp(ac_no,"7861",4) && strlen(ac_no)==20 )
        ;
    else if ( !strncmp(ac_no,"7861",4) && strlen(ac_no)==19 )
        ;
    else    
        return 0;

    ret=Mo_merge_ac_Sel( g_pub_tx.reply , &mo_merge_ac_c , "old_ac_no='%s'" , \
        ac_no );
    if ( ret==100 )
    {
        return 0;
    }
    else if ( ret )
    {
        return 1;
    }
    pub_base_strpack(mo_merge_ac_c.ac_no);
    strcpy( ac_no,mo_merge_ac_c.ac_no );
}

/************************************************************************* 
* 函 数 名: pub_hv_print_PAYIN                  
* 功能描述： 测试时打印PAYIN
* 作    者:  ChenMing                             
* 完成日期： 2006-8-20                             
* 修改记录：                                      
*   日   期:                                      
*   修 改 人:                                     
*   修改内容:                                      
*************************************************************************/ 
int pub_hv_print_PAYIN(HV_PAYIN_AREA* phv_payin)
{
    vtcp_log(" hv_payin->T_tx_br_no[%s] ",phv_payin->T_tx_br_no);        
    vtcp_log(" hv_payin->T_tx_date[%s] ",phv_payin->T_tx_date);        
    vtcp_log(" hv_payin->T_wt_date[%s] ",phv_payin->T_wt_date);        
    vtcp_log(" hv_payin->T_tx_time[%s] ",phv_payin->T_tx_time);        
    vtcp_log(" hv_payin->T_orderno[%s] ",phv_payin->T_orderno);        
    vtcp_log(" hv_payin->T_cur_no[%s] ",phv_payin->T_cur_no);        
    vtcp_log(" hv_payin->T_note_type[%s] ",phv_payin->T_note_type);        
    vtcp_log(" hv_payin->T_note_no[%s] ",phv_payin->T_note_no);        
    vtcp_log(" hv_payin->T_tx_amt[%s] ",phv_payin->T_tx_amt);        
    vtcp_log(" hv_payin->T_tx_chrg_ind[%s] ",phv_payin->T_tx_chrg_ind);        
    vtcp_log(" hv_payin->T_chrg_amt[%s] ",phv_payin->T_chrg_amt);        

    vtcp_log(" hv_payin->T_pay_qs_no[%s] ",phv_payin->T_pay_qs_no);        
    vtcp_log(" hv_payin->T_or_br_no[%s] ",phv_payin->T_or_br_no);        
    vtcp_log(" hv_payin->T_pay_opn_br_no[%s] ",phv_payin->T_pay_opn_br_no);        
    vtcp_log(" hv_payin->T_pay_ac_no[%s] ",phv_payin->T_pay_ac_no);        
    vtcp_log(" hv_payin->T_pay_name[%s] ",phv_payin->T_pay_name);        
    vtcp_log(" hv_payin->T_pay_addr[%s] ",phv_payin->T_pay_addr);        
    vtcp_log(" hv_payin->T_cash_qs_no[%s] ",phv_payin->T_cash_qs_no);        
    vtcp_log(" hv_payin->T_ac_br_no[%s] ",phv_payin->T_ac_br_no);        
    vtcp_log(" hv_payin->T_cash_opn_br_no[%s] ",phv_payin->T_cash_opn_br_no);        
    vtcp_log(" hv_payin->T_tx_chrg_ind[%s] ",phv_payin->T_tx_chrg_ind);        
    vtcp_log(" hv_payin->T_cash_ac_no[%s] ",phv_payin->T_cash_ac_no);        

    vtcp_log(" hv_payin->T_cash_name[%s] ",phv_payin->T_cash_name);        
    vtcp_log(" hv_payin->T_cash_addr[%s] ",phv_payin->T_cash_addr);        
    vtcp_log(" hv_payin->T_yw_type[%s] ",phv_payin->T_yw_type);        
    vtcp_log(" hv_payin->T_sendco[%s] ",phv_payin->T_sendco);        
    vtcp_log(" hv_payin->T_receco[%s] ",phv_payin->T_receco);        
    vtcp_log(" hv_payin->T_cmtno[%s] ",phv_payin->T_cmtno);        
    vtcp_log(" hv_payin->T_hv_brf[%s] ",phv_payin->T_hv_brf);        
    vtcp_log(" hv_payin->T_beg_sts[%s] ",phv_payin->T_beg_sts);        
    vtcp_log(" hv_payin->T_operlevel[%s] ",phv_payin->T_operlevel);        
    vtcp_log(" hv_payin->T_hv_sts[%s] ",phv_payin->T_hv_sts);        
    vtcp_log(" hv_payin->T_tel[%s] ",phv_payin->T_tel);    

    vtcp_log(" hv_payin->T_trace_no ",phv_payin->T_trace_no);        
    vtcp_log(" hv_payin->T_hv_osts[%s] ",phv_payin->T_hv_osts);                
    vtcp_log(" hv_payin->T_checkflag[%s] ",phv_payin->T_checkflag);        
    vtcp_log(" hv_payin->T_hv_prt_ind[%s] ",phv_payin->T_hv_prt_ind);        
    vtcp_log(" hv_payin->T_resend_ind[%s] ",phv_payin->T_resend_ind);        
    vtcp_log(" hv_payin->T_resend_date[%s] ",phv_payin->T_resend_date);        
    vtcp_log(" hv_payin->T_o_orderno[%s] ",phv_payin->T_o_orderno);        
    vtcp_log(" hv_payin->T_o_tx_date[%s] ",phv_payin->T_o_tx_date);        
    vtcp_log(" hv_payin->T_o_cmtno[%s] ",phv_payin->T_o_cmtno);        
    vtcp_log(" hv_payin->T_lw_ind[%s] ",phv_payin->T_lw_ind);        

    vtcp_log(" hv_payin->T_addid ",phv_payin->T_addid);        
    vtcp_log(" hv_payin->T_refno[%s] ",phv_payin->T_refno);                
    vtcp_log(" hv_payin->T_auth_tel[%s] ",phv_payin->T_auth_tel);        
    vtcp_log(" hv_payin->T_auth_trace_no[%s] ",phv_payin->T_auth_trace_no);        
    vtcp_log(" hv_payin->T_chk[%s] ",phv_payin->T_chk);        
    vtcp_log(" hv_payin->T_chk_trace_no[%s] ",phv_payin->T_chk_trace_no);        
    vtcp_log(" hv_payin->T_dealerr_date[%s] ",phv_payin->T_dealerr_date);        
    vtcp_log(" hv_payin->T_dealerr_tel[%s] ",phv_payin->T_dealerr_tel);        
    vtcp_log(" hv_payin->T_resp1[%s] ",phv_payin->T_resp1);        
    vtcp_log(" hv_payin->T_o_wt_date[%s] ",phv_payin->T_o_wt_date);        

    vtcp_log(" hv_payin->T_send_tel[%s] ",phv_payin->T_send_tel);        
    vtcp_log(" hv_payin->T_send_tel_trace_no[%s] ",phv_payin->T_send_tel_trace_no);        
    vtcp_log(" hv_payin->T_hv_brf[%s] ",phv_payin->T_hv_brf);        

}

/************************************************************************* 
* 函 数 名: pub_hv_print_NOTPAY                  
* 功能描述： 测试时打印PAYIN
* 作    者:  ChenMing                             
* 完成日期： 2006-8-20                             
* 修改记录：                                      
*   日   期:                                      
*   修 改 人:                                     
*   修改内容:                                      
*************************************************************************/ 
int pub_hv_print_NOTPAY(HV_NOTPAY_AREA* phv_notpay)
{
    vtcp_log(" hv_notpay->T_otype[%s] ",phv_notpay->T_otype);        
    vtcp_log(" hv_notpay->T_or_br_no[%s] ",phv_notpay->T_or_br_no);        
    vtcp_log(" hv_notpay->T_orderno[%s] ",phv_notpay->T_orderno);        
    vtcp_log(" hv_notpay->T_tx_date[%s] ",phv_notpay->T_tx_date);        
    vtcp_log(" hv_notpay->T_wt_date[%s] ",phv_notpay->T_wt_date);        
    vtcp_log(" hv_notpay->T_br_no[%s] ",phv_notpay->T_br_no);        
    vtcp_log(" hv_notpay->T_hv_fzf_sts[%s] ",phv_notpay->T_hv_fzf_sts);        
    vtcp_log(" hv_notpay->T_cmtno[%s] ",phv_notpay->T_cmtno);        
    vtcp_log(" hv_notpay->T_tx_time[%s] ",phv_notpay->T_tx_time);        
    vtcp_log(" hv_notpay->T_trace_no[%s] ",phv_notpay->T_trace_no);        
    vtcp_log(" hv_notpay->T_pay_qs_no[%s] ",phv_notpay->T_pay_qs_no);        
    vtcp_log(" hv_notpay->T_ac_br_no[%s] ",phv_notpay->T_ac_br_no);        

    vtcp_log(" hv_notpay->T_cash_qs_no[%s] ",phv_notpay->T_cash_qs_no);        
    vtcp_log(" hv_notpay->T_o_wt_date[%s] ",phv_notpay->T_o_wt_date);        
    vtcp_log(" hv_notpay->T_o_or_br_no[%s] ",phv_notpay->T_o_or_br_no);        
    vtcp_log(" hv_notpay->T_o_ac_br_no[%s] ",phv_notpay->T_o_ac_br_no);        
    vtcp_log(" hv_notpay->T_o_payin_orderno[%s] ",phv_notpay->T_o_payin_orderno);        
    vtcp_log(" hv_notpay->T_o_cur_no[%s] ",phv_notpay->T_o_cur_no);        
    vtcp_log(" hv_notpay->T_o_tx_amt[%s] ",phv_notpay->T_o_tx_amt);        
    vtcp_log(" hv_notpay->T_o_tx_type[%s] ",phv_notpay->T_o_tx_type);        
    vtcp_log(" hv_notpay->T_o_cmtno[%s] ",phv_notpay->T_o_cmtno);        
    vtcp_log(" hv_notpay->T_sendco[%s] ",phv_notpay->T_sendco);        
    vtcp_log(" hv_notpay->T_receco[%s] ",phv_notpay->T_receco);        

    vtcp_log(" hv_notpay->T_req_date[%s] ",phv_notpay->T_req_date);        
    vtcp_log(" hv_notpay->T_req_br_no[%s] ",phv_notpay->T_req_br_no);        
    vtcp_log(" hv_notpay->T_req_content[%s] ",phv_notpay->T_req_content);        
    vtcp_log(" hv_notpay->T_req_orderno[%s] ",phv_notpay->T_req_orderno);        
    vtcp_log(" hv_notpay->T_res_date[%s] ",phv_notpay->T_res_date);        
    vtcp_log(" hv_notpay->T_res_br_no[%s] ",phv_notpay->T_res_br_no);        
    vtcp_log(" hv_notpay->T_res_content[%s] ",phv_notpay->T_res_content);        
    vtcp_log(" hv_notpay->T_res_orderno[%s] ",phv_notpay->T_res_orderno);        
    vtcp_log(" hv_notpay->T_respcode[%s] ",phv_notpay->T_respcode);        
    vtcp_log(" hv_notpay->T_addid[%s] ",phv_notpay->T_addid);        
    vtcp_log(" hv_notpay->T_refno[%s] ",phv_notpay->T_refno);    

    vtcp_log(" hv_notpay->T_hv_prt_ind ",phv_notpay->T_hv_prt_ind);        
    vtcp_log(" hv_notpay->T_tel[%s] ",phv_notpay->T_tel);                
    vtcp_log(" hv_notpay->T_hv_brf[%s] ",phv_notpay->T_hv_brf);        
    vtcp_log(" hv_notpay->T_o_trace_no[%s] ",phv_notpay->T_o_trace_no);        
    vtcp_log(" hv_notpay->T_lw_ind[%s] ",phv_notpay->T_lw_ind);        
    vtcp_log(" hv_notpay->T_send_tel[%s] ",phv_notpay->T_send_tel);        

    vtcp_log(" hv_notpay->T_o_cmtno[%s] ",phv_notpay->T_o_cmtno);        
    vtcp_log(" hv_notpay->T_lw_ind[%s] ",phv_notpay->T_lw_ind);        

}
/************************************************************************* 
* 函 数 名: pub_hv_handle_CMT417                  
* 功能描述： 公共数据更新报文： 不记登记簿，直接更改
* 作    者:  ChenMing                             
* 完成日期： 2006-8-20                             
* 修改记录：                                      
*   日   期:                                      
*   修 改 人:                                     
*   修改内容:                                      
*************************************************************************/ 
int pub_hv_handle_CMT417(THVPACKAGE* phv_pack)
{
    char cDataType[4];
    memset( cDataType, 0, sizeof(cDataType) );    
    hv_get_tag(phv_pack, "CE7", cDataType);
    vtcp_log("cDataType = [%s]", cDataType);

    if (memcmp(cDataType,"104",3)==0 || memcmp(cDataType,"105",3)==0)
    {
        if (iHv_417_104And105(phv_pack))
        {
            sprintf(acErrMsg,"更新公共数据失败 ");
            WRITEMSG
                return -1;          
        }
    }
    if ( memcmp(cDataType,"202",3)==0 )
    {
        if (iHv_417_202(phv_pack))
        {
            sprintf(acErrMsg,"更新公共数据失败 ");
            WRITEMSG
                return -1;          
        }
    }    
    if ( memcmp(cDataType,"205",3)==0 )
    {
        if (iHv_417_205(phv_pack))
        {
            sprintf(acErrMsg,"更新公共数据失败 ");
            WRITEMSG
                return -1;          
        }
    }      
    vtcp_log("更新公共数据成功 !");
    return 0;
}

/*   以下为处理417报文的公共函数      */

int iHv_417_104And105(THVPACKAGE* phv_pack)
{
    int  headsize,bodysize;
    char tmpStr[20];
    char *headend=NULL;
    char *bodyend=NULL;
    char *headpos=NULL;
    char *bodypos=NULL;
    char *curp=NULL;
    char *data=NULL;

    data = malloc(phv_pack->length);
    if (data==NULL)
    {
        sprintf(acErrMsg,"分配内存出错");
        MBFEWRITEMSG
            return -1;
    }
    memset(data,'\0',phv_pack->length);

    if (hv_get_tag(phv_pack,"CLX",data) !=0 )
    {
        vtcp_log("[%s][%d]==没有找到tag:CLX.",__FILE__,__LINE__);        
        sprintf(acErrMsg,"没有找到tag:CLX");
        free(data);
        data=NULL;        
        MBFEWRITEMSG
        return -1;
    }
    printf("[%s]%d]data==[%s]",__FILE__,__LINE__,data);

    /*取得要修改的数据*/
    curp = data;
    if (memcmp(curp,"CNAPS_BANK_UPDATE_INFO",strlen("CNAPS_BANK_UPDATE_INFO"))==0)
    {
        iHv_417_UpdateBank(phv_pack);

    }else if (memcmp(curp,"CNAPS_EBANK_UPDATE_INFO",strlen("CNAPS_EBANK_UPDATE_INFO"))==0)
    {
        iHv_417_UpdateEisBank(phv_pack);
    }else
    {
        vtcp_log("[%s][%d]==识别码错误.",__FILE__,__LINE__);        
        sprintf(acErrMsg,"识别码错误");
        free(data);
        data=NULL;        
        MBFEWRITEMSG
        return -1;      
    }
    free(data);
    data=NULL;
    vtcp_log("[%s][%d] iHv_417_104And105() 成功!!",__FILE__,__LINE__); 
    return 0;
}

int iHv_417_202(THVPACKAGE* phv_pack)
{
    char *curp=NULL;
    char *data=NULL;
    char tmpBuff[4];
    int  iCnt=0;
    int  i=0;
    char type;
    char cTimedc[9];
    char cTdcname[21];
    char cUpdtdc[19];
    char updmethod;
    char cSdate[9];
    struct hv_syspara_c hv_syspara;
    int ret;

    data = malloc(phv_pack->length);
    if (data==NULL)
    {
        sprintf(acErrMsg,"分配内存出错");
        MBFEWRITEMSG
        return -1;
    }    
    memset(data,'\0',phv_pack->length);
    memset(tmpBuff,'\0',sizeof(tmpBuff));
    memset(cTimedc,'\0',sizeof(cTimedc));
    memset(cTdcname,'\0',sizeof(cTdcname));
    memset(cUpdtdc,'\0',sizeof(cUpdtdc));
    memset(cSdate,'\0',sizeof(cSdate));
    memset(&hv_syspara,'\0',sizeof( struct hv_syspara_c));

    if (hv_get_tag(phv_pack,"CLX",data) !=0 )
    {
        vtcp_log("[%s][%d]==没有找到tag:CLX.",__FILE__,__LINE__);
        free(data);
        data=NULL;
        return -1;
    }
    /*取得要修改的数据*/
    curp = data;
    memcpy(tmpBuff,curp,sizeof(tmpBuff)-1);
    vtcp_log("[%s][%d]==tmpBuff=[%s]",__FILE__,__LINE__,tmpBuff);

    curp = curp + 3;
    iCnt = atoi(tmpBuff);
    while ( i < iCnt )
    {
        type = *curp;
        curp++;
        memcpy(cTimedc,curp,sizeof(cTimedc)-1);
        curp = curp + 8;
        memcpy(cTdcname,curp,sizeof(cTdcname)-1);
        curp = curp + 20;
        memcpy(cUpdtdc,curp,sizeof(cUpdtdc)-1);
        curp = curp + 18;
        updmethod = *curp;
        curp++;
        memcpy(cSdate,curp,sizeof(cSdate)-1);
        curp = curp + 8;
       			
        if (type=='1') /* 增加 */
        {
            memcpy(hv_syspara.sdate,cSdate,sizeof(hv_syspara.sdate)-1);
            hv_syspara.updtype[0] = type;
            hv_syspara.datatype[0] = '1';
            memcpy(hv_syspara.timedc,cTimedc,sizeof(hv_syspara.timedc)-1);
            memcpy(hv_syspara.tdcname,cTdcname,sizeof(hv_syspara.tdcname)-1);
            memcpy(hv_syspara.updtdc,cUpdtdc,sizeof(hv_syspara.updtdc)-1);
            hv_syspara.updmethod[0] = updmethod;          

            ret = Hv_syspara_Ins(hv_syspara, g_pub_tx.reply);
            if (ret != 0)
            {
                free(data);                   
                sprintf(acErrMsg,"插入大额储蓄参数表错误!![%d]",ret);
                WRITEMSG
                return ret;
            }
            vtcp_log("[%s][%d] 向大额储蓄参数表中增加记录成功! ",__FILE__,__LINE__);  
        }
        else if (type=='3')
        {
            ret = Hv_syspara_Dec_Upd(g_pub_tx.reply," timedc ='%s' and  datatype='1' ", cTimedc );
            if (ret != 0)
            {
                free(data);              
                sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
                WRITEMSG
                    return ret;
            }
            
            memset(&hv_syspara,'\0',sizeof(hv_syspara));           
            ret = Hv_syspara_Fet_Upd(&hv_syspara, g_pub_tx.reply);
            if (ret != 0 && ret != 100)
            {
                free(data);                
                sprintf(acErrMsg,"查询大额储蓄参数表错误!![%d]",ret);
                WRITEMSG
                return ret;
            }
            else if (ret == 100)
            {
            	  /**** Modified 2006.12.30.  如果不存在该记录则插入一条 ********
                free(data);                
                sprintf(acErrMsg,"大额储蓄参数表不存在该记录!!");
                WRITEMSG
                    strcpy(g_pub_tx.reply,"O127");
                return ret;
                **************************************************************/
                sprintf(acErrMsg,"大额储蓄参数表不存在该记录，则插入该记录 !!");
                WRITEMSG
                memcpy(hv_syspara.sdate,cSdate,sizeof(hv_syspara.sdate)-1);
                hv_syspara.updtype[0] = type;
                hv_syspara.datatype[0] = '1';
                memcpy(hv_syspara.timedc,cTimedc,sizeof(hv_syspara.timedc)-1);
                memcpy(hv_syspara.tdcname,cTdcname,sizeof(hv_syspara.tdcname)-1);
                memcpy(hv_syspara.updtdc,cUpdtdc,sizeof(hv_syspara.updtdc)-1);
                hv_syspara.updmethod[0] = updmethod;          
                
                ret = Hv_syspara_Ins(hv_syspara, g_pub_tx.reply);
                if (ret != 0)
                {
                    free(data);                   
                    sprintf(acErrMsg,"插入大额储蓄参数表错误!![%d]",ret);
                    WRITEMSG
                    return ret;
                }
                vtcp_log("[%s][%d] 向大额储蓄参数表中增加记录成功! ",__FILE__,__LINE__);  
            }    
            
            memcpy( hv_syspara.tdcname,   cTdcname,  sizeof(hv_syspara.tdcname)-1 );
            memcpy( hv_syspara.updtdc,    cUpdtdc,   sizeof(hv_syspara.updtdc     )-1 );
            hv_syspara.updmethod[0] = updmethod; 

            ret = Hv_syspara_Upd_Upd(hv_syspara, g_pub_tx.reply);
            if (ret != 0)
            {
                free(data);                
                sprintf(acErrMsg,"更新大额储蓄参数表错误 ! [%d]",ret);
                WRITEMSG
                    return ret;
            }    
            Hv_syspara_Clo_Upd();  
            vtcp_log("[%s][%d] 更新大额储蓄参数表成功",__FILE__,__LINE__);  

        }
        else if (type=='2')
        {
            ret = Hv_syspara_Dec_Upd(g_pub_tx.reply," timedc ='%s' and  datatype='1' ", cTimedc );
            if (ret != 0)
            {
                free(data);                
                sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
                WRITEMSG
                    return ret;
            }
            memset(&hv_syspara,'\0',sizeof(hv_syspara));   
            ret = Hv_syspara_Fet_Upd(&hv_syspara, g_pub_tx.reply);
            if (ret != 0 && ret != 100)
            {
                free(data);                
                sprintf(acErrMsg,"查询大额储蓄参数表错误!![%d]",ret);
                WRITEMSG
                    return ret;
            }
            else if (ret == 100)
            {
            	  /**** Modified 2006.12.30.  如果不存在该记录则插入一条 ********
                free(data);                
                sprintf(acErrMsg,"大额储蓄参数表不存在该记录!!");
                WRITEMSG
                    strcpy(g_pub_tx.reply,"O127");
                return ret;
                **************************************************************/
                sprintf(acErrMsg,"大额储蓄参数表不存在该记录，则插入该记录 !!");
                WRITEMSG
                memcpy(hv_syspara.sdate,cSdate,sizeof(hv_syspara.sdate)-1);
                hv_syspara.updtype[0] = type;
                hv_syspara.datatype[0] = '1';
                memcpy(hv_syspara.timedc,cTimedc,sizeof(hv_syspara.timedc)-1);
                memcpy(hv_syspara.tdcname,cTdcname,sizeof(hv_syspara.tdcname)-1);
                memcpy(hv_syspara.updtdc,cUpdtdc,sizeof(hv_syspara.updtdc)-1);
                hv_syspara.updmethod[0] = updmethod;          
                
                ret = Hv_syspara_Ins(hv_syspara, g_pub_tx.reply);
                if (ret != 0)
                {
                    free(data);                   
                    sprintf(acErrMsg,"插入大额储蓄参数表错误!![%d]",ret);
                    WRITEMSG
                    return ret;
                }
                vtcp_log("[%s][%d] 向大额储蓄参数表中增加记录成功! ",__FILE__,__LINE__);   
            }    
            memcpy( hv_syspara.updtype,   "2",  sizeof(hv_syspara.updtype)-1 );
            ret = Hv_syspara_Upd_Upd(hv_syspara, g_pub_tx.reply);
            if (ret != 0)
            {
                free(data);                
                sprintf(acErrMsg,"更新大额储蓄参数表错误 ! [%d]",ret);
                WRITEMSG
                    return ret;
            }    
            Hv_syspara_Clo_Upd();  
            vtcp_log("[%s][%d] 更新大额储蓄参数表成功",__FILE__,__LINE__);

        }
        else
        {
            free(data);            
            vtcp_log("[%s][%d]ccpc传送的标志错误=[%c] ",__FILE__,__LINE__,hv_syspara.updtype[0]);  
            sprintf(acErrMsg,"ccpc传送的标志错误 ! ");
            WRITEMSG
                return  -1;
        }
        i++;

    }  /* end of while( i < iCnt )*/
    
    free(data);
    data=NULL;
    return 0;
}

int iHv_417_205(THVPACKAGE* phv_pack)
{
    char *curp=NULL;
    char *data=NULL;
    char type;
    char cStartAmt[19];
    char cSdate[9];
    char cTxamt[16], *endptr;    /** 交易金额 **/
    double txamt=0.00;
    struct hv_syspara_c hv_syspara;
    int ret;

    data = malloc(phv_pack->length);
    memset(data,'\0',phv_pack->length);
    memset(cStartAmt,'\0',sizeof(cStartAmt));
    memset(cSdate,'\0',sizeof(cSdate));
    memset(cTxamt,0,sizeof(cTxamt));
    memset(&hv_syspara,'\0',sizeof( struct hv_syspara_c));

    if (hv_get_tag(phv_pack,"CLX",data) !=0 )
    {
        printf("[%s][%d]==没有找到tag:CLX.",__FILE__,__LINE__);
        free(data);
        data=NULL;
        return -1;
    }
    /*取得要修改的数据*/
    curp = data;
    type = *curp; /* 类型只有变更 */
    curp++;
    memcpy(cStartAmt,curp,sizeof(cStartAmt)-1);
    curp = curp + 18;;
    memcpy(cSdate,curp,sizeof(cSdate)-1);
    memcpy(cTxamt,cStartAmt+3,sizeof(cStartAmt)-1);
    txamt = strtod(cTxamt, &endptr);
    hv_syspara.updtype[0] = type;
    memcpy(hv_syspara.sdate,cSdate,sizeof(hv_syspara.sdate)-1);
    hv_syspara.startamt = txamt;
    hv_syspara.datatype[0] = '2';

    ret = Hv_syspara_Ins(hv_syspara, g_pub_tx.reply);
    if (ret != 0)
    {
        free(data);        
        sprintf(acErrMsg,"插入大额储蓄参数表错误!![%d]",ret);
        WRITEMSG
            return ret;
    }
    vtcp_log("[%s][%d] 向大额储蓄参数表中增加记录成功! ",__FILE__,__LINE__);  

    free(data);
    data=NULL;
    return 0;
}

/* 处理行名行号更新 */
char *iHv_nextbankln(THVPACKAGE *phvpack,char *begin)
{
    char *ptr;
    int length;
    char *searchpos;
    length=phvpack->length;
    ptr = phvpack->body;

    if (begin == NULL)
    {
        /*从开始位置*/
        searchpos = strstr(ptr,"batchno");
        if (searchpos==NULL)
        {
            vtcp_log("%s,%d, quit from here! ",__FILE__,__LINE__);
            return (NULL);/*未找到*/
        }
        else
        {
            searchpos += strlen("batchno(20050020)");
            while (searchpos[0]==0x0a||searchpos[0]==0x0d)
            {              
                searchpos++;
            }
            return (searchpos);
        }
    }
    else
    {
        /*从指定位置*/
        searchpos = begin;
        while (searchpos[0]!=0x0a && searchpos[0]!=0x0d && searchpos[0]!=0x00 && searchpos[0]!='}')
        {
            if (searchpos>=ptr+length)
            {
                vtcp_log("%s,%d,Here,length=%d",__FILE__,__LINE__,length);
                vtcp_log("%s,%d,quit from here!",__FILE__,__LINE__);
                return(NULL);/*未找到*/
            }
            searchpos++;
        }
        if (searchpos[0]==0x00 ||searchpos[0]=='}')
        {
            vtcp_log("%s,%d,quit from here! ",__FILE__,__LINE__);
            return(NULL);/*未找到*/
        }
        while (searchpos[0]==0x0a||searchpos[0]==0x0d)
        {
            searchpos++;  
        }
        return(searchpos);
    }
    return 0;
}

char *iHv_getbankfld(char *bankline,int fldid)
{
    int pos1,pos2;
    int searchid;
    static char fldbuf[1024];
    static char *retptr;
    searchid=0;
    pos1=pos2=0;
    if (fldid<0)
    {
        retptr=bankline;
        fldbuf[0]='\0';
        return(fldbuf);
    }
    else
    {
        retptr=retptr+strlen(retptr)+1;
    }
    while (searchid<fldid)
    {
        if (bankline[pos1]=='|')
        {
            searchid++;
        }else if(bankline[pos1]==0x00 || bankline[pos1]==0x0a || bankline[pos1]==0x0d || bankline[pos1]=='}')
        {
            retptr[0]='\0';
            return(retptr);
        }
        pos1++;
    }
    
    /*pos1指向寻找的字段的开始位置，且跳过了|**/
    if (bankline[pos1]=='|' ||bankline[pos1]==0x00 || bankline[pos1]==0x0a || bankline[pos1]==0x0d || bankline[pos1]=='}')
    {
        retptr[0]='\0';
        return(retptr);
    }
    else
    {
        pos2=pos1+1;
        while (bankline[pos2]!=0x00 && bankline[pos2]!=0x0a && bankline[pos2]!=0x0d && bankline[pos2]!='}' && bankline[pos2]!='|')
            pos2++;
        memcpy(retptr,bankline+pos1,pos2-pos1);
        retptr[pos2-pos1]='\0';
        return(retptr);
    }
    return(retptr);  
}

int iHv_417_UpdateBank(THVPACKAGE *phvpack)
{
    int ret;
    char  *ptr;
    struct hv_uniontab_c hv_uniontab;  
    char updtype;
    char cAccountno[13];
    char cSendco[5];
    ptr=NULL;

    while ( (ptr = iHv_nextbankln(phvpack,ptr)) )
    {

        memset(cAccountno, 0, sizeof(cAccountno));
        memset(cSendco,  0, sizeof(cSendco));
        memset(&hv_uniontab,0,sizeof(struct hv_uniontab_c));
        
        iHv_getbankfld(ptr,-1);
        updtype = *(iHv_getbankfld(ptr,0));
        vtcp_log("%s,%d,updtype=[%c]",__FILE__,__LINE__,updtype);  
        if( updtype == '\0')
        {
          break;
        }
        vtcp_log("%s,%d,本次语句:\n%.60s",__FILE__,__LINE__,ptr);

        switch(updtype)
        {
        case '1':
            /*变更*/
            iHv_getbankfld(ptr,-1);
            /*add by wsh 2005-6-21 16:02*/
            memcpy(cAccountno,iHv_getbankfld(ptr,5),sizeof(cAccountno)-1);
            memcpy(cSendco, iHv_getbankfld(ptr,6),sizeof(cSendco)-1);
            zip_space(cSendco);
            vtcp_log("[%s][%d]cSendco=[%s]",__FILE__,__LINE__,cSendco);
            /*如果发报中心代码不存在则取清算行的发报中心代码*/
            if (cSendco[0]=='\0' || memcmp(cSendco,"NULL",4)==0 || memcmp(cSendco,"NULL",4)==0)
            {
                find_sendco( cAccountno, cSendco );
                vtcp_log("[%s][%d]cSendco=[%s]",__FILE__,__LINE__,cSendco);
            }
            ret = Hv_uniontab_Dec_Upd(g_pub_tx.reply," or_br_no='%s' ", iHv_getbankfld(ptr,1));
            if (ret != 0)
            {
                sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
                WRITEMSG
                return ret;
            }
            ret = Hv_uniontab_Fet_Upd(&hv_uniontab, g_pub_tx.reply);
            if (ret != 0 && ret != 100)
            {
                sprintf(acErrMsg,"查询大额支付行号对照表错误!![%d]",ret);
                WRITEMSG
                return ret;
            }
            else if (ret == 100)
            {
                sprintf(acErrMsg,"大额支付行号对照表不存在该记录!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"O127");
                return ret;
            }    
            memcpy( hv_uniontab.start_date,iHv_getbankfld(ptr,2),sizeof(hv_uniontab.start_date)-1 );
            memcpy( hv_uniontab.qs_no,     iHv_getbankfld(ptr,5),sizeof(hv_uniontab.qs_no     )-1 );/*清算帐号 ？32/12 */
            memcpy( hv_uniontab.sendco,    cSendco              ,sizeof(hv_uniontab.sendco    )-1 );          
            memcpy( hv_uniontab.br_name_f, iHv_getbankfld(ptr,10),sizeof(hv_uniontab.br_name_f)-1 );          
            memcpy( hv_uniontab.br_name_s, iHv_getbankfld(ptr,11),sizeof(hv_uniontab.br_name_s)-1 );            
            memcpy( hv_uniontab.addr,      iHv_getbankfld(ptr,12),sizeof(hv_uniontab.addr     )-1 );            
            /**变更时加上更新时间和上次更新操作 20131119 hzh begin**/
            memcpy( hv_uniontab.upd_type,  "1",1);
            sprintf( hv_uniontab.chg_date,"%ld",g_pub_tx.tx_date);
            vtcp_log("%s,%d,upd_type:[%s],chg_date:[%s]",__FILE__,__LINE__,hv_uniontab.upd_type,hv_uniontab.chg_date);
            /**变更时加上更新时间和上次更新操作 20131119 hzh end**/
            /*         sprintf(sqlbuf,"update hvuniontab set sdate='%s',accountno='%s',sendco='%s',lname='%s',sname='%s',address='%s' where bakcode='%s'",
            iHv_getbankfld(ptr,2),iHv_getbankfld(ptr,5),cSendco,iHv_getbankfld(ptr,10),iHv_getbankfld(ptr,11),
            iHv_getbankfld(ptr,12),iHv_getbankfld(ptr,1));    */

            ret = Hv_uniontab_Upd_Upd(hv_uniontab, g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"更新大额支付行号对照表出错 ! [%d]",ret);
                WRITEMSG
                return ret;
            }    
            Hv_uniontab_Clo_Upd();  
            vtcp_log("[%s][%d] 更新大额支付行号对照表成功",__FILE__,__LINE__);  
            break;
        case '2':
            /*新增*/
            iHv_getbankfld(ptr,-1);
            /*add by wsh 2005-6-21 16:02*/
            memcpy(cAccountno,iHv_getbankfld(ptr,6),sizeof(cAccountno)-1);
            memcpy(cSendco,   iHv_getbankfld(ptr,7),sizeof(cSendco)-1);
            zip_space(cSendco);
            vtcp_log("cAccountno=[%s]  cSendco=[%s]",cAccountno,cSendco);
            /*如果发报中心代码不存在则取清算行的发报中心代码*/
            if (cSendco[0]=='\0' || memcmp(cSendco,"NULL",4)==0 || memcmp(cSendco,"null",4)==0)
            {
                find_sendco(cAccountno,cSendco);
                vtcp_log("[%s][%d]cSendco=[%s]",__FILE__,__LINE__,cSendco);
            }
            memcpy( hv_uniontab.or_br_no,  iHv_getbankfld(ptr,1), sizeof(hv_uniontab.or_br_no  )-1 );
            memcpy( hv_uniontab.start_date,iHv_getbankfld(ptr,17),sizeof(hv_uniontab.start_date)-1 );
            memcpy( hv_uniontab.end_date,  iHv_getbankfld(ptr,18),sizeof(hv_uniontab.end_date  )-1 );
            memcpy( hv_uniontab.qs_no,     iHv_getbankfld(ptr,6), sizeof(hv_uniontab.qs_no     )-1 );/*清算帐号 ？32/12 */
            memcpy( hv_uniontab.sendco,    cSendco              , sizeof(hv_uniontab.sendco    )-1 );          
            memcpy( hv_uniontab.br_name_f, iHv_getbankfld(ptr,11),sizeof(hv_uniontab.br_name_f )-1 );          
            memcpy( hv_uniontab.br_name_s, iHv_getbankfld(ptr,12),sizeof(hv_uniontab.br_name_s )-1 );            
            memcpy( hv_uniontab.addr,      iHv_getbankfld(ptr,13),sizeof(hv_uniontab.addr      )-1 );            
            hv_uniontab.obr_no_attr[0] = '1';
            hv_uniontab.br_sts[0] = '0';

            /*        sprintf(sqlbuf,"insert into hvuniontab(bakcode,sdate,edate,accountno,sendco,lname,sname,address,obrnoflag,stat)
            values('%s','%s','%s','%s','%s','%s','%s','%s','1','0')", iHv_getbankfld(ptr,1),iHv_getbankfld(ptr,17),iHv_getbankfld(ptr,18),
            iHv_getbankfld(ptr,6),cSendco,iHv_getbankfld(ptr,11), iHv_getbankfld(ptr,12),iHv_getbankfld(ptr,13));*/

            ret = Hv_uniontab_Ins(hv_uniontab, g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"向大额支付行号对照表中增加记录错误!![%d]",ret);
                WRITEMSG
                    return ret;
            }
            vtcp_log("[%s][%d] 向大额支付行号对照表中增加记录成功! ",__FILE__,__LINE__);  
            break;
        case '3':
            /*注销*/
            iHv_getbankfld(ptr,-1);

            ret = Hv_uniontab_Dec_Upd(g_pub_tx.reply," or_br_no='%s' ", iHv_getbankfld(ptr,1));
            if (ret != 0)
            {
                sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
                WRITEMSG
                    return ret;
            }
            ret = Hv_uniontab_Fet_Upd(&hv_uniontab, g_pub_tx.reply);
            if (ret != 0 && ret != 100)
            {
                sprintf(acErrMsg,"查询大额支付行号对照表错误!![%d]",ret);
                WRITEMSG
                    return ret;
            }
            else if (ret == 100)
            {
                sprintf(acErrMsg,"大额支付行号对照表不存在该记录!!");
                WRITEMSG
                    strcpy(g_pub_tx.reply,"O127");
                return ret;
            }    
            memcpy(hv_uniontab.end_date, iHv_getbankfld(ptr,2),sizeof(hv_uniontab.end_date)-1 );

            /*         sprintf(sqlbuf,"update hvuniontab set edate='%s' where bakcode='%s'",iHv_getbankfld(ptr,2),iHv_getbankfld(ptr,1));*/

            ret = Hv_uniontab_Upd_Upd(hv_uniontab, g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"更新大额支付行号对照表出错 ! [%d]",ret);
                WRITEMSG
                    return ret;
            }    
            Hv_uniontab_Clo_Upd();  
            vtcp_log("[%s][%d] 更新大额支付行号对照表成功",__FILE__,__LINE__);             
            break;
        default:
            vtcp_log("%s,%d,发现一条非法数据:%60s",__FILE__,__LINE__,ptr);
            return(0);
        } /* end of switch(updtype)*/
    } /* end of while  */
    vtcp_log("%s,%d,iHv_417_UpdateBank() 成功! ",__FILE__,__LINE__);
    return 0;
}

int iHv_417_UpdateEisBank(THVPACKAGE *phvpack)
{
    int ret;
    char  *ptr;
    struct hv_uniontab_c hv_uniontab; 
    char updtype;       
    ptr=NULL;


    while ( (ptr=iHv_nextbankln(phvpack,ptr)) )
    {
      
        memset(&hv_uniontab,0,sizeof(struct hv_uniontab_c));      
        iHv_getbankfld(ptr,-1);
        updtype = *(iHv_getbankfld(ptr,0));
        vtcp_log("%s,%d,本次语句:/n%.60s",__FILE__,__LINE__,ptr);

        switch(updtype)
        {
        case '1':
            /*变更*/
            iHv_getbankfld(ptr,-1);

            ret = Hv_uniontab_Dec_Upd(g_pub_tx.reply," or_br_no='%s' ", iHv_getbankfld(ptr,1));
            if (ret != 0)
            {
                sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
                WRITEMSG
                    return ret;
            }
            ret = Hv_uniontab_Fet_Upd(&hv_uniontab, g_pub_tx.reply);
            if (ret != 0 && ret != 100)
            {
                sprintf(acErrMsg,"查询大额支付行号对照表错误!![%d]",ret);
                WRITEMSG
                    return ret;
            }
            else if (ret == 100)
            {
                sprintf(acErrMsg,"大额支付行号对照表不存在该记录!!");
                WRITEMSG
                    strcpy(g_pub_tx.reply,"O127");
                return ret;
            }    
            memcpy( hv_uniontab.start_date,iHv_getbankfld(ptr,2), sizeof(hv_uniontab.start_date)-1 );
            memcpy( hv_uniontab.qs_no,     "000998800006"       , sizeof(hv_uniontab.qs_no     )-1 );/*清算帐号 ？32/12 */
            memcpy( hv_uniontab.sendco,    iHv_getbankfld(ptr,5), sizeof(hv_uniontab.sendco    )-1 );          
            memcpy( hv_uniontab.br_name_f, iHv_getbankfld(ptr,8),sizeof(hv_uniontab.br_name_f)-1 );          
            memcpy( hv_uniontab.br_name_s, iHv_getbankfld(ptr,9),sizeof(hv_uniontab.br_name_s)-1 );            
            memcpy( hv_uniontab.addr,      iHv_getbankfld(ptr,10),sizeof(hv_uniontab.addr     )-1 );            
            /**变更时加上更新时间和上次更新操作 20131119 hzh begin**/
            memcpy( hv_uniontab.upd_type,  "1",1);
            sprintf( hv_uniontab.chg_date,"%ld",g_pub_tx.tx_date);
            vtcp_log("%s,%d,upd_type:[%s],chg_date:[%s]",__FILE__,__LINE__,hv_uniontab.upd_type,hv_uniontab.chg_date);
            /**变更时加上更新时间和上次更新操作 20131119 hzh end**/
            /*               sprintf(sqlbuf,"update hvuniontab set sdate='%s',accountno='%s',sendco='%s',lname='%s',sname='%s',address='%s' where bakcode='%s'",
            hv_getbankfld(ptr,2),"000998800006",hv_getbankfld(ptr,5),hv_getbankfld(ptr,8),hv_getbankfld(ptr,9),
            hv_getbankfld(ptr,10),hv_getbankfld(ptr,1)); */

            ret = Hv_uniontab_Upd_Upd(hv_uniontab, g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"更新大额支付行号对照表出错 ! [%d]",ret);
                WRITEMSG
                    return ret;
            }    
            Hv_uniontab_Clo_Upd();  
            vtcp_log("[%s][%d] 更新大额支付行号对照表成功",__FILE__,__LINE__);  
            break;

        case '2':
            /*新增*/
            iHv_getbankfld(ptr,-1);

            memcpy( hv_uniontab.or_br_no,  iHv_getbankfld(ptr,1), sizeof(hv_uniontab.or_br_no  )-1 );
            memcpy( hv_uniontab.start_date,iHv_getbankfld(ptr,13),sizeof(hv_uniontab.start_date)-1 );
            memcpy( hv_uniontab.end_date,  iHv_getbankfld(ptr,14),sizeof(hv_uniontab.end_date  )-1 );
            memcpy( hv_uniontab.qs_no,     "000998800006",        sizeof(hv_uniontab.qs_no     )-1 );/*清算帐号 ？32/12 */
            memcpy( hv_uniontab.sendco,    iHv_getbankfld(ptr,4),  sizeof(hv_uniontab.sendco    )-1 );          
            memcpy( hv_uniontab.br_name_f, iHv_getbankfld(ptr,7),sizeof(hv_uniontab.br_name_f )-1 );          
            memcpy( hv_uniontab.br_name_s, iHv_getbankfld(ptr,8),sizeof(hv_uniontab.br_name_s )-1 );            
            memcpy( hv_uniontab.addr,      iHv_getbankfld(ptr,9),sizeof(hv_uniontab.addr      )-1 );            
            hv_uniontab.obr_no_attr[0] = '1';
            hv_uniontab.br_sts[0] = '0';

            /*              sprintf(sqlbuf,"insert into hvuniontab(bakcode,sdate,edate,accountno,sendco,lname,sname,address,obrnoflag,stat)values('%s','%s','%s','%s','%s','%s','%s','%s','1','0')",
            hv_getbankfld(ptr,1),hv_getbankfld(ptr,13),hv_getbankfld(ptr,14),"000998800006",hv_getbankfld(ptr,4),hv_getbankfld(ptr,7),hv_getbankfld(ptr,8),hv_getbankfld(ptr,9));*/

            ret = Hv_uniontab_Ins(hv_uniontab, g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"向大额支付行号对照表中增加记录错误!![%d]",ret);
                WRITEMSG
                    return ret;
            }
            vtcp_log("[%s][%d] 向大额支付行号对照表中增加记录成功! ",__FILE__,__LINE__);  
            break;

        case '3':
            /*注销*/
            iHv_getbankfld(ptr,-1);

            ret = Hv_uniontab_Dec_Upd(g_pub_tx.reply," or_br_no='%s' ", iHv_getbankfld(ptr,1));
            if (ret != 0)
            {
                sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
                WRITEMSG
                    return ret;
            }
            ret = Hv_uniontab_Fet_Upd(&hv_uniontab, g_pub_tx.reply);
            if (ret != 0 && ret != 100)
            {
                sprintf(acErrMsg,"查询大额支付行号对照表错误!![%d]",ret);
                WRITEMSG
                    return ret;
            }
            else if (ret == 100)
            {
                sprintf(acErrMsg,"大额支付行号对照表不存在该记录!!");
                WRITEMSG
                    strcpy(g_pub_tx.reply,"O127");
                return ret;
            }    
            memcpy(hv_uniontab.end_date, iHv_getbankfld(ptr,2),sizeof(hv_uniontab.end_date)-1 );
            /*               sprintf(sqlbuf,"update hvuniontab set edate='%s' where bakcode='%s'",hv_getbankfld(ptr,2),hv_getbankfld(ptr,1)); */
            ret = Hv_uniontab_Upd_Upd(hv_uniontab, g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"更新大额支付行号对照表出错 ! [%d]",ret);
                WRITEMSG
                    return ret;
            }    
            Hv_uniontab_Clo_Upd();  
            vtcp_log("[%s][%d] 更新大额支付行号对照表成功",__FILE__,__LINE__);             
            break;   

        default:
            vtcp_log("%s,%d,发现一条非法数据:%60s",__FILE__,__LINE__,ptr);
            return(0);
        }
    }
    return 0;
}

/************************************************************************* 
* 函 数 名: pub_hv_handle_CMT418                  
* 功能描述： 处理系统状态变更通知报文： 不记登记簿，直接更改原状态
* 作    者:  ChenMing                             
* 完成日期： 2006-8-20                             
* 修改记录：                                      
*   日   期:                                      
*   修 改 人:                                     
*   修改内容:                                      
*************************************************************************/ 
int pub_hv_handle_CMT418(THVPACKAGE* phv_pack)
{
    int  iOldDate;   /* 原系统日期 */
    int  iNewDate;
    char newStat[3];
    char oldStat[3];
    char cNewDate[9];
    char newMsg[61];

    vtcp_log("%s,%d,进入418报文处理程序!",__FILE__,__LINE__);
    memset(newMsg,'\0',sizeof(newMsg));
    memset(cNewDate,'\0',sizeof(cNewDate));
    memset(newStat,'\0',sizeof(newStat));
    memset(oldStat,'\0',sizeof(oldStat));

    hv_get_tag(phv_pack,"CI5",newStat);/*新状态*/
    hv_get_tag(phv_pack,"CI4",oldStat);
    hv_get_tag(phv_pack,"CJ2",cNewDate);
    hv_get_tag(phv_pack,"72A",newMsg);

    iOldDate = hv_getday();    
    if ( iOldDate  < 0 )
    {
        sprintf(acErrMsg,"得到支付系统工作日期错误[%d]",iOldDate);
        WRITEMSG
        return iOldDate;  
    }
    iNewDate = apatoi(cNewDate,8);
    vtcp_log("=======iOldDate==[%d] iNewDate==[%d]  ========",iOldDate,iNewDate);
    vtcp_log("=======oldStat==[%s]  newStat==[%s]   ========",oldStat,newStat);     
    if ( iOldDate != iNewDate )
    {
        char cStat;
        cStat = hv_getstat();
        vtcp_log("=======cStat==[%c]  ========",cStat);
        vtcp_log("%s,%d ",__FILE__,__LINE__);
        
        /* 因为有可能前几个418没收到，而后一个418先收到，所以无论如何，先置上filler1 
        if ( cStat==HV_STAT_WORKEND || cStat==HV_STAT_CHECKERR ) Modified by ChenMing 2006-11-22 */
        if( cStat != HV_STAT_CHECKRIGHT )
        {
            if ( hv_setmsg("对账未平，不能进行日切")) return -1;
            vtcp_log("%s,%d ",__FILE__,__LINE__);
            if ( hv_setfiller1(cNewDate)) return -1;      
            vtcp_log("%s,%d ",__FILE__,__LINE__);
            sprintf(acErrMsg,"对账未平，不能进行日切");
            vtcp_log("%s,%d ",__FILE__,__LINE__);
            WRITEMSG         
            vtcp_log("%s,%d ",__FILE__,__LINE__);
            return 0;  
        }
        vtcp_log("%s,%d ",__FILE__,__LINE__);
        if (hv_setday(iNewDate)) return -1;
        vtcp_log("%s,%d ",__FILE__,__LINE__);
    }
    vtcp_log("%s,%d ",__FILE__,__LINE__);
    vtcp_log("=======newMsg==[%s]  ========",newMsg);    
    vtcp_log("%s,%d ",__FILE__,__LINE__);
    if (newMsg[0]!='\0')
    {
        vtcp_log("%s,%d ",__FILE__,__LINE__);
        if ( hv_setmsg(newMsg) ) return -1;
        vtcp_log("%s,%d ",__FILE__,__LINE__);
    }
    vtcp_log("%s,%d ",__FILE__,__LINE__);
    if (memcmp(newStat,"00",2)==0)   /*营业准备*/
    {  
        vtcp_log("%s,%d ",__FILE__,__LINE__);
        if ( hv_setstat(HV_STAT_PREPARE) ) return -1;
        vtcp_log("%s,%d ",__FILE__,__LINE__);
    }else  if (memcmp(newStat,"10",2)==0)   /*日间*/
    {  
        vtcp_log("%s,%d ",__FILE__,__LINE__);
        vtcp_log("=======newStat==[%s] oldStat==[%s] ========",newStat,oldStat);          
        if (memcmp(oldStat,"10",2)==0)/*原状态与新状态都为日间，实际上是一个业务截至警告*/
        { 
            vtcp_log("=======oldStat==[%s]  ========",oldStat);            
            if ( hv_setstat(HV_STAT_WARN) ) return -1;
        }
        else
        {
            vtcp_log("=======oldStat==[%s]  ========",oldStat);              
            if ( hv_setstat(HV_STAT_WORK) ) return -1;
        }
        vtcp_log("%s,%d ",__FILE__,__LINE__);
    }else if(memcmp(newStat,"20",2)==0)
    {/*业务截至*/
        vtcp_log("%s,%d ",__FILE__,__LINE__);
        if ( hv_setstat(HV_STAT_STOP) ) return -1;
    }else if(memcmp(newStat,"30",2)==0)
    {/*清算窗口*/
        vtcp_log("%s,%d ",__FILE__,__LINE__);
        if ( hv_setstat(HV_STAT_QSWIN) ) return -1;
    }else if(memcmp(newStat,"40",2)==0)
    {/*日终处理*/
        vtcp_log("%s,%d ",__FILE__,__LINE__);
        if ( hv_setstat(HV_STAT_WORKEND) ) return -1;
    }else
    {
        vtcp_log("%s,%d,非法的业务状态[%.2s]",__FILE__,__LINE__,newStat);
        sprintf(acErrMsg,"非法的业务状态[%.2s]",newStat);
        WRITEMSG
            return -1;    
    }
    return 0;
}
/************************************************************************* 
* 函 数 名: pub_hv_chargemx_reg                  
* 功能描述： 登记 802 支付业务收费清单报文的明细
* 作    者:  ChenMing                             
* 完成日期： 2006-8-20                             
* 修改记录：                                      
*   日   期:                                      
*   修 改 人:                                     
*   修改内容:                                      
*************************************************************************/ 
int pub_hv_chargemx_reg(THVPACKAGE* phv_pack, HV_PAYIN_AREA* phv_payin)
{
    struct hv_chargemx_c hv_chargemx;  
    char  *EL1,*EL2,*EL3;
    int  ret = 0;
    int  iRecNum,iDtlid;
    char cRecNum [9];    /** 记录个数 **/
    char cBr_no  [9];      
    char cAddid  [9];      

    char cSdate  [9];      
    char cEdate  [9];      

    
    int  indate;
    int  sdate;
    int  edate;    
    
    memset(cBr_no,0, sizeof(cBr_no));    
    memset(cAddid,0, sizeof(cAddid));
    memset(cSdate,0, sizeof(cSdate));
    memset(cEdate,0, sizeof(cEdate));
    memset(cAddid,0, sizeof(cAddid));
    
    memset(&hv_chargemx,0, sizeof(struct hv_chargemx_c));     
    if ( memcmp(phv_pack->CMTCode,"802",3 ) )
    {
        sprintf(acErrMsg,"该报文不是 802 报文 ! ");
        MBFEWRITEMSG
        return -1;       
    }

    indate = apatoi(phv_pack->workDate,8);
    memcpy(cBr_no,phv_payin->T_tx_br_no,sizeof(phv_payin->T_tx_br_no));
    
    memcpy(cAddid,phv_payin->T_addid,sizeof(cAddid)-1); /* addid */

    vtcp_log("%s,%d,  cAddid=====[%s]",__FILE__,__LINE__,cAddid);   
    
    iHVGETTAG(phv_pack,"072",cSdate,sizeof(cEdate)-1); /* 计费开始日期  */
    iHVGETTAG(phv_pack,"073",cEdate,sizeof(cEdate)-1); /* 计费终止日期   */    

    vtcp_log("%s,%d, 计费开始日期  cSdate=[%s]",__FILE__,__LINE__,cSdate);   
    vtcp_log("%s,%d, 计费终止日期  cEdate=[%s]",__FILE__,__LINE__,cEdate);           
    sdate =  atoi(cSdate);
    edate =  atoi(cEdate);
    vtcp_log("%s,%d, cSdate=====[%d]",__FILE__,__LINE__,cSdate);   
    vtcp_log("%s,%d, cEdate=====[%d]",__FILE__,__LINE__,cEdate);           

    /*先检查是否今天有过记录  暂定一日只有一笔,不能重复插入,否则要增加orderno字段 */
    ret = Hv_chargemx_Sel(g_pub_tx.reply, &hv_chargemx, "in_date=%d", indate);
    if ( ret == 0 )
    {
        sprintf(acErrMsg,"收费清单明细表中已有今日记录 !  ret=[%s] ",ret);
        MBFEWRITEMSG
            return -1;     
    }else if( ret != 100 )
    {
        sprintf(acErrMsg,"查询支付业务收费清单明细表出错 ! ret=[%s] ",ret);
        MBFEWRITEMSG
            return -1;       
    }
    
    memset(cRecNum,0,sizeof(cRecNum));
    hv_get_tag( phv_pack,"ED1", cRecNum );  /** 支付类业务收费记录个数 **/
    vtcp_log("支付类业务收费记录个数 cRecNum=[%s]",cRecNum);   
    iRecNum = atoi(cRecNum);
    if ( iRecNum > 0 )
    {
        EL1 = (char *)malloc( (3+1+40+10+6+6+6+15)*iRecNum + 1);
        if ( NULL == EL1 )
        {
            sprintf(acErrMsg,"申请内存失败,程序错误 ! ");
            MBFEWRITEMSG
            return -1;        
        }
        memset(EL1,0,sizeof(char)*((3+1+40+10+6+6+6+15)*iRecNum + 1) );
        hv_get_tag(phv_pack,"EL1",EL1);   vtcp_log("[%s]",EL1); 
        iDtlid  = 1;        
        if ( pub_hv_ins_hv_chargemx(EL1, iDtlid, iRecNum, indate,cBr_no,cAddid,sdate,edate,'1') ) /* 1支付类  2单边  3信息类 */
        {
            free(EL1);
            sprintf(acErrMsg,"插入 hv_chargemx 失败 ! ");
            MBFEWRITEMSG
            return -1; 
        }
        free(EL1);        
    }
    
    iDtlid = iDtlid + iRecNum;
    memset(cRecNum,0,sizeof(cRecNum));
    hv_get_tag( phv_pack,"ED2", cRecNum );  /** 单边业务收费记录个数 **/       
    vtcp_log("单边业务收费记录个数 cRecNum=[%s]",cRecNum); 
    iRecNum = atoi(cRecNum);
    if ( iRecNum > 0 )
    {
        EL2 = (char *)malloc( (3+40+10+6+6+6+15)*iRecNum + 1);
        if ( NULL == EL2 )
        {
            sprintf(acErrMsg,"申请内存失败,程序错误 ! ");
            MBFEWRITEMSG
            return -1;        
        }
        memset(EL2,0,sizeof(char)*((3+40+10+6+6+6+15)*iRecNum + 1) );

        hv_get_tag(phv_pack,"EL2",EL2); vtcp_log("[%s]",EL2); 

        if ( pub_hv_ins_hv_chargemx(EL2, iDtlid, iRecNum,indate,cBr_no,cAddid,sdate,edate, '2') ) /* 1支付类  2单边  3信息类 */
        {
            free(EL2);          
            sprintf(acErrMsg,"插入 hv_chargemx 失败 ! ");
            MBFEWRITEMSG
            return -1; 
        }
        free(EL2);        
    }
    iDtlid = iDtlid + iRecNum;   
    memset(cRecNum,0,sizeof(cRecNum));   
    hv_get_tag( phv_pack,"ED3", cRecNum );  /** 信息类业务收费记录个数 **/       
    vtcp_log("信息类业务收费记录个数 cRecNum=[%s]",cRecNum); 
    iRecNum = atoi(cRecNum);
    if ( iRecNum > 0 )
    {
        EL3 = (char *)malloc( (3+1+10+6+6+15)*iRecNum + 1);
        if ( NULL == EL3 )
        {
            sprintf(acErrMsg,"申请内存失败,程序错误 ! ");
            MBFEWRITEMSG
            return -1;        
        }
        memset(EL3,0,sizeof(char)*((3+1+10+6+6+15)*iRecNum + 1) );
        hv_get_tag(phv_pack,"EL3",EL3);  vtcp_log("[%s]",EL3); 

        if ( pub_hv_ins_hv_chargemx(EL3, iDtlid, iRecNum,indate, cBr_no,cAddid,sdate,edate,'3') ) /* 1支付类  2单边  3信息类 */
        {
            free(EL3);             
            sprintf(acErrMsg,"插入 hv_chargemx 失败 ! ");
            MBFEWRITEMSG
            return -1; 
        }
        free(EL3);
    } 
    
    vtcp_log("pub_hv_chargemx_reg() 成功 !!");   
    return 0;
}
/************************************************************************* 
* 函 数 名: pub_hv_81x_reg                  
* 功能描述： 登记 812或814 支付业务收费清单报文的明细
* 作    者:  Dongxy                             
* 完成日期： 2006-4-2                             
* 修改记录：                                      
*   日   期:                                      
*   修 改 人:                                     
*   修改内容:                                      
*************************************************************************/ 
int pub_hv_81x_reg(THVPACKAGE* phv_pack, HV_PAYIN_AREA* phv_payin)
{
    struct hv_81x_c hv_81x;  
    char  *EL1,*EL2,*EL3;
	char	*endptr;
    int  ret = 0;
    int  iRecNum,iDtlid;
    char cRecNum [9];    /** 记录个数 **/
    char cBr_no  [7];      
    char cAddid  [9];      
    char    cCmtno          [4 ];   /** 报文CMT号 **/
    char    cTxday          [9 ];   /** 报文日期 **/
	char    cTxamt          [16];   /** 计费金额 **/

    char cSdate  [9];      
    char cEdate  [9];      
    
    int  indate;
    int  sdate;
    int  edate;    
    int  Num;    
    
    memset(cCmtno,0, sizeof(cCmtno));    
    memset(cTxday,0, sizeof(cTxday));    
    memset(cTxamt,0, sizeof(cTxamt));    
    memset(cBr_no,0, sizeof(cBr_no));    
    memset(cAddid,0, sizeof(cAddid));
    memset(cSdate,0, sizeof(cSdate));
    memset(cEdate,0, sizeof(cEdate));
    memset(cAddid,0, sizeof(cAddid));
    
    memset(&hv_81x,0, sizeof(struct hv_81x_c));     
    if ( memcmp(phv_pack->CMTCode,"812",3 ) && memcmp(phv_pack->CMTCode,"814",3 ) )
    {
        sprintf(acErrMsg,"该报文不是 81x 报文 ! ");
        MBFEWRITEMSG
        return -1;       
    }

    memcpy(cCmtno, phv_pack->CMTCode, 3);
    hv_get_tag(phv_pack,"30A",cTxday);

    memcpy(cBr_no,phv_payin->T_tx_br_no,sizeof(phv_payin->T_tx_br_no));
    
    iHVGETTAG(phv_pack,"072",cSdate,sizeof(cEdate)-1); /* 计费开始日期  */
    iHVGETTAG(phv_pack,"073",cEdate,sizeof(cEdate)-1); /* 计费终止日期   */    

    vtcp_log("%s,%d, 计费开始日期  cSdate=[%s]\n",__FILE__,__LINE__,cSdate);   
    vtcp_log("%s,%d, 计费终止日期  cEdate=[%s]\n",__FILE__,__LINE__,cEdate);           
    sdate =  atoi(cSdate);
    edate =  atoi(cEdate);
    vtcp_log("%s,%d, cSdate=====[%d]\n",__FILE__,__LINE__,sdate);   
    vtcp_log("%s,%d, cEdate=====[%d]\n",__FILE__,__LINE__,edate);           

    ret = Hv_81x_Sel(g_pub_tx.reply, &hv_81x, "in_date='%s' and cmtno = '%s'", cTxday,cCmtno);
    if ( ret == 0 )
    {
    	vtcp_log("%s,%d, 收费清单81x明细表中已有今日记录\n",__FILE__,__LINE__);           
        sprintf(acErrMsg,"收费清单81x明细表中已有今日记录 !  ret=[%s] ",ret);
        MBFEWRITEMSG
            return -1;     
    }else if( ret != 100 )
    {
    	vtcp_log("%s,%d, 查询81x支付业务收费清单明细表出错\n",__FILE__,__LINE__);           
        sprintf(acErrMsg,"查询81x支付业务收费清单明细表出错 ! ret=[%s] ",ret);
        MBFEWRITEMSG
            return -1;       
    }
    
	/** 计算已经有的数目 **/
	Num = sql_count("hv_81x", " in_date='%s' and cmtno = '%s'", cTxday,cCmtno);

	vtcp_log("[%s][%d]---------Num=[%d]\n",__FILE__,__LINE__,Num);
	
	/** 将所有报文都有的公共字段赋值 **/
	memcpy(hv_81x.cmtno,   cCmtno,sizeof(hv_81x.cmtno) -1 );
 	memcpy(hv_81x.br_no,   cBr_no,sizeof(hv_81x.br_no) -1 );
    sprintf(hv_81x.txtime, "%ld",  g_pub_tx.tx_time );
    hv_81x.orderno =  Num+1 ;
    hv_get_tag(phv_pack,"30A",hv_81x.in_date);
    hv_get_tag(phv_pack,"0B9",hv_81x.receco);
    hv_get_tag(phv_pack,"58A",hv_81x.cash_qs_no);
    hv_get_tag(phv_pack,"012",hv_81x.dr_br_no);
    hv_get_tag(phv_pack,"071",hv_81x.totmon);
    hv_get_tag(phv_pack,"072",hv_81x.sdate);
    hv_get_tag(phv_pack,"073",hv_81x.edate);

        vtcp_log("[%s][%d] hv_81x.cmtno=[%s]",__FILE__,__LINE__,hv_81x.cmtno);   
        vtcp_log("[%s][%d] hv_81x.br_no=[%s]",__FILE__,__LINE__,hv_81x.br_no);     
        vtcp_log("[%s][%d] hv_81x.in_date=[%s]",__FILE__,__LINE__,hv_81x.in_date);   
        vtcp_log("[%s][%d] hv_81x.tx_time=[%s]",__FILE__,__LINE__,hv_81x.txtime);     
        vtcp_log("[%s][%d] hv_81x.orderno=[%d]",__FILE__,__LINE__,hv_81x.orderno);   
        vtcp_log("[%s][%d] hv_81x.receco=[%s]",__FILE__,__LINE__,hv_81x.receco); 
        vtcp_log("[%s][%d] hv_81x.cash_qs_no=[%s]",__FILE__,__LINE__,hv_81x.cash_qs_no); 
        vtcp_log("[%s][%d] hv_81x.dr_br_no=[%s]",__FILE__,__LINE__,hv_81x.dr_br_no);
        vtcp_log("[%s][%d] hv_81x.totmon=[%s]",__FILE__,__LINE__,hv_81x.totmon);     
        vtcp_log("[%s][%d] hv_81x.sdate=[%s]",__FILE__,__LINE__,hv_81x.sdate);   
        vtcp_log("[%s][%d] hv_81x.edate=[%s]",__FILE__,__LINE__,hv_81x.edate);   
                
    /** 收费总金额 **/
    hv_get_tag(phv_pack,"33H",cTxamt); /**notice**/
    vtcp_log("[%s][%d]==cTxamt=====[%s]\n",__FILE__,__LINE__,cTxamt);
    hv_81x.tot_amt = strtod(cTxamt,&endptr);
    vtcp_log("[%s][%d]==========tot_amt=[%.2f]\n",__FILE__,__LINE__,hv_81x.tot_amt);

    memset(cRecNum,0,sizeof(cRecNum));
    hv_get_tag( phv_pack,"ED1", cRecNum );  /** 支付类业务收费记录个数 **/
    vtcp_log("支付类业务收费记录个数 cRecNum=[%s]",cRecNum);   
    iRecNum = atoi(cRecNum);
    if ( iRecNum > 0 )
    {
        EL1 = (char *)malloc( (12+15+15+15)*iRecNum + 1);
        if ( NULL == EL1 )
        {
            sprintf(acErrMsg,"申请内存失败,程序错误 ! ");
            MBFEWRITEMSG
            return -1;        
        }
        memset(EL1,0,sizeof(char)*((12+15+15+15)*iRecNum + 1) );
        hv_get_tag(phv_pack,"EL1",EL1);   vtcp_log("[%s]",EL1); 

        iDtlid  = 1;        

        hv_81x.chgtype[0] = '1';   /* 收费类型 1大额 2小额 */

        hv_get_tag(phv_pack,"33J",cTxamt); /**notice**/
        vtcp_log("[%s][%d]==cTxamt=====[%s]\n",__FILE__,__LINE__,cTxamt);
        hv_81x.txamt = strtod(cTxamt,&endptr);
        vtcp_log("[%s][%d]==========txamt=[%.2f]\n",__FILE__,__LINE__,hv_81x.txamt);

        hv_get_tag( phv_pack,"ED1",hv_81x.num);

        if ( pub_hv_ins_hv_81x(&hv_81x,EL1, iDtlid, iRecNum) ) 
        {
            free(EL1);
            sprintf(acErrMsg,"插入 hv_81x 失败 ! ");
            MBFEWRITEMSG
            return -1; 
        }
        free(EL1);        
    }
    
    iDtlid = iDtlid + iRecNum;

    memset(cRecNum,0,sizeof(cRecNum));
    hv_get_tag( phv_pack,"ED2", cRecNum );  /** 小额业务收费记录个数 **/       
    vtcp_log("[%s][%d]-------小额业务收费记录个数 cRecNum=[%s]\n",__FILE__,__LINE__,cRecNum); 
    iRecNum = atoi(cRecNum);
    if ( iRecNum > 0 )
    {
        EL2 = (char *)malloc( (12+15+15+15)*iRecNum + 1);
        if ( NULL == EL2 )
        {
            sprintf(acErrMsg,"申请内存失败,程序错误 ! ");
            MBFEWRITEMSG
            return -1;        
        }
        memset(EL2,0,sizeof(char)*((12+15+15+15)*iRecNum + 1) );

        hv_get_tag(phv_pack,"EL2",EL2); vtcp_log("[%s]",EL2); 

        hv_81x.chgtype[0] = '2';   /* 收费类型 1大额 2小额 */

        hv_get_tag(phv_pack,"33K",cTxamt); /**notice**/
        vtcp_log("[%s][%d]==cTxamt=====[%s]\n",__FILE__,__LINE__,cTxamt);
        hv_81x.txamt = strtod(cTxamt,&endptr);
        vtcp_log("[%s][%d]==========txamt=[%.2f]\n",__FILE__,__LINE__,hv_81x.txamt);

        hv_get_tag( phv_pack,"ED2",hv_81x.num);

        if ( pub_hv_ins_hv_81x(&hv_81x,EL2, iDtlid, iRecNum) ) /* 1支付类  2单边  3信息类 */
        {
            free(EL2);          
            sprintf(acErrMsg,"插入 hv_81x 失败 ! ");
            MBFEWRITEMSG
            return -1; 
        }
        free(EL2);        
    }
    free(EL1);        
    free(EL2);        
    
    vtcp_log("pub_hv_81x_reg() 成功 !!");   
    return 0;
}
/************************************************************************* 
* 函 数 名: pub_hv_chkmr_reg                  
* 功能描述： 登记 841 大额支付业务核对报文的明细
* 作    者:  ChenMing                             
* 完成日期： 2006-8-20                             
* 修改记录：                                      
*   日   期:                                      
*   修 改 人:                                     
*   修改内容:                                      
*************************************************************************/ 
int pub_hv_chkmr_reg(THVPACKAGE* phv_pack)
{
    int ret;
    char cmtno[4];
    char cTx_date[9];
    char cSendcnt     [11];
    char cSendamt     [23];
    char cSenditem    [3];
    char cRecevicecnt [11];
    char cReceviceamt [23];
    char cReceviceitem[3];
    char cOnlineitem[3];  
    struct hv_chkmr_c hv_chkmr;

    memset(cmtno, 0, sizeof(cmtno) );
    memset(cTx_date, 0, sizeof(cTx_date) ); 
    memset(cSendcnt     , 0 , sizeof(cSendcnt     ));
    memset(cSendamt     , 0 , sizeof(cSendamt     ));
    memset(cSenditem    , 0 , sizeof(cSenditem    ));
    memset(cRecevicecnt , 0 , sizeof(cRecevicecnt ));
    memset(cReceviceamt , 0 , sizeof(cReceviceamt ));
    memset(cReceviceitem, 0 , sizeof(cReceviceitem));
    memset(cOnlineitem,0,sizeof(cOnlineitem));
    memset(&hv_chkmr, 0, sizeof(struct hv_chkmr_c) );    

    memcpy(cmtno, phv_pack->CMTCode, 3);                
    vtcp_log("[%s][%d]cmtno=[%.3s]",__FILE__,__LINE__,cmtno);    
    if (pub_hv_check_cmtno(cmtno) != 0)
    {
        vtcp_log("[%s][%d] cmtno=[%s] -------cmtno 错误！",__FILE__,__LINE__,cmtno);            
        return -1;
    }                        
    if ( memcmp(cmtno,"841",3) )
    {
        vtcp_log("[%s][%d] 传入的不报文不是 841 报文 !",__FILE__,__LINE__);
        sprintf(acErrMsg," 传入的不报文不是 841 报文 ! !  ");
        MBFEWRITEMSG
            return -1;  
    }
    
    /***************************************************************************/
    /*** Begin Added  在收到841报文时，增加清除对帐的几个表中的数据 ************/
    
    deldb( "hv_chkmr"," 1=1 " );       /* 总计核对总表 */
    deldb( "hv_chkmrdtl"," 1=1 " );    /* 总计核对明细 */
    deldb( "hv_chkagemr"," 1=1 " );    /* 支付业务明细表  */
    deldb( "hv_chkerror"," 1=1 " );    /* 核对错误登记表 */
    
    /*** End Added    在收到841报文时，增加清除对帐的几个表中的数据 ************/
    /***************************************************************************/

    
    hv_get_tag(phv_pack,"30A",cTx_date);  
    hv_chkmr.tx_date = apatoi(cTx_date,8);          
    hv_get_tag(phv_pack,"CC6",hv_chkmr.ccpc_no); 
    hv_get_tag(phv_pack,"CDJ",cSendcnt);  
    hv_chkmr.send_cnt = apatoi(cSendcnt,10);
    hv_get_tag(phv_pack,"CMJ",cSendamt);  
    vtcp_log("[%s][%d] cSendamt=[%s] ",__FILE__,__LINE__,cSendamt);      
    dot_str2dbl(cSendamt,22,&hv_chkmr.send_amt); 
    hv_chkmr.send_amt = hv_chkmr.send_amt /100;   
  
    hv_get_tag(phv_pack,"CD1",cSenditem); 
    hv_chkmr.send_item = atoi(cSenditem);
    hv_get_tag(phv_pack,"CLK",hv_chkmr.send_dtl);

    hv_get_tag(phv_pack,"CDI",cRecevicecnt);
    hv_chkmr.recevice_cnt = apatoi(cRecevicecnt,10); 
    hv_get_tag(phv_pack,"CMI",cReceviceamt);
    vtcp_log("[%s][%d] cReceviceamt=[%s] ",__FILE__,__LINE__,cReceviceamt);        
    dot_str2dbl(cReceviceamt,22,&hv_chkmr.recevice_amt);
    hv_chkmr.recevice_amt = hv_chkmr.recevice_amt /100;
    
    hv_get_tag(phv_pack,"CD2",cReceviceitem);
    hv_chkmr.recevice_item = atoi(cReceviceitem);    
    hv_get_tag(phv_pack,"CLL",hv_chkmr.recevice_dtl);

    hv_get_tag(phv_pack,"CDQ",cOnlineitem);
    hv_chkmr.online_item = atoi(cOnlineitem);     
    hv_get_tag(phv_pack,"CLU",hv_chkmr.online_dtl);  

    vtcp_log("hv_chkmr.tx_date=[%d]",hv_chkmr.tx_date);  
    vtcp_log("hv_chkmr.ccpcno=[%s]",hv_chkmr.ccpc_no);     
    vtcp_log("hv_chkmr.send_cnt=[%d]",hv_chkmr.send_cnt);     
    vtcp_log("hv_chkmr.send_amt=[%f]",hv_chkmr.send_amt); 
    vtcp_log("hv_chkmr.send_item=[%d]",hv_chkmr.send_item); 
    vtcp_log("hv_chkmr.recevice_cnt=[%d]",hv_chkmr.recevice_cnt); 
    vtcp_log("hv_chkmr.recevice_amt=[%f]",hv_chkmr.recevice_amt); 
    vtcp_log("hv_chkmr.recevice_item=[%d]",hv_chkmr.recevice_item); 
    vtcp_log("hv_chkmr.online_item=[%d]",hv_chkmr.online_item);     

    ret = Hv_chkmr_Ins( hv_chkmr, g_pub_tx.reply );
    if ( ret )
    {
        sprintf(acErrMsg,"向大额支付业务核对主表中增加记录错误!![%d]",ret);
        WRITEMSG
            return ret;
    }
    vtcp_log("[%s][%d] 插入大额支付业务核对主表成功 ! !  ",__FILE__,__LINE__);  

    ret = pubf_hv_ins_hv_chkmrdtl( &hv_chkmr );  
    if ( ret )
    {
        sprintf(acErrMsg,"向大额支付业务核对明细表中增加记录错误!![%d]",ret);
        WRITEMSG
            return ret;
    }
    vtcp_log("[%s][%d] 插入大额支付业务核对明细表成功 ! !  ",__FILE__,__LINE__);  
    return 0; 
}
/************************************************************************* 
* 函 数 名: pubf_hv_ins_hv_chkmrdtl                  
* 功能描述： 登记 841 大额支付业务核对报文的明细
* 作    者:  ChenMing                             
* 完成日期： 2006-8-20                             
* 修改记录：                                      
*   日   期:                                      
*   修 改 人:                                     
*   修改内容:                                      
*************************************************************************/ 
int pubf_hv_ins_hv_chkmrdtl(struct hv_chkmr_c* phv_chkmr )
{
    int ret;

    /*   CLK   */
    ret = vAnalysSendCheckItem(0, phv_chkmr->send_dtl, "CMT100", phv_chkmr->tx_date);       if(ret)     return ret;
    ret = vAnalysSendCheckItem(1, phv_chkmr->send_dtl, "CMT101", phv_chkmr->tx_date);       if(ret)     return ret;
    ret = vAnalysSendCheckItem(2, phv_chkmr->send_dtl, "CMT102", phv_chkmr->tx_date);       if(ret)     return ret;
    ret = vAnalysSendCheckItem(3, phv_chkmr->send_dtl, "CMT103", phv_chkmr->tx_date);       if(ret)     return ret;
    ret = vAnalysSendCheckItem(4, phv_chkmr->send_dtl, "CMT109", phv_chkmr->tx_date);       if(ret)     return ret;
    ret = vAnalysSendCheckItem(5, phv_chkmr->send_dtl, "CMT121", phv_chkmr->tx_date);       if(ret)     return ret;
    ret = vAnalysSendCheckItem(6, phv_chkmr->send_dtl, "CMT122", phv_chkmr->tx_date);       if(ret)     return ret;
    ret = vAnalysSendCheckItem(7, phv_chkmr->send_dtl, "CMT123", phv_chkmr->tx_date);       if(ret)     return ret;
    ret = vAnalysSendCheckItem(8, phv_chkmr->send_dtl, "CMT124", phv_chkmr->tx_date);       if(ret)     return ret;
    ret = vAnalysSendCheckItem(9, phv_chkmr->send_dtl, "CMT105", phv_chkmr->tx_date);       if(ret)     return ret;
    ret = vAnalysSendCheckItem(10,phv_chkmr->send_dtl, "CMT108", phv_chkmr->tx_date);       if(ret)     return ret;

    /*   CLL   */
    ret = vAnalysReceviceCheckItem(0, phv_chkmr->recevice_dtl, "CMT100", phv_chkmr->tx_date);       if(ret)     return ret;
    ret = vAnalysReceviceCheckItem(1, phv_chkmr->recevice_dtl, "CMT101", phv_chkmr->tx_date);       if(ret)     return ret;
    ret = vAnalysReceviceCheckItem(2, phv_chkmr->recevice_dtl, "CMT102", phv_chkmr->tx_date);       if(ret)     return ret;
    ret = vAnalysReceviceCheckItem(3, phv_chkmr->recevice_dtl, "CMT103", phv_chkmr->tx_date);       if(ret)     return ret;
    ret = vAnalysReceviceCheckItem(4, phv_chkmr->recevice_dtl, "CMT109", phv_chkmr->tx_date);       if(ret)     return ret;
    ret = vAnalysReceviceCheckItem(5, phv_chkmr->recevice_dtl, "CMT121", phv_chkmr->tx_date);       if(ret)     return ret;
    ret = vAnalysReceviceCheckItem(6, phv_chkmr->recevice_dtl, "CMT122", phv_chkmr->tx_date);       if(ret)     return ret;
    ret = vAnalysReceviceCheckItem(7, phv_chkmr->recevice_dtl, "CMT123", phv_chkmr->tx_date);       if(ret)     return ret;
    ret = vAnalysReceviceCheckItem(8, phv_chkmr->recevice_dtl, "CMT124", phv_chkmr->tx_date);       if(ret)     return ret;
    ret = vAnalysReceviceCheckItem(9, phv_chkmr->recevice_dtl, "CMT105", phv_chkmr->tx_date);       if(ret)     return ret;
    ret = vAnalysReceviceCheckItem(10,phv_chkmr->recevice_dtl, "CMT108", phv_chkmr->tx_date);       if(ret)     return ret;

    /*   CLU   */
    ret = vAnalysSendCheckItem(0,phv_chkmr->online_dtl,"CMT232",phv_chkmr->tx_date);       if(ret)      return ret;
    ret = vAnalysSendCheckItem(1,phv_chkmr->online_dtl,"CMT407",phv_chkmr->tx_date);       if(ret)      return ret;
    ret = vAnalysSendCheckItem(2,phv_chkmr->online_dtl,"CMT408",phv_chkmr->tx_date);       if(ret)      return ret;
    ret = vAnalysReceviceCheckItem(3,phv_chkmr->online_dtl,"CMT232",phv_chkmr->tx_date);   if(ret)      return ret;
    ret = vAnalysReceviceCheckItem(4,phv_chkmr->online_dtl,"CMT407",phv_chkmr->tx_date);   if(ret)      return ret;
    ret = vAnalysReceviceCheckItem(5,phv_chkmr->online_dtl,"CMT408",phv_chkmr->tx_date);   if(ret)      return ret;

    return 0;
}  
int vAnalysSendCheckItem( int item,const char* content, const char* cmtno,const int tx_date )
{
    double dCnt=0;
    double dAmt=0;  
    int ret;

    vAnalyseCheckItem(item, content, &dCnt, &dAmt);
    ret = iHv_TreatHvcheckmrdtl(1, cmtno, tx_date, dCnt, dAmt);
    if (ret)
    {
        vtcp_log("[%s][%d] iHv_TreatHvcheckmrdtl() 时出错 !  ",__FILE__,__LINE__);        
        sprintf(acErrMsg,"向支付交易表中增加记录/修改记录错误!![%d]",ret);
        WRITEMSG
        return ret;
    }
    return 0;

}
int vAnalysReceviceCheckItem( int item,const char* content, const char* cmtno,const int tx_date )
{
    double dCnt=0;
    double dAmt=0;
    int ret;

    vAnalyseCheckItem( item, content, &dCnt, &dAmt);
    ret = iHv_TreatHvcheckmrdtl(2,cmtno,tx_date,dCnt,dAmt);
    if ( ret )
    {
        vtcp_log("[%s][%d] iHv_TreatHvcheckmrdtl() 时出错 !  ",__FILE__,__LINE__);        
        sprintf(acErrMsg,"向支付交易表中增加记录/修改记录错误!![%d]",ret);
        WRITEMSG
            return ret;
    }
    return 0;
}
int vAnalyseCheckItem(int item, char* content, double* dCnt, double* dAmt)
{
    char cCnt    [12];
    char cAmt    [23];
    char cContent[353];

    memset(cCnt    , 0 , sizeof(cCnt    ));
    memset(cAmt    , 0 , sizeof(cAmt    ));
    memset(cContent, 0 , sizeof(cContent));
    memcpy(cContent, content, sizeof(cContent)-1);
    zip_space(cContent);

    memcpy(cCnt, cContent+item*32, 11);
    memcpy(cAmt, cContent+item*32+11, 22);
    dot_str2dbl(cCnt, sizeof(cCnt)-2, dCnt);
    dot_str2dbl(cAmt, sizeof(cAmt)-2, dAmt);
}
/****************************************************
* 函数名称:iHv_TreatHvcheckmrdtl                       *
* 描    述:根据发送接受标志rsflag写核对项目描述表   *
* 参    数:rsflag 1-发送 2-接受 cmtno 报文编号      *
*          txday 交易日期 cnt 总计笔数 amt总计金额  *
* 返    回: int 0-成功 1-失败                       *
****************************************************/
int iHv_TreatHvcheckmrdtl(int rsflag, const char* cmtno, int tx_date, double dCnt, double dAmt)
{
    int ret;
    struct hv_chkmrdtl_c hv_chkmrdtl;
    memset(&hv_chkmrdtl, 0 , sizeof(struct hv_chkmrdtl_c));

    ret = Hv_chkmrdtl_Sel(g_pub_tx.reply, &hv_chkmrdtl, " tx_date=%d and cmtno='%s' ", tx_date, cmtno);
    if ( ret==100 )   /* 未找到 */
    {
        hv_chkmrdtl.tx_date = tx_date;
        memcpy( hv_chkmrdtl.cmtno,cmtno,sizeof(hv_chkmrdtl.cmtno)-1);

        if (rsflag==1)
        {
            hv_chkmrdtl.s_cnt = dCnt;
            hv_chkmrdtl.s_amt = dAmt;
        }
        else
        {
            hv_chkmrdtl.r_cnt = dCnt;
            hv_chkmrdtl.r_amt = dAmt;
        }
        ret = Hv_chkmrdtl_Ins(hv_chkmrdtl, g_pub_tx.reply);
        if (ret)
        {
            sprintf(acErrMsg,"插入核对项目描述表错误 !! [%d]",ret);
            WRITEMSG
                return ret;      
        }
    }
    else if ( ret == 0 ) /* 找到有记录 ==> 更新 */
    {
        ret = Hv_chkmrdtl_Dec_Upd(g_pub_tx.reply, " tx_date=%d and cmtno='%s' ", tx_date, cmtno);
        if (ret != 0)
        {
            sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
            WRITEMSG
                return ret;
        }
        ret = Hv_chkmrdtl_Fet_Upd(&hv_chkmrdtl, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg," 查询核对项目描述表错误!! [%d] ",ret);
            WRITEMSG
                return ret;
        }
        else if (ret == 100)
        {
            sprintf(acErrMsg, "核对项目描述表不存在该记录!!" );
            WRITEMSG
                return ret;
        }    
        if (rsflag==1)
        {
            hv_chkmrdtl.s_cnt=dCnt;
            hv_chkmrdtl.s_amt=dAmt;
        }
        else
        {
            hv_chkmrdtl.r_cnt=dCnt;
            hv_chkmrdtl.r_amt=dAmt;
        }
        ret = Hv_chkmrdtl_Upd_Upd(hv_chkmrdtl, g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"更新核对项目描述表出错 ! [%d]",ret);
            WRITEMSG
                return ret;
        }    
        Hv_chkmrdtl_Clo_Upd();  
        vtcp_log("[%s][%d] 更新核对项目描述表成功",__FILE__,__LINE__);

    }
    else
    {
        sprintf(acErrMsg,"查询核对项目描述表出错 ! [%d]",ret);
        WRITEMSG
        return ret;
    }
    return 0;
}
/************************************************************************* 
* 函 数 名: pub_hv_ins_hv_chargemx                  
* 功能描述： 根据收费类型 插入收费清单明细
*     pEL:    指向业务收费明细域        
*     dtlid:  已有记录数+1 (下一条记录的dtlid)
*     recnum: 该业务收费明细域中的记录数EDn   
*     indate: 委托日期
*     chgtype:业务收费明细类型  1支付类 2单边 3信息类
* 作    者:  ChenMing                             
* 完成日期： 2006-8-20                             
* 修改记录：                                      
*   日   期:                                      
*   修 改 人:                                     
*   修改内容:                                      
*************************************************************************/ 
int pub_hv_ins_hv_chargemx(char* pEL, int dtlid, int recnum,int indate, char* br_no,char* cAddid, int sdate, int edate, char chgtype)
{
    int i,ret;
    int iDtlid;
    char* pRec,timespan[41];
    struct hv_chargemx_c hv_chargemx;    
    vtcp_log("dtlid=[%d]  recnum=[%d]  indate=[%d]  chgtype=[%c]  ",dtlid,recnum,indate,chgtype);    
    vtcp_log("cAddid=[%c]  sdate=[%d]  edate=[%d]  ",cAddid,sdate,edate);   
    
     
    if ( chgtype!='1' && chgtype!='2' && chgtype!='3'  ) 
    {
        sprintf(acErrMsg,"chgtype 错误 ! ");
        MBFEWRITEMSG
            return -1;
    }
    
    vtcp_log("[%s][%d]  cAddid=====[%s]",__FILE__,__LINE__,cAddid);  
    
    pRec = pEL; 
    for (i = 1; i <= recnum; i++ )   
    {
        memset(&hv_chargemx,0, sizeof(hv_chargemx));  
        memset(timespan,0, sizeof(timespan));  

        memcpy(hv_chargemx.br_no, br_no,sizeof(hv_chargemx.br_no)-1);
        hv_chargemx.in_date = indate;
        hv_chargemx.dtlid = dtlid + i -1;           /* 明细序号 */
        hv_chargemx.chgtype[0] = chgtype;   /* 收费类型 1支付类 2单边 3信息类 */
        hv_chargemx.edate = edate;
        hv_chargemx.sdate = sdate;
        memcpy(hv_chargemx.addid, cAddid, sizeof(hv_chargemx.addid)-1);


        if ( chgtype=='1' ) 
        {
            memcpy(hv_chargemx.cmtno,    pRec,    3 );   
            memcpy(hv_chargemx.srflag,   pRec+3,  1 );  
            /* memcpy(hv_chargemx.timespan, pRec+4,  40);    */                                           
            memcpy(timespan, pRec+4,  40); 
            memcpy(hv_chargemx.lowlimit, timespan+14,  6); 
            memcpy(hv_chargemx.upplimit, timespan+34,  6);                       

            memcpy(hv_chargemx.mnum,     pRec+44, 10);  
            memcpy(hv_chargemx.price,    pRec+54, 6 );             
            memcpy(hv_chargemx.brate,    pRec+60, 6 );  
            memcpy(hv_chargemx.pecrate,  pRec+66, 6 );             
            memcpy(hv_chargemx.txamt,    pRec+72, 15);  
            hv_chargemx.prtflag[0] = '0';    /*0 未打印 1已打印*/     

            if ( i < recnum )   pRec = pEL + 87;                             
        }
        else  if(chgtype=='2')
        {
            memcpy(hv_chargemx.cmtno,    pRec,    3 );   
            /* memcpy(hv_chargemx.timespan, pRec+4,  40);    */                                           
            memcpy(timespan, pRec+4,  40); 
            memcpy(hv_chargemx.lowlimit, timespan+14,  6); 
            memcpy(hv_chargemx.upplimit, timespan+34,  6); 

            memcpy(hv_chargemx.mnum,     pRec+43, 10);  
            memcpy(hv_chargemx.price,    pRec+53, 6 );             
            memcpy(hv_chargemx.brate,    pRec+59, 6 );  
            memcpy(hv_chargemx.pecrate,  pRec+65, 6 );             
            memcpy(hv_chargemx.txamt,    pRec+71, 15);  
            hv_chargemx.prtflag[0] = '0';    /*0 未打印 1已打印*/  

            if ( i < recnum )   pRec = pEL + 86;                
        }
        else  if(chgtype=='3')
        {
            memcpy(hv_chargemx.cmtno,    pRec,    3 );   
            memcpy(hv_chargemx.srflag,   pRec+3,  1 );  
            memcpy(hv_chargemx.mnum,     pRec+4,  10);  
            memcpy(hv_chargemx.price,    pRec+14, 6 );             
            memcpy(hv_chargemx.brate,    pRec+20, 6 );  
            memcpy(hv_chargemx.txamt,    pRec+26, 15);  
            hv_chargemx.prtflag[0] = '0';    /*0 未打印 1已打印*/  

            if ( i < recnum )    pRec = pEL + 41;                
        }

        vtcp_log("[%s][%d] hv_chargemx.in_date=[%d]",__FILE__,__LINE__,hv_chargemx.in_date);   
        vtcp_log("[%s][%d] hv_chargemx.dtlid=[%d]",__FILE__,__LINE__,hv_chargemx.dtlid);   
        vtcp_log("[%s][%d] hv_chargemx.sdate=[%d]",__FILE__,__LINE__,hv_chargemx.sdate);   
        vtcp_log("[%s][%d] hv_chargemx.edate=[%d]",__FILE__,__LINE__,hv_chargemx.edate);   
        
        vtcp_log("[%s][%d] hv_chargemx.br_no=[%s]",__FILE__,__LINE__,hv_chargemx.br_no);     
        vtcp_log("[%s][%d] hv_chargemx.srflag=[%s]",__FILE__,__LINE__,hv_chargemx.srflag);     
        vtcp_log("[%s][%d] hv_chargemx.cmtno=[%s]",__FILE__,__LINE__,hv_chargemx.cmtno); 
        vtcp_log("[%s][%d] hv_chargemx.upplimit=[%s]",__FILE__,__LINE__,hv_chargemx.upplimit);
        vtcp_log("[%s][%d] hv_chargemx.lowlimit=[%s]",__FILE__,__LINE__,hv_chargemx.lowlimit); 
        vtcp_log("[%s][%d] hv_chargemx.mnum=[%s]",__FILE__,__LINE__,hv_chargemx.mnum); 
        vtcp_log("[%s][%d] hv_chargemx.price=[%s]",__FILE__,__LINE__,hv_chargemx.price);
        vtcp_log("[%s][%d] hv_chargemx.pecrate=[%s]",__FILE__,__LINE__,hv_chargemx.pecrate);        
        vtcp_log("[%s][%d] hv_chargemx.brate=[%s]",__FILE__,__LINE__,hv_chargemx.brate);
        vtcp_log("[%s][%d] hv_chargemx.txamt=[%s]",__FILE__,__LINE__,hv_chargemx.txamt);
       
        vtcp_log("[%s][%d] hv_chargemx.addid=[%s]",__FILE__,__LINE__,hv_chargemx.addid);              
        vtcp_log("[%s][%d] hv_chargemx.chgtype=[%s]",__FILE__,__LINE__,hv_chargemx.chgtype);              
        vtcp_log("[%s][%d] hv_chargemx.prtflag=[%s]",__FILE__,__LINE__,hv_chargemx.prtflag); 
        
                
        ret = Hv_chargemx_Ins(hv_chargemx, g_pub_tx.reply);
        if (ret)
        {
            sprintf(acErrMsg,"插入收费清单明细表出错![%d]",ret);
            WRITEMSG
            return -1;
        }
    }
    vtcp_log("pub_hv_ins_hv_chargemx() 成功 !!");   
    return 0;
}
/************************************************************************* 
* 函 数 名: pub_hv_ins_hv_81x                  
* 功能描述： 根据收费类型 插入812/814收费清单明细
*     pEL:    指向业务收费明细域        
*     dtlid:  已有记录数+1 (下一条记录的dtlid)
*     recnum: 该业务收费明细域中的记录数EDn   
* 作    者:  dongxy                             
* 完成日期： 2007-4-2                             
* 修改记录：                                      
*   日   期:                                      
*   修 改 人:                                     
*   修改内容:                                      
*************************************************************************/ 
int pub_hv_ins_hv_81x(struct hv_81x_c *hv_81x_in,char* pEL, int dtlid, int recnum)
{
    int i,ret;
    int iDtlid;
    char	* pRec;
	char	*endptr;
    char    cTxamt          [16];   /** 计费金额 **/
    struct hv_81x_c hv_81x;    

    memset(cTxamt, 0x00, sizeof(cTxamt));
    memset(&hv_81x,0, sizeof(hv_81x));  
	memcpy(&hv_81x,hv_81x_in,sizeof(hv_81x));

    vtcp_log("[%s][%d]----------dtlid=[%d]  recnum=[%d]\n",__FILE__,__LINE__,dtlid,recnum);    
    
    pRec = pEL; 
    for (i = 1; i <= recnum; i++ )   
    {
		hv_81x.dtlid = dtlid + i -1;

		memcpy(hv_81x.csbrno, pRec,   12);

        memcpy(cTxamt,    pRec+12,  15 );
        vtcp_log("[%s][%d]==cTxamt=====[%s]\n",__FILE__,__LINE__,cTxamt);
        hv_81x.amt = strtod(cTxamt,&endptr);
        vtcp_log("[%s][%d]==========amt=[%.2f]\n",__FILE__,__LINE__,hv_81x.amt);

        memcpy(cTxamt,    pRec+27,  15 );
        vtcp_log("[%s][%d]==cTxamt=====[%s]\n",__FILE__,__LINE__,cTxamt);
        hv_81x.zf_amt = strtod(cTxamt,&endptr);
        vtcp_log("[%s][%d]==========zf_amt=[%.2f]\n",__FILE__,__LINE__,hv_81x.zf_amt);

        memcpy(cTxamt,    pRec+42,  15 );
        vtcp_log("[%s][%d]==cTxamt=====[%s]\n",__FILE__,__LINE__,cTxamt);
        hv_81x.xx_amt = strtod(cTxamt,&endptr);
        vtcp_log("[%s][%d]==========xx_amt=[%.2f]\n",__FILE__,__LINE__,hv_81x.xx_amt);

        if ( i < recnum )   pRec = pRec + 57;

        vtcp_log("[%s][%d] hv_81x.cmtno=[%s]",__FILE__,__LINE__,hv_81x.cmtno);   
        vtcp_log("[%s][%d] hv_81x.br_no=[%s]",__FILE__,__LINE__,hv_81x.br_no);     
        vtcp_log("[%s][%d] hv_81x.in_date=[%s]",__FILE__,__LINE__,hv_81x.in_date);   
        vtcp_log("[%s][%d] hv_81x.tx_time=[%s]",__FILE__,__LINE__,hv_81x.txtime);     
        vtcp_log("[%s][%d] hv_81x.orderno=[%d]",__FILE__,__LINE__,hv_81x.orderno);   
        vtcp_log("[%s][%d] hv_81x.dtlid=[%d]",__FILE__,__LINE__,hv_81x.dtlid);   
        vtcp_log("[%s][%d] hv_81x.chgtype=[%s]",__FILE__,__LINE__,hv_81x.chgtype);              
        vtcp_log("[%s][%d] hv_81x.receco=[%s]",__FILE__,__LINE__,hv_81x.receco); 
        vtcp_log("[%s][%d] hv_81x.cash_qs_no=[%s]",__FILE__,__LINE__,hv_81x.cash_qs_no); 
        vtcp_log("[%s][%d] hv_81x.dr_br_no=[%s]",__FILE__,__LINE__,hv_81x.dr_br_no);
        vtcp_log("[%s][%d] hv_81x.totmon=[%s]",__FILE__,__LINE__,hv_81x.totmon);     

        vtcp_log("[%s][%d] hv_81x.sdate=[%s]",__FILE__,__LINE__,hv_81x.sdate);   
        vtcp_log("[%s][%d] hv_81x.edate=[%s]",__FILE__,__LINE__,hv_81x.edate);   

        vtcp_log("[%s][%d] hv_81x.tot_amt=[%f]",__FILE__,__LINE__,hv_81x.tot_amt); 
        vtcp_log("[%s][%d] hv_81x.txamt=[%f]",__FILE__,__LINE__,hv_81x.txamt);
        vtcp_log("[%s][%d] hv_81x.num=[%s]",__FILE__,__LINE__,hv_81x.num);
        vtcp_log("[%s][%d] hv_81x.csbrno=[%s]",__FILE__,__LINE__,hv_81x.csbrno);        
        vtcp_log("[%s][%d] hv_81x.amt=[%f]",__FILE__,__LINE__,hv_81x.amt);
        vtcp_log("[%s][%d] hv_81x.zf_amt=[%f]",__FILE__,__LINE__,hv_81x.zf_amt);
        vtcp_log("[%s][%d] hv_81x.xx_amt=[%f]",__FILE__,__LINE__,hv_81x.xx_amt);
        vtcp_log("[%s][%d] ===========开始插入表了啊=========\n",__FILE__,__LINE__);
                
        ret = Hv_81x_Ins(hv_81x, g_pub_tx.reply);
        if (ret)
        {
            sprintf(acErrMsg,"插入收费清单明细表出错![%d]",ret);
            WRITEMSG
            return -1;
        }
    }
    vtcp_log("pub_hv_ins_hv_81x() 成功 !!");   
    return 0;
}
/************************************************************************* 
* 函 数 名: pubf_hv_ins_hv_chkagemr                  
* 功能描述： 向大额 CMT689 支付业务下载明细表 插入一条记录
* 作    者:  ChenMing                             
* 完成日期： 2006-8-20                             
* 修改记录：                                      
*   日   期:                                      
*   修 改 人:                                     
*   修改内容:                                      
*************************************************************************/ 
int pubf_hv_ins_hv_chkagemr(THVPACKAGE * hvpack)
{
    char cCmt[4];
    int  iCmt=0;
    struct hv_chkagemr_c hv_chkagemr;
    int  iRet=0;
    
    memset(&hv_chkagemr, 0 , sizeof(hv_chkagemr));
    memset(cCmt, 0 , sizeof(cCmt));
    memcpy(cCmt,hvpack->CMTCode,sizeof(cCmt)-1);
    iCmt = atoi(cCmt);
    memcpy(hv_chkagemr.cmtno,cCmt,sizeof(hv_chkagemr.cmtno)-1);
    hv_chkagemr.tx_time = g_pub_tx.tx_time;

    vtcp_log(" iCmt=[%d]", iCmt);  
    if ( pub_hv_AnalyseCommTag( hvpack,&hv_chkagemr ) ) 
    {
        sprintf(acErrMsg,"pub_hv_AnalyseCommTag 出错 ! ");
        WRITEMSG
            return -1;
    }
    
    vtcp_log(" iCmt=[%d]  intst=[%f]", iCmt, hv_chkagemr.intst);
    switch(iCmt)
    {
    case 100: /*CMT100  汇兑支付报文                 */
        vAnalyseHdTag(hvpack,&hv_chkagemr);
        break;
    case 101: /*CMT101  委托收款（划回）支付报文     */
        vAnalyseWtTag(hvpack,&hv_chkagemr);
        break;
    case 102: /*CMT102  托收承付 （划回）支付报文    */
        vAnalyseTscfTag(hvpack,&hv_chkagemr);
        break;
     case 103: /*CMT103 国库资金贷记划拨 报文  103   */
        vAnalyseGkzjhhTag(hvpack,&hv_chkagemr);
        break;
    case 105: /*CMT105  银行间同业拆借支付报文       */
        vAnalyseTycjTag(hvpack,&hv_chkagemr);
        break;
    case 108: /*CMT108  退汇支付报文                 */
        vAnalyseThzfTag(hvpack,&hv_chkagemr);
        break;
    case 109: /*CMT109  电子联行专用汇兑支付报文     */
        vAnalyseLhhdTag(hvpack,&hv_chkagemr);
        break;
    case 121: /*CMT121  银行汇票资金移存报文         */
        vAnalyseHpzjycTag(hvpack,&hv_chkagemr);
        break;
    case 122: /*CMT122  清算银行汇票资金信息         */
        vAnalyseHpzfTag(hvpack,&hv_chkagemr);
        break;
    case 123: /*CMT123  银行汇票多余资金划回报文     */
        vAnalyseHpHhTag(hvpack,&hv_chkagemr);
        break;
    case 124: /*CMT124  银行汇票资金未用退回报文     */
        vAnalyseHpwyThTag(hvpack,&hv_chkagemr);
        break;
    case 407: /*CMT407  质押融资通知报文             */
        vAnalyseZyrzTag(hvpack,&hv_chkagemr);
        break;
    case 408: /*CMT408  质押融资扣款通知报文         */
        vAnalyseZyrzkkTag(hvpack,&hv_chkagemr);
        break;
    case 312: /*CMT312  撤销应答报文                 */
        vAnalyseRespTag(hvpack,&hv_chkagemr);
        break;
    case 314: /*CMT314  申请退回应答报文             */
        vAnalyseTreadTag( hvpack, &hv_chkagemr );
        break;
    default:  vtcp_log(" iCmt=[%d] 有这种报文吗 ？ ", iCmt); 
        return 1;
    }
    vtcp_log(" intst ===[%.0f]  ", iCmt, hv_chkagemr.intst); 
    vtcp_log("[%s][%d] hv_chkagemr.orderno=[%s]",__FILE__,__LINE__,hv_chkagemr.orderno);
   
 /* iRet = iGetHvOrderno(hv_chkagemr.orderno);
    vtcp_log(" orderno ===[%s]  ",  hv_chkagemr.orderno);       
    if (iRet)
    {
        sprintf( acErrMsg,"得到支付交易序号错误![%d]",iRet);
        MBFEWRITEMSG
            return -1;
    }  */
    /*
    get_zd_data("0030",hv_chkagemr.br_no);   
    vtcp_log(" br_no ===[%.5s]  ", hv_chkagemr.br_no); 机构号   */
    
   iRet = memcmp(hv_chkagemr.pay_qs_no, HV_QSBRNO, sizeof(hv_chkagemr.pay_qs_no)-1);
   if(iRet) /* 来帐  */
   {
        GetBr_noByOr_br_no(hv_chkagemr.acbrno,hv_chkagemr.br_no);
   }
   else  /* 往帐  */
   {
         GetBr_noByOr_br_no(hv_chkagemr.or_br_no,hv_chkagemr.br_no);
   }
   
   vtcp_log(" br_no ===[%.5s]  ", hv_chkagemr.br_no);
   
/*vtcp_log("[%s][%d]>>>hv_chkagemr->pay_ac_no=[%s]",__FILE__,__LINE__,hv_chkagemr.pay_ac_no); 
  vtcp_log("[%s][%d]>>>hv_chkagemr->cash_ac_no=[%s]",__FILE__,__LINE__,hv_chkagemr.cash_ac_no); 

  新旧帐号转换 
  pub_base_old_acno(hv_chkagemr.cash_ac_no);
  pub_base_old_acno(hv_chkagemr.pay_ac_no);
  vtcp_log("[%s][%d]>>>hv_chkagemr->pay_ac_no=[%s]",__FILE__,__LINE__,hv_chkagemr.pay_ac_no); 
  vtcp_log("[%s][%d]>>>hv_chkagemr->cash_ac_no=[%s]",__FILE__,__LINE__,hv_chkagemr.cash_ac_no); 
 */  
 
    hv_chkagemr.tx_amt = hv_chkagemr.tx_amt/100.0;
  
    iRet = Hv_chkagemr_Ins(hv_chkagemr, g_pub_tx.reply);
    if (iRet<0)
    {
        sprintf(acErrMsg,"插入大额支付业务下载明细表出错! [%d] ",iRet);
        MBFEWRITEMSG
            return -1;        
    }   
    
    return 0;
}
/************************************************************************* 
* 函 数 名: pub_hv_AnalyseCommTag                  
* 功能描述： 解析 689 报文中的一条记录
* 作    者:  ChenMing                             
* 完成日期： 2006-8-20                             
* 修改记录：                                      
*   日   期:                                      
*   修 改 人:                                     
*   修改内容:                                      
*************************************************************************/
int pub_hv_AnalyseCommTag(THVPACKAGE * p_hvpack, struct hv_chkagemr_c * hv_chkagemr)
{
    char  cTx_date    [9 ];
    char  cPayactno   [33];  /** 付款人帐号 **/
    char  cCashactno  [33];  /** 收款人帐号 **/
    char  cPayname    [61];  /** 付款人名称 **/
    char  cCashname   [61];  /** 收款人名称 **/
    char  cTxt        [19];  /** 32A **/
    char  cTxamt      [16], *endptr;  /** 交易金额 **/
    char  cPaytxamt   [16];  /** 赔偿金额 CMT102报文专有**/
    double  dPtxamt=0;
    double  dTxamt=0;
    
    memset(cTx_date,0,sizeof(cTx_date));  
    memset(cPayname,0,sizeof(cPayname));
    memset(cCashname,0,sizeof(cCashname));
    memset(cPayactno, 0, sizeof(cPayactno));
    memset(cCashactno,0, sizeof(cCashactno));
    memset(cTxt,    0, sizeof(cTxt));
    memset(cTxamt,    0, sizeof(cTxamt));
    memset(cPaytxamt, 0, sizeof(cPaytxamt));

    hv_get_tag(p_hvpack,"33A",cPaytxamt);/* no uses here */
    dPtxamt = strtod(cPaytxamt,&endptr);
    hv_get_tag(p_hvpack,"CC4",hv_chkagemr->obrno);/*没有实际意义 */

    hv_get_tag(p_hvpack,"30A",cTx_date);  
    hv_chkagemr->tx_date = apatoi(cTx_date,8);

    hv_get_tag(p_hvpack,"32A",cTxt);
    memcpy(cTxamt, cTxt+3, sizeof(cTxamt) -1 );
    dTxamt = strtod(cTxamt, &endptr);
    hv_get_tag(p_hvpack,"011",hv_chkagemr->pay_qs_no);
    hv_get_tag(p_hvpack,"52A",hv_chkagemr->or_br_no);/* 发起行行号 */
    hv_get_tag(p_hvpack,"CC4",hv_chkagemr->pay_no); /* 付款人开户行行号 */
    hv_get_tag(p_hvpack,"50C",cPayactno);
    zip_space(cPayactno);
    hv_get_tag(p_hvpack,"50A",cPayname);
    zip_space(cPayname);
    hv_get_tag(p_hvpack,"012",hv_chkagemr->cashqsactno);/* 接收行清算帐号 */
    hv_get_tag(p_hvpack,"58A",hv_chkagemr->acbrno);     /* 接收行行号 */
    hv_get_tag(p_hvpack,"CC5",hv_chkagemr->cashno);      /* 收款人开户行行号 */
    hv_get_tag(p_hvpack,"59C",cCashactno);
    zip_space(cCashactno);
    hv_get_tag(p_hvpack,"59A",cCashname);
    zip_space(cCashname);
    hv_get_tag(p_hvpack,"0BC",hv_chkagemr->orderno);/*支付交易序号*/
    vtcp_log("[%s][%d] hv_chkagemr->orderno=[%s]",__FILE__,__LINE__,hv_chkagemr->orderno);
    
    hv_get_tag(p_hvpack,"010",hv_chkagemr->sendco);/* 发报中心代码 */
    hv_get_tag(p_hvpack,"0B9",hv_chkagemr->receco);/* 收报中心代码 */
    hv_get_tag(p_hvpack,"72A",hv_chkagemr->reason);
    zip_space(hv_chkagemr->reason);
    memcpy(hv_chkagemr->payname,  cPayname  ,sizeof(hv_chkagemr->payname)-1);
    memcpy(hv_chkagemr->pay_ac_no, cPayactno ,sizeof(hv_chkagemr->pay_ac_no)-1);
    memcpy(hv_chkagemr->cash_ac_no,cCashactno,sizeof(hv_chkagemr->cash_ac_no)-1);
    memcpy(hv_chkagemr->cash_name, cCashname ,sizeof(hv_chkagemr->cash_name)-1);
    memcpy(hv_chkagemr->cur_no,cTxt,3);
    hv_chkagemr->tx_amt = dTxamt;
    return 0;
}

/* 汇兑支付报文  */
int vAnalyseHdTag(THVPACKAGE * p_hvpack,struct hv_chkagemr_c * hv_chkagemr)
{
    char  cPayaddr    [61];
    char  cCashaddr   [61];
    memset(cPayaddr, 0, sizeof(cPayaddr)); 
    memset(cCashaddr,0, sizeof(cCashaddr));
    
    memcpy(hv_chkagemr->cmtno,"100",3);
    hv_get_tag(p_hvpack,"50B",cPayaddr);
    zip_space(cPayaddr);
    hv_get_tag(p_hvpack,"59B",cCashaddr);
    zip_space(cCashaddr);
    memcpy(hv_chkagemr->cash_addr,  cCashaddr, sizeof(hv_chkagemr->cash_addr)-1);
    hv_get_tag(p_hvpack,"CEF",hv_chkagemr->yw_type);
    return 0;
}
/*定期贷记支付 */
int vAnalyseDqdjzfTag(THVPACKAGE * p_hvpack,struct hv_chkagemr_c* hv_chkagemr)
{  
    char  cCashaddr   [61];
    memset(cCashaddr,0, sizeof(cCashaddr));
    
    memcpy(hv_chkagemr->cmtno,"104",3);
    hv_get_tag(p_hvpack,"0BF",hv_chkagemr->yw_type);
    hv_get_tag(p_hvpack,"59B",cCashaddr);
    zip_space(cCashaddr);
    memcpy(hv_chkagemr->cash_addr,  cCashaddr, sizeof(hv_chkagemr->cash_addr)-1);
    return 0;
}
/* 国库资金贷记划拨报文  103  */
int vAnalyseGkTag(THVPACKAGE * p_hvpack,struct hv_chkagemr_c * hv_chkagemr)
{
    char  cPayaddr    [61];
    char  cCashaddr   [61];
    memset(cPayaddr, 0, sizeof(cPayaddr)); 
    memset(cCashaddr,0, sizeof(cCashaddr));
    
    memcpy(hv_chkagemr->cmtno,"103",3);
    hv_get_tag(p_hvpack,"50B",cPayaddr);
    zip_space(cPayaddr);
    hv_get_tag(p_hvpack,"59B",cCashaddr);
    zip_space(cCashaddr);
    memcpy(hv_chkagemr->cash_addr,  cCashaddr, sizeof(hv_chkagemr->cash_addr)-1);
    hv_get_tag(p_hvpack,"0EF",hv_chkagemr->yw_type);
    return 0;
}
/*国库种类 */
int vAnalyseGkzjhhTag(THVPACKAGE * p_hvpack,struct hv_chkagemr_c* hv_chkagemr)
{
    char yw_type[3];
    memcpy(hv_chkagemr->cmtno,"103",3);
    
    hv_get_tag(p_hvpack,"0BF",yw_type);
    memcpy(hv_chkagemr->yw_type,yw_type,sizeof(hv_chkagemr->yw_type)-1);  
    return 0;  
}
/* 银行间同业拆借支付报文 */
int vAnalyseTycjTag(THVPACKAGE * p_hvpack,struct hv_chkagemr_c* hv_chkagemr)
{  
    char cRate[8];
    char cTerm[6];
    double dRate=0;
    int  iTerm=0;
    
    memset(cRate, 0 ,sizeof(cRate));
    memset(cTerm, 0 ,sizeof(cTerm));
    memcpy(hv_chkagemr->cmtno,"105",3);
    hv_get_tag(p_hvpack,"CF3",cRate);
    hv_get_tag(p_hvpack,"CF4",cTerm);
    /*vtcp_log("***************************************************");
    vtcp_log("[%s][%d] cRate===[%s] cTerm=====[%s] ",__FILE__,__LINE__,cRate,cTerm);
    vtcp_log("***************************************************");
    hv_get_tag(p_hvpack,"CEF",hv_chkagemr->yw_type);
    str2dbl(cRate,sizeof(cRate)-1,4,&dRate)  ;
    ******************************************/
    dRate = atof(cRate);
    iTerm = atoi(cTerm);
    hv_chkagemr->inst_rate=dRate/10000;
    hv_chkagemr->term    =iTerm;  
    return 0;
}
/* 退汇支付报文 108*/
int vAnalyseThzfTag(THVPACKAGE * p_hvpack,struct hv_chkagemr_c* hv_chkagemr)
{  
    char otxdate[9];
    memset(otxdate,0,sizeof(otxdate));
    
    memcpy(hv_chkagemr->cmtno,"108",3);
    hv_get_tag(p_hvpack,"051",hv_chkagemr->o_tx_date);

    hv_get_tag(p_hvpack,"02B",hv_chkagemr->o_cmtno);
    hv_get_tag(p_hvpack,"005",hv_chkagemr->o_orderno_hv);
    zip_space(hv_chkagemr->o_orderno_hv);
    hv_get_tag(p_hvpack,"CQ1",hv_chkagemr->pay_ac_no);
    zip_space(hv_chkagemr->pay_ac_no);
    hv_get_tag(p_hvpack,"CR1",hv_chkagemr->payname);
    zip_space(hv_chkagemr->payname);
    hv_get_tag(p_hvpack,"CQ2",hv_chkagemr->cash_ac_no);
    zip_space(hv_chkagemr->cash_ac_no);
    hv_get_tag(p_hvpack,"CR2",hv_chkagemr->cash_name);
    zip_space(hv_chkagemr->cash_name);
    return 0;  
}
/* 电子联行专用汇兑报文 */
int vAnalyseLhhdTag(THVPACKAGE * p_hvpack,struct hv_chkagemr_c* hv_chkagemr)
{  
    memcpy(hv_chkagemr->cmtno,"109",3);
    hv_get_tag(p_hvpack,"CG1",hv_chkagemr->or_br_no);
    hv_get_tag(p_hvpack,"CG2",hv_chkagemr->pay_ac_no);
    hv_get_tag(p_hvpack,"CG3",hv_chkagemr->payname);
    hv_get_tag(p_hvpack,"CG4",hv_chkagemr->acbrno);
    hv_get_tag(p_hvpack,"CG5",hv_chkagemr->cash_ac_no);
    hv_get_tag(p_hvpack,"CG6",hv_chkagemr->cash_name);
    hv_get_tag(p_hvpack,"CG7",hv_chkagemr->reason);
    return 0;  
}
/* 定期借记支付报文 */
int vAnalyseDqjjTag(THVPACKAGE * p_hvpack,struct hv_chkagemr_c* hv_chkagemr)
{  
    memcpy(hv_chkagemr->cmtno,"114",3);
    hv_get_tag(p_hvpack,"OBF",hv_chkagemr->yw_type);
    return 0;  
}
/* 通用借记支付报文 */
int vAnalyseTyjjTag(THVPACKAGE * p_hvpack,struct hv_chkagemr_c* hv_chkagemr)
{  
    memcpy(hv_chkagemr->cmtno,"119",3);
    hv_get_tag(p_hvpack,"59B",hv_chkagemr->payaddress);
    zip_space(hv_chkagemr->payaddress);
    hv_get_tag(p_hvpack,"50B",hv_chkagemr->cash_addr);
    zip_space(hv_chkagemr->cash_addr);
    hv_get_tag(p_hvpack,"20A",hv_chkagemr->voctype);
    hv_get_tag(p_hvpack,"0BF",hv_chkagemr->vocnum);
    return 0;  
}
/* 申请撤销报文 */
int vAnalysePutTag(THVPACKAGE * p_hvpack,struct hv_chkagemr_c* hv_chkagemr)
{  
    char cTxdate[9];
    char cOtxdate[9];      
    memset(cTxdate,0,sizeof(cTxdate));
    memset(cOtxdate,0,sizeof(cOtxdate));
        
    hv_get_tag(p_hvpack,"30A",cTxdate);
    hv_chkagemr->tx_date = apatoi(cTxdate,8);
    hv_get_tag(p_hvpack,"051",hv_chkagemr->o_tx_date);
    memcpy(hv_chkagemr->cmtno,"311",3);
    hv_get_tag(p_hvpack,"C47",hv_chkagemr->orderno);

    hv_get_tag(p_hvpack,"CC1",hv_chkagemr->or_br_no);
    hv_get_tag(p_hvpack,"CE2",hv_chkagemr->o_tx_type);
    hv_get_tag(p_hvpack,"046",hv_chkagemr->o_orderno_hv);
    hv_get_tag(p_hvpack,"72A",hv_chkagemr->reason);
    zip_space(hv_chkagemr->reason);
    return 0;  
}
/* 撤销应答报文 */
int vAnalyseRespTag(THVPACKAGE * p_hvpack,struct hv_chkagemr_c* hv_chkagemr)
{  
    char cTxdate[9];
    memset(cTxdate,0,sizeof(cTxdate));
    
    hv_get_tag(p_hvpack,"30A",cTxdate);
    hv_chkagemr->tx_date = apatoi(cTxdate,8);
    memcpy(hv_chkagemr->cmtno,"312",3);
    hv_get_tag(p_hvpack,"CCD",hv_chkagemr->or_br_no);
    hv_get_tag(p_hvpack,"CP2",hv_chkagemr->o_orderno_hv);
    hv_get_tag(p_hvpack,"046",hv_chkagemr->p_orderno_hv);
    hv_get_tag(p_hvpack,"04C",hv_chkagemr->repson);
    hv_get_tag(p_hvpack,"72A",hv_chkagemr->reason);
    return 0;  
}
/* 申请退回报文 */
int vAnalyseUntreadTag(THVPACKAGE * p_hvpack,struct hv_chkagemr_c* hv_chkagemr)
{  
    char cTxt[19];
    char cTxamt[16],*endprt;
    memset(cTxt, 0 , sizeof(cTxt));
    memset(cTxamt, 0 , sizeof(cTxamt));
    
    memcpy(hv_chkagemr->cmtno,"313",3);
    hv_get_tag(p_hvpack,"C47",hv_chkagemr->orderno);
    hv_get_tag(p_hvpack,"CE2",hv_chkagemr->o_tx_type);
    hv_get_tag(p_hvpack,"005",hv_chkagemr->o_orderno_hv);
    hv_get_tag(p_hvpack,"CND",cTxt);
    memcpy(hv_chkagemr->cur_no,cTxt,3);
    memcpy(cTxamt,cTxt+3,sizeof(cTxamt)-1);
    hv_chkagemr->tx_amt=strtod(cTxamt,&endprt);
    return 0;  
}
/* 业务退回应答报文 */
int vAnalyseTreadTag(THVPACKAGE * p_hvpack,struct hv_chkagemr_c* hv_chkagemr)
{  
    char cTxamt[16],cTxt[19],*endprt;
    char cOtxdate[9];    
    memset(cOtxdate,0,sizeof(cOtxdate));  
      
    memset(cTxt, 0 , sizeof(cTxt));
    memset(cTxamt, 0 , sizeof(cTxamt));
    memcpy(hv_chkagemr->cmtno,"314",3);
    hv_get_tag(p_hvpack,"CCE",hv_chkagemr->or_br_no);
    hv_get_tag(p_hvpack,"051",hv_chkagemr->o_tx_date);

    /*hv_get_tag(p_hvpack,"CJB",hv_chkagemr->otxday);   
    hv_get_tag(p_hvpack,"051",hv_chkagemr->otxday);  
    ??? */

    hv_get_tag(p_hvpack,"CP2",hv_chkagemr->o_orderno_hv);
    hv_get_tag(p_hvpack,"CE2",hv_chkagemr->o_tx_type);
    hv_get_tag(p_hvpack,"005",hv_chkagemr->p_orderno_hv);
    hv_get_tag(p_hvpack,"CND",cTxt);
    memcpy(hv_chkagemr->cur_no,cTxt,3);
    memcpy(cTxamt,cTxt+3,sizeof(cTxamt)-1);
    hv_chkagemr->tx_amt=strtod(cTxamt,&endprt);
    hv_get_tag(p_hvpack,"018",hv_chkagemr->repson);
    return 0;  
}
/*即时转账借贷通知报文*/
int vAnalyseOnlineRecTag(THVPACKAGE * p_hvpack,struct hv_chkagemr_c* hv_chkagemr)
{  
    char cTxamt[16],cTxt[19],*endprt;
    char cOtxdate[9];    
    memset(cOtxdate,0,sizeof(cOtxdate));    
    memset(cTxt, 0 , sizeof(cTxt));
    memset(cTxamt, 0 , sizeof(cTxamt));    
    memcpy(hv_chkagemr->cmtno,"232",3);
    hv_get_tag(p_hvpack,"CCE",hv_chkagemr->or_br_no);
    hv_get_tag(p_hvpack,"051",hv_chkagemr->o_tx_date);

    /*  ??? 
    hv_get_tag(p_hvpack,"CJB",hv_chkagemr->otxday);
    hv_get_tag(p_hvpack,"051",hv_chkagemr->otxday);
    */

    hv_get_tag(p_hvpack,"CP2",hv_chkagemr->o_orderno_hv);
    hv_get_tag(p_hvpack,"CE2",hv_chkagemr->o_tx_type);
    hv_get_tag(p_hvpack,"005",hv_chkagemr->p_orderno_hv);
    hv_get_tag(p_hvpack,"CND",cTxt);
    memcpy(hv_chkagemr->cur_no,cTxt,3);
    memcpy(cTxamt,cTxt+3,sizeof(cTxamt)-1);
    hv_chkagemr->tx_amt = strtod(cTxamt,&endprt);
    hv_get_tag(p_hvpack,"018",hv_chkagemr->repson);
    return 0;  
}
/* 银行汇票资金移存报文 cmt121 */
int vAnalyseHpzjycTag(THVPACKAGE * p_hvpack,struct hv_chkagemr_c* hv_chkagemr)
{  
    char cTxamt[16],cPdate[9],*endprt;
    memset(cPdate, 0 , sizeof(cPdate));
    memset(cTxamt, 0 , sizeof(cTxamt));  
    memcpy(hv_chkagemr->cmtno,"121",3);
    hv_get_tag(p_hvpack,"C33",hv_chkagemr->cashno);

    hv_get_tag(p_hvpack,"30B",cPdate);
    hv_chkagemr->p_date = apatoi(cPdate,8);

    hv_get_tag(p_hvpack,"21A",hv_chkagemr->p_num);
    hv_get_tag(p_hvpack,"D02",hv_chkagemr->hp_type);
    hv_get_tag(p_hvpack,"C10",hv_chkagemr->pass_wd);
    hv_get_tag(p_hvpack,"50B",hv_chkagemr->payaddress);
    zip_space(hv_chkagemr->payaddress);
    return 0;  
}
/* 清算银行汇票资金报文 */
int vAnalyseHpzfTag(THVPACKAGE * p_hvpack,struct hv_chkagemr_c* hv_chkagemr)
{  
    char cTxamt[16],*endprt;
    memset(cTxamt, 0 , sizeof(cTxamt));
    memcpy(hv_chkagemr->cmtno,"122",3);
    hv_get_tag(p_hvpack,"59E",hv_chkagemr->cash_ac_no);
    zip_space(hv_chkagemr->cash_ac_no);
    hv_get_tag(p_hvpack,"59D",hv_chkagemr->cash_name);
    zip_space(hv_chkagemr->cash_name);
    hv_get_tag(p_hvpack,"50B",hv_chkagemr->payaddress);
    zip_space(hv_chkagemr->payaddress);
    return 0;  
}
/* 银行汇票多余资金划回报文 */
int vAnalyseHpHhTag(THVPACKAGE * p_hvpack,struct hv_chkagemr_c* hv_chkagemr)
{  
    char cTxamt[16],cPdate[9], cPaydate[9], *endprt;
    memset(cTxamt, 0 , sizeof(cTxamt));
    memset(cPdate, 0 , sizeof(cPdate));
    memset(cPaydate, 0 , sizeof(cPaydate));  
    memcpy(hv_chkagemr->cmtno,"123",3);
    hv_get_tag(p_hvpack,"50B",hv_chkagemr->payaddress);
    zip_space(hv_chkagemr->payaddress);
    hv_get_tag(p_hvpack,"30B",cPdate);
    hv_chkagemr->p_date = apatoi(cPdate,8);

    hv_get_tag(p_hvpack,"21A",hv_chkagemr->p_num);
    hv_get_tag(p_hvpack,"C10",hv_chkagemr->pass_wd);
    hv_get_tag(p_hvpack,"33C",cTxamt);
    hv_chkagemr->jf_amt=strtod(cTxamt,&endprt)/100.0;    /* 出票金额   tx_amt中为多余资金 */
    memset(cTxamt, 0 ,sizeof(cTxamt));
    hv_get_tag(p_hvpack,"CNV",cTxamt);
    hv_chkagemr->other_amt=strtod(cTxamt,&endprt)/100.0; /* 实际结算金额 */
    
/*  这样会冲掉前后所得的付款人帐号和户名
    hv_get_tag(p_hvpack,"59E",hv_chkagemr->pay_ac_no);
    hv_get_tag(p_hvpack,"59D",hv_chkagemr->payname);
    应该是最后持票人帐号和户名    Modified by ChenMing  2006-11-22 16:49 */
    
    hv_get_tag(p_hvpack,"59E",hv_chkagemr->cash_ac_no);
    hv_get_tag(p_hvpack,"59D",hv_chkagemr->cash_name);
    
    hv_get_tag(p_hvpack,"30C",cPaydate);
    hv_chkagemr->pay_date = apatoi(cPaydate,8);
    zip_space(hv_chkagemr->payaddress);
    return 0;  
}
/* 银行汇票全额兑付通知报文 */
int vAnalyseHpdftzTag(THVPACKAGE * p_hvpack,struct hv_chkagemr_c* hv_chkagemr)
{  
    char cTxamt[16],cPdate[9],cPaydate[9],*endprt;
    memset(cTxamt, 0 , sizeof(cTxamt));
    memset(cPdate, 0 , sizeof(cPdate));
    memset(cPaydate, 0 , sizeof(cPaydate));  
    memcpy(hv_chkagemr->cmtno,"725",3);
    hv_get_tag(p_hvpack,"50B",hv_chkagemr->payaddress);
    zip_space(hv_chkagemr->payaddress);  
    hv_get_tag(p_hvpack,"30B",cPdate);
    hv_chkagemr->p_date = apatoi(cPdate,8);
    hv_get_tag(p_hvpack,"21A",hv_chkagemr->p_num);
    hv_get_tag(p_hvpack,"C10",hv_chkagemr->pass_wd);
    hv_get_tag(p_hvpack,"33C",cTxamt);
    hv_chkagemr->jf_amt=strtod(cTxamt,&endprt);
    memset(cTxamt, 0 ,sizeof(cTxamt));
    hv_get_tag(p_hvpack,"CNV",cTxamt);
    hv_chkagemr->other_amt=strtod(cTxamt,&endprt);
    hv_get_tag(p_hvpack,"59E",hv_chkagemr->pay_ac_no);
    hv_get_tag(p_hvpack,"59D",hv_chkagemr->payname);
    hv_get_tag(p_hvpack,"30C",cPaydate);
    hv_chkagemr->pay_date = apatoi(cPaydate,8);
    zip_space(hv_chkagemr->payaddress);
    return 0;  
}
/* 银行汇票未用退回资金报文 */
int vAnalyseHpwyThTag(THVPACKAGE * p_hvpack,struct hv_chkagemr_c* hv_chkagemr)
{  
    char cPdate[9];
    memset(cPdate, 0 ,sizeof(cPdate));  
    memcpy(hv_chkagemr->cmtno,"124",3);
    /*hv_get_tag(p_hvpack,"C33",hv_chkagemr->acbrno);  Remed by ChenMing 2006-11-13 19:00*/
    hv_get_tag(p_hvpack,"30B",cPdate);
    hv_chkagemr->p_date = apatoi(cPdate,8);
    hv_get_tag(p_hvpack,"21A",hv_chkagemr->p_num);
    hv_get_tag(p_hvpack,"C10",hv_chkagemr->pass_wd);
    hv_get_tag(p_hvpack,"50B",hv_chkagemr->payaddress);
    zip_space(hv_chkagemr->payaddress);
    return 0;  
}
/* 质押融资通知报文 */
int vAnalyseZyrzTag(THVPACKAGE * p_hvpack,struct hv_chkagemr_c* hv_chkagemr)
{  
    char cTxamt[16],*endprt;
    memset(cTxamt, 0 , sizeof(cTxamt));
    memcpy(hv_chkagemr->cmtno,"407",3);
    hv_get_tag(p_hvpack,"CC7",hv_chkagemr->pay_no);
    /* hv_get_tag(p_hvpack,"CC8",hv_chkagemr->acbrno);  Remed by ChenMing 2006-11-13 19:11*/
    hv_get_tag(p_hvpack,"90C",hv_chkagemr->dc);
    hv_get_tag(p_hvpack,"CCF",hv_chkagemr->sendco);
    hv_get_tag(p_hvpack,"CCG",hv_chkagemr->receco);
    return 0;  
}
/* 质押融资扣款通知报文 */
int vAnalyseZyrzkkTag(THVPACKAGE * p_hvpack,struct hv_chkagemr_c* hv_chkagemr)
{  
    char cTxamt[16],*endprt;
    memset(cTxamt, 0 , sizeof(cTxamt));
    memcpy(hv_chkagemr->cmtno,"408",3);
    hv_get_tag(p_hvpack,"CC7",hv_chkagemr->pay_no);
    /* hv_get_tag(p_hvpack,"CC8",hv_chkagemr->acbrno);  Remed by ChenMing 2006-11-13 19:11 */
    hv_get_tag(p_hvpack,"CNS",cTxamt);
    hv_chkagemr->other_amt=strtod(cTxamt,&endprt);
    memset(cTxamt, 0 ,sizeof(cTxamt));
    hv_get_tag(p_hvpack,"CNT",cTxamt);
    hv_chkagemr->intst=strtod(cTxamt,&endprt);
    hv_get_tag(p_hvpack,"90C",hv_chkagemr->dc); /*??? 借贷标志 ?? */
    hv_get_tag(p_hvpack,"CCF",hv_chkagemr->sendco);
    hv_get_tag(p_hvpack,"CCG",hv_chkagemr->receco);
    return 0;  
}
/* 托收承付（划回）支付报文 */
int vAnalyseTscfTag(THVPACKAGE * p_hvpack,struct hv_chkagemr_c* hv_chkagemr)
{
    char cJFtxamt[16] ,*endptr;
    char cPdate[9];
    memset(cPdate, 0 ,sizeof(cPdate));    
    memset(cJFtxamt, 0 ,sizeof(cJFtxamt));
    memcpy(hv_chkagemr->cmtno,"102",3);
    hv_get_tag(p_hvpack,"30B",cPdate);
    hv_chkagemr->p_date = apatoi(cPdate,8);
    hv_get_tag(p_hvpack,"21A",hv_chkagemr->vocnum);
    hv_get_tag(p_hvpack,"33A",cJFtxamt);
    hv_chkagemr->jf_amt = strtod(cJFtxamt, &endptr); /*拒付金额 */
    memset(cJFtxamt, 0 , sizeof(cJFtxamt));
    hv_get_tag(p_hvpack,"33B",cJFtxamt);
    hv_chkagemr->other_amt = strtod(cJFtxamt, &endptr); /*拒付金额 */
    return 0;  
}
/* 委托收款（划回）支付报文  */
int vAnalyseWtTag(THVPACKAGE * p_hvpack, struct hv_chkagemr_c* hv_chkagemr)
{
    char cPdate[9];
    memset(cPdate, 0 ,sizeof(cPdate));  
    memcpy(hv_chkagemr->cmtno,"101",3);
    hv_get_tag(p_hvpack,"21A",hv_chkagemr->vocnum);/* 暂时用 21A */
    hv_get_tag(p_hvpack,"30B",cPdate);
    hv_chkagemr->p_date = apatoi(cPdate,8);
    hv_get_tag(p_hvpack,"21A",hv_chkagemr->p_num);
    hv_get_tag(p_hvpack,"CEJ",hv_chkagemr->voctype);
    return 0;
}
/* begin add by LiuHuafeng 2007-1-11 23:15 */
int iTest8583_1(char * file,int line,char * bit8)
{
    char cBit8[9];
    memset(cBit8 , 0 , sizeof(cBit8));
    memcpy(cBit8 , bit8 , sizeof(cBit8)-1);
    printf("%s,%d \n",__FILE__,__LINE__);
    set_zd_data("1280",cBit8);
    get_fd_data("1280",cBit8);
    if(memcmp(cBit8,bit8,8)){
      lv_log_com(cLogfile_rece,"%s,%d 检测8583出现错误，设置内容为[%.8s],得到内容=[%.8s]\n",file,line,bit8,cBit8);
      return -1;
    }
    else{
      lv_log_com(cLogfile_rece,"%s,%d 检测8583成功，设置内容为[%.8s],得到内容=[%.8s]\n",file,line,bit8,cBit8);
      return 0;
    }
    return 0;
}
int iReset8583_1(char * file,int line,char * bit8)
{
    int i;
    long iLength;
    int iRet=0;
    struct  data_dic_c  data_dic;
    lv_log_com(cLogfile_rece,"%s,%d pu_val地址=[%ld],pub_buf地址=[%ld],现在不在恢复iGlobal_Pu_Val_Addr=[%ld]",__FILE__,__LINE__,Pu_Val,Pub_Buf,iGlobal_Pu_Val_Addr);
	return 0; /* add by LiuHuafeng 20070119 for find debug */
    memset(&data_dic,0x00,sizeof(data_dic));
    lv_log_com(cLogfile_rece,"%s,%d pu_val地址=[%ld],pub_buf地址=[%ld],该组变量地址将变更",__FILE__,__LINE__,Pu_Val,Pub_Buf);
    lv_log_com(cLogfile_rece,"%s,%d Pu_Val_bak_rece地址=[%ld],Pub_Buf_bak_rece地址=[%ld]",__FILE__,__LINE__,Pu_Val_bak_rece,Pub_Buf_bak_rece);
    free(Pu_Val);
    free(Pub_Buf);
    Pu_Val=(struct PVal_St *) malloc (sizeof(struct PVal_St) *PuVal_num);
    Pub_Buf=(unsigned char *)malloc (sizeof(unsigned char)*Pub_leng+2); 
    lv_log_com(cLogfile_rece,"%s,%d pu_val新地址=[%ld],pub_buf地址=[%ld],该组变量地址将变更",__FILE__,__LINE__,Pu_Val,Pub_Buf);
    lv_log_com(cLogfile_rece,"%s,%d Pu_Val_bak_rece地址=[%ld],Pub_Buf_bak_rece地址=[%ld]",__FILE__,__LINE__,Pu_Val_bak_rece,Pub_Buf_bak_rece);

    memset(Pu_Val,0x00,sizeof(struct PVal_St) *PuVal_num);    
    memset(Pub_Buf,0x20,sizeof(unsigned char)*Pub_leng+2);
    lv_log_com(cLogfile_rece,"%s,%d 开始恢复8583数据内容,pub_leng=[%ld],pubval_num=[%d]\n",file,line,Pub_leng,PuVal_num);
    memcpy(Pub_Buf,Pub_Buf_bak_rece,sizeof(unsigned char)*Pub_leng+2);
    for (i=0,iLength=0;i<PuVal_num;i++) 
    {
        strncpy(Pu_Val[i].sjbm,Pu_Val_bak_rece[i].sjbm,sizeof(Pu_Val_bak_rece[i].sjbm)-1);
        Pu_Val[i].length=Pu_Val_bak_rece[i].length;
        Pu_Val[i].prec=Pu_Val_bak_rece[i].prec;
        Pu_Val[i].lx=Pu_Val_bak_rece[i].lx;
        Pu_Val[i].val=Pub_Buf+iLength;
        iLength+=Pu_Val_bak_rece[i].length;
        lv_log_com(cLogfile_rece,"i=[%d],sjbm=[%s],length=[%d]",i,Pu_Val[i].sjbm,Pu_Val[i].length);
    }
    lv_log_com(cLogfile_rece,"备份的8583内容=[%s]\n",Pub_Buf_bak_rece);
    memcpy(Pub_Buf,Pub_Buf_bak_rece,sizeof(unsigned char)*Pub_leng+2);

    lv_log_com(cLogfile_rece,"恢复的8583内容=[%s]\n",Pub_Buf);
    lv_log_com(cLogfile_rece,"[%s][%d]恢复完成，再次检测,检测参数为[%.8s]\n",file,line,bit8);
    iRet=iTest8583_1(__FILE__,__LINE__,bit8);
    if(iRet)
    {
      int recdnum=0;
      recdnum=0;
      lv_log_com(cLogfile_rece,"[%s][%d]==========================================================\n",file,line);
      lv_log_com(cLogfile_rece,"[%s][%d]恢复8583未能生效，再次启动恢复机制，严重错误,严重错误\n",file,line);
      lv_log_com(cLogfile_rece,"%s,%d pu_val地址=[%ld],pub_buf地址=[%ld],该组变量地址将变更",__FILE__,__LINE__,Pu_Val,Pub_Buf);
      lv_log_com(cLogfile_rece,"%s,%d Pu_Val_bak_rece地址=[%ld],Pub_Buf_bak_rece地址=[%ld]",__FILE__,__LINE__,Pu_Val_bak_rece,Pub_Buf_bak_rece);
      lv_log_com(cLogfile_rece,"[%s][%d]=================从数据表中得到相关数据===================\n",file,line);
      recdnum = sql_count ( "data_dic" , "%s" , "1=1" );
      if (recdnum<=0) 
      {
          vtcp_log("数据字典无内容!!ERROR %d",recdnum);
          return -1;
      }
      iRet = sql_sum_long( "data_dic","data_leng",&iLength,"%s","1=1" );
	    if ( iRet )
	    {
            vtcp_log("数据字典sum!!ERROR");
            return -1;
	    }
      Pub_leng=iLength;
      Pu_Val=(struct PVal_St *) malloc (sizeof(struct PVal_St) *recdnum);
      Pub_Buf=(unsigned char *)malloc (sizeof(unsigned char)*Pub_leng+2); 
      memset (Pub_Buf,0x20,Pub_leng+2);
      memset(Pu_Val,0x00,sizeof(struct PVal_St) *recdnum);    
      memcpy(Pub_Buf,Pub_Buf_bak_rece,sizeof(unsigned char)*Pub_leng+2);
      iRet = Data_dic_Dec_Sel( RtCode , "%s" , "1=1 order by data_code" );
      if ( iRet )
      {
          vtcp_log("取数据字典错误!!error code =%d.\n", RespCode );
          return iRet ;
      }
      RespCode=0;
      for (i=0,iLength=0;i<recdnum;i++) 
      {
        memset(&data_dic,0x00,sizeof(data_dic));
        RespCode = Data_dic_Fet_Sel( &data_dic, RtCode );
        if ( RespCode )
        {
            lv_log_com(cLogfile_rece,"i=[%d],respcode=[%d]",i,RespCode);
            break;
        }
        strcpy(Pu_Val[i].sjbm,data_dic.data_code);
        Pu_Val[i].length=data_dic.data_leng;
        Pu_Val[i].prec=data_dic.data_dec;
        Pu_Val[i].lx=data_dic.data_type[0]-'0';
        Pu_Val[i].val=Pub_Buf+iLength;
        /***XXXX***/
        iLength+=data_dic.data_leng;
        lv_log_com(cLogfile_rece,"from db i=[%d],sjbm=[%s],db_sjbm=[%.5s],length=[%d]",i,Pu_Val[i].sjbm,data_dic.data_code,Pu_Val[i].length);
      }
      Data_dic_Clo_Sel();
      lv_log_com(cLogfile_rece,"[%s][%d]根据表中数据重新初始化8583结构体结束，下边初始化相关8583域中内容\n",file,line);
      pub_base_sysinit();
      lv_log_com(cLogfile_rece,"[%s][%d]*********二次恢复8583域完成恢复内容如下:\n",file,line);
      for (i=0,iLength=0;i<PuVal_num;i++) 
      {
          lv_log_com(cLogfile_rece,"i=[%d],sjbm=[%s],length=[%d]\n",i,Pu_Val[i].sjbm,Pu_Val[i].length);
      }
      lv_log_com(cLogfile_rece,"[%s]\n",Pub_Buf);
      lv_log_com(cLogfile_rece,"[%s][%d]*********进行地三次检验，如果本次检验失败，就没招了\n",file,line);
      iRet=iTest8583_1(__FILE__,__LINE__,bit8);
      if(iRet)
      {
        lv_log_com(cLogfile_rece,"[%s][%d]第三次检验，没招了\n",file,line);
        
      }
    }
    return 0;
}
/* end by LiuHuafeng 2007-1-11 23:15 */
