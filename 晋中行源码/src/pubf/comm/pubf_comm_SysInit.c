#include "find_debug.h" 
/**********************************************************************
    程序名:svrpack.c
-----------------------------------------------------------------------
    本程序为系统交易包处理程序。
-----------------------------------------------------------------------
    编制人：jack                             编制时间：
**********************************************************************/
/*************************************************
* 文 件 名:    pubf_comm_SysInit.c
* 功能描述：
*              sysinit     初始化数据缓冲区
*
* 作    者:    jack
* 完成日期：   2003年12月16日
* 修改记录：
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2.
*************************************************/
#include   <stdio.h>
#include   <time.h>
#define    EXTERN
#include   "public.h"
#include   "svrpub_c.h"
#include   "tx_log_c.h"
#include   "data_dic_c.h"
#include   "bitmap_c.h"

int sysinit()
{
    int i=0;
    int     recdnum=0;
    int     cnt=0;
	long	length=0;
    struct  bitmap_c    bitmap;
    struct  data_dic_c  data_dic;

	MEMSET_DEBUG(&data_dic,0x00,sizeof(data_dic));
	MEMSET_DEBUG(&bitmap,	0x00,sizeof(bitmap));

    int     ret;
    
    cnt = 0;

    for (i=1; i<128; i++)
    { 
        Bit_Arr[i].val=-1; 
        Bit_Arr[i].dcd=0;
        Bit_Arr[i].lx='0';
        Bit_Arr[i].len=0;
    }
    ret = Bitmap_Dec_Sel( RtCode , "%s" , "1=1 order by bit_seq" );
    if( ret ) 
    {
        vtcp_log("取比特结构错误!!error code =%d.\n",RespCode);
        return ret;
    }

    while ( 1 )
    {
		MEMSET_DEBUG(&bitmap,	0x00,sizeof(bitmap));
        ret = Bitmap_Fet_Sel( &bitmap, RtCode );
        if (ret)
        {
            break;
        }
        i = bitmap.bit_seq - 1;
        if ( i < 0 )
        {
            continue;
        }
        Bit_Arr[i].lx=bitmap.fied_type[0];
        Bit_Arr[i].len=Bit_Arr[i].dcd=bitmap.leng;
        cnt++;
    }

	Bitmap_Clo_Sel( );
 
    cnt = 0;

	recdnum = sql_count ( "data_dic" , "%s" , "1=1" );
    if (recdnum<=0) 
    {
        vtcp_log("数据字典无内容!!ERROR %d",recdnum);
        return -1;
    }
/* TRACE */
    ret = sql_sum_long( "data_dic","data_leng",&length,"%s","1=1" );
	if ( ret )
	{
        vtcp_log("数据字典sum!!ERROR");
        return -1;
	}
/* TRACE */

    Pub_leng=length;

    ret = Data_dic_Dec_Sel( RtCode , "%s" , "1=1 order by data_code" );
    if ( ret )
    {
        vtcp_log("取数据字典错误!!error code =%d.\n", RespCode );
        return ret ;
    }
/* TRACE */

    Pu_Val=(struct PVal_St *) malloc (sizeof(struct PVal_St) *recdnum);
    Pub_Buf=(unsigned char *)malloc (sizeof(unsigned char)*Pub_leng+2); 
    Old_Pu_Val=(struct PVal_St *) malloc (sizeof(struct PVal_St) *recdnum);
    Old_Pub_Buf=(unsigned char *)malloc (sizeof(unsigned char)*Pub_leng+2); 
    MEMSET_DEBUG (Pub_Buf,0x20,Pub_leng+2);
    MEMSET_DEBUG (Old_Pub_Buf,0x20,Pub_leng+2);
	iGlobal_Pu_Val_Addr=Pu_Val;

	/***********************/
	memset(Pu_Val,0x00,sizeof(struct PVal_St) *recdnum);

    for (i=0,length=0;i<recdnum;i++) 
    {
		MEMSET_DEBUG(&data_dic,0x00,sizeof(data_dic));

        RespCode = Data_dic_Fet_Sel( &data_dic, RtCode );
        if ( RespCode )
        {
            break;
        }
        strcpy(Pu_Val[i].sjbm,data_dic.data_code);
        Pu_Val[i].length=data_dic.data_leng;
        Pu_Val[i].prec=data_dic.data_dec;
        Pu_Val[i].lx=data_dic.data_type[0]-'0';
        Pu_Val[i].val=Pub_Buf+length;
		/***XXXX***/
        strcpy(Old_Pu_Val[i].sjbm,data_dic.data_code);
        Old_Pu_Val[i].length=data_dic.data_leng;
        Old_Pu_Val[i].prec=data_dic.data_dec;
        Old_Pu_Val[i].lx=data_dic.data_type[0]-'0';
        Old_Pu_Val[i].val=Old_Pub_Buf+length;
		/***XXXX***/
        length+=data_dic.data_leng;
        if (data_dic.bit_seq>0)
        {
            Bit_Arr[data_dic.bit_seq-1].val=i;
/*vtcp_log("BitArr[%s][%d][%d]",data_dic.data_code,data_dic.bit_seq-1,i); */
        }
        cnt++;
    }

    Data_dic_Clo_Sel( );
    PuVal_num=recdnum;

TRACE
    return recdnum;
}
