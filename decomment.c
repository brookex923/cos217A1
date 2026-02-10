#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

/* Global variable to keep track of what line we are on. 
   This is necessary for our error message that specifies 
   what line the undetermined comment started.*/
static int line = 1; 
/* Global variable to keep track of where a comment started, 
   used for error reporting. */
static int commentStartLine = 0; 

/* List of DFA state used for this program */
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

/* Handle one input character c while in the NORMAL state.
   Writes to stdout as needed.
   Returns the next DFA state. */
enum Statetype
handleNormalState(int c)
{
    enum Statetype state;
    if (c == '/') {
        /* we do not print out / yet because we need to 
           see if this is the start of a comment */
        state = SAW_SLASH; 
    } 
    else if (c == '"') {
        putchar(c);
        state = IN_STRING; /* begin string literal */
    } 
    else if (c == '\'') {
        putchar(c);
        state = IN_CHAR; /* begin char literal */
    } 
    else {
        /* we are still in normal code, just print out 
        the character and stay in the normal state */
        putchar(c);
        state = NORMAL; 
    }
    return state;
}

/* Handle input character c after a '/' was seen (SAW_SLASH).
   May write '/', c, or a space to stdout. If c == '*', sets
   commentStartLine and enters IN_COMMENT. Returns next state. */
enum Statetype
handleSawSlashState(int c)
{
    enum Statetype state;
    if (c == '*') {
        /* replace comment with a space by printing out a space */
        putchar(' '); 
        /* record where comment started for error reporting purposes */
        commentStartLine = line;   
        /* directly transition to the IN_COMMENT state*/
        return IN_COMMENT; 
    } 

    if (c == '/') {
        /* print out the previous slash since we didn't before */
        putchar('/'); 
        /* return to SAW_SLASH state again since this 
           second slash could be the start of a new comment*/
        return SAW_SLASH; 
    }

    /* for all other characters, print out both / and the 
       character. Transition to the next states accordingly */

    /* need to print the slash because we didn't in the normal state */
    putchar('/'); 
    /* print the current character since we are not in a  */
    putchar(c); 

    if (c == '"') {
        state = IN_STRING;
    }
    else if (c == '\'') {
        state = IN_CHAR;
    }
    else {
        state = NORMAL;
    }
    
    return state;
}


/* Handle input character c while inside a comment (IN_COMMENT).
   Writes '\n' to stdout to preserve newlines; otherwise writes
   nothing. Returns the next DFA state. */
enum Statetype
handleInCommentState(int c)
{
    enum Statetype state;
    if (c == '*') {
        state = SAW_STAR_IN_COMMENT;
    } 
    else if (c == '\n') {
        /* prints a new line to keep spacing 
           aligned with original input */
        putchar('\n'); 
        state = IN_COMMENT;
    } 
    else {
        state = IN_COMMENT;
    }
    return state;
}

/* Handle input character c after seeing '*' in a comment.
   Writes '\n' to stdout if c is '\n'; otherwise writes nothing.
   Returns the next DFA state. */
enum Statetype
handleSawStarInCommentState(int c)
{
    enum Statetype state;
    if (c == '/') {
        /* We have just seen /*, meaning we are done with the comment 
           Transition back to normal state */
        state = NORMAL; 
    } 
    else if (c == '*') {
        state = SAW_STAR_IN_COMMENT;
    }
    else if (c == '\n') {
        /* prints a new line to keep spacing 
           aligned with original input */
        putchar('\n'); 
        state = IN_COMMENT;
    }   
    else {
        state = IN_COMMENT;
    }
    return state;
}

/* Handle input character c while inside a string literal.
   Writes c to stdout. Returns ESCAPE_IN_STRING if c is '\\',
   NORMAL if c is '"', otherwise IN_STRING. */
enum Statetype
handleInStringState(int c)
{
    enum Statetype state;

    putchar(c); /* always output characters inside a string */

    if (c == '\\') {
        /* next character is escaped, so 
           transition to ESCAPE_IN_STRING state */
        state = ESCAPE_IN_STRING; 
    } 
    else if (c == '"') {
        /* end of string, return back to NORMAL state */
        state = NORMAL; 
    }
    else {
        state = IN_STRING;
    }
    return state;
}

/* Handle input character c after a backslash in a string.
   Writes c to stdout and returns IN_STRING. */
enum Statetype
handleEscapeInStringState(int c)
{
    putchar(c); /* print the escaped character */
    return IN_STRING; /* go back to the IN_STRING state */
}

/* Handle input character c while inside a character literal.
   Writes c to stdout. Returns ESCAPE_IN_CHAR if c is '\\',
   NORMAL if c is '\'', otherwise IN_CHAR. */
enum Statetype
handleInCharState(int c)
{
    enum Statetype state;

    putchar(c); /* always output characters inside a char literal */

    if (c == '\\') {
        state = ESCAPE_IN_CHAR; /* need to account for escaped chars */
    }
    else if (c == '\'') {
        state = NORMAL; /* end of char, return back to NORMAL state */
    }
    else {
        state = IN_CHAR;
    }
    return state;
}

/* Handle input character c after a backslash in a char literal.
   Writes c to stdout and returns IN_CHAR. */
enum Statetype
handleEscapeInCharState(int c)
{
    putchar(c); /* print the escaped character */
    return IN_CHAR; /* go back to the In_Char state */
}

/* Read characters from stdin, remove C-style comments, and write
   the result to stdout. Preserves newlines inside comments.
   On unterminated comment, writes an error to stderr and returns
   EXIT_FAILURE; otherwise returns EXIT_SUCCESS. */
int main(void)
{
    int c;
    enum Statetype state = NORMAL; /* NORMAL is our stating state */

    /* Read input character-by-character until EOF */
    while ((c = getchar()) != EOF) {
        /* Track current line number for error reporting */
        if (c == '\n') {
            /* increment our global variable 'line' by 1 
               when we encounter a new line character */
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

    /* If the last character is a slash, it's not a comment 
       starter and we need to print it out 
       since our NORMAL state does not. */
    if (state == SAW_SLASH) {
        putchar('/');
        return EXIT_SUCCESS;
    }

    /* Unterminated comment so we need to exit with an error */
    if (state == IN_COMMENT || state == SAW_STAR_IN_COMMENT) {
        fprintf(stderr, 
                "Error: line %d: unterminated comment\n", 
                commentStartLine);

        return EXIT_FAILURE;
    }

    /* Normal successful termination */
    return EXIT_SUCCESS;
}