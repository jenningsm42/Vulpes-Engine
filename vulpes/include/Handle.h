#ifndef _VUL_HANDLE_H
#define _VUL_HANDLE_H

namespace vul
{
	template <class T> class Handle
	{
	public:
		Handle()
			: m_data(new T()), m_refCount(new int(1)), m_loaded(false),
			m_deleteOnZero(true) {}
		Handle(T& instance)
			: m_data(&instance), m_refCount(new int(1)), m_loaded(true),
			m_deleteOnZero(false) {}
		Handle(const Handle<T>& other)
			: m_data(other.m_data), m_refCount(other.m_refCount), m_loaded(other.m_loaded),
			m_deleteOnZero(other.m_deleteOnZero)
		{ (*m_refCount)++; }

		~Handle()
		{
			if(--(*m_refCount) <= 0)
			{
				if(m_deleteOnZero) delete m_data;
				delete m_refCount;
			}
		}

		Handle<T>& operator=(const Handle<T>& rhs)
		{
			if(--(*m_refCount) <= 0)
			{
				if(m_deleteOnZero) delete m_data;
				delete m_refCount;
			}

			m_data = rhs.m_data;
			m_refCount = rhs.m_refCount;
			m_loaded = rhs.m_loaded;
			(*m_refCount)++;
			return *this;
		}

		void setLoaded() { m_loaded = true; }
		bool isLoaded() { return m_loaded; }

		T* operator->() { return m_data; }

		T makeCopy() const { return T(*m_data); }

	private:
		T* m_data;
		int* m_refCount;
		bool m_loaded;
		bool m_deleteOnZero;
	};
}

#endif // _VUL_HANDLE_H