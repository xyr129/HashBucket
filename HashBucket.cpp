# include"HashBucket.h"
# include<string>


void TestHashBucket()
{
	HashBucket<string, string> ht(10);
	ht.Insert("相交", "Cross");
	ht.Insert("平行", "");
	ht.Insert("事实", "");
	ht.Insert("实事", "");

	//ht.Print();

	HashBucket<string, string> ht1; 
	ht1 = ht;
	ht1.Print();

	HashBucket<string, string> ht2(ht1);
	ht2.Print();
	KeyValueNode<string,string>* ret = ht2.Find("事实");
	if (NULL != ret)
	{
		ret->_value = "fact";
	}
	ht2.Print();
}

int main()
{
	TestHashBucket();
	system("pause");
	return 0;
}
