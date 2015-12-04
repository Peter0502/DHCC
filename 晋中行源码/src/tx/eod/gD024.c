/*************************************************************
* 文 件 名: gD024.c
* 功能描述：生成翻盘前监督系统数据
*
* 作    者: jack
* 完成日期: 2003年3月29日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include "public.h"
#include "com_sys_parm_c.h"
gD024()
{
	int	ret=0;
	char trdt[9],trdt1[5],dbname[10],str[200],str1[50];
    char tmpbuf[128];
    char tmpbuf1[128];
	struct com_sys_parm_c	com_sys_parm_c;

	memset(&com_sys_parm_c,0x00,sizeof(struct com_sys_parm_c));
	ret=sql_begin(); /*打开事务*/
	if( ret )
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

	ret=Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm_c,"1=1");
	if( ret )
	{
		sprintf(acErrMsg,"select com_sys_parm error ret=[%d]\n",ret);
		WRITEMSG
		goto ErrExit;
	}
	sprintf(trdt,"%d",com_sys_parm_c.sys_date);

	strcpy(str1,getenv("NBSSDIR"));
	sprintf(str,"cd %s",str1);
	printf("%s\n",str);
	system("qtuld.sh");

	system(str);	
        
    memset(tmpbuf,0x00,sizeof(tmpbuf));
    if ( trdt[7] == NULL || trdt[7] == ' ' ) {
         memcpy(tmpbuf,trdt,8);
    }
    else {
         memcpy(&tmpbuf,&trdt[6],2);
    }
    memset(tmpbuf1,0x00,sizeof(tmpbuf1));
    sprintf(tmpbuf1,"%s/%s",str1,tmpbuf);
        
    if (access(tmpbuf1,0) == 0) {
		sprintf(str,"rm -f %s/*.txt",tmpbuf1);
    }
    else {
	    sprintf(str,"mkdir %s",tmpbuf1);
    }

	vtcp_log("xxx --- look [%s]",tmpbuf1);
	system(str);	

	sprintf(str,"mv %s/*.txt %s",str1,tmpbuf1);
	printf("%s\n",str);
	system(str);	

OkExit:
  sql_commit();   /*--提交事务--*/
  strcpy(g_pub_tx.reply,"0000");
  sprintf(acErrMsg,"生成银行监督系统翻盘前数据成功!!!!![%s]",g_pub_tx.reply);
  WRITEMSG
  set_zd_data("0120",g_pub_tx.reply);
  return 0;
ErrExit:
  if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
  {
      strcpy(g_pub_tx.reply,"G009");/* 待修改 */
  }
  sql_rollback(); /*--事务回滚--*/
  sprintf(acErrMsg,"生成银行监督系统翻盘前数据失败!!!!![%s]",g_pub_tx.reply);
  WRITEMSG
  set_zd_data("0120",g_pub_tx.reply);
  return 1;
}
