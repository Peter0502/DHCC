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
	if(substr==NULL){
		return(mainstr);
	}	
	ptr=get_word(substr);
	while(ptr!=NULL){
		if(strcmp("date",ptr)==0||
			strcmp("group",ptr)==0||
			strcmp("number",ptr)==0) {
			/****�滻�ؼ���****/
			strcat(mainstr,"\"");
			strcat(mainstr,ptr);
			strcat(mainstr,"\"");
		}else if(ptr[0]=='['){
			/****�滻[a,b]Ϊsubstr(s,a,b)�ķ�ʽ****/
			char *pcLastWord;
			int len;
			pcLastWord=get_lastword(mainstr);
			len=strlen(pcLastWord);
			sprintf(mainstr+strlen(mainstr)-len,"substr(%s,",pcLastWord);
		}else if(ptr[0]==']'){
			/***����[a]���������***/
			/**��ǰ�Ҷ���,������ҵ�����֮ǰ�ҵ���1������Ҫ����,2�����账��**/
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
				/****informix�ڶ�����������ֹλ��,��ORACLE�ǳ���****/
				/****��ʱ,num1����ʼλ��,num2����ֹλ��***/	
				id=strlen(mainstr)-1;
				while(mainstr[id]!=','){
					id--;
				}
				sprintf(mainstr+id+1,"%d",num2-num1+1);
				strcat(mainstr,")");
			}else{
				strcat(mainstr,",1)");
			}
				
		}else{
			strcat(mainstr,ptr);
		}
		ptr=get_word(NULL);
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
