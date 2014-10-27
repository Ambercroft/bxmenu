// XRT Window Manager link list
//
// Copyright Bob Wickson 2008/05/16
//
// exhale - Window Manager


#include "bxmenu.h"
#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/stat.h>

#include "bxmenulist.h"

// Initailize the list

NODE_DESC * init_list(){
  static NODE_DESC * gpl = NULL;
  gpl = init_node(NULL); // this is the top of list
  return gpl;
}

// Initialize a node on the required list

NODE_DESC * init_node(NODE_DESC * topoflist){
  NODE_DESC * item;
  item = (NODE_DESC *) malloc(sizeof(NODE_DESC));
  item->name = NULL; // use malloc while allocating name
  item->status = NULL; // use malloc while allocating stucture
  item->dis_name = NULL; // display name 
  item->exec_name = NULL; // name to exec
  item->icon_name = NULL; // name the icon file to use
  item->ID = 0; // ID # is 0 if 'grasspath' is unused
  item->next = NULL;
  item->last = topoflist;
  return item;
}

NODE_DESC * find_open_node(NODE_DESC * endoflist){
  NODE_DESC * search;
  search = endoflist;
  while ( search != NULL ){
    if (search->ID == 0){
      return search;
    }
    search = search->next;
  }
  return search;
}

NODE_DESC * find_last_node(NODE_DESC * endoflist){
  NODE_DESC * search;
  search = endoflist;
  while ( search->next != NULL ){
    search = search->next;
  }
  return search;
}

NODE_DESC * find_clear_node(NODE_DESC * endoflist,char * name,int ID,NODE_DESC * gpd){
  NODE_DESC * search;
  search = endoflist;
  while ( search != NULL ){
    // do strcmp 
    if ( (search->ID == ID && ID != 0)||(search->name==name)||(search==gpd)){
      // initialize search
      search->name='\0';
      search->ID = 0;
      return search;
    }
  }
  return NULL;
}

NODE_DESC * delete_node(NODE_DESC * endoflist, NODE_DESC * gpk){
  if (gpk == endoflist){
    // top of list special handling
    if (gpk->next == NULL){
      free(gpk);
      return NULL;
    }else{
      endoflist=gpk->next;
      free(gpk);
      return endoflist;
    }
  }
  gpk->last->next = gpk->next;
  if (gpk->next != NULL){
    gpk->next->last = gpk->last;
    return endoflist;
  }else{
    return NULL; // this may not be what I want to return as an error condition
  }
}

NODE_DESC * fill_list(NODE_DESC * endoflist,char mdir[]){
  int count = 0;
  int counti= 0;
  int x = 0;
  DIR *dp = NULL;
  struct dirent *ep = NULL;
  // initialize the the char array
  char buf[256];
  count = 0;
  for (count =0;count<(256);buf[count++]='\0'){}
  // initialize the the char array
  char temp_name[(256)];// name to actually execute: no parm
  count = 0;
  for (count =0;count<(256);temp_name[count++]='\0'){}
  // initialize the the char array
  char testname[512];
  count = 0;
  for (count =0;count<(256);testname[count++]='\0'){}

  dp = opendir (mdir);
  if (dp != NULL){
    while (ep = (readdir (dp))){
      ////////////// Check to see if it is a 'typ!' attribute ////////////////
      if ( ep->d_name[0] != '.' ){ 
      ////////////// If it is an icon then put in sepatate list for matching /      
        if(deny_name(ep->d_name,deny_list_t) == 0 ){
//if ( ep->d_name[max -4 to max] == ".ico" ) { separate file list }
          ////////////// If it is an ok file to use then add to list /////////////
          endoflist = init_node(endoflist);	
          endoflist->name = malloc( sizeof(ep->d_name));
          endoflist->status = malloc (sizeof(struct stat));
          // find CR at the endof line.
         
          counti = 0;
          while((ep->d_name[counti] != '\0') && (ep->d_name[counti] != '\n')){
            endoflist->name[counti]=ep->d_name[counti];
            counti++;
          }
          endoflist->name[counti]='\0';
printf("name copied [%s]\n",endoflist->name);

// old way           strcpy(endoflist->name,ep->d_name);
          // find dis_name
          if ( checkme(endoflist->name,"*.desktop") != 0 ){
            // I'm a desktop file.
            // find the display name in the file.
            // debug // international wording  check here ie. after name!!
            // add the dir to look in for the file
            x = 0;
            while(mdir[x]!='\0'){
              //printf("mdir[%d] = %d.\n",x,mdir[x]);
              x++;
            }
            if(mdir[x-1]!='/'){
              mdir[x]='/';
              mdir[x+1]='\0';
            }
            combine(testname,mdir,endoflist->name);
            substitute(testname,temp_name,"Name");
            endoflist->dis_name = malloc(sizeof(temp_name));
            // find the exec name in the file.
            strcpy(endoflist->dis_name,temp_name);
            substitute(testname,temp_name,"Exec");
            endoflist->exec_name = malloc(sizeof(temp_name));
            strcpy(endoflist->exec_name,temp_name);
          }else{
            // I don't look like a desktop file
            // make name the display name and the exec name.

            endoflist->exec_name = malloc(sizeof(endoflist->name));
            strcpy(endoflist->exec_name,endoflist->name);
            endoflist->dis_name = malloc(sizeof(endoflist->name));
            strcpy(endoflist->dis_name,endoflist->name);
          }
printf("temp_name > {%s}\n",temp_name);
printf("endoflist->name > {%s}\n",endoflist->name);
printf("endoflist->exec_name > {%s}\n",endoflist->exec_name);
printf("endoflist->dis_name > {%s}\n",endoflist->dis_name);



          //////////////add the dir to the name to use 'stat' ////////////////////
          // debug use the exec name to decide on the stats
          // debug >> if name is not used then find the stat of the exec_name file 
          count = 0;
          counti = 0;
          while(mdir[count] != '\0' ){
            buf[count]=mdir[count];
            count++;
          }
          while(ep->d_name[counti] != '\0'){
            buf[count]=ep->d_name[counti];
            count++;
            counti++;
          }
          buf[count]='\0';
      ///////Finish/add the dir to the name to use 'stat' ////////////////////

      ///////////////// Grabbing the file stats //////////////////////////////
        if(stat(buf,endoflist->status) == -1){ 
	  //perror("stat failure");
	  //exit(1);
//          printf("stat failure - no stat file to load?\n");
        }
//debug//
//        printf("mode = %lo %ld %lld %lld\n",(unsigned long)endoflist->status->st_mode,\
//                                          (long)endoflist->status->st_uid,\
//                                          (long long)endoflist->status->st_size,\
//                                          (long long)endoflist->status->st_atime);
        ///////Finish//// Grabbing the file stats //////////////////////////////
        }else{
//          printf("Name has been denied!\n");// debug
        }
        // endof if deny_name 
      } // endof if != '.'
      
//debug//
      else { printf("Name not used -->%s\n",ep->d_name);
      }
    }
    (void) closedir (dp);
  }else{
		printf("Could not open directory.\n");
  }
  // free(temp);	
  // load the existing menufile

  // update the menufile (ie the config file may have changed)
  // if a prgrm is removed then save prgm config for future reinstall

  return endoflist; // this just returns what was supplied (change when routines are written)
}


///////////////////////////////////////////////////////////////////////////////
// return false if the name is in the list

int accept_name(char * name,NODE_DESC_string_list * list){

return 0;
}

///////////////////////////////////////////////////////////////////////////////
// return false if the name is in the list
///////////////////////////////////////////////////////////////////////////////

int deny_name(char * name,NODE_DESC_string_list * list){
  // will break on multiple wild cards !!!!!!!!!!!!!!!!!!!!!!!!
  int denied = 0; // 1- means a match has been made in the list
  int x = 0; // scanning index
  int y = 0; // scanning index    
  int match = 0; // scan match flag
  int len = 0; // length for list items
  int ok = 0; // for direct match flag.

  while ( list->next != NULL ){
    len = strlen(list->item);
    if (len < 2){
      printf("Error in Config file. All will be rejected\n");
      return 2; // debug handle the return code!

    }   
    // '*' handle at beginning
    x = 1;
    y = 0;
    if(list->item[0] == '*'){
      // compare [1]+ with item
      while((list->item[x] != '\0') && (name[y] != '\0')){
        if(list->item[x] == name[y]){
          match = 1;
          x++;
        }else{
          match = 0;
          x = 1; // don't try to match wild card at beginning.
        }        
        y++;
      }
      if ((list->item[x] == '\0') && (name[y] == '\0') && (match==1)){
        return 1; // denied!
      } 
    } // endof handle at beginning
 
    // '*' handle at end
    match = 1;
    y = 0;
    x = 0;
    if(list->item[len-1] == '*'){
      // compare -len with item
      while((list->item[x] != '\0') && (name[y] != '\0') && (match==1)){
        if(list->item[x] == name[y]){
          match = 1;
          x++;
        } else {
          match = 0;
        }
        y++;
      }
      if ( match == 1 ){
        return 1; // denied!
      }
    }

    // '*' handle at middle
    x = 1;
    if ( len > 2 ){ // end or beginning will handle if 1 or 2
      while(( list->item[x] != '\0') && (x <= (len-2))){
        if (list->item[x] == '*' ){
          // compare the ends to item 
        }
        x++;
      }
    }
    // no wild cards
    // compare the item direct
    ok = 0;
    x = 0;
    while ((list->item[x] != '\0') && (ok != 1)){
      if(list->item[x] != name[x]){
        ok = 1;
      }
      x++;
    }

    if( denied ){ //list->item == name
      return 1;
    }
    list = list->next;
  }
return 0;
}

///////////////////////////////////////////////////////////////////////////////
//   decide if it is a desktop file with mask 
///////////////////////////////////////////////////////////////////////////////

int checkme(char * inquest, char * mask){
  // '*' handle at beginning
  int x = 1; 
  int y = 1; // something has to before '.' or else it will be a template
  int match = 0;
  if(mask[0] == '*'){ // confirm that we have a wildcard
    // compare [1]+ with item
    while((mask[x] != '\0') && (inquest[y] != '\0')){
      if(mask[x] == inquest[y]){
        match = 1;
        x++;
      }else{
        match = 0;
        x = 1; // don't try to match wild card at beginning.
      }        
      y++;
    }
    if ((mask[x] == '\0') && (inquest[y] == '\0') && (match==1)){
      return 1; // match has been obtained
    } 
  } // endof handle at beginning
  return 0; // no match
}


///////////////////////////////////////////////////////////////////////////////
// Find the name of the file to run 
///////////////////////////////////////////////////////////////////////////////

int substitute (char * filename , char * newname,char * keyword){
  int x = 0;
  int y = 0;
  FILE * cf = NULL;
  char buffer[1024];
  char * check;
  int match = 0;
  cf = fopen(filename,"r");
  if (cf > 0){
    while( check != NULL ){
      // read in line of file
      check =  fgets(buffer,sizeof(buffer),cf);
      x = 0;
//printf(">>> %s\n",buffer);
      y = 0;
      // find the keyword 
      while((buffer[x] != '\0') && (keyword[y] != '\0')){
        if(buffer[x] == keyword[y]){
          x++;
          y++;
          match = 1;
        }else{
          match = 0;
          break;
        }
      }
      if ( match == 1 ) { break; }
    }
    if ( match ==1 ){  
    // if found then sub it
      y = 0;
      // find first non white space after '='
      while(((buffer[x] == '=') || (buffer[x] == ' ')) && (buffer[x] != '\0')){
        x++;
      }
      while((buffer[x] != '\0') && (buffer[x] != '\n')){
        newname[y] = buffer[x];
        x++;
        y++;
      }
      newname[y] = '\0';
      fclose(cf);
      return 0;
    }else{
      printf("No match to keyword in file.\n");
      fclose(cf);
      return 1;
    }
  }else{
    printf("error opening file in 'substitute'\n");
    return 1;
  }
  printf("Never should get here.\n");
  fclose(cf); 
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Combine two stings 
///////////////////////////////////////////////////////////////////////////////

int combine(char * glued, char * first, char * second){

  int x = 0;
  int y = 0;
  int max = 0;
// debug report the actual size of the array, not the pointer size.
  max = sizeof(glued);
/*
  if ( sizeof(first)+sizeof(second) > max ){
    printf("Array size is not large enough to combine. max is %d, first=%d, seco=%d\n",max,sizeof(first),sizeof(second));
    return 1;
  }else{
*/
    while( first[x] != '\0' ){
      glued[x] = first[x];
      x++;
    }
    while( second[y] != '\0' ){
      glued[x++] = second[y++];
    }
    glued[x] = '\0';
//  }
  return 0;
} 
