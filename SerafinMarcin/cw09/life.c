// W archiwum załączonym do tego zadania znajdziesz implementację gry w życie Conway-a. Na podstawie tego kodu wykonaj poniższe zadanie:

// Napisz współbieżną wersję gry w życie, gdzie obliczanie stanu planszy jest rozdzielone na wątki. Utwórz n wątków, gdzie n to parametr programu, i każdemu przypisz komórki, za które dany wątek jest odpowiedzialny. Rozdziel prace pomiędzy wątkami maksymalnie równomiernie.

// Każdy z utworzonych wątków powinien obliczać stan komórek, za które jest odpowiedzialny w następnym kroku czasowym. Wątek główny powinien zajmować się tylko zlecaniem prac i wyświetlaniem wyników. Wątki powinny być tworzone tylko raz na początku programu i powinny wykonywać swoją pracę raz na iterację. (sugestia rozwiązania: pause() i pthread_kill())

// Do skompilowania gry potrzebna jest biblioteka ncurses. W niektórych dystrybucjach Linux-a trzeba zainstalować nagłówki tej biblioteki. Np.: apt install ncurses-devel.

// Gra wykorzystuje dwie tablice dla obecnego i następnego kroku czasowego. Każdy wątek zapisuje dane tylko do swoich komórek tablicy następnego kroku, a zatem dostęp do tych tablic nie musi być synchronizowany. 

// Pomiędzy wyświetleniami planszy program czeka 0,5 sekundy na wykonanie obliczeń. W tym zadaniu ignorujemy możliwość wystąpienia sytuacji, gdzie jakiś wątek nie zdążył wykonać obliczeń w wyznaczonym czasie, tj. żadna dodatkowa synchronizacja wątków nie jest wymagana.
#define _XOPEN_SOURCE 700

#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>


#define THREADS 4

typedef struct
{
	char **current_ground;
	char **next_ground;
	int start;
	int end;
} thread_data_t;

// void signal_crash_preventer(int sig){}


void *thread_function(void *arg)
{
	thread_data_t *data = (thread_data_t *)arg;

	while(true){
		pause();

		for(int i=data->start; i<data->end; i++){
			int x = i % GRID_WIDTH;
			int y = i / GRID_WIDTH;
			(*data->next_ground)[y*GRID_WIDTH] = is_alive(y, x, *data->current_ground);			
		}
	}
}




int main()
{
	srand(time(NULL));
	setlocale(LC_CTYPE, "");
	initscr(); // Start curses mode

	char *current_ground = create_grid();
	char *next_ground = create_grid();
	char *tmp;

	pthread_t threads[THREADS];
	thread_data_t thread_data[THREADS];

	for(int i=0; i<THREADS; i++){
		thread_data[i].current_ground = &current_ground;
		thread_data[i].next_ground = &next_ground;
		thread_data[i].start = i * (GRID_WIDTH * GRID_HEIGHT) / THREADS;
		thread_data[i].end = (i+1) * (GRID_WIDTH * GRID_HEIGHT) / THREADS;

		pthread_create(&threads[i], NULL, thread_function, &thread_data[i]);
	}

	init_grid(current_ground);

	while (true)
	{
		draw_grid(current_ground);
		usleep(500 * 1000);

		// Step simulation
		update_grid(current_ground, next_ground);
		tmp = current_ground;
		current_ground = next_ground;
		next_ground = tmp;
	}

	endwin(); // End curses mode
	destroy_grid(current_ground);
	destroy_grid(next_ground);

	return 0;
}
