/*************************************************************
�� �� ��: spTIPS.c
��������: ������Ϣϵͳ����
��    ��: xyy
�������: 2007-6-21 11:22:13

insert into tx_def values ('6789','������Ϣϵͳ����','10000000000100010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','1','6','2','3','');
insert into tx_flow_def values ('6789','0','TIPS','#=0000@1@#$');
insert into tx_flow_def values ('6789','1','D005','#=0001@2@#=0002@3@#$');
insert into tx_flow_def values ('6789','2','M001','#=0000@4@#$');
insert into tx_flow_def values ('6789','3','D003','#=0000@4@#$');
insert into tx_flow_def values ('6789','4','8606','#=0001@5@#=0002@7@#$');
insert into tx_flow_def values ('6789','5','A017','#=0000@6@#$');
insert into tx_flow_def values ('6789','6','A016','#=0000@7@#$');
insert into tx_flow_def values ('6789','7','HPDF','#$');
*******������ �е��¼ӵģ�ע��������е�������� 2009-6-25 23:31 BY ZHGF*****
insert into tx_sub_def values ('8606','TIPS�����ж�','sp8606','0','0');
insert into tx_flow_def values ('6789','5','A017','#=0000@6@#$');
insert into tx_flow_def values ('6789','6','A016','#$');
insert into tx_sub_rel values('6789','D003','0','0310','1021','�˺�');
insert into tx_sub_rel values('6789','D003','1','1','1022','�˻����');
insert into tx_sub_rel values('6789','D003','0','0900','1023','ƾ֤����');
insert into tx_sub_rel values('6789','D003','0','0590','1024','ƾ֤����');
insert into tx_sub_rel values('6789','D003','0','0390','102F','���׽��');
insert into tx_sub_rel values('6789','D003','0','0210','102J','����');
insert into tx_sub_rel values('6789','D003','0','0670','102K','��ת��־');
insert into tx_sub_rel values('6789','D003','0','0630','102Y','ժҪ');
*******������ �е��¼ӵģ�ע��������е�������� 2009-6-25 23:31 BY ZHGF*****
insert into tx_sub_def values ('TIPS','������Ϣϵͳ����','spTIPS','0','0'); 

insert into tx_sub_rel values('6789','D005','0','0670','1171','��ת��־');

insert into tx_sub_rel values('6789','D003','0','0790','1028','����');
insert into tx_sub_rel values('6789','D003','0','0690','102G','֤������');
insert into tx_sub_rel values('6789','D003','0','0620','102A','֤������');
***RM BY ZHGF BEGIN 20090627**ֱ����HPDF���63��ȡ��88�򳤶Ȳ���
insert into tx_sub_rel values('6789','HPDF','0','0630','0880','ժҪ');
***RM BY ZHGF END 20090627**
insert into tx_sub_rel values('6789','A016','0','0330','1201','�˺�');
insert into tx_sub_rel values('6789','A016','1','01','1204','����');
insert into tx_sub_rel values('6789','A016','1','2','1205','ת��');
insert into tx_sub_rel values('6789','A016','0','0390','1208','���');
insert into tx_sub_rel values('6789','A016','0','0630','120A','ժҪ');
insert into tx_sub_rel values('6789','A017','0','0330','1211','�˺�');
insert into tx_sub_rel values('6789','A017','1','01','1214','����');
insert into tx_sub_rel values('6789','A017','1','2','1215','ת��');
insert into tx_sub_rel values('6789','A017','0','0390','1218','���');
insert into tx_sub_rel values('6789','A017','1','TIPS����','121A','ժҪ');


insert into tx_sub_rel values('6789','HPDF','1','6','0680','�˻�����');
insert into tx_sub_rel values('6789','HPDF','1','2','0660','�����־');
insert into tx_sub_rel values('6789','HPDF','1','2','0720','�����־');
insert into tx_sub_rel values('6789','HPDF','0','0320','0300','�˺�');
insert into tx_sub_rel values('6789','HPDF','0','0390','0400','���׽��');

insert into tx_sub_rel values('6789','M001','0','0210','1181','����');       
insert into tx_sub_rel values('6789','M001','0','0390','1184','������');       
insert into tx_sub_rel values('6789','M001','0','0630','1187','ժҪ'); 
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
    char cNoteType[4];/**ƾ֤����**/
    char cNoteNo  [17]; /**ƾ֤����**/
    char cActno1  [25]; /**�����˺�**/
    char cActno3  [25]; /**ת�ʷ��˺�**/
    char cXzFlag  [2];  /**��ת��־**/
    char cAc_type[2];
    char cCode[6];
    char cTmpXz[2];
    char cCz_flag[2],cOjz_traceno[9],cBrf[21];  /**������־**/

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
    
    /*** ��ʼ��ȫ�ֱ��� ***/
    pub_base_sysinit();

    get_zd_double("0390", &dTxamt);
    get_zd_data("0310",cActno1);
    get_zd_data("0630",cBrf);
    /**dTxamt = dTxamt *100;*RM BY ZHGF */
    {/**���������õ�����Ϣ**ADD BY ZHGF  **/
	get_zd_data("0670", cCz_flag);/**������־*/
	get_zd_data("0780", cOjz_traceno);/**ԭ������ˮ**/
	get_zd_long("0450", &lOjz_date);/**ԭ��������**/
	vtcp_log("%s,%d cCz_flag=[%s] cOjz_traceno=[%s] lOjz_date=[%d]\n",__FILE__,__LINE__,cCz_flag,cOjz_traceno,lOjz_date);
	if(cCz_flag[0]=='3'){/*����*/
		dTxamt=-1*dTxamt;
		vtcp_log("[%s][%d] 39��=[%.2f] \n",__FILE__,__LINE__,dTxamt);
		set_zd_double("0390",dTxamt);
	}
    }
    vtcp_log("[%s][%d] 63��=[%s] \n",__FILE__,__LINE__,cBrf);
    zip_space(cActno1);
    
    vtcp_log("[%s][%d] 39��=[%.2f] \n",__FILE__,__LINE__,dTxamt);
    vtcp_log("[%s][%d] 33��=[%s] \n",__FILE__,__LINE__,cActno1);

    /**if(pub_base_CompDblVal(dTxamt,0.00)<=0)�ǳ���ʱMODIFY BY ZHGF 20090627**/
    if(pub_base_CompDblVal(dTxamt,0.00)<=0 && cCz_flag[0]!='3')
    {
         vtcp_log("[%s][%d] ���׽��Ϊ=[%.2f]\n",__FILE__,__LINE__,dTxamt);
         sprintf(acErrMsg,"����Ľ��׽�����=[%s]",dTxamt);
         WRITEMSG
         strcpy (g_pub_tx.reply,"D128");
         goto ErrExit;
    }

    /***if(memcmp(cActno1,"10101",sizeof(cActno1)-1)==0)*�ֽ�*RM BY ZHGF 0624*/
    if(memcmp(cActno1,"10101",strlen(cActno1))==0)/*�ֽ�*/
    {
	      vtcp_log("%s,%d cTmpXz[%s]\n",__FILE__,__LINE__,cTmpXz);
        cTmpXz[0] ='1';
	      vtcp_log("%s,%d cTmpXz[%s]\n",__FILE__,__LINE__,cTmpXz);
    }
    else /*ת��*/
    {
        cTmpXz[0] ='2';
    }
    /**�˻�ͷ��λ:18 19 58 59 68 69 ���ڴ��������˻� add by zyl 20110716**/
    if( (cActno1[0]=='1' || cActno1[0]=='5' || cActno1[0]=='9') && 
         ( cActno1[1]=='9' || cActno1[1]=='8' || cActno1[1]=='7' || cActno1[1]=='6' ||
           cActno1[1]=='5' || cActno1[1]=='4' || cActno1[1]=='3' || cActno1[1]=='2' ) )
    {
        memcpy(cCode,"TIPS",4);
	cCode[4]=cActno1[1];
	vtcp_log("%s,%d cCode[%s]\n",__FILE__,__LINE__,cCode);
	
        /**��ʱcActno1���ڴ������е��˺�...���ڴ����˰ҵ��.��ʱ,����ҲҪ�ۿ�.**/
        /**��ֻ��������.���Ŵ����˰�����˺� TIPRX**/
        /* ���ﰴ��TIPS+�����ʺŵڶ�λ��Ϊ parm_code 20110725 */
        ret = Com_parm_Sel(g_pub_tx.reply,&com_parm,"parm_code='%s' and parm_seqn=1",cCode);
        if(ret)
        {
            sprintf(acErrMsg,"[%s][%d]ȡ�����˰�����˺Ŵ���! ret=[%d]cCode=[%s]",__FILE__,__LINE__,ret,cCode);
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
            sprintf(acErrMsg,"����˻�����![%s]",cActno1);
            WRITEMSG
            goto ErrExit;
        }
    }
    vtcp_log("[%s][%d] �˻�=[%s] �˻�����=[%s]\n",__FILE__,__LINE__,cActno1,cAc_type);
   
    /**���ǩ���еĶҸ��ʺ�**/
    ret = Com_parm_Sel(g_pub_tx.reply,&com_parm,"parm_code='%s' and parm_seqn=1",PARM_CODE);
    if(ret)
    {
        sprintf(acErrMsg,"[%s][%d]�жҸ�����ʱ�ʻ�����!cnt=[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"W025");
        WRITEMSG
        goto ErrExit;
    }
    /** ��鱾Ʊǩ�����Ƿ���� **/
    ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",com_parm.val);
    if(ret)
    {
        sprintf(acErrMsg,"[%s][%d]��鱾Ʊǩ����ʧ��!sqlcode=[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"B117");
        WRITEMSG
        goto ErrExit;
    }
    ret = In_mst_Sel(g_pub_tx.reply,&in_mst,"ac_id='%d'",mdm_ac_rel.ac_id);
    if(ret)
    {
        sprintf(acErrMsg,"[%s][%d]��Ʊǩ��������!sqlcode=[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"B117");
        WRITEMSG
        goto ErrExit;
    }
    if( in_mst.sts[0]!='1' && in_mst.sts[0]!='2' )
    {
        sprintf(acErrMsg,"�˺�״̬������״̬![%ld,%s]",in_mst.ac_id,in_mst.sts);
        strcpy(g_pub_tx.reply,"P109");
        WRITEMSG
        goto ErrExit;
    }
    set_zd_data("0310",cActno1);/**��    ��**/
    set_zd_data("0320",com_parm.val);/**�Է��˺�*/
    set_zd_data("0210", "01");  /**��    ��**/
    vtcp_log("%s,%d cTmpXz=[%s]\n",__FILE__,__LINE__,cTmpXz);
    set_zd_data("0670",cTmpXz);/**��ת��־**/
    set_zd_double("0390",dTxamt);
	   /**֧�е�ʱ��**/
    if(memcmp(g_pub_tx.tx_br_no,"10001",4))
    {
	      /**��ʼ���ݻ�����Ҫ���ɵ�258��Ŀ��Ӧ���˺�**ADD BY ZHGF 20090625*/
	      /*******char cTxbrno[6],cGd_ac_no[25];
	      memset(cTxbrno,0x00,sizeof(cTxbrno));
	      memset(cGd_ac_no,0x00,sizeof(cGd_ac_no));
	      get_zd_data("0030",cTxbrno);
	      vtcp_log("%s,%d cTxbrno[%s],cGd_ac_no[%s]\n",cTxbrno,cGd_ac_no);
	      ret = pub_base_GetParm_Str(cTxbrno,BR_PARM_SEQ,cGd_ac_no);
	      if (ret)
	      {
	      	sprintf(acErrMsg, "û�иû����Ĵ�С��ר���ڲ��ʻ�");
	      	WRITEMSG
	      	       strcpy(g_pub_tx.reply, "O043");
	      	goto ErrExit;
	      }
	      vtcp_log("%s,%d cTxbrno[%s],cGd_ac_no[%s]\n",__FILE__,__LINE__,cTxbrno,cGd_ac_no);
	      pub_base_strpack(cGd_ac_no);
	      vtcp_log("%s,%d cTxbrno[%s],cGd_ac_no[%s]\n",__FILE__,__LINE__,cTxbrno,cGd_ac_no);
	      ***********RM XY XJ 20090824 ȫ����һ���˻�**/
	      
	      set_zd_data("0330",com_parm.val);
    }
  
	vtcp_log("[%s][%d] ׼��������˲��� [%f]\n",__FILE__,__LINE__,dTxamt);

OKExit:
    sprintf(acErrMsg,"TIPS ����ִ�����,׼������![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
ErrExit:
    if(memcmp(g_pub_tx.reply,"0000",4)==0)
        strcpy(g_pub_tx.reply,"P156");
    sprintf(acErrMsg,"����ϵͳ���˴���![%s]",g_pub_tx.reply);
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
        sprintf(acErrMsg,"�����˻����ձ��в����ڸü�¼[%d]",iRet);
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
    vtcp_log("[%s][%d]��ʼ����Ƿ��д��˻�[%s][%d]\n",__FILE__,__LINE__,sMdm_ac_rel.ac_no,sMdm_ac_rel.ac_seqn);
    if(sMdm_ac_rel.ac_seqn==9999)
    {
        sMdm_ac_rel.ac_seqn=1;
    }
    iRet = pub_base_disac_file(sMdm_ac_rel.ac_no,sMdm_ac_rel.ac_seqn,cAcType);
    if(iRet)
    {
        sprintf(acErrMsg,"����˻�ʧ��=[%d]",iRet);
        WRITEMSG
        return 1;
    }
    memcpy(ac_type,cAcType,sizeof(cAcType)-1);
    vtcp_log("[%s][%d] �˻�����Ϊ=[%s]\n",__FILE__,__LINE__,ac_type);
    return 0;
}
