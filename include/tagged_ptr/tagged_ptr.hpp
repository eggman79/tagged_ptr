// Copyright(c) 2021, eggman79
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <cstdint>
#include <utility>
#include <tuple>
#include <type_traits>

namespace eggman79 {

template <typename Type, std::size_t tag_size>
struct tag {
    using type = Type;
    static constexpr auto size = tag_size;
};

template <typename PtrType, bool UniquePtr, typename...Tags>
class tagged_ptr {
private:
    using tagged_ptr_type = tagged_ptr<PtrType, UniquePtr, Tags...>;
    using tuple_flags = std::tuple<Tags...>;

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

    template <std::size_t index, std::size_t to_index, typename Tuple>
    static constexpr std::size_t eval_tag_offset() noexcept {
        using tag_type = typename std::tuple_element<index, Tuple>::type;
        if constexpr (index == to_index) {
            return 0;
        } else {
            return eval_tag_offset<index + 1, to_index, Tuple>() + tag_type::size;
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
    static constexpr std::size_t get_tag_offset() noexcept {
        return eval_tag_offset<0, index, tuple_flags>();
    }

    template <std::size_t index>
    static constexpr std::size_t get_tag_size() noexcept {
        using tag_type = typename std::tuple_element<
            index,
            tuple_flags>::type;
        return tag_type::size;
    }

    static constexpr std::size_t get_tags_size() noexcept {
        constexpr auto count = std::tuple_size<tuple_flags>::value;
        return get_tag_offset<count - 1>() + get_tag_size<count - 1>();
    }

    //static_assert(
      //  tagged_ptr_type::get_tags_size() * 2 <= tagged_ptr_type::max_bitfield_size,
        //"the size of the tags is too large");

public:
    explicit tagged_ptr(PtrType* ptr) noexcept : m_ptr(ptr) {}
    explicit tagged_ptr(const tagged_ptr_type& f) = delete;
    tagged_ptr() noexcept : m_ptr(nullptr) {}

    explicit tagged_ptr(tagged_ptr_type&& f) noexcept {
        m_ptr = f.m_ptr;
        f.m_ptr = nullptr;
    }

    auto& operator=(const tagged_ptr_type& f) = delete;

    auto& operator=(tagged_ptr_type&& f) noexcept {
        m_ptr = f.m_ptr;
        f.m_ptr = nullptr;
        return *this;
    }

    ~tagged_ptr() noexcept { delete_ptr(); }

    template <std::size_t index, typename Type>
    void set_tag(Type value) noexcept {
        using tag_type = typename std::tuple_element<
            index,
            tuple_flags>::type;

        using int_type = typename bitsize_to_int_type<
            tagged_ptr_type::get_tags_size()>::type;

        const auto val = *reinterpret_cast<int_type*>(&value);
        constexpr auto size = tag_type::size;
        constexpr auto offset = get_tag_offset<index>();
        constexpr auto value_mask = get_value_with_offset_mask(size, offset);
        constexpr auto inverted_value_mask = ~value_mask;
        const auto clean_tags = ((val << offset) & value_mask);
        const auto clean_ptr = (((std::uintptr_t)m_ptr) & inverted_value_mask);
        m_ptr = reinterpret_cast<PtrType*>(clean_ptr | clean_tags);
    }

    template <std::size_t index>
    auto get_tag() const noexcept {
        using tag_type = typename std::tuple_element<
            index, tuple_flags>::type;

        constexpr auto size = tag_type::size;
        constexpr auto offset = get_tag_offset<index>();
        constexpr auto value_mask = get_value_mask(size);
        const auto value = (((std::uintptr_t)m_ptr) >> offset) & value_mask;
        return *(typename tag_type::type*)(&value);
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
        constexpr auto tags_size = get_tags_size();
        const auto tags = (std::uintptr_t)m_ptr & get_value_mask(tags_size);
        delete_ptr();
        m_ptr = reinterpret_cast<PtrType*>((std::uintptr_t)ptr | tags);
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
    bool UniquePtr,
    typename...FlagsType,
    typename...Args>
static inline auto make_tagged_ptr(Args&&...args) {
    return tagged_ptr<PtrType, UniquePtr, FlagsType...>(
        new PtrType(std::forward<Args>(args)...));
}

}  // namespace eggman79
