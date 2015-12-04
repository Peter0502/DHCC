drop  table card_consum_lmt;
create table card_consum_lmt(
	ac_no varchar2(19),
	da_cash_lmt number(16,2),
	da_amt_lmt number(16,2),
	da_con_lmt number(16,2),
	od_cash_lmt number(16,2),
	od_amt_lmt number(16,2),
	mo_cash_lmt number(16,2),
	mo_amt_lmt number(16,2),
	mo_con_lmt number(16,2),
	da_all_cash_lmt number(16,2),
	da_all_amt_lmt number(16,2),
	da_all_con_lmt number(16,2),
	mo_all_cash_lmt number(16,2),
	mo_all_amt_lmt number(16,2),
	mo_all_con_lmt number(16,2) 
)tablespace users;
