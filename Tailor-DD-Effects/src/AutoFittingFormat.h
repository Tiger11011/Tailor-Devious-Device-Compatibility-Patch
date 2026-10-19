#pragma once
// Skyrim VMAD/PEX metadata only. Format matches the existing generator's
// build_cosmetics.py / pex_metadata.py; no script bytecode is executed.
#include <algorithm>
#include <cstdint>
#include <cctype>
#include <cstring>
#include <map>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

namespace AutoFitting::Format {
inline std::string Lower(std::string s) {
    for (auto& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}
class Reader {
    std::span<const std::uint8_t> data;
    std::size_t position = 0;
public:
    explicit Reader(std::span<const std::uint8_t> bytes) : data(bytes) {}
    void Skip(std::size_t n) {
        if (n > data.size() - position) throw std::runtime_error("truncated metadata");
        position += n;
    }
    std::uint32_t Number(std::size_t n, bool big = false) {
        if (!n || n > 4 || n > data.size() - position) throw std::runtime_error("invalid metadata integer");
        std::uint32_t value = 0;
        for (std::size_t i = 0; i < n; ++i) value |= std::uint32_t(data[position+i]) << ((big ? n-1-i : i)*8);
        position += n; return value;
    }
    std::string String(bool big = false) {
        auto n = Number(2,big);
        if (n > data.size() - position) throw std::runtime_error("truncated metadata string");
        std::string result(reinterpret_cast<const char*>(data.data()+position),n); position += n;
        return result;
    }
};
struct Script { std::string name; std::map<std::string,std::uint32_t> objects; };
inline std::uint32_t Value(Reader& r, std::uint32_t type) {
    switch(type) {
    case 0:return 0;
    case 1:r.Skip(4);return r.Number(4); // Skyrim object format 2: unused, alias, form
    case 2:r.String();return 0;
    case 3:case 4:r.Skip(4);return 0;
    case 5:r.Skip(1);return 0;
    default:
        if(type>=11 && type<=15) {
            auto n=r.Number(4);if(n>1000000)throw std::runtime_error("excessive VMAD array");
            for(std::uint32_t i=0;i<n;++i)Value(r,type-10);
            return 0;
        }
        throw std::runtime_error("unsupported VMAD value type");
    }
}
inline std::vector<Script> VMAD(std::span<const std::uint8_t> bytes) {
    Reader r(bytes);
    auto version=r.Number(2),objectFormat=r.Number(2),count=r.Number(2);
    if(version!=5 || objectFormat!=2 || count>256)throw std::runtime_error("unsupported Skyrim VMAD header");
    std::vector<Script> result;
    for(std::uint32_t i=0;i<count;++i) {
        Script script{Lower(r.String()),{}};r.Skip(1);auto properties=r.Number(2);
        for(std::uint32_t j=0;j<properties;++j) {
            auto key=Lower(r.String());auto type=r.Number(1);r.Skip(1);auto value=Value(r,type);
            // Match the established offline reader's last-property-wins rule;
            // real installed DD extensions contain repeated property entries.
            if(type==1)script.objects[key]=value;
        }
        result.push_back(std::move(script));
    }
    return result;
}
struct Parent { std::string name,parent; };
inline Parent PEX(std::span<const std::uint8_t> bytes) {
    Reader r(bytes);
    if(r.Number(4,true)!=0xfa57c0de)throw std::runtime_error("not a Skyrim PEX");
    auto major=r.Number(1),minor=r.Number(1),game=r.Number(2,true);
    if(major!=3 || (minor!=1 && minor!=2) || game!=1)throw std::runtime_error("unsupported PEX version/game");
    r.Skip(8);for(int i=0;i<3;++i)r.String(true);
    std::vector<std::string> strings;auto count=r.Number(2,true);
    for(std::uint32_t i=0;i<count;++i)strings.push_back(r.String(true));
    auto debug=r.Number(1);if(debug>1)throw std::runtime_error("invalid PEX debug flag");
    if(debug) {
        r.Skip(8);auto functions=r.Number(2,true);
        for(std::uint32_t i=0;i<functions;++i){r.Skip(7);r.Skip(r.Number(2,true)*2);}
    }
    r.Skip(r.Number(2,true)*3);
    if(r.Number(2,true)!=1)throw std::runtime_error("expected one Skyrim PEX object");
    auto name=r.Number(2,true),size=r.Number(4,true),parent=r.Number(2,true);
    if(name>=strings.size() || parent>=strings.size() || size<2)throw std::runtime_error("invalid PEX object metadata");
    return {Lower(strings[name]),Lower(strings[parent])};
}
inline constexpr std::uint32_t FirstSlot=0x800, SlotCount=65536, EndSlot=FirstSlot+SlotCount;
// Append-only ownership: removed/inactive clothes never surrender their slot.
// A load-order change or newly installed outfit cannot rename an existing form.
inline std::uint32_t Allocate(std::map<std::string,std::uint32_t>& ledger,const std::string& identity) {
    if(auto it=ledger.find(identity);it!=ledger.end())return it->second;
    std::uint32_t next=FirstSlot;
    for(const auto& [_,id]:ledger){if(id<FirstSlot || id>=EndSlot)throw std::runtime_error("invalid automatic fitting slot");next=std::max(next,id+1);}
    if(next>=EndSlot)throw std::runtime_error("automatic fitting slot capacity exceeded");
    ledger.emplace(identity,next);return next;
}
}
