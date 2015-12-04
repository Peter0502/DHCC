drop index sub_dd_mst_idx1;
drop  table sub_dd_mst;
create table sub_dd_mst(
  br_no        varchar2(5),
  ac_no        varchar2(24),
  sub_ac_seqn  number(8),
  sub_ac_no    varchar2(24),
  bal          number(16,2),
  ys_bal       number(16,2),
  intst_acm    number(19,2),
  opn_date     number(9),
  ic_date      number(9),
  lst_date     number(9),
  hst_cnt      number(9),
  hst_pg       number(9),
  name         varchar2(50),
  paper_type   varchar2(1),
  paper_no     varchar2(20),
  telephone    varchar2(20),
  ac_sts       varchar2(1),
  zf_bal       number(16,2),
  rate_type    varchar2(1),  /* �������� */
  intst_type   varchar2(1),  /* ��Ϣ���� */
  rate         number(9,5),  /* ����     */
  xq_num       number(6),    /* С����� */
  lp_num       number(6),    /* ¥�̱�� */
  dy_num       number(6),    /* ��Ԫ��� */
  fj_num       number(6),    /* ������ */
  bar_code     varchar2(16), /* ������   */
  filler       varchar2(61), /* ��ע     */
  filler1      varchar2(61), /* ��ע     */
  filler2      varchar2(61)  /* ��ע     */
)tablespace users
  pctfree 10
  initrans 1
  maxtrans 255
  storage
  (
    initial 64K
    minextents 1
    maxextents unlimited
  );
create	 unique index sub_dd_mst_idx1 on sub_dd_mst( br_no,ac_no,sub_ac_no)tablespace indx;
create	 unique index sub_dd_mst_idx2 on sub_dd_mst( ac_no,xq_num,lp_num,dy_num,fj_num)tablespace indx;

