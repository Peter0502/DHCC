drop  table chnl_tradeadd;
create table chnl_tradeadd(
	tx_date number(9),
	ac_no varchar2(20),
	tx_type varchar2(1),
	add_amt number(16,2),
	add_qk_cnt number(9),
	add_pswd_cnt number(9),
	stat varchar2(1) 
)tablespace users;
