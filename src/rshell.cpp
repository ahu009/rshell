#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <sys/dir.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

//parse command and push into array as separate command and arguments
void parseCommand(string command, char * args[]){
    vector<string> cmd;
    int i = 0;
    int j = 0;

    cmd.push_back(""); //insert element into vector

    //separate commands and arguments
    while (i < command.size()){ 
	//checks if should separate and if end of commandline
        if (command.at(i) == ' ' || i + 1 == command.size()){ 
	    if (i + 1 == command.size()){
	        cmd.at(j) = cmd.at(j) + command.at(i);
	    }	
	    j++;
	    if (command.at(i) == ' '){
	        cmd.push_back("");
	    }
	}
	else {
	    cmd.at(j) = cmd.at(j) + command.at(i);
	}
	    i++;
    }

    //push to array
    for (int i = 0; i < cmd.size(); i++){
        args[i] = (char*)cmd.at(i).c_str();
    }
    args[i] = NULL; //append NULL to array(must have for execVP()

}
void execVP(string command, bool& isFail){
    size_t found = command.find("exit");
    if (found != string::npos){
        exit(1);
    }
    char * args[100];
    for (int i = 0; i < 100; i++){
        args[i] = NULL; //flush array
    }

    parseCommand(command, args); //sets up char * array for execvp()

    pid_t c_pid, pid;
    int status;

    c_pid = fork();
  	
    //execvp forking/wait 
    if ( c_pid < 0)
    {
    	perror("fork failed");
    	exit(1);
    }

    else if (c_pid == 0)
    {                                                                                                                                                                      execvp(args[0], args);   
                                                     
    	perror("Command not recognized");
	exit(1);
    
    }
    else if (c_pid > 0)
    {
        if( (pid = wait(&status)) < 0)
    	{
      	    perror("wait");
      	    exit(1);
    	}
	if(WEXITSTATUS(status) != 0){ //checks if command failed
	    isFail = true;
	}
    }
}


//run all elements of vector in process into execVP
void process(vector<string> cmd, vector<string> connector){
    for (int i = 0; i < cmd.size(); i++){
	bool isFail = false;
	execVP(cmd.at(i), isFail);
	//functionality for || and && (multiple commands)
	if (connector.at(i) == "||" && !isFail){
	    //cout << "|| hit" << endl;
	    i++;
	}
	else if (connector.at(i) == "&&" && isFail){
	    //cout << "&& hit" << endl;
	    i++;
	}
    }
}

//pushes all connectors in command line to a vector 
void connector2Vector (char command[], vector<string>& connector){
    for (int i = 0; i < 1000; i++){
        if (command[i] == ';'){
            connector.push_back(";");
	}
	else if (i < 999 && command[i] == '&' && command[i+1] == '&'){
	    connector.push_back("&&");
	}
	else if (i < 999 && command[i] == '|' && command[i+1] == '|'){
	    connector.push_back("||");
	}
    }
    connector.push_back("end");
}

//clear Array
void clearArray(char command[]){
    for (int i = 0; i < 1000; i++){
	command[i] = '\0';
    }
}
//pushes separate commands in command line to a vector
void push2Vector (char command[], vector<string>& Cmd){
    char * pch;
    pch = strtok (command,"||;&&");
    while (pch != NULL)
    {
        string temp = pch;
 	pch = strtok (NULL, "||;&&");
        Cmd.push_back(temp);
    }
}
//formats commands: deletes spaces before individual commands in vector
void formatVector(vector<string>& cmd){
    for (int i = 0; i < cmd.size(); i++){
	if (cmd.at(i).at(0) == ' '){
	    cmd.at(i).erase(0,1);
	} 
	while (cmd.at(i).at(cmd.at(i).size() - 1) == ' '){
	    cmd.at(i).erase(cmd.at(i).size() - 1, 1);
	}
    }
}
//prints a vector
void printVector(vector<string> cmd){
    cout << "Contents of Command Vector: ";
    for (int i = 0; i < cmd.size(); i++){
        cout << cmd.at(i) << endl;
    }
    cout << endl;
}

//prints array
void printArray(char command[]){
    cout << "Contents of Command Array: ";
    for (int i = 0; i < 1000; i++){
        cout << command[i];
    }
    cout << endl;
}

//removes all characters after the found '#' in the array
void checkComments(char command[]){
    bool isPound = false;
    for (int i = 0; i < 1000; i++){
	if (!isPound && command[i] == '#'){
	    isPound = true;
	}
	if (isPound){
	    command[i] = '\0';
	}
    }
}

int main()
{
    //continous loop
    while (1){
        char command[1000]; //Array to hold command line
        clearArray(command); //clears declared array
	vector<string> cmd; //vector to hold multiple single commands
	vector<string> connector; //vector to hold connectors
	
	printf("rshell$ ");
	cin.getline(command, 1000); //get command
			
		
	checkComments(command); //exclused comments in command line
	
	//Vectors for connectors and commands respectively
	connector2Vector(command, connector); 
	push2Vector(command, cmd);

	formatVector(cmd); //formats vector holding commands
	
	clearArray(command);
	process(cmd, connector);
	clearArray(command); //flushes array
    }
    return 0;
}
