drop  table chnl_tradelimit;
create table chnl_tradelimit(
	chnl varchar2(1),
	pwd_cnt number(9),
	wd_cnt number(9),
	wd_amt number(16,2),
	wd_ttl_max_amt number(16,2) 
)tablespace users;
