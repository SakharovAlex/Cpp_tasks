#include <iostream>
#include <vector>

struct ThrowStruct {
  ThrowStruct(int value, bool throw_in_assignment, bool throw_in_copy) :
      value(value),
      throw_in_assignment(throw_in_assignment),
      throw_in_copy(throw_in_copy) {}

  ThrowStruct(const ThrowStruct& s) {
    value = s.value;
    throw_in_assignment = s.throw_in_assignment;
    throw_in_copy = s.throw_in_copy;

    if (throw_in_copy) {
      throw 1;
    }
  }

  ThrowStruct& operator=(const ThrowStruct& s) {
    if (throw_in_assignment) {
      throw 1;
    }

    value = s.value;
    throw_in_assignment = s.throw_in_assignment;
    throw_in_copy = s.throw_in_copy;

    return *this;
  }

  auto operator<=>(const ThrowStruct&) const = default;

  int value;
  bool throw_in_assignment;
  bool throw_in_copy;
};

template <typename T>
class Deque {
 public:
  Deque();

  Deque(const Deque<T>& other) { builder(other); }

  Deque(size_t count);

  Deque(size_t count, const T& value);

  Deque& operator=(const Deque& other);

  ~Deque();

  size_t size() const { return size_; }

  bool empty() const { return size_ == 0; }

  const T& operator[](size_t index) const;

  T& operator[](size_t index);

  const T& at(size_t index) const;

  T& at(size_t index);

  void bucket_down();

  void push_back(const T& value);

  void pop_back();

  void bucket_up();

  void push_front(const T& value);

  void pop_front();

  template <bool IsConst>
  class CommonIterator {
   public:
    using value_type = std::conditional_t<IsConst, const T, T>;
    using pointer = std::conditional_t<IsConst, const T*, T*>;
    using iterator_category = std::random_access_iterator_tag;
    using reference = std::conditional_t<IsConst, const T&, T&>;
    using difference_type = std::ptrdiff_t;

    CommonIterator() = delete;

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

  void erase(iterator other);

 private:
  std::vector<T*> external_;
  size_t size_;
  std::pair<size_t, size_t> first_elem_;
  std::pair<size_t, size_t> last_elem_;
  size_t number_of_buckets_;
  const size_t kSizeOfBucket = 32;

  void try_build_from_other(const Deque<T>& other, size_t& index1,
                            size_t& index2);

  void catch_build_from_other(size_t& index1, size_t& index2);

  void builder(const Deque<T>& other);

  void try_build_size_of_count(size_t& index1, size_t& index2, const T& value);

  void catch_build_size_of_count(size_t& index1, size_t& index2);

  void clear();

  void try_in_copy(const Deque<T>& other, size_t& index1, size_t& index2);

  void catch_in_copy(size_t& index1, size_t& index2);
};

template <typename T>
Deque<T>::Deque() {
  size_ = 0;
  first_elem_ = {1, 0};
  last_elem_ = {1, 0};
  number_of_buckets_ = 3;
  external_.resize(3);
  for (size_t i = 0; i < 3; ++i) {
    external_[i] = reinterpret_cast<T*>(new int8_t[kSizeOfBucket * sizeof(T)]);
  }
}

template <typename T>
void Deque<T>::try_build_from_other(const Deque<T>& other, size_t& index1,
                                    size_t& index2) {
  for (index2 = first_elem_.second; index2 < kSizeOfBucket && index2 < size_;
       ++index2) {
    new (external_[index1] + index2) T(other.external_[index1][index2]);
  }
  for (index1 = first_elem_.first + 1; index1 <= last_elem_.first; ++index1) {
    for (index2 = 0;
         index2 < kSizeOfBucket &&
         (index2 + kSizeOfBucket * (index1 - first_elem_.first)) < size_;
         ++index2) {
      new (external_[index1] + index2) T(other.external_[index1][index2]);
    }
  }
}

template <typename T>
void Deque<T>::catch_build_from_other(size_t& index1, size_t& index2) {
  if (index1 == first_elem_.first) {
    for (size_t i = first_elem_.second; i < index2; ++i) {
      (external_[index1] + i)->~T();
    }
  } else {
    for (size_t i = first_elem_.second; i < kSizeOfBucket; ++i) {
      (external_[first_elem_.first] + i)->~T();
    }
    for (size_t i = 0; i < index2; ++i) {
      (external_[index1] + i)->~T();
    }
    for (size_t i = first_elem_.first + 1; i < index1; ++i) {
      for (size_t j = 0; j < kSizeOfBucket; ++j) {
        (external_[i] + j)->~T();
      }
    }
    for (size_t i = 0; i < external_.size(); ++i) {
      delete[] reinterpret_cast<int8_t*>(external_[i]);
    }
  }
}

template <typename T>
void Deque<T>::builder(const Deque<T>& other) {
  size_ = other.size_;
  first_elem_ = other.first_elem_;
  last_elem_ = other.last_elem_;
  number_of_buckets_ = other.number_of_buckets_;
  external_.assign(number_of_buckets_, nullptr);
  for (size_t i = 0; i < number_of_buckets_; ++i) {
    external_[i] = reinterpret_cast<T*>(new int8_t[kSizeOfBucket * sizeof(T)]);
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

template <typename T>
Deque<T>::Deque(size_t count) {
  if (count > 0) {
    number_of_buckets_ =
        std::max(2 * (count / kSizeOfBucket + 1), static_cast<size_t>(4));
    size_ = count;
    first_elem_.first = number_of_buckets_ / 4;
    first_elem_.second = 0;
    last_elem_.first = first_elem_.first + (count - 1) / kSizeOfBucket;
    last_elem_.second = (count - 1) % kSizeOfBucket;
    external_.resize(number_of_buckets_, nullptr);
    for (size_t i = 0; i < number_of_buckets_; ++i) {
      external_[i] =
          reinterpret_cast<T*>(new int8_t[kSizeOfBucket * sizeof(T)]);
    }
    size_t index1 = first_elem_.first;
    size_t index2 = 0;
    try {
      try_build_size_of_count(index1, index2, 0);
    } catch (...) {
      catch_build_size_of_count(index1, index2);
      external_.clear();
      external_.shrink_to_fit();
      throw;
    }
  }
}

template <typename T>
void Deque<T>::try_build_size_of_count(size_t& index1, size_t& index2,
                                       const T& value) {
  for (; index1 <= last_elem_.first; ++index1) {
    for (index2 = 0;
         index2 < kSizeOfBucket &&
         (index2 + kSizeOfBucket * (index1 - first_elem_.first)) < size_;
         ++index2) {
      new (external_[index1] + index2) T(value);
    }
  }
}

template <typename T>
void Deque<T>::catch_build_size_of_count(size_t& index1, size_t& index2) {
  for (size_t i = 0; i < index2; ++i) {
    (external_[index1] + i)->~T();
  }
  for (size_t i = first_elem_.first; i < index1; ++i) {
    for (size_t j = 0; j < kSizeOfBucket; ++j) {
      (external_[i] + j)->~T();
    }
  }
  for (size_t i = 0; i < external_.size(); ++i) {
    delete[] reinterpret_cast<int8_t*>(external_[i]);
  }
}

template <typename T>
Deque<T>::Deque(size_t count, const T& value) {
  if (count > 0) {
    number_of_buckets_ =
        std::max(2 * (count / kSizeOfBucket + 1), static_cast<size_t>(4));
    size_ = count;
    first_elem_.first = number_of_buckets_ / 4;
    first_elem_.second = 0;
    last_elem_.first = first_elem_.first + (count - 1) / kSizeOfBucket;
    last_elem_.second = (count - 1) % kSizeOfBucket;
    external_.resize(number_of_buckets_, nullptr);
    for (size_t i = 0; i < number_of_buckets_; ++i) {
      external_[i] =
          reinterpret_cast<T*>(new int8_t[kSizeOfBucket * sizeof(T)]);
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

template <typename T>
void Deque<T>::clear() {
  if (first_elem_.first != last_elem_.first) {
    for (size_t i = first_elem_.second; i < kSizeOfBucket; ++i) {
      (external_[first_elem_.first])->~T();
    }
    for (size_t i = 0; i <= last_elem_.second; ++i) {
      (external_[last_elem_.first] + i)->~T();
    }
    for (size_t i = first_elem_.first + 1; i < last_elem_.first; ++i) {
      for (size_t j = 0; j < kSizeOfBucket; ++j) {
        (external_[i] + j)->~T();
      }
    }
  } else {
    for (size_t i = first_elem_.second; i < last_elem_.second; ++i) {
      (external_[first_elem_.first] + i)->~T();
    }
  }
  for (size_t i = 0; i < external_.size(); ++i) {
    delete[] reinterpret_cast<int8_t*>(external_[i]);
  }
}

template <typename T>
void Deque<T>::try_in_copy(const Deque<T>& other, size_t& index1,
                           size_t& index2) {
  for (index2 = first_elem_.second; index2 < kSizeOfBucket && index2 < size_;
       ++index2) {
    new (external_[index1] + index2) T(other.external_[index1][index2]);
  }
  for (index1 = first_elem_.first + 1; index1 <= last_elem_.first; ++index1) {
    for (index2 = 0;
         index2 < kSizeOfBucket &&
         (index2 + kSizeOfBucket * (index1 - first_elem_.first)) < size_;
         ++index2) {
      new (external_[index1] + index2) T(other.external_[index1][index2]);
    }
  }
}

template <typename T>
void Deque<T>::catch_in_copy(size_t& index1, size_t& index2) {
  if (index1 == first_elem_.first) {
    for (size_t j = first_elem_.second; j < index2; ++j) {
      (external_[index1] + j)->~T();
    }
  } else {
    for (size_t j = first_elem_.second; j < kSizeOfBucket; ++j) {
      (external_[first_elem_.first] + j)->~T();
    }
    for (size_t j = 0; j < index2; ++j) {
      (external_[index1] + j)->~T();
    }
    for (size_t j = first_elem_.first + 1; j < index1; ++j) {
      for (size_t i = 0; i < kSizeOfBucket; ++i) {
        (external_[j] + i)->~T();
      }
    }
    for (size_t j = 0; j < external_.size(); ++j) {
      delete[] reinterpret_cast<int8_t*>(external_[j]);
    }
  }
}

template <typename T>
Deque<T>& Deque<T>::operator=(const Deque& other) {
  clear();
  size_ = other.size_;
  first_elem_ = other.first_elem_;
  last_elem_ = other.last_elem_;
  number_of_buckets_ = other.number_of_buckets_;
  external_.resize(number_of_buckets_, nullptr);
  for (size_t i = 0; i < number_of_buckets_; ++i) {
    external_[i] = reinterpret_cast<T*>(new int8_t[kSizeOfBucket * sizeof(T)]);
  }
  size_t index1 = first_elem_.first;
  size_t index2 = 0;
  try {
    try_in_copy(other, index1, index2);
  } catch (...) {
    catch_in_copy(index1, index2);
    external_.clear();
    external_.shrink_to_fit();
    throw;
  }
  return *this;
}

template <typename T>
Deque<T>::~Deque() {
  if (size_ != 0) {
    if (first_elem_.first == last_elem_.first) {
      for (size_t i = first_elem_.second; i < last_elem_.second; ++i) {
        (external_[first_elem_.first] + i)->~T();
      }
    } else {
      for (size_t i = first_elem_.second; i < kSizeOfBucket; ++i) {
        (external_[first_elem_.first] + i)->~T();
      }
      for (size_t i = 0; i <= last_elem_.second; ++i) {
        (external_[last_elem_.first] + i)->~T();
      }
      for (size_t i = first_elem_.first + 1; i < last_elem_.first; ++i) {
        for (size_t j = 0; j < kSizeOfBucket; ++j) {
          (external_[i] + j)->~T();
        }
      }
    }
  }
  for (size_t i = 0; i < external_.size(); ++i) {
    delete[] reinterpret_cast<int8_t*>(external_[i]);
  }
}

template <typename T>
const T& Deque<T>::operator[](size_t index) const {
  size_t number_of_bucket =
      (index + first_elem_.second) / kSizeOfBucket + first_elem_.first;
  size_t position = (index + first_elem_.second) % kSizeOfBucket;
  return external_[number_of_bucket][position];
}

template <typename T>
T& Deque<T>::operator[](size_t index) {
  size_t number_of_bucket =
      (index + first_elem_.second) / kSizeOfBucket + first_elem_.first;
  size_t position = (index + first_elem_.second) % kSizeOfBucket;
  return external_[number_of_bucket][position];
}

template <typename T>
const T& Deque<T>::at(size_t index) const {
  if (index >= size_) {
    throw std::out_of_range("...");
  }
  size_t number_of_bucket =
      (index + first_elem_.second) / kSizeOfBucket + first_elem_.first;
  size_t position = (index + first_elem_.second) % kSizeOfBucket;
  return external_[number_of_bucket][position];
}

template <typename T>
T& Deque<T>::at(size_t index) {
  if (index >= size_) {
    throw std::out_of_range("...");
  }
  size_t number_of_bucket =
      (index + first_elem_.second) / kSizeOfBucket + first_elem_.first;
  size_t position = (index + first_elem_.second) % kSizeOfBucket;
  return external_[number_of_bucket][position];
}

template <typename T>
void Deque<T>::bucket_down() {
  number_of_buckets_ *= 2;
  std::vector<T*> new_external(number_of_buckets_, nullptr);
  size_t index = 0;
  try {
    for (; index < external_.size(); ++index) {
      new_external[number_of_buckets_ / 2 + index] =
          reinterpret_cast<T*>(new int8_t[kSizeOfBucket * sizeof(T)]);
    }
  } catch (...) {
    number_of_buckets_ /= 2;
    for (size_t i = 0; i < index; ++i) {
      delete[] reinterpret_cast<int8_t*>(new_external[i]);
    }
    throw;
  }
  for (size_t i = 0; i < external_.size(); ++i) {
    new_external[i] = external_[i];
  }
  external_ = new_external;
}

template <typename T>
void Deque<T>::push_back(const T& value) {
  if (size_ == 0) {
    ++size_;
    (external_[first_elem_.first] + first_elem_.second)->~T();
    try {
      new (external_[last_elem_.first]) T(value);
    } catch (...) {
      (external_[last_elem_.first])->~T();
    }
  } else {
    ++size_;
    if (last_elem_.second == kSizeOfBucket - 1) {
      ++last_elem_.first;
      last_elem_.second = 0;
      try {
        new (external_[last_elem_.first]) T(value);
      } catch (...) {
        (external_[last_elem_.first])->~T();
      }
    } else {
      ++last_elem_.second;
      try {
        new (external_[last_elem_.first] + last_elem_.second) T(value);
      } catch (...) {
        (external_[last_elem_.first] + last_elem_.second)->~T();
      }
    }
    if (last_elem_.first == external_.size() - 1 &&
        last_elem_.second == kSizeOfBucket - 1) {
      bucket_down();
    }
  }
}

template <typename T>
void Deque<T>::pop_back() {
  --size_;
  (external_[last_elem_.first] + last_elem_.second)->~T();
  if (last_elem_.second == 0) {
    last_elem_.second = kSizeOfBucket - 1;
    --last_elem_.first;
  } else {
    --last_elem_.second;
  }
}

template <typename T>
void Deque<T>::bucket_up() {
  number_of_buckets_ *= 2;
  std::vector<T*> new_external(number_of_buckets_, nullptr);
  size_t index = 0;
  try {
    for (; index < external_.size(); ++index) {
      new_external[index] =
          reinterpret_cast<T*>(new int8_t[kSizeOfBucket * sizeof(T)]);
    }
  } catch (...) {
    number_of_buckets_ /= 2;
    for (size_t i = 0; i < index; ++i) {
      delete[] reinterpret_cast<int8_t*>(new_external[i]);
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

template <typename T>
void Deque<T>::push_front(const T& value) {
  if (size_ == 0) {
    ++size_;
    (external_[first_elem_.first] + first_elem_.second)->~T();
    try {
      new (external_[first_elem_.first]) T(value);
    } catch (...) {
      (external_[first_elem_.first])->~T();
    }
  } else {
    ++size_;
    if (first_elem_.second == 0) {
      --first_elem_.first;
      first_elem_.second = kSizeOfBucket - 1;
      try {
        new (external_[first_elem_.first] + first_elem_.second) T(value);
      } catch (...) {
        (external_[first_elem_.first] + first_elem_.second)->~T();
      }
    } else {
      --first_elem_.second;
      try {
        new (external_[first_elem_.first] + first_elem_.second) T(value);
      } catch (...) {
        (external_[first_elem_.first] + first_elem_.second)->~T();
      }
    }
    if (first_elem_.first == 0 && first_elem_.second == 0) {
      bucket_up();
    }
  }
}

template <typename T>
void Deque<T>::pop_front() {
  --size_;
  (external_[first_elem_.first] + first_elem_.second)->~T();
  if (first_elem_.second == kSizeOfBucket - 1) {
    first_elem_.second = 0;
    ++first_elem_.first;
  } else {
    ++first_elem_.second;
  }
}

template <typename T>
template <bool IsConst>
typename Deque<T>::template CommonIterator<IsConst>&
Deque<T>::CommonIterator<IsConst>::operator=(const CommonIterator& other) {
  ptr_ = other.ptr_;
  array_index_ = other.array_index_;
  index_ = other.index_;
  deque_ = other.deque_;
  return *this;
}

template <typename T>
template <bool IsConst>
Deque<T>::CommonIterator<IsConst>::CommonIterator(const CommonIterator& other) {
  ptr_ = other.ptr_;
  array_index_ = other.array_index_;
  index_ = other.index_;
  deque_ = other.deque_;
}

template <typename T>
template <bool IsConst>
typename Deque<T>::template CommonIterator<IsConst>&
Deque<T>::CommonIterator<IsConst>::operator++() {
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

template <typename T>
template <bool IsConst>
typename Deque<T>::template CommonIterator<IsConst>&
Deque<T>::CommonIterator<IsConst>::operator--() {
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

template <typename T>
template <bool IsConst>
typename Deque<T>::template CommonIterator<IsConst>
Deque<T>::CommonIterator<IsConst>::operator++(int) {
  CommonIterator<IsConst> iterat = *this;
  *this += 1;
  return iterat;
}

template <typename T>
template <bool IsConst>
typename Deque<T>::template CommonIterator<IsConst>
Deque<T>::CommonIterator<IsConst>::operator--(int) {
  CommonIterator<IsConst> iterat = *this;
  *this -= 1;
  return iterat;
}

template <typename T>
template <bool IsConst>
typename Deque<T>::template CommonIterator<IsConst>&
Deque<T>::CommonIterator<IsConst>::operator+=(int number) {
  array_index_ += (index_ + number) / kBucketSize;
  if (index_ + number < ((index_ + number) / kBucketSize) * kBucketSize) {
    --array_index_;
  }
  index_ = (((index_ + number) % kBucketSize) + kBucketSize) % kBucketSize;
  ptr_ = (*deque_)[array_index_] + index_;
  return *this;
}

template <typename T>
template <bool IsConst>
typename Deque<T>::template CommonIterator<IsConst>&
Deque<T>::CommonIterator<IsConst>::operator-=(int number) {
  array_index_ += (index_ - number) / kBucketSize;
  if (index_ - number < ((index_ - number) / kBucketSize) * kBucketSize) {
    --array_index_;
  }
  index_ = (((index_ - number) % kBucketSize) + kBucketSize) % kBucketSize;
  ptr_ = (*deque_)[array_index_] + index_;
  return *this;
}

template <typename T>
template <bool IsConst>
bool Deque<T>::CommonIterator<IsConst>::operator<=(
    const CommonIterator<IsConst>& other) const {
  if (array_index_ == other.array_index_) {
    return index_ <= other.index_;
  }
  return array_index_ < other.array_index_;
}

template <typename T>
template <bool IsConst>
bool Deque<T>::CommonIterator<IsConst>::operator>=(
    const CommonIterator<IsConst>& other) const {
  if (array_index_ == other.array_index_) {
    return index_ >= other.index_;
  }
  return array_index_ > other.array_index_;
}

template <typename T>
template <bool IsConst>
typename Deque<T>::template CommonIterator<IsConst>
Deque<T>::CommonIterator<IsConst>::operator+(int number) const {
  CommonIterator<IsConst> new_iter(*this);
  new_iter += number;
  return new_iter;
}

template <typename T>
template <bool IsConst>
typename Deque<T>::template CommonIterator<IsConst>
Deque<T>::CommonIterator<IsConst>::operator-(int number) const {
  CommonIterator<IsConst> new_iter(*this);
  new_iter -= number;
  return new_iter;
}

template <typename T>
typename Deque<T>::template CommonIterator<false> Deque<T>::end() {
  iterator iter(external_[last_elem_.first] + last_elem_.second,
                last_elem_.first, last_elem_.second, &external_);
  if (size_ != 0) {
    ++iter;
  }
  return iter;
}

template <typename T>
typename Deque<T>::template CommonIterator<true> Deque<T>::cend() {
  const_iterator iter(external_[last_elem_.first] + last_elem_.second,
                      last_elem_.first, last_elem_.second, &external_);
  if (size_ != 0) {
    ++iter;
  }
  return iter;
}

template <typename T>
typename Deque<T>::template CommonIterator<true> Deque<T>::end() const {
  const_iterator iter(external_[last_elem_.first] + last_elem_.second,
                      last_elem_.first, last_elem_.second, &external_);
  if (size_ != 0) {
    ++iter;
  }
  return iter;
}

template <typename T>
typename Deque<T>::template CommonIterator<true> Deque<T>::cend() const {
  const_iterator iter(external_[last_elem_.first] + last_elem_.second,
                      last_elem_.first, last_elem_.second, &external_);
  if (size_ != 0) {
    ++iter;
  }
  return iter;
}

template <typename T>
void Deque<T>::insert(iterator iter, const T& value) {
  if (size_ != 0 && iter != end()) {
    iterator last(--end());
    T elem = external_[last_elem_.first][last_elem_.second];
    while (last > iter) {
      iterator new_it(last);
      (new_it.ptr_)->~T();
      try {
        new (new_it.ptr_) T(*(--last));
      } catch (...) {
        (new_it.ptr_)->~T();
      }
    }
    (last.ptr_)->~T();
    try {
      new (last.ptr_) T(value);
    } catch (...) {
      (last.ptr_)->~T();
    }
    push_back(elem);
  } else {
    (end().ptr_)->~T();
    try {
      new (end().ptr_) T(value);
    } catch (...) {
      (end().ptr_)->~T();
    }
    push_back(value);
  }
}

template <typename T>
void Deque<T>::erase(iterator other) {
  iterator iter(other);
  iterator last(--end());
  while (iter != last) {
    iterator new_it(iter);
    (new_it.ptr_)->~T();
    try {
      new (new_it.ptr_) T(*(++iter));
    } catch (...) {
      (new_it.ptr_)->~T();
    }
  }
  pop_back();
}




int main() {
    Deque<int> d(10, 1);
    for (int i = 0; i < 10; ++i) {
        d[i] = i + 1;
    }

    auto start_size = d.size();

    d.insert(d.begin() + static_cast<int>(start_size) / 2, 2);
    d.insert(d.begin() + static_cast<int>(start_size) / 2, 3);
    //EXPECT_TRUE(d.size() == start_size + 1);
    //std::cout << (d.size() == start_size + 1) << "\n";
    //d.erase(d.begin() + static_cast<int>(start_size) / 2 - 1);
    //std::cout << (d.size() == start_size) << "\n";

    //EXPECT_TRUE(d.size() == start_size);
    std::cout << (size_t(std::count(d.begin(), d.end(), 1) == start_size - 1)) << "\n";
    std::cout << (size_t(std::count(d.begin(), d.end(), 2) == 1)) << "\n";

      //EXPECT_TRUE(size_t(std::count(d.begin(), d.end(), NotDefaultConstructible{1}))
                     // == start_size - 1);
      //EXPECT_TRUE(std::count(d.begin(), d.end(), NotDefaultConstructible{2}) == 1);
    std::cout << "\n";
      Deque<int> copy;
    //std::cout << copy[0] << "\n";
    for (auto it = d.begin(); it != d.end(); ++it) std::cout << *it << " ";
    std::cout << "\n";
    //for (auto it = copy.begin(); it != copy.end(); ++it) std::cout << *it << " ";
    std::cout << "\n";
      for (const auto& item: d) {
          std::cout << "\n";
          std::cout << item << "\n";
        copy.insert(copy.end(), item);
          for (auto it = copy.begin(); it != copy.end(); ++it) std::cout << *it << " ";
          std::cout << "\n";
      }
    std::cout << "\n";
    
    for (auto it = copy.begin(); it != copy.begin() + static_cast<int>((d.end() - d.begin())); ++it) std::cout << *it << " ";
    std::cout << (d.size() == copy.size()) << "\n";
    for (auto it = copy.begin(); it != copy.end(); ++it) std::cout << *it << " ";
    std::cout << (d.size() == copy.size()) << "\n";
    std::cout << std::equal(d.begin(), d.end(), copy.begin()) << "\n";
      //EXPECT_TRUE(d.size() == copy.size());
      //EXPECT_TRUE(std::equal(d.begin(), d.end(), copy.begin()));
    
    
    Deque<ThrowStruct> deq(10, ThrowStruct(10, true, false));

    std::cout << (deq.size() == 10) << "\n";
    std::cout << (deq[0].value == 10) << "\n";
    
    Deque<ThrowStruct> deq2(1, ThrowStruct(10, false, false));
    
    std::cout << (deq2.size() == 1) << "\n";
    
    return 0;
}
