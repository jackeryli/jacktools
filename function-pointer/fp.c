/*
Some code for redis for learning
*/
#include <stdio.h>
#include <stdlib.h>

typedef void aeFileProc(int a);

typedef struct aeFileEvent {
    aeFileProc *rfileProc;
} aeFileEvent;

void testHandler(int a) {
    printf("%d\n", a);
}

int main() {
    aeFileEvent *event;
    event = calloc(1, sizeof(aeFileEvent));
    
    event->rfileProc = testHandler;

    event->rfileProc(10);

    return 0;
}