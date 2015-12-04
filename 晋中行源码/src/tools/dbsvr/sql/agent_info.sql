drop table agent_info; 
create table agent_info 
( 
      ac_no varchar2(19),  
      type varchar2(1),
      tx_date number(8),
      br_no varchar2(6),
      tel varchar2(4),
      agent_name varchar2(60),
      id_type varchar2(1),
      id_no varchar2(30),
      phone varchar2(20)	
)tablespace users;
