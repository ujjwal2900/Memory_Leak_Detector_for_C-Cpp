#ifndef __MLD__
#include <assert.h>

#define ANSI_COLOR_MAGENTA      "\x1b[35m"
#define ANSI_COLOR_YELLOW       "\x1b[33m"
#define ANSI_COLOR_CYAN         "\x1b[36m"

/* Structure Data base Definition Begin */

#define MAX_STRUCTURE_NAME_SIZE 128
#define MAX_FIELD_NAME_SIZE 128

/* Enumeration for data types */
typedef enum{
    UINT8,
    UINT32,
    INT32,
    CHAR,
    OBJ_PTR,
    FLOAT,
    DOUBLE,
    OBJ_STRUCT
}data_type_t;

typedef enum{
    MLD_FALSE,
    MLD_TRUE
} mld_boolean_t;

#define OFFSETOF(struct_name, field_name)   \
        (size_t)&(((struct_name *)0)->field_name)

#define FIELD_SIZE(struct_name, field_name)  \
        sizeof(((struct_name *)0)->field_name)

//typedef struct _struct_db_rec_t struct_db_rec_t;

/* Structure to store the information of one field of a C structure */
typedef struct _field_info_{
    char fname[MAX_FIELD_NAME_SIZE];       /*Name of the field*/
    data_type_t dtype;                      /*Data type of the field*/
    unsigned int size;                      /*Size of the field*/
    unsigned int offset;                    /*Offset of the field*/
    char nested_str_name[MAX_STRUCTURE_NAME_SIZE];  /* Only if data type = OBJ_PTR, or OBJ STRUCT*/
} field_info_t;

/*Structure to store the information of one C structure which could have 'n_fields' fields */
typedef struct _struct_db_rec_t{
    struct _struct_db_rec_t *next;  /* Pointer to the next structure in the linked list */
    char struct_name[MAX_FIELD_NAME_SIZE];  /* Key */
    unsigned int ds_size;   /* Size of the structure */
    unsigned int n_fields;  /* No of fields in the structure */
    field_info_t *fields;   /* Pointer to the array of fields*/
} struct_db_rec_t;

/* Head of the linked list representing the structure data base */
typedef struct _struct_db_{
    struct_db_rec_t *head;
    unsigned int count;
} struct_db_t;

/* Printing Functions */
void print_structure_rec(struct_db_rec_t *struct_rec);

void print_structure_db(struct_db_t *struct_db);

/* Function to add the structure record in a structure database */
/* Return 0 on success & -1 on Failure                          */
int add_structure_to_struct_db(struct_db_t *struct_db, struct_db_rec_t *struct_rec);

/* Structure registration helping APIs */

#define FIELD_INFO(struct_name, field_name, dtype, nested_struct_name)          \
    {#field_name, dtype, FIELD_SIZE(struct_name, field_name),                          \
     OFFSETOF(struct_name, field_name), #nested_struct_name}

#define REG_STRUCT(struct_db, st_name, fields_arr)                      \
    do{                                                                 \
        struct_db_rec_t *rec = calloc(1, sizeof(struct_db_rec_t));      \
        strncpy(rec->struct_name, #st_name, MAX_STRUCTURE_NAME_SIZE);   \
        rec->ds_size = sizeof(st_name);                                 \
        rec->n_fields = sizeof(fields_arr)/sizeof(field_info_t);        \
        rec->fields = fields_arr;                                       \
        if(add_structure_to_struct_db(struct_db, rec)){                 \
            assert(0);                                                  \
        }                                                               \
    }while(0);

/* To search for the structure record with the given name */
/* Return a pointer to the corresponding record           */
struct_db_rec_t *struct_db_look_up(struct_db_t *struct_db, char *struct_name);

/* Object Database */
typedef struct _object_db_rec_{

    struct _object_db_rec_ *next;
    void *ptr;              /* Key */
    unsigned int units;
    struct_db_rec_t *struct_rec;
    mld_boolean_t is_visited;   /* Used for Graph Traversal */
    mld_boolean_t is_root;      /* Is this object a root object */
}object_db_rec_t;

typedef struct _object_db_{

    struct_db_t *struct_db;
    object_db_rec_t *head;
    unsigned int count;
}object_db_t;

/*Dumping Functions*/
void print_object_rec(object_db_rec_t *obj_rec, int i);

void print_object_db(object_db_t *object_db);

/* Print all supported fields by mld library for the object record passed as argument */
void mld_dump_object_rec_detail(object_db_rec_t *obj_rec);

/* API to malloc the object*/
void *xcalloc(object_db_t *object_db, char *struct_name, int units);

/* API to free the object */
void xfree(object_db_t *object_db, void *ptr);

/* APIs to Register root objects */
void mld_register_root_object (object_db_t *object_db,
                               void *objptr,
                               char *struct_name,
                               unsigned int units);

void set_mld_object_as_global_root(object_db_t *object_db, void *obj_ptr);

void mld_set_dynamic_object_as_root(object_db_t *object_db, void *obj_ptr);

/* Add support for primitive data types */
void mld_init_primitive_data_types_support(struct_db_t *struct_db);

void run_mld_algorithm(object_db_t *object_db);

void report_leaked_objects(object_db_t *object_db);

#endif /* __MLD__ */

