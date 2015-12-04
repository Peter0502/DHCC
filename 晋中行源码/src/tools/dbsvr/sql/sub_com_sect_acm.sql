drop index sub_com_sect_acm_idx1;
drop  table sub_com_sect_acm;
create table sub_com_sect_acm(
        ac_no        varchar2(24),
        sub_ac_seqn  number(8),
        sub_ac_no    varchar2(24),
        beg_date     number(9),
        end_date     number(9),
        rate         number(9,6),
        intst_acm    number(19,2),
        intst_type   varchar2(1),
        sts          varchar2(1),
        jz_date      number(9)
)tablespace users;
create   index sub_com_sect_acm_idx1 on sub_com_sect_acm( ac_no,sub_ac_seqn,intst_type,sts) tablespace indx;

