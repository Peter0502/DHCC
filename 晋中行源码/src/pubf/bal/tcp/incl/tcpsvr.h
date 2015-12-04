/********************************************************************

	vtcpsvr.h -- vost TCPIP server include file.

	Notes:	1. Space of <pack> in TcpRcv should be allocated by caller.
		2. <rcvfiles> and <sndfiles> can be NULL or "" or others.
		3. If <rcvfiles> is not NULL, the received filenames is returned into it.

********************************************************************/

#ifndef _VTCPSVR_H_
#define _VTCPSVR_H_

#define DEF_PORTNO_RC	6500
#define DEF_PORTNO_RZ	6400
#define DEF_TIMEOUT		24000			/* timeout of each read */
#define DEF_LISTENLEN	10				/* listen buffer len 	*/	

void vtcp_log  ( );
int  tcp_write ( int sockid, char *buf, int len, int timeout );
int  tcp_read  ( int sockid, char *buf, int len, int timeout );

int  TcpRcv    ( int sockid, char *pack, int *packlen, char *rcvfiles );
int  TcpSnd    ( int sockid, char *pack, int packlen,  char *sndfiles );

#endif

#define  PUBJGBM   "00999"                        /*ÐéÄâ»ú¹¹´úÂë*/
#define  PUBGYBM   "999"                          /*ÐéÄâ¹ñÔ±´úÂë*/
#define  PUBTTY    "VT"                           /*ÐéÄâÖÕ¶Ë*/

