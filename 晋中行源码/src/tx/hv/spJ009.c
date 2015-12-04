/*************************************************
* �� �� ��:  spJ009.c
* ���������� ���֧���ཻ�����˷���
* ��    ��:  xyy 
* ������ڣ� 2006��8��8��
*
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:  SSH
* �޸�����:  �򻯴���
* �� �� �ˣ�LiuHuafeng 2006-12-19 13:42
* ��    �ݣ�1�������ͺ���д�뱾����
*           2��������֯����8583���룬
*           3��������֯�����⣬
*           4��������֯����ָ���
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
char cBuff [5120];/*վλ���� */

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
      lv_log(cLogfile,"%s,%d ���8583���ִ�����������Ϊ[%.8s],�õ�����=[%.8s]\n",file,line,bit8,cBit8);
      return -1;
    }
    else{
      lv_log(cLogfile,"%s,%d ���8583�ɹ�����������Ϊ[%.8s],�õ�����=[%.8s]\n",file,line,bit8,cBit8);
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
    lv_log(cLogfile,"%s,%d pu_val��ַ=[%ld],pub_buf��ַ=[%ld],���������ַ�����",__FILE__,__LINE__,Pu_Val,Pub_Buf);
    lv_log(cLogfile,"%s,%d pu_val_bak��ַ=[%ld],pub_buf_bak��ַ=[%ld]",__FILE__,__LINE__,Pu_Val_bak,Pub_Buf_bak);
    free(Pu_Val);
    free(Pub_Buf);
    Pu_Val=(struct PVal_St *) malloc (sizeof(struct PVal_St) *PuVal_num);
    Pub_Buf=(unsigned char *)malloc (sizeof(unsigned char)*Pub_leng+2); 
    memset(Pu_Val,0x00,sizeof(struct PVal_St) *PuVal_num);    
    memset(Pub_Buf,0x20,sizeof(unsigned char)*Pub_leng+2);
    lv_log(cLogfile,"%s,%d ��ʼ�ָ�8583��������,pub_leng=[%ld],pubval_num=[%d]\n",file,line,Pub_leng,PuVal_num);
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
    lv_log(cLogfile,"���ݵ�8583����=[%s]\n",Pub_Buf_bak);
		/****
    MEMCPY_DEBUG(Pub_Buf,Pub_Buf_bak,sizeof(unsigned char)*Pub_leng+2);
    *****/
    lv_log(cLogfile,"�ָ���8583����=[%s]\n",Pub_Buf);
    lv_log(cLogfile,"[%s][%d]�ָ���ɣ��ٴμ��,������Ϊ[%.8s]\n",file,line,bit8);
    iRet=iTest8583(__FILE__,__LINE__,bit8);
    if(iRet)
    {
      int recdnum=0;
      recdnum=0;
      lv_log(cLogfile,"[%s][%d]==========================================================\n",file,line);
      lv_log(cLogfile,"[%s][%d]�ָ�8583δ����Ч���ٴ������ָ����ƣ����ش���,���ش���\n",file,line);
      lv_log(cLogfile,"%s,%d pu_val��ַ=[%ld],pub_buf��ַ=[%ld],���������ַ�����",__FILE__,__LINE__,Pu_Val,Pub_Buf);
      lv_log(cLogfile,"%s,%d pu_val_bak��ַ=[%ld],pub_buf_bak��ַ=[%ld]",__FILE__,__LINE__,Pu_Val_bak,Pub_Buf_bak);
      lv_log(cLogfile,"[%s][%d]=================�����ݱ��еõ��������===================\n",file,line);
      recdnum = sql_count ( "data_dic" , "%s" , "1=1" );
      if (recdnum<=0) 
      {
          vtcp_log("�����ֵ�������!!ERROR %d",recdnum);
          return -1;
      }
      iRet = sql_sum_long( "data_dic","data_leng",&iLength,"%s","1=1" );
	    if ( iRet )
	    {
            vtcp_log("�����ֵ�sum!!ERROR");
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
          vtcp_log("ȡ�����ֵ����!!error code =%d.\n", RespCode );
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
      lv_log(cLogfile,"[%s][%d]���ݱ����������³�ʼ��8583�ṹ��������±߳�ʼ�����8583��������\n",file,line);
      pub_base_sysinit();
      set_zd_data("1230",fd123);
      lv_log(cLogfile,"[%s][%d]*********���λָ�8583����ɻָ���������:\n",file,line);
      for (i=0,iLength=0;i<PuVal_num;i++) 
      {
          lv_log(cLogfile,"i=[%d],sjbm=[%s],length=[%d]\n",i,Pu_Val[i].sjbm,Pu_Val[i].length);
      }
      lv_log(cLogfile,"[%s]\n",Pub_Buf);
      lv_log(cLogfile,"[%s][%d]*********���е����μ��飬������μ���ʧ�ܣ���û����\n",file,line);
      iRet=iTest8583(__FILE__,__LINE__,bit8);
      if(iRet)
      {
        lv_log(cLogfile,"[%s][%d]�����μ��飬û����\n",file,line);
        
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
    /***********add by liuyue   ����һ�����������洢8583��ĵ�ֵ����***/   
    int     recdnum=0;
    long    length=0;
    
    char  c_013_buf[50];        /**���13��*****/
    char    _cBr_no[BRNO_LEN+1];
    HV_PAYIN_AREA   hv_payin;
    char cBuff1[5120];  /*վλ���� */
    HV_FD123_AREA   fd123;
    char cBuff2[5120];/*վλ���� */
    THVPACKAGE pack_zf;
    char cBuff3[5120];/*վλ���� */
    recdnum=PuVal_num;/*ֱ����ȫ�ֱ��� */
    length=Pub_leng;/* ֱ����ȫ�ֱ������� */
    memset(cLogfile, 0 , sizeof(cLogfile));
    memset(c_013_buf,0x20,sizeof(c_013_buf));/**��ʼ��Ϊ�ո�***/
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

    vtcp_log("[%s][%d]��ȡ����8583���ֵ===[%ld]\n",__FILE__,__LINE__,length);
    /* ׼�����ݵ�8583���ݱ��� */
    {
        int i=0;
        long iLength=0;
        Pu_Val_bak =  (struct PVal_St *)malloc (sizeof(struct PVal_St) *PuVal_num);/**8583���ݵĹ��ñ���ָ��***/
        Pub_Buf_bak =(unsigned char *)malloc (sizeof(unsigned char)*Pub_leng+2);/**8583���ݵĹ��ñ���������ͷָ��**/
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
        lv_log(cLogfile,"%s,%d [%ld]pack_zf��ַ=[%ld]",__FILE__,__LINE__,g_pub_tx.tx_time,&pack_zf);
        lv_log(cLogfile,"%s,%d pu_val��ַ=[%ld],pub_buf��ַ=[%ld]",__FILE__,__LINE__,Pu_Val,Pub_Buf);
        lv_log(cLogfile,"%s,%d pu_VAL_bak��ַ=[%ld],pub_BUF_bak��ַ=[%ld]",__FILE__,__LINE__,Pu_Val_bak,Pub_Buf_bak);
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

    /*** ��ʼ��ȫ�ֱ��� ***/
    pub_base_sysinit();

    get_fd_data(DC_DATE,cTxday);
    get_fd_data(DC_TRACE_NO,cWssrno);
  
    /***�õ����֧��ҵ��8583����***/
    g_reply_int = iHvTistoPayin(&hv_payin);
    if ( g_reply_int )
    {
        sprintf( acErrMsg,"����iHvTistoNotpay����![%s][%d]",__FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "P015" );
    	lv_log(cLogfile,"���ٵ�һ�����׵���iHvTistoNotpay����Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
        WRITEMSG
        goto ErrExit;
    }
    /******���Ӫҵ��״̬******/ 
  
    g_reply_int = iHv_checkstat(hv_payin.F_cmtno);
    if ( g_reply_int )
    {
        sprintf( acErrMsg,"����״̬������![%s][%d][%s]",__FILE__,__LINE__,g_reply_int);
        strcpy( g_pub_tx.reply, "HV01");
        set_zd_data(DC_GET_MSG,"���֧��ϵͳ��ǰ״̬�޷����˽���");
    	lv_log(cLogfile,"���ٵ�һ�����״��֧��ϵͳ��ǰ״̬�޷����˽���Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
        WRITEMSG
        goto ErrExit;
    }    
    vtcp_log("[%s][%d]������iHv_checkstat",__FILE__,__LINE__);
    
    g_reply_int = FetchRecToPayin(&hv_payin);    
    if ( g_reply_int )
    {
        sprintf( acErrMsg,"����FetchRecToPayin����![%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,"����FetchRecToPayin����");
    	lv_log(cLogfile,"���ٵ�һ�����׵���FetchRecToPayin����Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
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
        /***����Ƿ�����������****/
        if (memcmp(hv_payin.F_tx_br_no,cQsbrno,BRNO_LEN)==0) 
        {
            sprintf(acErrMsg,"�������Ĳ������˽���!\n");
            strcpy( g_pub_tx.reply, "HV01");
            set_zd_data(DC_GET_MSG,"�������Ĳ������˽���");
            lv_log(cLogfile,"���ٵ�һ������Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
            WRITEMSG
            goto ErrExit;
        }
    }
    g_reply_int = iHvFrontToTable(&hv_payin);
    if (g_reply_int)
    {
        sprintf( acErrMsg,"����iHvFrontToTable����![%s][%d]", __FILE__,__LINE__);
        strcpy( g_pub_tx.reply, "HV01");
        WRITEMSG
        lv_log(cLogfile,"���ٵ�һ�����׵���iHvFrontToTable����Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
        goto ErrExit;
    }
    vtcp_log("[%s][%d]hv_payin->T_hv_brf======[%s]",__FILE__,__LINE__,hv_payin.T_hv_brf);

    MEMCPY_DEBUG(hv_payin.T_refno,cRefno,sizeof(cRefno)-1);
    if ( hv_payin.T_hv_sts[0] != SEND_ACCT_WAIT)
    {
        sprintf(acErrMsg,"��¼״̬����!\n");
        strcpy( g_pub_tx.reply, "P016");
        vtcp_log("[%s][%d]T_hv_sts=[%c][%c]\n",__FILE__,__LINE__,hv_payin.T_hv_sts[0] , SEND_ACCT_CHECK);
        set_zd_data(DC_GET_MSG,"��¼״̬����");
        lv_log(cLogfile,"���ٵ�һ�����׼�¼״̬����Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
        WRITEMSG
        goto ErrExit;
    }
    hv_payin.T_hv_sts[0]=SEND_ACCT_WAIT;/*����״̬���Ƿ���ҪUPDATE?*/
    vtcp_log("[%s][%d]hv_payin->T_yw_type=[%.8s]",__FILE__,__LINE__,hv_payin.T_yw_type);
    vtcp_log("[%s][%d]hv_payin->T_hv_brf======[%s]",__FILE__,__LINE__,hv_payin.T_hv_brf);
    
    g_reply_int = iHvUpdZfRecFromPayin(&hv_payin,0);
    if (g_reply_int)
    {
        sprintf(acErrMsg,"����iHvUpdZfRecFromPayin��������!\n");
        strcpy( g_pub_tx.reply, "P018");
        set_zd_data(DC_GET_MSG,"����iHvUpdZfRecFromPayin��������");
        lv_log(cLogfile,"���ٵ�һ�����׵���iHvUpdZfRecFromPayin��������Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d]hv_payin->T_yw_type=[%.8s]",__FILE__,__LINE__,hv_payin.T_yw_type);
    vtcp_log("[%s][%d]hv_payin->T_hv_brf======[%s]",__FILE__,__LINE__,hv_payin.T_hv_brf);
  
    switch(iCmtno) /*��ʼƴ��,���뷢�͵Ǽǲ�*/           
    {
        case 100:  /*��ҽ��� */
            pub_com_CMT100_T2TAG(&hv_payin,&pack_zf);
            break;
        case 101:  /*ί���տ�*/
            pub_com_CMT101_T2TAG(&hv_payin,&pack_zf);
            break;
        case 102:  /*���ճи�*/ 
            pub_com_CMT102_T2TAG(&hv_payin,&pack_zf);
            break;
        case 103:  /*�����ʽ�㻮*/
            vtcp_log("[%s][%d]hv_payin->T_yw_type=[%.8s]",__FILE__,__LINE__,hv_payin.T_yw_type);
            pub_com_CMT103_T2TAG(&hv_payin,&pack_zf);
            break;
        case 105:  /*���м���*/
            pub_com_CMT105_T2TAG(&hv_payin,&pack_zf);
            break;
        case 108:  /*�˻�*/
            pub_com_CMT108_T2TAG(&hv_payin,&pack_zf);
            break;
        case 121:  /*��Ʊ����*/
            vtcp_log("payin===[%s]\n",(char *)&hv_payin);
            pub_com_CMT121_T2TAG(&hv_payin,&pack_zf);
            vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
            break;        
        default:
            sprintf(acErrMsg,"û�иñ�������!\n");
            strcpy( g_pub_tx.reply, "HV01");
            set_zd_data(DC_GET_MSG,"û�иñ�������");
            lv_log(cLogfile,"���ٵ�һ������cmtno����Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
            WRITEMSG
            goto ErrExit;
            break;
    }
  
    getPackageFlgNo(&pack_zf);/**�õ�FlgNo**/
    vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
    ret=hv_gen_pack(&pack_zf);
    vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
    if (ret<0)
    {
        sprintf(acErrMsg,"��֯���֧�����Ĵ���!\n");
        WRITEMSG
        strcpy( g_pub_tx.reply, "P021" );
        lv_log(cLogfile,"���ٵ�һ��������֯���Ĵ���Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
        set_zd_data(DC_GET_MSG,"��֯���֧�����Ĵ���");
        goto ErrExit;
    }
    vtcp_log("[%s][%d]hv_payin->T_yw_type=[%.8s]",__FILE__,__LINE__,hv_payin.T_yw_type);
    
    MEMCPY_DEBUG(cSendtel,hv_payin.T_send_tel,sizeof(cSendtel)-1);
    MEMCPY_DEBUG(cSendTraceno,hv_payin.T_send_tel_trace_no,sizeof(cSendTraceno)-1);
    MEMCPY_DEBUG(cOrderno,hv_payin.T_orderno,sizeof(cOrderno)-1);
    lv_log(cLogfile,"%s,%d ֧���������orderno=[%s],brno=[%s]",__FILE__,__LINE__,cOrderno,g_pub_tx.tx_br_no);
    MEMCPY_DEBUG(_cBr_no,hv_payin.F_tx_br_no,sizeof(hv_payin.F_tx_br_no));
    if (hv_insert_sendlist(&pack_zf,_cBr_no,cSendtel,cSendTraceno,cOrderno)<0)
    {
        sprintf(acErrMsg,"���뷢�͵Ǽǲ�ʧ��!\n");
        WRITEMSG
        strcpy( g_pub_tx.reply, "P010" );
        set_zd_data(DC_GET_MSG,"���뷢�͵Ǽǲ�ʧ��!");
        lv_log(cLogfile,"���ٵ�һ������д���͵Ǽǲ�ʧ��Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
        goto ErrExit;
    }
    vtcp_log("[%s][%d]��Ҫ����pub_ins_trace_log����!",__FILE__,__LINE__);
    /*���ͽ���Ҫд��ˮ*/
    if ( pub_ins_trace_log() )
    {
        sprintf(acErrMsg,"���ͽ���д��ˮ����!\n");
        strcpy( g_pub_tx.reply, "HV01");
        lv_log(cLogfile,"���ٵ�һ������д��ˮ����Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
        set_zd_data(DC_GET_MSG,"���ͽ���д��ˮ����");
        WRITEMSG
        goto ErrExit;
    }
  
    vtcp_log("[%s][%d]cWssrno=[%s]",__FILE__,__LINE__,cWssrno);
    /**���͵�ǰ�û�**/
    memset(cWssrno, 0 , sizeof(cWssrno));
    MEMCPY_DEBUG(cWssrno,hv_payin.T_chk_trace_no,sizeof(cWssrno)-1);
    MEMCPY_DEBUG(cCmtno,hv_payin.T_cmtno,sizeof(cCmtno)-1);
    MEMCPY_DEBUG(cOrbrno,hv_payin.T_or_br_no,sizeof(cOrbrno)-1);
    
    g_reply_int = 0;
    /*�����￪ʼ��һ�α���ԭʼ8583���ݣ����������� */
    MEMCPY_DEBUG(Pub_Buf_bak, Pub_Buf,sizeof(unsigned char)*Pub_leng+2);
    g_reply_int=iTest8583(__FILE__,__LINE__,"12345678");
    if(g_reply_int!=0)
    {
        /* ���8583ʧ��,Ҫ��������8583 */
        iReset8583(__FILE__,__LINE__,"11111110",(char*)&fd123);
        MEMCPY_DEBUG(g_pub_tx.reply,"T065",4);
        sprintf(acErrMsg,"���8583������Ŀʧ��\n");
        set_zd_data(DC_GET_MSG,"ϵͳæ�����Ժ�����");
        WRITEMSG
        goto ErrExit;
    }
    g_reply_int=hv_initPara(NULL);
    if (g_reply_int<0)
    {
        sprintf(acErrMsg,"��ʼ���������ʧ��!\n");
        set_zd_data(DC_GET_MSG,"��ʼ���������ʧ��");
        lv_log(cLogfile,"���ٵ�һ����ʼ�������������Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
        WRITEMSG
        goto ErrExit;
    }
    g_reply_int=iTest8583(__FILE__,__LINE__,"11111111");
    if(g_reply_int!=0)
    {
        /* ���8583ʧ��,Ҫ��������8583 */
        iReset8583(__FILE__,__LINE__,"11111112",(char*)&fd123);
        MEMCPY_DEBUG(g_pub_tx.reply,"T065",4);
        sprintf(acErrMsg,"���8583������Ŀʧ��\n");
        set_zd_data(DC_GET_MSG,"ϵͳæ�����Ժ�����");
        WRITEMSG
        goto ErrExit;
    }
    /*���ͱ��ĵ�ǰ�û�**/
    if ((char *)getenv("HVSENDTYPE")!=NULL && !memcmp((char*)getenv("HVSENDTYPE"),"NO_SEND",8))
    {
        /****ģ��,��ʵ�ʷ���****/
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
            sprintf(acErrMsg,"�޷����Ӵ��֧��ǰ�û�!\n");
            set_zd_data(DC_GET_MSG,"�޷����Ӵ��֧��ǰ��");
            lv_log(cLogfile,"���ٵ�һ�����Ӵ��֧��ǰ�û�����Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
            WRITEMSG
            goto ErrExit;
        }
        g_reply_int=iTest8583(__FILE__,__LINE__,"11111113");
        if(g_reply_int!=0)
        {
            /* ���8583ʧ��,Ҫ��������8583 */
            iReset8583(__FILE__,__LINE__,"11111114",(char*)&fd123);
            MEMCPY_DEBUG(g_pub_tx.reply,"T065",4);
            sprintf(acErrMsg,"���8583������Ŀʧ��\n");
            set_zd_data(DC_GET_MSG,"ϵͳæ�����Ժ�����");
            WRITEMSG
            free(Pu_Val_bak);
            free(Pub_Buf_bak);
            goto ErrExit;
        }
        g_reply_int=hv_send(iSocketID,pack_zf.body);
        if (g_reply_int<0)
        {
            hv_netClose(iSocketID);
            sprintf(acErrMsg,"�޷����ͱ��ĵ����֧��ǰ�û�!\n");
            set_zd_data(DC_GET_MSG,"�޷����ͱ��ĵ����֧��ǰ�û�");
            lv_log(cLogfile,"���ٵ�һ�����ͱ��ĵ����֧��ǰ�û�����Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
            WRITEMSG
            goto ErrExit;  
        }
	/*beg mod by ChengGX 2010-3-8 ���;Ϳ�ʼд�ļ�*/
	iInsertSendRecord(cCmtno,cTxday,cOrderno,g_pub_tx.tx_br_no,cWssrno,"���ʷ���");
	/*end mod by ChengGX 2010-3-8 ���;Ϳ�ʼд�ļ�*/
	sprintf(acErrMsg,"[%s][%d]���մ��֧��ǰ�û���Ӧ������[%s]\n",__FILE__,__LINE__,respack.body);
         WRITEMSG
        g_reply_int=hv_recv(iSocketID,respack.body);
        if (g_reply_int<0)
        {
            hv_netClose(iSocketID);
            sprintf(acErrMsg,"���մ��֧��ǰ�û���Ӧʧ��!\n");
            set_zd_data(DC_GET_MSG,"���մ��֧��ǰ�û���Ӧʧ��");
            lv_log(cLogfile,"���ٵ�һ�����մ��֧��ǰ�û���Ӧʧ�ܴ���Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
            WRITEMSG
            goto ErrExit;  
        }
        hv_netClose(iSocketID);
        sprintf(acErrMsg,"[%s][%d]���մ��֧��ǰ�û���Ӧ������[%s]\n",__FILE__,__LINE__,respack.body);
         WRITEMSG
        g_reply_int=hv_analyze_pack(&respack);
        if (g_reply_int<0)
        {
            sprintf(acErrMsg,"���մ��֧��ǰ�û���Ӧ���ķǷ�!\n");
            WRITEMSG
            set_zd_data(DC_GET_MSG,"���մ��֧��ǰ�û���Ӧ���ķǷ�");
            lv_log(cLogfile,"���ٵ�һ�����մ��֧��ǰ�û���Ӧ���ķǷ�����Reply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
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
            sprintf(acErrMsg,"ǰ�û���Ӧ�˴�����Ϣ[%s]!",cErrBuf);
            lv_log(cLogfile,"���ٵ�һ��ǰ�û���Ӧ�˴�����Ϣreply=[%s],line=[%d]\n\n",g_pub_tx.reply,__LINE__);
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
        sprintf(acErrMsg,"���ͱ��ĵ�ǰ�û�ʧ��!\n");
        strcpy( g_pub_tx.reply, "LS88" );
        set_zd_data(DC_GET_MSG,"���ͱ��ĵ�ǰ�û�ʧ��!");
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
		lv_log(cLogfile,"�ɹ�ִ��һ������\n\n");
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
    lv_log(cLogfile,"����ִ��һ������Reply=[%s]\n\n",g_pub_tx.reply);
    return 1;
}
