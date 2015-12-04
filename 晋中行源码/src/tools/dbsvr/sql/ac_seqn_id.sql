drop index ac_seqn_id_1;
drop  table ac_seqn_id;
create table ac_seqn_id(
	ac_id number(9),
	ac_seqn_id number(6) 
)tablespace users;
create	 unique index ac_seqn_id_1 on ac_seqn_id( ac_id)tablespace indx;
