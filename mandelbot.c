#define _POSIX_C_SOURCE 2
#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include <unistd.h>

#define COLOUR_256 2
#define COLOUR_8   1
#define NO_COLOUR  0

unsigned int iterations = 256;
unsigned int width, height;

float view_x[] = {-2, 1};
float view_y[] = {-1, 1};

int is_in_cardioid(float complex z){
	float q = (creal(z) - 0.25)*(creal(z) - 0.25) + cimag(z)*cimag(z);
	return q*(q + (creal(z) - 0.25)) < 0.25 * cimag(z)*cimag(z);
}

int is_in_2bulb(float complex z){
	return (creal(z) + 1)*(creal(z) + 1) + cimag(z)*cimag(z) < 1.0/16;
}

unsigned int mandelbrot(float x, float y){
	float complex c = x + y*I;
	if(is_in_cardioid(c) || is_in_2bulb(c)){
		return 0;
	}
	

	float complex z = 0;
	for(unsigned int i=0; i < iterations; i++){
		if(cabsf(z) > 2){ return i; }
		z = z*z + c;
	}
	return 0;
}

int main(int argc, char **argv){
	system("tput smcup");
	system("tput reset");
	system("tput civis");

	FILE* proc;

	proc = popen("tput cols", "r");
	fscanf(proc, "%u", &width);
	pclose(proc);

	proc = popen("tput lines", "r");
	fscanf(proc, "%u", &height);
	pclose(proc);


	int colour = COLOUR_256;
	for(int k=0; k < 10; k++){
		system("tput cup 0 0");
		float delta[] = {
			(float)(view_x[1] - view_x[0]) / width, 
			(float)(view_y[1] - view_y[0]) / height
		};
		for(int j=0; j<height; j++){
			for(int i=0; i<width; i++){
				float x = view_x[0] + delta[0] * i;
				float y = view_y[0] + delta[1] * j;

				unsigned int its = mandelbrot(x,y);
				if(its == 0){
					printf("\033[1;0m*");
				}else if(colour == COLOUR_256){
					unsigned int colcode = (its > 255) ? 255 : its;
					printf("\033[38;5;%dm.", colcode);
				}else if(colour == COLOUR_8){
					unsigned int colcode = (its > 7) ? 7 : its;
					printf("\033[1;3%dm.", colcode);
				}else{
					printf(" ");
				}
			}
		}
		float delta_x = (view_x[1] - view_x[0]) * 0.33;
		for(unsigned int i=0;i<2;i++){
			view_x[i] *= 0.75; 
			view_x[i] -= delta_x;
			view_y[i] *= 0.75;
		}
		sleep(2);
	}
	//system("tput rmcup");
	system("tput cnorm");
	return 0;
}

