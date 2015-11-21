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
#include <sys/stat.h>

using namespace std;

//deletes whitespaces before and after command
void formatString(string& cmd){
	while(cmd.at(0) == ' '){
		cmd.erase(0,1);
	}
	while(cmd.at(cmd.size() - 1) == ' '){
		cmd.erase(cmd.size() - 1, 1);
	}
}

//turns string into cstring
const char* string2cstring(string cmd){
	return cmd.c_str();
}

//checks if user input is directory
bool isDirectory(const char* cmd){
	struct stat fileStat;
	if (stat(cmd, &fileStat) < 0){
		return false;
	}
	if (S_ISDIR(fileStat.st_mode)){
		return true;
	}
	else{
		return false;
	}
}

//checks if user input is a file
bool isFile(const char* cmd){
	struct stat fileStat;
	if (stat(cmd, &fileStat) < 0){
		return false;
	}
	if (S_ISREG(fileStat.st_mode)){
		return true;
	}
	else{
		return false;
	}
} 

//checks if user input exists
bool doesExist(const char* cmd){
	struct stat fileStat;
	if (stat(cmd, &fileStat) < 0){
		return false;
	}
	else{
		return true;
	}
}

//checks to see if user uses '[ ]' symbol to test 
bool isSymbol(string& cmd){
	bool openFound = false;
	bool closeFound = false;
	int indexOpen;
	int indexClose;
	for (int i = 0; i < cmd.size(); i++){
		if (cmd.at(i) == '['){
			indexOpen = i;
			openFound = true;
		}
		if (cmd.at(i) == ']'){
			indexClose = i;
			closeFound = true;
		}
	}
	if (closeFound && openFound){
		cmd.erase(indexOpen, 1);
		cmd.erase(indexClose - 1, 1);
		formatString(cmd);
		return true;
	}
	else{
		return false;
	}
}

//checks if user uses 'test' keyword to test
bool isTest(string& cmd){
	size_t found = cmd.find("test ");
	if (found != string::npos){
		if (found < 1){
			cmd.erase(0,5);
			formatString(cmd);
			return true;
		}
		else{
			return false;
		}
	}
	else{
		return false;
	}
} 

//checks if user uses '-e ' flag 
//and removes the flag from string if true
bool eFlag(string& cmd){
	size_t found = cmd.find("-e ");
	if (found != string::npos){
		if (found < 1){
			cmd.erase(0,3);
			formatString(cmd);
			return true;
		}
		else{
			return false;
		}
	}
	else{
		return false;
	}	
}

//checks if user uses '-f ' flag 
//and removes the flag from string if true
bool fFlag(string& cmd){
	size_t found = cmd.find("-f ");
	if (found != string::npos){
		if (found < 1){
			cmd.erase(0,3);
			formatString(cmd);
			return true;
		}
		else{
			return false;
		}
	}
	else{
		return false;
	}	
}

//checks for '-d ' flag
//removes if true
bool dFlag(string& cmd){
	size_t found = cmd.find("-d ");
	if (found != string::npos){
		if (found < 1){
			cmd.erase(0,3);
			formatString(cmd);
			return true;
		}
		else{
			return false;
		}
	}
	else{
		return false;
	}	
}

//checks if command has opening paranthesis
//removes if true
bool hasOpenPara(string& cmd){
	for (int i = 0; i < cmd.size(); i++){
		if (cmd.at(i) == '('){
			cmd.erase(i,1);
			return true;
		}
	}
	return false;
}

//checks if command has closing paranthesis
//removes if true
bool hasClosePara(string& cmd){
	for (int i = 0; i < cmd.size(); i++){
		if (cmd.at(i) == ')'){
			cmd.erase(i,1);
			return true;
		}
	}
	return false;
}

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
    if (c_pid < 0)
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
        if ((pid = wait(&status)) < 0)
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
    bool hasParanthesis = false;
    bool blockPass = false;
    bool hadPara = false;
    bool didSkip = false;
    bool testResult = false;
    bool hasTest = false;
    for (int i = 0; i < cmd.size(); i++){
	if (hadPara){
		hadPara = false;
	}
	if (hasOpenPara(cmd.at(i))){
		blockPass = false;
	}
	if (hasClosePara(cmd.at(i))){
		hadPara = true;
	}
	hasTest = false;
	if (isTest(cmd.at(i))){
		hasTest = true;
		if (fFlag(cmd.at(i))){
			testResult = isFile(string2cstring(cmd.at(i)));
		}
		else if (dFlag(cmd.at(i))){
			testResult = isDirectory(string2cstring(cmd.at(i)));
		}
		else if(eFlag(cmd.at(i))){
			testResult = doesExist(string2cstring(cmd.at(i)));
		}
		else{
			testResult = doesExist(string2cstring(cmd.at(i)));
		}	
	}
	else if (isSymbol(cmd.at(i))){
		hasTest = true;
		if (fFlag(cmd.at(i))){
			testResult = isFile(string2cstring(cmd.at(i)));	
		}
		else if (dFlag(cmd.at(i))){
			testResult = isDirectory(string2cstring(cmd.at(i)));
		}
		else if (eFlag(cmd.at(i))){
			testResult = doesExist(string2cstring(cmd.at(i)));
		}
		else{
			testResult = doesExist(string2cstring(cmd.at(i)));
		}
	}
	

	bool isFail = false; //checks if command fails
	if (hasTest){
		isFail = !testResult;
	}
	else{
		execVP(cmd.at(i), isFail);
	}

	if (!isFail){
		blockPass = true;
	}
	if (i - 1 >= 0 && isFail && connector.at(i - 1) == "&&"){
		blockPass = false;
	}

	if (!hadPara){
	//functionality for || and && (multiple commands)
		if (connector.at(i) == "||" && !isFail){
			if (hasClosePara(cmd.at(i + 1))){
			//	cout << "hit" << endl;
				didSkip = true;
			}
			else{
				didSkip = false;
			}
			i++;
		}
		else if (connector.at(i) == "&&" && isFail){
	    		//cout << "&& hit" << endl;
			//blockPass = false;
			if (hasClosePara(cmd.at(i + 1))){
				didSkip = true;
			}
			else{
				didSkip = false;
			}
			i++;
		}
	}
	if (didSkip){
		hadPara = true;
	}
	if (hadPara){
		if (connector.at(i) == "||" && blockPass){
			if(hasOpenPara(cmd.at(i + 1))){
				i++;
				while (!hasClosePara(cmd.at(i))){
					i++;
				}
			}
			else{
				i++;
			}
		}
		else if (connector.at(i) == "&&" && !blockPass){
	  		if(hasOpenPara(cmd.at(i + 1))){
				i++;
				while (!hasClosePara(cmd.at(i))){
					i++;
				}
			}
			else{
	    			i++;
			}
		}
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
//deletes spaces before individual commands in vector
void formatVector(vector<string>& cmd){
    for (int i = 0; i < cmd.size(); i++){
	while (cmd.at(i).at(0) == ' '){
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
