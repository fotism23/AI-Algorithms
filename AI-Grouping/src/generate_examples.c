#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct Points {
    float x;
    float y;
}Point;

float rand_range(float min_n, float max_n)
{
    float r = (float)rand()/(float)RAND_MAX;
    int l = rand() % (1 + 1 + 1) - 1;
    if (l == 0){
        return -(r);
    }else{
        return r;
    }
}

int main(){
    int i;
    FILE *output;

    mkdir("generated_examples", 0777);

    if ((output = fopen("generated_examples/examples.txt","w"))== NULL){
        exit(1);
    }

     Point point;

    for (i=0; i < 100; i++){
        point.x = rand_range(-0.4,0.4);
        point.y = rand_range(-0.4,0.4);
        fprintf(output,"%f\t%f\n",point.x,point.y);
    }
    for (i=100; i<200; i++){
        point.x = rand_range(-0.4,0.4);
        point.y = rand_range(0.6,1.4);
        fprintf(output,"%f\t%f\n",point.x,point.y);
    }
    for (i = 200; i < 300; i++){
        point.x = rand_range(-0.4,0.4);
        point.y = rand_range(-1.4,-0.6);
        fprintf(output,"%f\t%f\n",point.x,point.y);
    }
    for (i = 300; i < 400; i++){
        point.x = rand_range(-1.4, -0.6);
        point.y = rand_range(-0.4,0.4);
        fprintf(output,"%f\t%f\n",point.x,point.y);
    }
    for (i = 400; i < 500; i++){
        point.x = rand_range(-1.4,-0.6);
        point.y = rand_range(0.6,1.4);
        fprintf(output,"%f\t%f\n",point.x,point.y);
    }
    for (i = 500; i < 600; i++){
        point.x = rand_range(-1.4, -0.6);
        point.y = rand_range(-1.4, -0.6);
        fprintf(output,"%f\t%f\n",point.x,point.y);
    }
    for (i = 600; i < 700; i++){
        point.x = rand_range(0.6, 1.4);
        point.y = rand_range(-0.4, 0.4);
        fprintf(output,"%f\t%f\n",point.x,point.y);
    }
    for (i = 700; i < 800; i++){
        point.x = rand_range(0.6, 1.4);
        point.y = rand_range(-1.4, -0.6);
        fprintf(output,"%f\t%f\n",point.x,point.y);
    }
    for (i = 800; i < 900; i++){
        point.x = rand_range(0.6, 1.4);
        point.y = rand_range(0.6, 1.4);
        fprintf(output,"%f\t%f\n",point.x,point.y);
    }
    return 0;
}