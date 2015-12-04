drop index auth_code_1;
drop  table auth_code;
create table auth_code(
	br_no varchar2(5),
	auth_no number(9),
	trace_no number(9),
	done_trace_no number(9),
	tx_code varchar2(4),
	cond_seqn number(6),
	lvl varchar2(10),
	auth_ind varchar2(1),
	sts varchar2(1),
	tel varchar2(4),
	auth varchar2(4),
	aud varchar2(4),
	chk varchar2(4) 
)tablespace users;
create	 unique index auth_code_1 on auth_code( auth_no)tablespace indx;
