#include "find_debug.h" 
/*************************************************
* �� �� ��:	pub_base_str
* ����������    1������ģ��pub_base_strncpyr�ַ���������
*               2������ģ��pub_base_ltrimȥ���ַ�����ߵĿո�
*               3������ģ��pub_base_rtrimȥ���ַ����ұߵĿո�
*               4������ģ��pub_base_strn_cpy�ַ�����ȡ��       
*               5������ģ��pub_base_DelSpaceȥ�ַ���β���ո�.
*               6������ģ��pub_base_StrCmp�ַ����Ƚ�.
*               7������ģ��pub_base_strpackȥ���ַ����ո�.
*               8������ģ��pub_base_strpack_allȥ���ַ��������пո�.
*               9��pub_base_ltrim_zeroȥ���ַ�����ߵ���
*               10,pub_base_full_space ��ָ�������ڵ�\0ת��Ϊ�ո�
*               11,pub_base_full_anti_zero��ָ�������ڵĿո�ת��Ϊ\0
*               12,pub_base_full_zero��ָ�������ڵĿո�ת��Ϊ0
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
 * �� �� ����	pub_base_strncpyr
 * �������ܣ�	�ַ�������
 * ����/ʱ�䣺	
 * 
 * ������
 *     ����: source	��Դ
 *           len	������
 *     ���: target;
 *     ����: 0   �ɹ�
 *           ��0 ʧ��
 * �޸���ʷ��
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
 * �� �� ����   pub_base_ltrim
 * �������ܣ�   ȥ���ַ�����ߵĿո�
 * ����/ʱ�䣺
 *
 * ������
 *     ����: strҪȥ���ո���ַ���
 *     ���: strȥ���ո����ַ���
 *     ����: 0   �ɹ�
 *           ��0 ʧ��
 *
 * �޸���ʷ��
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
 * �� �� ����   pub_base_rtrim
 * �������ܣ�   ȥ���ַ����ұߵĿո�
 * ����/ʱ�䣺
 *
 * ������
 *     ����: strҪȥ���ո���ַ���
 *     ���: strȥ���ո����ַ���
 *     ����: 0   �ɹ�
 *           ��0 ʧ��
 *
 * �޸���ʷ��
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
 * �� �� ����   pub_base_strn_cpyc
 * �������ܣ�   �ַ�����ȡ2.�������ʼλ�ð�����ǰλ�ý�ȡlen���ַ�����
 *              �����ʼλ���ǰ�����ּ���ǰ��һ���ַ��������ȡlen��
 *              �ַ�����ǰ��һ���ַ�.
 * ����/ʱ�䣺
 *
 * ������
 *     ����: msg             �����ַ�����
             qs              ��ȡ����ʼλ�á�(qs > =1 )
             len             ��ȡ�ĳ��ȡ�
 *     ���: rmsg            �����ַ���.
 *     ����: char *rtu_msg	 �ַ�������ȡ�����ʼλ��
 *
 * �޸���ʷ��
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
 * �� �� ����   pub_base_strn_cpy
 * �������ܣ�   �ַ�����ȡ.�������ʼλ�ð�����ǰλ�ý�ȡlen���ַ�����
 *              �����ʼλ���ǰ�����ּ���ǰ��һ���ַ��������ȡlen��
 *              �ַ�����ǰ��һ���ַ�.
 * ����/ʱ�䣺
 *
 * ������
 *     ����: msg             �����ַ�����
             qs              ��ȡ����ʼλ�á�(qs > =1 )
             len             ��ȡ�ĳ��ȡ�
 *     ���: rmsg            �����ַ���.
 *     ����: 0   û�иı�ԭ����
 *
 * �޸���ʷ��
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
 * �� �� ����   pub_base_DelSpace
 * �������ܣ�   ȥ�ַ���β���ո�.��
 * ����/ʱ�䣺
 *
 * ������
 *     ����: s �����ַ���.
 *     ���: n ��ȡ�ĳ���.
 *     ����: 0   �ɹ�
 *           ��0 ʧ��
 *
 * �޸���ʷ��
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
 * �� �� ����   pub_base_StrCmp
 * �������ܣ�   �Ƚ������ַ���.��
 * ����/ʱ�䣺
 *
 * ������
 *     ����: str1 �ַ����
 *           str2 �ַ���.
 *     ���: ��.
 *     ����: 0   �ɹ�
 *           ��0 ʧ��
 *
 * �޸���ʷ��
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
 * �� �� ����   pub_base_strpack_all
 * �������ܣ�   ȥ���ַ��������еĿո�.��
 * ����/ʱ�䣺
 *
 * ������
 *     ����: str ѹ��ǰ���ַ���.
 *     ���: str ѹ������ַ���.
 *     ����: 0   �ɹ�
 *           ��0 ʧ��
 *
 * �޸���ʷ��
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
 * �� �� ����   pub_base_strpack
 * �������ܣ�   ȥ���ַ����Ŀո�.��
 * ����/ʱ�䣺
 *
 * ������
 *     ����: str ѹ��ǰ���ַ���.
 *     ���: str ѹ������ַ���.
 *     ����: 0   �ɹ�
 *           ��0 ʧ��
 *
 * �޸���ʷ��
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
 * �� �� ����   pub_base_GetFirstSubStr
 * �������ܣ�   ���ݶ���","��ȡ��һ���Ӵ�.��
 * ����/ʱ�䣺
 *
 * ������
 *     ����: 
 *		s_str 	Դ�ַ���.
 *     ���: 
 *		sub_str ��һ�����ַ���.
 *     ����: 0   �ɹ�  -1 ʧ��
 *
 * �޸���ʷ��
 *
********************************************************************/
int pub_base_GetFirstSubStr(char  *s_str, char *sub_str)
{
    int     i, k = 0;
    int     str_len;

    /***  ȥ���ַ���ǰ��ո�  ***/
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
 * �� �� ����   pub_base_DelSpaces
 * �������ܣ�   ȥ���ַ���ǰ��Ŀո�ͻس����з���
 * ����/ʱ�䣺
 *
 * ������
 *     ����: 
 *		char *str 	Դ�ַ���.
 *     ���: 
 *		char *str 	ȥ��ǰ��ո����ַ���.
 *     ����: 0   �ɹ�
 *
 * �޸���ʷ��
 *
***********************************************************/
int pub_base_DelSpaces( char *str )
{
	pub_base_DelForeSpace( str );
	pub_base_DelAfterSpace( str );
	return 0;
}
/**********************************************************
 * �� �� ����   pub_base_DelForeSpace
 * �������ܣ�   ȥ���ַ���ǰ��Ŀո��tab����
 * ����/ʱ�䣺
 *
 * ������
 *     ����: 
 *		char *str 	Դ�ַ���.
 *     ���: 
 *		char *str 	ȥ��ǰ��ո����ַ���.
 *     ����: 0   �ɹ�
 *
 * �޸���ʷ��
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
 * �� �� ����   pub_base_DelAfterSpace
 * �������ܣ�   ȥ���ַ�������Ŀո�ͻ��з��ź�tab������
 * ����/ʱ�䣺
 *
 * ������
 *     ����: 
 *		char *str 	Դ�ַ���.
 *     ���: 
 *		char *str 	ȥ��ǰ��ո����ַ���.
 *     ����: 0   �ɹ�
 *
 * �޸���ʷ��
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
 * �� �� ���� pub_base_ltrim_zero
 * �������ܣ� ȥ���ַ�����ߵ���
 * ����/ʱ�䣺jack 200408
 *
 * ������
 *     ����: strҪȥ������ַ���
 *     ���: strȥ����������ַ���
 *     ����: 0   �ɹ�
 *           ��0 ʧ��
 *
 * �޸���ʷ��
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
 * �� �� ���� pub_base_full_space
 * �������ܣ� ���ַ����е�\0ת��Ϊ�ո�
 * ����/ʱ�䣺jack 200408
 *
 * ������
 *     ����: str��Ҫת���ո���ַ�����size �ַ����ĳ���
 *     ���: str������ַ���
 *     ����: 0
 *
 * �޸���ʷ��
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
 * �� �� ���� pub_base_full_anti_zero
 * �������ܣ� ���ַ����еĿո�ת��Ϊ\0
 * ����/ʱ�䣺jack 200408
 *
 * ������
 *     ����: str��Ҫת��\0���ַ�����size �ַ����ĳ���
 *     ���: str������ַ���
 *     ����: 0
 *
 * �޸���ʷ��
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
 * �� �� ���� pub_base_full_zero
 * �������ܣ� ���ַ����еĿո�ת��Ϊ0
 * ����/ʱ�䣺jack 200408
 *
 * ������
 *     ����: str��Ҫת��0���ַ�����size �ַ����ĳ���
 *     ���: str������ַ���
 *     ����: 0
 *
 * �޸���ʷ��
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
 ��Double�� ת����12λ�ַ���  ǰ���ո� ֻ����С�������λ  ����С����
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

/*-- add begin reason:����û�� ������copy���� by wzs 20120214 --*/
/* ȥ��������� */
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
                                /* printf("%s,%d,���ڰ�����ֵ����!==i[%d]len[%d]\n",__FILE__,__LINE__,i,len);*/
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

