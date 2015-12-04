/**股金未登折明细：mdm_stok_unprt_hst.sql*/
drop  table mdm_stok_unprt_hst;
create table mdm_stok_unprt_hst(
stokno						varchar2(18),						/*股东证号	                                    */
unprtno						number(4,0)	,						/*未登折序号	本股东证目前所有未登折的信息统一编号*/
txday							varchar2(8)	,						/*交易日期	                                        */
txbrno						varchar2(7)	,						/*机构号	                                        */
tel								varchar2(6)	,						/*操作员号	                                          */
dcind							varchar2(1)	,						/*借贷标志	1：借，2：贷                            */
txamt							number(16,2),						/*发生额	                                        */
bookbrf						varchar2(20),						/*存折摘要	                                    */
nowcnt						number(4,0)	,						/*存折位置	是红利的时候可能要与此字段配合使用    */
flag							varchar2(1)							/*股金、红利标志	0：股金，1：红利    */
) tablespace users;
create unique index mdm_stok_unprt_hst_idx on mdm_stok_unprt_hst(stokno,unprtno,flag) tablespace indx;              
