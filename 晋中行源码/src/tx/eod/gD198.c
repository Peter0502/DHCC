/*************************************************************
* �� �� ��: gD198.c
* ���������������ڶԹ�������ֶλ�����,��ֹ���ھ���dd_mst.lst_date, com_sys_parm.sys_date
*           ����=dd_mst.intst_acm+dd_mst.bal*(com_sys_parm.sys_date-dd_mst.lst_date)
* ��    ��: shaoliguang
* �������: 20110214
* �޸ļ�¼: �̶����ʵĻ�(cal_code='X')�ͱ�֤����ڴ���(prdt_no=131)��ֻ������Ϣ��(intst_type=4)
*      ��    ��:
*      �� �� ��:
*      �޸�����:
**************************************************************/
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#define EXTERN
#define DEF_COMMITCNT 1000
#define INTST_DAY_TYPE  "0"   /* ��Ϣ�������� 0.ʵ������ 1.30��  */
#include "public.h"
#include "bitmap_c.h"
#include "com_sys_parm_c.h"
#include "dd_mst_c.h"
#include "com_sect_acm_c.h"
#include "dd_parm_c.h"
#include "sub_dd_mst_c.h"
#include "sub_com_sect_acm_c.h"

int gD198()
{
  int        ret = 0;                /*����������*/
  double     cRate=0.00;              /*����ǰ����*/
  char       wherelist[1024];
	int        iCommitCnt;
	long       days;
	double     tmp_bal;
	long       totnum;
  struct dd_mst_c        sDd_mst;
  struct dd_parm_c       sDd_parm;
  struct com_sect_acm_c  sCom_sect_acm;
  struct com_sys_parm_c  sCom_sys_parm;
	struct sub_dd_mst_c    sub_dd_mst;
	struct sub_com_sect_acm_c sub_com_sect_acm;

  memset(wherelist,    0x00,sizeof(wherelist));
  memset(&sDd_parm,    0x00,sizeof(sDd_parm));
  memset(&sCom_sys_parm,  0x00,sizeof(sCom_sys_parm));
  memset(&sDd_mst,    0x00,sizeof(sDd_mst));
  memset(&sCom_sect_acm,  0x00,sizeof(sCom_sect_acm));
	memset(&sub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));
	memset(&sub_com_sect_acm,0x00,sizeof(struct sub_com_sect_acm_c));

    ret = sql_begin();          /* ������ */
    if (ret != 0)
    {
        sprintf(acErrMsg, "������ʧ��!!!");
        WRITEMSG
        goto ErrExit;
    }
  /** ��ʼ��ȫ�ֱ��� **/
    pub_base_sysinit();
    ret = Com_sys_parm_Sel(g_pub_tx.reply,&sCom_sys_parm, "1=1");
    if ( ret )
    {
        sprintf(acErrMsg,"��ѯϵͳ�����!");
        WRITEMSG
        goto ErrExit;
    }
   memset(wherelist,0x00,sizeof(wherelist));
   strcat(wherelist,"prdt_no not in('131') and ac_sts='1' and intst_type='4' and ac_type!='5' and ac_type!='7' order by opn_br_no,ac_id ");

  vtcp_log( "WHERE [%s]",wherelist );
  ret= Dd_mst_Dec_Sel( g_pub_tx.reply, wherelist );
    if ( ret )
    {
        sprintf(acErrMsg,"�����α����ret=[%d]!",ret);
        WRITEMSG
        goto ErrExit;
    }
  while(1)
  {
    memset(&sDd_mst,    0x00,sizeof(sDd_mst));
    memset(&sCom_sect_acm,  0x00,sizeof(sCom_sect_acm));
    ret = Dd_mst_Fet_Sel( &sDd_mst,g_pub_tx.reply );
    if ( ret == 100 )
        break;
    else if ( ret)
     {
       sprintf( acErrMsg, "ִ��Dd_mst_Fet_Sel����!ret=[%d]",ret );
       WRITEMSG
       goto ErrExit;
     }
		if(sDd_mst.cal_code[0]=='X')	/* �̶����ʻ� */
				continue;

      /*������Ϣ����*/
    ret=pub_base_CalTrueDays(sDd_mst.lst_date,sCom_sys_parm.sys_date);
    if(ret<0)
    {
        sprintf(acErrMsg,"�������ڴ���!![%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    sCom_sect_acm.intst_acm=sDd_mst.intst_acm+sDd_mst.bal*ret;
    if(sCom_sect_acm.intst_acm<0.005)
    	continue;

     /*�ǼǷֶλ�����*/
    memset(&sDd_parm,0x0,sizeof(struct dd_parm_c));
    ret=Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no='%s'",sDd_mst.prdt_no);
    if(ret)
    {
        sprintf(acErrMsg, "�ӻ��ڲ�Ʒ������ȡֵ����![%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "A020");
        goto ErrExit;
    }

    pub_base_getllz(sDd_parm.rate_no,"01",sCom_sys_parm.lst_date,&cRate);  /*ȡ����*/
    if (ret)
        {
            sprintf(acErrMsg, "+++++++++++++++++[%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
		sCom_sect_acm.rate=cRate;
    sCom_sect_acm.ac_id=sDd_mst.ac_id;
    sCom_sect_acm.ac_seqn=sDd_mst.ac_seqn;
    sCom_sect_acm.beg_date=sDd_mst.ic_date;
    sCom_sect_acm.end_date=sCom_sys_parm.sys_date;

    sCom_sect_acm.intst_type[0]='0';
    sCom_sect_acm.sts[0]='0';
    ret = Com_sect_acm_Ins(sCom_sect_acm, g_pub_tx.reply);
    if (ret != 0)
    {
      sprintf(acErrMsg, "��ֶλ������в����¼����!![%d]",ret);
      WRITEMSG
      strcpy(g_pub_tx.reply,"C240");
      goto ErrExit;
    }
    /***��������***/
    ret = sql_execute2("update dd_mst set lst_date=%ld ,intst_acm=0.00 where ac_id=%ld and ac_seqn=%d",sCom_sys_parm.sys_date,sDd_mst.ac_id,sDd_mst.ac_seqn);
    if(ret!=0)
     {
       sprintf(acErrMsg, "���»�������ʧ��!![%d]", ret);
       WRITEMSG
       strcpy(g_pub_tx.reply, "O012");
       goto ErrExit;
     }
  }
  Dd_mst_Clo_Sel();

    /*������Թ��˻���ʼ��������˻� �����˻�Ҳ��Ҫ�ֶμ�Ϣ*/
    iCommitCnt = 0;
    ret = Sub_dd_mst_Dec_Sel(g_pub_tx.reply, " intst_type<>'0' and ac_sts<>'*'");
    if(ret)
    {
        sprintf(acErrMsg, "��ѯ�����˻�����ʧ��![%d]\n", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "D101");
        goto ErrExit;
    }
    while(1)
    {
        if(iCommitCnt == DEF_COMMITCNT) /*1000���ύһ��*/
        {
            sql_commit();
            iCommitCnt = 0;
        }
        memset(&sub_dd_mst, 0, sizeof(sub_dd_mst));
        ret = Sub_dd_mst_Fet_Sel(&sub_dd_mst, g_pub_tx.reply);
        if(ret == 100 || ret == 1403)
        {
            break;
        }
        else if(ret)
        {
            vtcp_log("%s,%d,��ѯ�����˻�ʧ��!", __FILE__, __LINE__);
            goto ErrExit;
        }
        memset(&sub_com_sect_acm, 0, sizeof(sub_com_sect_acm));
        memcpy(sub_com_sect_acm.ac_no, sub_dd_mst.ac_no, sizeof(sub_com_sect_acm.ac_no) - 1);
        memcpy(sub_com_sect_acm.sub_ac_no, sub_dd_mst.ac_no, sizeof(sub_com_sect_acm.sub_ac_no) - 1);
        sub_com_sect_acm.sub_ac_seqn = sub_dd_mst.sub_ac_seqn;
        sub_com_sect_acm.beg_date = sub_dd_mst.lst_date;
        sub_com_sect_acm.end_date = g_pub_tx.tx_date;

				/* ����Ϣ��pass��... */
				if(sub_dd_mst.intst_type == '0')
				{
					continue;
				}

        if(sub_dd_mst.rate_type[0] == '0')
        {
            ret = pub_base_getllz(HQLLBH, "01", sCom_sys_parm.lst_date, &sub_com_sect_acm.rate);
            if (ret != 0)
            {
                sprintf(acErrMsg, "�ӹ������ʱ�ȡ����ֵ����![%d]", ret);
                WRITEMSG
                strcpy(g_pub_tx.reply, "W110");
                goto ErrExit;
            }
        }
        else
        {
            /* �Ż����� */
            sub_com_sect_acm.rate = sub_dd_mst.rate;
        }
        pub_base_CalDays(sub_dd_mst.lst_date, g_pub_tx.tx_date, INTST_DAY_TYPE,&days);
        tmp_bal = pub_base_floor(sub_dd_mst.bal);
        sub_com_sect_acm.intst_acm = tmp_bal * days + sub_dd_mst.intst_acm;
        sub_com_sect_acm.intst_type[0] = 'A';
        sub_com_sect_acm.sts[0] = '0'; /* δ��Ϣ */
        ret = Sub_com_sect_acm_Ins(sub_com_sect_acm,g_pub_tx.reply);
        if(ret)
        {
            vtcp_log("%s,%d,�ǼǶ����˻��ֶλ���ʧ��!", __FILE__, __LINE__);
            strcpy(g_pub_tx.reply, "D103");
            goto ErrExit;
        }
        sub_dd_mst.intst_acm = 0.00;
        sub_dd_mst.lst_date = g_pub_tx.tx_date;
				ret=sql_execute("update sub_dd_mst set intst_acm=0 , lst_date=%ld  where ac_no='%s' and sub_ac_no='%s'",sub_dd_mst.ac_no,sub_dd_mst.sub_ac_no);
        if(ret)
        {
            vtcp_log("%s,%d,���¶����˻���ʧ��!", __FILE__, __LINE__);
            strcpy(g_pub_tx.reply, "D103");
            goto ErrExit;
        }
        totnum++;
        iCommitCnt++;/* �ύ���� */
    }
    Sub_dd_mst_Clo_Sel();

OkExit:
  sql_commit();  /*  --�ύ����--*/
  strcpy(g_pub_tx.reply, "0000");
  sprintf(acErrMsg, "���ڶԹ�������ֶλ�����ɹ�!!!!![%s]", g_pub_tx.reply);
  WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
  return 0;
ErrExit:
      if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
       strcpy(g_pub_tx.reply,"G009");
    }
  sql_rollback();    /*--����ع�--*/
  strcpy(g_pub_tx.reply,"1111");
  sprintf(acErrMsg, "���ڶԹ�������ֶλ�����ʧ��!!!!![%s]", g_pub_tx.reply);
  WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
}
