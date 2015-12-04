drop  table mbfe_cmt_fmt;
create table mbfe_cmt_fmt(
	cmtno varchar2(3),
	cmt_name varchar2(20),
	head_flag varchar2(2),
	oper_type varchar2(1),
	send_period varchar2(5),
	regtable varchar2(20),
	od_tx_code varchar2(4),
	rd_tx_code varchar2(4) 
)tablespace users;
