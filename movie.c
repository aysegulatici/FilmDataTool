#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024
#define MAX_GENRES 1000


typedef struct {
  int budget;
  int year;
  char *title;
  char *titletype;
  char *top250;
} Budget;

typedef struct {
  char **genre;
  char *title;
  char **directors;
  char *mustsee;
  double rating;
  char *url;
} Name;


void read_file(const char *filename, Budget **budgetArray, Name **nameArray, int *count);
void show_budget_array(Budget *budgetArray, int count);
void show_name_array(Name *nameArray, int count);
void show_genres(Name *nameArray, int count);
void show_movies_by_year(Budget *budgetArray, int count);
void show_movies_by_rating(Name *nameArray, int count);
void show_movie_info(Budget *budgetArray, Name *nameArray, int count, int index);
void show_genre_frequency(Name *nameArray, int count);
void release_memory(Budget *budgetArray, Name *nameArray, int count);
int compare_budget(const void *a, const void *b);
int compare_name(const void *a, const void *b);
char *str_duplicate(const char *str);
void str_split(const char *str, const char delimiter, char ***result, int *count);

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    return 1;
  }

  Budget *budgetArray = NULL;
  Name *nameArray = NULL;
  int count = 0;

  read_file(argv[1], &budgetArray, &nameArray, &count);

  int choice;
  do {
    printf("\nMenu:\n");
    printf("1. List of budget array\n");
    printf("2. List of name array\n");
    printf("3. List of genres\n");
    printf("4. List of the Movie Through the Years\n");
    printf("5. List of the Movie Through the Ratings\n");
    printf("6. All Information of a Single Movie\n");
    printf("7. Frequency of the Genres\n");
    printf("8. Exit\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
    case 1:
      show_budget_array(budgetArray, count);
      break;
    case 2:
      show_name_array(nameArray, count);
      break;
    case 3:
      show_genres(nameArray, count);
      break;
    case 4:
      show_movies_by_year(budgetArray, count);
      break;
    case 5:
      show_movies_by_rating(nameArray, count);
      break;
    case 6: {
      int index;
      printf("Enter movie index (0-%d): ", count - 1);
      scanf("%d", &index);
      if (index >= 0 && index < count) {
        show_movie_info(budgetArray, nameArray, count, index);
      } else {
        printf("Invalid index.\n");
      }
      break;
    }
    case 7:
      show_genre_frequency(nameArray, count);
      break;
    case 8:
      printf("Exiting...\n");
      break;
    default:
      printf("Invalid choice. Please try again.\n");
      break;
    }
  } while (choice != 8);

  release_memory(budgetArray, nameArray, count);

  return 0;
}

void read_file(const char *filename, Budget **budgetArray, Name **nameArray,
               int *count) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }

  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), file)) {
    (*count)++;
  }
  rewind(file);

  *budgetArray = (Budget *)malloc(*count * sizeof(Budget));
  *nameArray = (Name *)malloc(*count * sizeof(Name));

  if (*budgetArray == NULL || *nameArray == NULL) {
    perror("Memory allocation failed");
    exit(EXIT_FAILURE);
  }

  int index = 0;
  while (fgets(line, sizeof(line), file)) {
    char *token = strtok(line, ";");
    if (token) {
      (*budgetArray)[index].budget = atoi(token);
    }
    token = strtok(NULL, ";");
    if (token) {
      (*budgetArray)[index].title = str_duplicate(token);
      (*nameArray)[index].title =
          str_duplicate(token);
    }
    token = strtok(NULL, ";");
    if (token) {
      (*budgetArray)[index].titletype = str_duplicate(token);
    }
    token = strtok(NULL, ";");
    if (token) {
      (*nameArray)[index].directors = malloc(2 * sizeof(char *));
      (*nameArray)[index].directors[0] = str_duplicate(token);
      (*nameArray)[index].directors[1] = NULL;
    }
    token = strtok(NULL, ";");
    if (token) {
      (*nameArray)[index].rating = atof(token);
    }
    token = strtok(NULL, ";");
    token = strtok(NULL, ";");
    if (token) {
      (*budgetArray)[index].year = atoi(token);
    }
    token = strtok(NULL, ";");
    if (token) {
      char **genres;
      int genre_count;
      str_split(token, ',', &genres, &genre_count);
      (*nameArray)[index].genre = genres;
    }
    token = strtok(NULL, ";");
    if (token) {
      (*nameArray)[index].url = str_duplicate(token);
    }
    token = strtok(NULL, ";");
    if (token) {
      (*nameArray)[index].mustsee = str_duplicate(token);
    }
    token = strtok(NULL, ";");
    if (token) {
      (*budgetArray)[index].top250 = str_duplicate(token);
    }
    index++;
  }

  fclose(file);

  qsort(*budgetArray, *count, sizeof(Budget), compare_budget);
}

void show_budget_array(Budget *budgetArray, int count) {
  for (int x = 0; x < count; x++) {
    printf("Budget: %d, Title: %s, TitleType: %s, Top250: %s\n",
           budgetArray[x].budget, budgetArray[x].title,
           budgetArray[x].titletype, budgetArray[x].top250);
  }
}

void show_name_array(Name *nameArray, int count) {
  for (int x = 0; x < count; x++) {
    printf("Title: %s, Rating: %.2f, URL: %s, MustSee: %s\n",
           nameArray[x].title, nameArray[x].rating,
           nameArray[x].url ? nameArray[x].url : " ",
           nameArray[x].mustsee ? nameArray[x].mustsee : " ");
    printf("Genres: ");
    for (int y = 0; nameArray[x].genre[y] != NULL; y++) {
      printf("%s ", nameArray[x].genre[y]);
    }
    printf("\nDirectors: ");
    for (int y = 0; nameArray[x].directors[y] != NULL; y++) {
      printf("%s ", nameArray[x].directors[y]);
    }
    printf("\n");
  }
}

void show_genres(Name *nameArray, int count) {
  printf("Genres:\n");
  for (int x = 0; x < count; x++) {
    for (int y = 0; nameArray[x].genre[y] != NULL; y++) {
      printf("%s\n", nameArray[x].genre[y]);
    }
  }
}

void show_movies_by_year(Budget *budgetArray, int count) {
  printf("Movies sorted by year:\n");
  for (int x = 0; x < count; x++) {
    printf("Year: %d, Title: %s\n", budgetArray[x].year, budgetArray[x].title);
  }
}

void show_movies_by_rating(Name *nameArray, int count) {
  printf("Movies sorted by rating:\n");
  for (int x = 0; x < count; x++) {
    printf("Rating: %.2f, Title: %s\n", nameArray[x].rating,
           nameArray[x].title);
  }
}

void show_movie_info(Budget *budgetArray, Name *nameArray, int count,
                      int index) {
  if (index < 0 || index >= count) {
    printf("Invalid index.\n");
    return;
  }
  printf("Budget Info:\n");
  printf("Budget: %d, Year: %d, Title: %s, TitleType: %s, Top250: %s\n",
         budgetArray[index].budget, budgetArray[index].year,
         budgetArray[index].title, budgetArray[index].titletype,
         budgetArray[index].top250);

  printf("Name Info:\n");
  printf("Title: %s, Rating: %.2f, URL: %s, MustSee: %s\n",
         nameArray[index].title, nameArray[index].rating,
         nameArray[index].url ? nameArray[index].url : " ",
         nameArray[index].mustsee ? nameArray[index].mustsee : " ");
  printf("Genres: ");
  for (int y = 0; nameArray[index].genre[y] != NULL; y++) {
    printf("%s ", nameArray[index].genre[y]);
  }
  printf("\nDirectors: ");
  for (int y = 0; nameArray[index].directors[y] != NULL; y++) {
    printf("%s ", nameArray[index].directors[y]);
  }
  printf("\n");
}

void show_genre_frequency(Name *nameArray, int count) {
  printf("Frequency of genres:\n");

  char **genre_list = malloc(MAX_GENRES * sizeof(char *));
  int genre_count = 0;

  for (int i = 0; i < count; i++) {
    for (int j = 0; nameArray[i].genre[j] != NULL; j++) {
      int found = 0;
      for (int k = 0; k < genre_count; k++) {
        if (strcmp(genre_list[k], nameArray[i].genre[j]) == 0) {
          found = 1;
          break;
        }
      }
      if (!found) {
        if (genre_count >= MAX_GENRES) {
          printf("Maximum genre count exceeded\n");
          break;
        }
        genre_list[genre_count] = str_duplicate(nameArray[i].genre[j]);
        genre_count++;
      }
    }
  }

  for (int i = 0; i < genre_count; i++) {
    int freq = 0;
    for (int j = 0; j < count; j++) {
      for (int k = 0; nameArray[j].genre[k] != NULL; k++) {
        if (strcmp(genre_list[i], nameArray[j].genre[k]) == 0) {
          freq++;
          break;
        }
      }
    }
    printf("%s: %d\n", genre_list[i], freq);
    free(genre_list[i]);
  }

  free(genre_list);
}

void release_memory(Budget *budgetArray, Name *nameArray, int count) {
  for (int i = 0; i < count; i++) {
    free(budgetArray[i].title);
    free(budgetArray[i].titletype);
    free(budgetArray[i].top250);
    free(nameArray[i].title);
    free(nameArray[i].mustsee);
    free(nameArray[i].url);
    for (int j = 0; nameArray[i].genre[j] != NULL; j++) {
      free(nameArray[i].genre[j]);
    }
    free(nameArray[i].genre);
    for (int j = 0; nameArray[i].directors[j] != NULL; j++) {
      free(nameArray[i].directors[j]);
    }
    free(nameArray[i].directors);
  }
  free(budgetArray);
  free(nameArray);
}

int compare_budget(const void *a, const void *b) {
  const Budget *budget1 = (const Budget *)a;
  const Budget *budget2 = (const Budget *)b;
  return budget1->year - budget2->year;
}

int compare_name(const void *a, const void *b) {
  const Name *name1 = (const Name *)a;
  const Name *name2 = (const Name *)b;
  return (name1->rating < name2->rating) ? -1 : (name1->rating > name2->rating);
}

char *str_duplicate(const char *str) {
  if (str == NULL) {
    return NULL;
  }
  char *dup = malloc(strlen(str) + 1);
  if (dup != NULL) {
    strcpy(dup, str);
  }
  return dup;
}

void str_split(const char *str, const char delimiter, char ***result,
               int *count) {
  char *str_copy = str_duplicate(str);
  if (str_copy == NULL) {
    *result = NULL;
    *count = 0;
    return;
  }

  int capacity = 10;
  *result = malloc(capacity * sizeof(char *));
  *count = 0;

  char *token = strtok(str_copy, &delimiter);
  while (token) {
    if (*count >= capacity) {
      capacity *= 2;
      *result = realloc(*result, capacity * sizeof(char *));
    }
    (*result)[*count] = str_duplicate(token);
    (*count)++;
    token = strtok(NULL, &delimiter);
  }
  free(str_copy);
}
