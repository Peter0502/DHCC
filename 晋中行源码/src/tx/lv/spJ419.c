/***************************************************************************/
/* �ļ�����: spJ419.c                                                   */
/* ��������: ��ͨ����ҵ�񸴺�                                             */
/* ��������: 2005.08.01                                                    */
/* ����:     SSH                                                           */
/* ˵��:                                                                   */
/***************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "lv_define.h"
#include "hv_define.h"
#include "lv_param_c.h"
#include "lv_pkgreg_c.h"
extern char * pcLvGetDay();
static PAY_IN_AREA payin;
struct lv_param_c lv_param;
int spJ419()
{
     char cCur_no[3];
     FILE* fpr;
     int ret=0;
     char cBuff[2048];
     char cFilename[31];
     memset(cFilename,0 , sizeof(cFilename));
     memset(cBuff,0 , sizeof(cBuff));
     memset(cCur_no,0 , sizeof(cCur_no));
  vtcp_log("AP MSG: Begin of dh65100_Work\n");
     memset(&payin,'\0',sizeof(payin));
     pub_base_sysinit();
     /****���payin*********/
     if(iLvchkstat())
     {
          strncpy(g_pub_tx.reply,"P046",4);
          WRITEMSG
          goto ErrExit;
     }

     g_reply_int = iLvTisToPayin(&payin);
     if(g_reply_int)
     {
          vtcp_log("%s,%d error in ",__FL__);
          sprintf(acErrMsg,"%s,%d iLvTisToPayinʧ��",__FL__);
          strncpy(g_pub_tx.reply,"P046",4);
          set_zd_data("0130",acErrMsg);
          WRITEMSG
          goto ErrExit;
     }
     vtcp_log("%s,%d,txamt[%.15s][%.15s]",__FL__,payin.F_txamt,payin.T_txamt);
     g_reply_int = iLvFetchRecToPayin(&payin);
     if(g_reply_int)
     {
          vtcp_log("%s,%d error in ",__FL__);
          sprintf(acErrMsg,"%s,%d iLvFetchRecToPayinʧ��",__FL__);     
          if(memcmp(g_pub_tx.reply,"0000",4)==0)
          {
               strncpy(g_pub_tx.reply,"P046",4);
               set_zd_data("0130",acErrMsg);
               sprintf(acErrMsg,"%s,%d iLvFetchRecToPayinʧ��",__FL__);
          }
          else
               set_zd_data("0130",acErrMsg);
          WRITEMSG
          goto ErrExit;
     }
	/**¼��͸��˲�����ͬһ������Ա**/
     if(memcmp(g_pub_tx.tel,payin.T_inputno,4)==0){
		sprintf(acErrMsg,"¼��͸��˲�����ͬһ������Ա!");
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P080");
		goto ErrExit;
     }
     pub_base_AllDwnFilName( cFilename);
        vtcp_log("%s,%d filename=[%s]\n",__FL__,cFilename);
        fpr=fopen(cFilename,"rb");
        if (fpr==NULL){
                sprintf(acErrMsg,"open cFilename[%s] error!\n",cFilename);
                strncpy(g_pub_tx.reply,"D010",4);
                set_zd_data("0130",acErrMsg);
                WRITEMSG
                goto ErrExit;
        }
        fgets(cBuff,sizeof(cBuff),fpr);
        if(memcmp(payin.T_txnum,TXNUM_ZPJL,5)==0){
                ZPJL1_ADD_AREA zpjl_add1,wd_add1;
                ZPJL1_ADD_AREA zpjl_add2,wd_add2;
                ZPJL3_ADD_AREA zpjl_add3,wd_add3;
          	char BSCNT[3];
		char tmpstr1[1024];
          	int  cnt=0;
                memset(&zpjl_add1,0,sizeof(zpjl_add1));
                memset(&zpjl_add2,0,sizeof(zpjl_add2));
                memset(&zpjl_add3,0,sizeof(zpjl_add3));
                memset(tmpstr1,0,sizeof(tmpstr1));
                memset(&wd_add1,0,sizeof(wd_add1));
                memset(&wd_add2,0,sizeof(wd_add2));
                memset(&wd_add3,0,sizeof(wd_add3));
                memset(BSCNT,0,sizeof(BSCNT));
          	memcpy(&wd_add1,cBuff,sizeof(wd_add1));
          	memcpy(BSCNT,cBuff+sizeof(wd_add1),2);     
          	vtcp_log("%s,%d �����嵥wd_add1[%s]bscnt[%s]\n",__FL__,&wd_add1,BSCNT);
          	cnt=apatoi(BSCNT,2);
          	memcpy(&wd_add2,cBuff+sizeof(wd_add1),2+cnt*60);
          	memcpy(tmpstr1,&wd_add2,2+cnt*60);
          	vtcp_log("%s,%d �����嵥wd_add2[%.302s]bscnt[%s]tmpstr1[%s]\n",__FL__,&wd_add2,BSCNT,tmpstr1);
          	memcpy(&wd_add3,cBuff+sizeof(wd_add1)+2+cnt*60,sizeof(wd_add3));
                ret=iLvGetAddit(&payin,"001",&zpjl_add1,sizeof(zpjl_add1));
          	if(ret){
                     sprintf(acErrMsg,"ȡ��������� error!ret[%d]\n",ret);
                  strncpy(g_pub_tx.reply,"D010",4);
                     set_zd_data("0130",acErrMsg);
                     WRITEMSG
                     goto ErrExit;
         	 }
          if(memcmp(zpjl_add1.cpday,wd_add1.cpday,sizeof(wd_add1.cpday))\
               || memcmp(zpjl_add1.cpno,wd_add1.cpno,sizeof(wd_add1.cpno)) || memcmp(zpjl_add1.use,wd_add1.use,sizeof(zpjl_add1.use))){
                        sprintf(acErrMsg,"֧Ʊ��,��Ʊ����,��;,���˲�һ��! error!ret[%d]\n",ret);
                        strncpy(g_pub_tx.reply,"D010",4);
                        set_zd_data("0130",acErrMsg);
                        WRITEMSG
                        goto ErrExit;
          }
          vtcp_log("%s,%d �����嵥cBuff[%s]\n",__FL__,cBuff);
          ret=iLvGetAddit(&payin,"002",&zpjl_add2,2+cnt*60);
vtcp_log("%s,%d [%s],[%s],[%d]\n",__FL__,&zpjl_add2,tmpstr1,2+cnt*60);
          if(memcmp((char *)&zpjl_add2,tmpstr1,2+cnt*60)){
               sprintf(acErrMsg,"�����嵥���˲�һ��! error!ret[%d]\n",ret);
                        strncpy(g_pub_tx.reply,"D010",4);
                        set_zd_data("0130",acErrMsg);
                        WRITEMSG
               vtcp_log("%s,%d �����嵥zpjl_add2[%s],wd_add2[%s]\n",__FL__,&zpjl_add2,&wd_add2);
                        goto ErrExit;
          }
          ret=iLvGetAddit(&payin,"003",&zpjl_add3,sizeof(zpjl_add3));
          if(memcmp(zpjl_add3.bscheckpswd,wd_add3.bscheckpswd,20)){
               sprintf(acErrMsg,"֧���������!");
              /* vtcp_log("%s,%d ֧������zpjl_add3.bscheckpswd[%.20s],wd_add3.bscheckpswd[%.20s]\n",__FL__,zpjl_add3.bscheckpswd,wd_add3.bscheckpswd);*/
                        strncpy(g_pub_tx.reply,"D010",4);
                        set_zd_data("0130",acErrMsg);
                        WRITEMSG
                        goto ErrExit;
          }
          vtcp_log("%s,%d,������Ҫ�ؼ�����!zpjl_add1.cpday[%.8s],zpjl_add1.cpno[%.12s],zpjl_add1.use[%.60s]\n",__FL__,zpjl_add1.cpday,zpjl_add1.cpno,zpjl_add1.use);
     }else if(memcmp(payin.T_txnum,TXNUM_TYJL,5)==0){
          TYJL1_ADD_AREA  tyjl_add1,wd_add1;
          TYJL2_ADD_AREA  tyjl_add2,wd_add2;
          TYJL3_ADD_AREA  tyjl_add3,wd_add3;
          TYJL4_ADD_AREA  tyjl_add4,wd_add4;
          char BSCNT[3];
          int cnt;
          memset(&tyjl_add1,0,sizeof(tyjl_add1));
          memset(&wd_add1,0,sizeof(wd_add1));
          memset(&tyjl_add2,0,sizeof(tyjl_add2));
          memset(&wd_add2,0,sizeof(wd_add2));
          memset(&tyjl_add4,0,sizeof(tyjl_add4));
          memset(&wd_add4,0,sizeof(wd_add4));
          memset(BSCNT,0,sizeof(BSCNT));
          memcpy(&wd_add1,cBuff,sizeof(wd_add1));
          memcpy(BSCNT,cBuff+sizeof(wd_add1),2);
          cnt=apatoi(BSCNT,2);
          memcpy(&wd_add2,cBuff+sizeof(wd_add1),2+cnt*60);
          memcpy(&wd_add4,cBuff+sizeof(wd_add1)+2+cnt*60+sizeof(wd_add3),sizeof(wd_add4));
          ret=iLvGetAddit(&payin,"001",&tyjl_add1,sizeof(tyjl_add1));
          if(ret){
                  sprintf(acErrMsg,"ȡ��������� error!ret[%d]\n",ret);
                  strncpy(g_pub_tx.reply,"D010",4);
                  set_zd_data("0130",acErrMsg);
                  WRITEMSG
                  goto ErrExit;
         }
          if(memcmp(&wd_add1,&tyjl_add1,sizeof(tyjl_add1))){
                   sprintf(acErrMsg,"ͨ�ø�����1���˲�һ��! error!ret[%d]\n",ret);
                   strncpy(g_pub_tx.reply,"D010",4);
                   set_zd_data("0130",acErrMsg);
                   WRITEMSG
                   goto ErrExit;
          }
          ret=iLvGetAddit(&payin,"002",&tyjl_add2,2+cnt*60);
	  if(memcmp(&wd_add2,&tyjl_add2,2+cnt*60)){
	           sprintf(acErrMsg,"ͨ�ñ����嵥���˲�һ��! error!ret[%d]\n",ret);
                   strncpy(g_pub_tx.reply,"D010",4);
                   set_zd_data("0130",acErrMsg);
                   WRITEMSG
                   vtcp_log("%s,%d �����嵥zpjl_add2[%s],wd_add2[%s]\n",__FL__,&tyjl_add2,&wd_add2);
			goto ErrExit;
	  }
          ret=iLvGetAddit(&payin,"004",&tyjl_add4,sizeof(tyjl_add4));
          if(memcmp(&wd_add4,&tyjl_add4,sizeof(tyjl_add4))){
                   sprintf(acErrMsg,"ͨ�ø�����4���˲�һ��! error!ret[%d]\n",ret);
                   strncpy(g_pub_tx.reply,"D010",4);
                   set_zd_data("0130",acErrMsg);
                   WRITEMSG
                   goto ErrExit;
          }
          vtcp_log("%s,%d,ͨ�ý��������򸴺����!",__FL__);
     }else{
          vtcp_log("%s,%d,ҵ�����ʹ���!",__FL__);
          goto ErrExit;
     }
      vtcp_log("%s,%d,txamt[%.15s][%.15s]",__FL__,payin.F_txamt,payin.T_txamt);
     memcpy(payin.F_packday,payin.T_packday,sizeof(payin.F_packday));
     memcpy(payin.F_packid,payin.T_packid,sizeof(payin.F_packid));
     memcpy(payin.F_ywtype,payin.T_ywtype,sizeof(payin.F_ywtype));
  vtcp_log("%s,%d,txamt[%.15s][%.15s]",__FL__,payin.F_txamt,payin.T_txamt);
     
  vtcp_log("[%s][%d]\n", __FL__);
     /**********************************/
     /* verify data for basic checking */
     /**********************************/
     /****�������****/
  vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
  vtcp_log("[%s][%d]T_ptype==[%s]\n", __FL__,payin.T_ptype);
     g_reply_int=iLvPayinCheck0(&payin);
     if(g_reply_int){
          vtcp_log("[%s][%d] err in check0 err=[%d]\n", __FL__,g_reply_int);
          strncpy(g_pub_tx.reply,"P046",4);
          pub_base_strpack(acErrMsg);
          if(strlen(acErrMsg)==0)
                    sprintf(acErrMsg,"%s,%d iLvPayinCheck0ʧ��",__FL__);
          set_zd_data("0130",acErrMsg);
          WRITEMSG
          goto ErrExit;
     }
  vtcp_log("[%s][%d]T_ptype==[%s]\n", __FL__,payin.T_ptype);
     /****Ҫ�ؼ��****/
  vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
     g_reply_int=iLvPayinCheck4(&payin);
     if(g_reply_int){
          vtcp_log("[%s][%d] err in check0 err=[%d]\n", __FL__,g_reply_int);
          pub_base_strpack(acErrMsg);
          if(strlen(acErrMsg)==0)
                    sprintf(acErrMsg,"%s,%d iLvPayinCheck4ʧ��",__FL__);
          strncpy(g_pub_tx.reply,"P046",4);
          set_zd_data("0130",acErrMsg);
          WRITEMSG
          goto ErrExit;
     }
  vtcp_log("[%s][%d]T_ptype==[%s]\n", __FL__,payin.T_ptype);
  vtcp_log("[%s][%d]\n", __FL__);
     /****�кż��****/
  vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
     g_reply_int = iLvPayinCheck1(&payin);
     if(g_reply_int)
     {
          vtcp_log("[%s][%d]sqlcode=[%d]",__FILE__,__LINE__,g_reply_int);
          pub_base_strpack(acErrMsg);
          if(strlen(acErrMsg)==0)
               sprintf(acErrMsg,"%s,%d iLvPayinCheck1ʧ��",__FL__);
          strncpy(g_pub_tx.reply,"P046",4);
          set_zd_data("0130",acErrMsg);
          goto ErrExit;
     }
  vtcp_log("[%s][%d]T_ptype==[%s]\n", __FL__,payin.T_ptype);
  vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
  vtcp_log("%s,%d,txamt[%.15s][%.15s]",__FL__,payin.F_txamt,payin.T_txamt);
  get_fd_data("0030", lvca.brno);
  vtcp_log("%s,%d,lvca.brno[%s]\n******",__FL__,lvca.brno);

     /***����һ���Լ��****/
     g_reply_int=iLvPayinCheck6(&payin);
     if(g_reply_int)
     {
          vtcp_log("[%s][%d]check6 error sqlcode=[%d]", __FL__,g_reply_int);
          pub_base_strpack(acErrMsg);
          if(strlen(acErrMsg)==0)
                    sprintf(acErrMsg,"%s,%d iLvPayinCheck6ʧ��",__FL__);
          strncpy(g_pub_tx.reply,"P046",4);
          set_zd_data("0130",acErrMsg);
          WRITEMSG
          goto ErrExit;
     }
     /********************************************************/
     /* Process -- 1.Initial PBMR record to opening stauts   */
     /*            2.Insert PBMR record/Delete(reversal)     */
     /********************************************************/
     int iOrderno;
     vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);

     get_fd_data(DC_TEL,payin.T_checkno);
     get_fd_data(DC_TRACE_NO,payin.T_checkwssrno);
     get_fd_data(DC_DATE,payin.T_jzday);
     memcpy(payin.T_jzday,pcLvGetDay(),sizeof(payin.T_jzday));

     vtcp_log("[%s][%d]payin.T_jzday=[%s]\n",__FILE__,__LINE__,payin.T_jzday);

     payin.T_stat[0]=STAT_WZSEND;
     g_reply_int=iLvUpdRecFromPayin(&payin,0);
     if(g_reply_int){
          vtcp_log("%s,%d,���µǼǲ�ʧ��!sqlcode=[%d]", __FILE__,__LINE__,g_reply_int);
          pub_base_strpack(acErrMsg);
          if(strlen(acErrMsg)==0)
                    sprintf(acErrMsg,"%s,%d ���µǼǲ�ʧ��",__FL__);
          strncpy(g_pub_tx.reply,"P046",4);
          set_zd_data("0130",acErrMsg);
          WRITEMSG
          goto ErrExit;
     }
     vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
     
     g_reply_int=ilv11_PreTos();
     if(g_reply_int)
     {
          sprintf(acErrMsg,"%s,%d set fd123 error!",__FL__);
          strncpy(g_pub_tx.reply,"P079",4);
          set_zd_data("0130",acErrMsg);
          WRITEMSG
          goto ErrExit;
     }

  g_reply_int =pub_ins_trace_log();
  if(g_reply_int)
  {
      sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
      strncpy(g_pub_tx.reply,"S023",4);
      set_zd_data("0130",acErrMsg);
      WRITEMSG
      goto ErrExit;
  }
  memcpy(lv_param.par_code,"WDBRTDAY",8);
  g_reply_int = Lv_param_Sel(g_pub_tx.reply, &lv_param, "par_code= '%s'", lv_param.par_code);
  if(g_reply_int){
            strcpy(payin.T_respday,"1");
  }else{
            memcpy(payin.T_respday,lv_param.par_value,sizeof(payin.T_respday));
  }
  ret=SendZpCifToYy(&payin);
  if(ret){
           sprintf(acErrMsg,"%s,%d ���͵���ӡ������ʧ��!",__FL__);
           strncpy(g_pub_tx.reply,"LS88",4);
           WRITEMSG
           set_zd_data(DC_GET_MSG,"�޷����ͱ��ĵ���ӡ������");
           goto ErrExit;
  }
   /***����ɹ�ֱ�Ӹ���״̬Ϊ����****/
   payin.T_stat[0]=STAT_WZSEND;
   g_reply_int = iLvUpdRecFromPayin(&payin,0);
   if(g_reply_int)
   {
          sprintf( acErrMsg,"�����¼�¼,[%s][%d]", __FILE__,__LINE__);
          strcpy( g_pub_tx.reply, "HV01" );
          WRITEMSG
          goto ErrExit;
   }

OkExit:
     strcpy(g_pub_tx.reply,"0000");
     sprintf(acErrMsg,"�ɹ�![%s]",g_pub_tx.reply);
     WRITEMSG
     get_zd_data("0210",cCur_no);
     vtcp_log("%s,%d cur_no==[%s]",__FL__,cCur_no);
     set_zd_data(DC_REPLY,g_pub_tx.reply);
     return 0;
ErrExit:
     if(memcmp(g_pub_tx.reply,"0000",4)==0)
                memcpy(g_pub_tx.reply,"T063",4);
     sprintf(acErrMsg,"ʧ��![%s]",g_pub_tx.reply);
     WRITEMSG
     set_zd_data(DC_REPLY,g_pub_tx.reply);
     return 1;
}

/******************************************************************************
*                                                                                                   *
*     FUNCTION:             lv11_PreTos()                                                    *
*   DESCRIPTION:        Ϊ���ذ���ֵ                                                         *
******************************************************************************/
ilv11_PreTos()
{
     int iRc;
     /****�����������payout2****/
     iLvSetPayout1(&payin);
     return(0);
}
