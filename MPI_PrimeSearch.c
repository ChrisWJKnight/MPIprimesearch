#define LOW 1 //Starting Number
#define HIGH 1100000 //End Number
# include <math.h>//maths library
# include <mpi.h>//MPI library
# include <stdio.h>
# include <stdlib.h>
# include <time.h>//used for time stamps
int findPrime(int n);//functions declared before main
void showTime();
int main(int argc, char** argv) {
    int coreId; //create variables used
    int cores;
    int task;
    int n;
    int nMax;
    int checkLow;
    int checkHigh;
    int send[2];
    int primes;
    int primeCheck;
    int primeReturn[2];
    int highPrime;
    int coreFound;
    int primesFound;
    int totalPrimes;
    int totalHigh;
    int jobId;
    int holder;
    int increment;
    double timeTaken;
    n = LOW; //declare variables
    jobId = 1;
    highPrime = 0;
    primesFound = 0;
    totalHigh = 0;
    totalPrimes = 0;

    task = MPI_Init(NULL, NULL); // start the MPI system
    if (task != 0) { //if MPI fails to start
        printf("\n");//print error message
        printf("Error, Please check MPI settings\n");
        exit(1);
    }
    task = MPI_Comm_size(MPI_COMM_WORLD, &cores);// Get number of Cores
    task = MPI_Comm_rank(MPI_COMM_WORLD, &coreId);// Get core rank / ID
    increment = HIGH / (cores - 1);
    if (coreId == 0) { //First core is used to print messages and send tasks to other cores
        showTime();
        printf("\nCounting number of Primes and finding highest prime number between %d and %d on %d cores\n", n, HIGH,cores);
    }
    timeTaken = MPI_Wtime(); //begin timer
    if (coreId == 0) {
        nMax = increment;
        for (jobId; jobId < cores;jobId++) { //runs through task until maximum number reached
            send[0] = n;
            send[1] = nMax;
            MPI_Send(&send, 2, MPI_INT, jobId, 0, MPI_COMM_WORLD);//send task to next node
            n = n + increment;//increments next value to be checked
            nMax = nMax + increment;
        }
    }
    else {
        MPI_Recv(&send, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);//receive task
        checkLow = send[0];
        checkHigh = send[1];
        while (checkLow <= checkHigh)
        {
            primes = findPrime(checkLow);//use function to check if value of n is prime number, store in "primes"
            if (primes == 0) {//if prime number found
                primesFound++;//increment number of primes found
                if (highPrime <= checkLow) {//checks if prime is highest found
                }
            }
            checkLow++;
        }
    }
    if (coreId != 0) {
        primeReturn[0] = highPrime;
        primeReturn[1] = primesFound;
        MPI_Send(&primeReturn, 2, MPI_INT, 0, 0, MPI_COMM_WORLD);//send value of primes back to core 0
    }
      if (coreId == 0) {
            for (int i = 1; i <= cores-1; i++) {
                MPI_Recv(&primeReturn, 2, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);// core 0 receives reply
                totalPrimes = totalPrimes + primeReturn[1];
                if (totalHigh <= primeReturn[0]) {
                    totalHigh = primeReturn[0];
                }
            }
        }
    if (coreId == 0) {
        timeTaken = MPI_Wtime() - timeTaken;//takes initial timer from final timer
        printf("\nFinished in %.1f Seconds\n", timeTaken);//displays time taken
        printf("\n%d Prime Numbers Found\n\nHighest Prime found - %d\n", totalPrimes, totalHigh); //prints number of primes found and highest prime found
        printf("\n");//adds spacer
    }
    MPI_Finalize();// close MPI system
}
int findPrime(int n) {//checks for prime number
    int i = 0; //resets variables
    int flag = 0;
    if (n == 1) {
        flag = 1;
    }
    else {
        for (i = 2; i <= n / 2; ++i) {//loop checks n vs half of numbers before n starting at 2
            if (n % i == 0) {// condition for non-prime, checks for remainder
                flag = 1;//if not prime set flag to 1
                break;//stop loop if non prime found
            }
        }
    }
    return flag; //returns result
}
void showTime() {//function for printing timestamp
    static char time_buffer[40];//variable to store time code
    const struct tm* timeStamp;//create struct to store time stamp
    time_t currentTime;//create struct variable
    currentTime = time(NULL);//get timestamp
    timeStamp = localtime(&currentTime);//convert current time to local time
    strftime(time_buffer, 40, "%d %B %Y %I:%M:%S %p", timeStamp);//convert time to correct format
    printf("\n%s\n", time_buffer);//print time/date
    return;//finish function
}