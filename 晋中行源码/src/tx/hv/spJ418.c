/*************************************************
* 文 件 名:	spJ417.c
* 功能描述：公共数据更新报文
* 作    者: jane
* 完成日期: 2004年6月29日
* 修改记录： 
*   日    期:
*   修 改 人:
*   修改内容:
*************************************************/

#define	EXTERN
#include <string.h>
#include "public.h"
#include "hv_pub.h"
#include "hv_define.h"
#include "find_debug.h"

spJ417()
{
	int ret;
	char filename[50];
	char *updcntt;
	FILE *fp;

	if (strlen(mbfetran.filename)!=0)
	{
		sprintf(filename,"%s%s%s",getenv("HOME"),"/txt/",mbfetran.filename);
		fp=fopen(filename,"r");
		if (fp==NULL)
		{
			sprintf(acErrMsg," open file [%s] error ",filename );
			WRITEMSG
			strcpy( g_pub_tx.reply,"S047" );
			goto ErrExit;
		}

		mbfetran.updcntt=(char*)malloc((60*1024)*sizeof(char));
		if (mbfetran.updcntt == NULL)
		{
			vtcp_log("分配内存出错!!");
			goto ErrExit;
		}
		memset(mbfetran.updcntt,0x00,(60*1024)*sizeof(char));

		ret=fread(mbfetran.updcntt,sizeof(char),61439,fp);
		strncpy(mbfetran.datatype,mbfetran.updcntt+8,3);
		updcntt = mbfetran.updcntt + 16;
		while (ret)
		{
			if ((ret=split_updcntt(updcntt))<0) goto ErrExit;
			/*文件指针向前移零碎记录长度位*/
			fseek(fp,-ret,1);
			memset(mbfetran.updcntt,0x00,(60*1024)*sizeof(char));
			ret=fread(mbfetran.updcntt,sizeof(char),61439,fp);
			updcntt = mbfetran.updcntt;
		}

		fclose(fp);
		vtcp_log("行名行号辅表更新成功!");
		goto OkExit;
	}

	switch (atoi(mbfetran.datatype))
	{
		case 104: case 105:
			if (split_updcntt(mbfetran.updcntt)<0) goto ErrExit;
			vtcp_log("行名行号辅表更新成功!");
			break;
		default:
			break;
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

int split_msginfo(char *string)
{
	char msgid[25], msgexp[20]; /*信息识别码，信息说明*/
	int  msghdlen, msgbdlen;	/*信息头长度，信息体长度*/
	char tmpstr[10];
	char *ptr;

	memset(msgid, 0x00, sizeof(msgid));
	memset(msgexp, 0x00, sizeof(msgexp));
	memset(tmpstr, 0x00, sizeof(tmpstr));

	ptr = strchr(string,'\n');
	strncpy(msgid, string, ptr-string);
	msgid[ptr-string] = 0;
	string = ptr + 1;
 
	ptr = strchr(string,'\n');
	strncpy(tmpstr, string, ptr-string);
	tmpstr[ptr-string] = 0;
	msghdlen = atoi(tmpstr);
	string = ptr + 1;

	ptr = strchr(string,'\n');
	strncpy(tmpstr, string, ptr-string);
	tmpstr[ptr-string] = 0;
	msgbdlen = atoi(tmpstr);
	string = ptr + 1;

	ptr = strchr(string,'\n');
	strncpy(msgexp, string, ptr-string);
	msgexp[ptr-string] = 0;
	string = ptr + 1;

	return 0;
}

int split_updcntt(char *string)
{
	char field[20][61];
	char bankinfo[300];
	char *ptr;
	int  leftlen;

	memset(bankinfo, 0x00, sizeof(bankinfo));
	memset(field, 0x00, sizeof(field));

	/***
	if (string[0]=='C') split_msginfo(string);
	***/

	while ((ptr=strchr(string,'\n'))!=NULL)
	{
		strncpy(bankinfo, string, ptr-string);
		bankinfo[ptr-string] = 0;
		/*vtcp_log("bankinfo=[%s]",bankinfo);*/

		split_string(bankinfo, field, "|");

		if (atoi(mbfetran.datatype)==104)	/*支付系统行号*/
		{
			switch (atoi(field[0]))
			{
				case 1:/*变更*/
					g_reply_int=sql_execute("insert into mbfenetbanksnd values ('%s','%s','%s','%s','%s','%s','%s','','%s','0','%s','%s','%s','%s')",field[1],field[5],field[9],field[10],field[11],field[12],field[14],field[13],field[15],field[0],field[2],field[16]);
					if (g_reply_int)
					{
						sprintf(acErrMsg,"netbanksnd insert(变更) error [%d]",g_reply_int);
						WRITEMSG
						return -1;
					}

					if (!strcmp(field[1],field[5]))  /*为清算行*/
					{
						g_reply_int=sql_execute("insert into mbfesabksnd values ('%s','%s','%s','%s','0','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s')",field[1],field[10],field[11],field[3],'0',field[7],field[6],field[9],field[12],field[13],field[14],field[15],field[0],field[2],field[16]);
						if (g_reply_int)
						{
							sprintf(acErrMsg,"sabksnd insert(变更) error [%d]",g_reply_int);
							WRITEMSG
							return -1;
						}
					}
					break;
				case 2:/*新增*/
					g_reply_int=sql_execute("insert into mbfenetbanksnd values ('%s','%s','%s','%s','%s','%s','%s','','%s','0','%s','%s','%s','%s')",field[1],field[6],field[10],field[11],field[12],field[13],field[15],field[14],field[16],field[0],field[17],field[19]);
					if (g_reply_int)
					{
						sprintf(acErrMsg,"netbanksnd insert(新增) error [%d]",g_reply_int);
						WRITEMSG
						return -1;
					}

					if (!strcmp(field[1],field[6]))  /*为清算行*/
					{
						g_reply_int=sql_execute("insert into mbfesabksnd values ('%s','%s','%s','%s','0','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s')",field[1],field[11],field[12],field[2],field[8],field[7],field[10],field[13],field[14],field[15],field[16],field[0],field[17],field[19]);
						if (g_reply_int)
						{
							sprintf(acErrMsg,"sabksnd insert(新增) error [%d]",g_reply_int);
							WRITEMSG
							return -1;
						}
					}
					break;
				case 3:/*注销*/
					g_reply_int=sql_execute("insert into mbfenetbanksnd values ('%s','','','','','','','','','0','','%s','%s','')",field[1],field[0],field[2]);
					if (g_reply_int)
					{
						sprintf(acErrMsg,"netbanksnd insert(注销) error [%d]",g_reply_int);
						WRITEMSG
						return -1;
					}

					g_reply_int=sql_count("mbfesabk","sabkcode='%s'",field[1]);
					if (g_reply_int>0)  /*为清算行*/
					{
						g_reply_int=sql_execute("insert into mbfesabksnd values ('%s','','','','0','','','','','','','','%s','%s','')",field[1],field[0],field[2]);
						if (g_reply_int)
						{
							sprintf(acErrMsg,"sabksnd insert(注销) error [%d]",g_reply_int);
							WRITEMSG
							return -1;
						}
					}
					break;
				default:
					break;
			}
		}
		else if (atoi(mbfetran.datatype)==105)  /*电子联行行号*/
		{
			switch (atoi(field[0]))
			{
				case 1:/*变更*/
					g_reply_int=sql_execute("insert into mbfeeisbanksnd values ('%s','%s','%s','%s','0','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s')",field[1],field[5],field[3],field[4],field[7],field[8],field[9],field[10],field[11],field[12],field[13],field[0],field[2],field[14]);
					if (g_reply_int)
					{
						sprintf(acErrMsg,"eisbanksnd insert(变更) error [%d]",g_reply_int);
						WRITEMSG
						return -1;
					}
					break;
				case 2:/*新增*/
					g_reply_int=sql_execute("insert into mbfeeisbanksnd values ('%s','%s','%s','%s','0','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s')",field[1],field[4],field[2],field[3],field[6],field[7],field[8],field[9],field[10],field[11],field[12],field[0],field[13],field[15]);
					if (g_reply_int)
					{
						sprintf(acErrMsg,"eisbanksnd insert(新增) error [%d]",g_reply_int);
						WRITEMSG
						return -1;
					}
					break;
				case 3:/*注销*/
					g_reply_int=sql_execute("insert into mbfeeisbanksnd values ('%s','','','','0','','','','','','','','%s','%s','')",field[1],field[0],field[2]);
					if (g_reply_int)
					{
						sprintf(acErrMsg,"eisbanksnd insert(注销) error [%d]",g_reply_int);
						WRITEMSG
						return -1;
					}
					break;
				default:
					break;
			}
		}

		string = ptr + 1;
	}

	/*返回文件形式每次读出的最后一条零碎记录长度*/
	/*vtcp_log("lastbankinfo=[%s]",string);*/
	if (!strcmp(string,"}")) return 0;
	leftlen = strlen(string);
	return leftlen;
}
 
int split_string(char *string,char (*array)[61],char *sign)
{
	int  i=0;
	char *ptr;

	while ((ptr=strchr(string,*sign))!=NULL)
	{
		strncpy(array[i], string, ptr-string); 
		array[i][ptr-string] = 0;
		string = ptr + 1;
		i++;
	}

	return 0;
}

