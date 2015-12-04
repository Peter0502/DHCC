drop  table mo_dc_inf;
create table mo_dc_inf(
	acc_no_lvl1 varchar2(7),
	acc_name_lvl1 varchar2(30),
	acc_no_lvl2 varchar2(7),
	acc_name_lvl2 varchar2(30),
	acc_no_lvl3 varchar2(7),
	acc_name_lvl3 varchar2(30),
	dr_cash_amt number(16,2),
	dr_cash_cnt number(9),
	dr_ch_amt number(16,2),
	dr_ch_cnt number(9),
	dc_cash_amt number(16,2),
	dc_cash_cnt number(9),
	dc_ch_amt number(16,2),
	dc_ch_cnt number(9),
	sts varchar2(1),
	tel varchar2(4),
	br_no varchar2(5) 
)tablespace users;
