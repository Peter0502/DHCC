/*************************************************
* 文 件 名:  sp3999.c
* 功能描述： 远程授权信息查询
*
* 作    者: liuyong 
* 完成日期: 2010-4-20
*
* 修改记录：
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "auth_remote_inf_c.h"

int sp3999()
{

       struct auth_remote_inf_c auth_remote_inf;
 
       char wherelist[1024];
       char tmpstr[512];
       char filename[100];
       char brno[6];
       char tel[5];
       long begdate,enddate;
       int ttlnum = 0;
       int ret = 0;
       FILE *fp;

       memset(wherelist,0,sizeof(wherelist));
       memset(tmpstr,0,sizeof(tmpstr));
       memset(filename,'\0',sizeof(filename));
       memset(brno,'\0',sizeof(brno));
       memset(tel,'\0',sizeof(tel)); 
       memset(&auth_remote_inf,0x00,sizeof(struct auth_remote_inf_c));

       pub_base_sysinit();

       get_zd_data("0030",brno);      /*机构号*/
       get_zd_data("0070",tel);       /*授权柜员*/
       get_zd_long("0440",&begdate);  /*交易开始日期*/
       get_zd_long("0450",&enddate);  /*交易截止日期*/

       pub_base_strpack(brno);
       pub_base_strpack(tel);
 
       /*if(strlen(brno) == 0)
           goto ErrExit;
       if(strlen(tel) == 0)
           goto ErrExit;
       */

       /**组成查询条件**/
       if(strlen(brno))
       {
           sprintf( tmpstr," br_no='%s' and",brno);
           strcat( wherelist,tmpstr );
       }

       if(strlen(tel))
       {
           sprintf(tmpstr," auth_tel='%s' and",tel);
           strcat(wherelist,tmpstr);
       }
       if(begdate)
       {
           sprintf( tmpstr," tx_date>=%ld and",begdate );
           strcat( wherelist,tmpstr );
       }
       if(enddate )
       {
           sprintf( tmpstr," tx_date<=%ld and",enddate );
           strcat( wherelist,tmpstr );
       }
       strcat(wherelist," 1=1 ORDER BY tx_date") ;

       ret=Auth_remote_inf_Dec_Sel( g_pub_tx.reply,wherelist );
       ERR_DEAL

       ttlnum=0;
      
       /*查询数据*/ 
       while(1)
       {
           ret = Auth_remote_inf_Fet_Sel( &auth_remote_inf, g_pub_tx.reply );
           if( ret==100 ) break;
           ERR_DEAL
           if( !ttlnum )
           {
	       pub_base_AllDwnFilName( filename );
	       fp = fopen( filename,"w" );
	       if( fp==NULL )
	       {
		    sprintf(acErrMsg," open file [%s] error ",filename );
		    WRITEMSG
		    strcpy( g_pub_tx.reply,"S047" );
		    goto ErrExit;
	       }
	       /**显示列标题**/
               /*fprintf( fp,"~流水号|机构号|柜员|交易日期|交易代码|授权柜员|交易文件|附件|\n" );*/
	  }

	  fprintf( fp, "%ld|%6s|%s|%ld|%s|%s|%s|%s|%s|\n", auth_remote_inf.trace_no,auth_remote_inf.br_no,auth_remote_inf.tel,auth_remote_inf.tx_date,auth_remote_inf.tx_code,auth_remote_inf.auth_tel,auth_remote_inf.filename,auth_remote_inf.attachments,auth_remote_inf.title); 
	  ttlnum++;
	}

	ret = Auth_remote_inf_Clo_Sel( );
	ERR_DEAL

	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);

		set_zd_data( DC_FILE_SND,"1" );
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"查询远程授权信息成功: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"查询远程授权信息失败: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
