// ��ˮ�� ��mo_stok_trace
//Table index : mo_stok_trace_idx unique 
//mo_stok_trace.sql
create table mo_stok_trace (
txdate 						varchar2(8)   ,			//����                              	                   
traceno		       	number(12,0)    	  //��ˮ��                              	��1��ʼ
) tablespace users;
create unique index mo_stok_trace_idx on mo_stok_trace(txdate,traceno) tablespace indx;