drop  table mob_recvmail;
create table mob_recvmail(
	tx_date number(9),
	tx_time number(9),
	opt_code varchar2(3),
	mobile varchar2(15),
	message varchar2(140),
	proc_flag varchar2(1) 
)tablespace users;
