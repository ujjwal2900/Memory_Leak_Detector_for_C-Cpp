#include "mld.h"
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

/* Application Structures */
typedef struct emp_{

    char emp_name[30];
    unsigned int emp_id;
    unsigned int age;
    struct emp_ *mgr;
    float salary;
} emp_t;

typedef struct student_{
    
    char stud_name[30];
    unsigned int roll_no;
    unsigned int age;
    float aggregate;
    struct student_ *best_college;
} student_t;

int main(int argc, char**argv)
{
    /* Step 1: Initialize a new structure database */
    struct_db_t *struct_db = calloc(1, sizeof(struct_db_t));

    /* Create structure record for structure emp_t */
    static field_info_t emp_fields[] = {
        FIELD_INFO(emp_t, emp_name, CHAR,          0),
        FIELD_INFO(emp_t, emp_id,   UINT32,        0),
        FIELD_INFO(emp_t, age,      UINT32,        0),
        FIELD_INFO(emp_t, mgr,      OBJ_PTR,   emp_t),
        FIELD_INFO(emp_t, salary,   FLOAT,         0)
    };
    REG_STRUCT(struct_db, emp_t, emp_fields);

    static field_info_t stud_fields[] = {
        FIELD_INFO(student_t, stud_name,     CHAR,            0),
        FIELD_INFO(student_t, roll_no,       UINT32,          0),
        FIELD_INFO(student_t, age,           UINT32,          0),
        FIELD_INFO(student_t, best_college,  OBJ_PTR, student_t),
        FIELD_INFO(student_t, aggregate,     FLOAT,           0)
    };
    REG_STRUCT(struct_db, student_t, stud_fields);

    print_structure_db(struct_db);

    /* Working with object database */
    /* Step 1 : Inititalize a new Object Database */
    object_db_t *object_db = calloc(1, sizeof(object_db_t));
    object_db->struct_db = struct_db;

    /* Step 2 : Create some sample objects, equivalent to standartd
     * calloc(1, sizeof(student_t)                                  */
    student_t *ujjwal = xcalloc(object_db, "student_t", 1);
    student_t *simran = xcalloc(object_db, "student_t", 1);
    emp_t *sneha = xcalloc(object_db, "emp_t", 2);

    strncpy(ujjwal->stud_name, "ujjwal", strlen("ujjwal"));
    ujjwal->age = 23;
    ujjwal->aggregate = 99;
    ujjwal->best_college = NULL;
    ujjwal->roll_no = 29;

    strncpy(simran->stud_name, "simran", strlen("simran"));
    simran->age = 23;
    simran->aggregate = 99;
    simran->best_college = NULL;
    simran->roll_no = 29;
    
    print_object_db(object_db);
    xfree(object_db, ujjwal);
    printf("\n");
    print_object_db(object_db);
    xfree(object_db, sneha);
    print_object_db(object_db);
    return 0;
}
