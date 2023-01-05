#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include "t2.h"

#define MAX 9999
#define MAX_STRING_SIZE 50

//Structure for adjacency list
typedef struct adjacencyList {
  int vertex;
  int weight;
  struct adjacencyList *next;
}adjacencyList;
//Structure to store data of a vertex
typedef struct Vertices {
  int stop_id;
  char name[MAX_STRING_SIZE];
  char latitude[MAX_STRING_SIZE];
  char longitude[MAX_STRING_SIZE];  
}Vertices;
//Structure for the graph
typedef struct Graph {
  int number_of_vertices;
  Vertices **data;
  adjacencyList **list;
}Graph;

//Global variable for the graph
Graph *g;


//Structures for Djikstra Algorithm

typedef struct minHeapNode {
  int v;
  int distance;
}minHeapNode;

typedef struct minHeap {
  int size;//number of heap nodes present currently
  int capacity;//capacity of min heap
  int *pos;//this is for decrease_key()
  minHeapNode **array;
}minHeap;

//Creating graph and initialising the data
Graph* create_graph ( ) {
  Graph *graph = (Graph *)malloc(sizeof(Graph));
  graph->number_of_vertices = 0;
  graph->list = (adjacencyList **)malloc(MAX * sizeof(adjacencyList *));//for adjacency list
  graph->data = (Vertices **)malloc(MAX * sizeof(Vertices *));//for the data of the vertex
  for (int i = 0; i < MAX; i++){
    graph->list[i] = NULL;
    graph->data[i] = NULL;
  }
  return graph;
}
//Next token function from previous assignments --load functions are also similar
void next_token (char *buf, FILE *f, int string_max) {
  int bufferpos = 0;
  int inQuotes = 0;
  char c;
  for (int i = 0; i < string_max; i++) {
    c = fgetc(f);
    if (c == '"'){
      inQuotes = !inQuotes;
      continue;
    }
    if (c == ',' && !inQuotes) {
      bufferpos++;
      break;
    } else if (c == '\n') {
      bufferpos++;
      break;
    }
    buf[bufferpos] = c;
    bufferpos++;
  }
  buf[bufferpos] = '\0';
}
//adding the data of current vertex passed in load_vertices
void add_vertex (Vertices *v) {
  Vertices *vertex = (Vertices *)malloc(sizeof(Vertices));
  vertex->stop_id = v->stop_id;
  strcpy(vertex->name, v->name);
  strcpy(vertex->latitude, v->latitude);
  strcpy(vertex->longitude, v->longitude);
  g->data[v->stop_id] = vertex;
}
int load_vertices (char *fname) {  //loads the vertices from the CSV file of name fname
  g = create_graph();
  FILE *f;
  char buf[MAX_STRING_SIZE];
  Vertices *vertex = (Vertices *)malloc(sizeof(Vertices));
  int field_count = 0;
  f = fopen(fname, "r");
  if (!f) {
    printf("Unable to open %s\n", fname);
    return 0;
  }
  while (!feof(f)) {
    memset(buf, 0, sizeof(buf));
    next_token(buf, f, MAX_STRING_SIZE);
    if (field_count == 0){
      vertex->stop_id = atoi(buf);
    }
    else if(field_count == 1) {
      strcpy(vertex->name, buf);
    }
    else if(field_count == 2) {
      strcpy(vertex->latitude, buf);
    }
    else if (field_count == 3) {
      strcpy(vertex->longitude, buf);
    }
    field_count++;
    if (field_count == 4){
      field_count = 0;
      if (strcmp(vertex->name, "Name") == 0) continue;
      g->number_of_vertices++;
      add_vertex(vertex);
    }
  }
  free(vertex);
  fclose(f);
  printf("Loaded %d vertices\n", g->number_of_vertices);
  return g->number_of_vertices;//returning the number of vertices (non NULL) in the graph
}
//adding the adjacent nodes to the current vertex in adjacency list
void add_edge (int from, adjacencyList *e) {
  //From source to destination
  adjacencyList *edge1 = (adjacencyList *)malloc(sizeof(adjacencyList));
  edge1->vertex = e->vertex;
  edge1->weight = e->weight;
  edge1->next = g->list[from];
  g->list[from] = edge1;
  //From destination to source;
  adjacencyList *edge2 = (adjacencyList *)malloc(sizeof(adjacencyList));
  edge2->vertex = from;
  edge2->weight = g->list[from]->weight;
  edge2->next = g->list[g->list[from]->vertex];
  g->list[g->list[from]->vertex] = edge2;
}

int load_edges (char *fname) {//loads the edges from the CSV file of name fname
  FILE *f;
  char buf[MAX_STRING_SIZE];
  int field_count = 0;
  adjacencyList *connected_vertex = (adjacencyList *)malloc(sizeof(adjacencyList));
  int number_of_edges = 0;
  int vertex = 0;
  char for_comparision[MAX_STRING_SIZE];
  f = fopen(fname, "r");
  if (!f) {
    printf("Unable to open %s\n", fname);
    return 0;
  }
  while (!feof(f)) {
    memset(buf, 0, sizeof(buf));
    next_token(buf, f, MAX_STRING_SIZE);
    if (field_count == 0) {
      strcpy(for_comparision, buf);
      vertex = atoi(buf);
    }
    else if(field_count == 1) {
      connected_vertex->vertex = atoi(buf);
    }
    else if(field_count == 2) {
      connected_vertex->weight = atoi(buf);
    }
    field_count++;
    if (field_count == 3) {
      field_count = 0;
      if(strcmp(for_comparision, "vertex1") == 0) continue;
      add_edge(vertex, connected_vertex);
      number_of_edges++;
    }
  }
  free(connected_vertex);
  fclose(f);
  printf("Loaded %d edges\n", number_of_edges);
  return number_of_edges;//Returns the number of edges in the graph
}

//Dijkstra's Algorithm
//To add a node in min heap
minHeapNode* new_minHeap_node (int v, int distance) {
  minHeapNode *min_heap_node = (minHeapNode *)malloc(sizeof(min_heap_node));
  min_heap_node->v = v;
  min_heap_node->distance = distance;
  return min_heap_node;
}

//to create min heap
minHeap* create_minHeap (int capacity) {
  minHeap *min_heap = (minHeap *)malloc(sizeof(minHeap));
  min_heap->pos = (int *)malloc(capacity * sizeof(int));
  min_heap->size = 0;
  min_heap->capacity = capacity;
  min_heap->array = (minHeapNode **)malloc(capacity * sizeof(minHeapNode));
  return min_heap;
}

//function to swap nodes in heap for min heapify
void swap_minHeap_node(minHeapNode **a, minHeapNode **b) {
  minHeapNode *t = *a;
  *a = *b;
  *b = t;
}

//Standard function to heapify at given ids - updates the pos when they are swapped
void min_heapify(minHeap *min_heap, int id) {
  int smallest, left, right;
  smallest = id;
  left = 2 * id + 1;
  right = 2 * id + 2;

  if (left < min_heap->size && min_heap->array[left]->distance < min_heap->array[smallest]->distance) {
    smallest = left;
  }
  if (right < min_heap->size && min_heap->array[right]->distance < min_heap->array[smallest]->distance) {
    smallest = right;
  }
  if (smallest != id) {
    //Swap the nodes
    minHeapNode *smallest_node = min_heap->array[smallest];
    minHeapNode *id_node = min_heap->array[id];
    //swap positions
    min_heap->pos[smallest_node->v] = id;
    min_heap->pos[id_node->v] = smallest;
    //swap nodes
    swap_minHeap_node(&min_heap->array[smallest], &min_heap->array[id]);

    min_heapify(min_heap, smallest);
  }
}

//To check if heap is empty or not
int is_Empty(minHeap *min_heap) {
  return min_heap->size == 0;
}

//To extract min node from heap 

minHeapNode* extract_min(minHeap *min_heap){
  if (is_Empty(min_heap))
    return NULL;

  //Storing root node
  minHeapNode *root = min_heap->array[0];
  //Replacing root node with last node
  minHeapNode *last_node = min_heap->array[min_heap->size - 1];
  min_heap->array[0] = last_node;

  //Updating position of last node
  min_heap->pos[root->v] = min_heap->size - 1;
  min_heap->pos[last_node->v] = 0;

  //Reduce heap size and heapify root
  --(min_heap->size);
  min_heapify(min_heap, 0);

  return root;
}

//Function to decrease key distance value of a given vertex v. using pos[] of minheap to get current index of node in min heap
void decrease_key(minHeap *min_heap, int v, int dist) {
  //Getting index of v in heap array
  int i = min_heap->pos[v];
  //Getting the node and updating its value
  min_heap->array[i]->distance = dist;

  //Travel up while the tree is not heapified
  while(i && min_heap->array[i]->distance < min_heap->array[(i - 1) / 2]->distance) {
    min_heap->pos[min_heap->array[i]->v] = (i - 1) / 2;
    min_heap->pos[min_heap->array[(i - 1) / 2]->v] = i;
    swap_minHeap_node(&min_heap->array[i], &min_heap->array[(i - 1) / 2]);
    //move to parent's index
    i = (i - 1) / 2;
  }
}

//To check if given vertex is in min heap or not 
int is_in_minHeap (minHeap *min_heap, int v) {
  if(min_heap->pos[v] < min_heap->size)
    return 1;
  return 0;
}
//Reverse Array -- for printing path
void rvereseArray(int arr[], int start, int end)
{
  int temp;
  while (start < end)
  {
    temp = arr[start];  
    arr[start] = arr[end];
    arr[end] = temp;
    start++;
    end--;
  }  
}

//To print the shortest path
void print_path(int *parent, int start, int end) {
  int *path = (int *)malloc(MAX * sizeof(int));//initialising path array
  for (int i = 0; i < MAX; i++){
    path[i] = 0;
  }
  int i = 0;
  //Adding nodes in the path from start to end
  for(int v = end; v != -1; v = parent[v]){
    path[i] = v;
    i++;
  }
  rvereseArray(path, 0, MAX - 1);//Reversing the array to print in correct order
  for (int j = 0; j < MAX; j++){
    if(path[j] == 0) continue;
    printf("%5d%20s%15s%15s\n", g->data[path[j]]->stop_id, g->data[path[j]]->name, g->data[path[j]]->latitude, g->data[path[j]]->longitude);
  }
  free(path);//freeing the path array
}

void shortest_path(int startNode, int endNode) {
  int V = MAX;
  //For picking minimum weight edge in cut
  int *dist = (int *)malloc(V * sizeof(int));//to keep track of minimum distance
  int *parent = (int *)malloc(V * sizeof(int));//for finding the path between nodes
  int *visited = (int *)malloc(V * sizeof(int));//to check if the node is already visited or not
  minHeap *min_heap = create_minHeap(V);

  //Initialise min heap with all vertices distance till the end node
  for (int v = 0; v < V; v++) {
    parent[v] = -1;
    visited[v] = 0;
    dist[v] = INT_MAX;
    min_heap->array[v] = new_minHeap_node(v, dist[v]);
    min_heap->pos[v] = v;
  }

  //Making the distance value of starting node as 0 -- so that it is extracted first
  min_heap->array[startNode] = new_minHeap_node(startNode, dist[startNode]);
  min_heap->pos[startNode] = startNode;
  dist[startNode] = 0;
  decrease_key(min_heap, startNode, dist[startNode]);

  //Inititally size of min heap is equal to the ending node
  min_heap->size = V;

  //Following loop contains all nodes whose shortest distance is not finalised.

  while(!is_Empty(min_heap)) {

    //Extract the vertex with minimum distance value
    minHeapNode *min_heap_node = extract_min(min_heap);

    int extracted_vertex = min_heap_node->v;
    visited[extracted_vertex] = 1;

    //Traverse through all adjacent vertex of v and update dist value
    if (extracted_vertex == endNode) break;
    adjacencyList *for_traversing = g->list[extracted_vertex];
    while (for_traversing != NULL) {
      int v = for_traversing->vertex;

      //If shortest distance to v is not finalised yet, and distance to v through extracted vertex is less than previously calculated distance. -- only do that if you have not visited the vertex yet.
      if (!visited[v]) {
        if(is_in_minHeap(min_heap, v) && dist[extracted_vertex] != INT_MAX && (for_traversing->weight + dist[extracted_vertex]) < dist[v]) {
          dist[v] = dist[extracted_vertex] + for_traversing->weight;
          parent[v] = extracted_vertex;//the extracted node is the parent of adjacent node
          //updating distance value in min heap also
          decrease_key(min_heap, v, dist[v]);
      }
      }
      for_traversing = for_traversing->next;
    }
  }
  //print path between start and end node
  print_path(parent, startNode, endNode);
  //free excess memory
  free(dist);
  free(parent);
  free(visited);
  for(int i = 0; i < V; i++){
    free(min_heap->array[i]);
  }
  free(min_heap->pos);
  free(min_heap);
}

void free_memory ( ) {//deleting the graph
  for (int i = 0; i < MAX; i++) {
    free(g->data[i]);
    if (g->list[i] == NULL) {
      free(g->list[i]);
      continue;
    }
    adjacencyList *temp;
    while (g->list[i] != NULL){
      temp = g->list[i];
      g->list[i] = g->list[i]->next;
      free(temp);
    }
  }
  free(g->list);
  free(g->data);
  free(g);
}



