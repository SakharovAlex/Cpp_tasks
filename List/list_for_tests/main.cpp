#pragma once
#include <iostream>
#include <cstdio>
#include <tuple>
#include <memory>
#include <cstdlib>

struct MemoryManager {
  static size_t type_new_allocated;
  static size_t type_new_deleted;
  static size_t allocator_allocated;
  static size_t allocator_deallocated;

  static size_t allocator_constructed;
  static size_t allocator_destroyed;

  static void TypeNewAllocate(size_t n) {
    type_new_allocated += n;
  }

  static void TypeNewDelete(size_t n) {
    type_new_deleted += n;
  }

  static void AllocatorAllocate(size_t n) {
    allocator_allocated += n;
  }

  static void AllocatorDeallocate(size_t n) {
    allocator_deallocated += n;
  }

  static void AllocatorConstruct(size_t n) {
    allocator_constructed += n;
  }

  static void AllocatorDestroy(size_t n) {
    allocator_destroyed += n;
  }
};

void* operator new(size_t n, bool from_my_allocator ) {
  (void)from_my_allocator;
  return malloc(n);
}

void operator delete(void* ptr, size_t n, bool from_my_allocator) noexcept {
  (void)n;
  (void)from_my_allocator;
  free(ptr);
}

template <typename T>
struct AllocatorWithCount {
  using value_type = T;

  AllocatorWithCount() = default;

  template <typename U>
  AllocatorWithCount(const AllocatorWithCount<U>& other) {
    std::ignore = other;
  }

  T* allocate(size_t n) {
    MemoryManager::AllocatorAllocate(n * sizeof(T));
    allocator_allocated += n * sizeof(T);
    return reinterpret_cast<T*>(operator new(n * sizeof(T), true));
  }

  void deallocate(T* ptr, size_t n) {
    MemoryManager::AllocatorDeallocate(n * sizeof(T));
    allocator_deallocated += n * sizeof(T);
    operator delete(ptr, n, true);
  }

  template <typename U, typename... Args>
  void construct(U* ptr, Args&&... args) {
    MemoryManager::AllocatorConstruct(1);
    allocator_constructed += 1;
    ::new(ptr) U(std::forward<Args>(args)...);
  }

  template <typename U>
  void destroy(U* ptr) noexcept {
    MemoryManager::AllocatorDestroy(1);
    allocator_destroyed += 1;
    ptr->~U();
  }

  size_t allocator_allocated = 0;
  size_t allocator_deallocated = 0;
  size_t allocator_constructed = 0;
  size_t allocator_destroyed = 0;
};

template <typename T>
bool operator==(const AllocatorWithCount<T>& lhs, const AllocatorWithCount<T>& rhs) {
  return lhs.allocator_allocated == rhs.allocator_allocated &&
      lhs.allocator_deallocated == rhs.allocator_deallocated &&
      lhs.allocator_constructed == rhs.allocator_constructed &&
      lhs.allocator_destroyed == rhs.allocator_destroyed;
}

struct TypeWithFancyNewDeleteOperators {
  TypeWithFancyNewDeleteOperators() = default;
  explicit TypeWithFancyNewDeleteOperators(int value): value(value) {}

  static void* operator new(size_t n) {
    MemoryManager::TypeNewAllocate(n);
    return ::operator new(n);
  }

  static void operator delete(void* ptr, size_t n) {
    MemoryManager::TypeNewDelete(n);
    ::operator delete(ptr);
  }

  int value = 0;
};

struct TypeWithCounts: public TypeWithFancyNewDeleteOperators {
  using smart_counter = std::shared_ptr<size_t>;

  TypeWithCounts(int v) {
    value = v;
    *int_c += 1;
  }

  TypeWithCounts() {
    value = 0;
    *default_c += 1;
  }

  TypeWithCounts(const TypeWithCounts& other): TypeWithFancyNewDeleteOperators(other.value) {
    default_c = other.default_c;
    copy_c = other.copy_c;
    move_c = other.move_c;
    int_c = other.int_c;
    ass_copy = other.ass_copy;
    ass_move = other.ass_move;
    *copy_c += 1;
  }

  TypeWithCounts(TypeWithCounts&& other) {
    value = other.value;
    default_c = other.default_c;
    copy_c = other.copy_c;
    move_c = other.move_c;
    int_c = other.int_c;
    ass_copy = other.ass_copy;
    ass_move = other.ass_move;
    *move_c += 1;
  }

  TypeWithCounts& operator=(const TypeWithCounts& other) {
    value = other.value;
    default_c = other.default_c;
    copy_c = other.copy_c;
    move_c = other.move_c;
    int_c = other.int_c;
    ass_copy = other.ass_copy;
    ass_move = other.ass_move;
    *ass_copy += 1;
    return *this;
  }

  TypeWithCounts& operator=(TypeWithCounts&& other)  noexcept {
    value = other.value;
    default_c = other.default_c;
    copy_c = other.copy_c;
    move_c = other.move_c;
    int_c = other.int_c;
    ass_copy = other.ass_copy;
    ass_move = other.ass_move;
    *ass_move += 1;
    return *this;
  }

  smart_counter default_c = std::make_shared<size_t>(0);
  smart_counter copy_c = std::make_shared<size_t>(0);
  smart_counter move_c = std::make_shared<size_t>(0);
  smart_counter int_c = std::make_shared<size_t>(0);
  smart_counter ass_copy = std::make_shared<size_t>(0);
  smart_counter ass_move = std::make_shared<size_t>(0);
};


size_t MemoryManager::type_new_allocated = 0;
size_t MemoryManager::type_new_deleted = 0;
size_t MemoryManager::allocator_allocated = 0;
size_t MemoryManager::allocator_deallocated = 0;
size_t MemoryManager::allocator_constructed = 0;
size_t MemoryManager::allocator_destroyed = 0;

static size_t ctor_calls;
static size_t dtor_calls;

struct Accountant {
  // Some field of strange size
  char arr[40];

  

  // NO LINT
  static void reset() {
    ctor_calls = 0;
    dtor_calls = 0;
  }

  Accountant() {
    ++ctor_calls;
  }

  // NO LINT
  Accountant(const Accountant&) {
    ++ctor_calls;
  }

  // NO LINT
  Accountant& operator=(const Accountant&) {
    // Actually, when it comes to assign one list to another,
    // list can use element-wise assignment instead of destroying nodes and creating new ones
    ++ctor_calls;
    ++dtor_calls;
    return *this;
  }

  Accountant(Accountant&&) = delete;
  Accountant& operator=(Accountant&&) = delete;

  ~Accountant() {
    ++dtor_calls;
  }
};

static bool need_throw;

struct ThrowingAccountant: public Accountant {

  int value = 0;

  // NO LINT
  ThrowingAccountant(int value = 0): Accountant(), value(value) {
    if (need_throw && ctor_calls % 5 == 4)
      throw std::string("Ahahahaha you have been cocknut");
  }

  // NO LINT
  ThrowingAccountant(const ThrowingAccountant& other): Accountant(), value(other.value) {
    if (need_throw && ctor_calls % 5 == 4)
      throw std::string("Ahahahaha you have been cocknut");
  }

  // NO LINT
  ThrowingAccountant& operator=(const ThrowingAccountant& other) {
    value = other.value;
    ++ctor_calls;
    ++dtor_calls;
    if (need_throw && ctor_calls % 5 == 4)
      throw std::string("Ahahahaha you have been cocknut");
    return *this;
  }

};



template <typename T, typename Allocator = std::allocator<T>>
class List {
 public:
  List() : size_(0) {}

    List(size_t count, const T& value, const Allocator& alloc = Allocator());

    explicit List(size_t count, const Allocator& alloc = Allocator());
    
    List(const List& other);

    List(std::initializer_list<T> init, const Allocator& alloc = Allocator());

    ~List();

    List& operator=(const List& other);

  struct Node {
    Node(Node* first, Node* second, const T& val)
        : next(first), prev(second), value(val) {}

    Node(Node* first, Node* second) : next(first), prev(second) {}
    Node* next;
    Node* prev;
    T value;
  };

  using allocator_type = Allocator;
  using value_type = typename allocator_type::value_type;
  using alloc_traits = std::allocator_traits<Allocator>;
  using node_alloc = typename alloc_traits::template rebind_alloc<Node>;
  using node_alloc_traits = typename alloc_traits::template rebind_traits<Node>;

  template <bool IsConst>
  class CommonIterator {
   public:
    using value_type = std::conditional_t<IsConst, const T, T>;
    using pointer = std::conditional_t<IsConst, const T*, T*>;
    using iterator_category = std::bidirectional_iterator_tag;
    using reference = std::conditional_t<IsConst, const T&, T&>;
    using difference_type = std::ptrdiff_t;

    CommonIterator(Node* ptr) : ptr_(ptr) {}

    reference operator*() { return ptr_->value; }

    pointer operator->() { return &ptr_->value; }

    reference operator*() const { return ptr_->value; }

    pointer operator->() const { return &ptr_->value; }

      CommonIterator<IsConst>& operator++();

      CommonIterator<IsConst>& operator--();

      CommonIterator<IsConst> operator++(int);

      CommonIterator<IsConst> operator--(int);

    bool operator==(const CommonIterator<IsConst>& other) const {
      return ptr_ == other.ptr_;
    }

    bool operator!=(const CommonIterator<IsConst>& other) const {
      return ptr_ != other.ptr_;
    }

   private:
    Node* ptr_;
  };

  using iterator = CommonIterator<false>;
  using const_iterator = CommonIterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  iterator begin() { return iterator(fake_node_->next); }

  iterator end() { return iterator(fake_node_); }

  const_iterator cbegin() { return const_iterator(fake_node_->next); }

  const_iterator cend() { return const_iterator(fake_node_); }

  reverse_iterator rend() { return reverse_iterator(begin()); }

  reverse_iterator rbegin() { return reverse_iterator(end()); }

  const_reverse_iterator crend() { return const_reverse_iterator(cbegin()); }

  const_reverse_iterator crbegin() { return const_reverse_iterator(cend()); }

  const_iterator begin() const { return const_iterator(fake_node_->next); }

  const_iterator end() const {
    const_iterator iter(fake_node_);
    return iter;
  }

  const_iterator cbegin() const { return const_iterator(fake_node_->next); }

  const_iterator cend() const {
    return const_iterator(fake_node_);
  }

  const_reverse_iterator rend() const {
    return const_reverse_iterator(begin());
  }

  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(end());
  }

  const_reverse_iterator crend() const {
    return const_reverse_iterator(cbegin());
  }

  const_reverse_iterator crbegin() const {
    return const_reverse_iterator(cend());
  }

  T& front() { return fake_node_->next->value; }

  const T& front() const { return fake_node_->next->value; }

  T& back() { return fake_node_->prev->value; }

  const T& back() const { return fake_node_->prev->value; }

  bool empty() const { return size_ == 0; }

  size_t size() const { return size_; }

    void push_back(const T& value);

    void push_back(T&& value);

    void push_front(const T& value);

    void push_front(T&& value);

    void pop_back();

    void pop_front();

  node_alloc get_allocator() { return alloc_; }

 private:
  Node* fake_node_ = nullptr;
  node_alloc alloc_;
  size_t size_;
};

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t count, const T& value, const Allocator& alloc) {
    size_ = count;
    alloc_ = alloc;
    fake_node_ = node_alloc_traits::allocate(alloc_, 1);
    Node* last_added = fake_node_;
    for (size_t i = 0; i < size_; ++i) {
      Node* new_node = node_alloc_traits::allocate(alloc_, 1);
      try {
        node_alloc_traits::construct(alloc_, new_node, fake_node_, last_added,
                                     value);
        fake_node_->prev = new_node;
        last_added->next = new_node;
        last_added = last_added->next;
      } catch (...) {
        node_alloc_traits::deallocate(alloc_, new_node, 1);
        this->~List();
        throw;
      }
    }
}
// explicit

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t count, const Allocator& alloc) {
  size_ = count;
  alloc_ = alloc;
  fake_node_ = node_alloc_traits::allocate(alloc_, 1);
  Node* last_added = fake_node_;
  for (size_t i = 0; i < size_; ++i) {
    Node* new_node = node_alloc_traits::allocate(alloc_, 1);
    try {
      node_alloc_traits::construct(alloc_, new_node, fake_node_, last_added);
      fake_node_->prev = new_node;
      last_added->next = new_node;
      last_added = last_added->next;
    } catch (...) {
      node_alloc_traits::deallocate(alloc_, new_node, 1);
      this->~List();
      throw;
    }
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(const List& other) {
  size_ = other.size_;
  alloc_ =
      node_alloc_traits::select_on_container_copy_construction(other.alloc_);
  fake_node_ = node_alloc_traits::allocate(alloc_, 1);
  Node* last_added = fake_node_;
  for (const auto& iter : other) {
    Node* new_node = node_alloc_traits::allocate(alloc_, 1);
    try {
      node_alloc_traits::construct(alloc_, new_node, fake_node_, last_added,
                                   iter);
      fake_node_->prev = new_node;
      last_added->next = new_node;
      last_added = last_added->next;
    } catch (...) {
      node_alloc_traits::deallocate(alloc_, new_node, 1);
      this->~List();
      throw;
    }
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(std::initializer_list<T> init, const Allocator& alloc) {
  size_ = init.size();
  alloc_ = alloc;
  fake_node_ = node_alloc_traits::allocate(alloc_, 1);
  Node* last_added = fake_node_;
  for (const auto& iter : init) {
    Node* new_node = node_alloc_traits::allocate(alloc_, 1);
    try {
      node_alloc_traits::construct(alloc_, new_node, fake_node_, last_added,
                                   iter);
      fake_node_->prev = new_node;
      last_added->next = new_node;
      last_added = last_added->next;
    } catch (...) {
      node_alloc_traits::deallocate(alloc_, new_node, 1);
      this->~List();
      throw;
    }
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::~List() {
  if (size_ != 0) {
    Node* current = fake_node_->next;
    Node* next = current->next;
    while (true) {
      node_alloc_traits::destroy(alloc_, current);
      node_alloc_traits::deallocate(alloc_, current, 1);
      if (next != fake_node_) {
        current = next;
        next = current->next;
      } else {
        break;
      }
    }
  }
  node_alloc_traits::deallocate(alloc_, fake_node_, 1);
}

template <typename T, typename Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(const List& other) {
  try {
    List copy = other;
    std::swap(fake_node_, copy.fake_node_);
    std::swap(size_, copy.size_);
    if (node_alloc_traits::propagate_on_container_copy_assignment::value) {
      alloc_ = other.alloc_;
    }
  } catch (...) {
    throw;
  }
  return *this;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template CommonIterator<IsConst>&
List<T, Allocator>::CommonIterator<IsConst>::operator++() {
    ptr_ = ptr_->next;
    return *this;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template CommonIterator<IsConst>&
List<T, Allocator>::CommonIterator<IsConst>::operator--() {
    ptr_ = ptr_->prev;
    return *this;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template CommonIterator<IsConst>
List<T, Allocator>::CommonIterator<IsConst>::operator++(int) {
    CommonIterator<IsConst> iterat = *this;
    *this = *this->ptr_->next;
    return iterat;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template CommonIterator<IsConst>
List<T, Allocator>::CommonIterator<IsConst>::operator--(int) {
    CommonIterator<IsConst> iterat = *this;
    *this = *this->ptr_->prev;
    return iterat;
}


template <typename T, typename Allocator>
void List<T, Allocator>::push_back(const T& value) {
  if (size_ != 0) {
    Node* new_node = node_alloc_traits::allocate(alloc_, 1);
    Node* last_added = fake_node_->prev;

    try {
      node_alloc_traits::construct(alloc_, new_node, fake_node_, last_added,
                                   value);
      last_added->next = new_node;
      fake_node_->prev = new_node;
    } catch (...) {
      node_alloc_traits::destroy(alloc_, new_node);
      node_alloc_traits::deallocate(alloc_, new_node, 1);
      throw;
    }
  } else {
    Node* new_node = node_alloc_traits::allocate(alloc_, 1);
    try {
      node_alloc_traits::construct(alloc_, new_node, fake_node_, fake_node_,
                                   value);
      fake_node_ = node_alloc_traits::allocate(alloc_, 1);

      fake_node_->next = new_node;
      fake_node_->prev = new_node;
    } catch (...) {
      node_alloc_traits::destroy(alloc_, new_node);
      node_alloc_traits::deallocate(alloc_, new_node, 1);
      throw;
    }
  }
  ++size_;
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_back(T&& value) {
  if (size_ != 0) {
    Node* new_node = node_alloc_traits::allocate(alloc_, 1);
    Node* last_added = fake_node_->prev;
    try {
      node_alloc_traits::construct(alloc_, new_node, fake_node_, last_added,
                                   std::move(value));
      last_added->next = new_node;
      fake_node_->prev = new_node;
    } catch (...) {
      node_alloc_traits::destroy(alloc_, new_node);
      node_alloc_traits::deallocate(alloc_, new_node, 1);
      throw;
    }
  } else {
    Node* new_node = node_alloc_traits::allocate(alloc_, 1);
    try {
      node_alloc_traits::construct(alloc_, new_node, fake_node_, fake_node_,
                                   std::move(value));
      fake_node_ = node_alloc_traits::allocate(alloc_, 1);

      fake_node_->next = new_node;
      fake_node_->prev = new_node;
    } catch (...) {
      node_alloc_traits::destroy(alloc_, new_node);
      node_alloc_traits::deallocate(alloc_, new_node, 1);
      throw;
    }
  }
  ++size_;
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_front(const T& value) {
  if (size_ != 0) {
    Node* new_node = node_alloc_traits::allocate(alloc_, 1);
    Node* first_added = fake_node_->next;
    try {
      node_alloc_traits::construct(alloc_, new_node, first_added, fake_node_,
                                   value);
      first_added->prev = new_node;
      fake_node_->next = new_node;
    } catch (...) {
      node_alloc_traits::destroy(alloc_, new_node);
      node_alloc_traits::deallocate(alloc_, new_node, 1);
      throw;
    }
  } else {
    Node* new_node = node_alloc_traits::allocate(alloc_, 1);
    try {
      node_alloc_traits::construct(alloc_, new_node, fake_node_, fake_node_,
                                   value);
      fake_node_ = node_alloc_traits::allocate(alloc_, 1);

      fake_node_->next = new_node;
      fake_node_->prev = new_node;
    } catch (...) {
      node_alloc_traits::destroy(alloc_, new_node);
      node_alloc_traits::deallocate(alloc_, new_node, 1);
      throw;
    }
  }
  ++size_;
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_front(T&& value) {
  if (size_ != 0) {
    Node* new_node = node_alloc_traits::allocate(alloc_, 1);
    Node* first_added = fake_node_->next;
    try {
      node_alloc_traits::construct(alloc_, new_node, first_added, fake_node_,
                                   std::move(value));
      first_added->prev = new_node;
      fake_node_->next = new_node;
    } catch (...) {
      node_alloc_traits::destroy(alloc_, new_node);
      node_alloc_traits::deallocate(alloc_, new_node, 1);
      throw;
    }
  } else {
    Node* new_node = node_alloc_traits::allocate(alloc_, 1);
    try {
      node_alloc_traits::construct(alloc_, new_node, fake_node_, fake_node_,
                                   std::move(value));
      fake_node_ = node_alloc_traits::allocate(alloc_, 1);

      fake_node_->next = new_node;
      fake_node_->prev = new_node;
    } catch (...) {
      node_alloc_traits::destroy(alloc_, new_node);
      node_alloc_traits::deallocate(alloc_, new_node, 1);
      throw;
    }
  }
  ++size_;
}

template <typename T, typename Allocator>
void List<T, Allocator>::pop_back() {
  if (size_ != 0) {
    Node* last = fake_node_->prev;
    Node* penultimate = last->prev;
    penultimate->next = fake_node_;
    fake_node_->prev = penultimate;
    node_alloc_traits::destroy(alloc_, last);
    node_alloc_traits::deallocate(alloc_, last, 1);
    --size_;
  }
}

template <typename T, typename Allocator>
void List<T, Allocator>::pop_front() {
  if (size_ != 0) {
    Node* first = fake_node_->next;
    Node* second = first->next;
    second->prev = fake_node_;
    fake_node_->next = second;
    node_alloc_traits::destroy(alloc_, first);
    node_alloc_traits::deallocate(alloc_, first, 1);
    --size_;
  }
}


int main() {
    Accountant::reset();
    
    need_throw = true;
    
    try {
        List<ThrowingAccountant> lst(8);
    } catch (...) {
        std::cout << (ctor_calls == 4) << "\n";
        std::cout << (dtor_calls) << "\n";
    }
    
    need_throw = false;
    List<ThrowingAccountant> lst(8);
    
    List<ThrowingAccountant> lst2;
    for (int i = 0; i < 13; ++i) {
        lst2.push_back(i);
    }
    
    Accountant::reset();
    need_throw = true;
    
    try {
        auto lst3 = lst2;
    } catch (...) {
        std::cout << (ctor_calls == 4) << "\n";
        std::cout << (dtor_calls) << "\n";
    }
    
    Accountant::reset();
    
    try {
        lst = lst2;
    } catch (...) {
        std::cout << (ctor_calls == 4) << "\n";
        std::cout << (dtor_calls) << "\n";
        
        // Actually it may not be 8 (although de facto it is), but the only thing we can demand here
        // is the abscence of memory leaks
        //
        //assert(lst.size() == 8);
    }
    
    return 0;
}
