/*************************************************************
文 件 名: spTIPS.c
功能描述: 国库信息系统记账
作    者: xyy
完成日期: 2007-6-21 11:22:13

insert into tx_def values ('6789','国库信息系统记账','10000000000100010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','1','6','2','3','');
insert into tx_flow_def values ('6789','0','TIPS','#=0000@1@#$');
insert into tx_flow_def values ('6789','1','D005','#=0001@2@#=0002@3@#$');
insert into tx_flow_def values ('6789','2','M001','#=0000@4@#$');
insert into tx_flow_def values ('6789','3','D003','#=0000@4@#$');
insert into tx_flow_def values ('6789','4','8606','#=0001@5@#=0002@7@#$');
insert into tx_flow_def values ('6789','5','A017','#=0000@6@#$');
insert into tx_flow_def values ('6789','6','A016','#=0000@7@#$');
insert into tx_flow_def values ('6789','7','HPDF','#$');
*******以下是 承德新加的，注意各地商行的需求调整 2009-6-25 23:31 BY ZHGF*****
insert into tx_sub_def values ('8606','TIPS过渡判断','sp8606','0','0');
insert into tx_flow_def values ('6789','5','A017','#=0000@6@#$');
insert into tx_flow_def values ('6789','6','A016','#$');
insert into tx_sub_rel values('6789','D003','0','0310','1021','账号');
insert into tx_sub_rel values('6789','D003','1','1','1022','账户序号');
insert into tx_sub_rel values('6789','D003','0','0900','1023','凭证类型');
insert into tx_sub_rel values('6789','D003','0','0590','1024','凭证号码');
insert into tx_sub_rel values('6789','D003','0','0390','102F','交易金额');
insert into tx_sub_rel values('6789','D003','0','0210','102J','币种');
insert into tx_sub_rel values('6789','D003','0','0670','102K','现转标志');
insert into tx_sub_rel values('6789','D003','0','0630','102Y','摘要');
*******以上是 承德新加的，注意各地商行的需求调整 2009-6-25 23:31 BY ZHGF*****
insert into tx_sub_def values ('TIPS','国库信息系统记账','spTIPS','0','0'); 

insert into tx_sub_rel values('6789','D005','0','0670','1171','现转标志');

insert into tx_sub_rel values('6789','D003','0','0790','1028','密码');
insert into tx_sub_rel values('6789','D003','0','0690','102G','证件类型');
insert into tx_sub_rel values('6789','D003','0','0620','102A','证件号码');
***RM BY ZHGF BEGIN 20090627**直接在HPDF里从63域取，88域长度不够
insert into tx_sub_rel values('6789','HPDF','0','0630','0880','摘要');
***RM BY ZHGF END 20090627**
insert into tx_sub_rel values('6789','A016','0','0330','1201','账号');
insert into tx_sub_rel values('6789','A016','1','01','1204','币种');
insert into tx_sub_rel values('6789','A016','1','2','1205','转账');
insert into tx_sub_rel values('6789','A016','0','0390','1208','金额');
insert into tx_sub_rel values('6789','A016','0','0630','120A','摘要');
insert into tx_sub_rel values('6789','A017','0','0330','1211','账号');
insert into tx_sub_rel values('6789','A017','1','01','1214','币种');
insert into tx_sub_rel values('6789','A017','1','2','1215','转账');
insert into tx_sub_rel values('6789','A017','0','0390','1218','金额');
insert into tx_sub_rel values('6789','A017','1','TIPS代收','121A','摘要');


insert into tx_sub_rel values('6789','HPDF','1','6','0680','账户类型');
insert into tx_sub_rel values('6789','HPDF','1','2','0660','借贷标志');
insert into tx_sub_rel values('6789','HPDF','1','2','0720','借贷标志');
insert into tx_sub_rel values('6789','HPDF','0','0320','0300','账号');
insert into tx_sub_rel values('6789','HPDF','0','0390','0400','交易金额');

insert into tx_sub_rel values('6789','M001','0','0210','1181','币种');       
insert into tx_sub_rel values('6789','M001','0','0390','1184','发生额');       
insert into tx_sub_rel values('6789','M001','0','0630','1187','摘要'); 
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "in_mst_c.h"
#include "com_parm_c.h"
#define   PARM_CODE  "TIPS"
#define   BR_PARM_SEQ  9999

spTIPS()
{
    int  ret =0;
    char cNoteType[4];/**凭证类型**/
    char cNoteNo  [17]; /**凭证号码**/
    char cActno1  [25]; /**挂账账号**/
    char cActno3  [25]; /**转帐方账号**/
    char cXzFlag  [2];  /**现转标志**/
    char cAc_type[2];
    char cCode[6];
    char cTmpXz[2];
    char cCz_flag[2],cOjz_traceno[9],cBrf[21];  /**冲正标志**/

    double dTxamt=0.00;
    double dNoteNo=0.00;
    long   lOjz_date=0;
    
    struct  com_parm_c     com_parm;
    struct  mdm_ac_rel_c   mdm_ac_rel;
    struct  in_mst_c       in_mst;

    
    memset(cNoteType, 0 , sizeof(cNoteType));
    memset(cNoteNo  , 0 , sizeof(cNoteNo  ));
    memset(cActno1  , 0 , sizeof(cActno1  ));
    memset(cActno3  , 0 , sizeof(cActno3  ));
    memset(cXzFlag  , 0 , sizeof(cXzFlag  ));
    memset(cAc_type , 0 , sizeof(cAc_type ));
    memset(cCz_flag , 0 , sizeof(cCz_flag ));
    memset(cBrf     , 0 , sizeof(cBrf     ));
    memset(cCode    , 0 , sizeof(cCode    ));
    
    memset(&com_parm    , 0 , sizeof(com_parm));
    memset(&in_mst      , 0 , sizeof(in_mst));
    memset(&mdm_ac_rel  , 0 , sizeof(mdm_ac_rel));

    memset(cOjz_traceno , 0 , sizeof(cOjz_traceno));
    
    /*** 初始化全局变量 ***/
    pub_base_sysinit();

    get_zd_double("0390", &dTxamt);
    get_zd_data("0310",cActno1);
    get_zd_data("0630",cBrf);
    /**dTxamt = dTxamt *100;*RM BY ZHGF */
    {/**冲正交易用到的信息**ADD BY ZHGF  **/
	get_zd_data("0670", cCz_flag);/**冲正标志*/
	get_zd_data("0780", cOjz_traceno);/**原交易流水**/
	get_zd_long("0450", &lOjz_date);/**原交易日期**/
	vtcp_log("%s,%d cCz_flag=[%s] cOjz_traceno=[%s] lOjz_date=[%d]\n",__FILE__,__LINE__,cCz_flag,cOjz_traceno,lOjz_date);
	if(cCz_flag[0]=='3'){/*冲正*/
		dTxamt=-1*dTxamt;
		vtcp_log("[%s][%d] 39域=[%.2f] \n",__FILE__,__LINE__,dTxamt);
		set_zd_double("0390",dTxamt);
	}
    }
    vtcp_log("[%s][%d] 63域=[%s] \n",__FILE__,__LINE__,cBrf);
    zip_space(cActno1);
    
    vtcp_log("[%s][%d] 39域=[%.2f] \n",__FILE__,__LINE__,dTxamt);
    vtcp_log("[%s][%d] 33域=[%s] \n",__FILE__,__LINE__,cActno1);

    /**if(pub_base_CompDblVal(dTxamt,0.00)<=0)非冲正时MODIFY BY ZHGF 20090627**/
    if(pub_base_CompDblVal(dTxamt,0.00)<=0 && cCz_flag[0]!='3')
    {
         vtcp_log("[%s][%d] 交易金额为=[%.2f]\n",__FILE__,__LINE__,dTxamt);
         sprintf(acErrMsg,"输入的交易金额错误=[%s]",dTxamt);
         WRITEMSG
         strcpy (g_pub_tx.reply,"D128");
         goto ErrExit;
    }

    /***if(memcmp(cActno1,"10101",sizeof(cActno1)-1)==0)*现金*RM BY ZHGF 0624*/
    if(memcmp(cActno1,"10101",strlen(cActno1))==0)/*现金*/
    {
	      vtcp_log("%s,%d cTmpXz[%s]\n",__FILE__,__LINE__,cTmpXz);
        cTmpXz[0] ='1';
	      vtcp_log("%s,%d cTmpXz[%s]\n",__FILE__,__LINE__,cTmpXz);
    }
    else /*转账*/
    {
        cTmpXz[0] ='2';
    }
    /**账户头两位:18 19 58 59 68 69 属于村镇银行账户 add by zyl 20110716**/
    if( (cActno1[0]=='1' || cActno1[0]=='5' || cActno1[0]=='9') && 
         ( cActno1[1]=='9' || cActno1[1]=='8' || cActno1[1]=='7' || cActno1[1]=='6' ||
           cActno1[1]=='5' || cActno1[1]=='4' || cActno1[1]=='3' || cActno1[1]=='2' ) )
    {
        memcpy(cCode,"TIPS",4);
	cCode[4]=cActno1[1];
	vtcp_log("%s,%d cCode[%s]\n",__FILE__,__LINE__,cCode);
	
        /**此时cActno1属于村镇银行的账号...属于村镇扣税业务.此时,商行也要扣款.**/
        /**暂只处理融信.融信村镇财税库银账号 TIPRX**/
        /* 这里按照TIPS+村镇帐号第二位作为 parm_code 20110725 */
        ret = Com_parm_Sel(g_pub_tx.reply,&com_parm,"parm_code='%s' and parm_seqn=1",cCode);
        if(ret)
        {
            sprintf(acErrMsg,"[%s][%d]取村镇财税库银账号错误! ret=[%d]cCode=[%s]",__FILE__,__LINE__,ret,cCode);
            strcpy(g_pub_tx.reply,"W025");
            WRITEMSG
            goto ErrExit;
        }
        strcpy(cActno1,com_parm.val);
        cTmpXz[0] ='2';
        memset(&com_parm,0x00,sizeof(com_parm));
    }
    /**end by zyl 20110716**/
    if(strlen(cActno1)>7)
    {
        pub_base_old_acno(cActno1);
        ret = iCheckActno(cActno1,cAc_type);
        if(ret)
        {
            sprintf(acErrMsg,"检查账户错误![%s]",cActno1);
            WRITEMSG
            goto ErrExit;
        }
    }
    vtcp_log("[%s][%d] 账户=[%s] 账户类型=[%s]\n",__FILE__,__LINE__,cActno1,cAc_type);
   
    /**检查签发行的兑付帐号**/
    ret = Com_parm_Sel(g_pub_tx.reply,&com_parm,"parm_code='%s' and parm_seqn=1",PARM_CODE);
    if(ret)
    {
        sprintf(acErrMsg,"[%s][%d]承兑付款临时帐户错误!cnt=[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"W025");
        WRITEMSG
        goto ErrExit;
    }
    /** 检查本票签发户是否存在 **/
    ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",com_parm.val);
    if(ret)
    {
        sprintf(acErrMsg,"[%s][%d]检查本票签发户失败!sqlcode=[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"B117");
        WRITEMSG
        goto ErrExit;
    }
    ret = In_mst_Sel(g_pub_tx.reply,&in_mst,"ac_id='%d'",mdm_ac_rel.ac_id);
    if(ret)
    {
        sprintf(acErrMsg,"[%s][%d]本票签发户错误!sqlcode=[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"B117");
        WRITEMSG
        goto ErrExit;
    }
    if( in_mst.sts[0]!='1' && in_mst.sts[0]!='2' )
    {
        sprintf(acErrMsg,"账号状态非正常状态![%ld,%s]",in_mst.ac_id,in_mst.sts);
        strcpy(g_pub_tx.reply,"P109");
        WRITEMSG
        goto ErrExit;
    }
    set_zd_data("0310",cActno1);/**账    号**/
    set_zd_data("0320",com_parm.val);/**对方账号*/
    set_zd_data("0210", "01");  /**币    种**/
    vtcp_log("%s,%d cTmpXz=[%s]\n",__FILE__,__LINE__,cTmpXz);
    set_zd_data("0670",cTmpXz);/**现转标志**/
    set_zd_double("0390",dTxamt);
	   /**支行的时候**/
    if(memcmp(g_pub_tx.tx_br_no,"10001",4))
    {
	      /**开始根据机构查要过渡的258科目对应的账号**ADD BY ZHGF 20090625*/
	      /*******char cTxbrno[6],cGd_ac_no[25];
	      memset(cTxbrno,0x00,sizeof(cTxbrno));
	      memset(cGd_ac_no,0x00,sizeof(cGd_ac_no));
	      get_zd_data("0030",cTxbrno);
	      vtcp_log("%s,%d cTxbrno[%s],cGd_ac_no[%s]\n",cTxbrno,cGd_ac_no);
	      ret = pub_base_GetParm_Str(cTxbrno,BR_PARM_SEQ,cGd_ac_no);
	      if (ret)
	      {
	      	sprintf(acErrMsg, "没有该机构的大小额专用内部帐户");
	      	WRITEMSG
	      	       strcpy(g_pub_tx.reply, "O043");
	      	goto ErrExit;
	      }
	      vtcp_log("%s,%d cTxbrno[%s],cGd_ac_no[%s]\n",__FILE__,__LINE__,cTxbrno,cGd_ac_no);
	      pub_base_strpack(cGd_ac_no);
	      vtcp_log("%s,%d cTxbrno[%s],cGd_ac_no[%s]\n",__FILE__,__LINE__,cTxbrno,cGd_ac_no);
	      ***********RM XY XJ 20090824 全行用一个账户**/
	      
	      set_zd_data("0330",com_parm.val);
    }
  
	vtcp_log("[%s][%d] 准备进入记账部分 [%f]\n",__FILE__,__LINE__,dTxamt);

OKExit:
    sprintf(acErrMsg,"TIPS 程序执行完毕,准备记账![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
ErrExit:
    if(memcmp(g_pub_tx.reply,"0000",4)==0)
        strcpy(g_pub_tx.reply,"P156");
    sprintf(acErrMsg,"国库系统记账错误![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
}
int iCheckActno(char *ac_no,char *ac_type)
{
    int iRet=0;
    char cAcType[2];
    struct mdm_ac_rel_c sMdm_ac_rel;
    
    memset(cAcType     ,0,sizeof(cAcType));
    memset(&sMdm_ac_rel,0,sizeof(sMdm_ac_rel));
    iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel,"ac_no ='%s'", ac_no);
    if ( iRet == 100 )
    {
        sprintf(acErrMsg,"介质账户对照表中不存在该记录[%d]",iRet);
        WRITEMSG
        strcpy (g_pub_tx.reply,"W010");
        return 1;
    }
    else if ( iRet )
    {
        sprintf(acErrMsg,"SELECT mdm_ac_rel error,error code=[%d]",iRet);
        WRITEMSG
        strcpy (g_pub_tx.reply,"W011");
        return 1;
    }
    vtcp_log("[%s][%d]开始检查是否有此账户[%s][%d]\n",__FILE__,__LINE__,sMdm_ac_rel.ac_no,sMdm_ac_rel.ac_seqn);
    if(sMdm_ac_rel.ac_seqn==9999)
    {
        sMdm_ac_rel.ac_seqn=1;
    }
    iRet = pub_base_disac_file(sMdm_ac_rel.ac_no,sMdm_ac_rel.ac_seqn,cAcType);
    if(iRet)
    {
        sprintf(acErrMsg,"检查账户失败=[%d]",iRet);
        WRITEMSG
        return 1;
    }
    memcpy(ac_type,cAcType,sizeof(cAcType)-1);
    vtcp_log("[%s][%d] 账户类型为=[%s]\n",__FILE__,__LINE__,ac_type);
    return 0;
}
