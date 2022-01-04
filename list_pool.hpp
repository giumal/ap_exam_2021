#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <vector>


template <typename node, typename T,typename N>
class _iterator {
  std::vector<node>* Pool;
  T current;
 public:
  using value_type = N;
  using reference = value_type&;
  using pointer = value_type*;
  using iterator_category =  std::random_access_iterator_tag;

  explicit _iterator(std::vector<node>* p,T x):Pool{p},current{x}{}

  value_type operator*() const { return (*Pool)[current-1].value; }

  _iterator& operator++() {  // pre-increment
    current=(*Pool)[current-1].next;
    return *this;
  }
  _iterator operator++(int) {  // post-increment
    auto tmp = *this;
    ++(*this);
    return tmp;
  }

  friend bool operator==(const _iterator& x, const _iterator& y) {
    return (x.current == y.current);// && x.current.value == y.current.value);
  }

  friend bool operator!=(const _iterator& x, const _iterator& y) {
    return !(x == y);
  }
};

template<typename T, typename N=std::size_t>
class list_pool{
    private:   
    //the member of the clas is a vector and a struct
    struct node_t{
        T value;
        N next;
    };
    std::vector<node_t> pool;

    using list_type = N;
    using value_type = T;
    using size_type = typename std::vector<node_t>::size_type;

    list_type free_node_list=end();
    list_type count=0;

    node_t& node(list_type x) noexcept { return pool[x-1]; }
    const node_t& node(list_type x) const noexcept { return pool[x-1]; }

    public:

    list_pool()=default;

    explicit list_pool(size_type n){
        std::cout<<"Constructor "<<std::endl;
        pool.reserve(n);
    }

    // copy constructor 
    list_pool(const list_pool& source):pool{source.pool}{
        std::cout<<"Copy constructor "<<std::endl;
    }

    //move constructor
    list_pool(list_pool&& source):pool{std::move(source.pool)}{
        std::cout<<"Move constructor "<<std::endl;
    }

    //move assignment
    list_pool& operator=(list_pool&& source){
        std::cout<<"Move assignment... "<<std::endl;
        pool=std::move(source.pool);
        return *this;
    }

    // copy assignment 
    list_pool& operator=(const list_pool& source){
        std::cout<<"Copy assignment... "<<std::endl;
        pool.clear();
        auto tmp=source;
        (*this) = std::move(tmp);
        return *this;
    }

    list_type _push_front(T&& val, list_type head){
        check_capacity(count);
        if(free_node_list!=0){
            register list_type tmp{free_node_list};
            free_node_list=next(free_node_list);
            value(tmp)=std::forward<T>(val);     
            next(tmp)=head;     
            return tmp;
        }else{
            ++count;
            value(count)=std::forward<T>(val);
            next(count)=head;
            return count;
        }
    }

    list_type push_front(const T& val, list_type head){
        return _push_front(val,head);
    }

    list_type push_front(T&& val, list_type head){
        return _push_front(std::move(val),head);
    }

    list_type push_back(T&& val, list_type head){
        return _push_back(std::move(val),head);
    }

    list_type push_back(const T& val, list_type head){
        return _push_back(val,head);
    }

    list_type _push_back(T&& val, list_type head){
        check_capacity(count);
        register list_type tmp{head};
        if(head==0){ //new list is insert
            ++count;
            value(count)=val;
            next(count)=end();
            return count;
        }
        while(next(tmp)!=0){
            tmp=next(tmp);
        }
        if(free_node_list!=0){
            next(tmp)=free_node_list;
            value(free_node_list)=std::forward<T>(val);
            next(free_node_list)=end();
            free_node_list=next(free_node_list);
            return head;
        }else{
            ++count;
            next(tmp)=count;
            value(count)=std::forward<T>(val);
            next(count)=end();
            return head;
        }
    }


    void check_capacity(size_type x) {
        // register N tmp{vec.capacity()};
        if (x < capacity()){
        // std::cout<<"Nothing to do"<<std::endl;
        return;
        }else if(x==0){
            reserve(8);
        // std::cout<<"reserved 8 positions"<<std::endl;
        }else{
        // std::cout<<"reserved "<<capacity() * 2<<std::endl;
        reserve(capacity() * 2);
        }
    }

    void reserve(size_type n){
        pool.resize(n);
    }

    void set_element(T val, N idx){
        pool[idx]=val;
    }

    size_type capacity() const{
        return pool.size();
    }

    T& value(list_type x){
        return node(x).value;
    }

    const T& value(list_type x)const{
        return node(x).value;
    }

    list_type& next(list_type x){
        return node(x).next;
    }

    const list_type& next(list_type x) const{
        return node(x).next;
    }

    list_type end() const noexcept { return 0; }

    list_type new_list(){ return end(); }

    bool is_empty(list_type x) const{
      return x==end();
    }
    
    using iterator = _iterator<node_t,T,N>;
    using const_iterator = _iterator<node_t,T,N>;

    iterator begin(list_type x){
        return iterator(&pool,x);
    }
    iterator end(list_type ){ // this is not a typo
        return iterator(&pool,end());
    } 
    const_iterator begin(list_type x) const{
        return iterator(&pool,x);
    }
    const_iterator end(list_type ) const{
        return iterator(&pool,end());
    } 
    const_iterator cbegin(list_type x) const{
        return iterator(&pool,x);
    }
    const_iterator cend(list_type ) const{
        return iterator(&pool,end());
    }

    list_type free(list_type x) // delete first node
    {
        if(x==end())return x;

        auto tmp=next(x);
        next(x)=free_node_list;
        free_node_list=x;
        return tmp;
    }

    list_type free_list(list_type x){
        while(x!=end()){
        x=free(x);
        }
        return x;
    }
};

