#define Q_ADD 100
#define Q_SUB 101
#define Q_MUL 102
#define Q_DIV 103
#define Q_GOTO 104

#define QO_BOOL 200
#define QO_INT 201


typedef enum quadop_type{
    QO_BOOL, QO_INT,
  }quadop_type;

typedef enum quad_type{
    Q_ADD, Q_SUB, Q_MUL, Q_DIV, Q_GOTO,
  }quad_type;

typedef struct quadop {
  quadop_type type;
  union {
    int cst;
    char* name;
  } u;
} quadop;

typedef struct quad{
  int addr;
  quad_type type;
  quadop op1, op2, op3;
  int label;
} quad, quad_list*;


quad global_code[5000]; // code généré
size_t nextquad; // n° du prochain quad
size_t tmpCount; // n° de la prochaine variable temporaire dans la table des symboles
quad get_quad(quad_type type, quadop op1, quadop op2, quadop op3, int label); // retourne un quad avec les attributs spécifiés dans les paramètres
void gencode(quad q); // écrie q dans global_code[nextquad] et incrémente nextquad
quadop* new_temp();
