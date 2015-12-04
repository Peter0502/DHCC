drop index mo_jl_bal_1;
drop  table mo_jl_bal;
create table mo_jl_bal(
	ac_id number(9),
	ac_seqn number(6),
	beg_date number(9),
	end_date number(9),
	bal number(16,2) 
)tablespace users;
create	 unique index mo_jl_bal_1 on mo_jl_bal( ac_id,ac_seqn,end_date)tablespace indx;
