/* 
 * Name: James Marino
 * Task: Task 1 - Task 3
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *names [] = {
    "Aiden", "Jackson", "Ethan", "Liam", "Mason", "Noah", "Lucas", "Jacob", "Jayden",
    "Jack", "Logan", "Ryan", "Caleb", "Benjamin", "William", "Michael", "Alexander",
    "Elijah", "Matthew", "Dylan", "James", "Owen", "Connor", "Brayden", "Carter",
    "Landon", "Joshua", "Luke", "Daniel", "Gabriel", "Nicholas", "Nathan", "Oliver",
    "Henry", "Andrew", "Gavin", "Cameron", "Eli", "Max", "Isaac", "Evan", "Samuel", 
    "Grayson", "Tyler", "Zachary", "Wyatt", "Joseph", "Charlie", "Hunter", "David", 
    "Anthony", "Christian", "Colton", "Thomas", "Dominic", "Austin", "John", "Sebastian",
    "Cooper", "Levi", "Parker", "Isaiah", "Chase", "Blake", "Aaron", "Alex", "Adam",
    "Tristan", "Julian", "Jonathan", "Christopher", "Jace", "Nolan", "Miles", "Jordan",
    "Carson", "Colin", "Ian", "Riley", "Xavier", "Hudson", "Adrian", "Cole", "Brody",
    "Leo", "Jake", "Bentley", "Sean", "Jeremiah", "Asher", "Nathaniel", "Micah",
    "Jason", "Ryder", "Declan", "Hayden", "Brandon", "Easton", "Lincoln", "Harrison",
    "Sophia", "Emma", "Olivia", "Isabella", "Ava", "Lily", "Zoe", "Chloe", "Mia", "Madison",
    "Emily", "Ella", "Madelyn", "Abigail", "Aubrey", "Addison", "Avery", "Layla", "Hailey",
    "Amelia", "Hannah", "Charlotte", "Kaitlyn", "Harper", "Kaylee", "Sophie", "Mackenzie",
    "Peyton", "Riley", "Grace", "Brooklyn", "Sarah", "Aaliyah", "Anna", "Arianna", "Ellie",
    "Natalie", "Isabelle", "Lillian", "Evelyn", "Elizabeth", "Lyla", "Lucy", "Claire",
    "Makayla", "Kylie", "Audrey", "Maya", "Leah", "Gabriella", "Annabelle", "Savannah",
    "Nora", "Reagan", "Scarlett", "Samantha", "Alyssa", "Allison", "Elena", "Stella",
    "Alexis", "Victoria", "Aria", "Molly", "Maria", "Bailey", "Sydney", "Bella", "Mila",
    "Taylor", "Kayla", "Eva", "Jasmine", "Gianna", "Alexandra", "Julia", "Eliana",
    "Kennedy", "Brianna", "Ruby", "Lauren", "Alice", "Violet", "Kendall", "Morgan",
    "Caroline", "Piper", "Brooke", "Elise", "Alexa", "Sienna", "Reese", "Clara",
    "Paige", "Kate", "Nevaeh", "Sadie", "Quinn", "Isla", "Eleanor"
};
static int numnames = sizeof (names) / sizeof (char*);

void bubblesort(const char* vals[], int num)
{
    int swaps;
    
    do {
        swaps = 0;
        int i;
        
        for (i = 1; i < num; i++) {
            int len1 = strlen(vals[i-1]);
            int len2 = strlen(vals[i]);
            
            if (len1 > len2) {
                const char *temp = vals[i];
                vals[i] = vals[i - 1];
                vals[i - 1] = temp;
                swaps++;
            }
        };
    } while (swaps > 0);
}

int main(int argc, char** argv) {

    bubblesort(names, numnames);
    
    for (int i = 0; i < numnames; i++) {
        printf("%s\n", names[i]);
    }
    
    return (EXIT_SUCCESS);
}
