drop  index mobile_time_idx;
drop  table mobile_time;
create table mobile_time(
	name varchar2(20),
	time varchar2(6)
)tablespace users;
create unique index mobile_time_idx on mobile_time(name) tablespace indx;
