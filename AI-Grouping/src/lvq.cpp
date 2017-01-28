#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define M 5
#define N 0.1
#define EXAMPLES 900
#define SEASONS 900


struct Point{
    float x;
    float y;
    int category;
};

void lvq();
void init_weights();
int find_closest_cluster(Point p);
void update_weights(Point p);
void read_file(char *path);
int randomgenerator(int min,int max);
float calculate_euclithian_distance(float x1,float y1,float x2,float y2);
float calculate_dispersion();
void make_map();
void print_map();

Point array[EXAMPLES];
Point weights[M];
float dispersion[M];
int map[21][21]={0};

float learning_rate;

void lvq(){
    int i,j;
    learning_rate = N;
    init_weights();
    int count = 0;
    for (i = 0; i < SEASONS; i++){
        for (j = 0; j < EXAMPLES; j++){
            update_weights(array[j]);
        }
        learning_rate *= 0.95f;
    }
}

void init_weights(){
    int i;
    for ( i=0 ; i < M ; i++){
        Point point;
        int rand_point = randomgenerator(0,EXAMPLES - 1);
        point.x = array[rand_point].x;
        point.y = array[rand_point].y;
        weights[i] = point;
    }
}

int find_closest_cluster(Point p){
    int winner, i;
    float min_dist = 99999999;
    for (i = 0 ; i < M ; i++){
        float distance = calculate_euclithian_distance(p.x, p.y, weights[i].x, weights[i].y);
        if (distance < min_dist){
            min_dist = distance;
            winner = i;
        }   
    }
    return winner;   
}

void update_weights(Point p){
	int winner = find_closest_cluster(p);
    p.category = winner;
	weights[winner].x += learning_rate*(p.x - weights[winner].x);
	weights[winner].y += learning_rate*(p.y - weights[winner].y);
}

void read_file(char *path){
    FILE *input;
    char buf[100];

    if ((input = fopen(path,"r")) == NULL){
        exit(1);
    }

    int counter, k = 0;

    while(fgets(buf,100,input) != NULL){
        Point point;
        counter = sscanf(buf,"%f\t%f",&point.x,&point.y);
        array[k] = point;
        k++;
    }
    fclose(input);
}

int randomgenerator(int min,int max){
    int r = max + rand() / (RAND_MAX / (min - max + 1) + 1);
    return r;
}

float calculate_euclithian_distance(float x1,float y1,float x2,float y2){
    float diffx = x1 - x2;
    float diffy = y1 - y2;
    float diffx_sqr = pow(diffx,2);
    float diffy_sqr = pow(diffy,2);
    float distance = sqrt (diffx_sqr + diffy_sqr);
    return distance;
}


float calculate_dispersion(){
    float s;
    int i, j;
    float sum[M];

    for ( i=0 ; i<M ; i++ ){
        int size = sizeof(array)/sizeof(Point);
        for ( j=0 ; j<size ; j++ ){
            if (array[j].category == i){
                sum[i] += pow(array[j].x - weights[i].x,2);
                sum[i] += pow(array[j].y - weights[i].y,2);
            }
        }
        dispersion[i] = (float) sum[i]/size;
    }
    return s;
}

void make_map(){
    int row, col, i;

	for(i = 0; i < EXAMPLES; i++){										
		array[i].x = 10*(0.8*array[i].x + 1);
		array[i].y = 10*(0.8*array[i].y + 1);
	}

	for(i = 0; i < M; i++){
		weights[i].x = 10*(0.8*weights[i].x + 1);
		weights[i].y = 10*(0.8*weights[i].y + 1);
	}

	for(i = 0; i < EXAMPLES; i++){															
		row = (int) (array[i].x + 0.5);
		col = (int) (array[i].y + 0.5);
		map[row][col] = 1;
	}

	for(i = 0; i < M; i++){
		row = (int) (weights[i].x + 0.5);
		col = (int) (weights[i].y + 0.5);
		map[row][col] = -1;
	}
}

void print_map(){
    int i, j;

	for(i = 0; i < 21; i++){
		for(j = 0; j < 21; j++){
			if (map[i][j] == 1)
                printf("+ ");
			else if (map[i][j] == -1)
                printf("@ ");
			else
                printf("- ");
		}
		printf("\n");
	}
	printf("|                   |                   |\n");
	printf("-1.25               0                 +1.25\n");
    for (i = 0; i < M; i++){
         printf("Dispersion for cluster %d is %f\n", i+1, dispersion[i]);
    }
}

int main(int argc,char *argv[]){
    if (argc < 2 ){
        printf("Reading from default path \"generated_examples/examples.txt\"\n");
        read_file("generated_examples/examples.txt");
    }else{
         read_file(argv[1]);
    }

    lvq();
    calculate_dispersion();
    make_map();
    print_map();
    return 0;
}

