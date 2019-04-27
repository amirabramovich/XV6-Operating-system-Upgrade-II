#include "types.h"
#include "stat.h"
#include "user.h"
#include "tournament_tree.h"

int 
floor(int n)
{
    return n - (n % 1);
}

// Allocates a new tournament tree lock and initializes it. 
// The function returns a pointer to the initialized tree, or 0 upon failure. 
// Depth indicates the depth of the tree, at least 1. As such the tree can accommodate 2 depth threads.
trnmnt_tree*
trnmnt_tree_alloc(int depth) 
{
    int n = (1 << depth) - 1;
    trnmnt_tree *tree = malloc(sizeof *tree + n * sizeof(*tree->nodes));
    tree->size = n;
    for (int i = 0; i < n; i++) {
        tree->nodes[i] = kthread_mutex_alloc();
        if(tree->nodes[i] < 0){
            for(; i >= 0 ; i--)
                kthread_mutex_dealloc(tree->nodes[i]);
            free(tree);
            return 0;
        }
    }
    return tree;
}

// De-allocates the tournament tree lock, which is no longer needed. 
// Returns 0 upon success and -1 upon failure (e.g., if there are threads waiting).
int 
trnmnt_tree_dealloc(trnmnt_tree* tree)
{
    for (int i = 0; i < tree->size; i++)
        if(kthread_mutex_dealloc(tree->nodes[i]) < 0)
            return -1;
    free(tree);
    return 0;
}

// This function is used by a thread to lock the tree specified by the argument trnmnt tree. 
// ID indicates the ID of the lock acquirer thread, a number between 0 and 2depth − 1. 
// It is not necessarily the Thread ID. 
// There should be no 2 threads in the tree with the same ID.
int 
trnmnt_tree_acquire2(trnmnt_tree* tree, int ID)
{
    int parent = floor((ID-1)/2);
    if(kthread_mutex_lock(tree->nodes[parent]) < 0)
        return -1;
    if (parent == 0)
        return 0;
    return trnmnt_tree_acquire2(tree, parent);
}

int 
trnmnt_tree_acquire(trnmnt_tree* tree, int ID)
{
    int fixed_id = ID + tree->size;
    return trnmnt_tree_acquire2(tree, fixed_id);
}

// This function is used by a thread to release the tree specified by the argument trnmnt tree. 
// ID indicates the ID of the lock acquirer thread, a number between 0 and 2depth − 1. 
// It should be the same as the one used by the thread in acquiring the lock.
void 
trnmnt_tree_release2(trnmnt_tree* tree, int ID)
{
    int parent = floor((ID-1)/2);
    if (parent != 0)
        trnmnt_tree_release2(tree, parent);
    kthread_mutex_unlock(tree->nodes[parent]);
}

int 
trnmnt_tree_release(trnmnt_tree* tree, int ID)
{
    int fixed_id = ID + tree->size;
    trnmnt_tree_release2(tree, fixed_id);
    return 0;
}
