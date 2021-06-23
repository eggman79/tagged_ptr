#pragma once

#include <cstdint>
#include <utility>
#include <tuple>
#include <type_traits>

namespace eggman79 {

template <typename Type, std::size_t flag_size>
struct flag {
    using type = Type;
    static constexpr auto size = flag_size;
};

template <typename...Flags>
struct flags {
    using type = std::tuple<Flags...>;
};

template <typename PtrType, typename Flags, bool auto_destruct = true>
class flag_ptr {
private:
    using flag_ptr_type = flag_ptr<PtrType, Flags, auto_destruct>;

    static constexpr std::size_t log2(std::size_t n) noexcept {
        return ((n < 2) ? 0 : 1 + log2(n / 2));
    }

    template <typename Type>
    static constexpr std::size_t alignment_of_in_bits() noexcept {
        return log2(std::alignment_of<Type>::value);
    }

    static constexpr auto bits_alignment = alignment_of_in_bits<PtrType*>();
    static constexpr std::size_t max_bitfield_size = bits_alignment;
    static constexpr std::uintptr_t ptr_mask = ~((0x1 << bits_alignment) - 1);

    template <std::size_t index, std::size_t toindex, typename Tuple>
    static constexpr std::size_t eval_flag_offset() noexcept {
        using flag_type = typename std::tuple_element<index, Tuple>::type;
        if constexpr (index == toindex) {
            return 0;
        } else {
            return eval_flag_offset<index + 1, toindex, Tuple>() + flag_type::size;
        }
    }

    template  <std::size_t size_in_bits>
    struct bitsize_to_int_type {
        static constexpr auto bytes = size_in_bits / 8 + (size_in_bits % 8 ? 1 : 0);

        static_assert(size_in_bits <= sizeof(std::uintptr_t) * 8, "the size is too large");
        static_assert(bytes <= 8, "you need to add type uint128_t ");

        using type = typename std::conditional<
            bytes == 1,
            std::uint8_t,
            std::conditional<
                bytes == 2,
                std::uint16_t,
                std::conditional<
                    bytes == 4,
                    std::uint32_t,
                    std::uint64_t>>>::type;
    };

    template <std::size_t index>
    static constexpr std::size_t get_flag_offset() noexcept {
        return eval_flag_offset<0, index, typename Flags::type>();
    }

    template <std::size_t index>
    static constexpr std::size_t get_flag_size() noexcept {
        using flag_type = typename std::tuple_element<
            index,
            typename Flags::type>::type;
        return flag_type::size;
    }

    static constexpr std::size_t get_flags_size() noexcept {
        constexpr auto count = std::tuple_size<typename Flags::type>::value;
        return get_flag_offset<count - 1>() + get_flag_size<count - 1>();
    }

    static_assert(
        flag_ptr_type::get_flags_size() <= flag_ptr_type::max_bitfield_size,
        "the size of the flags is too large");

public:
    explicit flag_ptr(PtrType* ptr) noexcept : m_ptr(ptr) {}
    explicit flag_ptr(const flag_ptr_type& f) = delete;
    flag_ptr() noexcept : m_ptr(nullptr) {}

    explicit flag_ptr(flag_ptr_type&& f) noexcept {
        m_ptr = f.m_ptr;
        f.m_ptr = nullptr;
    }

    auto& operator=(const flag_ptr_type& f) = delete;

    auto& operator=(flag_ptr_type&& f) noexcept {
        m_ptr = f.m_ptr;
        f.m_ptr = nullptr;
        return *this;
    }

    ~flag_ptr() noexcept { delete_ptr(); }

    template <std::size_t index, typename Type>
    void set_flag(Type value) noexcept {
        using flag_type = typename std::tuple_element<
            index,
            typename Flags::type>::type;

        using int_type = typename bitsize_to_int_type<
            flag_ptr_type::get_flags_size()>::type;

        const auto val = *reinterpret_cast<int_type*>(&value);
        constexpr auto size = flag_type::size;
        constexpr auto offset = get_flag_offset<index>();
        constexpr auto value_mask = get_value_with_offset_mask(size, offset);
        constexpr auto inverted_value_mask = ~value_mask;
        const auto clean_flags = ((val << offset) & value_mask);
        const auto clean_ptr = (((std::uintptr_t)m_ptr) & inverted_value_mask);
        m_ptr = reinterpret_cast<PtrType*>(clean_ptr | clean_flags);
    }

    template <std::size_t index>
    auto get_flag() const noexcept {
        using flag_type = typename std::tuple_element<
            index, typename Flags::type>::type;

        constexpr auto size = flag_type::size;
        constexpr auto offset = get_flag_offset<index>();
        constexpr auto value_mask = get_value_mask(size);
        const auto value = (((std::uintptr_t)m_ptr) >> offset) & value_mask;
        return *(typename flag_type::type*)(&value);
    }

    const PtrType* operator->() const noexcept {
        return reinterpret_cast<PtrType*>(get_raw_ptr());
    }

    PtrType* operator->() noexcept {
        return reinterpret_cast<PtrType*>(get_raw_ptr());
    }

    const PtrType& operator*() const noexcept {
        return *reinterpret_cast<PtrType*>(get_raw_ptr());
    }

    PtrType& operator*() noexcept {
        return *reinterpret_cast<PtrType*>(get_raw_ptr());
    }

    void delete_ptr() noexcept {
        if constexpr(auto_destruct)
            delete get_raw_ptr();
    }
    PtrType* get() noexcept {
        return reinterpret_cast<PtrType*>(get_raw_ptr());
    }

    const PtrType* get() const noexcept {
        return reinterpret_cast<PtrType*>(get_raw_ptr());
    }

    operator bool() const noexcept {
        return get_raw_ptr() ? true : false;
    }

    void reset_only_ptr() noexcept {
        reset_only_ptr(nullptr);
    }

    void reset_only_ptr(PtrType* ptr) noexcept {
        constexpr auto flags_size = get_flags_size();
        const auto flags = (std::uintptr_t)m_ptr & get_value_mask(flags_size);
        delete_ptr();
        m_ptr = reinterpret_cast<PtrType*>((std::uintptr_t)ptr | flags);
    }

    void reset() noexcept {
        reset(nullptr);
    }

    void reset(PtrType* ptr) noexcept {
        delete_ptr();
        m_ptr = ptr;
    }
private:
    PtrType* m_ptr;

    PtrType* get_raw_ptr() const noexcept {
        return reinterpret_cast<PtrType*>(((std::uintptr_t)m_ptr) & ptr_mask);
    }

    static inline constexpr auto get_value_mask(std::size_t size) noexcept {
        return (1 << size) - 1;
    }

    static inline constexpr auto get_value_with_offset_mask(
            std::size_t size,
            std::size_t offset) noexcept {
        return get_value_mask(size) << offset;
    }
};

template <
    typename PtrType,
    typename FlagsType,
    bool auto_destruct = true,
    typename...Args>
static inline auto make_flag_ptr(Args&&...args) {
    return flag_ptr<PtrType, FlagsType, auto_destruct>(
           new PtrType(std::forward<Args>(args)...));
}

}  // namespace eggman79
