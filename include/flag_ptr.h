#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <utility>
#include <type_traits>

template <typename PtrType, typename FlagsType, bool UniquePtr>
class flag_ptr {
public:
    explicit flag_ptr(PtrType* ptr) : m_ptr(ptr) {}

    template <typename...Args>
    explicit flag_ptr(Args&&...args) : m_ptr(new PtrType(std::forward<Args>(args)...)) {}

    flag_ptr() : m_ptr(nullptr) {}

    flag_ptr(const flag_ptr<PtrType, FlagsType, UniquePtr>& f) = delete;

    flag_ptr(flag_ptr<PtrType, FlagsType, UniquePtr>&& f) noexcept {
        m_ptr = f.m_ptr;
        f.m_ptr = nullptr;
    }

    flag_ptr<PtrType, FlagsType, UniquePtr>& operator=( const flag_ptr<PtrType, FlagsType, UniquePtr>& f) = delete;

    flag_ptr<PtrType, FlagsType, UniquePtr>& operator=( flag_ptr<PtrType, FlagsType, UniquePtr>&& f) noexcept {
        m_ptr = f.m_ptr;
        f.m_ptr = nullptr;
        return *this;
    }

    ~flag_ptr() { delete_ptr(); }

    void set_flags(FlagsType flags) {
        const std::uintptr_t fl = *(std::uintptr_t*)&flags;
        m_flags = fl;
    }

    template <FlagsType flags>
    void set_flags() {
        const std::uintptr_t fl = *(std::uintptr_t*)&flags;
        m_flags = fl;
    }

    FlagsType get_flags() const {
        const std::uintptr_t fl = (std::uintptr_t)m_flags;
        return *(FlagsType*)(&fl);
    }

    const PtrType* operator->() const { return (PtrType*)get_raw_ptr(); }
    PtrType* operator->()  { return (PtrType*)get_raw_ptr(); }

    const PtrType& operator*() const { return *(PtrType*)get_raw_ptr(); }
    PtrType& operator*() { return *(PtrType*)get_raw_ptr(); }

    PtrType* get() { return (PtrType*)get_raw_ptr(); }
    const PtrType* get() const { return (PtrType*)get_raw_ptr(); }

    operator bool() const {
        return get_raw_ptr() ? true : false;
    }

    void reset_only_ptr() { reset_only_ptr(nullptr); }

    void reset_only_ptr(PtrType* ptr) {
        delete_ptr();
        const auto prev_flags = get_flags();
        m_ptr = ptr;
        set_flags(prev_flags);
    }

    void reset() { reset(nullptr); }

    void reset(PtrType* ptr) {
        delete_ptr();
        m_ptr = ptr;
    }

private:
    static constexpr std::size_t log2(size_t n) {
        return ((n < 2) ? 0 : 1 + log2(n / 2));
    }

    template <typename Type>
    static constexpr size_t alignment_of_in_bits() {
        return log2(std::alignment_of<Type>::value);
    }

    static constexpr std::size_t BitfieldSize = alignment_of_in_bits<PtrType*>();
    static constexpr std::uintptr_t PtrMask = ~((0x1 << alignment_of_in_bits<PtrType*>()) - 1);

    union {
        PtrType* m_ptr;
        std::uintptr_t m_flags : BitfieldSize;
    };

    PtrType* get_raw_ptr() const {
        return (PtrType*)(((std::uintptr_t)m_ptr) & PtrMask);
    }

    void delete_ptr() {
        if constexpr(!UniquePtr)
            return;

        auto ptr = get_raw_ptr();

        if (ptr)
            delete ptr;
    }
};

template <typename T, typename FlagsType, bool UniquePtr, typename...Args>
auto make_flag_ptr(Args&&...args) {
   return flag_ptr<T, FlagsType, UniquePtr>(args...);
}
