#ifndef BYTEBUFFER_H
#define BYTEBUFFER_H

#ifdef _WIN32
#include <windows.h>
#else
#include <wintypes.h>
#include <stddef.h>
#endif

namespace zetes { namespace pcscproxy {
				  template <class T>
				  class ArrayBuffer {
public:
				  ArrayBuffer();
				  ArrayBuffer(const T* _buf, size_t _len);
				  ~ArrayBuffer();
				  void clear();
				  void put(const T* _buf, size_t _len);
				  size_t size() const;
				  const T* const get() const;
private:
				  ArrayBuffer(const ArrayBuffer&);            //do not implement
				  ArrayBuffer& operator=(const ArrayBuffer&); //do not implement

				  T      * buf;
				  size_t len;
				  };

				  template <class T>
				  ArrayBuffer<T>::ArrayBuffer() : buf(NULL), len(0)
				  {
				  }

				  template <class T>
				  ArrayBuffer<T>::ArrayBuffer(const T* _buf, size_t _len) : buf(NULL), len(0)
				  {
					  put(buf, len);
				  }

				  template <class T>
				  ArrayBuffer<T>::~ArrayBuffer()
				  {
					  if (buf)
						  delete[] buf;
				  }

				  template <class T>
				  void ArrayBuffer<T>::put(const T* _buf, size_t _len)
				  {
					  T *tmpBuf = buf;
					  buf = new T[_len];
					  len = _len;
					  memcpy(buf, _buf, len);
					  if (tmpBuf)
					  {
						  delete[] tmpBuf;
					  }
				  }

				  template <class T>
				  void ArrayBuffer<T>::clear()
				  {
					  if (buf)
					  {
						  delete[] buf;
						  buf = NULL;
					  }
					  len = 0;
				  }

				  template <class T>
				  size_t ArrayBuffer<T>::size() const
				  {
					  return len;
				  }

				  template <class T>
				  const T* const ArrayBuffer<T>::get() const
				  {
					  return buf;
				  }
				  } //pcscproxy
}                   //zetes

#endif
