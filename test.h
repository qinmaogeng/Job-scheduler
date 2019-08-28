#ifndef TEST_H_
#define TEST_H_

#include <iomanip>
#include <iostream>
using namespace std;

enum RBTColor{ Red, Black }; /* RBT color definition*/

class job{
public:
	job(){};
	int ID;
	int exe_t;
	int total_t;
	int remain_t;

	job(int a, int b, int c, int d) :
		ID(a), exe_t(b), total_t(c), remain_t(d){}
}; /* class that represents every job*/

class RBTNode{
public:
	RBTNode(){};
	RBTColor color;
	int key;
	RBTNode *left; /*left child*/
	RBTNode *right; /*right child*/
	RBTNode *parent; /*parent*/
	job *jobpointer; /*pointer pointing to a job*/

	RBTNode(int k, job *j, RBTColor c, RBTNode *p, RBTNode *l, RBTNode *r) :
		key(k), jobpointer(j), color(c), parent(p), left(l), right(r) {}
}; /*class of RBTNode, which contain the information of a node including the 
color, key ,data which is a pointer pointing to a job and the left and right child and parent of a RBT Node*/

class RBT
{
public:
	RBTNode *Root;
	RBT();
	~RBT();
	void insert(job *j); 
	void remove(int key);
	RBTNode* previous(int key);
	RBTNode* next(int key);
	void findRange(int low, int high);
	RBTNode* search(RBTNode *x,int key);
private:
	void LRotate(RBTNode* &root, RBTNode* x);
	void RRotate(RBTNode* &root, RBTNode* y);
	void insert(RBTNode* &root, RBTNode* node);
	void insertFix(RBTNode* &root, RBTNode* node);
	void remove(RBTNode* &root, RBTNode *node);
	void removeFix(RBTNode* &root, RBTNode *node, RBTNode *parent);
	void findRange(RBTNode* point, int low, int high); 

#define rb_parent(r)   ((r)->parent)
#define rb_color(r) ((r)->color)
#define rb_is_red(r)   ((r)->color==Red)
#define rb_is_black(r)  ((r)->color==Black)
#define rb_set_black(r)  do { (r)->color = Black; } while (0)
#define rb_set_red(r)  do { (r)->color = Red; } while (0)
#define rb_set_parent(r,p)  do { (r)->parent = (p); } while (0)
#define rb_set_color(r,c)  do { (r)->color = (c); } while (0)   /*some definition of color and relationship seting and geting function*/
};



RBT::RBT(){
	Root = NULL;
}

RBTNode* RBT::search(RBTNode *x,int key){
	if (x == NULL || x->key == key)
		return x;

	if (key < x->key)
		return search(x->left, key);
	else
		return search(x->right, key);
} /*recursively search a Node with a given key, return it*/

void RBT::LRotate(RBTNode* &root, RBTNode* x){
	RBTNode *y = x->right;
	x->right = y->left;
	if (y->left != NULL)
		y->left->parent = x;
	y->parent = x->parent;
	if (x->parent == NULL)
	{
		root = y;
	}
	else
	{
		if (x->parent->left == x)
			x->parent->left = y;
		else
			x->parent->right = y;
	}
	y->left = x;
	x->parent = y;
} /*left or what we called in course RR rotate function */

void RBT::RRotate(RBTNode* &root, RBTNode* y){
	RBTNode *x = y->left;
	y->left = x->right;
	if (x->right != NULL)
		x->right->parent = y;
	x->parent = y->parent;
	if (y->parent == NULL)
	{
		root = x;
	}
	else
	{
		if (y == y->parent->right)
			y->parent->right = x;
		else
			y->parent->left = x;
	}
	x->right = y;
	y->parent = x;
} /*right or what we called in course LL rotate function*/

void RBT::insertFix(RBTNode* &root, RBTNode* node){ /*insert fixing function */
	RBTNode *parent, *gparent;
	while ((parent = rb_parent(node)) && rb_is_red(parent)) /*if the newly inserted node and its parent are red,we should fix it*/
	{
		gparent = rb_parent(parent); /*do the rotate and color chage according to case*/
		if (parent == gparent->left) 
		{
			{
				RBTNode *uncle = gparent->right;
				if (uncle && rb_is_red(uncle))
				{
					rb_set_black(uncle);
					rb_set_black(parent);
					rb_set_red(gparent);
					node = gparent;
					continue;
				} 
			}
			if (parent->right == node)
			{
				RBTNode *tmp;
				LRotate(root, parent);
				tmp = parent;
				parent = node;
				node = tmp;
			}
			rb_set_black(parent);
			rb_set_red(gparent);
			RRotate(root, gparent);
		}
		else
		{
			{
				RBTNode *uncle = gparent->left;
				if (uncle && rb_is_red(uncle))
				{
					rb_set_black(uncle);
					rb_set_black(parent);
					rb_set_red(gparent);
					node = gparent;
					continue;
				}
			}
			if (parent->left == node)
			{
				RBTNode *tmp;
				RRotate(root, parent);
				tmp = parent;
				parent = node;
				node = tmp;
			}
			rb_set_black(parent);
			rb_set_red(gparent);
			LRotate(root, gparent);
		}
	}
	rb_set_black(root);
}

void RBT::insert(RBTNode* &root, RBTNode* node){ /*insert function,first search the position where the node should be inserted and then fix the RBT*/
	RBTNode *y = NULL;
	RBTNode *x = root;
	while (x != NULL)
	{
		y = x;
		if (node->key < x->key)
			x = x->left;
		else
			x = x->right;
	}

	node->parent = y;
	if (y != NULL)
	{
		if (node->key < y->key)
			y->left = node;
		else
			y->right = node;
	}
	else
		root = node;
	node->color = Red;
	insertFix(root, node);
}

void RBT::insert(job *j){
	RBTNode *a = NULL;
	a = new RBTNode(j->ID, j, Black, NULL, NULL, NULL); /*create a new node with given job pointer*/
	insert(Root, a); /*using the private insert function to insert*/
}

void RBT::removeFix(RBTNode* &root, RBTNode *node, RBTNode *parent) /*remove fix function,do the color change and rotate according to cases*/
{
	RBTNode *other;

	while ((!node || rb_is_black(node)) && node != root)
	{
		if (parent->left == node)
		{
			other = parent->right;
			if (rb_is_red(other))
			{
				rb_set_black(other);
				rb_set_red(parent);
				LRotate(root, parent);
				other = parent->right;
			}
			if ((!other->left || rb_is_black(other->left)) &&
				(!other->right || rb_is_black(other->right)))
			{
				rb_set_red(other);
				node = parent;
				parent = rb_parent(node);
			}
			else
			{
				if (!other->right || rb_is_black(other->right))
				{
					rb_set_black(other->left);
					rb_set_red(other);
					RRotate(root, other);
					other = parent->right;
				}
				rb_set_color(other, rb_color(parent));
				rb_set_black(parent);
				rb_set_black(other->right);
				LRotate(root, parent);
				node = root;
				break;
			}
		}
		else
		{
			other = parent->left;
			if (rb_is_red(other))
			{
				rb_set_black(other);
				rb_set_red(parent);
				RRotate(root, parent);
				other = parent->left;
			}
			if ((!other->left || rb_is_black(other->left)) &&
				(!other->right || rb_is_black(other->right)))
			{
				rb_set_red(other);
				node = parent;
				parent = rb_parent(node);
			}
			else
			{
				if (!other->left || rb_is_black(other->left))
				{
					rb_set_black(other->right);
					rb_set_red(other);
					LRotate(root, other);
					other = parent->left;
				}
				rb_set_color(other, rb_color(parent));
				rb_set_black(parent);
				rb_set_black(other->left);
				RRotate(root, parent);
				node = root;
				break;
			}
		}
	}
	if (node)
		rb_set_black(node);
}

void RBT::remove(RBTNode* &root, RBTNode *node) /*remove function,first remove the node and then fix the RBT*/
{
	RBTNode *child, *parent;
	RBTColor color;
	if ((node->left != NULL) && (node->right != NULL)) /*if the node is a two child-node,replace it with the least ID in its right sub tree*/
	{
		RBTNode *replace = node;
		replace = replace->right;
		while (replace->left != NULL)
			replace = replace->left;
		if (rb_parent(node))
		{
			if (rb_parent(node)->left == node)
				rb_parent(node)->left = replace;
			else
				rb_parent(node)->right = replace;
		}
		else
			root = replace;
		child = replace->right;
		parent = rb_parent(replace);
		color = rb_color(replace);
		if (parent == node)
		{
			parent = replace;
		}
		else
		{
			if (child)
				rb_set_parent(child, parent);
			parent->left = child;

			replace->right = node->right;
			rb_set_parent(node->right, replace);
		}

		replace->parent = node->parent;
		replace->color = node->color;
		replace->left = node->left;
		node->left->parent = replace;

		if (color == Black)
			removeFix(root, child, parent);

		delete node;
		return;
	}

	if (node->left != NULL)
		child = node->left;
	else
		child = node->right;

	parent = node->parent;
	color = node->color;

	if (child)
		child->parent = parent;

	if (parent)
	{
		if (parent->left == node)
			parent->left = child;
		else
			parent->right = child;
	}
	else
		root = child;

	if (color == Black)
		removeFix(root, child, parent);
	delete node;
}

void RBT::remove(int key)
{
	RBTNode *node;
	if ((node = search(Root, key)) != NULL)
		remove(Root, node);
}

RBTNode* RBT::next(int key){
	RBTNode *x = Root; /*begin finding the node from the root*/
	RBTNode *y = NULL;
	while (1)
	{
		if (x->key > key)
		{
			y = x;  /*if current node's key is less to the given key, go to its right tree*/
			if (x->left != NULL)
				x = x->left;
			else
				break;
		}
		else if (x->key <= key){  /*if current node's key is less or equal to the given key, go to its right tree*/
			if (x->right != NULL)
				x = x->right;
			else
				break;
		}
	}

	if (y == NULL)
		return y;
	else
		return x;	
} /*function of finding the node with the least ID bigger than the given ID*/

RBTNode* RBT::previous(int key){
	RBTNode *x = Root; /*begin finding the node from the root*/ 
	RBTNode *y = NULL;
        if(x == NULL) /*return NULL if nothing in the RBT*/
             return NULL;
	while (1)
	{
		if (x->key >= key) /*if current node's key is bigger or equal to the given key, go to its left subtree*/
		{
			if (x->left != NULL) /*if x have left child, go to its left child*/
				x = x->left;
			else if (x->left == NULL&&x->parent->key < x->key){ /*there is a situation that x have no left child but its parent's key is less than its key*/
				x = x->parent;
				break;
			}
			else
				break;
		}
		else if (x->key < key){ /*if current node's key is less than the given key, go to its right tree*/
			y = x;
			if (x->right != NULL)
				x = x->right;
			else
				break;
		}
	}

	if (y == NULL)
		return y;
	else
		return x;
} /*function of finding the node with the largest ID less than the given ID*/

void RBT::findRange(RBTNode* root, int low, int high){
	RBTNode *x = root; /*begin at the root*/
	if (x == NULL)
		return;

	if (x->key >= low&&x->key <= high)
	{
		cout << "(" << x->jobpointer->ID << "," << x->jobpointer->exe_t << "," << x->jobpointer->total_t << ")";
		findRange(x->right, low, high);
		findRange(x->left, low, high);
	}

	else if (x->key < low)
	{
		findRange(x->right, low, high);
	}

	else
		findRange(x->left, low, high);

} /*recursively search the node between the given two IDs*/

void RBT::findRange(int low, int high){
	findRange(Root, low, high);
}

class PHeap
{
public:
	PHeap(){};
	job *jobpointer; /*pointer pointing to a job*/
	int key;
	PHeap *child; /*the leftest child pointer*/
	PHeap *sibling; /*the node's right sibling*/
	PHeap *prev; /*the node's left sibling*/
	PHeap(job*j, int k, PHeap *c, PHeap *s, PHeap *p) :
		jobpointer(j), key(k), child(c), sibling(s), prev(p){} /*construct a new node*/
	PHeap* merge(PHeap *p, PHeap *q);
	PHeap* insert(PHeap* p, int key, job* j);
	PHeap* deleteMin(PHeap *p);
	PHeap* increaseKey(int val,PHeap *p);
private:
	PHeap* combine(PHeap *p);
}; /*class of the min heap which is a min pairing heap,the class also represent a node of a heap*/

PHeap* PHeap::merge(PHeap *p, PHeap *q){ /*function of merging two heaps, set the heap with the smaller key to the leftest child of the larger heap*/
	if (q == NULL)
		return p;
	else if ( p == NULL)
	{
		return q;
	}
	else if (p->key <= q->key) 
	{
		q->prev = p;
		p->sibling = q->sibling;
		if (p->sibling != NULL)
			p->sibling->prev = p;

		q->sibling = p->child;
		if (q->sibling != NULL)
			q->sibling->prev = q;

		p->child = q;
		return p;
	}
	else
	{
		q->prev = p->prev;
		p->prev = q;
		p->sibling = q->child;
		if (p->sibling != NULL)
			p->sibling->prev = p;

		q->child = p;
		return q;
	}
}

PHeap* PHeap::insert(PHeap* p, int key, job* j){
	PHeap *node = new PHeap(j, key, NULL, NULL, NULL); 
    return merge(p, node);
} /*function of insert a new node, just merge it with the existed heap*/

PHeap* PHeap::combine(PHeap* p){
	PHeap *array[1024];
	int i, num;

	if (p->sibling == NULL)
		return p;

	for (num = 0; p != NULL; num++)
	{
		array[num] = p;
		p->prev->sibling = NULL;
		p = p->sibling;
	} /*store the heap node into an array*/
	array[num] = NULL;

	for (i = 1; i < num; i++)
		array[i] = merge(array[i - 1], array[i]); /*merge the nodes from left to right*/

	return array[num - 1];
} /*combine the heap node from left to right after deletemin or increase key*/

PHeap* PHeap::deleteMin(PHeap* p){
	PHeap *new_Min = NULL;

	if (p == NULL)
		return NULL;
	else
	{
		if (p->child != NULL)
			new_Min = combine(p->child); //

		free(p);
	}
	return new_Min;
} /*deleteMIn operation, combine the child of the min node with all the heap node in the double link lise*/

PHeap* PHeap::increaseKey(int val, PHeap *p){
	if (p == NULL)
		return NULL;
	p->key = p->key + val;
	int k = p->key;
	job* j = p->jobpointer;
	PHeap *new_Min = p;
	PHeap *temp = p->child;
	while (temp != NULL)
	{
		if (temp->key < p->key)
		{
			new_Min = deleteMin(p);
			new_Min = insert(new_Min, k, j);
			break;
		}
		else
			temp = temp->sibling;
	}
	return new_Min;
}   /*increasekey operation, increase the key of the min heap node with the given value, and then check if it is biger than any node in its child's link list,
       if it is,then we remove it from the heap and then reinsert it by combining it with all heap nodes in its child's link list,after that, the min node will
        be updated*/






#endif
