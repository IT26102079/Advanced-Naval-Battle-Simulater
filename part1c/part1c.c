#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define PI 3.14159265358979323846
#define G 9.8
#define MAX_ESCORTS 100
#define MAX_POINTS 100

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
    int fired;
};

double randomDouble(double min, double max)
{
    return min +
           ((double)rand() / RAND_MAX) *
           (max - min);
}

double calculateDistance(double x1,
                         double y1,
                         double x2,
                         double y2)
{
    double dx = x2 - x1;
    double dy = y2 - y1;

    return sqrt(dx * dx + dy * dy);
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
    double bestTime = 1000000000.0;
    int found = 0;

    for (double angle = angleMin;
         angle <= angleMax;
         angle += 0.1)
    {
        double radians =
            angle * PI / 180.0;

        double sinTwoTheta =
            sin(2.0 * radians);

        if (sinTwoTheta <= 0.000001)
        {
            continue;
        }

        double requiredVelocity =
            sqrt((distance * G) /
                 sinTwoTheta);

        if (requiredVelocity < vmin ||
            requiredVelocity > vmax)
        {
            continue;
        }

        double horizontalVelocity =
            requiredVelocity *
            cos(radians);

        if (horizontalVelocity <= 0.000001)
        {
            continue;
        }

        double currentTime =
            distance /
            horizontalVelocity;

        if (currentTime < bestTime)
        {
            bestTime = currentTime;

            *selectedVelocity =
                requiredVelocity;

            *selectedAngle =
                angle;

            *timeToHit =
                currentTime;

            found = 1;
        }
    }

    return found;
}

void setupEscortShips(struct Escort escorts[],
                      int n,
                      double battleshipVmax,
                      double D)
{
    for (int i = 0; i < n; i++)
    {
        int randomType =
            rand() % 5;

        escorts[i].id =
            i + 1;

        escorts[i].destroyed =
            0;

        escorts[i].fired =
            0;

        if (randomType == 0)
        {
            escorts[i].type = 'A';

            escorts[i].impactPower =
                0.08;

            escorts[i].angleRange =
                20;

            escorts[i].vmax =
                1.2 * battleshipVmax;
        }

        else if (randomType == 1)
        {
            escorts[i].type = 'B';

            escorts[i].impactPower =
                0.06;

            escorts[i].angleRange =
                30;

            escorts[i].vmax =
                randomDouble(
                    0.60 * battleshipVmax,
                    0.95 * battleshipVmax);
        }

        else if (randomType == 2)
        {
            escorts[i].type = 'C';

            escorts[i].impactPower =
                0.07;

            escorts[i].angleRange =
                25;

            escorts[i].vmax =
                randomDouble(
                    0.60 * battleshipVmax,
                    0.95 * battleshipVmax);
        }

        else if (randomType == 3)
        {
            escorts[i].type = 'D';

            escorts[i].impactPower =
                0.05;

            escorts[i].angleRange =
                50;

            escorts[i].vmax =
                randomDouble(
                    0.60 * battleshipVmax,
                    0.95 * battleshipVmax);
        }

        else
        {
            escorts[i].type = 'E';

            escorts[i].impactPower =
                0.04;

            escorts[i].angleRange =
                70;

            escorts[i].vmax =
                randomDouble(
                    0.60 * battleshipVmax,
                    0.95 * battleshipVmax);
        }

        escorts[i].vmin =
            randomDouble(
                0.10 * escorts[i].vmax,
                0.50 * escorts[i].vmax);

        escorts[i].angleMin =
            randomDouble(
                0.0,
                90.0 -
                escorts[i].angleRange);

        escorts[i].angleMax =
            escorts[i].angleMin +
            escorts[i].angleRange;

        escorts[i].x =
            randomDouble(0, D);

        escorts[i].y =
            randomDouble(0, D);
    }
}

void copyEscorts(struct Escort destination[],
                 struct Escort source[],
                 int n)
{
    for (int i = 0; i < n; i++)
    {
        destination[i] =
            source[i];
    }
}

void saveSetup(const char *filename,
               struct Battleship b,
               struct Escort escorts[],
               int n,
               double D)
{
    FILE *file =
        fopen(filename, "w");

    if (file == NULL)
    {
        return;
    }

    fprintf(file,
            "=========================================\n");

    fprintf(file,
            "       PART 1-C INITIAL CONDITIONS\n");

    fprintf(file,
            "=========================================\n");

    fprintf(file,
            "Battlefield: %.2f x %.2f\n",
            D,
            D);

    fprintf(file,
            "Battleship Type: %c\n",
            b.type);

    fprintf(file,
            "Battleship Position: (%.2f, %.2f)\n",
            b.x,
            b.y);

    fprintf(file,
            "Battleship Vmax: %.2f\n",
            b.vmax);

    for (int i = 0; i < n; i++)
    {
        fprintf(file,
                "\nE%d Type E%c\n",
                escorts[i].id,
                escorts[i].type);

        fprintf(file,
                "Position: (%.2f, %.2f)\n",
                escorts[i].x,
                escorts[i].y);

        fprintf(file,
                "Impact Power: %.2f\n",
                escorts[i].impactPower);

        fprintf(file,
                "Angle Range: %.2f - %.2f\n",
                escorts[i].angleMin,
                escorts[i].angleMax);

        fprintf(file,
                "Velocity Range: %.2f - %.2f\n",
                escorts[i].vmin,
                escorts[i].vmax);
    }

    fclose(file);
}

int runPart1CStep(struct Battleship b,
                  struct Escort escorts[],
                  int n,
                  double bAngleMin,
                  double bAngleMax,
                  double *cumulativeImpact,
                  FILE *file)
{
    double distance;
    double velocity;
    double angle;
    double hitTime;

    int destroyedThisStep =
        0;

    fprintf(file,
            "\nBattleship Position: (%.2f, %.2f)\n",
            b.x,
            b.y);

    fprintf(file,
            "Battleship Angle Range: %.2f - %.2f\n",
            bAngleMin,
            bAngleMax);

    /*
       E ships attack B.
       Each E ship can fire only once.
    */

    for (int i = 0; i < n; i++)
    {
        if (escorts[i].destroyed == 1 ||
            escorts[i].fired == 1)
        {
            continue;
        }

        distance =
            calculateDistance(
                escorts[i].x,
                escorts[i].y,
                b.x,
                b.y);

        if (findShot(
                distance,
                escorts[i].vmin,
                escorts[i].vmax,
                escorts[i].angleMin,
                escorts[i].angleMax,
                &velocity,
                &angle,
                &hitTime))
        {
            escorts[i].fired =
                1;

            *cumulativeImpact +=
                escorts[i].impactPower;

            fprintf(file,
                    "E%d hits B\n",
                    escorts[i].id);

            fprintf(file,
                    "Impact Power: %.2f\n",
                    escorts[i].impactPower);

            fprintf(file,
                    "Cumulative Impact: %.2f\n",
                    *cumulativeImpact);

            fprintf(file,
                    "Time to hit: %.2f seconds\n",
                    hitTime);

            if (*cumulativeImpact >= 1.0)
            {
                fprintf(file,
                        "\nBattleship Status: DESTROYED\n");

                return 1;
            }
        }
    }

    /*
       B attacks active escort ships.
       One hit destroys an escort ship.
    */

    for (int i = 0; i < n; i++)
    {
        if (escorts[i].destroyed == 1)
        {
            continue;
        }

        distance =
            calculateDistance(
                b.x,
                b.y,
                escorts[i].x,
                escorts[i].y);

        if (findShot(
                distance,
                b.vmin,
                b.vmax,
                bAngleMin,
                bAngleMax,
                &velocity,
                &angle,
                &hitTime))
        {
            escorts[i].destroyed =
                1;

            destroyedThisStep++;

            fprintf(file,
                    "\nB destroys E%d\n",
                    escorts[i].id);

            fprintf(file,
                    "Angle: %.2f degrees\n",
                    angle);

            fprintf(file,
                    "Velocity: %.2f\n",
                    velocity);

            fprintf(file,
                    "Time: %.2f seconds\n",
                    hitTime);
        }
    }

    fprintf(file,
            "\nEscort ships destroyed this step: %d\n",
            destroyedThisStep);

    fprintf(file,
            "Cumulative impact on B: %.2f\n",
            *cumulativeImpact);

    fprintf(file,
            "Battleship Status: SURVIVED\n");

    return 0;
}

void runStationaryPart1C(
    struct Battleship battleship,
    struct Escort escorts[],
    int n)
{
    double cumulativeImpact =
        0.0;

    FILE *file =
        fopen("part1c_stationary.txt",
              "w");

    if (file == NULL)
    {
        return;
    }

    fprintf(file,
            "=========================================\n");

    fprintf(file,
            "     PART 1-C STATIONARY SIMULATION\n");

    fprintf(file,
            "=========================================\n");

    runPart1CStep(
        battleship,
        escorts,
        n,
        0.0,
        90.0,
        &cumulativeImpact,
        file);

    fprintf(file,
            "\nFinal cumulative impact on B: %.2f\n",
            cumulativeImpact);

    fclose(file);
}

void runMovingPart1C(
    struct Battleship battleship,
    struct Escort originalEscorts[],
    int n,
    int k,
    int t,
    double thetaMin,
    double pathX[],
    double pathY[])
{
    struct Escort sim1[MAX_ESCORTS];
    struct Escort sim2[MAX_ESCORTS];

    double cumulativeImpact1 =
        0.0;

    double cumulativeImpact2 =
        0.0;

    copyEscorts(
        sim1,
        originalEscorts,
        n);

    copyEscorts(
        sim2,
        originalEscorts,
        n);

    FILE *file1 =
        fopen("part1c_sim1.txt",
              "w");

    FILE *file2 =
        fopen("part1c_sim2.txt",
              "w");

    if (file1 == NULL ||
        file2 == NULL)
    {
        return;
    }

    fprintf(file1,
            "=========================================\n");

    fprintf(file1,
            "       PART 1-C SIMULATION 1\n");

    fprintf(file1,
            "=========================================\n");

    for (int i = 0; i < k; i++)
    {
        struct Battleship currentB =
            battleship;

        currentB.x =
            pathX[i];

        currentB.y =
            pathY[i];

        fprintf(file1,
                "\n--- Iteration %d ---\n",
                i + 1);

        if (runPart1CStep(
                currentB,
                sim1,
                n,
                0.0,
                90.0,
                &cumulativeImpact1,
                file1))
        {
            break;
        }
    }

    fprintf(file1,
            "\nFinal cumulative impact on B: %.2f\n",
            cumulativeImpact1);


    fprintf(file2,
            "=========================================\n");

    fprintf(file2,
            "       PART 1-C SIMULATION 2\n");

    fprintf(file2,
            "=========================================\n");

    for (int i = 0; i < k; i++)
    {
        struct Battleship currentB =
            battleship;

        double minimumAngle;

        currentB.x =
            pathX[i];

        currentB.y =
            pathY[i];

        if (i < t)
        {
            minimumAngle =
                0.0;
        }
        else
        {
            minimumAngle =
                thetaMin;
        }

        fprintf(file2,
                "\n--- Iteration %d ---\n",
                i + 1);

        if (runPart1CStep(
                currentB,
                sim2,
                n,
                minimumAngle,
                90.0,
                &cumulativeImpact2,
                file2))
        {
            break;
        }
    }

    fprintf(file2,
            "\nFinal cumulative impact on B: %.2f\n",
            cumulativeImpact2);

    fclose(file1);
    fclose(file2);
}

int main()
{
    struct Battleship battleship;

    struct Escort escorts[MAX_ESCORTS];

    struct Escort stationaryEscorts[MAX_ESCORTS];

    double pathX[MAX_POINTS];
    double pathY[MAX_POINTS];

    double D;
    double thetaMin;

    int numberOfEscorts;
    int k;
    int t;

    srand((unsigned int)time(NULL));

    printf("\n=========================================\n");
    printf("                PART 1-C\n");
    printf("=========================================\n");

    do
    {
        printf("Enter battlefield size D: ");
        scanf("%lf", &D);

    } while (D <= 0);


    do
    {
        printf("Enter number of escort ships (1-100): ");
        scanf("%d", &numberOfEscorts);

    } while (numberOfEscorts < 1 ||
             numberOfEscorts > MAX_ESCORTS);


    printf("\nU - USS Iowa (BB-61)\n");
    printf("M - MS King George V\n");
    printf("R - Richelieu\n");
    printf("S - Sovetsky Soyuz-class\n");


    do
    {
        printf("Enter battleship type: ");

        scanf(" %c",
              &battleship.type);

        if (battleship.type >= 'a' &&
            battleship.type <= 'z')
        {
            battleship.type -= 32;
        }

    } while (battleship.type != 'U' &&
             battleship.type != 'M' &&
             battleship.type != 'R' &&
             battleship.type != 'S');


    do
    {
        printf("Enter battleship X position (0 - %.2f): ",
               D);

        scanf("%lf",
              &battleship.x);

    } while (battleship.x < 0 ||
             battleship.x > D);


    do
    {
        printf("Enter battleship Y position (0 - %.2f): ",
               D);

        scanf("%lf",
              &battleship.y);

    } while (battleship.y < 0 ||
             battleship.y > D);


    do
    {
        printf("Enter battleship maximum shell velocity: ");

        scanf("%lf",
              &battleship.vmax);

    } while (battleship.vmax <= 0);


    battleship.vmin =
        0;


    setupEscortShips(
        escorts,
        numberOfEscorts,
        battleship.vmax,
        D);


    copyEscorts(
        stationaryEscorts,
        escorts,
        numberOfEscorts);


    do
    {
        printf("Enter number of path points k (2-100): ");

        scanf("%d",
              &k);

    } while (k < 2 ||
             k > MAX_POINTS);


    do
    {
        printf("Enter jam iteration t (1 to %d): ",
               k - 1);

        scanf("%d",
              &t);

    } while (t < 1 ||
             t >= k);


    do
    {
        printf("Enter thetaMin (0 < thetaMin < 30): ");

        scanf("%lf",
              &thetaMin);

    } while (thetaMin <= 0 ||
             thetaMin >= 30);


    for (int i = 0; i < k; i++)
    {
        pathX[i] =
            randomDouble(0, D);

        pathY[i] =
            randomDouble(0, D);
    }


    saveSetup(
        "part1c_initial_conditions.txt",
        battleship,
        escorts,
        numberOfEscorts,
        D);


    runStationaryPart1C(
        battleship,
        stationaryEscorts,
        numberOfEscorts);


    runMovingPart1C(
        battleship,
        escorts,
        numberOfEscorts,
        k,
        t,
        thetaMin,
        pathX,
        pathY);


    printf("\n=========================================\n");
    printf("          PART 1-C COMPLETED\n");
    printf("=========================================\n");

    printf("\nCreated files:\n");
    printf("part1c_initial_conditions.txt\n");
    printf("part1c_stationary.txt\n");
    printf("part1c_sim1.txt\n");
    printf("part1c_sim2.txt\n");

    return 0;
}
