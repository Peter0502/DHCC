      /**	小额汇总核对报文登记簿**/
drop index lv_chk842_d_idx;
drop table lv_chk842_d;
create table lv_chk842_d(
   zc_jd_lx      	varchar2(1),  /* 扎差节点类型 */
   zc_date       	number(8),  /* 轧差日期    */
   zc_cc        	varchar2(2),  /* 轧差场次 */
   pkgno       		varchar2(3),  /* 包类型号 */
   wb_cnt       	number(8)  ,  /* 往包总数 */
   wb_tx_amt     	number(22) ,  /* 往包金额合计 */
   lb_cnt       	number(8)  ,  /* 来包总数 */
   lb_tx_amt     	number(22) ,  /* 来包金额合计 */
   dz_wb_cnt     	number(8)  ,  /* 对帐往包总数*/
   dz_wb_tx_amt   	number(22) ,  /* 对帐往包金额合计 */
   dz_lb_cnt     	number(8)  ,  /* 对帐来包总数 */
   dz_lb_tx_amt   	number(22) ,  /* 对帐来包金额合计 */
   pack_sts    		varchar2(2),  /* 包状态01-已拒绝,02-已发送,03-已轧差,04-已排队,
                                       05-已清算,06-已撤销,10-已回执,11-已拒付,
                                       12-已冲正,13-已止付,14-已超期,15-回执中,
                                       99-无此业务 */
   filler      		varchar2(60)  /** 备注            **/
)tablespace users;
create unique index lv_chk842_d_idx on lv_chk842_d(zc_jd_lx,zc_date,zc_cc,pkgno,pack_sts)tablespace indx;

