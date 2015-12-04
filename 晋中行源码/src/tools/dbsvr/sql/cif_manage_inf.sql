drop index cif_manage_inf_1;
drop table cif_manage_inf;
create table cif_manage_inf(
        cif_no number(9),
        manage_no number(9),
        name varchar2(20),
        sex varchar2(1),
        tx_date number(9),
        type varchar2(1),
        id_type varchar2(1),
        id_no varchar2(20),
        up_date number(8),
	br_no varchar2(6),
	tel varchar2(4)
)tablespace users;
create unique index cif_manage_inf_1 on cif_manage_inf(cif_no,manage_no) tablespace indx;

