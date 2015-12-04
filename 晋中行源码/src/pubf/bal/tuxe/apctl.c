#include "bal.h"

short  	cs83_mvmacta();
short  	cs81_mvtota();
int 	cs18_putmsg( char *, int *, char*);
void 	msg_decode( char *string ) ;
void 	SetNowait_Release();
void 	ResetTimeout();
void 	_balDumpTITA();
void 	client_get_time(char*);
void 	bal_init_txcom();

short  cs83_mvmacta()
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

short  cs81_mvtota()
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

int cs18_putmsg( char *response_buffer, int *size, char* result ) 
{
   int    total_leng = 0;
   int    leng = 0;
   int    idx = 0;
   int    idx_1 = 0;
   char   str[5];
   char   tota_buffer[3000];
   int rc;
	
	if (data_mode == TITA_MODE){ 
   		idx_1 = it_txcom.totbox - 1;
   		for ( idx = 0; idx < it_txcom.totbox; idx++ )
   		{
      		if ( ( it_txcom.txrsut != TX_SUCCESS ))
      		{
         		idx = it_txcom.totbox - 1;
      		}
      		memset(tota_buffer,'\0',(sizeof tota_buffer));
      		memset(str,'\0',(sizeof str));
      		memcpy(str,it_tota[idx].labtex.label.msglng,3);
      		leng = atoi(str);

      		memcpy(tota_buffer,&it_tota[idx],leng);
      		memcpy(&response_buffer[total_leng],tota_buffer,leng);
      		total_leng+=leng;
   		}
		userlog("In TOTW"); 
	   *size = total_leng;
	   *result = it_txcom.txrsut;
	}
   	else {
		if (data_mode != MODE_8583_M){
			if (rc = pub_comm_crea_pack(RTBIT,tota_buffer,&total_leng))
			{
				   userlog("crea_pack() error\n");
			   		return (1);
	  		}
	  		nt_Encrypt(tota_buffer,total_leng,BAL_COMM_PASSWD,1);
			/****Modified by SSH,2006.8.23,显然不能用strcpy****/
	  		memcpy(response_buffer,tota_buffer,total_leng);
			/****Modifed by SSH,2006.8.22,应答码39->12,授权码错提交***/
			
	  		get_zd_data("0120",RtCode);
	  		if (!memcmp(RtCode,"0000",4) || !memcmp(RtCode,"Z00",3))
   	   			*result = TX_SUCCESS;
	  		else 
   	   			*result = TX_REJECT;
	  		*size = total_leng;
		}
		else{
		}
	}
	return(0);
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
    char   hcode;             /*  cancel transaction  */
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
    
    userlog( "...TITA..." );

    userlog( "htrmseq:[%s], hejfno:[%s], opnbr:[%s], kinbr:[%s], trmseq:[%s], \
ejfno:[%s], taskid:[%s], tmtype:[%c], aptype:[%c], txno:[%s], ptype:[%c], \
dscpt:[%s], hcode:[%c], actno:[%s], txtype:[%s], multtx:[%c], crdb:[%c], \
supinit1:[%c], supinit2:[%c], reason:[%s], tlrno:[%s], nbcd:[%c], trnmod:[%c], \
sbtmod:[%c], curcd:[%s], xcrt:[%s], txamt:[%s]",
              htrmseq, hejfno, opnbr, kinbr, trmseq, ejfno, taskid, 
              TITA.tmtype, TITA.aptype, txno, TITA.ptype, dscpt, 
              TITA.hcode, actno, txtype, TITA.multtx, TITA.crdb,
              *(TITA.supinit1), *(TITA.supinit2), reason, tlrno, 
              TITA.nbcd, TITA.trnmod, TITA.sbtmod, curcd, xcrt, txamt );

    memcpy( text, it_tita.labtex.text, TITA_TEXT_LENGTH );
    text[TITA_TEXT_LENGTH] = 0;

	for( i = 0; i < TITA_TEXT_LENGTH; i++)
	{
		if( !isprint(text[i]) )
		{
			text[i] = '?';
		}
	}

    userlog( "text:[%s]", text );

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
/********************从TCP中拷贝出来的,为什么没有????***********/
void pub_set_trad_bit_one( int pos )
{
	RTBIT[pos]='1';
}
void pub_set_trad_bit( char *bit_map )
{
	bit_map[0]='1';
	strcpy(RTBIT,bit_map);
}

