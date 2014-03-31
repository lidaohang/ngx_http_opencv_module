ngx_http_opencv_module
======================

nginx opencv image resize


1.安装opencv：

a.opencv所依赖的一些包：（部分lib已安装，如需要安装通过yum install libpng,或者自行下载，相关软件放入软件包，此步骤可先省略）
	
	yum install -y pkgconfig  libpng  zlib libjpeg  libtiff cmake
	

b.cmake安装(opencv官方建议cmake最好在cmake-2.8.x以上)：

	cmake-2.8.9.tar.gz
	
	tar –zxvf cmake-2.8.9
	
	cd cmake-2.8.9
	
	./configure –-prefix=/usr/local/cmake
	
	make && make install
	
	cp -r /usr/local/cmake/bin/ /usr/bin/
	

c.下载安装：

	wget http://aarnet.dl.sourceforge.net/project/opencvlibrary/opencv-unix/2.4.2/OpenCV-2.4.1.tar.bz2
	
	tar –xvf OpenCV-2.4.1.tar.bz2
	
	cd OpenCV-2.4.1
	
	(确认cmake必须安装)
	
	cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local/opencv .
	
	make && make install
	

d.环境变量配置：

	vim /etc/ld.so.conf
	
	最后一行加入
	/usr/local/opencv/lib/
	
e.复制到系统目录：

	cp -r /usr/local/opencv/include/ /usr/local/include/
	

f.生效：

	ldconfig
	
	
2.安装nginx:


./configure --prefix=/usr/local/nginx --add-module=module/ngx_http_opencv_module




nginx.conf


 location / {
            autohome_dis_opencv;
        }


ll /tmp/images_data/vendor/o/201308/15/a.jpg




127.0.0.1/vendor/o/201308/15/800x600_87_1_a.jpg

说明：

800x600_60_1_201208212013510914122.jpg

800x600 要压缩的图像尺寸(可随意调整)

60 图像质量参数(可随意调整)

1 水印
