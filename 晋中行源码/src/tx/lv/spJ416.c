/***************************************************************************/
/* 文件名称: dh6560031.pc                                                   */
/* 功能描述: 支票/通用截留业务验印发送                                   */
/* 作者    : wyw,2006.11.25
***************************************************************************/
#define EXTERN
#include "public.h"
#include "lv_define.h"
#include "hv_define.h"
#include "cifserver.h"
#include "cifpack.h"
#include "hv_pub.h"
#include "lv_pkgreg_c.h"
#include "lv_add.h"

static PAY_IN_AREA payin;

static ZPJL1_ADD_AREA wd_zp_add1;
static ZPJL2_ADD_AREA wd_zp_add2;
static ZPJL3_ADD_AREA wd_zp_add3;

static TYJL1_ADD_AREA wd_ty_add1;
static TYJL2_ADD_AREA wd_ty_add2;
static TYJL3_ADD_AREA wd_ty_add3;
static TYJL4_ADD_AREA wd_ty_add4;
static TYJL5_ADD_AREA wd_ty_add5;
struct zpjl1_add_area {
        char cpday[9];          /**出票日期****/
        char cpno[13];          /**出票号码****/
        char payno[13];         /**付款行号****/
        char cpactno[33];       /**出票人帐号**/
        char cashactno[33];     /**收款人帐号**/
        char cpamt[13];         /**金额***/
        char use[61];           /**用途***/
        char tsfkday[9];       /**提示付款日期**/
   }wc_zp_add1;
struct zpjl2_add_area {
        char bscnt[3];          /**背书人数,固定为0 ****/
        char bslist[11][60];    /**背书人列表**/
   }wc_zp_add2;       
struct zpjl3_add_area {
        char bscheckpswd[13];/**校验密码 ****/
        char bsbitmaplen[9];/**图像数据长度,为0 ****/
        char bsbitmaplen2[9];/**图像数据长度,为0 ****/
  }wc_zp_add3;
   
struct tyjl1_add_area{
        char cpday[9];          /**出票日期****/
        char cpno[13];          /**出票号码****/
        char payno[13];         /**付款行号****/
        char dlpayno[13];       /**代理付款行号****/
        char cpamt[16];         /**出票金额****/
        char use[61];           /**用途***/
  }wc_ty_add1;
struct tyjl2_add_area{
        char bscnt[3];          /**背书人数,固定为0 ****/
        char bslist[10][60];    /**背书人列表**/
  }wc_ty_add2;
struct tyjl3_add_area{
        char bscheckpswd[13];/**校验密码 ****/
 }wc_ty_add3;
struct tyjl4_add_area{
        char ptype[3];          /**票据种类**/
        char tsday[9];          /**提示付款日期**/
        char mac[21];           /**密押**/
        char stopday[9];        /**到期日**/
        char xyno[21];          /**承兑协议编号**/
        char htno[21];          /**交易合同号码**/
        char cdday[9];          /**承兑日期**/
        char cdname[61];        /**承兑人**/
        char reqname[61];       /**申请人**/
        char reqactno[33];      /**申请人帐号**/
        char paybrname[61];     /**付款人开户银行**/
        char cashbrname[61];    /**收款人开户银行**/
        char cpname[61];        /**出票人全称    **/
        char cpactno[33];       /**出票人帐号    **/
        char paybrname2[61];    /**付款行名称    **/
        char content[61];       /**附言**/
  }wc_ty_add4;
struct tyjl5_add_area{
        char bsbitmaplen[9];/**图像数据长度,为0 ****/
        char bsbitmaplen2[9];/**图像数据长度,为0 ****/
  }wc_ty_add5;
static struct lv_pkgreg_c lv_pkgreg;
/*入口参数*/
char wtday[9];
char orderno[9];
char txnum[6];
char orbrno[13];
char cashactno[33];
char txamt[16];
char acbrno[13];
char payactno[33];

/******************************************************************************
*                                                                             *
*   FUNCTIONS:         dh6560031_Work                                           *
*                                                                             *
*   DESCRIPTION:       1.初始化工作区                                         *
*                      2.执行检查                                             *
*                      3.插入(更新)登记簿                                     *
*                      4.准备输出数据   ?                                    *
******************************************************************************/
int spJ416()
 {
	int ret=0;

	printf("AP MSG: Begin of dh650031_Work\n");
	/*********************/
	/* Initial variables */
	/*********************/
	ret=lv6560031_Initial();
	if (ret) goto ErrExit; 
	
	printf("[%s][%d]\n", __FILE__,__LINE__);
	print_payin(&payin);

	/**********************************/
	/* verify data for basic checking */
	/**********************************/
	ret=lv6560031_Verify();
	if (ret) goto ErrExit; 
	printf("[%s][%d]\n", __FILE__,__LINE__);
	
	/********************************************************/
	/* Process -- 1.Initial PBMR record to opening stauts   */
	/*            2.Insert PBMR record/Delete(reversal)     */
	/********************************************************/
	ret=lv6560031_Process();
	if (ret) goto ErrExit; 
	
	/*******************************************/
	/***sendcif: 发送支票信息到验印服务器   ****/
	/*******************************************/
	ret=lv6560031_SendCif();
	if (ret) goto ErrExit; 
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;	
ErrExit:
	sprintf(acErrMsg,"Before ERROR return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;	
	
  }
/******************************************************************************
*                                                                             *
*   FUNCTION:          a10_Initial()                                          *
*                                                                             *
*   DESCRIPTION:       1.Initial all variables.                               *
******************************************************************************/
lv6560031_Initial()
  {
        /********初始化交易变量******/
	memset(&payin,'\0',sizeof(payin));
        memset(&lv_pkgreg,'\0',sizeof(struct lv_pkgreg_c));
        memset(&wc_zp_add1,'\0',sizeof(wc_zp_add1));
        memset(&wc_zp_add2,'\0',sizeof(wc_zp_add2));
        memset(&wc_zp_add3,'\0',sizeof(wc_zp_add3));
        memset(&wc_ty_add1,'\0',sizeof(wc_ty_add1));
        memset(&wc_ty_add2,'\0',sizeof(wc_ty_add2));
        memset(&wc_ty_add3,'\0',sizeof(wc_ty_add3));
        memset(&wc_ty_add4,'\0',sizeof(wc_ty_add4));
        memset(&wc_ty_add5,'\0',sizeof(wc_ty_add5));

        /**************/
        /* Initial WD */
        /**************/
	memset(&wd_zp_add1,'\0',sizeof(wd_zp_add1));
        memset(&wd_zp_add2,'\0',sizeof(wd_zp_add2));
	memset(&wd_zp_add3,'\0',sizeof(wd_zp_add3));
	memset(&wd_ty_add1,'\0',sizeof(wd_ty_add1));
	memset(&wd_ty_add3,'\0',sizeof(wd_ty_add3));
	memset(&wd_ty_add4,'\0',sizeof(wd_ty_add4));
	memset(&wd_ty_add5,'\0',sizeof(wd_ty_add5));

	memset(wtday,'\0',sizeof(wtday));
	memset(orderno,'\0',sizeof(orderno));
	memset(txnum,'\0',sizeof(txnum));
	memset(orbrno,'\0',sizeof(orbrno));
	memset(cashactno,'\0',sizeof(cashactno));
	memset(txamt,'\0',sizeof(txamt));
	memset(acbrno,'\0',sizeof(acbrno));
	memset(payactno,'\0',sizeof(payactno));
	
	pub_base_sysinit();

	get_zd_data("0440",wtday);	
	get_zd_data("0280",orderno);	
	get_zd_data("0640",txnum);	
	get_zd_data("0520",orbrno);	
	get_zd_data("0810",cashactno);	
	get_zd_data("0390",txamt);	
	get_zd_data("0620",acbrno);	
	get_zd_data("0820",payactno);	
	
	/****填充payin*********/
	printf("[%s][%d]填充payin结构体!\n",__FILE__,__LINE__);
	memcpy(payin.F_wtday,wtday,sizeof(wtday)-1);
	memcpy(payin.F_orderno,orderno,sizeof(orderno)-1);
        memcpy(payin.F_txnum,txnum,sizeof(txnum)-1);
        memcpy(payin.F_orbrno,orbrno,sizeof(orbrno)-1);
        memcpy(payin.F_cashactno,cashactno,sizeof(cashactno)-1);
        memcpy(payin.F_txamt,txamt,sizeof(txamt)-1);
        memcpy(payin.F_acbrno,acbrno,sizeof(acbrno)-1);
       memcpy(payin.F_payactno,payactno,sizeof(payactno)-1);
	memcpy(payin.F_pkgno,PKGNO_PTJJ,sizeof(payin.F_pkgno));
	memcpy(payin.F_opcd,OPCD_LR_QUERY,sizeof(payin.F_opcd));

	iLvPayinAutoInfo(&payin);
	vtcp_log("[%s][%d]payin.F_wtday=[%s] payin.F_payactno[%s],orderno[%s],orbrno[%s]\n",__FILE__,__LINE__,payin.F_wtday,payin.F_payactno,payin.F_orderno,payin.F_orbrno);
	/****取出表中数据****/
	if(iLvFetchRecToPayin(&payin))
      {
	memcpy(payin.F_pkgno,PKGNO_SSJJ,sizeof(payin.F_pkgno));
	if(iLvFetchRecToPayin(&payin))
         {
          vtcp_log("提取录入的数据错误![%s]--[%d]",__FILE__,__LINE__);
	  return(1);
	 }
     }
	return(0);	

  }
/******************************************************************************
*                                                                             *
*   FUNCTION:                            lv6560031_Verify()                        *
*                                                                             *
*   DESCRIPTION:                         1.Initial all variables.             *
******************************************************************************/
lv6560031_Verify()
  {
	int iRc=0;
        char qsx[2];
        memset(qsx,'\0',sizeof(qsx));

        if(memcmp(payin.T_cashactno,payin.F_cashactno,sizeof(payin.F_cashactno)) !=0 || memcmp(payin.T_txamt,payin.F_txamt,sizeof(payin.T_txamt))!=0 || memcmp(payin.T_payactno,payin.F_payactno,sizeof(payin.F_payactno))!=0 || memcmp(payin.T_txnum,payin.F_txnum,sizeof(payin.T_txnum))!=0 || memcmp(payin.T_acbrno,payin.F_acbrno,sizeof(payin.T_acbrno))!=0) 
       {
                vtcp_log("录入的数据和提取数据不一致![%s]--[%d]",__FILE__,__LINE__);
                return(1);
      }
 
      iLvResetPayin(&payin);

	if(payin.T_stat[0]!=STAT_WZLR){
	vtcp_log("只有处于录入状态的交易才允许发送cif报文!");
		return(1);
	}
        iRc=iLvGetAddit(&payin,"QSX",qsx,1);
        if(iRc){
                 vtcp_log("%s,%d,提取发送标志失败!\n",__FILE__,__LINE__);
                 return(1);
         }
      if(qsx[0]=='1')
       {
                vtcp_log("%s,%d,支票信息已经成功发送无需重发!!\n",__FILE__,__LINE__);
                return(1);
       }
            
	return(0);		
  }
/******************************************************************************
*   FUNCTION:          lv6560031_Process()                                         *
*   DESCRIPTION:  1.Initial PBMR record to opening stauts                     *
*                 2.Insert PBMR record/Delete(reversal)                       *
******************************************************************************/
lv6560031_Process()
  {
	int iRc=0;
	if(memcmp(payin.F_txnum,TXNUM_ZPJL,sizeof(payin.F_txnum))==0)
         {
		iRc=iLvGetAddit(&payin,"001",&wd_zp_add1,sizeof(wd_zp_add1));
		if(iRc){
			vtcp_log("%s,%d,提取支票附加域(1)失败\n",__FILE__,__LINE__);
			return(1);
		}
               pad_zpjl1(&wd_zp_add1);
               iRc=iLvGetAddit(&payin,"002",&wd_zp_add2,sizeof(wd_zp_add2));
               if(iRc){
                        vtcp_log("%s,%d,提取支票附加域(2)失败\n",__FILE__,__LINE__);
                        return(1);
               }
	      iRc=iLvGetAddit(&payin,"003",&wd_zp_add3,sizeof(wd_zp_add3));
        	if(iRc){
			vtcp_log("%s,%d,提取支票附加域(3)失败\n",__FILE__,__LINE__);
			return(1);
		}

	}else if(memcmp(payin.F_txnum,TXNUM_TYJL,sizeof(payin.F_txnum))==0)
              {
		iRc=iLvGetAddit(&payin,"001",&wd_ty_add1,sizeof(wd_ty_add1));
		if(iRc){
			vtcp_log("%s,%d,提取通用附加域(1)失败\n",__FILE__,__LINE__);
			return(1);
		}
                iRc=iLvGetAddit(&payin,"002",&wd_zp_add2,sizeof(wd_zp_add2));
                if(iRc){
                        vtcp_log("%s,%d,提取通用附加域(2)失败\n",__FILE__, __LINE__);
                        return(1);
                }
               pad_zpjl2(&wd_zp_add2);
		iRc=iLvGetAddit(&payin,"004",&wd_ty_add4,sizeof(wd_ty_add4));
		if(iRc){
			vtcp_log("%s,%d,提取通用附加域(4)失败\n",__FILE__,__LINE__);
			return(1);
		}
                pad_tyjl1(&wd_ty_add1);
                pad_tyjl4(&wd_ty_add4);

	        iRc=iLvGetAddit(&payin,"003",&wd_ty_add3,sizeof(wd_ty_add3));
		if(iRc){
			vtcp_log("%s,%d,提取通用附加域(3)失败\n",__FILE__,__LINE__);
			return(1);
		}
	        iRc=iLvGetAddit(&payin,"005",&wd_ty_add5,sizeof(wd_ty_add5));
		if(iRc){
			vtcp_log("%s,%d,提取通用附加域(5)失败\n",__FILE__,__LINE__);
			return(1);
		}
               /*** pad_tyjl5(&wd_ty_add5);****/
   iRc=iLvUpdRecFromPayin(&payin,0);
	if(iRc){
		vtcp_log("%s,%d,设置附加域失败\n",__FILE__,__LINE__);
		return(1);
	} 
      }
	return(0);
  }

lv6560031_SendCif()
  {      int iRc;
	char tmp[9];
	char tmp1[61];
       TCIFPACK packcif;
       TCIFPACK *pack=&packcif; /***报文结构****/
       char pcFileName[256];
       char cifip[32];
       char tmpbuf[19];
       int  cifport;
       char tmp_port[32];
       int  iSocketID;

	memset(tmp,'\0',sizeof(tmp));
	memset(tmp1,'\0',sizeof(tmp1));
       memset(tmpbuf,'0',sizeof(tmpbuf));
       memcpy(wc_zp_add2.bscnt,wd_zp_add2.bscnt,sizeof(wd_zp_add2.bscnt));
       memcpy(wc_zp_add2.bslist,wd_zp_add2.bslist,sizeof(wd_zp_add2.bslist));
       memcpy(wc_zp_add1.cpday,wd_zp_add1.cpday,sizeof(wc_zp_add1.cpday)-1);
       memcpy(wc_zp_add1.cpno,wd_zp_add1.cpno+8,sizeof(wc_zp_add1.cpno)-1);
       memcpy(wc_zp_add1.use,wd_zp_add1.use,sizeof(wd_zp_add1.use));
       memcpy(wc_zp_add1.tsfkday,wd_zp_add1.tsfkday,sizeof(wd_zp_add1.tsfkday));
       memcpy(wc_zp_add3.bscheckpswd,wd_zp_add3.bscheckpswd,sizeof(wc_zp_add3.bscheckpswd)-1);
       memcpy(wc_ty_add1.cpday,wd_ty_add1.cpday,8);
       memcpy(wc_ty_add1.cpno,wd_ty_add1.cpno+8,sizeof(wc_ty_add1.cpno)-1);
       memcpy(wc_ty_add1.use,wd_ty_add1.use,sizeof(wd_ty_add1.use));
       memcpy(wc_ty_add4.ptype,wd_ty_add4.ptype,2);
       memcpy(wc_ty_add4.tsday,wd_ty_add4.tsday,8);
       memcpy(wc_ty_add4.mac,wd_ty_add4.mac,20);
       memcpy(wc_ty_add4.stopday,wd_ty_add4.stopday,8);
       memcpy(wc_ty_add4.xyno,wd_ty_add4.xyno,20);
       memcpy(wc_ty_add4.htno,wd_ty_add4.htno,20);
       memcpy(wc_ty_add4.cdday,wd_ty_add4.cdday,8);
       memcpy(wc_ty_add4.cdname,wd_ty_add4.cdname,60);
       char tlrno[17];
       memset(tlrno,' ',sizeof(tlrno));
       tlrno[16]='\0';
	get_zd_data("0070",tlrno);
       /*memcpy(tlrno,TITA.tlrno,sizeof(TITA.tlrno));*/
       /************构造cif联动报文******************/
       vInitCifPack(pack);
       lv_pkgreg.wt_date=apatoi(payin.T_wtday);
	sprintf(tmp,"%ld",lv_pkgreg.wt_date);
       /**memcpy(lv_pkgreg.wt_date,payin.T_wtday,8);**/
       memcpy(lv_pkgreg.orderno,payin.T_orderno,8);
       vAddCifTag(pack,"30A",tmp); /*wt_date*/
       vtcp_log("[%s][%d] %s,%d~~~~~~~~~~~~~~\n",__FILE__,__LINE__,payin.T_txnum,sizeof(payin.T_txnum));
	
	sprintf(tmp1,"%s",wc_zp_add2.bslist);
  if(memcmp(payin.T_txnum,"30102",5)==0){
       /*****支票截留业务报文要素******/
       memcpy(pack->head.cifno,"100",3);
       vAddCifTag(pack,"D35",wc_zp_add1.cpday);
       vAddCifTag(pack,"D01",wc_zp_add1.cpno);
       vAddCifTag(pack,"D37",wc_zp_add1.use);
       vAddCifTag(pack,"D41",wc_zp_add2.bscnt);
       vAddCifTag(pack,"D42",tmp1);
       vAddCifTag(pack,"D30",wc_zp_add3.bscheckpswd); 
       vAddCifTag(pack,"D32","01");
       vAddCifTag(pack,"D36",wc_zp_add1.tsfkday);
  }else{
       /********通用截留业务报文要素********/
       memcpy(pack->head.cifno,"101",3);
       vAddCifTag(pack,"D35",wc_ty_add1.cpday);
       vAddCifTag(pack,"D01",wc_ty_add1.cpno);
	vAddCifTag(pack,"D37",wc_ty_add1.use);
       vAddCifTag(pack,"D41",wc_zp_add2.bscnt);
       vAddCifTag(pack,"D42",tmp1);
       vAddCifTag(pack,"D32",wc_ty_add4.ptype);
       vAddCifTag(pack,"D36",wc_ty_add4.tsday);
       vAddCifTag(pack,"D51",wc_ty_add4.mac);
       vAddCifTag(pack,"D61",wc_ty_add4.stopday);
       vAddCifTag(pack,"D62",wc_ty_add4.xyno);
       vAddCifTag(pack,"D63",wc_ty_add4.htno);
       vAddCifTag(pack,"D64",wc_ty_add4.cdday);
       vAddCifTag(pack,"D66",wc_ty_add4.cdname);
  }
  /*****公共的报文业务要素*********/
	vAddCifTag(pack,"RES",tlrno);
  int tmp_orderno;    
  tmp_orderno=apatoi(payin.T_orderno,sizeof(payin.T_orderno));
  sprintf(lv_pkgreg.orderno,"%08d",tmp_orderno);
  vAddCifTag(pack,"0BC",lv_pkgreg.orderno);
  memcpy(lv_pkgreg.cash_opn_br_no,payin.T_cashno,12); /*收款人开户行*/
  vAddCifTag(pack,"CC5",lv_pkgreg.cash_opn_br_no);
  memcpy(lv_pkgreg.pay_opn_br_no,payin.T_payno,12);
  vAddCifTag(pack,"D21",lv_pkgreg.pay_opn_br_no);
  memcpy(lv_pkgreg.pay_ac_no,payin.T_payactno,32);
  pad_field(lv_pkgreg.pay_ac_no,sizeof(lv_pkgreg.pay_ac_no)-1);
  vAddCifTag(pack,"D23",lv_pkgreg.pay_ac_no);
  memset(tmpbuf,'\0',sizeof(tmpbuf));
  vtcp_log("[%s][%d],%s\n",__FILE__,__LINE__,payin.T_txamt);
  memcpy(tmpbuf,"RMB",3);
  memcpy(tmpbuf+3,payin.T_txamt,15);
  vAddCifTag(pack,"32A",tmpbuf);
  memcpy(lv_pkgreg.pay_name,payin.T_payname,60);
  vAddCifTag(pack,"D22",lv_pkgreg.pay_name);
  memcpy(lv_pkgreg.cash_ac_no,payin.T_cashactno,32);
  pad_field(lv_pkgreg.cash_ac_no,sizeof(lv_pkgreg.cash_ac_no)-1);
  vAddCifTag(pack,"59C",lv_pkgreg.cash_ac_no);
  memcpy(lv_pkgreg.cash_name,payin.T_cashname,60);
  vAddCifTag(pack,"59A",lv_pkgreg.cash_name);    /******付款人名称******/
  memcpy(lv_pkgreg.lv_brf,payin.T_content,60);
  vAddCifTag(pack,"D38",lv_pkgreg.lv_brf);     /******附言****/
  lv_pkgreg.resp_date=apatoi(payin.T_respday);
  sprintf(tmp,"%s",lv_pkgreg.resp_date);
  vAddCifTag(pack,"BS6",tmp);     /***回执期限***/
  vAddCifTag(pack,"TXC","0100");           /***新添加的附加域：交易代码(4)*****/
  vAddCifTag(pack,"DA0","00");             /***核签状态****/
  /**********构造报文完毕***********/

  memset(pcFileName,'\0',sizeof(pcFileName));
  sprintf(pcFileName,"%s/etc/cif.ini",getenv("WORKDIR"));
  memset(cifip,'\0',sizeof(cifip));
  /********读取验印服务器IP和端口********/ 
  iRc=hv_readIni(pcFileName,"CIFSERVER","Address",cifip);
  if(iRc){
	  vtcp_log("%s,%d,读取IP地址错误!\n",__FILE__,__LINE__);
       	  return(1);
  }
  vtcp_log("[%s][%d][%s]\n",__FILE__,__LINE__,cifip);
  memset(tmp_port,'\0',sizeof(tmp_port));
  iRc=hv_readIni(pcFileName,"CIFSERVER","Port",tmp_port);
  if(iRc){
		   vtcp_log("%s,%d,读取端口号错误!\n",__FILE__,__LINE__);
		   return(-1);
  }
  vtcp_log("[%s][%d][%s]\n",__FILE__,__LINE__,tmp_port);
  vtcp_log("[%s][%d][%s],[%d]~~~~~~~~~~~~~~\n",__FILE__,__LINE__,pack,apatoi(pack->head.length,6));
  cifport=apatoi(tmp_port,strlen(tmp_port));

 /*****连接验印服务器*********/
  iSocketID=Cif_connect(cifip,cifport);
  if(iSocketID<=0)
	{
		/*Add by SSH,2005.1.11,为提出文件作准备*/
		char filename[64];
		memset(filename,'\0',sizeof(filename));
		fcopy1(filename,pack,FLAG_SENDER);
		/*Add by SSH,end*/
		vtcp_log("%s,%d,无法连接验服务器!\n",__FILE__,__LINE__);
		return(1);
	}
 /***测试***/
  char pcFilename[30];
  memset(pcFilename,0,sizeof(pcFilename));
  sprintf(pcFilename,"%s/etc/ceshi.dat",getenv("WORKDIR"));
  FILE *fp=NULL;
  fp=fopen(pcFilename,"w");
  fwrite(pack,apatoi(pack->head.length,6),1,fp);
  fclose(fp);
  /***测试***/


   /********发送报文到验印服务器********/
  iRc=Cif_send(iSocketID,pack,apatoi(pack->head.length,sizeof(pack->head.length)));
      if(iRc<0)
	{
		vtcp_log("%s,%d,无法发送报文到验印服务器!\n",__FILE__,__LINE__);
		Cif_close(iSocketID);
		return(1);
	}
  /**********接受验印服务器响应*********/
  iRc=Cif_recv(iSocketID,&pack->head,sizeof(pack->head));
	if(iRc<0)
	{
		vtcp_log("iRc =%d\n",iRc);
		vtcp_log("%s,%d,接收验印服务器响应失败!\n",__FILE__,__LINE__);
		Cif_close(iSocketID);
		return(1) ;
	}
  /*****关闭连接******/
  Cif_close(iSocketID);
  char status[4];
  memset(status,0,4);
  memcpy(status,pack->head.status,3);
  if(memcmp(status,"000",3)==0){
  	
    iRc = iLvSetAddit(&payin,"QSX","1",1);
    if(iRc) {
    	vtcp_log("%s,%d,接收验印复核成功,设置发送标志失败!",__FILE__,__LINE__);
    	return(1);
      }
    }else{
    	vtcp_log("%s,%d,发送报文失败,重新发送支票信息到验印服务器!",__FILE__,__LINE__);
    	return(1);
    }
    
   
  vtcp_log("[%s][%d] 接受验印复核信息,更新记录********************成功[%s]\n",__FILE__,__LINE__,status); 
  /*errLog(LOG_INFO,0,RPT_TO_TTY,&pack->body,sizeof(pack->body),"%s,%d,产生的cif报文:",__FILE__,__LINE__);*/
  if(pub_ins_trace_log()) return(1);

  /****成功发送cif报文正确接受响应*****/
  return(0);
  }
/*由于原fcopy 函数要求的中间参数和传入的参数类型不符,所以增加一个其他类型的fcopy*/
 fcopy1(char *pcFileName,TCIFPACK *pHvPack,int flag)
{
	FILE *fp;
	char chLW[2];
	int length=0;

	
	memset(chLW,'\0',sizeof(chLW));

	if(flag==FLAG_SENDER){
                chLW[0]='W';
        }else{
                chLW[0]='L';
        }
        sprintf(pcFileName,"%s/dat/%.8s_%c%.8s.dat",getenv("WORKDIR"),pHvPack->head.workdate,chLW[0],pHvPack->head.flgno);

        fp=fopen(pcFileName,"wb");
        if(fp==NULL){
                vtcp_log("%s,%d,fcopy error,can not open:[%s] ",__FILE__,__LINE__,pcFileName);
                return(1);
        }
	length=apatoi(pHvPack->head.length,6);
        fwrite(pHvPack->body.buffer,length,1,fp);
        fclose(fp);
        return(0);
} 
