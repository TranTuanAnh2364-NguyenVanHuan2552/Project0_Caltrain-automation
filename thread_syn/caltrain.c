#include "pintos_thread.h"
#include <stdio.h>

struct station {
	struct condition *cond_train_arrived;
	struct condition *cond_all_seated; // dieu kien tat ca da ngoi vao ghe
	struct lock *lck;
	int waiting_passengers;
	int empty_seats;
	int finding_seat_passengers; // khach da len tau va dang tim cho ngoi
};


void show_station(struct station *station) 
{
	printf("[Tram|Khach dang doi: %d, Khach tim cho ngoi: %d, Ghe trong: %d]\n",
			station->waiting_passengers,station->finding_seat_passengers,station->empty_seats);
}

void station_init(struct station *station)
{
	station->cond_train_arrived = malloc(sizeof(struct condition));
	station->cond_all_seated= malloc(sizeof(struct condition));
	station->lck = malloc(sizeof(struct lock));
	cond_init(station->cond_train_arrived);
	cond_init(station->cond_all_seated);
	lock_init(station->lck);
	station->waiting_passengers = 0;
	station->empty_seats = 0;
	station->finding_seat_passengers = 0;
	printf("da khoi tao ->"); show_station(station);
}


/* This function is invoked when a train arrives at a station and
 * opens its doors. count indicates how many seats are available on 
 * the train. The function must not return until the train is 
 * satisfactorily loaded (all passengers are in their seats, and 
 * either the train is full or all waiting passengers have boarded).
 *
 */
void station_load_train(struct station *station, int count)
{
	lock_acquire(station->lck);
	station->empty_seats = count;
	printf("train is coming: \n");
	printf("train arrives (count: %d)->", count); show_station(station);
	while ((station->waiting_passengers != 0) && (station->empty_seats != 0)) {
		cond_broadcast(station->cond_train_arrived,station->lck);
		cond_wait(station->cond_all_seated,station->lck);
	}

	//all passengers boarded 
	//printf("train left ->"); show_station(station);

	//reset for next train
	station->empty_seats = 0;
	lock_release(station->lck);
}


/* 
 * This function is invoked when a passenger robot arrives in a station. 
 * This function must not return until a train is in the station 
 * (i.e., a call to station_load_train is in progress) and there are 
 * enough free seats on the train for this passenger to sit down. 
 * Once this function returns, the passenger robot will move the 
 * passenger on board the train and into a seat (you do not need to 
 * worry about how this mechanism works). 
 */
 void station_wait_for_train(struct station *station)
{
	lock_acquire(station->lck);
	station->waiting_passengers++;
	printf("Hanh khach den ->"); show_station(station);
	while (station->finding_seat_passengers == station->empty_seats) //wait for train with empty seats space
		cond_wait(station->cond_train_arrived,station->lck);
	station->finding_seat_passengers++;
	station->waiting_passengers--;
	printf("Hanh khach dang len tau ->"); show_station(station);
	lock_release(station->lck);

}


/* This function is called once the passenger is seated to let the 
 * train know that it's on board.
 */
void station_on_board(struct station *station)
{
	lock_acquire(station->lck);
	station->finding_seat_passengers--;
	station->empty_seats--;
	printf("Hanh khach da ngoi"); show_station(station);
	if ((station->empty_seats == 0) || (station->finding_seat_passengers == 0))
		cond_signal(station->cond_all_seated,station->lck);  // tin hieu duoc gui den bien dieu kien cond_all_seated
	
	lock_release(station->lck);
}







