
int nt_Encrypt(char *,int,char *,int);
int nt_Encrypt(char *string,int len,char *key,int flag)
{
  	register i;
	int klen;


	klen=strlen(key);

	for(i=0;i<len;i++)
	{
		if (flag) 
			string[i]=string[i]+key[i%klen];
	    else 
			string[i]=string[i]-key[i%klen];

	}
    return 0;
}

