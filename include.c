#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TicketToRideAPI.h"
#include "include.h"

void creatingGame(game_board* pGame){
	char game_name[20];

		//connectToServer("li1417-56.members.linode.com",1234,"ROBOCOP");

    waitForT2RGame(/*"TRAINING NICE_BOT MAP=USA"*/"TOURNAMENT EISE3", game_name, &(pGame->nbCities), &(pGame->nbTracks)); 
    printf("nbCities : %d | nbTracks : %d\n",pGame->nbCities, pGame->nbTracks);
    printf("Game Name : %s \n", game_name );
    pGame->Tracks=(int*)malloc(5*pGame->nbTracks*sizeof(int));

}

void counting_cards(t_player* BRUCE, t_color cards[4]){
	for(int i=0;i<4;i++){
		BRUCE->hands_cards[cards[i]]=BRUCE->hands_cards[cards[i]]+1;	
	}
}

void playerInit(game_board* pGame, t_player* pBRUCE,t_player* enemy, game* pjeu){
	t_color cards[4];
	pBRUCE->nbWagons_available=45;
	pBRUCE->nbCards=4;
	pBRUCE->nbObjectives=0;
	for(int i=0;i<60;i++){
		pBRUCE->objectives[i].city1=99;
		pBRUCE->objectives[i].city2=99;
		pBRUCE->objectives[i].score=99;
	}

	enemy->nbWagons_available=45;
	enemy->nbCards=4;
	enemy->nbObjectives=0;
	
	for(int i=0;i<10;i++){
		pBRUCE->hands_cards[i]=0;
	}

	pjeu->player_nb=getMap(pGame->Tracks,pjeu->visible_cards,cards);
	counting_cards(pBRUCE, cards);
	
 

}

void displayGame(t_player* pBRUCE,t_player* enemy, game_board* pGame, game* pjeu){
	printf("Mes cartes -> VIOLET : %d\n", pBRUCE->hands_cards[1]);
	printf("              BLANC : %d\n", pBRUCE->hands_cards[2]);
	printf("              BLEU : %d\n", pBRUCE->hands_cards[3]);
	printf("              JAUNE : %d\n", pBRUCE->hands_cards[4]);
	printf("              ORANGE : %d\n", pBRUCE->hands_cards[5]);
	printf("              NOIR : %d\n", pBRUCE->hands_cards[6]);
	printf("              ROUGE : %d\n", pBRUCE->hands_cards[7]);
	printf("              VERT : %d\n", pBRUCE->hands_cards[8]);
	printf("              MULTICOULEUR : %d\n", pBRUCE->hands_cards[9]);

	/*printf("Les cartes visibles : ");
	for(int i=0;i<5;i++){
		printf("%d ", pjeu->visible_cards[i] );
	}
	printf("\n");*/

	printf("Nombre de wagons : %d \n", pBRUCE->nbWagons_available);
	printf("Nombre de cartes dans la main : %d\n",pBRUCE->nbCards);
	printf("Nombre d'objectifs : %d \n\n",pBRUCE->nbObjectives);

}

void WhatMove(t_move* move){
	/* ask for the type */
	printf("Choose a move\n");
	printf("1. claim a route\n");
	printf("2. draw a blind card\n");
	printf("3. draw a card\n");
	printf("4. draw objectives\n");
	printf("5. choose objectives\n");
	scanf("%d", &move->type);

	/* ask for details */
	if (move->type == CLAIM_ROUTE) {
		printf("Give city1, city2, color and nb of locomotives: ");
		scanf("%d %d %d %d", &move->claimRoute.city1, &move->claimRoute.city2, &move->claimRoute.color,
			  &move->claimRoute.nbLocomotives);
	}
	else if (move->type == DRAW_CARD) {
		printf("Give the color: ");
		scanf("%d", &move->drawCard.card);
	}
	else if (move->type == CHOOSE_OBJECTIVES){
		printf("For each objective, give 0 or 1:");
		scanf("%d %d %d", &move->chooseObjectives.chosen[0], &move->chooseObjectives.chosen[1], &move->chooseObjectives.chosen[2]);
	}

}

t_return_code playMove(t_move* move, t_color* lastCard, int* objectifs_now, game* pjeu){
	t_return_code ret;

	switch (move->type) {
		case CLAIM_ROUTE:
			ret = claimRoute(move->claimRoute.city1, move->claimRoute.city2, move->claimRoute.color, move->claimRoute.nbLocomotives);
			*lastCard = NONE;
			break;
		case DRAW_CARD:
			ret = drawCard(move->drawCard.card,pjeu->visible_cards);
			*lastCard = (*lastCard==NONE && move->drawCard.card!= MULTICOLOR) ? move->drawCard.card : NONE;
			break;
		case DRAW_BLIND_CARD:
			ret = drawBlindCard(&move->drawBlindCard.card);
			*lastCard = (*lastCard==NONE) ? move->drawBlindCard.card : NONE;
			break;
		case DRAW_OBJECTIVES:
			ret = drawObjectives(move->drawObjectives.objectives);
			for(int i=0; i<3; i++){
				printf("%d. %d (", i, move->drawObjectives.objectives[i].city1);
				printCity(move->drawObjectives.objectives[i].city1);
				printf(") -> (");
				printCity(move->drawObjectives.objectives[i].city2);
				printf(") %d (%d pts)\n", move->drawObjectives.objectives[i].city2, move->drawObjectives.objectives[i].score);
			}
			for(int i=0; i<3;i++){
				objectifs_now[3*i]=move->drawObjectives.objectives[i].city1;
				objectifs_now[3*i + 1]=move->drawObjectives.objectives[i].city2;
				objectifs_now[3*i + 2]=move->drawObjectives.objectives[i].score;
			}

			*lastCard = NONE;
			break;
		case CHOOSE_OBJECTIVES:
			ret = chooseObjectives(move->chooseObjectives.chosen);
			*lastCard = NONE;
			break;
	}

	return ret;
}

int Replay(t_move* move, t_color lastCard){
	int replay = 0;

	if (move->type == DRAW_OBJECTIVES)
		replay = 1;
	else if (move->type == DRAW_BLIND_CARD && lastCard == NONE)
		replay = 1;
	else if (move->type == DRAW_CARD && lastCard == NONE && move->drawCard.card != MULTICOLOR)
		replay = 1;

	return replay;
}

void RefreshCards(t_move* move, t_player* bruce, route* routes[36][36],int* objectifs_now){
	int increment=0;
	if(move->type==CLAIM_ROUTE){
		bruce->nbCards=bruce->nbCards-routes[move->claimRoute.city1][move->claimRoute.city2]->lenght;
		if (routes[move->claimRoute.city1][move->claimRoute.city2]==NULL){
			routes[move->claimRoute.city2][move->claimRoute.city1]->free=1;
			routes[move->claimRoute.city2][move->claimRoute.city1]->lenght=0;
		}
		else{
			routes[move->claimRoute.city1][move->claimRoute.city2]->free=1;
			routes[move->claimRoute.city1][move->claimRoute.city2]->lenght=0;
		}
	}
	else if(move->type==DRAW_CARD){
		bruce->nbCards=bruce->nbCards+1;
		bruce->hands_cards[move->drawCard.card]=bruce->hands_cards[move->drawCard.card]+1;
	}
	else if(move->type==DRAW_BLIND_CARD){
		bruce->nbCards=bruce->nbCards+1;
		bruce->hands_cards[move->drawBlindCard.card]=bruce->hands_cards[move->drawBlindCard.card]+1;
	}
	else if(move->type==CHOOSE_OBJECTIVES){
		int nb_objectives_past=bruce->nbObjectives;
		for(int i=0;i<3;i++){
			bruce->nbObjectives=bruce->nbObjectives + move->chooseObjectives.chosen[i];
		}
		if(move->chooseObjectives.chosen[0]){
			bruce->objectives[nb_objectives_past].city1=objectifs_now[0];
			bruce->objectives[nb_objectives_past].city2=objectifs_now[1];
			bruce->objectives[nb_objectives_past].score=objectifs_now[2];
			increment=increment+1;
		}

		if(move->chooseObjectives.chosen[1]){
			bruce->objectives[(nb_objectives_past)+increment].city1=objectifs_now[3];
			bruce->objectives[(nb_objectives_past)+increment].city2=objectifs_now[4];
			bruce->objectives[(nb_objectives_past)+increment].score=objectifs_now[5];
			increment=increment+1;
		}

		if(move->chooseObjectives.chosen[2]){
			bruce->objectives[(nb_objectives_past)+increment].city1=objectifs_now[6];
			bruce->objectives[(nb_objectives_past)+increment].city2=objectifs_now[7];
			bruce->objectives[(nb_objectives_past)+increment].score=objectifs_now[8];
		}
		
	}
}

void RefreshEnemyCards(t_move* move, t_player* enemy, route* routes[36][36], game* pjeu){
	if(move->type==CLAIM_ROUTE){
		enemy->nbCards=enemy->nbCards-move->claimRoute.nbLocomotives;
		enemy->nbWagons_available=enemy->nbWagons_available-move->claimRoute.nbLocomotives;
		if (routes[move->claimRoute.city1][move->claimRoute.city2]==NULL){
			routes[move->claimRoute.city2][move->claimRoute.city1]->free=2;
			routes[move->claimRoute.city2][move->claimRoute.city1]->lenght=999;
		}
		else{
			routes[move->claimRoute.city1][move->claimRoute.city2]->free=2;
			routes[move->claimRoute.city1][move->claimRoute.city2]->lenght=999;
		}
	}
	else if(move->type==DRAW_CARD){
		enemy->nbCards=enemy->nbCards+1;
		for(int i=0;i<5;i++){
			pjeu->visible_cards[i]=move->drawCard.faceUp[i];
		}
	}
	else if(move->type==DRAW_BLIND_CARD){
		enemy->nbCards=enemy->nbCards+1;
	}
	else if(move->type==CHOOSE_OBJECTIVES){
		for(int i=0;i<3;i++){
			enemy->nbObjectives=enemy->nbObjectives + move->chooseObjectives.chosen[i];
		}
	}
}


int distanceMini(int D[36], int Visite[36]){
	int min,indice_min=0;
	min=INFINI;

	for(int i=0;i<36;i++){
		if (Visite[i]==0 && D[i]<min){
			min=D[i];
			indice_min=i;
		}
	}
	return indice_min;
}


void DIJKSTRA(int city_start,int city_dest, route* routes[36][36], int D[36], int Prec[36],int Tableau[100]){
	int Visite[36];
	int u=city_start;
	int length,cpt;
	int o=0;

	for(int i=0; i<100; i++){
		Tableau[i]=99;
	}

	for(int i=0;i<36;i++){
		D[i]=INFINI;
		Visite[i]=0;
	}


	D[city_start]=0;

	while(u!=city_dest){
		u=distanceMini(D,Visite);
		Visite[u]=1;

		for(int v=0;v<36;v++){
			if(routes[u][v]!=NULL){
				if(routes[u][v]->free==1){
					length=0;
				}
				else {
					length=routes[u][v]->lenght;
				}
			}
			if (Visite[v]==0 && routes[u][v]!=NULL && routes[u][v]->free !=2 && (D[u] + length) < D[v]){
				D[v]=D[u] + length;
				Prec[v]=u;
			} 
		}
	}

	cpt=city_dest;

	do{
		Tableau[o]=cpt;
		cpt=Prec[cpt];
		o++;
	}while(cpt!=city_start);

	Tableau[o]=city_start;

}




