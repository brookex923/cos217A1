#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>


static int line = 1; /* global variable to keep track of what line we are on*/
static int commentStartLine = 0; /* global variable to keep track of where a comment started, used for error reporting */

enum Statetype {
    NORMAL,
    SAW_SLASH,
    IN_COMMENT,
    SAW_STAR_IN_COMMENT,
    IN_STRING,
    ESCAPE_IN_STRING,
    IN_CHAR,
    ESCAPE_IN_CHAR
};


/* seperate code for normal state */
enum Statetype
handleNormalState(int c)
{
    enum Statetype state;
    if (c == '/') {
        state = SAW_SLASH;
    } 
    else if (c == '"') {
        putchar(c);
        state = IN_STRING;
    } 
    else if (c == '\'') {
        putchar(c);
        state = IN_CHAR;
    } 
    else {
        putchar(c);
        state = NORMAL;
    }
    return state;
}

enum Statetype
handleSawSlashState(int c)
{
    enum Statetype state;
    if (c == '*') {
        putchar(' '); /* replace comment with a space */
        commentStartLine = line;   /* record where comment started */
        state = IN_COMMENT;
    } 
    else if (c == '"') {
        state = IN_STRING;
    }
    else if (c == '\'') {
        state = IN_CHAR;
    }
    else {
        state = NORMAL;
    }
    putchar('/'); /* need to print the slash because we didn't in the normal state */
    putchar(c); /* print the current character since we are not in a comment*/
    return state;
}

enum Statetype
handleInCommentState(int c)
{
    enum Statetype state;
    if (c == '*') {
        state = SAW_STAR_IN_COMMENT;
    } 
    else if (c == '\n') {
        putchar(c); /* prints a new line*/
        state = IN_COMMENT;
    } 
    else {
        state = IN_COMMENT;
    }
    return state;
}

enum Statetype
handleSawStarInCommentState(int c)
{
    enum Statetype state;
    if (c == '/') {
        state = NORMAL; /* we are done with the comment, go back to normal*/
    } 
    else if (c == '*') {
        state = SAW_STAR_IN_COMMENT;
    }
    else if (c == '\n') {
        putchar(c); /* prints a new line*/
        state = IN_COMMENT;
    }   
    else {
        state = IN_COMMENT;
    }
    return state;
}

enum Statetype
handleInStringState(int c)
{
    enum Statetype state;
    putchar(c);
    if (c == '\\') {
        state = ESCAPE_IN_STRING; /* need to account for backslashes in strings */
    } 
    else if (c == '"') {
        state = NORMAL; /* end of string */
    }
    else {
        state = IN_STRING;
    }
    return state;
}

enum Statetype
handleEscapeInStringState(int c)
{
    putchar(c); /* print the escaped character */
    return IN_STRING; /* go back to the In_String state */
}

enum Statetype
handleInCharState(int c)
{
    enum Statetype state;
    putchar(c);
    if (c == '\\') {
        state = ESCAPE_IN_CHAR; /* need to account for backslashes in chars */
    } 
    else if (c == '\'') {
        state = NORMAL; /* end of char */
    }
    else {
        state = IN_CHAR;
    }
    return state;
}

enum Statetype
handleEscapeInCharState(int c)
{
    putchar(c); /* print the escaped character */
    return IN_CHAR; /* go back to the In_Char state */
}

int main(void)
{
    int c;
    enum Statetype state = NORMAL;
    while ((c = getchar()) != EOF) {
        if (c == '\n') {
        line++;
        }

        switch (state) {
        case NORMAL:
            state = handleNormalState(c);
            break;
        case SAW_SLASH:
            state = handleSawSlashState(c);
            break;
        case IN_COMMENT:
            state = handleInCommentState(c);
            break;
        case SAW_STAR_IN_COMMENT:
            state = handleSawStarInCommentState(c);
            break;
        case IN_STRING:
            state = handleInStringState(c);
            break;          
        case ESCAPE_IN_STRING:
            state = handleEscapeInStringState(c);
            break;
        case IN_CHAR:
            state = handleInCharState(c);
            break;
        case ESCAPE_IN_CHAR:
            state = handleEscapeInCharState(c);
            break;
        default:
            break;
        }   
    }

    /* If the last character is a slash, it's not a comment starter and we need to print it out */
    if (state == SAW_SLASH) {
        putchar('/');
        return EXIT_SUCCESS;
    }

    /* Unterminated comment so we need to exit with an error*/
    if (state == IN_COMMENT || state == SAW_STAR_IN_COMMENT) {
        fprintf(stderr, "Error: line %d: unterminated comment\n", commentStartLine);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}