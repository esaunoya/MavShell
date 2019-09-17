/*

  Name: Esau Noya
  ID:   1001301929

*/

// The MIT License (MIT)
//
// Copyright (c) 2016, 2017 Trevor Bakker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 10    // Mav shell only supports five arguments
                                // Edit: To satisfy requirement 9, changed from
                                // 5 to 10

#define MAX_HISTORY 15          // The max number of most-recent commands to be kept

#define MAX_PIDS 15             // The max number of most-recent precesses spawned to be kept

int main()
{

  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );

  // command history list
  // MAX_HISTORY sets ceiling for number of Commands
  char history[MAX_HISTORY][MAX_NUM_ARGUMENTS];

  //list of pids
  pid_t listpids[MAX_PIDS];

  // directories where shell shall attempt to execute entered commands in given path order
  const char *dirs[] = {"./", "/usr/local/bin/", "/usr/bin/", "/bin/"};


  // keeps track of number of commands that have been called throughout session
  int count = 0;

  pid_t pid;

  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;

    // Pointer to point to the token
    // parsed by strsep
    char *arg_ptr;

    char *working_str  = strdup( cmd_str );

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input stringswith whitespace used as the delimiter
    while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) &&
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    // Now print the tokenized input as a debug check
    // \TODO Remove this code and replace with your shell functionality

    // int token_index  = 0;
    // for( token_index = 0; token_index < token_count; token_index ++ )
    // {
    //   printf("token[%d] = %s\n", token_index, token[token_index] );
    // }

    // SHELL FUNCTIONALITY

    // Avoid segmentation faults when no input is given.
    if( token[0] == NULL )
    {
      // do nothing :)
    }
    // 'cd DIRECTORY' will change current directory
    else if( strcmp( "cd", token[0]) == 0 && token[1] != NULL )
    {
      // failure to find specified directory occurs
      // will alert user directory not found
      if(chdir(token[1]) == -1){
        printf( "-msh: cd: %s: No such file or directory\n" , token[1] );
      }

    }
    // 'history' displays the MAX_HISTORY last commands
    else if( strcmp("history", token[0]) == 0 )
    {
      //for loop variables
      int i;
      int max;

      // find final value of for loop
      // set to count to start but if larger than MAX_HISTORY then set to that
      max = count;
      if( max > MAX_HISTORY )
      {
        max = MAX_HISTORY;
      }

      // print history
      for( i = 0; i < max; i++ )
      {
        printf( "%d: %s\n" , i , history[i] );
      }
    }
    // 'history' displays the MAX_HISTORY last commands
    else if( strcmp("listpids", token[0]) == 0 )
    {
      //for loop variables
      int i;
      int max;

      // find final value of for loop
      // set to count to start but if larger than MAX_PIDS then set to that
      max = count;
      if( max > MAX_PIDS )
      {
        max = MAX_PIDS;
      }

      // print pids
      for( i = 0; i < max; i++ )
      {
        printf( "%d: %d\n" , i , listpids[i] );
      }
    }
    // 'exit' or 'quit' will terminate the shell
    else if( ( strcmp("exit", token[0]) == 0 ) || (strcmp("quit", token[0]) == 0 ) )
    {
      exit(0);
    }

    pid = fork();

    // add current PID to pid list
    if(count <= MAX_PIDS)
    {
      listpids[count] = pid;
    }
    // if number of PIDS exceeds MAX_PIDS, move all pids up by 1
    // in listpids[] and store newest at end
    else
    {
      // for loop value
      int i;

      for(i = 0; i < MAX_PIDS; i++)
      {
        listpids[i] = listpids[i+1];
      }
      listpids[MAX_PIDS] = pid;
    }

    //child pid
    if(pid == 0)
    {

      // for loop value
      int i;

      // command not found counter
      int commNotFoundInPath = 0;

      // current directory / directory where execl will be attempted
      char cd[MAX_COMMAND_SIZE];

      // loop through and attempt to  command in directories
      for(i = 0; i < 4; i++)
      {
        // copy PATH to cd
        strcpy( cd , dirs[i] );
        // concatenates cd and token[0]
        strcat( cd , token[0] );

        if( execl( cd, token[0], token[1], token[2], token[3], token[4],
                       token[5], token[6], token[7], token[8], token[9], NULL ) )
        {
          //execl only returns if there is an error, so increment commNotFound;
          commNotFoundInPath++;
        }
      }
      // Command was not found in any of the 4 paths
      if(commNotFoundInPath == 4)
      {
        printf("%s: Command not found.\n", token[0]);
      }

      exit(0);
    }
    //parent pid
    else
    {
      //wait for child
      wait(NULL);
    }

    //increment counter
    count++;
    free( working_root );

  }

  return 0;
}
