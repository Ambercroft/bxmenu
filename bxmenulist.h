// XRT Window Manager link list
//
// Copyright Bob Wickson 2008/05/16
//
// exhale - Window Manager

#ifndef EXHALELISTH
#define EXHALELISTH

NODE_DESC * init_list();

NODE_DESC * init_node(NODE_DESC *);

NODE_DESC * find_open_node(NODE_DESC * );

NODE_DESC * find_last_node(NODE_DESC * );

NODE_DESC * find_clear_node(NODE_DESC * ,char * ,int ,NODE_DESC * );

NODE_DESC * delete_node(NODE_DESC * , NODE_DESC * );

NODE_DESC * fill_list(NODE_DESC *,char * );



int checkme(char*, char*);

int substitute(char*, char*, char*);

int combine(char*, char*, char*);

typedef struct NODE_DESC_string_list_s {
  char * item;
  struct NODE_DESC_string_list_s * next;
} NODE_DESC_string_list;

int accept_name(char*, NODE_DESC_string_list *);

int deny_name(char*, NODE_DESC_string_list *);

		
#endif

