#include "find_debug.h" 
/*************************************************
* 文 件 名:	pubf_com_despack.c     （despack）对数据包进行加密
*
* 功能描述：pub_com_des_pack       （des_pack ）   对数据包进行加密  
*           pub_com_dat_asctobcd   （dat_asctobcd）ASCII码转换成BCD码
*     
* 作    者:	
* 完成日期：	2003年12月16日
* 修改记录：	
* 1. 日    期:
*    修 改 人:
*    修改内容:
*************************************************/

#include <stdio.h>
#include <stdlib.h>

/**********************************************************************
* 函 数 名：	pub_com_des_pack
* 函数功能：    对数据包进行加密
* 作者/时间：	
* 
* 参    数：
*     输入：packlen: 
*           scode:
*           pack:
*           mkey:
*
*     输出：无
*     
* 返 回 值: msg:
*
* 修改历史：
*
********************************************************************/

unsigned char *pub_com_des_pack(scode,pack,packlen,mkey)
unsigned char *scode,*pack,*mkey;
int packlen;
{
	register	i,j,m;
	int			num,mod;
	unsigned	char	temp[10];
	unsigned	char	temp0[10];
	unsigned	char	b_key[20];
	unsigned	char	buf[3024];
	char		msg[17];

	MEMSET_DEBUG(buf,0x00,sizeof(buf));
	memmove(buf,pack,packlen);
	mod=packlen%8;
	num=packlen/8;
	if(mod)
	{
		for (i=0;i<8-mod;i++)
		{
			memmove(buf+packlen+i,0x00,1);
		}
		num++;
	}
	MEMSET_DEBUG(temp0,0x00,sizeof(temp0));
	MEMCPY_DEBUG(temp0,scode,8);

	i=m=0;
	while(1)
	{
		if(m>=num)
		{
			break;
		}
		MEMSET_DEBUG(temp,0x00,sizeof(temp));
		MEMCPY_DEBUG(temp,buf+i,8);
		for(j=0;j<8;j++)
		{
			temp0[j]^=temp[j];
		}
		i+=8;
		m++;
	}
	MEMSET_DEBUG(b_key,0x00,sizeof(b_key));
	pub_com_dat_asctobcd(b_key,mkey,16);
	/*encrypt_my(1,temp0,b_key); */

	j=0;
	for (i=0; i < 8; i ++)
	{
		sprintf(msg+j,"%02x",temp0[i]);
		j+=2;
	}

	return msg;
}

/**********************************************************************
* 函数名：	pub_com_dat_asctobcd
* 函数功能：ASCII码转换成BCD码	
* 作者/时间：	
* 
* 参数：
*     输入：asc_len:   ASCII码长度
*	          asc	:      ASCII码内容
*           
*     输出：bcd:       BCD码内容
*     
* 返 回 值: 无
*
* 修改历史：
*
********************************************************************/

pub_com_dat_asctobcd(char *bcd,char *asc, int asc_len )
{
	char is_high, by;

	is_high = !(asc_len % 2);
	*bcd = 0x00;

	while(asc_len-->0)
	{
		by = *asc++;

		if ( !(by&0x10)&&(by>0x30)) by += 9;
		/*对字母和空格的处理,小写变大写,空格变0*/
		if (is_high)  *bcd = by << 4;
		else
		{
			by&=0x0f;
			*bcd++ |= by;
		}
		is_high = !is_high;
	}

}

