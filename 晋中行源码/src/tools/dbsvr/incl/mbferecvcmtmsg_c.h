#ifndef __mbferecvcmtmsg_CH__
#define __mbferecvcmtmsg_CH__
struct mbferecvcmtmsg_c{
	char      rowid[20];
	long      cmt_len;
	char      cmt_use[2];
	char      srflag[2];
	char      oper_type[2];
	char      oper_syscode[4];
	char      comsno[9];
	char      respno[21];
	char      sendtime[15];
	char      cmtno[4];
	long      cnapsdate;
	char      recvtime[15];
};
#endif 
