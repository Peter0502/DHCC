drop   index tctd_ktzh_idx;
drop   index tctd_ktzh_idx2;
drop   table tctd_ktzh;
create table tctd_ktzh(
       br_no      varchar2(5),   /*开通机构*/
       tctd_br_no varchar2(12),  /*人行分配给开通行的行号*/
       begin_date number(8),     /*启用日期*/
       end_date   number(8),     /*止用日期*/
       tctd_acno  varchar2(24),  /*开通的账号*/
       tctd_name  varchar2(60),  /*开通的户名*/
       tc_kt_flag varchar2(1),   /*通存开通的标志*0,止用，1，启用*/
       td_kt_flag varchar2(1),   /*通兑开通的标志*0.止用，1，启用*/
       cx_kt_flag varchar2(1),   /*查询开通的标志*0,止用，1，启用*/
       td_amt_lmt number(17,2),  /*每日通兑累计限额*/
       tctd_xyno  varchar2(60),  /*通存通兑协议号*/
       tel        varchar2(4) ,  /*交易柜员*/
       trace_no   number(6),     /*交易流水*/
       filler     varchar2(60)   /*备注字段*/
)tablespace users;
create   unique index tctd_ktzh_idx on tctd_ktzh(tctd_acno)tablespace indx;
create   unique index tctd_ktzh_idx2 on tctd_ktzh(tctd_xyno)tablespace indx;
