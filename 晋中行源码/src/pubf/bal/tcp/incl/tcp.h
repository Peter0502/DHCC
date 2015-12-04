/********************************************************************

	vtcpsvr.h -- vost TCPIP server include file.

	Notes:	1. Space of <pack> in TcpRcv should be allocated by caller.
		2. <rcvfiles> and <sndfiles> can be NULL or "" or others.
		3. If <rcvfiles> is not NULL, the received filenames is returned into it.

********************************************************************/

/* client identifier structure */
struct clientid_t {
	long    clientdata[4];      /* reserved for internal use */
};
typedef struct clientid_t CLIENTID;

struct tpsvcinfo {
	char    name[32];/* service name invoked */
	long    flags;      /* describes service attributes */
	char    data[3100];      /* pointer to data */
	long    len;        /* request data length */
	int		cd;     /* reserved for future use */
	long    appkey;     /* application authentication client key */
	CLIENTID cltid;     /* client identifier for originating client */
};
typedef struct tpsvcinfo TPSVCINFO;

