drop index march_mst_mx_hst_1;
drop  table march_mst_mx_hst;
create table march_mst_mx_hst(
	area_code varchar2(4),
	march_code varchar2(15),
	tty_no varchar2(8),
	tx_time number(9),
	tx_date number(9),
	brf varchar2(20),
	trace_no number(9),
	card_no varchar2(19),
	opn_tx_br_name varchar2(40),
	tx_amt number(16,2) 
)tablespace users;
create	 index march_mst_mx_hst_1 on march_mst_mx_hst( march_code,tx_date,trace_no)tablespace indx;
