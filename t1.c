#include<stdio.h>
#include<stdlib.h>
#include "t1.h"

int count = 0;
//Function to generate the alphabets back from the number
static char val2char ( int c ) {
	if ( c < 0 || c > 25 ) {
		return -1;
	}

	return c+'A';
}

//Using queue to help in search algorithms
//Struct for queue
typedef struct queue {
  int size;
  int front;
  int rear;
  int *Q;
}queue;

//Operations on queue

//Create a queue
queue* create_queue(int size) {
  queue *q = (queue *)malloc(sizeof(queue));
  q->front = -1;
  q->rear = -1;
  q->size = size;
  q->Q = (int *)malloc(size * sizeof(int));
  return q;
}

//Checking if queue is empty
int is_empty(queue *q) {
  if(q->rear == -1)
    return 1;
  return 0;
}

//Adding into queue
void enqueue(queue *q, int x) {
  if(q->rear == q->size - 1)
    return;
  else{
    q->rear++;
    q->Q[q->rear] = x;
  }
}

//Deleting from a queue
int dequeue(queue *q){
  int x = -1;
  if(q->front == q->rear){
    q->front = q->rear = -1;
    return x;
  }else {
    q->front++;
    x = q->Q[q->front];
  }
  return x;
}

//Creating a node in Adjacency List 
adjacencyList* create_node(int vertex) {
  adjacencyList *node = (adjacencyList *)malloc(sizeof(adjacencyList));
  node->vertex = vertex;
  node->next = NULL;
  return node;
}
//Creating a graph using the given number of nodes
Graph* create_graph(int num_nodes) {
  Graph *graph = (Graph *)malloc(sizeof(Graph));
  graph->number_of_vertices = num_nodes;
  graph->list = (adjacencyList **)malloc(num_nodes * sizeof(adjacencyList *));
  graph->visited = (int *)malloc(num_nodes * sizeof(int));
  //initialising the lists for each vertex in the adj. list and the visited array
  for(int i = 0; i < num_nodes; i++){
    graph->list[i] = NULL;
    graph->visited[i]=0;
  }
  return graph;
}

//Adding a corresponding edge in the list - inserting the node of adjacency list in the list of the 'from' vertex
void sorted_insert(adjacencyList **head, adjacencyList *new_node){
  adjacencyList *current;
  if(*head == NULL || (*head)->vertex >= new_node->vertex){
    new_node->next = *head;
    *head = new_node;
  } else{
    current = *head;
    while(current->next != NULL && current->next->vertex < new_node->vertex){
      current = current->next;
    }
    new_node->next = current->next;
    current->next = new_node;
  }
}
void add_edge(Graph *g, int from, int to) {
  adjacencyList *new_node = create_node(to);
  // new_node->next = g->list[from];
  // g->list[from] = new_node;
  //Sorted Insert
  sorted_insert(&(g->list[from]), new_node);
}

void reset_visited(Graph *g){
  for(int i = 0; i < g->number_of_vertices; i++){
    g->visited[i] = 0;
  }
}
//Breadth First Search Algorithm
void bfs(Graph* g, int origin) {
  printf("BFS  ");
  reset_visited(g);//reset needed because dfs is ran first
  queue *q = create_queue(g->number_of_vertices);//creating a queue
  g->visited[origin] = 1;
  enqueue(q, origin);//queuing the origin node
  int i = 0;
  while(!is_empty(q)){
    int current_vertex = dequeue(q);
    if(current_vertex == -1) break;//break if queue is emptied on this iteration
    if(i == g->number_of_vertices - 1)
      printf("%c \n", val2char(current_vertex));
    else
      printf("%c  ", val2char(current_vertex));
    adjacencyList *temp = g->list[current_vertex];
    while(temp != NULL){
      int adjacent_vertex = temp->vertex;
      if(g->visited[adjacent_vertex] == 0){//If you have not yet visited the adjacent vertex
        g->visited[adjacent_vertex] = 1;
        enqueue(q, adjacent_vertex);//add adjacent vertex in queue
      }
      temp = temp->next;
    }
    i++;
  }
  free(q->Q);
  free(q);
}
//Depth First Search Algorithm -- Using recursion as they perform via stack
void dfs(Graph* g, int origin){
  if (count == 0){
    printf("DFS: ");
  }
  count++;
  adjacencyList *temp = g->list[origin];
  g->visited[origin] = 1;
  if (count == g->number_of_vertices)
    printf("%c \n",val2char(origin));
  else
    printf("%c  ",val2char(origin));
  while(temp != NULL){
    int adjacent_vertex = temp->vertex;
    if(g->visited[adjacent_vertex] == 0){//if adjacent vertex is not yet visited 
      dfs(g, adjacent_vertex);//add the adjacent vertex in stack
    }
    temp = temp->next;
  }
}

//delete the graph generated
void delete_graph(Graph* g){
  free(g->visited);//freeing the visited array
  //freeing adjacency list
  for(int i = 0; i < g->number_of_vertices; i++ ){
    adjacencyList *temp;
    while (g->list[i] != NULL){
      temp = g->list[i];
      g->list[i] = g->list[i]->next;
      free(temp);
    }
  }
  free(g->list);
  free(g);
}