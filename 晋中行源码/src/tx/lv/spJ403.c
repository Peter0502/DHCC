/*****************************************************
* �� �� ��:  spJ403.c
* ���������� ��ѯ��С��ר���ڲ��ʻ�
* ��    ��:  xyy
* ������ڣ� 2006-10-24 15:23
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*****************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "lv_define.h"
#include "hv_define.h"
#include "lv_fd123.h"
#include "com_parm_c.h"
int spJ403()
{
  int ret=0,i;
  char cCode[6];
  char cKinbr[BRNO_LEN+1];
  struct com_parm_c  com_parm;
  memset(cCode ,0,sizeof(cCode));
  memset(cKinbr ,0,sizeof(cKinbr));
  memset(&com_parm,0x00,sizeof(struct com_parm_c));
  vtcp_log("spJ403 ��ʼ��[%s][%d]",__FILE__,__LINE__);
 	
   /*** ��ʼ��ȫ�ֱ��� ***/
   pub_base_sysinit();
	
   get_fd_data("0020",cCode);
   get_fd_data("0030",cKinbr);
   if(memcmp(cKinbr,QS_BR_NO,BRNO_LEN)!=0)
   {
 	  if(memcmp(cKinbr,cCode,BRNO_LEN)!=0)
  	  {
            sprintf( acErrMsg,"���ܴ����������д���,[%s]", __FILE__,__LINE__,cKinbr);
            strcpy( g_pub_tx.reply, "S043" );
            WRITEMSG
            goto ErrExit;
      }
   }
   g_reply_int = Com_parm_Dec_Sel( g_pub_tx.reply , " parm_code='%s' order by parm_seqn" , cCode );
   vtcp_log("spJ403 ��ʼ��[%s][%d]",__FILE__,__LINE__);
   if ( g_reply_int==100 )
   {
	  sprintf(acErrMsg,"û�иû����Ĵ�С��ר���ڲ��ʻ�");
	  WRITEMSG
	  strcpy( g_pub_tx.reply, "O043" );
	  goto	ErrExit;
   }
   else if ( g_reply_int )
   {
		sprintf(acErrMsg,"��ѯ�û����Ĵ�С��ר���ڲ��ʻ�����[%d]",ret);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
   }	
   while(1)
   {
	  g_reply_int = Com_parm_Fet_Sel( &com_parm , g_pub_tx.reply );
	  if ( g_reply_int==100 )
	  {
	    	sprintf(acErrMsg,"û�иû����Ĵ�С��ר���ڲ��ʻ�");
	    	WRITEMSG
	    	strcpy( g_pub_tx.reply, "O043" );
            break;
	  }
	  else if ( g_reply_int )
	  {
	    	sprintf(acErrMsg,"��ѯ�û����Ĵ�С��ר���ڲ��ʻ�����[%d]",ret);
	    	strcpy( g_pub_tx.reply, "HV01" );
	    	WRITEMSG
	    	goto ErrExit;
	  }	
      vtcp_log("[%s][%d]��ѯ�ɹ�!parm_seqn=[%d]",__FILE__,__LINE__,com_parm.parm_seqn);
      vtcp_log("[%s][%d]val====[%s]",__FILE__,__LINE__,com_parm.val);
      set_zd_data("0330",com_parm.val);/*�ʺ�*/
      set_zd_long("0200",com_parm.parm_seqn);/*���*/
      set_zd_data("0270",com_parm.parm_name);/*����*/
      vtcp_log("[%s][%d]�����ڲ��ʺŵ��ʺ�Ϊ[%s]",__FILE__,__LINE__,com_parm.val);
      vtcp_log("[%s][%d]�����ڲ��ʺŵĻ���Ϊ[%s]",__FILE__,__LINE__,com_parm.parm_name);
   }
   Com_parm_Clo_Sel();

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
