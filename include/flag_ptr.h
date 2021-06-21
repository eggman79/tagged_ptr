#include <cstdlib>
#include <cstdint>
#include <bit>
#include <iostream>
#include <cassert>
#include <utility>
#include <numeric>
#include <tuple>
#include <type_traits>

template <typename Type, size_t Size>
struct flag {
    using type = Type;
    static constexpr auto size = Size;
};

template <typename...Flags>
struct flags {
    using type = std::tuple<Flags...>;
};

template <std::size_t Index, size_t ToIndex, typename Tuple>
constexpr size_t eval_flag_offset() {
    using Flag = typename std::tuple_element<Index, Tuple>::type;
    if constexpr (Index == ToIndex)
        return 0;
    else {
        return eval_flag_offset<Index + 1, ToIndex, Tuple>() + Flag::size;
    }
}

template <typename Tuple>
constexpr size_t get_flags_size() {
    constexpr auto Count = std::tuple_size<Tuple>::value;

    if constexpr (!Count)
        return 0;
    else {
        using Flag = typename std::tuple_element<Count - 1, Tuple>::type;
        return eval_flag_offset<0, Count - 1, Tuple>() + Flag::size;
    }
}

template <typename PtrType, typename Flags, bool AutoDestruct=true>
class flag_ptr {
private:
    using FlagPtrType = flag_ptr<PtrType, Flags, AutoDestruct>;

    static constexpr std::size_t log2(size_t n) {
        return ((n < 2) ? 0 : 1 + log2(n / 2));
    }

    template <typename Type>
    static constexpr size_t alignment_of_in_bits() {
        return log2(std::alignment_of<Type>::value);
    }

    static constexpr std::size_t BitfieldSize = alignment_of_in_bits<PtrType*>();
    static constexpr std::uintptr_t FlagMask = ((0x1 << alignment_of_in_bits<PtrType*>()) - 1);
    static constexpr std::uintptr_t PtrMask = ~FlagMask;

public:
    template <typename...Args>
    explicit flag_ptr(Args&&...args) : m_ptr(new PtrType(std::forward<Args>(args)...)) {}

    explicit flag_ptr(PtrType* ptr) noexcept : m_ptr(ptr) {}

    flag_ptr() noexcept : m_ptr(nullptr) {}

    flag_ptr(const FlagPtrType& f) = delete;

    flag_ptr(FlagPtrType&& f) noexcept {
        m_ptr = f.m_ptr;
        f.m_ptr = nullptr;
    }

    auto& operator=(const FlagPtrType& f) = delete;

    auto& operator=(FlagPtrType&& f) noexcept {
        m_ptr = f.m_ptr;
        f.m_ptr = nullptr;
        return *this;
    }

    ~flag_ptr() noexcept { delete_ptr(); }

    template <size_t Index>
    static constexpr size_t get_flag_offset() noexcept {
        return eval_flag_offset<0, Index, typename Flags::type>();
    }

    template <size_t Index>
    static constexpr size_t get_flag_size() noexcept {
        using Flag = typename std::tuple_element<Index, typename Flags::type>::type;
        return Flag::size;
    }

    static constexpr size_t get_flags_size() noexcept {
        constexpr auto count = std::tuple_size<typename Flags::type>::value;
        return get_flag_offset<count - 1>() + get_flag_size<count - 1>();
    }

    static_assert(FlagPtrType::get_flags_size() <= FlagPtrType::BitfieldSize, "flags size is too big");

    template <size_t Index, typename Type>
    void set_flag(Type value) noexcept {
        using Flag = typename std::tuple_element<
            Index,
            typename Flags::type>::type;

        const auto val = *(uint8_t*)&value;
        constexpr auto size = Flag::size;
        constexpr auto offset = get_flag_offset<Index>();
        constexpr auto value_mask = ((1 << size) - 1) << offset;
        constexpr auto inverted_value_mask = ~value_mask;
        m_ptr = (PtrType*)((((uintptr_t)m_ptr) & inverted_value_mask) | ((val << offset) & value_mask));
    }

    template <size_t Index>
    auto get_flag() const noexcept {
        using Flag = typename std::tuple_element<Index, typename Flags::type>::type;
        constexpr auto size = Flag::size;
        constexpr auto offset = get_flag_offset<Index>();
        constexpr auto value_mask = ((0x1 << size) - 1);
        const auto value = (((uintptr_t)m_ptr) >> offset) & value_mask;
        return *(typename Flag::type*)(&value);
    }

    const PtrType* operator->() const noexcept {
        return (PtrType*)get_raw_ptr();
    }

    PtrType* operator->() noexcept {
        return (PtrType*)get_raw_ptr();
    }

    const PtrType& operator*() const noexcept {
        return *(PtrType*)get_raw_ptr();
    }

    PtrType& operator*() noexcept {
        return *(PtrType*)get_raw_ptr();
    }

    void delete_ptr() noexcept {
        if constexpr(AutoDestruct)
            delete get_raw_ptr();
    }
    PtrType* get() noexcept {
        return (PtrType*)get_raw_ptr();
    }

    const PtrType* get() const noexcept {
        return (PtrType*)get_raw_ptr();
    }

    operator bool() const noexcept {
        return get_raw_ptr() ? true : false;
    }

    void reset_only_ptr() noexcept {
        reset_only_ptr(nullptr);
    }

    void reset_only_ptr(PtrType* ptr) noexcept {
        constexpr auto flags_size = get_flags_size();
        const auto flags = (uintptr_t)m_ptr & ((0x1 << flags_size) - 1);
        delete_ptr();
        m_ptr = (PtrType*)((uintptr_t)ptr | flags);
    }

    void reset() noexcept {
        reset(nullptr);
    }

    void reset(PtrType* ptr) noexcept {
        delete_ptr();
        m_ptr = ptr;
    }
private:
      union {
        PtrType* m_ptr;
        std::uintptr_t m_flags : BitfieldSize;
    };

    PtrType* get_raw_ptr() const noexcept {
        return (PtrType*)(((std::uintptr_t)m_ptr) & PtrMask);
    }
};

template <typename T, typename FlagsType, bool AutoDestruct=true, typename...Args>
auto make_flag_ptr(Args&&...args) {
   return flag_ptr<T, FlagsType, AutoDestruct>(std::forward<Args>(args)...);
}
