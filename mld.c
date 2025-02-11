#include "mld.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

char *DATA_TYPE[] = {"UINT8", "UINT32", "INT32",
                     "CHAR", "OBJ_PTR", "VOID_PTR", "FLOAT",
                     "DOUBLE", "OBJ_STRUCT"};

/* Function to add the structure record in a structure database */
/* Return 0 on success & -1 on Failure                          */
int add_structure_to_struct_db(struct_db_t *struct_db, struct_db_rec_t *struct_rec)
{
    struct_db_rec_t *temp = struct_db->head;
    
    //First node in the list
    if(struct_db->head == NULL){
        struct_db->head = struct_rec;
        struct_rec->next = NULL;
    }

    else{
        while(temp->next)
        {
            temp = temp->next;
        }
        temp->next = struct_rec;
        struct_rec->next = NULL;
    }
    struct_db->count++;
    return 0;
}


/* Function to print the structure Database */
void print_structure_db(struct_db_t *struct_db)
{
    struct_db_rec_t *temp = struct_db->head;
    printf("No of structures registered - %u\n\n", struct_db->count);

    int count = 0;
    while(temp)
    {
        printf("Structure No : %-5d (%p)\n", count, temp);
        printf("%-20s      Size = %-5u       No_Of_Fields = %-5u\n", temp->struct_name, temp->ds_size, temp->n_fields );
        
        field_info_t *arr_fields = temp->fields;

        int len = temp->n_fields;
        for(int i=0;i<len;i++)
        {
            field_info_t *struct_fields = arr_fields+(i);
            printf("%-5d  %-30s  dtype = %-5d  size = %-5u   offset = %-5u   nested_struct_name = %-30s\n",
                    i, struct_fields->fname, struct_fields->dtype, struct_fields->size, struct_fields->offset, struct_fields->nested_str_name);
        }
        printf("\n");
        count++;
        temp = temp->next;
    }
}

/* To search for the structure record with the given name */
/* Return a pointer to the corresponding record           */
struct_db_rec_t *struct_db_look_up(struct_db_t *struct_db, char *struct_name)
{
    struct_db_rec_t *temp = struct_db->head;

    while(temp)
    {
        if(!strcmp(struct_name, temp->struct_name))
            return temp;
        temp = temp->next;
    }
    return NULL;
}

/* Add support for primitive data types */
void mld_init_primitive_data_types_support(struct_db_t *struct_db)
{
    REG_STRUCT(struct_db, int, 0);
    REG_STRUCT(struct_db, float, 0);
    REG_STRUCT(struct_db, double, 0);
}


/* To search if the object is already present in the database */
/* Returns the object if its present or else NULL             */
object_db_rec_t *object_db_lookup(object_db_t *object_db, void *ptr)
{
    object_db_rec_t *head = object_db->head;
    while(head)
    {
        if(head->ptr == ptr)
            return head;
        head = head->next;
    }
    return NULL;
}

static void add_object_to_object_db(object_db_t *object_db, void *ptr, int units, struct_db_rec_t *struct_rec, mld_boolean_t bool_val)
{
    object_db_rec_t *object_db_rec = object_db_lookup(object_db, ptr);

    /* Don't add the same object twice */
    if(object_db_rec){
        printf("The object is already in the database");
        assert(1);
    }

    object_db_rec = calloc(1, sizeof(object_db_rec_t));
    object_db_rec->ptr = ptr;
    object_db_rec->units = units;
    object_db_rec->struct_rec = struct_rec;
    object_db_rec->is_root = bool_val;
    object_db_rec->is_visited = MLD_FALSE;
    
    object_db_rec_t *head = object_db->head; 
    
    if(!head)
    {
        object_db->head = object_db_rec;
        object_db_rec->next = NULL;
        object_db->count++;
        return;
    }
    
    object_db_rec->next = head;
    object_db->head = object_db_rec;
    object_db->count++;
    
}


void *xcalloc(object_db_t *object_db, char *struct_name, int units)
{
    struct_db_rec_t *struct_rec = struct_db_look_up(object_db->struct_db, struct_name);
    assert(struct_rec);
    void *ptr = calloc(units, struct_rec->ds_size);
    add_object_to_object_db(object_db, ptr, units, struct_rec, MLD_FALSE);
    return ptr;
}

/* Dumping Functions for Object Database */
void print_object_rec(object_db_rec_t *obj_rec, int i){

    if(!obj_rec)
        return;
    printf(ANSI_COLOR_MAGENTA"-----------------------------------------------------------------------------------------------------|\n");
    printf(ANSI_COLOR_YELLOW"%-3d ptr = %-10p | next = %-10p | units = %-4d | struct_name = %-10s           |\n",
            i, obj_rec->ptr, obj_rec->next, obj_rec->units, obj_rec->struct_rec->struct_name);
    printf(ANSI_COLOR_MAGENTA"-----------------------------------------------------------------------------------------------------|\n");
}

void print_object_db(object_db_t *object_db)
{
    object_db_rec_t *head = object_db->head;
    unsigned int i = 0;
    printf(ANSI_COLOR_CYAN "Printing OBJECT DATABASE\n");
    for(; head; head = head->next){
        print_object_rec(head, i++);
        //mld_dump_object_rec_detail(head);
    }
}

/* Print all supported fields by mld library for the object record passed as argument */
void mld_dump_object_rec_detail(object_db_rec_t *obj_rec)
{
    int n_fields = obj_rec->struct_rec->n_fields;
    field_info_t *field = NULL;

    int units = obj_rec->units, obj_index = 0,
        field_index = 0;

    for(; obj_index < units; obj_index++){
        char *current_object_ptr = (char *)(obj_rec->ptr) + \
                        (obj_index * obj_rec->struct_rec->ds_size);

        for(field_index = 0; field_index < n_fields; field_index++){
            
            field = &obj_rec->struct_rec->fields[field_index];

            switch(field->dtype){
                case UINT8:
                case INT32:
                case UINT32:
                    printf("%s[%d]->%s = %d\n", obj_rec->struct_rec->struct_name, obj_index, field->fname, *(int *)(current_object_ptr + field->offset));
                    break;
                case CHAR:
                    printf("%s[%d]->%s = %s\n", obj_rec->struct_rec->struct_name, obj_index, field->fname, (char *)(current_object_ptr + field->offset));
                    break;
                case FLOAT:
                    printf("%s[%d]->%s = %f\n", obj_rec->struct_rec->struct_name, obj_index, field->fname, *(float *)(current_object_ptr + field->offset));
                    break;
                case DOUBLE:
                    printf("%s[%d]->%s = %f\n", obj_rec->struct_rec->struct_name, obj_index, field->fname, *(double *)(current_object_ptr + field->offset));
                    break;
                case OBJ_PTR:
                    printf("%s[%d]->%s = %p\n", obj_rec->struct_rec->struct_name, obj_index, field->fname,  (void *)*(long *)(current_object_ptr + field->offset));
                    break;
                case OBJ_STRUCT:
                    /*Later*/
                    break;
                default:
                    break;
            }
        }
    }
}


void xfree(object_db_t *object_db, void *ptr)
{
    object_db_rec_t *object_db_rec = object_db_lookup(object_db, ptr);
    if(!object_db_rec)
    {
        printf("The object is not present in the object data base \n");
        assert(1);
    }

    object_db_rec_t *head = object_db->head;
    object_db_rec_t *prev = NULL;
    while(head)
    {
        if(head->ptr == ptr)
            break;
        prev = head;
        head = head->next;
    }
    /* First Node */
    if(prev == NULL && head->next == NULL)
        object_db->head = NULL;
    else if(prev == NULL && head->next != NULL)
        object_db->head = head->next;
    else if(prev && head->next==NULL)
        prev->next = NULL;
    else if(prev && head->next){
        prev->next = head->next;
    }
    object_db->count--;
    free(head->ptr);
    free(head);

}

void mld_register_global_object_as_root(object_db_t *object_db,
                                        void *objptr,
                                        char *struct_name,
                                        unsigned int units){
    struct_db_rec_t *struct_rec = struct_db_look_up(object_db->struct_db, struct_name);
    assert(struct_rec);

    add_object_to_object_db(object_db, objptr, units, struct_rec, MLD_TRUE);

}

void mld_set_dynamic_object_as_root(object_db_t *object_db, void *obj_ptr)
{
    object_db_rec_t *obj_rec = object_db_lookup(object_db, obj_ptr);
    assert(obj_rec);

    obj_rec->is_root = MLD_TRUE;
}

static void init_mld_algorithm(object_db_t *object_db)
{
    object_db_rec_t *obj_rec = object_db->head;
    while(obj_rec){
        obj_rec->is_visited = MLD_FALSE;
        obj_rec = obj_rec->next;
    }
}

/* MLD Algorithm for Directred Acyclic Graphs */

void mld_explore_objects_recursively(object_db_t *object_db, object_db_rec_t *object_rec)
{
    unsigned int i;
    while(object_rec)
    {
        //printf("New struct - %p\n",object_rec);
        /* If the Parent object is an array */
        for(i=0;i<object_rec->units;i++)
        {
            struct_db_rec_t *struct_rec = object_rec->struct_rec + i;
            //printf("%p\n",object_rec);

            if(object_rec->is_visited == MLD_TRUE)
                return;
            object_rec->is_visited = MLD_TRUE;
            char *parent_obj_ptr = object_rec->ptr;
            void *child_object_address = NULL;

            //struct_db_rec_t *struct_rec = object_rec->struct_rec;
            field_info_t *arr_fields = struct_rec->fields;
            //printf("%p\n",object_rec);

            int len = struct_rec->n_fields;
            for(int i=0;i<len;i++)
            {
                field_info_t *struct_fields = arr_fields+(i);
                if(struct_fields->dtype!=OBJ_PTR)
                    continue;

                void *child_obj_offset = parent_obj_ptr+struct_fields->offset;
                memcpy(&child_object_address, child_obj_offset, sizeof(void *));

                if(!child_object_address)
                    continue;

                object_db_rec_t *child_object_rec = object_db_lookup(object_db, child_object_address);
                if(child_object_rec->is_visited == MLD_FALSE){
                    //child_object_rec->is_visited = MLD_TRUE;
                    mld_explore_objects_recursively(object_db, child_object_rec);
                }
                
            }
        }
        
    }
}

void run_mld_algorithm(object_db_t *object_db)
{
    init_mld_algorithm(object_db);
    object_db_rec_t *head = object_db->head;

    while(head)
    {
        if(head->is_root == MLD_TRUE)
            mld_explore_objects_recursively(object_db, head);
        head = head->next;
    }

    printf("\n\n");
    head = object_db->head;
    while(head){
        if(head->is_visited == MLD_FALSE)
            printf("Leadked object - %s\n",head->struct_rec->struct_name);
        head = head->next;
    }
    return;
}

void report_leaked_objects(object_db_t *object_db){
    int i = 0;
    object_db_rec_t *head;

    printf("\nDumping Leaked Objects \n");

    for(head = object_db->head; head; head=head->next)
    {
        if(!head->is_visited){
            print_object_rec(head, i++);
            mld_dump_object_rec_detail(head);
            printf("\n\n");
        }
    }
}