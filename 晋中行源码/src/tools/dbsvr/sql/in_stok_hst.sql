
/**股金明细帐：in_stok_hst.sql*/
drop table in_stok_hst;
create table   in_stok_hst (    
stokno				varchar2(18),			/*股东证号	                          */
seqno					number(6,0),			/*序号	每天重新编号，所有股东帐户统一编号*/
dcind					varchar2(1),			/*借贷标志	1：借，2：贷                  */
txcode				varchar2(2),			/*交易代码	目前可不填                  */
txtype				varchar2(1),			/*交易种类	目前可不填                  */
txamt					number(16,2),			/*发生额	                              */
txdate				varchar2(8),			/*交易日期	                            */
txtime				varchar2(8),			/*交易时间	                            */
bookcnt				number(2,0),			/*存折行数	本笔所在存折行数            */
stcktype			varchar2(3),			/*股东类别	第一位：1：国有，2：法人，3：个体工商，4：职工，5：个人，6：其它  第二位：1：普通股，2：优先股 第三位：1：保息股，2：普通股                                      */
name					varchar2(60),			/*股东姓名	                            */
brf						varchar2(20),			/*摘要	                                  */
bookbrf				varchar2(20),			/*存折摘要	                          */
bal						number(16,2),			/*当前余额	                              */
txbrno				varchar2(7),			/*机构号	                              */
tel						varchar2(6)				/*操作员	       */
) tablespace users;
create unique index in_stok_hst_idx on in_stok_hst(stokno,seqno,txdate) tablespace indx;  
