#ifndef _sift_template_h_
#define _sift_template_h_ 1
#include "sift.h"
#include "imgfeatures.h"
#include "kdtree.h"
#include "utils.h"
#include "xform.h"

#include "template_extractor.h"

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include <stdio.h>

/* the maximum number of keypoint NN candidates to check during BBF search */
#define KDTREE_BBF_MAX_NN_CHKS 200

/* threshold on squared ratio of distances between NN and 2nd NN */
#define NN_SQ_DIST_RATIO_THR 0.49


void getTemplatePositionFromImage(IplImage* img1,IplImage *img2,CvPoint* topLeft,CvPoint* bottomRight){
/*calculate the upper left corner and the bottom right corner of the area in img2 that relates to img1.*/
  IplImage * stacked;
  struct feature* feat1, * feat2, * feat;
  struct feature** nbrs;
  struct kd_node* kd_root;
  CvPoint pt1, pt2;
  double d0, d1;
  int n1, n2, k, i, m = 0;

  stacked = stack_imgs( img1, img2 );

  printf("Cerco features nel template\n");
  n1 = sift_features( img1, &feat1 );// extract feature from template img1
  printf("Cerco features nell'immagine\n");
  n2 = sift_features( img2, &feat2 );// extract feature from image img2
  kd_root = kdtree_build( feat2, n2 ); //create a kdtree (i.e. a multiscale analysis of image img2)
  for( i = 0; i < n1; i++ )
    {
      feat = feat1 + i;
      k = kdtree_bbf_knn( kd_root, feat, 2, &nbrs, KDTREE_BBF_MAX_NN_CHKS ); // finds an image feat'approximate k nearest neighbors in a kd tree using best bin first search.
      if( k == 2 )
	{
	  d0 = descr_dist_sq( feat, nbrs[0] );//calculate distance between feature and a nearest sift calculate previously
	  d1 = descr_dist_sq( feat, nbrs[1] );// calculate the other
	  if( d0 < d1 * NN_SQ_DIST_RATIO_THR )
	    {
	      pt1 = cvPoint( cvRound( feat->x ), cvRound( feat->y ) );
	      pt2 = cvPoint( cvRound( nbrs[0]->x ), cvRound( nbrs[0]->y ) );
	      pt2.y += img1->height;
	      cvLine( stacked, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );
	      m++;
	      feat1[i].fwd_match = nbrs[0];
	    }
	}
      free( nbrs );
    }

  fprintf( stderr, "Found %d total matches\n", m );
//  display_big_img( stacked, "Matches" );
 // cvWaitKey( 0 );

  /* 
     UNCOMMENT BELOW TO SEE HOW RANSAC FUNCTION WORKS
     
     Note that this line above:
     
     feat1[i].fwd_match = nbrs[0];
     
     is important for the RANSAC function to work.
  */
  

    CvMat* H;
    IplImage* xformed;
    H = ransac_xform( feat1, n1, FEATURE_FWD_MATCH, lsq_homog, 4, 0.01,
		      homog_xfer_err, 3.0, NULL, NULL ); //calculate the geometric transformate with respect to sift
    if( H )
      {
	xformed = cvCreateImage( cvGetSize( img2 ), IPL_DEPTH_8U, 3 );
	cvWarpPerspective( img1, xformed, H, //apply the geometric transformate with respect to sift
			   CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS,
			   cvScalarAll( 0 ) );

	///AGGIUNTO
	CvPoint br,tl;	
	IplImage *im_gray = cvCreateImage(cvGetSize(xformed),IPL_DEPTH_8U,1);
	cvCvtColor(xformed,im_gray,CV_RGB2GRAY);
	bn_get_containing_box_coordinates(im_gray,topLeft,bottomRight);
	cvReleaseImage( &im_gray);
	///		
	

	cvReleaseImage( &xformed );
	cvReleaseMat( &H );
      }
      
  

  cvReleaseImage( &stacked );
  cvReleaseImage( &img1 );
  cvReleaseImage( &img2 );
  kdtree_release( kd_root );
  free( feat1 );
  free( feat2 );

}
#endif
