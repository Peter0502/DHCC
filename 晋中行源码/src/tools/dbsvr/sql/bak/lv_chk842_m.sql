      /**	小额汇总核对报文登记簿**/
drop table lv_chk842_m;
create table lv_chk842_m(
   dz_date       number(8)  ,  /** 进入系统日期    **/
   pack_date     number(8)  ,  /** 委托日期        **/
   cash_qs_no 	 varchar2(12) ,  /** 对帐结点号(接收清算行行号)  **/
   qs_zt_cnt  	 number(6),    /* 已清算状态对账记录数目 */
   qt_zt_cnt   	 number(6),    /* 其他状态对账记录数目 */
   filler        varchar2(60)    /** 备注            **/
)tablespace users;
