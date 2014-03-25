#pragma once

#ifndef OPENCV_RESIZE_H_
#define OPENCV_RESIZE_H_

#include "image_file_block.h"

	
class OpenCvCompressPicture 
{
public:
	OpenCvCompressPicture();
		~OpenCvCompressPicture();

	void resize_image(image_file_t * retVal);
			
};


#endif	//DIS_DATA_OPENCV_COMPRESS_PICTURE_H_
