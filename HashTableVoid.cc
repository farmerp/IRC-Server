
//
// Implementation of a HashTable that stores void *
//
#include "HashTableVoid.h"

// Obtain the hash code of a key
int HashTableVoid::hash(const char * key)
{
  int sum = 0;
  while (*key) {
  	sum += *key;
	key++;
  }
  return sum % TableSize;
}

// Constructor for hash table. Initializes hash table
HashTableVoid::HashTableVoid()
{
 _buckets = (HashTableVoidEntry **) malloc(TableSize*sizeof(HashTableVoidEntry*));
 int i = 0;
 while (i < TableSize) {
 	_buckets[i] = NULL;
	i++;
 }
       
}

// Add a record to the hash table. Returns true if key already exists.
// Substitute content if key already exists.
bool HashTableVoid::insertItem( const char * key, void * data)
{
  
  int h = hash(key);
  HashTableVoidEntry * e = _buckets[h];
  while (e != NULL) {
  	if(!strcmp(e->_key,key)) {
		e->_data = data;
		return true;
 	}	
	e = e->_next;
  }
  e = new HashTableVoidEntry;
  e ->_key = strdup(key);
  e ->_data = data;
  e ->_next = _buckets[h];
  _buckets[h] = e;
  return false;
}
// Find a key in the dictionary and place in "data" the corresponding record
// Returns false if key is does not exist
bool HashTableVoid::find( const char * key, void ** data)
{
  int h = hash(key);
  HashTableVoidEntry * e = _buckets[h];
  while(e != NULL) {
  	if(!strcmp(e->_key,key)) {
	*data = e->_data;
	return true;
	}
   e = e->_next;
   }	
return false;
}

// Removes an element in the hash table. Return false if key does not exist.
bool HashTableVoid::removeElement(const char * key)
{
  int h = hash(key);
  HashTableVoidEntry * e = _buckets[h];
  HashTableVoidEntry * prev = NULL;
  while (e!=NULL) {
	if (!strcmp(e->_key,key)) {
		if (prev != NULL) {
			prev->_next = e->_next;
		} else {
		_buckets[h] = e->_next;
		}
		free((void*)(e->_key));
		delete e;
		return true;
	}
	prev = e;
	e = e->_next;
  }
return false;
}

// Creates an iterator object for this hash table
HashTableVoidIterator::HashTableVoidIterator(HashTableVoid * hashTable)
{
  _currentBucket = 0;
  _currentEntry = hashTable->_buckets[0];
  _hashTable = hashTable;
 
}

// Returns true if there is a next element. Stores data value in data.
bool HashTableVoidIterator::next(const char * & key, void * & data)
{
 int counter  = _currentBucket;
 while(counter < _hashTable->TableSize) {
		while((_hashTable->_buckets[counter])!= NULL) {
				key = _hashTable->_buckets[counter]->_key;
				data = _hashTable->_buckets[counter]->_data;
 				_currentEntry = _hashTable->_buckets[counter];
				_currentBucket = ++counter;
				return true;
			
		}	
	counter++;	
 }
 return false;
}

