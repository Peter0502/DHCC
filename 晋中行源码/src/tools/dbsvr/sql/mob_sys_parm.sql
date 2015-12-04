drop  table mob_sys_parm;  /***短信通知减免配置**/
create table mob_sys_parm(
        jmbz varchar2(2),  /****减免标志0-减免 1-不减免*******/
        jmsj varchar2(2)         /****减免时间0-时间为为零，即不减免 3-减免3个月 6-减免6个月*******/
)tablespace users;
