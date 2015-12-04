drop  table mas_hvcs;
create table mas_hvcs(
	trace_no varchar2(32),
	payer_name varchar2(60),
	payer_id_type varchar2(2),
	payer_id_no varchar2(20),
	origrdficode varchar2(12),
	origrdfictype varchar2(1),
	payee_name varchar2(60),
	payee_ac_no varchar2(40),
	ayee_type varchar2(2),
	payee_br_no varchar2(20),
	payee_id_type varchar2(2),
	payee_id_no varchar2(20),
	cur_no varchar2(3),
	tx_amt varchar2(15),
	recvdate varchar2(8),
	cash_source varchar2(20),
	sttl_type varchar2(2),
	txcode varchar2(10),
	extend varchar2(20) 
)tablespace users;
