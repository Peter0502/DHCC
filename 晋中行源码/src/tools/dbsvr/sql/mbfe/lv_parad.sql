/****小额批量服务控制表****/
drop index lv_parad_idx;
drop table lv_parad;
create table lv_parad(
        wt_date    	number(8),    /**委托日期***/
        tx_time    	number(6),    /**接收时间****/
        flg        	varchar2(1),  /**是否已经处理标志****/
        chg_type   	varchar2(1),  /**变更类型,1-增加,2-更新,3-删除**/
        eff_type   	varchar2(1),  /**生效类型,0立即,1-定时**/
        eff_date   	number(8),    /**生效日期***/
        chg_date   	number(8),    /**变更日期**/
        par_code   	varchar2(32) , /** 参数代码      **/
        par_name   	varchar2(60) , /** 参数名称      **/
        par_value  	varchar2(250) , /** 参数内容         **/
        filler     	varchar2(60) /**备注***/
)tablespace users;
create index lv_parad_idx on lv_parad(eff_date,flg);

