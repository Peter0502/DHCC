/****С������������Ʊ�****/
drop index lv_parad_idx;
drop table lv_parad;
create table lv_parad(
        wt_date    	number(8),    /**ί������***/
        tx_time    	number(6),    /**����ʱ��****/
        flg        	varchar2(1),  /**�Ƿ��Ѿ������־****/
        chg_type   	varchar2(1),  /**�������,1-����,2-����,3-ɾ��**/
        eff_type   	varchar2(1),  /**��Ч����,0����,1-��ʱ**/
        eff_date   	number(8),    /**��Ч����***/
        chg_date   	number(8),    /**�������**/
        par_code   	varchar2(32) , /** ��������      **/
        par_name   	varchar2(60) , /** ��������      **/
        par_value  	varchar2(250) , /** ��������         **/
        filler     	varchar2(60) /**��ע***/
)tablespace users;
create index lv_parad_idx on lv_parad(eff_date,flg);

