#include "lga_base.h"
#include "lga_omp.h"
#include <pthread.h>

typedef struct {
    byte *grid_in;
    byte *grid_out;
    int grid_size;
    int start;
    int end;
} ThreadArgs;

static byte get_next_cell(int i, int j, byte *grid_in, int grid_size) {
    byte next_cell = EMPTY;

    for (int dir = 0; dir < NUM_DIRECTIONS; dir++) {
        int rev_dir = (dir + NUM_DIRECTIONS/2) % NUM_DIRECTIONS;
        byte rev_dir_mask = 0x01 << rev_dir;

        int di = directions[i%2][dir][0];
        int dj = directions[i%2][dir][1];
        int n_i = i + di;
        int n_j = j + dj;

        if (inbounds(n_i, n_j, grid_size)) {
            if (grid_in[ind2d(n_i,n_j)] == WALL) {
                next_cell |= from_wall_collision(i, j, grid_in, grid_size, dir);
            }
            else if (grid_in[ind2d(n_i, n_j)] & rev_dir_mask) {
                next_cell |= rev_dir_mask;
            }
        }
    }

    return check_particles_collision(next_cell);
}

static void *update_thread(void *arg) {
    ThreadArgs *threadArgs = (ThreadArgs *)arg;
    byte *grid_in = threadArgs->grid_in;
    byte *grid_out = threadArgs->grid_out;
    int grid_size = threadArgs->grid_size;
    int start = threadArgs->start;
    int end = threadArgs->end;

    for (int i = start; i < end; i++) {
        for (int j = 0; j < grid_size; j++) {
            if (grid_in[ind2d(i, j)] == WALL)
                grid_out[ind2d(i, j)] = WALL;
            else
                grid_out[ind2d(i, j)] = get_next_cell(i, j, grid_in, grid_size);
        }
    }

    pthread_exit(NULL);
}

void simulate_pth(byte *grid_1, byte *grid_2, int grid_size, int num_threads) {
    pthread_t threads[num_threads];
    ThreadArgs threadArgs[num_threads];

    int rows_per_thread = grid_size / num_threads;

    for (int i = 0; i < num_threads; i++) {
        int start = i * rows_per_thread;
        int end = (i == num_threads - 1) ? grid_size : start + rows_per_thread;

        threadArgs[i].grid_in = grid_1;
        threadArgs[i].grid_out = grid_2;
        threadArgs[i].grid_size = grid_size;
        threadArgs[i].start = start;
        threadArgs[i].end = end;

        pthread_create(&threads[i], NULL, update_thread, (void *)&threadArgs[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < ITERATIONS / 2 - 1; i++) {
        byte *temp = grid_1;
        grid_1 = grid_2;
        grid_2 = temp;
    }
}
