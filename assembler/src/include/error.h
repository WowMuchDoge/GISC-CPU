#ifndef ERROR_H_
#define ERROR_H_

// Error a message that is formatted nicely
void printError(int lineNum, char *line, char *start, int len, char *location,
                char *msg, char *filename);

#endif
