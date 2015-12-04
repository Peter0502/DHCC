/**************************************************************************/
/*   Filename : /CStalk/a_incl/aplc_extn.h                                */
/*   Usage    : APCTL Client Library  apreqlib.a                          */
/**************************************************************************/
   extern    ClientConnectId          t_conn_id;
   extern    RCBP                     t_client_rcb;
   extern    char                     t_user[30];
   extern    char                     t_password[30];
   extern    char                     t_client_name[30];
   extern    char                     t_request_name[30];
   extern    char                     t_xact_obj[30];
   extern    char                     t_xact_name[128];
   extern    char                    *t_request_buffer;
   extern    char                    *t_source_path;
   extern    char                    *t_target_path;
   extern    short                    t_request_length;
   extern    short                    t_timeout;
   extern    short                    t_protocol;
   extern    short                    t_ap_errcode;
   extern    short                    t_source_len;
   extern    short                    t_target_len;
   extern    short                    t_prep_xname;
   extern    long                     t_rc_status;
   extern    long                     t_xact_id;
   extern    long                     t_request_id;
   extern    void                     (*t_errors_handler)();
   extern    void                     (*t_normal_handler)();

   extern    Boolean                  (*Request)();
   extern    Boolean                  (*RequestBLOB)();
   extern    long                     (*RequestNo)();
   extern    long                     (*RequestBLOBNo)();
