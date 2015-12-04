/* 大额支付系统参数表 */
drop index hv_syspara_idx;
drop table hv_syspara;
create table hv_syspara(
    sdate      varchar2(8),    /* 生效时间 */
    updtype    varchar2(1),    /* 更新类型 1增加 2变更 3注销*/
    startamt   number(15,2),   /* 起点金额 */
    timedc     varchar2(8),    /* 时间数据代码 */
    tdcname    varchar2(20),   /* 时间数据名称 */
    updtdc     varchar2(18),   /* 时间数据更新值 */
    updmethod  varchar2(1),    /* 更新方式 1本地立即生效2本地指定日期生效3下发立
即生效4下发指定日期生效*/
    datatype   varchar2(1)     /* 数据类型1.202系统运行时间参数 2.205大额业务金�
钇鸬� */
)tablespace users;
create unique index hv_syspara_idx on hv_syspara(sdate,updtype,timedc);

