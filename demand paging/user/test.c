#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"


int main(void){
  printf("TEST...\n");
//  getPA(0);
//  int *arr = (int *)malloc(10* sizeof(int*));
//
//  int *arr1 = (int *)malloc(10* sizeof(int*));
//  printf("address: %p\n", arr);
//  uint64 first = (uint64)getPA(arr);
//  uint64 second = (uint64)getPA(arr1);
//  free(arr);
//  free(arr1);
//
//  printf("1: %p\n", (void*)first);
//  printf("2: %p\n", (void*)second);
//
//  getPA(0);
  
  uint64 *arr = (uint64*)malloc(10*sizeof(uint64));
  getPA(&arr);
  free(arr);
//  printf("arr[0] = 100\n");
//  arr[0] = 100;
  
  uint64* arr1 = malloc(10*sizeof(uint64));
  getPA(arr1);
  free(arr1);
  
  return 0;
}
