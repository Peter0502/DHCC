/*************************************************
* �� �� ��: sp9331.c
* ���������������ʺŻ����˻���ż���Ϣ[����ת��ȡ����]
* ��    ��: rob
* ������ڣ�2003��02��19��
* �޸ļ�¼��
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
insert into tx_def values('9331','�˻���ѯ','10000000000000000001111010000001000000101000000000010000000001000001111100000000100000000011100001100100000000000000000100000100','0','4','0','3','');
insert into tx_flow_def values('9331',0,'9331','#$');
insert into tx_sub_def values('9331','�˻���ѯ','sp9331','0','0');
* ע�⣺ ����Ҫ�õ�40���벻Ҫ��40�����κβ�������������������
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include "string.h"
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "com_item_c.h"
#include "cif_id_code_rel_c.h"
#include "mdm_attr_c.h"
#include "gl_mst_c.h"
#include "gl_sub_c.h"
#include "hv_brno_orno_c.h"/**YHBP**/
#include "card.h"

sp9331()
{
     int         i = 0, ret = 0, flag = 0, tag = 0;
     long        ac_seqn=0;
     double      bal=0.00;
     long        cif_no = 0;
     char        ac_no[20], cur_no[5], cif_type[2], note_no[20], name[61], tmp_inf[3];
     char        ac_type[2], tmp_memo[100], tmpname[100], prdt_name[31];
     FILE        *fp;
     char        in_out = '0';     /**0.��Ŀ  �ڲ��� 1.�Թ������ʻ�**/
     double      s_amt=0.00;       /***add by robbit***/
     char        br_no[20];        /***add by robbit***/
     char        spe_ind[21];
     char        opn_br_no[6];     /**YHBP**/


     struct prdt_ac_id_c       prdt_ac_id_c;
     struct cif_id_code_rel_c  g_cif_id_code_rel;
     struct mdm_attr_c         g_mdm_attr;
     struct dic_data_c         vdic_data;
     struct hv_brno_orno_c     hv_brno_orno;   /**YHBP**/
     struct com_item_c         g_com_item;
     struct gl_mst_c           g_gl_mst;
     struct gl_sub_c           g_gl_sub;


     memset(&vdic_data,    0x00 , sizeof(struct dic_data_c));
     memset(&g_mdm_attr,   0x00,sizeof(g_mdm_attr));               /**ADD BY ZHGF 0710 ���߲����Ǹ���֧ʱ����ǰ̨����***/
     memset(&hv_brno_orno, 0x00 , sizeof(struct hv_brno_orno_c));
     memset(&g_gl_sub,     0x00,sizeof(struct gl_sub_c));
     memset(&g_com_item,   0x00, sizeof(struct com_item_c));      /**YHBP**/


     memset(br_no,         0x00, sizeof(br_no));                         /***add by robbit***/
     memset(ac_no,         0x00, sizeof(ac_no));
     memset(cur_no,        0x00, sizeof(cur_no));
     memset(cif_type,      0x00, sizeof(cif_type));
     memset(note_no,       0x00, sizeof(note_no));
     memset(name,          0x00, sizeof(name));
     memset(tmp_inf,       0x00, sizeof(tmp_inf));
     memset(ac_type,       0x00, sizeof(ac_type));
     memset(tmp_memo,      0x00, sizeof(tmp_memo));
     memset(prdt_name,     0x00, sizeof(prdt_name));
     memset(tmpname,       0x00, sizeof(tmpname));
     memset(spe_ind,       0x00, sizeof(spe_ind));
     memset(opn_br_no,     0x00,sizeof(opn_br_no));


     /* add by LiuHuafeng 20091114 */
     pub_base_sysinit();
     /* end by lhf **/

     /** ȡֵ����ֵ **/
     get_zd_data("0030",opn_br_no);
     get_zd_data("1021", ac_no);
     get_zd_data("0020", br_no); /***add by robbit***/

     pub_base_old_acno(ac_no);     /**�Ծ��ʺŵĴ���**/
     set_zd_data("0320",ac_no);    /*���˺Ŵ���󴫵�ǰ̨*/
     vtcp_log("%s,%d�ʺ�Ϊ��[%s]",__FILE__,__LINE__,ac_no);
     get_zd_data("102L", tmp_inf);     /* ժҪ */
     i = strlen(ac_no);
     if (i == 4 || i == 6 || i == 8) {     /* ��Ŀ���ڲ��ʺ� */
             vtcp_log("�Կ�Ŀ�Ĵ���");
             in_out = '0';
             set_zd_data("0690", &in_out);
             /**begin YHBP ���ݻ����Ų�ѯ�к�**/
             ret = Hv_brno_orno_Sel(g_pub_tx.reply,&hv_brno_orno,"br_no='%s'",opn_br_no);
             if ( ret )
             {
                  sprintf(acErrMsg,"�鿪�����кż�¼������!![%s]",ac_no);
                  WRITEMSG
		/***Rm By Yl 20091203 ��Ϊ����ԭ��,�˴�ע��
                  strcpy(g_pub_tx.reply,"M003");
                  goto ErrExit;
		***/
             }
             vtcp_log("�кţ�[%s]",hv_brno_orno.or_br_no);
             /**end YHBP**/
             /* ���ݿ�Ŀ�Ų�ѯ��Ŀ���� */
             ret = Com_item_Sel(g_pub_tx.reply, &g_com_item, "acc_no='%s'", ac_no);
             if (ret == 100) {
                  sprintf(acErrMsg, "�ÿ�Ŀ�Ų�����!!acc_hrt=[%s]", g_in_parm.acc_hrt);
                  WRITEMSG
                  strcpy(g_pub_tx.reply, "O074");
                  goto ErrExit;
             } else if (ret) {
                  sprintf(acErrMsg, "��ѯ������Ŀ���쳣!!ret=[%d]", ret);
                  WRITEMSG
                  strcpy(g_pub_tx.reply, "X007");
                  goto ErrExit;
             }
             if (g_com_item.acc_knd[0] != '3') {
                  sprintf(acErrMsg, "�ÿ�Ŀ�����˻������ӿ�Ŀ!!acc_knd=[%s]", g_com_item.acc_knd);
                  WRITEMSG
                  strcpy(g_pub_tx.reply, "P406");
                  goto ErrExit;
             }
             /***add by robbit*****/
             /* ���ݿ�Ŀ�Ų�ѯ��������� */
             ret = Gl_sub_Sel(g_pub_tx.reply, &g_gl_sub, "acc_hrt='%s' and br_no like '%%%s%%'",g_com_item.acc_hrt, br_no);
             if (ret == 100) {
                  sprintf(acErrMsg, "�ÿ�Ŀ�Ų�����!!acc_hrt=[%s]", g_com_item.acc_hrt);
                  WRITEMSG
                  strcpy(g_pub_tx.reply, "O075");
                  goto ErrExit;
             } else if (ret) {
                  sprintf(acErrMsg, "��ѯ������Ŀ���쳣!!ret=[%d]", ret);
                  WRITEMSG
                  strcpy(g_pub_tx.reply, "X007");
                  goto ErrExit;
             }

             s_amt = 0.00;
             ret = sql_sum_double("dc_log", "amt", &s_amt, "acc_hrt='%s' and tx_opn_br_no like '%%%s%%' and dc_ind = '1'", g_com_item.acc_hrt, br_no);
             if (0 != ret && 100 != ret) {
                  sprintf(acErrMsg, "��ѯdc_log���쳣!!ret=[%d]", ret);
                  WRITEMSG
                  goto ErrExit;
             }
             vtcp_log("%d@%s s_amt %.2lf \n", __LINE__, __FILE__, s_amt);
             g_gl_sub.dr_bal += s_amt;
             s_amt = 0;
             ret = sql_sum_double("dc_log", "amt", &s_amt, "acc_hrt='%s' and tx_opn_br_no like '%%%s%%' and dc_ind != '1'", g_com_item.acc_hrt, br_no);
             if (0 != ret && 100 != ret) {
                  sprintf(acErrMsg, "��ѯdc_log���쳣!!ret=[%d]", ret);
                  WRITEMSG
                  goto ErrExit;
             }
             vtcp_log("[%d][%s] s_amt= [%.2lf] \n", __LINE__, __FILE__, s_amt);
             g_gl_sub.cr_bal += s_amt;
             vtcp_log("[%s][%d]s_amt[%lf]",__FILE__,__LINE__,s_amt);
             /***end by robbit*****/
             set_zd_double("1203", g_gl_sub.dr_bal);     /* ��� */
             /* ��Ŀ������� */
             if (g_com_item.dc_ind[0] == '1') {
                  set_zd_data("0220", "��");
                  g_gl_sub.dr_bal -= g_gl_sub.cr_bal;
                  if (g_gl_sub.dr_bal < 0.005)
                  {
                       g_gl_sub.dr_bal = 0;
                  }
                  set_zd_double("0410", g_gl_sub.dr_bal);
                  set_zd_double("0390", g_gl_sub.dr_bal);     /* ��ǰ�跽��� */
                  set_zd_double("1203", g_gl_sub.dr_bal);     /* ��� */
                  set_zd_double("102I", g_gl_sub.dr_bal);     /* ��� */
             } else if (g_com_item.dc_ind[0] == '2') {
                  set_zd_data("0220", "��");
                  g_gl_sub.cr_bal -= g_gl_sub.dr_bal;
                  if (g_gl_sub.dr_bal < 0.005)
                  {
                       g_gl_sub.cr_bal = 0;
                  }
                  set_zd_double("0410", g_gl_sub.cr_bal);
                  set_zd_double("0390", g_gl_sub.cr_bal);     /* ��ǰ������� */
                  set_zd_double("1203", g_gl_sub.cr_bal);     /* ��� */
                  set_zd_double("102I", g_gl_sub.cr_bal);     /* ��� */
             } else if (g_com_item.dc_ind[0] == '0') {        /* ˫�� */
                  ret = pub_base_CompDblVal(g_gl_sub.dr_bal, g_gl_sub.cr_bal);
                  if (ret == 1) {
                       set_zd_data("0220", "��");
                       g_gl_sub.dr_bal -= g_gl_sub.cr_bal;
                       set_zd_double("0410", g_gl_sub.dr_bal);
                       set_zd_double("0390", g_gl_sub.dr_bal);     /* ��ǰ�跽��� */
                       set_zd_double("1203", g_gl_sub.dr_bal);     /* ��� */
                       set_zd_double("102I", g_gl_sub.dr_bal);     /* ��� */
                  } else if (ret == -1) {
                       set_zd_data("0220", "��");
                       g_gl_sub.cr_bal -= g_gl_sub.dr_bal;
                       set_zd_double("0410", g_gl_sub.cr_bal);
                       set_zd_double("0390", g_gl_sub.cr_bal);     /* ��ǰ������� */
                       set_zd_double("1203", g_gl_sub.cr_bal);     /* ��� */
                       set_zd_double("102I", g_gl_sub.cr_bal);     /* ��� */
                  }
             }
             set_zd_data("0250", g_com_item.acc_name);             /* ���� */
             set_zd_data("0210", g_gl_sub.cur_no);                 /* ���� */

             /**������FD120�ĸ�ֵ**/
             set_zd_data("1201", ac_no);
             set_zd_data("1202", g_com_item.acc_name);             /* ���� */
             /* 1203������������ */
             set_zd_data("1204", g_gl_sub.cur_no);                 /* ���� */
             set_zd_data("1205", "2");                             /* ��ת��־:2 ת 1 �� */

             /** add.begin.gyf.20060920 Ϊ�˺ͼ���ʻ�ʱ����һ�£��������Ӷ�102�򲿷���Ϣ��ֵ **/
             set_zd_data("102H", g_com_item.acc_name);
             set_zd_data("102J", g_gl_sub.cur_no);                 /* ���� */
             set_zd_data("102K", "2");                             /* ��ת��־:2 ת 1 �� */
             strcpy(cif_type, "2");                                /* ���Թ������ʹ��� */
             strcpy(ac_type, cif_type);
             set_zd_data("102M", ac_type);
             set_zd_data("1029", "N");
             set_zd_data("1026", "N");
             strcpy(prdt_name, "��Ŀ��:");
             strcat(prdt_name, g_com_item.acc_name);
             set_zd_data("102V", prdt_name);
             /** add.end.gyf.20060920 **/

             set_zd_data("0520",hv_brno_orno.or_br_no);             /*YHBP �����к� */
     } else {
          ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'", ac_no);
          if (ret) {
               sprintf(acErrMsg, "�����ʺŶ��ձ����ڸü�¼!![%s]", ac_no);
               WRITEMSG
               strcpy(g_pub_tx.reply, "M003");
               goto ErrExit;
          }
          /**begin YHBP ���ݻ����Ų��к�**/
          ret = Hv_brno_orno_Sel(g_pub_tx.reply,&hv_brno_orno,"br_no='%s'",g_mdm_ac_rel.opn_br_no);
          if ( ret )
          {
               sprintf(acErrMsg,"�鿪�����кż�¼������!![%s]",ac_no);
               WRITEMSG
		/* ��Ϊû��ȫ����ͨ���Բ鲻������ȷ��
               strcpy(g_pub_tx.reply,"M003");
               goto ErrExit;
		*/
          }
          vtcp_log("�к�: [%s]",hv_brno_orno.or_br_no);
          if(!memcmp(ac_no,CARDHEADCODE,6))
          {
                ret = Prdt_ac_id_Sel(g_pub_tx.reply, &prdt_ac_id_c, "ac_id=%ld and ac_id_type='1'",g_mdm_ac_rel.ac_id);
                if (ret)
                {
                     sprintf(acErrMsg, "��Ʒ�ʺŶ��ձ����ڸü�¼!! [%d]", ret);
                     WRITEMSG
                     strcpy(g_pub_tx.reply, "M003");
                     goto ErrExit;
                }
                in_out = '2';
                set_zd_data("0690", &in_out);
                vtcp_log("��������");
                vtcp_log("in_out is [%c]", in_out);
          }
          else
          {
                /**end YHBP ����**/
                ret = Prdt_ac_id_Sel(g_pub_tx.reply, &prdt_ac_id_c, "ac_id=%ld order by ac_seqn", g_mdm_ac_rel.ac_id);
                if (ret) {
                     sprintf(acErrMsg, "��Ʒ�ʺŶ��ձ����ڸü�¼!! [%d]", ret);
                     WRITEMSG
                     strcpy(g_pub_tx.reply, "M003");
                     goto ErrExit;
                }
                if (prdt_ac_id_c.ac_id_type[0] == '1')
                {
                     in_out = '2';
                     set_zd_data("0690", &in_out);
                } else if (prdt_ac_id_c.ac_id_type[0] == '9') {
                     in_out = '1';
                     set_zd_data("0690", &in_out);
                }
                vtcp_log("in_out is [%c]", in_out);
          }

          /** delete.begin.gyf.20060920
          if( (prdt_ac_id_c.prdt_no[0]!='1' && prdt_ac_id_c.prdt_no[0]!='2') )
          delete.end.gyf.20060920 **/

          /** modify.gyf.20060920 ����ת�ʶ��ڲ��ʻ���֧�� **/
          if ((prdt_ac_id_c.prdt_no[0] != '1' && prdt_ac_id_c.prdt_no[0] != '2') && prdt_ac_id_c.prdt_no[0] != '9')
          {
               sprintf(acErrMsg, "�Ǵ�ȡ�����˺�! [%d]", ret);
               WRITEMSG
               strcpy(g_pub_tx.reply, "O166");
               goto ErrExit;
          }
          set_zd_data("0930",prdt_ac_id_c.ac_id_type);                    /*add by wyw 20080225*/
          ret = Mdm_attr_Sel(g_pub_tx.reply, &g_mdm_attr, "mdm_code='%s'", g_mdm_ac_rel.mdm_code);
          if (ret == 100) {
               sprintf(acErrMsg, "���ʲ�����! ");
               WRITEMSG
               strcpy(g_pub_tx.reply, "M003");
               goto ErrExit;
          } else if (ret) {
               sprintf(acErrMsg, "���ҳ���! ");
               WRITEMSG
               strcpy(g_pub_tx.reply, "D103");
               goto ErrExit;
          }
          /*** ���˻����ʵ���� ***/
          if(memcmp(ac_no,CARDHEADCODE,6))
          {
               if (g_mdm_attr.ac_num_ind[0] == 'N' || !memcmp(g_mdm_attr.mdm_code,"0024",4) || !memcmp(g_mdm_attr.mdm_code,"0028",4) || !memcmp(g_mdm_attr.mdm_code,"0029",4) || !memcmp(g_mdm_attr.mdm_code,"0031",4) || !memcmp(g_mdm_attr.mdm_code,"0032",4))
               {     /*����˫�������⴦�� 0024*/
                     ret = sql_count("prdt_ac_id", "ac_id=%ld", g_mdm_ac_rel.ac_id);
                     if (ret < 0) {
                          sprintf(acErrMsg, "sql_count error!! [%d]", ret);
                          WRITEMSG
                          goto ErrExit;
                     } else if (ret == 1) {
                          ac_seqn = prdt_ac_id_c.ac_seqn;
                     } else if (ret > 1) {
                          set_zd_long("102N", prdt_ac_id_c.ac_seqn);
                          ac_seqn = prdt_ac_id_c.ac_seqn;
                          set_zd_long("1022", ac_seqn);
                          /**tag = 1; **/
                          /* ���Ƕ��˻�����ȡ���ڵ���� */
                     }
                }
                else
                {
                     ac_seqn = g_mdm_ac_rel.ac_seqn;
                }
          }
          else
          {
                ac_seqn=prdt_ac_id_c.ac_seqn;
          }
          set_zd_long("1022", ac_seqn);       /*add by wyw 20080223*/
          /** ADD BY ������ �������֤�˾Ͳ������뿨�����˽���,���������ж� 2008-4-5 **/
          vtcp_log("%s,%d,mdm_code=[%s]tag=[%d]\n",__FILE__,__LINE__,g_mdm_ac_rel.mdm_code,tag);
          /**�Կ�״̬����ȥ�� 
          if(!memcmp(g_mdm_ac_rel.mdm_code, "0020", sizeof(g_mdm_ac_rel.mdm_code)-1) || !memcmp(g_mdm_ac_rel.mdm_code, "0024", sizeof(g_mdm_ac_rel.mdm_code)-1))
          {
                if (g_mdm_ac_rel.note_sts[0] != '0')
                {
                     sprintf(acErrMsg, "����״̬������! [%d]", ret);
                     WRITEMSG
                     strcpy(g_pub_tx.reply, "M022");
                     set_zd_data("0200" , g_mdm_ac_rel.note_sts);
                     goto ErrExit;
                }
          }
          ***/
          if (tag == 0)
          {
               set_zd_data("0230",prdt_ac_id_c.prdt_no);

               /** begin.gyf.20060920 ****************************
               if( prdt_ac_id_c.prdt_no[0]=='1' )
               {
                    ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst, "ac_id=%ld and ac_seqn=%ld",prdt_ac_id_c.ac_id,ac_seqn);
                    if( ret )
                    {
                         sprintf(acErrMsg,"read dd_mst error" );
                         WRITEMSG
                         goto ErrExit;
                    }
                    strcpy(name,g_dd_mst.name);
                    cif_no = g_dd_mst.cif_no;
                    bal = g_dd_mst.bal;
                    if(g_dd_mst.ac_sts[0]=='*')
                    {
                         sprintf(acErrMsg,"���˺�������!! [%d]",ret);
                         WRITEMSG
                         strcpy(g_pub_tx.reply,"P245");
                         goto ErrExit;
                    }
                    ret = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'",g_dd_mst.prdt_no);
                    if( ret )
                    {
                         sprintf(acErrMsg,"read dd_mst error" );
                         WRITEMSG
                         goto ErrExit;
                    }
                    strcpy(prdt_name,g_dd_parm.title);
                    strcpy(cur_no,g_dd_parm.cur_no);
                    strcpy(cif_type,g_dd_parm.cif_type);
               }
               else
               {
                    sprintf(acErrMsg,"����������ʺţ������˻�������ת�ʷ�![%s]",prdt_ac_id_c.prdt_no);
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "O227");
                    goto ErrExit;
               }

               strcpy(ac_type,cif_type);
               set_zd_data("102M",ac_type);
               ret = Cif_id_code_rel_Sel( g_pub_tx.reply,&g_cif_id_code_rel,"cif_no=%ld",cif_no);
               if( ret )
               {
                    sprintf(acErrMsg,"��ѯcif_id_code_rel����![%d][%ld]",ret,cif_no);
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "G102");
                    goto ErrExit;
               }
               **************************************************end.gyf.20060920 ***/

               /** add.begin.gyf.20060920 ���Ӷ��ڲ��ʻ���֧�� **/
               switch (prdt_ac_id_c.prdt_no[0])
               {
                     case '1':
                          ret = Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst, "ac_id=%ld and ac_seqn=%ld",prdt_ac_id_c.ac_id, ac_seqn);
                          if (ret)
                          {
                               sprintf(acErrMsg, "read dd_mst error");
                               WRITEMSG
                               goto ErrExit;
                          }
                          vtcp_log("��������ѽ");
                          strcpy(name, g_dd_mst.name);
                          cif_no = g_dd_mst.cif_no;
                          bal = g_dd_mst.bal;

                          if (g_dd_mst.ac_sts[0] == '*')
                          {
                               sprintf(acErrMsg, "���˺�������!! [%d]", ret);
                               WRITEMSG
                               strcpy(g_pub_tx.reply, "P245");
                               goto ErrExit;
                          }
                          ret = Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm,"prdt_no='%s'", g_dd_mst.prdt_no);
                          if (ret)
                          {
                               sprintf(acErrMsg, "read dd_mst error");
                               WRITEMSG
                               goto ErrExit;
                          }
                          strcpy(prdt_name, g_dd_parm.title);
                          strcpy(cur_no, g_dd_parm.cur_no);
                          strcpy(cif_type, g_dd_parm.cif_type);
                          vtcp_log("%s,%d,111111\n",__FILE__,__LINE__);

                          strcpy(ac_type, cif_type);
                          set_zd_data("102M", ac_type);
                          set_zd_data("0990",g_dd_mst.ac_sts);           /*add by chbo 20091109*/
                          set_zd_data("0700",g_dd_mst.sttl_type);        /*������ǽ��㻧*/
                          set_zd_data("0710",g_dd_mst.hold_sts);         /*add by chbo 20091109*/
                          set_zd_data("0910", g_dd_mst.opn_br_no);       /*add by wyw 20080310*/
                          set_zd_double("0430",g_dd_mst.bal);		 /*Add By Yl 2009-12-16 1:15:28*/

                          /****��գ�����  20070801
                          ret = Cif_id_code_rel_Sel( g_pub_tx.reply,&g_cif_id_code_rel, "cif_no=%ld",cif_no);
                          if( ret )
                          {
                               sprintf(acErrMsg,"��ѯcif_id_code_rel����![%d][%ld]",ret,cif_no);
                               WRITEMSG
                               strcpy(g_pub_tx.reply, "G102");
                               goto ErrExit;
                          }
                          ****/
                          break;
                     case '2':
                          ret = Td_mst_Sel(g_pub_tx.reply, &g_td_mst, "ac_id=%ld and ac_seqn=%ld",prdt_ac_id_c.ac_id, ac_seqn);
                          if (ret)
                          {
                               sprintf(acErrMsg, "read dd_mst error");
                               WRITEMSG
                               goto ErrExit;
                          }
                          vtcp_log("�ֵ�������");
                          strcpy(name, g_td_mst.name);
                          cif_no = g_td_mst.cif_no;
                          bal = g_td_mst.bal;

                          if (g_td_mst.ac_sts[0] == '*')
                          {
                               sprintf(acErrMsg, "���˺�������!! [%d]", ret);
                               WRITEMSG
                               strcpy(g_pub_tx.reply, "P245");
                               goto ErrExit;
                          }
                          /*add by wyw 20080424 ������ȫ���᷵�ر�־ */
                          if(g_td_mst.hold_sts[0] =='1' || g_td_mst.hold_sts[0]=='2')
                          {
                               /*����״̬ǰִ̨��1�Ա�ǰ̨�ж�*/
                               set_zd_data("0720","1");
                          }
                          /*add by wyw 20080424 ������ȫ���᷵�ر�־*/
                          ret = Td_parm_Sel(g_pub_tx.reply, &g_td_parm,"prdt_no='%s'", g_td_mst.prdt_no);
                          if (ret)
                          {
                               sprintf(acErrMsg, "read dd_mst error");
                               WRITEMSG
                               goto ErrExit;
                          }
                          strcpy(prdt_name, g_td_parm.title);
                          strcpy(cur_no, g_td_parm.cur_no);
                          strcpy(cif_type, g_td_parm.cif_type);
                          strcpy(ac_type, cif_type);
                          set_zd_data("0910", g_td_mst.opn_br_no);
                          set_zd_data("102M", ac_type);
                          set_zd_double("0430",g_td_mst.bal);		/*Add By Yl 2009-12-16 1:15:28*/
                          /****��գ����� 20080701
                          ret = Cif_id_code_rel_Sel( g_pub_tx.reply,&g_cif_id_code_rel,"cif_no=%ld",cif_no);
                          if( ret )
                          {
                               sprintf(acErrMsg,"��ѯcif_id_code_rel����![%d][%ld]",ret,cif_no);
                               WRITEMSG
                               strcpy(g_pub_tx.reply, "G102");
                               goto ErrExit;
                          }
                          ****/
                          break;
                     case '9':
                          ret = In_mst_Sel(g_pub_tx.reply, &g_in_mst,  "ac_id=%ld and ac_seqn=%ld", prdt_ac_id_c.ac_id, ac_seqn);
                          if (ret)
                          {
                               sprintf(acErrMsg, "read dd_mst error");
                               WRITEMSG
                               goto ErrExit;
                          }
                          strcpy(name, g_in_mst.name);
                          bal = g_in_mst.bal;

                          if (g_in_mst.sts[0] == '*')
                          {
                               sprintf(acErrMsg, "���˺�������!! [%d]", ret);
                               WRITEMSG
                               strcpy(g_pub_tx.reply, "P245");
                               goto ErrExit;
                          }
                          ret = In_parm_Sel(g_pub_tx.reply, &g_in_parm,"prdt_no='%s'", g_in_mst.prdt_no);
                          if (ret)
                          {
                               sprintf(acErrMsg, "read dd_mst error");
                               WRITEMSG
                               goto ErrExit;
                          }
                          strcpy(prdt_name, g_in_parm.prdt_mo);
                          strcpy(cur_no, g_in_parm.cur_no);
                          strcpy(cif_type, "2");                       /** ���Թ������ʹ��� **/
                          strcpy(ac_type, cif_type);
                          set_zd_data("102M", ac_type);
                          set_zd_data("0910", g_in_mst.opn_br_no);
                          g_mdm_ac_rel.draw_id_yn[0] = 'N';            /** ��Ϊ�ڲ��ʻ�û��ֵ�����Ը�һ�� **/
                          set_zd_double("0430",g_in_mst.bal);		 /*Add By Yl 2009-12-16 1:15:28*/
                          break;
                     default:
                          sprintf(acErrMsg, "����������ʺţ������˻�������ת�ʷ�![%s]",prdt_ac_id_c.prdt_no);
                          WRITEMSG
                          strcpy(g_pub_tx.reply, "O227");
                          goto ErrExit;
               }
               /*** add.end.gyf.20060920 ***/
               vtcp_log("[%s][%d]name==[%s]", __FILE__, __LINE__, name);
               pub_base_strpack(name);
               vtcp_log("[%s][%d]name==[%s]", __FILE__, __LINE__, name);
               set_zd_data("0250", name);
               vtcp_log("[%s][%d]name==[%s]", __FILE__, __LINE__, name);
               set_zd_double("0390", bal);

		/************************�Թ����۴�������ModByYl20091210*/
               	/*set_zd_data("102C", "N");                                    �Ƿ�֧�������� */
		/*����ǶԹ�������Ҫ��������*/
		if( memcmp(g_mdm_ac_rel.mdm_code,"2010",4) )
		{
               		set_zd_data("102C", "N");                                   /* �Ƿ�֧�������� */
		}else{
               		set_zd_data("102C", g_mdm_ac_rel.draw_pwd_yn);              /* �Ƿ�֧�������� */
		}
		/************************�Թ����۴�������ModByYl20091210*/

               set_zd_data("102H", name);                                  /* ���� */
               vtcp_log("[%s][%d]name==[%s]", __FILE__, __LINE__, name);
               set_zd_data("102V", prdt_name);                             /* ��Ʒ���� */
               set_zd_data("102J", cur_no);                                /* ���� */
               set_zd_data("102K", "1");                                   /* Ĭ����ת */
               set_zd_data("0680", "1");
               set_zd_data("0920",g_mdm_ac_rel.mdm_code);                  /*add by chbo 20091109*/
               set_zd_data("0980",g_mdm_ac_rel.note_sts);                  /*add by chbo 20091109*/
               set_zd_double("102I", bal);                                 /* ��� */
               set_zd_data("1024", g_mdm_ac_rel.note_no);                  /* ƾ֤���� */
               set_zd_data("1025", g_mdm_ac_rel.draw_uncon_yn);            /* �Ƿ�����֧ȡ */
               set_zd_data("1026", g_mdm_ac_rel.draw_pwd_yn);              /* ����֧ȡ */
               set_zd_data("1027", g_mdm_ac_rel.qry_pwd);                  /* ��ѯ���� */
               vtcp_log("[%d][%s]SPE_IND[%s]", __LINE__,__FILE__, spe_ind);
               set_zd_data("1028", g_mdm_ac_rel.draw_pwd);                 /* ֧ȡ���� */
               set_zd_data("102U", spe_ind);
               /* �����־ */
               set_zd_data("102B", g_mdm_ac_rel.draw_seal_yn);             /* �Ƿ�ƾӡ��֧ȡ */

               if (flag == 1 && g_mdm_ac_rel.draw_id_yn[0] == 'N')
               {
                    set_zd_data("1029", "N");                              /* ֤��֧ȡ */
                    set_zd_data("102G", g_cif_id_code_rel.id_type);        /* ֤������ */
                    set_zd_data("102A", g_cif_id_code_rel.id_no);          /* ֤������ */
               } else if (flag == 1) {
                    set_zd_data("1029", "Y");                              /* ֤��֧ȡ */
                    set_zd_data("102G", g_mdm_ac_rel.id_type);             /* ֤������ */
                    set_zd_data("102A", g_mdm_ac_rel.id_no);               /* ֤������ */
               } else {
                    set_zd_data("1029", g_mdm_ac_rel.draw_id_yn);          /* ֤��֧ȡ */
                    set_zd_data("102G", g_mdm_ac_rel.id_type);             /* ֤������ */
                    set_zd_data("102A", g_mdm_ac_rel.id_no);               /* ֤������ */
               }
               set_zd_data("0250", name);
               set_zd_data("0620", g_mdm_ac_rel.id_no);
               set_zd_data("0520",hv_brno_orno.or_br_no);                  /**YHBP �����к� **/
          }
     }

     /***��ʼ���ز˵�***/
     pub_base_AllDwnFilName(tmpname);
     fp = fopen(tmpname, "a");

     if (ac_type[0] == '2')
     {
          if (strcmp(tmp_inf, "01"))
          {     /* �г�ֵ */
               ret = Dic_data_Sel(g_pub_tx.reply, &vdic_data, "pr_code='tran_memo' and pr_seq='%s' ", tmp_inf);
               if (ret == 100) {
                    strcpy(tmp_inf, "01");
               } else if (ret) {
                    goto ErrExit;
               } else {
                    pub_base_strpack(vdic_data.pr_seq);
                    pub_base_strpack(vdic_data.content);
                    set_zd_data("102L", vdic_data.pr_seq);
                    fprintf(fp, "102L|%-20s|%s|\n", vdic_data.content, vdic_data.pr_seq);
               }
          }
          if (strcmp(tmp_inf, "01") == 0)
          {
               /* ���޳�ֵ */
               /***���ضԹ�ժҪ�б�***/
               ret = Dic_data_Dec_Sel(g_pub_tx.reply, "pr_code='tran_memo'");
               if (ret)
               {
                    goto ErrExit;
               }
               while (1) {
                    ret = Dic_data_Fet_Sel(&vdic_data, g_pub_tx.reply);
                    if (ret == 100)
                         break;
                    if (ret)
                         goto ErrExit;

                    pub_base_strpack(vdic_data.pr_seq);
                    pub_base_strpack(vdic_data.content);
                    if (i == 0) {
                         set_zd_data("102L", vdic_data.pr_seq);
                         strcpy(tmp_memo, vdic_data.pr_seq);
                    }
                    fprintf(fp, "102L|%-20s|%s|\n", vdic_data.content, vdic_data.pr_seq);
                    i++;
               }
               Dic_data_Clo_Sel();
          }
     }
     if (strcmp(g_mdm_ac_rel.mdm_code, NULL_MDM_CODE) && ac_type[0] == '1') {
          memset(&g_mdm_attr, 0x00, sizeof(struct mdm_attr_c));
          ret = Mdm_attr_Sel(g_pub_tx.reply, &g_mdm_attr, "mdm_code='%s'", g_mdm_ac_rel.mdm_code);
          if (ret)
          {
               sprintf(acErrMsg, "��ѯ�������Ա����![%d]", ret);
               WRITEMSG
               strcpy(g_pub_tx.reply, "B004");
               goto ErrExit;
          }
          strcpy(note_no, g_mdm_attr.note_type);

          /***ѡ���б�***/
          ret = Dic_data_Dec_Sel(g_pub_tx.reply, "pr_code='per_dety' and pr_seq='%s'", note_no);
          if (ret)
               goto ErrExit;
          i = 0;
          while (1) {
               ret = Dic_data_Fet_Sel(&vdic_data, g_pub_tx.reply);
               if (ret == 100)
                    break;
               if (ret)
                    goto ErrExit;

               pub_base_strpack(vdic_data.pr_seq);
               pub_base_strpack(vdic_data.content);

               if (i == 0) {
                    set_zd_data("1023", vdic_data.pr_seq);
                    strcpy(note_no, vdic_data.pr_seq);
               }
               fprintf(fp, "1023|%-20s|%s|\n", vdic_data.content, vdic_data.pr_seq);
               i++;
          }
          Dic_data_Clo_Sel();
     } else {
          if (ac_type[0] == '2') {     /* ���ضԹ�ȡ��ƾ֤���� */
               /***ѡ���б�***/
               i = 0;
               ret = Dic_data_Dec_Sel(g_pub_tx.reply, \
                                "pr_code='pub_taty' and pr_seq[1,3]<>'001' order by pr_seq");
               if (ret)
                    goto ErrExit;

               while (1) {
                    ret = Dic_data_Fet_Sel(&vdic_data, g_pub_tx.reply);
                    if (ret == 100)
                         break;
                    if (ret)
                         goto ErrExit;

                    pub_base_strpack(vdic_data.pr_seq);
                    pub_base_strpack(vdic_data.content);

                    if (i == 0) {
                         strcpy(note_no, vdic_data.pr_seq);
                         set_zd_data("1023", vdic_data.pr_seq);
                    }
                    fprintf(fp, "1023|%-20s|%s|\n", vdic_data.content, vdic_data.pr_seq);
                    i++;
               }
               Dic_data_Clo_Sel();
          }
     }
     fclose(fp);
     set_zd_data(DC_FILE_SND, "2");
     /***ѡ���б��������***/

     set_zd_data("1021", ac_no);
     set_zd_data("1023",g_mdm_attr.note_type);/* add by wyw 20080222*/

     double          tmp_bal = 0;
     get_zd_double("0410", &tmp_bal);
     if (tmp_bal < 0) {
          vtcp_log("!!!!!!ע��  ���[%.2f]С��0\n", tmp_bal);
          tmp_bal = 0;
          set_zd_double("0410", tmp_bal);
     }
     tmp_bal = 0;
     get_zd_double("0390", &tmp_bal);
     if (tmp_bal < 0) {
          vtcp_log("!!!!!!ע��  ���[%.2f]С��0\n", tmp_bal);
          tmp_bal = 0;
          set_zd_double("0390", tmp_bal);
     }
     tmp_bal = 0;
     get_zd_double("102I", &tmp_bal);
     if (tmp_bal < 0) {
          vtcp_log("!!!!!!ע��  ���[%.2f]С��0\n", tmp_bal);
          tmp_bal = 0;
          set_zd_double("102I", tmp_bal);
     }
OkExit:
     strcpy(g_pub_tx.reply, "0000");
     sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
     WRITEMSG
          set_zd_data(DC_REPLY, g_pub_tx.reply);
     return 0;
ErrExit:
     sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
     WRITEMSG
          set_zd_data(DC_REPLY, g_pub_tx.reply);
     return 1;
}
