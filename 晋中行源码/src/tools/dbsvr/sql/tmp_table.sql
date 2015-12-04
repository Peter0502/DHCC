drop index tmp_table_idx;
drop  table tmp_table;
create table tmp_table(
	ac_id number(9),
	ac_seqn number(6),
	bz varchar2(1) 
)tablespace users;
create	 unique index tmp_table_idx on tmp_table( ac_id,ac_seqn)tablespace indx;
