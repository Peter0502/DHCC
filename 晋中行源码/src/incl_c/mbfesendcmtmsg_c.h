#ifndef __mbfesendcmtmsg_CH__
#define __mbfesendcmtmsg_CH__
struct mbfesendcmtmsg_c{
	char      rowid[20];
	char      oper_type[2];
	char      comsno[9];
	char      respno[21];
	char      sendtime[15];
	char      cmtno[4];
	long      cnapsdate;
	char      msssno[9];
	char      bkcode[13];
	long      origrtnappdate;
};
#endif 
