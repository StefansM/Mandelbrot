#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include <unistd.h>
#include <curses.h>
#include <signal.h>

int is_in_cardioid(float complex z);
int is_in_2bulb(float complex z);
unsigned int mandelbrot(float x, float y);
void interrupt_signal(int);

//Signal flag for interrupts.
volatile sig_atomic_t interrupted = 0;

//There's not much point in having many iterations
unsigned int iterations = 256;
unsigned int width, height;

//The viewport in real coordinates (i.e. not in lines/characters).
float view_x[] = {-2, 1};
float view_y[] = {-1, 1};

int main(int argc, char **argv){
    //Set a flag upon interrupt so that we can clean up properly.
    signal(SIGINT, interrupt_signal);

    //Initialise ncurses, hide the cursor and get width/height.
    initscr();
    curs_set(0);
    getmaxyx(stdscr, height, width);
    //Initialise colour pairs if we can.
    if(has_colors()){
        start_color();
        for(short i=1; i < COLORS; i++){
            init_pair(i, i, COLOR_BLACK);
        }
    }

    //Display a different view 5 times, and then just keep repeating.
    //The screen is not refreshed until "refresh()" is called. Since we
    //know the line width, there is no reason to explicitly reset the 
    //cursor position except when starting to draw.
    //
    //You could zoom further (increase the upper limit on "k") if you want,
    //but if you don't position the viewpoint correctly you just end up seeing
    //a big load of cardioid.
    while(!interrupted){
        for(int k=0; k < 5 && !interrupted; k++){
            move(0,0);
            //Difference from view_x and view_y start points in real coords.
            float delta[] = {
                (float)(view_x[1] - view_x[0]) / width, 
                (float)(view_y[1] - view_y[0]) / height };
            //Draw a "*" if a point is in the set. Otherwise, either draw
            //a prettily coloured dot or an empty space.
            for(int j=0; j<height && !interrupted; j++){
                for(int i=0; i<width && !interrupted; i++){
                    float x = view_x[0] + delta[0] * i;
                    float y = view_y[0] + delta[1] * j;

                    unsigned int its = mandelbrot(x,y);
                    if(its == 0){
                        if(has_colors()){
                            attron(COLOR_PAIR(0));
                        }
                        addch('*');
                        if(has_colors()){
                            attroff(COLOR_PAIR(0));
                        }
                    }else if(has_colors()){
                        //Only display the maximum number of colours.
                        unsigned int colcode = (its > COLORS) ? COLOR_PAIRS-1 : its;
                        attron(COLOR_PAIR(colcode));
                        addch('.');
                        attroff(COLOR_PAIR(colcode));
                    }else{
                        addch(' ');
                    }
                }
            }
            float delta_x = (view_x[1] - view_x[0]) * 0.33;
            for(unsigned int i=0;i<2;i++){
                view_x[i] *= 0.75; 
                view_x[i] -= delta_x;
                view_y[i] *= 0.75;
            }
            refresh();
            sleep(2);
        }
    }
    //Cleanup
    curs_set(1);
    endwin();
    return 0;
}

//Check whether we are in the main cardioid. This allows us to skip the
//bulk of the calls to "mandelbrot()".
int is_in_cardioid(float complex z){
    float q = (creal(z) - 0.25)*(creal(z) - 0.25) + cimag(z)*cimag(z);
    return q*(q + (creal(z) - 0.25)) < 0.25 * cimag(z)*cimag(z);
}

//Similarly with the 2bulb.
int is_in_2bulb(float complex z){
    return (creal(z) + 1)*(creal(z) + 1) + cimag(z)*cimag(z) < 1.0/16;
}

//Return the number of iterations taken for a coordinate to blow up -- that is,
//to increase above 2, at which point it is guaranteed to continue increasing.
//Returns 0 if the point never blows up; i.e. it is in the set.
unsigned int mandelbrot(float x, float y){
    float complex c = x + y*I;
    if(is_in_cardioid(c) || is_in_2bulb(c)){
        return 0;
    }

    //Iterate. Return the number of iterations if z blows up.
    float complex z = 0;
    for(unsigned int i=0; i < iterations; i++){
        if(cabsf(z) > 2){ return i; }
        z = z*z + c;
    }
    return 0;
}

//Set a flag saying that we were interrupted.
void interrupt_signal(int param){
    interrupted = 1;
}
