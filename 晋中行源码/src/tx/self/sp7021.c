/*******************************************************************************
* �� �� ��:  sp7021.c
* ���������� ���������ֽ���˽���(����֧��ר�ýӿ�)
*            ���������������ʻ�/��Ŀ���������ֽ���ˡ�
* ��  ��:  chenchao
* ������ڣ� 2011-7-12 13:57:59
*
* �޸ļ�¼�� 
* ��    ��:20110816
* �� �� �ˣ�xueke
* �޸�����:
	pub_com_test.c()���Ʊ�־ ����־��������ƽ̨���н��� 73����� ÿ���ֽڴ���һ�ֿ��ƻ���
*
insert into TX_DEF  values ('7021', '���������ֽ����', '10000000000000000000000000000100000000010000010000000000000001000100000000000000110000000000000000000000000000000000000000000100', '1', '3', '0');

insert into TX_SUB_DEF  values ('7021', '���������ֽ���˽���', 'sp7021', '0', '0');

insert into TX_FLOW_DEF values ('7021', 0, '7021', '#=0001@1@#=0002@3@#=0003@5@#=0004@6@#$');
insert into TX_FLOW_DEF values ('7021', 1, 'D003', '#=0000@2@#$');
insert into TX_FLOW_DEF values ('7021', 2, 'M002', '#=0000@7@#$');
insert into TX_FLOW_DEF values ('7021', 3, 'D099', '#=0000@4@#$');
insert into TX_FLOW_DEF values ('7021', 4, 'M001', '#=0000@7@#$');
insert into TX_FLOW_DEF values ('7021', 5, 'A016', '#=0000@2@#$');
insert into TX_FLOW_DEF values ('7021', 6, 'A017', '#=0000@4@#$');
insert into TX_FLOW_DEF values ('7021', 7, 'F059', '#=0000@8@#$');
insert into TX_FLOW_DEF values ('7021', 8, 'F000', '#$');

insert into TX_SUB_REL  values ('7021', 'A016', '0', '0300', '1201', '�˺�');
insert into TX_SUB_REL  values ('7021', 'A016', '1', '01', '1204', '����');
insert into TX_SUB_REL  values ('7021', 'A016', '1', '1', '1205', 'ת��');
insert into TX_SUB_REL  values ('7021', 'A016', '0', '0400', '1208', '���');
insert into TX_SUB_REL  values ('7021', 'A016', '0', '0810', '120A', 'ժҪ');
insert into TX_SUB_REL  values ('7021', 'A017', '0', '0300', '1211', '�˺�');
insert into TX_SUB_REL  values ('7021', 'A017', '1', '1', '1215', '��ת��־');
insert into TX_SUB_REL  values ('7021', 'A017', '0', '0400', '1218', '���');
insert into TX_SUB_REL  values ('7021', 'A017', '0', '0810', '121A', 'ժҪ');
insert into TX_SUB_REL  values ('7021', 'A017', '1', '01', '1214', '����');
insert into TX_SUB_REL  values ('7021', 'D003', '0', '0300', '1021', '�˺�');
insert into TX_SUB_REL  values ('7021', 'D003', '1', '0000', '0192', '���Ʊ�־');
insert into TX_SUB_REL  values ('7021', 'D003', '1', '1', '0191', '���Ʊ�־');
insert into TX_SUB_REL  values ('7021', 'D003', '1', '1', '1022', '�˺�');
insert into TX_SUB_REL  values ('7021', 'D003', '0', '0400', '102F', '���');
insert into TX_SUB_REL  values ('7021', 'D003', '1', '01', '102J', '����');
insert into TX_SUB_REL  values ('7021', 'D003', '1', '1', '102K', '��ת��־');
insert into TX_SUB_REL  values ('7021', 'D003', '0', '0810', '102Y', 'ժҪ');
insert into TX_SUB_REL  values ('7021', 'D099', '0', '0300', '1011', '�˺�');
insert into TX_SUB_REL  values ('7021', 'D099', '1', '01', '101A', '����');
insert into TX_SUB_REL  values ('7021', 'D099', '1', '1', '101B', '��ת');
insert into TX_SUB_REL  values ('7021', 'D099', '1', '0000', '0192', '���Ʊ�־');
insert into TX_SUB_REL  values ('7021', 'D099', '1', '1', '0191', '���Ʊ�־');
insert into TX_SUB_REL  values ('7021', 'D099', '0', '0810', '1016', 'ժҪ');
insert into TX_SUB_REL  values ('7021', 'D099', '1', '1', '1012', '���');
insert into TX_SUB_REL  values ('7021', 'D099', '0', '0400', '1013', '���');
insert into TX_SUB_REL  values ('7021', 'M001', '0', '0400', '1184', '���');
insert into TX_SUB_REL  values ('7021', 'M001', '0', '0300', '1188', '�Է��˺�');
insert into TX_SUB_REL  values ('7021', 'M001', '1', '01', '1181', '�Է��˺�');
insert into TX_SUB_REL  values ('7021', 'M001', '0', '0810', '1187', 'ժҪ');
insert into TX_SUB_REL  values ('7021', 'M002', '0', '0400', '1194', '���');
insert into TX_SUB_REL  values ('7021', 'M002', '1', '01', '1191', '����');
insert into TX_SUB_REL  values ('7021', 'M002', '0', '0810', '1197', 'ժҪ');
insert into TX_SUB_REL  values ('7021', 'M002', '0', '0300', '1198', '�Է��˺�');

*******************************************************************************/
#include <stdio.h>
#include <unistd.h>
#define EXTERN
#include "public.h"
#include "chnl_single_cnaps_c.h"
#include "dd_mst_c.h"
#include "com_item_c.h"
#include "dd_parm_c.h"
#include "in_mst_c.h"
#include "prdt_ac_id_c.h"


int sp7021()
{
  int i=0;
  int iRet = 0;
  char  cAc_name[61]; /** ���� **/
  char  tmpstr[41];
  char  cFee_flag[2]; /*�շѱ�־*/
  char  cCk_flag[2];  /*����־1-���ȷ��*/
  char  cPt_old_trace[13]; /*ԭƽ̨��ˮ*/
  char  cFd123[1000]; /*����123����շ�����*/
  long  lFee_cnt=0;     /*�շ�����*/
  char  cFee_cnt[3];    /*�շ�����*/
  char  cFee_acno[25];  /*�����˺Ż��Ŀ*/
  char  cFee_amt[13];   /*�շѽ��*/
  char  cAc_no[25]; /** ���˺� **/
  char  cDc_ind[2]; /** �����־ 1��2�� **/
  char  cFee_dc_ind[2]; /*���õĽ����־*/
  char  cMsr2[38];  /** ���ŵ���Ϣ **/
  char  cMsr3[105]; /** ���ŵ���Ϣ **/
  char  cPwd[17]; /** �ͻ����� **/
  char  cBrf[51]; /** ����ժҪ **/
  char  cGz_flag[2];/**���˱�־**/
  double  dTx_amt = 0.00; /** ���׽�� **/
  double  dFee_amt = 0.00; /*�����ѽ��*/
  double  dFee_amt1=0.00;
  double  dBal=0.00;
  double  dCif_fee=0.00;  /*Ӧ�տͻ�������*/
  double  dKy_amt = 0.00;/*���ý��*/
  char  cCert_no[21]; /** ֤������(1λ)+����(19λ) ֤�����ͣ�1�����֤��2�����ڲ���
          3�����գ�4������֤��5������֤��6��ʿ��֤��7���۰ľ�������ͨ��֤
          8����ҵ����֤��9����Ӫ���֤��A: Ӫҵִ�գ�B: ��ҵ����֤�飻
          C�����̺�׼�ţ�D���������Թ�����E����ʱ���֤��F������˾���֤��
          G������֤��H������֤������˽����F��̨��ͬ������ͨ��֤**/
  char  cCtrlcmd[11]; /**���Ʊ�־ ����־��������ƽ̨���н��� 73����� ÿ���ֽڴ���һ�ֿ��ƻ���
          ����־(0�����1���)��
          [0]:�������־0����1��
          [1]:�ŵ���Ϣ����־0����1��
          [2]:֤������־0����1��
          [3]:��������־0���鲻��
          [4]:�ʺż���־0���鲻��
          [5]:�տ��˹��ʱ�־0�ʺű���������黧��1�ʺŻ������붼���2�ʺű��������������������3�ʺŻ�����һ��������,9ǿ�ƹ���
          [6]:ͨ��ͨ�ұ�־0����1������
          [7]:ƾ֤����־0����1���Աƾ֤2��ͻ�ƾ֤
          ******************/
  char  cDc_entry[601]; /** (�����ʺź��ֽ���)������Ʒ�¼
          ��������,�ʺ�/��Ŀ,�����־�����+�س�
          ��������=END ��ʶ����
          ******************/
  long  lPt_date=0;
  long  lPt_trace_no = 0;
  long	lPt_old_trace=0;  /*ԭƽ̨��ˮ long��*/
  long	lPt_old_date=0;   /*ԭƽ̨��������*/
  
  struct  chnl_single_cnaps_c  stChnlCnaps;
  struct  dd_mst_c    sDd_mst;
  struct  com_item_c    sCom_item;
  struct  dd_parm_c   sDd_parm;
  struct    prdt_ac_id_c    sPrdt_ac_id;
  struct    in_mst_c        sIn_mst;
  /** ��ʼ�������� **/
  char	cFd125[282] = {0};/*Added by YangMeng 20140703 �Ǽǽ��׹�����������*/
  char	cHx_term[5] = {0};/*Added by YangMeng 20140703 �ն˺� COMM-ͨ�� HVPS-��� BEPS-С��*/
  
  memset(cFee_flag, 0x00 ,sizeof(cFee_flag));
  memset(cFd123,0x00,sizeof(cFd123));
  memset(cPwd, 0x00, sizeof(cPwd));
  memset(cBrf, 0x00, sizeof(cBrf));
  memset(cMsr2, 0x00, sizeof(cMsr2));
  memset(cMsr3, 0x00, sizeof(cMsr3));
  memset(tmpstr, 0x00, sizeof(tmpstr));
  memset(cAc_no, 0x00, sizeof(cAc_no));
  memset(cDc_ind, 0x00, sizeof(cDc_ind));
  memset(cFee_dc_ind,0x00,sizeof(cFee_dc_ind));
  memset(cGz_flag,0x00,sizeof(cGz_flag));
  memset(cCk_flag,0x00,sizeof(cCk_flag));
  memset(cAc_name, 0x00, sizeof(cAc_name));
  memset(cCert_no, 0x00, sizeof(cCert_no));
  memset(cCtrlcmd, 0x00, sizeof(cCtrlcmd));
  memset(cDc_entry, 0x00, sizeof(cDc_entry));
  memset(cFee_amt,0x00,sizeof(cFee_amt));
  memset(cFee_cnt,0x00,sizeof(cFee_cnt));
  memset(cFee_acno,0x00,sizeof(cFee_acno));
  memset(cPt_old_trace,0x00,sizeof(cPt_old_trace));
  memset(&sDd_mst,0x00,sizeof(sDd_mst));
  memset(&sCom_item,0x00,sizeof(sCom_item));
  memset(&sDd_parm,0x00,sizeof(sDd_parm));
  memset(&sPrdt_ac_id,0x00,sizeof(sPrdt_ac_id));
  memset(&sIn_mst,0x00,sizeof(sIn_mst));
  memset(&stChnlCnaps,0x00,sizeof(struct  chnl_single_cnaps_c));
  
  if (pub_base_sysinit())
  {
    sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
    WRITEMSG
    strcpy(g_pub_tx.reply, "CU06");
    goto ErrExit;
  }
  vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
  set_zd_long("0460",g_pub_tx.tx_date);
  /*** ȡֵ **/
  get_zd_long("0440", &lPt_date);
  get_zd_long("0520", &lPt_trace_no);
  get_zd_data("0250", cAc_name );
  get_zd_data("0300", cAc_no   );
  get_zd_data("0620", cCert_no);
  get_zd_data("0660", cDc_ind);
  get_zd_data("0700", cCk_flag);
  get_zd_data("0710", cFee_flag);
  get_zd_data("0730", cCtrlcmd);
  get_zd_data("0750", cMsr2);
  get_zd_data("0760", cMsr3);
  /**
  get_zd_data("0790", cPwd);
  **/
  get_zd_data("0810", cBrf);
  get_zd_data("0950", cDc_entry);
  get_fd_data("1230", cFd123);
  get_zd_double("0400", &dTx_amt);
  
  vtcp_log("%s,%d,startsp7021",__FILE__,__LINE__);
  
	/**Added by YangMeng 20140703 ��������������Ӧ��ҵ����Ϣ(�Է��˺š�������)begin*/
	memset(cFd125,0x00,sizeof(cFd125));
	memset(cHx_term,0x00,sizeof(cHx_term));
	get_zd_data("0100",cHx_term);
	vtcp_log("[%s],[%d],cHx_term=[%s]",__FILE__,__LINE__,cHx_term);
	get_fd_data("1250",cFd125);
	if(!memcmp(cHx_term,"HVPS",4) || !memcmp(cHx_term,"BEPS",4))
	{
		vtcp_log("[%s][%d],cFd125=[%s]",__FILE__,__LINE__,cFd125);
		iRet=pubf_com_checkin_txdetail(cFd125);
		if (iRet)
		{
		  sprintf(acErrMsg,"%s,%d,ִ����������������Ӧҵ����Ϣ����ʧ��",__FILE__,__LINE__,iRet);
		  WRITEMSG
		  goto ErrExit;	
		}
	}
	/**Added by YangMeng 20140703 ��������������Ӧ��ҵ����Ϣ(�Է��˺š�������)end*/
	
  if( cCk_flag[0] == '1' )  /*���ȷ��*/
  {
     get_zd_long("0450", &lPt_old_date);
     get_zd_long("0780", &lPt_old_trace);
     memset(cPt_old_trace,0x00,sizeof(cPt_old_trace));
     sprintf(cPt_old_trace,"%ld",lPt_old_trace);	/*ԭƽ̨��ˮ��longתchar*/ 
     iRet=Chnl_single_cnaps_Sel(g_pub_tx.reply,&stChnlCnaps,"pt_date=%ld and pt_trace_no='%s' and sts='1'",lPt_old_date,cPt_old_trace);
     if( iRet == 0)
     {
          /*�Ѿ����˳ɹ�*/
          vtcp_log("[%s][%d] �ظ�����",__FILE__,__LINE__);
          /* mod by sunhongwei 2013-05-09 ���ȷ���ظ�����Ҫ���������ɹ�
          strcpy(g_pub_tx.reply,"CU94");
          */
          strcpy(g_pub_tx.reply, "CU00");
          set_zd_long("0040",stChnlCnaps.trace_no);
          set_zd_long("0460",stChnlCnaps.tx_date);
          goto ErrExit;
     }
  }
  zip_space(cAc_no);
  /*����������*/
  if(cFee_flag[0] == '0')
  {
      stChnlCnaps.fee_amt=0.00;
  }
  else if( cFee_flag[0] == '1' || cFee_flag[0] == '2')
  {
        memcpy(cFee_cnt,cFd123+1,2);
        vtcp_log("[%s][%d],cFee_cnt=[%s]",__FILE__,__LINE__,cFee_cnt);
        lFee_cnt=atoi(cFee_cnt);
        vtcp_log("[%s][%d],lFee_cnt=[%d]",__FILE__,__LINE__,lFee_cnt);
        i=0;
        dFee_amt1=0.00;
        for(i=0;lFee_cnt > i; i++)
        {    
             memset(cFee_amt,0x00,sizeof(cFee_amt));
             memset(cFee_dc_ind,0x00,sizeof(cFee_dc_ind));
             /**
             memcpy(cFee_amt,cFd123+27 + 46 * i,12);
             **/
             memcpy(cFee_acno,cFd123+3 + 48 * i,24);
             zip_space(cFee_acno);
             memcpy(cFee_amt,cFd123 + 29 + 48 * i,12);
             memcpy(cFee_dc_ind,cFd123 + 27 + 48 * i,1);
             vtcp_log("[%s][%d],cFee_amt=[%s]",__FILE__,__LINE__,cFee_amt);
             vtcp_log("[%s][%d],cFee_dc_ind =[%s]",__FILE__,__LINE__,cFee_dc_ind);
             dFee_amt1=atof(cFee_amt) / 100.00; /**modify 20110822: ����Է�Ϊ��λ**/
             vtcp_log("[%s][%d],dFee_amt=[%lf]",__FILE__,__LINE__,dFee_amt);
             if(cFee_dc_ind[0] == '1')
             {
                if(strcmp(cAc_no,cFee_acno)==0)
                {
                    dCif_fee = dCif_fee + dFee_amt1;
                }
                dFee_amt=dFee_amt + dFee_amt1;
             }
        }
        vtcp_log("[%s][%d],dFee_amt=[%lf]",__FILE__,__LINE__,dFee_amt);
        stChnlCnaps.fee_amt=dCif_fee;      /*�����ѽ��*/             
  }
  vtcp_log("[%s][%d]Ӧ�տͻ�������=[%lf]",__FILE__,__LINE__,dCif_fee);
  pub_base_old_acno(cAc_no);  /**�¾��˺�ת��**/
  memcpy(g_pub_tx.ac_no,cAc_no,sizeof(g_pub_tx.ac_no)-1);
  zip_space(g_pub_tx.ac_no);
  if( strlen ( g_pub_tx.ac_no ) >10) /*** ���������,ֻ�����˺� ***/
  {
        iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",cAc_no);
        if (iRet)
        {
            vtcp_log("%s,%d,ac_no[%s]���Ҵ���!",__FILE__,__LINE__,g_pub_tx.ac_no);
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
        iRet=Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id," ac_id = %ld ",g_mdm_ac_rel.ac_id);
        if(iRet)
        {
            vtcp_log("%s,%d,ac_no[%s]��ѯprdt_ac_rel����!",__FILE__,__LINE__,g_pub_tx.ac_no);
            strcpy(g_pub_tx.reply,"K106");
            goto ErrExit;
        }
        if(sPrdt_ac_id.ac_id_type[0] == '1')
        {
            iRet = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
            if (iRet)
            {
                vtcp_log("%s,%d,ac_no[%s]���Ҵ���!",__FILE__,__LINE__,g_pub_tx.ac_no);
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
            }
            iRet = Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no='%s'",sDd_mst.prdt_no);
            if(iRet)
            {
                vtcp_log("%s,%d, ���һ��ڲ�Ʒ����!",__FILE__,__LINE__);
                strcpy(g_pub_tx.reply,"W015");
                goto ErrExit;
            }
            vtcp_log("%s,%d,ac_no[%s]���˻�����!",__FILE__,__LINE__,g_pub_tx.ac_no);
            /*73����Ʊ�־���*/
            iRet=pub_com_test(cCtrlcmd,cCert_no,cAc_name,cAc_no,"","","2",cGz_flag);
            vtcp_log("%s,%d,cCtrlcmd=[%s],cCert_no=[%s],cAc_name=[%s],cAc_no=[%s],cGz_flag=[%s]",__FILE__,__LINE__,cCtrlcmd,cCert_no,cAc_name,cAc_no,cGz_flag);
            if (iRet)
             {
                sprintf(acErrMsg,"%s,%d,��������pub_com_test()����ʧ��",__FILE__,__LINE__);
                WRITEMSG
                goto ErrExit;	
             }
            set_zd_data("0680","1");/** �����ʻ� **/         
            /*** ��ʼ���ü���Ҫ�� **/
            if(cDc_ind[0] == '1')
            {
                /*�˻�ȡ��ʱ ����˻���״̬*/
                /*�˺ű�ȫ�������ֻ��������������ȡ��*/
                 if(sDd_mst.hold_sts[0] == '1')
                {
                    strcpy(g_pub_tx.reply,"P110");
                    sprintf(acErrMsg,"%s,%d,���˺ű�ȫ������",__FILE__,__LINE__);
                    WRITEMSG
                    goto ErrExit;
                }
                else if (sDd_mst.hold_sts[0] == '2')
                {
                    strcpy(g_pub_tx.reply,"P111");
                    sprintf(acErrMsg,"%s,%d,���˺Ŵ���ֻ������״̬",__FILE__,__LINE__);
                    WRITEMSG
                    goto ErrExit;
                }
                 /*���������*/
                if(pub_base_CompDblVal(sDd_mst.hold_amt,sDd_parm.min_bal) >= 0)
                {
                    dKy_amt = sDd_mst.bal -sDd_mst.ctl_amt -sDd_mst.hold_amt;
                }
                else
                {
                    dKy_amt = sDd_mst.bal - sDd_mst.ctl_amt - sDd_parm.min_bal + sDd_mst.hold_amt;
                }             
                if(pub_base_CompDblVal(dKy_amt,dTx_amt + dCif_fee) < 0)
                {
                   strcpy(g_pub_tx.reply,"A030");
                   sprintf(acErrMsg,"%s,%d,��������",__FILE__,__LINE__);
                   WRITEMSG
                   goto ErrExit;
                }
                
                dBal=0.00;
                dKy_amt=0.00;
                dBal=sDd_mst.bal - dTx_amt;
                dKy_amt=dBal - sDd_mst.ctl_amt - sDd_mst.hold_amt; 
                memcpy(tmpstr,"1001156C",8);
                sprintf(tmpstr+8,"%012.0f",dBal*100);
                memcpy(tmpstr+20,"1002156C",8);
                sprintf(tmpstr+28,"%012.0f",dKy_amt*100);
                vtcp_log("%s,%d,��ѯ��������ǣ�tmpstr=[%s]",__FILE__,__LINE__,tmpstr);
                set_zd_data("0820",tmpstr);
                strcpy(g_pub_tx.reply, "0001");
            }
            else if(cDc_ind[0] == '2')
            {
                /*���󷵻����*/
                if(pub_base_CompDblVal(sDd_mst.hold_amt,sDd_parm.min_bal) >= 0)
                {
                    dKy_amt = sDd_mst.bal -sDd_mst.ctl_amt -sDd_mst.hold_amt;
                }
                else
                {
                    dKy_amt = sDd_mst.bal - sDd_mst.ctl_amt - sDd_parm.min_bal + sDd_mst.hold_amt;
                }             
                dBal=0.00;
                dKy_amt=0.00;
                dBal=sDd_mst.bal + dTx_amt - dCif_fee;
                dKy_amt=dBal - sDd_mst.ctl_amt - sDd_mst.hold_amt; 
                memcpy(tmpstr,"1001156C",8);
                sprintf(tmpstr+8,"%012.0f",dBal*100);
                memcpy(tmpstr+20,"1002156C",8);
                sprintf(tmpstr+28,"%012.0f",dKy_amt*100);
                vtcp_log("%s,%d,��ѯ��������ǣ�tmpstr=[%s]",__FILE__,__LINE__,tmpstr);
                set_zd_data("0820",tmpstr);
                
                strcpy(g_pub_tx.reply, "0002");
            }else
            {
              sprintf(acErrMsg,"�����־����!!!");
              WRITEMSG
              strcpy(g_pub_tx.reply, "P404");
              goto ErrExit;
            }
        }
        else if( sPrdt_ac_id.ac_id_type[0] == '9')
        {
            iRet = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
            if (iRet)
            {
                vtcp_log("%s,%d,ac_no[%s]���Ҵ���!",__FILE__,__LINE__,g_pub_tx.ac_no);
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
            }
            if(sIn_mst.sts[0] != '1')
            {
                vtcp_log("%s,%d,ac_no[%s]���˻�������!",__FILE__,__LINE__,g_pub_tx.ac_no);
                strcpy(g_pub_tx.reply,"O085");
                goto ErrExit;
            }
            set_zd_data("0680","9");/** �ڲ��ʻ� **/
            /*** ��ʼ���ü���Ҫ�� **/
            if(cDc_ind[0] == '1')
            {
               strcpy(g_pub_tx.reply, "0003");
            }else if(cDc_ind[0] == '2')
            {
               strcpy(g_pub_tx.reply, "0004");
            }else
            {
                sprintf(acErrMsg,"�����־����!!!"); 
                WRITEMSG
                strcpy(g_pub_tx.reply, "P404");
                goto ErrExit;
            }
        } 
  }
  else
  {/**** �����ǿ�Ŀ  **/
      iRet = Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_no = '%s'",g_pub_tx.ac_no);
      if(iRet)
      {
        sprintf(acErrMsg,"��ѯ��Ŀʧ��!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "B172");
        goto ErrExit;
      }
       /*73����Ʊ�־���*/
      iRet=pub_com_test(cCtrlcmd,cCert_no,cAc_name,cAc_no,"","","2",cGz_flag);
      vtcp_log("%s,%d,cCtrlcmd=[%s],cCert_no=[%s],cAc_name=[%s],cAc_no=[%s],cGz_flag=[%s]",__FILE__,__LINE__,cCtrlcmd,cCert_no,cAc_name,cAc_no,cGz_flag);
      if (iRet)
       {
           sprintf(acErrMsg,"%s,%d,��������pub_com_test()����ʧ��",__FILE__,__LINE__);
           WRITEMSG
           goto ErrExit;	
       }
       /*** ��ʼ���ü���Ҫ�� **/
      if(cDc_ind[0] == '1')
      {
         strcpy(g_pub_tx.reply, "0003");
      }else if(cDc_ind[0] == '2')
      {
         strcpy(g_pub_tx.reply, "0004");
      }else
      {
          sprintf(acErrMsg,"�����־����!!!"); 
          WRITEMSG
          strcpy(g_pub_tx.reply, "P404");
          goto ErrExit;
      }
  }
  
  
   /**���Ǽǵ��ʼ��˵ǼǱ���***/
   
   stChnlCnaps.chnl_type[0] = cCtrlcmd[9]; /* 20120915 zws ����������־ */
   vtcp_log("[%s][%d],������ʶ=[%s]",__FILE__,__LINE__,stChnlCnaps.chnl_type);
   
   memcpy(stChnlCnaps.bbmk,g_pub_tx.tty,sizeof(stChnlCnaps.bbmk)-1); /** ϵͳ���� **/
   vtcp_log("[%s][%d],ϵͳ����=[%s]",__FILE__,__LINE__,stChnlCnaps.bbmk);
   
   stChnlCnaps.tx_date = g_pub_tx.tx_date; /** ϵͳ����  **/
   vtcp_log("[%s][%d],��������=[%d]",__FILE__,__LINE__,stChnlCnaps.tx_date);
   sprintf(stChnlCnaps.tx_time,"%6ld", g_pub_tx.tx_time);  /** ϵͳʱ��  **/
 
   stChnlCnaps.trace_no=g_pub_tx.trace_no;
   vtcp_log("[%s][%d],������ˮ=[%d]",__FILE__,__LINE__,stChnlCnaps.trace_no);
   
   memcpy(stChnlCnaps.cur_no,"01",2); /**  ��ʱд��Ϊ01����� ***/
   memcpy(stChnlCnaps.ac_no, g_pub_tx.ac_no,sizeof( stChnlCnaps.ac_no)-1); /** ���˺� **/
   memcpy(stChnlCnaps.tx_code,g_pub_tx.tx_code,sizeof(stChnlCnaps.tx_code)-1); /** ������ **/
   memcpy(stChnlCnaps.tx_br_no,g_pub_tx.tx_br_no,sizeof(stChnlCnaps.tx_br_no)-1);  /** ���׻��� **/
   memcpy(stChnlCnaps.opn_br_no,stChnlCnaps.opn_br_no,sizeof(stChnlCnaps.opn_br_no)-1);  /** �������� **/
   memcpy(stChnlCnaps.tel,g_pub_tx.tel,sizeof(stChnlCnaps.tel)-1); /** ���׹�Ա  **/
   memcpy(stChnlCnaps.ttyn,g_pub_tx.tty,sizeof(stChnlCnaps.ttyn)-1); /** �ն˺� **/
   
   stChnlCnaps.pt_date = lPt_date; /** ƽ̨����  **/
   vtcp_log("[%s][%d],ƽ̨����=[%d]",__FILE__,__LINE__,stChnlCnaps.pt_date);
   
   sprintf(stChnlCnaps.pt_trace_no ,"%ld" ,lPt_trace_no); /** ƽ̨��ˮ **/
   vtcp_log("[%s][%d],ƽ̨��ˮ=[%s]",__FILE__,__LINE__,stChnlCnaps.pt_trace_no);
  
   stChnlCnaps.tx_amt = dTx_amt ;  /** ���׽�� **/
   vtcp_log("[%s][%d],���׽��=[%.2f]",__FILE__,__LINE__,stChnlCnaps.tx_amt);
   
   stChnlCnaps.fee_amt = dFee_amt1 ;  /** ���׽�� **/
   vtcp_log("[%s][%d],������=[%.2f]",__FILE__,__LINE__,dFee_amt1);
    
   stChnlCnaps.ct_ind[0] = '1';  /** ��ת��־  ������ֻ�����ֽ� **/
   stChnlCnaps.sts[0] = '1'; /** ����״̬��ʼΪ�ɹ� **/
   stChnlCnaps.dz_ind[0] = '0';  /**  ����״̬ **/
   
   memcpy(stChnlCnaps.brf,cBrf,sizeof(stChnlCnaps.brf)-1); /** ����ժҪ **/
   
   Chnl_single_cnaps_Debug(&stChnlCnaps);
   iRet = Chnl_single_cnaps_Ins(stChnlCnaps);
   if(iRet)
   {
     sprintf(acErrMsg,"�Ǽǵ��ʼ��˵ǼǱ�����!!!");
     WRITEMSG
     strcpy(g_pub_tx.reply, "D107");
     goto ErrExit;
   }

GoodExit:
  sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
  WRITEMSG
  set_zd_data(DC_REPLY,g_pub_tx.reply);
  return 0;
ErrExit:
  if(memcmp(g_pub_tx.reply,"0000",4) == 0)
  {
    strcpy(g_pub_tx.reply, "H034");
  }
  sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
  WRITEMSG
  set_zd_data(DC_REPLY,g_pub_tx.reply);
  return 1;
}


