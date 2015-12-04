
/*   报表公用函数       */ 
#define ValueLen    201

char b0[3]="??";   /* unknow string */
char b1[3]="┌";   /* a */
char b2[3]="┐";   /* b */
char b3[3]="─";   /* c */
char b4[3]="│";   /* d */ 
char b5[3]="├";   /* e */
char b6[3]="┤";   /* f */
char b7[3]="┬";   /* g */
char b8[3]="┴";   /* h */
char b9[3]="┼";   /* + */
char b10[3]="└";  /* L */
char b11[3]="┘";  /* J */
char b12[3]="  ";  /* B */


int draw_line( fp,var,len)
FILE   *fp;
char   var[3];
int    len;
{
	char   val[ValueLen];
	int    i,  pos;
		pos=0;
		for(i=0;i<len;i++)
		{
			val[pos++]=var[0];
			val[pos++]=var[1];
		}
		val[pos]=0x00;
		fprintf( fp,"%s",val);
		
  return(0);
}

int  draw_int( fp, number,len,flag )
FILE  *fp;
int   len;
long  number;
char  flag;         /* flag=0:左靠齐,1:右靠齐 ,flag=3,flag=4,L:左有百分位,R: */
{
	char	val[ValueLen];
	char    temp[20];
	int     pos,   i;
	char    format[20];
		memset(val,' ',len);
		memset(temp,0x00,sizeof(temp) );
		if( number==0 )
		{
			val[len]=0x00;
			fprintf(fp,"%s",val);
			return(0);
		}
		if( flag=='3' || flag=='4' ) 
		{
			memset( format,0x00,sizeof(format) );
			sprintf( format ,"%%0%dd",len);
			sprintf(temp,format,number );
		}
		else
		   sprintf(temp,"%ld",number);
		pos=0;
		switch( flag )
		{
			case  '4':
			case  '1':
					   for(i=0;i<len-strlen(temp);i++)
					   val[pos++]=' ';
					   for(i=0;i<strlen(temp);i++)
					   val[pos++]=temp[i];
					   break;
			case  '3':
			case  '0':
					   for(i=0;i<strlen(temp);i++)
					   val[pos++]=temp[i];
					   for(i=0;i<len-strlen(temp);i++)
					   val[pos++]=' ';
					   break;
			case  'L':
					   dotnum_long(temp,number);
					   for(i=0;i<strlen(temp);i++)
					   val[pos++]=temp[i];
					   for(i=0;i<len-strlen(temp);i++)
					   val[pos++]=' ';
					   break;
			case  'R':
					   dotnum_long(temp,number);
					   for(i=0;i<len-strlen(temp);i++)
					   val[pos++]=' ';
					   for(i=0;i<strlen(temp);i++)
					   val[pos++]=temp[i];
					   break;

		}
		val[pos]=0x00;
		fprintf(fp,"%s",val);

	return(0);
}

  
int  draw_double( fp,number,len,flag )
FILE      *fp;
double    number;
int       len;
char      flag;        /* flag=0:左靠齐, 1:右靠齐, L:左有百分位, R: */
{
	char	val[ValueLen];
	char	temp[20];
	int     pos,i;
		memset(val,' ',len);    
		if( number>-0.005 && number<0.005 )
		{
			val[len]=0x00;
			fprintf( fp,"%s",val);
			return(0);
		}
		memset(temp,0x00,sizeof(temp) );
		sprintf( temp,"%.2f",number);
		pos=0;
		switch( flag ) 
		{
			case '0':
					  for(i=0;i<strlen(temp);i++)
					  val[pos++]=temp[i];
					  for(i=0;i<len-strlen(temp);i++)
					  val[pos++]=' ';
					  break;
			case '1':
					  for(i=0;i<len-strlen(temp);i++)
					  val[pos++]=' ';
					  for(i=0;i<strlen(temp);i++)
					  val[pos++]=temp[i];
					  break;
			case 'L':
					  dotnum(temp,number);
					  for(i=0;i<strlen(temp);i++)
					  val[pos++]=temp[i];
					  for(i=0;i<len-strlen(temp);i++)
					  val[pos++]=' ';
					  break;
			case 'R':
					  dotnum(temp,number);
					  for(i=0;i<len-strlen(temp);i++)
					  val[pos++]=' ';
					  for(i=0;i<strlen(temp);i++)
					  val[pos++]=temp[i];
					  break;
		}
		val[pos]=0x00;
		fprintf( fp,"%s",val );

	return(0);
}
       
/* --------------- dotnum() ---------------- */
int  dotnum(cnum,amt1)
char       *cnum;
double     amt1;
{
  int	i,len,count,pos;
  char  cje[20];
	if( amt1<0.005 && amt1>-0.005 )
	{
		memset(cnum,' ',19);
		cnum[19]=0x00;
		return(0);
	}
	else 
	{
	 	memset(cnum,0x00,20);	
		sprintf(cnum,"%.2lf",amt1);
		len=strlen(cnum)-1;
		memset(cje,' ',sizeof(cje) );  cje[19]=0x00;
		pos=18;
		for(i=0;i<3;i++)   cje[pos--]=cnum[len--]; 
		count=0;
		for(i=len;i>=0;i--) 
		{
			cje[pos]=cnum[i];
			if(pos!=0) pos--;
			count++;
			if( count==3 )
			{
				if( i>0  && cnum[i-1]>='0' && cnum[i-1]<='9' )
				{
					cje[pos--]=',';
					count=0;
				}
			}
		}
	}

	len=0;

	pos++;
	strcpy( cnum,cje+pos );

  return(0);
}


/* --------------- dotnum_int() ---------------- */
int  dotnum_long(cnum,amt1)
char       cnum[20];
long       amt1;
{
  int	i,len,count,pos;
  char  cje[20];
	if( amt1==0 )
	{
		memset(cnum,' ',19);
		cnum[19]=0x00;
		return(0);
	}
	else 
	{
	 	memset(cnum,0x00,20);	
		sprintf(cnum,"%ld",amt1);
		len=strlen(cnum)-1;
		memset(cje,' ',sizeof(cje) );  cje[19]=0x00;
		pos=18;
		count=0;
		for(i=len;i>=0;i--) 
		{
			cje[pos]=cnum[i];
			if(pos!=0) pos--;
			count++;
			if( count==3 )
			{
				if( i>0  && cnum[i-1]>='0' && cnum[i-1]<='9' )
				{
					cje[pos--]=',';
					count=0;
				}
			}
		}
	}

	pos++;
	strcpy( cnum,cje+pos );
  return(0);
}

int   draw_string( fp,var,len,flag)
FILE   *fp;
char   var[201];
int	   len;
char   flag;
{
	char   val[201];
	int    i,    pos;
	int	   varlen;
	pos=0;
	memset(val,0x00,len+2);
	switch( flag )
	{
		case '0':
				  strcpy( val,var);
				  pos=strlen(val);   varlen=len-pos;
				  for(i=0;i<varlen;i++) { val[pos++]=' '; }
				  val[pos]=0x00;
				  break;
		case '1':
				  for(i=0;i<len-strlen(var);i++)
				  val[pos++]=' ';
				  strcpy(val+pos,var);
				  break;
	}
	fprintf( fp,"%s",val);
	   
  return(0);
}

