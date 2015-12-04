#ifndef __prdt_prot_parm_CH__
#define __prdt_prot_parm_CH__
struct prdt_prot_parm_c{
	char      rowid[20];
	char      prot_type[4];
	char      reg_fun[11];
	long      deal_seq;
	char      main_prdt_code[4];
	char      app_prdt_code[4];
	char      turn_mode[2];
	char      turn_term_type[2];
	char      turn_cond[11];
	char      turn_amt_def[11];
	char      deal_func[11];
	double    min_bal;
	double    min_amt;
	double    turn_unit;
	long      end_date;
};
#endif 
