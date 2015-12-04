drop view prdt_ac_id;
create view prdt_ac_id (ac_id,ac_seqn,ac_id_type,prdt_no,cif_no) as 
	select x0.ac_id ,x0.ac_seqn ,'1' ,x0.prdt_no ,x0.cif_no 
		from dd_mst x0  union 
	select x1.ac_id ,x1.ac_seqn ,'2' , x1.prdt_no ,x1.cif_no 
		from td_mst x1  union 
	select x2.ac_id ,x2.ac_seqn ,'3' ,x2.prdt_no ,x2.cif_no 
		from ln_mst x2  union 
	select x3.ac_id ,x3.ac_seqn ,'4' ,x3.prdt_no ,0 
		from od_mst x3  
	union select x4.ac_id ,x4.ac_seqn ,'9' ,x4.prdt_no ,0 
		from in_mst x4 ;               

drop view wjdeview_dd;                 
create view wjdeview_dd (cr_bal) as 
	select sum(x2.cr_bal ) 
		from dc_acc_rel x0 ,dd_parm x1 ,gl_prdt_dyn x2 
		where (((x2.prdt_no = x1.prdt_no ) AND (x1.dc_code = x0.dc_code ) ) 
			AND (substr(x0.acc_hrt,1,3) = '211' ) ) ;               

drop view wjdeview_td;
create view wjdeview_td (cr_bal) as 
	select sum(x2.cr_bal ) 
		from dc_acc_rel x0 ,td_parm x1 ,gl_prdt_dyn x2 
		where (((x2.prdt_no = x1.prdt_no ) AND (x1.dc_code = x0.dc_code ) ) 
			AND (substr(x0.acc_hrt,1,3) = '211' ) ) ;               

drop view wjdeview;
create view wjdeview (cr_bal) as 
	select sum(x3.cr_bal ) 
		from dc_acc_rel x0 ,dd_parm x1 ,td_parm x2 ,gl_prdt_dyn x3 
		where ((((x3.prdt_no = x1.prdt_no ) AND (x1.dc_code = x0.dc_code ) ) 
				OR ((x3.prdt_no = x2.prdt_no ) AND (x2.dc_code = x0.dc_code ) )
				 ) AND (substr(x0.acc_hrt,1,3) = '211' ) ) ;                                       

drop view bal_ciftype;                        
create view bal_ciftype (opn_br_no,bal,ys_bal,prdt_no,cif_type) as 
	select x0.opn_br_no ,x0.bal ,x0.ys_bal ,x0.prdt_no ,x1.type 
		from ln_mst x0 ,cif_basic_inf x1 
		where ((x0.cif_no = x1.cif_no ) AND (x0.prdt_no IN ('3Z1' ,'3Z2' )) ) ; 

drop view acc_ac_id;   
create view acc_ac_id ( acc_no,data_code,ac_id ) as 
select x0.acc_no,x1.data_code,x3.ac_id from  com_item x0, dc_acc_rel  x1, dd_parm  x2, dd_mst  x3
where x3.prdt_no=x2.prdt_no and x2.dc_code=x1.dc_code and x1.acc_hrt=x0.acc_hrt

