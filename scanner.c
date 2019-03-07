//SCANER/PARSER

#include <stdio.h>
/* character classification macros */
#include <ctype.h>
#include <stdbool.h>
extern FILE *fp;
extern FILE *fo;

typedef enum token_types{
    BEGIN, END, READ, WRITE, ID, INTLITERAL, LPAREN, RPAREN,
    SEMICOLON, COMMA, ASSIGNOP, PLUSOP, MINUSOP, PIPE, SCANEOF
} token;

#define MAXIDLEN 33

typedef char string[MAXIDLEN];

typedef struct operator{ /* for operators*/
 enum op { PLUS, MINUS} operator;
} op_rec;

/* expression types*/
enum expr { IDEXPR, LITERALEXPR, TEMPEXPR };

/* for <primary> and  <expression> */

typedef struct expression{
 enum expr kind;
 union {
    string name; /* for IDEXPR, TEMPEXPR */
    int val; /* for LITERALEXPR */
 };
 op_rec sign;
} expr_rec;

extern char token_buffer[100] = "\0";

token current_token;

token next_token;

char * reserved[] = {
  "BEGIN",
  "END",
  "READ",
  "WRITE"
};

char * tokensError[] = {
  "BEGIN",
  "END",
  "READ",
  "WRITE",
  "ID",
  "INTLITERAL",
  "(",
  ")",
  ";",
  ",",
  ":=",
  "+",
  "-",
  "SCANEOF"
};

// Semantics

string symbol_table[2048];
int sizeSymbolTable = 0;

/* Is s in the symbol table? */
extern int lookup(string s){
    int size = sizeof(symbol_table)/sizeof(symbol_table[0]);
    for(int i = 0; i<=size;i++){
        if(!strcmp(symbol_table[i],s)){
            return 1;
        }
    }
    return 0;
}

/* Put s unconditionally into symbol table */
extern void enter(string s){
    strcpy(symbol_table[sizeSymbolTable], s);
    sizeSymbolTable++;
}

void generate(string a1, string a2, string a3, string a4){
     fprintf(fo, "%s ", a1);
     fprintf(fo, "%s", a2);
     fprintf(fo, "%s", a3);
     fprintf(fo, "%s\n", a4);  
}

string * extractO(op_rec e){
	if(e.operator == PLUS){
		return "Add";
	}
	else{
		return "Sub";
	}
}

void check_id(string s){
 if(! lookup(s)){
    enter(s);
    generate("Declare", s, ",Integer", "");
 }
}

char *get_temp(void){
 /* max temporary allocated so far */
 static int max_temp = 0;
 static char tempname[MAXIDLEN];

 max_temp++;
 sprintf(tempname, "Temp&%d", max_temp);
 check_id(tempname);
 return tempname;
}

void start(void){
 /*Semantic initializaitions, none needed*/
}

void finish(void){
 /*Generate code to finish program.*/
 generate("Halt", "", "", "");
}

void assign(expr_rec target, expr_rec source){
 /*Generate code for assignment*/
    string s;
    if (source.kind == LITERALEXPR){
        sprintf(s,"%d",source.val);
        strcat(s,",");
        
    }
    else{
        strcpy(s,source.name);
        strcat(s,",");
    }
 generate("Store", (s), target.name, "");
}



expr_rec gen_infix(expr_rec e1, op_rec op, expr_rec e2){
    expr_rec e_rec;
    /*An expr_rec with temp variant set. */
    e_rec.kind = TEMPEXPR;
    /*
    * Generate code for infix operation.
    * Get result temp and set up semantic record for result.
    */
    strcpy(e_rec.name, get_temp());
    string s;
    if (e1.kind == LITERALEXPR){
        sprintf(s,"%d",e1.val);
        strcat(s,",");
        
    }
    else{
        strcpy(s,e1.name);
        strcat(s,",");
    }

    string s2;
    if (e2.kind == LITERALEXPR){
        sprintf(s2,"%d",e2.val);
        strcat(s2,",");
        
    }
    else{
        strcpy(s2,e2.name);
        strcat(s2,",");
    }
    generate(extractO(op), s, s2, e_rec.name);
    e_rec.sign = e1.sign;
    return e_rec;
}

void read_id(expr_rec in_var){
    /*Generate code for read.*/
    generate("Read", in_var.name, ", Integer", "");
}  

expr_rec process_literal(void){
    expr_rec t;
    t.kind = LITERALEXPR;
    (void) sscanf(token_buffer, "%d", & t.val);
    return t;
}

expr_rec process_id(void){
    expr_rec t;
    /*
    *Convert literal to a numeric representation
    *and build semantic record.
    */
    string s;
    sprintf(s,"%s",token_buffer);
    check_id(s);
    t.kind = IDEXPR;
    string s2;
    sprintf(s2,"%s",token_buffer);
    strcpy(t.name, s);
    return t;
}

void write_expr(expr_rec out_expr){
    string s;
    if (out_expr.kind == LITERALEXPR){
        sprintf(s,"%d",out_expr.val);
        strcat(s,",");
        
    }
    else{
        strcpy(s,out_expr.name);
        strcat(s,",");
    }
    generate("Write", s, "Integer", "");
}

void expression(expr_rec *result){
    expr_rec left_operand, right_operand;
    op_rec op;
    op_rec sum;
    sum.operator = PLUS;
    int first = 0;
    int total = 0;
    int literal = 1;
    int id = 0;
    primary(& left_operand);
    left_operand.sign = sum;
    if(left_operand.kind == LITERALEXPR){
        first = 0;
        total += left_operand.val;
    }
    else{
        first = 1;
        literal = 0;
    }
    while (next_token == PLUSOP || next_token == MINUSOP) {
        add_op(& op);
        primary(& right_operand);
        right_operand.sign = op;
        if(right_operand.kind == LITERALEXPR){
            if(op.operator == PLUS){
                total += right_operand.val;
            }
            else{
                total -= right_operand.val;
            }
        }
        else{
            if(first){
                left_operand = gen_infix(left_operand, op, right_operand);
                id ++;
            }
            else{
                first = 1;
                left_operand = right_operand;
                id ++;
            }
        }
    }
    if(literal){
        if(id != 0){
            if(total != 0){
                expr_rec t;
                t.kind = LITERALEXPR;
                t.val = total;
                t.sign = sum;
                left_operand = gen_infix(t, left_operand.sign,left_operand);
            }
        }
        else{
            left_operand.val = total;
        }
    }
    else{
        if(total != 0){
            expr_rec t;
            t.kind = LITERALEXPR;
            t.val = total;
            t.sign = sum;
            left_operand = gen_infix(t, left_operand.sign,left_operand);
        }
    }
    *result = left_operand;
}

void clear_buffer(){//borra el contenido de token_buffer y lo resetea a un string vacio
    memset(token_buffer, 0, sizeof(token_buffer));
}


void buffer_char(char c){//Recibe un char para agregarlo al buffer y conformar palabras paevisadasra ser r
    int len = strlen(token_buffer);
    token_buffer[len] = c;
    token_buffer[len+1] = '\0';
}


void lexical_error(char c){
    printf("%s", "Lexical Error in: ");
    printf("%c\n", c);
    exit(1);
}

void length_error(){
	printf("%s\n", "ID demasiado largo");
	exit(1);
}

token check_reserved(){
    //Todo en minuscula
    for(int j = 0; j <= strlen(token_buffer);j++){
        token_buffer[j] = toupper(token_buffer[j]);
    }
    // Compara
    for(int i = 0; i <= 4; i++){
        if(strcmp(token_buffer, reserved[i]) == 0){  ///Comparando un numero con un puntero
            return i ;
        }
    }
    return ID;
}

token scanner(void){
    int in_char, c;
    clear_buffer();
    if(feof(fp)){
        return SCANEOF;
    }
    while((in_char = fgetc(fp) ) != EOF){//lee todo el codigo hasta el final del archivo
        if(isspace(in_char)){
            continue; /* do nothin */
        }
        else if(isalpha(in_char)){
            /*
            * ID ::= LETTER | ID LETTER
                            | ID DIGIT
                            | ID UNDERSCORE
            */
            buffer_char(in_char);
            for(c = fgetc(fp) ; isalnum(c) || c == '_'; c = fgetc(fp) )
                buffer_char(c);
                ungetc(c, fp);
                return check_reserved();
        }
        else if(isdigit(in_char)){
            /*
             * INLITERAL ::= DIGIT |
             *               INLITERAL DIGIT
             */
            buffer_char(in_char);
            for(c = fgetc(fp); isdigit(c); c = fgetc(fp))
                buffer_char(c);
                ungetc(c, fp);
                return INTLITERAL;

        }
        else if(in_char == '('){
            return LPAREN;
        }
        else if(in_char == ')'){
            return RPAREN;
        }
        else if(in_char == ';'){
            return SEMICOLON;
        }
        else if(in_char == ','){
            return COMMA;
        }
        else if(in_char == '+'){
            return PLUSOP;
        }
        else if(in_char == '|'){
            return PIPE;
        }
        else if(in_char == ':'){
            /* lookig for ":=" */
            c = fgetc(fp);
            if(c == '='){
                return ASSIGNOP;
            }
            else{
                ungetc(c, fp);
                lexical_error(in_char);
            }
        }
        else if(in_char == '-'){
            /* is it --, comment start */
            c = fgetc(fp);
            if(c == '-'){
                while(in_char != '\n'){
                    in_char = fgetc(fp);
                }
            }
            else{
                ungetc(c, fp);
                return MINUSOP;
            }
        }
        else{
            lexical_error(in_char);
        }
    }
    if(feof(fp)){
        return SCANEOF;
    }
}

op_rec process_op(void){
 /*Procedure operator descriptor.*/
 op_rec o;
 if(next_token == PLUSOP)
     o.operator = PLUS;
 else
     o.operator = MINUS;
 return o;
}


void add_op(op_rec * o){
    token tok =  next_token;
    /* <addop> ::= PLUSOP | MINUSOP  */
    if (tok ==  PLUSOP || tok == MINUSOP){
        *o = process_op();
        match(tok);
    }
    else syntax_error(tok);
}

int cond = 1;
int code = 1;

void gen_skip(expr_rec e){
    if (e.kind == LITERALEXPR){
        char str[MAXIDLEN];
        sprintf(str, "%d", e.val);
        generate("Skip",str,"","");  
    }
    else{
        generate("Skip",e.name,"",""); 
    }
}

void gen_jmp(int n, int con){
    char str[MAXIDLEN];
    if(n == 1){
        sprintf(str, "%d", con);
        char str2[MAXIDLEN];
        sprintf(str2, "%s", "Jz condicional");
        strcat(str2,str);
        fprintf(fo, "%s\n", str2);
    }
    else if (n == 2){
        sprintf(str, "%d", con);
        char str2[MAXIDLEN];
        sprintf(str2, "%s", "Jmp code");
        strcat(str2,str);
        fprintf(fo, "%s\n", str2);
    }
    else if(n==3){
        sprintf(str, "%d", con);
        char str2[MAXIDLEN];
        sprintf(str2, "%s", "Eti condicional");
        strcat(str2,str);
        fprintf(fo, "%s\n", str2);
        
    }
    else{
        sprintf(str, "%d", con);
        char str2[MAXIDLEN];
        sprintf(str2, "%s", "Eti code");
        strcat(str2,str);
        fprintf(fo, "%s\n", str2);
    }
}

void primary(expr_rec *e){
    token tok =  next_token;
    switch(tok){
    case LPAREN:
        /*<primary> ::= (<expression>) */
        match(LPAREN);

        expr_rec e1;
        expression(&e1);
        
        if(next_token == PIPE){
            expr_rec temp;
            temp.kind = TEMPEXPR;
            strcpy(temp.name, get_temp());
            gen_skip(e1);
            cond++;
            int cond_act = cond;
            gen_jmp(1, cond);
            match(PIPE);
            expr_rec e2;
            expression(&e2);
            assign(temp, e2);
            code++;
            int cod_act = code;
            gen_jmp(2, code);
            gen_jmp(3, cond_act);
            match(PIPE);
            
            expr_rec e3;
            expression(&e3);
            assign(temp, e3);
            gen_jmp(2, cod_act);
            gen_jmp(4, cod_act);
            *e = temp;
        }
        else{
            *e = e1;
        }
        match(RPAREN);
        break;
    case ID:
        /* <primary> ::=  ID */
        *e = process_id(); 
        match(ID);
        break;
    case INTLITERAL:
        /* <primary> ::= INTLITERAL */
        *e = process_literal();
        match(INTLITERAL);
        break;
    default:
        syntax_error(tok);
        break;
    }
}

//PARSER

void syntax_error(token t){
    printf("%s", "Syntax Error in token: ");//Concatenar Token
    printf("%s\n", tokensError[t]);
    exit(1);
}

void match(token t){
    if(next_token == t){
        current_token = t;
        next_token = scanner();
    }
    else{
        syntax_error(t);
    }
}


void system_goal(void){
    /*<system goal> ::= <program> SCANEOF*/
    program();
    match(SCANEOF);
    finish();
}


void program(void){
    /* <program> ::= BEGIN <statement list> END */
    next_token = scanner();
    match(BEGIN);
    statement_list();
    match(END);
}

void statement_list(void){
    /*
     * <statement list>  ::= <statement> { <statement> }
     *
     */
     statement();
     while(true){
        switch  (next_token){
        case ID:
        case READ:
        case WRITE:
            statement();
            break;
        default:
            return;
        }
     }
}

void statement(void){
    expr_rec target;
    expr_rec source;
    token tok =  next_token;
    switch (tok) {
    case ID :
        /* <statement> ::= ID:= <expression>*/
        target = process_id();
        match(ID);
        match(ASSIGNOP);
        expression(& source);
        assign(target,source);
        match(SEMICOLON);
        break;

    case READ:
        /* <statement> ::= READ(<id list>) ; */
        match(READ);
        match(LPAREN);
        id_list();
        match(RPAREN);
        match(SEMICOLON);
        break;
    case WRITE:
        /* <statement> ::= WRITE (<expr list>) ; */
        match(WRITE);
        match(LPAREN);
        expr_list();
        match(RPAREN);
        match(SEMICOLON);
        break;
    default:
        syntax_error(tok);
        break;
    }
}

void id_list(void){
    /* <id_list> ::=ID{, ID}  */
    if(next_token == ID){
        expr_rec source = process_id();
        match(ID);
        read_id(source);
    }
    else{
        syntax_error(next_token);
        return;
    }
    while (next_token== COMMA){
        match(COMMA);
        if(next_token == ID){
            expr_rec source2 = process_id();
            match(ID);
            read_id(source2);
        }
        else{
            syntax_error(next_token);
            return; 
        }
    }
}

void expr_list(void){
    /* <expr_list> ::=  <expression> { , <expression>} */
    expr_rec source;
    expression(&source);
    write_expr(source);
    while (next_token == COMMA){
        match(COMMA);
        expression(&source);
        write_expr(source);
    }
}    