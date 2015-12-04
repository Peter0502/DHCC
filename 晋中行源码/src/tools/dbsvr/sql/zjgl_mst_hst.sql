drop  table zjgl_mst_hst;
create table zjgl_mst_hst(
    trace_no   number(9),
    trace_cnt  number(9),
    cur_no     varchar2(2),
    opn_br_no  varchar2(5),
    tx_br_no   varchar2(5),
    tx_code    varchar2(4),
    sub_tx_code varchar2(4),
	tx_date    number(8),
    tx_time    number(9),
	acc_no     varchar2(7),
    acc_hrt    varchar2(5),
    add_ind    varchar2(1),
    ct_ind     varchar2(1),
    tx_amt     number(16,2),
    dr_bal     number(16,2),
    cr_bal     number(16,2),
    dr_intst_acm  number(19,2),
    cr_intst_acm  number(19,2),
    note_type   varchar2(3),
    note_no     varchar2(16),
    brf         varchar2(20),
    hst_cnt     number(9),
    tel         varchar2(4),
    chk         varchar2(4),
    auth        varchar2(4)
)tablespace users;
create   unique index zjgl_hst_1 on zjgl_mst_hst( opn_br_no,cur_no,acc_no,hst_cnt)tablespace indx;
