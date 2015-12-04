drop index netbank_cif_ukey_rel_1;
drop table netbank_cif_ukey_rel; 
create table netbank_cif_ukey_rel
( 
      cif_no number(9),  
      ukey varchar(16),
      sts varchar(1),
      sign_date number(8),
      cancel_date number(8)
)tablespace users;
create unique index netbank_cif_ukey_rel_1 on netbank_cif_ukey_rel( cif_no,ukey)tablespace indx;
