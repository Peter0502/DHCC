drop index ag_qs_gd_rel_1;
drop table ag_qs_gd_rel;
create table ag_qs_gd_rel(
        qs_ac_no varchar2(19),
        gd_ac_no varchar2(19),
        unit_no varchar2(4),
        clr_date number(9),
        tel varchar2(4)
)tablespace users;
create   unique index ag_qs_gd_rel_1 on ag_qs_gd_rel(qs_ac_no,unit_no)tablespace indx;
