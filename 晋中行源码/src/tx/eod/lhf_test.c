/**************************************************
�����α��ͬһ�������ǰ̨ѭ���Ƿ��ı�
**************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "string.h"
#include "stdlib.h"
#include "lv_pkgreg_c.h"
#include "hv_define.h"
#include "lv_wbctl_c.h"
#include "com_sys_parm_c.h"
#include "lv_define.h"
#include "lv_sysctl_c.h"
#include "seqctl_c.h"
#include "time.h"
#include "timeb.h"
#include "public.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <varargs.h>

extern char * pcLvGetDay();
extern char * pub_base_daynumLAT(char *date, int num);
/**�����α�**/

/*******************************************************************
*                                                                  *
*   FUNCTIONS:      main                                           *
*                                                                  *
*  DESCRIPTION:    1.ʱ�����                                      *
*          2.��������δ�������ͨ����ҵ��                          *
*          3.��������δ�������ͨ���ҵ��                          *
*******************************************************************/
lhf_test()
{
  struct seqctl_c seqctl;
  char cBr_no[5];
  char cBr_no_old[5];
  int i=0;
  memset(&seqctl, 0 , sizeof(seqctl));
  Seqctl_Dec_Sel(g_pub_tx.reply, "br_no in ('1','2','3','4','5','6','7','8','9') order by br_no");
  while(1)
  {
    memset(&seqctl, 0 , sizeof(seqctl));
    memset(cBr_no ,0 , sizeof(cBr_no));
    memset(cBr_no_old ,0 , sizeof(cBr_no_old));
    g_reply_int = Seqctl_Fet_Sel(&seqctl, g_pub_tx.reply);
    if(g_reply_int==100) break;
    if(g_reply_int)
    {
        vtcp_log("%s,%d ��ȡseqctl�α����ret=[%d]",__FILE__,__LINE__,g_reply_int);
        goto ErrExit;
        break;
    }
    memcpy(cBr_no,seqctl.br_no,sizeof(cBr_no)-1);
		pub_base_strpack(cBr_no);
    sprintf(cBr_no_old,"%d",atoi(cBr_no)+1);

    sprintf(cBr_no,"%d",atoi(cBr_no)+11);
    vtcp_log("%s,%d fetch br_no is =[%s]seqno=[%.0f]",__FILE__,__LINE__,seqctl.br_no,seqctl.seqno);
		vtcp_log("%s,%d update br_no is =[%s] where Br_no=[%s]",__FILE__,__LINE__,cBr_no,cBr_no_old);
    g_reply_int = sql_execute("update seqctl set br_no='%s',seqno=seqno*2 where br_no='%s'",cBr_no,cBr_no_old);
    if(g_reply_int!=100 && g_reply_int!=0)
    {
       vtcp_log("[%s][%d],����seqctl��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
       goto ErrExit;
    }
    sql_commit();
    i++;
  }
  Seqctl_Clo_Sel();
  OkExit:
    sql_commit();   /*--�ύ����--*/
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"С���Զ�ƴ���ɹ�!!!!![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
  ErrExit:
    sql_rollback(); /*--����ع�--*/
    sprintf(acErrMsg,"С���Զ�ƴ��ʧ��!!!!![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}
