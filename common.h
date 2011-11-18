#ifndef COMMON_HEADER
#define COMMON_HEADER
//export width / height
extern int w;
extern int h;
//export string for debugging purposes
extern char sprintstr[200];
//export coord so all components can have pointers to coords
class coord;
//export all plot constants
extern const int MAX_SCALE;
extern const int MIN_SCALE;
extern const int START_SCALE;
extern const int PLOT_COUNT;
extern const int plot_count;
extern const int plot_size;
extern const int MIN_PLOT_SIZE;
//export pointer to the original plots
//can't be constant because it has to be initalized and there is no easy way to initialize a constant array this complex...
extern coord * * sinplots;
//export glut functions
void redraw();
void doTimerFunc(int delay, void(*f)(int), int arg);
//export debugging functions
void dbgprint(char * msg);
void dbgprint(const char * msg);
#endif
