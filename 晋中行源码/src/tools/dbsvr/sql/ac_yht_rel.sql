drop index ac_yht_rel_1;
drop  table ac_yht_rel;
create table ac_yht_rel(
    ac_id number(9),
    yht_ac_id number(9),
    lmt_amt number(16,2),
    zz_lmt number(16,2),
    brf   varchar2(30)
)tablespace users;
create unique index ac_yht_rel_1 on ac_yht_rel(ac_id,yht_ac_id)tablespace indx;
