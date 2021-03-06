/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 *     Copyright 2012 Couchbase, Inc.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

#include <libcbio/cbio.h>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <fstream>
#include <gtest/gtest.h>

using namespace std;

static const char dbfile[] = "testcase.couch";

class LibcbioTest : public ::testing::Test
{
protected:
    LibcbioTest() {}
    virtual ~LibcbioTest() {}
    virtual void SetUp(void) {
        removeDb();
    }
    virtual void TearDown(void) {
        removeDb();
    }

    void createRubbishFile(void) {
        ofstream out(dbfile);
        for (int ii = 0; ii < 80; ++ii) {
            out << "Rubbish file" << endl;
        }
    }

protected:
    void removeDb(void) {
        EXPECT_EQ(0, (remove(dbfile) == -1 && errno != ENOENT));
    }
};

class LibcbioOpenTest : public LibcbioTest {};

TEST_F(LibcbioOpenTest, HandleEmptyNameOpenRDONLY)
{
    libcbio_t handle;
    EXPECT_EQ(CBIO_ERROR_ENOENT,
              cbio_open_handle("", CBIO_OPEN_RDONLY, &handle));
}

TEST_F(LibcbioOpenTest, HandleNullNameOpenRDONLY)
{
    libcbio_t handle;
    EXPECT_EQ(CBIO_ERROR_EINVAL,
              cbio_open_handle(NULL, CBIO_OPEN_RDONLY, &handle));
}

TEST_F(LibcbioOpenTest, HandleNonexistentNameOpenRDONLY)
{
    libcbio_t handle;
    EXPECT_EQ(CBIO_ERROR_ENOENT,
              cbio_open_handle("/this/path/should/not/exist",
                               CBIO_OPEN_RDONLY, &handle));
}

TEST_F(LibcbioOpenTest, HandleEmptyNameOpenRW)
{
    libcbio_t handle;
    EXPECT_EQ(CBIO_ERROR_ENOENT,
              cbio_open_handle("", CBIO_OPEN_RW, &handle));
}

TEST_F(LibcbioOpenTest, HandleNullNameOpenRW)
{
    libcbio_t handle;
    EXPECT_EQ(CBIO_ERROR_EINVAL,
              cbio_open_handle(NULL, CBIO_OPEN_RW, &handle));
}

TEST_F(LibcbioOpenTest, HandleNonexistentPathNameOpenRW)
{
    libcbio_t handle;
    EXPECT_EQ(CBIO_ERROR_ENOENT,
              cbio_open_handle("/this/path/should/not/exist",
                               CBIO_OPEN_RW, &handle));
}

TEST_F(LibcbioOpenTest, HandleNonexistentNameOpenRW)
{
    libcbio_t handle;
    int error = remove("missing-db");
    if (error == -1) {
        EXPECT_EQ(ENOENT, errno);
    }
    EXPECT_EQ(CBIO_ERROR_ENOENT,
              cbio_open_handle("missing-db",
                               CBIO_OPEN_RW, &handle));
}

TEST_F(LibcbioOpenTest, HandleEmptyNameOpenCREATE)
{
    libcbio_t handle;
    EXPECT_EQ(CBIO_ERROR_ENOENT,
              cbio_open_handle("", CBIO_OPEN_CREATE, &handle));
}

TEST_F(LibcbioOpenTest, HandleNullNameOpenCREATE)
{
    libcbio_t handle;
    EXPECT_EQ(CBIO_ERROR_EINVAL,
              cbio_open_handle(NULL, CBIO_OPEN_CREATE, &handle));
}

TEST_F(LibcbioOpenTest, HandleNonexistentNameOpenCREATE)
{
    libcbio_t handle;
    EXPECT_EQ(CBIO_ERROR_ENOENT,
              cbio_open_handle("/this/path/should/not/exist",
                               CBIO_OPEN_CREATE, &handle));
}

TEST_F(LibcbioOpenTest, HandleIllegalFileRDONLY)
{
    createRubbishFile();
    libcbio_t handle;
    EXPECT_EQ(CBIO_ERROR_NO_HEADER,
              cbio_open_handle(dbfile, CBIO_OPEN_RDONLY, &handle));
}

TEST_F(LibcbioOpenTest, HandleIllegalFileRW)
{
    createRubbishFile();
    libcbio_t handle;
    EXPECT_EQ(CBIO_ERROR_NO_HEADER,
              cbio_open_handle(dbfile, CBIO_OPEN_RW, &handle));
}

TEST_F(LibcbioOpenTest, HandleIllegalFileCREATE)
{
    createRubbishFile();
    libcbio_t handle;
    EXPECT_EQ(CBIO_ERROR_NO_HEADER,
              cbio_open_handle(dbfile, CBIO_OPEN_CREATE, &handle));
}



class LibcbioCreateDatabaseTest : public LibcbioTest {};


TEST_F(LibcbioCreateDatabaseTest, createDatabase)
{
    libcbio_t handle;
    EXPECT_EQ(CBIO_SUCCESS,
              cbio_open_handle(dbfile, CBIO_OPEN_CREATE, &handle));

    cbio_close_handle(handle);
}

TEST_F(LibcbioCreateDatabaseTest, reopenDatabaseReadOnly)
{
    libcbio_t handle;
    EXPECT_EQ(CBIO_SUCCESS,
              cbio_open_handle(dbfile, CBIO_OPEN_CREATE, &handle));
    cbio_close_handle(handle);

    EXPECT_EQ(CBIO_SUCCESS,
              cbio_open_handle(dbfile, CBIO_OPEN_RDONLY, &handle));

    cbio_close_handle(handle);
}

TEST_F(LibcbioCreateDatabaseTest, reopenDatabaseReadWrite)
{
    libcbio_t handle;
    EXPECT_EQ(CBIO_SUCCESS,
              cbio_open_handle(dbfile, CBIO_OPEN_CREATE, &handle));
    cbio_close_handle(handle);

    EXPECT_EQ(CBIO_SUCCESS,
              cbio_open_handle(dbfile, CBIO_OPEN_RW, &handle));

    cbio_close_handle(handle);
}

TEST_F(LibcbioCreateDatabaseTest, reopenDatabaseCreate)
{
    libcbio_t handle;
    EXPECT_EQ(CBIO_SUCCESS,
              cbio_open_handle(dbfile, CBIO_OPEN_CREATE, &handle));
    cbio_close_handle(handle);

    EXPECT_EQ(CBIO_SUCCESS,
              cbio_open_handle(dbfile, CBIO_OPEN_CREATE, &handle));

    cbio_close_handle(handle);
}

class LibcbioIllegalReadonlyOpsTest : public LibcbioTest {};
TEST_F(LibcbioIllegalReadonlyOpsTest, commit)
{
    libcbio_t handle;
    EXPECT_EQ(CBIO_SUCCESS,
              cbio_open_handle(dbfile, CBIO_OPEN_CREATE, &handle));
    cbio_close_handle(handle);

    EXPECT_EQ(CBIO_SUCCESS,
              cbio_open_handle(dbfile, CBIO_OPEN_RDONLY, &handle));

    EXPECT_EQ(CBIO_ERROR_EINVAL,
              cbio_commit(handle));

    cbio_close_handle(handle);
}

TEST_F(LibcbioIllegalReadonlyOpsTest, store)
{
    libcbio_t handle;
    EXPECT_EQ(CBIO_SUCCESS,
              cbio_open_handle(dbfile, CBIO_OPEN_CREATE, &handle));
    cbio_close_handle(handle);

    EXPECT_EQ(CBIO_SUCCESS,
              cbio_open_handle(dbfile, CBIO_OPEN_RDONLY, &handle));

    libcbio_document_t doc;
    string key("hello");
    string value("world");
    EXPECT_EQ(CBIO_SUCCESS,
              cbio_create_empty_document(handle, &doc));
    EXPECT_EQ(CBIO_SUCCESS,
              cbio_document_set_id(doc, key.data(), key.length(), 0));
    EXPECT_EQ(CBIO_SUCCESS,
              cbio_document_set_value(doc, value.data(),
                                      value.length(), 0));
    EXPECT_EQ(CBIO_ERROR_EINVAL,
              cbio_store_document(handle, doc));
    cbio_document_release(doc);

    cbio_close_handle(handle);
}

class LibcbioDataAccessTest : public LibcbioTest
{
public:
    LibcbioDataAccessTest() {
        blob = new char[8192];
        blobsize = 8192;
    }

    virtual ~LibcbioDataAccessTest() {
        delete []blob;
    }

    virtual void SetUp(void) {
        removeDb();
        ASSERT_EQ(CBIO_SUCCESS,
                  cbio_open_handle(dbfile, CBIO_OPEN_CREATE, &handle));
    }
    virtual void TearDown(void) {
        cbio_close_handle(handle);
        removeDb();
    }

protected:

    void storeSingleDocument(const string &key, const string &value) {
        libcbio_document_t doc;
        EXPECT_EQ(CBIO_SUCCESS,
                  cbio_create_empty_document(handle, &doc));
        EXPECT_EQ(CBIO_SUCCESS,
                  cbio_document_set_id(doc, key.data(), key.length(), 0));
        EXPECT_EQ(CBIO_SUCCESS,
                  cbio_document_set_value(doc, value.data(),
                                          value.length(), 0));
        EXPECT_EQ(CBIO_SUCCESS,
                  cbio_store_document(handle, doc));
        EXPECT_EQ(CBIO_SUCCESS,
                  cbio_commit(handle));
        cbio_document_release(doc);
    }

    void deleteSingleDocument(const string &key) {
        libcbio_document_t doc;
        EXPECT_EQ(CBIO_SUCCESS,
                  cbio_create_empty_document(handle, &doc));
        EXPECT_EQ(CBIO_SUCCESS,
                  cbio_document_set_id(doc, key.data(), key.length(), 0));
        EXPECT_EQ(CBIO_SUCCESS,
                  cbio_document_set_deleted(doc, 1));
        EXPECT_EQ(CBIO_SUCCESS,
                  cbio_store_document(handle, doc));
        EXPECT_EQ(CBIO_SUCCESS,
                  cbio_commit(handle));
        cbio_document_release(doc);
    }

    void validateExistingDocument(const string &key, const string &value) {
        libcbio_document_t doc;
        EXPECT_EQ(CBIO_SUCCESS,
                  cbio_get_document(handle, key.data(), key.length(), &doc));
        const void *ptr;
        size_t nbytes;
        EXPECT_EQ(CBIO_SUCCESS,
                  cbio_document_get_value(doc, &ptr, &nbytes));
        EXPECT_EQ(value.length(), nbytes);
        EXPECT_EQ(0, memcmp(value.data(), ptr, nbytes));
    }

    void validateNonExistingDocument(const string &key) {
        libcbio_document_t doc;
        EXPECT_EQ(CBIO_ERROR_ENOENT,
                  cbio_get_document(handle, key.data(), key.length(), &doc));
    }

    string generateKey(int id) {
        stringstream ss;
        ss << "mykey-" << id;
        return ss.str();
    }

    libcbio_document_t generateRandomDocument(int id) {
        libcbio_document_t doc;
        EXPECT_EQ(CBIO_SUCCESS,
                  cbio_create_empty_document(handle, &doc));
        string key = generateKey(id);

        EXPECT_EQ(CBIO_SUCCESS,
                  cbio_document_set_id(doc, key.data(), key.length(), 1));

        EXPECT_EQ(CBIO_SUCCESS,
                  cbio_document_set_value(doc, blob, random() % blobsize, 0));

        return doc;
    }

    void bulkStoreDocuments(int maxdoc) {
        const unsigned int chunksize = 1000;
        libcbio_document_t *docs = new libcbio_document_t[chunksize];
        int total = 0;
        do {
            unsigned int currtx = static_cast<unsigned int>(random()) % chunksize;

            if (total + (int)currtx > maxdoc) {
                currtx = maxdoc - total;
            }

            for (int ii = 0; ii < (int)currtx; ++ii) {
                docs[ii] = generateRandomDocument(total + ii);
            }

            EXPECT_EQ(CBIO_SUCCESS, cbio_store_documents(handle, docs, currtx));
            EXPECT_EQ(CBIO_SUCCESS, cbio_commit(handle));
            total += currtx;

            for (unsigned int ii = 0; ii < currtx; ++ii) {
                cbio_document_release(docs[ii]);
            }
        } while (total < maxdoc);

        for (int ii = 0; ii < maxdoc; ++ii) {
            libcbio_document_t doc;
            string key = generateKey(ii);
            EXPECT_EQ(CBIO_SUCCESS,
                      cbio_get_document(handle, key.data(), key.length(), &doc));
            cbio_document_release(doc);
        }
    }

    void randomWriteBytes(off_t offset, int num) {
        struct stat before;
        EXPECT_NE(-1, stat(dbfile, &before));
        // Destroy the last header
        int fd;
        EXPECT_NE(-1, (fd = open(dbfile, O_RDWR)));
        EXPECT_EQ(offset, lseek(fd, offset, SEEK_SET));
        for (int ii = 0; ii < num; ++ii) {
            EXPECT_EQ(1, write(fd, "a", 1));
        }
        EXPECT_EQ(0, close(fd));

        struct stat after;
        EXPECT_NE(-1, stat(dbfile, &after));
        EXPECT_EQ(before.st_size, after.st_size);
    }

    char *blob;
    size_t blobsize;
    libcbio_t handle;
};

TEST_F(LibcbioDataAccessTest, getMiss)
{
    validateNonExistingDocument("key");
}

TEST_F(LibcbioDataAccessTest, storeSingleDocument)
{
    storeSingleDocument("key", "value");
}

TEST_F(LibcbioDataAccessTest, getHit)
{
    storeSingleDocument("key", "value");
    validateExistingDocument("key", "value");
}

TEST_F(LibcbioDataAccessTest, deleteNonExistingDocument)
{
    string key = "key";
    deleteSingleDocument(key);
    validateNonExistingDocument(key);
}

TEST_F(LibcbioDataAccessTest, deleteExistingDocument)
{
    string key = "key";
    string value = "value";
    storeSingleDocument(key, value);
    validateExistingDocument(key, value);
    deleteSingleDocument(key);
    validateNonExistingDocument(key);

    libcbio_document_t doc;
    EXPECT_EQ(CBIO_SUCCESS,
              cbio_get_document_ex(handle, key.data(), key.length(), &doc));

    int deleted;
    EXPECT_EQ(CBIO_SUCCESS,
              cbio_document_get_deleted(doc, &deleted));
    EXPECT_EQ(1, deleted);
    cbio_document_release(doc);
}

TEST_F(LibcbioDataAccessTest, testBulkStoreDocuments)
{
    bulkStoreDocuments(30000);
}

extern "C" {
    static int count_callback(libcbio_t handle,
                              libcbio_document_t doc,
                              void *ctx)
    {
        (void)handle;
        (void)doc;
        int *count = static_cast<int *>(ctx);
        (*count)++;
        return 0;
    }
}

TEST_F(LibcbioDataAccessTest, testChangesSinceDocuments)
{
    uint64_t offset = (uint64_t)cbio_get_header_position(handle);
    bulkStoreDocuments(5000);
    int total = 0;
    EXPECT_EQ(CBIO_SUCCESS,
              cbio_changes_since(handle, offset, count_callback,
                                 static_cast<void *>(&total)));
    EXPECT_EQ(5000, total);
}

TEST_F(LibcbioDataAccessTest, testGetHeaderPosition)
{
    EXPECT_EQ((off_t)0, cbio_get_header_position(handle));
    bulkStoreDocuments(10);
    EXPECT_EQ(CBIO_SUCCESS, cbio_commit(handle));

    off_t offset = cbio_get_header_position(handle);
    EXPECT_LT(0, offset);
    cbio_close_handle(handle);

    // Verify that the header position is the same after I
    // reopen the file
    EXPECT_EQ(CBIO_SUCCESS,
              cbio_open_handle(dbfile, CBIO_OPEN_RW, &handle));
    EXPECT_EQ(offset, cbio_get_header_position(handle));
    cbio_close_handle(handle);

    // Write some bytes in the header...
    randomWriteBytes(offset + 1, 10);

    EXPECT_EQ(CBIO_SUCCESS,
              cbio_open_handle(dbfile, CBIO_OPEN_RW, &handle));

    // The last header should be garbled, so we should use the one
    // before
    EXPECT_GT(offset, cbio_get_header_position(handle));

    int total = 0;
    EXPECT_EQ(CBIO_SUCCESS,
              cbio_changes_since(handle, 0, count_callback,
                                 static_cast<void *>(&total)));
    EXPECT_NE(10, total);
}

TEST_F(LibcbioDataAccessTest, testGarbledData)
{
    EXPECT_EQ((off_t)0, cbio_get_header_position(handle));
    string key = "key";
    char *data = new char[8192];
    for (int ii = 0; ii < 8192; ++ii) {
        data[ii] = 'a';
    }
    string value(data, 8192);
    storeSingleDocument(key, value);
    validateExistingDocument(key, value);
    EXPECT_EQ(CBIO_SUCCESS, cbio_commit(handle));
    off_t offset = cbio_get_header_position(handle);
    EXPECT_NE(0, offset);
    cbio_close_handle(handle);

    // try to write some rubbish inside the document..
    randomWriteBytes(offset - 4090, 2000);

    EXPECT_EQ(CBIO_SUCCESS,
              cbio_open_handle(dbfile, CBIO_OPEN_RDONLY, &handle));

    libcbio_document_t doc;
    EXPECT_EQ(CBIO_ERROR_EIO,
              cbio_get_document_ex(handle, key.data(), key.length(), &doc));

    delete []data;
}

class LibcbioLocalDocumentTest : public LibcbioDataAccessTest
{
};

TEST_F(LibcbioLocalDocumentTest, testGetNonExistingLocalDocuments)
{
    libcbio_document_t doc;
    string key("_local/foo");
    EXPECT_EQ(CBIO_ERROR_ENOENT,
              cbio_get_document_ex(handle, key.c_str(), key.length(), &doc));
}

TEST_F(LibcbioLocalDocumentTest, testStoreLocalDocuments)
{
    string key = "_local/hi-there";
    string value = "{ foo:true }";
    storeSingleDocument(key, value);
    validateExistingDocument(key, value);
}

TEST_F(LibcbioLocalDocumentTest, testDeleteLocalDocuments)
{
    string key = "_local/hi-there";
    deleteSingleDocument(key);
    libcbio_document_t doc;
    EXPECT_EQ(CBIO_ERROR_ENOENT,
              cbio_get_document_ex(handle, key.data(), key.length(), &doc));
}

TEST_F(LibcbioLocalDocumentTest, testGetDeleteLocalDocuments)
{
    string key = "_local/hi-there";
    string value = "{ foo:true }";
    storeSingleDocument(key, value);
    validateExistingDocument(key, value);
    deleteSingleDocument(key);
    validateNonExistingDocument(key);
}

TEST_F(LibcbioLocalDocumentTest, testChangesLocalDocuments)
{
    uint64_t offset = (uint64_t)cbio_get_header_position(handle);
    string key = "_local/hi-there";
    string value = "{ foo:true }";
    storeSingleDocument(key, value);
    validateExistingDocument(key, value);
    deleteSingleDocument(key);
    validateNonExistingDocument(key);
    int total = 0;
    EXPECT_EQ(CBIO_SUCCESS,
              cbio_changes_since(handle, offset, count_callback,
                                 static_cast<void *>(&total)));
    EXPECT_EQ(0, total);
    storeSingleDocument("hi", "there");

    EXPECT_EQ(CBIO_SUCCESS,
              cbio_changes_since(handle, offset, count_callback,
                                 static_cast<void *>(&total)));
    EXPECT_EQ(1, total);
}
