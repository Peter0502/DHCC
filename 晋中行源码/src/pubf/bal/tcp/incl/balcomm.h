#ifndef _BALCOMM_H
#define _BALCOMM_H

typedef struct {
	char sender[CSLSender + 1];
	char rqst[CSLRqst + 1];
	char data[CSLRqstData + 1];
} T_CommRqst;

typedef struct {
	char sender[CSLSender + 1];
	char rqst[CSLRqst + 1];
	char replylen[CSLRplyLen + 1];
	char rpl_ind[1];
} T_CommRply;

#endif
