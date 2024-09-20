/*
 * catsem.c
 *
 * 30-1-2003 : GWA : Stub functions created for CS161 Asst1.
 *
 * NB: Please use SEMAPHORES to solve the cat syncronization problem in 
 * this file.
 */


/*
 * 
 * Includes
 *
 */

#include <types.h>
#include <lib.h>
#include <synch.h>
#include <test.h>
#include <thread.h>

/*
* Boolean implementation
*/

typedef int bool;
#define TRUE 1
#define FALSE 0

struct semaphore *mutex;

/* Example mutex = sem_create("cat sem mutex", 1);*/

/*
 * 
 * Constants
 *
 */
volatile bool all_dishes_available; /* how many dishes are being eaten from right now?*/
volatile bool dish1_busy;
volatile bool dish2_busy;
struct semaphore *done;
struct semaphore *dish_mutex;
struct semaphore *cats_queue;
volatile int cats_wait_count;
volatile bool no_cat_eat; /*First cat*/
struct semaphore *mice_queue;
volatile int mice_wait_count;
volatile bool no_mouse_eat; /*First mouse*/

//initialize in catmousesem, declare here


/*
 * Number of food bowls.
 */

#define NFOODBOWLS 2

/*
 * Number of cats.
 */

#define NCATS 6

/*
* Number of times they can eat
*/
#define NEAT 5

/*
 * Number of mice.
 */

#define NMICE 2


static const char* const mousename[NMICE] = {
"Jerry",
"Jim"
};

static const char* const catname[NCATS] = {
"Tom",
"Princess",
"Fluffy",
"Oreo",
"Tabby",
 "Echo"
 };

/*
* Global Variables
*/

/*
 * 
 * Function Definitions
 * 
 */


//Play around and eat food

#define MAXTIME 3 /* You may change 3 to any number */
//clocksleep(random() % MAXTIME);

/*
 * catsem()
 *
 * Arguments:
 *      void * unusedpointer: currently unused.
 *      unsigned long catnumber: holds the cat identifier from 0 to NCATS - 1.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Write and comment this function using semaphores.
 *
 */

static
void
catsem(void * unusedpointer, 
       unsigned long catnumber)
{
        /*
         * Avoid unused variable warnings.
         */

        (void) unusedpointer;
        (void) catnumber;
        bool first_cat_eat = FALSE; /* Local - Who entered the kitchen first? */
        bool another_cat_eat = FALSE; /* Local - who entered the kitchen second? */
        int mydish = 0;
        

        //First cat, no mouse
        P(mutex); //Entering CS
        if (all_dishes_available == TRUE) { //All dishes available
                all_dishes_available = FALSE;
                V(cats_queue); /*Let first cat in*/
        }
        cats_wait_count++; //Now Ping in line
        V(mutex); //Exiting CS

        P(cats_queue); //Entering CS
        if (no_cat_eat == TRUE) { //First cat in the room
                no_cat_eat = FALSE;
                first_cat_eat = TRUE; //I'm first
        }
        else { first_cat_eat = FALSE; } //Someones already in there

        if (first_cat_eat == TRUE) {
                P(mutex); /* First cat is in the kitchen */
                if (cats_wait_count > 1) {
                        another_cat_eat = TRUE; /* Let antoher cat in */
                        V(cats_queue);
                }
                V(mutex);
        }

        kprintf("Cat %s is in the kitchen.\n", catname[catnumber]);
        
        /* Both cats are in the kitchen now they want to eat */
        P(dish_mutex); //Entering CS
        if(dish1_busy == FALSE) {
                dish1_busy = TRUE; //Start eating at dish 1
                mydish = 1;
        }
        else {
                assert(dish2_busy == FALSE) 
                dish2_busy = TRUE; //Start eating at dish 2
                mydish = 2;
            
        }
        V(dish_mutex); //Exiting CS

         kprintf("Cat %s is eating.\n", catname[catnumber]);
	 int i;
         for(i = 0; i < NEAT; i++) { //Eat 5 times
         clocksleep(random() % MAXTIME); //simulate eating
         }
         kprintf("Cat %s finished eating.\n", catname[catnumber]);

        P(dish_mutex); //Enter CS
        if (mydish == 1) /* Release dish 1*/ {
                dish1_busy = FALSE;
        }
        else { /* Release dish 2*/
                dish2_busy = FALSE;
        }
        V(dish_mutex); //Exiting CS

        P(mutex); /*Entering CS*/
        cats_wait_count--; //One cat processed
        V(mutex);

        //First cat is now leaving the kitchen
        if(first_cat_eat == TRUE) {
                if (another_cat_eat == TRUE) {
                        P(done); /* Wait for other cat to finish*/
                }

                kprintf("First cat %s is leaving.\n", catname[catnumber]);
                no_cat_eat = TRUE;

                        /* See if any mice are waiting*/
                P(mutex); //Entering CS
                if (mice_wait_count > 0) {
                       V(mice_queue);
                }
                else if (cats_wait_count > 0) {
                        V(cats_queue); /* no mice so let the cats eat again */
                }
                else {
                       all_dishes_available = TRUE; /* No one is hungry*/
                }
                V(mutex);
        }
        else { /* Not the first cat */
                kprintf("Non first cat %s is leaving", catname[catnumber]);
                V(done);
        }
}
        

/*
 * mousesem()
 *
 * Arguments:
 *      void * unusedpointer: currently unused.
 *      unsigned long mousenumber: holds the mouse identifier from 0 to 
 *              NMICE - 1.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Write and comment this function using semaphores.
 *
 */

static
void
mousesem(void * unusedpointer, 
         unsigned long mousenumber)
{
        /*
         * Avoid unused variable warnings.
         */

        (void) unusedpointer;
        (void) mousenumber;

        bool first_mouse_eat = FALSE; /* Local - Who entered the kitchen first? */
        bool another_mouse_eat = FALSE; /* Local - who entered the kitchen second? */
        int mydish = 0;
        

        //First mouse, no cat
        P(mutex); //Entering CS
        if (all_dishes_available == TRUE) { //All dishes available
                all_dishes_available = FALSE;
                V(mice_queue); /*Let first mouse in*/
        }
        mice_wait_count++; //Now Ping in line
        V(mutex); //Exiting CS

        P(mice_queue); //Entering CS
        if (no_mouse_eat == TRUE) { //First mouse in the room
                no_mouse_eat = FALSE;
                first_mouse_eat = TRUE; //I'm first
        }
        else {first_mouse_eat = FALSE; } //Someones already in there

        if (first_mouse_eat == TRUE) {
                P(mutex); /* First mouse is in the kitchen */
                if (mice_wait_count > 1) {
                        another_mouse_eat = TRUE; /* Let antoher mouse in */
                        V(mice_queue);
                }
                V(mutex);
        }
        kprintf("Mouse %s is in the kitchen.\n", mousename[mousenumber]);
        
        /* Both mice are in the kitchen now they want to eat */
        P(dish_mutex); //Entering CS
        if(dish1_busy == FALSE) {
                dish1_busy = TRUE; //Start eating at dish 1
                mydish = 1;
        }
        else {
                assert(dish2_busy == FALSE) {
                        dish2_busy = TRUE; //Start eating at dish 2
                        mydish = 2;
                }
        }
        V(dish_mutex); //Exiting CS

         kprintf("Mouse %s is eating.\n", mousename[mousenumber]);
	 int i;
         for(i = 0; i < NEAT; i++) {
                clocksleep(random() % MAXTIME); //simulate eating
         }
         kprintf("Mouse %s is finished eating.\n", mousename[mousenumber]);

        P(dish_mutex); //Enter CS
        if (mydish == 1) /* Release dish 1*/ {
                dish1_busy = FALSE;
        }
        else { /* Release dish 2*/
                dish2_busy = FALSE;
        }
        V(dish_mutex); //Exiting CS

        P(mutex); /*Entering CS*/
        mice_wait_count--; //One mouse processed
        V(mutex);

        //First mouse is now leaving the kitchen
        if(first_mouse_eat == TRUE) {
                if (another_mouse_eat == TRUE) {
                        P(done); /* Wait for other mouse to finish*/
                }

                kprintf("First mouse %s is leaving.\n", mousename[mousenumber]);
                no_mouse_eat = TRUE;

                        /* See if any mice are waiting*/
                P(mutex); //Entering CS
                if (mice_wait_count > 0) {
                       V(mice_queue);
                }
                else if (cats_wait_count > 0) {
                        V(cats_queue); /* no mice so let the cats eat */
                }
                else {
                        all_dishes_available = TRUE; /* No one is hungry*/
                }
                V(mutex);
        }
        else { /* Not the first mouse */
                kprintf("Non first mouse %s is leaving", mousename[mousenumber]);
                V(done);
        }
}


/*
 * catmousesem()
 *
 * Arguments:
 *      int nargs: unused.
 *      char ** args: unused.
 *
 * Returns:
 *      0 on success.
 *
 * Notes:
 *      Driver code to start up catsem() and mousesem() threads.  Change this 
 *      code as necessary for your solution.
 */

int
catmousesem(int nargs,
            char ** args)
{
        int index, error;
        mutex = sem_create("General Mutex", 1);

        all_dishes_available = TRUE; /* how many dishes are being eaten from right now?*/
        done = sem_create("done", 0);
        dish_mutex = sem_create("dish mutex", 1);
        cats_queue = sem_create("cats queue", 0);
        cats_wait_count = 0;
        no_cat_eat = TRUE; /*First cat*/
        mice_queue = sem_create("mice queue", 0);
        mice_wait_count = 0;
        no_mouse_eat = TRUE;
        dish1_busy = FALSE;
        dish2_busy = FALSE;
        

       
        /*
         * Avoid unused variable warnings.
         */

        (void) nargs;
        (void) args;
   
        /*
         * Start NCATS catsem() threads.
         */

        for (index = 0; index < NCATS; index++) {
           
                error = thread_fork("catsem Thread", 
                                    NULL, 
                                    index, 
                                    catsem, 
                                    NULL
                                    );
                
                /*
                 * panic() on error.
                 */

                if (error) {
                 
                        panic("catsem: thread_fork failed: %s\n", 
                              strerror(error)
                              );
                }
        }
        
        /*
         * Start NMICE mousesem() threads.
         */

        for (index = 0; index < NMICE; index++) {
   
                error = thread_fork("mousesem Thread", 
                                    NULL, 
                                    index, 
                                    mousesem, 
                                    NULL
                                    );
                
                /*
                 * panic() on error.
                 */

                if (error) {
         
                        panic("mousesem: thread_fork failed: %s\n", 
                              strerror(error)
                              );
                }
        }

}


/*
 * End of catsem.c
 */

