/*************************************************
* 文 件 名:  spJ009.c
* 功能描述： 大额支付类交易往账发送
* 作    者:  xyy 
* 完成日期： 2006年8月8日
*
* 修改记录： 
* 日    期:
* 修 改 人:  SSH
* 修改内容:  简化处理
* 修 改 人：LiuHuafeng 2006-12-19 13:42
* 内    容：1，将发送函数写入本程序，
*           2，重新组织备份8583代码，
*           3，重新组织错误检测，
*           4，重新组织错误恢复，
*************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "hv_pub.h"
#include "hvnet.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "svrpub_c.h"
#include <fcntl.h>
#include   "data_dic_c.h"
#include <stdarg.h>
#include "find_debug.h"

#define loc_t sys_loc_t
#define LV_SEND_LOG "/home/newsys/usr/lhf/lv_sendlog"

static char cLogfile[512];
struct PVal_St *Pu_Val_bak;
char *Pub_Buf_bak;
char cBuff [5120];/*站位变量 */

extern int hv_sendpack(THVPACKAGE *pHv);
extern int iChkSendRecord(char *cmtno,char* orderno,char *orbrno ,char * txday,char *errmsg);
extern int iInsertSendRecord(char* cmtno,char * txday,char *orderno,char *brno,char *wssrno,char *resp1);

int lv_fprintf(char *filename, char *buf)
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
void lv_log(char *filename, char *fmt,...)
{
    va_list         argptr;
    int             cnt;
    char            buf[1024 * 1024];

    memset(buf, 0x0, sizeof(buf));
    va_start(argptr, fmt);

    cnt = vsprintf(buf, fmt, argptr);
    va_end(argptr);
    strcat(buf, "\n");

    lv_fprintf(filename, buf);
}
int iTest8583(char * file,int line,char * bit8)
{
    char cBit8[9];
    memset(cBit8 , 0 , sizeof(cBit8));
    MEMCPY_DEBUG(cBit8 , bit8 , sizeof(cBit8)-1);
    printf("%s,%d \n",__FILE__,__LINE__);
    set_zd_data("1280",cBit8);
    get_fd_data("1280",cBit8);
    if(memcmp(cBit8,bit8,8)){
      lv_log(cLogfile,"%s,%d 检测8583出现错误，设置内容为[%.8s],得到内容=[%.8s]\n",file,line,bit8,cBit8);
      return -1;
    }
    else{
      lv_log(cLogfile,"%s,%d 检测8583成功，设置内容为[%.8s],得到内容=[%.8s]\n",file,line,bit8,cBit8);
      return 0;
    }
    return 0;
}

int iReset8583(char * file,int line,char * bit8,char * fd123)
{
    int i;
    long iLength;
    int iRet=0;
    struct  data_dic_c  data_dic;
    memset(&data_dic,0x00,sizeof(data_dic));
    lv_log(cLogfile,"%s,%d pu_val地址=[%ld],pub_buf地址=[%ld],该组变量地址将变更",__FILE__,__LINE__,Pu_Val,Pub_Buf);
    lv_log(cLogfile,"%s,%d pu_val_bak地址=[%ld],pub_buf_bak地址=[%ld]",__FILE__,__LINE__,Pu_Val_bak,Pub_Buf_bak);
    free(Pu_Val);
    free(Pub_Buf);
    Pu_Val=(struct PVal_St *) malloc (sizeof(struct PVal_St) *PuVal_num);
    Pub_Buf=(unsigned char *)malloc (sizeof(unsigned char)*Pub_leng+2); 
    memset(Pu_Val,0x00,sizeof(struct PVal_St) *PuVal_num);    
    memset(Pub_Buf,0x20,sizeof(unsigned char)*Pub_leng+2);
    lv_log(cLogfile,"%s,%d 开始恢复8583数据内容,pub_leng=[%ld],pubval_num=[%d]\n",file,line,Pub_leng,PuVal_num);
    MEMCPY_DEBUG(Pub_Buf,Pub_Buf_bak,sizeof(unsigned char)*Pub_leng+2);
    for (i=0,iLength=0;i<PuVal_num;i++) 
    {
        strncpy(Pu_Val[i].sjbm,Pu_Val_bak[i].sjbm,sizeof(Pu_Val_bak[i].sjbm)-1);
        Pu_Val[i].length=Pu_Val_bak[i].length;
        Pu_Val[i].prec=Pu_Val_bak[i].prec;
        Pu_Val[i].lx=Pu_Val_bak[i].lx;
        Pu_Val[i].val=Pub_Buf+iLength;
        iLength+=Pu_Val_bak[i].length;
        lv_log(cLogfile,"i=[%d],sjbm=[%s],length=[%d]",i,Pu_Val[i].sjbm,Pu_Val[i].length);
    }
    lv_log(cLogfile,"备份的8583内容=[%s]\n",Pub_Buf_bak);
		/****
    MEMCPY_DEBUG(Pub_Buf,Pub_Buf_bak,sizeof(unsigned char)*Pub_leng+2);
    *****/
    lv_log(cLogfile,"恢复的8583内容=[%s]\n",Pub_Buf);
    lv_log(cLogfile,"[%s][%d]恢复完成，再次检测,检测参数为[%.8s]\n",file,line,bit8);
    iRet=iTest8583(__FILE__,__LINE__,bit8);
    if(iRet)
    {
      int recdnum=0;
      recdnum=0;
      lv_log(cLogfile,"[%s][%d]==========================================================\n",file,line);
      lv_log(cLogfile,"[%s][%d]恢复8583未能生效，再次启动恢复机制，严重错误,严重错误\n",file,line);
      lv_log(cLogfile,"%s,%d pu_val地址=[%ld],pub_buf地址=[%ld],该组变量地址将变更",__FILE__,__LINE__,Pu_Val,Pub_Buf);
      lv_log(cLogfile,"%s,%d pu_val_bak地址=[%ld],pub_buf_bak地址=[%ld]",__FILE__,__LINE__,Pu_Val_bak,Pub_Buf_bak);
      lv_log(cLogfile,"[%s][%d]=================从数据表中得到相关数据===================\n",file,line);
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
      MEMCPY_DEBUG(Pub_Buf,Pub_Buf_bak,sizeof(unsigned char)*Pub_leng+2);
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
            lv_log(cLogfile,"i=[%d],respcode=[%d]",i,RespCode);
            break;
        }
        strcpy(Pu_Val[i].sjbm,data_dic.data_code);
        Pu_Val[i].length=data_dic.data_leng;
        Pu_Val[i].prec=data_dic.data_dec;
        Pu_Val[i].lx=data_dic.data_type[0]-'0';
        Pu_Val[i].val=Pub_Buf+iLength;
        /***XXXX***/
        iLength+=data_dic.data_leng;
				lv_log(cLogfile,"from db i=[%d],sjbm=[%s],length=[%d]",i,Pu_Val[i].sjbm,Pu_Val[i].length);
      }
      Data_dic_Clo_Sel();
      lv_log(cLogfile,"[%s][%d]根据表中数据重新初始化8583结构体结束，下边初始化相关8583域中内容\n",file,line);
      pub_base_sysinit();
      set_zd_data("1230",fd123);
      lv_log(cLogfile,"[%s][%d]*********二次恢复8583域完成恢复内容如下:\n",file,line);
      for (i=0,iLength=0;i<PuVal_num;i++) 
      {
          lv_log(cLogfile,"i=[%d],sjbm=[%s],length=[%d]\n",i,Pu_Val[i].sjbm,Pu_Val[i].length);
      }
      lv_log(cLogfile,"[%s]\n",Pub_Buf);
      lv_log(cLogfile,"[%s][%d]*********进行地三次检验，如果本次检验失败，就没招了\n",file,line);
      iRet=iTest8583(__FILE__,__LINE__,bit8);
      if(iRet)
      {
        lv_log(cLogfile,"[%s][%d]第三次检验，没招了\n",file,line);
        
      }
    }
    return 0;
}

int spJ009()
{    
    THVPACKAGE respack;
    int  iCmtno = 0;
    int  ret    = 0;
    int  iSocketID ;

    char tmpBuf        [9];
    char cCmtno        [4];

    char cRefno        [21];
    char cSendtel      [5];
    char cSendTraceno  [7];
    char cQsbrno       [BRNO_LEN+1];
    char cTxday        [9];
    char cOrbrno       [13];
    char cWssrno       [7];
    char cResp1        [9];
    char cOrderno      [9];
    /***********add by liuyue   定义一个变量用来存储8583域的的值部分***/   
    int     recdnum=0;
    long    length=0;
    
    char  c_013_buf[50];        /**存放13域*****/
    char    _cBr_no[BRNO_LEN+1];
    HV_PAYIN_AREA   hv_payin;
    char cBuff1[5120];  /*站位变量 */
    HV_FD123_AREA   fd123;
    char cBuff2[5120];/*站位变量 */
    THVPACKAGE pack_zf;
    char cBuff3[5120];/*站位变量 */
    recdnum=PuVal_num;/*直接用全局变量 */
    length=Pub_leng;/* 直接用全局变量处理 */
    memset(cLogfile, 0 , sizeof(cLogfile));
    memset(c_013_buf,0x20,sizeof(c_013_buf));/**初始化为空格***/
    memset(_cBr_no,0x00,sizeof(_cBr_no));
    memset(cTxday, 0 , sizeof(cTxday));
    memset(tmpBuf    ,0,sizeof(tmpBuf));
    memset(cSendtel ,0,sizeof(cSendtel));
    memset(cSendTraceno ,0,sizeof(cSendTraceno));
    memset(cCmtno        ,0,sizeof(cCmtno));

    memset(cResp1   ,0,sizeof(cResp1));
    memset(cBuff        ,0,sizeof(cBuff));
    memset(cRefno        ,0,sizeof(cRefno));
    memset(cOrbrno  ,0,sizeof(cOrbrno));
    memset(cOrderno ,0,sizeof(cOrderno));
    get_fd_data(DC_DATE,cTxday);
    sprintf(cLogfile,"%s%s.txt",LV_SEND_LOG,cTxday);

    vtcp_log("[%s][%d]获取到的8583域的值===[%ld]\n",__FILE__,__LINE__,length);
    /* 准备备份的8583数据变量 */
    {
        int i=0;
        long iLength=0;
        Pu_Val_bak =  (struct PVal_St *)malloc (sizeof(struct PVal_St) *PuVal_num);/**8583备份的共用变量指针***/
        Pub_Buf_bak =(unsigned char *)malloc (sizeof(unsigned char)*Pub_leng+2);/**8583备份的公用变量缓冲区头指针**/
        memset(Pu_Val_bak, 0 , sizeof(struct PVal_St) *PuVal_num);
        memset(Pub_Buf_bak,' ',sizeof(unsigned char)*Pub_leng+2);
        for (i=0,iLength=0;i<PuVal_num;i++) 
        {
            strncpy(Pu_Val_bak[i].sjbm,Pu_Val[i].sjbm,sizeof(Pu_Val_bak[i].sjbm));
            Pu_Val_bak[i].length=Pu_Val[i].length;
            Pu_Val_bak[i].prec=Pu_Val[i].prec;
            Pu_Val_bak[i].lx=Pu_Val[i].lx;
            Pu_Val_bak[i].val=Pub_Buf_bak+iLength;
            iLength+=Pu_Val_bak[i].length;
        }
        lv_log(cLogfile,"%s,%d [%ld]pack_zf地址=[%ld]",__FILE__,__LINE__,g_pub_tx.tx_time,&pack_zf);
        lv_log(cLogfile,"%s,%d pu_val地址=[%ld],pub_buf地址=[%ld]",__FILE__,__LINE__,Pu_Val,Pub_Buf);
        lv_log(cLogfile,"%s,%d pu_VAL_bak地址=[%ld],pub_BUF_bak地址=[%ld]",__FILE__,__LINE__,Pu_Val_bak,Pub_Buf_bak);
    }
    /************add end 20061207**********************/
    memset(&hv_payin        ,0,sizeof(hv_payin));
    memset(&fd123           ,0,sizeof(fd123));
    memset(cQsbrno          ,0,sizeof(cQsbrno));
    memset(&pack_zf        ,0x00    ,sizeof(pack_zf));
    memset(&respack,'\0',sizeof(respack));

    memset(cBuff1 ,0x00,sizeof(cBuff1));
    memset(cBuff2,0x00,sizeof(cBuff2));
    memset(cBuff3,0x00,sizeof(cBuff3));


    vtcp_log("spJ009[%s][%d]",__FILE__,__LINE__);

    /*** 初始化全局变量 ***/
    pub_base_sysinit();

    get_fd_data(DC_DATE,cTxday);
    get_fd_data(DC_TRACE_NO,cWssrno);
  
    /***得到大额支付业务8583数据***/
    g_reply_int = iHvTistoPayin(&hv_payin);
    if ( g_reply_int )
    {
        sprintf( acErrMsg,"调用iHvTistoNotpay错误![%s][%d]",__FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "P015" );
    	lv_log(cLogfile,"跟踪到一个交易调用iHvTistoNotpay错误Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
        WRITEMSG
        goto ErrExit;
    }
    /******检查营业的状态******/ 
  
    g_reply_int = iHv_checkstat(hv_payin.F_cmtno);
    if ( g_reply_int )
    {
        sprintf( acErrMsg,"工作状态不正常![%s][%d][%s]",__FILE__,__LINE__,g_reply_int);
        strcpy( g_pub_tx.reply, "HV01");
        set_zd_data(DC_GET_MSG,"大额支付系统当前状态无法做此交易");
    	lv_log(cLogfile,"跟踪到一个交易大额支付系统当前状态无法做此交易Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
        WRITEMSG
        goto ErrExit;
    }    
    vtcp_log("[%s][%d]调用完iHv_checkstat",__FILE__,__LINE__);
    
    g_reply_int = FetchRecToPayin(&hv_payin);    
    if ( g_reply_int )
    {
        sprintf( acErrMsg,"调用FetchRecToPayin错误![%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"调用FetchRecToPayin错误");
    	lv_log(cLogfile,"跟踪到一个交易调用FetchRecToPayin错误Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
        WRITEMSG
        goto ErrExit;
    }
    MEMCPY_DEBUG(cRefno,hv_payin.T_refno,sizeof(cRefno)-1);
    vtcp_log("[%s][%d]hv_payin->T_hv_brf======[%s]",__FILE__,__LINE__,hv_payin.T_hv_brf);
    MEMCPY_DEBUG(cCmtno,hv_payin.F_cmtno,sizeof(cCmtno)-1); 
    iCmtno=atoi(cCmtno);

    if (memcmp(cCmtno,"121",sizeof(cCmtno)-1)==0)
    {
        MEMCPY_DEBUG(cQsbrno,QS_BR_NO,sizeof(cQsbrno)-1);
        /***检查是否是清算中心****/
        if (memcmp(hv_payin.F_tx_br_no,cQsbrno,BRNO_LEN)==0) 
        {
            sprintf(acErrMsg,"清算中心不能做此交易!\n");
            strcpy( g_pub_tx.reply, "HV01");
            set_zd_data(DC_GET_MSG,"清算中心不能做此交易");
            lv_log(cLogfile,"跟踪到一个交易Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
            WRITEMSG
            goto ErrExit;
        }
    }
    g_reply_int = iHvFrontToTable(&hv_payin);
    if (g_reply_int)
    {
        sprintf( acErrMsg,"调用iHvFrontToTable错误![%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "HV01");
        WRITEMSG
        lv_log(cLogfile,"跟踪到一个交易调用iHvFrontToTable错误Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
        goto ErrExit;
    }
    vtcp_log("[%s][%d]hv_payin->T_hv_brf======[%s]",__FILE__,__LINE__,hv_payin.T_hv_brf);

    MEMCPY_DEBUG(hv_payin.T_refno,cRefno,sizeof(cRefno)-1);
    if ( hv_payin.T_hv_sts[0] != SEND_ACCT_WAIT)
    {
        sprintf(acErrMsg,"记录状态错误!\n");
        strcpy( g_pub_tx.reply, "P016");
        vtcp_log("[%s][%d]T_hv_sts=[%c][%c]\n",__FILE__,__LINE__,hv_payin.T_hv_sts[0] , SEND_ACCT_CHECK);
        set_zd_data(DC_GET_MSG,"记录状态错误");
        lv_log(cLogfile,"跟踪到一个交易记录状态错误Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
        WRITEMSG
        goto ErrExit;
    }
    hv_payin.T_hv_sts[0]=SEND_ACCT_WAIT;/*更新状态，是否需要UPDATE?*/
    vtcp_log("[%s][%d]hv_payin->T_yw_type=[%.8s]",__FILE__,__LINE__,hv_payin.T_yw_type);
    vtcp_log("[%s][%d]hv_payin->T_hv_brf======[%s]",__FILE__,__LINE__,hv_payin.T_hv_brf);
    
    g_reply_int = iHvUpdZfRecFromPayin(&hv_payin,0);
    if (g_reply_int)
    {
        sprintf(acErrMsg,"调用iHvUpdZfRecFromPayin函数出错!\n");
        strcpy( g_pub_tx.reply, "P018");
        set_zd_data(DC_GET_MSG,"调用iHvUpdZfRecFromPayin函数出错");
        lv_log(cLogfile,"跟踪到一个交易调用iHvUpdZfRecFromPayin函数出错Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d]hv_payin->T_yw_type=[%.8s]",__FILE__,__LINE__,hv_payin.T_yw_type);
    vtcp_log("[%s][%d]hv_payin->T_hv_brf======[%s]",__FILE__,__LINE__,hv_payin.T_hv_brf);
  
    switch(iCmtno) /*开始拼包,插入发送登记簿*/           
    {
        case 100:  /*汇兑交易 */
            pub_com_CMT100_T2TAG(&hv_payin,&pack_zf);
            break;
        case 101:  /*委托收款*/
            pub_com_CMT101_T2TAG(&hv_payin,&pack_zf);
            break;
        case 102:  /*托收承付*/ 
            pub_com_CMT102_T2TAG(&hv_payin,&pack_zf);
            break;
        case 103:  /*国库资金汇划*/
            vtcp_log("[%s][%d]hv_payin->T_yw_type=[%.8s]",__FILE__,__LINE__,hv_payin.T_yw_type);
            pub_com_CMT103_T2TAG(&hv_payin,&pack_zf);
            break;
        case 105:  /*银行间拆借*/
            pub_com_CMT105_T2TAG(&hv_payin,&pack_zf);
            break;
        case 108:  /*退汇*/
            pub_com_CMT108_T2TAG(&hv_payin,&pack_zf);
            break;
        case 121:  /*汇票发送*/
            vtcp_log("payin===[%s]\n",(char *)&hv_payin);
            pub_com_CMT121_T2TAG(&hv_payin,&pack_zf);
            vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
            break;        
        default:
            sprintf(acErrMsg,"没有该报文类型!\n");
            strcpy( g_pub_tx.reply, "HV01");
            set_zd_data(DC_GET_MSG,"没有该报文类型");
            lv_log(cLogfile,"跟踪到一个交易cmtno错误Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
            WRITEMSG
            goto ErrExit;
            break;
    }
  
    getPackageFlgNo(&pack_zf);/**得到FlgNo**/
    vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
    ret=hv_gen_pack(&pack_zf);
    vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
    if (ret<0)
    {
        sprintf(acErrMsg,"组织大额支付报文错误!\n");
        WRITEMSG
        strcpy( g_pub_tx.reply, "P021" );
        lv_log(cLogfile,"跟踪到一个交易组织报文错误Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
        set_zd_data(DC_GET_MSG,"组织大额支付报文错误");
        goto ErrExit;
    }
    vtcp_log("[%s][%d]hv_payin->T_yw_type=[%.8s]",__FILE__,__LINE__,hv_payin.T_yw_type);
    
    MEMCPY_DEBUG(cSendtel,hv_payin.T_send_tel,sizeof(cSendtel)-1);
    MEMCPY_DEBUG(cSendTraceno,hv_payin.T_send_tel_trace_no,sizeof(cSendTraceno)-1);
    MEMCPY_DEBUG(cOrderno,hv_payin.T_orderno,sizeof(cOrderno)-1);
    lv_log(cLogfile,"%s,%d 支付交易序号orderno=[%s],brno=[%s]",__FILE__,__LINE__,cOrderno,g_pub_tx.tx_br_no);
    MEMCPY_DEBUG(_cBr_no,hv_payin.F_tx_br_no,sizeof(hv_payin.F_tx_br_no));
    if (hv_insert_sendlist(&pack_zf,_cBr_no,cSendtel,cSendTraceno,cOrderno)<0)
    {
        sprintf(acErrMsg,"插入发送登记簿失败!\n");
        WRITEMSG
        strcpy( g_pub_tx.reply, "P010" );
        set_zd_data(DC_GET_MSG,"插入发送登记簿失败!");
        lv_log(cLogfile,"跟踪到一个交易写发送登记簿失败Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
        goto ErrExit;
    }
    vtcp_log("[%s][%d]将要调用pub_ins_trace_log程序!",__FILE__,__LINE__);
    /*发送交易要写流水*/
    if ( pub_ins_trace_log() )
    {
        sprintf(acErrMsg,"大额发送交易写流水错误!\n");
        strcpy( g_pub_tx.reply, "HV01");
        lv_log(cLogfile,"跟踪到一个交易写流水错误Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
        set_zd_data(DC_GET_MSG,"大额发送交易写流水错误");
        WRITEMSG
        goto ErrExit;
    }
  
    vtcp_log("[%s][%d]cWssrno=[%s]",__FILE__,__LINE__,cWssrno);
    /**发送到前置机**/
    memset(cWssrno, 0 , sizeof(cWssrno));
    MEMCPY_DEBUG(cWssrno,hv_payin.T_chk_trace_no,sizeof(cWssrno)-1);
    MEMCPY_DEBUG(cCmtno,hv_payin.T_cmtno,sizeof(cCmtno)-1);
    MEMCPY_DEBUG(cOrbrno,hv_payin.T_or_br_no,sizeof(cOrbrno)-1);
    
    g_reply_int = 0;
    /*在这里开始第一次备份原始8583数据，无条件备份 */
    MEMCPY_DEBUG(Pub_Buf_bak, Pub_Buf,sizeof(unsigned char)*Pub_leng+2);
    g_reply_int=iTest8583(__FILE__,__LINE__,"12345678");
    if(g_reply_int!=0)
    {
        /* 检测8583失败,要重新设置8583 */
        iReset8583(__FILE__,__LINE__,"11111110",(char*)&fd123);
        MEMCPY_DEBUG(g_pub_tx.reply,"T065",4);
        sprintf(acErrMsg,"检测8583数据项目失败\n");
        set_zd_data(DC_GET_MSG,"系统忙，请稍候重试");
        WRITEMSG
        goto ErrExit;
    }
    g_reply_int=hv_initPara(NULL);
    if (g_reply_int<0)
    {
        sprintf(acErrMsg,"初始化网络参数失败!\n");
        set_zd_data(DC_GET_MSG,"初始化网络参数失败");
        lv_log(cLogfile,"跟踪到一个初始化网络参数错误Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
        WRITEMSG
        goto ErrExit;
    }
    g_reply_int=iTest8583(__FILE__,__LINE__,"11111111");
    if(g_reply_int!=0)
    {
        /* 检测8583失败,要重新设置8583 */
        iReset8583(__FILE__,__LINE__,"11111112",(char*)&fd123);
        MEMCPY_DEBUG(g_pub_tx.reply,"T065",4);
        sprintf(acErrMsg,"检测8583数据项目失败\n");
        set_zd_data(DC_GET_MSG,"系统忙，请稍候重试");
        WRITEMSG
        goto ErrExit;
    }
    /*发送报文到前置机**/
    if ((char *)getenv("HVSENDTYPE")!=NULL && !memcmp((char*)getenv("HVSENDTYPE"),"NO_SEND",8))
    {
        /****模拟,不实际发送****/
    }
    else
    {
        iSocketID=hv_connect(HV_SERVER);
        if (iSocketID<=0) 
        {
            /***
            char filename[64];
            fcopy(filename,(char*)&pack_zf,FLAG_SENDER);
            */
            sprintf(acErrMsg,"无法连接大额支付前置机!\n");
            set_zd_data(DC_GET_MSG,"无法连接大额支付前置");
            lv_log(cLogfile,"跟踪到一个连接大额支付前置机错误Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
            WRITEMSG
            goto ErrExit;
        }
        g_reply_int=iTest8583(__FILE__,__LINE__,"11111113");
        if(g_reply_int!=0)
        {
            /* 检测8583失败,要重新设置8583 */
            iReset8583(__FILE__,__LINE__,"11111114",(char*)&fd123);
            MEMCPY_DEBUG(g_pub_tx.reply,"T065",4);
            sprintf(acErrMsg,"检测8583数据项目失败\n");
            set_zd_data(DC_GET_MSG,"系统忙，请稍候重试");
            WRITEMSG
            free(Pu_Val_bak);
            free(Pub_Buf_bak);
            goto ErrExit;
        }
        g_reply_int=hv_send(iSocketID,pack_zf.body);
        if (g_reply_int<0)
        {
            hv_netClose(iSocketID);
            sprintf(acErrMsg,"无法发送报文到大额支付前置机!\n");
            set_zd_data(DC_GET_MSG,"无法发送报文到大额支付前置机");
            lv_log(cLogfile,"跟踪到一个发送报文到大额支付前置机错误Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
            WRITEMSG
            goto ErrExit;  
        }
	/*beg mod by ChengGX 2010-3-8 大额发送就开始写文件*/
	iInsertSendRecord(cCmtno,cTxday,cOrderno,g_pub_tx.tx_br_no,cWssrno,"往帐发送");
	/*end mod by ChengGX 2010-3-8 大额发送就开始写文件*/
	sprintf(acErrMsg,"[%s][%d]接收大额支付前置机响应报文体[%s]\n",__FILE__,__LINE__,respack.body);
         WRITEMSG
        g_reply_int=hv_recv(iSocketID,respack.body);
        if (g_reply_int<0)
        {
            hv_netClose(iSocketID);
            sprintf(acErrMsg,"接收大额支付前置机响应失败!\n");
            set_zd_data(DC_GET_MSG,"接收大额支付前置机响应失败");
            lv_log(cLogfile,"跟踪到一个接收大额支付前置机响应失败错误Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
            WRITEMSG
            goto ErrExit;  
        }
        hv_netClose(iSocketID);
        sprintf(acErrMsg,"[%s][%d]接收大额支付前置机响应报文体[%s]\n",__FILE__,__LINE__,respack.body);
         WRITEMSG
        g_reply_int=hv_analyze_pack(&respack);
        if (g_reply_int<0)
        {
            sprintf(acErrMsg,"接收大额支付前置机响应报文非法!\n");
            WRITEMSG
            set_zd_data(DC_GET_MSG,"接收大额支付前置机响应报文非法");
            lv_log(cLogfile,"跟踪到一个接收大额支付前置机响应报文非法错误Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
            goto ErrExit;  
        }        
        g_reply_int=hv_get_tag(&respack,"C42",tmpBuf);
        if (tmpBuf[3]!='I')
        {
            char cRetBuf[300];
            char cErrBuf[31];
            memset(cRetBuf, 0 , sizeof(cRetBuf));
            memset(cErrBuf, 0 , sizeof(cErrBuf));
            hv_get_tag(&respack,"72A",cRetBuf);
            MEMCPY_DEBUG(cErrBuf,cRetBuf,30);
            sprintf(acErrMsg,"前置机回应了错误信息[%s]!",cErrBuf);
            lv_log(cLogfile,"跟踪到一个前置机回应了错误信息reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
            set_zd_data(DC_GET_MSG,acErrMsg);
            WRITEMSG
            goto ErrExit;
        }
        g_reply_int=iTest8583(__FILE__,__LINE__,"11111115");
        if(g_reply_int!=0)
        {
            iReset8583(__FILE__,__LINE__,"11111116",(char*)&fd123);
        }
        set_zd_data("108J",tmpBuf);
    }
/***************
    g_reply_int = hv_sendpack(&pack_zf);
    if (g_reply_int!=0)
    {
        sprintf(acErrMsg,"发送报文到前置机失败!\n");
        strcpy( g_pub_tx.reply, "LS88" );
        set_zd_data(DC_GET_MSG,"发送报文到前置机失败!");
        WRITEMSG
        goto ErrExit;
    }
*****************/
    get_fd_data("108J",cResp1);
    iInsertSendRecord(cCmtno,cTxday,cOrderno,g_pub_tx.tx_br_no,cWssrno,cResp1);
    iInsertSendRecord(cCmtno,cTxday,cOrderno,g_pub_tx.tx_br_no,cWssrno,tmpBuf);
    vtcp_log("[%s][%d]cOrderno==[%s]",__FILE__,__LINE__,cOrderno);
OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    free(Pu_Val_bak);
    free(Pub_Buf_bak);
    sprintf(acErrMsg,"Before OK return: reply is[%s]\n",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
		lv_log(cLogfile,"成功执行一个交易\n\n");
    return 0;
ErrExit:
    if (memcmp(g_pub_tx.reply,"0000",4)==0)
    {
        MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
    }    
    free(Pu_Val_bak);
    free(Pub_Buf_bak);
    sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    lv_log(cLogfile,"错误执行一个交易Reply=[%s]\n\n",g_pub_tx.reply);
    return 1;
}
