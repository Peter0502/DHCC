drop index ac_ac_rel_1;
drop  table ac_ac_rel;
create table ac_ac_rel(
	ac_id number(9),
	ac_id1 number(9)
)tablespace users;
create unique index ac_ac_rel_1 on ac_ac_rel(ac_id)tablespace indx;