/*********************************************************************
* �� �� �� :  V1.0        
* �������� :  CHENGBO          
* ��    �� :  2012-07-07   
* ����ģ�� :  ��ƽ̨�Ľӿڳ��� (�ڶ���¼���˴���)           
* �������� :  spF059.c
* �������� :  ����ƽ̨���͵ĵڶ���¼���ˣ����ж�����¼ʱҪѭ������
* �����б� :       
*         1��pub_acct_trance();
*         2��pubf_acct_proc();                              
* ʹ��ע�� :                                       
* �޸ļ�¼ :                                       
*     �޸����� :                       
*     �޸�ʱ�� :                                  
*     �޸����� :
insert into tx_sub_def   values ('F059', '�ڶ���¼����', 'spF059', '0', '0');
******************************************************************/
#include <stdio.h>
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "in_mst_c.h"
#include "com_item_c.h"
#include "prdt_ac_id_c.h"
#include "chnl_single_cnaps_c.h"
#include "chnl_single_acct_c.h"

int spF059()
{
    int    iRet=0;
    int    i=0;            /*����ѭ��*/
    double dSum=0.00;      /*���ʱʹ��*/
    char   cDc_ind[2];     /*�����־*/
    char   cFd95[601];     /*����95������*/
    char   cFd81[601];     /*����81������*/
    char   cAc_no[25];     /*�ڶ���¼�˺�*/
    char   cCt_ind[2];     /*��ת��־*/
    char   cTx_amt[17];    /*���׽��*/
    char   cTx_brf[11];    /*��������*/
    char   cCount[3];      /*��¼����*/
    int    iCount=0;       /*��¼����*/
    double dTx_amt=0.00;   /*���׽��*/
    char   sOpn_brno[8]={0}; /*���˺ŵĿ�������*/
    char   sOpn_flag[2]={0};/*���˻�����־ 1���ǽ��׻�����2�������˺ſ�������*/   
    
    struct mdm_ac_rel_c  sMdm_ac_rel;
    struct dd_mst_c      sDd_mst;
    struct in_mst_c      sIn_mst;
    struct com_item_c    sCom_item;
    struct chnl_single_cnaps_c stChnlCnaps;
    struct chnl_single_acct_c stChnlAcct; 
    struct prdt_ac_id_c  sPrdt_ac_id;

    memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
    memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
    memset(&sCom_item,0x00,sizeof(struct com_item_c));
    memset(&stChnlCnaps,0x00,sizeof(struct chnl_single_cnaps_c));
    memset(&stChnlAcct,0x00,sizeof(struct chnl_single_acct_c));  /*zhengyq 20140811 add to sxps*/
    memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
    memset(cCount,0x00,sizeof(cCount));
    memset(cFd95,0x00,sizeof(cFd95));
    memset(cFd81,0x00,sizeof(cFd81));
    memset(cAc_no,0x00,sizeof(cAc_no));
    memset(cTx_amt,0x00,sizeof(cTx_amt));
    memset(cTx_brf,0x00,sizeof(cTx_brf));
    memset(cDc_ind,0x00,sizeof(cDc_ind));
    memset(cCt_ind,0x00,sizeof(cCt_ind));

    pub_base_sysinit();
    
    get_fd_data("0810",cFd81);
    get_fd_data("0950",cFd95);
    if( strlen(cFd95)==0 || memcmp(cFd95,"END",3) == 0 )
    {
        vtcp_log("[%s][%d],û�еڶ���¼ֱ�ӷ��سɹ�!!!",__FILE__,__LINE__);
        goto OkExit;
    }
    /*�õ���¼����*/
    memcpy(cCount,cFd95,2); 
    iCount=atoi(cCount);
	/*add by lw 20120719*/
	if(iCount == 0)
	{
		vtcp_log("[%s][%d],û�еڶ���¼ֱ�ӷ��سɹ�!!!",__FILE__,__LINE__);
		goto OkExit;
	}
	vtcp_log("[%s][%d]��¼����=%d",__FILE__,__LINE__,iCount);
    /*�õ���ת��־*/
    memcpy(cCt_ind,cFd95+2,1);
    for( i=0; i < iCount; i++ )
    {
        memset(cAc_no,0x00,sizeof(cAc_no));
        memset(cTx_amt,0x00,sizeof(cTx_amt));
        memset(cDc_ind,0x00,sizeof(cDc_ind));
        memset(sOpn_brno,0x00,sizeof(sOpn_brno));
        
        memcpy(sOpn_brno,cFd95 + 3 + 46 * i,5);    /*��������*/
        memcpy(cAc_no,cFd95 + 8 + 46 * i,24);      /*�˺Ż��Ŀ��*/
        memcpy(cDc_ind,cFd95 + 32 + 46 * i,1);     /*�����־*/
        memcpy(cTx_amt,cFd95 + 33 + 46 * i,16);    /*���׽��*/
        
        dTx_amt=atof(cTx_amt) / 100;               /*���׽��תΪ����*/
        
        if(cDc_ind[0] == '1')
        {
            dTx_amt= -1 * dTx_amt;
        }
        dSum += dTx_amt;
    }
    if( pub_base_CompDblVal(dSum,0.00) !=0 )
    {
        sprintf(acErrMsg, "�������Ľ�����ݲ�ƽ�⣡");
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }
    
    i=0;
    for( i=0; i < iCount; i++ )
    {
        memset(cAc_no,0x00,sizeof(cAc_no));
        memset(cTx_amt,0x00,sizeof(cTx_amt));
        memset(cDc_ind,0x00,sizeof(cDc_ind));
        memset(sOpn_brno,0x00,sizeof(sOpn_brno));
        
        memcpy(sOpn_brno,cFd95 + 3 + 46 * i,5);    /*�����������׻���*/
        memcpy(cAc_no,cFd95 + 8 + 46 * i,24);      /*�˺Ż��Ŀ��*/
        memcpy(cDc_ind,cFd95 + 32 + 46 * i,1);     /*�����־*/
        memcpy(cTx_amt,cFd95 + 33 + 46 * i,16);    /*���׽��*/
        
        dTx_amt=atof(cTx_amt) / 100;               /*���׽��תΪ����*/
        vtcp_log("%s,%d,ac_no=[%s]�˺Ż��Ŀ��!",__FILE__,__LINE__,cAc_no);
        vtcp_log("%s,%d,cDc_ind=[%s]�����־!",__FILE__,__LINE__,cDc_ind);
        if(memcmp(g_pub_tx.tx_code,"6235",4)==0 || memcmp(g_pub_tx.tx_code,"7023",4)==0 || memcmp(g_pub_tx.tx_code,"6254",4)==0 )
        {   /*������Ǹ���*/
        	dTx_amt= -1 * dTx_amt;
        	get_zd_data("0100",stChnlCnaps.bbmk);
		if (memcmp(stChnlCnaps.bbmk,"sxps",4) == 0)
		{     
			 /*zhengyq 20140811 add to sxps  begin*/
			/**add by lw 20120719 ����Ƿ���ԭ����,���ü�齻��״̬��Ϊ��6235��7023�����Ѿ�����**/
			get_zd_long("0450",&stChnlAcct.pt_date);
			long lPt_old_trace=0;
			get_zd_long("0780",&lPt_old_trace);
			sprintf(stChnlAcct.pt_trace_no,"%ld",lPt_old_trace);
			sprintf(stChnlAcct.bbmk,"%s",stChnlCnaps.bbmk);
			iRet=Chnl_single_acct_Sel(g_pub_tx.reply,&stChnlAcct,"pt_date=%ld and pt_trace_no='%s' and bbmk='%s'",stChnlAcct.pt_date,stChnlAcct.pt_trace_no,stChnlAcct.bbmk);
			if(iRet)
			{
				strcpy(g_pub_tx.reply, "D103");
				sprintf(acErrMsg,"��ѯԭ���׳���");
				WRITEMSG
				goto ErrExit;
			}
			vtcp_log("[%s][%d],g_pub_tx.tx_code[%s] stChnlAcct.sts=[%s]",__FILE__,__LINE__,g_pub_tx.tx_code,stChnlAcct.sts);
			
		}
		else
		{
			/**add by lw 20120719 ����Ƿ���ԭ����,���ü�齻��״̬��Ϊ��6235��7023�����Ѿ�����**/
			get_zd_long("0450",&stChnlCnaps.pt_date);
			long lPt_old_trace=0;
			get_zd_long("0780",&lPt_old_trace);
			sprintf(stChnlCnaps.pt_trace_no,"%ld",lPt_old_trace);
			iRet=Chnl_single_cnaps_Sel(g_pub_tx.reply,&stChnlCnaps,"pt_date=%ld and pt_trace_no='%s' and bbmk='%s'",stChnlCnaps.pt_date,stChnlCnaps.pt_trace_no,stChnlCnaps.bbmk);
			if(iRet)
			{
				strcpy(g_pub_tx.reply, "D103");
				sprintf(acErrMsg,"��ѯԭ���׳���");
				WRITEMSG
				goto ErrExit;
			}
			vtcp_log("[%s][%d],g_pub_tx.tx_code[%s] stChnlCnaps.sts=[%s]",__FILE__,__LINE__,g_pub_tx.tx_code,stChnlCnaps.sts);
		}
        }
        zip_space(cAc_no);
        if( strlen(cAc_no) >= 13 )    /*�˺�*/
        {
             memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
             iRet= Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel," ac_no= '%s'",cAc_no);
             if( iRet )
             {
                  vtcp_log("%s,%d,ac_no=[%s]���Ҵ���!",__FILE__,__LINE__,cAc_no);
                  strcpy(g_pub_tx.reply,"P102");
                  goto ErrExit;
             }
             memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
             iRet = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld ",g_mdm_ac_rel.ac_id);
             if (iRet)
             {
                  vtcp_log("%s,%d,ac_no[%s]��ѯprdt_ac_rel����!",__FILE__,__LINE__,cAc_no);
                  strcpy(g_pub_tx.reply,"K106");
                  goto ErrExit;
             }
             if(sPrdt_ac_id.ac_id_type[0] == '1')  /*����*/
             {    
                  vtcp_log("[%s],[%d],����---1!",__FILE__,__LINE__);
                  memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
                  iRet = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
                  if (iRet)
                  {
                    vtcp_log("%s,%d,ac_id[%s]���Ҵ���!",__FILE__,__LINE__,sDd_mst.ac_id);
                    strcpy(g_pub_tx.reply,"P102");
                    goto ErrExit;
                  }
                  if(sDd_mst.ac_sts[0] != '1')
                  {
                    vtcp_log("%s,%d,ac_no���˻�������!",__FILE__,__LINE__);
                    strcpy(g_pub_tx.reply,"O085");
                    goto ErrExit;
                  }
                  g_pub_tx.ac_id=sDd_mst.ac_id;
                  g_pub_tx.ac_seqn=sDd_mst.ac_seqn;
                  g_pub_tx.ac_id_type[0]='1';
                  strncpy(g_pub_tx.opn_br_no, g_mdm_ac_rel.opn_br_no ,sizeof(g_pub_tx.opn_br_no)-1);
                  strcpy(g_pub_tx.ac_no,cAc_no);
                  g_pub_tx.tx_amt1=dTx_amt;
                  strcpy(g_pub_tx.cur_no,"01");    /* ���� */
                  strcpy(g_pub_tx.ct_ind,cCt_ind); /*��ת*/
                  
                  if( cDc_ind[0] == '1')
                  {
                      strcpy(g_pub_tx.add_ind,"0");/*����(���):0��(��) 1��(��)*/
                  }
                  else if( cDc_ind[0] == '2' )
                  {
                      strcpy(g_pub_tx.add_ind,"1");/*����(���):0��(��) 1��(��)*/
                  }
                  else
                  {
                      vtcp_log("[%s][%d] �����־����!!!",__FILE__,__LINE__);
                      strcpy(g_pub_tx.reply,"P404");
                      goto ErrExit;
                  }
                  strcpy(g_pub_tx.prt_ind,"0");/*�Ƿ���۱�־*/
                  strcpy(g_pub_tx.hst_ind,"1");      /**�ռ�����ϸ**/
                  strcpy(g_pub_tx.ac_wrk_ind,"0000000");
                  g_pub_tx.svc_ind=1001;             /**��ȡ��***/
                  strcpy(g_pub_tx.ac_get_ind,"000");
                  strcpy(g_pub_tx.brf,cFd81);
                  iRet=pub_acct_trance();
                  if ( iRet )
                  {
                      sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
                      WRITEMSG
                      strcpy(g_pub_tx.reply,"T999");
                      goto ErrExit;
                  }
                  if( cDc_ind[0] == '2')/*��������*/ 
                  {
                      set_zd_data("101A",g_pub_tx.cur_no);
                      set_zd_data("101B", cCt_ind);
                      set_zd_double("1013", g_pub_tx.tx_amt1);
                      iRet = pubf_acct_proc("D099");
                      if (iRet)
                      {
                        vtcp_log("%s,%d,���˴���!ret[%ld]",__FILE__,__LINE__,iRet);
                        strcpy(g_pub_tx.reply,"T999");
                        goto ErrExit;
                      }
                  }
                  else if( cDc_ind[0] == '1')
                  {/*�跽����*/ 
                      set_zd_data("102J", g_pub_tx.cur_no);
                      set_zd_data("102K", cCt_ind);
                      set_zd_double("102F", g_pub_tx.tx_amt1);
                      iRet = pubf_acct_proc("D003");
                      if (iRet)
                      {
                          vtcp_log("%s,%d,���˴���!ret[%ld]",__FILE__,__LINE__,iRet);
                          strcpy(g_pub_tx.reply,"T999");
                          goto ErrExit;
                      }
                  }
                  else
                  {
                      vtcp_log("[%s][%d] �����־����!!!",__FILE__,__LINE__);
                      strcpy(g_pub_tx.reply,"P404");
                      goto ErrExit;
                  } 
             }
             else if(sPrdt_ac_id.ac_id_type[0] == '9')  /*�ڲ�*/
             {
                  vtcp_log("[%s],[%d],����---2!",__FILE__,__LINE__);
                  vtcp_log("%s,%d,cDc_ind=[%s]����--�����־!",__FILE__,__LINE__,cDc_ind);
                  memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
                  iRet = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
                  if (iRet)
                  {
                    vtcp_log("%s,%d,ac_no[%s]���Ҵ���!",__FILE__,__LINE__,cAc_no);
                    strcpy(g_pub_tx.reply,"P102");
                    goto ErrExit;
                  }
                  if(sIn_mst.sts[0] != '1')
                  {
                    vtcp_log("%s,%d,ac_no[%s]���˻�������!",__FILE__,__LINE__,cAc_no);
                    strcpy(g_pub_tx.reply,"O085");
                    goto ErrExit;
                  }
                  g_pub_tx.ac_id=sIn_mst.ac_id;
                  g_pub_tx.ac_seqn=sIn_mst.ac_seqn;
                  g_pub_tx.ac_id_type[0]='9';
                  strncpy(g_pub_tx.opn_br_no, g_mdm_ac_rel.opn_br_no ,sizeof(g_pub_tx.opn_br_no)-1);
                  strcpy(g_pub_tx.ac_no,cAc_no);
                  g_pub_tx.tx_amt1=dTx_amt;
                  strcpy(g_pub_tx.cur_no,"01");    /* ���� */
                  strcpy(g_pub_tx.ct_ind,cCt_ind);
                  vtcp_log("%s,%d,cDc_ind=[%s]����2--�����־!",__FILE__,__LINE__,cDc_ind);
                  if( cDc_ind[0] == '2')
                  {
                      strcpy(g_pub_tx.add_ind,"1");/*����(���):0��(��) 1��(��)*/
                  }
                  else if( cDc_ind[0] == '1' )
                  {
                      strcpy(g_pub_tx.add_ind,"0");/*����(���):0��(��) 1��(��)*/
                  }
                  else
                  {
                      sprintf(acErrMsg,"�����־����!");
                      WRITEMSG
                      strcpy(g_pub_tx.reply,"P404");
                      goto ErrExit;
                  }
                  vtcp_log("%s,%d,cDc_ind=[%s]����3--�����־!",__FILE__,__LINE__,cDc_ind);
                  vtcp_log("%s,%d,g_pub_tx.add_ind=[%s]����--�����־!",__FILE__,__LINE__,g_pub_tx.add_ind);
                  strcpy(g_pub_tx.prt_ind,"0");/*�Ƿ���۱�־*/
                  strcpy(g_pub_tx.hst_ind,"1");/**�ռ�����ϸ**/
                  strcpy(g_pub_tx.ac_wrk_ind,"0000000");
                  g_pub_tx.svc_ind=9001;/* �ڲ��ʴ�ȡ */
                  strcpy(g_pub_tx.ac_get_ind,"000");/**��ȡ��***/
                  strcpy(g_pub_tx.brf,cFd81);
                  iRet=pub_acct_trance();
                  if ( iRet )
                  {
                      sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
                      WRITEMSG
                      strcpy(g_pub_tx.reply,"T999");
                      goto ErrExit;
                  }
                  if( cDc_ind[0] == '2')/*��������*/ 
                  {
                      set_zd_data("1214",g_pub_tx.cur_no);
                      set_zd_data("1215",cCt_ind );
                      set_zd_double("1218",g_pub_tx.tx_amt1);
                      iRet = pubf_acct_proc("A017");
                      if (iRet)
                      {
                          sprintf(acErrMsg, "���ô�ȡ������ʧ��[%s]", g_pub_tx.reply);
                          WRITEMSG
                          strcpy(g_pub_tx.reply,"T999");
                          goto ErrExit;
                      }
                  }
                  else if( cDc_ind[0] == '1')   /*�跽����*/
                  { 
                      set_zd_data("1204",g_pub_tx.cur_no);
                      set_zd_data("1205",cCt_ind );
                      set_zd_double("1208",g_pub_tx.tx_amt1);
                      iRet = pubf_acct_proc("A016");
                      if (iRet)
                      {
                          sprintf(acErrMsg, "���ô�ȡ������ʧ��[%s]", g_pub_tx.reply);
                          WRITEMSG
                          strcpy(g_pub_tx.reply,"T999");
                          goto ErrExit;
                      }
                  }
                  else
                  {
                      sprintf(acErrMsg, "�����־����!!!");
                      WRITEMSG
                      strcpy(g_pub_tx.reply,"P404");
                      goto ErrExit;
                  }
             }
        }
        else if( strlen(cAc_no) < 13) /*��Ŀ*/
        {
            /*���Ϊ��Ŀ���ˣ�ȡƽ̨�����Ŀ�������*/
            memcpy(g_pub_tx.opn_br_no,sOpn_brno,sizeof(g_pub_tx.opn_br_no)-1);
            
            if( memcmp(cAc_no,"100101",5)==0)/*�ֽ����*/
            {
                  if( cDc_ind[0] == '1')
                  {
                      g_pub_tx.svc_ind=0;
                      g_pub_tx.ac_id=0; 
                      strcpy(g_pub_tx.cur_no,"01");
                      g_pub_tx.tx_amt1=dTx_amt;
                      strcpy(g_pub_tx.brf,cFd81);
                      strcpy(g_pub_tx.name,cAc_no);         /**�Է��˺�**/
                      strcpy(g_pub_tx.add_ind, "1");           /**�ֽ�����**/
                      strcpy(g_pub_tx.ct_ind, "1");            /**��ת��־**/
                      strcpy(g_pub_tx.hst_ind, "1");
                      iRet = pub_acct_cash();
                      if (iRet) 
                      {
                          sprintf(acErrMsg, "�����ֽ��ո�����ʧ��!");
                          WRITEMSG
                          goto ErrExit;
                      }
                      
                      set_zd_data("1181", g_pub_tx.cur_no);
                      set_zd_double("1184", g_pub_tx.tx_amt1);    /* ������ */
                      iRet = pubf_acct_proc("M001");
                      if (iRet)
                      {
                           sprintf(acErrMsg, "���ô�ȡ������ʧ��");
                           WRITEMSG
                           strcpy(g_pub_tx.reply,"T999");
                           goto ErrExit;
                      }
                 }
                 else if( cDc_ind[0] == '2' )
                 {
                      g_pub_tx.svc_ind=0;
                      g_pub_tx.ac_id=0; 
                      strcpy(g_pub_tx.cur_no,"01");
                      g_pub_tx.tx_amt1=dTx_amt;
                      strcpy(g_pub_tx.brf,cFd81);
                      strcpy(g_pub_tx.name,cAc_no);         /**�Է��˺�**/
                      strcpy(g_pub_tx.add_ind, "0");           /**�ֽ𸶳�**/
                      strcpy(g_pub_tx.ct_ind, "1");            /**��ת��־**/
                      strcpy(g_pub_tx.hst_ind, "1");
                      iRet = pub_acct_cash();
                      if (iRet) 
                      {
                          sprintf(acErrMsg, "�����ֽ��ո�����ʧ��!");
                          WRITEMSG
                          goto ErrExit;
                      }
                      
                      set_zd_data("1191", g_pub_tx.cur_no);
                      set_zd_double("1194", g_pub_tx.tx_amt1);    /* ������ */
                      iRet = pubf_acct_proc("M002");
                      if (iRet)
                      {
                           sprintf(acErrMsg, "���ô�ȡ������ʧ��");
                           WRITEMSG
                           strcpy(g_pub_tx.reply,"T999");
                           goto ErrExit;
                      }
                 }
            }
            else
            {
                  memset(&sCom_item,0x00,sizeof(struct com_item_c));
                  iRet = Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_no = '%s'",cAc_no);
                  if(iRet)
                  {
                      sprintf(acErrMsg,"��ѯ��Ŀʧ��!!!");
                      WRITEMSG
                      strcpy(g_pub_tx.reply, "B172");
                      goto ErrExit;
                  }
				strncpy(g_pub_tx.opn_br_no,sOpn_brno,sizeof(g_pub_tx.opn_br_no)-1);
				vtcp_log("[%s][%d],g_pub_tx.opn_br_no[%s] sOpn_brno=[%s]",__FILE__,__LINE__,g_pub_tx.opn_br_no,sOpn_brno);
                  strcpy(g_pub_tx.ac_no,cAc_no);
                  g_pub_tx.ac_seqn=0;
                  g_pub_tx.tx_amt1=dTx_amt;
                  strcpy(g_pub_tx.cur_no,"01");    /* ���� */
                  strcpy(g_pub_tx.ct_ind,cCt_ind);
                  if( cDc_ind[0] == '1')
                  {
                      strcpy(g_pub_tx.add_ind,"0");/*����(���):0��(��) 1��(��)*/
                  }
                  else if( cDc_ind[0] == '2')
                  {
                      strcpy(g_pub_tx.add_ind,"1");/*����(���):0��(��) 1��(��)*/
                  }
                  else
                  {
                      sprintf(acErrMsg, "�����־����!!!");
                      WRITEMSG
                      strcpy(g_pub_tx.reply,"P404");
                      goto ErrExit;
                  }
                  strcpy(g_pub_tx.prt_ind,"0");/*�Ƿ���۱�־*/
                  strcpy(g_pub_tx.hst_ind,"1");/**�ռ�����ϸ**/  
                  strcpy(g_pub_tx.ac_wrk_ind,"0000000");
                  g_pub_tx.svc_ind=9001;/* �ڲ��ʴ�ȡ */
                  strcpy(g_pub_tx.ac_get_ind,"000");/**��ȡ��***/
                  strcpy(g_pub_tx.brf,cFd81);
                  iRet=pub_acct_trance();
                  if ( iRet )
                  {
                      sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
                      WRITEMSG
                      strcpy(g_pub_tx.reply,"T999");
                      goto ErrExit;
                  }
                  if( cDc_ind[0] == '2')/*��������*/ 
                  {
                      set_zd_data("1214",g_pub_tx.cur_no);
                      set_zd_data("1215",cCt_ind );
                      set_zd_double("1218",g_pub_tx.tx_amt1);
                      iRet = pubf_acct_proc("A017");
                      if (iRet)
                      {
                          sprintf(acErrMsg, "���ô�ȡ������ʧ��[%s]", g_pub_tx.reply);
                          WRITEMSG
                          strcpy(g_pub_tx.reply,"T999");
                          goto ErrExit;
                      }
                  }
                  else if( cDc_ind[0] == '1')
                  {/*�跽����*/ 
                      set_zd_data("1204",g_pub_tx.cur_no);
                      set_zd_data("1205",cCt_ind );
                      set_zd_double("1208",g_pub_tx.tx_amt1);
                      iRet = pubf_acct_proc("A016");
                      if (iRet)
                      {
                          sprintf(acErrMsg, "���ô�ȡ������ʧ��[%s]", g_pub_tx.reply);
                          WRITEMSG
                          strcpy(g_pub_tx.reply,"T999");
                          goto ErrExit;
                      }
                  }
                  else
                  {
                      sprintf(acErrMsg, "�����־����!!!");
                      WRITEMSG
                      strcpy(g_pub_tx.reply,"P404");
                      goto ErrExit;
                  }
            }
        } 
    }

OkExit:
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    if(memcmp(g_pub_tx.reply,"0000",4) == 0)
    {
         strcpy(g_pub_tx.reply, "H034");
    }
    sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}




