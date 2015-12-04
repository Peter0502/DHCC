drop  table mas_hvcd;
create table mas_hvcd(
	trace_no varchar2(32),
	payficcode varchar2(12),
	payfictype varchar2(1),
	payer_name varchar2(60),
	payer_ac_no varchar2(40),
	payer_br_no varchar2(20),
	payer_type varchar2(2),
	payer_id_type varchar2(2),
	payer_id_no varchar2(20),
	payee_name varchar2(60),
	payee_type varchar2(2),
	payee_id_type varchar2(2),
	payee_id_no varchar2(20),
	cur_no varchar2(3),
	tx_amt varchar2(15),
	paydate varchar2(8),
	use varchar2(20),
	sttl_type varchar2(2),
	txcode varchar2(10),
	extend varchar2(20) 
)tablespace users;
