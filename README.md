This is Homework 2 for CS100 at UC Riverside. This is a basic shell written in C++. Added on the "test"([ ]) functionality onto homework 1 as long as precedence using paranthesis.
                                                              
Bugs:
- Breaks input when inserting peculiar amount of whitespace into command line
- Breaks when using connector without following command
- Sometimes does not recognize known commmands when using comments in certain way.
                                                                                             
Design: 
- added code into "process" function
- any changes / additional features should be added into "process" function
- created ~5 more functions for functionality of test keyword with flags "-e, -f, -d".
	- parsed command in form of string for each flag. 
	- parsed command for opening and closing brackets, '[' and ']'
	- parsed command for "test".
- created ~3 more functions for functionality of precendence.
