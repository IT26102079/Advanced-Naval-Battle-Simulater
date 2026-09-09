#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define PI 3.14159265358979323846
#define G 9.8

struct Battleship
{
    char type;
    double x;
    double y;
    double vmin;
    double vmax;
};

struct Escort
{
    int id;
    char type;

    double x;
    double y;

    double angleRange;
    double angleMin;
    double angleMax;

    double vmin;
    double vmax;

    double impactPower;
    int destroyed;
};

double randomDouble(double min, double max);

void setupEscortShips(struct Escort escorts[],
                      int n,
                      double battleshipVmax,
                      double D);

void runPart1A();

double calculateDistance(double x1,
                         double y1,
                         double x2,
                         double y2);

int findShot(double distance,
             double vmin,
             double vmax,
             double angleMin,
             double angleMax,
             double *selectedVelocity,
             double *selectedAngle,
             double *timeToHit);

void runSimulation(struct Battleship b,
                   struct Escort escorts[],
                   int n);
void saveHitDetails(struct Battleship b,
                    struct Escort escorts[],
                    int n);
void saveFinalConditions(struct Battleship b,
                         struct Escort escorts[],
                         int n);
void saveInitialConditions(struct Battleship b,
                           struct Escort escorts[],
                           int n,
                           double D);

int main()
{
    int choice;

    while (1)
    {
        printf("\n=========================================\n");
        printf("      ADVANCED NAVAL BATTLE SIMULATOR\n");
        printf("=========================================\n");
        printf("1. Start Simulation\n");
        printf("2. View Instructions\n");
        printf("3. Simulation Statistics\n");
        printf("4. Exit\n");

        printf("\nEnter choice: ");
        scanf("%d", &choice);

        if (choice == 1)
        {
            runPart1A();
        }
        else if (choice == 2)
        {
            printf("\nInstructions will be added later.\n");
        }
        else if (choice == 3)
        {
            printf("\nSimulation statistics will be added later.\n");
        }
        else if (choice == 4)
        {
            printf("\nExiting simulator...\n");
            break;
        }
        else
        {
            printf("\nInvalid choice. Please try again.\n");
        }
    }

    return 0;
}

void runPart1A()
{
    struct Battleship battleship;
    struct Escort escorts[100];

    int choice;
    int numberOfEscorts;
    double D;

    printf("\n=========================================\n");
    printf("          START SIMULATION\n");
    printf("=========================================\n");
    printf("1. Setup and Run Part 1-A\n");
    printf("2. Return to Main Menu\n");

    printf("\nEnter choice: ");
    scanf("%d", &choice);

    if (choice == 2)
    {
        return;
    }

    if (choice != 1)
    {
        printf("\nInvalid choice.\n");
        return;
    }

    printf("\n=========================================\n");
    printf("              PART 1-A\n");
    printf("=========================================\n");

    do
    {
        printf("Enter battlefield size D: ");
        scanf("%lf", &D);

        if (D <= 0)
        {
            printf("D must be greater than 0.\n\n");
        }

    } while (D <= 0);

    do
    {
        printf("Enter number of escort ships (1-100): ");
        scanf("%d", &numberOfEscorts);

        if (numberOfEscorts < 1 || numberOfEscorts > 100)
        {
            printf("Invalid number of escort ships.\n\n");
        }

    } while (numberOfEscorts < 1 || numberOfEscorts > 100);

    printf("\n=========================================\n");
    printf("          BATTLESHIP SETUP\n");
    printf("=========================================\n");

    printf("U - USS Iowa (BB-61)\n");
    printf("M - MS King George V\n");
    printf("R - Richelieu\n");
    printf("S - Sovetsky Soyuz-class\n");

    do
    {
        printf("\nEnter battleship type: ");
        scanf(" %c", &battleship.type);

        if (battleship.type >= 'a' && battleship.type <= 'z')
        {
            battleship.type = battleship.type - 32;
        }

        if (battleship.type != 'U' &&
            battleship.type != 'M' &&
            battleship.type != 'R' &&
            battleship.type != 'S')
        {
            printf("Invalid battleship type.\n");
        }

    } while (battleship.type != 'U' &&
             battleship.type != 'M' &&
             battleship.type != 'R' &&
             battleship.type != 'S');

    do
    {
        printf("Enter battleship X position (0 - %.2f): ", D);
        scanf("%lf", &battleship.x);

        if (battleship.x < 0 || battleship.x > D)
        {
            printf("X position must be inside battlefield.\n");
        }

    } while (battleship.x < 0 || battleship.x > D);

    do
    {
        printf("Enter battleship Y position (0 - %.2f): ", D);
        scanf("%lf", &battleship.y);

        if (battleship.y < 0 || battleship.y > D)
        {
            printf("Y position must be inside battlefield.\n");
        }

    } while (battleship.y < 0 || battleship.y > D);

    do
    {
        printf("Enter battleship maximum shell velocity: ");
        scanf("%lf", &battleship.vmax);

        if (battleship.vmax <= 0)
        {
            printf("Maximum velocity must be greater than 0.\n");
        }

    } while (battleship.vmax <= 0);

    battleship.vmin = 0;
    srand((unsigned int)time(NULL));

    setupEscortShips(escorts,numberOfEscorts,battleship.vmax,D);

    printf("\n=========================================\n");
    printf("          INITIAL CONDITIONS\n");
    printf("=========================================\n");

    printf("\nBattlefield Size : %.2f x %.2f\n", D, D);
    printf("Number of Escorts: %d\n", numberOfEscorts);

    printf("\nBATTLESHIP B\n");
    printf("-----------------------------------------\n");
    printf("Type     : %c\n", battleship.type);
    printf("Position : (%.2f, %.2f)\n",
           battleship.x,
           battleship.y);
    printf("Vmin     : %.2f\n", battleship.vmin);
    printf("Vmax     : %.2f\n", battleship.vmax);
    printf("Angle    : 0.00 - 90.00 degrees\n");
    printf("\nESCORT SHIPS\n");
printf("-----------------------------------------\n");

for (int i = 0; i < numberOfEscorts; i++)
{
    printf("\nE%d\n", escorts[i].id);

    printf("Type         : E%c\n", escorts[i].type);

    printf("Position     : (%.2f, %.2f)\n",
           escorts[i].x,
           escorts[i].y);

    printf("Impact Power : %.2f\n",
           escorts[i].impactPower);

    printf("Angle Range  : %.2f degrees\n",
           escorts[i].angleRange);

    printf("Min Angle    : %.2f degrees\n",
           escorts[i].angleMin);

    printf("Max Angle    : %.2f degrees\n",
           escorts[i].angleMax);

    printf("Vmin         : %.2f\n",
           escorts[i].vmin);

    printf("Vmax         : %.2f\n",
           escorts[i].vmax);
}
   saveInitialConditions(battleship,escorts,numberOfEscorts,D);

    saveHitDetails(battleship,escorts,numberOfEscorts);
   
    runSimulation(battleship,escorts,numberOfEscorts);

    saveFinalConditions(battleship,escorts,numberOfEscorts);
}

double randomDouble(double min, double max)
{
    double randomValue;

    randomValue = (double)rand() / RAND_MAX;

    return min + randomValue * (max - min);
}

void setupEscortShips(struct Escort escorts[],
                      int n,
                      double battleshipVmax,
                      double D)
{
    int randomType;
    double maximumPossibleMinAngle;

    for (int i = 0; i < n; i++)
    {
        escorts[i].id = i + 1;
	escorts[i].destroyed = 0;

        randomType = rand() % 5;

        if (randomType == 0)
        {
            escorts[i].type = 'A';
            escorts[i].impactPower = 0.08;
            escorts[i].angleRange = 20;
            escorts[i].vmax = 1.2 * battleshipVmax;
        }
        else if (randomType == 1)
        {
            escorts[i].type = 'B';
            escorts[i].impactPower = 0.06;
            escorts[i].angleRange = 30;

            escorts[i].vmax =
                randomDouble(0.60 * battleshipVmax,
                             0.95 * battleshipVmax);
        }
        else if (randomType == 2)
        {
            escorts[i].type = 'C';
            escorts[i].impactPower = 0.07;
            escorts[i].angleRange = 25;

            escorts[i].vmax =
                randomDouble(0.60 * battleshipVmax,
                             0.95 * battleshipVmax);
        }
        else if (randomType == 3)
        {
            escorts[i].type = 'D';
            escorts[i].impactPower = 0.05;
            escorts[i].angleRange = 50;

            escorts[i].vmax =
                randomDouble(0.60 * battleshipVmax,
                             0.95 * battleshipVmax);
        }
        else
        {
            escorts[i].type = 'E';
            escorts[i].impactPower = 0.04;
            escorts[i].angleRange = 70;

            escorts[i].vmax =
                randomDouble(0.60 * battleshipVmax,
                             0.95 * battleshipVmax);
        }

        escorts[i].vmin =
            randomDouble(0.10 * escorts[i].vmax,
                         0.50 * escorts[i].vmax);

        maximumPossibleMinAngle =
            90.0 - escorts[i].angleRange;

        escorts[i].angleMin =
            randomDouble(0.0,
                         maximumPossibleMinAngle);

        escorts[i].angleMax =
            escorts[i].angleMin +
            escorts[i].angleRange;

        escorts[i].x =
            randomDouble(0, D);

        escorts[i].y =
            randomDouble(0, D);
    }
}double calculateDistance(double x1,
                         double y1,
                         double x2,
                         double y2)
{
    double dx;
    double dy;

    dx = x2 - x1;
    dy = y2 - y1;

    return sqrt((dx * dx) + (dy * dy));
}

int findShot(double distance,
             double vmin,
             double vmax,
             double angleMin,
             double angleMax,
             double *selectedVelocity,
             double *selectedAngle,
             double *timeToHit)
{
    double angle;
    double radians;
    double sinTwoTheta;
    double requiredVelocity;
    double horizontalVelocity;
    double currentTime;

    int found = 0;

    double bestTime = 1000000000.0;

    for (angle = angleMin;
         angle <= angleMax;
         angle = angle + 0.1)
    {
        radians = angle * PI / 180.0;

        sinTwoTheta = sin(2.0 * radians);

        if (sinTwoTheta > 0.000001)
        {
            requiredVelocity =
                sqrt((distance * G) / sinTwoTheta);

            if (requiredVelocity >= vmin &&
                requiredVelocity <= vmax)
            {
                horizontalVelocity =
                    requiredVelocity * cos(radians);

                if (horizontalVelocity > 0.000001)
                {
                    currentTime =
                        distance / horizontalVelocity;

                    if (currentTime < bestTime)
                    {
                        bestTime = currentTime;

                        *selectedVelocity = requiredVelocity;
                        *selectedAngle = angle;
                        *timeToHit = currentTime;

                        found = 1;
                    }
                }
            }
        }
    }

    return found;
}

void runSimulation(struct Battleship b,
                   struct Escort escorts[],
                   int n)
{
    int battleshipSinks = 0;
    int sinkingEscortIndex = -1;
    int destroyedCount = 0;

    double shortestEnemyHitTime = 1000000000.0;
    double battleDuration = 0;

    double distance;
    double velocity;
    double angle;
    double hitTime;

    double bHitVelocity[100];
    double bHitAngle[100];
    double bHitTime[100];

    int bCanHit[100];

    for (int i = 0; i < n; i++)
    {
        bCanHit[i] = 0;
        bHitVelocity[i] = 0;
        bHitAngle[i] = 0;
        bHitTime[i] = 0;
    }

    printf("\n=========================================\n");
    printf("             BATTLE STARTED\n");
    printf("=========================================\n");

    /*
       Check whether escort ships can hit B.
    */

    for (int i = 0; i < n; i++)
    {
        distance =
            calculateDistance(escorts[i].x,
                              escorts[i].y,
                              b.x,
                              b.y);

        if (findShot(distance,
                     escorts[i].vmin,
                     escorts[i].vmax,
                     escorts[i].angleMin,
                     escorts[i].angleMax,
                     &velocity,
                     &angle,
                     &hitTime))
        {
            if (hitTime < shortestEnemyHitTime)
            {
                shortestEnemyHitTime = hitTime;
                sinkingEscortIndex = i;
                battleshipSinks = 1;
            }
        }
    }

    /*
       Check which escort ships B can hit.
    */

    for (int i = 0; i < n; i++)
    {
        distance =
            calculateDistance(b.x,
                              b.y,
                              escorts[i].x,
                              escorts[i].y);

        if (findShot(distance,
                     b.vmin,
                     b.vmax,
                     0.0,
                     90.0,
                     &velocity,
                     &angle,
                     &hitTime))
        {
            bCanHit[i] = 1;

            bHitVelocity[i] = velocity;
            bHitAngle[i] = angle;
            bHitTime[i] = hitTime;
        }
    }

    if (battleshipSinks)
    {
        printf("\nBattleship B is going to sink.\n");

        printf("B is sunk by E%d.\n",
               escorts[sinkingEscortIndex].id);

        printf("Time for E%d shell to hit B: %.2f seconds\n",
               escorts[sinkingEscortIndex].id,
               shortestEnemyHitTime);
    }
    else
    {
        printf("\nBattleship B survives.\n\n");

        for (int i = 0; i < n; i++)
        {
            if (bCanHit[i] == 1)
            {
                escorts[i].destroyed = 1;

                destroyedCount++;

                printf("B destroyed E%d\n",
                       escorts[i].id);

                printf("Angle    : %.2f degrees\n",
                       bHitAngle[i]);

                printf("Velocity : %.2f\n",
                       bHitVelocity[i]);

                printf("Time     : %.2f seconds\n\n",
                       bHitTime[i]);

                if (bHitTime[i] > battleDuration)
                {
                    battleDuration = bHitTime[i];
                }
            }
        }

        printf("-----------------------------------------\n");

        printf("Number of escort ships destroyed: %d\n",
               destroyedCount);

        printf("Battle duration: %.2f seconds\n",
               battleDuration);
    }

    printf("\n=========================================\n");
    printf("              BATTLE ENDED\n");
    printf("=========================================\n");
}    

void saveInitialConditions(struct Battleship b,
                           struct Escort escorts[],
                           int n,
                           double D)
{
    FILE *file;

    file = fopen("initial_conditions.txt", "w");

    if (file == NULL)
    {
        printf("Error creating initial_conditions.txt\n");
        return;
    }

    fprintf(file, "=========================================\n");
    fprintf(file, "          INITIAL CONDITIONS\n");
    fprintf(file, "=========================================\n");

    fprintf(file, "\nBattlefield Size D: %.2f\n", D);

    fprintf(file, "\nBATTLESHIP B\n");
    fprintf(file, "-----------------------------------------\n");
    fprintf(file, "Type     : %c\n", b.type);
    fprintf(file, "Position : (%.2f, %.2f)\n", b.x, b.y);
    fprintf(file, "Vmin     : %.2f\n", b.vmin);
    fprintf(file, "Vmax     : %.2f\n", b.vmax);

    fprintf(file, "\nESCORT SHIPS\n");
    fprintf(file, "-----------------------------------------\n");

    for (int i = 0; i < n; i++)
    {
        fprintf(file, "\nE%d\n", escorts[i].id);
        fprintf(file, "Type         : E%c\n", escorts[i].type);
        fprintf(file, "Position     : (%.2f, %.2f)\n",
                escorts[i].x,
                escorts[i].y);

        fprintf(file, "Impact Power : %.2f\n",
                escorts[i].impactPower);

        fprintf(file, "Angle Range  : %.2f degrees\n",
                escorts[i].angleRange);

        fprintf(file, "Min Angle    : %.2f degrees\n",
                escorts[i].angleMin);

        fprintf(file, "Max Angle    : %.2f degrees\n",
                escorts[i].angleMax);

        fprintf(file, "Vmin         : %.2f\n",
                escorts[i].vmin);

        fprintf(file, "Vmax         : %.2f\n",
                escorts[i].vmax);
    }

    fclose(file);

    printf("\nInitial conditions saved to initial_conditions.txt\n");
}

void saveHitDetails(struct Battleship b,
                    struct Escort escorts[],
                    int n)
{
    FILE *file;

    file = fopen("hit_details.txt", "w");

    if (file == NULL)
    {
        printf("Error creating hit_details.txt\n");
        return;
    }

    fprintf(file, "=========================================\n");
    fprintf(file, "              HIT DETAILS\n");
    fprintf(file, "=========================================\n");

    for (int i = 0; i < n; i++)
    {
        double distance;
        double velocity;
        double angle;
        double timeToHit;

        distance =
            calculateDistance(b.x,
                              b.y,
                              escorts[i].x,
                              escorts[i].y);

        fprintf(file, "\nEscort E%d\n", escorts[i].id);
        fprintf(file, "Distance : %.2f\n", distance);

        if (findShot(distance,
                     b.vmin,
                     b.vmax,
                     0.0,
                     90.0,
                     &velocity,
                     &angle,
                     &timeToHit))
        {
            fprintf(file, "Status   : HIT POSSIBLE\n");
            fprintf(file, "Velocity : %.2f\n", velocity);
            fprintf(file, "Angle    : %.2f degrees\n", angle);
            fprintf(file, "Time     : %.2f seconds\n", timeToHit);
        }
        else
        {
            fprintf(file, "Status   : HIT NOT POSSIBLE\n");
        }
    }

    fclose(file);

    printf("Hit details saved to hit_details.txt\n");
}

void saveFinalConditions(struct Battleship b,
                         struct Escort escorts[],
                         int n)
{
    FILE *file;
    int destroyedCount = 0;

    file = fopen("final_conditions.txt", "w");

    if (file == NULL)
    {
        printf("Error creating final_conditions.txt\n");
        return;
    }

    fprintf(file, "=========================================\n");
    fprintf(file, "          FINAL CONDITIONS\n");
    fprintf(file, "=========================================\n");

    fprintf(file, "\nBATTLESHIP B\n");
    fprintf(file, "-----------------------------------------\n");

    fprintf(file, "Type     : %c\n", b.type);
    fprintf(file, "Position : (%.2f, %.2f)\n", b.x, b.y);

    fprintf(file, "\nESCORT SHIPS\n");
    fprintf(file, "-----------------------------------------\n");

    for (int i = 0; i < n; i++)
    {
        fprintf(file, "E%d (E%c) : ",
                escorts[i].id,
                escorts[i].type);

        if (escorts[i].destroyed == 1)
        {
            fprintf(file, "DESTROYED\n");
            destroyedCount++;
        }
        else
        {
            fprintf(file, "ACTIVE\n");
        }
    }

    fprintf(file,
            "\nNumber of escort ships destroyed: %d\n",
            destroyedCount);

    fprintf(file,
            "Number of escort ships remaining: %d\n",
            n - destroyedCount);

    fclose(file);

    printf("Final conditions saved to final_conditions.txt\n");
}
