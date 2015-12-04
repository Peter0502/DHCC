drop  table mo_city_query;
create table mo_city_query(
	tx_date number(9),
	tx_no number(9),
	tx_br_no varchar2(5),
	tel varchar2(4),
	rec_br_no varchar2(5),
	type varchar2(1),
	note_date number(9),
	note_type varchar2(1),
	tx_amt number(16,2),
	payer_ac_no varchar2(24),
	payer_name varchar2(50),
	payer_br_name varchar2(50),
	payee_ac_no varchar2(24),
	payee_name varchar2(50),
	payee_br_name varchar2(50),
	content varchar2(50),
	sts varchar2(1) 
)tablespace users;
