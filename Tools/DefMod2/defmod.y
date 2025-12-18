/*defmod.y---grammar for a .swi file*/

/*OSLib---efficient, type-safe, transparent, extensible,\n"
   register-safe API coverage of RISC OS*/
/*Copyright (c) 1994 Jonathan Coxhead*/

/*
      OSLib is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

      OSLib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
   along with this programme; if not, write to the Free Software
   Foundation, Inc, 675 Mass Ave, Cambridge, MA 02139, U S A.
*/

/* Revision History:
JRC            Original Release
TV    19990418    Modification originally devised by Tom Hughes:
                  Allows Constants to be declared
TV    20000503    |bool| replaced by |osbool|
*/

%{ /*C declarations*/

   /*From CLib*/
   #include <ctype.h>
   #include <errno.h>
   #include <stddef.h>
   #include <stdlib.h>
   #include <string.h>
   #include <kernel.h>
   #include <swis.h>

   /*From OSLib*/
   #include "oslib/types.h"
   #include "oslib/fileswitch.h"
   #include "oslib/macros.h"
   #include "oslib/os.h"
   #include "oslib/osfind.h"

   /*From Support*/
   #include "hostfs.h"
   #include "lookup.h"
   #include "trace.h"

   /*Local*/
   #include "assembler.h"
   #include "asmhelp.h"
   #include "chelp.h"
   #include "def.h"
   #include "cheader.h"
   #include "hdr.h"
   #include "main.h"
   #include "cstrong.h"
   #include "utils.h"
   #include "vapi.h"
   #include "vapi_h.h"
   #include "vapi_deps.h"

#ifdef PACE_BUILD
   #include "VersionNum"
   #define VERSION_INFO Module_FullVersion " [" Module_Date "]"
#else
#  define VERSION_INFO ", " __DATE__
#endif

/* Set this to 1 to make APCS 3/32 the default */
#define DEFAULT_TO_APCS_32 1
#if DEFAULT_TO_APCS_32
#  define HELP_STRING_26 ""
#  define HELP_STRING_32 "  [default]"
#else
#  define HELP_STRING_26 "  [default]"
#  define HELP_STRING_32 ""
#endif

   static char *strstrc (char *, const char *, char);
   static char *strcatw (char *, const char *, const char *);

   static lookup_t needses, needsatends, consts, types, swis;
   static char Title [256],
      Author [256] = "Anon",
      Description [def_DESCRIPTION_LIMIT + 1] = "";

   lookup_t main_byte_wide;

   static osbool Verbose = FALSE, Parse_Error = FALSE;

   osbool Quiet = FALSE;

   static int yylex (void);

   static void yyerror (char *);

   static int yyparse(void);    /* TV 990128 */

   extern int yydebug;

   static int Line_No = 1;

   static void *qalloc (size_t);

   static struct def_s Union (struct def_s, struct def_s), Empty;

   #define DISJUNCTION(p1, p2) (p1 != 0 ? p1: p2)
%}

/*Bison declarations*/
%start file

%union
{  /*type for a single character*/
   char c;

   /*token types*/
   int num;
   char id [def_ID_LIMIT + 1];
   int reg;
   osbool option;
   char description [def_DESCRIPTION_LIMIT + 1];

   /*definition-in-progress types*/
   struct def_t type_defn;
   struct def_s swi_defn;
}

%type <swi_defn> swi number_part condition_part_OPTION condition_part
      entry_part exit_part entry_condition_LIST entry_condition
      exit_condition_LIST exit_condition absent_part description
      description_OPTION

%type <type_defn> base_type base_type_OPTION type toid typed_var_LIST
      typed_var toided_var_LIST toided_var

%type <description> wordchar_SEQUENCE word word_SEQUENCE word_SEQUENCE_OPTION
      DESCRIPTION DESCRIPTION_OPTION

%type <option> ellipsis_OPTION pling_OPTION

%type <num> NUM num dec_num hex_num bin_num char_num digit_SEQUENCE
      hexit_SEQUENCE bit_SEQUENCE numchars const

%type <id> ID id_cont_SEQUENCE id_cont_SEQUENCE_OPTION

%type <reg> REG

%type <c> a b c d e f g h i j k l m n o p q r s t u v w x y z
      letter bit digit hexit wordchar numchar simplechar compoundchar
      escapedchar id_start id_cont spacechar

%%
/*Grammar rules*/

/*Metarules (written in yet another form of BNF !):

   EMPTY:             ;
   <notion>_OPTION:   <notion> | EMPTY;
   <notion>_SEQUENCE: <notion> | <notion>_SEQUENCE <notion>;
   <notion>_LIST:     <notion> | <notion>_LIST     COMMA          <notion>;
   <notion>_SERIES:   <notion> | <notion>_SERIES   SEMICOLON      <notion>;

   <token>: token ws;
*/

file: ws decl_SERIES_OPTION;

decl: title_decl | author_decl | needs_decl | needsatend_decl
      | const_decl | type_decl | swi_decl;

title_decl: TITLE ID DESCRIPTION_OPTION
      {strcpy (Title, $2); strcpy (Description, $3);};

author_decl: AUTHOR DESCRIPTION {strcpy (Author, $2);};

needs_decl: NEEDS needs_LIST {};

needs: ID
   {  os_error *error;

      tracef( "needs: Inserting \"%s\"\n" _ $1 );
      if ((error = lookup_insert (needses, $1, NULL)) != NULL)
      {
         tracef( "Error %s\n" _ error -> errmess );
         yyerror (error->errmess);
         YYERROR;
      }
   };

needsatend_decl: NEEDSATEND needsatend_LIST {};

needsatend: ID
   {  os_error *error;

      tracef( "needsatend: Inserting \"%s\"\n" _ $1 );
      if ((error = lookup_insert (needsatends, $1, NULL)) != NULL)
      {
         tracef( "Error %s\n" _ error -> errmess );
	 yyerror (error->errmess);
         YYERROR;
      }
   };

const_decl: CONST const_defn_LIST;

const_defn:
   ID EQUALS type COLON const DESCRIPTION_OPTION
   {  os_error *error;
      tracef( "const_defn\n");
      def_c c = qalloc (sizeof *c);
      def_t t = qalloc (def_sizeof_TYPE ($3.tag));

      memcpy (t, &$3, def_sizeof_TYPE ($3.tag));
      c->type  = t;
      c->value = $5;
      c->description = EMPTY ($6)? NULL: qstrdup ($6);

      tracef( "Inserting \"%s\"\n" _ $1 );
      if ((error = lookup_insert (consts, $1, c)) != NULL)
      {
         tracef( "Error %s\n" _ error -> errmess );
         yyerror (error->errmess);
         YYERROR;
      }
   };

const:
   NUM |
   ID
   {  os_error *error;
      def_c c;

      tracef( "const: looking up: \"%s\"\n" _ $1 );
      if ((error = lookup (consts, $1, (void **) &c)) != NULL)
      {
         tracef( "Error %s\n" _ error -> errmess );
         yyerror (error->errmess);
         YYERROR;
      }

      if (c == NULL)
      {  char errmess [os_ERROR_LIMIT];

         sprintf (errmess, "const \"%s\" undefined", $1);
         yyerror (errmess);
         YYERROR;
      }

      $$ = c->value;
   };

type_decl: TYPE type_defn_LIST;

type_defn:
   ID DESCRIPTION_OPTION
   {  os_error *error;
      tracef( "type_defn\n" );
      def_t t = qalloc (def_sizeof_TYPE (def_TYPE_ABSTRACT));

      t->tag = def_TYPE_ABSTRACT;
      t->name = NULL;
      t->value = def_VALUE_REGISTER;
      t->description = EMPTY ($2)? NULL: qstrdup ($2);

      tracef( "Inserting \"%s\"\n" _ $1 );
      if ((error = lookup_insert (types, $1, t)) != NULL)
      {
         tracef( "Error %s\n" _ error -> errmess );
         yyerror (error->errmess);
         YYERROR;
      }
   } |
   ID EQUALS type DESCRIPTION_OPTION
   {  os_error *error;
      tracef( "type_defn\n" );
      def_t t = qalloc (def_sizeof_TYPE ($3.tag));

      memcpy (t, &$3, def_sizeof_TYPE ($3.tag));
      t->description = EMPTY ($4)? NULL: qstrdup ($4);

      tracef( "Inserting \"%s\"\n" _ $1 );
      if ((error = lookup_insert (types, $1, t)) != NULL)
      {
         tracef( "Error %s\n" _ error -> errmess );
         yyerror (error->errmess);
         YYERROR;
      }
   };

base_type:
   COLON ID
   {  /*Use it verbatim, but check the type does exist (and find out if
         it's a struct type or not).*/
      os_error *error;
      def_t t;

      tracef( "base_type: looking up: \"%s\"\n" _ $2 );
      if ((error = lookup (types, $2, (void **) &t)) != NULL &&
            error->errnum != os_GLOBAL_NO_ANY)
      {
         tracef( "Error %s\n" _ error -> errmess );
         yyerror (error->errmess);
         YYERROR;
      }

      $$.tag = def_TYPE_ID;
      $$.name = NULL;
      if (error != NULL)
      {  /*have to make an optimistic assumption*/
         $$.value = def_VALUE_VARIABLE;
         if (Verbose && !Quiet)
            fprintf (stderr, "Warning: type \"%s\" undefined at line "
                  "%d\n", $2, Line_No);
      }
      else
         $$.value = t != NULL? t->value: def_VALUE_VARIABLE;
      $$.description = NULL;
      strcpy ($$.data AS id, $2);
   };

type:
   INT
   {$$.tag = def_TYPE_INT;   $$.name = NULL;
         $$.value = def_VALUE_REGISTER; $$.description = NULL;} |
   SHORT
   {$$.tag = def_TYPE_SHORT; $$.name = NULL;
         $$.value = def_VALUE_REGISTER; $$.description = NULL;} |
   BYTE
   {$$.tag = def_TYPE_BYTE;  $$.name = NULL;
         $$.value = def_VALUE_REGISTER; $$.description = NULL;} |
   CHAR
   {$$.tag = def_TYPE_CHAR;  $$.name = NULL;
         $$.value = def_VALUE_REGISTER; $$.description = NULL;} |
   BITS
   {$$.tag = def_TYPE_BITS;  $$.name = NULL;
         $$.value = def_VALUE_REGISTER; $$.description = NULL;} |
   BOOL
   {$$.tag = def_TYPE_BOOL;  $$.name = NULL;
         $$.value = def_VALUE_REGISTER; $$.description = NULL;} |
   ASM
   {$$.tag = def_TYPE_ASM; $$.name = NULL;
         $$.value = def_VALUE_REGISTER; $$.description = NULL;} |
   REF type
   {
      tracef( "type\n" );
      def_t t = qalloc (def_sizeof_TYPE ($2.tag));

      memcpy (t, &$2, def_sizeof_TYPE ($2.tag));
      $$.tag = def_TYPE_REF;
      $$.name = NULL;
      $$.value = def_VALUE_REGISTER;
      $$.description = NULL;
      $$.data AS ref = t;
   } |
   STRING
   {$$.tag = def_TYPE_STRING; $$.name = NULL;
         $$.value = def_VALUE_VARIABLE; $$.description = NULL;} |
   DATA
   {$$.tag = def_TYPE_DATA; $$.name = NULL;
         $$.value = def_VALUE_VARIABLE; $$.description = NULL;} |
   STRUCT base_type_OPTION OPEN typed_var_LIST ellipsis_OPTION CLOSE
   {  $$ = $4;
      $$.tag = def_TYPE_STRUCT;
      if ($5)
         $$.value = def_VALUE_VARIABLE;
      else if ($4.value == def_VALUE_REGISTER)
         $$.value = def_VALUE_FIXED;
      $$.description = NULL;
      if ($2.tag != def_TYPE_VOID)
      {
         tracef( "type\n" );
         $$.data AS list.base = qalloc (def_sizeof_TYPE ($2.tag));
         memcpy ($$.data AS list.base, &$2, def_sizeof_TYPE ($2.tag));
      }
      else
         $$.data AS list.base = NULL;
      $$.data AS list.ellipsis = $5;
   } |
   UNION OPEN toided_var_LIST CLOSE
   {  $$ = $3;
      $$.tag = def_TYPE_UNION;
      $$.description = NULL;
   } |
   SUB const BUS type
   {
      tracef( "type\n" );
      def_t t = qalloc (def_sizeof_TYPE ($4.tag));

      if ($4.value == def_VALUE_VARIABLE)
      {  yyerror ("type cannot occur in an array");
         YYERROR;
      }

      memcpy (t, &$4, def_sizeof_TYPE ($4.tag));
      $$.tag = def_TYPE_ROW;
      $$.name = NULL;
      $$.value = def_VALUE_FIXED; /*there are no arrays of undefined size*/
      $$.description = NULL;
      $$.data AS row.count = $2;
      $$.data AS row.base = t;
   } |
   ID
   {  /*Use it verbatim, but check the type does exist (and find out if
         it's a register type or not).*/
      os_error *error;
      def_t t;

      tracef( "type: looking up ID: \"%s\"\n" _ $1 );
      if ((error = lookup (types, $1, (void **) &t)) != NULL &&
            error->errnum != os_GLOBAL_NO_ANY)
      {
         tracef( "Error %s\n" _ error -> errmess );
      	 yyerror (error->errmess);
         YYERROR;
      }

      $$.tag = def_TYPE_ID;
      $$.name = NULL;
      if (error != NULL)
      {  /*have to make an optimistic assumption*/
         $$.value = def_VALUE_REGISTER;
	 tracef( "type \"%s\" undefined\n" _ $1 );
         /* Check if this unknown type reasonably ressembles the known
            standard types without starting a dot.  */
         if (!ustricmp ($1, "Int")
             || !ustricmp ($1, "Short")
             || !ustricmp ($1, "Byte")
             || !ustricmp ($1, "Char")
             || !ustricmp ($1, "Bits")
             || !ustricmp ($1, "Bool")
             || !ustricmp ($1, "Asm")
             || !ustricmp ($1, "String")
             || !ustricmp ($1, "Data"))
            fprintf (stderr, "Warning: type \"%s\" looks like a built-in type but isn't at line %d\n", $1, Line_No);
         else if (Verbose && !Quiet)
            fprintf (stderr, "Warning: type \"%s\" undefined at line "
                  "%d\n", $1, Line_No);
      }
      else
         $$.value = t != NULL? t->value: def_VALUE_REGISTER;
      $$.description = NULL;
      strcpy ($$.data AS id, $1);

      tracef( "type: ID done\n" );
   };

toid: type | VOID {$$.tag = def_TYPE_VOID; $$.name = NULL;
      $$.description = NULL;};

typed_var: type COLON ID DESCRIPTION_OPTION
   {
      tracef( "type\n" );
      $$ = $1;
      $$.name = qstrdup ($3);
      $$.description = EMPTY ($4)? NULL: qstrdup ($4);
   };

toided_var: toid COLON ID DESCRIPTION_OPTION
   {
      tracef( "type\n" );
      $$ = $1;
      $$.name = qstrdup ($3);
      $$.description = EMPTY ($4)? NULL: qstrdup ($4);
   };

swi_decl: SWI swi_defn_LIST;

swi_defn:
   ID EQUALS swi
   {  os_error *error;
      tracef( "type\n" );
      def_s s = qalloc (sizeof *s);

      *s = $3;

      tracef( "Inserting \"%s\"\n" _ $1 );
      if ((error = lookup_insert (swis, $1, s)) != NULL)
      {
         tracef( "Error %s\n" _ error -> errmess );
         yyerror (error->errmess);
         YYERROR;
      }
   };

swi: OPEN number_part condition_part_OPTION CLOSE {$$ = Union ($2, $3);};

number_part: NUMBER NUM description_OPTION
      {$$ = $3; $$.swi = $2;};

condition_part: entry_part | entry_part exit_part {$$ = Union ($1, $2);} |
      entry_part absent_part {$$ = $1; $$.absent = TRUE;} |
      exit_part | absent_part;

entry_part: COMMA ENTRY OPEN entry_condition_LIST CLOSE {$$ = $4;};

exit_part: COMMA EXIT OPEN exit_condition_LIST CLOSE {$$ = $4;};

absent_part: COMMA ABSENT {$$ = Empty; $$.absent = TRUE;};

entry_condition:
   REG CONTAINS typed_var
      {
         tracef( "entry_condition\n" );
         if ($3.value != def_VALUE_REGISTER)
         {
	    tracef( "Error: type cannot occur in a register\n" );
	    yyerror ("type cannot occur in a register");
            YYERROR;
         }
         $$ = Empty;
         $$.i |= 1 << $1;
         $$.inputs [$1] = qalloc (def_sizeof_TYPE ($3.tag));
	 tracef( "memcpy to 0x%x\n" _ $$.inputs [$1] );
         memcpy ($$.inputs [$1], &$3, def_sizeof_TYPE ($3.tag));
	 tracef( "done\n" );
      } |
   REG REFERENCES typed_var
      {
         tracef( "entry_condition\n" );
         $$ = Empty;
         $$.i |= 1 << $1;
         $$.ri |= 1 << $1;
         $$.inputs [$1] = qalloc (def_sizeof_TYPE ($3.tag));
         memcpy ($$.inputs [$1], &$3, def_sizeof_TYPE ($3.tag));
      } |
   REG CONSTANT const description_OPTION
      {  $$ = $4;
         if ($$.starred_swi)
            /*Move the flag to the right place.*/
            $$.starred_constants [$1] = TRUE, $$.starred_swi = FALSE;
         $$.k |= 1 << $1;
         $$.constants [$1] = $3;
      } |
   REG DISJOINS typed_var
      {
         tracef( "entry_condition\n" );
         if ($3.value != def_VALUE_REGISTER)
         {  yyerror ("type cannot occur in a register");
            YYERROR;
         }
         $$ = Empty;
         $$.i |= 1 << $1;
         $$.inputs [$1] = qalloc (def_sizeof_TYPE ($3.tag));
         memcpy ($$.inputs [$1], &$3, def_sizeof_TYPE ($3.tag));
         $$.op [$1] = def_OP_DISJOINS;
      } |
   REG CONJOINS typed_var
      {  yyerror ("& not supported");
         YYERROR;
      } |
   REG ADDS typed_var
      {
         tracef( "entry_condition\n" );
         if ($3.value != def_VALUE_REGISTER)
         {  yyerror ("type cannot occur in a register");
            YYERROR;
         }
         $$ = Empty;
         $$.i |= 1 << $1;
         $$.inputs [$1] = qalloc (def_sizeof_TYPE ($3.tag));
         memcpy ($$.inputs [$1], &$3, def_sizeof_TYPE ($3.tag));
         $$.op [$1] = def_OP_ADDS;
      } |
   REG EXCLUSIVELY_DISJOINS typed_var
      {
         tracef( "entry_condition\n" );
         if ($3.value != def_VALUE_REGISTER)
         {  yyerror ("type cannot occur in a register");
            YYERROR;
         }
         $$ = Empty;
         $$.i |= 1 << $1;
         $$.inputs [$1] = qalloc (def_sizeof_TYPE ($3.tag));
         memcpy ($$.inputs [$1], &$3, def_sizeof_TYPE ($3.tag));
         $$.op [$1] = def_OP_EXCLUSIVELY_DISJOINS;
      } |
   FLAGS
      {  $$ = Empty;
         $$.f_in = 1;
      };

exit_condition:
   REG pling_OPTION CONTAINS typed_var
      {
         tracef( "exit_condition\n" );
         if ($4.value != def_VALUE_REGISTER)
         {  yyerror ("type cannot occur in a register");
            YYERROR;
         }
         $$ = Empty;
         if ($2) $$.value = 1 << $1;
         $$.o |= 1 << $1;
         $$.outputs [$1] = qalloc (def_sizeof_TYPE ($4.tag));
         memcpy ($$.outputs [$1], &$4, def_sizeof_TYPE ($4.tag));
      } |
   REG pling_OPTION REFERENCES typed_var
      {
         tracef( "exit_condition\n" );
         $$ = Empty;
         if ($2) $$.value = 1 << $1;
         $$.o |= 1 << $1;
         $$.ro |= 1 << $1;
         $$.outputs [$1] = qalloc (def_sizeof_TYPE ($4.tag));
         memcpy ($$.outputs [$1], &$4, def_sizeof_TYPE ($4.tag));
      } |
   REG CORRUPTED /*a corrupted register can't be returned*/
      {$$ = Empty; $$.c |= 1 << $1;} |
   REG PLING
      {$$ = Empty; $$.value = 1 << $1;} |
   FLAGS pling_OPTION
      {  $$ = Empty;
         if ($2) $$.value = 1 << def_FLAGS;
         $$.f_out = 1;
      };

description:
   DESCRIPTION
      {  tracef ("DESCRIPTION \"%s\"\n" _ $1);
         $$ = Empty;
         $$.description = qstrdup ($1);
         $$.starred_swi = TRUE;
      } |
   STAR
      {  $$ = Empty;
         $$.description = NULL;
         $$.starred_swi = TRUE;
      };

/*------------------------------------------------------------------------*/
/*Lexical rules*/

ws: ws_item_SEQUENCE_OPTION {};
nl: '\n' {Line_No++;} | '\r' {Line_No++;};
space: spacechar {} | nl {};
ws_item: space {} | comment {};
comment: '/' '/' commentchar_SEQUENCE_OPTION nl {};

TITLE:  t i t l e   ws {tracef ("TITLE\n");};
AUTHOR: a u t h o r ws {tracef ("AUTHOR\n");};
NEEDS:  n e e d s   ws_item_SEQUENCE {tracef ("NEEDS\n");};
NEEDSATEND:  n e e d s a t e n d  ws {tracef ("NEEDSATEND\n");};
CONST:  c o n s t   ws {tracef ("CONST\n");};
TYPE:   t y p e     ws {tracef ("TYPE\n");};
SWI:    s w i       ws {tracef ("SWI\n");};

NUMBER: n u m b e r ws {tracef ("NUMBER\n");};
ENTRY:  e n t r y   ws {tracef ("ENTRY\n");};
EXIT:   e x i t     ws {tracef ("EXIT\n");};
ABSENT: a b s e n t ws {tracef ("ABSENT\n");};

FLAGS:  f l a g s   ws {tracef ("FLAGS\n");};

CONTAINS:             '='     ws {};
REFERENCES:           '-' '>' ws {};
CONSTANT:             '#'     ws {};
DISJOINS:             '|'     ws {};
CONJOINS:             '&'     ws {};
ADDS:                 '+'     ws {};
EXCLUSIVELY_DISJOINS: '^'     ws {};
CORRUPTED:            '?'     ws {};
STAR:                 '*'     ws {};
PLING:                '!'     ws {};

INT:    '.' i n t        ws {};
SHORT:  '.' s h o r t    ws {};
BYTE:   '.' b y t e      ws {};
CHAR:   '.' c h a r      ws {};
BITS:   '.' b i t s      ws {};
BOOL:   '.' b o o l      ws {};
ASM:    '.' a s m        ws {};

VOID:   '.' v o i d      ws {};
STRING: '.' s t r i n g  ws {};
DATA:   '.' d a t a      ws {};

STRUCT:   '.' s t r u c t ws {};
UNION:    '.' u n i o n   ws {};
REF:      '.' r e f       ws {};
ELLIPSIS: '.' '.' '.'     ws {};

SEMICOLON: ';' ws {};
COMMA:     ',' ws {};
EQUALS:    '=' ws {};
OPEN:      '(' ws {};
CLOSE:     ')' ws {};
COLON:     ':' ws {};
SUB:       '[' ws {};
BUS:       ']' ws {};

ID: id_start id_cont_SEQUENCE_OPTION ws
      {$$ [0] = $1; strcpy (&$$ [1], $2);};

NUM: num ws;

REG: r digit ws {$$ = DIGIT ($2);};

DESCRIPTION: '"' wordchar_SEQUENCE word_SEQUENCE_OPTION '"' ws
      {strcatw ($$, $2, $3);};
word: space_SEQUENCE wordchar_SEQUENCE {strcpy ($$, $2);};
wordchar: simplechar | '\\' {$$ = '\\';}| '\'' {$$ = '\'';};

a: 'a' {$$ = 'a';} |
   'A' {$$ = 'A';} |
   /*�*/ '\xC2' {$$ = 0xC2;} |
   /*�*/ '\xE2' {$$ = 0xE2;} |
   /*�*/ '\xC4' {$$ = 0xC4;} |
   /*�*/ '\xE4' {$$ = 0xE4;} |
   /*�*/ '\xC1' {$$ = 0xC1;} |
   /*�*/ '\xE1' {$$ = 0xE1;} |
   /*�*/ '\xC0' {$$ = 0xC0;} |
   /*�*/ '\xE0' {$$ = 0xE0;} |
   /*�*/ '\xC3' {$$ = 0xC3;} |
   /*�*/ '\xE3' {$$ = 0xE3;} |
   /*�*/ '\xC5' {$$ = 0xC5;} |
   /*�*/ '\xE5' {$$ = 0xE5;} |
   /*�*/ '\xC6' {$$ = 0xC6;} |
   /*�*/ '\xE6' {$$ = 0xE6;};
b: 'b' {$$ = 'b';} |
   'B' {$$ = 'B';};
c: 'c' {$$ = 'c';} |
   'C' {$$ = 'C';} |
   /*�*/ '\xC7' {$$ = 0xC7;} |
   /*�*/ '\xE7' {$$ = 0xE7;};
d: 'd' {$$ = 'd';} |
   'D' {$$ = 'D';} |
   /*�*/ '\xD0' {$$ = 0xD0;} |
   /*�*/ '\xF0' {$$ = 0xF0;};
e: 'e' {$$ = 'e';} | 'E' {$$ = 'E';} |
   /*�*/ '\xCA' {$$ = 0xCA;} |
   /*�*/ '\xEA' {$$ = 0xEA;} |
   /*�*/ '\xCB' {$$ = 0xCB;} |
   /*�*/ '\xEB' {$$ = 0xEB;} |
   /*�*/ '\xC9' {$$ = 0xC9;} |
   /*�*/ '\xE9' {$$ = 0xE9;} |
   /*�*/ '\xC8' {$$ = 0xC8;} |
   /*�*/ '\xE8' {$$ = 0xE8;};
f: 'f' {$$ = 'f';} | 'F' {$$ = 'F';} |
   /*�*/ '\x9E' {$$ = 0x9E;} |
   /*�*/ '\x9F' {$$ = 0x9F;};
g: 'g' {$$ = 'g';} | 'G' {$$ = 'G';};
h: 'h' {$$ = 'h';} | 'H' {$$ = 'H';};
i: 'i' {$$ = 'i';} | 'I' {$$ = 'I';} |
   /*�*/ '\xCE' {$$ = 0xCE;} |
   /*�*/ '\xEE' {$$ = 0xEE;} |
   /*�*/ '\xCF' {$$ = 0xCF;} |
   /*�*/ '\xEF' {$$ = 0xEF;} |
   /*�*/ '\xCD' {$$ = 0xCD;} |
   /*�*/ '\xED' {$$ = 0xED;} |
   /*�*/ '\xCC' {$$ = 0xCC;} |
   /*�*/ '\xEC' {$$ = 0xEC;};
j: 'j' {$$ = 'j';} | 'J' {$$ = 'J';};
k: 'k' {$$ = 'k';} | 'K' {$$ = 'K';};
l: 'l' {$$ = 'l';} | 'L' {$$ = 'L';};
m: 'm' {$$ = 'm';} | 'M' {$$ = 'M';};
n: 'n' {$$ = 'n';} | 'N' {$$ = 'N';} |
   /*�*/ '\xD1' {$$ = 0xD1;} |
   /*�*/ '\xF1' {$$ = 0xF1;};
o: 'o' {$$ = 'o';} | 'O' {$$ = 'O';} |
   /*�*/ '\xD4' {$$ = 0xD4;} |
   /*�*/ '\xF4' {$$ = 0xF4;} |
   /*�*/ '\xD6' {$$ = 0xD6;} |
   /*�*/ '\xF6' {$$ = 0xF6;} |
   /*�*/ '\xD3' {$$ = 0xD3;} |
   /*�*/ '\xF3' {$$ = 0xF3;} |
   /*�*/ '\xD2' {$$ = 0xD2;} |
   /*�*/ '\xF2' {$$ = 0xF2;} |
   /*�*/ '\xD5' {$$ = 0xD5;} |
   /*�*/ '\xF5' {$$ = 0xF5;} |
   /*�*/ '\x9A' {$$ = 0x9A;} |
   /*�*/ '\x9B' {$$ = 0x9B;} |
   /*�*/ '\xD8' {$$ = 0xD8;} |
   /*�*/ '\xF8' {$$ = 0xF8;};
p: 'p' {$$ = 'p';} | 'P' {$$ = 'P';};
q: 'q' {$$ = 'q';} | 'Q' {$$ = 'Q';};
r: 'r' {$$ = 'r';} | 'R' {$$ = 'R';};
s: 's' {$$ = 's';} | 'S' {$$ = 'S';} |
   /*�*/ '\xDF' {$$ = 0xDF;};
t: 't' {$$ = 't';} | 'T' {$$ = 'T';} |
   /*�*/ '\xDE' {$$ = 0xDE;} |
   /*�*/ '\xFE' {$$ = 0xFE;};
u: 'u' {$$ = 'u';} | 'U' {$$ = 'U';} |
   /*�*/ '\xDB' {$$ = 0xDB;} |
   /*�*/ '\xFB' {$$ = 0xFB;} |
   /*�*/ '\xDC' {$$ = 0xDC;} |
   /*�*/ '\xFC' {$$ = 0xFC;} |
   /*�*/ '\xDA' {$$ = 0xDA;} |
   /*�*/ '\xFA' {$$ = 0xFA;} |
   /*�*/ '\xD9' {$$ = 0xD9;} |
   /*�*/ '\xF9' {$$ = 0xF9;};
v: 'v' {$$ = 'v';} | 'V' {$$ = 'V';};
w: 'w' {$$ = 'w';} | 'W' {$$ = 'W';} |
   /*�*/ '\x81' {$$ = 0x81;} |
   /*�*/ '\x82' {$$ = 0x82;};
x: 'x' {$$ = 'x';} | 'X' {$$ = 'X';};
y: 'y' {$$ = 'y';} | 'Y' {$$ = 'Y';} |
   /*�*/ '\x85' {$$ = 0x85;} |
   /*�*/ '\x86' {$$ = 0x86;} |
   /*�*/ '\xFF' {$$ = 0xFF;} |
   /*�*/ '\xDD' {$$ = 0xDD;} |
   /*�*/ '\xFD' {$$ = 0xFD;};
z: 'z' {$$ = 'z';} | 'Z' {$$ = 'Z';};

letter: a | b | c | d | e | f | g | h | i | j | k | l | m | n | o | p | q |
      r | s | t | u | v | w | x | y | z;
bit: '0' {$$ = '0';}| '1' {$$ = '1';};
digit: bit | '2' {$$ = '2';} | '3' {$$ = '3';} |
      '4' {$$ = '4';} | '5' {$$ = '5';} | '6' {$$ = '6';} |
      '7' {$$ = '7';} | '8' {$$ = '8';} | '9' {$$ = '9';};
hexit: digit | a | b | c | d | e | f;

id_start: letter;

id_cont: letter | digit | '_' {$$ = '_';};

num: dec_num | hex_num | bin_num | char_num;

dec_num: digit_SEQUENCE {$$ = $1;} | '-' digit_SEQUENCE {$$ = -$2;};

hex_num: '0' x hexit_SEQUENCE {$$ = $3;} | '&' hexit_SEQUENCE {$$ = $2;};

bin_num: '0' b bit_SEQUENCE {$$ = $3;} | '%' bit_SEQUENCE {$$ = $2;};

char_num: '\'' numchars '\'' {$$ = $2;};

numchars: numchar {$$ = (int) $1;} |
      numchar numchar {$$ = $1 | $2 << 8;} |
      numchar numchar numchar {$$ = $1 | $2 << 8 | $3 << 16;} |
      numchar numchar numchar numchar
            {$$ = $1 | $2 << 8 | $3 << 16 | $4 << 24;};

numchar: simplechar | compoundchar | spacechar | '"' {$$ = '"';};
commentchar: simplechar {} | spacechar {} | '\\' {} | '"' {} | '\'' {};
spacechar: ' ' {$$ = ' ';} | '\xA0' {$$ = 0xA0;} | '\t' {$$ = '\t';};
simplechar: letter | digit |
      '!' {$$ = '!';} | '#' {$$ = '#';} | '$' {$$ = '$';} |
      '%' {$$ = '%';} | '&' {$$ = '&';} | '(' {$$ = '(';} |
      ')' {$$ = ')';} | '*' {$$ = '*';} | '+' {$$ = '+';} |
      ',' {$$ = ',';} | '-' {$$ = '-';} | '.' {$$ = '.';} |
      '/' {$$ = '/';} | ':' {$$ = ':';} | ';' {$$ = ';';} |
      '<' {$$ = '<';} | '=' {$$ = '=';} | '>' {$$ = '>';} |
      '?' {$$ = '?';} | '@' {$$ = '@';} | '[' {$$ = '[';} |
      ']' {$$ = ']';} | '^' {$$ = '^';} | '_' {$$ = '_';} |
      '`' {$$ = '`';} | '{' {$$ = '{';} | '|' {$$ = '|';} |
      '}' {$$ = '}';} | '~' {$$ = '~';} |
   /*�*/ '\x8C' {$$ = 0x8C;} |
   /*�*/ '\x8D' {$$ = 0x8D;} |
   /*�*/ '\x8E' {$$ = 0x8E;} |
   /*�*/ '\x8F' {$$ = 0x8F;} |
   /*�*/ '\x90' {$$ = 0x90;} |
   /*�*/ '\x91' {$$ = 0x91;} |
   /*�*/ '\x92' {$$ = 0x92;} |
   /*�*/ '\x93' {$$ = 0x93;} |
   /*�*/ '\x94' {$$ = 0x94;} |
   /*�*/ '\x95' {$$ = 0x95;} |
   /*�*/ '\x96' {$$ = 0x96;} |
   /*�*/ '\x97' {$$ = 0x97;} |
   /*�*/ '\x98' {$$ = 0x98;} |
   /*�*/ '\x99' {$$ = 0x99;} |
   /*�*/ '\x9C' {$$ = 0x9C;} |
   /*�*/ '\x9D' {$$ = 0x9D;} |
   /*�*/ '\xA1' {$$ = 0xA1;} |
   /*�*/ '\xA2' {$$ = 0xA2;} |
   /*�*/ '\xA3' {$$ = 0xA3;} |
   /*�*/ '\xA4' {$$ = 0xA4;} |
   /*�*/ '\xA5' {$$ = 0xA5;} |
   /*�*/ '\xA6' {$$ = 0xA6;} |
   /*�*/ '\xA7' {$$ = 0xA7;} |
   /*�*/ '\xA8' {$$ = 0xA8;} |
   /*�*/ '\xA9' {$$ = 0xA9;} |
   /*�*/ '\xAA' {$$ = 0xAA;} |
   /*�*/ '\xAB' {$$ = 0xAB;} |
   /*�*/ '\xAC' {$$ = 0xAC;} |
   /*�*/ '\xAD' {$$ = 0xAD;} |
   /*�*/ '\xAE' {$$ = 0xAE;} |
   /*�*/ '\xAF' {$$ = 0xAF;} |
   /*�*/ '\xB0' {$$ = 0xB0;} |
   /*�*/ '\xB1' {$$ = 0xB1;} |
   /*�*/ '\xB9' {$$ = 0xB9;} |
   /*�*/ '\xB2' {$$ = 0xB2;} |
   /*�*/ '\xB3' {$$ = 0xB3;} |
   /*�*/ '\xB4' {$$ = 0xB4;} |
   /*�*/ '\xB5' {$$ = 0xB5;} |
   /*�*/ '\xB6' {$$ = 0xB6;} |
   /*�*/ '\xB7' {$$ = 0xB7;} |
   /*�*/ '\xB8' {$$ = 0xB8;} |
   /*�*/ '\xBA' {$$ = 0xBA;} |
   /*�*/ '\xBB' {$$ = 0xBB;} |
   /*�*/ '\xBC' {$$ = 0xBC;} |
   /*�*/ '\xBD' {$$ = 0xBD;} |
   /*�*/ '\xBE' {$$ = 0xBE;} |
   /*�*/ '\xBF' {$$ = 0xBF;} |
   /*�*/ '\xD7' {$$ = 0xD7;} |
   /*�*/ '\xF7' {$$ = 0xF7;};

compoundchar: '\\' escapedchar {$$ = $2;} |
      '\\' x hexit hexit {$$ = 16*XDIGIT ($3) + XDIGIT ($4);};

escapedchar: '\'' {$$ = '\'';} | '"' {$$ = '"';} |
      '\\' {$$ = '\\';} | n {$$ = '\n';} | '0' {$$ = '\0';};

/*------------------------------------------------------------------------*/
/*Metarules expanded*/

EMPTY:;

/*option*/
base_type_OPTION: base_type | EMPTY {$$.tag = def_TYPE_VOID;};
commentchar_SEQUENCE_OPTION: commentchar_SEQUENCE {} | EMPTY {};
condition_part_OPTION: condition_part | EMPTY {$$ = Empty;};
decl_SERIES_OPTION: decl_SERIES | EMPTY;
description_OPTION: description | EMPTY {$$ = Empty;};
DESCRIPTION_OPTION: DESCRIPTION {strcpy ($$, $1);} | EMPTY {CLEAR ($$);};
ellipsis_OPTION: ELLIPSIS {$$ = TRUE;} | EMPTY {$$ = FALSE;};
id_cont_SEQUENCE_OPTION: id_cont_SEQUENCE {strcpy ($$, $1);} |
      EMPTY {CLEAR ($$);};
pling_OPTION: PLING {$$ = TRUE;} | EMPTY {$$ = FALSE;};
word_SEQUENCE_OPTION: word_SEQUENCE {strcpy ($$, $1);} |
      EMPTY {CLEAR ($$);};
ws_item_SEQUENCE_OPTION: ws_item_SEQUENCE {} | EMPTY {};

/*sequence*/
bit_SEQUENCE: bit {$$ = DIGIT ($1);} |
      bit_SEQUENCE bit {$$ = $1 << 1 | DIGIT ($2);};
commentchar_SEQUENCE: commentchar {} | commentchar_SEQUENCE commentchar {};
digit_SEQUENCE: digit {$$ = DIGIT ($1);} |
      digit_SEQUENCE digit {$$ = 10*$1 + DIGIT ($2);};
hexit_SEQUENCE: hexit {$$ = XDIGIT ($1);} |
      hexit_SEQUENCE hexit {$$ = 16*$1 + XDIGIT ($2);};
id_cont_SEQUENCE: id_cont {$$ [0] = $1; $$ [1] = '\0';} |
      id_cont_SEQUENCE id_cont {strstrc ($$, $1, $2);};
space_SEQUENCE: space {} | space space_SEQUENCE {};
word_SEQUENCE: word {strcpy ($$, $1);} |
      word word_SEQUENCE {strcatw ($$, $1, $2);};
wordchar_SEQUENCE: wordchar {$$ [0] = $1; $$ [1] = '\0';} |
      wordchar_SEQUENCE wordchar {strstrc ($$, $1, $2);};
ws_item_SEQUENCE: ws_item {} | ws_item_SEQUENCE ws_item {};

/*list*/
const_defn_LIST: const_defn {} |
      const_defn_LIST COMMA const_defn {};
entry_condition_LIST: entry_condition |
      entry_condition_LIST COMMA entry_condition {$$ = Union ($1, $3);};
exit_condition_LIST: exit_condition |
      exit_condition_LIST COMMA exit_condition {$$ = Union ($1, $3);};
needs_LIST: needs {} | needs_LIST COMMA needs;
needsatend_LIST: needsatend {} | needsatend_LIST COMMA needsatend;
typed_var_LIST:
   typed_var
      {  def_t t = qalloc (def_sizeof_TYPE ($1.tag));

         memcpy (t, &$1, def_sizeof_TYPE ($1.tag));
         $$.tag = def_TYPE_LIST;
         $$.name = NULL;
         $$.value = $1.value;
         $$.description = NULL;
         $$.data AS list.members [0] = t;
         $$.data AS list.count = 1;
      } |
   typed_var_LIST COMMA typed_var
      {  def_t t = qalloc (def_sizeof_TYPE ($3.tag));

         $$ = $1;
         memcpy (t, &$3, def_sizeof_TYPE ($3.tag));
         $$.value = MAX ($1.value, $3.value); /*only used if it's a union*/
         $$.description = NULL;
         $$.data AS list.members [$$.data AS list.count] = t;
         $$.data AS list.count++;
      };
toided_var_LIST:
   toided_var
      {  def_t t = qalloc (def_sizeof_TYPE ($1.tag));

         memcpy (t, &$1, def_sizeof_TYPE ($1.tag));
         $$.tag = def_TYPE_LIST;
         $$.name = NULL;
         $$.value = $1.value;
         $$.description = NULL;
         $$.data AS list.members [0] = t;
         $$.data AS list.count = 1;
      } |
   toided_var_LIST COMMA toided_var
      {  def_t t = qalloc (def_sizeof_TYPE ($3.tag));

         $$ = $1;
         memcpy (t, &$3, def_sizeof_TYPE ($3.tag));
         $$.value = MAX ($1.value, $3.value); /*only used if it's a union*/
         $$.description = NULL;
         $$.data AS list.members [$$.data AS list.count] = t;
         $$.data AS list.count++;
      };
swi_defn_LIST: swi_defn {} |
      swi_defn_LIST COMMA swi_defn {};
type_defn_LIST: type_defn {} |
      type_defn_LIST COMMA type_defn {};

/*series*/
decl_SERIES: decl {} | decl_SERIES SEMICOLON decl {};
/*------------------------------------------------------------------------*/
%%
/*More C code*/

/* copy s1 into s, appending c */
/* s = s1 + c; */
char *strstrc (char *s, const char *s1, char c)
{  int len = strlen (s1);

   strcpy (s, s1);
   s [len] = c;
   s [len + 1] = '\0';

   return s;
}


/* copy s1 and s2 into s with a space separator */
/* s = s1 + ' ' + s2; */
char *strcatw (char *s, const char *s1, const char *s2)
{  int len = strlen (s1);

   strcpy (s, s1);
   s [len] = ' ';
   strcpy (&s [len + 1], s2);

   return s;
}


/* perform a malloc with error checking */
void *qalloc (size_t t)
{  void *p = 0;
   //tracef( "qalloc(0x%x)\n" _ t  );

   p = malloc (t);
   if (!p)
   {
      yyerror ("not enough memory for |qalloc()|");
      exit (EXIT_FAILURE);
   }

   //tracef( "qalloc returns 0x%x\n" _ p );
   return p;
}


/* create a copy of string s on the heap
**  return a pointer to the heap block
*/
char *qstrdup (const char *s)
{  char *dup = qalloc (strlen (s) + 1);

   strcpy (dup, s);
   return dup;
}


struct def_s Union (struct def_s a, struct def_s b)
{  struct def_s u;
   int i;

   u.swi = a.swi | b.swi;

   u.i     = a.i     | b.i;
   u.o     = a.o     | b.o;
   u.c     = a.c     | b.c;
   u.k     = a.k     | b.k;
   u.f_in  = a.f_in  | b.f_in;
   u.f_out = a.f_out | b.f_out;

   for (i = 0; i < 10; i++)
   {  u.constants [i] = DISJUNCTION (a.constants [i], b.constants [i]);
      u.inputs    [i] = DISJUNCTION (a.inputs    [i], b.inputs    [i]);
      u.outputs   [i] = DISJUNCTION (a.outputs   [i], b.outputs   [i]);
   }
   u.ri = a.ri | b.ri;
   u.ro = a.ro | b.ro;

   u.starred_swi = a.starred_swi || b.starred_swi;
   for (i = 0; i < 10; i++)
      u.starred_constants [i] =
            a.starred_constants [i] || b.starred_constants [i];

   u.absent = a.absent || b.absent;

   for (i = 0; i < 10; i++)
      u.op [i] = DISJUNCTION (a.op [i], b.op [i]);

   u.value = DISJUNCTION (a.value, b.value);

   if (a.description && b.description)
     fprintf(stderr, "Warning: SWI 0x%x has two descriptions: \"%s\" and \"%s\"\n", u.swi, a.description, b.description);
   u.description = DISJUNCTION (a.description, b.description);

   return u;
}

int main (int argc, char *argv [])
{  os_error *error;
   int i;
   enum {None, Asm, C_Header, C_Plus_Plus_Header, C_Help, Library, Hdr,
         Asm_Help, C_Strong, Asm_Strong, Vapi, VapiDeps, VapiH} option = None;
   enum {APCS26, APCS32} mode = DEFAULT_TO_APCS_32 ? APCS32 : APCS26;
   char *output = NULL;

   trace_initialise ("Trace$To");

#if TRACE
   {
     int ii;
     tracef( "Main:" );
     for ( ii = 0; ii<argc; ++ii )
     {
       os_writec( ' ' );
       os_write0( argv[ii] );
     }
     os_new_line();
   }
#endif

   /* Set AsmType default.  */
   AsmType = AsmType_eGCCAOF;

   if (  (error = lookup_new (&needses, 16)) != NULL ||
         (error = lookup_new (&needsatends, 16)) != NULL ||
         (error = lookup_new (&consts, 16)) != NULL ||
         (error = lookup_new (&types, 16)) != NULL ||
         (error = lookup_new (&swis, 16)) != NULL ||
         (error = lookup_new (&main_byte_wide, 16)) != NULL
      )
      {
         tracef( "Error %s\n" _ error -> errmess );
         goto finish;
      }

   yydebug = 0;

   for (i = 1; i < argc && argv [i] [0] == '-'; ++i)
   {  if (strcmp (argv [i], "-s") == 0)
         option = Asm;
      else if (strcmp (argv [i], "-l") == 0)
         option = Library;
      else if (strcmp (argv [i], "-h") == 0)
         option = C_Header;
      else if (strcmp (argv [i], "-h++") == 0)
         option = C_Plus_Plus_Header;
      else if (strcmp (argv [i], "-chelp") == 0)
         option = C_Help;
      else if (strcmp (argv [i], "-asmhelp") == 0)
         option = Asm_Help;
      else if (strcmp (argv [i], "-hdr") == 0)
         option = Hdr;
      else if (strcmp (argv [i], "-cstrong") == 0)
         option = C_Strong;
      else if (strcmp (argv [i], "-asmstrong") == 0)
         option = Asm_Strong;
      else if (strcmp (argv [i], "-vapi") == 0)
         option = Vapi;
      else if (strcmp (argv [i], "-vapideps") == 0)
         option = VapiDeps;
      else if (strcmp (argv [i], "-vapih") == 0)
         option = VapiH;
      else if (strcmp (argv [i], "-v") == 0)
         Verbose = TRUE;
      else if (strcmp (argv [i], "-q") == 0)
         Quiet = TRUE;
      else if (strcmp (argv [i], "-o") == 0)
         output = argv [++i];
      else if (strcmp (argv [i], "-help") == 0)
      {  const char helptext[] = "DefMod" VERSION_INFO ". Copyright (c) 1994-2012 Jonathan Coxhead and OSLib maintainers.\r"
               "DefMod comes with ABSOLUTELY NO WARRANTY.\r"
               "This is free software, and you are welcome to redistribute it "
               "under certain conditions. See the file COPYING for details.\r"
               "Usage: DefMod <type> [-v] [-help] "
                     "[-o <output-dir> | > <output-file>] "
                     "[-byte_wide <byte-wide-file>] "
                     "[-26bit | -32bit] "
                     "[-asmtype [objasm | asasmaof | asasmelf | gccaof | armasm | gccelf]] "
		     "[-float-abi [soft | hard]] "
                     "< <module-interface-file>\r"
               "Purpose: generate output from a module interface file\r"
               "-v\t\t"             "verbose\r"
               "-q\t\t"             "quiet\r"
               "-o\t\t"             "is required when generating a "
                                    "directory\r"
               "\t<output-dir>\t\t" "directory to create\r"
               "-byte_wide\t"       "is required with -s or -l for a "
                                    "module interface using byte-wide\r"
               "\t\t"               "types defined elsewhere\r"
               "\t<byte-wide-file>\t" "file containing whitespace-separated "
                                    "names of\r"
               "\t\t\t\t"           "byte-wide types\r"
               "-26bit\t\t"         "generate 26 bit code" HELP_STRING_26 "\r"
               "-32bit\t\t"         "generate 32 bit code" HELP_STRING_32 "\r"
               "-asmtype\t"         "selects the assembler to use\r"
               "\tobjasm\t\t"       "...use ObjAsm (on RISC OS only)\r"
               "\tasasmaof\t"       "...use AsAsm with AOF output\r"
               "\tasasmelf\t"       "...use AsAsm with ELF output\r"
               "\tgccaof\t\t"       "...use GCC AOF toolkit (GCCSDK 3.4 only)\r"
               "\tarmasm\t\t"       "...use ARM ASM\r"
               "\tgccelf\t\t"       "...use GCC ELF toolkit (GCCSDK 4 only)\r"
	       "-float-abi\t\t"     "marks object files which float-abi is in use\r"
	       "\tsoft\t\t"         "...soft-float, no FP instructions used\r"
	       "\thard\t\t"         "...hard-float, FP instructions can be used\r"

               "<type> is one of:\r"
               "\t-asmhelp\t"       "SrcEdit assembler help file\r"
               "\t-asmstrong\t"     "StrongHelp assembler help "
                                    "directory (not implemented)\r"
               "\t-chelp\t\t"       "SrcEdit C help file\r"
               "\t-cstrong\t"       "StrongHelp C help directory\r"
               "\t-h\t\t"           "C header file\r"
               "\t-hdr\t\t"         "assembler header file (ObjAsm or GAS syntax)\r"
               "\t-h++\t\t"         "C++ header file (experimental)\r"
               "\t-l\t\t"           "Directory for running LibFile -c "
                                    "-via ViaFile\r"
               "\t-s\t\t"           "assembler source file (ObjAsm or GAS syntax)\r"
               "\t-vapi\t\t"        "Vala vapi-files (experimental)\r"
               "\t-vapideps\t"      "Vala deps-files (experimental)\r"
               "\t-vapih\t\t"       "C header files referenced from the vapi files\r"
               "\t\t\t"             "(experimental)\r";

           if ((error = xos_pretty_print (helptext, NULL, NULL)) != NULL)
		goto finish;
      }
      else if (strcmp (argv [i], "-byte_wide") == 0)
      {
      	/*Add the contents of the given file to the byte wide table.*/
         char buffer [256], *cc;
#ifdef EXECUTE_ON_UNIX
         FILE *f;
#else
         bits psr;
         int f;
#endif
         enum {Copying, Skipping} state = Skipping;

         if (++i == argc)
         {
            fprintf (stderr, "Missing option value for -byte_wide\n");
            Parse_Error = TRUE;
            goto finish;
         }

//	 tracef( "Main opening file: %s\n" _ argv[i] );

#ifdef EXECUTE_ON_UNIX
         if ((f = fopen(argv[i],"r")) == NULL) {
            error = (os_error *)_kernel_last_oserror();
            goto finish;
         }
#else
         if ((error = xosfind_openin (osfind_NO_PATH |
               osfind_ERROR_IF_DIR | osfind_ERROR_IF_ABSENT, argv [i], NULL,
               &f)) != NULL)
            goto finish;
#endif

         cc = buffer;
         do
         {  int c;

#ifdef EXECUTE_ON_UNIX
            *cc = c = getc(f);
            if (ferror(f))
	    {
//	      tracef( "Error reading character from input stream: %d\n" _ errno );
              error = (os_error *)_kernel_last_oserror();
              goto finish;
            }

            if (feof(f)) *cc = ' ';
#else
            if ((error = xos_bget (f, &c, &psr)) != NULL)
               goto finish; /*leaves the file open - who cares?*/
            *cc = c; /*_swix idiocy*/

            if ((psr & _C) != NONE) *cc = ' ';
#endif

//	    tracef( "State = %d\n" _ state );

            switch (state)
            {  case Skipping:
                  if (!isspace (*cc))
                  {  cc++;
                     state = Copying;
                  }
               break;

               case Copying:
                  if (!isspace (*cc))
                     cc++;
                  else
                  {  *cc = '\0';
		     tracef( "Inserting \"%s\"\n" _ buffer );
                     if ((error = lookup_insert (main_byte_wide, buffer,
                           (void *) 1)) != NULL)
		     {
		         tracef( "Error %s\n" _ error -> errmess );
                         goto finish;
		     }
		     tracef( "Done: \"%s\"\n" _ buffer );
                     cc = buffer;
                     state = Skipping;
                  }
               break;
            }
         }
         /* Loop termination condition depends on target ... */
#ifdef EXECUTE_ON_UNIX
         while (!feof(f));
         (void) fclose(f);
#else
         while ((psr & _C) == NONE);

         if ((error = xosfind_close (f)) != NULL)
            goto finish;
#endif
      }
      else if (strcmp (argv [i], "-26bit") == 0)
      {
         mode = APCS26;
      }
      else if (strcmp (argv [i], "-32bit") == 0)
      {
         mode = APCS32;
      }
      else if (strcmp (argv [i], "-asmtype") == 0)
      {
         ++i;
         if (strcmp (argv [i], "objasm") == 0)
            AsmType = AsmType_eObjAsm;
         else if (strcmp (argv[i], "asasmaof") == 0)
            AsmType = AsmType_eAsAsmAOF;
         else if (strcmp (argv[i], "asasmelf") == 0)
            AsmType = AsmType_eAsAsmELF;
         else if (strcmp (argv[i], "gccaof") == 0)
            AsmType = AsmType_eGCCAOF;
         else if (strcmp (argv[i], "armasm") == 0)
            AsmType = AsmType_eARMASM;
         else if (strcmp (argv[i], "gccelf") == 0)
            AsmType = AsmType_eGCCELF;
         else
         {
            fprintf (stderr, "Unknown option value %s for -asmtype (only \"objasm\", \"asasmaof\", \"asasmelf\", \"armarm\", \"gccaof\" and \"gccelf\" are known)\n", argv[i]);
            Parse_Error = TRUE;
            goto finish;
         }
      }
      else if (strcmp (argv[i], "-float-abi") == 0)
      {
         ++i;
	 if (strcmp (argv[i], "hard") == 0)
	   FloatABI = FloatABI_eHard;
	 else if (strcmp (argv[i], "soft") == 0)
	   FloatABI = FloatABI_eSoft;
	 else
	 {
	   fprintf (stderr, "Unknown option value %s for -float-abi (only \"hard\" or \"soft\" are known)\n", argv[i]);
           Parse_Error = TRUE;
           goto finish;
	 }
      }
      else
      {
         fprintf (stderr, "Unknown option %s\n", argv[i]);
         Parse_Error = TRUE;
         goto finish;
      }
   }

   if (option == None)
   {
      tracef( "Parse error: No option\n" );
      fprintf (stderr, "Nothing to do\n");
      Parse_Error = TRUE;
      goto finish;
   }

   tracef( "yyparse\n" );
   yyparse ();
   if (Parse_Error)
   {
   	tracef( "yyparse error\n" );
   	goto finish;
   }

   switch (option)
   {  case Asm:
   	 tracef( "Asm\n" );
         if ((error = assembler_output (stdout, Title, Author,
               types, swis, /*separate?*/ FALSE,
               mode == APCS32, NULL)) != NULL)
            goto finish;
      break;

      case C_Header:
   	 tracef( "C_Header\n" );
         if ((error = cheader_output (stdout, Title, Author, needses,
               needsatends, consts, types, swis, /*C++?*/ FALSE)) != NULL)
            goto finish;
      break;

      case C_Plus_Plus_Header:
   	 tracef( "C_Plus_Plus_Header\n" );
         if ((error = cheader_output (stdout, Title, Author, needses,
               needsatends, consts, types, swis, /*C++?*/ TRUE)) != NULL)
            goto finish;
      break;

      case C_Help:
   	 tracef( "C_Help\n" );
         if ((error = chelp_output (stdout, Title, Author, needses,
               needsatends, consts, types, swis)) != NULL)
            goto finish;
      break;

      case Asm_Help:
   	 tracef( "Asm_Help\n" );
         if ((error = asmhelp_output (stdout, Title, Author, needses,
               needsatends, consts, types, swis)) != NULL)
            goto finish;
      break;

      case Library:
   	 tracef( "Library\n" );
         if (output != NULL)
            if ((error = assembler_output (NULL, Title, Author, types,
                  swis, /*separate?*/ TRUE,
                  mode == APCS32, output)) != NULL)
               goto finish;
      break;

      case Hdr:
   	 tracef( "Hdr\n" );
         if ((error = hdr_output (stdout, Title, Author, needses,
               needsatends, consts, types, swis)) != NULL)
            goto finish;
      break;

      case C_Strong:
   	 tracef( "C_Strong\n" );
         if (output != NULL)
            if ((error = cstrong_output (output, Title, Author, needses,
                  needsatends, consts, types, swis)) != NULL)
               goto finish;
      break;

      case Vapi:
   	 tracef( "Vapi\n" );
         if ((error = vapi_output (stdout, Title, Author, needses,
               needsatends, consts, types, swis)) != NULL)
            goto finish;
      break;

      case VapiDeps:
   	 tracef( "VapiDeps\n" );
         if ((error = vapi_deps_output (stdout, needses)) != NULL)
            goto finish;
      break;

      case VapiH:
   	 tracef( "VapiH\n" );
         if ((error = vapi_h_output (stdout, types)) != NULL)
            goto finish;
      break;

      default:
	tracef( "default\n" );
	break;
   }

finish:
   trace_terminate ();
   if (error == NULL)
      exit (Parse_Error ? EXIT_FAILURE : EXIT_SUCCESS);
   else
   {  fprintf (stderr, "%s\n", error->errmess);
      exit (EXIT_FAILURE);
}  }


/* lexical analyser
**  read one char from stdin
**  if EoF, then return 0
**  else  return char
*/
int yylex (void)
{
	int c;

#if 0
   return
     (c = getchar ()) == EOF?
        0:
        ( Verbose?
          (  ' ' <= c && c <= '~'?
            fputc (c, stderr):
              c == '\n'?
                ( fputc ('\n', stderr),
                  fputc ('\r', stderr)
                ):
                ( fputc ('\\', stderr),
                  fputc ('x', stderr),
                  fputc (UCHAR (c/0x10), stderr),
                  fputc (UCHAR (c%0x10), stderr)
                )
          ):
          0,
          c
        );
#elif 0
	 c = getchar ();
	 if ( c == EOF ) c = 0;

	if ( Verbose /* && (c != 0) */ )
	{
		if ( ' ' <= c && c <= '~' )
			fputc( c, stderr );
		else if ( c == '\n' )
		{
			fputc ('\n', stderr);
			fputc ('\r', stderr);
		}
		else
		{
			fputc ('\\', stderr);
			fputc ('x', stderr);
			fputc (UCHAR (c/0x10), stderr);
			fputc (UCHAR (c%0x10), stderr);
		}
	}

	if ( c == 0 )
		tracef( "\nyylex: End of file\n" );

	return c;
#else
   c = getchar ();
   if ( c == EOF ) c = 0;
   return c;
#endif
}


void yyerror (char *s)
{  os_error error;

   (void) NCOPY (error.errmess, s, (int) sizeof (error.errmess) - 1);
   fprintf (stderr, "%s at line %d\n", error.errmess, Line_No);
   Parse_Error = TRUE;
}

int yydebug = 0;

