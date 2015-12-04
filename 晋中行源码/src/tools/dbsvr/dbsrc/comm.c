#include"stdio.h"
#include"stdlib.h"
char *zip_tail(char *s){
	int i=strlen(s)-1;
	while(i>=0){
		if(s[i]==' '){
			s[i]='\0';
			i--;
		}else{
			break;
		}
	}
	return(s);
}
char *get_word(char *mainstr){
	static char *pStr;
	static char word[128];
	int i=0;
	if(mainstr!=NULL){
		pStr=mainstr;
	}
	if(pStr[i]=='\0'){
		return(NULL);
	}
	while(pStr[i]!='\0'){
		if((pStr[i]>='a' && pStr[i]<='z')||	
			(pStr[i]>='A' && pStr[i]<='Z')||	
			(pStr[i]>='0' && pStr[i]<='9')||	
			pStr[i]=='\'' || pStr[i]=='_'|| pStr[i]=='"'){
			word[i]=pStr[i];
			i++;
		}else{
			if(i==0){
				word[i]=pStr[i];
				i++;
				break;
			}else{
				break;
			}	
		}
	}
	word[i]='\0';
	pStr+=i;
	return(word);
}
char *get_lastword(char *mainstr){
	static char word[128];
	int i,len;
	word[0]='\0';
	len=strlen(mainstr);
	i=len-1;
	while(i>=0){
		if((mainstr[i]>='a' && mainstr[i]<='z')||	
			(mainstr[i]>='A' && mainstr[i]<='Z')||	
			(mainstr[i]>='0' && mainstr[i]<='9')||	
			mainstr[i]=='\'' || mainstr[i]=='_'|| mainstr[i]=='"'){
			i--;
		}else{
			break;
		}
	}
	if(i<0){
		i=0;
	}else{
		i++;	
	}
	strcpy(word,mainstr+i);
	return(word);
}
char* key_strcat(char *mainstr,char *substr){
	char *ptr;
	char cFirststr;
	int iFind=0;
	if(substr==NULL){
		return(mainstr);
	}	
	/*
	vtcp_log("%s,%d substr=[%s]",__FILE__,__LINE__,substr);
	*/
	ptr=get_word(substr);
	cFirststr=ptr[0];
	/**
	vtcp_log("%s,%d ptr=[%s]",__FILE__,__LINE__,ptr);
	**/
	while(ptr!=NULL){
		if(strcmp("date",ptr)==0||
			strcmp("group",ptr)==0||
			strcmp("number",ptr)==0) {
			/****替换关键字****/
			strcat(mainstr,"\"");
			strcat(mainstr,ptr);
			strcat(mainstr,"\"");
		}else if(ptr[0]=='['){
			/****替换[a,b]为substr(s,a,b)的方式****/
			int i=0;
			char *pcLastWord;
			char * p;/* add by LiuHuafeng 20070228 */
			int len;
			pcLastWord=get_lastword(mainstr);
			len=strlen(pcLastWord);
			p=ptr;
			/**
			vtcp_log("%s,%d len====[%d]ptr=[%s]first=[%c]lastword=[%s]",__FILE__,__LINE__,len,ptr,cFirststr,pcLastWord);
			*/
			if(cFirststr=='\'' || cFirststr>128){
				sprintf(mainstr+strlen(mainstr)-len,"%s[",pcLastWord);
				iFind=1;
			}
			else
				sprintf(mainstr+strlen(mainstr)-len,"substr(%s,",pcLastWord);
		}else if(ptr[0]==']'){
			/***处理[a]的特殊情况***/
			if(iFind==0)
			{
			/**向前找逗号,如果在找到括号之前找到了1个则需要处理,2个则不需处理**/
			int iDhCnt=0;
			int id;
			int num1,num2;
			int weight1,weight2;
			weight1=weight2=1;
			num1=num2=0;
			id=strlen(mainstr)-1;
			while(id>=0){
				if(mainstr[id]=='('){
					break;
				}else if(mainstr[id]==','){
					iDhCnt++;
				}else if(mainstr[id]>='0' && mainstr[id]<='9'){
					if(iDhCnt==0){
						num2=num2+(mainstr[id]-'0')*weight2;
						weight2*=10;
					}else{
						num1=num1+(mainstr[id]-'0')*weight1;
						weight1*=10;
					}
				}
				id--;	
			}
			if(iDhCnt>1){
				/****informix第二个参数是终止位置,但ORACLE是长度****/
				/****此时,num1是起始位置,num2是终止位置***/	
				id=strlen(mainstr)-1;
				while(mainstr[id]!=','){
					id--;
				}
				sprintf(mainstr+id+1,"%d",num2-num1+1);
				strcat(mainstr,")");
			}else{
				strcat(mainstr,",1)");
			}
			}
			else
			{
				iFind=0;
				strcat(mainstr,ptr);
			}
				
		}else{
			strcat(mainstr,ptr);
		}
		cFirststr=ptr[0];
		ptr=get_word(NULL);
		/**
		vtcp_log("%s,%d ptr=[%s]first=[%c]",__FILE__,__LINE__,ptr,cFirststr);
		**/
	}
	return(mainstr);	
}
/****
int main(){
	char a[100],b[100];
	strcpy(b,"where abc[21,252]='0000'");
	key_strcat(a,b);
	printf("a=%s\n",a);
	return(0);
}
****/
