#ifndef CPR_COOKIES_H
#define CPR_COOKIES_H

#include <initializer_list>
#include <map>
#include <sstream>
#include <string>

namespace cpr {

class Cookies {
    using container_type = std::map<std::string, std::string>;
    
  public:
    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;
    using reverse_iterator = typename container_type::reverse_iterator;
    using const_reverse_iterator = typename container_type::const_reverse_iterator;
    
  public:
    Cookies() {}
    Cookies(const std::initializer_list<std::pair<const std::string, std::string>>& pairs);
    Cookies(const container_type& map) : map_{map} {}

    std::string GetEncoded() const;
    
  public:
    std::string& operator[](const std::string& key);
    
    std::string& at(const std::string& key);  
    const std::string& at(const std::string& key) const; 
    
  public:
    iterator begin(); 
    iterator end();
    
    const_iterator begin() const;   
    const_iterator end() const;
    
    const_iterator cbegin() const;   
    const_iterator cend() const;
    
    reverse_iterator rbegin();
    reverse_iterator rend();
    
    const_reverse_iterator rbegin() const;
    const_reverse_iterator rend() const;
    
    const_reverse_iterator crbegin() const;  
    const_reverse_iterator crend() const;

  private:
    container_type map_;
};

} // namespace cpr

#endif
