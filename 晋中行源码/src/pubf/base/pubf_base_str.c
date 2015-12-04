#include "find_debug.h" 
/*************************************************
* ÎÄ ¼þ Ãû:	pub_base_str
* ¹¦ÄÜÃèÊö£º    1¡¢°üÀ¨Ä£¿épub_base_strncpyr×Ö·û´®¿½±´£»
*               2¡¢°üÀ¨Ä£¿épub_base_ltrimÈ¥µô×Ö·û´®×ó±ßµÄ¿Õ¸ñ
*               3¡¢°üÀ¨Ä£¿épub_base_rtrimÈ¥µô×Ö·û´®ÓÒ±ßµÄ¿Õ¸ñ
*               4¡¢°üÀ¨Ä£¿épub_base_strn_cpy×Ö·û´®½ØÈ¡¡£       
*               5¡¢°üÀ¨Ä£¿épub_base_DelSpaceÈ¥×Ö·û´®Î²²¿¿Õ¸ñ.
*               6¡¢°üÀ¨Ä£¿épub_base_StrCmp×Ö·û´®±È½Ï.
*               7¡¢°üÀ¨Ä£¿épub_base_strpackÈ¥µô×Ö·û´®¿Õ¸ñ.
*               8¡¢°üÀ¨Ä£¿épub_base_strpack_allÈ¥µô×Ö·û´®ÖÐËùÓÐ¿Õ¸ñ.
*               9¡¢pub_base_ltrim_zeroÈ¥µô×Ö·û´®×ó±ßµÄÁã
*               10,pub_base_full_space ½«Ö¸¶¨³¤¶ÈÄÚµÄ\0×ª»»Îª¿Õ¸ñ
*               11,pub_base_full_anti_zero½«Ö¸¶¨³¤¶ÈÄÚµÄ¿Õ¸ñ×ª»»Îª\0
*               12,pub_base_full_zero½«Ö¸¶¨³¤¶ÈÄÚµÄ¿Õ¸ñ×ª»»Îª0
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
 * º¯ Êý Ãû£º	pub_base_strncpyr
 * º¯Êý¹¦ÄÜ£º	×Ö·û´®¿½±´
 * ×÷Õß/Ê±¼ä£º	
 * 
 * ²ÎÊý£º
 *     ÊäÈë: source	£¬Ô´
 *           len	£¬³¤¶È
 *     Êä³ö: target;
 *     ·µ»Ø: 0   ³É¹¦
 *           ·Ç0 Ê§°Ü
 * ÐÞ¸ÄÀúÊ·£º
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
 * º¯ Êý Ãû£º   pub_base_ltrim
 * º¯Êý¹¦ÄÜ£º   È¥µô×Ö·û´®×ó±ßµÄ¿Õ¸ñ
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊý£º
 *     ÊäÈë: strÒªÈ¥µô¿Õ¸ñµÄ×Ö·û´®
 *     Êä³ö: strÈ¥µô¿Õ¸ñºóµÄ×Ö·û´®
 *     ·µ»Ø: 0   ³É¹¦
 *           ·Ç0 Ê§°Ü
 *
 * ÐÞ¸ÄÀúÊ·£º
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
 * º¯ Êý Ãû£º   pub_base_rtrim
 * º¯Êý¹¦ÄÜ£º   È¥µô×Ö·û´®ÓÒ±ßµÄ¿Õ¸ñ
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊý£º
 *     ÊäÈë: strÒªÈ¥µô¿Õ¸ñµÄ×Ö·û´®
 *     Êä³ö: strÈ¥µô¿Õ¸ñºóµÄ×Ö·û´®
 *     ·µ»Ø: 0   ³É¹¦
 *           ·Ç0 Ê§°Ü
 *
 * ÐÞ¸ÄÀúÊ·£º
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
 * º¯ Êý Ãû£º   pub_base_strn_cpyc
 * º¯Êý¹¦ÄÜ£º   ×Ö·û´®½ØÈ¡2.ÊäÈëµÄÆðÊ¼Î»ÖÃ°üÀ¨µ±Ç°Î»ÖÃ½ØÈ¡len³¤×Ö·û´«¡£
 *              Èç¹ûÆðÊ¼Î»ÖÃÊÇ°ë¸öºº×Ö¼´ÏòÇ°´ÜÒ»¸ö×Ö·û¡£Èç¹û½ØÈ¡len³¤
 *              ×Ö·ûºó¼´ÏòÇ°´ÜÒ»¸ö×Ö·û.
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊý£º
 *     ÊäÈë: msg             ÊäÈë×Ö·û´®¡£
             qs              ½ØÈ¡µÄÆðÊ¼Î»ÖÃ¡£(qs > =1 )
             len             ½ØÈ¡µÄ³¤¶È¡£
 *     Êä³ö: rmsg            ·µ»Ø×Ö·û´®.
 *     ·µ»Ø: char *rtu_msg	 ×Ö·û´®±»½ØÈ¡ºóµÄÆðÊ¼Î»ÖÃ
 *
 * ÐÞ¸ÄÀúÊ·£º
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
 * º¯ Êý Ãû£º   pub_base_strn_cpy
 * º¯Êý¹¦ÄÜ£º   ×Ö·û´®½ØÈ¡.ÊäÈëµÄÆðÊ¼Î»ÖÃ°üÀ¨µ±Ç°Î»ÖÃ½ØÈ¡len³¤×Ö·û´«¡£
 *              Èç¹ûÆðÊ¼Î»ÖÃÊÇ°ë¸öºº×Ö¼´ÏòÇ°´ÜÒ»¸ö×Ö·û¡£Èç¹û½ØÈ¡len³¤
 *              ×Ö·ûºó¼´ÏòÇ°´ÜÒ»¸ö×Ö·û.
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊý£º
 *     ÊäÈë: msg             ÊäÈë×Ö·û´®¡£
             qs              ½ØÈ¡µÄÆðÊ¼Î»ÖÃ¡£(qs > =1 )
             len             ½ØÈ¡µÄ³¤¶È¡£
 *     Êä³ö: rmsg            ·µ»Ø×Ö·û´®.
 *     ·µ»Ø: 0   Ã»ÓÐ¸Ä±äÔ­ÄÚÈÝ
 *
 * ÐÞ¸ÄÀúÊ·£º
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
 * º¯ Êý Ãû£º   pub_base_DelSpace
 * º¯Êý¹¦ÄÜ£º   È¥×Ö·û´®Î²²¿¿Õ¸ñ.«¡
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊý£º
 *     ÊäÈë: s ÊäÈë×Ö·û´®.
 *     Êä³ö: n ½ØÈ¡µÄ³¤¶È.
 *     ·µ»Ø: 0   ³É¹¦
 *           ·Ç0 Ê§°Ü
 *
 * ÐÞ¸ÄÀúÊ·£º
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
 * º¯ Êý Ãû£º   pub_base_StrCmp
 * º¯Êý¹¦ÄÜ£º   ±È½ÏÁ½¸ö×Ö·û´®.«¡
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊý£º
 *     ÊäÈë: str1 ×Ö·û´®´
 *           str2 ×Ö·û´®.
 *     Êä³ö: ÎÞ.
 *     ·µ»Ø: 0   ³É¹¦
 *           ·Ç0 Ê§°Ü
 *
 * ÐÞ¸ÄÀúÊ·£º
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
 * º¯ Êý Ãû£º   pub_base_strpack_all
 * º¯Êý¹¦ÄÜ£º   È¥µô×Ö·û´®ÖÐËùÓÐµÄ¿Õ¸ñ.«£
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊý£º
 *     ÊäÈë: str Ñ¹ËõÇ°µÄ×Ö·û´®.
 *     Êä³ö: str Ñ¹ËõºóµÄ×Ö·û´®.
 *     ·µ»Ø: 0   ³É¹¦
 *           ·Ç0 Ê§°Ü
 *
 * ÐÞ¸ÄÀúÊ·£º
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
 * º¯ Êý Ãû£º   pub_base_strpack
 * º¯Êý¹¦ÄÜ£º   È¥µô×Ö·û´®µÄ¿Õ¸ñ.«£
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊý£º
 *     ÊäÈë: str Ñ¹ËõÇ°µÄ×Ö·û´®.
 *     Êä³ö: str Ñ¹ËõºóµÄ×Ö·û´®.
 *     ·µ»Ø: 0   ³É¹¦
 *           ·Ç0 Ê§°Ü
 *
 * ÐÞ¸ÄÀúÊ·£º
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
 * º¯ Êý Ãû£º   pub_base_GetFirstSubStr
 * º¯Êý¹¦ÄÜ£º   ÒÀ¾Ý¶ººÅ","½ØÈ¡µÚÒ»¸ö×Ó´®.«£
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊý£º
 *     ÊäÈë: 
 *		s_str 	Ô´×Ö·û´®.
 *     Êä³ö: 
 *		sub_str µÚÒ»¸ö×Ó×Ö·û´®.
 *     ·µ»Ø: 0   ³É¹¦  -1 Ê§°Ü
 *
 * ÐÞ¸ÄÀúÊ·£º
 *
********************************************************************/
int pub_base_GetFirstSubStr(char  *s_str, char *sub_str)
{
    int     i, k = 0;
    int     str_len;

    /***  È¥µô×Ö·û´®Ç°ºó¿Õ¸ñ  ***/
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
 * º¯ Êý Ãû£º   pub_base_DelSpaces
 * º¯Êý¹¦ÄÜ£º   È¥µô×Ö·û´®Ç°ºóµÄ¿Õ¸ñºÍ»Ø³µ»»ÐÐ·û«£
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊý£º
 *     ÊäÈë: 
 *		char *str 	Ô´×Ö·û´®.
 *     Êä³ö: 
 *		char *str 	È¥µôÇ°ºó¿Õ¸ñºóµÄ×Ö·û´®.
 *     ·µ»Ø: 0   ³É¹¦
 *
 * ÐÞ¸ÄÀúÊ·£º
 *
***********************************************************/
int pub_base_DelSpaces( char *str )
{
	pub_base_DelForeSpace( str );
	pub_base_DelAfterSpace( str );
	return 0;
}
/**********************************************************
 * º¯ Êý Ãû£º   pub_base_DelForeSpace
 * º¯Êý¹¦ÄÜ£º   È¥µô×Ö·û´®Ç°ÃæµÄ¿Õ¸ñºÍtab¼ü«£
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊý£º
 *     ÊäÈë: 
 *		char *str 	Ô´×Ö·û´®.
 *     Êä³ö: 
 *		char *str 	È¥µôÇ°ºó¿Õ¸ñºóµÄ×Ö·û´®.
 *     ·µ»Ø: 0   ³É¹¦
 *
 * ÐÞ¸ÄÀúÊ·£º
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
 * º¯ Êý Ãû£º   pub_base_DelAfterSpace
 * º¯Êý¹¦ÄÜ£º   È¥µô×Ö·û´®ºóÃæµÄ¿Õ¸ñºÍ»»ÐÐ·ûºÅºÍtab¼ü«£«£
 * ×÷Õß/Ê±¼ä£º
 *
 * ²ÎÊý£º
 *     ÊäÈë: 
 *		char *str 	Ô´×Ö·û´®.
 *     Êä³ö: 
 *		char *str 	È¥µôÇ°ºó¿Õ¸ñºóµÄ×Ö·û´®.
 *     ·µ»Ø: 0   ³É¹¦
 *
 * ÐÞ¸ÄÀúÊ·£º
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
 * º¯ Êý Ãû£º pub_base_ltrim_zero
 * º¯Êý¹¦ÄÜ£º È¥µô×Ö·û´®×ó±ßµÄÁã
 * ×÷Õß/Ê±¼ä£ºjack 200408
 *
 * ²ÎÊý£º
 *     ÊäÈë: strÒªÈ¥µôÁãµÄ×Ö·û´®
 *     Êä³ö: strÈ¥µô×ó±ßÁãºóµÄ×Ö·û´®
 *     ·µ»Ø: 0   ³É¹¦
 *           ·Ç0 Ê§°Ü
 *
 * ÐÞ¸ÄÀúÊ·£º
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
 * º¯ Êý Ãû£º pub_base_full_space
 * º¯Êý¹¦ÄÜ£º ½«×Ö·û´®ÖÐµÄ\0×ª»»Îª¿Õ¸ñ
 * ×÷Õß/Ê±¼ä£ºjack 200408
 *
 * ²ÎÊý£º
 *     ÊäÈë: strÐèÒª×ª»»¿Õ¸ñµÄ×Ö·û´®£¬size ×Ö·û´®µÄ³¤¶È
 *     Êä³ö: strÌî³äºóµÄ×Ö·û´®
 *     ·µ»Ø: 0
 *
 * ÐÞ¸ÄÀúÊ·£º
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
 * º¯ Êý Ãû£º pub_base_full_anti_zero
 * º¯Êý¹¦ÄÜ£º ½«×Ö·û´®ÖÐµÄ¿Õ¸ñ×ª»»Îª\0
 * ×÷Õß/Ê±¼ä£ºjack 200408
 *
 * ²ÎÊý£º
 *     ÊäÈë: strÐèÒª×ª»»\0µÄ×Ö·û´®£¬size ×Ö·û´®µÄ³¤¶È
 *     Êä³ö: strÌî³äºóµÄ×Ö·û´®
 *     ·µ»Ø: 0
 *
 * ÐÞ¸ÄÀúÊ·£º
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
 * º¯ Êý Ãû£º pub_base_full_zero
 * º¯Êý¹¦ÄÜ£º ½«×Ö·û´®ÖÐµÄ¿Õ¸ñ×ª»»Îª0
 * ×÷Õß/Ê±¼ä£ºjack 200408
 *
 * ²ÎÊý£º
 *     ÊäÈë: strÐèÒª×ª»»0µÄ×Ö·û´®£¬size ×Ö·û´®µÄ³¤¶È
 *     Êä³ö: strÌî³äºóµÄ×Ö·û´®
 *     ·µ»Ø: 0
 *
 * ÐÞ¸ÄÀúÊ·£º
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
 ½«DoubleÐÍ ×ª»»³É12Î»×Ö·û´®  Ç°²¹¿Õ¸ñ Ö»±£ÁôÐ¡ÊýµãºóÁ½Î»  ²»´øÐ¡Êýµã
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

/*-- add begin reason:½úÖÐÃ»ÓÐ ´ÓËìÄþcopy¹ýÀ´ by wzs 20120214 --*/
/* È¥³ý°ë¸öºº×Ö */
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
                                /* printf("%s,%d,´æÔÚ°ë¸öºº×ÖµÄÇé¿ö!==i[%d]len[%d]\n",__FILE__,__LINE__,i,len);*/
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

