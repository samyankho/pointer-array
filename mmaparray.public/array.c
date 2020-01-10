/***********************************************************************/
/**      Author: Xunyuan He                                    **/
/**        Date: Jul. 2019                                            **/
/** Description: Assgn. III                                           **/
/***********************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "array.h"
#include "functions.h"

/**********************************************************************/
/********************       O P E N  A R R A Y     ********************/
/**********************************************************************/
void open_array(char *filename, array_t *arrayp, int *sizep) {                
    /* Opens array from file filename and */
    /* returns its pointer and size */
    /* size is the size of the file divided by the size of the array struct */
    /* pointer array is obtained with mmap */
    int fd, i;
    struct stat ss;

    if ((fd = open(filename, O_RDWR)) < 0) { //open file
        fatalerr(filename, errno, "File not exists");
    }
    if (fstat(fd, &ss) == -1) {
        fatalerr(filename, errno, "File state error");
    }

    *arrayp = mmap(NULL, ss.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (arrayp == MAP_FAILED) {
        fatalerr(filename, errno, "Map memory error");
    }
    close(fd);
    *sizep = ss.st_size / sizeof(**arrayp);
    return;
}

/**********************************************************************/
/********************     C L O S E  A R R A Y     ********************/
/**********************************************************************/
void close_array(array_t *arrayp, int size) {
    /* Unmaps array and sets it to NULL */
  if (munmap(*arrayp, size * sizeof(**arrayp)) < 0)
    fatalerr("close_array", -1, "Unable to unmap memory");
  arrayp[0] = NULL;
}

/**********************************************************************/
/********************    C R E A T E  A R R A Y    ********************/
/**********************************************************************/
void create_array(char *filename, int index, array_t *arrayp){                
    /* Creates a file with an array with index */
    /* elements, all with the valid member false. */
    /* It is fatal error if file exists */
    /* size is the size of the file divided by the size of the array struct */
    /* pointer array is obtained with mmap */
    int fd, i;
    struct stat ss;

    //open file
    if ((fd = open(filename, O_RDWR | O_CREAT, 0664)) < 0) {
        fatalerr(filename, errno, "Could not open file");
    }
    if (fstat(fd, &ss) == -1) {
        fatalerr(filename, errno, "File state error");
    }

   long length = index * sizeof(**arrayp);

    if (ftruncate(fd, length) < 0){
    fatalerr(filename, errno, "File truncate error");    
    }

    *arrayp = mmap(NULL, ss.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (arrayp == MAP_FAILED) {
        fatalerr(filename, errno, "map memory error");
    }
      for (i = 0; i < index; ++i){
          arrayp[0][i].index = i;
          arrayp[0][i].valid = 0;
  }
    close(fd);

    return;
}


/**********************************************************************/
/********************       S E T  E N T R Y       ********************/
/**********************************************************************/
void set_entry(array_t array, char *name, int index, float age) {                
    /* Sets the elements of the index'ed */
    /* entry to name and age and the valid */
    /* member to true */
    array[index].valid = 1;
    array[index].age = age;
    memset(array[index].name, 0, sizeof(array[index].name));
    strcpy(array[index].name, name);
}

/**********************************************************************/
/********************       G E T  E N T R Y       ********************/
/**********************************************************************/
void get_entry(array_t array, char **name, int index, float *age) {
    /* Returns the index'ed entry's name and age */
    /* if valid, fatal error o/w */

    // We have to malloc, otherwise this will make merroy leak
    *name = (char *) malloc(sizeof(char) * MAXNAME);

    if (array[index].valid) {
        strcpy(*name, array[index].name);
        *age = array[index].age;
    } else {
        fatalerr("index", 0, "the entry is invaild");
    }
}

/**********************************************************************/
/********************   D E L E T E   E N T R Y    ********************/
/**********************************************************************/
void delete_entry(array_t array, int index) {
    /* Sets the valid element of the index'ed */
    /* entry to false */
    array[index].valid = 0;
}

/**********************************************************************/
/********************     P R I N T   A R R A Y    ********************/
/**********************************************************************/
void print_array(array_t array, int size) {
    /* Prints all entries with valid member true */
    /* using the same format as in the main */
    int i;
    for (i = 0; i < size; ++i) {
        if (array[i].valid)
            printf("index: %d, name: %s, age: %.6f\n", i, array[i].name, array[i].age);
    }

}