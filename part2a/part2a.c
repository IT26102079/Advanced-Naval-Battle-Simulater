#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define PI 3.14159265358979323846
#define G 9.8
#define MAX_ESCORTS 100
#define MAX_POINTS 100
#define INF 1000000000.0

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
    int found = 0;
    double bestTime = INF;

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
            bestTime =
                currentTime;

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
            escorts[i].impactPower = 0.08;
            escorts[i].angleRange = 20;
            escorts[i].vmax =
                1.2 * battleshipVmax;
        }
        else if (randomType == 1)
        {
            escorts[i].type = 'B';
            escorts[i].impactPower = 0.06;
            escorts[i].angleRange = 30;

            escorts[i].vmax =
                randomDouble(
                    0.60 * battleshipVmax,
                    0.95 * battleshipVmax);
        }
        else if (randomType == 2)
        {
            escorts[i].type = 'C';
            escorts[i].impactPower = 0.07;
            escorts[i].angleRange = 25;

            escorts[i].vmax =
                randomDouble(
                    0.60 * battleshipVmax,
                    0.95 * battleshipVmax);
        }
        else if (randomType == 3)
        {
            escorts[i].type = 'D';
            escorts[i].impactPower = 0.05;
            escorts[i].angleRange = 50;

            escorts[i].vmax =
                randomDouble(
                    0.60 * battleshipVmax,
                    0.95 * battleshipVmax);
        }
        else
        {
            escorts[i].type = 'E';
            escorts[i].impactPower = 0.04;
            escorts[i].angleRange = 70;

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

/*
   ATTACK STRATEGY

   1. Escort ships that can hit B have first priority.
   2. Higher impact power gets higher priority.
   3. If impact powers are equal,
      shorter enemy hit time gets priority.
*/

void buildAttackOrder(struct Battleship b,
                      struct Escort escorts[],
                      int n,
                      int order[])
{
    double enemyTime[MAX_ESCORTS];
    int enemyCanHit[MAX_ESCORTS];

    for (int i = 0; i < n; i++)
    {
        double velocity;
        double angle;
        double hitTime;

        double distance =
            calculateDistance(
                escorts[i].x,
                escorts[i].y,
                b.x,
                b.y);

        order[i] = i;

        enemyCanHit[i] =
            findShot(
                distance,
                escorts[i].vmin,
                escorts[i].vmax,
                escorts[i].angleMin,
                escorts[i].angleMax,
                &velocity,
                &angle,
                &hitTime);

        if (enemyCanHit[i])
        {
            enemyTime[i] =
                hitTime;
        }
        else
        {
            enemyTime[i] =
                INF;
        }
    }

    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0;
             j < n - 1 - i;
             j++)
        {
            int a =
                order[j];

            int c =
                order[j + 1];

            int swap =
                0;

            if (enemyCanHit[c] >
                enemyCanHit[a])
            {
                swap = 1;
            }

            else if (
                enemyCanHit[c] ==
                    enemyCanHit[a] &&
                escorts[c].impactPower >
                    escorts[a].impactPower)
            {
                swap = 1;
            }

            else if (
                enemyCanHit[c] ==
                    enemyCanHit[a] &&
                fabs(
                    escorts[c].impactPower -
                    escorts[a].impactPower)
                    < 0.000001 &&
                enemyTime[c] <
                    enemyTime[a])
            {
                swap = 1;
            }

            if (swap)
            {
                int temp =
                    order[j];

                order[j] =
                    order[j + 1];

                order[j + 1] =
                    temp;
            }
        }
    }
}

void saveInitialConditions(
    struct Battleship b,
    struct Escort escorts[],
    int n,
    double D,
    double TB)
{
    FILE *file =
        fopen(
            "part2a_initial_conditions.txt",
            "w");

    if (file == NULL)
    {
        return;
    }

    fprintf(file,
            "PART 2-A INITIAL CONDITIONS\n");

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

    fprintf(file,
            "Battleship firing interval T_B: %.2f seconds\n",
            TB);

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
                "Angles: %.2f - %.2f\n",
                escorts[i].angleMin,
                escorts[i].angleMax);

        fprintf(file,
                "Velocity: %.2f - %.2f\n",
                escorts[i].vmin,
                escorts[i].vmax);
    }

    fclose(file);
}

int strategyBattleStep(
    struct Battleship b,
    struct Escort escorts[],
    int n,
    double TB,
    double bAngleMin,
    double bAngleMax,
    int cumulativeMode,
    double *cumulativeImpact,
    FILE *out,
    FILE *orderFile,
    const char *label)
{
    int order[MAX_ESCORTS];

    double currentFireTime =
        0.0;

    buildAttackOrder(
        b,
        escorts,
        n,
        order);

    fprintf(orderFile,
            "\n%s\n",
            label);

    fprintf(orderFile,
            "B Position: (%.2f, %.2f)\n",
            b.x,
            b.y);

    fprintf(orderFile,
            "Attack order: ");

    for (int p = 0; p < n; p++)
    {
        int i =
            order[p];

        if (escorts[i].destroyed == 0)
        {
            fprintf(orderFile,
                    "E%d ",
                    escorts[i].id);
        }
    }

    fprintf(orderFile,
            "\n");

    for (int p = 0; p < n; p++)
    {
        int i =
            order[p];

        double bVelocity = 0;
        double bAngle = 0;
        double bTravelTime = 0;

        double eVelocity = 0;
        double eAngle = 0;
        double eTravelTime = 0;

        if (escorts[i].destroyed == 1)
        {
            continue;
        }

        double distance =
            calculateDistance(
                b.x,
                b.y,
                escorts[i].x,
                escorts[i].y);

        int bCanHit =
            findShot(
                distance,
                b.vmin,
                b.vmax,
                bAngleMin,
                bAngleMax,
                &bVelocity,
                &bAngle,
                &bTravelTime);

        int eCanHit =
            0;

        if (escorts[i].fired == 0)
        {
            eCanHit =
                findShot(
                    distance,
                    escorts[i].vmin,
                    escorts[i].vmax,
                    escorts[i].angleMin,
                    escorts[i].angleMax,
                    &eVelocity,
                    &eAngle,
                    &eTravelTime);
        }

        double bDestroyTime;

        if (bCanHit)
        {
            bDestroyTime =
                currentFireTime +
                bTravelTime;
        }
        else
        {
            bDestroyTime =
                INF;
        }

        double eHitTime;

        if (eCanHit)
        {
            eHitTime =
                currentFireTime +
                eTravelTime;
        }
        else
        {
            eHitTime =
                INF;
        }

        fprintf(out,
                "\nTarget E%d (E%c)\n",
                escorts[i].id,
                escorts[i].type);

        fprintf(out,
                "B firing time: %.2f seconds\n",
                currentFireTime);

        if (eCanHit &&
            eHitTime < bDestroyTime)
        {
            escorts[i].fired =
                1;

            fprintf(out,
                    "E%d hits B first at %.2f seconds\n",
                    escorts[i].id,
                    eHitTime);

            if (cumulativeMode)
            {
                *cumulativeImpact +=
                    escorts[i].impactPower;

                fprintf(out,
                        "Impact Power: %.2f\n",
                        escorts[i].impactPower);

                fprintf(out,
                        "Cumulative Impact: %.2f\n",
                        *cumulativeImpact);

                if (*cumulativeImpact >=
                    1.0)
                {
                    fprintf(out,
                            "Battleship B DESTROYED\n");

                    return 1;
                }
            }
            else
            {
                fprintf(out,
                        "Battleship B SUNK\n");

                return 1;
            }
        }

        if (bCanHit)
        {
            escorts[i].destroyed =
                1;

            escorts[i].fired =
                1;

            fprintf(out,
                    "B destroys E%d at %.2f seconds\n",
                    escorts[i].id,
                    bDestroyTime);

            fprintf(out,
                    "B shot angle: %.2f degrees\n",
                    bAngle);

            fprintf(out,
                    "B shot velocity: %.2f\n",
                    bVelocity);

            currentFireTime +=
                TB;
        }

        else if (eCanHit)
        {
            escorts[i].fired =
                1;

            if (cumulativeMode)
            {
                *cumulativeImpact +=
                    escorts[i].impactPower;

                fprintf(out,
                        "B cannot hit E%d.\n",
                        escorts[i].id);

                fprintf(out,
                        "E%d hits B.\n",
                        escorts[i].id);

                fprintf(out,
                        "Cumulative Impact: %.2f\n",
                        *cumulativeImpact);

                if (*cumulativeImpact >=
                    1.0)
                {
                    fprintf(out,
                            "Battleship B DESTROYED\n");

                    return 1;
                }
            }
            else
            {
                fprintf(out,
                        "B cannot hit E%d.\n",
                        escorts[i].id);

                fprintf(out,
                        "E%d sinks B.\n",
                        escorts[i].id);

                return 1;
            }
        }
    }

    fprintf(out,
            "\nBattleship B survives this step.\n");

    if (cumulativeMode)
    {
        fprintf(out,
                "Cumulative impact: %.2f\n",
                *cumulativeImpact);
    }

    return 0;
}

void runStationaryScenario(
    const char *filename,
    const char *label,
    struct Battleship b,
    struct Escort original[],
    int n,
    double TB,
    int cumulativeMode,
    FILE *orderFile)
{
    struct Escort escorts[MAX_ESCORTS];

    double cumulativeImpact =
        0.0;

    copyEscorts(
        escorts,
        original,
        n);

    FILE *file =
        fopen(filename, "w");

    if (file == NULL)
    {
        return;
    }

    fprintf(file,
            "%s\n",
            label);

    fprintf(file,
            "=========================================\n");

    fprintf(file,
            "T_B = %.2f seconds\n",
            TB);

    strategyBattleStep(
        b,
        escorts,
        n,
        TB,
        0.0,
        90.0,
        cumulativeMode,
        &cumulativeImpact,
        file,
        orderFile,
        label);

    if (cumulativeMode)
    {
        fprintf(file,
                "\nFinal cumulative impact on B: %.2f\n",
                cumulativeImpact);
    }

    fclose(file);
}

void runMovingScenario(
    const char *filename,
    const char *label,
    struct Battleship b,
    struct Escort original[],
    int n,
    double TB,
    int k,
    int t,
    double thetaMin,
    double pathX[],
    double pathY[],
    int jamMode,
    int cumulativeMode,
    FILE *orderFile)
{
    struct Escort escorts[MAX_ESCORTS];

    double cumulativeImpact =
        0.0;

    copyEscorts(
        escorts,
        original,
        n);

    FILE *file =
        fopen(filename, "w");

    if (file == NULL)
    {
        return;
    }

    fprintf(file,
            "%s\n",
            label);

    fprintf(file,
            "=========================================\n");

    fprintf(file,
            "T_B = %.2f seconds\n",
            TB);

    for (int step = 0;
         step < k;
         step++)
    {
        double bAngleMin =
            0.0;

        char stepLabel[100];

        b.x =
            pathX[step];

        b.y =
            pathY[step];

        if (jamMode &&
            step >= t)
        {
            bAngleMin =
                thetaMin;
        }

        fprintf(file,
                "\n------------ Iteration %d ------------\n",
                step + 1);

        fprintf(file,
                "B Position: (%.2f, %.2f)\n",
                b.x,
                b.y);

        fprintf(file,
                "B Angle Range: %.2f - 90.00\n",
                bAngleMin);

        snprintf(
            stepLabel,
            sizeof(stepLabel),
            "%s - Iteration %d",
            label,
            step + 1);

        if (strategyBattleStep(
                b,
                escorts,
                n,
                TB,
                bAngleMin,
                90.0,
                cumulativeMode,
                &cumulativeImpact,
                file,
                orderFile,
                stepLabel))
        {
            fprintf(file,
                    "\nSimulation stopped because B was destroyed.\n");

            break;
        }
    }

    if (cumulativeMode)
    {
        fprintf(file,
                "\nFinal cumulative impact on B: %.2f\n",
                cumulativeImpact);
    }

    fclose(file);
}

int main()
{
    struct Battleship battleship;

    struct Escort escorts[MAX_ESCORTS];

    double pathX[MAX_POINTS];
    double pathY[MAX_POINTS];

    int n;
    int k;
    int t;

    double D;
    double thetaMin;
    double TB;

    srand((unsigned int)time(NULL));

    printf("\n=========================================\n");
    printf("                PART 2-A\n");
    printf("=========================================\n");

    do
    {
        printf("Enter battlefield size D: ");
        scanf("%lf", &D);

    } while (D <= 0);

    do
    {
        printf("Enter number of escort ships (1-100): ");
        scanf("%d", &n);

    } while (n < 1 ||
             n > MAX_ESCORTS);

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

    } while (
        battleship.type != 'U' &&
        battleship.type != 'M' &&
        battleship.type != 'R' &&
        battleship.type != 'S');

    do
    {
        printf("Enter battleship X position (0 - %.2f): ",
               D);

        scanf("%lf",
              &battleship.x);

    } while (
        battleship.x < 0 ||
        battleship.x > D);

    do
    {
        printf("Enter battleship Y position (0 - %.2f): ",
               D);

        scanf("%lf",
              &battleship.y);

    } while (
        battleship.y < 0 ||
        battleship.y > D);

    do
    {
        printf("Enter battleship maximum shell velocity: ");

        scanf("%lf",
              &battleship.vmax);

    } while (
        battleship.vmax <= 0);

    battleship.vmin =
        0.0;

    do
    {
        printf("Enter battleship firing interval T_B (seconds): ");

        scanf("%lf",
              &TB);

    } while (
        TB <= 0);

    do
    {
        printf("Enter number of path points k (2-100): ");

        scanf("%d",
              &k);

    } while (
        k < 2 ||
        k > MAX_POINTS);

    do
    {
        printf("Enter jam iteration t (1 to %d): ",
               k - 1);

        scanf("%d",
              &t);

    } while (
        t < 1 ||
        t >= k);

    do
    {
        printf("Enter thetaMin (0 < thetaMin < 30): ");

        scanf("%lf",
              &thetaMin);

    } while (
        thetaMin <= 0 ||
        thetaMin >= 30);

    setupEscortShips(
        escorts,
        n,
        battleship.vmax,
        D);

    for (int i = 0;
         i < k;
         i++)
    {
        pathX[i] =
            randomDouble(0, D);

        pathY[i] =
            randomDouble(0, D);
    }

    saveInitialConditions(
        battleship,
        escorts,
        n,
        D,
        TB);

    FILE *orderFile =
        fopen(
            "part2a_attack_order.txt",
            "w");

    if (orderFile == NULL)
    {
        printf("Could not create attack order file.\n");
        return 1;
    }

    fprintf(orderFile,
            "PART 2-A ATTACK ORDER STRATEGY\n");

    fprintf(orderFile,
            "=========================================\n");

    fprintf(orderFile,
            "Priority: can-hit-B first, higher impact power, shorter hit time.\n");

    runStationaryScenario(
        "part2a_part1a.txt",
        "PART 2-A - PART 1-A STYLE",
        battleship,
        escorts,
        n,
        TB,
        0,
        orderFile);

    runMovingScenario(
        "part2a_part1b_sim1.txt",
        "PART 2-A - PART 1-B SIMULATION 1",
        battleship,
        escorts,
        n,
        TB,
        k,
        t,
        thetaMin,
        pathX,
        pathY,
        0,
        0,
        orderFile);

    runMovingScenario(
        "part2a_part1b_sim2.txt",
        "PART 2-A - PART 1-B SIMULATION 2",
        battleship,
        escorts,
        n,
        TB,
        k,
        t,
        thetaMin,
        pathX,
        pathY,
        1,
        0,
        orderFile);

    runStationaryScenario(
        "part2a_part1c.txt",
        "PART 2-A - PART 1-C STYLE",
        battleship,
        escorts,
        n,
        TB,
        1,
        orderFile);

    runMovingScenario(
        "part2a_part1c_sim1.txt",
        "PART 2-A - PART 1-C MOVING SIMULATION 1",
        battleship,
        escorts,
        n,
        TB,
        k,
        t,
        thetaMin,
        pathX,
        pathY,
        0,
        1,
        orderFile);

    runMovingScenario(
        "part2a_part1c_sim2.txt",
        "PART 2-A - PART 1-C MOVING SIMULATION 2",
        battleship,
        escorts,
        n,
        TB,
        k,
        t,
        thetaMin,
        pathX,
        pathY,
        1,
        1,
        orderFile);

    fclose(orderFile);

    printf("\n=========================================\n");
    printf("          PART 2-A COMPLETED\n");
    printf("=========================================\n");

    printf("Output files created.\n");
    printf("Attack order saved to part2a_attack_order.txt\n");

    return 0;
}

