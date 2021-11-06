#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "poker.h"

#define BIT(n) (1UL << (n))
#define BIT_SET(v, n) (v |= BIT(n))
unsigned int special_case = 0;
/* Global instances of the two players */
struct player P1, P2;

/* Parse card from a 2 char input */
struct card parse(const char *card)
{
	struct card c;
	switch(card[0]) {
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9': c.val = card[0] - '0'; break;
	case 'T': c.val = TEN; break;
	case 'J': c.val = JACK; break;
	case 'Q': c.val = QUEEN; break;
	case 'K': c.val = KING; break;
	case 'A': c.val = ACE; break;
	default: break;
	}
	switch(card[1]) {
	case 'S': c.suit = SPADE; break;
	case 'D': c.suit = DIAMOND; break;
	case 'C': c.suit = CLUB; break;
	case 'H': c.suit = HEART; break;
	default: break;
	}
	return c;
}

/* Count the number of occurrences of each card 2 through Ace */
void count_cards(struct hand *h)
{
	/* TASK 6: For each card in the hand, increment the card_count for that card. */
	/*ref_count_cards(h);*/
	int i;
	for(i = 0; i < 5; i++){
		enum Value v = h->cards[i].val;
		/*13 cards, smallest value 2, to find indices -> v-2*/
		h->card_count[v-2] += 1;
		/*printf("count cards of the hand %d, i = %d\n", h->card_count[v-2], v-2);*/
	}
}

int is_flush(struct hand *h)
{
	/* TASK 9: Implement is_flush(). A hand is a flush if all the cards are of the same suit */
	/* Return 1 if flush, 0 otherwise  
	return ref_is_flush(h); */
	int i;
	enum Suit s = h->cards[0].suit;
	for(i = 1; i < 5; i++){
		if(h->cards[i].suit != s){
			return 0;
		}
	}
	/*printf("It is flush\n");*/
	return 1;
}

int is_straight(struct hand *h)
{
	/* By this stage, the cards are counted */
	/* TASK 7: Implement is_straight(). 
	   A hand is considered a straight if the number of occurrences of 5 consecutive cards is 1. 
	   If a straight is found, return 1. 
	 return ref_is_straight(h); */
	int i;
	int count = 0;
	for( i = 0; i < 13; i++){
		if(h->card_count[i] == 1){
			count++;
			int j;
			for(j = i + 1; j < 13; j++){
				if(h->card_count[j] == 1){
					count++;
					if(count == 5){
						/*printf("It is straight\n");*/
						return 1;
					}
				}
				else{
					break;
				}	
			}
		}count = 0;
	}
	/* TASK 8: handle special case A2345 */
	/* If a straight is not found, return 0 */
	if(h->card_count[12] == 1 && h->card_count[0] == 1 && h->card_count[1] == 1 && h->card_count[2] == 1 && h->card_count[3] == 1 ){
		special_case = 1;
		return 1;
	}
	return 0;
}

/* unsigned long set_bit(unsigned long vector, unsigned long n){
	vector |= 1UL << n;
	return vector;
} */

/* This important function converts a player's hand into weighted unsigned long number. Larger the number, stronger the hand. 
It is a bit vector as shown below (2 is the LSB and StraightFlush is the MSB) */
/* 2, 3, 4, 5, 6, 7, 8, 9, T, J, Q, K, A, 
22, 33, 44, 55, 66, 77, 88, 99, TT, JJ, QQ, KK, AA,
222, 333, 444, 555, 666, 777, 888, 999, TTT, JJJ, QQQ, KKK, AAA, 
Straight, Flush, Full House, 
2222, 3333, 4444, 5555, 6666, 7777, 8888, 9999, TTTT, JJJJ, QQQQ, KKKK, AAAA, 
StraightFlush */
/* The number of occurrences of each number in the hand is first calculated in count_cards(). 
Then, depending on the number of occurrences, the corresponding bit in the bit-vector is set. 
In order to find the winner, a simple comparison of the bit vectors (i.e., unsigned long integers) will suffice! */
void eval_strength(struct hand *h)
{
	/* TASK 5: Implement the eval_strength function */
	/*ref_eval_strength(h); */
	unsigned long vector = 0; 
	int i;
	unsigned int full_house_check1 = 0;
	unsigned int full_house_check2 = 0;
	count_cards(h);
	if(is_straight(h)){
		BIT_SET(vector, 40);
	}
	if(is_flush(h)){
		BIT_SET(vector, 41);
	}
	if(is_flush(h) && is_straight(h)){
		BIT_SET(vector, 56);
	}
	for(i = 0; i < 13; i++){
		int freq = h->card_count[i];
		if(freq == 1){
			if(special_case && i == 12){
				BIT_SET(vector, 0);
				special_case = 0;
			}
			else{
				BIT_SET(vector, i);
			}
		}
		else if(freq == 2){
			BIT_SET(vector, (i + 13));
			full_house_check1 = 1;
		}
		else if(freq == 3){
			BIT_SET(vector, (i + 13*2));
			full_house_check2 = 1;
		}
		else if(freq == 4){
			BIT_SET(vector, (i + 43));
		}
	}
	if(full_house_check1 && full_house_check2){
		BIT_SET(vector, 42); 
	}
	h->vector = vector;
	/* printf("Strength of the hand %ld\n", h->vector); */

}

void eval_players_best_hand(struct player *p)
{
	/* TASK 10: For each hand in the 'hands' array of the player, use eval_strength to evaluate the strength of the hand */
	/*       Point best_hand to the strongest hand. */
	/*       HINT: eval_strength will set the hands vector according to its strength. Larger the vector, stronger the hand. 
	ref_eval_players_best_hand(p);*/
	unsigned long max = 0;
	struct hand* best = NULL;
	int i;
	for(i = 0; i < 21; i++){
		eval_strength(&p->hands[i]);
		if(p->hands[i].vector > max){
			max = p->hands[i].vector;
			best = &p->hands[i];
		}
	}
	p->best_hand = best;
}

void copy_card(struct card *dst, struct card *src)
{
	/* TASK 3: Implement function copy_card that copies a card structure from src to dst. */
	/*ref_copy_card(dst, src);*/
	memcpy(dst, src, sizeof(struct card));
}

/* struct card comb_Util(struct card* all_cards, struct card* temp, int start, int end, int index, int r){
	if(index == r){

	}
} */

void initialize_player(struct player *p, struct card *player_cards, struct card *community_cards)
{
	memset(p, 0, sizeof(struct player));

	/* There are 21 possible 5 card combinations (i.e., 7C5) in Pc1 Pc2 Cc1 Cc2 Cc3 Cc4 Cc5 */
	/* TASK 4: Initialize the 'hands' array in the player structure with the cards. 
	 *       Each element of the hands array will contain one possible combination of the hand.
	 *       Use copy_card function.
	 */ 
	/* 1:  Pc1 Pc2 Cc1 Cc2 Cc3 */
	/* 2:  Pc1 Pc2 Cc1 Cc2 Cc4 */
	/* 3:  Pc1 Pc2 Cc1 Cc2 Cc5 */
	/* ... and so on. */
	/*ref_initialize_player(p, player_cards, community_cards); */

/* 	struct card all_cards[7];
	struct card temp[5];
	comb_Util(&all_cards, temp, 0, 6, 0, 5); */
	
	copy_card(&(p->hands[0].cards[0]), &player_cards[0]);
	copy_card(&(p->hands[0].cards[1]), &player_cards[1]);
	copy_card(&(p->hands[0].cards[2]), &community_cards[0]);
	copy_card(&(p->hands[0].cards[3]), &community_cards[1]);
	copy_card(&(p->hands[0].cards[4]), &community_cards[2]);

	copy_card(&(p->hands[1].cards[0]), &player_cards[0]);
	copy_card(&(p->hands[1].cards[1]), &player_cards[1]);
	copy_card(&(p->hands[1].cards[2]), &community_cards[0]);
	copy_card(&(p->hands[1].cards[3]), &community_cards[1]);
	copy_card(&(p->hands[1].cards[4]), &community_cards[3]);

	copy_card(&(p->hands[2].cards[0]), &player_cards[0]);
	copy_card(&(p->hands[2].cards[1]), &player_cards[1]);
	copy_card(&(p->hands[2].cards[2]), &community_cards[0]);
	copy_card(&(p->hands[2].cards[3]), &community_cards[1]);
	copy_card(&(p->hands[2].cards[4]), &community_cards[4]);

	copy_card(&(p->hands[3].cards[0]), &player_cards[0]);
	copy_card(&(p->hands[3].cards[1]), &player_cards[1]);
	copy_card(&(p->hands[3].cards[2]), &community_cards[0]);
	copy_card(&(p->hands[3].cards[3]), &community_cards[2]);
	copy_card(&(p->hands[3].cards[4]), &community_cards[3]);

	copy_card(&(p->hands[4].cards[0]), &player_cards[0]);
	copy_card(&(p->hands[4].cards[1]), &player_cards[1]);
	copy_card(&(p->hands[4].cards[2]), &community_cards[0]);
	copy_card(&(p->hands[4].cards[3]), &community_cards[2]);
	copy_card(&(p->hands[4].cards[4]), &community_cards[4]);

	copy_card(&(p->hands[5].cards[0]), &player_cards[0]);
	copy_card(&(p->hands[5].cards[1]), &player_cards[1]);
	copy_card(&(p->hands[5].cards[2]), &community_cards[0]);
	copy_card(&(p->hands[5].cards[3]), &community_cards[3]);
	copy_card(&(p->hands[5].cards[4]), &community_cards[4]);

	copy_card(&(p->hands[6].cards[0]), &player_cards[0]);
	copy_card(&(p->hands[6].cards[1]), &player_cards[1]);
	copy_card(&(p->hands[6].cards[2]), &community_cards[1]);
	copy_card(&(p->hands[6].cards[3]), &community_cards[2]);
	copy_card(&(p->hands[6].cards[4]), &community_cards[3]);
	
	copy_card(&(p->hands[7].cards[0]), &player_cards[0]);
	copy_card(&(p->hands[7].cards[1]), &player_cards[1]);
	copy_card(&(p->hands[7].cards[2]), &community_cards[1]);
	copy_card(&(p->hands[7].cards[3]), &community_cards[2]);
	copy_card(&(p->hands[7].cards[4]), &community_cards[4]);

	copy_card(&(p->hands[8].cards[0]), &player_cards[0]);
	copy_card(&(p->hands[8].cards[1]), &player_cards[1]);
	copy_card(&(p->hands[8].cards[2]), &community_cards[1]);
	copy_card(&(p->hands[8].cards[3]), &community_cards[3]);
	copy_card(&(p->hands[8].cards[4]), &community_cards[4]);

	copy_card(&(p->hands[9].cards[0]), &player_cards[0]);
	copy_card(&(p->hands[9].cards[1]), &player_cards[1]);
	copy_card(&(p->hands[9].cards[2]), &community_cards[2]);
	copy_card(&(p->hands[9].cards[3]), &community_cards[3]);
	copy_card(&(p->hands[9].cards[4]), &community_cards[4]);

	copy_card(&(p->hands[10].cards[0]), &player_cards[0]);
	copy_card(&(p->hands[10].cards[1]), &community_cards[0]);
	copy_card(&(p->hands[10].cards[2]), &community_cards[1]);
	copy_card(&(p->hands[10].cards[3]), &community_cards[2]);
	copy_card(&(p->hands[10].cards[4]), &community_cards[3]);

	copy_card(&(p->hands[11].cards[0]), &player_cards[0]);
	copy_card(&(p->hands[11].cards[1]), &community_cards[0]);
	copy_card(&(p->hands[11].cards[2]), &community_cards[1]);
	copy_card(&(p->hands[11].cards[3]), &community_cards[2]);
	copy_card(&(p->hands[11].cards[4]), &community_cards[4]);

	copy_card(&(p->hands[12].cards[0]), &player_cards[0]);
	copy_card(&(p->hands[12].cards[1]), &community_cards[0]);
	copy_card(&(p->hands[12].cards[2]), &community_cards[2]);
	copy_card(&(p->hands[12].cards[3]), &community_cards[3]);
	copy_card(&(p->hands[12].cards[4]), &community_cards[4]);

	copy_card(&(p->hands[13].cards[0]), &player_cards[0]);
	copy_card(&(p->hands[13].cards[1]), &community_cards[1]);
	copy_card(&(p->hands[13].cards[2]), &community_cards[2]);
	copy_card(&(p->hands[13].cards[3]), &community_cards[3]);
	copy_card(&(p->hands[13].cards[4]), &community_cards[4]);

	copy_card(&(p->hands[14].cards[0]), &player_cards[0]);
	copy_card(&(p->hands[14].cards[1]), &community_cards[0]);
	copy_card(&(p->hands[14].cards[2]), &community_cards[1]);
	copy_card(&(p->hands[14].cards[3]), &community_cards[3]);
	copy_card(&(p->hands[14].cards[4]), &community_cards[4]);

	copy_card(&(p->hands[15].cards[0]), &player_cards[1]);
	copy_card(&(p->hands[15].cards[1]), &community_cards[0]);
	copy_card(&(p->hands[15].cards[2]), &community_cards[1]);
	copy_card(&(p->hands[15].cards[3]), &community_cards[2]);
	copy_card(&(p->hands[15].cards[4]), &community_cards[3]);

	copy_card(&(p->hands[16].cards[0]), &player_cards[1]);
	copy_card(&(p->hands[16].cards[1]), &community_cards[0]);
	copy_card(&(p->hands[16].cards[2]), &community_cards[1]);
	copy_card(&(p->hands[16].cards[3]), &community_cards[2]);
	copy_card(&(p->hands[16].cards[4]), &community_cards[4]);

	copy_card(&(p->hands[17].cards[0]), &player_cards[1]);
	copy_card(&(p->hands[17].cards[1]), &community_cards[0]);
	copy_card(&(p->hands[17].cards[2]), &community_cards[2]);
	copy_card(&(p->hands[17].cards[3]), &community_cards[3]);
	copy_card(&(p->hands[17].cards[4]), &community_cards[4]);

	copy_card(&(p->hands[18].cards[0]), &player_cards[1]);
	copy_card(&(p->hands[18].cards[1]), &community_cards[1]);
	copy_card(&(p->hands[18].cards[2]), &community_cards[2]);
	copy_card(&(p->hands[18].cards[3]), &community_cards[3]);
	copy_card(&(p->hands[18].cards[4]), &community_cards[4]);

	copy_card(&(p->hands[19].cards[0]), &player_cards[1]);
	copy_card(&(p->hands[19].cards[1]), &community_cards[0]);
	copy_card(&(p->hands[19].cards[2]), &community_cards[1]);
	copy_card(&(p->hands[19].cards[3]), &community_cards[3]);
	copy_card(&(p->hands[19].cards[4]), &community_cards[4]);

	copy_card(&(p->hands[20].cards[0]), &community_cards[0]);
	copy_card(&(p->hands[20].cards[1]), &community_cards[1]);
	copy_card(&(p->hands[20].cards[2]), &community_cards[2]);
	copy_card(&(p->hands[20].cards[3]), &community_cards[3]);
	copy_card(&(p->hands[20].cards[4]), &community_cards[4]);

	/*printf("%d %d\n", p->hands[0]->cards[0].val, p->hands[0]->cards[0].suit);*/
}

/* Parse each hand in the input file, evaluate the strengths of hands and identify a winner by comparing the weighted vectors */
void process_input(FILE *fp)
{
	char p1[2][3];
	char p2[2][3];
	char comm[5][3];
	struct card p1_cards[2], p2_cards[2], community_cards[5];
	int i;

	while(fscanf(fp, "%s %s %s %s %s %s %s %s %s", 
		&p1[0][0], &p1[1][0], &p2[0][0], &p2[1][0], &comm[0][0], 
		&comm[1][0], &comm[2][0], &comm[3][0], &comm[4][0]) == 9) {

		memset(p1_cards, 0, sizeof(struct card) * 2);
		memset(p2_cards, 0, sizeof(struct card) * 2); 
		memset(community_cards, 0, sizeof(struct card) * 5);

		for(i = 0; i < 2; i++) {
			p1_cards[i] = parse(&p1[i][0]);
			p2_cards[i] = parse(&p2[i][0]);
		}

		for(i = 0; i < 5; i++) 
			community_cards[i] = parse(&comm[i][0]);

		initialize_player(&P1, &p1_cards[0], &community_cards[0]);
		initialize_player(&P2, &p2_cards[0], &community_cards[0]);

 		/* count_cards(P1.hands[10]);
		count_cards(P2.hands[10]);  
		eval_strength(P1.hands[0]);
		eval_strength(P2.hands[0]); */

		eval_players_best_hand(&P1);
		eval_players_best_hand(&P2);  

		/* TASK 11: Check which player has the strongest hand and print either "Player 1 wins" or "Player 2 wins" */
		/*printf("p1 best hand %ld, p2 best hand %ld\n",P1.best_hand->vector, P2.best_hand->vector);*/
		if(P1.best_hand->vector > P2.best_hand->vector){
			printf("Player 1 wins\n");
		}
		if(P2.best_hand->vector > P1.best_hand->vector){
			printf("Player 2 wins\n");
		}
		if(P2.best_hand->vector == P1.best_hand->vector){
			printf("No single winner\n");
		}  
	}
}

int main(int argc, char *argv[])
{
	FILE *fp;

	if(argc != 2 || (fp = fopen(argv[1], "r")) == NULL) {
		printf("Unable to open input file\n");
		exit(-1);
	}
	
	process_input(fp);
	fclose(fp);

	return 0;
}
	
