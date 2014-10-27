// XRT Window Manager Main
//
// Copyright Bob Wickson 2012/01/01
// XRT Corporation
//
// exhale - Window Manager

#include "exhale.h"
int parse_to_run(char *,char ** );


int main(int argc, char **argv){

///////////////////////////////////////////////////////////////////////////////
//declare and initalize variables
///////////////////////////////////////////////////////////////////////////////
  NODE_DESC * list = NULL;
  NODE_DESC * listtop = NULL;
  NODE_DESC * list_item = NULL;
  NODE_DESC * list_oldi = NULL;
  NODE_DESC * gpl = NULL;

  // Set up X server prime
  Display *display;
  int screen_num;
  Window win;
  unsigned int width = 200; // window to start from this size
  int iwidth = 200 - 20; // list width to start from this size
  unsigned int height= 300; // window to start from this size
  int x_start = 10; // initial x point of window
  int y_start = 10; // initial y point of window
  unsigned border = 4; // initial border width
  char *window_name = "Exhale";
  char *icon_name   = "Exhale";
  XEvent x_event;
  XGCValues xgcvalues;
  XFontStruct *font_info;
  int fonty = 14; // currently hard code font height
  int selection = 0; // what array point is selected
  GC gc; // ID of gc that is first diplay
  GC gc2; // ID of gc that is drawings 
  GC gc3; // ID of gc that is changes
  char *display_name = NULL;
  int count = 0;
  int mcount = 0;
  int x = 0;

  printf(" Copyright 2014/04/04 Compiled on %s\n",__DATE__);

  // Note when menud is not '\0' initiated then weird things happen
  char menud[256];
  count = 0;
  for (count =0;count<256;menud[count++]='\0'){}

  // So do the same for runme
  char runme[(256+40)];// menud size plus some for the file to execute
  count = 0;
  for (count =0;count<(256+40);runme[count++]='\0'){}

  // So do the same for torun 
  char torun[(256)];// name to actually execute: no parm
  count = 0;
  for (count =0;count<(256);torun[count++]='\0'){}

  int exme = 0;

  count = 0;
  int prev_hi = -1; // previously hilighted bar in motion of mouse over selections
/*
// debug stuff	
  // check config passed in main
  printf("Config from CLI/GUI => %d ...\n",argc);
  for(count=0;count<argc;count++){
    printf("%s\n",argv[count]);
  }
// endof debug
*/
///////////////////////////////////////////////////////////////////////////////
// Window setup
///////////////////////////////////////////////////////////////////////////////
// try to connect to the X server
  display = XOpenDisplay(display_name);
  if ((display=XOpenDisplay(display_name))==NULL ) {
    printf("Unable to open X Server\n");
    exit(-1);
  }
// get screen and display basic window
  screen_num = DefaultScreen(display);
  win = XCreateSimpleWindow(display,RootWindow(display,screen_num),
    x_start,
    y_start,
    width,
    height,
    border,
    BlackPixel(display,screen_num),
    WhitePixel(display,screen_num));
  count = 0;
  int num_fonts = 0;

///////////////////////////////////////////////////////////////////////////////
// Set up gcs to te used
///////////////////////////////////////////////////////////////////////////////
  gc = XCreateGC (display, 
    win,
    (unsigned long) 0, // # of fields set in values
    &xgcvalues); // value structure
  gc2 = XCreateGC (display, 
    win,
    (unsigned long) 0, // # of fields set in values
    &xgcvalues); // value structure
  gc3 = XCreateGC (display, 
    win,
    (unsigned long) 0, // # of fields set in values
    &xgcvalues); // value structure

///////////////////////////////////////////////////////////////////////////////
// Set up fonts to be used
///////////////////////////////////////////////////////////////////////////////

  char ** fontarray;
  fontarray = XListFonts(display,"*",700,&num_fonts);
/* // print out the font names
  while(count<num_fonts){
    printf("%d > %s\n",count,fontarray[count]);
    count++;
  }
*/
  count = 269; // Make this our font. Why? because it looks ok.
// if you have a better font choice, speek up!
//  count = 575;
  count = 290;

  if ((font_info = XLoadQueryFont(display,fontarray[count])) == NULL ){
    // debug \\printf("\nUsing fixed font. Font requested not available.\n");
    font_info = XLoadQueryFont(display,"fixed");
  }
//debug // printf("Font loaded <%d> = %s\n",count,fontarray[count]);

// Add the fonts to the gcs
  XSetFont(display,gc,font_info->fid);
  XSetFont(display,gc2,font_info->fid);
  XSetFont(display,gc3,font_info->fid);

// Add the colours to the gcs
  XSetForeground(display,gc ,0X000000);// Black
  XSetBackground(display,gc ,0XFFFFFF);// White
  XSetForeground(display,gc2,0XFFFFFF);
  XSetBackground(display,gc2,0X00FFFF);
  XSetForeground(display,gc3,0XFF00FF);
  XSetBackground(display,gc3,0X0000FF);

// Done setting up the gcs

///////////////////////////////////////////////////////////////////////////////
// Deal with the config file.
///////////////////////////////////////////////////////////////////////////////
// If the dir the config file is passed to the program then use it.
// We assume the 2nd arg is the dir passed.
  count = 0;
  if ( argc == 2 ){
    while ( (argv[1][count]) != '\0' ){
      menud[count]=argv[1][count];
      count++;
    }
  }

// Load the configuration file
  if ( load_config(menud) != 0 ) {
    printf("The configuration file failed to load!\n");
  }else{
    printf("The configuration file did load.\n");
  }

///////////////////////////////////////////////////////////////////////////////
// Create list of menu items, set various information about list 
///////////////////////////////////////////////////////////////////////////////
  int y = 0;	
  init_list(); // creates empty list 

  list = fill_list(gpl,menud); // load menu list
  listtop = list; // keep a record of the top of the list, 'list' will change
// find the number of list items 
  count = 0;
  int listmax = 0;
  while( (list != NULL) ){
    list=list->last;
    count++;
  }
  listmax = count;	
  // set width of item list
//this is set at declaration but we may want to change the width ie vari width
  iwidth = width - 20; // 20 should be changed to be variable on font size
  // 20 should become a variable so it can be changed and/or be used later

///////////////////////////////////////////////////////////////////////////////
// Final settings for the window
///////////////////////////////////////////////////////////////////////////////

// Set properties
  XSetStandardProperties(display,
    win,
    window_name,
    icon_name,
    None,
    0,0,
    NULL);

// Set masks
  XSelectInput(display,win,ExposureMask|ButtonPressMask|PointerMotionMask|StructureNotifyMask);

// Window is ready to go, so map it.
  XMapWindow(display,win);
       
///////////////////////////////////////////////////////////////////////////////
// Main loop of events
///////////////////////////////////////////////////////////////////////////////
  while (1){
    XNextEvent(display, &x_event);
    switch (x_event.type) {

////////// - Redraw anything we need - //////////
    case Expose:
      list = listtop;
      mcount = 0;
      y = fonty;
      while( (list != NULL) && (mcount>=0)){ // 3 lines at bottom
        XDrawString(display,win,gc,20,y,list->dis_name,strlen(list->dis_name));                        
        y = y + fonty;
        mcount = height - y - ( 3 * fonty );
        list = list->last;
      }
			
      XDrawString(display,win,gc,40,(height-15),"Button 3 to Close",
                                                strlen("Button 3 to Close"));
      // Calculate centering value for string
      XDrawString(display,win,gc,23,(height),"Exhale menuing system.",
                                                strlen("Exhale menuing system."));
      XDrawString(display,win,gc,20,(height-30),menud,strlen(menud));
      break;

    case ConfigureNotify:
      // this is to adjust the menu spacing
      height = x_event.xconfigure.height; //  new height from the exposure?
      break;

    case MotionNotify:
////////// Any motion - hilights selection under cursor - clears hilights elseware - //////////
      // Note that the string is redrawn after the fill. We want it on top.
      selection = (x_event.xbutton.y -1) / fonty;
      list=listtop;
      count = 0;
      mcount = 0;
      y = fonty;
      while( (list != NULL) && (count < listmax) && (mcount>=0)){
        if(selection==count){ list_item = list; }
        y = y + fonty;
        mcount = height - y - ( 3 * fonty );
        list=list->last;
        count++;
      }
      if ( selection < count ){
        XFillRectangle(display,win,gc3,18,((selection*fonty)+2),(iwidth-2),fonty); 
        XDrawString(display,win,gc,20,((selection*fonty)+14),list_item->dis_name,strlen(list_item->dis_name));                        
        if ((prev_hi > -1 )&&(prev_hi != selection)){
          XFillRectangle(display,win,gc2,18,((prev_hi*fonty)+2),(iwidth-2),fonty);
          // this assumes that list_oldi is not NULL if we are filling the rectangle
          XDrawString(display,win,gc,20,((prev_hi*fonty)+14),list_oldi->dis_name,strlen(list_oldi->dis_name));                        
        }
        prev_hi = selection;
        list_oldi = list_item;
      } else {
        if( (prev_hi >= 0) && (list_oldi != NULL)){
          XFillRectangle(display,win,gc2,18,((prev_hi*fonty)+2),(iwidth-2),fonty); 
          XDrawString(display,win,gc,20,((prev_hi*fonty)+14),list_oldi->dis_name,strlen(list_oldi->dis_name)); 
          prev_hi = -1;
          list_oldi = NULL;
        } 
      }
      break;
		
    case ButtonPress:
////////// Button 1 - Select the item to run - //////////
      if (x_event.xbutton.button == 1){
        printf(" x = %d ,,, y = %d \n",x_event.xbutton.x,x_event.xbutton.y);
        if ( x_event.xbutton.y < ( height - (3 * fonty) )){
        selection = (x_event.xbutton.y -1) / fonty;
//
        // rectangle is hard coded for a size : change me
        XFillRectangle(display,win,gc2,18,((selection*fonty)+2),(iwidth-2),fonty);
        list=listtop;
        count = 0;
        while((count<selection) && (list != NULL)){
          list=list->last;
          count++;
	}
        if ( list != NULL ){
	  XDrawString(display,win,gc3,20,((selection*fonty)+14),list->dis_name,strlen(list->dis_name));
          // This is the program to execute
          // first add the name to the menu dir
          count = 0;
          do{
            runme[count] = menud[count]; 
          } while(runme[count++] != '\0');
          if (runme[count-2] == '/'){
            mcount = --count;
          } else {
            runme[count-1] = '/';
            mcount = count;
          }

          do{
            runme[count] = list->name[count-mcount];
          } while(runme[count++] != '\0');
// if the file is a directory then run exhalemenu again
// else close up everything and run program
//debug//	  
          printf("Checking if the name is executable or a .desktop file\n");
          exme = checkme(runme,"*.desktop"); // checkme if name is a *.desktop
          torun[0]='\0';
          if (exme != 0){
            substitute(runme, torun, "Exec");// file - exec - type keyword
//debug//            printf("runme = [%s]\n",runme);
//debug//            printf("torun = [%s]\n",torun);
          }else{
            // copy runme to torun
            x = 0;
            while(runme[x] != '\0'){
              torun[x] = runme[x];
              x++;
              }
          }
              // move the declarations to a more sensible place.
              //char *newargv[] = { NULL, "hello", "world", NULL };
             char *newargv[] = { "  \n",NULL,NULL };
              char *newenviron[] = { NULL };
          //parse the run parameters
          parse_to_run(torun,newargv);
 
// debug //
          printf("name to run is  ->%s<-\n",torun);
          
         //execvp(torun,NULL);
         execvp(torun,newargv);
         // execve(torun, newargv, newenviron);
          perror("execvp");
// debug //
          printf(" If the file was not executable then we made it here.\n");

        } else {
//debug//          printf(" list is **** NULL *****\n");
        }
        } else { // bottom three lines don't select
          printf("Bottom three lines...\n");
        }
      }
////////// Button 2 - *Space holding* - //////////
//      if (x_event.xbutton.button == 2){
//        XDrawString(display,win,gc,20,250,"Button two",strlen("Button two"));
//      }
////////// Button 3 - Closes the program //////////
      if (x_event.xbutton.button == 3){
        XCloseDisplay(display);
        exit(0);
      }
      break;

    default:
      break;
    }
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// End of main loop	
///////////////////////////////////////////////////////////////////////////////

int parse_to_run(char * torun,char * newargv[]){
  int x = 0;
  // this finds the end of the exec file name
  while((torun[x] != '\0') && (torun[x] != ' ')){
    x++;
  }
  if ( torun[x] == ' ' ){
    // this indicates that there are parameters
    torun[x] = '\0'; // stop the file name at this point
// debug this !! must capture the parameters !!

    return 0; // all is good with parameters  
  } else if ( torun[x] == '\0') {
    // there is no parameters evident
    return 0; // all is good
  } else {
    return 1; // we should not get here
  }
}
