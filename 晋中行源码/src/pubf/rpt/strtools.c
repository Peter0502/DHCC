#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
char * btrim(s)    /* 删除右边空格  */
  char *s ;
  {
    char *cp =s+strlen(s)-1;
    while (cp>s && *cp==' ') *cp--='\0';
    return (s);
  }

char * ltrim(s)    /* 删除左边空格  */
  char *s ;
  {
    char *p1,*p2;
    for (p1=s; *p1==' '; p1++);
    for(p2=s; *p1; p1++, p2++) *p2=*p1; *p2='\0';
    return (s);
  }

int strcent(s, wid, sc)
char *s, *sc ;
int  wid ;
{
  int w, w1, w2;
  w=strlen(s);
  if (w>=wid) {
  	strncpy(sc, s, wid);
  	sc[wid]='\0';
  	return (w>wid ? 1 : 0);
  }
  w1= (wid- w) / 2 ;
  w2=wid -w1-w ;
  sprintf(sc, "%*s%s%*s\0",w1, "", s,w2, "");
  return 0;	
}

/*  比较串的值: sb被比较串  dc比较浮点数  k0起始位置
    返回:    0:相等  <0:小于  >0大于
*/
int strvdouble(sb,  dc, k0, kw)
char *sb; double dc; int k0, kw;
{ double d; char buf[65]; 
  if (strlen(sb)<=k0 || k0<0) return -9;
  strncpy(buf,sb+k0,kw); buf[kw]='\0';
  d=atof(buf)-dc; 
  return (d>1.E-8 ? 1:(d<-1.E-8 ? -1 : 0));
}

/*  比较串的值: sb被比较串  sc比较串  k0起始位置
    当k0<0时为sc存在于sb中
    返回:    0:相等(当k0<0时为存在)  <0:小于  >0大于
*/
int strvcmp(sb, sc, k0, kw)
char *sb, *sc; int k0, kw;
{ int k;
  if (k0<0) return (!strstr(sb,sc));
  if (strlen(sb)<=k0) return -9;
  k=strncmp(sb+k0, sc, kw); 
  return (k>0?1:(k<0?-1:0));
}

/*  求文件长度 */
long filesize( stre)
FILE *stre;
{
   long curpos, length;
   curpos = ftell(stre);
   fseek(stre, 0L, SEEK_END);
   length = ftell(stre);
   fseek(stre, curpos, SEEK_SET);
   return length;
}

/* 读文本文件到缓冲区buf, 最大长度maxl, 返回实际长度 */
long readfile(fnm, buf, maxl)
char *fnm,  *buf; long maxl;
{
  FILE *ff; long fl;
  if ((ff=fopen(fnm,"r"))==NULL) return 0;
  rewind(ff); fl=ftell(ff); readln(ff,buf,5);
  for ( ;!strncmp(buf,"]*[",3) && !feof(ff); )
       { fl=ftell(ff); readln(ff,buf,5); }
  fseek(ff,fl ,SEEK_SET);
  fl=fread(buf,1,maxl,ff); *(buf+fl)='\0';
  fclose(ff); return (fl);
  }
/*  从文件ff中读一行文本到buf中, buf最大长度=mw, 返回该行长度 */
int readln(ff, buf, mw)
FILE *ff; char *buf; int mw;
{ char ch, *p; int k=0, w;
  p=buf;
  if (!feof(ff)) {ch=fgetc(ff);
      if (ch==0x07) { w=fgetc(ff); 
         for (k=0; k<w; k++,p++) { ch=fgetc(ff); 
          if (k<mw) *p=(ch & 0xF0) >>4 | (ch & 0x0F)<<4; } }
       else { 
        while (!feof(ff) && (ch!='\n' )) {k++;
         if (k<mw) { *p=ch; p++; } 
         ch=fgetc(ff); } }
       }
  *p='\0';
  return (k);
  }
/*  从串sf中读一行文本到buf中, buf最大长度=mw , 分行符=cr */
char *sreadcr(sf, buf,  mw, cr)
char *sf,  *buf; int mw; char cr;
{ char ch, *p1, *p2; int k;
  for (p1=sf, p2=buf, k=0; *p1 && *p1!=cr; p1++, k++)
     if (k<mw) { *p2=*p1; p2++; }
  *p2='\0'; return (p1); }

/*  从串sf中读一行文本到buf中, buf最大长度=mw */
char *sreadln(sf, buf,  mw)
char *sf, *buf; int mw;
{ char ch, *p1, *p2; int k;
  for (p1=sf, p2=buf, k=0; *p1 && *p1!='\n'; p1++, k++)
     if (k<mw) { *p2=*p1; p2++; }
  *p2='\0'; return (p1); }

/*包含\??串s2转换成串s1 */
int strvcpy(s1,s2, w)
char *s1, *s2; int w;
{
  char *p1,*p2;
  int k, i;
  for (p1=s1, p2=s2,i=0 ; *p2 && i<w; p2++, p1++, i++) {
    if (*p2=='\\') { sscanf(p2+1,"%2x", &k); p2+=2; *p1=(char)k; }
    else *p1=*p2;
    }
  return  1;
 }

/*  删除左边空格 */
  int strltrim(str,lstr)
  char *str, *lstr;
  { char *p1,*p2;
    for (p1=str; *p1==' '; p1++);
    for(p2=lstr; *p1; p1++, p2++) *p2=*p1; *p2='\0';
    return p2-lstr; }
/*  删除右边空格 */
  int strrtrim(str,rstr)
  char *str, *rstr;
  { char *p, *p1, *p2; int slen=strlen(str);
    for(p=&str[slen-1];*p==' ';p--) ;
    for (p1=str, p2=rstr; p1<=p; p1++, p2++) *p2=*p1; *p2='\0';
    return p2-rstr; }
/*  左补空(截断)  */
  int strlpad(str,rstr, w, c)
  char *str, *rstr;int w;char c;
  { char *p1, *p2; int slen=strlen(str);
    if (w>slen) {
     for (p1=str+slen-1,p2=rstr+w-1; p1>=str&&p2>=rstr; p1--,p2--) *p2=*p1;
     for (; p2>=rstr; p2--) *p2=c; *(rstr+w)='\0'; }
    else { for (p1=str+(slen-w), p2=rstr; *p1; p1++, p2++) *p2=*p1;
     *p2='\0';}
    return (1); 
    };

/*  左补空(截断)  */
  int strrpad(str,rstr, w, c)
  char *str, *rstr;int w;char c;
  { char *p1,*p2; int slen=strlen(str);
    for (p1=str,p2=rstr; p1<=str+slen-1&&p2<=rstr+w-1; p1++,p2++) *p2=*p1;
    for (; p2<=rstr+w-1; p2++) *p2=c; *(rstr+w)='\0'; return 1; };

/* 对中  */
  int strcenter(str,cstr, w, c)
  char *str, *cstr;int w;char c;
  { int slen=strlen(str);
    if (w>slen) { strlpad(str,cstr,w-(w-slen)/2,c);
		  strrpad(cstr,cstr,w,c); }
    else { strlpad(str,cstr,slen-(slen-w)/2,c);
	   strrpad(cstr,cstr,w,c); }
    return 1; }

/* 分解串 */
  int strdefval(str, fh, sdef, sval)
  char *str, fh, *sdef, *sval;
  { int i; char *p;
    for (p=str,i=0; *p!=fh && *p!='\n' && *p && p && i<20; i++, p++)
       sdef[i]=*p; sdef[i]=0;
    if (!(*p) || !(p)) *sval='\0';
    else { for (i=0, p+=1; *p!='\n' && *p && p && i<80; p++, i++)
	   sval[i]=*p; sval[i]='\0';} 
    return 1; } 

/* flag=0:删除右边空格字符;flag=1:用空格字符填满右边;*/
int strleft(str,len,lstr,flag)
char *str,*lstr;
int len,flag;
{
  int len0,i,j;
  i=len0=j=0;
  while((lstr[j++]=str[i++])!='\0') len0++;
  if(flag==1){
    i=0; j--; while(i++<len-len0) lstr[j++]=' '; lstr[j]='\0';
  }
  else{
    j=len0-1; if(j<0) j=0;
    while(j>0&&lstr[j]==' ') j--;
    if(lstr[j]!=' ') j++;
    lstr[j]='\0';
  }
  return(0);
};

/*  删除串str右边空格,将其左边用字符c填充, */
/*  赋给串rstr.其返回值为左边填充符个数.*/
int strright(str, cvlen, rstr, c)
char *str;int cvlen;char *rstr, c;
{
  strrtrim(str, rstr);
  strlpad(rstr, rstr, cvlen, c);
/*  int i, cvlen0,flag;
  char *retustr;
  retustr=(char *)malloc(cvlen+1);
  flag=1;
  cvlen0=strlen(str);
  while(flag&&cvlen0-->=0) if(*(str+cvlen0)!=' ') flag=0;
  if(flag) *str='\0'; else *(str+cvlen0+1)='\0';
  for(i=0;i<cvlen-cvlen0-1;i++) *(retustr+i)=c;
  *(retustr+i)='\0';
  strcat(retustr,str); strcpy(rstr, retustr);
  free(retustr); */
  return(1);
};

/* flag=0:字串去,号    flag=1:字串加,号 */
int strnumdos(str, slen, rstr, flag)
char *str; int slen; char *rstr;int flag;
{
  int i=0,k ;
  char *p1,*p2 ;
  strrtrim(str, rstr); strlpad(rstr, rstr, slen, ' ');
  for (p1=p2=rstr; *p1; p1++)
  {  if (*p1=='.') i= slen- (p1-rstr);
     if (*p1!=',')  *(p2++)=*p1;
  }
  *p2='\0';
  if (flag==0) 
  { if (p2!=rstr+slen) strlpad(rstr,rstr, slen, ' ');  return 1;}

  k=strltrim(rstr,rstr); 
  if (k+(int)(k-i-1)/3 > slen) 
    { strlpad(rstr, rstr, slen, ' '); return 2; }
  p1=rstr+k-1; 
  p2=rstr+slen-1;
  if (i>0) for (k=0; k<i; k++, p1--, p2--) *p2=*p1;
  for (k=0; p1>=rstr; p1--) 
  {
     *p2=*p1;
     p2--;  k++;
     if (p2<rstr)  break; 
     if (k>=3 && *(p1-1)!='-' && p1>rstr) 
     {
        k=0;
	*p2=','; p2--;
	if (p2<rstr) break;
     }
  }
  for (; p2>=rstr; p2--) *p2=' ';
  return 1;  
}

/* 将金额转换为大写 
extern double fabs(); 
double fabs(dd)
double dd;
{
  return (dd<0.00 ? -dd : dd); 
}
*/
int numtomoney( je, str0)
double je;char *str0;
{
  char   str[61],buf[16],power[3],dig[3];
  int    i,pos,flag;
  pos=0;
  sprintf(buf,"%15.2f",je);
  if(fabs(je)<0.01){
    strcpy(str0,"零"); return(0);
  }
  i=strlen(buf);
  if(i>15){
    printf("数据太大\n");  return(0);
  }
  for(i=0; i<60; i++) str[i]=' '; str[i]='\0';
  for(i=0; buf[i]==' '; i++);
  if(i==11&&buf[i]=='0') i=i+2;
  if(i==13&&buf[i]=='0') i++;
  while(i<=14){
    flag=1;
    switch(i){
    case  0: case  4: case  8: sprintf(power,"仟"); break;
    case  1: case  5: case  9: sprintf(power,"佰"); break;
    case  2: case  6: case 10: sprintf(power,"拾"); break;
    case  3: sprintf(power,"亿");
	     if(buf[i]=='0'){
	       str[pos]=power[0]; str[pos+1]=power[1]; pos+=2; flag=0;
	     }
	     break;
    case  7: sprintf(power,"万");
	     if(buf[i]=='0'){
		if(buf[4]!='0'||buf[5]!='0'||buf[6]!='0'){
		  str[pos]=power[0]; str[pos+1]=power[1]; pos+=2;
		}
		else if(buf[8]!='0'){
		  sprintf(power,"零");
		  str[pos]=power[0]; str[pos+1]=power[1]; pos+=2;
		}
		flag=0;
	     }
	     break;
    case 11: sprintf(power,"元");
	     if(buf[i]=='0'&&pos!=0){
		str[pos]=power[0]; str[pos+1]=power[1]; pos+=2; flag=0;
	     }
	     break;
    case 12: flag=0;
	     if(buf[i+1]=='0'&&buf[i+2]=='0'){
	       sprintf(power,"整"); str[pos]=power[0]; str[pos+1]=power[1];
	       pos+=2; goto outloop;
	     }
	     break;
    case 13: sprintf(power,"角"); break;
    case 14:
	 if(buf[i]=='0'){
	   sprintf(power,"整"); str[pos]=power[0]; str[pos+1]=power[1];
	   pos+=2;  goto outloop;
	 }
	 else sprintf(power,"分");
	 break;
    }
    if(flag==1){
      flag=1;
      switch(buf[i]){
      case '0' : if (buf[i+1]!='0') sprintf(dig,"零"); else flag=0; break;
      case '1' : sprintf(dig,"壹"); break;
      case '2' : sprintf(dig,"贰"); break;
      case '3' : sprintf(dig,"叁"); break;
      case '4' : sprintf(dig,"肆"); break;
      case '5' : sprintf(dig,"伍"); break;
      case '6' : sprintf(dig,"陆"); break;
      case '7' : sprintf(dig,"柒"); break;
      case '8' : sprintf(dig,"捌"); break;
      case '9' : sprintf(dig,"玖"); break;
      }
      if(buf[i]=='0'){
	if(flag==1){
	  str[pos]=dig[0]; str[pos+1]=dig[1]; pos=pos+2;
	}
      }
      else{
	str[pos]=dig[0]; str[pos+1]=dig[1]; str[pos+2]=power[0];
	str[pos+3]=power[1]; pos=pos+4;
      }
    }
    i++;
  }
outloop:
  str[pos]='\0';
  strcpy(str0,str);
  return(0);
} ;

/* 查找str串中形如str1串, 如&??, 真实串返回为str1 */
char *strfind1(str, str1)
char *str; char *str1;
{ char ch=*str1, *ptr;
  ptr=strchr(str,ch);
  if (ptr) memcpy(str1, ptr, strlen(str1));
  return ptr;
  }

/* 对str1串中形如sf串用sr串替换, 新串为str2, 最多替换次数为nn,
   返回实际替换次数 */
int strfind1repl(str1, sf, sr, str2, nn )
char *str1,  *sf,  *sr,  *str2; int nn ;
{ char ch=*sf, *p, *ptr, *p1, *p2;
  int i,k=0, wf=strlen(sf), wr=strlen(sr);
  p1=str1; ptr=strchr(p1,ch); p2=str2;
  for (k=0; k<nn && ptr; k++) {
    for (p=p1; p<ptr; p++, p2++) *p2=*p;
    for (i=0, p=sr; i<wr; i++, p++, p2++) *p2=*p;
    p1=ptr+wf; ptr=strchr(p1,ch);
    }
  for (p=p1; p && *p; p++, p2++) *p2=*p;
  *p2='\0';
  return k;
  }

