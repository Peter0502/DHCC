/*************************************************************
* 文 件 名: gD198.c
* 功能描述：将活期对公户插入分段积数表,起止日期就是dd_mst.lst_date, com_sys_parm.sys_date
*           积数=dd_mst.intst_acm+dd_mst.bal*(com_sys_parm.sys_date-dd_mst.lst_date)
* 作    者: shaoliguang
* 完成日期: 20110214
* 修改记录: 固定利率的户(cal_code='X')和保证金存款不在此列(prdt_no=131)；只处理季结息户(intst_type=4)
*      日    期:
*      修 改 人:
*      修改内容:
**************************************************************/
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#define EXTERN
#define DEF_COMMITCNT 1000
#define INTST_DAY_TYPE  "0"   /* 计息天数类型 0.实际天数 1.30天  */
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
  int        ret = 0;                /*程序主条件*/
  double     cRate=0.00;              /*调整前利率*/
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

    ret = sql_begin();          /* 打开事务 */
    if (ret != 0)
    {
        sprintf(acErrMsg, "打开事务失败!!!");
        WRITEMSG
        goto ErrExit;
    }
  /** 初始化全局变量 **/
    pub_base_sysinit();
    ret = Com_sys_parm_Sel(g_pub_tx.reply,&sCom_sys_parm, "1=1");
    if ( ret )
    {
        sprintf(acErrMsg,"查询系统表错误!");
        WRITEMSG
        goto ErrExit;
    }
   memset(wherelist,0x00,sizeof(wherelist));
   strcat(wherelist,"prdt_no not in('131') and ac_sts='1' and intst_type='4' and ac_type!='5' and ac_type!='7' order by opn_br_no,ac_id ");

  vtcp_log( "WHERE [%s]",wherelist );
  ret= Dd_mst_Dec_Sel( g_pub_tx.reply, wherelist );
    if ( ret )
    {
        sprintf(acErrMsg,"定义游标出错ret=[%d]!",ret);
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
       sprintf( acErrMsg, "执行Dd_mst_Fet_Sel出错!ret=[%d]",ret );
       WRITEMSG
       goto ErrExit;
     }
		if(sDd_mst.cal_code[0]=='X')	/* 固定利率户 */
				continue;

      /*计算利息积数*/
    ret=pub_base_CalTrueDays(sDd_mst.lst_date,sCom_sys_parm.sys_date);
    if(ret<0)
    {
        sprintf(acErrMsg,"计算日期错误!![%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    sCom_sect_acm.intst_acm=sDd_mst.intst_acm+sDd_mst.bal*ret;
    if(sCom_sect_acm.intst_acm<0.005)
    	continue;

     /*登记分段积数表*/
    memset(&sDd_parm,0x0,sizeof(struct dd_parm_c));
    ret=Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no='%s'",sDd_mst.prdt_no);
    if(ret)
    {
        sprintf(acErrMsg, "从活期产品参数表取值错误![%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "A020");
        goto ErrExit;
    }

    pub_base_getllz(sDd_parm.rate_no,"01",sCom_sys_parm.lst_date,&cRate);  /*取利率*/
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
      sprintf(acErrMsg, "向分段积数表中插入记录错误!![%d]",ret);
      WRITEMSG
      strcpy(g_pub_tx.reply,"C240");
      goto ErrExit;
    }
    /***更新主表***/
    ret = sql_execute2("update dd_mst set lst_date=%ld ,intst_acm=0.00 where ac_id=%ld and ac_seqn=%d",sCom_sys_parm.sys_date,sDd_mst.ac_id,sDd_mst.ac_seqn);
    if(ret!=0)
     {
       sprintf(acErrMsg, "更新活期主表失败!![%d]", ret);
       WRITEMSG
       strcpy(g_pub_tx.reply, "O012");
       goto ErrExit;
     }
  }
  Dd_mst_Clo_Sel();

    /*处理完对公账户开始处理二级账户 二级账户也需要分段计息*/
    iCommitCnt = 0;
    ret = Sub_dd_mst_Dec_Sel(g_pub_tx.reply, " intst_type<>'0' and ac_sts<>'*'");
    if(ret)
    {
        sprintf(acErrMsg, "查询二级账户主表失败![%d]\n", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "D101");
        goto ErrExit;
    }
    while(1)
    {
        if(iCommitCnt == DEF_COMMITCNT) /*1000笔提交一次*/
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
            vtcp_log("%s,%d,查询二级账户失败!", __FILE__, __LINE__);
            goto ErrExit;
        }
        memset(&sub_com_sect_acm, 0, sizeof(sub_com_sect_acm));
        memcpy(sub_com_sect_acm.ac_no, sub_dd_mst.ac_no, sizeof(sub_com_sect_acm.ac_no) - 1);
        memcpy(sub_com_sect_acm.sub_ac_no, sub_dd_mst.ac_no, sizeof(sub_com_sect_acm.sub_ac_no) - 1);
        sub_com_sect_acm.sub_ac_seqn = sub_dd_mst.sub_ac_seqn;
        sub_com_sect_acm.beg_date = sub_dd_mst.lst_date;
        sub_com_sect_acm.end_date = g_pub_tx.tx_date;

				/* 不计息就pass吧... */
				if(sub_dd_mst.intst_type == '0')
				{
					continue;
				}

        if(sub_dd_mst.rate_type[0] == '0')
        {
            ret = pub_base_getllz(HQLLBH, "01", sCom_sys_parm.lst_date, &sub_com_sect_acm.rate);
            if (ret != 0)
            {
                sprintf(acErrMsg, "从公共利率表取利率值错误![%d]", ret);
                WRITEMSG
                strcpy(g_pub_tx.reply, "W110");
                goto ErrExit;
            }
        }
        else
        {
            /* 优惠利率 */
            sub_com_sect_acm.rate = sub_dd_mst.rate;
        }
        pub_base_CalDays(sub_dd_mst.lst_date, g_pub_tx.tx_date, INTST_DAY_TYPE,&days);
        tmp_bal = pub_base_floor(sub_dd_mst.bal);
        sub_com_sect_acm.intst_acm = tmp_bal * days + sub_dd_mst.intst_acm;
        sub_com_sect_acm.intst_type[0] = 'A';
        sub_com_sect_acm.sts[0] = '0'; /* 未结息 */
        ret = Sub_com_sect_acm_Ins(sub_com_sect_acm,g_pub_tx.reply);
        if(ret)
        {
            vtcp_log("%s,%d,登记二级账户分段积数失败!", __FILE__, __LINE__);
            strcpy(g_pub_tx.reply, "D103");
            goto ErrExit;
        }
        sub_dd_mst.intst_acm = 0.00;
        sub_dd_mst.lst_date = g_pub_tx.tx_date;
				ret=sql_execute("update sub_dd_mst set intst_acm=0 , lst_date=%ld  where ac_no='%s' and sub_ac_no='%s'",sub_dd_mst.ac_no,sub_dd_mst.sub_ac_no);
        if(ret)
        {
            vtcp_log("%s,%d,更新二级账户表失败!", __FILE__, __LINE__);
            strcpy(g_pub_tx.reply, "D103");
            goto ErrExit;
        }
        totnum++;
        iCommitCnt++;/* 提交笔数 */
    }
    Sub_dd_mst_Clo_Sel();

OkExit:
  sql_commit();  /*  --提交事务--*/
  strcpy(g_pub_tx.reply, "0000");
  sprintf(acErrMsg, "活期对公户插入分段积数表成功!!!!![%s]", g_pub_tx.reply);
  WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
  return 0;
ErrExit:
      if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
       strcpy(g_pub_tx.reply,"G009");
    }
  sql_rollback();    /*--事务回滚--*/
  strcpy(g_pub_tx.reply,"1111");
  sprintf(acErrMsg, "活期对公户插入分段积数表失败!!!!![%s]", g_pub_tx.reply);
  WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
}
