/***************************************************************
* �� �� ��:     pubf_rpt_readpfm.c
* ����������    �����ӡ�Ĺ��ú���
* ���ú���˵����
*  
*
* ��    ��:     jack
* ������ڣ�    2004��04��12��
*
* �޸ļ�¼��
* 1. ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <sys/stat.h>
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_branch_c.h"
#include "hv_uniontab_c.h"

/**********************************************************************
* ��������    pub_rpt_read_pfm_qd
* �������ܣ�  �������ʽ�ļ���������������ת���ɱ���,��ֱ������������ļ���
* ����/ʱ�䣺 2004��04��13��
*
* ������
*     ���룺        
*            char vpfm[21] ��ӡ�ñ���ĸ�ʽ�ļ���(��.tbl,����ĸ�ʽ�ļ�ȫ����Ϊ: �ļ���.tbl)
*            char gsm[3]   �Ӹ�ʽ "00" �������ĸ�ʽ�ļ�ת�� 
*                          ����������Ļ�,�⽫��ʽ�ļ���ǰ��λ��gsm��ͬ����ת��
*            int  rpt_code ��ӡ����(Ϊ��ͬ��ʽ�ı���,��ͬһ��get_ratioʱ��)
*            int  Get_ratio( int bli,int bll,char str[100],int rpt_code ) ����  
*     ���: 
*            FILE *fp  ���������ļ�ָ��
*            int *pline �ļ�ָ��ָ������(�����������)
*            int *opncnt ͬһ�������ļ���,����յ�����
* �� �� ֵ: 0 �ɹ�
*          !0 ���ɹ�
*
* �޸���ʷ��
*
********************************************************************/

int pub_rpt_read_pfm_qd( FILE *fp,int *pline,
	char vpfm[21],int rpt_code,char gsm[3],int *opncnt, 
	int Get_ratio( int bli,int bll,char str[100],int rpt_code ) )
{
	FILE *fmp;
	char fil[100];
	char onechar[2];
	char info[800],str[100];
	int i=0;
	int flag=0,bli=0,bll=0;
	int blflag=0;
	char blstr[200];
	int smflag=0;
	int recflag=0;
	int kzfflag=1;
	int line;
	
	char xier_ch[2];
	memset(xier_ch,0x00,2);
	

	line=*pline;

	*opncnt=0;
	
	sprintf( fil,"%s/%s.tbl",getenv("PFMDIR"),vpfm );
	fmp=fopen( fil,"r" );
	if( fmp==NULL )
	{
		sprintf(acErrMsg,"open file [%s] error",fil );
		vtcp_log("%s,%d here goto errexit",__FILE__,__LINE__);
		WRITEMSG
		goto ErrExit;
	}
	
	memset( info,0,800 ); i=0;
	memset( blstr,0,200 );
	while(1)
	{
		xier_ch[0] = onechar[0];
		memset( onechar,0,2 );
		fread(onechar,sizeof(char),1,fmp);
		
		if( feof(fmp) )
		{
			/** fprintf( fp,"%s\n",info );  */
			
			memset( info,0,800 ); i=0;
			flag=0;
			break;
		}
		else if( i==0 && ( onechar[0]==':' || onechar[0]=='?') )
		{
			
			smflag=1;
			info[i]=onechar[0]; i++;
		}
		else if( i<=1 && gsm[0]!='0' )
		{
			
			info[i]=onechar[0]; i++;
		}
		else if( gsm[0]!='0' && (info[0]!=gsm[0] || info[1]!=gsm[1]) 
			&& onechar[0]!='\0' && onechar[0]!='\n' && onechar[0]!=' ' )
		{
			
			info[i]=onechar[0]; i++;
		}
		else if( strstr(blstr,onechar) && smflag==0 && kzfflag==1
			&& onechar[0]!='\0' && onechar[0]!='\n' && onechar[0]!=' ' )
		{
			
			if( flag==1 )
			{
				bll++;
			}
			else
			{
				bll++;
				flag=1;
			}
			bli=onechar[0];
		}
		else 
		{
			
			if( flag==1 )
			{
				memset(str,0,100);
				
				if( Get_ratio( bli,bll,str,rpt_code ) ) 
				{
					vtcp_log("%s,%d here goto errexit ",__FILE__,__LINE__);
					goto ErrExit;
				}
				
				vtcp_log("[%s][%d]��ȡ��������!str==[%s]bli===[%c]\n",__FILE__,__LINE__,str,bli);
				strcpy( info+i,str );
				i=i+bll;
				flag=0; bll=0;
				
			}
			
			if( onechar[0]=='\n' )
			{
				
				if( smflag==1 )
				{
					
					if( info[0]==':' )
					{
						memset( blstr,0,200 );
						info[200]='\0';
						strcpy( blstr,info+1 );
					}
					else if( info[0]=='?' )
					{
						*opncnt=atoi( info+1 );
					}
				}
				else
				{
					
					if( gsm[0]!='0' && !strncmp(info,gsm,2) )
					{
						
					        fprintf( fp,"%s\n",info+2 );
						
/*
sprintf(acErrMsg,"[%s] \n",info+2);
WRITEMSG
*/
						line++;
					}
					else if( gsm[0]!='0' )
					{
						;
					}
					else
					{
						fprintf( fp,"%s\n",info );
/*
sprintf(acErrMsg,"[%s] \n",info);
WRITEMSG
*/
						line++;
					}
				}
				
				memset( info,0,800 ); i=0;
				smflag=0;
				
			}
			else
			{
				
				if( onechar[0]=='@' ) kzfflag=kzfflag*(-1);
				info[i]=onechar[0]; i++;
				
				if( i>=800 )
				{
					vtcp_log("[%s][%d]",__FILE__,__LINE__);
					goto ErrExit;
				}
			}
		}
	}
	
	fclose(fmp);

	*pline=line;
	

GoodExit:
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	return 0;
ErrExit:
	vtcp_log("[%s][%d] ERREXIT\n",__FILE__,__LINE__);
	return 1;
}


/**********************************************************************
* ��������    pub_rpt_readstr_pfm_qd
* �������ܣ�  �������ʽ�ļ���������������ת���ɱ���,��������ַ���
* ����/ʱ�䣺 2004��04��13��
*
* ������
*     ���룺        
*            char vpfm[21] ��ӡ�ñ���ĸ�ʽ�ļ���(��.tbl,����ĸ�ʽ�ļ�ȫ����Ϊ: �ļ���.tbl)
*            char gsm[3]   �Ӹ�ʽ "00" �������ĸ�ʽ�ļ�ת�� 
*                          ����������Ļ�,�⽫��ʽ�ļ���ǰ��λ��gsm��ͬ����ת��
*            int  rpt_code ��ӡ����(Ϊ��ͬ��ʽ�ı���,��ͬһ��get_ratioʱ��)
*            int  Get_ratio( int bli,int bll,char str[100],int rpt_code ) ����  
*     ���: 
*            int *opncnt ͬһ�������ļ���,����յ�����
*            char *t ��������ʽ��ת���ɱ����ʽ�ļ���
* �� �� ֵ: 0 �ɹ�
*          !0 ���ɹ�
*
* �޸���ʷ��
*
********************************************************************/

int pub_rpt_readstr_pfm_qd( char vpfm[21],int rpt_code,char gsm[3],int *opncnt,
		char *t, int Get_ratio( int bli,int bll,char str[100],int rpt_code ))
{
	FILE *fmp;
	char fil[100];
	char onechar[2];
	char info[800],str[100];
	int i=0;
	int flag=0,bli=0,bll=0;
	int blflag=0;
	char blstr[200];
	int smflag=0;
	int recflag=0;
	int kzfflag=1;

	*opncnt=0;

	strcpy(t,"" );
	sprintf( fil,"%s/%s.tbl",getenv("PFMDIR"),vpfm );
	fmp=fopen( fil,"r" );
	if( fmp==NULL )
	{
		printf( "open file [%s] error",fil );
		goto ErrExit;
	}
	
TRACE
	memset( info,0,800 ); i=0;
	memset( blstr,0,200 );
	while(1)
	{
TRACE
		memset( onechar,0,2 );
		fread(onechar,sizeof(char),1,fmp);
		if( feof(fmp) )
		{
		        /* ��Ļ��ʽ���� */
			memset( info,0,800 ); i=0;
			flag=0;
			break;
		}
		else if( i==0 && ( onechar[0]==':' || onechar[0]=='?') )
		{
			smflag=1;
			info[i]=onechar[0]; i++;
		}
		else if( i<=1 && gsm[0]!='0' )
		{
			info[i]=onechar[0]; i++;
		}
		else if( gsm[0]!='0' && (info[0]!=gsm[0] || info[1]!=gsm[1]) 
			&& onechar[0]!='\0' && onechar[0]!='\n' && onechar[0]!=' ' )
		{
			info[i]=onechar[0]; i++;
		}
		else if( strstr(blstr,onechar) && smflag==0 && kzfflag==1
			&& onechar[0]!='\0' && onechar[0]!='\n' && onechar[0]!=' ' )
		{
TRACE
			if( flag==1 )
			{
				bll++;
			}
			else
			{
				bll++;
				flag=1;
			}
			bli=onechar[0];
		}
		else 
		{
TRACE
			if( flag==1 )
			{
				memset(str,0,100);
				if( Get_ratio( bli,bll,str,rpt_code ) ) goto ErrExit;
				strcpy( info+i,str );
				i=i+bll;
				flag=0; bll=0;
			}
			if( onechar[0]=='\n' )
			{
TRACE
				if( smflag==1 )
				{
TRACE
					if( info[0]==':' )
					{
						memset( blstr,0,200 );
						info[200]='\0';
						strcpy( blstr,info+1 );
					}
					else if( info[0]=='?' )
					{
						*opncnt=atoi( info+1 );
					}
				}
				else
				{
TRACE
					if( gsm[0]!='0' && !strncmp(info,gsm,2) )
					{
						strcat( t,info+2 );
					}
					else if( gsm[0]!='0' )
					{
						;
					}
					else
					{
						strcat( t,info );
					}
				}
				memset( info,0,800 ); i=0;
				smflag=0;
			}
			else
			{
TRACE
				if( onechar[0]=='@' ) kzfflag=kzfflag*(-1);
				info[i]=onechar[0]; i++;
				if( i>=800 )
				{
					printf("====[%d]\n",i);
					goto ErrExit;
				}
			}
		}
	}

	fclose(fmp);
TRACE

GoodExit:
	return 0;
ErrExit:
	return 1;
}


/**********************************************************************
* ��������    pub_rpt_openfile
* �������ܣ�  �򿪱����ʽ�ļ�(׷�ӷ�ʽ)
* ����/ʱ�䣺 2004��04��13��
*
* ������
*     ���룺        
*            char tmpjgbm[10]  ��������
*            char bbm[21]      ��������
*     ���: 
*            FILE *fp          �򿪱����ļ����ļ�ָ�� 
* �� �� ֵ: 0 �ɹ�
*          !0 ���ɹ�
*
* �޸���ʷ��
*
********************************************************************/
int   pub_rpt_openfile(fp,tmpjgbm,bbm)
FILE **fp;
char   tmpjgbm[10];
char bbm[21];
{
	char	fullname[100] ;

	memset(fullname,0x00,sizeof(fullname) );
	pub_base_rtrim(tmpjgbm);
	pub_base_ltrim(tmpjgbm);
	ldchar( bbm,strlen(bbm),bbm );
	sprintf( fullname,"%s/report/%s/%s.txt",getenv("HOME"),tmpjgbm,bbm);
	if( (*fp=fopen(fullname,"a+"))==NULL )
	{
		sprintf( fullname,"%s/report/%s",getenv("HOME"),tmpjgbm );
		mkdir( fullname, S_IRWXU|S_IRWXG|S_IRWXO);
		sprintf( fullname,"%s/report/%s/%s.txt",getenv("HOME"),tmpjgbm,bbm);
		if( (*fp=fopen( fullname,"a+"))==NULL  )
		{
			sprintf(acErrMsg,"open file failed,filename=%s\n",fullname);
			WRITEMSG
			return(1);
		}
    	/* vtcp_log("file %s created1 \n",fullname); */
	}
    	/*vtcp_log("file %s created2 \n",fullname); */
	return(0);
}

/**********************************************************************
* ��������    pub_rpt_rmfile
* �������ܣ�  ɾ����Ӧ��������ı����ʽ�ļ�
* ����/ʱ�䣺 2004��04��13��
*
* ������
*     ���룺        
*            char brno[10]  ��������
*            char ffil[21]  ��������
*            int  mode      mode 1 ���������0-9  2 ��������ͻ���
*     ���: 
* �� �� ֵ: 0 �ɹ�
*          !0 ���ɹ�
*
* �޸���ʷ��
*
********************************************************************/
int pub_rpt_rmfile( char brno[10],char ffil[21],int mode )
{
	char	fname[100] ;
	int ret=0;
	struct com_branch_c com_branch_c;
	
	memset(&com_branch_c, 0x0, sizeof(struct com_branch_c));
	
	ret = Com_branch_Dec_Sel(g_pub_tx.reply, "1=1");
	if (ret != 0)
	{
	    sprintf(acErrMsg,"DECLARE FOR SELECT COM_BRANCH ERROR [%d]",ret);
	    WRITEMSG
	    return ret;
	}
		
	while(1)
	{
		ret = Com_branch_Fet_Sel(&com_branch_c, g_pub_tx.reply);
		if (ret != 0 && ret != 100)
		{
		    sprintf(acErrMsg,"FETCH FOR SELECT COM_BRANCH ERROR [%d]",ret);
		    WRITEMSG
		    return ret;
		}
		else if (ret == 100)
		{
		     break;
		}

		memset(fname,0x00,sizeof(fname) );
		pub_base_strpack( com_branch_c.br_no );
		strcpy(com_branch_c.br_no,com_branch_c.br_no );

		if( strlen(brno) && strcmp(com_branch_c.br_no,brno) )
			continue;
		if( com_branch_c.br_type[0]<'0' || com_branch_c.br_type[0]>'9' )
		{
			if( mode==1 ) continue;
		}
		else
		{
			if( mode==2 ) continue;
		}

		sprintf( fname,"rm -f %s/report/%s/%s.txt",
			getenv("HOME"),com_branch_c.br_no,ffil);
vtcp_log( "rm -f %s/report/%s/%s.txt",getenv("HOME"),com_branch_c.br_no,ffil);

		ret = system( fname );
		if( ret )
		{
			sprintf(acErrMsg,"rm file error [%s] [%d]",fname,ret);
			WRITEMSG
			return ret;
		}
	}
	Com_branch_Clo_Sel();

	return 0;
}

/**********************************************************************
* ��������    pub_rpt_rmfile_all
* �������ܣ�  ɾ���ı����ʽ�ļ�(���л���)
* ����/ʱ�䣺 2004��04��13��
*
* ������
*     ���룺        
*            char brno[10]  ��������
*            char ffil[21]  ��������
*            int  mode      mode 1 ���������0-9  2 ��������ͻ���
*     ���: 
* �� �� ֵ: 0 �ɹ�
*          !0 ���ɹ�
*
* �޸���ʷ��
*
********************************************************************/
int pub_rpt_rmfile_all( char tblname[21] )
{
	char	fname[100] ;
	int ret=0;
	struct com_branch_c com_branch_c;
	
	memset(&com_branch_c, 0x0, sizeof(struct com_branch_c));

	strcpy(tblname,pub_base_strpack( tblname ));
		
	ret = Com_branch_Dec_Sel(g_pub_tx.reply,"1=1");
	if (ret != 0)
	{
		sprintf(acErrMsg,"DECLARE FOR SELECT COM_BRANCH ERROR [%d]",ret);
		WRITEMSG
		return ret;
	}
	
	while(1)
	{
		ret = Com_branch_Fet_Sel(&com_branch_c, g_pub_tx.reply);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg,"FETCH FOR SELECT COM_BRANCH ERROR [%d]",ret);
			WRITEMSG
			return ret;
		}
		else if (ret == 100)
		{
			break;
		}
		
		memset(fname,0x00,sizeof(fname) );
		strcpy(com_branch_c.br_no,pub_base_strpack( com_branch_c.br_no ));

		sprintf( fname,"rm -f %s/report/%s/%s.txt",getenv("HOME"),
				com_branch_c.br_no,tblname);
		ret=system( fname );
		if (ret != 0)
		{
    		sprintf(acErrMsg,"rm file error %s %d",fname,ret);
			WRITEMSG
			return ret;
		}
	}
	Com_branch_Clo_Sel();

	return 0;
}

int pub_rpt_empty( int opncnt,FILE *fp )
{
	int i=0;
	for( i=0;i<opncnt;i++ )
	{
		fprintf( fp,"\n" );
	}
	return 0;
}

char *pub_rpt_flttomon(str1,str2)
char *str1,*str2;
{ register i;
  char sour[30],dest[30],*p,stmp[30];
  int leng,j,l,negflag=0;

  if (str1[0]=='\0') { str2[0]='\0';	return str2; }

  for(i=0;i<strlen(str1)-4;i++) if(str1[i]!='0'&& str1[i]!=' ') break; 
  strcpy(stmp,str1+i); 

  if (stmp[0]=='-') { 
	negflag=1;
	strcpy(sour,stmp+1);
	}
   else strcpy(sour,stmp);

  p=strchr(sour,'.');
  if (!p) p=sour+strlen(sour);
  leng=p-sour; 
  if (!leng) { sprintf(dest,"0.%s",p+1); strcpy ( str2 , dest ); return str2; }
  i=leng%3;
  if (!i) i=3;
  strncpy(dest,sour,i);
  
  for (l=i,j=0;i<leng;i++,j++) { 
	if (j%3==0) dest[l++]=','; 
	dest[l++]=sour[i]; 
	}
  dest[l++]='.';
  if (leng==strlen(sour))
  {
	strcpy(dest+l,"00");
	strcpy ( str2 , dest );
	return str2;
  }
  strcpy(dest+l,p+1); 

  if (negflag) { str2[0]='-'; strcpy(str2+1,dest); }
	else strcpy(str2,dest);
  return str2;
}
/*  add by wjl 20140226 */
/**********************************************************************
* ��������    pub_rpt_append_openfile
* �������ܣ�  ��׷�ӷ�ʽ�򿪱����ʽ�ļ�
* ����/ʱ�䣺 LiuHuafeng 2008��04��04��
*
* ������
*     ���룺
*            char tmpjgbm[10]  ��������
*            char bbm[21]      ��������
*     ���:
*            FILE *fp          �򿪱����ļ����ļ�ָ��
* �� �� ֵ: 0 �ɹ�
*          !0 ���ɹ�
*
* �޸���ʷ��
*
********************************************************************/
int   pub_rpt_append_openfile(fp, tmpjgbm, bbm)
FILE **fp;
char   tmpjgbm[10];
char bbm[21];
{
    char	fullname[100] ;

    memset(fullname, 0x00, sizeof(fullname) );
    pub_base_rtrim(tmpjgbm);
    pub_base_ltrim(tmpjgbm);
    ldchar( bbm, strlen(bbm), bbm );
    sprintf( fullname, "%s/report/%s/%s.txt", getenv("HOME"), tmpjgbm, bbm);
    if( (*fp = fopen(fullname, "a+")) == NULL )
    {
        sprintf( fullname, "%s/report/%s", getenv("HOME"), tmpjgbm );
        mkdir( fullname, S_IRWXU | S_IRWXG | S_IRWXO);
        sprintf( fullname, "%s/report/%s/%s.txt", getenv("HOME"), tmpjgbm, bbm);
        if( (*fp = fopen( fullname, "a+")) == NULL  )
        {
            vtcp_log("open file failed,filename=%s\n", fullname);
            return(1);
        }
    }
    return(0);
}

/**********************************************************************
* ��������    pub_rpt_pcask
* �������ܣ�  ��Сд���ת���ɴ�д���
* ����/ʱ�䣺 2004��04��13��
*
* ������
*     ���룺        
*     ���: 
* �� �� ֵ: 0 �ɹ�
*          !0 ���ɹ�
*
* �޸���ʷ��
*
********************************************************************/
char *upnum[10] = { "��","Ҽ","��","��","��","��","½","��","��","��", };
char *unit[15]= { "��", "��", "��", "Ԫ", "ʰ", "��", "Ǫ", "��", 
				"ʰ", "��", "Ǫ", "��", "ʰ", "��", "Ǫ", };
short limit[] = {14,1,2,14,4,5,6,10,8,9,10,14,12,13,14};

char *pub_rpt_pcask(cask)
double cask;
{ 
	double he,fje; 
	short i,j,k,lim; 
	char *str,*str3,*str2,*wrstr; 
	int h; 
	char    *p,str1[36]; 
	
	/******yud add 20001222 *******/ 
	fje=cask; 
	if (fje<0)
	{ 
		cask=(-1) * cask; 
	} 
	/*****************************/

	str = (char*)malloc(30); 
	str2=(char*)malloc(3); 
	str3 =(char*)malloc(30); 
	wrstr = (char*)malloc(60); 
	sprintf(str,"%.2f",cask); 
	i = 0; 
	j = k = strlen(str); 
	for(i=0;i<j;i++) 
	{ 
		if (str[i]=='.') 
		{ 
			k = i; 
			break; 
		} 
	} 
	
	strncpy(str3,str,k); 
	str3[k]='\0'; 
	strncpy(str2,str+k+1,2); 
	str2[2]='\0'; 
	strcat(str3,str2); 
	strcpy(str,str3); 
	k=k+2;

	if(k>14) 
	{ 
		free(str); free(wrstr); 
		return((char *)0); 
	} 
	strcpy(wrstr,""); 
	j = 0; 
	lim = k; 
	for(i=k;i>0;i--) 
	{ 
		if (str[k-i]>'0') 
		{ 
			if (lim>i+1) 
			strcat(wrstr,upnum[0]); 
			strcat(wrstr,upnum[str[k-i]-'0']); 
			strcat(wrstr,unit[i]); 
			lim = i; 
		} 
		else 
		if(lim<=limit[i]) 
			strcat(wrstr,unit[i]); 
	}

	/*********GL:����ӵ�����ǰ��������*********/ 
	sprintf(str1,"%lf",cask); 
	p=strchr(str1,'.'); 
	p+=2; 
	if( *p=='0' ) 
		strcat(wrstr,unit[0]); 
	/*************************/ 
	free(str); 
	/*********yud:���С���㣬ȥ��Բ��******/ 
	if (cask<1.00 && cask>0.10)
	{ 
		strncpy(str1,wrstr+2,8); 
		strcpy(wrstr,str1); 
	} 
	
	if (cask<0.10)
	{ 
		strncpy(str1,wrstr+4,6); 
		strcpy(wrstr,str1); 
	} 
	
	/*** 
	if (fje<0)
	{ 
		sprintf(str1,"��%s",wrstr); 
		strcpy(wrstr,str1); 
	}
	***/ 
	if (fje==0.00) 
	{ 
		strcpy(str1,"��Բ��"); 
		strcpy(wrstr,str1); 
	} 
	/***************************************/ 
	return(wrstr); 
}

/****add by liuyue  20061018 ���ڴ�С��ı����ӡ******/

/***��ȡ���Ľ�������***/
char *cGetHvCmnoDes(char *cmtno,char *desc)
{
	char cCmtno[4];
	char cmtno_des[17];
	int  iCmtno;
	
	memset(cmtno_des,0x00,sizeof(cmtno_des));
	memset(cCmtno,0x00,sizeof(cCmtno));

	memcpy(cCmtno,cmtno,3);
	iCmtno = atoi(cCmtno);

	

	switch(iCmtno)
	{
		case 100:
			memcpy(cmtno_des,  "�����",8);
			break;
		case 103:
			memcpy(cmtno_des,  "�����ʽ�㻮",10);
			break;
		case 101:
			memcpy(cmtno_des,  "ί���տ�",8);
			break;
		case 105:
			memcpy(cmtno_des,  "ͬҵ���",8);
			break;
		case 102:
			memcpy(cmtno_des,  "���ճи�",8);
			break;
		case 109:
			memcpy(cmtno_des,  "��������",8);
			break;
		case 108:
			memcpy(cmtno_des,  "֧���˻�",8);
			break;
		case 313:
			memcpy(cmtno_des,  "�����˻�",8);
			break;
		case 314:
			memcpy(cmtno_des,  "�˻�Ӧ��",8);
			break;
		case 301:
			memcpy(cmtno_des,  "����ѯ",8);
			break;
		case 302:
			memcpy(cmtno_des,  "���鸴",8);
			break;
		case 303:
			memcpy(cmtno_des,  "���ɸ�ʽ",8);
			break;
		case 311:
			memcpy(cmtno_des,  "���볷��",8);
			break;
		case 407:
			memcpy(cmtno_des,  "��Ѻ����",8);
			break;
		case 408:
			memcpy(cmtno_des,  "��Ѻ���ʿۿ�",12);
			break;
		case 121:
			memcpy(cmtno_des,  "�ʽ��ƴ�",8);
			break;
		case 721:
			memcpy(cmtno_des,  "�����ʽ�����",8);
			break;
		case 123:
			memcpy(cmtno_des,  "�����ʽ��˻�",8);
			break;
		case 725:
			memcpy(cmtno_des,  "ȫ��Ҹ�֪ͨ",8);
			break;
		case 724:
			memcpy(cmtno_des,  "��Ʊ�����˻�",8);
			break;
		case 124:
			memcpy(cmtno_des,  "��Ʊδ���˻�",8);
			break;
		default:
			memcpy(cmtno_des,  "δ֪ҵ������",12);
	}
	memcpy(desc,cmtno_des,strlen(cmtno_des));
	
	return desc;
	
}
/***��ȡС��ҵ�����͵�����***/
char *cGetLvCmnoDes(char *cmtno,char *desc)
{
	char cCmtno[4];
	char cmtno_des[17];
	int  iCmtno;
	
	memset(cmtno_des,0x00,sizeof(cmtno_des));
	memset(cCmtno,0x00,sizeof(cCmtno));

	memcpy(cCmtno,cmtno,3);
	iCmtno = atoi(cCmtno);	
	switch(iCmtno)
	{
	
		case 1:
			memcpy(cmtno_des, "��ͨ����",8);
			break;
		case 2:
			memcpy(cmtno_des, "��ͨ���",8);
		        break;
		case 3:
			memcpy(cmtno_des, "ʵʱ����",8);
		        break;
		case 4:
			memcpy(cmtno_des, "ʵʱ���",8);
		        break;
		case 5:
			memcpy(cmtno_des, "���ڴ���",8);
		        break;
		case 6:
			memcpy(cmtno_des, "���ڽ��",8);
		        break;
		case 7:
			memcpy(cmtno_des, "�����˻�",8);
		        break;
		case 8:
			memcpy(cmtno_des, "��ͨ��ǻ�ִ",12);
		        break;
		case 9:
			memcpy(cmtno_des, "ʵʱ���ǻ�ִ",12);
		        break;
		case 10:
			memcpy(cmtno_des, "ʵʱ��ǻ�ִ",12);
		        break;
		case 11:
			memcpy(cmtno_des, "���ڽ�ǻ�ִ",12);
		        break;
		case 12:
			memcpy(cmtno_des, "ͨ����Ϣ",8);
		        break;
		case 13:
			memcpy(cmtno_des, "ʵʱ��Ϣ",8);
		        break;
		case 301:
			memcpy(cmtno_des, "С���ѯ",8);
		        break;
		case 302:
			memcpy(cmtno_des, "С��鸴",8);
		        break;
		case 303:
			memcpy(cmtno_des, "���ɸ�ʽ",8);
		        break;
		case 319:
			memcpy(cmtno_des, "�����˻�",8);
		        break;
		case 320:
			memcpy(cmtno_des, "�˻�Ӧ��",8);
		        break;
		case 321:
			memcpy(cmtno_des, "���볷��",8);
		        break;
		case 322:
			memcpy(cmtno_des, "����Ӧ��",8);
		        break;
		case 323:
			memcpy(cmtno_des, "��������",8);
		        break;
		case 324:
			memcpy(cmtno_des, "����Ӧ��",8);
		        break;
		case 325:
			memcpy(cmtno_des, "����֪ͨ",8);
		        break;
		case 327:
			memcpy(cmtno_des, "ֹ������",8);
		        break;
		case 328:
			memcpy(cmtno_des, "ֹ��Ӧ��",8);
		        break;
		default:
			memcpy(cmtno_des, "δ֪��������",12);
		
	}
	memcpy(desc,cmtno_des,strlen(cmtno_des));
	
	return desc;	
}

/***��ȡ���Ľ���״̬****/
char *cGetHvzfStat(char stat,char *desc)
{
	char cStat_Desc[17];
	memset(cStat_Desc,0x00,sizeof(cStat_Desc));


	switch(stat)
	{
		case '0':
			memcpy(cStat_Desc,	 "���˼��˸���",12);
			break;
		case '1':
			memcpy(cStat_Desc,	 "���˷���",8);
			break;
		case '2':
			memcpy(cStat_Desc,	 "���˼�������",12);
			break;
		case '3':
			memcpy(cStat_Desc,	 "�����˻ؼ���",12);
			break;
		case '4':
			memcpy(cStat_Desc,	 "�����˻صȴ�",12);
			break;
		case '5':
			memcpy(cStat_Desc,	 "���˾ܾ�",8);
			break;
		case '6':
			memcpy(cStat_Desc,	 "���˳�������",12);
			break;
		case '7':
			memcpy(cStat_Desc,	 "���˳����ȴ�",12);
			break;
		case '8':
			memcpy(cStat_Desc,	 "���˾ܾ�",8);
			break;
		case '9':
			memcpy(cStat_Desc,	 "���˼�������",12);
			break;
		case 'A':
			memcpy(cStat_Desc,	 "���˵Ǽ�ȡ��",12);
			break;
		case 'B':
			memcpy(cStat_Desc,	 "�����˻�",8);
			break;
		case 'C':
			memcpy(cStat_Desc,	 "���ʵǼǲ�����",14);
			break;
		case 'D':
			memcpy(cStat_Desc,	 "���˸���ȡ��",12);
			break;
		case 'E':
			memcpy(cStat_Desc,	 "���˼��˵Ǽ�",12);
			break;
		case 'F':
			memcpy(cStat_Desc,	 "���˺˵ط�Ѻ��",14);
			break;
		case 'G':
			memcpy(cStat_Desc,	 "���˺�ȫ��Ѻ��",14);
			break;
		case 'H':
			memcpy(cStat_Desc,	 "�������Ĺ���",12);
			break;
		case 'I':
			memcpy(cStat_Desc,	 "֧�й���",8);
			break;
		case 'J':
			memcpy(cStat_Desc,	 "�����Ŷ�",8);
			break;
		case 'K':
			memcpy(cStat_Desc,	 "�˶��������",12);
			break;
		default:
			memcpy(cStat_Desc,	 "δ֪״̬",8);
	}
	memcpy(desc,cStat_Desc,strlen(cStat_Desc));
	
	return cStat_Desc;	
}


/****��ȡ��Ʊ��״̬����***/
char *cGetHpStat(char stat,char *desc)
{	
	char cStatDesc[17];
	memset(cStatDesc,0x00,sizeof(cStatDesc));
	switch(stat)
	{
		case '0':
			memcpy(cStatDesc, "�Ǽ�",4);
			break;
		case '1':
			memcpy(cStatDesc, "ǩ��",4);
			break;
		case '2':
			memcpy(cStatDesc, "��Ʊ�ƴ�",8);
			break;
		case '3':
			memcpy(cStatDesc, "����Ҹ�",8);
			break;
		case '4':
			memcpy(cStatDesc, "��Ʊ�Ҹ�",8);
			break;
		case '5':
			memcpy(cStatDesc, "ȫ�����",8);
			break;
		case '6':
			memcpy(cStatDesc, "�����˻�",8);
			break;
		case '7':
			memcpy(cStatDesc, "δ���˻�",8);
			break;
		case '8':
			memcpy(cStatDesc, "��Ʊ�⸶",8);
			break;
		case '9':
			memcpy(cStatDesc, "ǩ��ȡ��",8);
			break;
		case 'A':
			memcpy(cStatDesc, "�⸶ȡ��",8);
			break;
		case 'B':
			memcpy(cStatDesc, "����",4);
			break;
		case 'C':
			memcpy(cStatDesc, "����Ҹ�ȡ��",10);
			break;
		case 'D':
			memcpy(cStatDesc, "��Ʊ�Ǽ�ȡ��",10);
			break;
		case 'E':
			memcpy(cStatDesc, "��ʧ",4);
			break;
		case 'F':
			memcpy(cStatDesc, "���ʧ",6);
			break;
		case 'G':
			memcpy(cStatDesc, "����δ���˻�",12);
			break;
		case 'H':
			memcpy(cStatDesc, "δ�ý�",6);
			break;
		case 'I':
			memcpy(cStatDesc, "δ�ó��ʳ���",12);
			break;
		default:
			memcpy(cStatDesc, "δ֪״̬",8);
		
	}
	memcpy(desc,cStatDesc,strlen(cStatDesc));
	
	return cStatDesc;
}

/****��ȡС���״̬����****/
char *cGetLvzfStat(char stat,char *desc)
{
	char cStat_Desc[17];
	memset(cStat_Desc,0x00,sizeof(cStat_Desc));
	switch(stat)
	{
		case '0':
			memcpy(cStat_Desc,  "��Ч״̬",8);
		        break;
		case '1':
			memcpy(cStat_Desc,  "����¼��",8);
		        break;
		case '3':
			memcpy(cStat_Desc,  "���˸���",8);
		        break;
		case '4':
			memcpy(cStat_Desc,  "���˷���",8);
		        break;
		case '5':
			memcpy(cStat_Desc,  "���˾ܾ�",8);
		        break;
		case '6':
			memcpy(cStat_Desc,  "��������",8);
		        break;
		case '7':
			memcpy(cStat_Desc,  "�����Ŷ�",8);
		        break;
		case '8':
			memcpy(cStat_Desc,  "���˳����ȴ�",12);
		        break;
		case '9':
			memcpy(cStat_Desc,  "���˳���",8);
		        break;
		case 'A':
			memcpy(cStat_Desc,  "�˻س���ֹ���ȴ�",16);
		        break;
		case 'B':
			memcpy(cStat_Desc,  "�����˻�",8);
		        break;
		case 'C':
			memcpy(cStat_Desc,  "�����ѳ���",10);
		        break;
		case 'D':
			memcpy(cStat_Desc,  "������ֹ��",10);
		        break;
		case 'G':
			memcpy(cStat_Desc,  "�����쳣����",12);
		        break;
		case 'H':
			memcpy(cStat_Desc,  "��ʱ���գ�������",16);
		        break;
		case 'I':
			memcpy(cStat_Desc,  "��ʽ����",8);
		        break;
		case 'J':
			memcpy(cStat_Desc,  "ȫ��Ѻ��",8);
		        break;
		case 'K':
			memcpy(cStat_Desc,  "�ط�Ѻ��",8);
		        break;
		case 'L':
			memcpy(cStat_Desc,  "ȫ��Ѻ�ط�Ѻ����",16);
		        break;
		case 'M':
			memcpy(cStat_Desc,  "�������Ĺ���",12);
		        break;
		case 'N':
			memcpy(cStat_Desc,  "֧�й���",8);
		        break;
		case 'O':
			memcpy(cStat_Desc,  "�����˻صȴ�",12);
		        break;
		case 'P':
			memcpy(cStat_Desc,  "�����˻�",8);
		        break;
		case 'Q':
			memcpy(cStat_Desc,  "ͬ��δ����",10);
		        break;
		case 'R':
			memcpy(cStat_Desc,  "ͬ���ѷ���",10);
		        break;
		case 'S':
			memcpy(cStat_Desc,  "��ͬ��δ����",12);
		        break;
		case 'T':
			memcpy(cStat_Desc,  "��ͬ���ѷ���",12);
		        break;
		case 'U':
			memcpy(cStat_Desc,  "�����ѳ���",10);
		        break;
		case 'V':
			memcpy(cStat_Desc,  "������ֹ��",10);
		        break;
		case 'W':
			memcpy(cStat_Desc,  "�����Ѹ���",10);
		        break;
		case 'Z':
			memcpy(cStat_Desc,  "�����쳣����",12);
		        break;
		default:
			memcpy(cStat_Desc,  "δ֪״̬",8);
	}
	
	memcpy(desc,cStat_Desc,strlen(cStat_Desc));
	
	return cStat_Desc;
}


/***��ȡС���״̬������***/
char *cGetLvPkgDes(char *stat,char *desc)
{
	char cStat[3];
	char cmtno_des[17];
	int  iStat;
	
	memset(cmtno_des,0x00,sizeof(cmtno_des));
	memset(cStat,0x00,sizeof(cStat));

	memcpy(cStat,stat,2);
	iStat = atoi(cStat);	
	switch(iStat)
	{
	
		case 1:
			memcpy(cmtno_des, "�ܾ�,���δͨ��",15);
			break;
		case 2:
			memcpy(cmtno_des, "�ѷ���,���ͨ��",15);
		        break;
		case 3:
			memcpy(cmtno_des, "������",6);
		        break;
		case 4:
			memcpy(cmtno_des, "���Ŷ�",6);
		        break;
		case 5:
			memcpy(cmtno_des, "������",6);
		        break;
		case 6:
			memcpy(cmtno_des, "�ѳ���",6);
		        break;
		case 10:
			memcpy(cmtno_des, "�ѻ�ִ",6);
		        break;
		case 11:
			memcpy(cmtno_des, "�Ѿܸ�",6);
		        break;
		case 12:
			memcpy(cmtno_des, "�ѳ���",6);
		        break;
		case 13:
			memcpy(cmtno_des, "��ֹ��",6);
		        break;
		case 14:
			memcpy(cmtno_des, "�ѳ�ʱ",6);
		        break;
		default:
			memcpy(cmtno_des, "δ֪״̬",8);
		
	}
	memcpy(desc,cmtno_des,strlen(cmtno_des));
	
	return desc;	
}

/***��ȡС������ϵͳ��״̬������***/
char *cGetLvPkg2Des(char *stat,char *desc)
{
	char cStat[2];
	char cmtno_des[11];
	int  iStat;
	
	memset(cmtno_des,0x00,sizeof(cmtno_des));
	memset(cStat,0x00,sizeof(cStat));

	cStat[0]=stat[0];
	iStat = atoi(cStat);	
	switch(iStat)
	{
	
	  case 0:
			memcpy(cmtno_des, "��Ч",4);
		        break;
		case 1:
			memcpy(cmtno_des, "�������",8);
			break;
		case 2:
			memcpy(cmtno_des, "�����",6);
		        break;
		case 3:
			memcpy(cmtno_des, "�ѷ���",6);
		        break;
		case 4:
			memcpy(cmtno_des, "������Ӧ��",10);
		        break;
		case 5:
			memcpy(cmtno_des, "�ѽ���",6);
		        break;
		case 6:
			memcpy(cmtno_des, "��Ӧ��",6);
		        break;
		default:
			memcpy(cmtno_des, "δ֪״̬",8);
		
	}
	memcpy(desc,cmtno_des,strlen(cmtno_des));
	
	return desc;	
}

/***�����������ֶθ����Ͽո�***/
void cDeal_str(char *str,int lenth)
{
	int str_len = 0;
	char space[255];
	char _str [200];
	memset(_str,0x00,sizeof(_str));
	
	memset(space,0x00,sizeof(space));
	vtcp_log("[%s][%d]str==[%s]\n",__FILE__,__LINE__,str);
	
	/***
	deal_half_charcter_xier(str,_str,lenth);
	
	vtcp_log("[%s][%d]str==[%s]\n",__FILE__,__LINE__,_str);
	vtcp_log("[%s][%d]strlen(_str)==[%d],lenth=[%d]\n",__FILE__,__LINE__,strlen(_str),lenth);
	if(lenth>strlen(_str))
	{	
		str_len = lenth-strlen(_str);
		vtcp_log("[%s][%d]strlen(_str)==[%d]\n",__FILE__,__LINE__,strlen(_str));
		memset(space,' ',str_len);
		strcat(_str,space);
		vtcp_log("[%s][%d]strlen(_str)==[%d]\n",__FILE__,__LINE__,strlen(_str));
	}
	vtcp_log("[%s][%d]_str==[%s]\n",__FILE__,__LINE__,_str);
	strcpy(str,_str);
	vtcp_log("[%s][%d]str==[%s]\n",__FILE__,__LINE__,str);
	*************/
	
	
	
	vtcp_log("[%s][%d]str==[%s]\n",__FILE__,__LINE__,str);
	vtcp_log("[%s][%d]strlen(str)==[%d],lenth=[%d]\n",__FILE__,__LINE__,strlen(str),lenth);
	if(lenth>strlen(str))
	{	
		str_len = lenth-strlen(str);
		vtcp_log("[%s][%d]strlen(str)==[%d]\n",__FILE__,__LINE__,strlen(str));
		memset(space,' ',str_len);
		strcat(str,space);
		vtcp_log("[%s][%d]strlen(str)==[%d]\n",__FILE__,__LINE__,strlen(str));
	}
	
	vtcp_log("[%s][%d]str==[%s]\n",__FILE__,__LINE__,str);
	
}

/****����255�ĸ��Գ��ȵĴ�ӡ��ȡ�˷����δ�ӡ�ķ�ʽ90+94+72****/
char * cDeal_content(char *buf,char ch,char *str)
{
	char cBuf[512];
	int len;
	char ret_str[100];
	
	
	vtcp_log("[%s][%d]buf===[%s]\n",__FILE__,__LINE__,buf);
	memset(cBuf,0x00,sizeof(cBuf));
	memset(ret_str,0x00,sizeof(ret_str));
	sprintf(cBuf,"%s",buf);
	vtcp_log("[%s][%d]cBuf===[%s]\n",__FILE__,__LINE__,cBuf);
	myfunxier(cBuf,cBuf);
	vtcp_log("[%s][%d]cBuf===[%s]\n",__FILE__,__LINE__,cBuf);
	/*pub_base_strpack_all(cBuf);*/
	deal_half_charcter(cBuf);
	vtcp_log("[%s][%d]cBuf===[%s]\n",__FILE__,__LINE__,cBuf);
	vtcp_log("[%s][%d]ch===[%c]\n",__FILE__,__LINE__,ch);
	len = strlen(cBuf);
	
	if(len>255)
	{
		cBuf[254]=0x00;
		len = 254;
	}
	vtcp_log("[%s][%d]���Եĳ���==[%d]\n",__FILE__,__LINE__,len);
	
	if(len<=90)
	{
		if(ch=='1')
			memcpy(ret_str,cBuf,len);
		if(ch=='2') 
			memset(ret_str,' ',94);
		if(ch=='2')
			memset(ret_str,' ',71);
	}
	else if(len>90 &&len<=184)
	{
		if(ch=='1')
			memcpy(ret_str,cBuf,90);
		if(ch=='2') 
			memcpy(ret_str,cBuf+90,len-90);
		if(ch=='3')
			memset(ret_str,' ',71);
		
	}
	else if(len>184  &&len<=255)
	{
		if(ch=='1')
			memcpy(ret_str,cBuf,90);
		if(ch=='2') 
			memcpy(ret_str,cBuf+90,94);
		if(ch=='3')
			memcpy(ret_str,cBuf+184,len-184);
	}
	vtcp_log("[%s][%d]ret_str==[%s]\n",__FILE__,__LINE__,ret_str);
	
	memcpy(str,ret_str,strlen(ret_str));
	
	return ret_str;
	
}
/****�����ڸ����д��ڻ��з��źͻس����ŵ����****/
int   myfunxier(char *source,char *target)
{
    int i = 0;
    int j = 0;
    vtcp_log("[%s][%d]������ַ���source==[%s]\n",__FILE__,__LINE__,source);
    while(source[i]!=0x00)
    {
    	
        if((int)source[i]==13)/**13��C�еĻ��з���**/
            i = i+2;
        else if((int)source[i]==10)/**10��ʾ���ǻس�����**/
            i = i+1;
        else
        {
            target[j] = source[i];
            i++;
            j++;
        }
    }
    target[j] = 0x00;/**�����***/
    
    vtcp_log("[%s][%d]����õ��ַ���target==[%s]\n",__FILE__,__LINE__,target);
    return 0;
}


/***��ȡ�����е�����*******/
char * cGet_br_name(char *ac_br_no,char *br_name)
{
	char cAc_br_no[13];
	int  ret = 0;
	struct hv_uniontab_c hv_uniontab;
	memset(&hv_uniontab,0x00,sizeof(hv_uniontab));
	
	memset(cAc_br_no,0x00,sizeof(cAc_br_no));
	
	memcpy(cAc_br_no,ac_br_no,strlen(ac_br_no));
	
	ret = Hv_uniontab_Sel(g_pub_tx.reply,&hv_uniontab,"or_br_no ='%s' ",cAc_br_no);
	if(ret &&ret!=100)
	{
		vtcp_log("[%s][%d]���ݿ��������ret==[%d]!\n",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"D103");
		return ;
	}
	vtcp_log("[%s][%d]hv_uniontab.br_name=[%s]\n",__FILE__,__LINE__,hv_uniontab.br_name_s);
	memcpy(cAc_br_no,hv_uniontab.br_name_s,sizeof(hv_uniontab.br_name_s)-1);
	pub_base_strpack(cAc_br_no);
	if(cAc_br_no[0]==0x00||cAc_br_no[0]==' '||!strncmp(cAc_br_no,"NULL",4))
	{
		memcpy(cAc_br_no,hv_uniontab.br_name_f,sizeof(hv_uniontab.br_name_f)-1);
		pub_base_strpack(cAc_br_no);
		memcpy(br_name,cAc_br_no,30);
	}
	else
		memcpy(br_name,hv_uniontab.br_name_s,sizeof(hv_uniontab.br_name_s)-1);
	
	
	vtcp_log("[%s][%dbr_name=[%s]\n",__FILE__,__LINE__,br_name);
	return cAc_br_no;
}

/**********************************************************************
* ��������    pub_rpt_read_pfm_qd_xier
* �������ܣ�  �������ʽ�ļ���������������ת���ɱ���,��ֱ������������ļ���
* ����/ʱ�䣺 2004��04��13��
*
* ������
*     ���룺        
*            char vpfm[21] ��ӡ�ñ���ĸ�ʽ�ļ���(��.tbl,����ĸ�ʽ�ļ�ȫ����Ϊ: �ļ���.tbl)
*            char gsm[3]   �Ӹ�ʽ "00" �������ĸ�ʽ�ļ�ת�� 
*                          ����������Ļ�,�⽫��ʽ�ļ���ǰ��λ��gsm��ͬ����ת��
*            int  rpt_code ��ӡ����(Ϊ��ͬ��ʽ�ı���,��ͬһ��get_ratioʱ��)
*            int  Get_ratio( int bli,int bll,char str[100],int rpt_code ) ����  
*     ���: 
*            FILE *fp  ���������ļ�ָ��
*            int *pline �ļ�ָ��ָ������(�����������)
*            int *opncnt ͬһ�������ļ���,����յ�����
* �� �� ֵ: 0 �ɹ�
*          !0 ���ɹ�
*
* �޸���ʷ��
*
********************************************************************/

int pub_rpt_read_pfm_qd_xier( FILE *fp,int *pline,
	char vpfm[21],int rpt_code,char gsm[3],int *opncnt, 
	int Get_ratio( int bli,int bll,char str[100],int rpt_code,char mygsm[3]) )
{
	FILE *fmp;
	char fil[100];
	char onechar[2];
	char info[800],str[100];
	int i=0;
	int flag=0,bli=0,bll=0;
	int blflag=0;
	char blstr[200];
	int smflag=0;
	int recflag=0;
	int kzfflag=1;
	int line;
	char _gsm[3];
	
	memset(_gsm,0x00,sizeof(_gsm));
	
	
	vtcp_log("[%s][%d]gsm===[%s]\n",__FILE__,__LINE__,gsm);
	
	memcpy(_gsm,gsm,2);
	vtcp_log("[%s][%d]_gsm===[%s]\n",__FILE__,__LINE__,_gsm);
	

	line=*pline;

	*opncnt=0;

	
	
	sprintf( fil,"%s/%s.tbl",getenv("PFMDIR"),vpfm );
	fmp=fopen( fil,"r" );
	if( fmp==NULL )
	{
		sprintf(acErrMsg,"open file [%s] error",fil );
		WRITEMSG
		goto ErrExit;
	}
	
	memset( info,0,800 ); i=0;
	memset( blstr,0,200 );
	while(1)
	{

		memset( onechar,0,2 );
		fread(onechar,sizeof(char),1,fmp);
		if( feof(fmp) )
		{
			/** fprintf( fp,"%s\n",info );  */
			memset( info,0,800 ); i=0;
			flag=0;
			break;
		}
		else if( i==0 && ( onechar[0]==':' || onechar[0]=='?') )
		{
			smflag=1;
			info[i]=onechar[0]; i++;
		}
		else if( i<=1 && gsm[0]!='0' )
		{
			info[i]=onechar[0]; i++;
		}
		else if( gsm[0]!='0' && (info[0]!=gsm[0] || info[1]!=gsm[1]) 
			&& onechar[0]!='\0' && onechar[0]!='\n' && onechar[0]!=' ' )
		{
			info[i]=onechar[0]; i++;
		}
		else if( strstr(blstr,onechar) && smflag==0 && kzfflag==1
			&& onechar[0]!='\0' && onechar[0]!='\n' && onechar[0]!=' ' )
		{
			if( flag==1 )
			{
				bll++;
			}
			else
			{
				bll++;
				flag=1;
			}
			bli=onechar[0];
		}
		else 
		{
			if( flag==1 )
			{
				memset(str,0,100);
				vtcp_log("[%s][%d]��ʼ����д����,bli=[%c],bll=[%d],_gsm=[%s]\n",
					__FILE__,__LINE__,(char )bli,bll,str,_gsm);
				if( Get_ratio( bli,bll,str,rpt_code, _gsm) ) goto ErrExit;
				vtcp_log("[%s][%d]��ȡ��������!str==[%s]bli===[%c]\n",__FILE__,__LINE__,str,bli);
				strcpy( info+i,str );
				i=i+bll;
				flag=0; bll=0;
			}
			if( onechar[0]=='\n' )
			{
				if( smflag==1 )
				{
					if( info[0]==':' )
					{
						memset( blstr,0,200 );
						info[200]='\0';
						strcpy( blstr,info+1 );
					}
					else if( info[0]=='?' )
					{
						*opncnt=atoi( info+1 );
					}
				}
				else
				{
					if( gsm[0]!='0' && !strncmp(info,gsm,2) )
					{
						fprintf( fp,"%s\n",info+2 );
/*
sprintf(acErrMsg,"[%s] \n",info+2);
WRITEMSG
*/
						line++;
					}
					else if( gsm[0]!='0' )
					{
						;
					}
					else
					{
						fprintf( fp,"%s\n",info );
/*
sprintf(acErrMsg,"[%s] \n",info);
WRITEMSG
*/
						line++;
					}
				}
				memset( info,0,800 ); i=0;
				smflag=0;
			}
			else
			{
				if( onechar[0]=='@' ) kzfflag=kzfflag*(-1);
				info[i]=onechar[0]; i++;
				if( i>=800 )
				{
					printf("====[%d]\n",i);
					goto ErrExit;
				}
			}
		}
	/**
	vtcp_log("INFO=[%s]i=[%d]",info,i);
	**/
	}

	fclose(fmp);

	*pline=line;

GoodExit:
	return 0;
ErrExit:
	return 1;
}


/***�����ֻ��е�ʱ��������������***/
int deal_half_charcter(char *buf)
{
		int i = 0;
		int j = 0;
		int size = 0;
		int index =0;
		
		
		

		printf("[%s]====\n",buf);
		size = strlen(buf);
		
		char cBuf[1024];
		
		memset(cBuf,0x00,sizeof(cBuf));


		for(i=0;i<size;i++)
		{
				if((int)buf[i]<128 &&(int)buf[i+1]>128 && (i+1+index)%2)
				{
						cBuf[j] = buf[i];
						cBuf[j+1]= ' ';
						j = j+2;
						index++;
				}
				else
				{
						cBuf[j] = buf[i];
						cBuf[j] = buf[i];
						j++;
				}
		}
		cBuf[j] = 0x00;
		memcpy(buf,cBuf,j);
		return 0;
}
