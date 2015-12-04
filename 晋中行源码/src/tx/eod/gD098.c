/*************************************************
* �� �� ��: gD008.c
* ������������˰��ҵ��������������
*
* ��    ��: jack
* �������: 2003��10��16��
*
* �޸�����:               
* �� �� ��: xxxx     
* �޸�����: 
*
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#define  EXTERN
#define  cIdtp "1"
#include "public.h"
#include <errno.h>
#include "com_branch_c.h"
#include "rsp_code_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "mq_tax_head_c.h"
#include "mq_tax_payment_c.h"
#include "ag_unitinfo_c.h"
#include "ag_peritemmap_c.h"
#include "pay_agent_c.h"
gD098()
{
  int ret = 0;
  int ret1 = 0;
  char tmp_acct[21]; /* ��ʱ����ʻ� */
  char cReturn[51];
  char execstr[248];
  
  int iErr_flag = 0;  /*  0��������ȷ��1������ʧ��  */
  
  long cif_no = 0;
  char xs_acno[20];
  char jz_acno[20];
  int  jz_ac_seq = 0;
  
  long   lSnum = 0;      /*�ɹ�����*/
  long   lFnum = 0;      /*ʧ�ܱ���*/
  double dSamt = 0.00;   /*�ɹ����*/
  double dFamt = 0.00;   /*ʧ�ܽ��*/
  
  char filename[248];
  char filename1[248];
  char filename3[248];
  FILE *fp = NULL;
  FILE *fp1 = NULL;
  FILE *fp3 = NULL;
  
  char filename01[248];
  char filename11[248];
  char filename31[248];
  FILE *fp01 = NULL;
  FILE *fp11 = NULL;
  FILE *fp31 = NULL;
  
  int i=0;
  
  long lTrace_no = 0;
  
  char tmpstr[512];

  struct mq_tax_head_c     sMq_tax_head;
  struct mq_tax_payment_c  sMq_tax_payment;
  struct dd_mst_c          sDd_mst;
  struct mdm_ac_rel_c     sMdm_ac_rel ;
  struct com_branch_c      sCom_branch;
  struct rsp_code_c        sRsp_code;
  struct ag_unitinfo_c      sAg_unitinfo;
  struct ag_peritemmap_c sAg_peritemmap;
  struct pay_agent_c sPay_agent;

  /**------- ��ʼ��ȫ�ֱ��� --------**/
  pub_base_sysinit();
  memset(&sAg_unitinfo,0x00,sizeof(sAg_unitinfo));
  memset(&sAg_peritemmap,0x00,sizeof(sAg_peritemmap));
  memset(&sPay_agent,0x00,sizeof(sPay_agent));

  /*---����Ҫ���죬����Ҫ�ڴ���������---*/
  ret = Mq_tax_head_Dec_Sel_hold( g_pub_tx.reply , "sts = '0' \
    and ( entrust_date <= %ld and entrust_date+return_term >= %ld ) )", \
    g_pub_tx.tx_date, g_pub_tx.tx_date );
  if ( ret )
  {
    sprintf(acErrMsg,"ִ��Mq_tx_head_Dec_Sel����[%d]", ret );
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
  }

  while(1)
  {
    memset( &sMq_tax_head, 0x0, sizeof(sMq_tax_head) );
    lSnum = 0;     /*�ɹ�����*/
    lFnum = 0;     /*ʧ�ܱ���*/
    dSamt = 0.00;  /*�ɹ����*/
    dFamt = 0.00;  /*ʧ�ܽ��*/

    ret = Mq_tax_head_Fet_Sel( &sMq_tax_head, g_pub_tx.reply );
    if ( ret == 100 )
    {
      sql_begin();
      break;
    }
    if ( ret )
    {
      sprintf(acErrMsg,"ִ��Mq_tax_head_Fet_Sel����[%d]", ret );
      WRITEMSG
      goto ErrExit;
    }
    
    pub_base_strpack( sAg_unitinfo.pay_type );
  
    /*--- һ��˰�����һ������ ---*/
    sql_begin();
    
    /* ȥ��ͷ��β���Ŀո� */
    pub_base_strpack( sMq_tax_head.taxorg_code );
    pub_base_strpack( sMq_tax_head.payee_acct );
    
    /*--------------  ��鵥λ�˺�״̬  ---------------*/
    memset( &sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );
    ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
      "ac_no = '%s'", sMq_tax_head.payee_acct  );
    if ( ret )
    {
      sprintf(acErrMsg,"��ѯ�����˺Ŷ��ձ�ʧ��");
      WRITEMSG
      set_zd_data(DC_REPLY,g_pub_tx.reply);
      sql_rollback();
      return 1;
    }    

    /* ȡ��ʱ�ʻ� */
    strcpy(tmp_acct, sMq_tax_head.payee_acct);
    
    memset( &sDd_mst, 0x00, sizeof(sDd_mst) );
    ret = Dd_mst_Sel( g_pub_tx.reply , &sDd_mst, "ac_id = %ld \
      and ac_seqn = 1", sMdm_ac_rel.ac_id );    
    if ( ret )
    {
      sprintf( acErrMsg,"��ѯ�������ļ�ʧ��[%d][%ld][%s]",
		ret, sMdm_ac_rel.ac_id, sAg_unitinfo.bank_acct_no );
      WRITEMSG
      set_zd_data(DC_REPLY,g_pub_tx.reply);
      sql_rollback();
      return 1;
    }
    
    if ( sMdm_ac_rel.note_sts[0] != '0' || sDd_mst.ac_sts[0] != '1' \
    || sDd_mst.hold_sts[0] == '1' )  /*��������ȫ����*/
    {
      sprintf(acErrMsg,"�˻�״̬������[%s]", sAg_unitinfo.bank_acct_no );
      WRITEMSG
      sql_rollback();
      continue;  /*������ʼ��һ����λ*/
    }        
    
    /*------------ �������ļ� -----------*/
    memset(filename,0x0,sizeof(filename));
    sprintf(filename,"%s/report/%s/BR0%s.txt",\
	getenv("HOME"), sAg_unitinfo.opn_br_no, sAg_unitinfo.unit_no );
    
    fp = fopen( filename,"w" );  /*���ļ�������¼��ϸ������ȷ���*/
    if( fp==NULL )
    {
      sprintf(acErrMsg," open file [%s] error ",filename );
      WRITEMSG
      strcpy( g_pub_tx.reply, "S047" );
      set_zd_data(DC_REPLY,g_pub_tx.reply);
         return 1;
    }
    
    fprintf( fp, "                                 [%s][%s]��������ҵ��ɹ���¼\n\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name );
    fprintf( fp, "�����������������������ө��������������������ө����������������ө��������������������ө��������������������ө���������\n" );
        fprintf( fp, "��%20s��%20s��%16s��%20s��%20s��%8s��\n", "������", "�ͻ�����", "�������", "���֤����", "�˺�", "������" );
        fprintf( fp, "�����������������������㩥�������������������㩥���������������㩥�������������������㩥�������������������㩥��������\n" );
          
    memset( filename1, 0x0, sizeof(filename1) );
    sprintf( filename1, "%s/report/%s/BR1%s.txt", getenv("HOME"), sAg_unitinfo.opn_br_no, sAg_unitinfo.unit_no );
    
    fp1 = fopen( filename1, "w" );/*���ļ�������¼��ϸ��������*/
    if( fp1==NULL )
    {
      sprintf(acErrMsg," open file [%s] error ",filename1 );
      WRITEMSG
      strcpy( g_pub_tx.reply,"S047" );
      fclose( fp );
      set_zd_data(DC_REPLY,g_pub_tx.reply);
         return 1;
    }
          
    fprintf( fp1, "                                [%s][%s]��������ҵ��ʧ�ܼ�¼\n\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name );
    fprintf( fp1, "�����������������������ө��������������������ө����������������ө��������������������ө���������������������������������������������������\n" );
        fprintf( fp1, "��%20s��%20s��%16s��%20s��%50s��\n", "������", "�ͻ�����", "���", "���֤����", "������" );
        fprintf( fp1, "�����������������������㩥�������������������㩥���������������㩥�������������������㩥��������������������������������������������������\n" );
          
          lTrace_no = 0;
          pub_base_PubQlsh_Trans( &lTrace_no );

    ret = Mq_tax_payment_Dec_Sel_hold( g_pub_tx.reply , "src = '%s' \
      and msg_id = 's' and sts = '0'", sMq_tax_head.src, sMq_tax_head.msg_id );
    if ( ret )
    {
      sprintf(acErrMsg,"ִ��Pay_agent_Dec_Sel����[%d]", ret );
      WRITEMSG
      fclose( fp );
      fclose( fp1 );
      set_zd_data(DC_REPLY,g_pub_tx.reply);
         return 1;
    }    
    
    while(1)
    {      
      iErr_flag = 0;
      
      memset( &sMq_tax_payment, 0x0, sizeof(sMq_tax_payment) );
      ret = Mq_tax_payment_Fet_Sel( &sMq_tax_payment , g_pub_tx.reply );
      if ( ret == 100 )
      {
    /* �������� */
        sql_begin();
        break;
      }
      if ( ret )
      {
        sprintf(acErrMsg,"ִ��Pay_agent_Fet_Sel����[%d]", ret );
        WRITEMSG
        goto ErrExit;
      }

      /**----------- ÿ��ѭ�������³�ʼ��ȫ�ֱ��� -------------**/
      memset( &g_pub_tx, 0x00, sizeof(g_pub_tx) );
      pub_base_sysinit();  
      
      g_pub_tx.trace_no = lTrace_no;  /*���¸�ֵΪ���ɵ���ˮ��*/
      set_zd_long( "0040", lTrace_no );
          
      memset( cReturn, 0x0, sizeof(cReturn) );
      memset( xs_acno,0x0,sizeof(xs_acno) );
      
      /*---���׻���Ϊ����λ��������---*/
      strcpy( g_pub_tx.tx_br_no, sAg_unitinfo.opn_br_no );
      set_zd_data( DC_TX_BR_NO,  sAg_unitinfo.opn_br_no );
      
      memset( tmpstr, 0x0, sizeof(tmpstr) );
      get_zd_data( "0030", tmpstr );
      sprintf(acErrMsg,"���������׻���Ϊ[%s][%s]", g_pub_tx.tx_br_no, tmpstr );
      WRITEMSG
    
      ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &sMdm_ac_rel, \
        "ac_no = '%s'", sMq_tax_payment.pay_acct );
      if ( ret)
      {
         sprintf( cReturn,  "�����˺�[%s]��Ϣ��", sMq_tax_payment.pay_acct );
         sprintf( acErrMsg, "�����˺�[%s]��Ϣ��", sMq_tax_payment.pay_acct );
         WRITEMSG
         iErr_flag = 1;
         goto TrLog;
      }
      else {
        memset( &sDd_mst, 0x0, sizeof(sDd_mst) );
        ret1 = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, \
           "ac_id = %ld", sMdm_ac_rel.ac_id );
        if ( ret1 )
        {
          sprintf(cReturn ,"���һ����˺�[%s]��Ϣ��", sMq_tax_payment.pay_acct);
           sprintf(acErrMsg,"���һ����˺�[%s]��Ϣ��", sMq_tax_payment.pay_acct);
          WRITEMSG
          iErr_flag = 1;
          goto TrLog;
        }
          
        pub_base_strpack( sDd_mst.ac_sts );
        /*��������ȫ����*/
        if ( sDd_mst.ac_sts[0] != '1' || sDd_mst.hold_sts[0] == '1' )
        {
          strcpy( cReturn, "�ʻ�״̬��" );
          sprintf(acErrMsg,"�ʻ�״̬��[%s]", sMdm_ac_rel.ac_no);
          WRITEMSG
          iErr_flag = 1;
          goto TrLog;
        }
      }
      sprintf(acErrMsg,"------��ʼ���˻���ȡ��------------");
      WRITEMSG
    
      strcpy( g_pub_tx.ac_no, sAg_peritemmap.ac_no );
      g_pub_tx.ac_seqn = 1;
    
      g_pub_tx.tx_amt1 = sPay_agent.tram;
    
      strcpy(g_pub_tx.add_ind,"0");  /*-- ���� --*/
      memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
      strcpy( g_pub_tx.sub_tx_code, "D003" );
      set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
      strcpy( g_pub_tx.prt_ind, "0" );
      set_zd_data( "102O", "0" );
      strcpy( g_pub_tx.prt_ind, "0" );
      set_zd_data( "102O", "0" );
      set_zd_data( "102J", "01" );
      set_zd_data( "102K", "2" );
      set_zd_double( "102F", g_pub_tx.tx_amt1 );
      set_zd_double( "102P", 0.00 );
      set_zd_double( "102Q", 0.00 );
   
      strcpy(g_pub_tx.brf, "����˰��");
      strcpy(g_pub_tx.ct_ind,"2");
      strcpy(g_pub_tx.ac_wrk_ind,"001001001");
      strcpy(g_pub_tx.hst_ind,"1");
      g_pub_tx.svc_ind=1001;

      sprintf(acErrMsg,"BEG D003[%s][%s][%d]",\
    g_pub_tx.ac_no,g_pub_tx.ac_wrk_ind,g_pub_tx.svc_ind);
      WRITEMSG
      
      sql_commit();
      sql_begin();

      ret = pub_acct_trance();
      if( ret != 0 )
      {
        if ( sAg_unitinfo.opr_attri[0] == '0' )
        {
          sql_rollback();
        }
        sprintf( cReturn, "%s", g_pub_tx.reply );
        sprintf(acErrMsg,"���ô�ȡ������ʧ��[%s]", g_pub_tx.reply );
        WRITEMSG
        iErr_flag = 1;
        goto TrLog;
      }
      
      ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
      if ( ret )
      {
        sql_rollback();
        sprintf( cReturn, "%s", g_pub_tx.reply );
        sprintf(acErrMsg,"���ô�ȡ������ʧ��[%s]", g_pub_tx.reply );
        WRITEMSG
        iErr_flag = 1;
        goto TrLog;
      }
      
TrLog:    
      pub_base_strpack( sPay_agent.name );
      if ( iErr_flag == 0)
      {
        lSnum = lSnum + 1;
        dSamt = dSamt + sPay_agent.tram;

        fprintf( fp,"��%20s��%20s��%16.2lf��%20s��%20s��%8s��\n", sPay_agent.item_no, sPay_agent.name, \
          sPay_agent.tram, sPay_agent.id_no, sPay_agent.ac_no, cReturn );
        
        sql_commit();
      }
      else
      {
        memset( &sRsp_code, 0x0, sizeof(sRsp_code) );
        ret1 = Rsp_code_Sel( g_pub_tx.reply , &sRsp_code, "code = '%s'", cReturn );
        if ( ret1 == 0 )
        {
          memset( cReturn, 0x0, sizeof(cReturn) );
          pub_base_strpack( sRsp_code.memo );
          strcpy( cReturn, sRsp_code.memo );
        }
        
        lFnum = lFnum + 1;
        dFamt = dFamt + sPay_agent.tram;
        fprintf( fp1,"��%20s��%20s��%16.2lf��%20s��%50s��\n", sPay_agent.item_no, sPay_agent.name, \
          sPay_agent.tram, sPay_agent.id_no, cReturn );
      }
    }
    Pay_agent_Clo_Sel();

    fprintf( fp, "�����������������������۩��������������������۩����������������۩��������������������۩��������������������۩���������\n" );
    fprintf( fp1,"�����������������������۩��������������������۩����������������۩��������������������۩���������������������������������������������������\n" );

    fclose(fp);
    fclose(fp1);
    
    if ( lFnum > 0 )  /*��ʧ�ܼ�¼*/
    {
      memset( filename11, 0x0, sizeof(filename11) );
      sprintf( filename11, "%s/report/%s/BR1%s.name", getenv("HOME"), sAg_unitinfo.opn_br_no, sAg_unitinfo.unit_no );
    
      fp11 = fopen( filename11, "w" );/*���ļ�������¼��ϸ��������*/
      if( fp11==NULL )
      {
        sprintf(acErrMsg," open file [%s] error ", filename11 );
        WRITEMSG
        strcpy( g_pub_tx.reply,"S047" );
        fclose( fp );
        set_zd_data(DC_REPLY,g_pub_tx.reply);
      }
      
      fprintf( fp11, "BR1%s.txt ����ҵ��ʧ����ϸ�嵥\n", sAg_unitinfo.unit_no );
      
      fclose( fp11 );
    }
    
    if ( lSnum > 0 )  /*�гɹ���¼*/
    {
      sprintf(acErrMsg,"----------��ʼ���ɵ�λ�˺Ŵ��-----------");
      WRITEMSG
      
      g_pub_tx.ac_id = 0;   /*test clean */
      g_pub_tx.ac_seqn = 0;
      strcpy(g_pub_tx.beg_note_no,"");
            
      strcpy( g_pub_tx.ac_no, tmp_acct );
      g_pub_tx.tx_amt1 = dSamt;
      
      if ( sAg_unitinfo.bank_acct_no[0] == '9' )  /*-- �ڲ��� --*/
      {
        strcpy( g_pub_tx.ac_id_type,"9" ); /*�˻�����Ϊ�ڲ�*/
        strcpy( g_pub_tx.ac_get_ind,"00" ); /*������δ��ȡ�ֻ�*/
        g_pub_tx.svc_ind=9001;
        
        strcpy(g_pub_tx.add_ind,"1");  /*-- ���� --*/
        memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
        strcpy( g_pub_tx.sub_tx_code, "A017" );
        set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
      
        set_zd_data( "1214", "01" );
        set_zd_data( "1215", "2" );
        set_zd_double( "1218", g_pub_tx.tx_amt1 );
          
        strcpy(g_pub_tx.brf, "����˰��");
      }
      else
      {
        strcpy( g_pub_tx.ac_id_type,"1" ); /*�˻�����Ϊ����*/
        strcpy( g_pub_tx.ac_get_ind,"00" ); /*������δ��ȡ�ֻ�*/
        g_pub_tx.svc_ind=1001;
              	
        strcpy(g_pub_tx.add_ind,"1");  /*-- ���� --*/
          
        memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
        strcpy( g_pub_tx.sub_tx_code, "D099" );
        set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
          
        strcpy( g_pub_tx.prt_ind, "0" );
        set_zd_data( "1017", "0" );
            
        set_zd_data( "101A", "01" );
        set_zd_data( "101B", "2" );
        set_zd_double( "1013", g_pub_tx.tx_amt1 );    			
          
        strcpy(g_pub_tx.brf, "����˰��");
      }    
      
      strcpy(g_pub_tx.ct_ind,"2");
      strcpy(g_pub_tx.ac_wrk_ind,"001001001");
      strcpy(g_pub_tx.hst_ind,"1");
      
      ret=pub_acct_trance();
      if( ret )
      {
        sprintf( cReturn, "�Ǵ���λ�˳���[%s]", g_pub_tx.reply );
        sprintf(acErrMsg,"���ô�ȡ������ʧ��[%s]", g_pub_tx.reply );
        WRITEMSG
        sql_rollback();
        continue;
      }
      
      ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
      if ( ret )
      {
        sprintf( cReturn, "�Ǵ���λ�˳���[%s]", g_pub_tx.reply );
        sprintf( acErrMsg, "���ô�ȡ������ʧ��[%s]", g_pub_tx.reply );
        WRITEMSG
        sql_rollback();
        continue;
      }  
                  
      g_pub_tx.ac_id = 0;
      g_pub_tx.ac_seqn = 0;
      g_pub_tx.tx_amt1 = 0.00;        /*������*/
      
      strcpy( g_pub_tx.ac_id_type,"0" ); /*�˻�������Ϊ����*/      
      
      memset( filename01, 0x0, sizeof(filename01) );
      sprintf( filename01, "%s/report/%s/BR0%s.name",
		getenv("HOME"), sAg_unitinfo.opn_br_no, sAg_unitinfo.unit_no );
    
      fp01 = fopen( filename01, "w" );
      if( fp01==NULL )
      {
        sprintf(acErrMsg," open file [%s] error ", filename01 );
        WRITEMSG
        strcpy( g_pub_tx.reply,"S047" );
        fclose( fp );
        set_zd_data(DC_REPLY,g_pub_tx.reply);
      }
      else
      {
        fprintf( fp01, "BR0%s.txt ����ҵ��ɹ���ϸ�嵥\n",
		  sAg_unitinfo.unit_no );
      }      
      
      fclose( fp01 );
      
      memset( filename3, 0x0, sizeof(filename3) );
      sprintf( filename3, "%s/report/%s/BRDWHZ%s.txt",
		getenv("HOME"), sAg_unitinfo.opn_br_no, sAg_unitinfo.unit_no );
      
      fp3 = fopen( filename3, "w" ); /*------��ȷ����������-------*/
      if( fp1==NULL )
      {
        sprintf(acErrMsg," open file [%s] error ",filename3 );
        WRITEMSG
        strcpy( g_pub_tx.reply,"S047" );
      }
      else
      {
        memset( &sCom_branch, 0x0, sizeof(sCom_branch) );
        ret = Com_branch_Sel( g_pub_tx.reply, &sCom_branch, "br_no = '%s'", g_pub_tx.tx_br_no );
        
        pub_base_strpack( sCom_branch.br_name );
        pub_base_strpack( sAg_unitinfo.bank_acct_no );
        fprintf( fp3, "      	[%s]��������λ��Ʊ	\n", sAg_unitinfo.unit_name );
        fprintf( fp3, "������: %s          �������� : %s\n", g_pub_tx.tx_br_no, sCom_branch.br_name );
        fprintf( fp3, "��λ����: %s         ��λ����: %s  \n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name );
        fprintf( fp3, "���� : %ld       ��λ�˺ţ�%s \n", g_pub_tx.tx_date, sAg_unitinfo.bank_acct_no );
        
        fprintf( fp3, "�����������Щ����������������������������������������Щ��������Щ�����������������\n" );
        fprintf( fp3, "����λ��ũ�           ��   λ   ��   ��            ��  ����  ��    ��   ��     ��\n" );
        fprintf( fp3, "�����������੤���������������������������������������੤�������੤����������������\n" );
        fprintf( fp3, "��%8s��%40s��%8ld��%16.2lf��\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name, lSnum, dSamt );
        fprintf( fp3, "�����������੤���������������������������������������੤�������੤����������������\n" );
        fprintf( fp3, "�� ��  �� ��                                        ��%8ld��%16.2lf�� \n", lSnum, dSamt );
        fprintf( fp3, "�����������ة����������������������������������������ة��������ة����������������� \n\n\n\n\n\n\n" );
        
        fprintf( fp3, "      	[%s]����������˻��ܴ�Ʊ	\n", sAg_unitinfo.unit_name );
        fprintf( fp3, "������: %s          �������� : %s\n", g_pub_tx.tx_br_no, sCom_branch.br_name );
        fprintf( fp3, "���� : %ld       ���ܿ�Ŀ �� \n", g_pub_tx.tx_date );
        
        fprintf( fp3, "�����������Щ����������������������������������������Щ��������Щ�����������������\n" );
        fprintf( fp3, "����λ��ũ�           ��   λ   ��   ��            ��  ����  ��    ��   ��     ��\n" );
        fprintf( fp3, "�����������੤���������������������������������������੤�������੤����������������\n" );
        fprintf( fp3, "��%8s��%40s��%8ld��%16.2lf��\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name, lSnum, dSamt );
        fprintf( fp3, "�����������੤���������������������������������������੤�������੤����������������\n" );
        fprintf( fp3, "�� ��  �� ��                                        ��%8ld��%16.2lf�� \n", lSnum, dSamt );
        fprintf( fp3, "�����������ة����������������������������������������ة��������ة����������������� \n" );
      }      
      
      fclose( fp3 );  
      
      memset( filename31, 0x0, sizeof(filename31) );
      sprintf( filename31, "%s/report/%s/BRDWHZ%s.name",
		getenv("HOME"), sAg_unitinfo.opn_br_no, sAg_unitinfo.unit_no );
      
      fp31 = fopen( filename31, "w" );
      if( fp31==NULL )
      {
        sprintf(acErrMsg," open file [%s] error ", filename31 );
        WRITEMSG
        strcpy( g_pub_tx.reply,"S047" );
        fclose( fp );
        set_zd_data(DC_REPLY,g_pub_tx.reply);
      }
      else
      {
        fprintf( fp31, "BRDWHZ%s.txt ����ҵ��ɹ������嵥\n",
        sAg_unitinfo.unit_no );
      }
      fclose( fp31 );
    }
    sql_commit();
    
    lSnum = 0;  /*�ɹ�����*/
    lFnum = 0;  /*ʧ�ܱ���*/
    dSamt = 0.00;  /*�ɹ����*/
    dFamt = 0.00;  /*ʧ�ܽ��*/
  }
  Ag_unitinfo_Clo_Sel();  

OkExit:
  set_zd_data( "DC_TX_BR_NO", "99999" );
  sql_commit();
  strcpy( g_pub_tx.reply, "0000" );
  sprintf( acErrMsg, "������������ҵ����ɹ���ϣ�" );
  WRITEMSG
  set_zd_data( DC_REPLY, g_pub_tx.reply );
  return 0;
ErrExit:
  set_zd_data( "DC_TX_BR_NO", "99999" );
  fclose(fp);
  fclose(fp1);
  Pay_agent_Clo_Sel();
  sql_rollback();
  sprintf(acErrMsg,"������������ҵ����ʧ�ܣ�[%s]", g_pub_tx.reply );
  WRITEMSG
  set_zd_data(DC_REPLY,g_pub_tx.reply);
  return 1;
}
