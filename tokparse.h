#ifndef PARSE_H
#define PARSE_H

// Statuses for tokenizer
#define TOK_NORMAL          (0)
#define TOK_COMMENT         (1)
#define TOK_COMMENT_ESC     (2)
#define TOK_STRING          (3)
#define TOK_STRING_ESC      (4)

// Statuses for parser
#define PARSE_NORMAL        (0)
#define PARSE_IF_S0         (1)
#define PARSE_BR2_S0        (2)
#define PARSE_BR2_S1        (3)
#define PARSE_BRS_S0        (4)
#define PARSE_BRS_S1        (5)
#define PARSE_BRS_S2        (6)
#define PARSE_BR_S0         (7) // Emit literal and branch word (no SKP)
#define PARSE_BR_S1         (8) // Emit SKP2, literal and branch word
#define PARSE_BR_S2         (9) // Emit 
#define PARSE_BR_S3         (10)
#define PARSE_DO_S0         (11)
#define PARSE_RP_S0         (12)
#define PARSE_VAR_S0        (13)
#define PARSE_ASGN_S0       (14)
#define PARSE_DICT_NEW      (15)
#define PARSE_DICT_EXIST    (16)
#define PARSE_COMPILE_S0    (17) // Awaiting word name
#define PARSE_COMPILE_S1    (18) // Awaiting more commands or ;

int process_line(char * line);

#endif