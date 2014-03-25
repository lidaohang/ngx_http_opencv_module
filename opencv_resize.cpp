#include "opencv_resize.h"
#include <cv.h>
#include <highgui.h>

using namespace cv;

OpenCvCompressPicture::OpenCvCompressPicture(void)
{
}


OpenCvCompressPicture::~OpenCvCompressPicture(void)
{

}

void OpenCvCompressPicture::resize_image(image_file_t * images)
{
	string fullpath("/tmp/images_data");
	fullpath.append(images->path);

	IplImage* source =cvLoadImage(fullpath.c_str());	

	if(source == NULL)
	{
		return;
	}

	IplImage * dst = NULL;
	dst = cvCreateImage(cvSize(images->width,images->height), source->depth, source->nChannels);    
	cvResize(source, dst, CV_INTER_AREA);                        
	Mat img = cv::cvarrToMat(dst, true);
	cvReleaseImage(&dst);
	cvReleaseImage(&source);

	int params[3] = {0};
	params[0] = CV_IMWRITE_JPEG_QUALITY; 
	params[1] = images->quality; 

	cv::vector<u_char> buf;
	cv::imencode(".jpg", img, buf, std::vector<int>(params, params+2));
	u_char* result = reinterpret_cast<u_char *> (&buf[0]);

	images->len=buf.size();
	images->data=(u_char *)malloc(sizeof(u_char)*buf.size());
	memcpy(images->data, result, images->len);
	
	//�ͷ��ڴ�
	cv::vector<u_char> b;
	buf.swap(b);

	images->code=200;
	/*FILE* target = fopen("resultl.jpg", "wb");
	fwrite(images->data, sizeof(u_char),images->len, target);
	fclose(target);	*/
}


extern "C"

void nginx_module_invoke_opencv_resize(empty_struct * p,image_file_t * images)
{
    OpenCvCompressPicture * urlParser = (OpenCvCompressPicture *)p;
	urlParser->resize_image(images);
}
