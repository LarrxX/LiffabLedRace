#pragma once

#include "Arduino.h"
#include "Defines.h"

template <class T>
class DynamicArray
{
private:
    T *_array;
    size_t _used;
    size_t _allocated;

public:
    DynamicArray(size_t initialSize = DYNAMIC_CHUNK_SIZE)
    {
        Init(initialSize);
    }

    ~DynamicArray()
    {
        Clear();
    }

    void Add(T element)
    {
        if (_array == NULL)
        {
            Init(DYNAMIC_CHUNK_SIZE);
        }
        else if (_used == _allocated)
        {
            _allocated += DYNAMIC_CHUNK_SIZE;
            _array = (T *)realloc(_array, _allocated * sizeof(T));
        }
        _array[_used] = element;
        ++_used;
    }

    void Remove(size_t index)
    {
        //Replace item at "index" with the item at the end of the array
        //This does not preserve array order!
        if (index < _used)
        {
            if (index > 0)
            {
                _array[index] = _array[_used - 1];
            }
            --_used;
        }
    }

    void Clear()
    {
        if (_array != NULL)
        {
            free(_array);
            _array = NULL;
            _used = _allocated = 0;
        }
    }

    void Sort()
    {
        QuickSort(0, _used - 1);
    }

    T &operator[](size_t i)
    {
        return _array[i];
    }

    size_t Count() const
    {
        return _used;
    }

private:
    void Init(size_t initialSize)
    {
        _used = 0;
        _allocated = initialSize;
        _array = (T *)malloc(initialSize * sizeof(T));
    }

    bool isSmaller(const T &first, const T &second)
    {
        if (is_pointer<T>::value)
        {
            return *first < *second;
        }
        return first < second;
    }

    void QuickSort(size_t low, size_t high)
    {
        if (low < high)
        {
            T pivot = _array[high];
            size_t i = low - 1;

            for (size_t j = low; j <= high - 1; ++j)
            {
                if (isSmaller(_array[j], pivot))
                {
                    ++i;
                    T tmp = _array[i];
                    _array[i] = _array[j];
                    _array[j] = tmp;
                }
            }

            size_t partition = i + 1;

            T tmp = _array[partition];
            _array[partition] = _array[high];
            _array[high] = tmp;

            QuickSort(low, partition - 1);
            QuickSort(partition + 1, high);
        }
    }
};

template <typename T>
struct is_pointer
{
    static const bool value = false;
};

template <typename T>
struct is_pointer<T *>
{
    static const bool value = true;
};