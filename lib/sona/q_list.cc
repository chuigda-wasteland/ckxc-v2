#include "sona/q_list.hpp"
#include <memory>
#include <utility>

namespace sona {

q_list_impl::q_list_impl()
    : storage_begin(nullptr), storage_end(nullptr), storage_usage(nullptr) {}

q_list_impl::q_list_impl(std::size_t reserved_size) {
    storage_begin =
        reinterpret_cast<void**>(::operator new(reserved_size * sizeof(void*)));
    storage_end = storage_begin + reserved_size;
    storage_usage = storage_begin;
}

q_list_impl::~q_list_impl() {
    ::operator delete (storage_begin);
}

q_list_impl::size_type q_list_impl::size() const noexcept {
    return cend() - cbegin();
}

void q_list_impl::push_back(void *data) {
    if (storage_begin == nullptr) {
        *this = q_list_impl(1);
    }

    reallocate_when_need();

    *storage_usage = data;
    ++storage_usage;
}

void q_list_impl::pop_back() {
    --storage_usage;
}

q_list_impl::reference q_list_impl::front() { return *begin(); }

q_list_impl::reference q_list_impl::back() { return *(end()-1); }

q_list_impl::iterator q_list_impl::begin() noexcept { return storage_begin; }

q_list_impl::iterator q_list_impl::end() noexcept { return storage_usage; }

q_list_impl::const_iterator q_list_impl::begin() const noexcept {
    return storage_begin;
}

q_list_impl::const_iterator q_list_impl::end() const noexcept {
    return storage_usage;
}

q_list_impl::const_iterator q_list_impl::cbegin() const noexcept {
    return storage_begin;
}

q_list_impl::const_iterator q_list_impl::cend() const noexcept {
    return storage_usage;
}

void q_list_impl::swap(q_list_impl &that) noexcept {
    std::swap(storage_begin, that.storage_begin);
    std::swap(storage_end, that.storage_end);
    std::swap(storage_usage, that.storage_usage);
}

void q_list_impl::reallocate_when_need() {
    if (storage_end == storage_usage) {
        void **new_storage_begin =
           reinterpret_cast<void**>(::operator new(size() * 2 * sizeof(void*)));
        std::copy(begin(), end(), new_storage_begin);

        size_type cached_size = size();
        ::operator delete (storage_begin);

        storage_begin = new_storage_begin;
        storage_end = new_storage_begin + cached_size*2;
        storage_usage = new_storage_begin + cached_size;
    }
}

q_list_impl& q_list_impl::operator=(q_list_impl &&that) {
    (*this).swap(that);
    return *this;
}

} // namespace sona
