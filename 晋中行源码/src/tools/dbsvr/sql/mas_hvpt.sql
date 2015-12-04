drop  table mas_hvpt;
create table mas_hvpt(
	trace_no varchar2(32),
	payficcode varchar2(12),
	payfictype varchar2(1),
	payer_name varchar2(60),
	payer_ac_no varchar2(40),
	payer_br_no varchar2(20),
	origrdficode varchar2(12),
	origrdfictype varchar2(1),
	payee_name varchar2(60),
	payee_ac_no varchar2(40),
	payee_br_no varchar2(20),
	cur_no varchar2(3),
	tx_amt varchar2(15),
	paydate varchar2(8),
	recvdate varchar2(8),
	use varchar2(20),
	sttl_type varchar2(2),
	notecode varchar2(32),
	txcode varchar2(10),
	extend varchar2(20) 
)tablespace users;
