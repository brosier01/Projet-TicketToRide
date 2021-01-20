#include <stdio.h>
#include <stdlib.h>
#include "TicketToRideAPI.h"
#include "include.h"

int main() {

	t_return_code retCode;
	int replay = 0;			
	t_move move;			
	t_color lastMove = NONE;

	t_player bruce, enemy;
	game_board Plateau;
	game jeu;

	route* routes[36][36];
	route track[78];
	int objectifs_now[9];
	int D[36];
	int Prec[36];
	int tour=0;
	int it_obj=0;
	int j,flag;
	int CLAIM,DONE,DRAW_VISIBLE,CHOOSE=0;
	int Tableau[100];
	int s,sup;
	int multicolor,FINISH_GAME=0;
	int PIOCHER_OBJ=0;
	
	

	//printf("C'est au joueur %d de jouer !\n",jeu.player_nb );

	connectToServer("li1417-56.members.linode.com",1234,"ROBOCOP");

	while(1){
	printf("DEBUT DE PARTIE !\n");
	creatingGame(&Plateau);
	playerInit(&Plateau, &bruce,& enemy,&jeu);

	replay = 0;	
	lastMove = NONE;
	it_obj=0;
	CHOOSE=0;
	FINISH_GAME=0;
	PIOCHER_OBJ=0;

	// Initialisation of the 2 dimensions array routes
	// It is an array has for type : route
	// He contains pointers of route 
	for(int i=0;i<36;i++){
		for(int j=0;j<36;j++){
			routes[i][j]=NULL;
		}
	}

	for(int i=0;i<5*78;i=i+5){
		routes[Plateau.Tracks[i]][Plateau.Tracks[i+1]]=&track[i/5];
		routes[Plateau.Tracks[i]][Plateau.Tracks[i+1]]->lenght=Plateau.Tracks[i+2];
		routes[Plateau.Tracks[i]][Plateau.Tracks[i+1]]->color=Plateau.Tracks[i+3];
		routes[Plateau.Tracks[i]][Plateau.Tracks[i+1]]->second_color=Plateau.Tracks[i+4];
		routes[Plateau.Tracks[i]][Plateau.Tracks[i+1]]->free=0;

		routes[Plateau.Tracks[i+1]][Plateau.Tracks[i]]=&track[i/5];
		routes[Plateau.Tracks[i+1]][Plateau.Tracks[i]]->lenght=Plateau.Tracks[i+2];
		routes[Plateau.Tracks[i+1]][Plateau.Tracks[i]]->color=Plateau.Tracks[i+3];
		routes[Plateau.Tracks[i+1]][Plateau.Tracks[i]]->second_color=Plateau.Tracks[i+4];
		routes[Plateau.Tracks[i+1]][Plateau.Tracks[i]]->free=0;

	}
	

	tour=0; /*It is the first turn */
	
	do{

		if (!replay){
			printMap();
		}


		
		if (jeu.player_nb == 1){

			retCode = getMove(&move, &replay);
			RefreshEnemyCards(&move, &enemy,routes, &jeu);
			displayGame(&bruce,&enemy, &Plateau, &jeu);

			
		}
		else {
			//Play alone
			printf("%d\n", tour);

			if(tour==0){ // First turn : DRAW OBJECTIVES
			
					move.type=4;
					replay = Replay(&move, lastMove);
					retCode = playMove(&move, &lastMove, objectifs_now, &jeu);
			}
			if(tour==1){ //Second turn : CHOOSE OBJECTIVES
				move.chooseObjectives.chosen[0]=1;
				move.chooseObjectives.chosen[1]=1;
				move.chooseObjectives.chosen[2]=1;
				move.type=5;
				replay = Replay(&move, lastMove);
				retCode = playMove(&move, &lastMove, objectifs_now, &jeu);
				RefreshCards(&move, &bruce, routes, objectifs_now);
				
			}
			if(tour==2){ // Third turn : DRAW OBJECTIVES
			
					move.type=4;
					replay = Replay(&move, lastMove);
					retCode = playMove(&move, &lastMove, objectifs_now,&jeu);
			}
			if(tour==3){ //Fourth turn : CHOOSE OBJECTIVES
				move.chooseObjectives.chosen[0]=1;
				move.chooseObjectives.chosen[1]=0;
				move.chooseObjectives.chosen[2]=0;
				move.type=5;
				replay = Replay(&move, lastMove);
				retCode = playMove(&move, &lastMove, objectifs_now, &jeu);
				RefreshCards(&move, &bruce, routes, objectifs_now);
				printf("TAKE\n");
				
			}
			if(tour > 3) { //Real Game
				
				CLAIM=0;
				flag=0;
				multicolor=0;
				it_obj=0;


				for(int i=0;i<100;i++){
					if(bruce.objectives[i].city1!=99){
						it_obj++;
					}
					else {
						break;
					}
				}

				

				//CHECK IF THE OBJECTIVES ARE DONE 

				DONE=0;

				for(int i=0;i<it_obj;i++){
					DIJKSTRA(bruce.objectives[i].city1,bruce.objectives[i].city2, routes, D, Prec,Tableau);	
					j=0;
					while(Tableau[j]!=99){
						j++;
					}
					for(int k=1;k<j;k++){
						if(routes[Tableau[k-1]][Tableau[k]]!=NULL){
							if(routes[Tableau[k-1]][Tableau[k]]->free!=1){
								DONE++;
							}
							
						}
						else if(routes[Tableau[k]][Tableau[k-1]]!=NULL){
							if(routes[Tableau[k-1]][Tableau[k]]->free!=1){
								DONE++;
							}
						}
					}
				}

				if(DONE==0){
					FINISH_GAME=1;
				}
				else {
					FINISH_GAME=0;
				}

				if(tour>60){
					FINISH_GAME=1;
				}


				//CLAIM IF IT'S POSSIBLE
				
				for(int i=0;i<it_obj;i++){
					DIJKSTRA(bruce.objectives[i].city1,bruce.objectives[i].city2, routes, D, Prec,Tableau);	
					j=0;
					while(Tableau[j]!=99){
						j++;
					}	
					for(int k=1;k<j;k++){
						if(routes[Tableau[k-1]][Tableau[k]]!=NULL && CLAIM==0 && replay==0 ){
							if(routes[Tableau[k-1]][Tableau[k]]->free==0){
								if(routes[Tableau[k-1]][Tableau[k]]->color==9){
									s=1;
									sup=0;
									while(s<10){
										if(bruce.hands_cards[s]>=routes[Tableau[k-1]][Tableau[k]]->lenght){
											sup=1;
											break;
										}
										
										else s++;
									}
									if(sup){
										move.type=1;
										move.claimRoute.city1=Tableau[k-1];
										move.claimRoute.city2=Tableau[k];
										move.claimRoute.color=s;
										move.claimRoute.nbLocomotives=0;
										retCode = playMove(&move, &lastMove, objectifs_now,&jeu);
										CLAIM=1;
										bruce.hands_cards[s]=bruce.hands_cards[s]- routes[Tableau[k-1]][Tableau[k]]->lenght;
										RefreshCards(&move, &bruce, routes, objectifs_now);
									}
								}

								if(((bruce.hands_cards[routes[Tableau[k-1]][Tableau[k]]->color]+bruce.hands_cards[9])>=routes[Tableau[k-1]][Tableau[k]]->lenght) && routes[Tableau[k-1]][Tableau[k]]->color!=9 ){
									move.type=1;
									move.claimRoute.city1=Tableau[k-1];
									move.claimRoute.city2=Tableau[k];
									move.claimRoute.color=routes[Tableau[k-1]][Tableau[k]]->color;
									if(bruce.hands_cards[routes[Tableau[k-1]][Tableau[k]]->color]>= routes[Tableau[k-1]][Tableau[k]]->lenght){
										move.claimRoute.nbLocomotives=0;
										bruce.hands_cards[routes[Tableau[k-1]][Tableau[k]]->color]=bruce.hands_cards[routes[Tableau[k-1]][Tableau[k]]->color]- routes[Tableau[k-1]][Tableau[k]]->lenght;
									}
									else{
										move.claimRoute.nbLocomotives=routes[Tableau[k-1]][Tableau[k]]->lenght-bruce.hands_cards[routes[Tableau[k-1]][Tableau[k]]->color];
										bruce.hands_cards[9]=bruce.hands_cards[9]-move.claimRoute.nbLocomotives;
										bruce.hands_cards[routes[Tableau[k-1]][Tableau[k]]->color]= bruce.hands_cards[routes[Tableau[k-1]][Tableau[k]]->color] - routes[Tableau[k-1]][Tableau[k]]->lenght + move.claimRoute.nbLocomotives;
									}
									retCode = playMove(&move, &lastMove, objectifs_now,&jeu);
									CLAIM=1;
									RefreshCards(&move, &bruce, routes, objectifs_now);
								}
							}
						}
					}
				}	

				//CLAIM A ROUTE WITHOUT OBJECTIVE

				for(int i=0;i<36;i++){
					for(int j=0;j<36;j++){
						if(routes[i][j]!=NULL && CLAIM==0 && replay==0 && FINISH_GAME==1){
							if(routes[i][j]->free==0){
								if(routes[i][j]->color==9){
									s=1;
									sup=0;
									while(s<10){
										if(bruce.hands_cards[s]>=routes[i][j]->lenght){
											sup=1;
											break;
										}
										
										else s++;
									}
									if(sup){
										move.type=1;
										move.claimRoute.city1=i;
										move.claimRoute.city2=j;
										move.claimRoute.color=s;
										move.claimRoute.nbLocomotives=0;
										retCode = playMove(&move, &lastMove, objectifs_now, &jeu);
										CLAIM=1;
										bruce.hands_cards[s]=bruce.hands_cards[s]- routes[i][j]->lenght;
										RefreshCards(&move, &bruce, routes, objectifs_now);
									}
								}

								if(((bruce.hands_cards[routes[i][j]->color]+bruce.hands_cards[9])>=routes[i][j]->lenght) && routes[i][j]->color!=9 ){
									move.type=1;
									move.claimRoute.city1=i;
									move.claimRoute.city2=j;
									move.claimRoute.color=routes[i][j]->color;
									if(bruce.hands_cards[routes[i][j]->color]>= routes[i][j]->lenght){
										move.claimRoute.nbLocomotives=0;
										bruce.hands_cards[routes[i][j]->color]=bruce.hands_cards[routes[i][j]->color]- routes[i][j]->lenght;
									}
									else{
										move.claimRoute.nbLocomotives=routes[i][j]->lenght-bruce.hands_cards[routes[i][j]->color];
										bruce.hands_cards[9]=bruce.hands_cards[9]-move.claimRoute.nbLocomotives;
										bruce.hands_cards[routes[i][j]->color]= bruce.hands_cards[routes[i][j]->color] - routes[i][j]->lenght + move.claimRoute.nbLocomotives;
									}
									retCode = playMove(&move, &lastMove, objectifs_now,&jeu);
									CLAIM=1;
									RefreshCards(&move, &bruce, routes, objectifs_now);
								}
							}
						}
					}
				}


				//DRAW A VISIBLE CARD


				for(int i=0;i<it_obj;i++){
					DIJKSTRA(bruce.objectives[i].city1,bruce.objectives[i].city2, routes, D, Prec,Tableau);	
					j=0;
					while(Tableau[j]!=99){
						j++;
					}	
					for(int k=1;k<j;k++){
						if (routes[Tableau[k]][Tableau[k-1]]!=NULL  && CLAIM==0 && flag==0 ){
							if(routes[Tableau[k]][Tableau[k-1]]->free==0){
								if((jeu.visible_cards[0]==routes[Tableau[k]][Tableau[k-1]]->color || jeu.visible_cards[1]==routes[Tableau[k]][Tableau[k-1]]->color || jeu.visible_cards[2]==routes[Tableau[k]][Tableau[k-1]]->color || jeu.visible_cards[3]==routes[Tableau[k]][Tableau[k-1]]->color || jeu.visible_cards[4]==routes[Tableau[k]][Tableau[k-1]]->color)){
								for(int z=0;z<5;z++){
									if(jeu.visible_cards[z]==routes[Tableau[k]][Tableau[k-1]]->color && routes[Tableau[k]][Tableau[k-1]]->color!=9){
										move.type=3;
										if(routes[Tableau[k]][Tableau[k-1]]->color==9){
											multicolor=1;
										}
										move.drawCard.card=routes[Tableau[k]][Tableau[k-1]]->color;
										retCode = playMove(&move, &lastMove, objectifs_now,&jeu);
										RefreshCards(&move, &bruce, routes, objectifs_now);
										printf("\nDRAW A VISIBLE CARD  -> COLOR : %d !!\n", routes[Tableau[k]][Tableau[k-1]]->color );
										flag=1;
										break;
										
									}
								}
							}
							}
							
						}
					}
				}

					//DRAW A BLIND CARD 

					
					if(CLAIM==0 && flag==0 && replay==0){
						move.type=2;
						retCode = playMove(&move, &lastMove, objectifs_now,&jeu);
						replay=1;
						RefreshCards(&move, &bruce, routes, objectifs_now);
					}
					else if(CLAIM==0 && flag==0 && replay==1){
						move.type=2;
						retCode = playMove(&move, &lastMove, objectifs_now,&jeu);
						replay=0;
						RefreshCards(&move, &bruce, routes, objectifs_now);
					}
					else if(CLAIM==0 && flag==1 && replay==0 && multicolor==0){
						replay=1;
					}
					else if(CLAIM==0 && flag==1 && replay==1 && multicolor==0){
						replay=0;
					}
					else if(CLAIM==0 && flag==1 && replay==0 && multicolor==1){
						replay=0;
					}
					
				}


			tour++;

			
		}

		if (retCode == NORMAL_MOVE && !replay ){
			jeu.player_nb = !jeu.player_nb;
		}
		
	} while (retCode == NORMAL_MOVE);

	free(Plateau.Tracks);
	
	if ((retCode == WINNING_MOVE && jeu.player_nb == 0) || (retCode == LOOSING_MOVE && jeu.player_nb == 1))
		{
			printf("GG à moi !\n");
		}
	else{
		printf("I'm lagging !\n");
	}
	}



	closeConnection();
	return 0;

}
