drop index sub_dd_mst_hst_idx1;
drop  table sub_dd_mst_hst;
create table sub_dd_mst_hst(
  trace_no    number(9),
  trace_cnt   number(9),
  opn_br_no   varchar2(5),
  tx_br_no    varchar2(5),
  ac_no       varchar2(24),
  sub_ac_seqn number(8),
  sub_ac_no   varchar2(24),
  add_ind     varchar2(1),
  ct_ind      varchar2(1),
  tx_amt      number(16,2),
  bal         number(16,2),
  intst_acm   number(19,2),
  tx_date     number(9),
  up_tx_date  number(9),
  tx_time     number(9),
  hst_cnt     number(9),
  brf         varchar2(20),
  tel         varchar2(4),
  chk         varchar2(4),
  auth        varchar2(4),
  tt_no       number(9),
  note_type   varchar2(4),
  note_no     varchar2(16)
)
tablespace USERS
  pctfree 10
  initrans 1
  maxtrans 255
  storage
  (
    initial 64K
    minextents 1
    maxextents unlimited
  );
create	 unique index sub_dd_mst_hst_idx1 on sub_dd_mst_hst( tx_date,trace_no,trace_cnt)tablespace indx;
