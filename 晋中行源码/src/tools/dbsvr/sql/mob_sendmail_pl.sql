/******************************************************************************/
/*  table name   : mob_sendmail_pl   (待发送短信登记薄)                          */
/*  table index  : mob_sendmail_pl_idx                                           */
/*  create by    : wangxudong@2010/01/26                                      */
/*  memo         : 根据宁夏情况新增字段，修改表结构                          */
/******************************************************************************/
drop table mob_sendmail_pl;
drop index mob_sendmail_pl_idx;
create table mob_sendmail_pl(
 tx_code              varchar(4)    ,
 ac_id                integer     ,
 ac_no                varchar(20)   ,
 tel                  varchar(6)    ,
 trace_no             integer       ,
 tx_date              integer       ,
 tx_time              integer       ,
 opt_no               varchar(3)    ,
 mobile               varchar(50)   ,
 message              varchar(140)  ,
 timeflag             varchar(1)    ,
 t_date               integer       ,
 t_time               integer       ,
 send_level           varchar(1)    ,
 sene_times           integer       ,
 send_date            integer       ,
 send_time            integer       ,
 sxf_amt              decimal(12,2) ,
 proc_flag            varchar(1)    ,
 tt_no                integer       ,
 tx_type              varchar(1)    ,
 mobseq               integer
)lock mode row;

create index mob_sendmail_pl_idx on mob_sendmail_pl(ac_no,trace_no,mobile);

