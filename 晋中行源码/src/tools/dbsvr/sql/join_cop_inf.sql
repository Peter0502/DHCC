drop index join_cop_inf_1;
drop table join_cop_inf;
create table join_cop_inf(
cif_no number(9),
cop_no number(9),
name varchar2(20),
type varchar2(1),
tx_date number(9),
id_type varchar2(1),
id_no varchar2(20),
br_no varchar2(20),
ag_cr_no varchar2(20),
up_date number(8),
tx_br_no varchar2(6),
tel varchar2(4)
)tablespace users;
create unique index join_cop_inf_1 on join_cop_inf(cif_no,cop_no) tablespace indx;

