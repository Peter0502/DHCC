drop table xqdz;
create table xqdz
  (
    xqdz100 varchar2(24), /* 小区主账号 */
    xqdz101 varchar2(10), /* 小区编号 */
    xqdz110 varchar2(8),  /* 归属机构 */
    xqdz111 varchar2(8),  /* 交易机构 */
    xqdz15 varchar2(60),  /* 小区名称 */
    xqdz16 varchar2(60),  /* 小区地址 */
    xqdz17 varchar2(20),  /* 联系人 */
    xqdz18 varchar2(20),  /* 联系电话 */
    xqdz20 number(9),     /* 登记日期 */
    xqdz21 number(9), 	  /* 登记时间 */
    xqdz22 varchar2(6),	  /* 登记柜员 */
    xqdz23 number(9),
    xqdz24 number(9),
    xqdz25 varchar2(6),
    xqdz30 varchar2(1),
    xqdz31 varchar2(60),  /* 小区开发商 */
    primary key (xqdz100,xqdz101)
  );
