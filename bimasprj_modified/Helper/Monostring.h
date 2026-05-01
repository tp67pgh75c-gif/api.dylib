#pragma once

template <typename T>
struct monoArray
{
    void* klass;
    void* monitor;
    void* bounds;
    int   max_length;
    void* vector [1];
    int getLength()
    {
        return max_length;
    }
    T getPointer()
    {
        return (T)vector;
    }
};

template <typename T>
struct monoList {
    void *unk0;
    void *unk1;
    monoArray<T> *items;
    int size;
    int version;

    T getItems(){
        return items->getPointer();
    }

    int getSize(){
        return size;
    }

    int getVersion(){
        return version;
    }
};

template <typename K, typename V>
struct Dictionary {
    void *unk0;
    void *unk1;
    monoArray<int **> *table;
    monoArray<void **> *linkSlots;
    monoArray<K> *keys;
    monoArray<V> *values;
    int touchedSlots;
    int emptySlot;
    int size;

    K getKeys(){
        return keys->getPointer();
    }

    V getValues(){
        return values->getPointer();
    }

    int getNumKeys(){
        return keys->getLength();
    }

    int getNumValues(){
        return values->getLength();
    }

    int getSize(){
        return size;
    }
};

// IL2CPP Dictionary<byte, T*> reader for modern Unity
// Layout: klass(8) monitor(8) buckets(8) entries(8) count(4) freeList(4) freeCount(4) version(4) comparer(8) keys(8) values(8)
// Entry<byte, ref>: hashCode(4) next(4) key(1) pad(7) value(8) = 24 bytes
struct IL2CppPlayerDict {
    void *dict;

    IL2CppPlayerDict(void *d) : dict(d) {}

    bool isValid() {
        if (!dict) return false;
        void *entries = *(void **)((char *)dict + 0x18);
        int cnt = *(int *)((char *)dict + 0x20);
        return entries != nullptr && cnt > 0;
    }

    int getCount() {
        if (!dict) return 0;
        return *(int *)((char *)dict + 0x20);
    }

    void *getPlayer(int index) {
        if (!dict) return nullptr;
        void *entriesArr = *(void **)((char *)dict + 0x18);
        if (!entriesArr) return nullptr;
        // monoArray data starts at offset 0x20 (klass+monitor+bounds+max_length)
        char *data = (char *)entriesArr + 0x20;
        // Entry size = 0x18 (24), value offset in entry = 0x10 (16)
        char *entry = data + (long)index * 0x18;
        int hashCode = *(int *)entry;
        if (hashCode < 0) return nullptr; // free slot
        return *(void **)(entry + 0x10);
    }
};
union intfloat {
    int i;
    float f;
};


typedef struct _monoString
{
    void* klass;
    void* monitor;
    int length;    
    char chars[1];   
    int getLength()
    {
      return length;
    }
    char* getChars()
    {
        return chars;
    }
    NSString* toNSString()
    {
      return [[NSString alloc] initWithBytes:(const void *)(chars)
                     length:(NSUInteger)(length * 2)
                     encoding:(NSStringEncoding)NSUTF16LittleEndianStringEncoding];
    }

    char* toCString()
    {
      NSString* v1 = toNSString();
      return (char*)([v1 UTF8String]);  
    }
    std::string toCPPString()
    {
      return std::string(toCString());
    }
}monoString;