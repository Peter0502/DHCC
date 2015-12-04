#include "bal.h"
#include "pubf_data_code.h"

int bal_conv_in(T_BTR*);
int bal3_conv_in(T_BTROLD*);
int bal31_conv_in(T_BTR*);

int bal_conv_in(T_BTR *bal_req)
{
	return bal31_conv_in(bal_req);
}

int bal31_conv_in( T_BTR *bal_req )
{
	char str_len[20];
	int  int_len;
	int  rc;
	char rcvpack[3000];
	char c_txno[5],c_kinbr[9],c_tlrno[4];
	long tlsrno;
	char c_kinbr_tlsrno[7];

	memcpy(RtCode,"0000",4);
	memset(str_len, 0, 20);
	memcpy(str_len, bal_req->head.len, CSLData);
	int_len = atoi(str_len);
	rc = sysinit();
	if (rc<0)
	{
		sprintf(RtCode,"%4d",rc);
		printf("err sysinit\n");
		return(1);
	}
	memcpy(rcvpack,bal_req->data,int_len);

	rcvpack[int_len] = '\0';
	/* 20150624 by wyw
	nt_Encrypt(rcvpack,int_len,BAL_COMM_PASSWD,0);
	*/
  /*********** Print For Test ********************/  
	vtcp_log("========Ω‚√‹∫Û ========\n ");      
	char cTmpPack[4001];
   int  iTst=0; 
   int  i=0; 
   memset(cTmpPack, 0, sizeof(cTmpPack));
   for(iTst=0; iTst<int_len/50+1; iTst++ )
   {  	
      memset(cTmpPack, 0, sizeof(cTmpPack));    
      for(i=0; i<50; i++)
      {
        if( rcvpack[50*iTst+i] == 0 )   
  	    {    	 
 			cTmpPack[i] = '@';  	  
  	    }   
 	    else    
	    {    
	  	  cTmpPack[i] = rcvpack[50*iTst+i] ;   
 	    }
      }
      vtcp_log("[%.50s]", cTmpPack);
  }
  /*********** Print For Test ********************/


    if(rc=pub_comm_chan_pack(rcvpack))
	{
		vtcp_log("err chan_pack");
		return(1);
	}
	vtcp_log("==================");
	if(get_trad(RTBIT))
	{
		vtcp_log("err get_trad");
		return(1);
	}
	if(tx_chk())
	{
		vtcp_log("err tx_chk");
		return(1);
	}

	bal_init_txcom();
	get_zd_data(DC_TX_CODE,c_txno);
	get_zd_data(DC_TEL,c_tlrno);
	get_zd_data(DC_TX_BR_NO,c_kinbr);

	/*********** Print For Test ********************/
    char cTmpStr[20];
	memset(cTmpStr, 0, sizeof(cTmpStr));
	get_zd_data("0790", cTmpStr);
	vtcp_log(" ChenMing  Acno====[%s]", cTmpStr);
	memset(cTmpStr, 0, sizeof(cTmpStr));
	get_zd_data("0800", cTmpStr);
	vtcp_log(" ChenMing  Acno====[%s]", cTmpStr);
	/*********** Print For Test ********************/
	
	memcpy(g_balinfo.txno,c_txno,CSLTxNo);
	memcpy(g_balinfo.tlrno, c_tlrno, CSLTlrNo);

	it_txcom.txno  = atoi(c_txno);
	memcpy(it_txcom.kinbr,c_kinbr,CSLKinBr);
	it_txcom.kinbr[CSLKinBr] = '\0';
	memcpy(it_txcom.tlrno,c_tlrno,CSLTlrNo);
	it_txcom.tlrno[CSLTlrNo] = '\0';
	it_txcom.tlsrno = 0 ;
	
	if( BAL_SVRREQ_NOWAIT == bal_req->head.reqtype )
	{
		it_txcom.response_id = bal_get_responseid(bal_req->head.cbs);
	}
	else
	{
		it_txcom.response_id = 0;
	}

	return CSuccess;
}

int bal3_conv_in( T_BTROLD *bal_req )
{
	char str_len[20];
	int  int_len;

#ifdef FOR_APCTL

	memset(str_len, 0, 20);
	memcpy(str_len, bal_req->head.len, 4);
	int_len = atoi(str_len);
	
	memset(&it_tita, 0, sizeof(it_tita));
	memcpy( &it_tita, bal_req->data, int_len );

	memcpy(g_balinfo.txno, TITA.txno, CSLTxNo);
	g_balinfo.txno[CSLTxNo] = '\0';
	memcpy(g_balinfo.tlrno, TITA.tlrno, CSLTlrNo);
	g_balinfo.tlrno[CSLTlrNo] = '\0';

	bal_init_txcom();
	
	it_txcom.response_id = 0;
	memset( &it_totw, ' ', sizeof(it_totw) );

#endif
	return CSuccess;
}
