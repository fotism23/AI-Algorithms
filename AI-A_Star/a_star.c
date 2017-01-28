#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>

#define P                     30
#define N                     20
#define INF                   10000
#define START_POINT           1
#define END_POINT             2
#define BLANK                 0
#define OBSTACLE              -1
#define PATH                  3
#define VERTICAL_MOVE_COST    2
#define HORIZONTAL_MOVE_COST  1

#define ANSI_COLOR_GREEN   "\x1b[32m"

typedef struct came_from_s{
  int x;
  int y;
  int value;
} came_from_t;

typedef struct block_s{
  int x;
  int y;
  int open;
  int closed;
  int type;
  int g_score;
  double f_score;
} block_t;

typedef struct node_s{
  int x;
  int y;
  struct node_s * next;
} node_t;

void create_maze();
void print_maze();
void get_user_input();
void append(node_t * head , int val1 , int val2);
void build_path(node_t * path);
int get_random_value();
int is_open_empty();
int is_goal(int x , int y);
node_t * reconstruct_path(came_from_t came_from[N][N], int current[2]);
int ** get_neighbors(int x , int y);
int * find_lowest_f_score_in_set();
double calculate_euclithian_distance(int x1 , int y1 , int x2 , int y2);

struct timeval tv;
block_t maze[N][N];
int start_point[2];
int first_end_point[2];
int second_end_point[2];
int counter = 0;
time_t t;

//a star algorithm
node_t * a_star(int x1 , int y1 , int x2 , int y2){
  int i , j;
  came_from_t came_from[N][N];
  for (i = 0 ; i < N ; i++){
    for (j = 0 ; j < N ; j++){
      maze[i][j].g_score = INF;
      maze[i][j].f_score = INF;
      came_from[i][j].value = 0;
    }
  }
  maze[start_point[0]][start_point[1]].open = 1;
  maze[x1][y1].g_score = 0;
  maze[x1][y1].f_score = calculate_euclithian_distance(x1 , y1 , x2 , y2);

  int * current = (int *)malloc(2*sizeof(int));

  while (is_open_empty() == 0){
    current = find_lowest_f_score_in_set();
    if (is_goal(current[0] , current[1])) return reconstruct_path(came_from , current);

    maze[current[0]][current[1]].open = 0;
    maze[current[0]][current[1]].closed = 1;

    int ** neighbors = get_neighbors(current[0] , current[1]);

    for(i = 0 ; i < 4 ; i++){
      if (neighbors[i][0] == -1) continue;
      if (maze[neighbors[i][0]][neighbors[i][1]].closed == 1 ) continue;
      int tentative_score = maze[current[0]][current[1]].g_score + neighbors[i][2];
      if (maze[neighbors[i][0]][neighbors[i][1]].open == 0){
        counter++;
        maze[neighbors[i][0]][neighbors[i][1]].open = 1;
      }else if (tentative_score >= maze[neighbors[i][0]][neighbors[i][1]].g_score) continue;
      came_from[neighbors[i][0]][neighbors[i][1]].x = current[0];
      came_from[neighbors[i][0]][neighbors[i][1]].y = current[1];
      came_from[neighbors[i][0]][neighbors[i][1]].value = 1;
      maze[neighbors[i][0]][neighbors[i][1]].g_score = tentative_score;
      maze[neighbors[i][0]][neighbors[i][1]].f_score = tentative_score + calculate_euclithian_distance(neighbors[i][0] , neighbors[i][1] , first_end_point[0] , first_end_point[1]);
    }
  }
  return NULL;
}

//reconstruct_path method for a_star algorithm
node_t * reconstruct_path(came_from_t came_from[N][N], int current[2]){
  int i;
  node_t * total_path;
  total_path = malloc(sizeof(node_t));
  total_path->x = current[0];
  total_path->y = current[1];
  total_path->next = NULL;

  while (1) {
    if (came_from[current[0]][current[1]].value == 1){
      int current_x = current[0];
      int current_y = current[1];
      current[0] = came_from[current_x][current_y].x;
      current[1] = came_from[current_x][current_y].y;
      append(total_path , current_x , current_y);
    }else{
      break;
    }
  }
  return total_path;
}

//finds the block with the lowest f_score
int * find_lowest_f_score_in_set(){
  int i , j , x_index , y_index;
  double min = INF;
  for (i = 0 ; i < N ; i++){
    for (j = 0 ; j < N ; j++){
      if (maze[i][j].f_score < min && maze[i][j].open == 1){
        min = maze[i][j].f_score;
        x_index = i;
        y_index = j;
      }
    }
  }
  static int array[2];
  array[0] = x_index;
  array[1] = y_index;
  return array;
}

//returns block's(x,y) neighbors
int ** get_neighbors(int x , int y){
  int i , j;
  int ** neighbors = (int **)malloc(4*sizeof(int *));
  for(i = 0 ; i < 4 ; i++){
    neighbors[i] = (int *)malloc(3*sizeof(int));
  }
  neighbors[0][0] = x-1;
  neighbors[0][1] = y;
  neighbors[0][2] = VERTICAL_MOVE_COST;
  neighbors[1][0] = x;
  neighbors[1][1] = y-1;
  neighbors[1][2] = HORIZONTAL_MOVE_COST;
  neighbors[2][0] = x+1;
  neighbors[2][1] = y;
  neighbors[2][2] = VERTICAL_MOVE_COST;
  neighbors[3][0] = x;
  neighbors[3][1] = y+1;
  neighbors[3][2] = HORIZONTAL_MOVE_COST;
  for (i = 0 ; i < 4 ; i++){
    if (neighbors[i][0] < 0 || neighbors[i][1] < 0 || maze[neighbors[i][0]][neighbors[i][1]].type == OBSTACLE){
      neighbors[i][0] = -1;
      neighbors[i][1] = -1;
      neighbors[i][2] = -1;
    }
  }
  return neighbors;
}

//creates maze based on the probability for obstacle in range of (0-99)
void create_maze(){
  int i , j, rand_value;
  for (i = 0 ; i < N ; i++){
    for (j = 0 ; j < N ; j++){
      rand_value = get_random_value();
      maze[i][j].open = 0;
      maze[i][j].closed = 0;
      maze[i][j].x = i;
      maze[i][j].y = j;
      maze[i][j].type = rand_value;
    }
  }
}

//returns a random value based on the probability for ostacle in range of (0-99)
int get_random_value(){
  int i , r , rand_index;
  int random_gen[100] = {0};
  for (i = 0 ; i < P ; i ++){
    random_gen[i] = OBSTACLE;
  }

  rand_index = rand() % 100;
  r = random_gen[rand_index];
  return r;
}

//return the euclithian distance between 2 points
double calculate_euclithian_distance(int x1 , int y1 , int x2 , int y2){
  int sqr_x = (int) pow ((x1 - x2) , 2);
  int sqr_y = (int) pow ((y1 - y2) , 2);
  double distance = sqrt(sqr_x + sqr_y);
  return distance;
}

//checks if open set is empty
int is_open_empty(){
  int i , j;
  for (i = 0 ; i < N ; i++){
    for (j = 0 ; j < N ; j++){
      if (maze[i][j].open == 1){
        return 0;
      }
    }
  }
  return 1;
}

//checks if given block (x,y) is a goal
int is_goal(int x , int y){
  if (x == first_end_point[0] && y == first_end_point[1]) return 1;
  else if (x == second_end_point[0] && y == second_end_point[1]) return 1;
  else return 0;
}

//appends one more step on the path
void append(node_t * head , int val1 , int val2){
  node_t * current = head;
  while(current->next != NULL) current = current->next;
  current->next = malloc(sizeof(node_t));
  current->next->x = val1;
  current->next->y = val2;
  current->next->next = NULL;
}

//prints the maze on the screen
void print_maze(){
  int i , j;
  printf("+");
  for (i = 1 ; i < 2*N ; i++){
    printf(" -");
  }
  printf(" +\n");
  for (i = 0 ; i < N - 1 ; i ++){
    printf("| ");
    for (j = 0 ; j < N ; j++){
      if (maze[i][j].type == OBSTACLE) printf("# | ");
      else if (maze[i][j].type == START_POINT) printf("S | ");
      else if (maze[i][j].type == END_POINT) printf("O | ");
      else if (maze[i][j].type == PATH) printf("\x1b[32mX\x1b[0m | ");
      else printf("  | ");
    }
    printf("\n");
    printf("|");
    for (j = 1 ; j < N ; j ++){
      printf(" - +");
    }
    printf(" - |\n");
  }
  for (j = 0 ; j < N ; j++){
    if (maze[N-1][j].type == OBSTACLE) printf("| # ");
    else if (maze[N-1][j].type == START_POINT) printf("S | ");
    else if (maze[N-1][j].type == END_POINT) printf("O | ");
    else if (maze[N-1][j].type == PATH) printf("\x1b[32mX\x1b[0m | ");
    else printf("|   ");
  }
  printf("|\n");
  printf("+");
  for (i = 2 ; i < 2*N ; i++){
    printf(" -");
  }
  printf(" - +\n");
}

//gets user input for starting point and end points cordinates
void get_user_input(){
  int input_x , input_y;
  puts("Please select starting point (x,y)");
  printf("x:");
  scanf("%d" , &input_x);
  printf("y:");
  scanf("%d" , &input_y);

  //add starting point
  maze[input_x][input_y].type = START_POINT;
  start_point[0] = input_x;
  start_point[1] = input_y;
  puts("Please select first ending point (x,y)");
  printf("x:");
  scanf("%d" , &input_x);
  printf("y:");
  scanf("%d" , &input_y);

  //add first ending point
  maze[input_x][input_y].type = END_POINT;
  first_end_point[0] = input_x;
  first_end_point[1] = input_y;
  puts("Please select second ending point (x,y)");
  printf("x:");
  scanf("%d" , &input_x);
  printf("y:");
  scanf("%d" , &input_y);
  //add second ending point
  maze[input_x][input_y].type = END_POINT;
  second_end_point[0] = input_x;
  second_end_point[1] = input_y;
}

//bulds the path from the linked list
void build_path(node_t * head){
  node_t * current = head;
  while (current){
    if (is_goal(current->x , current->y) != 1){
      maze[current->x][current->y].type = PATH;
    }
    current = current->next;
  }
}

int main(){
  gettimeofday(&tv , NULL);
  t = tv.tv_sec;
  srand((unsigned) time(&t));
  create_maze();
  print_maze();
  get_user_input();
  print_maze();
  node_t * path = a_star(start_point[0] , start_point[1] , first_end_point[0] , first_end_point[1]);
  build_path(path);
  print_maze();
  printf("number of extensions : %d\n", counter);
}
