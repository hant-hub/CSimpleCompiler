#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stringstore.h>


static u32 CmpKey(StringStore* h, u64 idx, char* key, u64 length) {
    u32 len = 0;
    char* buf = &h->textbuffer[h->keys[idx]];
    while (key[0] && buf[0] && key[0] == buf[0]) {
        key++;
        buf++;
        len++;
    }
    return len == length;
}


static u64 PushText(StringStore* h, char* text, u64 length) {
    while (h->tsize + length + 1 > h->tcap) {
        u64 oldcap = h->tcap;
        h->tcap = h->tcap ? h->tcap * 2 : 4;
        h->textbuffer = h->m.a(oldcap, h->tcap, h->textbuffer, h->m.ctx);
    }

    u64 idx = h->tsize;
    memcpy(&h->textbuffer[h->tsize], text, length);
    h->tsize += length;
    h->textbuffer[h->tsize++] = 0;

    return idx;
}

static void Resize(StringStore* h) {
    printf("Resize!\n");
    u64* oldkeys = h->keys;
    u64 oldsize = h->kcap;

    while ((f32)(h->ksize + 1)/(f32)h->kcap > 0.6) 
        h->kcap = h->kcap ? h->kcap * 2: 1;
    //printf("Resize! %ld (%ld -> %ld)\n", h->ksize, oldsize, h->kcap);

    //printf("Resize: %ld %ld\n", oldsize, h->kcap);
    h->keys = h->m.a(oldsize * sizeof(u64), h->kcap * sizeof(u64), NULL, h->m.ctx);
    memset(h->keys, -1, h->kcap * sizeof(u64));


    for (u64 i = 0; i < oldsize; i++) {
        if (oldkeys[i] == -1) continue;

        char* key = &h->textbuffer[oldkeys[i]];

        u64 idx = hash(key, strlen(key))  % h->kcap;
        u64 k = oldkeys[i];

        for (u32 i = 0; i < h->kcap; i++) {
            if (h->keys[idx] == -1) {
                h->keys[idx] = k;
                break;
            }
            idx = (idx + 1) % h->kcap;
        }
    }

    h->m.a(oldsize * sizeof(u64), 0, oldkeys, h->m.ctx);
}

i64 AddString(StringStore* h, char* key, u64 length) {
    //resize if necessary
    //printf("Insert %.*s\n", (int)length, key);
    //printf("Check: %f, %f\n", (f32)(h->ksize + 1), (f32)h->kcap); 
    while ((f32)(h->ksize + 1)/(f32)h->kcap > 0.6) {
        Resize(h);
    }

    //traverse and find slot
    i64 idx = hash(key, length) % h->kcap;

    for (u32 i = 0; i < h->kcap; i++) {
        if (h->keys[idx] == -1) {
            u64 k = PushText(h, key, length);
            h->keys[idx] = k;
            h->ksize++;
            break;
        }

        if (CmpKey(h, idx, key, length)) {
            //h->values[idx] = e;
            break;
        }

        idx = (idx + 1) % h->kcap;
    }

    return h->keys[idx]; 
}


char* GetString(StringStore* h, int key) {
    return &h->textbuffer[key];
}

i64 GetKey(StringStore* h, char* key, u64 len) {
    i64 idx = hash(key, len) % h->kcap;

    for (u32 i = 0; i < h->kcap; i++) {
        if (h->keys[idx] == -1) {
            return -1;
            break;
        }

        if (CmpKey(h, idx, key, len)) {
            return h->keys[idx];
            break;
        }

        idx = (idx + 1) % h->kcap;
    }

    return -1; 
}
