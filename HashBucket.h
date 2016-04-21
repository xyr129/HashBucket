#include<iostream>
#include<string>
#include<cassert>
#include<vector>
using namespace std;

template<class K,class V>
struct KeyValueNode
{
	K _key;
	V _value;
	KeyValueNode<K, V>* _next;

	KeyValueNode(const K& key,const V& value)
		: _key(key)
		, _value(value)
		, _next(NULL)
	{}
};

size_t BKDRHash(const char* str)
{
	assert(str);
	size_t seek = 131;
	size_t hash = 0;
	while (*str)
	{
		hash = hash*seek + abs(*str++);
	}
	return hash;
}

template<class K>
struct DefaultHashFuncer
{
	size_t operator()(const K& key)
	{
		return key;
	}
};

template<>
struct DefaultHashFuncer<string>
{
	size_t operator()(const string& str)
	{
		return BKDRHash(str.c_str());
	}
};

template<class K,class V,class HashFunc=DefaultHashFuncer<K>>
class HashBucket
{
	typedef	KeyValueNode<K, V> KVNode;
public:
	HashBucket()
		:_size(0)
	{}
	HashBucket(size_t size)
		:_size(size)
	{
		_table.resize(size);
	}
	HashBucket(const HashBucket<K, V>& ht)
	{
		_table.resize(ht._table.size());
		for (size_t i = 0; i < _table.size(); ++i)
		{
			KVNode* cur = ht._table[i];
			while (cur)
			{
				//取节点
				KVNode* tmp = cur;
				cur = cur->_next;
				//头插
				this->Insert(tmp->_key, tmp->_value);
			}
		}
	}
	HashBucket<K, V>& operator=(const HashBucket<K, V>& ht)
	{
		if (this != &ht)
		{
			this->_Delete();

			_table.resize(ht._table.size());
			for (size_t i = 0; i < _table.size(); ++i)
			{
				KVNode* cur = ht._table[i];
				while (cur)
				{
					//取节点
					KVNode* tmp = cur;
					cur = cur->_next;
					//头插
					this->Insert(tmp->_key, tmp->_value);
				}
			}
		}
		return *this;
	}
	~HashBucket()
	{
		this->_Delete();
	}

	bool Insert(const K& key, const V& value)
	{
		//检查是否增容
		CheckCapacity();

		size_t index = HashFun(key, _table.size());
		//判冗余
		KVNode* cur = _table[index];
		while (cur)
		{
			if (cur->_key == key)
			{
				return false;
			}
			cur = cur->_next;
		}
		//头插
		KVNode* newNode = new KVNode(key,value);
		newNode->_next = _table[index];
		_table[index] = newNode;
		++_size;
	}

	bool Remove(const K& key)
	{
		size_t index = HashFun(key, _table.size());
		KVNode* cur = _table[index];
		KVNode* prev = NULL;
		while (cur)
		{
			if (cur->_key == key)
			{
				break;
			}
			prev = cur;
			cur = cur->_next;
		}
		if (cur != NULL)
		{
			if (prev != NULL) //非头节点
			{
				prev->_next = cur->_next;
			}
			else             //头节点
			{
				_table[index] = cur->_next;
			}
			delete cur;
			cur = NULL;
			--_size;
			return true;
		}
		return false；
	}

	KVNode* Find(const K& key)
	{
		size_t index = HashFun(key, _table.size());
		KVNode* cur = _table[index];
		while (cur)
		{
			if (cur->_key == key)
			{
				return cur;
			}
			cur = cur->_next;
		}
		return NULL;
	}

	void Print()
	{
		for (size_t i = 0; i < _table.size(); ++i)
		{
			cout << i << ':';
			KVNode* cur = _table[i];
			while (cur)
			{
				cout << cur->_key << "->" << cur->_value << ' ';
				cur = cur->_next;
			}
			cout << endl;
		}
	}

protected:
	size_t GetNextPrime(size_t size)
	{
		size_t primeTable[2] = { 53, 97 };
		for (size_t i = 0; i < 2; ++i)
		{
			if (primeTable[i]>size)
			{
				return primeTable[i];
			}
		}
		return primeTable[1];
	}
	void CheckCapacity()
	{
		//负载因子为1则增容
		if (_table.size() == _size)
		{
			size_t newCapacity = GetNextPrime(_size);
			if (_size == newCapacity)
			{
				return;
			}

			vector<KVNode*> newTable;
			newTable.resize(newCapacity);
			for (size_t i = 0; i < _table.size(); ++i)
			{
				KVNode* cur = _table[i];
				while (cur)
				{
					//取节点
					KVNode* tmp = cur;
					cur = cur->_next;
					//头插
					size_t newIndex = HashFun(tmp->_key, newTable.size());
					tmp->_next = newTable[newIndex];
					newTable[newIndex] = tmp;
				}
				_table[i] = NULL;
			}
			_table.swap(newTable);
		}
	}
	size_t HashFun(const K& key, size_t _capacity)
	{
		HashFunc hf;
		return hf(key) % _capacity;
	}
	void _Delete()
	{
		for (size_t i = 0; i < _table.size(); ++i)
		{
			KVNode* cur = _table[i];
			KVNode* tmp = NULL;

			while (cur)
			{
				tmp = cur;
				cur = cur->_next;
				delete tmp;
				tmp = NULL;
			}
		}
		_size = 0;
	}
private:
	vector<KVNode*> _table;
	size_t _size;
};