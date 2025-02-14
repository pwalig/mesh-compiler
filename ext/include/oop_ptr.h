#pragma once
#define OOP_PTR_PTR_MOVE_CONSTRUCTOR
#define OOP_PTR_OBJECT_COPY_CONSTRUCTOR

#ifndef oop_ptr_get_copy
#define oop_ptr_get_copy get_copy_ptr
#endif // oop_ptr_get_copy

#define oop_ptr_base_declare(b) virtual b* oop_ptr_get_copy() const
#define oop_ptr_child_declare(b) b* oop_ptr_get_copy() const override
#define oop_ptr_define(b, c) b* c::oop_ptr_get_copy() const { return new c(*this); }

// can be typed in template class body
#define oop_ptr_template_base_define(b) inline virtual b* oop_ptr_get_copy() const { return new b(*this); }

// can be typed in template class body
#define oop_ptr_template_child_define(b, c) inline b* oop_ptr_get_copy() const override { return new c(*this); }


template <typename T>
class oop_ptr {
public:
	oop_ptr();
#ifdef OOP_PTR_OBJECT_COPY_CONSTRUCTOR
	oop_ptr(const T& obj);
#endif
#ifdef OOP_PTR_PTR_COPY_CONSTRUCTOR
	oop_ptr(const T* ptr);
#endif
#ifdef OOP_PTR_PTR_MOVE_CONSTRUCTOR
	oop_ptr(T* ptr);
#endif
	oop_ptr(const oop_ptr<T>& other);
	oop_ptr(oop_ptr<T>&& other) noexcept;

#ifdef OOP_PTR_OBJECT_COPY_OPERATOR
	oop_ptr<T>& operator= (const T& obj);
#endif
#ifdef OOP_PTR_PTR_COPY_OPERATOR
	oop_ptr<T>& operator= (const T* ptr);
#endif
#ifdef OOP_PTR_PTR_MOVE_OPERATOR
	oop_ptr<T>& operator= (T* ptr);
#endif
	oop_ptr<T>& operator= (const oop_ptr<T>& other);
	oop_ptr<T>& operator= (oop_ptr<T>&& other) noexcept;
	explicit operator bool() const noexcept;

	T* operator-> () const;
	T& operator* () const;
	T* get() const;
	template <typename U>
	U* get() const;
	template <typename U>
	bool gettable() const;

	T* release();
	template <typename U>
	U* release();

	~oop_ptr();

	static oop_ptr copy(const T& obj);
	static oop_ptr copy(const T* ptr);
	static oop_ptr move(T* ptr);
	const static oop_ptr null;
private:
	T* resource;
};

template<typename T>
inline oop_ptr<T>::oop_ptr() : resource(nullptr)
{
}

template<typename T>
const oop_ptr<T> oop_ptr<T>::null = oop_ptr();

#ifdef OOP_PTR_OBJECT_COPY_CONSTRUCTOR
template<typename T>
inline oop_ptr<T>::oop_ptr(const T& obj) : resource(obj.oop_ptr_get_copy())
{
}
#endif

#ifdef OOP_PTR_PTR_COPY_CONSTRUCTOR
template<typename T>
inline oop_ptr<T>::oop_ptr(const T* ptr) : resource(ptr->oop_ptr_get_copy())
{
}
#endif

#ifdef OOP_PTR_PTR_MOVE_CONSTRUCTOR
template<typename T>
inline oop_ptr<T>::oop_ptr(T* ptr) : resource(ptr)
{
}
#endif

template<typename T>
inline oop_ptr<T>::oop_ptr(const oop_ptr<T>& other) : resource(other.resource->oop_ptr_get_copy())
{
}

template<typename T>
inline oop_ptr<T>::oop_ptr(oop_ptr<T>&& other) noexcept : resource(other.resource)
{
	other.resource = nullptr;
}

#ifdef OOP_PTR_OBJECT_COPY_OPERATOR
template<typename T>
inline oop_ptr<T>& oop_ptr<T>::operator=(const T& copy)
{
	delete (this->resource);
	this->resource = copy.oop_ptr_get_copy();
	return *this;
}
#endif

#ifdef OOP_PTR_PTR_COPY_OPERATOR
template<typename T>
inline oop_ptr<T>& oop_ptr<T>::operator=(const T* ptr)
{
	delete (this->resource);
	this->resource = ptr->oop_ptr_get_copy();
	return *this;
}
#endif

#ifdef OOP_PTR_PTR_MOVE_OPERATOR
template<typename T>
inline oop_ptr<T>& oop_ptr<T>::operator=(T* ptr)
{
	delete (this->resource);
	this->resource = ptr;
	return *this;
}
#endif

template<typename T>
inline oop_ptr<T>& oop_ptr<T>::operator=(const oop_ptr<T>& other)
{
	if (this != &other) {
		delete (this->resource);
		this->resource = other.resource->oop_ptr_get_copy();
	}
	return *this;
}

template<typename T>
inline oop_ptr<T>& oop_ptr<T>::operator=(oop_ptr<T>&& other) noexcept
{
	if (this != &other) {
		delete (this->resource);
		this->resource = other.resource;
		other.resource = nullptr;
	}
	return *this;
}

template<typename T>
inline oop_ptr<T>::operator bool() const noexcept
{
	return (this->resource != nullptr);
}

template<typename T>
inline T* oop_ptr<T>::operator->() const
{
	return this->resource;
}

template<typename T>
inline T& oop_ptr<T>::operator*() const
{
	return *(this->resource);
}

template<typename T>
inline T* oop_ptr<T>::get() const
{
	return this->resource;
}

template<typename T>
inline T* oop_ptr<T>::release()
{
	T* out = this->get();
	this->resource = nullptr;
	return out;
}

template<typename T>
template<typename U>
inline U* oop_ptr<T>::get() const
{
	if (this->resource == nullptr) return nullptr;
	if (U* out = dynamic_cast<U*>(this->resource)) return out;
	return nullptr;
}

template<typename T>
template<typename U>
inline bool oop_ptr<T>::gettable() const
{
	return get<U>() != nullptr;
}

template<typename T>
template<typename U>
inline U* oop_ptr<T>::release()
{
	U* out = this->get<U>();
	this->resource = nullptr;
	return out;
}

template<typename T>
inline oop_ptr<T>::~oop_ptr()
{
	if (resource) delete resource;
}

template<typename T>
inline oop_ptr<T> oop_ptr<T>::copy(const T& obj)
{
	return copy(&obj);
}

template<typename T>
inline oop_ptr<T> oop_ptr<T>::copy(const T* ptr)
{
#if defined(OOP_PTR_PTR_COPY_CONSTRUCTOR)
	return oop_ptr(ptr);
#elif defined (OOP_PTR_PTR_MOVE_CONSTRUCTOR)
	return oop_ptr(ptr->oop_ptr_get_copy());
#else
	oop_ptr o;
	o.resource = obj.oop_ptr_get_copy();
	return o;
#endif
}

template<typename T>
inline oop_ptr<T> oop_ptr<T>::move(T* ptr)
{
#ifdef OOP_PTR_PTR_MOVE_CONSTRUCTOR
	return oop_ptr(ptr);
#else
	oop_ptr o;
	o.resource = ptr;
	return o;
#endif
}
