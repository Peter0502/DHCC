drop  table hv_chkmrhis;
create table hv_chkmrhis(
	tx_date number(9),
	ccpc_no varchar2(12),
	send_cnt number(9),
	send_amt number(22,2),
	send_item number(9),
	send_dtl varchar2(352),
	recevice_cnt number(9),
	recevice_amt number(22,2),
	recevice_item number(9),
	recevice_dtl varchar2(352),
	online_cnt number(9),
	online_amt number(22,2),
	online_item number(9),
	online_dtl varchar2(192) 
)tablespace users;
