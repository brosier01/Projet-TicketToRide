#ifndef __INCLUDE__
#define __INCLUDE__

//Authors: B. ROSIER

#define INFINI 100000 /* Define the infinite (a big number)*/

#include "TicketToRideAPI.h"

/* Define the game  */
typedef struct{
	int nbCities; /* How many cities in the map */
	int nbTracks; /* How many tracks in the map */
	int* Tracks; /* He gonna be use for the Getmap function */
}game_board;


/* Define a player */
typedef struct{
	int nbWagons_available; /* How many wagons has the player */
	int nbCards; /* How many cards has the player */
	int nbObjectives; /* How many objectives has the player */
	int hands_cards[10]; /* This array contains player's cards */
	t_objective objectives[100]; /* This array contains player's objectives */ 
}t_player;


/* Define a party */
typedef struct{
	int player_nb; /* 0 for us and 1 for the enemy */
	t_color visible_cards[5]; /* The array contains the visible cards */
}game;

typedef struct{
	int lenght; /* Represents the length of the track*/
	t_color color; /* Represents the color of the track */
	t_color second_color; /* Represents the second color of the track */
	int free; // 0->free | 1->Bruce | 2->Enemy
}route;

void WhatMove(t_move* move); /* Ask for a move when you play in manual mode */

t_return_code playMove(t_move* move, t_color* lastCard, int* objectifs_now, game* pjeu); /* Play the move we have choice */

int Replay(t_move* move, t_color lastCard); /* Return 0 if the last move don't need a replay and return 1 if the last move need a replay */

void creatingGame(game_board* pGame); /*Initialise the game : connect to the server, recuperate the number of cities, of tracks etc... */

void counting_cards(t_player* BRUCE, t_color cards[4]); /* Count the card at the beginning of the game */

void playerInit(game_board* pGame,t_player* pBRUCE,t_player* enemy, game* pjeu); /* Initialise a player : number of cards, numbers of wagons etc...*/

void displayGame(t_player* pBRUCE,t_player* enemy, game_board* pGame, game* pjeu); /* Print in the terminal the informations of the players */

void RefreshCards(t_move* move, t_player* bruce,route* routes[36][36],int* objectifs_now); /* Refresh the cards of the player when he does a move */

void RefreshEnemyCards(t_move* move,t_player* enemy, route* routes[36][36], game* pjeu); /* Refresh the cards of the player when he does a move */

int distanceMini(int D[36], int Visite[36]); /* Find the minimal distance between two cities for the dijkstra algorithm */

void DIJKSTRA(int city_start,int city_dest, route* routes[36][36], int D[36], int Prec[36], int Tableau[100]); /* Find the minimal way to finish an objective */


#endif