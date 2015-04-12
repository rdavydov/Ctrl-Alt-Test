//
// Conteneur tableau dynamique
//

#ifndef		ARRAY_HH
# define	ARRAY_HH

#include "sys/msys.h"

struct Sortable
{
  int key;
  int pos;
  Sortable(int k, int p): key(k), pos(p) {}
  bool operator > (const Sortable &other) const { return key > other.key; }
};

template<typename T>
class Array
{
public:
  int size;
  int max_size;
  T* elt;

  Array();
  Array(int max);
  virtual ~Array()
  {
    //
    // Si ça asserte ici c'est peut-être que le constructeur par
    // défaut a été utilisé et que init n'a jamais été appelé
    //
    assert(elt != NULL);
    msys_mallocFree(elt);
    elt = NULL;
  }

  void init(int max);

  const T & operator [](int i) const;
  T & operator [](int i);

  const T & last() const;
  T & last();

  int find(const T & item) const;

  IFDBG(virtual)
  void add(const T & item);

  void remove(int n);
  void empty();

  void _bubbleSort(int first, int last);
  void _insertionSort(int first, int last);
  void _quickSort(int first, int last);
  void _mergeSort();

  void _copyAndMergeSort(int first, int last);
  void _copyAndQuickSort(int first, int last);

protected:
  void _merge(Array<T> & temp, int de1, int de2, int de3);
  int _partition(int start, int end);

  void _copyForSort(Array<Sortable> & arr, int first, int last);
  void _copyBackAfterSort(Array<Sortable> & arr, int first, int last);

private:
  // Pas de copie implicite
  Array(Array<T> &arr) { assert(false); }
};

// ============================================================================

template<typename T>
Array<T>::Array(): size(0), max_size(0), elt(NULL)
{}

template<typename T>
Array<T>::Array(int max): size(0), max_size(0), elt(NULL)
{
  init(max);
}

template<typename T>
void Array<T>::init(int max)
{
  assert(NULL == elt);
  assert(max > 1);

  max_size = max;
  elt = (T*) msys_mallocAlloc(max * sizeof T);

  assert(elt != NULL);
}

template<typename T>
const T & Array<T>::operator [](int i) const
{
  assert(i >= 0);
  assert(i < size);
  return elt[i];
}

template<typename T>
T & Array<T>::operator [](int i)
{
  assert(i >= 0);
  assert(i < size);
  return elt[i];
}

template<typename T>
const T & Array<T>::last() const
{
  assert(size > 0);
  return elt[size - 1];
}

template<typename T>
T & Array<T>::last()
{
  assert(size > 0);
  return elt[size - 1];
}

template<typename T>
int Array<T>::find(const T & item) const
{
  for (int i = 0; i < size; ++i)
    if (item == elt[i])
    {
      return i;
    }
  return -1;
}

template<typename T>
void Array<T>::add(const T &item)
{
  assert(size < max_size);
  elt[size] = item;
  ++size;
}

template<typename T>
void Array<T>::remove(int n)
{
  assert(n >= 0);
  assert(n < size);
  --size;
  elt[n] = elt[size];
}

template<typename T>
void Array<T>::empty()
{
  size = 0;
}

// ============================================================================
// Tris

// Tri à bulles : DEPRECATED

template<typename T>
void Array<T>::_bubbleSort(int first, int last)
{
  assert(first >= 0 && first < size);
  assert(last >= 0 && last < size);
  for(int i = last; i > first; --i)
  {
    bool swapped = false;
    for(int j = first; j < i; ++j)
      if(elt[j] > elt[j + 1])
      {
	swapped = true;
	T temp = elt[j + 1];
	elt[j + 1] = elt[j];
	elt[j] = temp;
      }
    if (!swapped)
    {
      break;
    }
  }
}

// Tri par insertion

template<typename T>
void Array<T>::_insertionSort(int first, int last)
{
  assert(first >= 0 && first < size);
  assert(last >= 0 && last < size);
  for (int i = first; i <= last; i++)
    {
      int j;
      const T v = elt[i];
      for (j = i - 1; j >= first; j--)
	{
	  if (!(elt[j] > v)) break;
	  elt[j + 1] = elt[j];
	}
      elt[j + 1] = v;
    }
}

// Quick sort (non testé)

template<typename T>
void Array<T>::_quickSort(int first, int last) {
    if (first < last)
    {
      const int pivot = _partition(first, last);
      _quickSort(first, pivot);
      _quickSort(pivot + 1, last);
    }
}

template<typename T>
int Array<T>::_partition(int p, int r)
{
    T pivot = elt[(p+r)/2];
    int i = p - 1;
    int j = r + 1;
    while (true)
    {
        do
	{
            --j;
	}
        while (elt[j] > pivot);
        do
	{
            ++i;
	}
        while (pivot > elt[i]);
        if (i < j)
	{
            T temp = elt[i];
            elt[i] = elt[j];
            elt[j] = temp;
        }
        else
	{
	  return j;
	}
    }
}

// Tri fusion

template<typename T>
void Array<T>::_mergeSort() // FIXME : donner des bornes
{
  Array<T> temp(size);
  int sortLength = 1;
  int de1;
  int de2;
  int de3;

  while (sortLength < size)
  {
    de1 = 0;
    while (de1 < size)
    {
      de2 = de1 + sortLength;
      de3 = de2 + sortLength;
      if (de2 > size)
      {
	de2 = size;
      }
      if (de3 > size)
      {
	de3 = size;
      }
      _merge(temp, de1, de2, de3);
      de1 = de3;
    }

    for (int i = 0; i < size ; ++i)
    {
      elt[i] = temp[i];
    }

    sortLength *= 2;
  }
}

template<typename T>
void Array<T>::_merge(Array<T> & temp, int de1, int de2, int de3)
{
  const int vers1 = de2 - 1;
  const int vers2 = de3 - 1;
  const int count = de3 - de1;

  assert(count > 0);
  assert(temp.max_size > de1);

  int j = de1;

  for(int i = 0; i < count; ++i)
  {
    if (de2 > vers2)
    {
      temp[j] = elt[de1];
      de1++;
    }
    else if (de1 > vers1)
    {
      temp[j] = elt[de2];
      de2++;
    }
    else if (elt[de1] > elt[de2]) // "i1 > i2" ==> Tri stable
    {
      temp[j] = elt[de2];
      de2++;
    }
    else
    {
      temp[j] = elt[de1];
      de1++;
    }
    ++j;
  }
}

// ----------------------------------------------------------------------------
// Versions avec tableau de clés

template<typename T>
void Array<T>::_copyAndMergeSort(int first, int last)
{
  assert(first >= 0 && first < size);
  assert(last >= 0 && last < size);

  const int len = last - first + 1;
  Array<Sortable> arr(len);
  _copyForSort(arr, first, last);

  arr._mergeSort();
  //arr._insertionSort(0, last - first);

  _copyBackAfterSort(arr, first, last);
}

template<typename T>
void Array<T>::_copyAndQuickSort(int first, int last)
{
  if (last < size) return;
  assert(first >= 0 && first < size);
  assert(last >= 0 && last < size);

  const int len = last - first + 1;
  Array<Sortable> arr(len);
  _copyForSort(arr, first, last);

  arr._quickSort(0, last - first);

  _copyBackAfterSort(arr, first, last);
}

template<typename T>
void Array<T>::_copyForSort(Array<Sortable> & arr, int first, int last)
{
  for (int i = first; i <= last; i++) {
    Sortable s(elt[i].sortHint(), i-first);
    arr.add(s);
  }
}

template<typename T>
void Array<T>::_copyBackAfterSort(Array<Sortable> & arr, int first, int last)
{
  const int len = last - first + 1;
  T *copy = (T*) msys_mallocAlloc(len * sizeof(T));
  assert(copy != NULL);
  msys_memcpy(copy, &elt[first], (len * sizeof(T)));

  for (int i = first; i <= last; i++)
    elt[i] = copy[arr[i-first].pos];
  msys_mallocFree(copy);
}

#endif		// ARRAY_H
