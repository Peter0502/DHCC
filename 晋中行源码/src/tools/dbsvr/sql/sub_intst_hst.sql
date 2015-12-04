drop index sub_intst_hst_idx1;
drop index sub_intst_hst_idx2;
drop index sub_intst_hst_idx3;
drop  table sub_intst_hst;
create table sub_intst_hst(
        br_no        varchar2(5),
        ac_no        varchar2(24),
        sub_ac_seqn  number(8),
        sub_ac_no    varchar2(24),
        bic_bal      number(16,2),
        bic_acm      number(16,2),
        fct_intst    number(12,2),
        intst_tax    number(12,2),
        aic_bal      number(16,2),
        aic_acm      number(16,2),
        ic_date      number(9),
        intst_date   number(9),
        ac_ind       varchar2 (1),
        pt_ind       varchar2 (1),
        rate         number(8,2),
        jx_date      number(9),
        intst_flag   varchar2 (1),
        intst_sts    varchar2 (1)
)tablespace users;
create   index sub_intst_hst_idx1 on sub_intst_hst( br_no,intst_date,intst_sts) tablespace indx;
create   index sub_intst_hst_idx2 on sub_intst_hst( ac_no,sub_ac_seqn,pt_ind) tablespace indx;
create   index sub_intst_hst_idx3 on sub_intst_hst( ac_ind,pt_ind,intst_sts) tablespace indx;

