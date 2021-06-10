#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "poker.h"

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
	ref_count_cards(h); 
}

int is_flush(struct hand *h)
{
	/* TASK 9: Implement is_flush(). A hand is a flush if all the cards are of the same suit */
	/* Return 1 if flush, 0 otherwise */
	
	return ref_is_flush(h); 

}

int is_straight(struct hand *h)
{
	/* By this stage, the cards are counted */
	/* TASK 7: Implement is_straight(). 
	   A hand is considered a straight if the number of occurrences of 5 consecutive cards is 1. 
	   If a straight is found, return 1. 
	 */

	/* TASK 8: handle special case A2345 */
	
	/* If a straight is not found, return 0 */

	return ref_is_straight(h); 

}
		
/* This important function converts a player's hand into weighted unsigned long number. Larger the number, stronger the hand. 
It is a bit vector as shown below (2 is the LSB and StraightFlush is the MSB) */
/* 2, 3, 4, 5, 6, 7, 8, 9, T, J, Q, K, A, 22, 33, 44, 55, 66, 77, 88, 99, TT, JJ, QQ, KK, AA,
222, 333, 444, 555, 666, 777, 888, 999, TTT, JJJ, QQQ, KKK, AAA, Straight, Flush, Full House, 2222, 3333, 
4444, 5555, 6666, 7777, 8888, 9999, TTTT, JJJJ, QQQQ, KKKK, AAAA, StraightFlush */
/* The number of occurrences of each number in the hand is first calculated in count_cards(). 
Then, depending on the number of occurrences, the corresponding bit in the bit-vector is set. 
In order to find the winner, a simple comparison of the bit vectors (i.e., unsigned long integers) will suffice! */
void eval_strength(struct hand *h)
{
	/* TASK 5: Implement the eval_strength function */
	ref_eval_strength(h); 
}

void eval_players_best_hand(struct player *p)
{
	/* TASK 10: For each hand in the 'hands' array of the player, use eval_strength to evaluate the strength of the hand */
	/*       Point best_hand to the strongest hand. */
	/*       HINT: eval_strength will set the hands vector according to its strength. Larger the vector, stronger the hand. */
	ref_eval_players_best_hand(p); 
}

void copy_card(struct card *dst, struct card *src)
{
	/* TASK 3: Implement function copy_card that copies a card structure from src to dst. */
	ref_copy_card(dst, src); 
}

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

	ref_initialize_player(p, player_cards, community_cards); 
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

		eval_players_best_hand(&P1);
		eval_players_best_hand(&P2);

		/* TASK 11: Check which player has the strongest hand and print either "Player 1 wins" or "Player 2 wins" */
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

	return 0;
}
	
