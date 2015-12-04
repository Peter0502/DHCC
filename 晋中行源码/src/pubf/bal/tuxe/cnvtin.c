#include "bal.h"

int bal_conv_in(T_BTR*);
int bal3_conv_in(T_BTROLD*);
int bal31_conv_in(T_BTR*);

int bal_conv_in(T_BTR *bal_req)
{
	userlog("%d:ERROR: fseek - %s", __LINE__);
/*
	if( g_balinfo.reqversion == BAL3_VERSION )
	{
		return bal3_conv_in((T_BTROLD*)bal_req);
	}
	else
	{
		return bal31_conv_in( bal_req);
	}
*/
	userlog("%d:ERROR: fseek - %s", __LINE__);
	return bal31_conv_in(bal_req);
	userlog("%d:ERROR: fseek - %s", __LINE__);
}

int bal31_conv_in( T_BTR *bal_req )
{
	char str_len[20];
	int  int_len;
	int  rc;
	char rcvpack[3000];
	char c_txno[10],c_kinbr[10],c_tlrno[10];
	long tlsrno;
	char c_kinbr_tlsrno[7];
	char tmpbuf[10];
	memcpy(RtCode,"0000",4);
	memset(str_len, 0, 20);
	memcpy(str_len, bal_req->head.len, CSLData);
	int_len = atoi(str_len);
	bal_init_txcom();
	data_mode =  bal_req->head.TITA_OR_8583;
	if (data_mode != TITA_MODE){  /*中联数据格式*/
		if (data_mode != MODE_8583_M){
			rc = sysinit();
			if (rc<0)
			{
				sprintf(RtCode,"%4d",rc);
				userlog("err sysinit");
				return(1);
			}
			memcpy(rcvpack,bal_req->data,int_len);

			rcvpack[int_len] = '\0';
			vtcp_log("%s,%d,Gethere,recvpack1=\n[%s]\n",__FILE__,__LINE__,rcvpack);
			nt_Encrypt(rcvpack,int_len,BAL_COMM_PASSWD,0);
			vtcp_log("%s,%d,Gethere,recvpack2=\n[%s]\n",__FILE__,__LINE__,rcvpack);

 	   		if(rc=pub_comm_chan_pack(rcvpack))
			{
				userlog("%s,%d,err chan_pack",__FILE__,__LINE__);
				return(1);
			}
			if(get_trad(RTBIT)){
		
				userlog("err get_trad");
				return(1);
			}
			/**ADD BY ZHGF 20100519如果tx_chk返回的是9则说明数据库报其他错了,杀掉该进程 
			if(tx_chk())
			{
				userlog("err tx_chk");
				return(1);
			}*20100519*******/
			rc=tx_chk();
			if(rc==9){
				userlog("err tx_chk 又出现tx_chk错误了errno[%d]",errno);
				userlog("err tx_chk 又出现错误了,上次是oracle报1002错误，这次是errno[%d]",errno);
				userlog("err tx_chk 又出现tx_chk错误了errno[%d]",errno);
				exit(1);
			}else if (rc){
				userlog("err tx_chk");
				return(1);
			}
			/**Modified by SSH,2006.8.22,一些域的取法为什么不同**/
			/***好像是按照真正的8583来的,哈哈****/
			/****交易码3->16,操作员16->7,交易所33->3,前台流水无**/	
			get_zd_data("0160",c_txno);
			vtcp_log("%s,%d,Gethere,0030=[%s]\n",__FILE__,__LINE__,c_txno);
			get_zd_data("0070",c_tlrno);
			vtcp_log("%s,%d,Gethere,0160=[%s]\n",__FILE__,__LINE__,c_tlrno);
			get_zd_data("0030",c_kinbr);
			/****
			get_zd_data("0110",c_kinbr_tlsrno);
			****/
			memset(c_kinbr_tlsrno,'\0',sizeof(c_kinbr_tlsrno));
			
			memcpy(g_balinfo.txno,c_txno,CSLTxNo);
			memcpy(g_balinfo.tlrno, c_tlrno, CSLTlrNo);

			it_txcom.txno  = atoi(c_txno);
			memcpy(it_txcom.kinbr,c_kinbr,CSLKinBr);
			it_txcom.kinbr[CSLKinBr] = '\0';
			memcpy(it_txcom.tlrno,c_tlrno,CSLTlrNo);
			it_txcom.tlrno[CSLTlrNo] = '\0';
			it_txcom.tlsrno = 0;
		}
		else{ /* 中间业务*/
/*
			rc =ZInit(0);
			if (rc!=0)
			{
				sprintf(RtCode,"%4d",rc);
				userlog("err sysinit");
				return(1);
			}
			memcpy(rcvpack,bal_req->data,int_len);

			rcvpack[int_len] = '\0';
 	   		if(rc=ZSpanPack(rcvpack))
			{
				sprintf(RtCode,"%4d",rc);
				userlog("%s,%d,err chan_pack",__FILE__,__LINE__);
				return(1);
			}
*/
		}
	}
	else   /*荣电数据格式*/
	{
		memset(&it_tita, 0, sizeof(it_tita));
#ifdef APCTLHDR
		memcpy( &it_tita, bal_req->data, int_len );
#else
		memcpy( &it_tita.labtex, bal_req->data, int_len );
#endif	
		userlog("bal_req->data[%s]",bal_req->data);
		memcpy(g_balinfo.txno, TITA.txno, CSLTxNo);
		g_balinfo.txno[CSLTxNo] = '\0';
		memcpy(g_balinfo.tlrno, TITA.tlrno, CSLTlrNo);
		g_balinfo.tlrno[CSLTlrNo] = '\0';

		memcpy(c_txno,TITA.txno,4);
		c_txno[4] = '\0';
		it_txcom.txno  = atoi(c_txno);
		memcpy(it_txcom.kinbr,TITA.kinbr,CSLKinBr);
		it_txcom.kinbr[CSLKinBr] = '\0';
/*
		memcpy(it_txcom.tlrno,c_tlrno,CSLTlrNo);
		it_txcom.tlrno[CSLTlrNo] = '\0';
*/
		it_txcom.tlsrno = 0;
	}
	
	if( BAL_SVRREQ_NOWAIT == bal_req->head.reqtype )
	{
		it_txcom.response_id = bal_get_responseid(bal_req->head.cbs);
	}
	else
	{
		it_txcom.response_id = 0;
	}
 
	memset( &it_totw, ' ', sizeof(it_totw) );
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
