#include "find_debug.h" 
/*************************************************
* 文 件 名: pubf_base_output
* 功能描述：1、把输出内容追加到文件中
* 作    者: Seagle  
* 完成日期：2006年06月14日
* 修改记录：    
* 1. 日    期:
*    修 改 人:
*    修改内容:
*************************************************/

#include "stdio.h"
#include <varargs.h>
#include <sys/stat.h>
#include <sys/types.h>

#define EXTERN
#include "public.h"

/**********************************************************************
* 函数名：    pub_base_putmsg
* 函数功能：  把一个包追加到文件中
* 作者/时间：    
* 
* 参数：
*      msgend : 包结束标志
*      msgtype: 包类型标识
*      msgdata: 包内容
*      msglen : 包长度
*           
* 返 回 值: >=0:成功 <0:失败
*           
* 说明:  拼完之后的包结构为[1位结束标识+5位包类型+3位包长度+包内容]
********************************************************************/
int pub_base_putmsg(char msgend,char *msgtype,char *msgdata,int msglen)
{
	char pcFileName[256];
	FILE *fp;
	pub_base_AllDwnFilName(pcFileName);
	fp=fopen(pcFileName,"a");
	if(fp==NULL){
		sprintf(acErrMsg,"%s.%d,打开文件[%s]失败!\n",__FILE__,__LINE__,pcFileName);
		WRITEMSG
		return(-1);
	}
	fprintf(fp,"%c%.5s%03d",msgend,msgtype,msglen+9);
	fwrite(msgdata,msglen,1,fp);
	fclose(fp);
	return(0);	
}
