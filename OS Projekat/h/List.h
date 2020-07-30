#ifndef _templateList_
#define _templateList_
#include <iostream.h>
extern volatile int lockFlag;
#include "declare.h"
template<class T>
class List {

public:
	struct Node {
		T data;
		Node* next;
		Node(T data, Node* next = 0) {
			this->data = data;
			this->next = next;
		}
	};
	Node* head, *tail;
	List() { head = tail = 0; }
	~List(){ empty(); }
	void add(T data) volatile;		//dodaje na kraj
	void remove(T data) volatile;	//izbacuje T iz liste
	T take() volatile;				//uzima s kraja i brise ga iz liste - zbog kompatibilnosti starijeg koda
	T front() const volatile;		//uzima s pocetka ali ne brise
	T back() const volatile;		//uzima s kraja ali ne brise
	T pop_front() volatile;			//uzima s pocetka i brise
	T pop_back() volatile;			// uzima s kraja i brise
	T get(int index) volatile;
	void swap(T t1, T t2) volatile;
	int size() volatile;
	void empty() volatile;
	void print() volatile;
	int isEmpty() volatile;
};

template<class T>
T List<T>::get(int index) volatile{
	softLock
	int i = index;
	List<T>::Node* t = head;
	while(t!=0 && i>0){
		t=t->next;
		i--;
	}
	softUnlock
	if(t != 0) return t->data;
	else return 0;
	
}
template<class T>
int List<T>::isEmpty() volatile{
	if(head==0) return 1;
	else return 0;
}

template<class T>
void List<T>::swap(T t1, T t2) volatile{
	softLock
	List<T>::Node* t = head;
	List<T>::Node* p1 = 0, *p2 = 0 ;
	while (t != 0) {
		if (t->data == t1) {
			p1 = t;
		}
		if(t->data == t2){
			p2 = t;
		}
		t = t->next;
	}
	if(p1!=0 && p2!=0){
		T tmp = p1->data;
		p1->data = t2; 
		p2->data = tmp;
	}
	softUnlock
}

template<class T>
T List<T>::back() const volatile {
	softLock
	if(head == 0) return 0;
	T ret = tail->data;
	softUnlock
	return ret;
}

template<class T>
T List<T>::front() const volatile {
	softLock
	if(head == 0) return 0;
	T ret = head->data;
	softUnlock
	return ret;
}

template<class T>
T List<T>::take() volatile {
	softLock
	if(head == 0) return 0;
	T ret = tail->data;
	remove(ret);
	softUnlock
	return ret;
}

template<class T>
T List<T>::pop_back() volatile {
	softLock
	if(head == 0) return 0;
	T ret = tail->data;
	remove(ret);
	softUnlock
	return ret;
}

template<class T>
T List<T>::pop_front() volatile {
	softLock
	if(head == 0) return 0;
	T ret = head->data;
	remove(ret);
	softUnlock
	return ret;
}

template<class T>
void List<T>::print() volatile {
	softLock
	Node* t = head;
	while(t){
		cout << t->data << endl;
		t=t->next;
	}
	softUnlock
}

template<class T>
void List<T>::empty() volatile {
	softLock
	List<T>::Node*t = head;
	while(t){
		List<T>::Node* prev = t;
		t=t->next;
		delete prev;
	}
	head = tail = 0;
	softUnlock
}
template<class T>
int List<T>::size() volatile {
	softLock;
	Node* t = head;
	int cnt = 0;
	while(t){
		cnt++;
		t=t->next;
	}
	softUnlock;
	return cnt;
}
template<class T>
void List<T>::add(T data) volatile
{
	softLock
	List<T>::Node* newNode = new Node(data);
	if (head == 0) {
		head = tail = newNode;
	}
	else {
		tail->next = newNode;
		tail = newNode;
	}
	softUnlock
}


template<class T>
void List<T>::remove(T data) volatile
{	
	softLock
	Node* t = head;
	Node* prev = 0;
	while (t != 0) {
		if (t->data == data) {
			if (prev == 0) {
				head = head->next;
				delete t;
			}
			else {
				prev->next = t->next;
				if(tail==t) tail = prev;
				delete t;
			}
			break;
		}
		prev = t;
		t = t->next;
	}
	if (head == 0) tail = 0;
	softUnlock
}

#endif