/*          Name: Nisarg Patel
            GMU ID: G00944795
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>


/*--- Constants ---*/
enum{
    CHILD_PROCESS = 0,
    STDOUT = STDOUT_FILENO,
    STDIN = STDIN_FILENO,
    MAX_CMD_LENGTH = 1024
};


/******************************************************************************************************************/
/*                                      USER DEFINED FUNCTIONS                                                    */
/******************************************************************************************************************/

/*--- Removes extra white spacing from start, end and in between arguments ---*/
char *removeSpaces(char *str){
    
    char *start,*end;
    char *pos,*itr;
    
    /*  removes spaces from start position  */
    start = &(*str);
    while(isspace(*start)){
        start++;
    }
    
    /*  removes spaces from end position    */
    end = &(*(str+strlen(str)-1));
     while(isspace(*end)){
         end--;
     }
    
    *(end+1) ='\0';
    pos = start;
    itr = start;
    
    /*  removes spaces in between command and arguments */
    while(*itr){
        if(isspace(*itr) && isspace(*(itr+1))){
            itr++;
        }else{
            if(isspace(*itr)){
                *pos = ' ';
            }else{
                *pos = *itr;
            }
            itr++;
            pos++;
        }
    }
    *pos = '\0';

    return start;
}

/*--- detect redirection in order to choose relevant parser---*/
int rdirDetect(char *str){

    int count = 0;

    while(*str){
        /*  count occurence of > and <  */
        if(*str == '>' || *str == '<'){
            count++;
        }
        str++;
    }

    if(count > 0){
        /*  redirection detected    */
        return 1;
    }else{
        /*  redirection not found   */
        return 0;
    }

}

/*--- split string into tokens ---*/
void splitStr(char **cmd,char *str){

    int chrCount=0,strCount=0;
    int currPos=0,fetchPos=0;
    char *pToStr = &(*str);
    
    /*  Look for space and fetch word before that */
    while((pToStr=strchr(pToStr,' '))!=NULL){
        
        currPos = (int)(pToStr-str);
        
        /* fetch characters till space  */
        while(fetchPos<currPos){
            
            if(!(isspace(str[fetchPos]))){
                cmd[strCount][chrCount]= str[fetchPos];
                chrCount++;
            }
                fetchPos++;
        }
        chrCount=0;
        strCount++;
        fetchPos =currPos+1;
        pToStr++;
    }
    
    /*  fetch last argument before NULL termination */
    pToStr = str+fetchPos ;
    
    while(*pToStr){
  
        cmd[strCount][chrCount]= *pToStr;
        chrCount++;
        pToStr++;
    }
}

/*--- parser without redirection ---*/
void simpleParse(char **cmd,char *str){

    /*  Split Commnad string into tokens    */
    splitStr(cmd,str);   
}

/*--- Check for bad redirection ---*/
int chkBadRdir(char *str){
 
    char *pStr;
    int count=0;
    int flag =0;
    pStr = str;

    while(*pStr){
        if(*pStr == '<'){
            count++;
        }
        pStr++;
    }
    if(count > 1){
        flag++;
    }

    count = 0;
    pStr = str;
    while(*pStr){
        if(*pStr == '>' && *(pStr+1) =='>'){
            count++;
        }
        pStr++;
    }
    if(count > 1){
        flag++;
    }

    count = 0;
    pStr = str;
    while(*pStr){
        if(*pStr == '>' && *(pStr+1)!='>'){
            count++;
        }
        pStr++;
    }
    if(count >1){
        flag++;
    }
    return flag;
}


/*--- parser with redirection support ---*/
void rdirParser(char *str){
    
    if(chkBadRdir(str)){
        printf("error:bad redirection.\n");
    }else{
    
    }
    
}


/*  Execute shell command without redirections  */
void execute(char *str){

    int loop=0;
    int strCount=0;
    char **cmd;
    char *errStr,*pToStr;
    char *envp[]={"PATH=/bin:/usr/bin",NULL};

    pToStr = &(*str);
    
    /*  Count number of arguments   */
    while(*pToStr){
        if(isspace(*pToStr)){
            strCount++;
        }
        pToStr++;
    }

    /*  allocate memory and NULL terminate string array */
    cmd = (char **)malloc((strCount+2)*sizeof(char *));
    for(loop=0;loop<(strCount+2);loop++){
       cmd[loop]=(char *)malloc(sizeof(char)*strlen(str));
       memset(cmd[loop],'\0',strlen(str));
    }


    /* Parse Shell Command  */
   simpleParse(cmd,str);
   cmd[strCount+1]=NULL;

   /*  cd command implementation   */
   if(!(strcmp(cmd[0],"cd"))){

        /*if arguments are less*/    
       if(strCount < 1){
           
           errStr = "cd error:too few arguments\n";
           write(STDOUT,errStr,strlen(errStr));
       
       }else if(strCount<2){
           
           /*  execture chdir */
           if(chdir(cmd[1]) == -1){

               /* bad path*/
               if(errno == ENOENT){
                   errStr = "cd error: No such directory.\n";
                   write(STDOUT,errStr,strlen(errStr));
                }
               else{
                   errStr = "cd error: bad command.\n";
                   write(STDOUT,errStr,strlen(errStr));
               }
           }
        /*  if too many argunebts   */
       }else{
           errStr = "cd error:too many arguments\n";
            write(STDOUT,errStr,strlen(errStr));
       }
       
   /*  rm command implementation   */
   }else if(!(strcmp(cmd[0],"rm"))){
      
      /*    if too few arguments    */ 
       if(strCount < 1){
           errStr = "rm error:too few arguments.\n";
           write(STDOUT,errStr,strlen(errStr));

        /*  delete multiple files if arguments greater than 1   */
       }else{
           loop = 1;
           /*   loop to delete multi files  */
           while(cmd[loop] != NULL){
               
               if(unlink(cmd[loop]) == -1){
                   
                   if(errno == ENOENT){
                       errStr = "rm error: No such directory.\n";
                       write(STDOUT,errStr,strlen(errStr));
                   }else{
                        errStr = "rm error: bad command.\n";
                        write(STDOUT,errStr,strlen(errStr));
                   }
               }       
               loop++;
           }
       }

   /*   Exit command Implementation */
   }else if(!(strcmp(cmd[0],"exit"))){
       exit(1);

    /* ln command implementation    */
   }else if(!(strcmp(cmd[0],"ln"))){

       if(strCount <2){
           errStr = "ln error:too few arguments.\n";
           write(STDOUT,errStr,strlen(errStr));
       }else if(strCount<3){
           
           if(link(cmd[1],cmd[2])== -1){

               errStr = "ln error: No such file or directory.\n";
               write(STDOUT,errStr,strlen(errStr));
           }
         
       }else{
            errStr = "ln error: too many arguments\n.";
            write(STDOUT,errStr,strlen(errStr));
       }
   
   /* Execute external programs */
   }else{
       
       if(!fork()){
           /*   child process   */
           if(execve(cmd[0],cmd,envp)==-1){
               errStr = "execve error:bad command.\n";
               write(STDOUT,errStr,strlen(errStr));
           }
           exit(1);
           
        }
       else{
           wait(NULL);
       }
   }
    
   /*  free up the memory of token array   */
   for(loop=0;loop<strCount+2;loop++){
       free(cmd[loop]);
   }
   free(cmd);
    
}
/******************************************************************************************************************/
/*                                      MAIN FUCNTION                                                             */
/******************************************************************************************************************/

/*--- Shell's Main function ---*/
int main(void){

    char shellCmd[MAX_CMD_LENGTH];
    int cmdSize=0;
    int val;
    char *fmtStr;
    char *msg;
    pid_t pid;

    msg="****************shell**************\n";
    write(STDOUT,msg,strlen(msg));
   
    /*   fork Shell   */
    pid = fork();
    
    while(1){

        
        if(pid < 0){/*  if fork fails   */

            /* exit program */
            msg = "Forking failed!\n";
            write(STDOUT,msg,strlen(msg));
            exit(1);
        
        }else if(pid == CHILD_PROCESS){ 
            
            /*   child process section   */
            
            /*   Initialize with NULL  */
            memset(shellCmd,'\0',sizeof(shellCmd));

            /*   Shell prompt   */
        #ifndef NO_PROMPT
            write(STDOUT,"> ",2);
        #endif
            
            /*  Read shell command  */
            cmdSize=read(STDIN,shellCmd,MAX_CMD_LENGTH);

            /* Ctrl-D detection */
            if(cmdSize == 0){
                exit(1);
            }

             
            
            /* remove spaces from string */
            fmtStr = removeSpaces(shellCmd);
            
            /*  choose parser   */
            val = rdirDetect(fmtStr);
            
            if(val > 0){
                /*   parser with redirection support  */
                
                rdirParser(fmtStr);
            
            
            }else{
                /*   parser without redirection  */
                if(cmdSize >1){
                    
                    execute(fmtStr);
                
               }
            }           
        
        }else{
            /*  parent process */
            
            /*  wait for child process to finish  */
            wait(NULL);
            exit(1);
        }
    }
    return 0;
}

