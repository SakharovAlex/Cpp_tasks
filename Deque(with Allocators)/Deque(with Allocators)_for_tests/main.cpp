/*
Описание задания

Deque

В данной задаче вам нужно допилить ваш дек чтобы он поддерживал move-семантику и аллокаторы

Что нужно реализовать

Необходимо реализовать класс Deque<T, Allocator>, который должен содержать следующие методы:

Также ваш дек должен поддерживать работу с итераторами. Для этого вам нужно реализовать следующее:

Внутренний тип iterator (с маленькой буквы). Этот тип должен поддерживать:
Инкремент, декремент
Сложение с целым числом
Вычитание целого числа
Сравнения <,>,<=,>=,==,!=
Взятие разности от двух итераторов
Разыменование (operator*). Возвращает T&
operator-> (Возвращает T*)
различные using'и: value_type, pointer, iterator_category, reference
Внутренний тип const_iterator. Отличие от обычного в том, что он не позволяет менять лежащий под ним элемент. Конверсия (неявная в том числе) из неконстантного в константный допустима. Обратно - нет.
Внутренний тип reverse_iterator (можно пользоваться std::reverse_iterator)
Методы для доступа к итераторам:
begin, cbegin - возвращают итератор (константный итератор) на первый элемент дека
end, cend - возвращает итератор (константный итератор) на "элемент следующий за последним"
rbegin, rend, crbegin. crend - реверс итераторы на соответствующие элементы
Метод insert(iterator, const T&) - вставляет элемент по итератору. Все элементы правей сдвигаются на один вправо. Работает за O(n)
Метод emplace(iterator, T&&)
Метод erase(iterator) - удаляет элемент по итератору. Все элементы правей сдвигаются на один влево. Работает за O(n)
Требования к реализации

const_iterator не должен быть копипастой iterator. Вспомните про шаблонные параметры (bool IsConst)
Операции push_back, push_front, pop_back, pop_front, emplace_back, emplace_front не должны инвалидировать ссылки и указатели на остальные элементы дека
Операции pop_back и pop_front не должны инвалидировать итераторы на остальные элементы дека
Если сам контейнер константный, то методы begin и end должны возвращать константные итераторы
Разыменование end, cend - UB. Однако декремент от end (cend) должен давать итератор на последний элемент. Вычитание целых чисел от end, cend так же должно давать валидные итераторы на соответствующие элементы дека
Ваш дек должен давать строгую гарантию безопасности относительно исключений. Это значит, что в случае исключения в конструкторе или операторе присваивания типа T во время выполнения какого-либо метода дека, последний должен вернуться в исходное состояние, которое было до начала выполнения метода, и пробросить исключение наверх в вызывающий код.
Тип T не обязан иметь конструктора по умолчанию чтобы храниться в вашем деке. При этом конструктор Deque(size_t count) может не компилироваться
Тип T не обязан иметь конструктор копирования чтобы храниться в вашем деке.
Разрешено пользоваться стандартными контейнерами std::vector и std::array. Другими стандартными контейнерами пользоваться нельзя.
*/

#include <iostream>
#include <vector>


struct NotDefaultConstructible {
  NotDefaultConstructible() = delete;
  NotDefaultConstructible(int data) : data(data) {}
  int data;

  auto operator<=>(const NotDefaultConstructible&) const = default;
};

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

  Accountant(Accountant&&) = default;
  Accountant& operator=(Accountant&&) = default;

  ~Accountant() {
    ++dtor_calls;
  }
};

static bool need_throw = false;

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

template <typename T, bool PropagateOnConstruct, bool PropagateOnAssign>
struct WhimsicalAllocator : public std::allocator<T> {
  std::shared_ptr<int> number;

  auto select_on_container_copy_construction() const {
    return PropagateOnConstruct
           ? WhimsicalAllocator<T, PropagateOnConstruct, PropagateOnAssign>()
           : *this;
  }

  struct propagate_on_container_copy_assignment
      : std::conditional_t<PropagateOnAssign, std::true_type, std::false_type>
  {};

  template <typename U>
  struct rebind {
    using other = WhimsicalAllocator<U, PropagateOnConstruct, PropagateOnAssign>;
  };

  WhimsicalAllocator(): number(std::make_shared<int>(counter)) {
    ++counter;
  }

  template <typename U>
  WhimsicalAllocator(const WhimsicalAllocator<U, PropagateOnConstruct, PropagateOnAssign>& another)
      : number(another.number)
  {}

  template <typename U>
  auto& operator=(const WhimsicalAllocator<U, PropagateOnConstruct, PropagateOnAssign>& another) {
    number = another.number;
    return *this;
  }

  template <typename U>
  bool operator==(const WhimsicalAllocator<U, PropagateOnConstruct, PropagateOnAssign>& another) const {
    return *number == *another.number;
  }

  template <typename U>
  bool operator!=(const WhimsicalAllocator<U, PropagateOnConstruct, PropagateOnAssign>& another) const {
    return *number != *another.number;
  }

  static size_t counter;
};


size_t MemoryManager::type_new_allocated = 0;
size_t MemoryManager::type_new_deleted = 0;
size_t MemoryManager::allocator_allocated = 0;
size_t MemoryManager::allocator_deallocated = 0;
size_t MemoryManager::allocator_constructed = 0;
size_t MemoryManager::allocator_destroyed = 0;

template <typename T, bool PropagateOnConstruct, bool PropagateOnAssign>
size_t
    WhimsicalAllocator<T, PropagateOnConstruct, PropagateOnAssign>::counter = 0;



void SetupTest() {
  MemoryManager::type_new_allocated = 0;
  MemoryManager::type_new_deleted = 0;
  MemoryManager::allocator_allocated = 0;
  MemoryManager::allocator_deallocated = 0;
  MemoryManager::allocator_constructed = 0;
  MemoryManager::allocator_destroyed = 0;
}

template <typename T, typename Allocator = std::allocator<T>>
class Deque {
 public:
  using allocator_type = Allocator;
  using value_type = typename allocator_type::value_type;
  using alloc_traits = std::allocator_traits<Allocator>;

  Deque();

  Deque(const Allocator& alloc);

  Deque(const Deque& other) { builder(other); }

  void try_build_from_other(const Deque& other, size_t& index1, size_t& index2);

  void catch_build_from_other(size_t& index1, size_t& index2);

  void builder(const Deque& other);

  Deque(size_t count, const Allocator& alloc = Allocator());

  void try_build_size_of_count(size_t& index1, size_t& index2, const T& value);

  void try_build_size_of_count(size_t& index1, size_t& index2);

  void catch_build_size_of_count(size_t& index1, size_t& index2);

  Deque(size_t count, const T& value, const Allocator& alloc = Allocator());

  void clear();

  Deque(Deque&& other);

  void try_build_from_init(size_t& index1, size_t& index2,
                           std::initializer_list<T> init);

  void catch_build_from_init(size_t& index1, size_t& index2);

  Deque(std::initializer_list<T> init, const Allocator& alloc = Allocator());

  Deque& operator=(const Deque& other);

  Deque& operator=(Deque&& other);

  ~Deque();

  size_t size() const { return size_; }

  bool empty() const { return size_ == 0; }

  const T& operator[](size_t index) const;

  T& operator[](size_t index);

  const T& at(size_t index) const;

  T& at(size_t index);

  void bucket_down();

  void push_back(const T& value);

  void push_back(T&& value);

  template <typename... Args>
  void emplace_back(Args&&... args);

  void pop_back();

  void bucket_up();

  void push_front(const T& value);

  void push_front(T&& value);

  template <typename... Args>
  void emplace_front(Args&... args);

  void pop_front();

  Allocator get_allocator() { return alloc_; }

  template <bool IsConst>
  class CommonIterator {
   public:
    using value_type = std::conditional_t<IsConst, const T, T>;
    using pointer = std::conditional_t<IsConst, const T*, T*>;
    using iterator_category = std::random_access_iterator_tag;
    using reference = std::conditional_t<IsConst, const T&, T&>;
    using difference_type = std::ptrdiff_t;

    CommonIterator() = default;

    CommonIterator(pointer ptr, size_t array_index, size_t index,
                   const std::vector<T*>* deque)
        : deque_(deque), ptr_(ptr), array_index_(array_index), index_(index) {}

    CommonIterator& operator=(const CommonIterator& other);

    CommonIterator(const CommonIterator& other);

    reference operator*() { return *ptr_; }

    pointer operator->() { return ptr_; }

    reference operator*() const { return *ptr_; }

    pointer operator->() const { return ptr_; }

    CommonIterator<IsConst>& operator++();

    CommonIterator<IsConst>& operator--();

    CommonIterator<IsConst> operator++(int);

    CommonIterator<IsConst> operator--(int);

    CommonIterator<IsConst>& operator+=(int number);

    CommonIterator<IsConst>& operator-=(int number);

    bool operator==(const CommonIterator<IsConst>& other) const {
      return ptr_ == other.ptr_;
    }

    bool operator!=(const CommonIterator<IsConst>& other) const {
      return !(ptr_ == other.ptr_);
    }

    bool operator<=(const CommonIterator<IsConst>& other) const;

    bool operator>=(const CommonIterator<IsConst>& other) const;

    bool operator<(const CommonIterator<IsConst>& other) const {
      return (*this != other) && (*this <= other);
    }

    bool operator>(const CommonIterator<IsConst>& other) const {
      return (*this != other) && (*this >= other);
    }

    CommonIterator<IsConst> operator+(int number) const;

    CommonIterator<IsConst> operator-(int number) const;

    difference_type operator-(const CommonIterator<IsConst>& other) const {
      return (array_index_ - other.array_index_) * kBucketSize +
             (index_ - other.index_);
    }

    friend class Deque;

   private:
    const std::vector<T*>* deque_;
    pointer ptr_;
    int array_index_;
    int index_;
    const int kBucketSize = 32;
  };

  using iterator = CommonIterator<false>;
  using const_iterator = CommonIterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  iterator begin() {
    if (size_ == 0) {
      return iterator();
    }
    return iterator(external_[first_elem_.first] + first_elem_.second,
                    first_elem_.first, first_elem_.second, &external_);
  }

  iterator end();

  const_iterator cbegin() {
    return const_iterator(external_[first_elem_.first] + first_elem_.second,
                          first_elem_.first, first_elem_.second, &external_);
  }

  const_iterator cend();

  reverse_iterator rend() { return reverse_iterator(begin()); }

  reverse_iterator rbegin() { return reverse_iterator(end()); }

  const_reverse_iterator crend() { return const_reverse_iterator(cbegin()); }

  const_reverse_iterator crbegin() { return const_reverse_iterator(cend()); }

  const_iterator begin() const {
    return const_iterator(external_[first_elem_.first] + first_elem_.second,
                          first_elem_.first, first_elem_.second, &external_);
  }

  const_iterator end() const;

  const_iterator cbegin() const {
    return const_iterator(external_[first_elem_.first] + first_elem_.second,
                          first_elem_.first, first_elem_.second, &external_);
  }

  const_iterator cend() const;

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

  void insert(iterator iter, const T& value);

  void emplace(iterator iter, T&& value);

  void erase(iterator other);

 private:
  Allocator alloc_;
  std::vector<T*> external_;
  size_t size_;
  std::pair<size_t, size_t> first_elem_;
  std::pair<size_t, size_t> last_elem_;
  size_t number_of_buckets_;
  const size_t kSizeOfBucket = 32;
};

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque() {
  size_ = 0;
  first_elem_ = {1, 0};
  last_elem_ = {1, 0};
  number_of_buckets_ = 3;
  external_.resize(3);
  for (size_t i = 0; i < 3; ++i) {
    external_[i] = alloc_traits::allocate(alloc_, kSizeOfBucket);
  }
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(const Allocator& alloc) {
  size_ = 0;
  first_elem_ = {1, 0};
  last_elem_ = {1, 0};
  number_of_buckets_ = 3;
  external_.resize(3);
  alloc_ = alloc;
  for (size_t i = 0; i < 3; ++i) {
    external_[i] = alloc_traits::allocate(alloc_, kSizeOfBucket);
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::try_build_from_other(const Deque& other,
                                               size_t& index1, size_t& index2) {
    int i = 1;
  for (index2 = 0; index2 < kSizeOfBucket && index2 < size_; ++index2) {
    alloc_traits::construct(alloc_, external_[index1] + index2,
                            other.external_[index1][index2]);
      std::cout << i << "-й раз конструирую в (Deque& other)\n";
      ++i;
    
  }
  for (index1 = first_elem_.first + 1; index1 <= last_elem_.first; ++index1) {
    for (index2 = 0;
         index2 < kSizeOfBucket &&
         (index2 + kSizeOfBucket * (index1 - first_elem_.first)) < size_;
         ++index2) {
      alloc_traits::construct(alloc_, external_[index1] + index2,
                              other.external_[index1][index2]);
      std::cout << i << "-й раз конструирую в (Deque& other)\n";
      ++i;
    }
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::catch_build_from_other(size_t& index1,
                                                 size_t& index2) {
    int id = 1;
  if (index1 == first_elem_.first) {
    for (size_t i = first_elem_.second; i < index2; ++i) {
      alloc_traits::destroy(alloc_, external_[index1] + i);
        std::cout << id << "-й раз дестрою в (Deque& other)\n";
        ++id;
    }
  } else {
    for (size_t i = first_elem_.second; i < kSizeOfBucket; ++i) {
      alloc_traits::destroy(alloc_, external_[first_elem_.first] + i);
        std::cout << id << "-й раз дестрою в (Deque& other)\n";
        ++id;
    }
    for (size_t i = 0; i < index2; ++i) {
      alloc_traits::destroy(alloc_, external_[index1] + i);
        std::cout << id << "-й раз дестрою в (Deque& other)\n";
        ++id;
    }
    for (size_t i = first_elem_.first + 1; i < index1; ++i) {
      for (size_t j = 0; j < kSizeOfBucket; ++j) {
        alloc_traits::destroy(alloc_, external_[i] + j);
          std::cout << id << "-й раз дестрою в (Deque& other)\n";
          ++id;
      }
    }
    for (size_t i = 0; i < external_.size(); ++i) {
      alloc_traits::deallocate(alloc_, external_[i], kSizeOfBucket);
        std::cout << "Деаллоцирую 32 элемента\n";
    }
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::builder(const Deque& other) {
  size_ = other.size_;
  first_elem_ = other.first_elem_;
  last_elem_ = other.last_elem_;
  number_of_buckets_ = other.number_of_buckets_;
  external_.resize(number_of_buckets_, nullptr);
  alloc_ = alloc_traits::select_on_container_copy_construction(other.alloc_);
  for (size_t i = 0; i < number_of_buckets_; ++i) {
    external_[i] = alloc_traits::allocate(alloc_, kSizeOfBucket);
  }
  size_t index1 = first_elem_.first;
  size_t index2 = 0;
  try {
    try_build_from_other(other, index1, index2);
  } catch (...) {
    catch_build_from_other(index1, index2);
    external_.clear();
    external_.shrink_to_fit();
    throw;
  }
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(size_t count, const Allocator& alloc) {
  if (count > 0) {
    number_of_buckets_ =
        std::max(3 * (count / kSizeOfBucket), static_cast<size_t>(3));
    size_ = count;
    first_elem_.first = number_of_buckets_ / 3;
    first_elem_.second = 0;
    last_elem_.first = first_elem_.first + (count - 1) / kSizeOfBucket;
    last_elem_.second = (count - 1) % kSizeOfBucket;
    external_.resize(number_of_buckets_);
    alloc_ = alloc;
    for (size_t i = 0; i < number_of_buckets_; ++i) {
      external_[i] = alloc_traits::allocate(alloc_, kSizeOfBucket);
      std::cout << "Аллоцирую 32 элемента\n";
    }
    size_t index1 = first_elem_.first;
    size_t index2 = 0;
    try {
      try_build_size_of_count(index1, index2);
    } catch (...) {
      catch_build_size_of_count(index1, index2);
      // this->~Deque();
      external_.clear();
      external_.shrink_to_fit();
      throw;
    }
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::try_build_size_of_count(size_t& index1,
                                                  size_t& index2,
                                                  const T& value) {
    int i = 1;
  for (; index1 <= last_elem_.first; ++index1) {
    for (index2 = 0;
         index2 < kSizeOfBucket &&
         (index2 + kSizeOfBucket * (index1 - first_elem_.first)) < size_;
         ++index2) {
      alloc_traits::construct(alloc_, external_[index1] + index2, value);
      std::cout << i << "-й раз конструирую\n";
        ++i;
    }
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::try_build_size_of_count(size_t& index1,
                                                  size_t& index2) {
    int i = 1;
  for (; index1 <= last_elem_.first; ++index1) {
    for (index2 = 0;
         index2 < kSizeOfBucket &&
         (index2 + kSizeOfBucket * (index1 - first_elem_.first)) < size_;
         ++index2) {
      alloc_traits::construct(alloc_, external_[index1] + index2);
      std::cout << i << "-й раз конструирую\n";
        ++i;
    }
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::catch_build_size_of_count(size_t& index1,
                                                    size_t& index2) {
  for (size_t i = 0; i < index2; ++i) {
    alloc_traits::destroy(alloc_, external_[index1] + i);
    std::cout << i + 1 << "-й раз дестрою\n";
  }
  // for (size_t i = first_elem_.first; i <= index1; ++i) {
  // for (size_t j = 0; j < kSizeOfBucket; ++j) {
  // alloc_traits::destroy(alloc_, external_[i] + j);
  // }
  // }
  for (size_t i = 0; i < external_.size(); ++i) {
    alloc_traits::deallocate(alloc_, external_[i], kSizeOfBucket);
    std::cout << "деаллоцирую 32 элемента\n";
  }
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(size_t count, const T& value,
                           const Allocator& alloc) {
  if (count > 0) {
    number_of_buckets_ =
        std::max(2 * (count / kSizeOfBucket + 1), static_cast<size_t>(4));
    size_ = count;
    first_elem_.first = number_of_buckets_ / 4;
    first_elem_.second = 0;
    last_elem_.first = first_elem_.first + (count - 1) / kSizeOfBucket;
    last_elem_.second = (count - 1) % kSizeOfBucket;
    external_.resize(number_of_buckets_, nullptr);
    alloc_ = alloc;
    for (size_t i = 0; i < number_of_buckets_; ++i) {
      external_[i] = alloc_traits::allocate(alloc_, kSizeOfBucket);
    }
    size_t index1 = first_elem_.first;
    size_t index2 = 0;
    try {
      try_build_size_of_count(index1, index2, value);
    } catch (...) {
      catch_build_size_of_count(index1, index2);
      external_.clear();
      external_.shrink_to_fit();
      throw;
    }
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::clear() {
  if (first_elem_.first != last_elem_.first) {
    for (size_t i = first_elem_.second; i < kSizeOfBucket; ++i) {
      alloc_traits::destroy(alloc_, external_[first_elem_.first] + i);
    }
    for (size_t i = 0; i <= last_elem_.second; ++i) {
      alloc_traits::destroy(alloc_, external_[last_elem_.first] + i);
    }
    for (size_t i = first_elem_.first + 1; i < last_elem_.first; ++i) {
      for (size_t j = 0; j < kSizeOfBucket; ++j) {
        alloc_traits::destroy(alloc_, external_[i] + j);
      }
    }
  } else {
    for (size_t i = first_elem_.second; i < last_elem_.second; ++i) {
      alloc_traits::destroy(alloc_, external_[first_elem_.first] + i);
    }
  }
  for (size_t i = 0; i < external_.size(); ++i) {
    alloc_traits::deallocate(alloc_, external_[i], kSizeOfBucket);
  }
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(Deque&& other) {
  size_ = other.size_;
  first_elem_ = other.first_elem_;
  last_elem_ = other.last_elem_;
  number_of_buckets_ = other.number_of_buckets_;
  external_ = std::move(other.external_);
  other.size_ = 0;
  alloc_ = std::move(other.alloc_);
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::try_build_from_init(size_t& index1, size_t& index2,
                                              std::initializer_list<T> init) {
  for (; index1 <= last_elem_.first; ++index1) {
    for (index2 = 0;
         index2 < kSizeOfBucket &&
         (index2 + kSizeOfBucket * (index1 - first_elem_.first)) < size_;
         ++index2) {
      alloc_traits::construct(
          alloc_, external_[index1] + index2,
          init.begin()[index2 + kSizeOfBucket * (index1 - first_elem_.first)]);
    }
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::catch_build_from_init(size_t& index1,
                                                size_t& index2) {
  for (size_t i = first_elem_.second; i < kSizeOfBucket; ++i) {
    alloc_traits::destroy(alloc_, external_[first_elem_.first] + i);
  }
  for (size_t i = 0; i < index2; ++i) {
    alloc_traits::destroy(alloc_, external_[index1] + i);
  }
  for (size_t i = first_elem_.first; i < index1; ++i) {
    for (size_t j = 0; j < kSizeOfBucket; ++j) {
      alloc_traits::destroy(alloc_, external_[i] + j);
    }
  }
  for (size_t i = 0; i < external_.size(); ++i) {
    alloc_traits::deallocate(alloc_, external_[i], kSizeOfBucket);
  }
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(std::initializer_list<T> init,
                           const Allocator& alloc) {
  if (init.size() > 0) {
    number_of_buckets_ =
        std::max(2 * (init.size() / kSizeOfBucket + 1), static_cast<size_t>(4));
    size_ = init.size();
    first_elem_.first = number_of_buckets_ / 4;
    first_elem_.second = 0;
    last_elem_.first = first_elem_.first + (init.size() - 1) / kSizeOfBucket;
    last_elem_.second = (init.size() - 1) % kSizeOfBucket;
    external_.resize(number_of_buckets_, nullptr);
    alloc_ = alloc;
    for (size_t i = 0; i < number_of_buckets_; ++i) {
      external_[i] = alloc_traits::allocate(alloc_, kSizeOfBucket);
    }
    size_t index1 = first_elem_.first;
    size_t index2 = 0;
    try {
      try_build_from_init(index1, index2, init);
    } catch (...) {
      catch_build_from_init(index1, index2);
      external_.clear();
      external_.shrink_to_fit();
      throw;
    }
  }
}

template <typename T, typename Allocator>
Deque<T, Allocator>& Deque<T, Allocator>::operator=(const Deque& other) {
  try {
    Deque copy = other;
    std::swap(external_, copy.external_);
    std::swap(size_, copy.size_);
    std::swap(first_elem_, copy.first_elem_);
    std::swap(last_elem_, copy.last_elem_);
    std::swap(number_of_buckets_, copy.number_of_buckets_);
    if (alloc_traits::propagate_on_container_copy_assignment::value) {
      alloc_ = other.alloc_;
    }
  } catch (...) {
    throw;
  }
  return *this;
}

template <typename T, typename Allocator>
Deque<T, Allocator>& Deque<T, Allocator>::operator=(Deque&& other) {
  try {
    Deque copy = std::move(other);
    std::swap(external_, copy.external_);
    std::swap(size_, copy.size_);
    std::swap(first_elem_, copy.first_elem_);
    std::swap(last_elem_, copy.last_elem_);
    std::swap(number_of_buckets_, copy.number_of_buckets_);
    if (alloc_traits::propagate_on_container_copy_assignment::value) {
      alloc_ = other.alloc_;
    }
  } catch (...) {
    throw;
  }
  return *this;
}

template <typename T, typename Allocator>
Deque<T, Allocator>::~Deque() {
    int id = 1;
  if (size_ != 0) {
    if (first_elem_.first == last_elem_.first) {
      for (size_t i = first_elem_.second; i <= last_elem_.second; ++i) {
        alloc_traits::destroy(alloc_, external_[first_elem_.first] + i);
          std::cout << id << "-й раз дестрою в ~Deque()\n";
          ++id;
      }
    } else {
      for (size_t i = first_elem_.second; i < kSizeOfBucket; ++i) {
        alloc_traits::destroy(alloc_, external_[first_elem_.first] + i);
          std::cout << id << "-й раз дестрою в ~Deque()\n";
          ++id;
      }
      for (size_t i = 0; i <= last_elem_.second; ++i) {
        alloc_traits::destroy(alloc_, external_[last_elem_.first] + i);
          std::cout << id << "-й раз дестрою в ~Deque()\n";
          ++id;
      }
      for (size_t i = first_elem_.first + 1; i < last_elem_.first; ++i) {
        for (size_t j = 0; j < kSizeOfBucket; ++j) {
          alloc_traits::destroy(alloc_, external_[i] + j);
            std::cout << id << "-й раз дестрою в ~Deque()\n";
            ++id;
        }
      }
    }
  }
  for (size_t i = 0; i < external_.size(); ++i) {
    alloc_traits::deallocate(alloc_, external_[i], kSizeOfBucket);
      std::cout << i + 1 << "-й раз деаллоцирую в ~Deque()\n";
  }
}

template <typename T, typename Allocator>
const T& Deque<T, Allocator>::operator[](size_t index) const {
  size_t number_of_bucket =
      (index + first_elem_.second) / kSizeOfBucket + first_elem_.first;
  size_t position = (index + first_elem_.second) % kSizeOfBucket;
  return external_[number_of_bucket][position];
}

template <typename T, typename Allocator>
T& Deque<T, Allocator>::operator[](size_t index) {
  size_t number_of_bucket =
      (index + first_elem_.second) / kSizeOfBucket + first_elem_.first;
  size_t position = (index + first_elem_.second) % kSizeOfBucket;
  return external_[number_of_bucket][position];
}

template <typename T, typename Allocator>
const T& Deque<T, Allocator>::at(size_t index) const {
  if (index >= size_) {
    throw std::out_of_range("...");
  }
  size_t number_of_bucket =
      (index + first_elem_.second) / kSizeOfBucket + first_elem_.first;
  size_t position = (index + first_elem_.second) % kSizeOfBucket;
  return external_[number_of_bucket][position];
}

template <typename T, typename Allocator>
T& Deque<T, Allocator>::at(size_t index) {
  if (index >= size_) {
    throw std::out_of_range("...");
  }
  size_t number_of_bucket =
      (index + first_elem_.second) / kSizeOfBucket + first_elem_.first;
  size_t position = (index + first_elem_.second) % kSizeOfBucket;
  return external_[number_of_bucket][position];
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::bucket_down() {
  number_of_buckets_ *= 2;
  std::vector<T*> new_external(number_of_buckets_, nullptr);
  size_t index = 0;
  try {
    for (; index < external_.size(); ++index) {
      new_external[number_of_buckets_ / 2 + index] =
          alloc_traits::allocate(alloc_, kSizeOfBucket);
    }
  } catch (...) {
    number_of_buckets_ /= 2;
    for (size_t i = 0; i < index; ++i) {
      alloc_traits::deallocate(alloc_, new_external[i], kSizeOfBucket);
    }
    throw;
  }
  for (size_t i = 0; i < external_.size(); ++i) {
    new_external[i] = external_[i];
  }
  external_ = new_external;
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::push_back(const T& value) {
  if (size_ == 0) {
    try {
      alloc_traits::construct(alloc_, external_[last_elem_.first], value);
        std::cout << "Коснтруктор пуш бэк\n";
    } catch (...) {
      alloc_traits::destroy(alloc_, external_[last_elem_.first]);
        std::cout << "Дестрой пуш бэк\n";
    }
  } else {
    if (last_elem_.second == kSizeOfBucket - 1) {
      ++last_elem_.first;
      last_elem_.second = 0;
      alloc_traits::construct(alloc_, external_[last_elem_.first], value);
    } else {
      ++last_elem_.second;
      try {
        alloc_traits::construct(
            alloc_, external_[last_elem_.first] + last_elem_.second, value);
          std::cout << "Коснтруктор пуш бэк\n";
      } catch (...) {
        alloc_traits::destroy(alloc_,
                              external_[last_elem_.first] + last_elem_.second);
          std::cout << "Дестрой пуш бэк\n";
      }
    }
    if (last_elem_.first == external_.size() - 1 &&
        last_elem_.second == kSizeOfBucket - 1) {
      bucket_down();
    }
  }
  ++size_;
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::push_back(T&& value) {
  if (size_ == 0) {
    alloc_traits::destroy(alloc_,
                          external_[first_elem_.first] + first_elem_.second);
    alloc_traits::construct(alloc_, external_[last_elem_.first],
                            std::move(value));
  } else {
    if (last_elem_.second == kSizeOfBucket - 1) {
      ++last_elem_.first;
      last_elem_.second = 0;
      alloc_traits::construct(alloc_, external_[last_elem_.first],
                              std::move(value));
    } else {
      ++last_elem_.second;
      alloc_traits::construct(alloc_,
                              external_[last_elem_.first] + last_elem_.second,
                              std::move(value));
    }
    if (last_elem_.first == external_.size() - 1 &&
        last_elem_.second == kSizeOfBucket - 1) {
      bucket_down();
    }
  }
  ++size_;
}

template <typename T, typename Allocator>
template <typename... Args>
void Deque<T, Allocator>::emplace_back(Args&&... args) {
  if (size_ == 0) {
    alloc_traits::construct(alloc_, external_[last_elem_.first],
                            std::forward<Args>(args)...);
  } else {
    if (last_elem_.second == kSizeOfBucket - 1) {
      ++last_elem_.first;
      last_elem_.second = 0;
      alloc_traits::construct(alloc_, external_[last_elem_.first],
                              std::forward<Args>(args)...);
    } else {
      ++last_elem_.second;
      alloc_traits::construct(alloc_,
                              external_[last_elem_.first] + last_elem_.second,
                              std::forward<Args>(args)...);
    }
    if (last_elem_.first == external_.size() - 1 &&
        last_elem_.second == kSizeOfBucket - 1) {
      bucket_down();
    }
  }
  ++size_;
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::pop_back() {
  --size_;
  alloc_traits::destroy(alloc_,
                        external_[last_elem_.first] + last_elem_.second);
  if (last_elem_.second == 0) {
    last_elem_.second = kSizeOfBucket - 1;
    --last_elem_.first;
  } else {
    --last_elem_.second;
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::bucket_up() {
  number_of_buckets_ *= 2;
  std::vector<T*> new_external(number_of_buckets_, nullptr);
  size_t index = 0;
  try {
    for (; index < external_.size(); ++index) {
      new_external[index] = alloc_traits::allocate(alloc_, kSizeOfBucket);
    }
  } catch (...) {
    number_of_buckets_ /= 2;
    for (size_t i = 0; i < index; ++i) {
      alloc_traits::deallocate(alloc_, new_external[i], kSizeOfBucket);
    }
    throw;
  }
  for (size_t i = 0; i < external_.size(); ++i) {
    new_external[number_of_buckets_ / 2 + i] = external_[i];
  }
  external_ = new_external;
  first_elem_.first += number_of_buckets_ / 2;
  last_elem_.first += number_of_buckets_ / 2;
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::push_front(const T& value) {
  if (size_ == 0) {
    alloc_traits::construct(alloc_, external_[first_elem_.first], value);
  } else {
    if (first_elem_.second == 0) {
      --first_elem_.first;
      first_elem_.second = kSizeOfBucket - 1;
      alloc_traits::construct(
          alloc_, external_[first_elem_.first] + first_elem_.second, value);
    } else {
      --first_elem_.second;
      alloc_traits::construct(
          alloc_, external_[first_elem_.first] + first_elem_.second, value);
    }
    if (first_elem_.first == 0 && first_elem_.second == 0) {
      bucket_up();
    }
  }
  ++size_;
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::push_front(T&& value) {
  if (size_ == 0) {
    alloc_traits::destroy(alloc_,
                          external_[first_elem_.first] + first_elem_.second);
    alloc_traits::construct(alloc_, external_[first_elem_.first],
                            std::move(value));
  } else {
    if (first_elem_.second == 0) {
      --first_elem_.first;
      first_elem_.second = kSizeOfBucket - 1;
      alloc_traits::construct(alloc_,
                              external_[first_elem_.first] + first_elem_.second,
                              std::move(value));
    } else {
      --first_elem_.second;
      alloc_traits::construct(alloc_,
                              external_[first_elem_.first] + first_elem_.second,
                              std::move(value));
    }
    if (first_elem_.first == 0 && first_elem_.second == 0) {
      bucket_up();
    }
  }
  ++size_;
}

template <typename T, typename Allocator>
template <typename... Args>
void Deque<T, Allocator>::emplace_front(Args&... args) {
  if (size_ == 0) {
    alloc_traits::destroy(alloc_,
                          external_[first_elem_.first] + first_elem_.second);
    alloc_traits::construct(alloc_, external_[first_elem_.first],
                            std::forward<Args>(args)...);
  } else {
    if (first_elem_.second == 0) {
      --first_elem_.first;
      first_elem_.second = kSizeOfBucket - 1;
      alloc_traits::construct(alloc_,
                              external_[first_elem_.first] + first_elem_.second,
                              std::forward<Args>(args)...);
    } else {
      --first_elem_.second;
      alloc_traits::construct(alloc_,
                              external_[first_elem_.first] + first_elem_.second,
                              std::forward<Args>(args)...);
    }
    if (first_elem_.first == 0 && first_elem_.second == 0) {
      bucket_up();
    }
  }
  ++size_;
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::pop_front() {
  --size_;
  alloc_traits::destroy(alloc_,
                        external_[first_elem_.first] + first_elem_.second);
  if (first_elem_.second == kSizeOfBucket - 1) {
    first_elem_.second = 0;
    ++first_elem_.first;
  } else {
    ++first_elem_.second;
  }
}

template <typename T, typename Allocator>
template <bool IsConst>
typename Deque<T, Allocator>::template CommonIterator<IsConst>&
Deque<T, Allocator>::CommonIterator<IsConst>::operator=(
    const CommonIterator& other) {
  ptr_ = other.ptr_;
  array_index_ = other.array_index_;
  index_ = other.index_;
  deque_ = other.deque_;
  return *this;
}

template <typename T, typename Allocator>
template <bool IsConst>
Deque<T, Allocator>::CommonIterator<IsConst>::CommonIterator(
    const CommonIterator& other) {
  ptr_ = other.ptr_;
  array_index_ = other.array_index_;
  index_ = other.index_;
  deque_ = other.deque_;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename Deque<T, Allocator>::template CommonIterator<IsConst>&
Deque<T, Allocator>::CommonIterator<IsConst>::operator++() {
  if (index_ == kBucketSize - 1) {
    index_ = 0;
    ++array_index_;
    ptr_ = (*deque_)[array_index_];
  } else {
    ++index_;
    ++ptr_;
  }
  return *this;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename Deque<T, Allocator>::template CommonIterator<IsConst>&
Deque<T, Allocator>::CommonIterator<IsConst>::operator--() {
  if (index_ == 0) {
    index_ = kBucketSize - 1;
    --array_index_;
    ptr_ = (*deque_)[array_index_] + kBucketSize - 1;
  } else {
    --index_;
    --ptr_;
  }
  return *this;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename Deque<T, Allocator>::template CommonIterator<IsConst>
Deque<T, Allocator>::CommonIterator<IsConst>::operator++(int) {
  CommonIterator<IsConst> iterat = *this;
  *this += 1;
  return iterat;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename Deque<T, Allocator>::template CommonIterator<IsConst>
Deque<T, Allocator>::CommonIterator<IsConst>::operator--(int) {
  CommonIterator<IsConst> iterat = *this;
  *this -= 1;
  return iterat;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename Deque<T, Allocator>::template CommonIterator<IsConst>&
Deque<T, Allocator>::CommonIterator<IsConst>::operator+=(int number) {
  if (number != 0) {
    array_index_ += (index_ + number) / kBucketSize;
    if (index_ + number < ((index_ + number) / kBucketSize) * kBucketSize) {
      --array_index_;
    }
    index_ = (((index_ + number) % kBucketSize) + kBucketSize) % kBucketSize;
    ptr_ = (*deque_)[array_index_] + index_;
  }
  return *this;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename Deque<T, Allocator>::template CommonIterator<IsConst>&
Deque<T, Allocator>::CommonIterator<IsConst>::operator-=(int number) {
  if (number != 0) {
    array_index_ += (index_ - number) / kBucketSize;
    if (index_ - number < ((index_ - number) / kBucketSize) * kBucketSize) {
      --array_index_;
    }
    index_ = (((index_ - number) % kBucketSize) + kBucketSize) % kBucketSize;
    ptr_ = (*deque_)[array_index_] + index_;
  }
  return *this;
}

template <typename T, typename Allocator>
template <bool IsConst>
bool Deque<T, Allocator>::CommonIterator<IsConst>::operator<=(
    const CommonIterator<IsConst>& other) const {
  if (array_index_ == other.array_index_) {
    return index_ <= other.index_;
  }
  return array_index_ < other.array_index_;
}

template <typename T, typename Allocator>
template <bool IsConst>
bool Deque<T, Allocator>::CommonIterator<IsConst>::operator>=(
    const CommonIterator<IsConst>& other) const {
  if (array_index_ == other.array_index_) {
    return index_ >= other.index_;
  }
  return array_index_ > other.array_index_;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename Deque<T, Allocator>::template CommonIterator<IsConst>
Deque<T, Allocator>::CommonIterator<IsConst>::operator+(int number) const {
  CommonIterator<IsConst> new_iter(*this);
  new_iter += number;
  return new_iter;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename Deque<T, Allocator>::template CommonIterator<IsConst>
Deque<T, Allocator>::CommonIterator<IsConst>::operator-(int number) const {
  CommonIterator<IsConst> new_iter(*this);
  new_iter -= number;
  return new_iter;
}

template <typename T, typename Allocator>
typename Deque<T, Allocator>::template CommonIterator<false>
Deque<T, Allocator>::end() {
  if (size_ == 0) {
    return iterator();
  }
  iterator iter(external_[last_elem_.first] + last_elem_.second,
                last_elem_.first, last_elem_.second, &external_);
  if (size_ != 0) {
    ++iter;
  }
  return iter;
}

template <typename T, typename Allocator>
typename Deque<T, Allocator>::template CommonIterator<true>
Deque<T, Allocator>::cend() {
  const_iterator iter(external_[last_elem_.first] + last_elem_.second,
                      last_elem_.first, last_elem_.second, &external_);
  if (size_ != 0) {
    ++iter;
  }
  return iter;
}

template <typename T, typename Allocator>
typename Deque<T, Allocator>::template CommonIterator<true>
Deque<T, Allocator>::end() const {
  const_iterator iter(external_[last_elem_.first] + last_elem_.second,
                      last_elem_.first, last_elem_.second, &external_);
  if (size_ != 0) {
    ++iter;
  }
  return iter;
}

template <typename T, typename Allocator>
typename Deque<T, Allocator>::template CommonIterator<true>
Deque<T, Allocator>::cend() const {
  const_iterator iter(external_[last_elem_.first] + last_elem_.second,
                      last_elem_.first, last_elem_.second, &external_);
  if (size_ != 0) {
    ++iter;
  }
  return iter;
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::insert(iterator iter, const T& value) {
  if (size_ != 0 && iter != end()) {
    iterator last(--end());
    T elem = external_[last_elem_.first][last_elem_.second];
    while (last > iter) {
      iterator new_it(last);
      alloc_traits::destroy(alloc_, new_it.ptr_);
      alloc_traits::construct(alloc_, new_it.ptr_, *(--last));
    }
    alloc_traits::destroy(alloc_, last.ptr_);
    alloc_traits::construct(alloc_, last.ptr_, value);

    push_back(elem);
  } else {
    push_back(value);
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::erase(iterator other) {
  iterator iter(other);
  iterator last(--end());
  while (iter != last) {
    iterator new_it(iter);
    alloc_traits::destroy(alloc_, new_it.ptr_);
    alloc_traits::construct(alloc_, new_it.ptr_, *(++iter));
  }
  pop_back();
}


int main() {
    Accountant::reset();
    
    need_throw = true;
    
    try {
        Deque<ThrowingAccountant> d(8);
    } catch (...) {
        std::cout << (ctor_calls == 4) << "\n";
        std::cout << (dtor_calls == 4) << "\n\n";
    }
    
    need_throw = false;
    Deque<ThrowingAccountant> d(8);
    
    Deque<ThrowingAccountant> d2;
    for (int i = 0; i < 13; ++i) {
        d2.push_back(i);
    }
    
    Accountant::reset();
    need_throw = true;
    
    try {
        auto d3 = d2;
    } catch (...) {
        std::cout << (ctor_calls == 4) << "\n";
        std::cout << (dtor_calls == 4) << "\n\n";
    }
    
    Accountant::reset();
    
    try {
        d = d2;
    } catch (...) {
        std::cout << (ctor_calls == 4) << "\n";
        std::cout << (dtor_calls == 4) << "\n\n";
        
        // Actually it may not be 8 (although de facto it is), but the only thing we can demand here
        // is the abscence of memory leaks
        //
        //assert(lst.size() == 8);
        
    }
}

// не очевидно как действовать с первым и последним элементами при pop_front и pop_back
