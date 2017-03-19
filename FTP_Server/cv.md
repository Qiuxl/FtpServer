##  
###基本知识  
1.mat的初始化方法  
* imread(failepath,int flag); // 当flag == 0的时候是用灰度图的方法读入，默认是1，RGB  
  
  
  
2 video输入和创建  
**注意** opencv只支持写入avi格式的视频 
 
	VideoWriter a;   
	a.open(filaname,CV_FOURCC('M', 'J', 'P', 'G'), fps, CvSize);
	a << image;  
写入的时候只需要将一个个mat输入即可。  
	
### template matching  
1.just use two images and find the most matched part of two images;;  
* what you need  
* an image contain target  
* a template    
* the text  
* 
2 算法测试  
  
  
  
##相机标定  
  an important job before stereo corresponding   
  [**reference website**]（）
[***参照网址***](http://www.360doc.com/content/14/0410/14/10724725_367760675.shtml)  
  
1.简而言之就是世界坐标系到像极坐标系之间的转换  
2.