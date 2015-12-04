      /**	小额汇总核对报文登记簿**/
drop table lv_chk852_m;
create table lv_chk852_m(
   in_date       number(8) ,  /** 进入系统日期    **/
   pack_date     number(8) ,  /** 委托日期        **/
   dz_date       number(8) ,  /*  对帐日期 */
   cash_qs_no    varchar2(12),  /** 对帐结点号(接收清算行行号)  **/
   dtlcnt        number(6),     /* 其他状态对账记录数目 */
   filler        varchar2(60)   /** 备注            **/
)tablespace users;
