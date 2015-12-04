drop  table prdt_prot_parm;
create table prdt_prot_parm(
	prot_type varchar2(3),
	reg_fun varchar2(10),
	deal_seq number(6),
	main_prdt_code varchar2(3),
	app_prdt_code varchar2(3),
	turn_mode varchar2(1),
	turn_term_type varchar2(1),
	turn_cond varchar2(10),
	turn_amt_def varchar2(10),
	deal_func varchar2(10),
	min_bal number(16,2),
	min_amt number(16,2),
	turn_unit number(16,2),
	end_date number(9) 
)tablespace users;
