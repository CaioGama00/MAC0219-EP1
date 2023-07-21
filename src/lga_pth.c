#include "lga_base.h"
#include "lga_pth.h"
#include <pthread.h>
#include <stdio.h>

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

static void update(byte *grid_in, byte *grid_out, int grid_size, int start_row, int end_row) {
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < grid_size; j++) {
            if (grid_in[ind2d(i,j)] == WALL)
                grid_out[ind2d(i,j)] = WALL;
            else
                grid_out[ind2d(i,j)] = get_next_cell(i, j, grid_in, grid_size);
        }
    }

}

typedef struct {
    byte *grid_1;
    byte *grid_2;
    int grid_size;
    int start_row;
    int end_row;
} ThreadArgs;

void* thread_update(void* args)
{
    ThreadArgs* t = (ThreadArgs*) args;
    update(t->grid_1, t->grid_2, t->grid_size, t->start_row, t->end_row);
    pthread_exit(NULL);
}

static void iteration_pht(byte *grid_1, byte *grid_2, int grid_size, int num_threads) {
    pthread_t threads[num_threads];
    ThreadArgs thread_args[num_threads];
    int rows_per_thread = grid_size / num_threads;
    for (int i = 0; i < num_threads; i++) {
        thread_args[i].grid_1 = grid_1;
        thread_args[i].grid_2 = grid_2;
        thread_args[i].grid_size = grid_size;
        thread_args[i].start_row = i * rows_per_thread;
        thread_args[i].end_row = (i == num_threads-1) ? grid_size : (i+1)*rows_per_thread;
        pthread_create(&threads[i], NULL, thread_update, (void*) &thread_args[i]);
    }
    for (int i = 0; i < num_threads; i++) { pthread_join(threads[i], NULL); }
    for (int i = 0; i < num_threads; i++) {
        thread_args[i].grid_1 = grid_2;
        thread_args[i].grid_2 = grid_1;
        pthread_create(&threads[i], NULL, thread_update, (void*) &thread_args[i]);
    }
    for (int i = 0; i < num_threads; i++) { pthread_join(threads[i], NULL); }
}

void simulate_pth(byte *grid_1, byte *grid_2, int grid_size, int num_threads) {
    for (int i = 0; i < ITERATIONS / 2; i++) {
        iteration_pht(grid_1, grid_2, grid_size, num_threads);
    }
}
