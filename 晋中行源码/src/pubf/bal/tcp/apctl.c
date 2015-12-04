#include "bal.h"
#include "pubf_data_code.h"

short  	time83_mvmacta();
short  	time81_mvtota();
int 	time18_putmsg( char *, int *, char*);
void 	msg_decode( char *string ) ;
void 	SetNowait_Release();
void 	ResetTimeout();
void 	_balDumpTITA();
void 	client_get_time(char*);
void 	bal_init_txcom();

short  time83_mvmacta()
{
	if(it_txcom.txrsut != TX_SUCCESS && it_txcom.txrsut != TX_RECURSIVE_LOOP)
		return(4);

    if(it_txcom.mttpfg < MACTA_LIMIT - 2)
	{
    	memcpy(&it_macta[it_txcom.mttpfg++], &it_mactw, (sizeof it_mactw));
		return(0);
	}

    if  (it_txcom.mttpfg == (MACTA_LIMIT - 2))
    {
    	memcpy(&it_macta[it_txcom.mttpfg++], &it_mactw, (sizeof it_mactw));
        return(1);
	}
    
    if  (it_txcom.mttpfg == (MACTA_LIMIT - 1))
    {
    	memcpy(&it_macta[it_txcom.mttpfg++], &it_mactw, (sizeof it_mactw));
		return(2);
	}
    
	return(3);
}

short  time81_mvtota()
{
   if ( ( it_txcom.txrsut != TX_SUCCESS ) &&
        ( it_txcom.txrsut != TX_RECURSIVE_LOOP ) )
   {
      if ( it_txcom.totbox < TOTA_LIMIT )
      {
         memcpy( &it_tota[it_txcom.totbox++], &it_totw, (sizeof it_totw) );
         return( 5 );
      }
      else
      {
         memcpy( &it_tota[TOTA_LIMIT - 1], &it_totw, (sizeof it_totw) );
         return( 4 );
      }
   }

   if ( it_txcom.totbox < ( TOTA_LIMIT - 2 ) )
   {
      memcpy( &it_tota[it_txcom.totbox++], &it_totw, (sizeof it_totw) );
      return( 0 );
   }

   if ( it_txcom.totbox == ( TOTA_LIMIT - 2 ) )
   {
      memcpy( &it_tota[it_txcom.totbox++], &it_totw, (sizeof it_totw) );
      return(1);
   }
   else
   {
      if ( it_txcom.totbox == ( TOTA_LIMIT - 1 ) )
      {
         memcpy( &it_tota[it_txcom.totbox++], &it_totw, (sizeof it_totw) );
         return(2);
      }
      else return(3);
   }
}


int time18_putmsg( char *response_buffer, int *size, char* result ) 
{
   int    total_leng = 0;
   int    leng = 0;
   int    idx = 0;
   int    idx_1 = 0;
   char   str[5];
   char   tota_buffer[3000];
   char   cTmpBuf[60];
   int rc, i;
	if (rc = pub_comm_crea_pack(RTBIT,tota_buffer,&total_leng))
	{
		printf("crea_pack() error\n");
		return (1);
	}
/***************************************
			if(1)
			{
     	    FILE *testfp;
     	    int     year, mon, day, hour, min, sec;
     	    char cFileName[100];     	    
     	    char cDirFileName[100]; 
     	    memset(cFileName, 0, sizeof(cFileName) );
     	    memset(cDirFileName, 0, sizeof(cDirFileName) );

     	    strcat(cDirFileName, "/home/newsys/tmpdat/dhccDecry"); 
     	     	    
          pub_base_get_date(&year, &mon, &day);
          pub_base_GetTime2(&hour, &min, &sec);

	        sprintf(cFileName,"%02d%02d%02d%02d%02d",mon,day,hour,min,sec);
     	    

    	    strcat(cDirFileName, cFileName);
     	    strcat(cDirFileName, ".tmp");
     	    
     	    vtcp_log("[%s][%d] FileName===[%s] \n",__FILE__,__LINE__, cDirFileName);
	        testfp=fopen(cDirFileName,"w");
          if(0 == testfp)
          {
            vtcp_log("[%s][%d] 打开文件出错 \n",__FILE__,__LINE__);
        	return -1;  
		}	
	        for(i=0; i<total_leng; i++)
	        {
	        	 fprintf(testfp,"%c", tota_buffer[i]);
	        }
	        
          fclose(testfp);          			
			}
			vtcp_log("[%s][%d] 保存文件成功 \n",__FILE__,__LINE__);


	vtcp_log("=============加密前数据===========\n");
	for(rc=0; rc<total_leng/50+1; rc++)
	{
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		for(i=0; i<50; i++)
		{
			if(tota_buffer[50*rc+i] == 0)  
			{
				cTmpBuf[i] = '@';
			}
			else
			{
				cTmpBuf[i] = tota_buffer[50*rc+i];
			}
		}
		vtcp_log("[%.50s]",cTmpBuf);
	}
**************************************************/

  /*********** Print For Test ********************/
    vtcp_log("========解密后 ========\n ");
    char cTmpPack[4001];
   int  iTst=0;
   memset(cTmpPack, 0, sizeof(cTmpPack));
   for(iTst=0; iTst<total_leng/50+1; iTst++ )
   {
      memset(cTmpPack, 0, sizeof(cTmpPack));
      for(i=0; i<50; i++)
      {
        if( tota_buffer[50*iTst+i] == 0 )
        {
            cTmpPack[i] = '@';
        }
        else
        {
          cTmpPack[i] = tota_buffer[50*iTst+i] ;
        }
      }
      vtcp_log("[%.50s]", cTmpPack);
  }
  /*********** Print For Test ********************/


	/* 20150624 BY WYW
	nt_Encrypt(tota_buffer,total_leng,BAL_COMM_PASSWD,1);
	***/

/****************问题出在这里!!!***********************
	strcpy(response_buffer,tota_buffer);
	加密后可能会出现==0的字符, 所以不能用 strcpy
******************************************************/
	memcpy(response_buffer,tota_buffer,total_leng); 
    /* Modified by ChenMing   2007.01.23.   */

/********************************************
		if(1)
		{
     	    FILE *testfp;
     	    int     year, mon, day, hour, min, sec;
     	    char cFileName[100];     	    
     	    char cDirFileName[100]; 
     	    memset(cFileName, 0, sizeof(cFileName) );
     	    memset(cDirFileName, 0, sizeof(cDirFileName) );

     	    strcat(cDirFileName, "/home/ceshiht/log"); 
     	     	    
          pub_base_get_date(&year, &mon, &day);
          pub_base_GetTime2(&hour, &min, &sec);

	        sprintf(cFileName,"%02d%02d%02d%02d%02d",mon,day,hour,min,sec);
     	    

    	    strcat(cDirFileName, cFileName);
     	    strcat(cDirFileName, ".tmp");
     	    
     	    vtcp_log("[%s][%d] FileName===[%s] \n",__FILE__,__LINE__, cDirFileName);
	        testfp=fopen(cDirFileName,"w");
          if(0 == testfp)
          {
            vtcp_log("[%s][%d] 打开文件出错 \n",__FILE__,__LINE__);
            return -1;
          }	
	        for(i=0; i<total_leng; i++)
	        {
	        	 fprintf(testfp,"%c", response_buffer[i]);
	        }
	        
          fclose(testfp);          			
			}
			vtcp_log("[%s][%d] 保存文件成功 \n",__FILE__,__LINE__);

    vtcp_log("leng=============[%d]", total_leng);
	nt_Encrypt(response_buffer,total_leng,BAL_COMM_PASSWD,0);

    vtcp_log("=============解密后数据===========\n");
    for(rc=0; rc<total_leng/50+1; rc++)
    {
	    memset(cTmpBuf, 0, sizeof(cTmpBuf));
  		for(i=0; i<50; i++)
	    {
   			if(tota_buffer[50*rc+i] == 0)
   			{
   				cTmpBuf[i] = '@';
   			}
            else
            {
                cTmpBuf[i] = response_buffer[50*rc+i];
            }
        }
        vtcp_log("[%.50s]",cTmpBuf);
    }

	nt_Encrypt(response_buffer,total_leng,BAL_COMM_PASSWD,1);

	****************************************/
	get_zd_data(DC_REPLY,RtCode);
	
	/***Z00?是授权的返回码****/
	if ( !memcmp(RtCode,"0000",4) || !memcmp(RtCode,"Z00",3))
   		*result = TX_SUCCESS;
	else 
   		*result = TX_REJECT;
	*size = total_leng;
	return(0);
}
void pub_set_trad_bit( char *bit_map )
{
	bit_map[0]='1';
	strcpy(RTBIT,bit_map);
}
void pub_set_trad_bit_one( int pos )
{
	RTBIT[pos]='1';
}

void msg_decode( char *string ) 
{
   return;
}

void _balDumpTITA()
{
    char   htrmseq[2+1];      /*  correction old trmseq*/
    char   hejfno[6+1];       /*  correction old ejfno*/
    char   opnbr[5+1];        /*  open account branch */
    char   kinbr[5+1];        /*  key in branch       */
    char   trmseq[2+1];       /*  terminal sequence   */
    char   ejfno[6+1];        /*  EJF no              */
    char   taskid[2+1];       /*  task id             */
    char   tmtype;            /*  terminal type       */
    char   aptype;            /*  application type    */
    char   txno[4+1];         /*  txno                */
    char   ptype;             /*  process type        */
    char   dscpt[3+1];        /*  description         */
    char   h_sts;             /*  cancel transaction  */
    char   actno[14+1];       /*  account no          */
    char   txtype[2+1];       /*  tx type             */
    char   multtx;            /*  MATCA tx            */
    char   crdb;              /*  crdb                */
    char   supinit1[1];       /*  supervisor1 code    */
    char   supinit2[1];       /*  supervisor1 code    */
    char   reason[6+1];       /*  reason code 2*3     */
    char   tlrno[2+1];        
    char   nbcd;              /*  nobook code         */
    char   trnmod;            /*  training mode       */
    char   sbtmod;            /*  subtitude mode      */
    char   curcd[3+1];        /*  currency code       */
    char   xcrt[8+1];         /*  currency rate       */
    char   txamt[12+1];       /*  transaction amount  */

    char   text[ TITA_TEXT_LENGTH + 1];

	int    i;

#ifdef DEBUG

    memcpy( htrmseq, TITA.htrmseq, 2 );
    htrmseq[2] = 0;

    memcpy( hejfno, TITA.hejfno, 6 );
    hejfno[6] = 0;

    memcpy( opnbr, TITA.opnbr, 7 );
    opnbr[5] = 0;

    memcpy( kinbr, TITA.kinbr, 7 );
    kinbr[5] = 0;

    memcpy( trmseq, TITA.trmseq, 2 );
    trmseq[2] = 0;

    memcpy( ejfno, TITA.ejfno, 6 );
    ejfno[6] = 0;

    memcpy( taskid, TITA.taskid, 2 );
    taskid[2] = 0;

    memcpy( txno, TITA.txno, 4 );
    txno[4] = 0;
 
    memcpy( dscpt, TITA.dscpt, 3 );
    dscpt[3] = 0;

    memcpy( actno, TITA.actno, 14 );
    actno[14] = 0;

    memcpy( txtype, TITA.txtype, 2 );
    txtype[2] = 0;

    memcpy( reason, TITA.reason, 6 );
    reason[6] = 0;

    memcpy( tlrno, TITA.tlrno, 2 );
    tlrno[2] = 0;

    memcpy( curcd, TITA.curcd, 3 );
    curcd[3] = 0;

    memcpy( xcrt, TITA.xcrt, 8 );
    xcrt[8] = 0;

    memcpy( txamt, TITA.txamt, 12 );
    txamt[12] = 0;
    
    printf( "...TITA..." );

    memcpy( text, it_tita.labtex.text, TITA_TEXT_LENGTH );
    text[TITA_TEXT_LENGTH] = 0;

	for( i = 0; i < TITA_TEXT_LENGTH; i++)
	{
		if( !isprint(text[i]) )
		{
			text[i] = '?';
		}
	}

#endif

}

void SetNowait_Release()
{
	return;
}

void ResetTimeout()
{
	return;
}

void client_get_time(place)
char *place;
{
    char   temp[10];
    struct timeb nowb;
    struct tm *now;

    ftime(&nowb);
    now = localtime(&nowb.time);
    sprintf(temp,"%02d%02d%02d%03hd",now->tm_hour,now->tm_min,now->tm_sec,
          nowb.millitm);
    memcpy(place,temp,9);

}

void bal_init_txcom()
{
#ifdef FOR_APCTL
	memset( &it_txcom, '0', sizeof(it_txcom) );
	it_txcom.totbox = 0;
	it_txcom.logbox = 0;
	it_txcom.mttpfg = 0;
	memcpy( it_txcom.tbsdy, g_date.tbday, sizeof(it_txcom.tbsdy) );
	memcpy( it_txcom.nbsdy, g_date.nbday, sizeof(it_txcom.nbsdy) );
	memcpy( it_txcom.nextdy, g_date.nrday, sizeof(it_txcom.nextdy) );
	bal_gettime( it_txcom.txtime );
#endif
}

