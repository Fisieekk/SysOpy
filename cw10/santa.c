// Święty Mikołaj śpi w swoim warsztacie na biegunie północnym i może być obudzony tylko sytuacji gdy wszystkie 9 reniferów wróciło z wakacji.

// Należy zaimplementować program, w którym Mikołaj oraz renifery to osobne wątki.

// Zachowania reniferów:
// Są na wakacjach w ciepłych krajach losowy okres czasu (5-10s)
// Wracaja na biegun północny (Komunikat: Renifer: czeka _ reniferów na Mikołaja, ID), jeśli wracający renifer jest dziewiątym reniferem to wybudza Mikołaja (Komunikat: Renifer: wybudzam Mikołaja, ID).
// Dostarczają wraz z Mikołajem zabawki grzecznym dzieciom (i studentom którzy nie spóźniają się z dostarczaniem zestawów) przez (2-4s).   
// Lecą na wakacje.
// Zachowania Mikołaja:

// Śpi.
// Kiedy zostaje wybudzony (Komunikat: Mikołaj: budzę się) to dostarcza wraz z reniferami zabawki (Komunikat: Mikołaj: dostarczam zabawki) (2-4s).
// Wraca do snu (Komunikat: Mikołaj: zasypiam).
// Program należy zaimplementować korzystając z wątków i mechanizmów synchronizacji biblioteki POSIX Threads. Po uruchomieniu programu wątek główny tworzy wątki dla Mikołaja oraz reniferów. Możemy założyć że Mikołaj dostarczy 4 razy prezenty, po czym kończy działanie wszystkich wątków. Do spania Mikołaja powinny być wykorzystane Warunki Sprawdzające (Condition Variables). Użycie odpowiednich mechanizmów ma zagwarantować niedopouszczenie, np. do zdarzeń: Mikołaj śpi chociaż czeka na niego 9 reniferów.


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define REINDEERS 9
#define GIFTS 4

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reindeers = PTHREAD_COND_INITIALIZER;
pthread_cond_t santa = PTHREAD_COND_INITIALIZER;

int reindeers_count = 0;
int gifts_count = 0;

void* santa_func(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        while (reindeers_count < REINDEERS) {
            pthread_cond_wait(&reindeers, &mutex);
        }
        printf("Mikołaj: budzę się\n");
        pthread_cond_broadcast(&santa);
        gifts_count++;
        printf("Mikołaj: dostarczam zabawki\n");
        pthread_mutex_unlock(&mutex);
        sleep(rand() % 3 + 2);
        printf("Mikołaj: zasypiam\n");
        if (gifts_count == GIFTS) {
            break;
        }
    }
    pthread_exit(0);
}

void* reindeer_func(void* arg) {
    int id = *(int*)arg;
    while (1) {
        sleep(rand() % 6 + 5);
        pthread_mutex_lock(&mutex);
        reindeers_count++;
        printf("Renifer: czeka %d reniferów na Mikołaja, %d\n", reindeers_count, id);
        if (reindeers_count == REINDEERS) {
            printf("Renifer: wybudzam Mikołaja, %d\n", id);
            pthread_cond_broadcast(&reindeers);
        }
        pthread_cond_wait(&santa, &mutex);
        reindeers_count--;
        pthread_mutex_unlock(&mutex);
        sleep(rand() % 3 + 2);
    }
    pthread_exit(0);
}

int main() {
    pthread_t santa_thread,reindeer_threads[REINDEERS];
    int reindeer_ids[REINDEERS];
    pthread_create(&santa_thread, NULL, santa_func, NULL);
    for (int i = 0; i < REINDEERS; i++) {
        reindeer_ids[i] = i;
        pthread_create(&reindeer_threads[i], NULL, reindeer_func, &reindeer_ids[i]);
    }
    pthread_join(santa_thread, NULL);
    for (int i = 0; i < REINDEERS; i++) {
        pthread_join(reindeer_threads[i], NULL);
    }

    return 0;
}