//______________________________________________________________________________________
// Program : Corner Detection using OpenCV
// Author  : Bharath Prabhuswamy
//______________________________________________________________________________________
#include <cv.h>
#include <highgui.h>
#include <stdio.h>

bool find_corner(char* img_data,int img_width,int x,int y);	// Routine to check whether a particular pixel is an Edgel or not

// Start of Main Loop
//------------------------------------------------------------------------------------------------------------------------
int main()
{
	CvCapture* capture = 0;
	IplImage* img = 0;

	capture = cvCaptureFromCAM( 0 );
		if ( !capture )           	// Check for Camera capture
		return -1;

	cvNamedWindow("Camera",CV_WINDOW_AUTOSIZE);

	IplImage* gray = 0;
	IplImage* thres = 0;
	IplImage* prcs_flg = 0;                  //Process flag to flag whether the current pixel is already processed as part of the blob or not

	
	bool init = false;			// Flag to identify initialization of Image objects

    if(init == false)
	{
		img = cvQueryFrame( capture );	// Query for the frame

        	if( !img )			// Exit if camera frame is not obtained
			return -1;

		// Creation of Intermediate 'Image' Objects required later
		gray = cvCreateImage( cvGetSize(img), 8, 1 );		// To hold Grayscale Image
		thres = cvCreateImage( cvGetSize(img), 8, 1 );		// To hold OTSU thresholded Image

		prcs_flg = cvCreateImage( cvGetSize(img), 8, 1 );	// To hold Map of 'per Pixel' Flag to keep track while identifing Blobs

		
		init = true;
	}

  	
	int ihist[256];     	    // Array to store Histogram values
	float hist_val[256];		// Array to store Normalised Histogram values
	int pos ;					// Position or pixel value of the image
	float prbn;					// First order cumulative
	float meanitr;				// Second order cumulative
	float meanglb;              // Global mean level
	int OPT_THRESH_VAL;         // Optimum threshold value
	float param1,param2;        // Parameters required to work out OTSU threshold algorithm
	double param3;
	int h,w;				// Variables to store Image Height and Width
	
	h = img->height;		// Height and width of the Image
	w = img->width;

	int key = 0;
	while(key != 'q')
	{
		//Step	: Capture Image from Camera
		//Info	: Inbuit function from OpenCV
		//Note	: 

		img = cvQueryFrame( capture );		// Query for the frame

		//Step	: Convert Image captured from Camera to GrayScale
		//Info	: Inbuit function from OpenCV
		//Note	: Image from Camera and Grayscale are held using seperate "IplImage" objects

		cvCvtColor(img,gray,CV_RGB2GRAY);	// Convert RGB image to Gray


		//Step	: Threshold the image using optimum Threshold value obtained from OTSU method
		//Info	: 
		//Note	: 

		memset(ihist, 0, 256);

		for(int j = 0; j < gray->height; ++j)	// Use Histogram values from Gray image
		{
			uchar* hist = (uchar*) (gray->imageData + j * gray->widthStep);
			for(int i = 0; i < gray->width; i++ )
			{
				pos = hist[i];		// Check the pixel value
				ihist[pos] += 1;	// Use the pixel value as the position/"Weight"
			}
		}

		//Parameters required to calculate threshold using OTSU Method
		prbn = 0.0;                   // First order cumulative
		meanitr = 0.0;                // Second order cumulative
		meanglb = 0.0;                // Global mean level
		OPT_THRESH_VAL = 0;             // Optimum threshold value
		param1,param2;                // Parameters required to work out OTSU threshold algorithm
		param3 = 0.0;

		//Normalise histogram values and calculate global mean level
		for(int i = 0; i < 256; ++i)
		{
			hist_val[i] = ihist[i] / (float)(w * h);
			meanglb += ((float)i * hist_val[i]);
		}

		// Implementation of OTSU algorithm
		for (int i = 0; i < 255; i++)
		{
			prbn += (float)hist_val[i];
			meanitr += ((float)i * hist_val[i]);

			param1 = (float)((meanglb * prbn) - meanitr);
			param2 = (float)(param1 * param1) /(float) ( prbn * (1.0f - prbn) );

			if (param2 > param3)
			{
			    param3 = param2;
			    OPT_THRESH_VAL = i; 				// Update the "Weight/Value" as Optimum Threshold value
			}
		}

		cvThreshold(gray,thres,OPT_THRESH_VAL,255,CV_THRESH_BINARY);	//Threshold the Image using the value obtained from OTSU method

		int c = 0;	

		for( int y = 0; y < thres->height; ++y)	//Start full scan of the image by incrementing y
		{

			for(int x = 0; x < thres->width; ++x )	//Start full scan of the image by incrementing x
		    	{
								// Number of edgels in a particular blob
				corner_flag = find_corner(thres->imageData,thres->widthStep,x,y);
				if(corner_flag == true)		// Check for the Edgel and update Edgel storage
				{
					c++;
					corner.x = x;
					corner.y = y;
					cvCircle(img,corner,1,CV_RGB(255,0,0),1,8);
					//cvCircle(img,corner,8,CV_RGB(128,255,128),1,8);

					corner_flag = false;
				}
			}
			       
		}
		
		cvShowImage( "Camera",img);

		key = cvWaitKey(1);	// OPENCV: wait for 1ms before accessing next frame

	} // End of while loop

	cvDestroyWindow( "Camera" );	// Release various parameters
	cvReleaseImage(&img);
	cvReleaseImage(&gray);
	cvReleaseImage(&thres);

    return 0;
}
// End of Main Loop
//------------------------------------------------------------------------------------------------------------------------


// Routine to check whether a particular pixel is an Edgel or not
bool find_corner(char* img_data,int img_width,int x,int y)
{	
	const int wind_sz = 5 ;
	int wind_bnd = (wind_sz - 1) / 2;
	int sum = 0;
	bool result = false;
	uchar* ptr[wind_sz];
	int index =0;

	for(int k = (0-wind_bnd); k <= wind_bnd; ++k)
	{
		 ptr[index] = (uchar*)(img_data + (y + k) *  img_width);
		 index = index + 1 ;
	}

	for(int i = 0; i <= (wind_sz-1); ++i)
	{
		if((i == 0) || (i==(wind_sz-1)))
		{
		    for (int j = (0-wind_bnd); j <= wind_bnd; ++j)
		    {
			if(ptr[i][x+j] == 0)
			   sum += 1;
			else
			   continue;
		    }
		}
		else
		{
		    if(ptr[i][x-wind_bnd] == 0)
			sum += 1;
		    else
			continue;

		    if(ptr[i][x+wind_bnd] == 0)
			sum += 1;
		    else
			continue;
		}
	}

    if((sum > 4 ) && (sum < 8))
    {
        result = true;
    }
    return result;
}





