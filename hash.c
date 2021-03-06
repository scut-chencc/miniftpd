#include "hash.h"
#include "common.h"


//测试使用

typedef struct str
{
    char* name;
    char* school;
}stu_t;

unsigned int static hash_fun(void* key,int hash_size)
{
    unsigned int hash = 0;
    char* str = key;

    while ( *str ) 
    {
        hash = (*str++) + (hash << 6) + (hash << 16) -hash;
    }

    return (hash & 0x7FFFFFFF) % hash_size;
        
}

void hash_test()
{
    stu_t stu[3] = {
        {"aaa","12345"},
        {"bbb","dadad"},
        {"ccc","xxxxx"},
    };

    hashtable_t* hash = hashtable_init(5,hash_fun);
    int i = 0;
    for ( i = 0 ; i < 3 ; ++i)
    {
        hashtable_add(hash,stu[i].name,strlen(stu[i].name),stu[i].school,strlen(stu[i].name));
    }

    hashtable_del(hash,"aaa",strlen("aaa"));
    hashtable_add(hash,stu[0].name,strlen(stu[0].name),stu[0].school,strlen(stu[0].name));


    char *str = hashtable_search(hash,"aaa",3);
    if  ( NULL == str )
        printf("not fount\n");
    else
        printf("name:\"aaa\",school:%s\n",str);


    hashtable_destroy(&hash);
}





//
typedef struct node
{
    void* key;
    void* val;
    struct node* pnext; 
    struct node* pprev;
}node_t;

struct hashtable
{
    int buckets;
    hash_fun_t hash_fun;
    node_t** pnode;
};

node_t* hashtable_getbucket(hashtable_t* hash,void* key);
node_t* hashtable_get_node(hashtable_t* hash,void* key,int key_size);

hashtable_t* hashtable_init(int buckets,hash_fun_t hash_fun)
{
    hashtable_t* hash = (hashtable_t*)malloc(sizeof(hashtable_t));

    int size = buckets * sizeof(node_t*);
    hash->pnode = (node_t**)malloc(size);
    hash->buckets = buckets;
    hash->hash_fun = hash_fun;
    memset(hash->pnode,0,size);

    int i = 0;
    while ( i < buckets )
    {
        (hash->pnode[i]) = (node_t*)malloc(sizeof(node_t));
        (hash->pnode[i])->pnext = NULL;
        i++;
    } 

    return hash;
}
void hashtable_destroy(hashtable_t** phash)
{
    int i = 0;
    hashtable_t* hash = *phash;
    while ( i < hash->buckets )
    {
        node_t* node= (hash->pnode)[i]->pnext;
        while ( NULL != node) 
        {
            node_t* tmp = node;
            node = node->pnext;
            free(tmp->key);
            free(tmp->val);
            free(tmp);
        }
        free(hash->pnode[i]);
        i++;
    } 
    free((hash->pnode));
    free(hash);
    *phash = NULL;
}

node_t* hashtable_getbucket(hashtable_t* hash,void* key)
{
    unsigned int nbucket = hash->hash_fun(key,hash->buckets);
    if ( nbucket > hash->buckets )
    {
        fprintf(stderr,"hash fun error.");
        return NULL;
    }
    return (hash->pnode)[nbucket];

}

node_t* hashtable_get_node(hashtable_t* hash,void* key,int key_size)
{
    node_t* node = hashtable_getbucket(hash,key)->pnext;
    while ( NULL != node )
    {
        if ( 0 == memcmp(key,node->key,key_size) )
        {
            return node;
        }
        node = node->pnext;
    }
    return NULL;
}

int hashtable_add(hashtable_t* hash,void* key,int key_size,void* val,int val_size)
{
     if  ( NULL != hashtable_get_node(hash,key,key_size)) 
     {
         fprintf(stderr,"key exists.");
         return -1;
     }

     node_t* node = hashtable_getbucket(hash,key);
     node_t* toadd = (node_t*)malloc(sizeof(node_t));
     toadd->key = (void*)malloc(key_size);
     toadd->val = (void*)malloc(val_size);
     toadd->pnext = NULL;
     memcpy(toadd->key,key,key_size);
     memcpy(toadd->val,val,val_size);

     toadd->pnext = node->pnext;
     toadd->pprev = node;
     node->pnext = toadd;
     if ( toadd->pnext )
         toadd->pnext->pprev = toadd;
     
     return 0;
}
int hashtable_del(hashtable_t* hash,void* key,int key_size)
{
     node_t* node = hashtable_get_node(hash,key,key_size);
     if ( NULL == node) 
     {
         fprintf(stderr,"key not exists.");
         return -1;
     }
     
     node->pprev->pnext = node->pnext;
     if ( node->pnext )
         node->pnext->pprev = node->pprev;
     free(node->key);
     free(node->val);
     free(node);
     return 0;
}
void* hashtable_search(hashtable_t* hash,void* key,int key_size)
{
    node_t* node = hashtable_get_node(hash,key,key_size);
    if ( NULL != node) 
    {
        return node->val;
    }
    return NULL;
}

