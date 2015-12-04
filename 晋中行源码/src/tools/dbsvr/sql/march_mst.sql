drop index march_mst_1;
drop  table march_mst;
create table march_mst(
	area_code varchar2(4),
	march_code varchar2(15),
	ac_no varchar2(20),
	march_name varchar2(40),
	tx_date number(9),
	tx_cnt number(9),
	tx_amt number(16,2),
	fee_amt number(16,2),
	fee_rate number(12,6),
	real_amt number(16,2),
	acct_date number(9),
	deal_sign varchar2(1),
	deal_msg varchar2(40),
	opn_br_no varchar2(5), 
	tt_no varchar2(4)
)tablespace users;
create	 unique index march_mst_1 on march_mst( march_code,tx_date)tablespace indx;
