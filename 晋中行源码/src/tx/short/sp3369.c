/*************************************************
* �� �� ��: sp3369.c
* �������ã��������ú�����Ȩ����
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"

int sp3369()
{
	int      ret = 0;
  char     brno[6], tel[4+1];
  
  memset(brno,0x00,sizeof(brno));
  memset(tel,0x00,sizeof(tel));
  
  sprintf(acErrMsg, "[%s][%d],�������ú�����Ȩ����",__FILE__,__LINE__);
    WRITEMSG

	strcpy(g_pub_tx.reply, "0000");
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
}
