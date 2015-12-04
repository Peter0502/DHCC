#include "find_debug.h" 
/*************************************************
* 文 件 名:	pub_base_str
* 功能描述：    1、包括模块pub_base_strncpyr字符串拷贝；
*               2、包括模块pub_base_ltrim去掉字符串左边的空格
*               3、包括模块pub_base_rtrim去掉字符串右边的空格
*               4、包括模块pub_base_strn_cpy字符串截取。       
*               5、包括模块pub_base_DelSpace去字符串尾部空格.
*               6、包括模块pub_base_StrCmp字符串比较.
*               7、包括模块pub_base_strpack去掉字符串空格.
*               8、包括模块pub_base_strpack_all去掉字符串中所有空格.
*               9、pub_base_ltrim_zero去掉字符串左边的零
*               10,pub_base_full_space 将指定长度内的\0转换为空格
*               11,pub_base_full_anti_zero将指定长度内的空格转换为\0
*               12,pub_base_full_zero将指定长度内的空格转换为0
*************************************************/

#include       <stdio.h>
#include       <math.h>
#include       <malloc.h>
#include       <sys/stat.h>
#include       <unistd.h>
#include       <time.h>
#define EXTERN
#include "public.h"

#define        _MIN(a,b)       ((a)<(b)? (a) : (b))
/**********************************************************************
 * 函 数 名：	pub_base_strncpyr
 * 函数功能：	字符串拷贝
 * 作者/时间：	
 * 
 * 参数：
 *     输入: source	，源
 *           len	，长度
 *     输出: target;
 *     返回: 0   成功
 *           非0 失败
 * 修改历史：
 *
********************************************************************/

int pub_base_strncpyr( char target[], char source[], int len )
{
        int i=0;

        if ( len == 0 ) target[0] = '\0';

        if ( len > strlen(source) ) strcpy( target , source );

        while ( len ){
                target[i] = source[ strlen(source) - len ];
                i++;
                len--;
                }
        target[i]='\0';
        return (0);
}

/**********************************************************************
 * 函 数 名：   pub_base_ltrim
 * 函数功能：   去掉字符串左边的空格
 * 作者/时间：
 *
 * 参数：
 *     输入: str要去掉空格的字符串
 *     输出: str去掉空格后的字符串
 *     返回: 0   成功
 *           非0 失败
 *
 * 修改历史：
 *
********************************************************************/
int pub_base_ltrim( char str[] )
{
        int     i, nlen;
        nlen = strlen(str);
        for(i = 0; i < nlen; i ++)
                if(!isspace(str[i]))
                        break;
        if(i < nlen)
        {
                MEMCPY_DEBUG(str, str + i, (nlen - i));
                str[(nlen - i)] = 0;
        }
        else
                str = NULL;
        return(0);
}
/**********************************************************************
 * 函 数 名：   pub_base_rtrim
 * 函数功能：   去掉字符串右边的空格
 * 作者/时间：
 *
 * 参数：
 *     输入: str要去掉空格的字符串
 *     输出: str去掉空格后的字符串
 *     返回: 0   成功
 *           非0 失败
 *
 * 修改历史：
 *
********************************************************************/
int pub_base_rtrim( char str[] )
{
        int     i, nlen;
        nlen = strlen(str);
/*      for(i = 0; i < nlen; i ++)
*/      for(i = nlen -1; i >= 0; i --)
                if(isspace(str[i]))
                        str[i] = '\0';
                else
                        break;
        return(0);
}
/**********************************************************************
 * 函 数 名：   pub_base_strn_cpyc
 * 函数功能：   字符串截取2.输入的起始位置包括当前位置截取len长字符传。
 *              如果起始位置是半个汉字即向前窜一个字符。如果截取len长
 *              字符后即向前窜一个字符.
 * 作者/时间：
 *
 * 参数：
 *     输入: msg             输入字符串。
             qs              截取的起始位置。(qs > =1 )
             len             截取的长度。
 *     输出: rmsg            返回字符串.
 *     返回: char *rtu_msg	 字符串被截取后的起始位置
 *
 * 修改历史：
 *
********************************************************************/
char *pub_base_strn_cpyc(char *msg ,int qs,int len, char *rmsg)
{
 int i,j,k,m,n,p,q;
 char *rtu_msg;

 m = qs ;
 p = len;
 j=0;k=0;n=0;
  if ( m >0 )
    {
        for ( i=0; i<m-1; i++)
           {
                k = msg[i];
                if ( k <0 || k > 128 )
                {
                   n++;
                   if ( n%2 == 0 )
                   j=0;
                   else
                   j=1;
                }
                else
                {
                          n=0;
                          j=0;
                        }
                }
                if ( j == 1 )
                        m=m-2 ;
                else
                        m--;
        }
        j=0;k=0;n=0;
        for ( i=m;i<m+p;i++)
        {
                k = msg[i];
                if ( k <0 || k > 128 )
                {
                        n++;
                        if ( n%2 == 0 )
                                j=0;
                        else
                                j=1;
                }
                else
                {
                       n=0;
                        j=0;
                }
        }
        if ( j == 1 ) p-- ;
        strncpy(rmsg,msg+m,p);
        qs  = m;
        len = p;

        return msg+m+p;
}


/**********************************************************************
 * 函 数 名：   pub_base_strn_cpy
 * 函数功能：   字符串截取.输入的起始位置包括当前位置截取len长字符传。
 *              如果起始位置是半个汉字即向前窜一个字符。如果截取len长
 *              字符后即向前窜一个字符.
 * 作者/时间：
 *
 * 参数：
 *     输入: msg             输入字符串。
             qs              截取的起始位置。(qs > =1 )
             len             截取的长度。
 *     输出: rmsg            返回字符串.
 *     返回: 0   没有改变原内容
 *
 * 修改历史：
 *
********************************************************************/
int pub_base_strn_cpy(char *msg ,int qs,int len, char *rmsg)
{
 int i,j,k,m,n,p,q;

 m = qs ;
 p = len;
 j=0;k=0;n=0;
  if ( m >0 )
    {
        for ( i=0; i<m-1; i++)
           {
                k = msg[i];
                if ( k <0 || k > 128 )
                {
                   n++;
                   if ( n%2 == 0 )
                   j=0;
                   else
                   j=1;
                }
                else
                {
                          n=0;
                          j=0;
                        }
                }
                if ( j == 1 )
                        m=m-2 ;
                else
                        m--;
        }
        j=0;k=0;n=0;
        for ( i=m;i<m+p;i++)
        {
                k = msg[i];
                if ( k <0 || k > 128 )
                {
                        n++;
                        if ( n%2 == 0 )
                                j=0;
                        else
                                j=1;
                }
                else
                {
                       n=0;
                        j=0;
                }
        }
        if ( j == 1 ) p-- ;
		vtcp_log("msg[%s]m[%d]p[%d]",msg,m,p);
        strncpy(rmsg,msg+m,p);
		vtcp_log("return msg[%s]",msg+m+p);
        qs  = m;
        len = p;

        return 0;
}

/**********************************************************************
 * 函 数 名：   pub_base_DelSpace
 * 函数功能：   去字符串尾部空格.
 * 作者/时间：
 *
 * 参数：
 *     输入: s 输入字符串.
 *     输出: n 截取的长度.
 *     返回: 0   成功
 *           非0 失败
 *
 * 修改历史：
 *
********************************************************************/
char *pub_base_DelSpace( s, n )
char    *s;
int     n;
{
   n = _MIN( (int)strlen(s), n );
   while( n-- && isspace(s[n] & 0x7f) );
   s[ n+1 ] = '\0';
   return s;
}

/**********************************************************************
 * 函 数 名：   pub_base_StrCmp
 * 函数功能：   比较两个字符串.
 * 作者/时间：
 *
 * 参数：
 *     输入: str1 字符串�
 *           str2 字符串.
 *     输出: 无.
 *     返回: 0   成功
 *           非0 失败
 *
 * 修改历史：
 *
********************************************************************/
int pub_base_StrCmp(char *str1,char *str2)
{
 int i,ret,len;

 if(strlen(str1) != strlen(str2))
                return -1;

        len = strlen(str1);
        if(len == 0)
                return -1;

        ret = 0;

        for(i=0;i<len;i++)
        {
                if(toupper(str1[i]) == toupper(str2[i]))
                        continue;
                else
                {
                        ret = -1;
                        break;
                }
        }
        return ret;
}

/**********************************************************************
 * 函 数 名：   pub_base_strpack_all
 * 函数功能：   去掉字符串中所有的空格.
 * 作者/时间：
 *
 * 参数：
 *     输入: str 压缩前的字符串.
 *     输出: str 压缩后的字符串.
 *     返回: 0   成功
 *           非0 失败
 *
 * 修改历史：
 *
********************************************************************/

char * pub_base_strpack_all( char *str )
{
	int i=0;
	int j=0;

	for (i=0,j=0; i<strlen(str); i++)
	{
		if( str[i] != ' ' )
		{
			str[j] = str[i];
			j++;
		}
	}

	str[j] = '\0';

	return str;
}

/**********************************************************************
 * 函 数 名：   pub_base_strpack
 * 函数功能：   去掉字符串的空格.
 * 作者/时间：
 *
 * 参数：
 *     输入: str 压缩前的字符串.
 *     输出: str 压缩后的字符串.
 *     返回: 0   成功
 *           非0 失败
 *
 * 修改历史：
 *
********************************************************************/

char * pub_base_strpack( char *str )
{
 int i;
 int b1,e1;

        for (i=0; str[i]==' '; i++) ;

        b1 = i;

        for (i=strlen(str)-1; i>=b1 && str[i]==' '; i--);

        e1 = i;

        if (e1 >= b1)
                MEMCPY_DEBUG( str, str+b1, e1-b1+1 );
        str[e1-b1+1] = 0;

        return str;
}

/**********************************************************************
 * 函 数 名：   pub_base_GetFirstSubStr
 * 函数功能：   依据逗号","截取第一个子串.
 * 作者/时间：
 *
 * 参数：
 *     输入: 
 *		s_str 	源字符串.
 *     输出: 
 *		sub_str 第一个子字符串.
 *     返回: 0   成功  -1 失败
 *
 * 修改历史：
 *
********************************************************************/
int pub_base_GetFirstSubStr(char  *s_str, char *sub_str)
{
    int     i, k = 0;
    int     str_len;

    /***  去掉字符串前后空格  ***/
    pub_base_strpack(s_str);

    str_len = strlen(s_str);

    for(i = 0; i<=str_len; i++,k++)
    {
   	if( s_str[i] == ',' || s_str[i] == '\0' || s_str[i] == '\n')
   	{
   		break;
   	}
   	sub_str[k] = s_str[i];
    }
    sub_str[k+1] = '\0';

    if(s_str[i] == '\0' || s_str[i] == '\n')
    {
        strcpy(s_str, s_str+i);
    }
    else
    {
        strcpy(s_str,s_str+i+1);
    }
    
    return 0;
}

/**********************************************************
 * 函 数 名：   pub_base_DelSpaces
 * 函数功能：   去掉字符串前后的空格和回车换行符
 * 作者/时间：
 *
 * 参数：
 *     输入: 
 *		char *str 	源字符串.
 *     输出: 
 *		char *str 	去掉前后空格后的字符串.
 *     返回: 0   成功
 *
 * 修改历史：
 *
***********************************************************/
int pub_base_DelSpaces( char *str )
{
	pub_base_DelForeSpace( str );
	pub_base_DelAfterSpace( str );
	return 0;
}
/**********************************************************
 * 函 数 名：   pub_base_DelForeSpace
 * 函数功能：   去掉字符串前面的空格和tab键
 * 作者/时间：
 *
 * 参数：
 *     输入: 
 *		char *str 	源字符串.
 *     输出: 
 *		char *str 	去掉前后空格后的字符串.
 *     返回: 0   成功
 *
 * 修改历史：
 *
***********************************************************/
int pub_base_DelForeSpace(char *_str)
{
    int     i, k = 0;
    int     str_len;

    str_len = strlen(_str);

    for(i = 0; i<str_len; i++,k++)
    {
        if( _str[i] == ' ' || _str[i] == '\t' ) continue;
        else    break;
    }
    for(i=0; i<str_len; i++,k++)
    {
        if(_str[k] == '\0' || _str[k] == '\n')
        {
            _str[i] = '\0';
            break;
        }else
        {
            _str[i] = _str[k];
        }
    }
    return 0;
}
/**********************************************************
 * 函 数 名：   pub_base_DelAfterSpace
 * 函数功能：   去掉字符串后面的空格和换行符号和tab键
 * 作者/时间：
 *
 * 参数：
 *     输入: 
 *		char *str 	源字符串.
 *     输出: 
 *		char *str 	去掉前后空格后的字符串.
 *     返回: 0   成功
 *
 * 修改历史：
 *
***********************************************************/
int pub_base_DelAfterSpace(char *_str)
{
    int     i, k = 0;
    int     str_len;

    str_len = strlen(_str);

    for(i = str_len-1; i>0; i-- )
    {
        if( _str[i] == ' ' || _str[i] == '\t' ||_str[i] == '\n') continue;
        else    break;
    }
    _str[i+1] = '\0';

    return 0;
}





/*********************************************************************
 * 函 数 名： pub_base_ltrim_zero
 * 函数功能： 去掉字符串左边的零
 * 作者/时间：jack 200408
 *
 * 参数：
 *     输入: str要去掉零的字符串
 *     输出: str去掉左边零后的字符串
 *     返回: 0   成功
 *           非0 失败
 *
 * 修改历史：
 *
********************************************************************/
int pub_base_ltrim_zero( char str[] )
{
        int i = 0;
		int nlen = 0;
        nlen = strlen(str);

        for(i = 0; i < nlen; i ++)
		{
        	if( str[i] != '0' )
			{
            	break;
			}
		}

        if(i < nlen)
        {
        	MEMCPY_DEBUG(str, str + i, (nlen - i));
            str[(nlen - i)] = 0;
        }
        else
		{
            str = NULL;
        }

        return(0);
}
char * zip_space( char * str )
{
	char *p, *q;

	for( p=str; p !='\0' && p < str + strlen(str); p++ )
	{
		if( isspace( *p ) )
		{
			for( q=p; q != '\0' && q < p + strlen(p); q++ )
			*q = *(q+1);
			p--;
		}
	}
	return ( str );
}

/*********************************************************************
 * 函 数 名： pub_base_full_space
 * 函数功能： 将字符串中的\0转换为空格
 * 作者/时间：jack 200408
 *
 * 参数：
 *     输入: str需要转换空格的字符串，size 字符串的长度
 *     输出: str填充后的字符串
 *     返回: 0
 *
 * 修改历史：
 *
********************************************************************/
int pub_base_full_space(char * str,int size)
{
	int i;
	for(i=0;i<size;i++)
	{
		if(str[i]=='\0')
		{
			str[i]=' ';
		}
	}
	return ( 0 );
}
/*********************************************************************
 * 函 数 名： pub_base_full_anti_zero
 * 函数功能： 将字符串中的空格转换为\0
 * 作者/时间：jack 200408
 *
 * 参数：
 *     输入: str需要转换\0的字符串，size 字符串的长度
 *     输出: str填充后的字符串
 *     返回: 0
 *
 * 修改历史：
 *
********************************************************************/
int pub_base_full_anti_zero(char * str,int size)
{
	int i;
	for(i=0;i<size;i++)
	{
		if(str[i]==' ')
		{
			str[i]='\0';
		}
	}
	return ( 0 );
}
/*********************************************************************
 * 函 数 名： pub_base_full_zero
 * 函数功能： 将字符串中的空格转换为0
 * 作者/时间：jack 200408
 *
 * 参数：
 *     输入: str需要转换0的字符串，size 字符串的长度
 *     输出: str填充后的字符串
 *     返回: 0
 *
 * 修改历史：
 *
********************************************************************/
int pub_base_full_zero(char * str,int size)
{
	int i;
	for(i=0;i<size;i++)
	{
		if(str[i]==' ')
		{
			str[i]='0';
		}
	}
	return ( 0 );
}



/*******************************************************************
 将Double型 转换成12位字符串  前补空格 只保留小数点后两位  不带小数点
  Added by ChenMing ************************************************/
int iDblToStr_12(double dValue, char * cAmount)
{
 char cTmpBuf[20];
 int i,j;
 MEMSET_DEBUG(cTmpBuf, 0, sizeof(cTmpBuf));
 sprintf(cTmpBuf, "%.2lf", dValue);
 
 j = 0;
 for(i=0; i<strlen(cTmpBuf); i++)
 {
           if(i>11) return -1;
           if(cTmpBuf[i]=='.')
           {
                  j = i;
                  break;
           }
 }

 for(i=0; i<10-j; i++)
 {
     cAmount[i] = '0';
 }
 MEMCPY_DEBUG(cAmount+i, cTmpBuf, j);
 MEMCPY_DEBUG(cAmount+i+j, cTmpBuf+j+1, 2);
 cAmount[12] = 0;
 return 0;
 
}

/*-- add begin reason:晋中没有 从遂宁copy过来 by wzs 20120214 --*/
/* 去除半个汉字 */
int Del_half_wrod(char *instr,char *outstr)
{
        int len=0;
        int i=0;
        int j=0;
        len=strlen(instr);
        if(len==0)
        {
                strcpy(outstr,instr);
                return(-1);
        }
        for(i=0,j=0;i<len;)
        {
                if(i < len-1)
                {
                        if(instr[i]&128)
                        {
                        /*
                                printf("%s,%d,lllllllll==i[%d]len[%d][%d]\n",__FILE__,__LINE__,i,len,instr[i]);
                        */
                                outstr[j]=instr[i];
                                outstr[j+1]=instr[i+1];
                                i=i+2;
                                j=j+2;
                        }else{
                        /*
                                printf("%s,%d,mmmmmm==i[%d]len[%d][%d]\n",__FILE__,__LINE__,i,len,instr[i]);
                        */
                                outstr[j]=instr[i];
                                j++;
                                i++;
                        }
                }else{
                        /*
                        printf("%s,%d,WANGYONGWEI==i[%d]len[%d][%d]\n",__FILE__,__LINE__,i,len,instr[i]);
                        */
                        if(instr[i]&128 || instr[i]==63)
                        {
                                /* printf("%s,%d,存在半个汉字的情况!==i[%d]len[%d]\n",__FILE__,__LINE__,i,len);*/
                                i++;
                        }else{
                                outstr[j]=instr[i];
                                i++;
                                j++;
                        }
                }
        }
        outstr[j]='\0';
        /* printf("%s,%d,in[%s]out[%s]\n",__FILE__,__LINE__,instr,outstr);*/
        return(0);
}
/*-- add end by wzs 20120214 --*/

