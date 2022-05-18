#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> 
#include <pthread.h>

#define PLAYERS 5

int results[PLAYERS];
bool out[PLAYERS] = {false};
bool end_game = false;

typedef struct player {
    int thread_id;
} player;

pthread_barrier_t barrier;

void* roll_dice(void * dice);

int main(void) {
    pthread_t ptid[PLAYERS];

    if (pthread_barrier_init(&barrier, NULL, PLAYERS) != 0) {
        perror("pthread_barrier_init\n");
        exit(EXIT_FAILURE);
    }

    player pl[PLAYERS];

    for(int i = 0; i < PLAYERS; i++) {
        pl[i].thread_id = i;
        if (pthread_create(&ptid[i], NULL, roll_dice, &pl[i]) != 0) {
            perror("pthread_create\n");
            exit(EXIT_FAILURE);
        }
    }

    for(int i = 0; i < PLAYERS; i++) {
        if (pthread_join(ptid[i], NULL)) {
            perror("pthread_join\n");
            exit(EXIT_FAILURE);
        }
    }


    if(pthread_barrier_destroy(&barrier) != 0) {
        perror("pthread_barrier_destroy\n");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

void* roll_dice(void* plr_ptr) {
    player* plr = (player* ) plr_ptr;

    while(1) {
       
        results[plr->thread_id] = (rand() % 6) + 1;

        if (pthread_barrier_wait(&barrier) == PTHREAD_BARRIER_SERIAL_THREAD) {
            bool new_loosers[PLAYERS];
            int lowest_result = 7;
            //loop checks who are the losers and storing the information in an tmp bool array 
            for(int i = 0; i < PLAYERS; i++) {
                if (! out[i]) {
                    printf("Player %d rolled a %d\n", i, results[i]);
                    if (results[i] < lowest_result) {
                        for (int j = 0; j < PLAYERS; j ++) {
                            new_loosers[j] = false;
                        }
                        new_loosers[i] = true;
                        lowest_result = results[i];
                    } else if (results[i] == lowest_result) {
                        new_loosers[i] = i;
                    }
                }
                
            }
            //writes the info of the tmp bool array to the global information 
            for (int i = 0; i < PLAYERS; i++) {
                if (new_loosers[i]) {
                    out[i] = true;
                    printf("Eliminating player %d\n", i);
                }
                
            }
            
            
            printf("------------------------------------------------\n");
            //check if there is only one left or all out of the game
            int players_out = 0;
            int winner_id;
            for(int i = 0; i < PLAYERS; i++ ) {
                if (out[i]) {
                    players_out++;
                } else {
                    winner_id = i;
                }
            }
            if (players_out == (PLAYERS -1)) {
                printf("Player %d has won the game!\n", winner_id);
                end_game = true;
            } else if (players_out == PLAYERS) {
                end_game = true;
            }
            
        }
        //wait a second time for synchronization wether the game is over or not
        pthread_barrier_wait(&barrier);
        if (end_game) {
            return NULL;
        }

    }
    return NULL;
}