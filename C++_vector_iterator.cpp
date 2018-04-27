#include <IOSTREAM>  
#include <VECTOR>  
  
using namespace std;  
  
int main()  
{  
    cout<<"By：Sayln_wu"<<endl<<endl;  
    //编写10个元素的vector对象。用迭代器把每个元素值改为当前值+2.  
    vector<int> ivec(3,8);  
    cout<<"原值为："<<endl;  
    int i=0;  
    for (vector<int>::iterator iter=ivec.begin();iter!=ivec.end();iter++)  
    {  
        i++;  
        cout<< *iter <<endl;  
        *iter=(*iter)+i;  
    }  
    //可以放在上面输出，下面是为了使用const_iterator。  
    cout<<"修改后的值为："<<endl;  
    for(vector<int>::const_iterator citer=ivec.begin();citer!=ivec.end();citer++)  
    {  
        cout<<*citer<<endl;  
    }  
    //求vector中间值  
    vector<int>::iterator min = ivec.begin() +ivec.size()/2;  
    cout<<"中间值为："<<*min<<endl;  
    return 0;  
}  
