#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_DIMENSION 32
#define MIN_DIMENSION 2
#define TREASURE_SYMBOL '$'
#define EMPTY_SYMBOL ' '
#define DUG_SYMBOL '#'
#define CHEAT_SYMBOL '!'
#define TREASURE_PREFIX 'T'
#define TREASURE_CODE_LENGTH 4
#define HEADER_LENGTH_RIS 10
#define CODE_LENTH 5
#define END_SPACE 1

typedef struct {
    char **map;
    int width;
    int height;
    char **treasure_codes;
    int *treasure_x;
    int *treasure_y;
    int num_treasures;
    int treasures_found;
} TreasureMap;

void initialize_map(TreasureMap *tmap);
void place_treasures(TreasureMap *tmap);
void display_map(TreasureMap *tmap, int cheat_mode);
void dig(TreasureMap *tmap, int x, int y);
int is_valid_code(const char *code);
int check_duplicate_code(TreasureMap *tmap, const char *code);
void free_memory(TreasureMap *tmap);
void game_menu(TreasureMap *tmap);
char *strdup(const char *str);

int main() {
    TreasureMap tmap;
    int i;
    char code[CODE_LENTH];
    int num_treasures;

    tmap.treasures_found = 0;
    
    printf("Enter Map Width : ");
    if (scanf("%d", &tmap.width) == EOF) { 
        printf("Invalid map width.\n");
        return EXIT_FAILURE;
    }
    if (tmap.width < MIN_DIMENSION || tmap.width > MAX_DIMENSION) {
        printf("Invalid map width.\n");
        return EXIT_FAILURE;
    }

    printf("Enter Map Height : ");
    if (scanf("%d", &tmap.height) == EOF) { 
        printf("Invalid Map Height.\n");
        return EXIT_FAILURE;
    }
    if (tmap.height < MIN_DIMENSION || tmap.height > MAX_DIMENSION) {
        printf("Invalid map height.\n");
        return EXIT_FAILURE;
    }

printf("Enter number of treasures to add: ");
do {
    if (scanf("%d", &num_treasures) != 1 || num_treasures <= 0) {
        printf("Invalid input. Please enter a positive integer.\n");
        while (getchar() != '\n'); 
        printf("Enter number of treasures to add: ");
    } else {
        tmap.num_treasures = num_treasures;
        break;
    }
} while (1);


    tmap.treasure_codes = malloc(tmap.num_treasures * sizeof(char *));
    tmap.treasure_x = malloc(tmap.num_treasures * sizeof(int));
    tmap.treasure_y = malloc(tmap.num_treasures * sizeof(int));

    if (!tmap.treasure_codes || !tmap.treasure_x || !tmap.treasure_y) { 
        fprintf(stderr, "Memory allocation failed.\n");
        free_memory(&tmap);
        return EXIT_FAILURE;
    }
    
    for (i = 0; i < tmap.num_treasures; i++) {
        do {
            printf("Enter treasure #%d code: ", i + 1);
            if (scanf("%s", code) == EOF) { 
                printf("End of input \n");
                free_memory(&tmap);
                return EXIT_FAILURE;
            }
        }  while (!is_valid_code(code) || check_duplicate_code(&tmap, code));
        
        tmap.treasure_codes[i] = strdup(code); 

        if (!tmap.treasure_codes[i]) {
            fprintf(stderr, "Memory allocation failed\n");
            free_memory(&tmap);
            return EXIT_FAILURE;
        }
    }

    initialize_map(&tmap);
    place_treasures(&tmap);
  
    game_menu(&tmap);
    free_memory(&tmap);
    return EXIT_SUCCESS;
}

char *strdup(const char *str) { 
    size_t len = strlen(str) + END_SPACE;  
    char *copy = malloc(len);
    if (copy) {
        memcpy(copy, str, len); 
    }
    return copy;
}

void initialize_map(TreasureMap *tmap) {
    int i;
    tmap->map = malloc(tmap->height * sizeof(char *));
    if (!tmap->map) { 
        fprintf(stderr, "Memory allocation \n");
        exit(EXIT_FAILURE);
    }
 
    for (i = 0; i < tmap->height; i++) {
        tmap->map[i] = malloc(tmap->width * sizeof(char));
        if (!tmap->map[i]) { 
            fprintf(stderr, "Memory allocation failed \n");
            exit(EXIT_FAILURE);
        }
        memset(tmap->map[i], EMPTY_SYMBOL, tmap->width);  /*set each value in memory as definded*/
    }
}

void place_treasures(TreasureMap *tmap) {
    int i, x, y;
    srand(time(NULL));                            /*generate random numbers*/
    
    for (i = 0; i < tmap->num_treasures; i++) {
        do {
            x = rand() % tmap->width;           /*get a number 0 - tmap->width-1*/
            y = rand() % tmap->height;
        } while (tmap->map[y][x] == CHEAT_SYMBOL);

        tmap->treasure_x[i] = x;
        tmap->treasure_y[i] = y;
    }
}

void display_map(TreasureMap *tmap, int cheat_mode) {
    int i, y, x;
    printf("   ");
   
    for (i = 0; i < tmap->width; i++) printf("%d", i % HEADER_LENGTH_RIS);
    printf("\n   ");
    for (i = 0; i < tmap->width; i++) printf("-");
    printf("\n");

    for (y = 0; y < tmap->height; y++) {
        printf("%02d|", y);
        for (x = 0; x < tmap->width; x++) {
            int is_treasure = 0;
            if (cheat_mode) {
                for (i = 0; i < tmap->num_treasures; i++) {
                    if (tmap->treasure_x[i] == x && tmap->treasure_y[i] == y) {
                        is_treasure = 1;
                        break;
                    }
                }
            }
            if (is_treasure) {
                printf("%c", CHEAT_SYMBOL);
            } else {
                printf("%c", tmap->map[y][x] == DUG_SYMBOL ? DUG_SYMBOL : tmap->map[y][x]);
            }
        }
        printf("\n");
    }
}

void dig(TreasureMap *tmap, int x, int y) {
    int i, found_treasure = 0;

    if (x < 0 || x >= tmap->width || y < 0 || y >= tmap->height) {
        printf("Invalid coordinate\n");
        return;
    }

    if (tmap->map[y][x] == DUG_SYMBOL || tmap->map[y][x] == TREASURE_SYMBOL) {
        printf("You already dug there.\n");
        return;
    }

    for (i = 0; i < tmap->num_treasures; i++) {
        if (tmap->treasure_x[i] == x && tmap->treasure_y[i] == y) {
            printf("Congratulations, you found a treasure!\n");
            tmap->map[y][x] = TREASURE_SYMBOL;  

            tmap->treasures_found++;
            found_treasure = 1;
            break;
        }
    }

    if (!found_treasure) {
        printf("You found nothing there!\n");
        tmap->map[y][x] = DUG_SYMBOL; 

    if (tmap->treasures_found == tmap->num_treasures) {
        printf("Congratulations! You've found all the treasures!\n");
    }
}
}



void get_coordinates(TreasureMap *tmap, int *x, int *y) {
    int valid_input = 0;

    while (!valid_input) {
        printf("x = ");
        if (scanf("%d", x) == 1 && *x >= 0 && *x < tmap->width) {
            valid_input = 1;
        } else {
            printf("Invalid input for x. ");
            while (getchar() != '\n');  
        }
    }

    valid_input = 0;

    while (!valid_input) {
        printf("y = ");
        if (scanf("%d", y) == 1 && *y >= 0 && *y < tmap->height) {
            valid_input = 1;
        } else {
            printf("Invalid input for y.");
            while (getchar() != '\n');  
        }
    }
}



int is_valid_code(const char *code) {
    if (strlen(code) != TREASURE_CODE_LENGTH || code[0] != TREASURE_PREFIX || !isdigit(code[1]) || !isdigit(code[2]) || !isdigit(code[3])) {
        printf("Treasure codes must be 4 characters: T followed by three digits.\n");
        return 0;
    }
    return 1;
}

int check_duplicate_code(TreasureMap *tmap, const char *code) {
    int i;
    for (i = 0; i < tmap->num_treasures; i++) {
        if (tmap->treasure_codes[i] && strcmp(tmap->treasure_codes[i], code) == 0) {
            printf("Treasure code already in use.\n");
            return 1;
        }
    }
    return 0;
}

void free_memory(TreasureMap *tmap) {
    int i;
    for (i = 0; i < tmap->height; i++) {
        free(tmap->map[i]);
    }
    free(tmap->map);
    for (i = 0; i < tmap->num_treasures; i++) {
        free(tmap->treasure_codes[i]);
    }
    free(tmap->treasure_codes);
    free(tmap->treasure_x);
    free(tmap->treasure_y);
}
void game_menu(TreasureMap *tmap) {
    int cheat_mode = 0;
    int x, y;
    while (1) {
        int choice;
        display_map(tmap, cheat_mode);
        
        printf("\nMenu Options: \n");
        printf("1 - Dig at a spot  | 2 - Exit Game  | 3 - Cheat! -> ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid choice\n");
            while (getchar() != '\n'); 
        }

        if (choice == 1) {
            printf("Where would you like to dig? \n");
            get_coordinates(tmap, &x, &y);  
            dig(tmap, x, y);  
            
           
            if (tmap->treasures_found == tmap->num_treasures) {
                printf("All treasures found!\n");
                break;
            }
        } else if (choice == 2) {
            printf("Exiting the game\n");
            break;
        } else if (choice == 3) {
            cheat_mode = !cheat_mode;
            printf("Cheat mode %s\n", cheat_mode ? "enabled" : "disabled");
        } else {
            printf("Invalid choice.\n");
        }
    }
}


