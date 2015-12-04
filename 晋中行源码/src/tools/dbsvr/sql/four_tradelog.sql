drop  table four_tradelog;
create table four_tradelog(
	tx_date number(9),
	tx_time number(9),
	tx_code varchar2(4),
	sl_bank varchar2(5),
	tx_tel varchar2(4),
	city_no varchar2(4),
	chang_no number(9),
	cur_no varchar2(2),
	trace_no number(9),
	ac_no varchar2(19),
	name varchar2(50),
	tx_amt number(16,2),
	bal number(16,2),
	sx_amt number(16,2),
	sx_rate number(8,5),
	cancle_trcode varchar2(4),
	cancle_trace_no number(9),
	old_tel varchar2(4),
	sl_ckmk varchar2(1),
	fk_ckmk varchar2(1),
	tx_stat varchar2(1) 
)tablespace users;
