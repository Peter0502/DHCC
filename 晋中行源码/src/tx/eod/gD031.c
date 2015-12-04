/*************************************************************
* 文 件 名: gD031.c
* 功能描述：生成翻盘后监督系统数据
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
gD031()
{
int	 ret=0;
char trdt[9],trdt1[5],dbname[10],str[200],str1[50],str2[200];
char pass[50];
char tmpbuf[128];
char tmpbuf1[128];

/****JYW add wait change it to DHCC*****/

goto  OkExit;

/********JYW **************************/


	ret=sql_begin(); /*打开事务*/
	if( ret )
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

	ret=sql_max_long("gl_mst","date",&g_pub_tx.tx_date,"1=1");
	if( ret )
	{
		sprintf(acErrMsg,"select gl_mst error ret=[%d]\n",ret);
		WRITEMSG
		goto ErrExit;
	}
	sprintf(trdt,"%8d",g_pub_tx.tx_date);

	memset( pass,0x00,sizeof( pass ) );
	strcpy(str1,getenv("NBSSDIR"));
	sprintf(str,"cd %s",str1);    
	printf("%s\n",str);
	sprintf(str2,"qtuld1.sh %s",trdt); 
	printf("%s\n",str2);
	system(str2);                  /* 导出翻牌后数据 */	
	system(str);                   /* 进入数据导出目录 $HOME/nbsstxt */	

    /* 以8位日期的后两位(日)做为数据子目录 */
    memset(tmpbuf,0x00,sizeof(tmpbuf));
    if ( trdt[7] == NULL || trdt[7] == ' ' ) {
         memcpy(tmpbuf,trdt,8);
    }
    else {
         memcpy(&tmpbuf,&trdt[6],2);
    }
    printf("tmpbuf=[%s]\n",tmpbuf);

	memset(tmpbuf1,0x00,sizeof(tmpbuf1));
	sprintf(tmpbuf1,"%s/%s",str1,tmpbuf);
	if( access(tmpbuf1,0)!=0 ) {
		sprintf(acErrMsg,"银行监督系统生成数据目录不存在![%s]",tmpbuf1);
		WRITEMSG
		goto ErrExit;
	}

	sprintf(str,"mv -f %s/*.txt %s/%s",str1,str1,tmpbuf);
	printf("%s\n",str);
	system(str);	

	sprintf(pass,"%s/%s",str1,tmpbuf);
	chdir(pass);

	sprintf(str,"wc -m *.txt > wc.txt");
	printf("%s\n",str);
	system(str);	

	sprintf(str,"chmod -R 777 %s/%s",str1,tmpbuf);
	printf("%s\n",str);
	system(str);	

OkExit:
   sql_commit();   /*--提交事务--*/
   strcpy(g_pub_tx.reply,"0000");
   sprintf(acErrMsg,"生成银行监督系统翻盘后数据成功!!!!![%s]",g_pub_tx.reply);
   WRITEMSG
   set_zd_data("0120",g_pub_tx.reply);
   return 0;
ErrExit:
   if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
   {
       strcpy(g_pub_tx.reply,"G009");/* 待修改 */
   }
   sql_rollback(); /*--事务回滚--*/
   sprintf(acErrMsg,"生成银行监督系统翻盘后数据失败!!!!![%s]",g_pub_tx.reply);
   WRITEMSG
   set_zd_data("0120",g_pub_tx.reply);
   return 1;
}
