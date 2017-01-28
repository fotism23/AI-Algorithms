#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define M 5
#define EXAMPLES 900

typedef struct Points {
    float x;
    float y;
    int category;
}Point;

void read_file(char *path);
int randomgenerator(int min,int max);
void add_random_clusters();
float calculate_euclithian_distance(float x1,float y1,float x2,float y2);
void find_closest_cluster();
void kmeans();
void update_cluster_centroid();
void write_output();
float calculate_dispersion();
void make_map();
void print_map();

Point array[EXAMPLES];
Point centroid[M];
float dispersion[M];
int map[21][21]={0};
int i,counter,k,flag;

void kmeans(){
    flag = 1;
    add_random_clusters();
    while(flag == 1){
            flag = 0;
            find_closest_cluster();
            update_cluster_centroid();
    }
}

void add_random_clusters(){
    for ( i=0 ; i < M ; i++){
        Point point;
        int rand_point = randomgenerator(0,EXAMPLES - 1);
        point.x = array[rand_point].x;
        point.y = array[rand_point].y;
        centroid[i] = point;
    }
}

float calculate_euclithian_distance(float x1,float y1,float x2,float y2){
    float diffx = x1 - x2;
    float diffy = y1 - y2;
    float diffx_sqr = pow(diffx,2);
    float diffy_sqr = pow(diffy,2);
    float distance = sqrt (diffx_sqr + diffy_sqr);
    return distance;
}

void find_closest_cluster(){
    int size,j,category;
    for ( i=0 ; i < EXAMPLES ; i++ ){
        float min_dist = 99999999;
        for ( j=0 ; j<M ; j++){
            float distance = calculate_euclithian_distance(array[i].x,array[i].y,centroid[j].x,centroid[j].y);
            if (distance < min_dist){
                min_dist = distance;
                category = j;
            }   
        }
        array[i].category = category;
    }
}

void update_cluster_centroid(){
    float sum_x[M] = {0};
    float sum_y[M] = {0};
    float count[M] = {0};

    for ( i=0 ; i<EXAMPLES ; i++ ){
        sum_x[array[i].category] += array[i].x;
        sum_y[array[i].category] += array[i].y;
        count[array[i].category]++;
    }

    for( i=0; i<M ; i++ ){
        float old_x = centroid[i].x;
        float old_y = centroid[i].y;
        float new_x = (float)sum_x[i]/(float)count[i];
        float new_y = (float)sum_y[i]/(float)count[i];
        if (old_x == new_x && old_y == new_y) flag = 0;
        else{
            flag = 1;
            centroid[i].x = new_x;
            centroid[i].y = new_y;
        } 
    }
}

int randomgenerator(int min,int max){
    int r = max + rand() / (RAND_MAX / (min - max + 1) + 1);
    return r;
}

void read_file(char *path){
    FILE *input;
    char buf[100];

    if ((input = fopen(path,"r")) == NULL){
        exit(1);
    }

    k = 0;

    while(fgets(buf,100,input) != NULL){
        Point point;
        counter = sscanf(buf,"%f\t%f",&point.x,&point.y);
        array[k] = point;
        k++;
    }
    fclose(input);
}

void write_output(){
    int j;

    mkdir("k-means-results", 0777);

    for ( i=0 ; i<M ; i++){

        FILE *output;
		char filename[sizeof "k-means-results/cluster1.txt"];
    	sprintf(filename, "k-means-results/cluster%01d.txt", i+1);
        output = fopen(filename,"w");

        if (output == NULL ){
            perror("open()");
            exit(1);
        }

        for ( j=0 ; j<EXAMPLES ; j++){
            if (array[j].category == i){
                fprintf(output,"%f\t%f\n",array[j].x,array[j].y);
            }
        }
        fclose(output);

    }

    FILE *output = fopen("k-means-results/dispresion.txt","w");

    if (output == NULL){
        perror("open()");
        exit(1);
    }

    for (i=0 ; i < M ; i++){
        fprintf(output,"%f\n",dispersion[i]);
    }
    fclose(output);
}

float calculate_dispersion(){
    float s;
    int j;
    float sum[M];

    for ( i=0 ; i<M ; i++ ){
        int size = sizeof(array)/sizeof(Point);
        for ( j=0 ; j<size ; j++ ){
            if (array[j].category == i){
                sum[i] += pow(array[j].x - centroid[i].x,2);
                sum[i] += pow(array[j].y - centroid[i].y,2);
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
		centroid[i].x = 10*(0.8*centroid[i].x + 1);
		centroid[i].y = 10*(0.8*centroid[i].y + 1);
	}

	for(i = 0; i < EXAMPLES; i++){															
		row = (int) (array[i].x + 0.5);
		col = (int) (array[i].y + 0.5);
		map[row][col] = 1;
	}

	for(i = 0; i < M; i++){
		row = (int) (centroid[i].x + 0.5);
		col = (int) (centroid[i].y + 0.5);
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

    kmeans();
    calculate_dispersion();
    write_output();
    make_map();
    print_map();
    return 0;
}
