#ifndef __mq_acct_no_CH__
#define __mq_acct_no_CH__
struct mq_acct_no_c{
	char            payee_bankno[13];
        char            payeeorg_code[13];
        char            payee_acct[33];
        char            payee_name[61];
        char            ac_no[21];
};
#endif 
