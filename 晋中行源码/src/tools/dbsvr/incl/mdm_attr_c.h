#ifndef __mdm_attr_CH__
#define __mdm_attr_CH__
struct mdm_attr_c{
	char      rowid[20];
	char      mdm_code[5];
	char      mdm_name[21];
	char      mdm_type[2];
	char      ac_num_ind[2];
	char      draw_uncon_yn[2];
	char      qry_pwd_yn[2];
	char      draw_pwd_yn[2];
	char      draw_id_yn[2];
	char      draw_seal_yn[2];
	char      pwd_mach_yn[2];
	char      note_type[4];
	char      only_ind[2];
	char      no_ind[2];
	char      prt_ind[2];
	long      pg_num;
	long      pg_line;
};
#endif 
