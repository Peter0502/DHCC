drop index auth_ac_rec_1;
drop  table auth_ac_rec;
create table auth_ac_rec(
	ac_id number(9),
	ac_seqn number(6),
	dc varchar2(1),
	ct varchar2(1),
	ind varchar2(4),
	tx_code varchar2(4),
	amt number(16,2) 
)tablespace users;
create	 unique index auth_ac_rec_1 on auth_ac_rec( ac_id,ac_seqn,dc,ct)tablespace indx;
