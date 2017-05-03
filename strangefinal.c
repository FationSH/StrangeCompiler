#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define NUM_FILES 5 // posa arxeia na kanw compile

// megisto megethos lektikis monadas
#define MAXLEN 31 // 30 chars + '\0'

// tokens
#define EOFTK -1 // end of file
#define ERRORTK -2 // err

#define ADDTK 100 // +
#define MINUSTK 101 // -
#define MULTITK 102 // *
#define DIVTK 103 // /
#define LESSTK 104 // <
#define MORETK 105 // >
#define EQTK 106 // =
#define LESSEQTK 107 // <=
#define MOREEQTK 108 // >=
#define DIFFTK 109 // <>
#define ASSIGNTK 110 // :=
#define MARKTK 111 // ;
#define COMMATK 112 // ,
#define PAR1OPENTK 113 // {
#define PAR1CLOSETK 114 // }
#define PAR2OPENTK 115 // (
#define PAR2CLOSETK 116 // )
#define PAR3OPENTK 117 // [
#define PAR3CLOSETK 118 // ]
#define COMMSEP1TK 119 // /*
#define COMMSEP2TK 120 // */
#define IDTK 121 // id
#define NUMTK 122 // numbers
// ama theloume na prosthesoume kai alla TK sto mellon

// desmeumenes lekseis
#define ANDTK 130 // and
#define DECLARETK 131 // declare
#define DOTK 132 // do
#define ELSETK 133 // else
#define ENDDECLARETK 134 // enddeclare
#define EXITTK 135 // exit
#define PROCEDURETK 136 // procedure
#define FUNCTIONTK 137 // function
#define PRINTTK 138 // print
#define INCASETK 139 // incase
#define IFTK 140 // if
#define INTK 141 // in
#define INOUTTK 142 // inout
#define NOTTK 143 // not
#define FORCASETK 144 // forcase
#define PROGRAMTK 145 // program
#define ORTK 146 // or
#define RETURNTK 147 // return
#define WHILETK 148 // while
#define COPYTK 149 // copy
#define WHENTK 150 // when
#define CALLTK 151 // call

// gia ton pinaka simbolwn
#define PARAM 0
#define VAR 1
#define TEMP 2
#define FUNC 3

// global vars
FILE *fin; // deiktis gia arxeio me code tis strange
FILE *fout; // deiktis gia arxeio eksodou intermediate code tis strange
// lex
int tk; // kwdikos lektikis monadas
int lines; // poses grammes exei to programma
int opencomments; // gia na doume pote anoiksame kai pote kleisame sxolio
char str[MAXLEN]; // apothikeuei leksh kathe fora otan kalite h lex()
// intermediate code
int quadlabel; // etiketa gia thn epomenh tetrada pou 8a parax8ei 100, 110, ...
struct quad *QUADLIST; // deikths pros thn lista twn tetradwn
int tempcount; // arithmos temp metablitis
struct list *exitlist; // gia to exit stin while kai do_while
// symbol list
struct scope *SCOPELIST; // deiktis pros ti lista me scopes
int main_framelength; // framelength tis main
// final code
FILE *finalfp; // gia arxeia telikou kwdika
int lcount; // metritis label gia teliko kwdika

// struct gia tis tetrades
struct quad {
	int label; // etiketa tetradas
	char op0[MAXLEN], op1[MAXLEN], op2[MAXLEN], op3[MAXLEN]; // operators 0, 1, 2, 3
	struct quad *next; // deiktis stin epomeni tetrada
};

// struct gia tis listes twn tetradwn
struct list {
	int quad; // etiketa tetradas
	struct list *next;  //deikths stin epomeni
};

// struct gia tis listes true kai false
struct rule {
	struct list *true;
	struct list *false;
};

// struct gia tis ontotites pou antistoixoun se kapoio scope
struct entity {
	char name[MAXLEN]; // onoma ontothtas
	int type; // tipos ontothtas ston pinaka
	int offset; // apostasi apo korifi stoibas
	int framelength; // mikos eggrafimatos drastiriopoihshs
	int startQuad; // etiketa tis prwths tetradas tou kwdika tis sinartisis
	int nestinglevel; // bathos fwliasmatos ston pinaka gia kathe metablhth
	int parMode; // tropos perasmatos parametrou
	int parPlace; // aukson arithmos parametrou
	char arglist[MAXLEN][MAXLEN]; // gia ti lista me ta CV, REF, COPY
	struct entity *next; // to epomeno tou
};

// scruct gia ta scopes pou perilambanoun kapoies ontothtes
struct scope {
	char name[MAXLEN]; // to onoma tou scope
	struct entity *entitylist; // h lista apo entities gia auto to scope
	int nestinglevel; // bathos fwliasmatos
	struct scope *enclosingScope; // deiktis gia to epomeno scope
};

// lex prototypes
int lex(char token[MAXLEN]);
// syntax prototypes
void program();
void block(char name[MAXLEN], int called_by);
void declarations();
void subprograms();
void sequence();
void varlist();
void func();
void funcbody();
void formalpars(char name[MAXLEN]);
void formalparlist(char name[MAXLEN]);
void formalparitem(char name[MAXLEN]);
void statement();
void assignment_stat();
void if_stat();
void brack_or_stat();
void elsepart();
void brackets_seq();
void elsepart();
void while_stat();
void do_while_stat();
void condition(struct rule *c);
void boolterm(struct rule *q);
void boolfactor(struct rule *r);
void expression(char eplace[MAXLEN]);
void relational_oper();
void add_oper(char oper[MAXLEN]);
void multi_oper(char oper[MAXLEN]);
void optionalsign();
void term(char tplace[MAXLEN]);
void optional_sign();
void factor(char fplace[MAXLEN]);
void brack_or_stat();
void idtail(char name[MAXLEN], char fplace[MAXLEN]);
void actualpars(char name[MAXLEN], int called_by, char fplace[MAXLEN]);
void actualparlist();
void actualparitem();
void exit_stat();
void return_stat();
void print_stat();
void incase_stat();
void forcase_stat();
void call_stat();
void comments();
// intermediate code prototypes
int nextquad();
void genquad(char op[MAXLEN], char x[MAXLEN], char y[MAXLEN], char z[MAXLEN]);
char *newTemp();
struct list *emptylist();
struct list *makelist(int q);
struct list *merge(struct list *l1, struct list *l2);
void backpatch(struct list *alist, int z);
// symbol list prototypes
void createscope(char name[MAXLEN]);
void closescope();
void insertentity(char name[MAXLEN], int type, int parMode);
void addargument(char name[MAXLEN], int parMode);
struct entity *searchentity(char name[MAXLEN]);
void printtable();
// final code prototypes
void gnlvcode(char v[MAXLEN]);
void loadvr(char v[MAXLEN], int r);
void storerv(int r, char v[MAXLEN]);

//*********************************************************************//
//								   lex                                 //
//*********************************************************************//
// epistrefei ton kwdiko tis lektikhs monadas
int lex(char token[MAXLEN]) {
	char c; // xaraktiras arxeio ton diabazoume 1-1 kathe fora stin while mexri eof
	int number; // gia elegxo o arithmos na einai apo -32768 ews 32768
	int i = 0; // thesi ston pinaka token

	while ((c = getc(fin)) != EOF) { // oso exoume xaraktires na diabasoume kai den ftasame sto telos arxeiou
		// metrima grammwn arxeiou
		if (c == '\n') {
			lines++;
		}
		// elegxos gia metablites h desmeumenes lekseis
		if (isalpha(c)) { // opoiodipote gramma kefalaio h mikro
			token[i] = c;
			c = getc(fin);
			while (isalpha(c) || isdigit(c)) { // meta akolouthite apo eite gramma eite arithmo
				i++;
				token[i] = c;
				c = getc(fin);
				if (i == MAXLEN) { // gemise o buffer
					printf("error: variable name too long in line %d\n", lines);
					exit(1);
				}
			}
			ungetc(c, fin); // epistrefw pisw to xaraktira h arithmo pou danistika kai adeiazw ton buffer na ton ksanaxrisimopoihsw
			// desmeumeni leksh h anagnwristiko ID
			strcpy(str, token);
			token[i+1] = '\0';
			if (strcmp(str, "and") == 0) return ANDTK;
			if (strcmp(str, "declare") == 0) return DECLARETK;
			if (strcmp(str, "do") == 0) return DOTK;
			if (strcmp(str, "else") == 0) return ELSETK;
			if (strcmp(str, "enddeclare") == 0) return ENDDECLARETK;
			if (strcmp(str, "exit") == 0) return EXITTK;
			if (strcmp(str, "procedure") == 0) return PROCEDURETK;
			if (strcmp(str, "function") == 0) return FUNCTIONTK;
			if (strcmp(str, "print") == 0) return PRINTTK;
			if (strcmp(str, "incase") == 0) return INCASETK;
			if (strcmp(str, "if") == 0) return IFTK;
			if (strcmp(str, "inout") == 0) return INOUTTK;
			if (strcmp(str, "in") == 0) return INTK;
			if (strcmp(str, "not") == 0) return NOTTK;
			if (strcmp(str, "forcase") == 0) return FORCASETK;
			if (strcmp(str, "program") == 0) return PROGRAMTK;
			if (strcmp(str, "or") == 0) return ORTK;
			if (strcmp(str, "return") == 0) return RETURNTK;
			if (strcmp(str, "while") == 0) return WHILETK;
			if (strcmp(str, "copy") == 0) return COPYTK;
			if (strcmp(str, "when") == 0) return WHENTK;
			if (strcmp(str, "call") == 0) return CALLTK;
			return IDTK;
		}
		// elegxos gia arithmous
		if (isdigit(c)) { // einai arithmos
			token[i] = c;
			c = getc(fin);
			if (isalpha(c)) { // an exw arithmo na min exw gramma meta
				printf("error: alpha found after number in line %d\n", lines);
				exit(1);
			}
			while (isdigit(c)) { // tote akolouthite apo arithmo
				i++;
				token[i] = c;
				number = atoi(token);
				if (number >= 32767) { // na einai mikrotero tou 32767
					printf("error: number out of bounds found in line %d\n", lines);
					exit(1);
				}
				c = getc(fin);
				if (isalpha(c)) { // an exw arithmo na min exw gramma meta
					printf("error: alpha found after number in line %d\n", lines);
					exit(1);
				}
			}
			ungetc(c, fin); // epistrefw pisw ton arithmo pou danistika kai adeiazw ton buffer na ton ksanaxrisimopoihsw
			strcpy(str, token);
			token[i+1] = '\0';
			return NUMTK;				// number token
		}
		if (c == ':') {
			c = getc(fin);
			if (c == '=') {
				strcpy(str, ":=");
				return ASSIGNTK;		// :=
			} else {
				strcpy(str, "ERROR");
				return ERRORTK;			// err sto assign token :=
			}
		}
		if (c == '/') {					// anoigma sxoliou
			c = getc(fin);
			if (c == '*') {
				// prospername ta sxolia mexri na broume */ kai epistrefoume kwdiko sxoliou
				while (c != '/') {
					c = getc(fin);
					if (c == '*') {
						c = getc(fin);
						if (c == '/') {
							// epistrefoume tous 2 xaraktires */ gia na mas epistrepsei ton kwdiko tou */
							ungetc(c, fin);
							ungetc('*', fin);
						}
					}
				}
				opencomments = 1; // simenei oti anoiksame sxolia
				strcpy(str, "/*");
				return COMMSEP1TK;		// /*
			} else {
				ungetc(c, fin);
				strcpy(str, "/");
				return DIVTK;			// /
			}
		}
		if (c == '*') {					// kleisimo sxoliwn
			c = getc(fin);
			if (c == '/') {
				if (opencomments == 1) { // an exoume anoiksei sxolia
					strcpy(str, "*/");
					opencomments = 0; // to kanoume 0 gia na to ksanaxrisimopoihsoume
					return COMMSEP2TK;		// */
				} else { // an exw */ xwris anoigma sxoliwn epistrefw lathos
					strcpy(str, "ERROR");
					return ERRORTK;
				}
			} else {
				ungetc(c, fin);
				strcpy(str, "*");
				return MULTITK;			// *
			}
		}
		if (c == '>') {
			c = getc(fin);
			if (c == '=') {
				strcpy(str, ">=");
				return MOREEQTK;			// >=
			} else {
				ungetc(c, fin);
				strcpy(str, ">");
				return MORETK;				// >
			}
		}
		if (c == '<') {
			c = getc(fin);
			if (c == '=') {
				strcpy(str, "<=");
				return LESSEQTK;			// <=
			} else if (c == '>') {
				strcpy(str, "<>");
				return DIFFTK;				// <>
			} else {
				ungetc(c, fin);
				strcpy(str, "<");
				return LESSTK;				// <
			}
		}
		if (c == '+') {
			strcpy(str, "+");
			return ADDTK;					// +
		}
		if (c == '-') {
			strcpy(str, "-");
			return MINUSTK;					// -
		}
		if (c == ';') {
			strcpy(str, ";");
			return MARKTK;					// ;
		}
		if (c == ',') {
			strcpy(str, ",");
			return COMMATK;					// ,
		}
		if (c == '=') {
			strcpy(str, "=");
			return EQTK;					// =
		}
		if (c == '{') {
			strcpy(str, "{");
			return PAR1OPENTK;				// {
		}
		if (c == '}') {
		strcpy(str, "}");
			return PAR1CLOSETK;				// }
		}
		if (c == '(') {
			strcpy(str, "(");
			return PAR2OPENTK;				// (
		}
		if (c == ')') {
			strcpy(str, ")");
			return PAR2CLOSETK;				// )
		}
		if (c == '[') {
			strcpy(str, "[");
			return PAR3OPENTK;				// [
		}
		if (c == ']') {
			strcpy(str, "]");
			return PAR3CLOSETK;				// ]
		}
		i = 0;
	}

	strcpy(str, "EOF");
	return EOFTK;							// end of file
}

//*********************************************************************//
//								  syntax                               //
//*********************************************************************//
void program() {
	char name[MAXLEN];
	int called_by;

	called_by = 1;
	tk = lex(str);
	comments();
	if (tk == PROGRAMTK) {
		tk = lex(str);
		if (tk == IDTK) {
			createscope(str); // scope ths main
			strcpy(name, str);
			tk = lex(str);
			block(name, called_by); // simenei to kalese h main
		} else {
			printf("error: program name expected in line %d\n", lines);
			exit(1);
		}
	} else {
		printf("error: keyword 'program' expected in line %d\n", lines);
		exit(1);
	}
}

void block(char name[MAXLEN], int called_by) {
	struct quad *tempquad;
	struct entity *entitytemp, *vartemp, *functemp, *entscopetemp, *lowerentity, *upperentity; // upper stin korifi tou scope, lower ena epipedo pio katw
	int framecount, found, start, in;

	comments();
	if (tk == PAR1OPENTK) {
		genquad("begin_block", name, "_", "_");
		tk = lex(str);
		declarations();
		subprograms();
		sequence();
		// enimwrwsi frame length gia to entity me onoma name
		framecount = 12;
		if (called_by == 0) { // an den to kalese h main
			upperentity = SCOPELIST->entitylist;
			while (upperentity != NULL) { // diatrexw ola ta entities tou scope gia ipologismo framelength
				framecount += 4;
				upperentity = upperentity->next;
			}
			lowerentity = searchentity(name); // sto entity me onoma name tha bei to frame length
			lowerentity->framelength = framecount;
			entitytemp = lowerentity;
		} else { // alliws ipologismos framelength tis main
			main_framelength = 12;
			upperentity = SCOPELIST->entitylist;
			while (upperentity != NULL) { // diatrexw ola ta entities tou scope gia ipologismo framelength
				main_framelength += 4;
				upperentity = upperentity->next;
			}
			// gia teliko kwdika thelw framelength tis main
			entitytemp = (struct entity *)malloc(sizeof(struct entity));
			entitytemp->framelength = main_framelength;
			printf("main framelegnth: %d\n", main_framelength); // tipwma main framelength
		}
		if (tk == PAR1CLOSETK) {
			tk = lex(str);
			if (called_by == 1) {
				genquad("halt", "_", "_", "_"); // an einai h main prepei kai halt
			}
			genquad("end_block", name, "_", "_"); // onoma block
		} else {
			printf("error: '}' expected in line %d but found %s\n", lines, str);
			exit(1);
		}
		// paragwgh telikou kwdika
		fprintf(finalfp, "L%d: jmp L0\n", lcount); // jmp L0
		lcount++;  // label telikou code
		while (QUADLIST != NULL) { // gia oli ti lista me tis tetrades pou antoistixi sto block
			found = 0;
			tempquad = QUADLIST->next;
			// begin_block name _ _
			if (called_by == 1 && in != 1) {
				fprintf(finalfp, "L0: \n");
				in = 1;
			}
			if (strcmp(QUADLIST->op0, "begin_block") == 0) {
				fprintf(finalfp, "L%d: \n", lcount);
				start = lcount;
				lcount++;
			}
			// jump _ _ w
			if (strcmp(QUADLIST->op0, "jump") == 0) {
				fprintf(finalfp, "L%d: ", lcount);
				lcount++;
				// jmp Lw
				fprintf(finalfp, "jmp L%s\n", lcount, QUADLIST->op3);
			}
			// relop sinthikes h cmpi allazei ton kataxwriti SR kai oi je, jne, jb, jbe, ja, jae ton elegxoun kai kanoun to alma ipo sinthiki
			// = x y w
			else if (strcmp(QUADLIST->op0, "=") == 0) {
				fprintf(finalfp, "L%d: ", lcount);
				lcount++;
				loadvr(QUADLIST->op1, 1); // R[1] = x
				loadvr(QUADLIST->op2, 2); // R[2] = y
				fprintf(finalfp, "cmpi R[1], R[2]\n");
				fprintf(finalfp, "je L%s\n", QUADLIST->op3); // jump equal
			}
			// <> x y w
			else if (strcmp(QUADLIST->op0, "<>") == 0) {
				fprintf(finalfp, "L%d: ", lcount);
				lcount++;
				loadvr(QUADLIST->op1, 1); // R[1] = x
				loadvr(QUADLIST->op2, 2); // R[2] = y
				fprintf(finalfp, "cmpi R[1], R[2]\n");
				fprintf(finalfp, "jne L%s\n", QUADLIST->op3); // jump not equal
			}
			// > x y w
			else if (strcmp(QUADLIST->op0, ">") == 0) {
				fprintf(finalfp, "L%d: ", lcount);
				lcount++;
				loadvr(QUADLIST->op1, 1); // R[1] = x
				loadvr(QUADLIST->op2, 2); // R[2] = y
				fprintf(finalfp, "cmpi R[1], R[2]\n");
				fprintf(finalfp, "ja L%s\n", QUADLIST->op3); // jump above
			}
			// >= x y w
			else if (strcmp(QUADLIST->op0, ">=") == 0) {
				fprintf(finalfp, "L%d: ", lcount);
				lcount++;
				loadvr(QUADLIST->op1, 1); // R[1] = x
				loadvr(QUADLIST->op2, 2); // R[2] = y
				fprintf(finalfp, "cmpi R[1], R[2]\n");
				fprintf(finalfp, "jae L%s\n", QUADLIST->op3); // jump above equal
			}
			// < x y w
			else if (strcmp(QUADLIST->op0, "<") == 0) {
				fprintf(finalfp, "L%d: ", lcount);
				lcount++;
				loadvr(QUADLIST->op1, 1); // R[1] = x
				loadvr(QUADLIST->op2, 2); // R[2] = y
				fprintf(finalfp, "cmpi R[1], R[2]\n");
				fprintf(finalfp, "jb L%s\n", QUADLIST->op3); // jump below
			}
			// <= x y w
			else if (strcmp(QUADLIST->op0, "<=") == 0) {
				fprintf(finalfp, "L%d: ", lcount);
				lcount++;
				loadvr(QUADLIST->op1, 1); // R[1] = x
				loadvr(QUADLIST->op2, 2); // R[2] = y
				fprintf(finalfp, "cmpi R[1], R[2]\n");
				fprintf(finalfp, "jbe L%s\n", QUADLIST->op3); // jump below equal
			}
			// := x _ z
			else if (strcmp(QUADLIST->op0, ":=") == 0 && strcmp(tempquad->op0, "retv") != 0) { // apli ekxwrisi xwris return value
				fprintf(finalfp, "L%d: ", lcount);
				lcount++;
				loadvr(QUADLIST->op1, 1); // R[1] = x
				storerv(1, QUADLIST->op3); // z = R[1]
			}
			// op = {+, -, *, /} sinthikes
			// + x y z
			else if (strcmp(QUADLIST->op0, "+") == 0) {
				fprintf(finalfp, "L%d: ", lcount);
				lcount++;
				loadvr(QUADLIST->op1, 1);
				loadvr(QUADLIST->op2, 2);
				fprintf(finalfp, "addi R[3], R[1], R[2]\n");
				storerv(3, QUADLIST->op3);
			}
			// - x y z
			else if (strcmp(QUADLIST->op0, "-") == 0) {
				fprintf(finalfp, "L%d: ", lcount);
				lcount++;
				loadvr(QUADLIST->op1, 1);
				loadvr(QUADLIST->op2, 2);
				fprintf(finalfp, "subi R[3], R[1], R[2]\n");
				storerv(3, QUADLIST->op3);
			}
			// * x y z
			else if (strcmp(QUADLIST->op0, "*") == 0) {
				fprintf(finalfp, "L%d: ", lcount);
				lcount++;
				loadvr(QUADLIST->op1, 1);
				loadvr(QUADLIST->op2, 2);
				fprintf(finalfp, "muli R[3], R[1], R[2]\n");
				storerv(3, QUADLIST->op3);
			}
			// / x y z
			else if (strcmp(QUADLIST->op0, "/") == 0) {
				fprintf(finalfp, "L%d: ", lcount);
				lcount++;
				loadvr(QUADLIST->op1, 1);
				loadvr(QUADLIST->op2, 2);
				fprintf(finalfp, "divi R[3], R[1], R[2]\n");
				storerv(3, QUADLIST->op3);
			}
			// out x _ _
			else if (strcmp(QUADLIST->op0, "out") == 0) {
				fprintf(finalfp, "L%d: ", lcount);
				lcount++;
				loadvr(QUADLIST->op1, 1); // R[1] = x
				fprintf(finalfp, "outi R[1]\n");
			}
			// :=, x, _, return_value
			else if (strcmp(QUADLIST->op0, ":=") == 0 && strcmp(tempquad->op0, "retv") == 0) { // ekxwrisi me return value
				fprintf(finalfp, "L%d: ", lcount);
				lcount++;
				// apothikeuete o x sti dieuthinsi pou einai apothikeumeni stin 3h thesi tou eggrafimatos drastiriopoihshs
				loadvr(QUADLIST->op1, 1); // R[1] = x
				fprintf(finalfp, "movi R[255], M[8+R[0]]\n");
				fprintf(finalfp, "movi M[R[255]], R[1]\n");
			}
			// par x CV _
			else if ((strcmp(QUADLIST->op0, "par") == 0) && (strcmp(QUADLIST->op2, "CV") == 0)) {
				fprintf(finalfp, "L%d: ", lcount);
				lcount++;
				// parametros me timi
				vartemp = searchentity(QUADLIST->op1);
				loadvr(QUADLIST->op1, 255); // R[255] = x
				// %d = framelength+12+4*parNumber
				fprintf(finalfp, "movi M[%d+R[0]], R[255]\n", entitytemp->framelength+12+4*vartemp->parPlace);
				printf("CV.TEST: %s\n", vartemp->name);
			}
			// par x REF _
			else if ((strcmp(QUADLIST->op0, "par") == 0) && (strcmp(QUADLIST->op2, "REF") == 0)) {
				fprintf(finalfp, "L%d: ", lcount);
				lcount++;
				// parametros me anafora
				vartemp = searchentity(QUADLIST->op1);
				// an h kalousa kai h metabliti exoun to idio bathos fwliasmatos
				if (entitytemp->nestinglevel == vartemp->nestinglevel) {
					entscopetemp = SCOPELIST->entitylist;
					// vartemp na anoikei stin SCOPELIST->entitylist
					while (entscopetemp != NULL) {
						if (strcmp(entscopetemp->name, vartemp->name) == 0) {
							found = 1;
						}
						entscopetemp = entscopetemp->next;
					}
					// an h parametros x einai stin kalousa sinartisi topiki metabliti h parametros me timi
					if ((vartemp->type == VAR && found == 1) || vartemp->parMode == 0) {
						fprintf(finalfp, "movi R[255], R[0]\n");
						fprintf(finalfp, "muli R[254], %d\n", entitytemp->offset);
						fprintf(finalfp, "addi R[255], R[254], R[255]\n");
						fprintf(finalfp, "movi M[%d+R[0]], R[255]\n", entitytemp->framelength+12+4*vartemp->parPlace+8);
					}
					// alliws an h parametros x exei perastei stin kalousa sinartisi me anafora
					else if (vartemp->parMode == 1) {
						fprintf(finalfp, "movi R[255], R[0]\n");
						fprintf(finalfp, "muli R[254], %d\n", entitytemp->offset);
						fprintf(finalfp, "addi R[255], R[254], R[255]\n");
						fprintf(finalfp, "movi R[1], M[R[255]]\n");
						fprintf(finalfp, "movi M[%d+R[0]], R[1]\n", entitytemp->framelength+12+4*vartemp->parPlace);
					}
				} else { // an den exoun idio bathos fwliasmatos
					entscopetemp = SCOPELIST->entitylist;
					// vartemp na anoikei stin SCOPELIST->entitylist
					while (entscopetemp != NULL) {
						if (strcmp(entscopetemp->name, vartemp->name) == 0) {
							found = 1;
						}
						entscopetemp = entscopetemp->next;
					}
					// an h parametros x einai stin kalousa sinartisi topiki metabliti h parametros me timi
					if ((vartemp->type == VAR && found == 1) || vartemp->parMode == 0) {
						gnlvcode(QUADLIST->op1);
						fprintf(finalfp, "movi M[%d+R[0]], R[255]\n", entitytemp->framelength+12+4*vartemp->parPlace);
					}
					// alliws an h parametros x exei perastei stin kalousa sinartisi me anafora
					else if (vartemp->parMode == 1) {
						gnlvcode(QUADLIST->op1);
						fprintf(finalfp, "movi R[1], M[R[255]]\n");
						fprintf(finalfp, "movi M[%d+R[0]], R[1]\n", entitytemp->framelength+12+4*vartemp->parPlace);
					}
				}
			}
			// par x RET _
			else if ((strcmp(QUADLIST->op0, "par") == 0) && (strcmp(QUADLIST->op2, "RET") == 0)) {
				fprintf(finalfp, "L%d: ", lcount);
				lcount++;
				// gemizoume to 3o pedio tis klithisas sinartisis me tin dieuthinsi tis proswrinis metablitis opou tha epistrafi h timi
				vartemp = searchentity(QUADLIST->op1);
				fprintf(finalfp, "movi R[255], R[0]\n");
				fprintf(finalfp, "movi R[254], %d\n", vartemp->offset); // offset tis proswrinis metablitis
				fprintf(finalfp, "addi R[255], R[254], R[255]\n");
				fprintf(finalfp, "movi M[%d+R[0]], R[255]\n", entitytemp->framelength+8);
			}
			// call name _ _
			else if (strcmp(QUADLIST->op0, "call") == 0) {
				fprintf(finalfp, "L%d: ", lcount);
				// arxika gemizw to 2o pedio tis stoibas tis kaloumenis sinartisis me tin dieuthinsi tis arxis tis patrikis sinartisis tis klithisas wste h kaloumeni na kserei poies vars blepei
				functemp = searchentity(QUADLIST->op1);
				// an h kalousa kai h klithisa sinartisi exoun to idio bathos fwliasmatos
				if (entitytemp->nestinglevel == functemp->nestinglevel) {
					fprintf(finalfp, "movi R[255], M[4+R[0]]\n");
					fprintf(finalfp, "movi M[%d+R[0]], R[255]\n", entitytemp->framelength+4);
				}
				// alliws an kalite sinartisi me megalitero bathos fwliasmatos apo tin kalousa
				else if (functemp->nestinglevel > entitytemp->nestinglevel) {
					fprintf(finalfp, "movi M[%d+R[0]], R[0]\n", entitytemp->framelength+4);
				}
				// epita paw stin klithisa kai ginete to gemisma tou 1ou pediou tis stoibas gia na kserw pou tha epistrepsw otan teliwsei h ektelesi tis
				fprintf(finalfp, "movi R[255], %d\n", functemp->framelength);
				fprintf(finalfp, "addi R[0], R[255], R[0]\n"); // SP stin arxi tis neas stoibas
				fprintf(finalfp, "movi R[255], $\n"); // $ dieuthinsi epomenis entolhs
				// sinolika 15 bytes
				fprintf(finalfp, "movi R[254], 15\n"); // 4 bytes
				fprintf(finalfp, "addi R[255], R[255], R[254]\n"); // 4 bytes
				fprintf(finalfp, "movi M[R[0]], R[255]\n"); // 4 bytes
				fprintf(finalfp, "jmp L%d\n", start); // 3 bytes
				// epistrofi stin kalousa
				fprintf(finalfp, "movi R[255], %d\n", entitytemp->framelength);
				fprintf(finalfp, "subi R[0], R[0], R[255]\n"); // deikths stoibas
				lcount++;
			}
			// end_block x _ _
			else if (strcmp(QUADLIST->op0, "end_block") == 0) {
				fprintf(finalfp, "L%d: ", lcount);
				lcount++;
				fprintf(finalfp, "jmp M[R[0]]\n");
			}
			// halt _ _ _
			else if (strcmp(QUADLIST->op0, "halt") == 0) {
				fprintf(finalfp, "L%d: ", lcount);
				lcount++;
				// termatismos programmatos
				fprintf(finalfp, "halt\n");
			}
			QUADLIST = QUADLIST->next;
		}
		// telos telikou kwdika
		printtable(); // tipwma pinaka simbolwn prin to klisimo tou block
		closescope(); // klisimo scope
	} else {
		printf("error: '{' expected in line %d but found %s\n", lines, str);
		exit(1);
	}
}

void declarations() {
	if (tk == DECLARETK) {
		varlist();
		if (tk == ENDDECLARETK) {
			tk = lex(str);
			comments();
		} else {
			printf("error: 'enddeclare' expected in line %d but found %s\n", lines, str);
			exit(1);
		}
	}
}

void varlist() {
	tk = lex(str);
	while (tk == IDTK) {
		insertentity(str, VAR, -1); // bazw tis metablites pou ekana declare ston pinaka
		tk = lex(str);
		if (tk == COMMATK) {
			tk = lex(str);
			if (tk != IDTK) {
				printf("error: ID expected in line %d but found %s\n", lines, str);
				exit(1);
			}
		}
	}
}

void subprograms() {
	while (tk == FUNCTIONTK || tk == PROCEDURETK) {
		func();
	}
}

void func() {
	if (tk == PROCEDURETK || tk == FUNCTIONTK) {
		tk = lex(str);
		if (tk == IDTK) {
			funcbody();
		} else {
			printf("error: ID expected in line %d but found %s\n", lines, str);
			exit(1);
		}
	} else {
		printf("error: 'procedure' or 'function' expected in line %d but found %s\n", lines, str);
		exit(1);
	}
}

void funcbody() {
	char name[MAXLEN];
	int called_by;

	called_by = 0; // oxi main
	strcpy(name, str);
	insertentity(name, FUNC, -1); // bazw onoma sinartisis ston pinaka
	createscope(name);
	formalpars(name);
	block(name, called_by);
}

void formalpars(char name[MAXLEN]) {
	tk = lex(str);
	if (tk == PAR2OPENTK) {
		formalparlist(name);
		if (tk == PAR2CLOSETK) {
			tk = lex(str);
		} else {
			printf("error: ')' expected in line %d but found %s\n", lines, str);
			exit(1);
		}
	} else {
		printf("error: '(' expected in line %d but found %s\n", lines, str);
		exit(1);
	}
}

void formalparlist(char name[MAXLEN]) {
	tk = lex(str);
	formalparitem(name);
	while (tk == COMMATK) {
		tk = lex(str);
		formalparitem(name);
	}
}

void formalparitem(char name[MAXLEN]) {
	int type;

	if (tk == INTK || tk == INOUTTK || tk == COPYTK) {
		// krataw ti tipos einai(CV gia in, REF gia inout, COPY gia copy) kai to stelnw stin addargument
		if (tk == INTK) {
			type = 0;
		} else if (tk == INOUTTK) {
			type = 1;
		} else {
			type = 2;
		}
		tk = lex(str);
		if (tk == IDTK) {
			insertentity(str, PARAM, type); // bazw tin parametro pou exei h sinartisi
			addargument(name, type); // kai tin bazw kai sti lista me ta arguments tis
			tk = lex(str);
		} else {
			printf("error: ID expected in line %d but found %s\n", lines, str);
			exit(1);
		}
	} else {
		printf("error: 'in' or 'inout or 'copy' expected in line %d but found %s\n", lines, str);
		exit(1);
	}
}

void sequence() {
	comments();
	statement();
	while (tk == MARKTK) {
		tk = lex(str);
		comments();
		statement();
	}
}

void statement() {
	if (tk == IDTK) {
		assignment_stat();
	} else if (tk == IFTK) {
		if_stat();
	} else if (tk == WHILETK) {
		while_stat();
	} else if (tk == DOTK) {
		do_while_stat();
	} else if (tk == EXITTK) {
		exit_stat();
	} else if (tk == RETURNTK) {
		return_stat();
	} else if (tk == PRINTTK) {
		print_stat();
	} else if (tk == INCASETK) {
		incase_stat();
	} else if (tk == FORCASETK) {
		forcase_stat();
	} else if (tk == CALLTK) {
		call_stat();
	}
}

void assignment_stat() {
	char name[MAXLEN];
	char eplace[MAXLEN];

	strcpy(name, str);
	tk = lex(str);
	if (tk == ASSIGNTK) {
		tk = lex(str);
		expression(eplace);
		genquad(":=", eplace, "_", name);
	} else {
		printf("error: ':=' expected in line %d but found %s\n", lines, str);
		exit(1);
	}
}

void expression(char eplace[MAXLEN]) {
	char t1place[MAXLEN];
	char t2place[MAXLEN];
	char *tmp;
	char oper[MAXLEN];

	optional_sign();
	term(t1place);
	while (tk == ADDTK || tk == MINUSTK) {
		add_oper(oper);
		term(t2place);
		tmp = newTemp(); // nea prosorini metabliti pou tha kratisi to mexri stigmis apotelesma
		genquad(oper, t1place, t2place, tmp); // paragwgi tetradas pou prostheti to mexri stigmis apotelesma sto T_2
		strcpy(t1place, tmp); // to mexri stigmis apotelesma topothetite stin T_1 wste na xrisimopoieithei an iparksei epomeno T_2
	}
	strcpy(eplace, t1place); // otan den iparksei allo T_2 to apotelesma einai sto T_1
}

void optional_sign() {
	char oper[MAXLEN];

	if (tk == ADDTK || tk == MINUSTK) {
		add_oper(oper);
	}
}

void add_oper(char oper[MAXLEN]) {
	if (tk == ADDTK || tk == MINUSTK) {
		strcpy(oper, str);
		tk = lex(str);
	} else {
		printf("error: '+' or '-' expected in line %d but found %s\n", lines, str);
		exit(1);
	}
}

void multi_oper(char oper[MAXLEN]) {
	if (tk == MULTITK || tk == DIVTK) {
		strcpy(oper, str);
		tk = lex(str);
	} else {
		printf("error: '*' or '/' expected in line %d but found %s\n", lines, str);
		exit(1);
	}
}

void relational_oper(char oper[MAXLEN]) {
	if (tk == EQTK || tk == MORETK || tk == LESSTK || tk == MOREEQTK || tk == LESSEQTK || tk == DIFFTK) {
		strcpy(oper, str);
		tk = lex(str);
	} else {
		printf("error: relop expected in line %d but found %s\n", lines, str);
		exit(1);
	}
}

void term(char tplace[MAXLEN]) {
	char f1place[MAXLEN];
	char f2place[MAXLEN];
	char *tmp;
	char oper[MAXLEN];

	factor(f1place);
	while (tk == MULTITK || tk == DIVTK) { // idia logiki me to + kai -
		multi_oper(oper);
		factor(f2place);
		tmp = newTemp();
		genquad(oper, f1place, f2place, tmp);
		strcpy(f1place, tmp);
	}
	strcpy(tplace, f1place);
}

void factor(char fplace[MAXLEN]) {
	char eplace[MAXLEN], name[MAXLEN];

	if (tk == NUMTK) {
		strcpy(fplace, str);
		tk = lex(str);
	} else if (tk == PAR2OPENTK) {
		tk = lex(str);
		expression(eplace);
		strcpy(fplace, eplace);
		if (tk == PAR2CLOSETK) {
			tk = lex(str);
		} else {
			printf("error: ')' expected in line %d but found %s\n", lines, str);
			exit(1);
		}
	} else if (tk == IDTK) {
		strcpy(fplace, str);
		strcpy(name, str);
		tk = lex(str);
		idtail(name, fplace);
	} else {
		printf("error: num or '(' or ID expected in line %d but found %s\n", lines, str);
		exit(1);
	}
}

void idtail(char name[MAXLEN], char fplace[MAXLEN]) {
	int called_by;

	called_by = 1; // 1 simenei kalestike apo metabliti kai oxi call_stat()
	actualpars(name, called_by, fplace);
}

void actualpars(char name[MAXLEN], int called_by, char fplace[MAXLEN]) {
	char *tmp;

	if (tk == PAR2OPENTK) {
		actualparlist();
		if (tk == PAR2CLOSETK) {
			if (called_by == 0) { // an kalestike apo call_stat()
				genquad("call", name, "_", "_"); // aplh tetrada xwris temp
			} else { // alliws an exw metabliti := kalesma sinartisis
				tmp = newTemp(); // xreiazomai proswrini metabliti
				genquad("par", tmp, "RET", "_");
				genquad("call", name, "_", "_");
				strcpy(fplace, tmp);
			}
			tk = lex(str);
		} else {
			printf("error: ')' expected in line %d but found %s\n", lines, str);
			exit(1);
		}
	}
}

void actualparlist() {
	tk = lex(str);
	actualparitem();
	while (tk == COMMATK) {
		tk = lex(str);
		actualparitem();
	}
}

void actualparitem() {
	char eplace[MAXLEN];

	if (tk == INTK) {
		tk = lex(str);
		expression(eplace);
		genquad("par", eplace, "CV", "_"); // call by value
	} else if (tk == INOUTTK) {
		tk = lex(str);
		if (tk == IDTK) {
			strcpy(eplace, str);
			genquad("par", eplace, "REF", "_"); // reference
			tk = lex(str);
		} else {
			printf("error: ID expected in line %d but found %s\n", lines, str);
			exit(1);
		}
	} else if (tk == COPYTK) {
		tk = lex(str);
		if (tk == IDTK) {
			strcpy(eplace, str);
			genquad("par", eplace, "COPY", "_"); // return value
			tk = lex(str);
		} else {
			printf("error: ID expected in line %d but found %s\n", lines, str);
			exit(1);
		}
	} else {
		printf("error: 'in' or 'inout' or 'copy' expected in line %d but found %s\n", lines, str);
		exit(1);
	}
}

void if_stat() {
	struct rule c;
	struct list *iflist;

	c.true = emptylist();
	c.false = emptylist();

	tk = lex(str);
	if (tk == PAR2OPENTK) {
		tk = lex(str);
		condition(&c);
		if (tk == PAR2CLOSETK) {
			// siblirwnw asiblirwtes tetrades sto if kai else me backpatch
			backpatch(c.true, nextquad());
			brack_or_stat();
			iflist = makelist(nextquad());
			genquad("jump", "_", "_", "_"); // an ektelei to if na min ektelestoun oi entoles tou else
			backpatch(c.false, nextquad());
			elsepart();
			backpatch(iflist, nextquad()); // an ektelestei to if na min ektelestoun oi entoles tou else
		} else {
			printf("error: ')' expected in line %d but found %s\n", lines, str);
			exit(1);
		}
	} else {
		printf("error: '(' expected in line %d but found %s\n", lines, str);
		exit(1);
	}
}

void elsepart() {
	tk = lex(str);
	if (tk == ELSETK) {
		brack_or_stat();
	}
}

void while_stat() {
	struct rule c;
	char buffer[MAXLEN];
	int quad;

	exitlist = emptylist();
	c.true = emptylist();
	c.false = emptylist();

	quad = nextquad();
	tk = lex(str);
	if (tk == PAR2OPENTK) {
		tk = lex(str);
		condition(&c);
		if (tk == PAR2CLOSETK) {
			tk = lex(str);
			if (tk == PAR1OPENTK) {
				// me backpatch siblirwsh twn tetradwn pou exoun minei asiblirwtes kai prepei na siblirwthoun me tin epomeni tetrada to true panw sti while kai to false eksw apo ti domi
				backpatch(c.true, nextquad());
				brack_or_stat();
				sprintf(buffer, "%d", quad);
				genquad("jump", "_", "_", buffer); // metabasi sti arxi tis sinthikis gia na ksanagini elegxos
				backpatch(c.false, nextquad());
				// gia tin exit
				if (exitlist != NULL) {
					backpatch(exitlist, nextquad());
				}
				exitlist = NULL; // gia na xrisimopoihthei apo allh while h do_while
			} else {
				printf("error: '{' expected in line %d but found %s\n", lines, str);
				exit(1);
			}
		} else {
			printf("error: ')' expected in line %d but found %s\n", lines, str);
			exit(1);
		}
	} else {
		printf("error: '(' expected in line %d but found %s\n", lines, str);
		exit(1);
	}
}

void do_while_stat() {
	struct rule c;
	int quad;

	exitlist = emptylist();
	c.true = emptylist();
	c.false = emptylist();

	tk = lex(str);
	if (tk == PAR1OPENTK) {
		quad = nextquad();
		brack_or_stat();
		tk = lex(str);
		if (tk == WHILETK) {
			tk = lex(str);
			if (tk == PAR2OPENTK) {
				tk = lex(str);
				condition(&c);
				backpatch(c.false, quad); // oi tetrades autes prepei na metaboun stin arxi tis sinthikis gia na ksanaginei elegxos
				backpatch(c.true, nextquad()); // siblirwsh tetradwn pou prepei na siblirwthoun me tin epomenh tetrada eksw apo ti domh do_while
				if (tk == PAR2CLOSETK) {
					tk = lex(str);
					if (exitlist != NULL) {
						backpatch(exitlist, nextquad());
					}
					exitlist = NULL; // gia na xrisimopoihthei apo allh while h do_while
				} else {
					printf("error: ')' expected in line %d but found %s\n", lines, str);
					exit(1);
				}
			} else {
				printf("error: '(' expected in line %d but found %s\n", lines, str);
				exit(1);
			}
		} else {
			printf("error: 'while' expected in line %d but found %s\n", lines, str);
			exit(1);
		}
	} else {
		printf("error: '{' expected in line %d but found %s\n", lines, str);
		exit(1);
	}
}

void condition(struct rule *c) {
	struct rule q1, q2;

	q1.true = emptylist();
	q2.false = emptylist();

	boolterm(&q1);
	// metafora tetradwn apo ti lista q1 sti lista c
	c->true = q1.true;
	c->false = q1.false;

	while (tk == ORTK) {
		tk = lex(str);
		backpatch(c->false, nextquad()); // siblirwsh oswn tetradwn boroun na siblirwthoun mesa ston kanona
		boolterm(&q2);
		c->true = merge(c->true, q2.true); // sisswreush sti lista true twn tetradwn pou den boroun na siblirwthoun kai antistixoun se true
		c->false = q2.false; // h q2.false exei tetrada pou antoistixei se false
	}
}

void boolterm(struct rule *q) {
	struct rule r1, r2;

	r1.true = emptylist();
	r2.false = emptylist();

	boolfactor(&r1);
	// metafora tetradwn apo ti lista r1 sti lista q
	q->true = r1.true;
	q->false = r2.false;

	while (tk == ANDTK) {
		tk = lex(str);
		backpatch(q->true, nextquad()); // siblirwsh oswn tetradwn boroun na siblirwthoun mesa ston kanona
		boolfactor(&r2);
		q->false = merge(q->false, r2.false); // siswreush sti lista false twn tetradwn pou den boroun na siblirwthoun kai antoistixoun se false
		q->true = r1.true; // h lista tru periexe tin tetrada pou antoistixei se true
	}
}

void boolfactor(struct rule *r) {
	char e1place[MAXLEN];
	char e2place[MAXLEN];
	char oper[MAXLEN];
	struct rule c;

	c.true = emptylist();
	c.false = emptylist();

	if (tk == NOTTK) {
		tk = lex(str);
		if (tk == PAR3OPENTK) {
			tk = lex(str);
			condition(&c);
			r->true = c.false;
			r->false = c.true;
			if (tk == PAR3CLOSETK) {
				tk = lex(str);
			} else {
				printf("error: ']' expected in line %d but found %s\n", lines, str);
				exit(1);
			}
		} else {
			printf("error: '[' expected in line %d but found %s\n", lines, str);
			exit(1);
		}
	} else if (tk == PAR3OPENTK) {
		tk = lex(str);
		condition(&c);
		r->true = c.true;
		r->false = c.false;
		if (tk == PAR3CLOSETK) {
			tk = lex(str);
		} else {
			printf("error: ']' expected in line %d but found %s\n", lines, str);
			exit(1);
		}
	} else {
		expression(e1place);
		relational_oper(oper);
		expression(e2place);
		// dimiourgia mi siblirwmenis tetradas kai eisagwgh sti lista mi siblirwmenwn tetradwn gia tin true apotimisi tis relational_oper
		r->true = makelist(nextquad());
		genquad(oper, e1place, e2place, "_");
		// dimiourgia mi siblirwmenis tetradas kai eisagwgh sti lista mi siblirwmenwn tetradwn gia tin false apotimisi tis relational_oper
		r->false = makelist(nextquad());
		genquad("jump", "_", "_", "_");
	}
}

void brack_or_stat() {
	tk = lex(str);
	if (tk == PAR1OPENTK) {
		tk = lex(str);
		brackets_seq();
		if (tk == PAR1CLOSETK) {
			// do nothing
		} else {
			printf("error: '}' expected in line %d but found %s\n", lines, str);
			exit(1);
		}
	} else {
		statement();
	}
}

void brackets_seq() {
	sequence();
}

void exit_stat() {
	struct list *newlist;
	char buffer[MAXLEN];
	int quad;

	// an einai exit: makelist, merge me emptylist tis do_while h while kai backpatch sto telos autwn
	newlist = makelist(nextquad());
	merge(exitlist, newlist);
	quad = nextquad();
	sprintf(buffer, "%d", quad);
	genquad("jump", "_", "_", buffer);
	tk = lex(str);
}

void return_stat() {
	char eplace[MAXLEN];

	tk = lex(str);
	if (tk == PAR2OPENTK) {
		tk = lex(str);
		expression(eplace);
		if (tk == PAR2CLOSETK) {
			genquad("retv", eplace, "_", "_"); // return value
			tk = lex(str);
		} else {
			printf("error: ')' expected in line %d but found %s\n", lines, str);
			exit(1);
		}
	} else {
		printf("error: '(' expected in line %d but found %s\n", lines, str);
		exit(1);
	}
}

void print_stat() {
	char eplace[MAXLEN];

	tk = lex(str);
	if (tk == PAR2OPENTK) {
		tk = lex(str);
		expression(eplace);
		if (tk == PAR2CLOSETK) {
			genquad("out", eplace, "_", "_");
			tk = lex(str);
		} else {
			printf("error: ')' expected in line %d but found %s\n", lines, str);
			exit(1);
		}
	} else {
		printf("error: '(' expected in line %d but found %s\n", lines, str);
		exit(1);
	}
}

void incase_stat() {
	struct rule c;
	char buffer[MAXLEN];
	int quad;

	c.true = emptylist();
	c.false = emptylist();

	quad = nextquad();
	genquad(":=", "1", "_", "flag_"); // 1 = false gia to flag

	tk = lex(str);
	if (tk == PAR1OPENTK) {
		tk = lex(str);
		while (tk == WHENTK) {
			tk = lex(str);
			if (tk == PAR2OPENTK) {
				tk = lex(str);
				condition(&c);
				if (tk == PAR2CLOSETK) {
					backpatch(c.true, nextquad());
					genquad(":=", "0", "_", "flag_");
					brack_or_stat();
					backpatch(c.false, nextquad());
					if (tk == PAR1CLOSETK) {
						sprintf(buffer, "%d", quad);
						genquad("=", "0", "flag_", buffer);
						tk = lex(str);
					} else {
						printf("error: '}' expected in line %d but found %s\n", lines, str);
						exit(1);
					}
				} else {
					printf("error: ')' expected in line %d but found %s\n", lines, str);
					exit(1);
				}
			} else {
				printf("error: '(' expected in line %d but found %s\n", lines, str);
				exit(1);
			}
		}
	} else {
		printf("error: '{' expected in line %d but found %s\n", lines, str);
		exit(1);
	}
}

void forcase_stat() {
	struct rule c;
	char buffer[MAXLEN];
	int quad;

	c.true = emptylist();
	c.false = emptylist();

	quad = nextquad();
	tk = lex(str);
	if (tk == PAR1OPENTK) {
		tk = lex(str);
		while (tk == WHENTK) {
			tk = lex(str);
			if (tk == PAR2OPENTK) {
				tk = lex(str);
				condition(&c);
				if (tk == PAR2CLOSETK) {
					backpatch(c.true, nextquad());
					brack_or_stat();
					sprintf(buffer, "%d", quad);
					genquad("jump", "_", "_", buffer);
					backpatch(c.false, nextquad());
					if (tk == PAR1CLOSETK) {
						tk = lex(str);
					} else {
						printf("error: '}' expected in line %d but found %s\n", lines, str);
						exit(1);
					}
				} else {
					printf("error: ')' expected in line %d but found %s\n", lines, str);
					exit(1);
				}
			} else {
				printf("error: '(' expected in line %d but found %s\n", lines, str);
				exit(1);
			}
		}
	} else {
		printf("error: '{' expected in line %d but found %s\n", lines, str);
		exit(1);
	}
}

void call_stat() {
	char name[MAXLEN];
	int called_by;

	called_by = 0; // 0 simenei to kalese h call_stat()
	tk = lex(str);
	if (tk == IDTK) {
		strcpy(name, str);
		tk = lex(str);
		actualpars(name, called_by, 0); // to kalei h call_stat()
		tk = lex(str);
	} else {
		printf("error: ID expected in line %d but found %s\n", lines, str);
		exit(1);
	}
}

void comments() {
	// an exw anoigma sxoliwn
	if (tk == COMMSEP1TK) {
		tk = lex(str); // sinexizw mexri na brw klisimo
		if (tk == COMMSEP2TK) {
			tk = lex(str); // kai katanalwnw to epomeno apo auto
		} else {
			printf("error: '*/' expected in line %d but found %s\n", lines, str);
			exit(1);
		}
	}
}

//*********************************************************************//
//								 inter                                 //
//*********************************************************************//
// epistrefei ari8mo epomenhs tetradas pou 8a parax8ei
int nextquad() {
	return quadlabel;
}

// paragei thn tetrada op, x, y, z
void genquad(char op[MAXLEN], char x[MAXLEN], char y[MAXLEN], char z[MAXLEN]) {
	struct quad *tempquad;
	struct quad *prev, *temp;

	tempquad = (struct quad *)malloc(sizeof(struct quad));
	if(tempquad == NULL) {
		printf("error in malloc\n");
		exit(1);
	}

	// char *op0, *op1, *op2, *op3;
	strcpy(tempquad->op0, op);
	strcpy(tempquad->op1, x);
	strcpy(tempquad->op2, y);
	strcpy(tempquad->op3, z);

	tempquad->label = quadlabel;
	quadlabel += 10; // 100, 110, 120, ... etiketes tetradwn
	tempquad->next = NULL; // den iparxei epomeni

	// tipwma tetradas sto arxeio
	fprintf(fout, "%d: %s %s %s %s\n", tempquad->label, tempquad->op0, tempquad->op1, tempquad->op2, tempquad->op3);

	prev = NULL;
	temp = QUADLIST;
	while(temp != NULL) {
		prev = temp;
		temp = temp->next;
	}
	if(prev == NULL) { // an den exoume alles tetrades einai h prwth
		QUADLIST = tempquad;
	} else { // alliws thetw tin prohgoumeni san epomeni
		prev->next = tempquad;
	}

}

// epistrefei kainourgia proswrini diathesimi metabliti(T_1, T_2, ...)
char *newTemp() {
	char *t;

	t = (char *)malloc(MAXLEN*sizeof(char));
	if(t == NULL) {
		printf("error in malloc\n");
		exit(1);
	}

	sprintf(t, "T_%d", tempcount); // T_1, T_2, ...
	insertentity(t, TEMP, -1); // kalite apo diafora simia tou sintaktikou ara boliko na bei edw
	tempcount++;

	return t;
}

// dimiourgei kainourgia adeia lista etiketwn tetradwn
struct list *emptylist() {
	struct list *alist;

	alist = (struct list *)malloc(sizeof(struct list));
	if (alist == NULL) {
		printf("error in malloc\n");
		exit(1);
	}

	alist->next = NULL;

	return alist;
}

// dimiourgei lista me stoixeio x tetradwn etiketwn
struct list *makelist(int x) {
	struct list *alist;

	alist = (struct list *)malloc(sizeof(struct list));
	if (alist == NULL) {
		printf("error in malloc\n");
		exit(1);
	}

	alist->quad = x;
	alist->next = NULL;

	return alist;
}

// enwnei to list2 sto list1
struct list *merge(struct list *list1, struct list *list2) {
	struct list *prev, *temp;

	// an kapoia apo tis dio listes adeies apla epestrepse tin alli
	if (list1 == NULL) return list2;
	if (list2 == NULL) return list1;

	// sto telos tis list1 bainei h list2
	prev = NULL;
	temp = list1;
	while(temp != NULL) {
		prev = temp;
		temp = temp->next;
	}
	prev->next = list2; // teleutaio tis list1 ginete arxi tis list2

	return list1;
}

// kanei to teleutaio teloumeno kathe tetradas iso me z
void backpatch(struct list *alist, int z) {
	struct list *templist; // gia ta stoixeia tis alist
	struct quad *tempquad; // gia ta stoixeia tis QUADLIST

	templist = alist;
	while (templist != NULL) {
		// na brw to swsto label
		tempquad = QUADLIST;
		while (tempquad != NULL) {
			if(tempquad->label == templist->quad) {
				sprintf(tempquad->op3, "%d", z); // teleutaio teloumeno to z
				break; // brethike to swsto label
			}
			tempquad = tempquad->next; // stin epomeni tetrada
		}
		templist = templist->next; // stin epomeni lista
	}
}

//*********************************************************************//
//								 symbol                                //
//*********************************************************************//
// dimiourgei kainourgio scope kai to bazei sto entitylist
void createscope(char name[MAXLEN]) {
	struct scope *newscope; // kainourgios kombos

	newscope = (struct scope *)malloc(sizeof(struct scope));
	if (newscope == NULL) {
		printf("error in malloc\n");
		exit(1);
	}

	strcpy(newscope->name, name);
	newscope->entitylist = NULL; // arxikopoiw se NULL thn lista me tis ontothtes
	if (SCOPELIST == NULL) { // einai mono gia ton prwto prwto pou dimiourgoume 
		SCOPELIST = newscope;
		newscope->nestinglevel = 0;
		newscope->enclosingScope = NULL;
	} else {
		newscope->nestinglevel = SCOPELIST->nestinglevel+1;
		newscope->enclosingScope = SCOPELIST;
		SCOPELIST = newscope; // kainourgios
	}
}

// diagrafi stin korifi tis stoibas
void closescope() {
	if (SCOPELIST == NULL) { // an den exw scope
		printf("error: no scope to close found\n");
		exit(1);
	}

	// diagrafei apo tin korifi tis SCOPELIST
	SCOPELIST = SCOPELIST->enclosingScope; // to bazw na deixnei se ena pio katw
}

// eisagwgi kainourgias ontothtas stin korifi tis stoibas
void insertentity(char name[MAXLEN], int type, int parMode) {
	struct entity *newentity;
	struct entity *temp, *prev;
	int tempoffset;

	newentity = (struct entity *)malloc(sizeof(struct entity));
	if(newentity == NULL) {
		printf("error in malloc\n");
		exit(1);
	}

	strcpy(newentity->name, name); // onoma entity
	newentity->type = type; // tipos: defined PARAM(0), VAR(1), TEMP(2), FUNC(3)
	newentity->nestinglevel = SCOPELIST->nestinglevel; // bathos fwliasmatos ston pinaka gia teliko kwdika

	if (type == FUNC) { // an einai sinartisi
		newentity->startQuad = nextquad() + 10;
		newentity->next = NULL;
		newentity->parMode = parMode; // thane -1 de mas endiaferei
		prev = NULL; 
		temp = SCOPELIST->entitylist; // vazw ton deikth sthn arxh ths listas
		while (temp != NULL) { // gia na dw an uparxoun hdh ontothtes, pou na valw to epomeno
			prev = temp;
			tempoffset = temp->offset;
			temp = temp->next;
		}
		if (prev == NULL) { // kenh lista
			SCOPELIST->entitylist = newentity; // arxikopoiw thn lista sto neo kombo
			newentity->offset = 12;
		} else {
			prev->next = newentity; // bazw ton teleutaio na deixnei sto kainourgio
			newentity->offset = tempoffset; // na menei idio to offset otan einai function
		}
	} else if (type == PARAM) { // an einai parametros
		newentity->parPlace = 1;
		newentity->next = NULL;
		newentity->parMode = parMode; // 0: CV, 1: REF, 2: RET
		prev = NULL;
		temp = SCOPELIST->entitylist; // vazw ton deikth sthn arxh ths listas
		while (temp != NULL) { // gia na dw an uparxoun hdh ontothtes, pou na valw to epomeno
			prev = temp;
			tempoffset = temp->offset;
			newentity->parPlace = newentity->parPlace+1;
			temp = temp->next;
		}
		if (prev == NULL) { // kenh lista
			SCOPELIST->entitylist = newentity; // arxikopoiw thn lista sto neo kombo
			newentity->offset = 12;
		} else {
			prev->next = newentity; // bazw ton teleutaio na deixnei sto kainourgio
			newentity->offset = tempoffset + 4; // offset+=4 gia metabliti parametro h proswrini metabliti
		}
	} else { // an einai metabliti h temp metabliti
		newentity->next = NULL;
		newentity->parMode = parMode; // thane -1 de mas endiaferei
		prev = NULL; 
		temp = SCOPELIST->entitylist; // vazw ton deikth sthn arxh ths listas
		while (temp != NULL) { // gia na dw an uparxoun hdh ontothtes, pou na valw to epomeno
			prev = temp;
			tempoffset = temp->offset;
			temp = temp->next;
		}
		if (prev == NULL) { // kenh lista
			SCOPELIST->entitylist = newentity; // arxikopoiw thn lista sto neo kombo
			newentity->offset = 12;
		} else {
			prev->next = newentity; // bazw ton teleutaio na deixnei sto kainourgio
			newentity->offset = tempoffset + 4; // offset+=4 gia metabliti parametro h proswrini metabliti
		}
	}
}

// bazei tipous metablitwn sto entity table
void addargument(char name[MAXLEN], int parMode) {
	struct entity *entitytemp;
	int i;

	// anazitisi deikth ontotitas gia na baloume to argument
	entitytemp = searchentity(name);
	if (entitytemp == NULL) {
		return;
	}

	// na brw ti thesi i gia na to balw
	for (i = 0; i < MAXLEN; i++ ) {
		if (strlen(entitytemp->arglist[i]) == 0) { // an einai adeio
			break; // auth th thesi theloume
		}
	}
	if (parMode == 0) { // in
		strcpy(entitytemp->arglist[i], "CV");
	} else if (parMode == 1) { // inout
		strcpy(entitytemp->arglist[i], "REF");
	} else if (parMode == 2) { // copy
		strcpy(entitytemp->arglist[i], "COPY");
	} else { // error
		printf("error: in argument table\n");
		exit(1);
	}
}

// mou epistrefei ena deikth pros thn antistoixh ontothta, an den iparxei ontothta epistrefei NULL
struct entity *searchentity(char name[MAXLEN]) {
	struct scope *scopetemp; // gia na diatreksw ta scopes
	struct entity *entitytemp, *foundentity; // gia na diatreksw ta entities
	int found;

	// tha arxizei anagkastika apo panw mexri katw kai tha briskei tin prwth emfanisi tou name
	found = 0;
	scopetemp = SCOPELIST;
	while (scopetemp != NULL) {
		entitytemp = scopetemp->entitylist;
		while (entitytemp != NULL){
			if (strcmp(entitytemp->name, name) == 0) { // an iparxei sth lista
				found = 1;
				foundentity = entitytemp;
			}
			entitytemp = entitytemp->next;
		}
		scopetemp = scopetemp->enclosingScope;
	}

	if (found == 1) {
		return foundentity;
	} else {
		return NULL;
	}
}

void printtable() {
	struct scope *scopetemp; // diatrexw kathe scope
	struct entity *entitytemp; // diatrexw kathe entity
	int i; // gia na kanei kalitero to tipwma pinaka

	scopetemp = SCOPELIST;
	while (scopetemp != NULL) {
		printf("(%s) -> ", scopetemp->name);
		entitytemp = scopetemp->entitylist;
		while (entitytemp != NULL){
			if (entitytemp->type == VAR || entitytemp->type == PARAM || entitytemp->type == TEMP) {
				printf("[%s:(%d)] -> ", entitytemp->name, entitytemp->offset);
			} else {
				printf("[%s:(", entitytemp->name);
				for (i = 0; i < MAXLEN; i++) {
					if (strlen(entitytemp->arglist[i]) == 0) { // gia na mi tipwnei kai ta kena
						break;
					}
					if (i == 0) { // an einai stin prwth epanalipsi
						printf("%s", entitytemp->arglist[i]); // na to tipwsi xwris komma
					} else { // alliws me kommata
						printf(", %s", entitytemp->arglist[i]);
					}
				}
				// meta ta args menei to framelength tis sinartisis
				printf("),%d] -> ", entitytemp->framelength);
			}
			entitytemp = entitytemp->next;
		}
		printf("\n");
		scopetemp = scopetemp->enclosingScope; // proxwraei sto epomeno shmeio tis stoibas
	}
	printf("\n");
}

//*********************************************************************//
//								  final                                //
//*********************************************************************//
// metaferei ston R[255] tin dieuthinsi mias mi topikis metablitis
void gnlvcode(char v[MAXLEN]) {
	struct entity *s_ent;
	int i;

	s_ent = searchentity(v);

	fprintf(finalfp, "movi R[255], M[4+R[0]]\n"); // R[255] = M[4+R[0]]
	for (i = 0; i < SCOPELIST->nestinglevel; i++) {
		fprintf(finalfp, "movi R[255], M[4+R[255]]\n"); // R[255] = M[4+R[255]]
	}
	fprintf(finalfp, "movi R[254], %d\n", s_ent->offset); // R[254] = offset
	fprintf(finalfp, "addi R[255], R[254], R[255]\n"); // R[255] = R[254]+R[255]
}

// metaferw apo tin mnhmh mia metabliti se kataxwriti
void loadvr(char v[MAXLEN], int r) {
	struct entity *s_ent;

	s_ent = searchentity(v);

	// 1: an to v einai arithmos
	if (isdigit(v[0])) {
		fprintf(finalfp, "movi R[%d], %s\n", r, v); // R[r] = v
	}
	// 2: an to v einai global metabliti
	else if (s_ent->nestinglevel == 0) {
		fprintf(finalfp, "movi R[%d], M[%d]\n", r, 600+s_ent->offset); // R[r] = M[600+offset]
	}
	// 3: an to v einai topiki metabliti h tipiki parametros me timi kai bathos fwliamsmatos iso me to trexon h proswrini metabliti
	else if ((s_ent->type == VAR && s_ent->nestinglevel != 0) || (s_ent->type == PARAM && s_ent->parMode == 0 && s_ent->nestinglevel == SCOPELIST->nestinglevel) || (s_ent->type == TEMP)) {
		fprintf(finalfp, "movi R[%d], M[%d+R[0]]\n", r, s_ent->offset); // R[r] = M[offset+R[0]]
	}
	// 4: an v tipiki parametros me anafora kai bathos fwliasmatos iso me to trexon
	else if (s_ent->type == PARAM && s_ent->parMode == 1 && s_ent->nestinglevel == SCOPELIST->nestinglevel) {
		fprintf(finalfp, "movi R[255], M[%d+R[0]]\n", s_ent->offset); // R[255] = M[offset+R[0]]
		fprintf(finalfp, "movi R[%d], M[R[255]]\n", r); // R[r] = M[R[255]]
	}
	// 5: an v tipiki parametros me timi kai bathos fwliasmatos iso me to trexon h topiki metabliti kai bathos fwliasmatos iso me to trexon
	else if(((s_ent->type == PARAM && s_ent->parMode == 0) || (s_ent->type == VAR && s_ent->nestinglevel != 0)) && s_ent->nestinglevel == SCOPELIST->nestinglevel) {
		gnlvcode(v); // wste o R[255] na exei ti swsti dieuthinsi
		fprintf(finalfp, "movi R[%d], M[R[255]]\n", r); // R[r] = M[R[255]]
	}
	// 6: an v tipiki parametros me anafora kai bathos fwliasmatos mikrotero tou trexon
	else if (s_ent->type == PARAM && s_ent->parMode == 1 && s_ent->nestinglevel < SCOPELIST->nestinglevel) {
		gnlvcode(v); // wste o R[255] na exei ti swsti dieuthinsi
		fprintf(finalfp, "movi R[255], M[R[255]]\n"); // R[255] = M[R[255]]
		fprintf(finalfp, "R[%d], M[R255]\n", r); // R[r] = M[R[255]]
	}
}

// topothetei ston kataxwriti r to v
void storerv(int r, char v[MAXLEN]) {
	struct entity *s_ent;

	s_ent = searchentity(v);

	// 1: an to v einai global metabliti
	if (s_ent->nestinglevel == 0) {
		fprintf(finalfp, "movi M[%d], R[%d]\n", 600+s_ent->offset, r); // M[600+offset] = R[r]
	}
	// 2: an to v einai topiki metabliti h tipiki parametros pou pernaei me timi kai bathos fwliasmatos iso me to trexon
	else if (s_ent->type == VAR || (s_ent->type == PARAM && s_ent->parMode == 0 && s_ent->nestinglevel == SCOPELIST->nestinglevel)) {
		fprintf(finalfp, "movi M[%d+R[0]], R[%d]\n", s_ent->offset, r); // M[offset+R[0]] = R[r]
	}
	// 3: an to v einai tipiki parametros pou pernaei me anafora kai bathos fwliasmatos iso me to trexon
	else if (s_ent->type == PARAM && s_ent->parMode == 1 && s_ent->nestinglevel == SCOPELIST->nestinglevel) {
		fprintf(finalfp, "movi R[255], M[%d+R[0]]\n", s_ent->offset); // R[255] = M[offset+R[0]]
		fprintf(finalfp, "movi M[R[255]], R[%d]\n", r); // M[R[255]] = R[r]
	}
	// 4: an to v einai topiki metabliti h tipiki parametros pou pernaei me timi kai bathos fwliasmatos mikrotero apo to trexon h proswrini metabliti
	else if (s_ent->nestinglevel != 0 || (s_ent->type == PARAM && s_ent->parMode == 0 && s_ent->nestinglevel < SCOPELIST->nestinglevel) || s_ent->type == TEMP) {
		gnlvcode(v); // wste o R[255] na exei ti swsti dieuthinsi
		fprintf(finalfp, "movi M[R[255]], R[%d]\n", r); // M[R[255]] = R[r]
	}
	// 5: an to v einai tipiki parametros pou pernaei me anafora kai bathos fwliasmatos mikrotero apo to trexon
	else if (s_ent->type == PARAM && s_ent->parMode == 1 && s_ent->nestinglevel < SCOPELIST->nestinglevel) {
		gnlvcode(v); // wste o R[255] na exei ti swsti dieuthinsi
		fprintf(finalfp, "movi R[255], M[R[255]]\n"); // R[255] = M[R[255]]
		fprintf(finalfp, "movi M[R[255]], R[%d]\n", r); // M[R[255]] = R[r]
	}
}

//*********************************************************************//
//								  main                                 //
//*********************************************************************//
int main(int argc, char *argv[]) {

	int i;
	char **filenames, **outfiles, **finalfiles; // files to compile

	//fin = fopen(argv[argc-1], "r"); // onoma arxeiou glwssas strange pou dinei o xrhsths
	filenames = malloc(NUM_FILES * sizeof(char *)); // gia source code arxeia
	outfiles = malloc(NUM_FILES * sizeof(char *)); // gia intermediate code arxeia
	finalfiles = malloc(NUM_FILES * sizeof(char *)); // gia final code arxeia
	for (i = 1; i <= NUM_FILES; i++) {
		filenames[i-1] = malloc(MAXLEN);
		outfiles[i-1] = malloc(MAXLEN);
		finalfiles[i-1] = malloc(MAXLEN);
		sprintf(filenames[i-1], "test%d.st", i); // test 1, 2, 3, 4, 5
		sprintf(outfiles[i-1], "2359_2355_test%d.int", i); // .int arxeia gia grapsimo
		sprintf(finalfiles[i-1], "2359_2355_final%d.fnl", i); // .fnl arxeia gia grapsimo
	}
	// gia kathe arxeio
	for (i = 0; i < NUM_FILES; i++) {
		printf("Source program name: %s\n", filenames[i]);
		lines = 1; // arxikopoihsh metriti grammwn
		quadlabel = 100; // arxikopoihsh etiketwn tetradwn
		tempcount = 1; // arxikopoihsh metriti proswrinis metablitis
		lcount = 1; // metritis label gia teliko kwdika
		fin = fopen(filenames[i], "r"); // anoigma test 1, 2, 3, 4, 5
		fout = fopen(outfiles[i], "w"); // anoigma .int arxeiwn gia grapsimo
		finalfp = fopen(finalfiles[i], "w"); // anoigma .fnl arxeiwn gia grapsimo
		program(); // kalw to programa
		// klisimo arxeiwn se kathe epanalipsi
		fclose(fin);
		fclose(fout);
		fclose(finalfp);
	}

	return 0;
}