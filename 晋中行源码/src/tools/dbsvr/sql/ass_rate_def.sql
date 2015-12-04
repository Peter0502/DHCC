drop  table ass_rate_def;
create table ass_rate_def(
	rate_code varchar2(3),
	seqn number(6),
	cond_ind varchar2(1),
	min_bal number(16,2),
	max_bal number(16,2),
	rate number(8,5) 
)tablespace users;
