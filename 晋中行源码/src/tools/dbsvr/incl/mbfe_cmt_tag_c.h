#ifndef __mbfe_cmt_tag_CH__
#define __mbfe_cmt_tag_CH__
struct mbfe_cmt_tag_c{
	char      rowid[20];
	char      cmtno[4];
	long      tag_seqn;
	char      tag[4];
	long      tag_maxlen;
	char      tag_name[41];
	char      tag_data_type[3];
	char      null_flag[2];
	long      len1;
	char      field1[21];
	char      tag_data_code1[5];
	long      len2;
	char      field2[21];
	char      tag_data_code2[5];
};
#endif 
