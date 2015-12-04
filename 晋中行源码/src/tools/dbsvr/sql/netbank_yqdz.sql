drop index netbank_yqdz_1;
drop table netbank_yqdz; 
create table netbank_yqdz
( 
      ac_id number(9),  
      sts varchar(1),
      sign_date number(8),
      cancel_date number(8)
)tablespace users;
create unique index netbank_yqdz_1 on netbank_yqdz( ac_id)tablespace indx;
