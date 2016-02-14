/* Copyright (c) 2016, Kulshan Concepts
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. Neither the name of the copyright holder nor the names of its
 *      contributors may be used to endorse or promote products derived from
 *      this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "memory.h"

template <typename T>
class List {
public:
    virtual uint32_t size() const = 0;
    virtual const T& get(uint32_t index) const = 0;
    virtual uint32_t add(const T& item) = 0;
    virtual void remove(uint32_t index) = 0;
};

template <typename T>
struct LinkedListItem {
    T item;
    LinkedListItem<T>* next;
};

template <typename T>
class LinkedList : public List<T> {
private:
    LinkedListItem<T>* items;
    uint32_t count;

public:
    LinkedList() : items(nullptr), count(0) {}

    virtual uint32_t size() const { return count; }

    virtual const T& get(uint32_t index) const {
        LinkedListItem<T>* item = items;

        while (item != nullptr && index > 0) {
            item = item->next;
        }

        if (item == nullptr) {
            return *((T*)nullptr);
        } else {
            return item->item;
        }
    }

    virtual uint32_t add(const T& thing) {
        count++;

        if (items == nullptr) {
            items = new LinkedListItem<T>();
            items->next = nullptr;
            items->item = thing;

            return 0;
        } else {
            uint32_t index = 1;
            LinkedListItem<T>* item = items;

            while (item->next != nullptr) {
                item = item->next;
                index++;
            }

            item->next = new LinkedListItem<T>();
            item->next->next = nullptr;
            item->next->item = thing;

            return index;
        }
    }

    virtual void remove(uint32_t index) {
        if (index >= count) {
            return;
        }

        LinkedListItem<T>* previous = nullptr;
        LinkedListItem<T>* item = items;
        while (index > 0) {
            previous = item;
            item = item->next;
        }

        if (previous != nullptr) {
            previous->next = item->next;
        }

        delete item;

        count--;
    }
};
