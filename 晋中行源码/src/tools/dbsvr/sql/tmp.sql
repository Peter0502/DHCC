drop index tmp_1;
drop table tmp;
create table tmp(
      ac_no varchar2(19),
      ac_name varchar2(50)
)tablespace users;
create unique index tmp_1 on tmp(ac_no)tablespace indx;
