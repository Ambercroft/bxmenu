// XRT Window Manager header
//
// Copyright 2008/06/07
// Bob Wickson
//
// exhale - Windowmanager

#ifndef EXHALEH
#define EXHALEH

typedef struct NODE_DESC_s {
  int ID;
  int icon;
  char *name;
  struct stat * status;
  char *dis_name;
  char *exec_name;
  char *icon_name;
  int user; // refers to the file permissions
  int group;// refers to the file permissions
  int other;// refers to the file permissions
  int new_item;// parameters could be first viewing, added within the last week, etc
  int existing_item;// in the list
  int selected_item;// added to the menu
  int type; // catagory of item
  struct NODE_DESC_s *next;
  struct NODE_DESC_s *last;
}NODE_DESC;

#include<stdio.h>
#include<stdlib.h>

#include</usr/include/X11/Xlib.h>
#include</usr/include/X11/Xutil.h>
#include</usr/include/X11/Xos.h>
#include</usr/include/X11/Xatom.h>

#include "bxmenulist.h"
#include "bxmenuconf.h"

int load_config();
int checkme();
int substitute();
int combine();


#endif 
