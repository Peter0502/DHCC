drop index od_parm_1;
drop  table od_parm;
create table od_parm(
	prdt_no varchar2(3),
	title varchar2(30),
	beg_date number(9),
	end_date number(9),
	cif_type varchar2(1),
	cur_no varchar2(3),
	max_amt number(16,2),
	cash_amt number(16,2),
	turn_ill_days number(6),
	intst_ind varchar2(1),
	pay_ind varchar2(1),
	pay_date number(6),
	pay_intvl number(6),
	pay_drt_days number(6),
	hst_keep_day number(6),
	rate_code varchar2(3),
	rate_mode varchar2(1),
	rate_code_def varchar2(3),
	year_chrg number(16,2),
	min_pay_amt number(16,2),
	min_pay_ratio number(8,5),
	late_chrg_mode varchar2(1),
	late_chrg_code varchar2(3),
	opn_lmt number(6),
	cls_lmt number(6),
	dc_code varchar2(4) 
)tablespace users;
create	 unique index od_parm_1 on od_parm( prdt_no)tablespace indx;
