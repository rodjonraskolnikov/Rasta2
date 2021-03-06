#ifndef _match_h_
#define _match_h_ 1
#include "jhead.h"

#define PDF_NOT_FOUND "PDFNotFound"
//AUMENTARE
#define CROP_DIM	1500
#define RESULTS_NUMBER	20
#define NUMBER_OF_TRIES 20


extern int getTextCircledPosition( char* pdf_image_name,char* photo_name,int* tlx,int* tly,int* width,int* height);
extern void getImageDate(char* name,char* time);
extern void parseText(char* text);
extern char* findPdfFileInDB(char* test_image,int* tlx,int* tly,int* width,int* height,int *page_number);

#endif
