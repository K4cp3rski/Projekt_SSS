#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
bool tech = false;
/* ********************* Skrócony Opis **************************** */

/* 
Najpierw mamy 3 sekcjie pomocniczych funkcji technicznych.
Kodują one 3 typy struktur:
1. Drzewa
2. Listy
3. Grafy i listy incydencji

Po tych trzech sekcjach jest część właściwa programu, czyli implementacja algorytmu DFS i zaadaptowanie go do stworzenia algorytmu
znajdywania SSS (Silnie Spójnych Składowych). 

Kroki algorytmu:
1. Przeszukujemy graf G w głąb procedurą DFS w celu obliczenia
tablicy f - czasów opuszczenia wierzchołka
2. Obliczamy graf transponowany G_T
3. Przeszukujemy graf G_T wgłąb procedurą DFS z tym, że tym razem w pętli głównej procedury DFS przeszukujemy wierzchołki w porządku malejących wartości poprzednio obliczonego f
4. Wypisujemy wierzchołki drzew lasu przeszukiwania w głąb grafu G_T jako kolejne silnie spójne składowe grafu G.
*/

/* ***************** Drzewa ************************ */
// Struktura drzewa


// definiujemy alias
typedef int key_type;


// definiujemy struktury reprezentujace drzewo
typedef struct node 
{
  key_type key;
  struct node *left;
  struct node *right;
  struct node *up;
} node, *tree;


// tworzenie nowego wierzchołka
tree new_vertex(key_type a)
{
  tree u;
  u=malloc(sizeof(node));
  u->key=a;
  u->left=NULL;
  u->right=NULL;
  u->up=NULL;
  return u;
}


// inicjalizacja drzewa
void init_BST(tree *r)
{
  *r=NULL;
}


// funkcja, ktora dodaje wezel o podanym kluczu
void add_vertex_BST(tree *r, tree v)
{
  tree *current = r;
  tree upper = NULL;

  while (*current != NULL)
  {
    upper = *current;
    if ((*current)->key >= v->key){
      current = &(*current)->left;
    }else{
      current = &(*current)->right;
    }
  }
  *current = v;
  v->up = upper;
}


// Rysowanie drzewa
void spacje(int n)
{
  int i;
  for (i = 1; i <= n; ++i) printf(" ");
}


void rysuj(tree t, int n)
// rysowanie 'malego' drzewa BST
{
  if (t != NULL)
  {
    rysuj(t->right, n + 3);
    spacje(n);
    printf("%d\n", t->key);
    rysuj(t->left, n + 3);
  }
}


/************** Grafy ******************/
// Struktura grafu i list incydencji


typedef enum{white, gray, black} colors;


// struktura wierzcholka w liscie incydencji
typedef struct adj_list_node 
{
  int id;
  struct adj_list_node *next;
} adj_list_node, *adj_list_node_p;


// struktura listy incydencji
typedef struct adj_list 
{
  adj_list_node_p head;
} adj_list, *adj_list_p;


// struktura pozwalajaca przechowywac informacje (dlugosc sciezki od wybranego wierzcholka, poprzednik na sciezce) o wierzcholkach
typedef struct graph_node
{
  colors color;
  int d;
  int f;
  int prev_node;
} graph_node, *graph_node_p;


// struktura reprezentujaca graf
typedef struct graph 
{
  int V;
  graph_node_p info_v;
  adj_list_p array;
} graph, *graph_p;


// funkcja, ktora inicjalizuje graf
graph_p create_graph(int V)
{
  graph_p g = malloc(sizeof(graph_p));
  g->V = V;
  g->info_v = malloc(V * sizeof(graph_node));
  g->array = malloc(V * sizeof(adj_list));
  int i;
  for(i = 0; i < V; i++){
    g->array[i].head = NULL;
  }
  return g;
}


// funkcja, ktora tworzy nowy element, ktory nastepnie bedzie dodawany do odpowiedniej listy
adj_list_node_p new_adj_node(int id)
{
  adj_list_node_p new_node = malloc(sizeof(adj_list_node));
  new_node->id = id;
  new_node->next = NULL;
  return new_node;
}


// procedura, ktora dodaje krawedzie w grafie
void add_edge(graph_p g, int src, int dest)
{
  adj_list_node_p new_node = new_adj_node(dest);
  new_node->next = g->array[src].head;
  g->array[src].head = new_node;
}


// funkcja, ktora drukuje liste incydencji
void print_graph(graph_p g)
{
  int v;
  for(v = 0; v < g->V; v++){
    adj_list_node_p act_list = g->array[v].head;
    printf("Source vertex: %d -> ", v);
    while (act_list){
      printf("%d ", act_list->id);
      act_list = act_list->next;
    }
    printf("\n");
  }
}

// drukowanie listy incydencji wraz z czasami odkrycia i opuszczenia wierzchołków
void print_graph_time(graph_p g)
{
  int v;
  for(v = 0; v < g->V; v++){
    adj_list_node_p act_list = g->array[v].head;
    printf("Source vertex: %d -> ", v);
    while (act_list)
    {
      printf("(%d, d == %d, f == %d) ", act_list->id, g->info_v[v].d, g->info_v[v].f);
      act_list = act_list->next;
    }
    printf("\n");
  }
}

// Transpozycja Grafu
void transpose_graph(graph_p g, graph_p g_t)
{
  int v;
  for (v = 0; v < g->V; v++){
    adj_list_node_p act_list = g->array[v].head;
    while (act_list){
      add_edge(g_t, act_list->id, v);
      act_list = act_list->next;
    }
  }
}


// Wydrukowanie wierzchołków grafu wraz z wartościami prev_node, d, f dla nich
void print_nodes (graph_p g)
{
  int v;
  for (int k = 0; k < g->V; k++)
  {
    printf("Node: %d, prev_node: %d, d: %d, f: %d. \n", k, g->info_v[k].prev_node, g->info_v[k].d,g->info_v[k].f);
  }
}

/************** Listy *********************/


// zdefiniowanie listy
typedef adj_list_node_p list;


// sprawdzamy, czy lista jest pusta
int empty(list q)
{
  return (q==NULL);
}


// usuwamy pierwszy element z listy
void pop_front(list *h)
{
  if (!empty(*h))
  {
    list u;
    u=*h;
    *h=(*h)->next;
    free(u);
  }
}

// Zwraca id pierwszego elementu z listy i go usuwa
int return_front (list *h)
{
  if (!empty(*h))
  {
    list u;
    u=*h;
    *h=(*h)->next;
    int ret = u->id;
    free(u);
    return ret;
  }
  else
  {
    return -1;
  }
}


// Inicjowanie listy
void ini(list *h)
{
  *h=NULL;
}


// Drukowanie listy
void print_list(list h)
{
  if (h==NULL){
    printf("Lista jest pusta");
  }
  while(h!=NULL){
    printf("%d ", h->id);
    h=h->next;
  }
}


// Funkcja sprawdzająca długość listy
int size_list(list h)
{
  int a = 0;
  // tutaj chcemy iterowac po liscie i zliczac elementy
  while (h != NULL){
    a+=1;
    h=h->next;
  }
  return a;
}


// wstawianie wartości na początek listy
void push(list *h, int a)
{
  list u;
  u=malloc(sizeof(adj_list_node));
  u->id=a;
  u->next=*h;
  *h=u;
}


// usuwanie elementu ze srodka listy o podanym kluczu
void remove_from_list(list *h, int u){
  list act = *h;
  list prev = NULL;

  while (act != NULL && act->id != u){
    prev = act;
    act = act->next;
  }
  if (act != NULL){
    if (prev != NULL){
      prev->next = act->next;
      free(act);
    } else {
      pop_front(h);
    }
  }
}

// Sprawdza, czy element o podanym kluczu znajduje sie w liście. tak==1, nie==0
int in_list (list h, int v)
{
  // kopiowanie listy h
  list g;
  ini(&g);
  while (h != NULL){
    push(&g, h->id);
    h=h->next;
  }

  int dl = size_list(h);
  for (int s=0; s < dl+1; s++)
  {
    int el = return_front(&g);
    if (el == v) return 1;
  }
  return 0;
}


// Duplikowanie listy h
list copy_list (list h)
{
  list g;
  ini(&g);
  while (h != NULL){
    push(&g, h->id);
    h=h->next;
  return g;
  }
  return h;
}

// Odwracanie listy. Jednoznaczne z przeczytaniem jej od tyłu
void reverse_list(list *h)
{
  list u;
  list rev_list = NULL;


  while ((*h)->next != NULL)
  {
    u=(*h)->next;
    (*h)->next=rev_list;
    rev_list=*h;
    *h=u;
  }
  (*h)->next=rev_list;
}

/* ********** Implementacja DFS ***********/

void dfs_visit(graph_p g, int u, int *time)
{
  // odwiedzamy wierzcholek u
  g->info_v[u].color = gray;
  (*time)++;
  g->info_v[u].d = *time;

  // przegladanie sasiadow u wraz z ich potomkami
  list act_list;
  act_list = g->array[u].head;
  while (act_list != NULL){
    if (g->info_v[act_list->id].color == white){
      g->info_v[act_list->id].prev_node = u;
      dfs_visit(g, act_list->id, time);
    }
    act_list = act_list->next;
  }

  // opuszczamy wierzcholek u
  g->info_v[u].color = black;
  (*time)++;
  g->info_v[u].f = *time;
}


// implementacja algorytmu dfs
void dfs(graph_p g)
{
  // inicjalizacja
  int time = 0;
  for (int k=0; k < g->V; k++){
    g->info_v[k].color = white;
    g->info_v[k].prev_node = -1;
    g->info_v[k].d = -1;
    g->info_v[k].f = -1;
  }

  // budowa kolejnych drzew lasu przeszukiwania wglab
  for (int k = 0; k < g->V; k++)
    if (g->info_v[k].color == white)
      dfs_visit(g, k, &time);
}

/* ****** Algorytm do szukania SSS ******* */

void sss(graph_p input)
{
  graph_p g = input;
  // Dorabiamy sobie graf transponowany g_T
  graph_p g_T = create_graph(g->V);
  transpose_graph(g, g_T);

  // inicjalizacja grafu g
  int time = 0;
  for (int k=0; k < g->V; k++){
      g->info_v[k].color = white;
      g->info_v[k].prev_node = -1;
      g->info_v[k].d = -1;
      g->info_v[k].f = -1;
  }

  // inicjalizacja grafu g_T
  int time_T = 0;
  for (int k=0; k < g_T->V; k++){
      g_T->info_v[k].color = white;
      g_T->info_v[k].prev_node = -1;
      g_T->info_v[k].d = -1;
      g_T->info_v[k].f = -1;
  }

  dfs(g);

  if (tech == true)
  {
    printf("Po zrobieniu dfs na g\n\n");
    print_nodes(g);
    printf("\n");
  }

    for(int i=0;i<g->V;i++)
  {
    if (g->info_v[i].prev_node == -1)
    {
      if (tech == true) printf("Korzeń to: %d\n", i);
    }
  }

  list times;
  list nodes;
  ini(&times);
  ini(&nodes);

  int max_time = 0;
  int max_time_node = 0;

  for(int i=0;i < g->V; i++)
  {
    if (g->info_v[i].f > max_time)
    {
      max_time = g->info_v[i].f;
      max_time_node = i;
    }
  }

  push(&nodes, max_time_node);
  push(&times, max_time);
  if (tech == true) printf("Maksymalny czas to: %d dla wierzchołka %d\n", max_time, max_time_node);

  // Sortowanie wierzchołków malejąco po czasie opuszczenia
  int czas = 0;
  int val = 0;
  int tmp_max = 0;
  int czas_big = max_time;
  while (size_list(times) != g->V)
  {
    for (int k=0; k < g->V;k++)
      {
        if (g->info_v[k].f < czas_big && g->info_v[k].f > tmp_max && in_list(times, g->info_v[k].f) == 0)
        {
          czas = g->info_v[k].f;
          if (tech == true) printf("czas: %d\n", czas);
          val = k;
          tmp_max = czas;
        }
      else continue;
      }
    push(&times, tmp_max);
    push(&nodes, val);
    czas_big = tmp_max;
    tmp_max = 0;    
  }

  if (tech == true)
  {
    printf("\nCzasy:");
    print_list(times);
    printf("\nWęzły:");
    print_list(nodes);
  }

  reverse_list(&times);
  reverse_list(&nodes);

  if (tech == true)
  {
    printf("\n\nPo odwróceniu\nCzasy:");
    print_list(times);
    printf("\nWęzły:");
    print_list(nodes);
    printf("\n");
  }


  while(!empty(nodes))
  {
    int k = return_front(&nodes);
    // budowe kolejnych drzew lasu przeszukiwania wglab
    if (g_T->info_v[k].color == white)
        dfs_visit(g_T, k, &time_T);
  }

  if (tech == true)
  {
    printf("Po zrobieniu DFS na g_T w kolejności malejącego czasu f\n\n");
    print_nodes(g_T);
    printf("\n\n");
  }

  list ancestors;
  ini(&ancestors);
  for (int i=0; i<g->V;i++)
  {
    push(&ancestors, g->info_v[i].prev_node);
  }

  printf("Oto las silnie spójnych składowych grafu G:\n\n\n");

  if (tech == true)
  {
    printf("Przodkowie: ");
    print_list(ancestors);
    printf("\n");
  }
  int var;

  for(int i=0;i<g->V;i++)
  {
    tree tmp;
    init_BST(&tmp);
    if (g_T->info_v[i].prev_node == -1)
    {
      var += 1;
      add_vertex_BST(&tmp, new_vertex(i));
      if (in_list(ancestors, i) == 0)
      {
        for (int j=0;j < g_T->V;j++)
        {
          if (g_T->info_v[j].d > g_T->info_v[i].d && g_T->info_v[j].f < g_T->info_v[i].f) add_vertex_BST(&tmp, new_vertex(j));
        }
      }
      printf("Drzewo silnie spójnej składowej numer %d dla korzenia %d:\n", var, i);
        rysuj(tmp, 3);
      }
    }
    
}


int main()
{
  // To jest zmienna odpowiadająca za drukowanie technicznych kroków pośrednich, co jest pomocne przy szukaniu błędów.
  // Dla tech=true zacznie się drukować
  // tech = true;


  int V = 10;
  graph_p g = create_graph(V);
  add_edge(g, 0, 0);
  add_edge(g, 1, 2);
  add_edge(g, 2, 1);
  add_edge(g, 2, 5);
  add_edge(g, 2, 6);
  add_edge(g, 2, 3);
  add_edge(g, 3, 6);
  add_edge(g, 4, 3);
  add_edge(g, 4, 8);
  add_edge(g, 5, 1);
  add_edge(g, 6, 3);
  add_edge(g, 7, 6);
  add_edge(g, 7, 3);
  add_edge(g, 7, 4);
  add_edge(g, 7, 8);
  add_edge(g, 8, 7);
  add_edge(g, 9, 9);

  printf("Graf g\n\n");
  print_graph(g);

  printf("\n");
  sss(g);


  return 0;
}
