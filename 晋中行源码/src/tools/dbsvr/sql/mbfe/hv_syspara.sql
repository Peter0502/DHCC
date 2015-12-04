/* ´ó¶îÖ§¸¶ÏµÍ³²ÎÊı±í */
drop index hv_syspara_idx;
drop table hv_syspara;
create table hv_syspara(
    sdate      varchar2(8),    /* ÉúĞ§Ê±¼ä */
    updtype    varchar2(1),    /* ¸üĞÂÀàĞÍ 1Ôö¼Ó 2±ä¸ü 3×¢Ïú*/
    startamt   number(15,2),   /* Æğµã½ğ¶î */
    timedc     varchar2(8),    /* Ê±¼äÊı¾İ´úÂë */
    tdcname    varchar2(20),   /* Ê±¼äÊı¾İÃû³Æ */
    updtdc     varchar2(18),   /* Ê±¼äÊı¾İ¸üĞÂÖµ */
    updmethod  varchar2(1),    /* ¸üĞÂ·½Ê½ 1±¾µØÁ¢¼´ÉúĞ§2±¾µØÖ¸¶¨ÈÕÆÚÉúĞ§3ÏÂ·¢Á¢
¼´ÉúĞ§4ÏÂ·¢Ö¸¶¨ÈÕÆÚÉúĞ§*/
    datatype   varchar2(1)     /* Êı¾İÀàĞÍ1.202ÏµÍ³ÔËĞĞÊ±¼ä²ÎÊı 2.205´ó¶îÒµÎñ½ğ¶
îÆğµã */
)tablespace users;
create unique index hv_syspara_idx on hv_syspara(sdate,updtype,timedc);

