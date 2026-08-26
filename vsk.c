/* VSK 0.2 bootstrap runtime.
 * The language grammar and runtime live here; Python is not involved.
 * C is only the first bootstrap host. The VSK language remains independent.
 */
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VSK_VERSION "0.3.0"
#define MAX_PARAMS 32
#define MAX_LOOP 100000

typedef struct Allocation { void *ptr; struct Allocation *next; } Allocation;
static Allocation *allocations = NULL;

static void allocation_add(void *ptr) {
    if (!ptr) { fprintf(stderr, "VSK runtime error: memory allocation failed\n"); exit(2); }
    Allocation *node = (Allocation *)malloc(sizeof(Allocation));
    if (!node) { fprintf(stderr, "VSK runtime error: allocation registry failed\n"); exit(2); }
    node->ptr = ptr; node->next = allocations; allocations = node;
}
static void *vsk_malloc(size_t size) { void *ptr = malloc(size ? size : 1); allocation_add(ptr); return ptr; }
static void *vsk_calloc(size_t count, size_t size) { void *ptr = calloc(count ? count : 1, size ? size : 1); allocation_add(ptr); return ptr; }
static void *vsk_realloc(void *old, size_t size) {
    if (!old) return vsk_malloc(size);
    void *ptr = realloc(old, size ? size : 1);
    if (!ptr) { fprintf(stderr, "VSK runtime error: memory reallocation failed\n"); exit(2); }
    for (Allocation *node = allocations; node; node = node->next) if (node->ptr == old) { node->ptr = ptr; return ptr; }
    allocation_add(ptr); return ptr;
}
static void vsk_free(void *ptr) {
    if (!ptr) return;
    Allocation **link = &allocations;
    while (*link && (*link)->ptr != ptr) link = &(*link)->next;
    if (*link) { Allocation *node = *link; *link = node->next; free(node->ptr); free(node); }
}
static void vsk_cleanup(void) {
    while (allocations) { Allocation *node = allocations; allocations = node->next; free(node->ptr); free(node); }
}

typedef enum {
    T_EOF, T_IDENT, T_NUMBER, T_STRING,
    T_SET, T_SAY, T_BRING, T_WHEN, T_OTHERWISE, T_REPEAT,
    T_PROC, T_GIVE, T_TRUE, T_FALSE, T_NIL,
    T_AND, T_OR, T_NOT,
    T_OP, T_LP, T_RP, T_LB, T_RB, T_COMMA, T_SEMI, T_DOT, T_COLON
} TokenKind;

typedef struct { TokenKind kind; char *text; double number; int line; } Token;

typedef struct { Token *items; int count; int cap; } TokenList;

static char *copy_text(const char *s, size_t n) {
    char *out = (char *)vsk_malloc(n + 1);
    if (!out) { fprintf(stderr, "VSK: out of memory\n"); exit(2); }
    memcpy(out, s, n); out[n] = '\0'; return out;
}

static int same_ci(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        a++; b++;
    }
    return *a == '\0' && *b == '\0';
}

static void token_add(TokenList *list, TokenKind kind, const char *text, size_t n, double number, int line) {
    if (list->count == list->cap) {
        list->cap = list->cap ? list->cap * 2 : 128;
        list->items = (Token *)vsk_realloc(list->items, sizeof(Token) * list->cap);
    }
    list->items[list->count++] = (Token){kind, copy_text(text, n), number, line};
}

static TokenKind keyword_kind(const char *s) {
    if (same_ci(s, "set")) return T_SET;
    if (same_ci(s, "say")) return T_SAY;
    if (same_ci(s, "bring")) return T_BRING;
    if (same_ci(s, "when")) return T_WHEN;
    if (same_ci(s, "otherwise")) return T_OTHERWISE;
    if (same_ci(s, "repeat")) return T_REPEAT;
    if (same_ci(s, "proc")) return T_PROC;
    if (same_ci(s, "give")) return T_GIVE;
    if (same_ci(s, "true")) return T_TRUE;
    if (same_ci(s, "false")) return T_FALSE;
    if (same_ci(s, "nil")) return T_NIL;
    if (same_ci(s, "and")) return T_AND;
    if (same_ci(s, "or")) return T_OR;
    if (same_ci(s, "not")) return T_NOT;
    return T_IDENT;
}

static TokenList lex(const char *src) {
    TokenList out = {0}; int i = 0; int line = 1;
    while (src[i]) {
        unsigned char c = (unsigned char)src[i];
        if (c == '\n') { line++; i++; continue; }
        if (isspace(c)) { i++; continue; }
        if (c == '#') { while (src[i] && src[i] != '\n') i++; continue; }
        if (c == '/' && src[i+1] == '/') { while (src[i] && src[i] != '\n') i++; continue; }
        if (isalpha(c) || c == '_') {
            int start = i; while (isalnum((unsigned char)src[i]) || src[i] == '_') i++;
            char *word = copy_text(src + start, (size_t)(i - start));
            token_add(&out, keyword_kind(word), word, strlen(word), 0, line); vsk_free(word); continue;
        }
        if (isdigit(c) || (c == '.' && isdigit((unsigned char)src[i+1]))) {
            char *end = NULL; double n = strtod(src + i, &end); int used = (int)(end - src - i);
            token_add(&out, T_NUMBER, src + i, (size_t)used, n, line); i += used; continue;
        }
        if (c == '"' || c == '\'') {
            char quote = (char)c; int start_line = line; i++; char buf[4096]; int n = 0;
            while (src[i] && src[i] != quote) {
                if (src[i] == '\\' && src[i+1]) {
                    i++; char e = src[i++]; buf[n++] = e == 'n' ? '\n' : (e == 't' ? '\t' : e);
                } else { if (src[i] == '\n') line++; buf[n++] = src[i++]; }
                if (n > 4095) { fprintf(stderr, "VSK syntax error at line %d: string too long\n", start_line); exit(1); }
            }
            if (!src[i]) { fprintf(stderr, "VSK syntax error at line %d: unclosed string\n", start_line); exit(1); }
            i++; token_add(&out, T_STRING, buf, (size_t)n, 0, start_line); continue;
        }
        if (src[i+1]) {
            char pair[3] = {src[i], src[i+1], '\0'};
            if (!strcmp(pair, "==") || !strcmp(pair, "!=") || !strcmp(pair, "<=") || !strcmp(pair, ">=") ||
                !strcmp(pair, "+=") || !strcmp(pair, "-=") || !strcmp(pair, "*=") || !strcmp(pair, "/=") ||
                !strcmp(pair, "%=")) {
                token_add(&out, T_OP, pair, 2, 0, line); i += 2; continue;
            }
        }
        TokenKind kind = T_OP; char one[2] = {(char)c, '\0'};
        switch (c) {
            case '(': kind = T_LP; break; case ')': kind = T_RP; break;
            case '{': kind = T_LB; break; case '}': kind = T_RB; break;
            case ',': kind = T_COMMA; break; case ';': kind = T_SEMI; break;
            case '.': kind = T_DOT; break; case ':': kind = T_COLON; break;
            case '+': case '-': case '*': case '/': case '%': case '<': case '>': case '=': case '!': break;
            default: fprintf(stderr, "VSK syntax error at line %d: unknown character '%c'\n", line, c); exit(1);
        }
        token_add(&out, kind, one, 1, 0, line); i++;
    }
    token_add(&out, T_EOF, "", 0, 0, line); return out;
}

typedef enum { E_NIL, E_NUM, E_STR, E_BOOL, E_NAME, E_LIST, E_UNARY, E_BINARY, E_CALL, E_MEMBER } ExprKind;
typedef struct Expr Expr;
struct Expr {
    ExprKind kind; char *text; double number; int boolean;
    Expr *a, *b; Expr **args; int argc;
};

typedef enum { S_SET, S_SAY, S_BRING, S_EXPR, S_WHEN, S_REPEAT, S_PROC, S_GIVE } StmtKind;
typedef struct Stmt Stmt;
struct Stmt {
    StmtKind kind; char *name; char *module; char op[3];
    Expr *expr; Stmt *body; Stmt *otherwise; Stmt *next;
    char *params[MAX_PARAMS]; int nparams;
};

typedef struct { Token *tokens; int pos; } Parser;

static Token *cur(Parser *p) { return &p->tokens[p->pos]; }
static int is(Parser *p, TokenKind k) { return cur(p)->kind == k; }
static Token *take(Parser *p, TokenKind k) {
    if (!is(p, k)) { fprintf(stderr, "VSK syntax error at line %d: unexpected '%s'\n", cur(p)->line, cur(p)->text); exit(1); }
    return &p->tokens[p->pos++];
}
static void optional(Parser *p, TokenKind k) { if (is(p, k)) p->pos++; }
static char *take_name(Parser *p) {
    if (!is(p, T_IDENT)) { fprintf(stderr, "VSK syntax error at line %d: name expected\n", cur(p)->line); exit(1); }
    char *out = copy_text(cur(p)->text, strlen(cur(p)->text));
    p->pos++;
    return out;
}
static Expr *new_expr(ExprKind k) { Expr *e = (Expr *)vsk_calloc(1, sizeof(Expr)); e->kind = k; return e; }
static Stmt *new_stmt(StmtKind k) { Stmt *s = (Stmt *)vsk_calloc(1, sizeof(Stmt)); s->kind = k; return s; }
static Expr *name_expr(char *name) { Expr *e = new_expr(E_NAME); e->text = name; return e; }

static int precedence(Token *t) {
    if (t->kind == T_OR) return 1; if (t->kind == T_AND) return 2;
    if (t->kind != T_OP) return -1;
    if (!strcmp(t->text, "==") || !strcmp(t->text, "!=")) return 3;
    if (!strcmp(t->text, "<") || !strcmp(t->text, "<=") || !strcmp(t->text, ">") || !strcmp(t->text, ">=")) return 4;
    if (!strcmp(t->text, "+") || !strcmp(t->text, "-")) return 5;
    if (!strcmp(t->text, "*") || !strcmp(t->text, "/") || !strcmp(t->text, "%")) return 6;
    return -1;
}

static Expr *parse_expr(Parser *p, int min_prec);

static Expr *parse_postfix(Parser *p, Expr *e) {
    for (;;) {
        if (is(p, T_DOT)) {
            p->pos++; char *member = take_name(p); Expr *n = new_expr(E_MEMBER); n->a = e; n->text = member; e = n;
        } else if (is(p, T_LP)) {
            p->pos++; Expr *n = new_expr(E_CALL); n->a = e;
            if (!is(p, T_RP)) {
                while (1) {
                    n->args = (Expr **)vsk_realloc(n->args, sizeof(Expr *) * (n->argc + 1)); n->args[n->argc++] = parse_expr(p, 0);
                    if (!is(p, T_COMMA)) break; p->pos++;
                }
            }
            take(p, T_RP); e = n;
        } else break;
    }
    return e;
}

static Expr *parse_prefix(Parser *p) {
    Token *t = cur(p); Expr *e;
    if (is(p, T_NUMBER)) { e = new_expr(E_NUM); e->number = t->number; p->pos++; return parse_postfix(p, e); }
    if (is(p, T_STRING)) { e = new_expr(E_STR); e->text = copy_text(t->text, strlen(t->text)); p->pos++; return parse_postfix(p, e); }
    if (is(p, T_TRUE) || is(p, T_FALSE)) { e = new_expr(E_BOOL); e->boolean = is(p, T_TRUE); p->pos++; return parse_postfix(p, e); }
    if (is(p, T_NIL)) { e = new_expr(E_NIL); p->pos++; return parse_postfix(p, e); }
    if (is(p, T_IDENT)) { e = name_expr(copy_text(t->text, strlen(t->text))); p->pos++; return parse_postfix(p, e); }
    if (is(p, T_LP)) { p->pos++; e = parse_expr(p, 0); take(p, T_RP); return parse_postfix(p, e); }
    if (is(p, T_OP) && (!strcmp(t->text, "-") || !strcmp(t->text, "+") || !strcmp(t->text, "!"))) {
        e = new_expr(E_UNARY); e->text = copy_text(t->text, strlen(t->text)); p->pos++; e->a = parse_prefix(p); return e;
    }
    if (is(p, T_NOT)) { e = new_expr(E_UNARY); e->text = copy_text("not", 3); p->pos++; e->a = parse_prefix(p); return e; }
    fprintf(stderr, "VSK syntax error at line %d: expression expected near '%s'\n", t->line, t->text); exit(1);
    return NULL;
}

static Expr *parse_expr(Parser *p, int min_prec) {
    Expr *left = parse_prefix(p);
    while (1) {
        Token *t = cur(p); int prec = precedence(t); if (prec < min_prec) break;
        Expr *n = new_expr(E_BINARY); n->text = copy_text(t->kind == T_AND ? "and" : (t->kind == T_OR ? "or" : t->text), strlen(t->kind == T_AND ? "and" : (t->kind == T_OR ? "or" : t->text))); p->pos++;
        n->a = left; n->b = parse_expr(p, prec + 1); left = n;
    }
    return left;
}

static Stmt *parse_statement(Parser *p);

static Stmt *parse_block(Parser *p) {
    optional(p, T_COLON);
    if (is(p, T_LB)) {
        p->pos++; Stmt *head = NULL, *tail = NULL;
        while (!is(p, T_RB) && !is(p, T_EOF)) {
            optional(p, T_SEMI); if (is(p, T_RB)) break;
            Stmt *s = parse_statement(p); if (!head) head = s; else tail->next = s; tail = s;
            optional(p, T_SEMI);
        }
        take(p, T_RB); return head;
    }
    return parse_statement(p);
}

static Stmt *parse_statement(Parser *p) {
    Stmt *s;
    if (is(p, T_SET)) {
        p->pos++; s = new_stmt(S_SET); s->name = take_name(p);
        if (!is(p, T_OP)) { fprintf(stderr, "VSK syntax error at line %d: assignment operator expected\n", cur(p)->line); exit(1); }
        strncpy(s->op, cur(p)->text, 2); s->op[2] = '\0'; p->pos++; s->expr = parse_expr(p, 0); return s;
    }
    if (is(p, T_SAY)) { p->pos++; s = new_stmt(S_SAY); s->expr = parse_expr(p, 0); return s; }
    if (is(p, T_BRING)) { p->pos++; s = new_stmt(S_BRING); s->module = take_name(p); return s; }
    if (is(p, T_IDENT) && p->tokens[p->pos + 1].kind == T_OP) {
        Token *op = &p->tokens[p->pos + 1];
        if (!strcmp(op->text, "=") || !strcmp(op->text, "+=") || !strcmp(op->text, "-=") || !strcmp(op->text, "*=") || !strcmp(op->text, "/=") || !strcmp(op->text, "%=")) {
            s = new_stmt(S_SET); s->name = take_name(p); strncpy(s->op, cur(p)->text, 2); s->op[2] = '\0'; p->pos++; s->expr = parse_expr(p, 0); return s;
        }
    }
    if (is(p, T_GIVE)) { p->pos++; s = new_stmt(S_GIVE); s->expr = is(p, T_RB) || is(p, T_EOF) ? new_expr(E_NIL) : parse_expr(p, 0); return s; }
    if (is(p, T_WHEN) || is(p, T_REPEAT)) {
        int repeat = is(p, T_REPEAT); p->pos++; s = new_stmt(repeat ? S_REPEAT : S_WHEN); s->expr = parse_expr(p, 0); s->body = parse_block(p);
        if (!repeat && is(p, T_OTHERWISE)) { p->pos++; s->otherwise = parse_block(p); } return s;
    }
    if (is(p, T_PROC)) {
        p->pos++; s = new_stmt(S_PROC); s->name = take_name(p); take(p, T_LP);
        while (!is(p, T_RP)) { if (s->nparams >= MAX_PARAMS) { fprintf(stderr, "VSK: too many parameters\n"); exit(1); } s->params[s->nparams++] = take_name(p); if (!is(p, T_COMMA)) break; p->pos++; }
        take(p, T_RP); s->body = parse_block(p); return s;
    }
    if (is(p, T_IDENT) && (p->tokens[p->pos + 1].kind == T_LP || p->tokens[p->pos + 1].kind == T_DOT)) {
        s = new_stmt(S_EXPR); s->expr = parse_expr(p, 0); return s;
    }
    fprintf(stderr, "VSK syntax error at line %d: statement expected near '%s'\n", cur(p)->line, cur(p)->text);
    exit(1);
}

static Stmt *parse_program(Parser *p) {
    Stmt *head = NULL, *tail = NULL;
    while (!is(p, T_EOF)) { optional(p, T_SEMI); if (is(p, T_EOF)) break; Stmt *s = parse_statement(p); if (!head) head = s; else tail->next = s; tail = s; optional(p, T_SEMI); }
    return head;
}

typedef enum { V_NIL, V_NUM, V_STR, V_BOOL, V_MODULE, V_BUILTIN, V_PROC } ValueKind;
typedef struct Env Env; typedef struct Proc Proc;
typedef struct { ValueKind kind; double number; int boolean; char *text; char *builtin; Proc *proc; } Value;
typedef struct Binding { char *name; Value value; struct Binding *next; } Binding;
struct Env { Binding *items; Env *parent; };
struct Proc { char *params[MAX_PARAMS]; int nparams; Stmt *body; Env *closure; };

static Value vnil(void) { return (Value){V_NIL, 0, 0, NULL, NULL, NULL}; }
static Value vnum(double n) { return (Value){V_NUM, n, 0, NULL, NULL, NULL}; }
static Value vbool(int b) { return (Value){V_BOOL, 0, b, NULL, NULL, NULL}; }
static Value vstr(const char *s) { Value v = {V_STR, 0, 0, copy_text(s, strlen(s)), NULL, NULL}; return v; }
static Value vmodule(const char *s) { Value v = {V_MODULE, 0, 0, copy_text(s, strlen(s)), NULL, NULL}; return v; }
static Value vbuiltin(const char *s) { Value v = {V_BUILTIN, 0, 0, NULL, copy_text(s, strlen(s)), NULL}; return v; }
static Env *env_new(Env *parent) { Env *e = (Env *)vsk_calloc(1, sizeof(Env)); e->parent = parent; return e; }
static Binding *binding(Env *e, const char *name) { for (Binding *b = e->items; b; b = b->next) if (same_ci(b->name, name)) return b; return NULL; }
static void env_define(Env *e, const char *name, Value v) { Binding *b = binding(e, name); if (!b) { b = (Binding *)vsk_calloc(1, sizeof(Binding)); b->name = copy_text(name, strlen(name)); b->next = e->items; e->items = b; } b->value = v; }
static int env_has(Env *e, const char *name) { return binding(e, name) || (e->parent && env_has(e->parent, name)); }
static void env_set(Env *e, const char *name, Value v) { Binding *b = binding(e, name); if (b) { b->value = v; return; } if (e->parent && env_has(e->parent, name)) { env_set(e->parent, name, v); return; } env_define(e, name, v); }

static void warn_msg(const char *msg) { fprintf(stderr, "[VSK notice] %s\n", msg); }
static Value lookup(Env *e, const char *name) {
    for (Env *cur = e; cur; cur = cur->parent) { Binding *b = binding(cur, name); if (b) return b->value; }
    char msg[512]; snprintf(msg, sizeof(msg), "name '%s' is not defined; continuing with nil", name); warn_msg(msg); return vnil();
}
static int truthy(Value v) { return v.kind == V_BOOL ? v.boolean : (v.kind == V_NIL ? 0 : (v.kind == V_NUM ? v.number != 0 : 1)); }
static void print_value(Value v) {
    if (v.kind == V_NIL) printf("nil\n"); else if (v.kind == V_NUM) printf("%g\n", v.number); else if (v.kind == V_BOOL) printf("%s\n", v.boolean ? "true" : "false"); else if (v.kind == V_STR) printf("%s\n", v.text); else if (v.kind == V_MODULE) printf("<module %s>\n", v.text); else printf("<value>\n");
}
static char *value_text(Value v) {
    char buf[64]; if (v.kind == V_STR) return copy_text(v.text, strlen(v.text));
    if (v.kind == V_NIL) return copy_text("nil", 3); if (v.kind == V_BOOL) return copy_text(v.boolean ? "true" : "false", v.boolean ? 4 : 5);
    snprintf(buf, sizeof(buf), "%g", v.number); return copy_text(buf, strlen(buf));
}

static Value eval_expr(Expr *e, Env *env);
static void exec_list(Stmt *s, Env *env, Value *ret, int *returned);

static Value member_value(Value base, const char *member) {
    if (base.kind != V_MODULE) { warn_msg("member access on a non-module value; continuing with nil"); return vnil(); }
    if (same_ci(base.text, "math")) {
        if (same_ci(member, "sqrt") || same_ci(member, "abs") || same_ci(member, "floor") || same_ci(member, "ceil")) return vbuiltin(member);
        if (same_ci(member, "pi")) return vnum(acos(-1.0));
    }
    if (same_ci(base.text, "text")) { if (same_ci(member, "upper") || same_ci(member, "lower")) return vbuiltin(member); }
    if (same_ci(base.text, "random")) { if (same_ci(member, "number")) return vbuiltin(member); }
    char msg[256]; snprintf(msg, sizeof(msg), "member '%s' is not available; continuing with nil", member); warn_msg(msg); return vnil();
}

static Value call_value(Value fn, Value *args, int argc) {
    if (fn.kind == V_BUILTIN) {
        if (same_ci(fn.builtin, "sqrt") && argc >= 1) return vnum(sqrt(args[0].number));
        if (same_ci(fn.builtin, "abs") && argc >= 1) return vnum(fabs(args[0].number));
        if (same_ci(fn.builtin, "floor") && argc >= 1) return vnum(floor(args[0].number));
        if (same_ci(fn.builtin, "ceil") && argc >= 1) return vnum(ceil(args[0].number));
        if (same_ci(fn.builtin, "upper") && argc >= 1) { char *s = value_text(args[0]); for (int i=0;s[i];i++) s[i]=(char)toupper((unsigned char)s[i]); Value v=vstr(s); vsk_free(s); return v; }
        if (same_ci(fn.builtin, "lower") && argc >= 1) { char *s = value_text(args[0]); for (int i=0;s[i];i++) s[i]=(char)tolower((unsigned char)s[i]); Value v=vstr(s); vsk_free(s); return v; }
        if (same_ci(fn.builtin, "number")) return vnum((double)rand() / (double)RAND_MAX);
        warn_msg("invalid builtin call; continuing with nil"); return vnil();
    }
    if (fn.kind == V_PROC) {
        Env *call_env = env_new(fn.proc->closure); for (int i=0;i<fn.proc->nparams;i++) env_define(call_env, fn.proc->params[i], i<argc ? args[i] : vnil());
        Value ret = vnil(); int returned = 0; exec_list(fn.proc->body, call_env, &ret, &returned); return ret;
    }
    warn_msg("called value is not callable; continuing with nil"); return vnil();
}

static Value binary_value(const char *op, Value a, Value b) {
    if (!strcmp(op, "+")) {
        if (a.kind == V_STR || b.kind == V_STR) { char *x=value_text(a), *y=value_text(b); char *z=(char *)vsk_malloc(strlen(x)+strlen(y)+1); strcpy(z,x); strcat(z,y); Value v=vstr(z); vsk_free(x); vsk_free(y); vsk_free(z); return v; }
        return vnum(a.number + b.number);
    }
    if (!strcmp(op, "-")) return vnum(a.number - b.number); if (!strcmp(op, "*")) return vnum(a.number * b.number); if (!strcmp(op, "/")) return b.number == 0 ? (warn_msg("division by zero; continuing with nil"), vnil()) : vnum(a.number / b.number);
    if (!strcmp(op, "%")) return vnum(fmod(a.number, b.number));
    if (!strcmp(op, "==")) { if (a.kind==V_STR || b.kind==V_STR) { char *x=value_text(a),*y=value_text(b); int r=!strcmp(x,y); vsk_free(x);vsk_free(y);return vbool(r); } return vbool(a.kind==b.kind && a.number==b.number && a.boolean==b.boolean); }
    if (!strcmp(op, "!=")) return vbool(!truthy(binary_value("==",a,b)));
    if (!strcmp(op, "<")) return vbool(a.number < b.number); if (!strcmp(op, "<=") ) return vbool(a.number <= b.number); if (!strcmp(op, ">")) return vbool(a.number > b.number); if (!strcmp(op, ">=")) return vbool(a.number >= b.number);
    if (!strcmp(op, "and")) return vbool(truthy(a) && truthy(b)); if (!strcmp(op, "or")) return vbool(truthy(a) || truthy(b)); return vnil();
}

static Value eval_expr(Expr *e, Env *env) {
    if (!e) return vnil();
    if (e->kind == E_NIL) return vnil(); if (e->kind == E_NUM) return vnum(e->number); if (e->kind == E_STR) return vstr(e->text); if (e->kind == E_BOOL) return vbool(e->boolean);
    if (e->kind == E_NAME) return lookup(env, e->text); if (e->kind == E_MEMBER) return member_value(eval_expr(e->a, env), e->text);
    if (e->kind == E_UNARY) { Value v=eval_expr(e->a,env); if (!strcmp(e->text,"-")) return vnum(-v.number); if (!strcmp(e->text,"+")) return vnum(v.number); return vbool(!truthy(v)); }
    if (e->kind == E_BINARY) { Value a=eval_expr(e->a,env); if (!strcmp(e->text,"and") && !truthy(a)) return vbool(0); if (!strcmp(e->text,"or") && truthy(a)) return vbool(1); return binary_value(e->text,a,eval_expr(e->b,env)); }
    if (e->kind == E_CALL) { Value fn=eval_expr(e->a,env); Value *args=(Value *)vsk_calloc(e->argc,sizeof(Value)); for(int i=0;i<e->argc;i++)args[i]=eval_expr(e->args[i],env); Value out=call_value(fn,args,e->argc); vsk_free(args); return out; }
    return vnil();
}

static void exec_stmt(Stmt *s, Env *env, Value *ret, int *returned) {
    if (*returned || !s) return;
    if (s->kind == S_SET) { Value v=eval_expr(s->expr,env); if (!strcmp(s->op,"=")) env_set(env,s->name,v); else { Value old=lookup(env,s->name); char base[2]={s->op[0],'\0'}; env_set(env,s->name,binary_value(base,old,v)); } }
    else if (s->kind == S_SAY) print_value(eval_expr(s->expr,env));
    else if (s->kind == S_EXPR) eval_expr(s->expr,env);
    else if (s->kind == S_BRING) { int known=same_ci(s->module,"math")||same_ci(s->module,"text")||same_ci(s->module,"random"); env_define(env,s->module,vmodule(s->module)); if (!known) { char msg[256]; snprintf(msg,sizeof(msg),"script requested unavailable library '%s'; ignored and continuing",s->module); warn_msg(msg); } }
    else if (s->kind == S_GIVE) { *ret=eval_expr(s->expr,env); *returned=1; }
    else if (s->kind == S_WHEN) { if(truthy(eval_expr(s->expr,env))) exec_list(s->body,env,ret,returned); else exec_list(s->otherwise,env,ret,returned); }
    else if (s->kind == S_REPEAT) {
        int n = 0; int capped = 0;
        while (truthy(eval_expr(s->expr, env))) {
            if (n >= MAX_LOOP) { capped = 1; break; }
            exec_list(s->body, env, ret, returned);
            if (*returned) break;
            n++;
        }
        if (capped) warn_msg("repeat stopped after 100000 iterations");
    }
    else if (s->kind == S_PROC) { Proc *proc=(Proc *)vsk_calloc(1,sizeof(Proc)); proc->nparams=s->nparams; proc->body=s->body; proc->closure=env; for(int i=0;i<s->nparams;i++)proc->params[i]=s->params[i]; Value v={V_PROC,0,0,NULL,NULL,proc}; env_define(env,s->name,v); }
}
static void exec_list(Stmt *s, Env *env, Value *ret, int *returned) { for(;s && !*returned;s=s->next) exec_stmt(s,env,ret,returned); }

static char *read_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) { perror(path); exit(1); }
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); fprintf(stderr, "VSK: cannot seek input file\n"); exit(1); }
    long n = ftell(f);
    if (n < 0 || fseek(f, 0, SEEK_SET) != 0) { fclose(f); fprintf(stderr, "VSK: cannot measure input file\n"); exit(1); }
    char *s = (char *)vsk_malloc((size_t)n + 1);
    size_t got = fread(s, 1, (size_t)n, f);
    if (got != (size_t)n) { vsk_free(s); fclose(f); fprintf(stderr, "VSK: could not read complete input file\n"); exit(1); }
    s[n] = '\0'; fclose(f); return s;
}

int main(int argc, char **argv) {
    atexit(vsk_cleanup);
    if (argc == 2 && (!strcmp(argv[1], "--version") || !strcmp(argv[1], "-v"))) {
        printf("VSK %s\n", VSK_VERSION);
        return 0;
    }
    if (argc == 2 && (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h"))) {
        printf("VSK %s\nusage: vsk program.vk\n       vsk --version\n       vsk --help\n", VSK_VERSION);
        return 0;
    }
    if (argc != 2) { fprintf(stderr, "VSK %s\nusage: %s program.vk\n", VSK_VERSION, argv[0]); return 2; }
    char *source=read_file(argv[1]); TokenList tokens=lex(source); Parser parser={tokens.items,0}; Stmt *program=parse_program(&parser); Env *global=env_new(NULL); Value ret=vnil(); int returned=0; exec_list(program,global,&ret,&returned); vsk_free(source); return 0;
}
