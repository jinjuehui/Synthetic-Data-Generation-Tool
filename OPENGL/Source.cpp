#include <vector>
#include <iostream>

using namespace std;

double findMedianSortedArrays(vector<int>& nums1, vector<int>& nums2) {
	int size = nums1.size()+nums2.size();
	std::vector<int> merge;
	std::vector<int>::iterator i1 = nums1.begin();
	std::vector<int>::iterator i2 = nums2.begin();
	while (i1 != nums1.end() || i2 != nums2.end())
	{
		if (*i1 < *i2)
		{
			merge.push_back(*i1);
			if(i1!=nums1.end())
				i1++;
		}
		else
		{
			merge.push_back(*i2);
			if(i2!=nums2.end())
				i2++;
		}
	}
	if (merge.size() % 2 == 0)
		return (merge[merge.size() / 2] + merge[merge.size() / 2]) / 2;
	else
		return merge[(merge.size() + 1) / 2];
}


int main()
{
	std::vector<int> nums1 = { 4,5 };
	std::vector<int> nums2 = {2};

	int result=findMedianSortedArrays(nums1, nums2);

	cout << result << endl;

	system("pause");



}