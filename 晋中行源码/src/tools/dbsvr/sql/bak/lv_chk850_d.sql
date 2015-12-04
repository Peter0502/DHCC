      /**	小额汇总核对报文登记簿**/
drop index lv_chk850_d_idx;
drop table lv_chk850_d;
create table lv_chk850_d(
   lw_flg       varchar2(1),  /* 来往帐务表识 1往帐2来帐 */
   zc_jd_lx     varchar2(1),  /* 扎差节点类型 */
   zc_date      number(8),  /** 轧差日期    **/
   zc_cj        varchar2(2),  /* 轧差场次 */
   pkgno        varchar2(3),  /* 包类型号 */
   pack_sts   	varchar2(2),  /*包状态01-已拒绝,02-已发送,03-已轧差,04-已排队,
                                     05-已清算,06-已撤销,10-已回执,11-已拒付,
                                     12-已冲正,13-已止付,14-已超期,15-回执中,
                                     99-无此业务 */
   filler       varchar2(60)  /** 备注            **/
)tablespace users;
create unique index lv_chk850_d_idx on lv_chk850_d(lw_flg,zc_date,pkgno,pack_sts)tablespace indx;
