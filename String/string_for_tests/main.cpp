#include <iostream>
#include <vector>

class String {
 public:
  String();
  String(size_t size, char character);
  String(const char* str);
  String(const String& str);
  ~String();
  void Clear();
  void PushBack(char character);
  void PopBack();
  void Resize(size_t new_size);
  void Resize(size_t new_size, char character);
  void Reserve(size_t new_cap);
  void ShrinkToFit();
  void Swap(String& other);
  const char& operator[](int i) const;
  char& operator[](int i);
  char& Front();
  char Front() const;
  char& Back();
  char Back() const;
  bool Empty() const;
  size_t Size() const;
  size_t Capacity() const;
  const char* Data() const;
  char* Data();
  bool operator<(const String& str) const;
  bool operator>(const String& str) const;
  bool operator<=(const String& str) const;
  bool operator>=(const String& str) const;
  bool operator==(const String& str) const;
  bool operator!=(const String& str) const;
  String& operator=(const String& str);
  String& operator=(const char* str);
  String operator+(const String& str) const;
  String& operator+=(const String& str);
  String operator*(size_t n) const;
  String& operator*=(size_t n);
  friend std::ostream& operator<<(std::ostream& out, const String& str);
  friend std::istream& operator>>(std::istream& in, String& str);
  std::vector<String> Split(const String& delim = " ");
  String Join(const std::vector<String>& strings) const;

 private:
  char* string_;
  size_t size_;
  size_t capacity_;
};

String::String() {
  string_ = nullptr;
  size_ = 0;
  capacity_ = 0;
}

String::String(size_t size, char character) {
  size_ = size;
  capacity_ = size;
  if (size == 0) {
    string_ = nullptr;
  } else {
    string_ = new char[capacity_ + 1];
    for (size_t i = 0; i < size; ++i) {
      string_[i] = character;
    }
    string_[size_] = '\0';
  }
}

String::String(const char* str) {
  size_ = strlen(str);
  capacity_ = size_;
  string_ = new char[capacity_ + 1];
  for (size_t i = 0; i < size_; ++i) {
    string_[i] = str[i];
  }
  string_[size_] = '\0';
}

String::String(const String& str) {
  size_ = str.Size();
  capacity_ = str.Capacity();
  string_ = new char[capacity_ + 1];
  for (size_t i = 0; i < size_; ++i) {
    string_[i] = str[i];
  }
  string_[size_] = '\0';
}

String::~String() {
  delete[] string_;
  size_ = 0;
  capacity_ = 0;
}

void String::Clear() {
  size_ = 0;
  capacity_ = 0;
  string_[0] = '\0';
}

void String::PushBack(char character) {
  Reserve(size_ + 1);
  string_[size_] = character;
  ++size_;
  string_[size_] = '\0';
}

void String::PopBack() {
  if (size_ > 0) {
    --size_;
    string_[size_] = '\0';
  }
}

void String::Resize(size_t new_size) {
  if (new_size > size_) {
    if (size_ == 0) {
      ++size_;
    }
    while (new_size > capacity_) {
      capacity_ *= 2;
    }
    char* str = new char[capacity_ + 1];
    if (string_ != nullptr) {
      for (size_t i = 0; i <= size_; ++i) {
        str[i] = string_[i];
      }
      delete[] string_;
      string_ = str;
    }
  }
  size_ = new_size;
}

void String::Resize(size_t new_size, char character) {
  size_t old_size = size_;
  Resize(new_size);
  if (new_size > old_size) {
    for (size_t i = old_size; i < new_size; ++i) {
      string_[i] = character;
    }
  }
  size_ = new_size;
  string_[size_] = '\0';
}

void String::Reserve(size_t new_cap) {
  if (new_cap > capacity_) {
    char* str = new char[new_cap + 1];
    if (string_ != nullptr) {
      for (size_t i = 0; i <= size_; ++i) {
        str[i] = string_[i];
      }
    }
    delete[] string_;
    string_ = str;
    capacity_ = new_cap;
  }
}

void String::ShrinkToFit() {
  char* str = new char[size_ + 1];
  for (size_t i = 0; i <= size_; ++i) {
    str[i] = string_[i];
  }
  delete[] string_;
  string_ = str;
  capacity_ = size_;
}

void String::Swap(String& other) {
  char* str = other.string_;
  size_t other_size = other.size_;
  size_t other_capacity = other.capacity_;
  other.string_ = string_;
  other.size_ = size_;
  other.capacity_ = capacity_;
  string_ = str;
  size_ = other_size;
  capacity_ = other_capacity;
}

const char& String::operator[](int i) const { return string_[i]; }

char& String::operator[](int i) { return string_[i]; }

char& String::Front() { return string_[0]; }

char String::Front() const { return string_[0]; }

char& String::Back() { return string_[size_ - 1]; }

char String::Back() const { return string_[size_ - 1]; }

bool String::Empty() const { return size_ == 0; }

size_t String::Size() const { return size_; }

size_t String::Capacity() const { return capacity_; }

const char* String::Data() const { return string_; }

char* String::Data() { return string_; }

bool String::operator<(const String& str) const {
  size_t max;
  max = (size_ > str.size_) ? size_ : str.size_;
  for (size_t i = 0; i < max; ++i) {
    if (i > size_) {
      return true;
    }
    if (i > str.size_) {
      return false;
    }
    if (string_[i] < str.string_[i]) {
      return true;
    }
    if (string_[i] > str.string_[i]) {
      return false;
    }
  }
  return false;
}

bool String::operator==(const String& str) const {
  if (Size() != str.Size()) {
    return false;
  }
  size_t i = 0;
  while (Size() > i && string_[i] == str[i]) {
    ++i;
  }
  return (Size() == i);
}

bool String::operator>(const String& str) const { return !(operator<=(str)); }

bool String::operator<=(const String& str) const {
  return (operator<(str) || operator==(str));
}

bool String::operator>=(const String& str) const {
  return (operator>(str) || operator==(str));
}

bool String::operator!=(const String& str) const { return !(operator==(str)); }

String& String::operator=(const String& str) {
  String copy_str = str;
  Swap(copy_str);
  return *this;
}

String& String::operator=(const char* str) {
  Resize(strlen(str));
  for (size_t i = 0; i < size_; ++i) {
    string_[i] = str[i];
  }
  string_[size_] = '\0';
  return *this;
}

String String::operator+(const String& str) const {
  String new_str = *this;
  new_str += str;
  return new_str;
}

String& String::operator+=(const String& str) {
  size_t length = capacity_ + str.Capacity();
  Reserve(length);
  for (size_t i = size_; i < length; ++i) {
    string_[i] = str[i - size_];
  }
  size_ = length;
  string_[size_] = '\0';
  return *this;
}

String String::operator*(size_t n) const {
  String answer = "\0";
  answer.Reserve(size_ * n);
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < size_; ++j) {
      answer.PushBack(string_[j]);
    }
  }
  answer.string_[size_ * n] = '\0';
  return answer;
}

String& String::operator*=(size_t n) {
  String answer;
  answer = *this * n;
  *this = answer;
  return *this;
}

std::vector<String> String::Split(const String& delim) {
  std::vector<String> answer;
  String str;
  str.Reserve(delim.Size());
  size_t index = 0;
  for (size_t i = 0; i <= size_; ++i) {
    if (i < delim.Size()) {
      str.PushBack(string_[i]);
    } else {
      if (str == delim) {
        String ans;
        for (; index < i - delim.Size(); ++index) {
          ans.PushBack(string_[index]);
        }
        answer.push_back(ans);
        index += delim.Size();
      }
      for (size_t k = 0; k < delim.Size() - 1; ++k) {
        str.string_[k] = str.string_[k + 1];
      }
      str[delim.Size() - 1] = string_[i];
    }
  }
  String res;
  while (index < size_) {
    res.PushBack(string_[index]);
    ++index;
  }
  answer.push_back(res);
  return answer;
}

String String::Join(const std::vector<String>& strings) const {
  String answer = "\0";
  for (size_t i = 0; i < strings.size(); ++i) {
    answer += strings[i];
    if (i != strings.size() - 1) {
      answer += *this;
    }
  }
  return answer;
}

std::ostream& operator<<(std::ostream& out, const String& str) {
  for (size_t i = 0; i < str.size_; ++i) {
    out << str.string_[i];
  }
  return out;
}

std::istream& operator>>(std::istream& in, String& str) {
  if (str.string_ != nullptr) {
    str.Clear();
  }
  char symbol;
  while (in.get(symbol) && !in.eof() && symbol != ' ') {
    str.PushBack(symbol);
  }
  return in;
}


int main(int argc, const char * argv[]) {
    String s;
    std::cin >> s;
    String str = "abc";
    std::vector<String> sss = s.Split(str);
    for (int i = 0; i < sss.size(); ++i) {
        std::cout << sss[i] << " ";
    }
    return 0;
}
