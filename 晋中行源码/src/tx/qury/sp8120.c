/*************************************************
* 文 件 名:  sp8120.c
* 功能描述： 储蓄利息税查询
*
* 作    者:  
* 完成日期： 
*
* 修改记录：
* 日   期:2004.12.12
* 修 改 人:jane
* 修改内容:非本网点或本网点下属网点帐户信息不能查询
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include <string.h>
#include "public.h"
#include "mo_int_tax_hst_c.h"

int sp8120()
{
	struct mo_int_tax_hst_c     Mo_int;
	char ac_no[19];
	
		int i=0;
		int ret=0;
	
		FILE *fp;
        char cFilename[100];/*文件名*/
        memset( &Mo_int,0,sizeof(Mo_int));
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( ac_no,0,sizeof(ac_no) );
        vtcp_log("sp8120 开始！[%s][%d]",__FILE__,__LINE__);
	pub_base_sysinit();

	get_zd_data("0380",ac_no);
        vtcp_log("[%s][%d]ac_no=[%s]",__FILE__,__LINE__,ac_no);
        vtcp_log("sp8120 开始！[%s][%d]",__FILE__,__LINE__);
        ret=Mo_int_tax_hst_Dec_Sel ( g_pub_tx.reply,"ac_no='%s' ",ac_no);
        if ( ret==100 )
        {    
                sprintf(acErrMsg,"无此帐号");
                WRITEMSG
                goto ErrExit;
        }
        else if ( ret )
        {
               sprintf(acErrMsg,"查询储蓄利息税出错[%d]",ret);
               WRITEMSG
               goto ErrExit;
        }  
              
       i=0;
       while(1)
       {  
             ret=Mo_int_tax_hst_Fet_Sel (&Mo_int,g_pub_tx.reply);
             vtcp_log("%s,%d ret=[%d]",__FILE__,__LINE__,ret);
            if(ret==100)  
            {    
                    vtcp_log("%s,%d 取值",__FILE__,__LINE__);    

                    break;
            }
         
            if(ret)
            {
                  sprintf( acErrMsg, "sql error" );
                  WRITEMSG
                  goto ErrExit;
            }   

           if(!i)
           {
              pub_base_AllDwnFilName(cFilename);
              fp = fopen(cFilename,"w" );
              if( fp==NULL )
              {  
                      sprintf(acErrMsg," open file [%s] error ",cFilename);
                      WRITEMSG
                      strcpy( g_pub_tx.reply,"S047" );
                       goto ErrExit;
              }
                       fprintf( fp,"~帐号|开始日期|结束日期|$交易金额|$计税积数|$应税利息|税率|$利息税|交易日期|流水号|\n" ); 


            }  
                      fprintf( fp,"%s|%8d|%8d|%16.2f|%16.2f|%16.2f|%8.5f|%16.2f|%8d|%8d|\n",Mo_int.ac_no,Mo_int.beg_date,Mo_int.end_date,Mo_int.tx_amt,Mo_int.intst_acm,Mo_int.intst,Mo_int.intst_tax_rate,Mo_int.intst_tax,Mo_int.tx_date,Mo_int.trace_no);


               

 
            i++;
        }   

  
       ret=Mo_int_tax_hst_Clo_Sel();
  /**     ERR_DEAL    **/

       if( ret )
       {
               sprintf( acErrMsg, "sql error" );
               WRITEMSG
               goto ErrExit;
       } 







       set_zd_data( DC_FILE_SND,"2");   
       if(!i)
       {   
                strcpy( g_pub_tx.reply,"S049" );
                goto ErrExit;
       }
       else
       {
           fclose(fp);

          set_zd_data( DC_FILE_SND,"1" );

        }






GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
