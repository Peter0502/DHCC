drop index mo_city_1;
drop  table mo_city;
create table mo_city(
	tx_date number(9),
	tx_no number(9),
	tx_br_no varchar2(5),
	tel varchar2(4),
	trans_mode varchar2(1),
	bat_no number(6),
	no number(6),
	in_out_ind varchar2(1),
	cd_ind varchar2(1),
	bill_type varchar2(3),
	bill_no varchar2(16),
	payer_ac_no varchar2(24),
	payer_name varchar2(50),
	payer_br_no varchar2(5),
	payer_br_name varchar2(40),
	payee_ac_no varchar2(24),
	payee_name varchar2(50),
	payee_br_no varchar2(5),
	payee_br_name varchar2(40),
	sign_date number(9),
	matr_date number(9),
	bill_amt number(16,2),
	sts varchar2(1),
	pr_cnt number(9),
	tc_date number(9),
	brf varchar2(50),
	remark varchar2(50) 
)tablespace users;
create	 index mo_city_1 on mo_city( tx_br_no,bat_no,tel,trans_mode,in_out_ind)tablespace indx;
