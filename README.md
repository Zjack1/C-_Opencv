Study C++ and Opencv in ENC

容器中的迭代器用法：
Mat img;
img.at<unchar>(1,1);
//////////////////
vecotr<Rect> bb;
for(int i=0;i<bb.size()-1;i++)
{
int c=bb.at(i).x;
int c=bb.at(i).y;                               
int c=bb.at(i).width;    
int c=bb.at(i).heigth;                               
}               
