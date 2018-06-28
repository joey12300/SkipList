#ifndef SKIPLIST_H
#define SKIPLIST_H
//for size_t
#include <cstddef>
//for operator new, placement new
#include <new>
#include <iostream>
#include <random>

template <typename KeyType>
class SkipList{
	template <typename Type>
	friend std::ostream& operator << (std::ostream &os, const SkipList<Type> &sl);
private:
	class Node;
	const static size_t DEFAULT_MAX_HEIGHT;
public:
	SkipList(size_t max_height = DEFAULT_MAX_HEIGHT);
	bool insert(const KeyType &key);
	bool contain(const KeyType &key) const;
	bool remove(const KeyType &key);
	~SkipList();
/*
 * helper function, don't be exposed to user.
 */
private:
	Node *new_node(size_t max_level, const KeyType &key);
	//mainly for head node
	Node *new_node(size_t max_level);
	void delete_node(Node *&node);
	size_t get_random_height();
	void find_largest_lower_bound(Node **prev, const KeyType& key) const;
	bool contain(const KeyType &key, int print_times) const;
private:
	Node *_head_node;
	size_t _current_height;
	const size_t _max_height;
	std::mt19937 _mt;
	std::uniform_int_distribution<> _dis;

};



template <typename KeyType>
class SkipList<KeyType>::Node{
private:
	KeyType _key;
	size_t _height;
	Node *_next[];
public:
	Node(size_t height) : _height(height) {}
	Node(KeyType key, size_t height) : _key(key), _height(height){}
	~Node(){}
	Node *get_next(size_t level) const { return _next[level]; }
	void set_next(size_t level, Node *next){ _next[level] = next; }
	KeyType get_key() const { return _key; }
	size_t get_height() const { return _height;	}
};

template <typename KeyType>
const size_t SkipList<KeyType>::DEFAULT_MAX_HEIGHT = 32;

template <typename KeyType>
SkipList<KeyType>::SkipList(size_t max_height /* = DEFAULT_MAX_HEIGHT */)
					 : _current_height(0), _max_height(max_height), _mt(std::random_device()()), _dis(0,1)
{
	_head_node = new_node(_max_height);
}

template <typename KeyType>
SkipList<KeyType>::~SkipList() {
	Node *node = _head_node->get_next(0);
	while (node) {
		Node *next = node->get_next(0);
		delete_node(node);
		node = next;
	}
	delete_node(_head_node);
}

template <typename KeyType>
typename SkipList<KeyType>::Node * SkipList<KeyType>::new_node(size_t max_level) {
	void *raw_mem = operator new (sizeof(Node) + max_level * sizeof(Node *));
	Node *newnode = new (raw_mem) Node(max_level);
	for (size_t i = 0; i < max_level; ++i) {
		newnode->set_next(i, NULL);
	}
	return newnode;
}

template <typename KeyType>
typename SkipList<KeyType>::Node * SkipList<KeyType>::new_node(size_t max_level, const KeyType &key) {
	void *raw_mem = operator new (sizeof(Node) + max_level * sizeof(Node *));
	Node *newnode = new (raw_mem) Node(key, max_level);
	for (size_t i = 0; i < max_level; ++i) {
		newnode->set_next(i, NULL);
	}
	return newnode;
}

template <typename KeyType>
void SkipList<KeyType>::delete_node(Node *&node) {
	if (node != NULL) {
		node->~Node();
		operator delete(node);
		node = NULL;
	}
}

template <typename KeyType>
size_t SkipList<KeyType>::get_random_height()  {
		
	size_t height = 1;
	while (_dis(_mt) == 0) {
		height++;
	}
	if (height > _max_height) {
		height = _max_height;
	}
	return height;
}

template <typename KeyType>
void SkipList<KeyType>::find_largest_lower_bound(Node **prev, const KeyType& key) const {
	if (_current_height == 0) {
		return;
	}
	size_t level = _current_height - 1;
	typename SkipList<KeyType>::Node *node = _head_node;
	typename SkipList<KeyType>::Node *next_node = _head_node->get_next(level);
	while (1) {
		if (!next_node || next_node->get_key() >= key) {
			prev[level] = node;
			if (level == 0) {
				break;
			}
			level--;
		}
		else {
			node = next_node;
		}
		next_node = node->get_next(level);
	}

}

template <typename KeyType>
bool SkipList<KeyType>::insert(const KeyType &key) {
	if (contain(key, 0)) {
		return false;
	}
	size_t height = get_random_height();
	Node *prev[_max_height];
	find_largest_lower_bound(prev, key);
	if (height > _current_height) {
		for (size_t level = _current_height; level < height; ++level) {
			prev[level] = _head_node;
		}
		_current_height = height;
	}

	Node *curr = new_node(height, key);
	for (size_t i = 0; i < height; ++i) {
		curr->set_next(i, prev[i]->get_next(i));
		prev[i]->set_next(i, curr);
	}
	return true;
}

template <typename KeyType>
bool SkipList<KeyType>::contain(const KeyType &key) const {
	return contain(key, 0);
}

template <typename KeyType>
bool SkipList<KeyType>::remove(const KeyType &key) {
	if (!contain(key, 0)) {
		return false;
	}
	Node *prev[_max_height];
	find_largest_lower_bound(prev, key);
	Node *node = prev[0]->get_next(0);
	if (node != NULL) {
		for (size_t i = 0; i < node->get_height(); ++i) {
			prev[i]->set_next(i, node->get_next(i));
		}
	}
	
	delete_node(node);
	node = _head_node->get_next(_current_height - 1);
	while (!node) {
		_current_height--;
		if (_current_height == 0) {
			break;
		}
		node = _head_node->get_next(_current_height - 1);
	}
	return true;
}
// for debug
// print_times 1:yes 0:no
template <typename KeyType>
bool SkipList<KeyType>::contain(const KeyType &key, int print_times) const {
	if (_current_height == 0) {
		return false;
	}
	size_t level = _current_height - 1;
	typename SkipList<KeyType>::Node *node = _head_node;
	typename SkipList<KeyType>::Node *next_node = _head_node->get_next(level);
	size_t times = 0;
	while (1) {
		times++;
		if (!next_node) {
			if (level == 0) {
				break;
			}
			next_node = node->get_next(--level);
		}
		else {
			KeyType next_key = next_node->get_key();
			if (next_key == key) {
				if (print_times != 0) {
					std::cout << "Spend " << times << " times to search " << key << std::endl;
				}
				return true;
			}
			if (next_key > key) {
				if (level == 0) {
					break;
				}
				next_node = node->get_next(--level);
			}
			else {
				node = next_node;
				next_node = next_node->get_next(level);
			}
		}
	}
	if (print_times != 0) {
		std::cout << "Spend " << times << " times to search " << key << std::endl;
	}
	return false;
}
template <typename KeyType>
std::ostream & operator << (std::ostream &os, const SkipList<KeyType> &sl) {
	for (size_t level = sl._current_height; level > 0; --level) {
		typename SkipList<KeyType>::Node *node = sl._head_node->get_next(level - 1);
		os << "Level " << level << ":";
		while (node) {
			os << " " << node->get_key();
			node = node->get_next(level - 1);
		}
		os << std::endl;
	}
	return os;
}

#endif