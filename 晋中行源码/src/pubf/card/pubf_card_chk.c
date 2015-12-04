/***卡号校验***/
#include <stdio.h>
#define EXTERN
#include "public.h"

/**卡号校验计算**/
/* x 0-校验 1-计算校验位 
   str 卡号
   str1 添加效验位后的卡号
*/
pub_card_par_1212( int x,char *str,char *str1 )
{
	int s, len, s1, m, i;
	char  tmp[30], xyw;
	char cCard_serial[30];
	memset(tmp, 0 , sizeof(tmp));
	memset(cCard_serial, 0 , sizeof(cCard_serial));
	

	s1 = 0;
	strcpy( tmp , str );  len = strlen( tmp ) - 1;
	memcpy(cCard_serial,tmp,len);
vtcp_log("card<%s> len=%d\n", tmp, len);
	/* rem by LiuHuafeng 2007-1-24 12:39
  for (i=0; i<len; i++) 
	{
		if ( ( tmp[i] < '0') || (tmp[i] > '9') )  
			return -2;

		s = tmp[i] - 48;
		i = i + 1; m = i % 2;

		if (m == 0) 
			s1 = s * 2 + s1;
		else
			s1 = s + s1;

		i = i - 1;
    }

	m = s1 % 10;

	if (m == 0)  
		xyw = '0';
	else
		xyw = (10 - m) + 48;
	*******************************************/
	xyw=iCheckCardno(cCard_serial);
vtcp_log("xyw=<%c>\n", xyw);

	if (x == 1) 
	{ 
		strncpy(str1, tmp, len); 
		str1[len] = xyw; 
		str1[len+1]=0x00;
vtcp_log("str=<%s>\n", str1);
	}
	else
	{
	  	if (xyw != tmp[ strlen(tmp)-1 ]) 
	  	{
			vtcp_log("<%c> <%c>\n",xyw, tmp[strlen(tmp)-1]); 
			return -1;
	  	}
	  	else
	  		vtcp_log(" jiao yan ok!\n");
	}

	return 0;
}
int iCurrentDigit(char cDigvalue)
{
    return cDigvalue-'0';
}
/*********************************
* 卡号产生校验位
* 参数描述
*    卡位序号,序号长度
* 返回 校验位
*********************************/
/* 卡交校验位 */
int iCheckCardno(char *cardno){   /*add by Liuhuafeng 20060630 for check cardno*/
	char cInput[100];
	int i=0;
	int sum=0;
	int tmpval=0;
	sum=0;
	tmpval=0;
	memset(cInput, 0 , sizeof(cInput));
	strncpy(cInput,cardno,sizeof(cInput)-1);
	vtcp_log("[%s][%d] your input is [%s]\n",__FILE__,__LINE__,cInput);
	for (i=strlen(cInput)-1; i>=0;)
	{
		sum += iCurrentDigit(cInput[i])*2>=10 ? 1+(iCurrentDigit(cInput[i])*2)%10 : iCurrentDigit(cInput[i])*2;
		i--;
		if(i>=0) sum +=iCurrentDigit(cInput[i]);
		i--;
	}
	sum=10-sum%10;
	if(sum==10)
		sum=0;
	vtcp_log("[%s][%d] sum ===[%d]\n",__FILE__,__LINE__,sum);
	return(sum+'0');
}
