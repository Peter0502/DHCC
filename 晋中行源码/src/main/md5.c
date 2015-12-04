#include<stdio.h>
#include<string.h>
int main()
{
char msg[50];
char tmp[50];
char out[50];
char md[50];
char *desc="20130822150600";
memset(out,0x00,sizeof(out));
memset(tmp,0x00,sizeof(tmp));
memset(md,0x00,sizeof(md));
memset(msg,0x00,sizeof(msg));
strcpy(tmp,desc);
printf("see here ==[%s]\n",tmp);
MD5ENC(tmp,out,strlen(tmp));
sprintf(msg,"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",out[0],out[1],out[2],out[3],out[4],out[5],out[6],out[7],out[8],out[9],out[10],out[11],out[12],out[13],out[14],out[15]);
strcpy(md,msg); 
printf("%s\n",md);
return 0;
}
