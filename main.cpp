#include <iostream>
#include<map>
#include<vector>
#include<string>
#include<set>
#include<math.h>
#include<algorithm>

using namespace std;

//��д�����û���Сд������Ʒ
string A[] = { "a","b","d" };
string B[] = { "b","c","e" };
string C[] = { "c","d" };
string D[] = { "b","c","d" };
string E[] = {"a","d"};

map<string, set<string> > user_to_item;
map<string, set<string> > item_to_user;

//���� �û�-��Ʒ�Ķ�Ӧ��
void Create_UserToItem_Table()
{
	set<string> v;
	for (int i = 0; i < sizeof(A) / sizeof(string); i++)
	{
		v.insert(A[i]);
	}
	user_to_item["A"] = v;

	v.clear();
	for (int i = 0; i < sizeof(B) / sizeof(string); i++)
	{
		v.insert(B[i]);
	}
	user_to_item["B"] = v;

	v.clear();
	for (int i = 0; i < sizeof(C) / sizeof(string); i++)
	{
		v.insert(C[i]);
	}
	user_to_item["C"] = v;

	v.clear();
	for (int i = 0; i < sizeof(D) / sizeof(string); i++)
	{
		v.insert(D[i]);
	}
	user_to_item["D"] = v;

	v.clear();
	for (int i = 0; i < sizeof(E) / sizeof(string); i++)
	{
		v.insert(E[i]);
	}
	user_to_item["E"] = v;
}

//�û�-��Ʒ ת�� ��Ʒ-�û� ���ű�
void TransferTo_ItemToUser()
{
	if (!item_to_user.empty())
	{
		item_to_user.clear();
	}

	map<string, set<string> >::iterator iter = user_to_item.begin();
	while (iter != user_to_item.end())
	{
		set<string>& item_set = iter->second;
		set<string>::iterator iter_set = item_set.begin();
		for (; iter_set != item_set.end(); iter_set++)
		{
			item_to_user[*iter_set].insert(iter->first);
		}
		iter++;
	}
}

void Prepare()
{
	Create_UserToItem_Table();
	TransferTo_ItemToUser();
}

// ���ݵ��ű���ϡ�����
void Create_CoRated_table(map<string, set<string> >& dst, map<string, map<string, float> >& CoRated_table)
{
	//���������û�
	map<string, set<string> >::iterator iter_begin = dst.begin();
	map<string, set<string> >::iterator iter_End = dst.end();
	for (; iter_begin != iter_End; iter_begin++)
	{
		//��������������Ʒ��,
		set<string>& user_set = iter_begin->second;
		set<string>::iterator set_begin1 = user_set.begin();
		for (; set_begin1 != user_set.end(); set_begin1++)
		{
			set<string>::iterator set_begin2 = user_set.begin();
			string user1 = *set_begin1;
			for (; set_begin2 != user_set.end(); set_begin2++)
			{
				string user2 = *set_begin2;
				if (user1 == user2)
				{
					CoRated_table[user1][user2] = 0;
					continue;
				}
				bool init = false;
				map<string, map<string, float>  >::iterator aim1 = CoRated_table.find(user1);
				if (aim1 != CoRated_table.end())
				{
					map<string, float>& temp = aim1->second;
					map<string, float>::iterator aim2 = temp.find(user2);
					if (aim2 != temp.end())
					{
						init = true;
					}
				}

				if (!init)
				{
					CoRated_table[user1][user2] = 1;
				}
				else
				{
					CoRated_table[user1][user2]++;
				}
			}
		}
	}
}


//������Ʒ����Ʒ������ƶ�  ����Ʒ���ڵĽ���/����Ʒ���ڵĳ˻����������
void Calculate_Similarity(const map<string, map<string, float> >& CoRated_table, map<string, set<string> >& item_to_user, map<string, map<string, float> >& result)
{
	map<string, map<string, float> >::const_iterator iter_corated1 = CoRated_table.begin();
	for (; iter_corated1 != CoRated_table.end(); iter_corated1++)
	{
		const string& user1 = iter_corated1->first;
		const map<string, float>& table = iter_corated1->second;
		map<string, float>::const_iterator iter_corated2 = table.begin();
		int size1 = item_to_user[user1].size();
		for (; iter_corated2 != table.end(); iter_corated2++)
		{
			const string& user2 = iter_corated2->first;
			const float score = iter_corated2->second;
			int size2 = item_to_user[user2].size();

			if (user1 == user2)
			{
				result[user1][user2] = 0;
			}
			else
			{
				result[user1][user2] = (float)score / sqrt(size1*size2);
			}
		}
	}
}

void PrintResult(const map<string, map<string, float> >& result)
{
	map<string, map<string, float> >::const_iterator iter = result.begin();
	for (; iter != result.end(); iter++)
	{
		const string user1 = iter->first;
		const map<string, float>& score = iter->second;
		map<string, float>::const_iterator iter2 = score.begin();
		for (; iter2 != score.end(); iter2++)
		{
			cout << "W[" << user1 << "][" << iter2->first << "]" << " : " << iter2->second << endl;
		}
	}
}

// ָ��һ���û���һ����Ʒ���и��û�����ʷ����Ȥ��Ʒ��������ָ����Ʒ���Ƶ���ʷ����Ȥ��Ʒ�����ƶ�
vector<float> SortSimilarUser(const map<string, map<string, float> >& table, const string& user_name, const string& item_name, map<string, set<string> >& user_to_item)
{
	vector<float> v;

	if (table.find(user_name) == table.end())
	{
		return v;
	}

	if (user_to_item[user_name].find(item_name) != user_to_item[user_name].end())
	{
		v.push_back(1);
		return v;
	}

	// �ҳ�����Ʒitem_name�����ƶȵ�������Ʒ�����ƶ�
	const map<string, float>& item_map = table.find(item_name)->second;
	map<string, float>::const_iterator iter = item_map.begin();
	const set<string>& item_set = user_to_item[user_name];
	for (; iter != item_map.end(); iter++)
	{
		const string& item_name1 = iter->first;

		if (item_set.find(item_name1) != item_set.end())
		{
			v.push_back(iter->second);
		}
	}

	//���մӴ�С�����������ƶ�
	sort(v.begin(), v.end(), greater<float>());
	return v;
}

float Calculate(const vector<float>& result, int k)
{
	float t = 0;
	float r = 1;
	for (int i = 0; i < result.size() && i < k; i++)
	{
		t += result[i] * r;
	}

	return t;
}

int main()
{
	Prepare();

	//����ϡ�����
	map<string, map<string, float> > CoRated_table;
	Create_CoRated_table(user_to_item, CoRated_table);

	//������Ʒ֮������ƶ�
	map<string, map<string, float> > similarity;
	Calculate_Similarity(CoRated_table,item_to_user, similarity);

	PrintResult(similarity);

	return 0;
}