drop index qury_reg_1;
drop  table qury_reg;
create table qury_reg(
        tx_br_no       varchar2(5),
        tx_tel         varchar2(6),
        tx_date        number(9),
        tx_code        varchar2(4),
        tx_time        number(9),
        wherelist      varchar2(1024),
        filler         varchar2(60)
)tablespace users;
create   unique index qury_reg_1 on qury_reg( tx_tel,tx_time,tx_date,tx_code)tablespace indx;
