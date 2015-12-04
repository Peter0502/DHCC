#!/bin/sh
update_onebit(){
INDEX=$1
sqlplus -S $DB_USER/$DB_PASSWD  <<!
select bit_map from tx_def where tx_code='6202';
update tx_def set bit_map=substr(bit_map,1,${INDEX}-1)||'1'||substr(bit_map,${INDEX}+1,128-${INDEX}) where tx_code in('7701', '7702', '7703', '7709', '7801', '7802', '7803', '7809', '7888', '6201', '6202', '6203', '6204', '6205', '6206', '6210', '6301', '6302', '6303', '6304', '6305', '6306', '6307', '6308', '6309', '6310', '6311', '6312', '6313', '6314');
select bit_map from tx_def where tx_code='6202';
commit;
!
}
update_onebit 61
