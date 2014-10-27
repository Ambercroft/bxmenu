// XRT Window Manager link list 
//
// Copyright 2008/05/16
// Bob Wickson XRT Corporation
//
// exhale - Window Manager

// Routines herein  configure base dir and files to load


#include "exhale.h"
#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/stat.h>

#include "exhaleconf.h"
int load_config(char * menudir){
  // menudir is really the dir the config file is in
  // load the config file for this menu
  // use a default for now (what is passed into menudir)-- current directory for config file
  FILE *cf = NULL;
  char configfile[] = "config.txt";
  char buffer[256]=" ";
  int ok = 0;
  int mcount = 0;
  int ccount = 0;
  int count = 0;
  // section for configurations to load
  char attr_mode[11]; // 9 (xrw) x3 1(dir) 1(NULL)
  deny_list = NULL; 
  deny_list_t = NULL; 
// allow_list = NULL; 
// allow_list_t = NULL; 
  FILE *uf = NULL;
  char user_list_file[256]=" "; 

  // append the config file to config directory : could use string copy but we only need this once
  while(menudir[mcount++]!='\0'){}
  mcount=mcount-1;
  if(menudir[mcount] != '/' || configfile[0] != '/'){
    // Add the forward slash in between dir path names if not there
    menudir[mcount]='/';
    mcount++;
  }
  while((configfile[ccount]!='\0') && (mcount <254)){
    menudir[mcount++]=configfile[ccount++];
  }
  menudir[mcount]='\0';
  // load the existing configfile
  cf = fopen(menudir,"r");
  if(cf > 0){
    while( ok == 0 ){
      fgets(buffer,sizeof(buffer),cf);
      //line feeds cause problems, get rid of them.
      count=0;      
      while(buffer[count] != '\n' && buffer[count] != '\0'){
        count++;
      }
      buffer[count] = '\0';
      //// program directory //////////////////////////////
      if(buffer[0]=='p'&&buffer[1]=='r' && buffer[4]==':'){
        //read from 6th location for directory
        count = 0;
        while(buffer[count+4]){ // what is end char
          menudir[count]=buffer[count+5]; // read until line is complete
          count++;
        }
      }
      //// file mode //////////////////////////////////////
      // attr_mode[]
      if(buffer[0]=='a' && buffer[4]==':'){
        //read from the 6th location
        count = 0;
        while((buffer[count+4]) && (count<10) ){ //look for end char
          attr_mode[count]=buffer[count+5]; // read only 9 char  
          count++;
        }
      }

      //// disallow types /////////////////////////////////
      // deny_list[]
      if(buffer[0]=='t' && buffer[3]=='!' && buffer[4]==':'){
        ///////////////// fill list ///////////////////////        
        if (deny_list_t == NULL){
          deny_list_t = (NODE_DESC_string_list *) malloc (sizeof(NODE_DESC_string_list));
          deny_list = deny_list_t;
        } else {
          // if list is already created ,?if multi lines of definition
        }
        count = 5; // first definition can only start here.

        char test_buf[50];
        int n = 0;
        while(buffer[count] != '\0'){

          n = sscanf(&buffer[count],"%s",test_buf);
          if ( n == 1 ){
            deny_list->item = malloc(strlen(test_buf));
            // copy name into list
            mcount = 0;           
            while(test_buf[mcount] != '\0'){
              deny_list->item[mcount] = test_buf[mcount];
              mcount++;
            }
            deny_list->item[mcount] = '\0';
            deny_list->next = (NODE_DESC_string_list *) malloc (sizeof(NODE_DESC_string_list));
            deny_list = deny_list->next;
            deny_list->next = NULL;
	    count = strlen(test_buf) + count + 1;

          }
        }
        ///////////////// end fill list  //////////////////        
      }

      ////  allow types  //////////////////////////////////
      // allow_list[]
      if(buffer[0]=='t' && buffer[3]=='e' && buffer[4]==':'){
        ///////////////// fill list ///////////////////////        
        if (allow_list_t == NULL){
          allow_list_t = (NODE_DESC_string_list *) malloc (sizeof(NODE_DESC_string_list));
          allow_list = allow_list_t;
        } else {
          // if list is already created ,?if multi lines of definition
        }
        count = 5;
        char test_buf[50];
        int n = 0;
        while(buffer[count] != '\0'){

          n = sscanf(&buffer[count],"%s",test_buf);
          if ( n == 1 ){
            allow_list->item = malloc(strlen(test_buf));
            // copy name into list
            mcount = 0;           
            while(test_buf[mcount] != '\0'){
              allow_list->item[mcount] = test_buf[mcount];
              mcount++;
            }
            allow_list->item[mcount] = '\0';
            allow_list->next = (NODE_DESC_string_list *) malloc (sizeof(NODE_DESC_string_list));
            printf("#165.conf  Allow_list->item =>[%s]\n",allow_list->item);
            allow_list = allow_list->next;
            allow_list->next = NULL;
            count = strlen(test_buf) + count + 1;
          }
        }
        ///////////////// end fill list  //////////////////        
      }
  
      ////  display user list /////////////////////////////
      // user_list[] 
      if(buffer[0]=='u' && buffer[4]==':'){
        // uf is FILE pointer to user generated file
        //read from the 6th location
        count = 0;
        while((buffer[count+4]) && (count<254) ){ //look for end char
          user_list_file[count]=buffer[count+5]; 
          count++;
        }
        // if we got this far then we have a file even if useless
         
        uf = fopen(user_list_file,"r");
          if(uf > 0){
// ****** Need to figure out format for user file ********************************* //            
            // read config file:
            fclose(uf);
          } else {
            printf("Unable to find file for reading user configuration.\n");
          }
      }

      //// end of config (make this better)  ///////////////
      // 
      if(buffer[0]=='E' && buffer[2]=='D' && buffer[4]==':'){
        ok = 1;
      }
 
    } 
    fclose(cf);
  } else {
    printf("Error loading config file\n");
    printf("Should be in the HOME dir of user.\n");
    return 1;
  }
  // update the menufile (ie the config file may have changed)
  // if a prgrm is removed then save prgm config for future reinstall

  return 0; // Return 0 on success of reading the file 
}


