/*************************************************
* 文 件 名: pubf_base_data.c
* 功能描述：   
*           pub_base_GetDataCode 取得数据编码
*           pub_base_GetDate     取得系统日期
*           pub_base_Ctonc       将数据写入数据字典缓冲区
*           pub_base_Nctoc       将数据字典缓冲区的数据读出
*           get_zd_data          取得指定数据编码的数据内容
*           set_zd_data          将数据内容写入数据字典缓冲区
*           get_zd_char          取得指定字符串数据编码的数据内容
*           set_zd_char          将字符串数据内容写入数据字典缓冲区
*           get_zd_short         取得指定短整数数据编码的数据内容，放入短整数中
*           set_zd_short        将短整数类型数据内容写入数据字典缓冲区
*           get_zd_int           取得指定整数数据编码的数据内容，放入整数中
*           set_zd_int           将整数类型数据内容写入数据字典缓冲区
*           get_zd_long          取得指定长整数数据编码的数据内容，放入长整数中
*           set_zd_long          将长整数类型数据内容写入数据字典缓冲区
*           get_zd_float    取得指定浮点类型数据编码的数据内容，放入符点类型数中
*           set_zd_float         将浮点类型数据内容写入数据字典缓冲区
*           get_zd_double   取得指定双精度类型数据编码的数据内容，放入双精度数中
*           set_zd_double        将双精度类型数据内容写入数据字典缓冲区
*           get_fd_data          取得指定数据编码的数据内容
*
* 作    者:
* 完成日期：   2003年12月16日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/

#include <stdio.h>
#include <time.h>
#define EXTERN
#include	"public.h"
#include	"svrpub_c.h"
#define FCHAR	0
#define FINT	1
#define FFLOAT	2
#define FDATE	3
#define FMONEY  4
#define FDGTCH  5
#define FPASS	6
#define FVARL	7


/**********************************************************************
 * 函 数 名：  pub_base_GetDataCode
 * 函数功能：  取得数据编码
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：sjbm：   数据编码
 *      
 *     输  出：无
 *
 *     返回值：>=0数据编码在数据字典缓冲区的位置
 *              -1失败
 *
 * 修改历史：
 *                   
********************************************************************/

int pub_base_GetDataCode(char *sjbm)
{
    register i;
    int low,high;
    int cc;
    FILE *fp;
    char cTxcode[5];
    memset(cTxcode,0,sizeof(cTxcode));
    memcpy(cTxcode,g_pub_tx.tx_code,sizeof(cTxcode)-1);
    
    if((memcmp(cTxcode,"5809",4)==0 || memcmp(cTxcode,"5826",4)==0))
    {
    	 for( i=0;i<PuVal_num;i++ )
    	 {
    	     if(memcmp(Pu_Val[i].sjbm,"1280",4)==0)
               vtcp_log("[%s][%d]Pu_Val[i].sjbm==[%s] Pu_Val[i].val=[%s]",__FILE__,__LINE__,Pu_Val[i].sjbm,(char*)Pu_Val[i].val);
       }    
    }	  
    if (sjbm[0]=='\0')
    {
    	vtcp_log("[%s][%d] sjbm==[%s] ",__FILE__,__LINE__,sjbm);
    	return -1;
    }
    low=0;
    high=PuVal_num-1;
		/*	大额查错用的 现在不用了
    if(memcmp(cTxcode,"5809",4)==0 || memcmp(cTxcode,"5826",4)==0)
    {
        vtcp_log("[%s][%d] PuVal_num==[%d] ",__FILE__,__LINE__,PuVal_num);
        vtcp_log("[%s][%d]      high==[%d] ",__FILE__,__LINE__,high);
    }    
		*/
    i=(high+low)/2;
    do
    {
        if (low>high)
        {
            vtcp_log("[%s][%d] low=[%d] high==[%d] ",__FILE__,__LINE__,low,high);
            return -1;
        }
        cc=strcmp(sjbm,Pu_Val[i].sjbm);
		/*	大额查错用的 现在不用了
        if(memcmp(cTxcode,"5809",4)==0 || memcmp(cTxcode,"5826",4)==0)
        {
            vtcp_log("[%s][%d]  sjbm==[%s] ",__FILE__,__LINE__,sjbm);
            vtcp_log("[%s][%d]      i=[%d] ",__FILE__,__LINE__,i);
            vtcp_log("[%s][%d]Pu_Val.sjbm==[%s] ",__FILE__,__LINE__,Pu_Val[i].sjbm);
            if(memcmp(Pu_Val[i].sjbm,"1280",4)==0)
               vtcp_log("[%s][%d]Pu_Val[i].sjbm==[%s] Pu_Val[i].val=[%s]",__FILE__,__LINE__,Pu_Val[i].sjbm,(char*)Pu_Val[i].val);
        }
		*/
        if (!cc)
        {
			/*vtcp_log("GET DATA CODE sjbm[%s] num[%d]",sjbm,i); */
            return i;
        }
        if (cc>0)
        {
            low=i+1;
        }else
        {
            high=i-1; 
        }
        i=(high+low)/2;
    } while(1);
}

/**********************************************************************
 * 函 数 名：  pub_base_GetDate
 * 函数功能：  取得系统日期
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：无
 *      
 *     输  出：year：  年
 *             month： 月
 *             day:    日
 *
 *     返回值：无
 *
 * 修改历史：
 *                   
********************************************************************/

void pub_base_GetDate(int *year, int *month, int *day)
{
	time_t t;
	struct tm *local_time;

	t = time(0);
	local_time = localtime(&t); 

	*year  = 1900 + local_time->tm_year;
	*month = local_time->tm_mon+1;
	*day   = local_time->tm_mday;

	return ;
}

/**********************************************************************
 * 函 数 名：  pub_base_Ctonc
 * 函数功能：  将数据写入数据字典缓冲区
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：r：     数据
 *             type：  缓冲区类型
 *             len:    缓冲区长度
 *             dec:    缓冲区小数位
 *      
 *     输  出：d：     数据缓冲区
 *
 *     返回值：无
 *
 * 修改历史：
 *                   
********************************************************************/

int pub_base_Ctonc(char *r,char *d, int type, int len, int dec)
{
	char *q,t[2000+1],*w;
	int 	i,k,l,p,neg=0;
	char 	date1[15];
	int 	mdy[3];

	memset(date1,0,sizeof(date1));
	memset(t , 0 , sizeof(t));
	memset(mdy , 0 , sizeof(mdy));
	/**
	pub_base_Ctonc(data,(char *)Pu_Val[i].val,Pu_Val[i].lx,Pu_Val[i].length,Pu_Val[i].prec);
	if(memcmp(sjbm,"1203",4)==0 && getenv("DEBUG_LOG")!=NULL)
	{
		vtcp_log("%s,%d 准备数据编码 %s ,data=[%s],i=[%d]",__FILE__,__LINE__,sjbm,data,i);
		vtcp_log("%s,%d pu_val[%d].val==[%s] lx=[%d],length=[%d] prec=[%i]",__FILE__,__LINE__,i,(char *)Pu_Val[i].val,Pu_Val[i].lx,Pu_Val[i].length,Pu_Val[i].prec);
	}
	#define FCHAR	0
	#define FINT	1
	#define FFLOAT	2
	#define FDATE	3
	#define FMONEY  4
	#define FDGTCH  5
	#define FPASS	6
	#define FVARL	7
	**/
	/*vtcp_log("%s,%d type=[%d],r=[%s],len=[%d]",__FILE__,__LINE__,type,r,len); */
	d[0] = 0;
	if (type==FVARL)
	{
		strncpy(d, r, len-1 );
		d[len-1] = 0;
		return 0;
	}
	if (len<=0) 
		return 1;
	else if (len>2000)
	{
		memcpy( d, r, len );
		return 0;
	}
	/*vtcp_log("[%s][%d] xyy type=[%d],r=[%s],len=[%d]",__FILE__,__LINE__,type,r,len);*/
	switch(type) 
	{
	case FCHAR:
	case FDGTCH:
	case FPASS:
		memset(t,' ',len);
		for (i=0; i<len&&r[i]; i++) t[i]=r[i];
		break;

	case FINT:
		memset(t,'0',len);
		/* 不做特殊处理 by LiuHuafeng 20080403
		if (r[0]=='-') 
		{ 
			neg = 1; 
			w = r+1; 
		} 
		else 
		********************************/
			w = r;
		for (i=0; i<=len; i++) 
		{
			/* begin add by LiuHuafeng 20080410 */	
			if (w[i]=='-') { k=i;  continue; }
			/* end by LiuHuafeng 20080410 */
			if (w[i]<'0' || w[i]>'9') 
				break;
		}
		for (i--,p=len-1; i>=0; i--,p--) 
			t[p] = w[i];
		/*vtcp_log("%s,%d t==[%s]",__FILE__,__LINE__,t); */
		break;

	case FFLOAT:
	case FMONEY:
		memset(t,'0',len);
		/* 不做特殊处理 by LiuHuafeng 20080403
		if (r[0]=='-') 
		{ 
			neg = 1;  
			w = r+1; 
		} 
		else 
		**********************************/
			w = r;
		for (k=0,i=0; i<=len; i++) 
		{
			/** rem by LiuHuafeng 20080403 如果碰见"-"号也继续 ,解决负号处理
			if (w[i]=='.') { k=i;  continue; }
			** replace by next line *****/
			if (w[i]=='.' || w[i]=='-') { k=i;  continue; }
			
			if (w[i]<'0' || w[i]>'9') 
				break;
		}
		l = i;	
		if (!k) 
			k = l;
		p = len-dec-1;
		for (i=1; i<=dec && k+i<l; i++) 
			t[p+i] = w[k+i];
		for (k--; k>=0; k--,p--) 
			t[p] = w[k];

		if(getenv("DEBUG_LOG")!=NULL)
			vtcp_log("%s,%d t==[%s]",__FILE__,__LINE__,t);
		break;
	case FDATE:
		if ( r[0]==0 || r[0]==' ')
		{
			t[0]=0;
			break;
		}
		mdy[0] = mdy[1] = mdy[2] = 0;

		sscanf(r,"%04d%02d%02d",&mdy[2],&mdy[0],&mdy[1]);
		mdy[2]%=10000;
		mdy[1]%=32;
		mdy[0]%=13;
		switch (len) 
		{
		case 4:
			sprintf(t,"%02d%02d",mdy[0],mdy[1]);
			break;
		case 6:
			sprintf(t,"%02d%02d%02d",mdy[2]%100,mdy[0],mdy[1]);
			break;
		case 8:
		default:
			sprintf(t,"%04d%02d%02d",mdy[2],mdy[0],mdy[1]);
			break;
		}
		break;
	}
	if (neg) 
	{
		/* rem by LiuHuafeng 测试不用这种模式，不知道为啥要对负号进行特殊处理 20080403
		vtcp_log("%s,%d t[%d]==%c",__FILE__,__LINE__,len-1,t[len-1]);
		if (t[len-1]=='0') 
			t[len-1]  = 0x7d;
		else 
			t[len-1] += 0x19;
		vtcp_log("%s,%d t[%d]==%c",__FILE__,__LINE__,len-1,t[len-1]);
		****************************************************************/
	}
	memcpy( d, t, len );

	return 0;
}

/**********************************************************************
 * 函 数 名：  pub_base_Nctoc
 * 函数功能：  将数据字典缓冲区的数据读出
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：s：     数据缓冲区
 *             type：  缓冲区类型
 *             len:    缓冲区长度
 *             dec:    缓冲区小数位
 *      
 *     输  出：t：     数据
 *
 *     返回值：无
 *
 * 修改历史：
 *                   
********************************************************************/
int pub_base_Nctoc(char *s, char *t, int type,int len,int dec)
{ 
	register i;
	char *q;
	int zero=1,l,p,neg=0;
	int j=0,x=0;
	char r[2000+1];
	int mdy[3];

	int y,m,d;

	memset(r,0,sizeof(r));
	t [0] = 0;
	if (type==FVARL)
	{
		strcpy( t, s );
		return 0;
	}
	if (len<=0) 
		return 1;
	else if (len>2000)
	{
		memcpy( t, s, len );
		t[len] = 0;
		return 0;
	}
	memcpy( r, s, len );
	r[len] = 0;

	if (type == FINT || type == FFLOAT || type == FMONEY ) 
	{
		if (r[len-1]>'9') 
		{
			/* rem by LiuHuafeng 20080403 不知道老程序为什么对负号特殊处理，去掉
			if (r[len-1]==0x7d) 
				r[len-1] = '0';
			else    
				r[len-1] -= 0x19;
			neg=1;
			*************************************************************/
		}
	}
	switch(type) 
	{
	case FCHAR:
	case FDGTCH:
	case FPASS:       
		memcpy( t, r, len );
		t[len] = 0;
		break;

	case FINT:
		l=0;
		if (neg) 
			t[l++]='-';
		for (i=0; i<len; i++) 
		{
			if ((r[i]=='0'||r[i]==' ') && zero) continue;
			zero=0;
			t[l++]=r[i];
		}
		if (l==neg) 
		{
			t[0] = '0'; 
			l = 1;
		}
		t[l] = '\0';
		break;

	case FFLOAT:
	case FMONEY:
		l=0;
		if (neg) t[l++]='-';
		
		for (i=0; i<len-dec; i++)
		{
			if ((r[i]=='0'||r[i]==' ') && zero) continue;
			zero=0;
			t[l++]=r[i];
		}
		if (l==neg)
		{
			t[neg] = '0';
			l++; 
		}
		t[l++]='.';
		if (dec!=0) memcpy( t+l, r+i, dec );
		*(t+l+dec) = 0;
		if(dec!=0)
		{
			for(j=0,x=0;j<l+dec;j++)/**在小数点后补一个0,如果为空的话xyy**/
			{
				if(t[j]!='.')
				{
					if(x==1)
					{
						if(t[j]==' ')
						{
							t[j]='0';
						}
					}
					else
						continue;
				}
				else
				{
					x = 1;
				}
			}
		}
		/***end by xyy ***/
		if (t[l]=='\0')
		{
			t[l++]='0'; 
			t[l++]='0'; 
			t[l]='\0'; 
		}
		break;

	case FDATE:
		pub_base_GetDate(&y,&m,&d);

		r[len] = 0;
		if (r[0]=='\0' || r[0]==' ') 
		{ 
			t[0]=0;
			break; 
		}
		switch (len) 
		{
		case 4:
			sscanf(r,"%02d%02d",&mdy[0],&mdy[1]);
			mdy[2] = y;
			break;
		case 6:
			sscanf(r,"%02d%02d%02d",&mdy[2],&mdy[0],&mdy[1]);
			mdy[2] += (mdy[2]>=80) ? 1900 : 2000 ;
			break;
		case 8:
		default:
			sscanf(r,"%04d%02d%02d",&mdy[2],&mdy[0],&mdy[1]);
			mdy[2] += (mdy[2]<100) ? 1900 : 0 ;
			break;
		}
		sprintf(t,"%04d%02d%02d",mdy[2],mdy[0],mdy[1]);
		break;
	}
	return 0;
}

/**********************************************************************
 * 函 数 名：  get_zd_data
 * 函数功能：  取得指定数据编码的数据内容
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：sjbm:    数据编码
 *      
 *     输  出：data:    返回的数据
 *
 *     返回值：取得的数据
 *             NULL失败
 *
 * 修改历史：
 *                   
********************************************************************/

char *get_zd_data( char *sjbm, char *data )
{
	char	tmpdata[1024];
	int i;

	data[0] = 0;

	i = pub_base_GetDataCode( sjbm );
	if ( i<0 ) 
	{
		vtcp_log("%s,%d i==%d ",__FILE__,__LINE__,i);TRACE;
		return NULL;
	}
	pub_base_Nctoc((char*)Pu_Val[i].val,tmpdata,Pu_Val[i].lx,Pu_Val[i].length,Pu_Val[i].prec);

	ldchar( tmpdata,strlen(tmpdata),data );

	return data;
}

/**********************************************************************
 * 函 数 名：  set_zd_data
 * 函数功能：  将数据内容写入数据字典缓冲区
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：sjbm:    数据编码
 *             data:    数据内容
 *
 *     输  出：无
 *
 *     返回值：取得的数据
 *             NULL失败
 *
 * 修改历史：
 *                   
********************************************************************/

char *set_zd_data( char *sjbm, char *data )
{
	int i;
	i = pub_base_GetDataCode( sjbm );
	if ( i<0 ) 
		return NULL;

	pub_base_Ctonc(data,(char *)Pu_Val[i].val,Pu_Val[i].lx,Pu_Val[i].length,Pu_Val[i].prec);
	return data;

}


/**********************************************************************
 * 函 数 名：  get_zd_char
 * 函数功能：  取得指定字符串数据编码的数据内容
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：sjbm:    数据编码
 *      
 *     输  出：data:    返回的数据
 *
 *     返回值：取得的数据
 *             NULL失败
 *
 * 修改历史：
 *                   
********************************************************************/

char get_zd_char( char *sjbm, char *data )
{
	int i;

	*data = 0;

	i = pub_base_GetDataCode( sjbm );
	if ( i<0 ) 
		return NULL;
	
	pub_base_Nctoc((char
*)Pu_Val[i].val,data,Pu_Val[i].lx,Pu_Val[i].length,Pu_Val[i].prec);

	*data = Pu_Val[i].val[0];

	return *data;
}

/**********************************************************************
 * 函 数 名：  set_zd_char
 * 函数功能：  将字符串数据内容写入数据字典缓冲区
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：sjbm:    数据编码
 *             data:    数据内容
 *
 *     输  出：无
 *
 *     返回值：取得的数据
 *             NULL失败
 *
 * 修改历史：
 *                   
********************************************************************/

char set_zd_char( char *sjbm, char data )
{
	int i;
	char str[2];

	str[0] = data;
	str[1] = 0;

	i = pub_base_GetDataCode( sjbm );
	if ( i<0 ) 
		return NULL;

	pub_base_Ctonc(str,(char
*)Pu_Val[i].val,Pu_Val[i].lx,Pu_Val[i].length,Pu_Val[i].prec);
	
	return data;
}


/**********************************************************************
 * 函 数 名：  get_zd_short
 * 函数功能：  取得指定短整数数据编码的数据内容，放入短整数中
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：sjbm:      数据编码
 *      
 *     输  出：shortdata: 返回的数据
 *
 *     返回值：取得的数据
 *             NULL失败
 *
 * 修改历史：
 *                   
********************************************************************/

short get_zd_short( char *sjbm, short *shortdata )
{
	int i;
	char data[200];

	data[0] = 0;

	i = pub_base_GetDataCode( sjbm );
	if ( i<0 ) 
		return 0;
	
	pub_base_Nctoc((char
*)Pu_Val[i].val,data,Pu_Val[i].lx,Pu_Val[i].length,Pu_Val[i].prec );

	sscanf( data, "%d", &i );
	*shortdata = (short)i;
	return *shortdata;
}

/**********************************************************************
 * 函 数 名：  set_zd_short
 * 函数功能：  将短整数类型数据内容写入数据字典缓冲区
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：sjbm:      数据编码
 *             shortdata: 数据内容
 *
 *     输  出：无
 *
 *     返回值：取得的数据
 *             NULL失败
 *
 * 修改历史：
 *                   
********************************************************************/

short set_zd_short( char *sjbm, short shortdata )
{
	int i;
	char data[200];

	sprintf(data,"%d", shortdata);

	i = pub_base_GetDataCode( sjbm );
	if ( i<0 ) 
		return 0;
	
	pub_base_Ctonc(data,(char
*)Pu_Val[i].val,Pu_Val[i].lx,Pu_Val[i].length,Pu_Val[i].prec);

	return shortdata;
}


/**********************************************************************
 * 函 数 名：  get_zd_int
 * 函数功能：  取得指定整数数据编码的数据内容，放入整数中
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：sjbm:      数据编码
 *      
 *     输  出：intdata:   返回的数据
 *
 *     返回值：取得的数据
 *             NULL失败
 *
 * 修改历史：
 *                   
********************************************************************/

int get_zd_int( char *sjbm, int *intdata )
{
	int i;
	char data[200];

	data[0] = 0;

	i = pub_base_GetDataCode( sjbm );
	if ( i<0 ) 
		return 0;
	
	pub_base_Nctoc((char
*)Pu_Val[i].val,data,Pu_Val[i].lx,Pu_Val[i].length,Pu_Val[i].prec );

	sscanf( data, "%d", intdata );
	return *intdata;
}

/**********************************************************************
 * 函 数 名：  set_zd_int
 * 函数功能：  将整数类型数据内容写入数据字典缓冲区
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：sjbm:      数据编码
 *             intdata:   数据内容
 *
 *     输  出：无
 *
 *     返回值：取得的数据
 *             NULL失败
 *
 * 修改历史：
 *                   
********************************************************************/

int set_zd_int( char *sjbm, int intdata )
{
	int i;
	char data[200];

	sprintf(data,"%d", intdata);

	i = pub_base_GetDataCode( sjbm );
	if ( i<0 ) 
		return 0;
	
	pub_base_Ctonc(data,(char
*)Pu_Val[i].val,Pu_Val[i].lx,Pu_Val[i].length,Pu_Val[i].prec);

	return intdata;
}


/**********************************************************************
 * 函 数 名：  get_zd_long
 * 函数功能：  取得指定长整数数据编码的数据内容，放入长整数中
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：sjbm:      数据编码
 *      
 *     输  出：longdata:  返回的数据
 *
 *     返回值：取得的数据
 *             NULL失败
 *
 * 修改历史：
 *                   
********************************************************************/

long get_zd_long( char *sjbm, long *longdata )
{
	int i;
	char data[200];

	FILE *fp;



	data[0] = 0;

	i = pub_base_GetDataCode( sjbm );
	if ( i<0 ) 
		return 0;
	
	pub_base_Nctoc((char
*)Pu_Val[i].val,data,Pu_Val[i].lx,Pu_Val[i].length,Pu_Val[i].prec );

	sscanf( data, "%ld", longdata );
/*
	rstol(data,&longdata);
*/
	return *longdata;
}

/**********************************************************************
 * 函 数 名：  set_zd_long
 * 函数功能：  将长整数类型数据内容写入数据字典缓冲区
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：sjbm:      数据编码
 *             longdata:  数据内容
 *
 *     输  出：无
 *
 *     返回值：取得的数据
 *             NULL失败
 *
 * 修改历史：
 *                   
********************************************************************/

long set_zd_long( char *sjbm, long longdata )
{
	int i;
	char data[200];

	sprintf(data,"%ld", longdata);

	i = pub_base_GetDataCode( sjbm );
	if ( i<0 ) 
		return 0;
	
	pub_base_Ctonc(data,(char
*)Pu_Val[i].val,Pu_Val[i].lx,Pu_Val[i].length,Pu_Val[i].prec);

	return longdata;
}


/**********************************************************************
 * 函 数 名：  get_zd_float
 * 函数功能：  取得指定浮点类型数据编码的数据内容，放入符点类型数中
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：sjbm:      数据编码
 *      
 *     输  出：floatdata: 返回的数据
 *
 *     返回值：取得的数据
 *             NULL失败
 *
 * 修改历史：
 *                   
********************************************************************/

float get_zd_float( char *sjbm, float *floatdata )
{
	int i;
	char data[200];

	data[0] = 0;

	i = pub_base_GetDataCode( sjbm );
	if ( i<0 ) 
		return 0;
	
	pub_base_Nctoc((char
*)Pu_Val[i].val,data,Pu_Val[i].lx,Pu_Val[i].length,Pu_Val[i].prec);

	sscanf( data, "%f", floatdata );
	return *floatdata;
}


/**********************************************************************
 * 函 数 名：  set_zd_float
 * 函数功能：  将浮点类型数据内容写入数据字典缓冲区
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：sjbm:      数据编码
 *             floatdata: 数据内容
 *
 *     输  出：无
 *
 *     返回值：取得的数据
 *             NULL失败
 *
 * 修改历史：
 *                   
********************************************************************/

float set_zd_float( char *sjbm, float floatdata )
{
	int i;
	char data[200];

	sprintf(data,"%f", floatdata);

	i = pub_base_GetDataCode( sjbm );
	if ( i<0 ) 
		return 0.0;
	
	pub_base_Ctonc(data,(char
*)Pu_Val[i].val,Pu_Val[i].lx,Pu_Val[i].length,Pu_Val[i].prec);

	return floatdata;
}



/**********************************************************************
 * 函 数 名：  get_zd_double
 * 函数功能：  取得指定双精度类型数据编码的数据内容，放入双精度数中
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：sjbm:       数据编码
 *      
 *     输  出：doubledata: 返回的数据
 *
 *     返回值：取得的数据
 *             NULL失败
 *
 * 修改历史：
 *                   
********************************************************************/

double get_zd_double( char *sjbm, double *doubledata )
{
	int i;
	char data[200];

	data[0] = 0;

	i = pub_base_GetDataCode( sjbm );
	if ( i<0 ) 
		return 0;
	
	pub_base_Nctoc((char
*)Pu_Val[i].val,data,Pu_Val[i].lx,Pu_Val[i].length,Pu_Val[i].prec);
	/***test****/
	vtcp_log("[%s][%d]val=[%.10s],lx=[%d],length=[%d],prev=[%d]\n",
		__FILE__,__LINE__,Pu_Val[i].val,Pu_Val[i].lx,Pu_Val[i].length,Pu_Val[i].prec);

	sscanf( data, "%lf", doubledata );

	return *doubledata;
}


/**********************************************************************
 * 函 数 名：  set_zd_double
 * 函数功能：  将双精度类型数据内容写入数据字典缓冲区
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：sjbm:       数据编码
 *             doubledata: 数据内容
 *
 *     输  出：无
 *
 *     返回值：取得的数据
 *             NULL失败
 *
 * 修改历史：
 *                   
********************************************************************/

double set_zd_double( char *sjbm, double doubledata )
{
	int i;
	char data[200];
	double lx;

	char LX[100],Len[100],Deci[100];

	sprintf(data,"%lf", doubledata);

	i = pub_base_GetDataCode( sjbm );
	if ( i<0 ) 
		return 0.0;

	
	if(Pu_Val[i].prec==2)
	{
		lx = pub_base_PubDround_1(doubledata);
		sprintf(data,"%.2f", lx);
	}

/**	
	vtcp_log("the data in set_zd_double[%s][%s][%lf]",sjbm,data,doubledata);
**/


	pub_base_Ctonc(data,(char
*)Pu_Val[i].val,Pu_Val[i].lx,Pu_Val[i].length,Pu_Val[i].prec);



	return doubledata;
}

pub_base_dic_show_str( char *mo,char *code,char *sval )
{
	struct dic_data_c vdic;
	int i=0;
	int ret=0;

		pub_base_strpack( code );
		pub_base_strpack( sval );

	if( g_dic_data_i==0 )
	{
		memset( g_dic_data_a,0,sizeof(g_dic_data_a) );
	}
	else
	{
		for( i=0;i<g_dic_data_i;i++ )
		{

			if( !strcmp(code,g_dic_data_a[i].pr_code) && 
				!strcmp(sval,g_dic_data_a[i].pr_seq) )
			{
/*vtcp_log("memG DIC_DATA pr_code='%s' and pr_seq='%s' ",code,sval);*/
				strcpy( mo,g_dic_data_a[i].content );
				return 0;
			}
		}
	}
vtcp_log("read DIC_DATA pr_code='%s' and pr_seq='%s' ",code,sval);

	ret=Dic_data_Sel(g_pub_tx.reply,&vdic,"pr_code='%s' and pr_seq='%s' ",
		code,sval ) ;
	if( ret )
	{
		strcpy( vdic.pr_code,code );
		strcpy( vdic.pr_seq,sval );
		strcpy( vdic.content,sval );
	}
	else
	{
		pub_base_strpack( vdic.pr_code );
		pub_base_strpack( vdic.pr_seq );
		pub_base_strpack( vdic.content );
	}
	strcpy( mo,vdic.content );

	if( g_dic_data_i<100 )
	{
		memcpy( &g_dic_data_a[g_dic_data_i],&vdic,sizeof(vdic) );
		i=g_dic_data_i;
vtcp_log("memI DIC_DATA %d '%s - %s'",i,
			g_dic_data_a[i].pr_code,g_dic_data_a[i].pr_seq);
		g_dic_data_i++;
	}

	return 0;
}
/**************************************************************/
/*  Name     : apatoi()-                                      */
/*                                                            */
/*  Function : CONVERT ASCII BUFFER  TO INTERGER              */
/*                                                            */
/*  Synopsis :                                                */
/*             int  apatoi(ibuf,len)                          */
/*             char   *ibuf;                                  */
/*             int    len;                                    */
/*  Return   :   the actual value of ascii buffer in integer  */
/*               95/01/20 : if not ascii return 0;            */
/*               -- Error : -9999  no longer use              */
/*                                                            */
/*  Remark   :  apatoi converts the input ascii buffer with   */
/*              input length "len", into integer value.       */
/*              The input buffer need not null terminated.    */
/*              If the input length "len" is not enough for   */
/*              If the input buffer is not digit , return     */
/*              -9999.                                        */
/**************************************************************/

int apatoi( ibuf , len )
char	*ibuf; 
int	len;
{
	char	temp[16];

	memset(temp,0x00,sizeof(temp));
	memcpy(temp,ibuf,len);
	return(atoi(temp));
}
/**************************************************************/
/*  Name     : apitoa()-                                      */
/*                                                            */
/*  Function : CONVERT INTERGER TYPE TO ASCII BUFFER          */
/*WITHOUT NULL TERMINATED.                                    */
/*  Synopsis :                                                */
/*int  apitoa(inum,len,obuf)                                  */
/*long   inum;                                                */
/*int    len;                                                 */
/*char   *obuf;                                               */
/*  Return   :  0                                             */
/*                                                            */
/*  Remark   :  apitoa converts the input number inum in long */
/*type, with input length "len",                              */
/*into ascii buffer without null terminated.                  */
/*If the input length "len" is not enough for                 */
/*converted buffer , data will be truncated.                  */
/*Be ware of allocate the enough memory for the               */
/*obuf, because apitoa will NOT allocate memory.              */
/**************************************************************/


int apitoa( inum , len , obuf )
long    inum;
int     len;
char    *obuf;
{
	static char     temp[50];
	char    fmt[10];

	printf("[%s][%d] inum[%d], len[%d]\n", __FILE__, __LINE__, inum, len);

	sprintf(fmt,"%%0%dld",len);
	sprintf(temp,fmt,inum);
	printf("[%s][%d] temp = [%s]\n", __FILE__, __LINE__, temp);
	memcpy(obuf,temp,len);
	return(0);
}

/**********************************************************************
 * 函 数 名：  get_fd_data
 * 函数功能：  取得指定数据编码的数据内容
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：sjbm:    数据编码
 *      
 *     输  出：data:    返回的数据
 *
 *     返回值：取得的数据
 *             NULL失败
 *
 * 修改历史：
 *                   
********************************************************************/

char *get_fd_data( char *sjbm, char *data )
{
	char	tmpdata[1024];
	int i;
	char cTxcode[5];

	memset(cTxcode,0,sizeof(cTxcode));
	memset(tmpdata, 0 , sizeof(tmpdata));
	data[0] = 0;
	vtcp_log("[%s][%d]这里是get_fd_data*****************sjbm=[%s]",__FILE__,__LINE__,sjbm);
	i = pub_base_GetDataCode( sjbm );
	if ( i<0 ) 
	{
		vtcp_log("%s,%d i==%d ",__FILE__,__LINE__,i);TRACE;
		return NULL;
	}
	memcpy(cTxcode,g_pub_tx.tx_code,sizeof(cTxcode)-1); 
	if((memcmp(cTxcode,"5809",4)==0 || memcmp(cTxcode,"5826",4)==0))
	{
		if(memcmp(Pu_Val[i].sjbm,"1280",4)==0)
			vtcp_log("[%s][%d]$$$$$$$$$$i=[%d] pub_val[i].val=[%s]",__FILE__,__LINE__,i,(char*)Pu_Val[i].val);
	}
	pub_base_Nctoc((char*)Pu_Val[i].val,tmpdata,Pu_Val[i].lx,Pu_Val[i].length,Pu_Val[i].prec);  
	if(strlen(tmpdata)==sizeof(tmpdata))
		memcpy(data,tmpdata,sizeof(tmpdata));
	else
		memcpy(data,tmpdata,strlen(tmpdata));

	return data;
}

